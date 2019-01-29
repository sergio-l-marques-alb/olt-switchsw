/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_fastPathQOSACLQOSACL.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to acl-object.xml
*
* @create  16 January 2008
*
* @author  
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_fastPathQOSACLQOSACL_obj.h"
#include <ctype.h>
#include <stdlib.h>
#include "usmdb_qos_acl_api.h"
#include "acl_exports.h"
#include "diffserv_exports.h"
#include "dot1q_exports.h"
#include "usmdb_util_api.h"
#include "usmdb_mib_vlan_api.h"
#include "usmdb_timerange_api.h"

L7_RC_t fpObjUtil_QosAclVlanCheckValid(L7_uint32 UnitIndex, L7_uint32 aclVlanIndex, L7_uint32 aclVlanDirection,
                         L7_uint32 aclVlanSequence, L7_uint32 aclVlanAclType, L7_uint32 aclVlanAclId);

L7_RC_t fpobjUtil_aclIndex_aclRuleIndex_GetNext(L7_uint32 unitNum,L7_uint32 objaclIndexValue,L7_uint32 objaclRuleIndexValue,L7_uint32 *nextObjaclIndexValue,L7_uint32 *nextObjaclRuleIndexValue);

L7_RC_t fpObjUtil_aclMacIndex_aclMacRuleIndex_GetNext(L7_uint32 unitNum,
                                                      L7_uint32 objaclMacIndexValue,
                                                      L7_uint32 objaclMacRuleIndexValue,
                                                      L7_uint32 *nextObjaclMacIndexValue,
                                                      L7_uint32 *nextObjaclMacRuleIndexValue);


L7_RC_t fpObjGet_aclIpv6Index_aclIpv6RuleIndex_GetNext(L7_uint32 unitNum,L7_uint32 objaclIpv6IndexValue,
                                                       L7_uint32 objaclIpv6RuleIndexValue,
                                                       L7_uint32 *nextObjaclIpv6IndexValue,
                                                       L7_uint32 *nextObjaclIpv6RuleIndexValue);

L7_RC_t fpobj_AclVlanIDGetNewVlanId(L7_uint32 vlanId, L7_uint32 *aclVlanIndex);

L7_RC_t fpobj_AclVlanIDCheck(L7_uint32 aclVlanIndex);

L7_RC_t fpobj_AclVlanGetNext(L7_uint32 aclVlanIndex, L7_uint32 *aclVlanIndexNext);

L7_RC_t fpobjUtil_QosAclIntfNext(L7_uint32 UnitIndex, L7_uint32 *aclIfIndex, L7_uint32 *aclIfDirection,
                   L7_uint32 *aclIfSequence, L7_uint32 *aclIfAclType, L7_uint32 *aclIfAclId);

L7_RC_t fpobj_QosAclIntfRemove(L7_uint32 UnitIndex, L7_uint32 aclIfIndex, L7_uint32 aclIfDirection,
                        L7_uint32 aclIfSequence, L7_uint32 aclIfAclType, L7_uint32 aclIfAclId);

L7_RC_t fpobj_QosAclIntfAdd(L7_uint32 UnitIndex, L7_uint32 aclIfIndex, L7_uint32 aclIfDirection,
                  L7_uint32 aclIfSequence, L7_uint32 aclIfAclType, L7_uint32 aclIfAclId);

L7_BOOL fpObj_isNameACL(L7_char8 *aclIDOrName);
L7_BOOL fpObj_isNameACL(L7_char8 *aclIDOrName)
{
   if(atoi(aclIDOrName))
   {
       return L7_FALSE;
   }
   else
   {
      return L7_TRUE;
   }
}
/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclIfIndex
*
* @purpose Get 'aclIfIndex'
*
* @description [aclIfIndex] The interface to which this ACL instance applies.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclIfIndex (void *wap, void *bufp)
{

  xLibU32_t objaclIfIndexValue;
  xLibU32_t nextObjaclIfIndexValue;
  xLibU32_t objaclIfDirectionValue;
  xLibU32_t nextObjaclIfDirectionValue;
  xLibU32_t objaclIfSequenceValue;
  xLibU32_t nextObjaclIfSequenceValue;
  xLibU32_t objaclIfAclTypeValue; xLibU32_t nextObjaclIfAclTypeValue;
  xLibU32_t objaclIfAclIdValue;
  xLibU32_t nextObjaclIfAclIdValue;
/*  L7_uint32 intIfNum=0      */ 
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIfIndex,
                          (xLibU8_t *) & objaclIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objaclIfIndexValue = objaclIfDirectionValue = objaclIfSequenceValue = objaclIfAclTypeValue =
      objaclIfAclIdValue = 0;
  }
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclIfIndexValue, owa.len);
    objaclIfDirectionValue = objaclIfSequenceValue = objaclIfAclTypeValue = objaclIfAclIdValue = 0;
    /* store the value of ifindex
    intIfNum = objaclIfIndexValue;*/
    do
    {
      if(((fpobjUtil_QosAclIntfNext(L7_UNIT_CURRENT,&objaclIfIndexValue,&objaclIfDirectionValue,
                  &objaclIfSequenceValue,&objaclIfAclTypeValue,
                  &objaclIfAclIdValue) == L7_SUCCESS) ) &&
          (usmDbGetNextPhysicalIntIfNumber(objaclIfIndexValue,&nextObjaclIfIndexValue) == L7_SUCCESS))
      {
        nextObjaclIfIndexValue = objaclIfIndexValue ;
        nextObjaclIfDirectionValue=objaclIfDirectionValue;
        nextObjaclIfSequenceValue=objaclIfSequenceValue;
        nextObjaclIfAclTypeValue=objaclIfAclTypeValue;
        nextObjaclIfAclIdValue=objaclIfAclIdValue;

        owa.l7rc = L7_SUCCESS;        
      }
      else
      {
        owa.l7rc = L7_FAILURE;        
      }
/*      objaclIfIndexValue = intIfNum;  */
    }
    while ((owa.l7rc == L7_SUCCESS) && (objaclIfIndexValue == nextObjaclIfIndexValue));

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  nextObjaclIfIndexValue = objaclIfIndexValue;
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjaclIfIndexValue, owa.len);

  /* return the object value: aclIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjaclIfIndexValue,
                           sizeof (objaclIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjList_fastPathQOSACLQOSACL_aclIfIndex
*
* @purpose List 'aclIfIndex'
 *@description  [aclIfIndex] The interface to which this ACL instance applies.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_fastPathQOSACLQOSACL_aclIfIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objaclIfIndexValue;
  xLibU32_t nextObjaclIfIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof (objaclIfIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIfIndex,
                          (xLibU8_t *) & objaclIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbIntIfNumTypeFirstGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF,
                                 0, &nextObjaclIfIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclIfIndexValue, owa.len);
    owa.l7rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT,  USM_PHYSICAL_INTF | USM_LAG_INTF,
                                  0,objaclIfIndexValue, &nextObjaclIfIndexValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjaclIfIndexValue, owa.len);

  /* return the object value: aclIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjaclIfIndexValue,
                           sizeof (nextObjaclIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclIfAclIdOrName
*
* @purpose Get 'aclIfAclIdOrName'
 *@description  [aclIfAclIdOrName] (Pseudo-Key) for aclIfAclId, The ACL identifier value, which can also be
* name for MAC and IP Named ACL's  is interpreted based on the
* aclIfType object. For the IP ACLs, the actual ACL number is its
* identifier as follows: IP standard ranges from 1-99, while IP extended
* ranges from 100-199. Here, aclIfAclId represents aclIndex.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclIfAclIdOrName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objaclIfAclIdOrNameValue;
  xLibU32_t aclType;
  xLibU16_t aclTypeSize = sizeof(aclType);
  xLibU32_t keyaclIfAclIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIfAclId */
  owa.len = sizeof (keyaclIfAclIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIfAclId,
                          (xLibU8_t *) & keyaclIfAclIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIfAclIdValue, owa.len);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIfAclType,(xLibU8_t *) & aclType,&aclTypeSize);
  /* get the value from application */
  if(aclType == L7_ACL_TYPE_IP)
  {
      if (usmDbQosAclNamedIndexCheckValid(L7_UNIT_CURRENT, L7_ACL_TYPE_IP, keyaclIfAclIdValue) == L7_SUCCESS)
     {
         owa.l7rc= usmDbQosAclNameGet(L7_UNIT_CURRENT,keyaclIfAclIdValue,objaclIfAclIdOrNameValue);
     }
     /* acl is not Named ACL hence convert the acl Number from integer  and assign it to */
     else
     {
         memset(objaclIfAclIdOrNameValue,0,sizeof(objaclIfAclIdOrNameValue));
         osapiSnprintf(objaclIfAclIdOrNameValue,sizeof(objaclIfAclIdOrNameValue),"%u",keyaclIfAclIdValue);
         owa.l7rc = L7_SUCCESS;
     }

  }
  else if(aclType == L7_ACL_TYPE_MAC)
  {
     owa.l7rc = usmDbQosAclMacNameGet(L7_UNIT_CURRENT, keyaclIfAclIdValue,
                              (L7_uchar8 *)objaclIfAclIdOrNameValue); 
  }
  else if(aclType == L7_ACL_TYPE_IPV6)
  {
      memset(objaclIfAclIdOrNameValue,0,sizeof(objaclIfAclIdOrNameValue));
      osapiSnprintf(objaclIfAclIdOrNameValue,sizeof(objaclIfAclIdOrNameValue),"%u",keyaclIfAclIdValue);
      owa.l7rc = L7_SUCCESS;
  }
  else
  {
      memset(objaclIfAclIdOrNameValue,0,sizeof(objaclIfAclIdOrNameValue));
      owa.l7rc = L7_SUCCESS;
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objaclIfAclIdOrNameValue, strlen (objaclIfAclIdOrNameValue));

  /* return the object value: aclIfAclIdOrName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objaclIfAclIdOrNameValue,
                           strlen (objaclIfAclIdOrNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclIfAclIdOrName
*
* @purpose Set 'aclIfAclIdOrName'
 *@description  [aclIfAclIdOrName] (Pseudo-Key) for aclIfAclId, The ACL identifier value, which can also be
* name for MAC and IP Named ACL's  is interpreted based on the
* aclIfType object. For the IP ACLs, the actual ACL number is its
* identifier as follows: IP standard ranges from 1-99, while IP extended
* ranges from 100-199. Here, aclIfAclId represents aclIndex.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclIfAclIdOrName (void *wap, void *bufp)
{
  
  return XLIBRC_SUCCESS;
}


/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclIfDirection
*
* @purpose Get 'aclIfDirection'
*
* @description [aclIfDirection] The interface direction to which this ACL instance applies.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclIfDirection (void *wap, void *bufp)
{

  xLibU32_t objaclIfIndexValue;
  xLibU32_t nextObjaclIfIndexValue = 0;
  xLibU32_t objaclIfDirectionValue;
  xLibU32_t nextObjaclIfDirectionValue;
  xLibU32_t objaclIfSequenceValue;
  xLibU32_t nextObjaclIfSequenceValue;
  xLibU32_t objaclIfAclTypeValue;
  xLibU32_t nextObjaclIfAclTypeValue;
  xLibU32_t objaclIfAclIdValue;
  xLibU32_t nextObjaclIfAclIdValue;
  L7_uint32 intIfNum=0;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIfIndex,
                          (xLibU8_t *) & objaclIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclIfIndexValue, owa.len);

  /* retrieve key: aclIfDirection */
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIfDirection,
                          (xLibU8_t *) & objaclIfDirectionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objaclIfDirectionValue = objaclIfSequenceValue = objaclIfAclTypeValue = objaclIfAclIdValue = 0;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclIfDirectionValue, owa.len);
  objaclIfSequenceValue = objaclIfAclTypeValue = objaclIfAclIdValue = 0;
  do
  {
    if((fpobjUtil_QosAclIntfNext(L7_UNIT_CURRENT,&intIfNum,&objaclIfDirectionValue,
                                 &objaclIfSequenceValue,&objaclIfAclTypeValue,
                                 &objaclIfAclIdValue) == L7_SUCCESS) &&
          (usmDbGetNextPhysicalIntIfNumber(intIfNum,&nextObjaclIfIndexValue) == L7_SUCCESS))
    {
      nextObjaclIfDirectionValue=objaclIfDirectionValue;
      nextObjaclIfSequenceValue=objaclIfSequenceValue;
      nextObjaclIfAclTypeValue=objaclIfAclTypeValue;
      nextObjaclIfAclIdValue=objaclIfAclIdValue;
      owa.l7rc = L7_SUCCESS;
    }
    else
    {
      owa.l7rc = L7_FAILURE;
    }
  }
  while ((owa.l7rc == L7_SUCCESS) && (objaclIfIndexValue == nextObjaclIfIndexValue)
          && (objaclIfDirectionValue == nextObjaclIfDirectionValue));

  if ((owa.l7rc != L7_SUCCESS) || (objaclIfIndexValue != nextObjaclIfIndexValue))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjaclIfDirectionValue, owa.len);

  /* return the object value: aclIfDirection */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjaclIfDirectionValue,
                           sizeof (objaclIfDirectionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjList_fastPathQOSACLQOSACL_aclIfDirection
*
* @purpose List 'aclIfDirection'
 *@description  [aclIfDirection] The interface direction to which this ACL
* instance applies.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_fastPathQOSACLQOSACL_aclIfDirection (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objaclIfDirectionValue;
  xLibU32_t nextObjaclIfDirectionValue;

  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof (objaclIfDirectionValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIfDirection,
                          (xLibU8_t *) & objaclIfDirectionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbGetFirstUnknown (L7_UNIT_CURRENT, &nextObjaclIfDirectionValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclIfDirectionValue, owa.len);
    owa.l7rc =
      usmDbGetNextUnknown (L7_UNIT_CURRENT, objaclIfDirectionValue, &nextObjaclIfDirectionValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjaclIfDirectionValue, owa.len);

  /* return the object value: aclIfDirection */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjaclIfDirectionValue,
                           sizeof (nextObjaclIfDirectionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclIfSequence
*
* @purpose Get 'aclIfSequence'
*
* @description [aclIfSequence] The relative evaluation sequence of this ACL for this interface and direction. When multiple ACLs are allowed for a given interface and direction, the sequence number determines the order in which the list of ACLs are evaluated,with lower sequence number
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclIfSequence (void *wap, void *bufp)
{

  xLibU32_t objaclIfIndexValue;
  xLibU32_t nextObjaclIfIndexValue;
  xLibU32_t objaclIfDirectionValue;
  xLibU32_t nextObjaclIfDirectionValue=0;
  xLibU32_t objaclIfSequenceValue;
  xLibU32_t nextObjaclIfSequenceValue;
  xLibU32_t objaclIfAclTypeValue;
  xLibU32_t nextObjaclIfAclTypeValue;
  xLibU32_t objaclIfAclIdValue;
  xLibU32_t nextObjaclIfAclIdValue;
  L7_uint32 intIfNum=0;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIfIndex,
                          (xLibU8_t *) & objaclIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclIfIndexValue, owa.len);

  /* retrieve key: aclIfDirection */
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIfDirection,
                          (xLibU8_t *) & objaclIfDirectionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclIfDirectionValue, owa.len);

  /* retrieve key: aclIfSequence */
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIfSequence,
                          (xLibU8_t *) & objaclIfSequenceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objaclIfSequenceValue = objaclIfAclTypeValue = objaclIfAclIdValue = 0;
  }  
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclIfSequenceValue, owa.len);
  objaclIfAclTypeValue = objaclIfAclIdValue = 0;
  do
  {
    if((fpobjUtil_QosAclIntfNext(L7_UNIT_CURRENT,&intIfNum,&objaclIfDirectionValue,
                                &objaclIfSequenceValue,&objaclIfAclTypeValue,
                                &objaclIfAclIdValue) == L7_SUCCESS) &&
          (usmDbGetNextPhysicalIntIfNumber(intIfNum,&nextObjaclIfIndexValue) == L7_SUCCESS))
    { 
      nextObjaclIfDirectionValue=objaclIfDirectionValue;
      nextObjaclIfSequenceValue=objaclIfSequenceValue;
      nextObjaclIfAclTypeValue=objaclIfAclTypeValue;
      nextObjaclIfAclIdValue=objaclIfAclIdValue;
      owa.l7rc = L7_SUCCESS;
    }
    else
    {
      owa.l7rc = L7_FAILURE;
    }
  }
  while ((owa.l7rc == L7_SUCCESS) && (objaclIfIndexValue == nextObjaclIfIndexValue)
          && (objaclIfDirectionValue == nextObjaclIfDirectionValue)
          && (objaclIfSequenceValue == nextObjaclIfSequenceValue));
  
  if ((owa.l7rc != L7_SUCCESS) || (objaclIfIndexValue != nextObjaclIfIndexValue)
      || (objaclIfDirectionValue != nextObjaclIfDirectionValue))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjaclIfSequenceValue, owa.len);

  /* return the object value: aclIfSequence */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjaclIfSequenceValue,
                           sizeof (objaclIfSequenceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclIfAclType
*
* @purpose Get 'aclIfAclType'
*
* @description [aclIfAclType] The type of this ACL, which is used to interpret the aclIfId object value. Each type of ACL uses its own numbering scheme for identification (see aclIfAclId object for details). The aclIfAclId object must be specified along with this object.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclIfAclType (void *wap, void *bufp)
{

  xLibU32_t objaclIfIndexValue;
  xLibU32_t nextObjaclIfIndexValue;
  xLibU32_t objaclIfDirectionValue;
  xLibU32_t nextObjaclIfDirectionValue=0;
  xLibU32_t objaclIfSequenceValue;
  xLibU32_t nextObjaclIfSequenceValue=0;
  xLibU32_t objaclIfAclTypeValue;
  xLibU32_t nextObjaclIfAclTypeValue;
  xLibU32_t objaclIfAclIdValue;
  xLibU32_t nextObjaclIfAclIdValue;
  L7_uint32 intIfNum=0;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIfIndex,
                          (xLibU8_t *) & objaclIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclIfIndexValue, owa.len);

  /* retrieve key: aclIfDirection */
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIfDirection,
                          (xLibU8_t *) & objaclIfDirectionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclIfDirectionValue, owa.len);

  /* retrieve key: aclIfSequence */
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIfSequence,
                          (xLibU8_t *) & objaclIfSequenceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclIfSequenceValue, owa.len);

  /* retrieve key: aclIfAclType */
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIfAclType,
                          (xLibU8_t *) & objaclIfAclTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objaclIfAclTypeValue = objaclIfAclIdValue = 0;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclIfAclTypeValue, owa.len);
  objaclIfAclIdValue = 0;
  do
  {
    if((fpobjUtil_QosAclIntfNext(L7_UNIT_CURRENT,&intIfNum,&objaclIfDirectionValue,
                                &objaclIfSequenceValue,&objaclIfAclTypeValue,
                                &objaclIfAclIdValue) == L7_SUCCESS) &&
          (usmDbGetNextPhysicalIntIfNumber(intIfNum,&nextObjaclIfIndexValue) == L7_SUCCESS))
    {
      nextObjaclIfDirectionValue=objaclIfDirectionValue;
      nextObjaclIfSequenceValue=objaclIfSequenceValue;
      nextObjaclIfAclTypeValue=objaclIfAclTypeValue;
      nextObjaclIfAclIdValue=objaclIfAclIdValue;
      owa.l7rc = L7_SUCCESS;
    }
    else
    {
      owa.l7rc = L7_FAILURE;
    }
  }
  while ((owa.l7rc == L7_SUCCESS) && (objaclIfIndexValue == nextObjaclIfIndexValue)
          && (objaclIfDirectionValue == nextObjaclIfDirectionValue)
          && (objaclIfSequenceValue == nextObjaclIfSequenceValue)
          && (objaclIfAclTypeValue == nextObjaclIfAclTypeValue));

  if ((owa.l7rc != L7_SUCCESS) || (objaclIfIndexValue != nextObjaclIfIndexValue)
      || (objaclIfDirectionValue != nextObjaclIfDirectionValue)
      || (objaclIfSequenceValue != nextObjaclIfSequenceValue))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjaclIfAclTypeValue, owa.len);

  /* return the object value: aclIfAclType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjaclIfAclTypeValue,
                           sizeof (objaclIfAclTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjList_fastPathQOSACLQOSACL_aclIfAclType
*
* @purpose List 'aclIfAclType'
 *@description  [aclIfAclType] The type of this ACL, which is used to interpret
* the aclIfId object value. Each type of ACL uses its own numbering
* scheme for identification (see aclIfAclId object for details).
* The aclIfAclId object must be specified along with this object.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_fastPathQOSACLQOSACL_aclIfAclType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objaclIfAclTypeValue;
  xLibU32_t nextObjaclIfAclTypeValue;

  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof (objaclIfAclTypeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIfAclType,
                          (xLibU8_t *) & objaclIfAclTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbGetFirstUnknown (L7_UNIT_CURRENT, &nextObjaclIfAclTypeValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclIfAclTypeValue, owa.len);
    owa.l7rc =
      usmDbGetNextUnknown (L7_UNIT_CURRENT, objaclIfAclTypeValue, &nextObjaclIfAclTypeValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjaclIfAclTypeValue, owa.len);

  /* return the object value: aclIfAclType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjaclIfAclTypeValue,
                           sizeof (nextObjaclIfAclTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclIfAclId
*
* @purpose Get 'aclIfAclId'
*
* @description [aclIfAclId] The ACL identifier value, which is interpreted based on the aclIfType object. For the IP ACLs, the actual ACL number is its identifier as follows: IP standard ranges from 1-99, while IP extended ranges from 100-199. Here, aclIfAclId represents aclIndex.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclIfAclId (void *wap, void *bufp)
{

  xLibU32_t objaclIfIndexValue;
  xLibU32_t nextObjaclIfIndexValue;
  xLibU32_t objaclIfDirectionValue;
  xLibU32_t nextObjaclIfDirectionValue=0;
  xLibU32_t objaclIfSequenceValue;
  xLibU32_t nextObjaclIfSequenceValue=0;
  xLibU32_t objaclIfAclTypeValue;
  xLibU32_t nextObjaclIfAclTypeValue=0;
  xLibU32_t objaclIfAclIdValue;
  xLibU32_t nextObjaclIfAclIdValue;
  L7_uint32 intIfNum=0; 
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIfIndex,
                          (xLibU8_t *) & objaclIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclIfIndexValue, owa.len);

  /* retrieve key: aclIfDirection */
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIfDirection,
                          (xLibU8_t *) & objaclIfDirectionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclIfDirectionValue, owa.len);

  /* retrieve key: aclIfSequence */
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIfSequence,
                          (xLibU8_t *) & objaclIfSequenceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclIfSequenceValue, owa.len);

  /* retrieve key: aclIfAclType */
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIfAclType,
                          (xLibU8_t *) & objaclIfAclTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclIfAclTypeValue, owa.len);

  /* retrieve key: aclIfAclId */
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIfAclId,
                          (xLibU8_t *) & objaclIfAclIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objaclIfAclIdValue = 0;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclIfAclIdValue, owa.len);

  if((fpobjUtil_QosAclIntfNext(L7_UNIT_CURRENT,&intIfNum,&objaclIfDirectionValue,
                                &objaclIfSequenceValue,&objaclIfAclTypeValue,
                                &objaclIfAclIdValue) == L7_SUCCESS) &&
          (usmDbGetNextPhysicalIntIfNumber(intIfNum,&nextObjaclIfIndexValue) == L7_SUCCESS))
  {
    nextObjaclIfDirectionValue=objaclIfDirectionValue;
    nextObjaclIfSequenceValue=objaclIfSequenceValue;
    nextObjaclIfAclTypeValue=objaclIfAclTypeValue;
    nextObjaclIfAclIdValue=objaclIfAclIdValue;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    owa.l7rc = L7_FAILURE;
  }

  if ((owa.l7rc != L7_SUCCESS) || (objaclIfIndexValue != nextObjaclIfIndexValue)
      || (objaclIfDirectionValue != nextObjaclIfDirectionValue)
      || (objaclIfSequenceValue != nextObjaclIfSequenceValue)
      || (objaclIfAclTypeValue != nextObjaclIfAclTypeValue))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjaclIfAclIdValue, owa.len);

  /* return the object value: aclIfAclId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjaclIfAclIdValue,
                           sizeof (objaclIfAclIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclIndex
*
* @purpose Get 'aclIndex'
*
* @description [aclIndex] The IP ACL table index this instance is associated with.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclIndex (void *wap, void *bufp)
{

  xLibU32_t objaclIndexValue;
  xLibU32_t nextObjaclIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                          (xLibU8_t *) & objaclIndexValue, &owa.len);
  /*include implementation of pseudo keys too */
  if (owa.rc != XLIBRC_SUCCESS)
  {
    objaclIndexValue =0;
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbQosAclNumGetFirst(L7_UNIT_CURRENT, &objaclIndexValue); 
    if(owa.l7rc !=  L7_SUCCESS)
    {
        owa.l7rc =usmDbQosAclNamedIndexGetFirst(L7_UNIT_CURRENT, L7_ACL_TYPE_IP, &objaclIndexValue);        
    }
     nextObjaclIndexValue = objaclIndexValue;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclIndexValue, owa.len);
    owa.l7rc = usmDbQosAclNumGetNext(L7_UNIT_CURRENT,objaclIndexValue,&nextObjaclIndexValue);
    if(owa.l7rc != L7_SUCCESS)
    {
        owa.l7rc = usmDbQosAclNamedIndexGetNext(L7_UNIT_CURRENT, L7_ACL_TYPE_IP, objaclIndexValue, &nextObjaclIndexValue);
    }
    
  }

  /* return End of the Table */
  if(owa.l7rc !=  L7_SUCCESS)
  {
     owa.rc = XLIBRC_ENDOF_TABLE;
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjaclIndexValue, owa.len);

  /* return the object value: aclIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjaclIndexValue, sizeof (objaclIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;     
    

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclRuleIndex
*
* @purpose Get 'aclRuleIndex'
*
* @description [aclRuleIndex] The index of this rule instance within an IP ACL.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclRuleIndex (void *wap, void *bufp)
{

  xLibU32_t objaclIndexValue;
  xLibU32_t nextObjaclIndexValue;
  xLibU32_t objaclRuleIndexValue;
  xLibU32_t nextObjaclRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                          (xLibU8_t *) & objaclIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclIndexValue, owa.len);

  /* retrieve key: aclRuleIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                          (xLibU8_t *) & objaclRuleIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objaclRuleIndexValue = 0;
    if((owa.l7rc = usmDbQosAclRuleGetFirst(L7_UNIT_CURRENT,objaclIndexValue,&objaclRuleIndexValue)) == L7_SUCCESS)
    {
      owa.l7rc = L7_SUCCESS;
      nextObjaclIndexValue = objaclIndexValue;
      nextObjaclRuleIndexValue = objaclRuleIndexValue;
    }           
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclRuleIndexValue, owa.len);
   owa.l7rc = usmDbQosAclRuleGetNext(L7_UNIT_CURRENT,objaclIndexValue,objaclRuleIndexValue,&nextObjaclRuleIndexValue);            
  }   

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjaclRuleIndexValue, owa.len);

  /* return the object value: aclRuleIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjaclRuleIndexValue,
                           sizeof (objaclRuleIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclMacIndex
*
* @purpose Get 'aclMacIndex'
*
* @description [aclMacIndex] The MAC ACL table index this instance is associated with When creating a new MAC ACL, refer to the aclMacIndexNextFree object to determine the next available aclMacIndex to use.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclMacIndex (void *wap, void *bufp)
{

  xLibU32_t objaclMacIndexValue;
  xLibU32_t nextObjaclMacIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclMacIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                          (xLibU8_t *) & objaclMacIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objaclMacIndexValue =  0;
	 nextObjaclMacIndexValue = 0;
    owa.l7rc = usmDbQosAclMacIndexGetFirst(L7_UNIT_CURRENT,&nextObjaclMacIndexValue) ;
	 
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclMacIndexValue, owa.len);
    owa.l7rc =  usmDbQosAclMacIndexGetNext(L7_UNIT_CURRENT, 
			                                                                objaclMacIndexValue,
			                                                                &nextObjaclMacIndexValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjaclMacIndexValue, owa.len);

  /* return the object value: aclMacIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjaclMacIndexValue,
                           sizeof (objaclMacIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclMacRuleIndex
*
* @purpose Get 'aclMacRuleIndex'
*
* @description [aclMacRuleIndex] The index of this rule instance within an MAC ACL.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclMacRuleIndex (void *wap, void *bufp)
{

  xLibU32_t objaclMacIndexValue;
  xLibU32_t objaclMacRuleIndexValue;
  xLibU32_t nextObjaclMacRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclMacIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                          (xLibU8_t *) & objaclMacIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclMacIndexValue, kwa.len);

  /* retrieve key: aclMacRuleIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleIndex,
                          (xLibU8_t *) & objaclMacRuleIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objaclMacRuleIndexValue = 0;
  	nextObjaclMacRuleIndexValue = 0;    
	owa.l7rc = usmDbQosAclMacRuleGetFirst(L7_UNIT_CURRENT,
		                                  objaclMacIndexValue,&objaclMacRuleIndexValue);
   nextObjaclMacRuleIndexValue = objaclMacRuleIndexValue;

  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclMacRuleIndexValue, owa.len);

    owa.l7rc = usmDbQosAclMacRuleGetNext(L7_UNIT_CURRENT, objaclMacIndexValue, 
                                  objaclMacRuleIndexValue, &nextObjaclMacRuleIndexValue);
  }

  if (owa.l7rc!= L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjaclMacRuleIndexValue, owa.len);

  /* return the object value: aclMacRuleIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjaclMacRuleIndexValue,
                           sizeof (objaclMacRuleIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclIpv6Index
*
* @purpose Get 'aclIpv6Index'
*
* @description [aclIpv6Index] The IPv6 ACL table index this instance is associated with When creating a new IPv6 ACL, refer to the aclIPv6IndexNextFree object to determine the next available aclIpv6Index to use
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclIpv6Index (void *wap, void *bufp)
{

  xLibU32_t objaclIpv6IndexValue;
  xLibU32_t nextObjaclIpv6IndexValue;
  xLibU32_t objaclIpv6RuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIpv6Index */
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                          (xLibU8_t *) & objaclIpv6IndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objaclIpv6IndexValue = objaclIpv6RuleIndexValue = 0;
    owa.l7rc = usmDbQosAclNamedIndexGetFirst(L7_UNIT_CURRENT,L7_ACL_TYPE_IPV6,&nextObjaclIpv6IndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclIpv6IndexValue, owa.len);
    owa.l7rc = usmDbQosAclNamedIndexGetNext(L7_UNIT_CURRENT,L7_ACL_TYPE_IPV6,objaclIpv6IndexValue, &nextObjaclIpv6IndexValue);
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjaclIpv6IndexValue, owa.len);

  /* return the object value: aclIpv6Index */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjaclIpv6IndexValue, sizeof (nextObjaclIpv6IndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclIpv6RuleIndex
*
* @purpose Get 'aclIpv6RuleIndex'
*
* @description [aclIpv6RuleIndex] The index of this rule instance within an IPv6 ACL.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclIpv6RuleIndex (void *wap, void *bufp)
{

  xLibU32_t objaclIpv6IndexValue;
  xLibU32_t objaclIpv6RuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIpv6Index */
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                          (xLibU8_t *) & objaclIpv6IndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclIpv6IndexValue, owa.len);

  /* retrieve key: aclIpv6RuleIndex */
  owa.len=sizeof(objaclIpv6RuleIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleIndex,
                          (xLibU8_t *) & objaclIpv6RuleIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objaclIpv6RuleIndexValue = 0;
    owa.l7rc = usmDbQosAclRuleGetFirst(L7_UNIT_CURRENT,objaclIpv6IndexValue,&objaclIpv6RuleIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclIpv6RuleIndexValue, owa.len);
    owa.l7rc = usmDbQosAclRuleGetNext(L7_UNIT_CURRENT,objaclIpv6IndexValue,objaclIpv6RuleIndexValue,&objaclIpv6RuleIndexValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &objaclIpv6RuleIndexValue, owa.len);

  /* return the object value: aclIpv6RuleIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclIpv6RuleIndexValue,
                           sizeof (objaclIpv6RuleIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclIpv6RuleAction
*
* @purpose Get 'aclIpv6RuleAction'
*
* @description [aclIpv6RuleAction]: The type of action this IPv6 ACL rule
*              should perform. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclIpv6RuleAction (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6IndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6RuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclIpv6RuleActionValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIpv6Index */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                           (xLibU8_t *) & keyaclIpv6IndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, kwa1.len);

  /* retrieve key: aclIpv6RuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleIndex,
                           (xLibU8_t *) & keyaclIpv6RuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6RuleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbQosAclRuleActionGet (L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                       keyaclIpv6RuleIndexValue,
                                       &objaclIpv6RuleActionValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: aclIpv6RuleAction */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclIpv6RuleActionValue,
                           sizeof (objaclIpv6RuleActionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclIpv6RuleAction
*
* @purpose Set 'aclIpv6RuleAction'
*
* @description [aclIpv6RuleAction]: The type of action this IPv6 ACL rule
*              should perform. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclIpv6RuleAction (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclIpv6RuleActionValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6IndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6RuleIndexValue;
  FPOBJ_TRACE_ENTER (bufp);
  xLibU32_t objaclIpv6RuleStatusValue;
  /* retrieve row status object value */
  owa.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleStatus,
                   (xLibU8_t *) & objaclIpv6RuleStatusValue, &owa.len);
  if (objaclIpv6RuleStatusValue == L7_ROW_STATUS_DESTROY)
  {
    return XLIBRC_SUCCESS;
  }

  /* retrieve object: aclIpv6RuleAction */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclIpv6RuleActionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclIpv6RuleActionValue, owa.len);

  /* retrieve key: aclIpv6Index */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                           (xLibU8_t *) & keyaclIpv6IndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, kwa1.len);

  /* retrieve key: aclIpv6RuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleIndex,
                           (xLibU8_t *) & keyaclIpv6RuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6RuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbQosAclRuleActionAdd (L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                       keyaclIpv6RuleIndexValue,
                                       objaclIpv6RuleActionValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    if (owa.l7rc == L7_REQUEST_DENIED)
    {
      owa.rc = XLIBRC_ACL_CHANGE_REQ_DENY_APP_RULE_LIMIT;
    }
    else
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclIpv6RuleLogging
*
* @purpose Get 'aclIpv6RuleLogging'
*
* @description [aclIpv6RuleLogging]: Flag to indicate that the ACL rule is
*              being logged. A hardware count of the number of times this
*              rule is hit is reported via the aclTrapRuleLogEvent notification.
*              This object may be supported for an aclIPv6RuleAction
*              setting of permit(1) and/or deny(2), 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclIpv6RuleLogging (void *wap,
                                                           void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6IndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6RuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclIpv6RuleLoggingValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIpv6Index */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                           (xLibU8_t *) & keyaclIpv6IndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, kwa1.len);

  /* retrieve key: aclIpv6RuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleIndex,
                           (xLibU8_t *) & keyaclIpv6RuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6RuleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbQosAclRuleLoggingGet (L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                        keyaclIpv6RuleIndexValue,
                                        &objaclIpv6RuleLoggingValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: aclIpv6RuleLogging */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclIpv6RuleLoggingValue,
                           sizeof (objaclIpv6RuleLoggingValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclIpv6RuleLogging
*
* @purpose Set 'aclIpv6RuleLogging'
*
* @description [aclIpv6RuleLogging]: Flag to indicate that the ACL rule is
*              being logged. A hardware count of the number of times this
*              rule is hit is reported via the aclTrapRuleLogEvent notification.
*              This object may be supported for an aclIPv6RuleAction
*              setting of permit(1) and/or deny(2), 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclIpv6RuleLogging (void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclIpv6RuleLoggingValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6IndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6RuleIndexValue;
  FPOBJ_TRACE_ENTER (bufp);
  
   xLibU32_t objaclIpv6RuleStatusValue;
  /* retrieve row status object value */
  owa.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleStatus,
                   (xLibU8_t *) & objaclIpv6RuleStatusValue, &owa.len);
  if (objaclIpv6RuleStatusValue == L7_ROW_STATUS_DESTROY)
  {
    return XLIBRC_SUCCESS;
  }

  /* retrieve object: aclIpv6RuleLogging */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclIpv6RuleLoggingValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclIpv6RuleLoggingValue, owa.len);

  /* retrieve key: aclIpv6Index */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                           (xLibU8_t *) & keyaclIpv6IndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, kwa1.len);

  /* retrieve key: aclIpv6RuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleIndex,
                           (xLibU8_t *) & keyaclIpv6RuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6RuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbQosAclRuleLoggingAdd(L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                              keyaclIpv6RuleIndexValue,
                              objaclIpv6RuleLoggingValue);
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
* @function fpObjGet_fastPathQOSACLQOSACL_aclIpv6RuleAssignQueueId
*
* @purpose Get 'aclIpv6RuleAssignQueueId'
*
* @description [aclIpv6RuleAssignQueueId]: Queue identifier to which all inbound
*              packets matching this ACL rule are directed. This object
*              defaults to the standardqueue assignment for user priority
*              0 traffic per the IEEE 802.1D specification based on the
*              number of assignable queues in the system: 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclIpv6RuleAssignQueueId (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6IndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6RuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclIpv6RuleAssignQueueIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIpv6Index */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                           (xLibU8_t *) & keyaclIpv6IndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, kwa1.len);

  /* retrieve key: aclIpv6RuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleIndex,
                           (xLibU8_t *) & keyaclIpv6RuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6RuleIndexValue, kwa2.len);

  /* get the value from application */
  if(usmDbQosAclIsFieldConfigured(L7_UNIT_CURRENT, keyaclIpv6IndexValue, keyaclIpv6RuleIndexValue, ACL_ASSIGN_QUEUEID) == L7_TRUE)
  {
    owa.l7rc =
      usmDbQosAclRuleAssignQueueIdGet (L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                     keyaclIpv6RuleIndexValue,
                                     &objaclIpv6RuleAssignQueueIdValue);
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

  /* return the object value: aclIpv6RuleAssignQueueId */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objaclIpv6RuleAssignQueueIdValue,
                    sizeof (objaclIpv6RuleAssignQueueIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclIpv6RuleAssignQueueId
*
* @purpose Set 'aclIpv6RuleAssignQueueId'
*
* @description [aclIpv6RuleAssignQueueId]: Queue identifier to which all inbound
*              packets matching this ACL rule are directed. This object
*              defaults to the standardqueue assignment for user priority
*              0 traffic per the IEEE 802.1D specification based on the
*              number of assignable queues in the system: 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclIpv6RuleAssignQueueId (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclIpv6RuleAssignQueueIdValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6IndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6RuleIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

    xLibU32_t objaclIpv6RuleStatusValue;
  /* retrieve row status object value */
  owa.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleStatus,
                   (xLibU8_t *) & objaclIpv6RuleStatusValue, &owa.len);
  if (objaclIpv6RuleStatusValue == L7_ROW_STATUS_DESTROY)
  {
    return XLIBRC_SUCCESS;
  }

  /* retrieve object: aclIpv6RuleAssignQueueId */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclIpv6RuleAssignQueueIdValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclIpv6RuleAssignQueueIdValue, owa.len);

  /* retrieve key: aclIpv6Index */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                           (xLibU8_t *) & keyaclIpv6IndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, kwa1.len);

  /* retrieve key: aclIpv6RuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleIndex,
                           (xLibU8_t *) & keyaclIpv6RuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6RuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbQosAclRuleAssignQueueIdAdd(L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                              keyaclIpv6RuleIndexValue,
                              objaclIpv6RuleAssignQueueIdValue);
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
* @function fpObjGet_fastPathQOSACLQOSACL_aclIpv6RuleTimeRangeName
*
* @purpose Get 'aclIpv6RuleTimeRangeName'
*
* @description [aclIpv6RuleTimeRangeName]:Name of time range associated with the IP ACL Rule  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclIpv6RuleTimeRangeName(void *wap,
                                                           void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibS32_t keyaclIpv6IndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6RuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objaclIpv6RuleTimeRangeNameValue;
  FPOBJ_TRACE_ENTER (bufp);
 
  /* retrieve key: aclIpv6Index */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                           (xLibU8_t *) & keyaclIpv6IndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, kwa1.len);

  /* retrieve key: aclIpv6RuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleIndex,
                           (xLibU8_t *) & keyaclIpv6RuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6RuleIndexValue, kwa2.len);


  /* get the value from application */
  owa.l7rc = usmDbQosAclRuleTimeRangeNameGet(L7_UNIT_CURRENT, keyaclIpv6IndexValue, keyaclIpv6RuleIndexValue, objaclIpv6RuleTimeRangeNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objaclIpv6RuleTimeRangeNameValue, owa.len);  
  /* return the object value: aclIpv6RuleTimeRangeName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objaclIpv6RuleTimeRangeNameValue,
                           strlen (objaclIpv6RuleTimeRangeNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclIpv6RuleTimeRangeName
*
* @purpose Set 'aclIpv6RuleTimeRangeName'
*
* @description [aclIpv6RuleTimeRangeName]:Name of time range associated with the IPv6 ACL Rule
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclIpv6RuleTimeRangeName (void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objaclIpv6RuleTimeRangeNameValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6IndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6RuleIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

   xLibU32_t objaclIpv6RuleStatusValue;
  /* retrieve row status object value */
  owa.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleStatus,
                   (xLibU8_t *) & objaclIpv6RuleStatusValue, &owa.len);
  if (objaclIpv6RuleStatusValue == L7_ROW_STATUS_DESTROY)
  {
    return XLIBRC_SUCCESS;
  }

  /* retrieve object: aclIpv6RuleLogging */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objaclIpv6RuleTimeRangeNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objaclIpv6RuleTimeRangeNameValue, owa.len);

  /* retrieve key: aclIpv6Index */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                           (xLibU8_t *) & keyaclIpv6IndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, kwa1.len);

  /* retrieve key: aclIpv6RuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleIndex,
                           (xLibU8_t *) & keyaclIpv6RuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6RuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbQosAclRuleTimeRangeNameAdd(L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                              keyaclIpv6RuleIndexValue,
                              objaclIpv6RuleTimeRangeNameValue);
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
* @function fpObjList_fastPathQOSACLQOSACL_aclIpv6RuleTimeRangeName
*
* @purpose Get 'aclIpv6RuleTimeRangeName'
*
* @description [aclIpv6RuleTimeRangeName]:Name of time range associated with the IP ACL Rule
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_fastPathQOSACLQOSACL_aclIpv6RuleTimeRangeName(void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
#ifdef L7_TIMERANGES_PACKAGE
  xLibStr256_t timeRangeNameValue;
  xLibStr256_t nextTimeRangeNameValue;
  xLibU32_t timeRangeIndexValue;
  xLibU32_t nextTimeRangeIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof (timeRangeNameValue);
  memset(timeRangeNameValue,0, sizeof(timeRangeNameValue));
  memset(nextTimeRangeNameValue, 0, sizeof(nextTimeRangeNameValue));
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleTimeRangeName,
                          (xLibU8_t *) timeRangeNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    timeRangeIndexValue =  0;
    nextTimeRangeIndexValue = 0;
    owa.l7rc = usmDbTimeRangeIndexFirstGet(L7_UNIT_CURRENT,
                  &nextTimeRangeIndexValue);
    if(owa.l7rc == L7_SUCCESS)
    {
          /* get the value from application */
          owa.l7rc = usmDbTimeRangeNameGet(L7_UNIT_CURRENT,
                                   nextTimeRangeIndexValue,
                                   (L7_uchar8 *) nextTimeRangeNameValue);

    }
  }
  else
  {

      owa.l7rc =usmDbTimeRangeNameToIndex(L7_UNIT_CURRENT, (L7_uchar8 *)timeRangeNameValue, &timeRangeIndexValue);
      if(owa.l7rc == L7_SUCCESS)
      {
          /* get next time range index and name */
          FPOBJ_TRACE_CURRENT_KEY(bufp, &timeRangeIndexValue, owa.len);
          owa.l7rc = usmDbTimeRangeIndexNextGet(L7_UNIT_CURRENT,  timeRangeIndexValue,
                  &nextTimeRangeIndexValue);
          if(owa.l7rc == L7_SUCCESS)
          {
             /* get the value from application */
              owa.l7rc = usmDbTimeRangeNameGet(L7_UNIT_CURRENT,
                                   nextTimeRangeIndexValue,
                                   (L7_uchar8 *) nextTimeRangeNameValue);
          }

      }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextTimeRangeNameValue, owa.len);

  /* return the object value: aclIfDirection */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextTimeRangeNameValue,
                           strlen(nextTimeRangeNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
#endif
  return owa.rc;
}
/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclIpv6RuleTimeRangeStatus
*
* @purpose Get 'aclIpv6RuleTimeRangeStatus'
*
* @description [aclIpv6RuleTimeRangeStatus]: Flag Indicates the IPv6 ACL Rule Status.
* ACL rule Status is derived from the status of time range associated with ACL. 
* If no time range is associated to an ACL rule then status of ACL rule is always active. 
* If time range is associated to an ACL rule and time range is non existing in system,
* then ACL rule status is considered to be active
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclIpv6RuleTimeRangeStatus (void *wap,
                                                           void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6IndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6RuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclIpv6RuleTimeRangeStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIpv6Index */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                           (xLibU8_t *) & keyaclIpv6IndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, kwa1.len);

  /* retrieve key: aclIpv6RuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleIndex,
                           (xLibU8_t *) & keyaclIpv6RuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6RuleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbQosAclRuleStatusGet (L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                        keyaclIpv6RuleIndexValue,
                                        &objaclIpv6RuleTimeRangeStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: aclIpv6RuleLogging */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objaclIpv6RuleTimeRangeStatusValue,
                           sizeof (objaclIpv6RuleTimeRangeStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjList_fastPathQOSACLQOSACL_aclIpv6RuleRedirectIntf
*
* @purpose List 'aclIpv6RuleRedirectIntf'
 *@description  [aclIpv6RuleRedirectIntf] A non-zero value indicates the external
* ifIndex to which all inbound packets matching this ACL rule are
* directed. A value of zero means packet redirection is not in effect,
* which is the default value of this object. Note that packet
* redirection and mirr
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_fastPathQOSACLQOSACL_aclIpv6RuleRedirectIntf (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objaclRuleRedirectIntfValue;
  xLibU32_t nextObjaclRuleRedirectIntfValue;
  xLibU32_t mask = USM_PHYSICAL_INTF;
  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof (objaclRuleRedirectIntfValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleRedirectIntf,
                          (xLibU8_t *) & objaclRuleRedirectIntfValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objaclRuleRedirectIntfValue =0;
  }
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclRuleRedirectIntfValue, owa.len);
  if (usmDbFeaturePresentCheck(L7_UNIT_CURRENT,L7_FLEX_QOS_ACL_COMPONENT_ID,L7_ACL_INTF_LAG_SUPPORT_FEATURE_ID) == L7_TRUE)
  {
    mask = USM_PHYSICAL_INTF | USM_LAG_INTF;
  }
  owa.l7rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT,  mask,
                                  0,objaclRuleRedirectIntfValue, &nextObjaclRuleRedirectIntfValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjaclRuleRedirectIntfValue, owa.len);

  /* return the object value: aclRuleRedirectIntf */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjaclRuleRedirectIntfValue,
                           sizeof (nextObjaclRuleRedirectIntfValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}



/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclIpv6RuleRedirectIntf
*
* @purpose Get 'aclIpv6RuleRedirectIntf'
*
* @description [aclIpv6RuleRedirectIntf]: A non-zero value indicates the external
*              ifIndex to which all inbound packets matching this
*              Ipv6 ACL rule are directed. A value of zero means packet redirection
*              is not in effect, which is the default value of this
*              object. Note that packet redirection and 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclIpv6RuleRedirectIntf (void *wap,
                                                                void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6IndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6RuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclIpv6RuleRedirectIntfValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIpv6Index */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                           (xLibU8_t *) & keyaclIpv6IndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, kwa1.len);

  /* retrieve key: aclIpv6RuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleIndex,
                           (xLibU8_t *) & keyaclIpv6RuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6RuleIndexValue, kwa2.len);

  /* get the value from application */
  if(usmDbQosAclIsFieldConfigured(L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                    keyaclIpv6RuleIndexValue, ACL_REDIRECT_INTF) == L7_TRUE)
  {
    owa.l7rc =
      usmDbQosAclRuleRedirectIntfGet (L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                    keyaclIpv6RuleIndexValue,
                                    &objaclIpv6RuleRedirectIntfValue);
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

  /* return the object value: aclIpv6RuleRedirectIntf */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclIpv6RuleRedirectIntfValue,
                           sizeof (objaclIpv6RuleRedirectIntfValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclIpv6RuleRedirectIntf
*
* @purpose Set 'aclIpv6RuleRedirectIntf'
*
* @description [aclIpv6RuleRedirectIntf]: A non-zero value indicates the external
*              ifIndex to which all inbound packets matching this
*              Ipv6 ACL rule are directed. A value of zero means packet redirection
*              is not in effect, which is the default value of this
*              object. Note that packet redirection and 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclIpv6RuleRedirectIntf (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclIpv6RuleRedirectIntfValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6IndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6RuleIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  xLibU32_t objaclIpv6RuleStatusValue;
  /* retrieve row status object value */
  owa.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleStatus,
                   (xLibU8_t *) & objaclIpv6RuleStatusValue, &owa.len);
  if (objaclIpv6RuleStatusValue == L7_ROW_STATUS_DESTROY)
  {
    return XLIBRC_SUCCESS;
  }

  /* retrieve object: aclIpv6RuleRedirectIntf */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclIpv6RuleRedirectIntfValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclIpv6RuleRedirectIntfValue, owa.len);

  /* retrieve key: aclIpv6Index */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                           (xLibU8_t *) & keyaclIpv6IndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, kwa1.len);

  /* retrieve key: aclIpv6RuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleIndex,
                           (xLibU8_t *) & keyaclIpv6RuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6RuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc =
    usmDbQosAclRuleRedirectIntfAdd (L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                    keyaclIpv6RuleIndexValue,
                                    objaclIpv6RuleRedirectIntfValue);
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
* @function fpObjList_fastPathQOSACLQOSACL_aclIpv6RuleMirrorIntf
*
* @purpose List 'aclIpv6RuleMirrorIntf'
 *@description  [aclIpv6RuleMirrorIntf] A non-zero value indicates the external
* ifIndex to which all inbound packets matching this ACL rule are
* copied. A value of zero means packet mirroring is not in effect, which
* is the default value of this object. Note that packet mirroring
* and redirectio
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_fastPathQOSACLQOSACL_aclIpv6RuleMirrorIntf (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objaclRuleMirrorIntfValue;
  xLibU32_t nextObjaclRuleMirrorIntfValue;
  xLibU32_t mask = USM_PHYSICAL_INTF;

  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof (objaclRuleMirrorIntfValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleMirrorIntf,
                          (xLibU8_t *) & objaclRuleMirrorIntfValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objaclRuleMirrorIntfValue =0;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclRuleMirrorIntfValue, owa.len);
  if (usmDbFeaturePresentCheck(L7_UNIT_CURRENT,L7_FLEX_QOS_ACL_COMPONENT_ID,L7_ACL_INTF_LAG_SUPPORT_FEATURE_ID) == L7_TRUE)
  {
    mask = USM_PHYSICAL_INTF | USM_LAG_INTF;
  }
  owa.l7rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT,  mask,
                                  0,objaclRuleMirrorIntfValue, &nextObjaclRuleMirrorIntfValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjaclRuleMirrorIntfValue, owa.len);

  /* return the object value: aclRuleMirrorIntf */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjaclRuleMirrorIntfValue,
                           sizeof (nextObjaclRuleMirrorIntfValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclIpv6RuleMirrorIntf
*
* @purpose Get 'aclIpv6RuleMirrorIntf'
*
* @description [aclIpv6RuleMirrorIntf]: A non-zero value indicates the external
*              ifIndex to which all inbound packets matching this IPv6
*              ACL rule are copied. A value of zero means packet mirroring
*              is not in effect, which is the default value of this object.
*              Note that packet mirroring and redir 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclIpv6RuleMirrorIntf (void *wap,
                                                              void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6IndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6RuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclIpv6RuleMirrorIntfValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIpv6Index */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                           (xLibU8_t *) & keyaclIpv6IndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, kwa1.len);

  /* retrieve key: aclIpv6RuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleIndex,
                           (xLibU8_t *) & keyaclIpv6RuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6RuleIndexValue, kwa2.len);

  /* get the value from application */
  if(usmDbQosAclIsFieldConfigured(L7_UNIT_CURRENT, keyaclIpv6IndexValue, keyaclIpv6RuleIndexValue,
          ACL_MIRROR_INTF) == L7_TRUE)
  {
    owa.l7rc =
       usmDbQosAclRuleMirrorIntfGet (L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                  keyaclIpv6RuleIndexValue,
                                  &objaclIpv6RuleMirrorIntfValue);
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

  /* return the object value: aclIpv6RuleMirrorIntf */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclIpv6RuleMirrorIntfValue,
                           sizeof (objaclIpv6RuleMirrorIntfValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclIpv6RuleMirrorIntf
*
* @purpose Set 'aclIpv6RuleMirrorIntf'
*
* @description [aclIpv6RuleMirrorIntf]: A non-zero value indicates the external
*              ifIndex to which all inbound packets matching this IPv6
*              ACL rule are copied. A value of zero means packet mirroring
*              is not in effect, which is the default value of this object.
*              Note that packet mirroring and redir 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclIpv6RuleMirrorIntf (void *wap,
                                                              void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclIpv6RuleMirrorIntfValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6IndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6RuleIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  xLibU32_t objaclIpv6RuleStatusValue;
  /* retrieve row status object value */
  owa.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleStatus,
                   (xLibU8_t *) & objaclIpv6RuleStatusValue, &owa.len);
  if (objaclIpv6RuleStatusValue == L7_ROW_STATUS_DESTROY)
  {
    return XLIBRC_SUCCESS;
  }

  /* retrieve object: aclIpv6RuleMirrorIntf */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclIpv6RuleMirrorIntfValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclIpv6RuleMirrorIntfValue, owa.len);

  /* retrieve key: aclIpv6Index */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                           (xLibU8_t *) & keyaclIpv6IndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, kwa1.len);

  /* retrieve key: aclIpv6RuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleIndex,
                           (xLibU8_t *) & keyaclIpv6RuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6RuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc =
    usmDbQosAclRuleMirrorIntfAdd (L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                  keyaclIpv6RuleIndexValue,
                                  objaclIpv6RuleMirrorIntfValue);
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
* @function fpObjGet_fastPathQOSACLQOSACL_aclIpv6RuleMatchEvery
*
* @purpose Get 'aclIpv6RuleMatchEvery'
*
* @description [aclIpv6RuleMatchEvery]: Flag to indicate that the ACL rule
*              is defined to match on every IP packet regardless of content.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclIpv6RuleMatchEvery (void *wap,
                                                              void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6IndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6RuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclIpv6RuleMatchEveryValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIpv6Index */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                           (xLibU8_t *) & keyaclIpv6IndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, kwa1.len);

  /* retrieve key: aclIpv6RuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleIndex,
                           (xLibU8_t *) & keyaclIpv6RuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6RuleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbQosAclRuleEveryGet (L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                      keyaclIpv6RuleIndexValue,
                                      &objaclIpv6RuleMatchEveryValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: aclIpv6RuleMatchEvery */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclIpv6RuleMatchEveryValue,
                           sizeof (objaclIpv6RuleMatchEveryValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclIpv6RuleMatchEvery
*
* @purpose Set 'aclIpv6RuleMatchEvery'
*
* @description [aclIpv6RuleMatchEvery]: Flag to indicate that the ACL rule
*              is defined to match on every IP packet regardless of content.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclIpv6RuleMatchEvery (void *wap,
                                                              void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclIpv6RuleMatchEveryValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6IndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6RuleIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  xLibU32_t objaclIpv6RuleStatusValue;
  /* retrieve row status object value */
  owa.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleStatus,
                   (xLibU8_t *) & objaclIpv6RuleStatusValue, &owa.len);
  if (objaclIpv6RuleStatusValue == L7_ROW_STATUS_DESTROY)
  {
    return XLIBRC_SUCCESS;
  }

  /* retrieve object: aclIpv6RuleMatchEvery */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclIpv6RuleMatchEveryValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclIpv6RuleMatchEveryValue, owa.len);

  /* retrieve key: aclIpv6Index */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                           (xLibU8_t *) & keyaclIpv6IndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, kwa1.len);

  /* retrieve key: aclIpv6RuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleIndex,
                           (xLibU8_t *) & keyaclIpv6RuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6RuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbQosAclRuleEveryAdd (L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                      keyaclIpv6RuleIndexValue,
                                      objaclIpv6RuleMatchEveryValue);
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
* @function fpObjGet_fastPathQOSACLQOSACL_aclIpv6RuleProtocol
*
* @purpose Get 'aclIpv6RuleProtocol'
*
* @description [aclIpv6RuleProtocol]: icmp - 1 igmp - 2 ip - 4 tcp - 6 udp
*              - 17 All values from 1 to 255 are valid. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclIpv6RuleProtocol (void *wap,
                                                            void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6IndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6RuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclIpv6RuleProtocolValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIpv6Index */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                           (xLibU8_t *) & keyaclIpv6IndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, kwa1.len);

  /* retrieve key: aclIpv6RuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleIndex,
                           (xLibU8_t *) & keyaclIpv6RuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6RuleIndexValue, kwa2.len);

  /* get the value from application */
  if(usmDbQosAclIsFieldConfigured(L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                         keyaclIpv6RuleIndexValue, ACL_PROTOCOL) == L7_TRUE)
  {
     owa.l7rc = usmDbQosAclRuleProtocolGet (L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                         keyaclIpv6RuleIndexValue,
                                         &objaclIpv6RuleProtocolValue);
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

  /* return the object value: aclIpv6RuleProtocol */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclIpv6RuleProtocolValue,
                           sizeof (objaclIpv6RuleProtocolValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclIpv6RuleProtocol
*
* @purpose Set 'aclIpv6RuleProtocol'
*
* @description [aclIpv6RuleProtocol]: icmp - 1 igmp - 2 ip - 4 tcp - 6 udp
*              - 17 All values from 1 to 255 are valid. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclIpv6RuleProtocol (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclIpv6RuleProtocolValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6IndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6RuleIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  xLibU32_t objaclIpv6RuleStatusValue;
  /* retrieve row status object value */
  owa.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleStatus,
                   (xLibU8_t *) & objaclIpv6RuleStatusValue, &owa.len);
  if (objaclIpv6RuleStatusValue == L7_ROW_STATUS_DESTROY)
  {
    return XLIBRC_SUCCESS;
  }

  /* retrieve object: aclIpv6RuleProtocol */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclIpv6RuleProtocolValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclIpv6RuleProtocolValue, owa.len);

  /* retrieve key: aclIpv6Index */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                           (xLibU8_t *) & keyaclIpv6IndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, kwa1.len);

  /* retrieve key: aclIpv6RuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleIndex,
                           (xLibU8_t *) & keyaclIpv6RuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6RuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbQosAclRuleProtocolAdd (L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                         keyaclIpv6RuleIndexValue,
                                         objaclIpv6RuleProtocolValue);
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
* @function fpObjGet_fastPathQOSACLQOSACL_aclIpv6RuleSrcL4Port
*
* @purpose Get 'aclIpv6RuleSrcL4Port'
*
* @description [aclIpv6RuleSrcL4Port]: The Source Port Number (Layer 4) used
*              in the ACL Classification. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclIpv6RuleSrcL4Port (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6IndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6RuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclIpv6RuleSrcL4PortValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIpv6Index */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                           (xLibU8_t *) & keyaclIpv6IndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, kwa1.len);

  /* retrieve key: aclIpv6RuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleIndex,
                           (xLibU8_t *) & keyaclIpv6RuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6RuleIndexValue, kwa2.len);

  /* get the value from application */
  if (usmDbQosAclIsFieldConfigured(L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                          keyaclIpv6RuleIndexValue,
                                          ACL_SRCPORT) == L7_TRUE)
  {
     owa.l7rc = usmDbQosAclRuleSrcL4PortGet (L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                          keyaclIpv6RuleIndexValue,
                                          &objaclIpv6RuleSrcL4PortValue);
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

  /* return the object value: aclIpv6RuleSrcL4Port */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclIpv6RuleSrcL4PortValue,
                           sizeof (objaclIpv6RuleSrcL4PortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclIpv6RuleSrcL4Port
*
* @purpose Set 'aclIpv6RuleSrcL4Port'
*
* @description [aclIpv6RuleSrcL4Port]: The Source Port Number (Layer 4) used
*              in the ACL Classification. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclIpv6RuleSrcL4Port (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclIpv6RuleSrcL4PortValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6IndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6RuleIndexValue;
  FPOBJ_TRACE_ENTER (bufp);
    xLibU32_t objaclIpv6RuleStatusValue;
  /* retrieve row status object value */
  owa.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleStatus,
                   (xLibU8_t *) & objaclIpv6RuleStatusValue, &owa.len);
  if (objaclIpv6RuleStatusValue == L7_ROW_STATUS_DESTROY)
  {
    return XLIBRC_SUCCESS;
  }

  /* retrieve object: aclIpv6RuleSrcL4Port */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclIpv6RuleSrcL4PortValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclIpv6RuleSrcL4PortValue, owa.len);

  /* retrieve key: aclIpv6Index */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                           (xLibU8_t *) & keyaclIpv6IndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, kwa1.len);

  /* retrieve key: aclIpv6RuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleIndex,
                           (xLibU8_t *) & keyaclIpv6RuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6RuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbQosAclRuleSrcL4PortAdd (L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                          keyaclIpv6RuleIndexValue,
                                          objaclIpv6RuleSrcL4PortValue);
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
* @function fpObjGet_fastPathQOSACLQOSACL_aclIpv6RuleSrcL4PortRangeStart
*
* @purpose Get 'aclIpv6RuleSrcL4PortRangeStart'
*
* @description [aclIpv6RuleSrcL4PortRangeStart]: The Source Port Number(Layer
*              4) range start. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclIpv6RuleSrcL4PortRangeStart (void
                                                                       *wap,
                                                                       void
                                                                       *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6IndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6RuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclIpv6RuleSrcL4PortRangeStartValue;
  L7_uint32 endport;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIpv6Index */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                           (xLibU8_t *) & keyaclIpv6IndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, kwa1.len);

  /* retrieve key: aclIpv6RuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleIndex,
                           (xLibU8_t *) & keyaclIpv6RuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6RuleIndexValue, kwa2.len);

  /* get the value from application */
  if (usmDbQosAclIsFieldConfigured(L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                      keyaclIpv6RuleIndexValue,ACL_SRCSTARTPORT) == L7_TRUE)
  {
    owa.l7rc =
      usmDbQosAclRuleSrcL4PortRangeGet (L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                      keyaclIpv6RuleIndexValue,
                                      &objaclIpv6RuleSrcL4PortRangeStartValue,
                                      &endport);
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

  /* return the object value: aclIpv6RuleSrcL4PortRangeStart */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objaclIpv6RuleSrcL4PortRangeStartValue,
                    sizeof (objaclIpv6RuleSrcL4PortRangeStartValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclIpv6RuleSrcL4PortRangeStart
*
* @purpose Set 'aclIpv6RuleSrcL4PortRangeStart'
*
* @description [aclIpv6RuleSrcL4PortRangeStart]: The Source Port Number(Layer
*              4) range start. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclIpv6RuleSrcL4PortRangeStart (void
                                                                       *wap,
                                                                       void
                                                                       *bufp)
{
    return XLIBRC_SUCCESS;
}


/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclIpv6RuleSrcL4PortRangeEnd
*
* @purpose Get 'aclIpv6RuleSrcL4PortRangeEnd'
*
* @description [aclIpv6RuleSrcL4PortRangeEnd]: The Source Port Number(Layer
*              4) range end. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclIpv6RuleSrcL4PortRangeEnd (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6IndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6RuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclIpv6RuleSrcL4PortRangeEndValue;
  L7_uint32 startPort;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIpv6Index */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                           (xLibU8_t *) & keyaclIpv6IndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, kwa1.len);

  /* retrieve key: aclIpv6RuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleIndex,
                           (xLibU8_t *) & keyaclIpv6RuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6RuleIndexValue, kwa2.len);

  /* get the value from application */
  if (usmDbQosAclIsFieldConfigured(L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                      keyaclIpv6RuleIndexValue, ACL_SRCENDPORT) == L7_TRUE)
  {
    owa.l7rc =
       usmDbQosAclRuleSrcL4PortRangeGet (L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                      keyaclIpv6RuleIndexValue,
                                      &startPort,
                                      &objaclIpv6RuleSrcL4PortRangeEndValue);
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

  /* return the object value: aclIpv6RuleSrcL4PortRangeEnd */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objaclIpv6RuleSrcL4PortRangeEndValue,
                    sizeof (objaclIpv6RuleSrcL4PortRangeEndValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclIpv6RuleSrcL4PortRangeEnd
*
* @purpose Set 'aclIpv6RuleSrcL4PortRangeEnd'
*
* @description [aclIpv6RuleSrcL4PortRangeEnd]: The Source Port Number(Layer
*              4) range end. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclIpv6RuleSrcL4PortRangeEnd (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclIpv6RuleSrcL4PortRangeEndValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6IndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6RuleIndexValue;
  L7_uint32 startPort;
  FPOBJ_TRACE_ENTER (bufp);

  xLibU32_t objaclIpv6RuleStatusValue;
  /* retrieve row status object value */
  owa.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleStatus,
                   (xLibU8_t *) & objaclIpv6RuleStatusValue, &owa.len);
  if (objaclIpv6RuleStatusValue == L7_ROW_STATUS_DESTROY)
  {
    return XLIBRC_SUCCESS;
  }

  /* retrieve object: aclIpv6RuleSrcL4PortRangeEnd */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclIpv6RuleSrcL4PortRangeEndValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclIpv6RuleSrcL4PortRangeEndValue, owa.len);

  /* retrieve key: aclIpv6Index */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                           (xLibU8_t *) & keyaclIpv6IndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, kwa1.len);

  /* retrieve key: aclIpv6RuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleIndex,
                           (xLibU8_t *) & keyaclIpv6RuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6RuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.len = sizeof(startPort);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleSrcL4PortRangeStart,(xLibU8_t *) & startPort,&owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return owa.rc;
  }
  if (startPort > objaclIpv6RuleSrcL4PortRangeEndValue)
  {
    owa.rc = XLIBRC_ACL_INVALID_PORT_RANGE ;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return owa.rc;
  }
  owa.l7rc =
    usmDbQosAclRuleSrcL4PortRangeAdd (L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                      keyaclIpv6RuleIndexValue,
                                      startPort,
                                      objaclIpv6RuleSrcL4PortRangeEndValue);
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
* @function fpObjGet_fastPathQOSACLQOSACL_aclIpv6RuleDestL4Port
*
* @purpose Get 'aclIpv6RuleDestL4Port'
*
* @description [aclIpv6RuleDestL4Port]: The Destination Port (Layer 4) used
*              in ACl classification. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclIpv6RuleDestL4Port (void *wap,
                                                              void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6IndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6RuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclIpv6RuleDestL4PortValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIpv6Index */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                           (xLibU8_t *) & keyaclIpv6IndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, kwa1.len);

  /* retrieve key: aclIpv6RuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleIndex,
                           (xLibU8_t *) & keyaclIpv6RuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6RuleIndexValue, kwa2.len);

  /* get the value from application */
  if (usmDbQosAclIsFieldConfigured(L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                          keyaclIpv6RuleIndexValue, ACL_DSTPORT) == L7_TRUE)
  {
    owa.l7rc = usmDbQosAclRuleDstL4PortGet (L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                          keyaclIpv6RuleIndexValue,
                                          &objaclIpv6RuleDestL4PortValue);
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

  /* return the object value: aclIpv6RuleDestL4Port */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclIpv6RuleDestL4PortValue,
                           sizeof (objaclIpv6RuleDestL4PortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclIpv6RuleDestL4Port
*
* @purpose Set 'aclIpv6RuleDestL4Port'
*
* @description [aclIpv6RuleDestL4Port]: The Destination Port (Layer 4) used
*              in ACl classification. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclIpv6RuleDestL4Port (void *wap,
                                                              void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclIpv6RuleDestL4PortValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6IndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6RuleIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  xLibU32_t objaclIpv6RuleStatusValue;
  /* retrieve row status object value */
  owa.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleStatus,
                   (xLibU8_t *) & objaclIpv6RuleStatusValue, &owa.len);
  if (objaclIpv6RuleStatusValue == L7_ROW_STATUS_DESTROY)
  {
    return XLIBRC_SUCCESS;
  }

  /* retrieve object: aclIpv6RuleDestL4Port */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclIpv6RuleDestL4PortValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclIpv6RuleDestL4PortValue, owa.len);

  /* retrieve key: aclIpv6Index */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                           (xLibU8_t *) & keyaclIpv6IndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, kwa1.len);

  /* retrieve key: aclIpv6RuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleIndex,
                           (xLibU8_t *) & keyaclIpv6RuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6RuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbQosAclRuleDstL4PortAdd (L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                          keyaclIpv6RuleIndexValue,
                                          objaclIpv6RuleDestL4PortValue);
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
* @function fpObjGet_fastPathQOSACLQOSACL_aclIpv6RuleDestL4PortRangeStart
*
* @purpose Get 'aclIpv6RuleDestL4PortRangeStart'
*
* @description [aclIpv6RuleDestL4PortRangeStart]: The Destination Port (Layer
*              4) starting range used in ACL classification. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclIpv6RuleDestL4PortRangeStart (void
                                                                        *wap,
                                                                        void
                                                                        *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6IndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6RuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclIpv6RuleDestL4PortRangeStartValue;
  L7_uint32 endPort;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIpv6Index */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                           (xLibU8_t *) & keyaclIpv6IndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, kwa1.len);

  /* retrieve key: aclIpv6RuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleIndex,
                           (xLibU8_t *) & keyaclIpv6RuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6RuleIndexValue, kwa2.len);

  /* get the value from application */
  if (usmDbQosAclIsFieldConfigured(L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                      keyaclIpv6RuleIndexValue,ACL_DSTSTARTPORT) == L7_TRUE)
  {
    owa.l7rc =
      usmDbQosAclRuleDstL4PortRangeGet (L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                      keyaclIpv6RuleIndexValue,
                                      &objaclIpv6RuleDestL4PortRangeStartValue,
                                      &endPort);
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

  /* return the object value: aclIpv6RuleDestL4PortRangeStart */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) & objaclIpv6RuleDestL4PortRangeStartValue,
                    sizeof (objaclIpv6RuleDestL4PortRangeStartValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclIpv6RuleDestL4PortRangeStart
*
* @purpose Set 'aclIpv6RuleDestL4PortRangeStart'
*
* @description [aclIpv6RuleDestL4PortRangeStart]: The Destination Port (Layer
*              4) starting range used in ACL classification. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclIpv6RuleDestL4PortRangeStart (void
                                                                        *wap,
                                                                        void
                                                                        *bufp)
{
  return XLIBRC_SUCCESS;
}


/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclIpv6RuleDestL4PortRangeEnd
*
* @purpose Get 'aclIpv6RuleDestL4PortRangeEnd'
*
* @description [aclIpv6RuleDestL4PortRangeEnd]: The Destination Port (Layer
*              4) ending range used in ACL classification. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclIpv6RuleDestL4PortRangeEnd (void *wap,
                                                                      void
                                                                      *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6IndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6RuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclIpv6RuleDestL4PortRangeEndValue;
  L7_uint32 startPort;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIpv6Index */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                           (xLibU8_t *) & keyaclIpv6IndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, kwa1.len);

  /* retrieve key: aclIpv6RuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleIndex,
                           (xLibU8_t *) & keyaclIpv6RuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6RuleIndexValue, kwa2.len);

  /* get the value from application */
  if (usmDbQosAclIsFieldConfigured(L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                      keyaclIpv6RuleIndexValue,ACL_DSTENDPORT) == L7_TRUE) 
  {
    owa.l7rc =
       usmDbQosAclRuleDstL4PortRangeGet (L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                      keyaclIpv6RuleIndexValue,
                                      &startPort,
                                      &objaclIpv6RuleDestL4PortRangeEndValue);
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

  /* return the object value: aclIpv6RuleDestL4PortRangeEnd */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objaclIpv6RuleDestL4PortRangeEndValue,
                    sizeof (objaclIpv6RuleDestL4PortRangeEndValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclIpv6RuleDestL4PortRangeEnd
*
* @purpose Set 'aclIpv6RuleDestL4PortRangeEnd'
*
* @description [aclIpv6RuleDestL4PortRangeEnd]: The Destination Port (Layer
*              4) ending range used in ACL classification. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclIpv6RuleDestL4PortRangeEnd (void *wap,
                                                                      void
                                                                      *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclIpv6RuleDestL4PortRangeEndValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6IndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6RuleIndexValue;
  L7_uint32 startPort;
  FPOBJ_TRACE_ENTER (bufp);

  xLibU32_t objaclIpv6RuleStatusValue;
  /* retrieve row status object value */
  owa.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleStatus,
                   (xLibU8_t *) & objaclIpv6RuleStatusValue, &owa.len);
  if (objaclIpv6RuleStatusValue == L7_ROW_STATUS_DESTROY)
  {
    return XLIBRC_SUCCESS;
  }

  /* retrieve object: aclIpv6RuleDestL4PortRangeEnd */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclIpv6RuleDestL4PortRangeEndValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclIpv6RuleDestL4PortRangeEndValue, owa.len);

  /* retrieve key: aclIpv6Index */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                           (xLibU8_t *) & keyaclIpv6IndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, kwa1.len);

  /* retrieve key: aclIpv6RuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleIndex,
                           (xLibU8_t *) & keyaclIpv6RuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6RuleIndexValue, kwa2.len);
  owa.len = sizeof(startPort);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleDestL4PortRangeStart,(xLibU8_t *) & startPort,&owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return owa.rc;
  }
  if (startPort > objaclIpv6RuleDestL4PortRangeEndValue)
  {
    owa.rc = XLIBRC_ACL_INVALID_PORT_RANGE ;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return owa.rc;
  }

  /* set the value in application */
  owa.l7rc =
    usmDbQosAclRuleDstL4PortRangeAdd (L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                      keyaclIpv6RuleIndexValue,
                                      startPort,
                                      objaclIpv6RuleDestL4PortRangeEndValue);
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
* @function fpObjGet_fastPathQOSACLQOSACL_aclIpv6RuleStatus
*
* @purpose Get 'aclIpv6RuleStatus'
*
* @description [aclIpv6RuleStatus]: active(1) - this ACL Rule is active createAndGo(4)
*              - set to this value to create an instance destroy(6)
*              - set to this value to delete an instance 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclIpv6RuleStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6IndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6RuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclIpv6RuleStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIpv6Index */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                           (xLibU8_t *) & keyaclIpv6IndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, kwa1.len);

  /* retrieve key: aclIpv6RuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleIndex,
                           (xLibU8_t *) & keyaclIpv6RuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6RuleIndexValue, kwa2.len);

  /* get the value from application */
  objaclIpv6RuleStatusValue = L7_ROW_STATUS_ACTIVE;
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: aclIpv6RuleStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclIpv6RuleStatusValue,
                           sizeof (objaclIpv6RuleStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclIpv6RuleStatus
*
* @purpose Set 'aclIpv6RuleStatus'
*
* @description [aclIpv6RuleStatus]: active(1) - this ACL Rule is active createAndGo(4)
*              - set to this value to create an instance destroy(6)
*              - set to this value to delete an instance 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclIpv6RuleStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclIpv6RuleStatusValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6IndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6RuleIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclIpv6RuleStatus */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclIpv6RuleStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclIpv6RuleStatusValue, owa.len);

  /* retrieve key: aclIpv6Index */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                           (xLibU8_t *) &keyaclIpv6IndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, kwa1.len);

  /* retrieve key: aclIpv6RuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleIndex,
                           (xLibU8_t *) & keyaclIpv6RuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6RuleIndexValue, kwa2.len);

    /* call the usmdb only for add and delete */
  if (objaclIpv6RuleStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    /* Create a row */
    owa.l7rc = usmDbQosAclRuleActionAdd(L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                keyaclIpv6RuleIndexValue,L7_ACL_DENY);
    if (owa.l7rc != L7_SUCCESS)
    {
      if (owa.l7rc == L7_REQUEST_DENIED)
      {
        owa.rc = XLIBRC_ACL_CHANGE_REQ_DENY_APP_RULE_LIMIT;
      }
      else
      {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      }
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else if (objaclIpv6RuleStatusValue == L7_ROW_STATUS_DESTROY)
  {
    /* Delete the existing row */
    owa.l7rc = usmDbQosAclRuleRemove(L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                keyaclIpv6RuleIndexValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      if (owa.l7rc == L7_REQUEST_DENIED)
      {
        owa.rc = XLIBRC_ACL_CHANGE_REQ_DENY_APP_USING;
      }
      else
      {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      }
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  return XLIBRC_SUCCESS;
}


/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclIpv6RuleFlowLabel
*
* @purpose Get 'aclIpv6RuleFlowLabel'
*
* @description [aclIpv6RuleFlowLabel]: Flow label is 20-bit number that is
*              unique to an IPv6 packet, used by end stations to signify
*              quality-of-service handling in routers. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclIpv6RuleFlowLabel (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6IndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6RuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclIpv6RuleFlowLabelValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIpv6Index */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                           (xLibU8_t *) & keyaclIpv6IndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, kwa1.len);

  /* retrieve key: aclIpv6RuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleIndex,
                           (xLibU8_t *) & keyaclIpv6RuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6RuleIndexValue, kwa2.len);

  /* get the value from application */
  if (usmDbQosAclIsFieldConfigured(L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                     keyaclIpv6RuleIndexValue,ACL_FLOWLBLV6) == L7_TRUE)
  {
    owa.l7rc =
       usmDbQosAclRuleIpv6FlowLabelGet (L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                     keyaclIpv6RuleIndexValue,
                                     &objaclIpv6RuleFlowLabelValue);
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

  /* return the object value: aclIpv6RuleFlowLabel */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclIpv6RuleFlowLabelValue,
                           sizeof (objaclIpv6RuleFlowLabelValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclIpv6RuleFlowLabel
*
* @purpose Set 'aclIpv6RuleFlowLabel'
*
* @description [aclIpv6RuleFlowLabel]: Flow label is 20-bit number that is
*              unique to an IPv6 packet, used by end stations to signify
*              quality-of-service handling in routers. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclIpv6RuleFlowLabel (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclIpv6RuleFlowLabelValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6IndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6RuleIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclIpv6RuleFlowLabel */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclIpv6RuleFlowLabelValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclIpv6RuleFlowLabelValue, owa.len);

  /* retrieve key: aclIpv6Index */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                           (xLibU8_t *) & keyaclIpv6IndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, kwa1.len);

  /* retrieve key: aclIpv6RuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleIndex,
                           (xLibU8_t *) & keyaclIpv6RuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6RuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc =
    usmDbQosAclRuleIpv6FlowLabelAdd (L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                     keyaclIpv6RuleIndexValue,
                                     objaclIpv6RuleFlowLabelValue);
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
* @function fpObjGet_fastPathQOSACLQOSACL_aclIpv6RuleIPDSCP
*
* @purpose Get 'aclIpv6RuleIPDSCP'
*
* @description [aclIpv6RuleIPDSCP]: The Differentiated Services Code Point
*              value. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclIpv6RuleIPDSCP (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6IndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6RuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclIpv6RuleIPDSCPValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIpv6Index */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                           (xLibU8_t *) & keyaclIpv6IndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, kwa1.len);

  /* retrieve key: aclIpv6RuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleIndex,
                           (xLibU8_t *) & keyaclIpv6RuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6RuleIndexValue, kwa2.len);

  /* get the value from application */
  if (usmDbQosAclIsFieldConfigured(L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                       keyaclIpv6RuleIndexValue, ACL_IPDSCP) == L7_TRUE)
  {
    owa.l7rc = usmDbQosAclRuleIPDscpGet (L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                       keyaclIpv6RuleIndexValue,
                                       &objaclIpv6RuleIPDSCPValue);
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

  /* return the object value: aclIpv6RuleIPDSCP */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclIpv6RuleIPDSCPValue,
                           sizeof (objaclIpv6RuleIPDSCPValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclIpv6RuleIPDSCP
*
* @purpose Set 'aclIpv6RuleIPDSCP'
*
* @description [aclIpv6RuleIPDSCP]: The Differentiated Services Code Point
*              value. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclIpv6RuleIPDSCP (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclIpv6RuleIPDSCPValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6IndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6RuleIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclIpv6RuleIPDSCP */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclIpv6RuleIPDSCPValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclIpv6RuleIPDSCPValue, owa.len);

  /* retrieve key: aclIpv6Index */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                           (xLibU8_t *) & keyaclIpv6IndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, kwa1.len);

  /* retrieve key: aclIpv6RuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleIndex,
                           (xLibU8_t *) & keyaclIpv6RuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6RuleIndexValue, kwa2.len);

  /* set the value in application */
  if (objaclIpv6RuleIPDSCPValue != L7_ACL_IPDSCP_OTHER)
  {
    owa.l7rc = usmDbQosAclRuleIPDscpAdd (L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                         keyaclIpv6RuleIndexValue,
                                         objaclIpv6RuleIPDSCPValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclIpv6RuleIPDSCPValue
*
* @purpose Get 'aclIpv6RuleIPDSCPValue'
*
* @description [aclIpv6RuleIPDSCPValue]: The Differentiated Services Code Point
*              value. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclIpv6RuleIPDSCPValue (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6IndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6RuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclIpv6RuleIPDSCPValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIpv6Index */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                           (xLibU8_t *) & keyaclIpv6IndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, kwa1.len);

  /* retrieve key: aclIpv6RuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleIndex,
                           (xLibU8_t *) & keyaclIpv6RuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6RuleIndexValue, kwa2.len);

  /* get the value from application */
  if (usmDbQosAclIsFieldConfigured(L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                       keyaclIpv6RuleIndexValue, ACL_IPDSCP) == L7_TRUE)
  {
    owa.l7rc = usmDbQosAclRuleIPDscpGet (L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                       keyaclIpv6RuleIndexValue,
                                       &objaclIpv6RuleIPDSCPValue);
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

  /* return the object value: aclIpv6RuleIPDSCP */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclIpv6RuleIPDSCPValue,
                           sizeof (objaclIpv6RuleIPDSCPValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclIpv6RuleIPDSCPValue
*
* @purpose Set 'aclIpv6RuleIPDSCPValue'
*
* @description [aclIpv6RuleIPDSCPValue]: The Differentiated Services Code Point
*              value. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclIpv6RuleIPDSCPValue (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclIpv6RuleIPDSCPValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6IndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6RuleIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclIpv6RuleIPDSCP */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclIpv6RuleIPDSCPValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclIpv6RuleIPDSCPValue, owa.len);

  /* retrieve key: aclIpv6Index */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                           (xLibU8_t *) & keyaclIpv6IndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, kwa1.len);

  /* retrieve key: aclIpv6RuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleIndex,
                           (xLibU8_t *) & keyaclIpv6RuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6RuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbQosAclRuleIPDscpAdd (L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                       keyaclIpv6RuleIndexValue,
                                       objaclIpv6RuleIPDSCPValue);
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
* @function fpObjGet_fastPathQOSACLQOSACL_aclRuleSrcIpv6Prefix
*
* @purpose Get 'aclRuleSrcIpv6Prefix'
*
* @description [aclRuleSrcIpv6Prefix]: The Ipv6 Prefix Address configured
*              on the Service Port. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclRuleSrcIpv6Prefix (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6IndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6RuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV6_t));
  xLibIpV6_t objaclRuleSrcIpv6PrefixValue;
  L7_in6_prefix_t ipv6Addr;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIpv6Index */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                           (xLibU8_t *) & keyaclIpv6IndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, kwa1.len);

  /* retrieve key: aclIpv6RuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleIndex,
                           (xLibU8_t *) & keyaclIpv6RuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6RuleIndexValue, kwa2.len);

  /* get the value from application */
  if (usmDbQosAclIsFieldConfigured(L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                   keyaclIpv6RuleIndexValue, ACL_SRCIPV6) == L7_TRUE)
  {
    owa.l7rc =
       usmDbQosAclRuleSrcIpv6AddrGet (L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                   keyaclIpv6RuleIndexValue,
                                   &ipv6Addr);
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

  memcpy(&objaclRuleSrcIpv6PrefixValue, &ipv6Addr.in6Addr, sizeof(objaclRuleSrcIpv6PrefixValue));
  /* return the object value: aclRuleSrcIpv6Prefix */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclRuleSrcIpv6PrefixValue,
                           sizeof (objaclRuleSrcIpv6PrefixValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclRuleSrcIpv6Prefix
*
* @purpose Set 'aclRuleSrcIpv6Prefix'
*
* @description [aclRuleSrcIpv6Prefix]: The Ipv6 Prefix Address configured
*              on the Service Port. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclRuleSrcIpv6Prefix (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV6_t));
  xLibIpV6_t objaclRuleSrcIpv6PrefixValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6IndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6RuleIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclIpv6PrefixLengthValue;
  L7_in6_prefix_t ipv6Addr;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclRuleSrcIpv6Prefix */
  memset(&ipv6Addr, 0x00, sizeof(ipv6Addr));
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclRuleSrcIpv6PrefixValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclRuleSrcIpv6PrefixValue, owa.len);
  memcpy(&ipv6Addr.in6Addr, &objaclRuleSrcIpv6PrefixValue, sizeof(ipv6Addr.in6Addr));

  /* retrieve key: aclIpv6Index */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                           (xLibU8_t *) & keyaclIpv6IndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, kwa1.len);

  /* retrieve key: aclIpv6RuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleIndex,
                           (xLibU8_t *) & keyaclIpv6RuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6RuleIndexValue, kwa2.len);

  kwa3.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleSrcIpv6PrefixLength,
                           (xLibU8_t *) & objaclIpv6PrefixLengthValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    objaclIpv6PrefixLengthValue=0;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclIpv6PrefixLengthValue, kwa3.len);
  ipv6Addr.in6PrefixLen = (L7_int32) objaclIpv6PrefixLengthValue;

  /* set the value in application */
  owa.l7rc =
    usmDbQosAclRuleSrcIpv6AddrAdd (L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                   keyaclIpv6RuleIndexValue,
                                   &ipv6Addr);
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
* @function fpObjGet_fastPathQOSACLQOSACL_aclRuleSrcIpv6PrefixLength
*
* @purpose Get 'aclRuleSrcIpv6PrefixLength'
*
* @description [aclRuleSrcIpv6PrefixLength]: The Prefix Length. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclRuleSrcIpv6PrefixLength (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6IndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6RuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclRuleSrcIpv6PrefixLengthValue;
  L7_in6_prefix_t ipv6Addr;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIpv6Index */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                           (xLibU8_t *) & keyaclIpv6IndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, kwa1.len);

  /* retrieve key: aclIpv6RuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleIndex,
                           (xLibU8_t *) & keyaclIpv6RuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6RuleIndexValue, kwa2.len);

  /* get the value from application */
  if(usmDbQosAclIsFieldConfigured(L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                   keyaclIpv6RuleIndexValue, ACL_SRCIPV6) == L7_TRUE)
  {
    owa.l7rc =
       usmDbQosAclRuleSrcIpv6AddrGet (L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                   keyaclIpv6RuleIndexValue,
                                   &ipv6Addr);
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

  objaclRuleSrcIpv6PrefixLengthValue=(xLibU32_t)ipv6Addr.in6PrefixLen;
  /* return the object value: aclRuleSrcIpv6PrefixLength */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objaclRuleSrcIpv6PrefixLengthValue,
                    sizeof (objaclRuleSrcIpv6PrefixLengthValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclRuleSrcIpv6PrefixLength
*
* @purpose Set 'aclRuleSrcIpv6PrefixLength'
*
* @description [aclRuleSrcIpv6PrefixLength]: The Prefix Length. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclRuleSrcIpv6PrefixLength (void *wap,
                                                                   void *bufp)
{
  /* Setting this as part of IPV6 Address Set. Hence returning success here. */
  return XLIBRC_SUCCESS;
}


/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclRuleDstIpv6Prefix
*
* @purpose Get 'aclRuleDstIpv6Prefix'
*
* @description [aclRuleDstIpv6Prefix]: The Ipv6 Prefix Address configured
*              on the Service Port. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclRuleDstIpv6Prefix (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6IndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6RuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV6_t));
  xLibIpV6_t objaclRuleDstIpv6PrefixValue;
  L7_in6_prefix_t ipv6Addr;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIpv6Index */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                           (xLibU8_t *) & keyaclIpv6IndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, kwa1.len);

  /* retrieve key: aclIpv6RuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleIndex,
                           (xLibU8_t *) & keyaclIpv6RuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6RuleIndexValue, kwa2.len);

  /* get the value from application */
  if (usmDbQosAclIsFieldConfigured(L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                   keyaclIpv6RuleIndexValue,ACL_DSTIPV6) == L7_TRUE)
  {
    owa.l7rc =
      usmDbQosAclRuleDstIpv6AddrGet (L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                   keyaclIpv6RuleIndexValue,
                                   &ipv6Addr);
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

  memcpy(&objaclRuleDstIpv6PrefixValue, &ipv6Addr.in6Addr, sizeof(objaclRuleDstIpv6PrefixValue));
  /* return the object value: aclRuleDstIpv6Prefix */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclRuleDstIpv6PrefixValue,
                           sizeof (objaclRuleDstIpv6PrefixValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclRuleDstIpv6Prefix
*
* @purpose Set 'aclRuleDstIpv6Prefix'
*
* @description [aclRuleDstIpv6Prefix]: The Ipv6 Prefix Address configured
*              on the Service Port. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclRuleDstIpv6Prefix (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV6_t));
  xLibIpV6_t objaclRuleDstIpv6PrefixValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6IndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6RuleIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclIpv6PrefixLengthValue;
  L7_in6_prefix_t ipv6Addr;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclRuleDstIpv6Prefix */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclRuleDstIpv6PrefixValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclRuleDstIpv6PrefixValue, owa.len);

  /* retrieve key: aclIpv6Index */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                           (xLibU8_t *) & keyaclIpv6IndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, kwa1.len);

  /* retrieve key: aclIpv6RuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleIndex,
                           (xLibU8_t *) & keyaclIpv6RuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6RuleIndexValue, kwa2.len);

  /* retrieve key: aclIpv6RuleIpv6PrefixLength */
  kwa3.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleDstIpv6PrefixLength,
                           (xLibU8_t *) &objaclIpv6PrefixLengthValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    objaclIpv6PrefixLengthValue = 0;
  }
  
  memcpy(&ipv6Addr.in6Addr, &objaclRuleDstIpv6PrefixValue, sizeof(ipv6Addr.in6Addr));
  ipv6Addr.in6PrefixLen = (L7_int32) objaclIpv6PrefixLengthValue;
 
  /* set the value in application */
  owa.l7rc =
    usmDbQosAclRuleDstIpv6AddrAdd (L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                   keyaclIpv6RuleIndexValue,
                                   &ipv6Addr);
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
* @function fpObjGet_fastPathQOSACLQOSACL_aclRuleDstIpv6PrefixLength
*
* @purpose Get 'aclRuleDstIpv6PrefixLength'
*
* @description [aclRuleDstIpv6PrefixLength]: The Prefix Length. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclRuleDstIpv6PrefixLength (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6IndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6RuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclRuleDstIpv6PrefixLengthValue;
  L7_in6_prefix_t ipv6Addr;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIpv6Index */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                           (xLibU8_t *) & keyaclIpv6IndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, kwa1.len);

  /* retrieve key: aclIpv6RuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleIndex,
                           (xLibU8_t *) & keyaclIpv6RuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6RuleIndexValue, kwa2.len);

  /* get the value from application */
  if (usmDbQosAclIsFieldConfigured(L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                   keyaclIpv6RuleIndexValue,ACL_DSTIPV6) == L7_TRUE)
  {
    owa.l7rc =
       usmDbQosAclRuleDstIpv6AddrGet (L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                   keyaclIpv6RuleIndexValue,
                                   &ipv6Addr);
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

  objaclRuleDstIpv6PrefixLengthValue = (xLibU32_t) ipv6Addr.in6PrefixLen;
  /* return the object value: aclRuleDstIpv6PrefixLength */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objaclRuleDstIpv6PrefixLengthValue,
                    sizeof (objaclRuleDstIpv6PrefixLengthValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclRuleDstIpv6PrefixLength
*
* @purpose Set 'aclRuleDstIpv6PrefixLength'
*
* @description [aclRuleDstIpv6PrefixLength]: The Prefix Length. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclRuleDstIpv6PrefixLength (void *wap,
                                                                   void *bufp)
{
  /* Set is taken care when we set the IPV6 address itself. Hence returning Success here */
  return XLIBRC_SUCCESS;
}


/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclStatus
*
* @purpose Get 'aclStatus'
*
* @description [aclStatus]: Status of this instance. Entries can not be deleted
*              until all rows in the aclIfTable and aclRuleTable with
*              corresponding values of aclIndex have been deleted. active(1)
*              - this ACL instance is active createAndGo(4) - set to
*              this value to create an in 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                          (xLibU8_t *) & keyaclIndexValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, kwa.len);
  /* get the value from application */
  objaclStatusValue = L7_ROW_STATUS_ACTIVE;
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: aclStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclStatusValue,
                           sizeof (objaclStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclStatus
*
* @purpose Set 'aclStatus'
*
* @description [aclStatus]: Status of this instance. Entries can not be deleted
*              until all rows in the aclIfTable and aclRuleTable with
*              corresponding values of aclIndex have been deleted. active(1)
*              - this ACL instance is active createAndGo(4) - set to
*              this value to create an in 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclStatusValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;
  xLibStr256_t aclIdOrName;
  xLibU32_t aclId;
  xLibU16_t aclIdOrNameSize = sizeof(aclIdOrName);
  L7_BOOL flag;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objaclStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclStatusValue, owa.len);

  /* retrieve key: aclIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                          (xLibU8_t *) & keyaclIndexValue, &kwa.len);
  if ((kwa.rc != XLIBRC_SUCCESS)  && (objaclStatusValue != L7_ROW_STATUS_CREATE_AND_GO))
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  xLibFilterGet (wap,XOBJ_fastPathQOSACLQOSACL_aclIPNamedIndex,(xLibU8_t *)aclIdOrName,&aclIdOrNameSize);
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, kwa.len);
    /* call the usmdb only for add and delete */
  if (objaclStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    /* Create a row */
     
    flag = fpObj_isNameACL(aclIdOrName);
    if(flag == L7_FALSE)
    {
       aclId = atoi(aclIdOrName);
      if(aclId < L7_ACL_MIN_STD1_ID || aclId > L7_ACL_MAX_EXT1_ID)
      {
         owa.rc = XLIBRC_IP_ACL_ID_NOT_VALID;  /* TODO: Change if required */
         FPOBJ_TRACE_EXIT (bufp, owa);
         return owa.rc;
      }
      owa.l7rc = usmDbQosAclNumCheckValid(L7_UNIT_CURRENT, aclId);
      if(owa.l7rc != L7_SUCCESS)
      {       
        owa.l7rc = usmDbQosAclCreate(L7_UNIT_CURRENT, aclId);
        if (owa.l7rc != L7_SUCCESS)
        {
          if(owa.l7rc == L7_TABLE_IS_FULL)
          {
            owa.rc = XLIBRC_ACL_TABLE_FULL_ERROR; 
          }
          else if(owa.l7rc == L7_REQUEST_DENIED)
          {
            owa.rc = XLIBRC_ACL_CHANGE_REQ_DENY_APP; 
          }
          else
          {
            owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
          }
          FPOBJ_TRACE_EXIT (bufp, owa);
          return owa.rc;
        }
      }
      /* already exists */
      else
      {
        owa.rc = XLIBRC_ACL_ID_ALREADY_EXISTS;  /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
   }
   else
   {
     /*Check if acl name already exists*/
      if(usmDbQosAclNameToIndex(L7_UNIT_CURRENT, aclIdOrName, &aclId) == L7_SUCCESS)
      {
        /* InValid Name already exists */
        owa.rc = XLIBRC_ACL_NAME_ALREADY_EXISTS;  /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
      if((owa.l7rc = usmDbQosAclNamedIndexNextFree(L7_UNIT_CURRENT, L7_ACL_TYPE_IP, &aclId)) != L7_SUCCESS)
      {
          if(owa.l7rc == L7_TABLE_IS_FULL)
          {
              owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
              FPOBJ_TRACE_EXIT (bufp, owa);
              return owa.rc;
          }
          else if(owa.l7rc == L7_NOT_SUPPORTED)
          {
              owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
              FPOBJ_TRACE_EXIT (bufp, owa);
              return owa.rc;
          }          
      }
      else
      {
          if((owa.l7rc = usmDbQosAclCreate(L7_UNIT_CURRENT, aclId)) != L7_SUCCESS)
          {
              if(owa.l7rc == L7_TABLE_IS_FULL)
              {
                owa.rc = XLIBRC_ACL_TABLE_FULL_ERROR;
              }
              else if(owa.l7rc == L7_REQUEST_DENIED)
              {
                owa.rc = XLIBRC_ACL_CHANGE_REQ_DENY_APP; 
              }
              else
              {
                owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
              }
              FPOBJ_TRACE_EXIT (bufp, owa);
              return owa.rc;
          }
          /* acl Name */
          if((owa.l7rc = usmDbQosAclNameAdd(L7_UNIT_CURRENT, aclId, aclIdOrName)) != L7_SUCCESS)
          {
               owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
              FPOBJ_TRACE_EXIT (bufp, owa);
              return owa.rc; 
          }
      }
    }
    /* Creation of index successful Push the index into the filter*/
    xLibFilterSet(wap,XOBJ_fastPathQOSACLQOSACL_aclIndex, 0, (xLibU8_t *)&aclId,sizeof(aclId)); 
  }
  else if (objaclStatusValue == L7_ROW_STATUS_DESTROY)
  {
    /* Delete the existing row */    
    owa.l7rc = usmDbQosAclDelete(L7_UNIT_CURRENT, keyaclIndexValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      if (owa.l7rc == L7_REQUEST_DENIED)
      {
        owa.rc = XLIBRC_ACL_CHANGE_REQ_DENY_APP_USING;
      }
      else
      {
        owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      }
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  return XLIBRC_SUCCESS;
}
/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclMacRuleAction
*
* @purpose Get 'aclMacRuleAction'
*
* @description [aclMacRuleAction]: The type of action this MAC ACL rule should
*              perform. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclMacRuleAction (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclMacRuleActionValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclMacIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                           (xLibU8_t *) & keyaclMacIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwa1.len);

  /* retrieve key: aclMacRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleIndex,
                           (xLibU8_t *) & keyaclMacRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacRuleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbQosAclMacRuleActionGet (L7_UNIT_CURRENT, keyaclMacIndexValue,
                                          keyaclMacRuleIndexValue,
                                          &objaclMacRuleActionValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: aclMacRuleAction */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclMacRuleActionValue,
                           sizeof (objaclMacRuleActionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclMacRuleAction
*
* @purpose Set 'aclMacRuleAction'
 *@description  [aclMacRuleAction] The type of action this MAC ACL rule should
* perform.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclMacRuleAction (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objaclMacRuleActionValue;

  xLibU32_t keyaclMacIndexValue;
  xLibU32_t keyaclMacRuleIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclMacRuleAction */
  owa.len = sizeof (objaclMacRuleActionValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objaclMacRuleActionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclMacRuleActionValue, owa.len);

  /* retrieve key: aclMacIndex */
  owa.len = sizeof (keyaclMacIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                          (xLibU8_t *) & keyaclMacIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, owa.len);

  /* retrieve key: aclMacRuleIndex */
  owa.len = sizeof (keyaclMacRuleIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleIndex,
                          (xLibU8_t *) & keyaclMacRuleIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacRuleIndexValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbQosAclMacRuleActionAdd(L7_UNIT_CURRENT, keyaclMacIndexValue,
                                keyaclMacRuleIndexValue, objaclMacRuleActionValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    if (owa.l7rc == L7_REQUEST_DENIED)
    {
      owa.rc = XLIBRC_ACL_CHANGE_REQ_DENY_APP_RULE_LIMIT;
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
* @function fpObjGet_fastPathQOSACLQOSACL_aclMacRuleCos
*
* @purpose Get 'aclMacRuleCos'
*
* @description [aclMacRuleCos]: The Class of Service (COS) used in the MAC
*              ACL Classification. This is the three-bit user priority field
*              in the 802.1Q tag header of a tagged Ethernet frame. For
*              frames containing a double VLAN tag, this field is located
*              in the first/outer tag. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclMacRuleCos (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclMacRuleCosValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclMacIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                           (xLibU8_t *) & keyaclMacIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwa1.len);

  /* retrieve key: aclMacRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleIndex,
                           (xLibU8_t *) & keyaclMacRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacRuleIndexValue, kwa2.len);

  /* get the value from application */
 if(usmDbQosAclMacIsFieldConfigured(L7_UNIT_CURRENT, keyaclMacIndexValue,keyaclMacRuleIndexValue,ACL_MAC_COS) == L7_TRUE)
 {
         
    owa.l7rc = usmDbQosAclMacRuleCosGet (L7_UNIT_CURRENT, keyaclMacIndexValue,
                                       keyaclMacRuleIndexValue,
                                       &objaclMacRuleCosValue);

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

  /* return the object value: aclMacRuleCos */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclMacRuleCosValue,
                           sizeof (objaclMacRuleCosValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclMacRuleCos
*
* @purpose Set 'aclMacRuleCos'
*
* @description [aclMacRuleCos]: The Class of Service (COS) used in the MAC
*              ACL Classification. This is the three-bit user priority field
*              in the 802.1Q tag header of a tagged Ethernet frame. For
*              frames containing a double VLAN tag, this field is located
*              in the first/outer tag. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclMacRuleCos (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclMacRuleCosValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacRuleIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclMacRuleCos */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclMacRuleCosValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclMacRuleCosValue, owa.len);

  /* retrieve key: aclMacIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                           (xLibU8_t *) & keyaclMacIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwa1.len);

  /* retrieve key: aclMacRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleIndex,
                           (xLibU8_t *) & keyaclMacRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacRuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbQosAclMacRuleCosAdd (L7_UNIT_CURRENT, keyaclMacIndexValue,
                                       keyaclMacRuleIndexValue,
                                       objaclMacRuleCosValue);
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
* @function fpObjGet_fastPathQOSACLQOSACL_aclMacRuleCos2
*
* @purpose Get 'aclMacRuleCos2'
*
* @description [aclMacRuleCos2]: The Secondary Class of Service (COS2) used
*              in the MAC ACL Classification. This is the three-bit user
*              priority field in the second/inner tag header of a double
*              VLAN tagged Ethernet frame. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclMacRuleCos2 (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclMacRuleCos2Value;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclMacIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                           (xLibU8_t *) & keyaclMacIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwa1.len);

  /* retrieve key: aclMacRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleIndex,
                           (xLibU8_t *) & keyaclMacRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacRuleIndexValue, kwa2.len);

  /* get the value from application */
  if(usmDbQosAclMacIsFieldConfigured(L7_UNIT_CURRENT, keyaclMacIndexValue,keyaclMacRuleIndexValue,ACL_MAC_COS2) == L7_TRUE)
  {

     owa.l7rc = usmDbQosAclMacRuleCos2Get (L7_UNIT_CURRENT, keyaclMacIndexValue,
                                        keyaclMacRuleIndexValue,
                                        &objaclMacRuleCos2Value);
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

  /* return the object value: aclMacRuleCos2 */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclMacRuleCos2Value,
                           sizeof (objaclMacRuleCos2Value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclMacRuleCos2
*
* @purpose Set 'aclMacRuleCos2'
*
* @description [aclMacRuleCos2]: The Secondary Class of Service (COS2) used
*              in the MAC ACL Classification. This is the three-bit user
*              priority field in the second/inner tag header of a double
*              VLAN tagged Ethernet frame. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclMacRuleCos2 (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclMacRuleCos2Value;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacRuleIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclMacRuleCos2 */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclMacRuleCos2Value, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclMacRuleCos2Value, owa.len);

  /* retrieve key: aclMacIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                           (xLibU8_t *) & keyaclMacIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwa1.len);

  /* retrieve key: aclMacRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleIndex,
                           (xLibU8_t *) & keyaclMacRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacRuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbQosAclMacRuleCos2Add (L7_UNIT_CURRENT, keyaclMacIndexValue,
                                        keyaclMacRuleIndexValue,
                                        objaclMacRuleCos2Value);
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
* @function fpObjGet_fastPathQOSACLQOSACL_aclMacRuleDestMacAddr
*
* @purpose Get 'aclMacRuleDestMacAddr'
*
* @description [aclMacRuleDestMacAddr]: The Destination MAC address used in
*              the MAC ACL Classification. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclMacRuleDestMacAddr (void *wap,
                                                              void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t objaclMacRuleDestMacAddrValue;
  L7_uchar8 strDstMacMask [L7_MAC_ADDR_LEN];


  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclMacIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                           (xLibU8_t *) & keyaclMacIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwa1.len);

  /* retrieve key: aclMacRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleIndex,
                           (xLibU8_t *) & keyaclMacRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacRuleIndexValue, kwa2.len);

  /* get the value from application */
  if(usmDbQosAclMacIsFieldConfigured(L7_UNIT_CURRENT, keyaclMacIndexValue,
                                         keyaclMacRuleIndexValue,ACL_MAC_DSTMAC) == L7_TRUE)
  {

     owa.l7rc =
      usmDbQosAclMacRuleDstMacAddrMaskGet (L7_UNIT_CURRENT, keyaclMacIndexValue,
                                         keyaclMacRuleIndexValue,
                                         (L7_uchar8 *)objaclMacRuleDestMacAddrValue,strDstMacMask);

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

  /* return the object value: aclMacRuleDestMacAddr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objaclMacRuleDestMacAddrValue,
                           sizeof (objaclMacRuleDestMacAddrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclMacRuleDestMacAddr
*
* @purpose Set 'aclMacRuleDestMacAddr'
*
* @description [aclMacRuleDestMacAddr]: The Destination MAC address used in
*              the MAC ACL Classification. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclMacRuleDestMacAddr (void *wap,
                                                              void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t objaclMacRuleDestMacAddrValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacRuleIndexValue;
  xLibStr6_t strDstMacMask;
  xLibU16_t strDstMacMaskSize = sizeof(xLibStr6_t);
  xLibU32_t index;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclMacRuleDestMacAddr */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objaclMacRuleDestMacAddrValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objaclMacRuleDestMacAddrValue, owa.len);

  /* retrieve key: aclMacIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                           (xLibU8_t *) & keyaclMacIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwa1.len);

  /* retrieve key: aclMacRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleIndex,
                           (xLibU8_t *) & keyaclMacRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  owa.rc =xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleDestMacMask,(xLibU8_t *) strDstMacMask,&strDstMacMaskSize);
   if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return owa.rc;
  } 
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacRuleIndexValue, kwa2.len);

  for (index=0; index<L7_MAC_ADDR_LEN; index++)
  {
    strDstMacMask[index] = strDstMacMask[index] ^ (0xFF);
  }

  /* set the value in application */

  owa.l7rc = usmDbQosAclMacRuleDstMacAdd (L7_UNIT_CURRENT, keyaclMacIndexValue,
                                          keyaclMacRuleIndexValue,
                                           objaclMacRuleDestMacAddrValue,strDstMacMask);
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
* @function fpObjGet_fastPathQOSACLQOSACL_aclMacRuleDestMacMask
*
* @purpose Get 'aclMacRuleDestMacMask'
*
* @description [aclMacRuleDestMacMask]: The Destination MAC address mask used
*              in the MAC ACL Classification.This mask value identifies
*              the portion of the aclMacRuleDestMacAddr that is compared
*              against a packet. A non-contiguous mask value is permitted.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclMacRuleDestMacMask (void *wap,
                                                              void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t objaclMacRuleDestMacMaskValue;
  L7_uchar8 strDstMac [L7_MAC_ADDR_LEN];
  xLibU32_t index;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclMacIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                           (xLibU8_t *) & keyaclMacIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwa1.len);

  /* retrieve key: aclMacRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleIndex,
                           (xLibU8_t *) & keyaclMacRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacRuleIndexValue, kwa2.len);

  /* get the value from application */
  if(usmDbQosAclMacIsFieldConfigured(L7_UNIT_CURRENT, keyaclMacIndexValue,
                                         keyaclMacRuleIndexValue,ACL_MAC_DSTMAC) == L7_TRUE)
  {


    owa.l7rc =
       usmDbQosAclMacRuleDstMacAddrMaskGet (L7_UNIT_CURRENT, keyaclMacIndexValue,
                                         keyaclMacRuleIndexValue,
                                         strDstMac,
                                          (L7_uchar8 *)objaclMacRuleDestMacMaskValue);
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

  for (index=0; index<L7_MAC_ADDR_LEN; index++)
  {
    objaclMacRuleDestMacMaskValue[index] = objaclMacRuleDestMacMaskValue[index] ^ (0xFF);
  }

  /* return the object value: aclMacRuleDestMacMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objaclMacRuleDestMacMaskValue,
                           sizeof (objaclMacRuleDestMacMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclMacRuleDestMacMask
*
* @purpose Set 'aclMacRuleDestMacMask'
*
* @description [aclMacRuleDestMacMask]: The Destination MAC address mask used
*              in the MAC ACL Classification.This mask value identifies
*              the portion of the aclMacRuleDestMacAddr that is compared
*              against a packet. A non-contiguous mask value is permitted.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclMacRuleDestMacMask (void *wap,
                                                              void *bufp)
{
  return XLIBRC_SUCCESS;
}


/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclMacRuleEtypeKey
*
* @purpose Get 'aclMacRuleEtypeKey'
*
* @description [aclMacRuleEtypeKey]: The Ethertype keyword used in the MAC
*              ACL Classification. A keyword of custom(1) requires that the
*              aclMacRuleEtypeValue object also be set. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclMacRuleEtypeKey (void *wap,
                                                           void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclMacRuleEtypeKeyValue;
  L7_uint32 eTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclMacIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                           (xLibU8_t *) & keyaclMacIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwa1.len);

  /* retrieve key: aclMacRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleIndex,
                           (xLibU8_t *) & keyaclMacRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacRuleIndexValue, kwa2.len);

  /* get the value from application */
  if(usmDbQosAclMacIsFieldConfigured(L7_UNIT_CURRENT, keyaclMacIndexValue,
                                   keyaclMacRuleIndexValue,
                                   ACL_MAC_ETYPE_KEYID) == L7_TRUE)
  {

     owa.l7rc =
      usmDbQosAclMacRuleEtypeKeyGet (L7_UNIT_CURRENT, keyaclMacIndexValue,
                                    keyaclMacRuleIndexValue,
                                   &objaclMacRuleEtypeKeyValue,
                                   &eTypeValue);
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

  /* return the object value: aclMacRuleEtypeKey */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclMacRuleEtypeKeyValue,
                           sizeof (objaclMacRuleEtypeKeyValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclMacRuleEtypeKey
*
* @purpose Set 'aclMacRuleEtypeKey'
*
* @description [aclMacRuleEtypeKey]: The Ethertype keyword used in the MAC
*              ACL Classification. A keyword of custom(1) requires that the
*              aclMacRuleEtypeValue object also be set. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclMacRuleEtypeKey (void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclMacRuleEtypeKeyValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacRuleIndexValue;
  L7_uint32 eTypeValue =0;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclMacRuleEtypeKey */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclMacRuleEtypeKeyValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclMacRuleEtypeKeyValue, owa.len);

  /* retrieve key: aclMacIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                           (xLibU8_t *) & keyaclMacIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwa1.len);

  /* retrieve key: aclMacRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleIndex,
                           (xLibU8_t *) & keyaclMacRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacRuleIndexValue, kwa2.len);

  /* set the value in application */
  
  if(objaclMacRuleEtypeKeyValue !=L7_QOS_ETYPE_KEYID_CUSTOM)
  {
  owa.l7rc =
    usmDbQosAclMacRuleEtypeKeyAdd (L7_UNIT_CURRENT, keyaclMacIndexValue,
                                   keyaclMacRuleIndexValue,
                                   objaclMacRuleEtypeKeyValue,
                                   eTypeValue);
  }
  else
  {
     owa.l7rc  = L7_SUCCESS;
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
* @function fpObjGet_fastPathQOSACLQOSACL_aclMacRuleEtypeValue
*
* @purpose Get 'aclMacRuleEtypeValue'
*
* @description [aclMacRuleEtypeValue]: The Ethertype custom value used in
*              the MAC ACL Classification. This object is only valid if the
*              aclMacRuleEtypeKey is set to custom(1). The allowed value
*              for this object is 0x0600 to 0xFFFF (1536 to 65535). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclMacRuleEtypeValue (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  L7_uint32 etypeKey;
  xLibU32_t objaclMacRuleEtypeValueValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclMacIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                           (xLibU8_t *) & keyaclMacIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwa1.len);

  /* retrieve key: aclMacRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleIndex,
                           (xLibU8_t *) & keyaclMacRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacRuleIndexValue, kwa2.len);

  /* get the value from application */
  if(usmDbQosAclMacIsFieldConfigured(L7_UNIT_CURRENT, keyaclMacIndexValue,
                                   keyaclMacRuleIndexValue,ACL_MAC_ETYPE_KEYID) == L7_TRUE)
  {

    owa.l7rc =
     usmDbQosAclMacRuleEtypeKeyGet (L7_UNIT_CURRENT, keyaclMacIndexValue,



                                   keyaclMacRuleIndexValue,
                                   &etypeKey,
                                   &objaclMacRuleEtypeValueValue);
    if(etypeKey != L7_QOS_ETYPE_KEYID_CUSTOM)
    {
       owa.l7rc = L7_FAILURE;
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

  /* return the object value: aclMacRuleEtypeValue */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclMacRuleEtypeValueValue,
                           sizeof (objaclMacRuleEtypeValueValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclMacRuleEtypeValue
*
* @purpose Set 'aclMacRuleEtypeValue'
*
* @description [aclMacRuleEtypeValue]: The Ethertype custom value used in
*              the MAC ACL Classification. This object is only valid if the
*              aclMacRuleEtypeKey is set to custom(1). The allowed value
*              for this object is 0x0600 to 0xFFFF (1536 to 65535). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclMacRuleEtypeValue (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclMacRuleEtypeValueValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacRuleIndexValue;
  L7_uint32 etypeKeyValue;
  xLibU16_t etypeKeyValueSize;
  FPOBJ_TRACE_ENTER (bufp);

  etypeKeyValueSize = sizeof (xLibU32_t);
  /* retrieve object: aclMacRuleEtypeValue */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclMacRuleEtypeValueValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclMacRuleEtypeValueValue, owa.len);

  /* retrieve key: aclMacIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                           (xLibU8_t *) & keyaclMacIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwa1.len);

  /* retrieve key: aclMacRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleIndex,
                           (xLibU8_t *) & keyaclMacRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }

  owa.rc = xLibFilterGet (wap,XOBJ_fastPathQOSACLQOSACL_aclMacRuleEtypeKey,
                           (xLibU8_t *) & etypeKeyValue, &etypeKeyValueSize);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;  
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacRuleIndexValue, kwa2.len);

  /* set the value in application */
  if(etypeKeyValue == L7_QOS_ETYPE_KEYID_CUSTOM)
  {
  owa.l7rc =
    usmDbQosAclMacRuleEtypeKeyAdd (L7_UNIT_CURRENT, keyaclMacIndexValue,
                                   keyaclMacRuleIndexValue,
                                   etypeKeyValue,
                                   objaclMacRuleEtypeValueValue);
  }
  else
  {
     owa.l7rc = L7_SUCCESS;
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
* @function fpObjGet_fastPathQOSACLQOSACL_aclMacRuleSrcMacAddr
*
* @purpose Get 'aclMacRuleSrcMacAddr'
*
* @description [aclMacRuleSrcMacAddr]: The Source MAC address used in the
*              MAC ACL Classification. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclMacRuleSrcMacAddr (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t objaclMacRuleSrcMacAddrValue;
  xLibStr6_t strSrcMacMask [L7_MAC_ADDR_LEN];
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclMacIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                           (xLibU8_t *) & keyaclMacIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwa1.len);

  /* retrieve key: aclMacRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleIndex,
                           (xLibU8_t *) & keyaclMacRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacRuleIndexValue, kwa2.len);

  /* get the value from application */
  if(usmDbQosAclMacIsFieldConfigured(L7_UNIT_CURRENT, keyaclMacIndexValue,
                                         keyaclMacRuleIndexValue,ACL_MAC_SRCMAC) == L7_TRUE)
  {
         
    owa.l7rc =
    usmDbQosAclMacRuleSrcMacAddrMaskGet (L7_UNIT_CURRENT, keyaclMacIndexValue,
                                         keyaclMacRuleIndexValue,
                                          (L7_uchar8 *)objaclMacRuleSrcMacAddrValue,
                                         (L7_uchar8 *)strSrcMacMask);
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

  /* return the object value: aclMacRuleSrcMacAddr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objaclMacRuleSrcMacAddrValue,
                           sizeof (objaclMacRuleSrcMacAddrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclMacRuleSrcMacAddr
*
* @purpose Set 'aclMacRuleSrcMacAddr'
*
* @description [aclMacRuleSrcMacAddr]: The Source MAC address used in the
*              MAC ACL Classification. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclMacRuleSrcMacAddr (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t objaclMacRuleSrcMacAddrValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacRuleIndexValue;
  xLibStr6_t strSrcMacMask;
  xLibU16_t strSrcMacMaskSize = sizeof (xLibStr6_t);
  xLibU32_t index;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclMacRuleSrcMacAddr */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objaclMacRuleSrcMacAddrValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objaclMacRuleSrcMacAddrValue, owa.len);

  /* retrieve key: aclMacIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                           (xLibU8_t *) & keyaclMacIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwa1.len);

  /* retrieve key: aclMacRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleIndex,
                           (xLibU8_t *) & keyaclMacRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacRuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.rc =xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleSrcMacMask, (xLibU8_t *) strSrcMacMask, &strSrcMacMaskSize);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  for (index=0; index<L7_MAC_ADDR_LEN; index++)
  {
    strSrcMacMask[index] = strSrcMacMask[index] ^ (0xFF);
  }

  owa.l7rc = usmDbQosAclMacRuleSrcMacAdd (L7_UNIT_CURRENT, keyaclMacIndexValue,
                                          keyaclMacRuleIndexValue,
                                           (L7_uchar8 *)objaclMacRuleSrcMacAddrValue,
                                          (L7_uchar8 *)strSrcMacMask);
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
* @function fpObjGet_fastPathQOSACLQOSACL_aclMacRuleSrcMacMask
*
* @purpose Get 'aclMacRuleSrcMacMask'
*
* @description [aclMacRuleSrcMacMask]: The Source MAC address mask used in
*              the MAC ACL Classification. This mask value identifies the
*              portion of the aclMacRuleSrcMacAddr that is compared against
*              a packet. A non-contiguous mask value is permitted. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclMacRuleSrcMacMask (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t objaclMacRuleSrcMacMaskValue;
  L7_uchar8 strSrcMac [L7_MAC_ADDR_LEN];
  xLibU32_t index;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclMacIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                           (xLibU8_t *) & keyaclMacIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwa1.len);

  /* retrieve key: aclMacRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleIndex,
                           (xLibU8_t *) & keyaclMacRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacRuleIndexValue, kwa2.len);

  /* get the value from application */
  if(usmDbQosAclMacIsFieldConfigured(L7_UNIT_CURRENT, keyaclMacIndexValue,
                                         keyaclMacRuleIndexValue,ACL_MAC_SRCMAC) == L7_TRUE)
  {
     owa.l7rc =
      usmDbQosAclMacRuleSrcMacAddrMaskGet (L7_UNIT_CURRENT, keyaclMacIndexValue,
                                         keyaclMacRuleIndexValue,
                                         strSrcMac,
                                         (L7_uchar8 *)objaclMacRuleSrcMacMaskValue);

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

  for (index=0; index<L7_MAC_ADDR_LEN; index++)
  {
    objaclMacRuleSrcMacMaskValue[index] = objaclMacRuleSrcMacMaskValue[index] ^ (0xFF);
  }

  /* return the object value: aclMacRuleSrcMacMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objaclMacRuleSrcMacMaskValue,
                           sizeof (objaclMacRuleSrcMacMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclMacRuleSrcMacMask
*
* @purpose Set 'aclMacRuleSrcMacMask'
*
* @description [aclMacRuleSrcMacMask]: The Source MAC address mask used in
*              the MAC ACL Classification. This mask value identifies the
*              portion of the aclMacRuleSrcMacAddr that is compared against
*              a packet. A non-contiguous mask value is permitted. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclMacRuleSrcMacMask (void *wap,
                                                             void *bufp)
{
  return XLIBRC_SUCCESS;
}


/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclMacRuleVlanId
*
* @purpose Get 'aclMacRuleVlanId'
*
* @description [aclMacRuleVlanId]: The VLAN ID value used in the MAC ACL Classification.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclMacRuleVlanId (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclMacRuleVlanIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclMacIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                           (xLibU8_t *) & keyaclMacIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwa1.len);

  /* retrieve key: aclMacRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleIndex,
                           (xLibU8_t *) & keyaclMacRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacRuleIndexValue, kwa2.len);

  /* get the value from application */
  if(usmDbQosAclMacIsFieldConfigured(L7_UNIT_CURRENT, keyaclMacIndexValue,
                                          keyaclMacRuleIndexValue,ACL_MAC_VLANID) == L7_TRUE)
  {

      owa.l7rc = usmDbQosAclMacRuleVlanIdGet (L7_UNIT_CURRENT, keyaclMacIndexValue,
                                          keyaclMacRuleIndexValue,
                                          &objaclMacRuleVlanIdValue);
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

  /* return the object value: aclMacRuleVlanId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclMacRuleVlanIdValue,
                           sizeof (objaclMacRuleVlanIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclMacRuleVlanId
*
* @purpose Set 'aclMacRuleVlanId'
*
* @description [aclMacRuleVlanId]: The VLAN ID value used in the MAC ACL Classification.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclMacRuleVlanId (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclMacRuleVlanIdValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacRuleIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclMacRuleVlanId */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclMacRuleVlanIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclMacRuleVlanIdValue, owa.len);

  /* retrieve key: aclMacIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                           (xLibU8_t *) & keyaclMacIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwa1.len);

  /* retrieve key: aclMacRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleIndex,
                           (xLibU8_t *) & keyaclMacRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacRuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbQosAclMacRuleVlanIdAdd (L7_UNIT_CURRENT, keyaclMacIndexValue,
                                          keyaclMacRuleIndexValue,
                                          objaclMacRuleVlanIdValue);
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
* @function fpObjGet_fastPathQOSACLQOSACL_aclMacRuleVlanIdRangeStart
*
* @purpose Get 'aclMacRuleVlanIdRangeStart'
*
* @description [aclMacRuleVlanIdRangeStart]: The VLAN ID range start value
*              used in the MAC ACL Classification.Setting this value greater
*              than the current aclMacRuleVlanIdRangeEnd changes the VLAN
*              ID range end to the same value as the range start. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclMacRuleVlanIdRangeStart (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclMacRuleVlanIdRangeStartValue;
  L7_uint32 endVlan;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclMacIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                           (xLibU8_t *) & keyaclMacIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwa1.len);

  /* retrieve key: aclMacRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleIndex,
                           (xLibU8_t *) & keyaclMacRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacRuleIndexValue, kwa2.len);

  /* get the value from application */
  if(usmDbQosAclMacIsFieldConfigured(L7_UNIT_CURRENT, keyaclMacIndexValue,
                                      keyaclMacRuleIndexValue,ACL_MAC_VLANID_START) == L7_TRUE)
  {

    owa.l7rc =
        usmDbQosAclMacRuleVlanIdRangeGet (L7_UNIT_CURRENT, keyaclMacIndexValue,
                                      keyaclMacRuleIndexValue,
                                      &objaclMacRuleVlanIdRangeStartValue,
                                      &endVlan);
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

  /* return the object value: aclMacRuleVlanIdRangeStart */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objaclMacRuleVlanIdRangeStartValue,
                    sizeof (objaclMacRuleVlanIdRangeStartValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclMacRuleVlanIdRangeStart
*
* @purpose Set 'aclMacRuleVlanIdRangeStart'
*
* @description [aclMacRuleVlanIdRangeStart]: The VLAN ID range start value
*              used in the MAC ACL Classification.Setting this value greater
*              than the current aclMacRuleVlanIdRangeEnd changes the VLAN
*              ID range end to the same value as the range start. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclMacRuleVlanIdRangeStart (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclMacRuleVlanIdRangeStartValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacRuleIndexValue;
/*  L7_uint32 startVlan,endVlan; */
  L7_uint32 endVlan;
  xLibU16_t endVlanSize = sizeof(endVlan);
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclMacRuleVlanIdRangeStart */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclMacRuleVlanIdRangeStartValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclMacRuleVlanIdRangeStartValue, owa.len);

  /* retrieve key: aclMacIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                           (xLibU8_t *) & keyaclMacIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwa1.len);

  /* retrieve key: aclMacRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleIndex,
                           (xLibU8_t *) & keyaclMacRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacRuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.rc  = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleVlanIdRangeEnd,(xLibU8_t *) & endVlan,&endVlanSize);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* range validation */
  if (endVlan < objaclMacRuleVlanIdRangeStartValue)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
 
  owa.l7rc =
    usmDbQosAclMacRuleVlanIdRangeAdd (L7_UNIT_CURRENT, keyaclMacIndexValue,
                                      keyaclMacRuleIndexValue,
                                      objaclMacRuleVlanIdRangeStartValue,endVlan);
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
* @function fpObjGet_fastPathQOSACLQOSACL_aclMacRuleVlanIdRangeEnd
*
* @purpose Get 'aclMacRuleVlanIdRangeEnd'
*
* @description [aclMacRuleVlanIdRangeEnd]: The VLAN ID range end value used
*              in the MAC ACL Classification. Setting this value less than
*              the current aclMacRuleVlanIdRangeStart changes the VLAN
*              ID range start to the same value as the range end. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclMacRuleVlanIdRangeEnd (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  L7_uint32 startValue;
  xLibU32_t objaclMacRuleVlanIdRangeEndValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclMacIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                           (xLibU8_t *) & keyaclMacIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwa1.len);

  /* retrieve key: aclMacRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleIndex,
                           (xLibU8_t *) & keyaclMacRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacRuleIndexValue, kwa2.len);

  /* get the value from application */
  if(usmDbQosAclMacIsFieldConfigured(L7_UNIT_CURRENT, keyaclMacIndexValue,
                                      keyaclMacRuleIndexValue,ACL_MAC_VLANID_START) == L7_TRUE)
  {

    owa.l7rc =
      usmDbQosAclMacRuleVlanIdRangeGet (L7_UNIT_CURRENT, keyaclMacIndexValue,
                                      keyaclMacRuleIndexValue,
                                      &startValue,
                                      &objaclMacRuleVlanIdRangeEndValue);
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

  /* return the object value: aclMacRuleVlanIdRangeEnd */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objaclMacRuleVlanIdRangeEndValue,
                    sizeof (objaclMacRuleVlanIdRangeEndValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclMacRuleVlanIdRangeEnd
*
* @purpose Set 'aclMacRuleVlanIdRangeEnd'
*
* @description [aclMacRuleVlanIdRangeEnd]: The VLAN ID range end value used
*              in the MAC ACL Classification. Setting this value less than
*              the current aclMacRuleVlanIdRangeStart changes the VLAN
*              ID range start to the same value as the range end. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclMacRuleVlanIdRangeEnd (void *wap,
                                                                 void *bufp)
{
  return XLIBRC_SUCCESS;
}


/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclMacRuleVlanId2
*
* @purpose Get 'aclMacRuleVlanId2'
*
* @description [aclMacRuleVlanId2]: The Secondary VLAN ID value used in the
*              MAC ACL Classification. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclMacRuleVlanId2 (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclMacRuleVlanId2Value;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclMacIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                           (xLibU8_t *) & keyaclMacIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwa1.len);

  /* retrieve key: aclMacRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleIndex,
                           (xLibU8_t *) & keyaclMacRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacRuleIndexValue, kwa2.len);

  /* get the value from application */
  if(usmDbQosAclMacIsFieldConfigured(L7_UNIT_CURRENT, keyaclMacIndexValue,
                                           keyaclMacRuleIndexValue,ACL_MAC_VLANID2) == L7_TRUE)
  {

      owa.l7rc = usmDbQosAclMacRuleVlanId2Get (L7_UNIT_CURRENT, keyaclMacIndexValue,
                                           keyaclMacRuleIndexValue,
                                           &objaclMacRuleVlanId2Value);
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

  /* return the object value: aclMacRuleVlanId2 */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclMacRuleVlanId2Value,
                           sizeof (objaclMacRuleVlanId2Value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclMacRuleVlanId2
*
* @purpose Set 'aclMacRuleVlanId2'
*
* @description [aclMacRuleVlanId2]: The Secondary VLAN ID value used in the
*              MAC ACL Classification. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclMacRuleVlanId2 (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclMacRuleVlanId2Value;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacRuleIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclMacRuleVlanId2 */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclMacRuleVlanId2Value, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclMacRuleVlanId2Value, owa.len);

  /* retrieve key: aclMacIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                           (xLibU8_t *) & keyaclMacIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwa1.len);

  /* retrieve key: aclMacRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleIndex,
                           (xLibU8_t *) & keyaclMacRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacRuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbQosAclMacRuleVlanId2Add (L7_UNIT_CURRENT, keyaclMacIndexValue,
                                           keyaclMacRuleIndexValue,
                                           objaclMacRuleVlanId2Value);
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
* @function fpObjGet_fastPathQOSACLQOSACL_aclMacRuleVlanId2RangeStart
*
* @purpose Get 'aclMacRuleVlanId2RangeStart'
*
* @description [aclMacRuleVlanId2RangeStart]: The Secondary VLAN ID range
*              start value used in the MAC ACL Classification. Setting this
*              value greater than the current aclMacRuleVlanId2RangeEnd
*              changes the Secondary VLAN ID range end to the same value as
*              the range start. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclMacRuleVlanId2RangeStart (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclMacRuleVlanId2RangeStartValue;
  L7_uint32 endVlan2;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclMacIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                           (xLibU8_t *) & keyaclMacIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwa1.len);

  /* retrieve key: aclMacRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleIndex,
                           (xLibU8_t *) & keyaclMacRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacRuleIndexValue, kwa2.len);

  /* get the value from application */
  if(usmDbQosAclMacIsFieldConfigured(L7_UNIT_CURRENT, keyaclMacIndexValue,
                                       keyaclMacRuleIndexValue,ACL_MAC_VLANID2_START) == L7_TRUE)
  {

    owa.l7rc =
    usmDbQosAclMacRuleVlanId2RangeGet (L7_UNIT_CURRENT, keyaclMacIndexValue,
                                       keyaclMacRuleIndexValue,
                                       &objaclMacRuleVlanId2RangeStartValue,
                                       &endVlan2);
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

  /* return the object value: aclMacRuleVlanId2RangeStart */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objaclMacRuleVlanId2RangeStartValue,
                    sizeof (objaclMacRuleVlanId2RangeStartValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclMacRuleVlanId2RangeStart
*
* @purpose Set 'aclMacRuleVlanId2RangeStart'
*
* @description [aclMacRuleVlanId2RangeStart]: The Secondary VLAN ID range
*              start value used in the MAC ACL Classification. Setting this
*              value greater than the current aclMacRuleVlanId2RangeEnd
*              changes the Secondary VLAN ID range end to the same value as
*              the range start. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclMacRuleVlanId2RangeStart (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclMacRuleVlanId2RangeStartValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacRuleIndexValue;
  /* L7_uint32 endVlan2,startVlan2;  */
  L7_uint32 endVlan2;
  xLibU16_t endVlan2Size = sizeof(endVlan2);
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclMacRuleVlanId2RangeStart */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclMacRuleVlanId2RangeStartValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclMacRuleVlanId2RangeStartValue, owa.len);

  /* retrieve key: aclMacIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                           (xLibU8_t *) & keyaclMacIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwa1.len);

  /* retrieve key: aclMacRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleIndex,
                           (xLibU8_t *) & keyaclMacRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacRuleIndexValue, kwa2.len);

  /* set thie value in application */
  owa.rc =xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleVlanId2RangeEnd,(xLibU8_t *) &endVlan2,&endVlan2Size);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.l7rc =
    usmDbQosAclMacRuleVlanId2RangeAdd (L7_UNIT_CURRENT, keyaclMacIndexValue,
                                       keyaclMacRuleIndexValue,
                                       objaclMacRuleVlanId2RangeStartValue,
                                       endVlan2);
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
* @function fpObjGet_fastPathQOSACLQOSACL_aclMacRuleVlanId2RangeEnd
*
* @purpose Get 'aclMacRuleVlanId2RangeEnd'
*
* @description [aclMacRuleVlanId2RangeEnd]: The Secondary VLAN ID range end
*              value used in the MAC ACL Classification. Setting this value
*              less than the current aclMacRuleVlanId2RangeStart changes
*              the Secondary VLAN ID range start to the same value as the
*              range end. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclMacRuleVlanId2RangeEnd (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclMacRuleVlanId2RangeEndValue;
  L7_uint32 startVlan;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclMacIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                           (xLibU8_t *) & keyaclMacIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwa1.len);

  /* retrieve key: aclMacRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleIndex,
                           (xLibU8_t *) & keyaclMacRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacRuleIndexValue, kwa2.len);

  /* get the value from application */
  if(usmDbQosAclMacIsFieldConfigured(L7_UNIT_CURRENT, keyaclMacIndexValue,
                                       keyaclMacRuleIndexValue,ACL_MAC_VLANID2_START) == L7_TRUE)
  {

    owa.l7rc =
    usmDbQosAclMacRuleVlanId2RangeGet (L7_UNIT_CURRENT, keyaclMacIndexValue,
                                       keyaclMacRuleIndexValue,
                                       &startVlan, 
                                       &objaclMacRuleVlanId2RangeEndValue);
  }
  else
  {
     owa.l7rc = XLIBRC_FAILURE;
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: aclMacRuleVlanId2RangeEnd */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objaclMacRuleVlanId2RangeEndValue,
                    sizeof (objaclMacRuleVlanId2RangeEndValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclMacRuleVlanId2RangeEnd
*
* @purpose Set 'aclMacRuleVlanId2RangeEnd'
*
* @description [aclMacRuleVlanId2RangeEnd]: The Secondary VLAN ID range end
*              value used in the MAC ACL Classification. Setting this value
*              less than the current aclMacRuleVlanId2RangeStart changes
*              the Secondary VLAN ID range start to the same value as the
*              range end. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclMacRuleVlanId2RangeEnd (void *wap,
                                                                  void *bufp)
{
  return XLIBRC_SUCCESS;
}


/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclMacRuleStatus
*
* @purpose Get 'aclMacRuleStatus'
*
* @description [aclMacRuleStatus]: Status of this instance. It could be wither
*              active, createandgo or destroy 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclMacRuleStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclMacRuleStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclMacIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                           (xLibU8_t *) & keyaclMacIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwa1.len);

  /* retrieve key: aclMacRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleIndex,
                           (xLibU8_t *) & keyaclMacRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacRuleIndexValue, kwa2.len);

  if (keyaclMacRuleIndexValue < L7_ACL_MIN_RULE_NUM || 
      keyaclMacRuleIndexValue > L7_ACL_MAX_RULE_NUM)
  {
    owa.rc = XLIBRC_ACL_INVALID_RULE_ID_RANGE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  objaclMacRuleStatusValue = L7_ROW_STATUS_ACTIVE;
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: aclMacRuleStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclMacRuleStatusValue,
                           sizeof (objaclMacRuleStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclMacRuleStatus
*
* @purpose Set 'aclMacRuleStatus'
*
* @description [aclMacRuleStatus]: Status of this instance. It could be wither
*              active, createandgo or destroy 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclMacRuleStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclMacRuleStatusValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacRuleIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclMacRuleStatus */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclMacRuleStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclMacRuleStatusValue, owa.len);

  /* retrieve key: aclMacIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                           (xLibU8_t *) & keyaclMacIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwa1.len);

  /* retrieve key: aclMacRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleIndex,
                           (xLibU8_t *) & keyaclMacRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }

  if (keyaclMacRuleIndexValue < L7_ACL_MIN_RULE_NUM || 
      keyaclMacRuleIndexValue > L7_ACL_MAX_RULE_NUM)
  {
    owa.rc = XLIBRC_ACL_INVALID_RULE_ID_RANGE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacRuleIndexValue, kwa2.len);

    /* call the usmdb only for add and delete */
  if (objaclMacRuleStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    /* Create a row */
    owa.l7rc = usmDbQosAclMacRuleActionAdd(L7_UNIT_CURRENT, keyaclMacIndexValue,
                                keyaclMacRuleIndexValue,L7_ACL_DENY);
    if (owa.l7rc != L7_SUCCESS)
    {
      if (owa.l7rc == L7_REQUEST_DENIED)
      {
        owa.rc = XLIBRC_ACL_CHANGE_REQ_DENY_APP_RULE_LIMIT;
      }
      else
      {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      }
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else if (objaclMacRuleStatusValue == L7_ROW_STATUS_DESTROY)
  {
    /* Delete the existing row */
    owa.l7rc = usmDbQosAclMacRuleRemove(L7_UNIT_CURRENT, keyaclMacIndexValue,
                                keyaclMacRuleIndexValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      if (owa.l7rc == L7_REQUEST_DENIED)
      {
        owa.rc = XLIBRC_ACL_CHANGE_REQ_DENY_APP_USING;
      }
      else
      {
        owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      }
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  return XLIBRC_SUCCESS;


}


/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclMacRuleAssignQueueId
*
* @purpose Get 'aclMacRuleAssignQueueId'
*
* @description [aclMacRuleAssignQueueId]: Queue identifier to which all inbound
*              packets matching this MAC ACL rule are directed. This
*              object defaults to the standard queue assignment for user
*              priority 0 traffic per the IEEE 802.1D specification based
*              on the number of assignable queues in the sys 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclMacRuleAssignQueueId (void *wap,
                                                                void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclMacRuleAssignQueueIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclMacIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                           (xLibU8_t *) & keyaclMacIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwa1.len);

  /* retrieve key: aclMacRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleIndex,
                           (xLibU8_t *) & keyaclMacRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacRuleIndexValue, kwa2.len);

  /* get the value from application */
  if(usmDbQosAclMacIsFieldConfigured(L7_UNIT_CURRENT, keyaclMacIndexValue,keyaclMacRuleIndexValue,ACL_MAC_ASSIGN_QUEUEID) == L7_TRUE)
  {

    owa.l7rc =
    usmDbQosAclMacRuleAssignQueueIdGet (L7_UNIT_CURRENT, keyaclMacIndexValue,
                                        keyaclMacRuleIndexValue,
                                        &objaclMacRuleAssignQueueIdValue);
  }
  else
  {
    owa.l7rc = XLIBRC_FAILURE;
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: aclMacRuleAssignQueueId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclMacRuleAssignQueueIdValue,
                           sizeof (objaclMacRuleAssignQueueIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclMacRuleAssignQueueId
*
* @purpose Set 'aclMacRuleAssignQueueId'
*
* @description [aclMacRuleAssignQueueId]: Queue identifier to which all inbound
*              packets matching this MAC ACL rule are directed. This
*              object defaults to the standard queue assignment for user
*              priority 0 traffic per the IEEE 802.1D specification based
*              on the number of assignable queues in the sys 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclMacRuleAssignQueueId (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclMacRuleAssignQueueIdValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacRuleIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclMacRuleAssignQueueId */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclMacRuleAssignQueueIdValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclMacRuleAssignQueueIdValue, owa.len);

  /* retrieve key: aclMacIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                           (xLibU8_t *) & keyaclMacIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwa1.len);

  /* retrieve key: aclMacRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleIndex,
                           (xLibU8_t *) & keyaclMacRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacRuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc =
    usmDbQosAclMacRuleAssignQueueIdAdd (L7_UNIT_CURRENT, keyaclMacIndexValue,
                                        keyaclMacRuleIndexValue,
                                        objaclMacRuleAssignQueueIdValue);
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
* @function fpObjGet_fastPathQOSACLQOSACL_aclMacRuleRedirectIntf
*
* @purpose Get 'aclMacRuleRedirectIntf'
*
* @description [aclMacRuleRedirectIntf]: A non-zero value indicates the external
*              ifIndex to which all inbound packets matching this MAC
*              ACL rule are directed. A value of zero means packet redirection
*              is not in effect, which is the default value of this
*              object. Note that packet redirection and 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclMacRuleRedirectIntf (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclMacRuleRedirectIntfValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclMacIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                           (xLibU8_t *) & keyaclMacIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwa1.len);

  /* retrieve key: aclMacRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleIndex,
                           (xLibU8_t *) & keyaclMacRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacRuleIndexValue, kwa2.len);

  /* get the value from application */
  if(usmDbQosAclMacIsFieldConfigured(L7_UNIT_CURRENT, keyaclMacIndexValue,
                                       keyaclMacRuleIndexValue,ACL_MAC_REDIRECT_INTF) == L7_TRUE)
  {
 
     owa.l7rc =
        usmDbQosAclMacRuleRedirectIntfGet (L7_UNIT_CURRENT, keyaclMacIndexValue,
                                       keyaclMacRuleIndexValue,
                                       &objaclMacRuleRedirectIntfValue);
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

  /* return the object value: aclMacRuleRedirectIntf */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclMacRuleRedirectIntfValue,
                           sizeof (objaclMacRuleRedirectIntfValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclMacRuleRedirectIntf
*
* @purpose Set 'aclMacRuleRedirectIntf'
*
* @description [aclMacRuleRedirectIntf]: A non-zero value indicates the external
*              ifIndex to which all inbound packets matching this MAC
*              ACL rule are directed. A value of zero means packet redirection
*              is not in effect, which is the default value of this
*              object. Note that packet redirection and 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclMacRuleRedirectIntf (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclMacRuleRedirectIntfValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacRuleIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclMacRuleRedirectIntf */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclMacRuleRedirectIntfValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclMacRuleRedirectIntfValue, owa.len);

  /* retrieve key: aclMacIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                           (xLibU8_t *) & keyaclMacIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwa1.len);

  /* retrieve key: aclMacRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleIndex,
                           (xLibU8_t *) & keyaclMacRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacRuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc =
    usmDbQosAclMacRuleRedirectIntfAdd (L7_UNIT_CURRENT, keyaclMacIndexValue,
                                       keyaclMacRuleIndexValue,
                                       objaclMacRuleRedirectIntfValue);
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
* @function fpObjList_fastPathQOSACLQOSACL_aclMacRuleRedirectIntf
*
* @purpose List 'aclMacRuleRedirectIntf'
 *@description  [aclMacRuleRedirectIntf] A non-zero value indicates the external
* ifIndex to which all inbound packets matching this MAC ACL rule
* are directed. A value of zero means packet redirection is not in
* effect, which is the default value of this object. Note that
* packet redirection and
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_fastPathQOSACLQOSACL_aclMacRuleRedirectIntf (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objaclMacRuleRedirectIntfValue;
  xLibU32_t nextObjaclMacRuleRedirectIntfValue;
  xLibU32_t mask = USM_PHYSICAL_INTF;

  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof (objaclMacRuleRedirectIntfValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleRedirectIntf,
                          (xLibU8_t *) & objaclMacRuleRedirectIntfValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objaclMacRuleRedirectIntfValue =0;
  }    
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclMacRuleRedirectIntfValue, owa.len);
  if (usmDbFeaturePresentCheck(L7_UNIT_CURRENT,L7_FLEX_QOS_ACL_COMPONENT_ID,L7_ACL_INTF_LAG_SUPPORT_FEATURE_ID) == L7_TRUE)
  {
    mask = USM_PHYSICAL_INTF | USM_LAG_INTF;
  }
  owa.l7rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT,  mask,
                                  0,objaclMacRuleRedirectIntfValue, &nextObjaclMacRuleRedirectIntfValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjaclMacRuleRedirectIntfValue, owa.len);

  /* return the object value: aclMacRuleRedirectIntf */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjaclMacRuleRedirectIntfValue,
                           sizeof (nextObjaclMacRuleRedirectIntfValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclMacRuleMatchEvery
*
* @purpose Get 'aclMacRuleMatchEvery'
*
* @description [aclMacRuleMatchEvery]:  Flag to indicate that the MAC ACL
*              rule is defined to match all packets, regardless of Ethertype.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclMacRuleMatchEvery (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclMacRuleMatchEveryValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclMacIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                           (xLibU8_t *) & keyaclMacIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwa1.len);

  /* retrieve key: aclMacRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleIndex,
                           (xLibU8_t *) & keyaclMacRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacRuleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbQosAclMacRuleEveryGet (L7_UNIT_CURRENT, keyaclMacIndexValue,
                                         keyaclMacRuleIndexValue,
                                         &objaclMacRuleMatchEveryValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: aclMacRuleMatchEvery */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclMacRuleMatchEveryValue,
                           sizeof (objaclMacRuleMatchEveryValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclMacRuleMatchEvery
*
* @purpose Set 'aclMacRuleMatchEvery'
*
* @description [aclMacRuleMatchEvery]:  Flag to indicate that the MAC ACL
*              rule is defined to match all packets, regardless of Ethertype.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclMacRuleMatchEvery (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclMacRuleMatchEveryValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacRuleIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclMacRuleMatchEvery */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclMacRuleMatchEveryValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclMacRuleMatchEveryValue, owa.len);

  /* retrieve key: aclMacIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                           (xLibU8_t *) & keyaclMacIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwa1.len);

  /* retrieve key: aclMacRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleIndex,
                           (xLibU8_t *) & keyaclMacRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacRuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbQosAclMacRuleEveryAdd (L7_UNIT_CURRENT, keyaclMacIndexValue,
                                         keyaclMacRuleIndexValue,
                                         objaclMacRuleMatchEveryValue);
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
* @function fpObjGet_fastPathQOSACLQOSACL_IsMirrorIntfConfigured
*
* @purpose Get 'IsMirrorIntfConfigured'
*
* @description [IsMirrorIntfConfigured]: A non-zero value indicates that the
*              mirror interface is already configured for this particular
*              acl/rule combination and zero indicates no mirror interface is
*              configured.
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_IsMirrorIntfConfigured(void *wap,
                                                              void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t value, intIfNum;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclMacIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                           (xLibU8_t *) & keyaclMacIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwa1.len);

  /* retrieve key: aclMacRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleIndex,
                           (xLibU8_t *) & keyaclMacRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacRuleIndexValue, kwa2.len);

  owa.l7rc = usmDbQosAclMacRuleMirrorIntfGet(L7_UNIT_CURRENT, keyaclMacIndexValue, 
                                             keyaclMacRuleIndexValue, &intIfNum);
  if (owa.l7rc != L7_SUCCESS)
  {
    value = L7_FALSE;
  }
  else
  {
    value = L7_TRUE;
  }

  /* return the object value: aclMacRuleRedirectIntf */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & value, sizeof (value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_IsRedirectIntfConfigured
*
* @purpose Get 'IsRedirectIntfConfigured'
*
* @description [IsRedirectIntfConfigured]: A non-zero value indicates that the 
*              redirect interface is already configured for this particular
*              acl/rule combination and zero indicates no redirect interface is
*              configured. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_IsRedirectIntfConfigured(void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t value,intIfNum;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclMacIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                           (xLibU8_t *) & keyaclMacIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwa1.len);

  /* retrieve key: aclMacRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleIndex,
                           (xLibU8_t *) & keyaclMacRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacRuleIndexValue, kwa2.len);

  owa.l7rc = usmDbQosAclMacRuleRedirectIntfGet(L7_UNIT_CURRENT, keyaclMacIndexValue,
                                             keyaclMacRuleIndexValue, &intIfNum);
  if (owa.l7rc != L7_SUCCESS)
  {
    value = L7_FALSE;
  }
  else
  {
    value = L7_TRUE;
  }

  /* return the object value: aclMacRuleRedirectIntf */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & value, sizeof (value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclMacRuleMirrorIntf
*
* @purpose Get 'aclMacRuleMirrorIntf'
*
* @description [aclMacRuleMirrorIntf]: A non-zero value indicates the external
*              ifIndex to which all inbound packets matching this MAC
*              ACL rule are copied. A value of zero means packet mirroring
*              is not in effect, which is the default value of this object.
*              Note that packet mirroring and redirec 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclMacRuleMirrorIntf (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclMacRuleMirrorIntfValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclMacIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                           (xLibU8_t *) & keyaclMacIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwa1.len);

  /* retrieve key: aclMacRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleIndex,
                           (xLibU8_t *) & keyaclMacRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacRuleIndexValue, kwa2.len);

  /* get the value from application */
  if(usmDbQosAclMacIsFieldConfigured(L7_UNIT_CURRENT, keyaclMacIndexValue,
                                     keyaclMacRuleIndexValue,ACL_MAC_MIRROR_INTF) == L7_TRUE)
  {
 
     owa.l7rc =
       usmDbQosAclMacRuleMirrorIntfGet (L7_UNIT_CURRENT, keyaclMacIndexValue,
                                     keyaclMacRuleIndexValue,
                                     &objaclMacRuleMirrorIntfValue);
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

  /* return the object value: aclMacRuleMirrorIntf */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclMacRuleMirrorIntfValue,
                           sizeof (objaclMacRuleMirrorIntfValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclMacRuleMirrorIntf
*
* @purpose Set 'aclMacRuleMirrorIntf'
*
* @description [aclMacRuleMirrorIntf]: A non-zero value indicates the external
*              ifIndex to which all inbound packets matching this MAC
*              ACL rule are copied. A value of zero means packet mirroring
*              is not in effect, which is the default value of this object.
*              Note that packet mirroring and redirec 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclMacRuleMirrorIntf (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclMacRuleMirrorIntfValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacRuleIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclMacRuleMirrorIntf */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclMacRuleMirrorIntfValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclMacRuleMirrorIntfValue, owa.len);

  /* retrieve key: aclMacIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                           (xLibU8_t *) & keyaclMacIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwa1.len);

  /* retrieve key: aclMacRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleIndex,
                           (xLibU8_t *) & keyaclMacRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacRuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc =
    usmDbQosAclMacRuleMirrorIntfAdd (L7_UNIT_CURRENT, keyaclMacIndexValue,
                                     keyaclMacRuleIndexValue,
                                     objaclMacRuleMirrorIntfValue);
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
* @function fpObjList_fastPathQOSACLQOSACL_aclMacRuleMirrorIntf
*
* @purpose List 'aclMacRuleMirrorIntf'
 *@description  [aclMacRuleMirrorIntf] A non-zero value indicates the external
* ifIndex to which all inbound packets matching this MAC ACL rule
* are copied. A value of zero means packet mirroring is not in
* effect, which is the default value of this object. Note that packet
* mirroring and redirec
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_fastPathQOSACLQOSACL_aclMacRuleMirrorIntf (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objaclMacRuleMirrorIntfValue;
  xLibU32_t nextObjaclMacRuleMirrorIntfValue;
  
  xLibU32_t mask = USM_PHYSICAL_INTF;

  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof (objaclMacRuleMirrorIntfValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleMirrorIntf,
                          (xLibU8_t *) & objaclMacRuleMirrorIntfValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objaclMacRuleMirrorIntfValue =0;
  }
    
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclMacRuleMirrorIntfValue, owa.len);
  owa.l7rc =  usmDbValidIntIfNumNext (objaclMacRuleMirrorIntfValue,
                                     &nextObjaclMacRuleMirrorIntfValue);  
  if (usmDbFeaturePresentCheck(L7_UNIT_CURRENT,L7_FLEX_QOS_ACL_COMPONENT_ID,L7_ACL_INTF_LAG_SUPPORT_FEATURE_ID) == L7_TRUE)
  {
    mask = USM_PHYSICAL_INTF | USM_LAG_INTF; 
  }
  owa.l7rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT,  mask,
                                  0,objaclMacRuleMirrorIntfValue, &nextObjaclMacRuleMirrorIntfValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjaclMacRuleMirrorIntfValue, owa.len);

  /* return the object value: aclMacRuleMirrorIntf */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjaclMacRuleMirrorIntfValue,
                           sizeof (nextObjaclMacRuleMirrorIntfValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclMacRuleLogging
*
* @purpose Get 'aclMacRuleLogging'
*
* @description [aclMacRuleLogging]: Flag to indicate that the ACL rule is
*              being logged. A hardware count of the number of times this
*              rule is hit is reported via the aclTrapRuleLogEvent notification.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclMacRuleLogging (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclMacRuleLoggingValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclMacIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                           (xLibU8_t *) & keyaclMacIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwa1.len);

  /* retrieve key: aclMacRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleIndex,
                           (xLibU8_t *) & keyaclMacRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacRuleIndexValue, kwa2.len);

  /* get the value from application */

  if(usmDbQosAclMacIsFieldConfigured(L7_UNIT_CURRENT, keyaclMacIndexValue,
                                           keyaclMacRuleIndexValue, ACL_MAC_LOGGING) == L7_TRUE)
  {
 
     owa.l7rc = usmDbQosAclMacRuleLoggingGet (L7_UNIT_CURRENT, keyaclMacIndexValue,
                                           keyaclMacRuleIndexValue,
                                           &objaclMacRuleLoggingValue);
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

  /* return the object value: aclMacRuleLogging */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclMacRuleLoggingValue,
                           sizeof (objaclMacRuleLoggingValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclMacRuleLogging
*
* @purpose Set 'aclMacRuleLogging'
*
* @description [aclMacRuleLogging]: Flag to indicate that the ACL rule is
*              being logged. A hardware count of the number of times this
*              rule is hit is reported via the aclTrapRuleLogEvent notification.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclMacRuleLogging (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclMacRuleLoggingValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacRuleIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclMacRuleLogging */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclMacRuleLoggingValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclMacRuleLoggingValue, owa.len);

  /* retrieve key: aclMacIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                           (xLibU8_t *) & keyaclMacIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwa1.len);

  /* retrieve key: aclMacRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleIndex,
                           (xLibU8_t *) & keyaclMacRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacRuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbQosAclMacRuleLoggingAdd (L7_UNIT_CURRENT, keyaclMacIndexValue,
                                           keyaclMacRuleIndexValue,
                                           objaclMacRuleLoggingValue);
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
* @function fpObjGet_fastPathQOSACLQOSACL_aclMacRuleTimeRangeName
*
* @purpose Get 'aclMacRuleTimeRangeName'
*
* @description [aclMacRuleTimeRangeName]:Name of time range associated with the MAC ACL Rule
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclMacRuleTimeRangeName(void *wap,
                                                           void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objaclMacRuleTimeRangeNameValue;
  FPOBJ_TRACE_ENTER (bufp);
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclMacIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                           (xLibU8_t *) & keyaclMacIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwa1.len);

  /* retrieve key: aclMacRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleIndex,
                           (xLibU8_t *) & keyaclMacRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacRuleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbQosAclMacRuleTimeRangeNameGet(L7_UNIT_CURRENT, keyaclMacIndexValue, keyaclMacRuleIndexValue, (L7_uchar8 *)objaclMacRuleTimeRangeNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objaclMacRuleTimeRangeNameValue, owa.len);
  /* return the object value: aclMacRuleTimeRangeName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objaclMacRuleTimeRangeNameValue, strlen(objaclMacRuleTimeRangeNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclMacRuleTimeRangeName
*
* @purpose Set 'objaclMacRuleTimeRangeName'
*
* @description [objaclMacRuleTimeRangeName]:Name of time range associated with the MAC ACL Rule
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclMacRuleTimeRangeName(void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objaclMacRuleTimeRangeNameValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacRuleIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclIpv6RuleLogging */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objaclMacRuleTimeRangeNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objaclMacRuleTimeRangeNameValue, owa.len);


  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclMacIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                           (xLibU8_t *) &keyaclMacIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwa1.len);

  /* retrieve key: aclMacRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleIndex,
                           (xLibU8_t *) & keyaclMacRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacRuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbQosAclMacRuleTimeRangeNameAdd(L7_UNIT_CURRENT, keyaclMacIndexValue,
                              keyaclMacRuleIndexValue,
                              (L7_uchar8 *)objaclMacRuleTimeRangeNameValue);
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
* @function fpObjList_fastPathQOSACLQOSACL_aclMacRuleTimeRangeName
*
* @purpose Get 'aclMacRuleTimeRangeName'
*
* @description [aclMacRuleTimeRangeName]:Name of time range associated with the MAC ACL Rule
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_fastPathQOSACLQOSACL_aclMacRuleTimeRangeName(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
#ifdef L7_TIMERANGES_PACKAGE
  xLibStr256_t timeRangeNameValue;
  xLibStr256_t nextTimeRangeNameValue;
  xLibU32_t timeRangeIndexValue;
  xLibU32_t nextTimeRangeIndexValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof (timeRangeNameValue);
  memset(timeRangeNameValue,0, sizeof(timeRangeNameValue));
  memset(nextTimeRangeNameValue, 0, sizeof(nextTimeRangeNameValue));
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleTimeRangeName,
                          (xLibU8_t *) timeRangeNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    timeRangeIndexValue =  0;
    nextTimeRangeIndexValue = 0;
    owa.l7rc = usmDbTimeRangeIndexFirstGet(L7_UNIT_CURRENT,
                  &nextTimeRangeIndexValue);
    if(owa.l7rc == L7_SUCCESS)
    {
          /* get the value from application */
          owa.l7rc = usmDbTimeRangeNameGet(L7_UNIT_CURRENT,
                                   nextTimeRangeIndexValue,
                                   (L7_uchar8 *) nextTimeRangeNameValue);

    }       
  }
  else
  {
  
      owa.l7rc =usmDbTimeRangeNameToIndex(L7_UNIT_CURRENT, (L7_uchar8 *)timeRangeNameValue, &timeRangeIndexValue);
      if(owa.l7rc == L7_SUCCESS)
      {
          /* get next time range index and name */
          FPOBJ_TRACE_CURRENT_KEY(bufp, &timeRangeIndexValue, owa.len);
          owa.l7rc = usmDbTimeRangeIndexNextGet(L7_UNIT_CURRENT,  timeRangeIndexValue,
                  &nextTimeRangeIndexValue);
          if(owa.l7rc == L7_SUCCESS)
          {
             /* get the value from application */
              owa.l7rc = usmDbTimeRangeNameGet(L7_UNIT_CURRENT,
                                   nextTimeRangeIndexValue,
                                   (L7_uchar8 *) nextTimeRangeNameValue);
          }

      }     
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextTimeRangeNameValue, owa.len);

  /* return the object value: aclIfDirection */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextTimeRangeNameValue,
                           strlen(nextTimeRangeNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
#endif
  return owa.rc;
}
/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclMacRuleTimeRangeStatus
*
* @purpose Get 'aclMacRuleTimeRangeStatus'
*
* @description [aclMacRuleTimeRangeStatus]: Flag Indicates the MAC ACL Rule Status.
* ACL rule Status is derived from the status of time range associated with ACL.
* If no time range is associated to an ACL rule then status of ACL rule is always active.
* If time range is associated to an ACL rule and time range is non existing in system,
* then ACL rule status is considered to be active
*
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclMacRuleTimeRangeStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclMacRuleTimeRangeStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclMacIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                           (xLibU8_t *) & keyaclMacIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwa1.len);

  /* retrieve key: aclMacRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacRuleIndex,
                           (xLibU8_t *) & keyaclMacRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacRuleIndexValue, kwa2.len);

  /* get the value from application */

  owa.l7rc = usmDbQosAclMacRuleStatusGet (L7_UNIT_CURRENT, keyaclMacIndexValue,
                                           keyaclMacRuleIndexValue,
                                           &objaclMacRuleTimeRangeStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: aclMacRuleTimeRangeStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclMacRuleTimeRangeStatusValue,
                           sizeof (objaclMacRuleTimeRangeStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclRuleAction
*
* @purpose Get 'aclRuleAction'
*
* @description [aclRuleAction]: The type of action this rule should perform,
*              either permit or deny. Used by aclTrapRuleLogEvent trap.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclRuleAction (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclRuleActionValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                           (xLibU8_t *) & keyaclIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, kwa1.len);

  /* retrieve key: aclRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                           (xLibU8_t *) & keyaclRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbQosAclRuleActionGet (L7_UNIT_CURRENT, keyaclIndexValue,
                                       keyaclRuleIndexValue,
                                       &objaclRuleActionValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: aclRuleAction */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclRuleActionValue,
                           sizeof (objaclRuleActionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclRuleAction
*
* @purpose Set 'aclRuleAction'
*
* @description [aclRuleAction]: The type of action this rule should perform,
*              either permit or deny. Used by aclTrapRuleLogEvent trap.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclRuleAction (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclRuleActionValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclRuleIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclRuleAction */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclRuleActionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclRuleActionValue, owa.len);

  /* retrieve key: aclIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                           (xLibU8_t *) & keyaclIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, kwa1.len);

  /* retrieve key: aclRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                           (xLibU8_t *) & keyaclRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbQosAclRuleActionAdd (L7_UNIT_CURRENT, keyaclIndexValue,
                                       keyaclRuleIndexValue,
                                       objaclRuleActionValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    if (owa.l7rc == L7_REQUEST_DENIED)
    {
      owa.rc = XLIBRC_ACL_CHANGE_REQ_DENY_APP_RULE_LIMIT;
    }
    else
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclRuleProtocol
*
* @purpose Get 'aclRuleProtocol'
*
* @description [aclRuleProtocol]: icmp - 1 igmp - 2 ip - 4 tcp - 6 udp - 17
*              All values from 1 to 255 are valid. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclRuleProtocol (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclRuleProtocolValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                           (xLibU8_t *) & keyaclIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, kwa1.len);

  /* retrieve key: aclRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                           (xLibU8_t *) & keyaclRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, kwa2.len);

  /* get the value from application */
  if (usmDbQosAclIsFieldConfigured(L7_UNIT_CURRENT, keyaclIndexValue,
                                   keyaclRuleIndexValue, ACL_PROTOCOL) == L7_TRUE)
  {
    owa.l7rc = usmDbQosAclRuleProtocolGet (L7_UNIT_CURRENT, keyaclIndexValue,
                                           keyaclRuleIndexValue, &objaclRuleProtocolValue);
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

  /* return the object value: aclRuleProtocol */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclRuleProtocolValue,
                           sizeof (objaclRuleProtocolValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclRuleProtocol
*
* @purpose Set 'aclRuleProtocol'
*
* @description [aclRuleProtocol]: icmp - 1 igmp - 2 ip - 4 tcp - 6 udp - 17
*              All values from 1 to 255 are valid. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclRuleProtocol (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclRuleProtocolValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclRuleIndexValue;
  xLibU32_t aclRuleProtocolKeyword;
  xLibU16_t aclRuleProtocolKeywordSize = sizeof(aclRuleProtocolKeyword);

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclRuleProtocol */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclRuleProtocolValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclRuleProtocolValue, owa.len);

  /* retrieve key: aclIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                           (xLibU8_t *) & keyaclIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, kwa1.len);

  /* retrieve key: aclRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                           (xLibU8_t *) & keyaclRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  owa.rc = xLibFilterGet(wap, XOBJ_fastPathQOSACLQOSACL_aclRuleProtocolKeyWord,(xLibU8_t *) &aclRuleProtocolKeyword, &aclRuleProtocolKeywordSize);
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, kwa2.len);

  if(!aclRuleProtocolKeyword)
  {  
     /* set the value in application */
     owa.l7rc = usmDbQosAclRuleProtocolAdd (L7_UNIT_CURRENT, keyaclIndexValue,
                                         keyaclRuleIndexValue,
                                         objaclRuleProtocolValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  else{
      owa.rc = XLIBRC_SUCCESS;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclRuleSrcIpAddress
*
* @purpose Get 'aclRuleSrcIpAddress'
*
* @description [aclRuleSrcIpAddress]: The Source IP Address used in the ACL
*              Classification. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclRuleSrcIpAddress (void *wap,
                                                            void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclRuleSrcIpAddressValue;
  L7_uint32 ipMask;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                           (xLibU8_t *) & keyaclIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, kwa1.len);

  /* retrieve key: aclRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                           (xLibU8_t *) & keyaclRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, kwa2.len);

  /* get the value from application */
  if (usmDbQosAclIsFieldConfigured(L7_UNIT_CURRENT, keyaclIndexValue,
                                          keyaclRuleIndexValue,ACL_SRCIP) == L7_TRUE)
  {
     owa.l7rc = usmDbQosAclRuleSrcIpMaskGet (L7_UNIT_CURRENT, keyaclIndexValue,
                                          keyaclRuleIndexValue,
                                          &objaclRuleSrcIpAddressValue,&ipMask);
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

  /* return the object value: aclRuleSrcIpAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclRuleSrcIpAddressValue,
                           sizeof (objaclRuleSrcIpAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclRuleSrcIpAddress
*
* @purpose Set 'aclRuleSrcIpAddress'
*
* @description [aclRuleSrcIpAddress]: The Source IP Address used in the ACL
*              Classification. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclRuleSrcIpAddress (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclRuleSrcIpAddressValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclRuleIndexValue;
  L7_uint32 ipMask;
  L7_uint32 ipMask1,ipAddress1;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclRuleSrcIpAddress */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclRuleSrcIpAddressValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclRuleSrcIpAddressValue, owa.len);

  /* retrieve key: aclIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                           (xLibU8_t *) & keyaclIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, kwa1.len);

  /* retrieve key: aclRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                           (xLibU8_t *) & keyaclRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, kwa2.len);

  /* retrieve object: srcIpMask */
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleSrcIpMask,
                          (xLibU8_t *) &ipMask, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
   
  owa.l7rc = usmDbQosAclRuleSrcIpMaskGet (L7_UNIT_CURRENT, keyaclIndexValue,
                                          keyaclRuleIndexValue,
                                          &ipAddress1,&ipMask1);
  if (owa.l7rc != L7_SUCCESS)
  {
    /* Rule already present  with these attributes src ip and mack , hence cannot create */
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  ipMask = ipMask^(~(L7_uint32 )0);

  owa.l7rc = usmDbQosAclRuleSrcIpMaskAdd (L7_UNIT_CURRENT, keyaclIndexValue,
                                          keyaclRuleIndexValue,
                                          objaclRuleSrcIpAddressValue,ipMask);
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
* @function fpObjGet_fastPathQOSACLQOSACL_aclRuleSrcIpMask
*
* @purpose Get 'aclRuleSrcIpMask'
*
* @description [aclRuleSrcIpMask]: The Source IP Mask used in the ACL Classification.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclRuleSrcIpMask (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclRuleSrcIpMaskValue;
  L7_uint32 srcIp;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                           (xLibU8_t *) & keyaclIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, kwa1.len);

  /* retrieve key: aclRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                           (xLibU8_t *) & keyaclRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, kwa2.len);

  /* get the value from application */
  if (usmDbQosAclIsFieldConfigured(L7_UNIT_CURRENT, keyaclIndexValue,
                                          keyaclRuleIndexValue,ACL_SRCIP_MASK) == L7_TRUE)
  {
     owa.l7rc = usmDbQosAclRuleSrcIpMaskGet (L7_UNIT_CURRENT, keyaclIndexValue,
                                          keyaclRuleIndexValue,&srcIp,
                                          &objaclRuleSrcIpMaskValue);
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

  objaclRuleSrcIpMaskValue = objaclRuleSrcIpMaskValue^(~(L7_uint32 )0);

  /* return the object value: aclRuleSrcIpMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclRuleSrcIpMaskValue,
                           sizeof (objaclRuleSrcIpMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclRuleSrcIpMask
*
* @purpose Set 'aclRuleSrcIpMask'
*
* @description [aclRuleSrcIpMask]: The Source IP Mask used in the ACL Classification.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclRuleSrcIpMask (void *wap, void *bufp)
{
  return  XLIBRC_SUCCESS;
}


/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclRuleSrcL4Port
*
* @purpose Get 'aclRuleSrcL4Port'
*
* @description [aclRuleSrcL4Port]: The Source Port Number (Layer 4) used in
*              the ACL Classification. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclRuleSrcL4Port (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclRuleSrcL4PortValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                           (xLibU8_t *) & keyaclIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, kwa1.len);

  /* retrieve key: aclRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                           (xLibU8_t *) & keyaclRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, kwa2.len);

  /* get the value from application */
  if (usmDbQosAclIsFieldConfigured(L7_UNIT_CURRENT, keyaclIndexValue,
                                          keyaclRuleIndexValue,ACL_SRCPORT) == L7_TRUE)
  {
     owa.l7rc = usmDbQosAclRuleSrcL4PortGet (L7_UNIT_CURRENT, keyaclIndexValue,
                                          keyaclRuleIndexValue,
                                          &objaclRuleSrcL4PortValue);
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

  /* return the object value: aclRuleSrcL4Port */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclRuleSrcL4PortValue,
                           sizeof (objaclRuleSrcL4PortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclRuleSrcL4Port
*
* @purpose Set 'aclRuleSrcL4Port'
*
* @description [aclRuleSrcL4Port]: The Source Port Number (Layer 4) used in
*              the ACL Classification. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclRuleSrcL4Port (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclRuleSrcL4PortValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclRuleIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclRuleSrcL4Port */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclRuleSrcL4PortValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclRuleSrcL4PortValue, owa.len);

  /* retrieve key: aclIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                           (xLibU8_t *) & keyaclIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, kwa1.len);

  /* retrieve key: aclRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                           (xLibU8_t *) & keyaclRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbQosAclRuleSrcL4PortAdd (L7_UNIT_CURRENT, keyaclIndexValue,
                                          keyaclRuleIndexValue,
                                          objaclRuleSrcL4PortValue);
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
* @function fpObjGet_fastPathQOSACLQOSACL_aclRuleSrcL4PortRangeStart
*
* @purpose Get 'aclRuleSrcL4PortRangeStart'
*
* @description [aclRuleSrcL4PortRangeStart]: The Source Port Number(Layer
*              4) range start. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclRuleSrcL4PortRangeStart (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclRuleSrcL4PortRangeStartValue;
  L7_uint32 endPort;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                           (xLibU8_t *) & keyaclIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, kwa1.len);

  /* retrieve key: aclRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                           (xLibU8_t *) & keyaclRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, kwa2.len);

  /* get the value from application */
  if (usmDbQosAclIsFieldConfigured(L7_UNIT_CURRENT, keyaclIndexValue,
                                      keyaclRuleIndexValue,ACL_SRCSTARTPORT) == L7_TRUE)
  {
    owa.l7rc =
      usmDbQosAclRuleSrcL4PortRangeGet (L7_UNIT_CURRENT, keyaclIndexValue,
                                      keyaclRuleIndexValue,
                                      &objaclRuleSrcL4PortRangeStartValue,
                                      &endPort);
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

  /* return the object value: aclRuleSrcL4PortRangeStart */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objaclRuleSrcL4PortRangeStartValue,
                    sizeof (objaclRuleSrcL4PortRangeStartValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclRuleSrcL4PortRangeStart
*
* @purpose Set 'aclRuleSrcL4PortRangeStart'
*
* @description [aclRuleSrcL4PortRangeStart]: The Source Port Number(Layer
*              4) range start. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclRuleSrcL4PortRangeStart (void *wap,
                                                                   void *bufp)
{
  return XLIBRC_SUCCESS;
}


/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclRuleSrcL4PortRangeEnd
*
* @purpose Get 'aclRuleSrcL4PortRangeEnd'
*
* @description [aclRuleSrcL4PortRangeEnd]: The Source Port Number(Layer 4)
*              range end. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclRuleSrcL4PortRangeEnd (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclRuleSrcL4PortRangeEndValue;
  L7_uint32 startPort;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                           (xLibU8_t *) & keyaclIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, kwa1.len);

  /* retrieve key: aclRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                           (xLibU8_t *) & keyaclRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, kwa2.len);

  /* get the value from application */
  if (usmDbQosAclIsFieldConfigured(L7_UNIT_CURRENT, keyaclIndexValue,
                                      keyaclRuleIndexValue, ACL_SRCENDPORT) == L7_TRUE)
  {
    owa.l7rc =
       usmDbQosAclRuleSrcL4PortRangeGet (L7_UNIT_CURRENT, keyaclIndexValue,
                                      keyaclRuleIndexValue,
                                      &startPort,
                                      &objaclRuleSrcL4PortRangeEndValue);
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

  /* return the object value: aclRuleSrcL4PortRangeEnd */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objaclRuleSrcL4PortRangeEndValue,
                    sizeof (objaclRuleSrcL4PortRangeEndValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclRuleSrcL4PortRangeEnd
*
* @purpose Set 'aclRuleSrcL4PortRangeEnd'
*
* @description [aclRuleSrcL4PortRangeEnd]: The Source Port Number(Layer 4)
*              range end. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclRuleSrcL4PortRangeEnd (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclRuleSrcL4PortRangeEndValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclRuleIndexValue;
  L7_uint32 startPort;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclRuleSrcL4PortRangeEnd */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclRuleSrcL4PortRangeEndValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclRuleSrcL4PortRangeEndValue, owa.len);

  /* retrieve key: aclIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                           (xLibU8_t *) & keyaclIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, kwa1.len);

  /* retrieve key: aclRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                           (xLibU8_t *) & keyaclRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, kwa2.len);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleSrcL4PortRangeStart,(xLibU8_t *) & startPort,&owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return owa.rc;
  }
  /* set the value in application */
  if (startPort > objaclRuleSrcL4PortRangeEndValue)
  {
    owa.rc = XLIBRC_ACL_INVALID_PORT_RANGE ;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return owa.rc;
  }

  owa.l7rc =
    usmDbQosAclRuleSrcL4PortRangeAdd (L7_UNIT_CURRENT, keyaclIndexValue,
                                      keyaclRuleIndexValue,
                                      startPort,
                                      objaclRuleSrcL4PortRangeEndValue);
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
* @function fpObjGet_fastPathQOSACLQOSACL_aclRuleDestIpAddress
*
* @purpose Get 'aclRuleDestIpAddress'
*
* @description [aclRuleDestIpAddress]: The Destination IP Address used in
*              the ACL Classification. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclRuleDestIpAddress (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclRuleDestIpAddressValue;
  L7_uint32 dstIpMask;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                           (xLibU8_t *) & keyaclIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, kwa1.len);

  /* retrieve key: aclRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                           (xLibU8_t *) & keyaclRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, kwa2.len);

  /* get the value from application */
  if (usmDbQosAclIsFieldConfigured(L7_UNIT_CURRENT, keyaclIndexValue,
                                          keyaclRuleIndexValue,ACL_DSTIP) == L7_TRUE)
  {
    owa.l7rc = usmDbQosAclRuleDstIpMaskGet (L7_UNIT_CURRENT, keyaclIndexValue,
                                          keyaclRuleIndexValue,
                                          &objaclRuleDestIpAddressValue,&dstIpMask);
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

  /* return the object value: aclRuleDestIpAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclRuleDestIpAddressValue,
                           sizeof (objaclRuleDestIpAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclRuleDestIpAddress
*
* @purpose Set 'aclRuleDestIpAddress'
*
* @description [aclRuleDestIpAddress]: The Destination IP Address used in
*              the ACL Classification. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclRuleDestIpAddress (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclRuleDestIpAddressValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclRuleIndexValue;
  L7_uint32 dstIpMask;
  L7_uint32 dstIpMask1,dstIpAddress1;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclRuleDestIpAddress */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclRuleDestIpAddressValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclRuleDestIpAddressValue, owa.len);

  /* retrieve key: aclIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                           (xLibU8_t *) & keyaclIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, kwa1.len);

  /* retrieve key: aclRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                           (xLibU8_t *) & keyaclRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, kwa2.len);
  
  /* retrieve object: srcIpMask */
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleDestIpMask,
                          (xLibU8_t *) &dstIpMask, &owa.len); 
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* set the value in application */  
  owa.l7rc = usmDbQosAclRuleDstIpMaskGet (L7_UNIT_CURRENT, keyaclIndexValue,
                                          keyaclRuleIndexValue,
                                          &dstIpAddress1,&dstIpMask1);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  dstIpMask = dstIpMask^(~(L7_uint32 )0);

  owa.l7rc = usmDbQosAclRuleDstIpMaskAdd (L7_UNIT_CURRENT, keyaclIndexValue,
                                          keyaclRuleIndexValue,
                                          objaclRuleDestIpAddressValue,dstIpMask);
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
* @function fpObjGet_fastPathQOSACLQOSACL_aclRuleDestIpMask
*
* @purpose Get 'aclRuleDestIpMask'
*
* @description [aclRuleDestIpMask]: The Destination IP Mask used in the ACL
*              Classification. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclRuleDestIpMask (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclRuleDestIpMaskValue;
  L7_uint32 dstIp;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                           (xLibU8_t *) & keyaclIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, kwa1.len);

  /* retrieve key: aclRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                           (xLibU8_t *) & keyaclRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, kwa2.len);

  /* get the value from application */
  if (usmDbQosAclIsFieldConfigured(L7_UNIT_CURRENT, keyaclIndexValue,
                                          keyaclRuleIndexValue, ACL_DSTIP_MASK) == L7_TRUE)
  {
  
    owa.l7rc = usmDbQosAclRuleDstIpMaskGet (L7_UNIT_CURRENT, keyaclIndexValue,
                                          keyaclRuleIndexValue,&dstIp,
                                          &objaclRuleDestIpMaskValue);
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

  objaclRuleDestIpMaskValue = objaclRuleDestIpMaskValue^(~(L7_uint32 )0);

  /* return the object value: aclRuleDestIpMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclRuleDestIpMaskValue,
                           sizeof (objaclRuleDestIpMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclRuleDestIpMask
*
* @purpose Set 'aclRuleDestIpMask'
*
* @description [aclRuleDestIpMask]: The Destination IP Mask used in the ACL
*              Classification. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclRuleDestIpMask (void *wap, void *bufp)
{
  return XLIBRC_SUCCESS;
}


/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclRuleDestL4Port
*
* @purpose Get 'aclRuleDestL4Port'
*
* @description [aclRuleDestL4Port]: The Destination Port (Layer 4) used in
*              ACl classification. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclRuleDestL4Port (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclRuleDestL4PortValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                           (xLibU8_t *) & keyaclIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, kwa1.len);

  /* retrieve key: aclRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                           (xLibU8_t *) & keyaclRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, kwa2.len);

  /* get the value from application */
  if (usmDbQosAclIsFieldConfigured(L7_UNIT_CURRENT, keyaclIndexValue,
                                          keyaclRuleIndexValue,ACL_DSTPORT) == L7_TRUE)
  {

     owa.l7rc = usmDbQosAclRuleDstL4PortGet (L7_UNIT_CURRENT, keyaclIndexValue,
                                          keyaclRuleIndexValue,
                                          &objaclRuleDestL4PortValue);
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

  /* return the object value: aclRuleDestL4Port */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclRuleDestL4PortValue,
                           sizeof (objaclRuleDestL4PortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclRuleDestL4Port
*
* @purpose Set 'aclRuleDestL4Port'
*
* @description [aclRuleDestL4Port]: The Destination Port (Layer 4) used in
*              ACl classification. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclRuleDestL4Port (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclRuleDestL4PortValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclRuleIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclRuleDestL4Port */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclRuleDestL4PortValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclRuleDestL4PortValue, owa.len);

  /* retrieve key: aclIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                           (xLibU8_t *) & keyaclIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, kwa1.len);

  /* retrieve key: aclRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                           (xLibU8_t *) & keyaclRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbQosAclRuleDstL4PortAdd (L7_UNIT_CURRENT, keyaclIndexValue,
                                          keyaclRuleIndexValue,
                                          objaclRuleDestL4PortValue);
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
* @function fpObjGet_fastPathQOSACLQOSACL_aclRuleDestL4PortRangeStart
*
* @purpose Get 'aclRuleDestL4PortRangeStart'
*
* @description [aclRuleDestL4PortRangeStart]: The Destination Port (Layer
*              4) starting range used in ACL classification. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclRuleDestL4PortRangeStart (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclRuleDestL4PortRangeStartValue;
  L7_uint32 endPort;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                           (xLibU8_t *) & keyaclIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, kwa1.len);

  /* retrieve key: aclRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                           (xLibU8_t *) & keyaclRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, kwa2.len);

  /* get the value from application */
  if (usmDbQosAclIsFieldConfigured(L7_UNIT_CURRENT, keyaclIndexValue,
                                      keyaclRuleIndexValue, ACL_DSTSTARTPORT) == L7_TRUE)
  { 
    owa.l7rc =
        usmDbQosAclRuleDstL4PortRangeGet (L7_UNIT_CURRENT, keyaclIndexValue,
                                      keyaclRuleIndexValue,
                                      &objaclRuleDestL4PortRangeStartValue,&endPort);
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

  /* return the object value: aclRuleDestL4PortRangeStart */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objaclRuleDestL4PortRangeStartValue,
                    sizeof (objaclRuleDestL4PortRangeStartValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclRuleDestL4PortRangeStart
*
* @purpose Set 'aclRuleDestL4PortRangeStart'
*
* @description [aclRuleDestL4PortRangeStart]: The Destination Port (Layer
*              4) starting range used in ACL classification. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclRuleDestL4PortRangeStart (void *wap,
                                                                    void *bufp)
{
  return XLIBRC_SUCCESS;
}


/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclRuleDestL4PortRangeEnd
*
* @purpose Get 'aclRuleDestL4PortRangeEnd'
*
* @description [aclRuleDestL4PortRangeEnd]: The Destination Port (Layer 4)
*              ending range used in ACL classification 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclRuleDestL4PortRangeEnd (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  L7_uint32 startPort;
  xLibU32_t objaclRuleDestL4PortRangeEndValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                           (xLibU8_t *) & keyaclIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, kwa1.len);

  /* retrieve key: aclRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                           (xLibU8_t *) & keyaclRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, kwa2.len);

  /* get the value from application */
  if (usmDbQosAclIsFieldConfigured(L7_UNIT_CURRENT, keyaclIndexValue,
                                      keyaclRuleIndexValue, ACL_DSTENDPORT) == L7_TRUE)
  {
    owa.l7rc =
        usmDbQosAclRuleDstL4PortRangeGet (L7_UNIT_CURRENT, keyaclIndexValue,
                                      keyaclRuleIndexValue,
                                      &startPort,
                                      &objaclRuleDestL4PortRangeEndValue);
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

  /* return the object value: aclRuleDestL4PortRangeEnd */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objaclRuleDestL4PortRangeEndValue,
                    sizeof (objaclRuleDestL4PortRangeEndValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclRuleDestL4PortRangeEnd
*
* @purpose Set 'aclRuleDestL4PortRangeEnd'
*
* @description [aclRuleDestL4PortRangeEnd]: The Destination Port (Layer 4)
*              ending range used in ACL classification 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclRuleDestL4PortRangeEnd (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclRuleDestL4PortRangeEndValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclRuleIndexValue;
  L7_uint32 startPort;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclRuleDestL4PortRangeEnd */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclRuleDestL4PortRangeEndValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclRuleDestL4PortRangeEndValue, owa.len);

  /* retrieve key: aclIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                           (xLibU8_t *) & keyaclIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, kwa1.len);

  /* retrieve key: aclRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                           (xLibU8_t *) & keyaclRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, kwa2.len);
  owa.len = sizeof(startPort);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleDestL4PortRangeStart,(xLibU8_t *) & startPort,&owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return owa.rc;
  }
  if (startPort > objaclRuleDestL4PortRangeEndValue)
  {
    owa.rc = XLIBRC_ACL_INVALID_PORT_RANGE ;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return owa.rc;
  }

  owa.l7rc =
    usmDbQosAclRuleDstL4PortRangeAdd (L7_UNIT_CURRENT, keyaclIndexValue,
                                      keyaclRuleIndexValue,
                                      startPort, 
                                      objaclRuleDestL4PortRangeEndValue);
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
* @function fpObjGet_fastPathQOSACLQOSACL_aclRuleIPDSCP
*
* @purpose Get 'aclRuleIPDSCP'
*
* @description [aclRuleIPDSCP]: The Differentiated Services Code Point value.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclRuleIPDSCP (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclRuleIPDSCPValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                           (xLibU8_t *) & keyaclIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, kwa1.len);

  /* retrieve key: aclRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                           (xLibU8_t *) & keyaclRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, kwa2.len);

  /* get the value from application */
  if (usmDbQosAclIsFieldConfigured(L7_UNIT_CURRENT, keyaclIndexValue,
                                       keyaclRuleIndexValue, ACL_IPDSCP) == L7_TRUE) 
  {
    owa.l7rc = usmDbQosAclRuleIPDscpGet (L7_UNIT_CURRENT, keyaclIndexValue,
                                       keyaclRuleIndexValue,
                                       &objaclRuleIPDSCPValue);
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

  /* return the object value: aclRuleIPDSCP */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclRuleIPDSCPValue,
                           sizeof (objaclRuleIPDSCPValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclRuleIPDSCP
*
* @purpose Set 'aclRuleIPDSCP'
*
* @description [aclRuleIPDSCP]: The Differentiated Services Code Point value.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclRuleIPDSCP (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclRuleIPDSCPValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclRuleIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclRuleIPDSCP */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclRuleIPDSCPValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclRuleIPDSCPValue, owa.len);

  /* retrieve key: aclIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                           (xLibU8_t *) & keyaclIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, kwa1.len);

  /* retrieve key: aclRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                           (xLibU8_t *) & keyaclRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbQosAclRuleIPDscpAdd (L7_UNIT_CURRENT, keyaclIndexValue,
                                       keyaclRuleIndexValue,
                                       objaclRuleIPDSCPValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_IP_ACL_IP_DSCP_OUT_OF_RANGE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclRuleIPDSCPEnum
*
* @purpose Get 'aclRuleIPDSCPEnum'
*
* @description [aclRuleIPDSCPEnum]: The Differentiated Services Code Point value.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclRuleIPDSCPEnum (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclRuleIPDSCPValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                           (xLibU8_t *) & keyaclIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, kwa1.len);

  /* retrieve key: aclRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                           (xLibU8_t *) & keyaclRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, kwa2.len);

  /* get the value from application */
  if (usmDbQosAclIsFieldConfigured(L7_UNIT_CURRENT, keyaclIndexValue,
                                       keyaclRuleIndexValue, ACL_IPDSCP) == L7_TRUE) 
  {
    owa.l7rc = usmDbQosAclRuleIPDscpGet (L7_UNIT_CURRENT, keyaclIndexValue,
                                       keyaclRuleIndexValue,
                                       &objaclRuleIPDSCPValue);
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

  /* return the object value: aclRuleIPDSCP */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclRuleIPDSCPValue,
                           sizeof (objaclRuleIPDSCPValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclRuleIPDSCPValue
*
* @purpose Set 'aclRuleIPDSCPEnum'
*
* @description [aclRuleIPDSCPEnum]: The Differentiated Services Code Point value.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclRuleIPDSCPEnum (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclRuleIPDSCPValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclRuleIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclRuleIPDSCP */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclRuleIPDSCPValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclRuleIPDSCPValue, owa.len);

  /* retrieve key: aclIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                           (xLibU8_t *) & keyaclIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, kwa1.len);

  /* retrieve key: aclRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                           (xLibU8_t *) & keyaclRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, kwa2.len);

  /* set the value in application */
  if (objaclRuleIPDSCPValue != L7_ACL_IPDSCP_OTHER)
  {
    owa.l7rc = usmDbQosAclRuleIPDscpAdd (L7_UNIT_CURRENT, keyaclIndexValue,
                                         keyaclRuleIndexValue,
                                         objaclRuleIPDSCPValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_IP_ACL_IP_DSCP_OUT_OF_RANGE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclRuleIpPrecedence
*
* @purpose Get 'aclRuleIpPrecedence'
*
* @description [aclRuleIpPrecedence]: The Type of Service (TOS) IP Precedence
*              value. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclRuleIpPrecedence (void *wap,
                                                            void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclRuleIpPrecedenceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                           (xLibU8_t *) & keyaclIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, kwa1.len);

  /* retrieve key: aclRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                           (xLibU8_t *) & keyaclRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, kwa2.len);

  /* get the value from application */
  if (usmDbQosAclIsFieldConfigured(L7_UNIT_CURRENT, keyaclIndexValue,
                                             keyaclRuleIndexValue, ACL_IPPREC) == L7_TRUE)
  {
    owa.l7rc = usmDbQosAclRuleIPPrecedenceGet (L7_UNIT_CURRENT, keyaclIndexValue,
                                             keyaclRuleIndexValue,
                                             &objaclRuleIpPrecedenceValue);
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

  /* return the object value: aclRuleIpPrecedence */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclRuleIpPrecedenceValue,
                           sizeof (objaclRuleIpPrecedenceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclRuleIpPrecedence
*
* @purpose Set 'aclRuleIpPrecedence'
*
* @description [aclRuleIpPrecedence]: The Type of Service (TOS) IP Precedence
*              value. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclRuleIpPrecedence (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclRuleIpPrecedenceValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclRuleIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclRuleIpPrecedence */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclRuleIpPrecedenceValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclRuleIpPrecedenceValue, owa.len);

  /* retrieve key: aclIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                           (xLibU8_t *) & keyaclIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, kwa1.len);

  /* retrieve key: aclRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                           (xLibU8_t *) & keyaclRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbQosAclRuleIPPrecedenceAdd (L7_UNIT_CURRENT, keyaclIndexValue,
                                             keyaclRuleIndexValue,
                                             objaclRuleIpPrecedenceValue);
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
* @function fpObjGet_fastPathQOSACLQOSACL_aclRuleIpTosBits
*
* @purpose Get 'aclRuleIpTosBits'
*
* @description [aclRuleIpTosBits]: The Type of Service (TOS) Bits value. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclRuleIpTosBits (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclRuleIpTosBitsValue;
  xLibStr256_t buf;
  L7_uint32 tosMask;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                           (xLibU8_t *) & keyaclIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, kwa1.len);

  /* retrieve key: aclRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                           (xLibU8_t *) & keyaclRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, kwa2.len);

  /* get the value from application */
  if (usmDbQosAclIsFieldConfigured(L7_UNIT_CURRENT, keyaclIndexValue,
                                      keyaclRuleIndexValue, ACL_IPTOS) == L7_TRUE)
  {
     owa.l7rc = usmDbQosAclRuleIPTosGet (L7_UNIT_CURRENT, keyaclIndexValue,
                                      keyaclRuleIndexValue,
                                      &objaclRuleIpTosBitsValue,&tosMask);
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

  osapiSnprintf(buf,sizeof(buf),"%2.2x",objaclRuleIpTosBitsValue);
 
  /* return the object value: aclRuleIpTosBits */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) buf,
                           strlen(buf));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclRuleIpTosBits
*
* @purpose Set 'aclRuleIpTosBits'
*
* @description [aclRuleIpTosBits]: The Type of Service (TOS) Bits value. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclRuleIpTosBits (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibStr256_t objaclRuleIpTosBitsValue;
  xLibStr256_t objaclRuleIpTosMaskValue;
  xLibU32_t uintobjaclRuleIpTosBitsValue;
  xLibU32_t uintobjaclRuleIpTosMaskValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclRuleIndexValue;
  xLibStr256_t tosBitsValue;
  xLibU8_t strTosBits;
  xLibStr256_t tosMaskValue;
  xLibU8_t strTosMask;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclRuleIpTosBits */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *)objaclRuleIpTosBitsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objaclRuleIpTosBitsValue, owa.len);
 

  /* retrieve key: aclIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                           (xLibU8_t *) & keyaclIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, kwa1.len);

  memset (tosBitsValue, 0, sizeof(tosBitsValue));
  osapiSnprintf(tosBitsValue, sizeof(tosBitsValue), objaclRuleIpTosBitsValue);
  if (usmDbConvertTwoDigitHex(tosBitsValue, &strTosBits) == L7_FALSE)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  uintobjaclRuleIpTosBitsValue = strTosBits;

  /* retrieve key: aclRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                           (xLibU8_t *) & keyaclRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, kwa2.len);

  kwa3.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIpTosMask,
                           (xLibU8_t *)objaclRuleIpTosMaskValue , &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclRuleIpTosMaskValue, kwa3.len);

  memset (tosMaskValue, 0, sizeof(tosMaskValue));
  osapiSnprintf(tosMaskValue, sizeof(tosMaskValue), objaclRuleIpTosMaskValue);
  if (usmDbConvertTwoDigitHex(tosMaskValue, &strTosMask) == L7_FALSE)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  uintobjaclRuleIpTosMaskValue = strTosMask;
  uintobjaclRuleIpTosMaskValue = ~uintobjaclRuleIpTosMaskValue & 0x000000FF;
  owa.l7rc = usmDbQosAclRuleIPTosAdd (L7_UNIT_CURRENT, keyaclIndexValue,
                                      keyaclRuleIndexValue,
                                      uintobjaclRuleIpTosBitsValue,
                                      uintobjaclRuleIpTosMaskValue);
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
* @function fpObjGet_fastPathQOSACLQOSACL_aclRuleIpTosMask
*
* @purpose Get 'aclRuleIpTosMask'
*
* @description [aclRuleIpTosMask]: The Type of Service (TOS) Mask value 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclRuleIpTosMask (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  L7_uint32 ipTos;
  xLibStr256_t buf;
  xLibU32_t objaclRuleIpTosMaskValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                           (xLibU8_t *) & keyaclIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, kwa1.len);

  /* retrieve key: aclRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                           (xLibU8_t *) & keyaclRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, kwa2.len);

  /* get the value from application */
  if (usmDbQosAclIsFieldConfigured(L7_UNIT_CURRENT, keyaclIndexValue,
                                      keyaclRuleIndexValue, ACL_IPTOS) == L7_TRUE)
  {
     owa.l7rc = usmDbQosAclRuleIPTosGet (L7_UNIT_CURRENT, keyaclIndexValue,
                                      keyaclRuleIndexValue,&ipTos,
                                      &objaclRuleIpTosMaskValue);
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
  objaclRuleIpTosMaskValue = ~objaclRuleIpTosMaskValue & 0x000000FF;
  osapiSnprintf(buf,sizeof(buf),"%2.2x",objaclRuleIpTosMaskValue);


  /* return the object value: aclRuleIpTosMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)buf,
                           strlen (buf));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclRuleIpTosMask
*
* @purpose Set 'aclRuleIpTosMask'
*
* @description [aclRuleIpTosMask]: The Type of Service (TOS) Mask value 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclRuleIpTosMask (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclRuleIpTosMaskValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclRuleIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclRuleIpTosMask */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclRuleIpTosMaskValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclRuleIpTosMaskValue, owa.len);

  /* retrieve key: aclIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                           (xLibU8_t *) & keyaclIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, kwa1.len);

  /* retrieve key: aclRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                           (xLibU8_t *) & keyaclRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, kwa2.len);

  owa.l7rc = L7_SUCCESS;
  owa.rc = XLIBRC_SUCCESS;    /* TODO: Change if required */
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclRuleStatus
*
* @purpose Get 'aclRuleStatus'
*
* @description [aclRuleStatus]: Status of this instance. It could be either
*              active, createAndGo, destroy 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclRuleStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclRuleStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                           (xLibU8_t *) & keyaclIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, kwa1.len);

  /* retrieve key: aclRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                           (xLibU8_t *) & keyaclRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, kwa2.len);

  if (keyaclRuleIndexValue < L7_ACL_MIN_RULE_NUM || 
      keyaclRuleIndexValue > L7_ACL_MAX_RULE_NUM)
  {
    owa.rc = XLIBRC_ACL_INVALID_RULE_ID_RANGE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  /* get the value from application */
  objaclRuleStatusValue = L7_ROW_STATUS_ACTIVE;
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: aclRuleStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclRuleStatusValue,
                           sizeof (objaclRuleStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclRuleStatus
*
* @purpose Set 'aclRuleStatus'
*
* @description [aclRuleStatus]: Status of this instance. It could be either
*              active, createAndGo, destroy 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclRuleStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclRuleStatusValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclRuleIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclRuleStatus */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclRuleStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclRuleStatusValue, owa.len);

  /* retrieve key: aclIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                           (xLibU8_t *) & keyaclIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, kwa1.len);

  /* retrieve key: aclRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                           (xLibU8_t *) & keyaclRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, kwa2.len);

  if (keyaclRuleIndexValue < L7_ACL_MIN_RULE_NUM || 
      keyaclRuleIndexValue > L7_ACL_MAX_RULE_NUM)
  {
    owa.rc = XLIBRC_ACL_INVALID_RULE_ID_RANGE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
    /* call the usmdb only for add and delete */
  if (objaclRuleStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    /* Create a row */
    owa.l7rc = usmDbQosAclRuleActionAdd(L7_UNIT_CURRENT, keyaclIndexValue,
                                keyaclRuleIndexValue, L7_ACL_DENY);
    if (owa.l7rc != L7_SUCCESS)
    {
      if (owa.l7rc == L7_REQUEST_DENIED)
      {
        owa.rc = XLIBRC_ACL_CHANGE_REQ_DENY_APP_RULE_LIMIT;
      }
      else
      {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      }
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else if (objaclRuleStatusValue == L7_ROW_STATUS_DESTROY)
  {
    /* Delete the existing row */
    owa.l7rc = usmDbQosAclRuleRemove(L7_UNIT_CURRENT, keyaclIndexValue,keyaclRuleIndexValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      if (owa.l7rc == L7_REQUEST_DENIED)
      {
        owa.rc = XLIBRC_ACL_CHANGE_REQ_DENY_APP_USING;
      }
      else
      {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      }
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  return XLIBRC_SUCCESS;

}


/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclRuleAssignQueueId
*
* @purpose Get 'aclRuleAssignQueueId'
*
* @description [aclRuleAssignQueueId]: Queue identifier to which all inbound
*              packets matching this ACL rule are directed. This object
*              defaults to the standard queue assignment for user priority
*              0 traffic per the IEEE 802.1D specification based on the
*              number of assignable queues in the system: 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclRuleAssignQueueId (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclRuleAssignQueueIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                           (xLibU8_t *) & keyaclIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, kwa1.len);

  /* retrieve key: aclRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                           (xLibU8_t *) & keyaclRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, kwa2.len);

  /* get the value from application */
  if (usmDbQosAclIsFieldConfigured(L7_UNIT_CURRENT, keyaclIndexValue,
                                   keyaclRuleIndexValue, ACL_ASSIGN_QUEUEID) == L7_TRUE)
  {
    owa.l7rc = usmDbQosAclRuleAssignQueueIdGet (L7_UNIT_CURRENT, keyaclIndexValue,
                                              keyaclRuleIndexValue,
                                              &objaclRuleAssignQueueIdValue);
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

  /* return the object value: aclRuleAssignQueueId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclRuleAssignQueueIdValue,
                           sizeof (objaclRuleAssignQueueIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclRuleAssignQueueId
*
* @purpose Set 'aclRuleAssignQueueId'
*
* @description [aclRuleAssignQueueId]: Queue identifier to which all inbound
*              packets matching this ACL rule are directed. This object
*              defaults to the standard queue assignment for user priority
*              0 traffic per the IEEE 802.1D specification based on the
*              number of assignable queues in the system: 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclRuleAssignQueueId (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclRuleAssignQueueIdValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclRuleIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclRuleAssignQueueId */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclRuleAssignQueueIdValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclRuleAssignQueueIdValue, owa.len);

  /* retrieve key: aclIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                           (xLibU8_t *) & keyaclIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, kwa1.len);

  /* retrieve key: aclRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                           (xLibU8_t *) & keyaclRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbQosAclRuleAssignQueueIdAdd (L7_UNIT_CURRENT, keyaclIndexValue,
                                              keyaclRuleIndexValue,
                                              objaclRuleAssignQueueIdValue);
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
* @function fpObjGet_fastPathQOSACLQOSACL_aclRuleRedirectIntf
*
* @purpose Get 'aclRuleRedirectIntf'
*
* @description [aclRuleRedirectIntf]: A non-zero value indicates the external
*              ifIndex to which all inbound packets matching this ACL
*              rule are directed. A value of zero means packet redirection
*              is not in effect, which is the default value of this object.
*              Note that packet redirection and mirr 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclRuleRedirectIntf (void *wap,
                                                            void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclRuleRedirectIntfValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                           (xLibU8_t *) & keyaclIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, kwa1.len);

  /* retrieve key: aclRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                           (xLibU8_t *) & keyaclRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, kwa2.len);

  /* get the value from application */
  if (usmDbQosAclIsFieldConfigured(L7_UNIT_CURRENT, keyaclIndexValue,
                                             keyaclRuleIndexValue, ACL_REDIRECT_INTF) == L7_TRUE)
  {
    owa.l7rc = usmDbQosAclRuleRedirectIntfGet (L7_UNIT_CURRENT, keyaclIndexValue,
                                             keyaclRuleIndexValue,
                                             &objaclRuleRedirectIntfValue);
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

  /* return the object value: aclRuleRedirectIntf */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclRuleRedirectIntfValue,
                           sizeof (objaclRuleRedirectIntfValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclRuleRedirectIntf
*
* @purpose Set 'aclRuleRedirectIntf'
*
* @description [aclRuleRedirectIntf]: A non-zero value indicates the external
*              ifIndex to which all inbound packets matching this ACL
*              rule are directed. A value of zero means packet redirection
*              is not in effect, which is the default value of this object.
*              Note that packet redirection and mirr 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclRuleRedirectIntf (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclRuleRedirectIntfValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclRuleIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclRuleRedirectIntf */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclRuleRedirectIntfValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclRuleRedirectIntfValue, owa.len);

  /* retrieve key: aclIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                           (xLibU8_t *) & keyaclIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, kwa1.len);

  /* retrieve key: aclRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                           (xLibU8_t *) & keyaclRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbQosAclRuleRedirectIntfAdd (L7_UNIT_CURRENT, keyaclIndexValue,
                                             keyaclRuleIndexValue,
                                             objaclRuleRedirectIntfValue);
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
* @function fpObjList_fastPathQOSACLQOSACL_aclRuleRedirectIntf
*
* @purpose List 'aclRuleRedirectIntf'
 *@description  [aclRuleRedirectIntf] A non-zero value indicates the external
* ifIndex to which all inbound packets matching this ACL rule are
* directed. A value of zero means packet redirection is not in effect,
* which is the default value of this object. Note that packet
* redirection and mirr
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_fastPathQOSACLQOSACL_aclRuleRedirectIntf (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objaclRuleRedirectIntfValue;
  xLibU32_t nextObjaclRuleRedirectIntfValue;
  xLibU32_t mask =USM_PHYSICAL_INTF;

  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof (objaclRuleRedirectIntfValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleRedirectIntf,
                          (xLibU8_t *) & objaclRuleRedirectIntfValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objaclRuleRedirectIntfValue =0;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclRuleRedirectIntfValue, owa.len);

  if (usmDbFeaturePresentCheck(L7_UNIT_CURRENT,L7_FLEX_QOS_ACL_COMPONENT_ID,L7_ACL_INTF_LAG_SUPPORT_FEATURE_ID) == L7_TRUE)
  {
    mask = USM_PHYSICAL_INTF | USM_LAG_INTF;
  }
  owa.l7rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT,  mask,
                                  0,objaclRuleRedirectIntfValue, &nextObjaclRuleRedirectIntfValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjaclRuleRedirectIntfValue, owa.len);

  /* return the object value: aclRuleRedirectIntf */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjaclRuleRedirectIntfValue,
                           sizeof (nextObjaclRuleRedirectIntfValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclRuleMatchEvery
*
* @purpose Get 'aclRuleMatchEvery'
*
* @description [aclRuleMatchEvery]: Flag to indicate that the ACL rule is
*              defined to match on every IP packetregardless of content. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclRuleMatchEvery (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclRuleMatchEveryValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                           (xLibU8_t *) & keyaclIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, kwa1.len);

  /* retrieve key: aclRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                           (xLibU8_t *) & keyaclRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbQosAclRuleEveryGet (L7_UNIT_CURRENT, keyaclIndexValue,
                                      keyaclRuleIndexValue,
                                      &objaclRuleMatchEveryValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: aclRuleMatchEvery */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclRuleMatchEveryValue,
                           sizeof (objaclRuleMatchEveryValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclRuleMatchEvery
*
* @purpose Set 'aclRuleMatchEvery'
*
* @description [aclRuleMatchEvery]: Flag to indicate that the ACL rule is
*              defined to match on every IP packetregardless of content. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclRuleMatchEvery (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclRuleMatchEveryValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclRuleIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclRuleMatchEvery */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclRuleMatchEveryValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclRuleMatchEveryValue, owa.len);

  /* retrieve key: aclIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                           (xLibU8_t *) & keyaclIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, kwa1.len);

  /* retrieve key: aclRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                           (xLibU8_t *) & keyaclRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbQosAclRuleEveryAdd (L7_UNIT_CURRENT, keyaclIndexValue,
                                      keyaclRuleIndexValue,
                                      objaclRuleMatchEveryValue);
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
* @function fpObjGet_fastPathQOSACLQOSACL_aclRuleMirrorIntf
*
* @purpose Get 'aclRuleMirrorIntf'
*
* @description [aclRuleMirrorIntf]: A non-zero value indicates the external
*              ifIndex to which all inbound packets matching this ACL rule
*              are copied. A value of zero means packet mirroring is not
*              in effect, which is the default value of this object. Note
*              that packet mirroring and redirectio 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclRuleMirrorIntf (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclRuleMirrorIntfValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                           (xLibU8_t *) & keyaclIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, kwa1.len);

  /* retrieve key: aclRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                           (xLibU8_t *) & keyaclRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, kwa2.len);

  /* get the value from application */
  if (usmDbQosAclIsFieldConfigured(L7_UNIT_CURRENT, keyaclIndexValue,
                                           keyaclRuleIndexValue,ACL_MIRROR_INTF) == L7_TRUE)
  {
    owa.l7rc = usmDbQosAclRuleMirrorIntfGet (L7_UNIT_CURRENT, keyaclIndexValue,
                                           keyaclRuleIndexValue,
                                           &objaclRuleMirrorIntfValue);
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

  /* return the object value: aclRuleMirrorIntf */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclRuleMirrorIntfValue,
                           sizeof (objaclRuleMirrorIntfValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclRuleMirrorIntf
*
* @purpose Set 'aclRuleMirrorIntf'
*
* @description [aclRuleMirrorIntf]: A non-zero value indicates the external
*              ifIndex to which all inbound packets matching this ACL rule
*              are copied. A value of zero means packet mirroring is not
*              in effect, which is the default value of this object. Note
*              that packet mirroring and redirectio 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclRuleMirrorIntf (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclRuleMirrorIntfValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclRuleIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclRuleMirrorIntf */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclRuleMirrorIntfValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclRuleMirrorIntfValue, owa.len);

  /* retrieve key: aclIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                           (xLibU8_t *) & keyaclIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, kwa1.len);

  /* retrieve key: aclRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                           (xLibU8_t *) & keyaclRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbQosAclRuleMirrorIntfAdd (L7_UNIT_CURRENT, keyaclIndexValue,
                                           keyaclRuleIndexValue,
                                           objaclRuleMirrorIntfValue);
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
* @function fpObjList_fastPathQOSACLQOSACL_aclRuleMirrorIntf
*
* @purpose List 'aclRuleMirrorIntf'
 *@description  [aclRuleMirrorIntf] A non-zero value indicates the external
* ifIndex to which all inbound packets matching this ACL rule are
* copied. A value of zero means packet mirroring is not in effect, which
* is the default value of this object. Note that packet mirroring
* and redirectio
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_fastPathQOSACLQOSACL_aclRuleMirrorIntf (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objaclRuleMirrorIntfValue;
  xLibU32_t nextObjaclRuleMirrorIntfValue;
  xLibU32_t mask = USM_PHYSICAL_INTF;
  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof (objaclRuleMirrorIntfValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleMirrorIntf,
                          (xLibU8_t *) & objaclRuleMirrorIntfValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objaclRuleMirrorIntfValue =0;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclRuleMirrorIntfValue, owa.len);
  if (usmDbFeaturePresentCheck(L7_UNIT_CURRENT,L7_FLEX_QOS_ACL_COMPONENT_ID,L7_ACL_INTF_LAG_SUPPORT_FEATURE_ID) == L7_TRUE)
  {
    mask = USM_PHYSICAL_INTF | USM_LAG_INTF;
  }
  owa.l7rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT,  mask,
                                  0,objaclRuleMirrorIntfValue, &nextObjaclRuleMirrorIntfValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjaclRuleMirrorIntfValue, owa.len);

  /* return the object value: aclRuleMirrorIntf */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjaclRuleMirrorIntfValue,
                           sizeof (nextObjaclRuleMirrorIntfValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
 * * @function fpObjGet_fastPathQOSACLQOSACL_aclMacName
 * *
 * * @purpose Get 'aclMacName'
 * *
 * * @description [aclMacName]: The name of this MAC ACL entry, which must consist
 * *              of1 to 31 alphanumeric characters and uniquely identify
 * *              this MAC ACL. An existing MAC ACL can be renamed by setting
 * *              this object to a new name.
 * *
 * * @return
 * *******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclMacName (void *wap, void *bufp)
{
  fpObjWa_t kwaaclMacIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objaclMacNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclMacIndex */
  kwaaclMacIndex.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                                     (xLibU8_t *) & keyaclMacIndexValue,
                                     &kwaaclMacIndex.len);
  if (kwaaclMacIndex.rc != XLIBRC_SUCCESS)
  {
    kwaaclMacIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaaclMacIndex);
    return kwaaclMacIndex.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwaaclMacIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbQosAclMacNameGet(L7_UNIT_CURRENT, keyaclMacIndexValue,
                              (L7_uchar8 *)objaclMacNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: aclMacName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objaclMacNameValue,
                           strlen (objaclMacNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * * @function fpObjSet_fastPathQOSACLQOSACL_aclMacName
 * *
 * * @purpose Set 'aclMacName'
 * *
 * * @description [aclMacName]: The name of this MAC ACL entry, which must consist
 * *              of1 to 31 alphanumeric characters and uniquely identify
 * *              this MAC ACL. An existing MAC ACL can be renamed by setting
 * *              this object to a new name.
 * *
 * * @return
 * *******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclMacName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objaclMacNameValue;
	
  fpObjWa_t kwaaclMacIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;

	fpObjWa_t kwaaclMacRowStatus = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacRowStatusValue;
	
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclMacName */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objaclMacNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objaclMacNameValue, owa.len);

  /* retrieve key: aclMacIndex */
  kwaaclMacIndex.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                                     (xLibU8_t *) & keyaclMacIndexValue,
                                     &kwaaclMacIndex.len);
  if (kwaaclMacIndex.rc != XLIBRC_SUCCESS)
  {
    kwaaclMacIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaaclMacIndex);
    return kwaaclMacIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwaaclMacIndex.len);

  /*get the row status object value */
  kwaaclMacRowStatus.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacStatus,
                                     (xLibU8_t *) &keyaclMacRowStatusValue,
                                     &kwaaclMacRowStatus.len);
  if (kwaaclMacRowStatus.rc != XLIBRC_SUCCESS)
  {
    keyaclMacRowStatusValue = L7_ROW_STATUS_ACTIVE;
  }


  /* set the value in application */
  if(keyaclMacRowStatusValue == L7_ROW_STATUS_ACTIVE)
  {
    owa.l7rc = usmDbQosAclMacNameChange (L7_UNIT_CURRENT, keyaclMacIndexValue,
        (L7_uchar8 *)objaclMacNameValue);

    if (owa.l7rc != L7_SUCCESS)
    {
      if(owa.l7rc == L7_ALREADY_CONFIGURED)
      {
        /* Name already exists */
        owa.rc = XLIBRC_ACL_NAME_ALREADY_EXISTS;  /* TODO: Change if required */
      }
      else
      {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      }
    }
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * * @function fpObjGet_fastPathQOSACLQOSACL_aclMacStatus
 * *
 * * @purpose Get 'aclMacStatus'
 * *
 * * @description [aclMacStatus]: Status of this instance. ACL MAC entries can
 * *              not be deleted until all rows in the aclIfTable and aclRuleTable
 * *              with corresponding values of aclMacIndex have been
 * *              deleted.It could be either active, createAndGo or destroy
 * *
 * * @return
 * *******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclMacStatus (void *wap, void *bufp)
{
  fpObjWa_t kwaaclMacIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclMacStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclMacIndex */
  kwaaclMacIndex.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                                     (xLibU8_t *) & keyaclMacIndexValue,
                                     &kwaaclMacIndex.len);
  if (kwaaclMacIndex.rc != XLIBRC_SUCCESS)
  {
    kwaaclMacIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaaclMacIndex);
    return kwaaclMacIndex.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwaaclMacIndex.len);

  /* get the value from application */
  objaclMacStatusValue = L7_ROW_STATUS_ACTIVE;
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: aclMacStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclMacStatusValue,
                           sizeof (objaclMacStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * * @function fpObjSet_fastPathQOSACLQOSACL_aclMacStatus
 * *
 * * @purpose Set 'aclMacStatus'
 * *
 * * @description [aclMacStatus]: Status of this instance. ACL MAC entries can
 * *              not be deleted until all rows in the aclIfTable and aclRuleTable
 * *              with corresponding values of aclMacIndex have been
 * *              deleted.It could be either active, createAndGo or destroy
 * *
 * * @return
 * *******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclMacStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclMacStatusValue;
  fpObjWa_t kwaaclMacIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclMacIndexValue;
  xLibStr256_t aclMacName;
  xLibU16_t  aclMacNameSize= sizeof(aclMacName);
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclMacStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objaclMacStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }   
  FPOBJ_TRACE_VALUE (bufp, &objaclMacStatusValue, owa.len);

  /* retrieve key: aclMacIndex */
  kwaaclMacIndex.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacIndex,
                                     (xLibU8_t *) & keyaclMacIndexValue,
                                     &kwaaclMacIndex.len);
  if (kwaaclMacIndex.rc != XLIBRC_SUCCESS && objaclMacStatusValue != L7_ROW_STATUS_CREATE_AND_GO) 
  {          
    kwaaclMacIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaaclMacIndex);
    return kwaaclMacIndex.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclMacIndexValue, kwaaclMacIndex.len);

  /* get the value of aclMacName from the filters  */
 
  memset(aclMacName , 0,aclMacNameSize); 
  xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclMacName,
                                     (xLibU8_t *)  aclMacName,
                                     &aclMacNameSize);
   
  /* call the usmdb only for add and delete */
  if (objaclMacStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    /* Create a row */         

    /* Check if the name does not exists already */
    owa.l7rc = usmDbQosAclMacNameToIndex(L7_UNIT_CURRENT, aclMacName,&keyaclMacIndexValue);
    if(owa.l7rc == L7_SUCCESS)   /*Name already exists*/
    {    
        owa.rc = XLIBRC_ACL_NAME_ALREADY_EXISTS;
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
    }  
    /* Get the next Valid Index */
    owa.l7rc = usmDbQosAclMacIndexNext(L7_UNIT_CURRENT, &keyaclMacIndexValue);   
    if(owa.l7rc == L7_TABLE_IS_FULL)
    {
         owa.rc = XLIBRC_FAILURE;
         FPOBJ_TRACE_EXIT (bufp, owa);
         return owa.rc;
    }
    else if(owa.l7rc == L7_FAILURE)
    {
      owa.rc = XLIBRC_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }    
    owa.l7rc = usmDbQosAclMacCreate(L7_UNIT_CURRENT, keyaclMacIndexValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      if(owa.l7rc == L7_TABLE_IS_FULL)
      {
        owa.rc = XLIBRC_ACL_TABLE_FULL_ERROR;
      }
      else if(owa.l7rc == L7_REQUEST_DENIED)
      {
        owa.rc = XLIBRC_ACL_CHANGE_REQ_DENY_APP;
      }
      else
      { 
        owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      } 
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    owa.l7rc = usmDbQosAclMacNameAdd(L7_UNIT_CURRENT, keyaclMacIndexValue,aclMacName);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      usmDbQosAclMacDelete(L7_UNIT_CURRENT, keyaclMacIndexValue);
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    /* Creation of index successful Push the index into the filter*/
    /* Pass type as 0 */
    owa.rc = xLibFilterSet(wap,XOBJ_fastPathQOSACLQOSACL_aclMacIndex,0,(xLibU8_t *) &keyaclMacIndexValue,sizeof(keyaclMacIndexValue)); 
    if(owa.rc !=XLIBRC_SUCCESS)
    {
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  else if (objaclMacStatusValue == L7_ROW_STATUS_DESTROY)
  {
    /* Delete the existing row */
    owa.l7rc = usmDbQosAclMacDelete(L7_UNIT_CURRENT, keyaclMacIndexValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      if (owa.l7rc == L7_REQUEST_DENIED)
      {
        owa.rc = XLIBRC_ACL_CHANGE_REQ_DENY_APP_USING;
      }
      else
      { 
        owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      } 
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  return XLIBRC_SUCCESS;
}
/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclIpv6Name
*
* @purpose Get 'aclIpv6Name'
*
* @description [aclIpv6Name]: The name of this IPv6 ACL entry, which must
*              consist of1 to 31 alphanumeric characters and uniquely identify
*              setting this object to a new name. This object must be
*              set to complete a new IPv6 ACL row instance. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclIpv6Name (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6IndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objaclIpv6NameValue;
  FPOBJ_TRACE_ENTER (bufp);
  
  /* retrieve key: aclIpv6Index */
  kwa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index, (xLibU8_t *) & keyaclIpv6IndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbQosAclNameGet (L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                 (L7_uchar8 *)objaclIpv6NameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

 
  /* return the object value: aclIpv6Name */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objaclIpv6NameValue,
                           strlen (objaclIpv6NameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclIpv6Name
*
* @purpose Set 'aclIpv6Name'
*
* @description [aclIpv6Name]: The name of this IPv6 ACL entry, which must
*              consist of1 to 31 alphanumeric characters and uniquely identify
*              setting this object to a new name. This object must be
*              set to complete a new IPv6 ACL row instance. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclIpv6Name (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objaclIpv6NameValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6IndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclIpv6Name */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objaclIpv6NameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objaclIpv6NameValue, owa.len);

  /* retrieve key: aclIpv6Index */
  kwa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                          (xLibU8_t *) & keyaclIpv6IndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbQosAclNameChange (L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                                 (L7_uchar8 *)objaclIpv6NameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    if(owa.l7rc == L7_ALREADY_CONFIGURED)
    {
      /* Name already exists */
      owa.rc = XLIBRC_ACL_NAME_ALREADY_EXISTS;  /* TODO: Change if required */
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
* @function fpObjGet_fastPathQOSACLQOSACL_aclIpv6Status
*
* @purpose Get 'aclIpv6Status'
*
* @description [aclIpv6Status]: Status of this instance.This could be either
*              active, createAndGo or destroy 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclIpv6Status (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6IndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclIpv6StatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIpv6Index */
  kwa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                          (xLibU8_t *) & keyaclIpv6IndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, kwa.len);

  /* get the value from application */
  objaclIpv6StatusValue = L7_ROW_STATUS_ACTIVE;
 

  /* return the object value: aclIpv6Status */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclIpv6StatusValue,
                           sizeof (objaclIpv6StatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclIpv6Status
*
* @purpose Set 'aclIpv6Status'
*
* @description [aclIpv6Status]: Status of this instance.This could be either
*              active, createAndGo or destroy 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclIpv6Status (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclIpv6StatusValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIpv6IndexValue;
  xLibStr256_t aclName;
  xLibU16_t aclNameSize = sizeof(aclName);
  xLibU32_t aclId;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclIpv6Status */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objaclIpv6StatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclIpv6StatusValue, owa.len);

  /* retrieve key: aclIpv6Index */
  kwa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                          (xLibU8_t *) & keyaclIpv6IndexValue, &kwa.len);
 
  if( (kwa.rc != XLIBRC_SUCCESS) && (objaclIpv6StatusValue != L7_ROW_STATUS_CREATE_AND_GO))
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, kwa.len);

  xLibFilterGet (wap,XOBJ_fastPathQOSACLQOSACL_aclIpv6Name,(xLibU8_t *)aclName,&aclNameSize);
    
  /* For ACL Creation operation ... */
  
  if (objaclIpv6StatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
      /*Check if acl name already exists*/
      if(usmDbQosAclNameToIndex(L7_UNIT_CURRENT, aclName, &aclId) == L7_SUCCESS)
      {
          /* Error : Name already exists */
          owa.rc = XLIBRC_ACL_NAME_ALREADY_EXISTS;  /* TODO: Change if required */
          FPOBJ_TRACE_EXIT (bufp, owa);
          return owa.rc;
      }

      if((owa.l7rc = usmDbQosAclNamedIndexNextFree(L7_UNIT_CURRENT, L7_ACL_TYPE_IPV6, &aclId)) != L7_SUCCESS)
      {
          if(owa.l7rc == L7_TABLE_IS_FULL)
          {
            owa.rc = XLIBRC_ACL_TABLE_FULL_ERROR; 
          }
          else if(owa.l7rc == L7_NOT_SUPPORTED)
          {
            owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
          }          
          else 
          {
            /* Unknown failure */
            owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
          }          
          FPOBJ_TRACE_EXIT (bufp, owa);
          return owa.rc;
      }

      /* Create the ACL */

      if((owa.l7rc = usmDbQosAclCreate(L7_UNIT_CURRENT, aclId)) != L7_SUCCESS)
      {
        if(owa.l7rc == L7_TABLE_IS_FULL)
        {
          owa.rc = XLIBRC_ACL_TABLE_FULL_ERROR; 
        }
        else if(owa.l7rc == L7_REQUEST_DENIED)
        {
          owa.rc = XLIBRC_ACL_CHANGE_REQ_DENY_APP; 
        }
        else
        {
          owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
        }
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
      /* acl Name */
      if((owa.l7rc = usmDbQosAclNameAdd(L7_UNIT_CURRENT, aclId, aclName)) != L7_SUCCESS)
      {
          owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
          FPOBJ_TRACE_EXIT (bufp, owa);
          return owa.rc; 
      }  

      /* Creation of index successful Push the index into the filter*/
      xLibFilterSet(wap,XOBJ_fastPathQOSACLQOSACL_aclIpv6Index, 0, (xLibU8_t *)&aclId,sizeof(aclId)); 

      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
  }

  else if (objaclIpv6StatusValue == L7_ROW_STATUS_DESTROY)
  {
      /* Delete the existing row */
      owa.l7rc = usmDbQosAclDelete(L7_UNIT_CURRENT, keyaclIpv6IndexValue);
      if (owa.l7rc != L7_SUCCESS)
      {
        if (owa.l7rc == L7_REQUEST_DENIED)
        {
          owa.rc = XLIBRC_ACL_CHANGE_REQ_DENY_APP_USING;
        }
        else
        {
          owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
        }
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
  }
  return XLIBRC_SUCCESS;


}


/*******************************************************************************
 * @function fpObjGet_fastPathQOSACLQOSACL_aclIfStatus
 *
 * @purpose Get 'aclIfStatus'
 *
 * @description [aclIfStatus]: Status of this instance.. It could be eitherr
 *              active, createAndGo or destroy. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclIfStatus (void *wap, void *bufp)
{
    fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
    xLibU32_t keyaclIfIndexValue;
    fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
    xLibU32_t keyaclIfDirectionValue;
    fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
    xLibU32_t keyaclIfSequenceValue;
    fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
    xLibU32_t keyaclIfAclTypeValue;
    fpObjWa_t kwa5 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
    xLibU32_t keyaclIfAclIdValue;
    fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
    xLibU32_t objaclIfStatusValue;
    FPOBJ_TRACE_ENTER (bufp);

    /* retrieve key: aclIfIndex */
    kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIfIndex,
            (xLibU8_t *) & keyaclIfIndexValue, &kwa1.len);
    if (owa.l7rc != L7_SUCCESS)
    {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
    }
    FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIfIndexValue, kwa1.len);

    /* retrieve key: aclIfDirection */
    kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIfDirection,
            (xLibU8_t *) & keyaclIfDirectionValue, &kwa2.len);
    if (owa.l7rc != L7_SUCCESS)
    {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
    }
    FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIfDirectionValue, kwa2.len);

    /* retrieve key: aclIfSequence */
    kwa3.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIfSequence,
            (xLibU8_t *) & keyaclIfSequenceValue, &kwa3.len);
    if (owa.l7rc != L7_SUCCESS)
    {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
    }
    FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIfSequenceValue, kwa3.len);

    /* retrieve key: aclIfAclType */
    kwa4.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIfAclType,
            (xLibU8_t *) & keyaclIfAclTypeValue, &kwa4.len);
    if (owa.l7rc != L7_SUCCESS)
    {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
    }
    FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIfAclTypeValue, kwa4.len);

    /* retrieve key: aclIfAclId */
    kwa5.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIfAclId,
            (xLibU8_t *) & keyaclIfAclIdValue, &kwa5.len);
    if (owa.l7rc != L7_SUCCESS)
    {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
    }
    FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIfAclIdValue, kwa5.len);

    /* get the value from application */
    objaclIfStatusValue = L7_ROW_STATUS_ACTIVE;
    if (owa.l7rc != L7_SUCCESS)
    {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
    }

    /* return the object value: aclIfStatus */
    owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclIfStatusValue,
            sizeof (objaclIfStatusValue));
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
}


/*******************************************************************************
 * @function fpObjSet_fastPathQOSACLQOSACL_aclIfStatus
 *
 * @purpose Set 'aclIfStatus'
 *
 * @description [aclIfStatus]: Status of this instance.. It could be eitherr
 *              active, createAndGo or destroy. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclIfStatus (void *wap, void *bufp)
{
    fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
    xLibU32_t objaclIfStatusValue;
    fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
    xLibU32_t keyaclIfIndexValue;
    fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIfDirectionValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIfSequenceValue;
  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIfAclTypeValue;
  fpObjWa_t kwa5 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIfAclIdValue;
  xLibBool_t seqFlag = XLIB_TRUE; 
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclIfStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objaclIfStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclIfStatusValue, owa.len);

  /* retrieve key: clIfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIfIndex,
                           (xLibU8_t *) & keyaclIfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIfIndexValue, kwa1.len);

  /* retrieve key: aclIfDirection */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIfDirection,
                           (xLibU8_t *) & keyaclIfDirectionValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIfDirectionValue, kwa2.len);

  /* retrieve key: aclIfSequence */
  kwa3.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIfSequence,
                           (xLibU8_t *) & keyaclIfSequenceValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS && (objaclIfStatusValue != L7_ROW_STATUS_CREATE_AND_GO))
  {
    seqFlag = XLIB_FALSE;
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIfSequenceValue, kwa3.len);

  /* retrieve key: aclIfAclType */
  kwa4.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIfAclType,
                           (xLibU8_t *) & keyaclIfAclTypeValue, &kwa4.len);
  if (kwa4.rc != XLIBRC_SUCCESS)
  {
    kwa4.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa4);
    return kwa4.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIfAclTypeValue, kwa4.len);

  /* retrieve key: aclIfAclId */
  kwa5.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIfAclId,
                           (xLibU8_t *) & keyaclIfAclIdValue, &kwa5.len);
  if (kwa5.rc != XLIBRC_SUCCESS)
  {
    kwa5.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa5);
    return kwa5.rc;
  }
FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIfAclIdValue, kwa5.len);
    /* call the usmdb only for add and delete */
  if (objaclIfStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    if(seqFlag == XLIB_TRUE)
    {
       keyaclIfSequenceValue  = L7_ACL_AUTO_INCR_INTF_SEQ_NUM;
    }
    /* Create a row */
    owa.l7rc = fpobj_QosAclIntfAdd(L7_UNIT_CURRENT, keyaclIfIndexValue,
                                keyaclIfDirectionValue,
                                keyaclIfSequenceValue,
                                keyaclIfAclTypeValue,
                                keyaclIfAclIdValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else if (objaclIfStatusValue == L7_ROW_STATUS_DESTROY)
  {
    /* Delete the existing row */
    owa.l7rc = fpobj_QosAclIntfRemove (L7_UNIT_CURRENT, keyaclIfIndexValue,
                                keyaclIfDirectionValue,
                                keyaclIfSequenceValue,
                                keyaclIfAclTypeValue,
                                keyaclIfAclIdValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  return XLIBRC_SUCCESS;

}


/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclMacIndexNextFree
*
* @purpose Get 'aclMacIndexNextFree'
*
* @description [aclMacIndexNextFree]: This object contains an unused value
*              for the aclMacIndex to be used when creating a new MAC ACL.
*              A value of zero indicates the ACL table is full. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclMacIndexNextFree (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclMacIndexNextFreeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbQosAclMacIndexNext (L7_UNIT_CURRENT, &objaclMacIndexNextFreeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclMacIndexNextFreeValue,
                     sizeof (objaclMacIndexNextFreeValue));

  /* return the object value: aclMacIndexNextFree */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclMacIndexNextFreeValue,
                           sizeof (objaclMacIndexNextFreeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclIpv6IndexNextFree
*
* @purpose Get 'aclIpv6IndexNextFree'
*
* @description [aclIpv6IndexNextFree]: This object contains an unused value
*              for the aclIPv6Index to be used when creating a new IPv6
*              ACL. A value of zero indicates the ACL table is full. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclIpv6IndexNextFree (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclIpv6IndexNextFreeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbQosAclNamedIndexNextFree (L7_UNIT_CURRENT,L7_ACL_TYPE_IPV6,
                                   &objaclIpv6IndexNextFreeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclIpv6IndexNextFreeValue,
                     sizeof (objaclIpv6IndexNextFreeValue));

  /* return the object value: aclIpv6IndexNextFree */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclIpv6IndexNextFreeValue,
                           sizeof (objaclIpv6IndexNextFreeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclTrapRuleAction
*
* @purpose Get 'aclTrapRuleAction'
*
* @description [aclTrapRuleAction]: The type of action this rule should perform,
*              either permit or deny Used by aclTrapRuleLogEvent trap.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclTrapRuleAction (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclTrapRuleActionValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclTrapRuleActionValue,
                     sizeof (objaclTrapRuleActionValue));

  /* return the object value: aclTrapRuleAction */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclTrapRuleActionValue,
                           sizeof (objaclTrapRuleActionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclTrapRuleIndex
*
* @purpose Get 'aclTrapRuleIndex'
*
* @description [aclTrapRuleIndex]: The index of an ACL rule instance.Used
*              by aclTrapRuleLogEvent trap. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclTrapRuleIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclTrapRuleIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclTrapRuleIndexValue,
                     sizeof (objaclTrapRuleIndexValue));

  /* return the object value: aclTrapRuleIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclTrapRuleIndexValue,
                           sizeof (objaclTrapRuleIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclTrapRuleHitCount
*
* @purpose Get 'aclTrapRuleHitCount'
*
* @description [aclTrapRuleHitCount]: Number of times the ACL rule was hit
*              during the most recent logging interval. Used by aclTrapRuleLogEvent
*              trap. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclTrapRuleHitCount (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclTrapRuleHitCountValue;
  FPOBJ_TRACE_ENTER (bufp);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclTrapRuleHitCountValue,
                     sizeof (objaclTrapRuleHitCountValue));

  /* return the object value: aclTrapRuleHitCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclTrapRuleHitCountValue,
                           sizeof (objaclTrapRuleHitCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclTrapFlag
*
* @purpose Get 'aclTrapFlag'
*
* @description [aclTrapFlag]: ACL Trap Flag - Enables or disables ACL trap
*              generation. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclTrapFlag (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclTrapFlagValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbQosAclTrapFlagGet (L7_UNIT_CURRENT, &objaclTrapFlagValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclTrapFlagValue, sizeof (objaclTrapFlagValue));

  /* return the object value: aclTrapFlag */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclTrapFlagValue,
                           sizeof (objaclTrapFlagValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclTrapFlag
*
* @purpose Set 'aclTrapFlag'
*
* @description [aclTrapFlag]: ACL Trap Flag - Enables or disables ACL trap
*              generation. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclTrapFlag (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclTrapFlagValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclTrapFlag */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objaclTrapFlagValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclTrapFlagValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbQosAclTrapFlagSet (L7_UNIT_CURRENT, objaclTrapFlagValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/* Changes related to vlan based ACLs */
L7_RC_t
fpObjUtil_QosAclVlanCheckValid(L7_uint32 UnitIndex, L7_uint32 aclVlanIndex, L7_uint32 aclVlanDirection,
                         L7_uint32 aclVlanSequence, L7_uint32 aclVlanAclType, L7_uint32 aclVlanAclId)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 count = 0;
  L7_ACL_VLAN_DIR_LIST_t listInfo;

  L7_uint32 temp_direction = 3; /* Initializing with a wrong direction value */


  if((aclVlanDirection != L7_INBOUND_ACL) && (aclVlanDirection != L7_OUTBOUND_ACL))
  {
      rc = L7_FAILURE;
  }
  else
  {
    temp_direction = aclVlanDirection;
  }

  rc = usmDbQosAclVlanDirAclListGet(UnitIndex,aclVlanIndex,temp_direction,&listInfo);

  if(rc == L7_SUCCESS)
    for(count = 0;count < listInfo.count;count++)
 {
      rc = L7_FAILURE;
      if(listInfo.listEntry[count].seqNum == aclVlanSequence)
      {
        rc = L7_SUCCESS;
        break;
      }
    }
    if((rc == L7_SUCCESS) && (listInfo.listEntry[count].aclType == aclVlanAclType)
                        && (listInfo.listEntry[count].aclId == aclVlanAclId))
  {
    rc = L7_SUCCESS;
  }
  else
    rc = L7_FAILURE;

  return rc;
}

L7_RC_t fpobjUtil_aclIndex_aclRuleIndex_GetNext(L7_uint32 unitNum,L7_uint32 objaclIndexValue,L7_uint32 objaclRuleIndexValue,L7_uint32 *nextObjaclIndexValue,L7_uint32 *nextObjaclRuleIndexValue)
{
  L7_RC_t rc = L7_FAILURE;
  if(usmDbQosAclRuleGetNext(unitNum,objaclIndexValue,objaclRuleIndexValue,nextObjaclRuleIndexValue) == L7_SUCCESS)
  {
     *nextObjaclIndexValue = objaclIndexValue;
     rc = L7_SUCCESS;
  }
  else
  {
    if(usmDbQosAclNumGetNext(unitNum,objaclIndexValue,nextObjaclIndexValue) == L7_SUCCESS)
    {
       if(usmDbQosAclRuleGetFirst(unitNum,*nextObjaclIndexValue,nextObjaclRuleIndexValue) ==  L7_SUCCESS)
       {
         rc = L7_SUCCESS;
       }

    }
  }
  return rc;
}

L7_RC_t fpObjUtil_aclMacIndex_aclMacRuleIndex_GetNext(L7_uint32 unitNum,
                                                      L7_uint32 objaclMacIndexValue,
                                                      L7_uint32 objaclMacRuleIndexValue,
                                                      L7_uint32 *nextObjaclMacIndexValue,
                                                      L7_uint32 *nextObjaclMacRuleIndexValue)
{
  L7_RC_t rc = L7_FAILURE;
  if(usmDbQosAclMacRuleGetNext(unitNum,objaclMacIndexValue,objaclMacRuleIndexValue,nextObjaclMacRuleIndexValue) == L7_SUCCESS)
  {
    *nextObjaclMacIndexValue = objaclMacIndexValue;
    rc = L7_SUCCESS;
 }
  else
  {
    if(usmDbQosAclMacIndexGetNext(unitNum,objaclMacIndexValue,nextObjaclMacIndexValue) == L7_SUCCESS)
    {
       if(usmDbQosAclMacRuleGetFirst(unitNum,*nextObjaclMacIndexValue,nextObjaclMacRuleIndexValue) == L7_SUCCESS)
       {
          rc = L7_SUCCESS;
       }

    }
  }
  return rc;
}

L7_RC_t fpObjGet_aclIpv6Index_aclIpv6RuleIndex_GetNext(L7_uint32 unitNum,L7_uint32 objaclIpv6IndexValue,
                                                       L7_uint32 objaclIpv6RuleIndexValue,
                                                       L7_uint32 *nextObjaclIpv6IndexValue,
                                                       L7_uint32 *nextObjaclIpv6RuleIndexValue)
{

  L7_RC_t rc = L7_FAILURE;
  if(usmDbQosAclRuleGetNext(unitNum,objaclIpv6IndexValue,objaclIpv6RuleIndexValue,nextObjaclIpv6RuleIndexValue) == L7_SUCCESS)
  {
     *nextObjaclIpv6IndexValue = objaclIpv6RuleIndexValue;
     rc = L7_SUCCESS;
  }
  else
  {
    if(usmDbQosAclNamedIndexGetNext(unitNum,L7_ACL_TYPE_IPV6,objaclIpv6IndexValue,nextObjaclIpv6IndexValue) == L7_SUCCESS)
    {
       if(usmDbQosAclRuleGetFirst(L7_UNIT_CURRENT,*nextObjaclIpv6IndexValue,nextObjaclIpv6RuleIndexValue) ==  L7_SUCCESS)
     {
         rc = L7_SUCCESS;
       }

    }
  }
  return rc;
}

L7_RC_t fpobj_AclVlanIDGetNewVlanId(L7_uint32 vlanId, L7_uint32 *aclVlanIndex)
{
   if(vlanId >=L7_DOT1Q_DEFAULT && vlanId <= L7_MAX_VLAN_ID)
   {
      *aclVlanIndex = vlanId;
      return L7_SUCCESS;
   }

   return L7_FAILURE;
}


L7_RC_t fpobj_AclVlanIDCheck(L7_uint32 aclVlanIndex)
{
   if(aclVlanIndex >= L7_DOT1Q_DEFAULT && aclVlanIndex <= L7_MAX_VLAN_ID)
   {
     return L7_SUCCESS;
   }
   return L7_FAILURE;
}

L7_RC_t fpobj_AclVlanGetNext(L7_uint32 aclVlanIndex, L7_uint32 *aclVlanIndexNext)
{
   if(aclVlanIndex >= L7_DOT1Q_DEFAULT-1  && aclVlanIndex < L7_MAX_VLAN_ID)
   {
     *aclVlanIndexNext = aclVlanIndex+1;
  return L7_SUCCESS;
   }
   return L7_FAILURE;
}

L7_RC_t fpobjUtil_QosAclIntfNext(L7_uint32 UnitIndex, L7_uint32 *aclIfIndex, L7_uint32 *aclIfDirection,
                   L7_uint32 *aclIfSequence, L7_uint32 *aclIfAclType, L7_uint32 *aclIfAclId)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 count = 0;

  L7_ACL_INTF_DIR_LIST_t listInfo;

  L7_uint32 nextAclIfIndex,nextAclIfDirection,nextAclIfSequence;

  L7_uint32 temp_direction;



  if((*aclIfDirection !=  L7_INBOUND_ACL) && (*aclIfDirection == L7_OUTBOUND_ACL))
  {
     temp_direction = (*aclIfDirection)-1;
  }
  else
  {
     temp_direction = *aclIfDirection;
  }

  while(1)
  {
     rc = usmDbQosAclIntfDirSequenceGetNext(UnitIndex,*aclIfIndex,temp_direction,
                                             *aclIfSequence,
                                             &nextAclIfSequence);
  if(rc == L7_SUCCESS)
    {
      *aclIfSequence = nextAclIfSequence;
      break;
    }

    if (rc != L7_SUCCESS)
    {
      /* Trying to get next (interface + direction) in use */
      rc = usmDbQosAclIntfDirGetNext(UnitIndex,
                                     *aclIfIndex,
                                     temp_direction,
                                     &nextAclIfIndex,
                                     &nextAclIfDirection);

      if(rc != L7_SUCCESS)
      {
        break;
      }

      else if( rc == L7_SUCCESS && (usmDbQosAclIsInterfaceInUse(UnitIndex,
                                                       nextAclIfIndex,
                                                       nextAclIfDirection)) == L7_TRUE)
      {
          *aclIfIndex = nextAclIfIndex;
          temp_direction = nextAclIfDirection;
          *aclIfSequence = 0;
          rc = L7_SUCCESS;
      }

      else if( rc == L7_SUCCESS && (usmDbQosAclIsInterfaceInUse(UnitIndex,
                                                    nextAclIfIndex,
                                                       nextAclIfDirection)) != L7_TRUE)
      {
  *aclIfIndex = nextAclIfIndex;
          temp_direction = nextAclIfDirection;
          *aclIfSequence = 0;
          rc = L7_FAILURE;
          continue;
      }


      if (rc == L7_SUCCESS)
      {
        /* Get the first sequence for this interface and direction with input value of
           *aclIfSequence = 0 */
        rc = usmDbQosAclIntfDirSequenceGetNext(UnitIndex,nextAclIfIndex,nextAclIfDirection,
                                               *aclIfSequence,
                                               &nextAclIfSequence);

        if((rc == L7_SUCCESS) || (rc == L7_ERROR))      
        {
          *aclIfIndex = nextAclIfIndex;
          temp_direction = nextAclIfDirection;
          *aclIfSequence = nextAclIfSequence;
          break;
        }

      }
    } /* end of if (rc != L7_SUCCESS) */

  } /* end of while(1) */


  /* Once got the values of next intfIndex, Direction and sequence, now need to get
  next aclType and aclId. For a pair of (intfIndex+ Direction), there would be only one
     combination of (aclType + aclId) for a particular value of aclIfSequence. */
  if(rc == L7_SUCCESS)
 rc = usmDbQosAclIntfDirAclListGet( UnitIndex,*aclIfIndex,temp_direction,&listInfo);

  if(rc == L7_SUCCESS)
    for(count = 0;count < listInfo.count;count++)
    {
      rc = L7_FAILURE;
      if(listInfo.listEntry[count].seqNum == *aclIfSequence)
      {
        rc = L7_SUCCESS;
        break;
      }
    }

  if(rc == L7_SUCCESS)
  {
    if((temp_direction != L7_INBOUND_ACL) &&(temp_direction != L7_OUTBOUND_ACL))
    {
       rc = L7_FAILURE;
    }
    else
    {
      *aclIfDirection = temp_direction;
    }

    *aclIfAclType = listInfo.listEntry[count].aclType;
    *aclIfAclId =   listInfo.listEntry[count].aclId;
  }

  return rc;
}


L7_RC_t fpobj_QosAclIntfAdd(L7_uint32 UnitIndex, L7_uint32 aclIfIndex, L7_uint32 aclIfDirection,
                  L7_uint32 aclIfSequence, L7_uint32 aclIfAclType, L7_uint32 aclIfAclId)
{
  L7_RC_t rc = L7_SUCCESS;


  if((aclIfDirection != L7_INBOUND_ACL) && (aclIfDirection != L7_OUTBOUND_ACL))
  {
    rc = L7_FAILURE;
  }


  if (rc == L7_SUCCESS && aclIfAclType == L7_ACL_TYPE_IP)
    rc = usmDbQosAclInterfaceDirectionAdd(UnitIndex, aclIfIndex,
                                          aclIfDirection, aclIfAclId, aclIfSequence);

  if (rc == L7_SUCCESS && aclIfAclType == L7_ACL_TYPE_IPV6)
    rc = usmDbQosAclInterfaceDirectionAdd(UnitIndex, aclIfIndex,
                                          aclIfDirection, aclIfAclId, aclIfSequence);

  if (rc == L7_SUCCESS && aclIfAclType == L7_ACL_TYPE_MAC)
    rc = usmDbQosAclMacInterfaceDirectionAdd(UnitIndex, aclIfIndex,
                                          aclIfDirection, aclIfAclId, aclIfSequence);

  return rc;
}

L7_RC_t fpobj_QosAclIntfRemove(L7_uint32 UnitIndex, L7_uint32 aclIfIndex, L7_uint32 aclIfDirection,
                        L7_uint32 aclIfSequence, L7_uint32 aclIfAclType, L7_uint32 aclIfAclId)
{
  L7_RC_t rc = L7_SUCCESS;

  if((aclIfDirection != L7_INBOUND_ACL) && (aclIfDirection != L7_OUTBOUND_ACL))
  {
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS && aclIfAclType == L7_ACL_TYPE_IP)
  {
    rc = usmDbQosAclInterfaceDirectionRemove(UnitIndex, aclIfIndex,
                                             aclIfDirection, aclIfAclId);
  }

  if (rc == L7_SUCCESS && aclIfAclType == L7_ACL_TYPE_IPV6)
  {
    rc = usmDbQosAclInterfaceDirectionRemove(UnitIndex, aclIfIndex,aclIfDirection, aclIfAclId);
  }

  if (rc == L7_SUCCESS && aclIfAclType == L7_ACL_TYPE_MAC)
  {

    rc = usmDbQosAclMacInterfaceDirectionRemove(UnitIndex, aclIfIndex,
                                          aclIfDirection, aclIfAclId);
  }

  return rc;
}
/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclIdSummary
*
* @purpose Get 'aclIdSummary'
 *@description  [aclIdSummary] aclIdSummary.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclIdSummary (void *wap, void *bufp)
{
 return  XLIBRC_SUCCESS;
}

/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclRuleCountSummary
*
* @purpose Get 'aclRuleCountSummary'
 *@description  [aclRuleCountSummary] aclRuleCountSummary.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclRuleCountSummary (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objaclRuleCountSummaryValue = 0;
  xLibU32_t ruleId;
  xLibBool_t flag = XLIB_FALSE;

  xLibU32_t keyaclIdSummaryValue;
  

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIdSummary */
  owa.len = sizeof (keyaclIdSummaryValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIdSummary,
                          (xLibU8_t *) & keyaclIdSummaryValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIdSummaryValue, owa.len);
  /* get the value from application */
  owa.l7rc = usmDbQosAclMacIndexCheckValid(L7_UNIT_CURRENT,keyaclIdSummaryValue);
  if(owa.l7rc == L7_SUCCESS)
  {
      flag = XLIB_TRUE;
       owa.l7rc = usmDbQosAclMacRuleGetFirst(L7_UNIT_CURRENT, keyaclIdSummaryValue, &ruleId);
       while (owa.l7rc == L7_SUCCESS)
      {
            objaclRuleCountSummaryValue++;
            owa.l7rc = usmDbQosAclMacRuleGetNext(L7_UNIT_CURRENT,keyaclIdSummaryValue, ruleId, &ruleId);
      }
  }
  if(flag == XLIB_FALSE)
  {
    owa.l7rc = usmDbQosAclNumCheckValid(L7_UNIT_CURRENT, keyaclIdSummaryValue);
    if(owa.l7rc == L7_SUCCESS)
    {
          flag = XLIB_TRUE;
          owa.l7rc = usmDbQosAclRuleGetFirst(L7_UNIT_CURRENT,keyaclIdSummaryValue,&ruleId);
          while (owa.l7rc == L7_SUCCESS)
          {
             objaclRuleCountSummaryValue++;
             owa.l7rc = usmDbQosAclRuleGetNext(L7_UNIT_CURRENT,keyaclIdSummaryValue, ruleId, &ruleId);
          }
    }        
  } 
  
  /* TODO CODE shall be added for IPv6 ACL to get the number of rules count */ 
  FPOBJ_TRACE_VALUE (bufp, &objaclRuleCountSummaryValue, sizeof (objaclRuleCountSummaryValue));

  /* return the object value: aclRuleCountSummary */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclRuleCountSummaryValue,
                           sizeof (objaclRuleCountSummaryValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclIdIfDirectionSummary
*
* @purpose Get 'aclIdIfDirectionSummary'
 *@description  [aclIdIfDirectionSummary] The interface direction to which this
* ACL instance applies.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclIdIfDirectionSummary (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objaclIdIfDirectionSummaryValue;
  xLibU32_t nextObjaclIdIfDirectionSummaryValue;
  xLibU32_t keyaclIdSummaryValue;
  L7_ACL_DIRECTION_t dir;
  L7_ACL_ASSIGNED_INTF_LIST_t intfList[L7_ACL_DIRECTION_TOTAL]={};
  L7_ACL_ASSIGNED_VLAN_LIST_t vlanList[L7_ACL_DIRECTION_TOTAL]={};

  FPOBJ_TRACE_ENTER (bufp);
  /* retrieve key: aclIdSummary */
  owa.len = sizeof (keyaclIdSummaryValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIdSummary,
                          (xLibU8_t *) & keyaclIdSummaryValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.len = sizeof (objaclIdIfDirectionSummaryValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIdIfDirectionSummary,
                          (xLibU8_t *) & objaclIdIfDirectionSummaryValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    for(dir=L7_INBOUND_ACL; dir <L7_ACL_DIRECTION_TOTAL ; dir++)
    {
      if((usmDbQosAclAssignedIntfDirListGet(L7_UNIT_CURRENT, keyaclIdSummaryValue, dir, &intfList[dir]) == L7_SUCCESS)||
          (usmDbQosAclAssignedVlanDirListGet(L7_UNIT_CURRENT, keyaclIdSummaryValue, dir, &vlanList[dir]) == L7_SUCCESS) ||
           (usmDbQosAclMacAssignedIntfDirListGet(L7_UNIT_CURRENT, keyaclIdSummaryValue, dir, &intfList[dir]) == L7_SUCCESS) ||
           (usmDbQosAclMacAssignedVlanDirListGet(L7_UNIT_CURRENT, keyaclIdSummaryValue, dir, &vlanList[dir]) == L7_SUCCESS))
      {
        break;
      }
    }
  }
  else
  {
    for(dir=objaclIdIfDirectionSummaryValue+1; dir <L7_ACL_DIRECTION_TOTAL ; dir++)
    {
      if((usmDbQosAclAssignedIntfDirListGet(L7_UNIT_CURRENT, keyaclIdSummaryValue, dir, &intfList[dir]) == L7_SUCCESS)||
          (usmDbQosAclAssignedVlanDirListGet(L7_UNIT_CURRENT, keyaclIdSummaryValue, dir, &vlanList[dir]) == L7_SUCCESS) ||
           (usmDbQosAclMacAssignedIntfDirListGet(L7_UNIT_CURRENT, keyaclIdSummaryValue, dir, &intfList[dir]) == L7_SUCCESS) ||
           (usmDbQosAclMacAssignedVlanDirListGet(L7_UNIT_CURRENT, keyaclIdSummaryValue, dir, &vlanList[dir]) == L7_SUCCESS))
      {
        break;
      }
    }
    if(dir >= L7_ACL_DIRECTION_TOTAL)
    {
      owa.rc = XLIBRC_ENDOF_TABLE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }

  nextObjaclIdIfDirectionSummaryValue = dir;
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjaclIdIfDirectionSummaryValue, owa.len);

  /* return the object value: aclIdIfDirectionSummary */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjaclIdIfDirectionSummaryValue,
                           sizeof (nextObjaclIdIfDirectionSummaryValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclMacInterfaceListSummary
*
* @purpose Get 'aclMacInterfaceListSummary'
 *@description  [aclMacInterfaceListSummary] The interface to which this  MAC
* ACL instance applies.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclMacInterfaceListSummary (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objaclMacInterfaceListSummaryValue,stat;

  xLibU32_t keyaclIdSummaryValue;
  xLibU32_t keyaclIdIfDirectionSummaryValue;
  xLibU32_t flag,i;
  L7_ACL_ASSIGNED_INTF_LIST_t intfList;
  flag = 1;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIdSummary */
  owa.len = sizeof (keyaclIdSummaryValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIdSummary,
                          (xLibU8_t *) & keyaclIdSummaryValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
 memset (objaclMacInterfaceListSummaryValue,0,sizeof(objaclMacInterfaceListSummaryValue));
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIdSummaryValue, owa.len);

  /* retrieve key: aclIdIfDirectionSummary */
  owa.len = sizeof (keyaclIdIfDirectionSummaryValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIdIfDirectionSummary,
                          (xLibU8_t *) & keyaclIdIfDirectionSummaryValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIdIfDirectionSummaryValue, owa.len);

  
  /* get the value from applicaton */

  owa.l7rc =usmDbQosAclMacAssignedIntfDirListGet(1, keyaclIdSummaryValue, keyaclIdIfDirectionSummaryValue, &intfList);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  if(intfList.count != 0)
  {
     for (i = 0; i < intfList.count; i++)
     {
        memset(stat, 0x0, sizeof(stat));
        osapiSnprintf(stat, sizeof(stat),"%d",intfList.intIfNum[i]);
         if(flag == 1)
        {
          OSAPI_STRNCAT(objaclMacInterfaceListSummaryValue,stat);
          flag =0;
        }
        else
        {
          OSAPI_STRNCAT(objaclMacInterfaceListSummaryValue,",");
          OSAPI_STRNCAT(objaclMacInterfaceListSummaryValue, stat);
        }
    }    
  }
        
  FPOBJ_TRACE_VALUE (bufp, objaclMacInterfaceListSummaryValue,
                     sizeof (objaclMacInterfaceListSummaryValue));

  /* return the object value: aclMacInterfaceListSummary */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)  objaclMacInterfaceListSummaryValue,
                           strlen (objaclMacInterfaceListSummaryValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclMacVlanListSummary
*
* @purpose Get 'aclMacVlanListSummary'
 *@description  [aclMacVlanListSummary] The Vlan to which this MAC ACL instance
* applies.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclMacVlanListSummary (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objaclMacVlanListSummaryValue,stat;
  xLibU32_t flag =1,i;
  xLibU32_t keyaclIdSummaryValue;
  xLibU32_t keyaclIdIfDirectionSummaryValue;
  L7_ACL_ASSIGNED_VLAN_LIST_t vlanList;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIdSummary */
  owa.len = sizeof (keyaclIdSummaryValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIdSummary,
                          (xLibU8_t *) & keyaclIdSummaryValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIdSummaryValue, owa.len);

  memset (objaclMacVlanListSummaryValue,0,sizeof(objaclMacVlanListSummaryValue));
  /* retrieve key: aclIdIfDirectionSummary */
  owa.len = sizeof (keyaclIdIfDirectionSummaryValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIdIfDirectionSummary,
                          (xLibU8_t *) & keyaclIdIfDirectionSummaryValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIdIfDirectionSummaryValue, owa.len);

  /* get the value from application */
  if(usmDbQosAclMacAssignedVlanDirListGet(1, keyaclIdSummaryValue, keyaclIdIfDirectionSummaryValue, &vlanList) == L7_SUCCESS)
  {
    if(vlanList.count != 0)
    {
      for (i = 0; i < vlanList.count; i++)
      {
         memset(stat, 0x0, sizeof(stat));
         osapiSnprintf(stat, sizeof(stat),"%d",vlanList.vlanNum[i]);
         if(flag == 1)
         {
          OSAPI_STRNCAT(objaclMacVlanListSummaryValue,stat);
          flag =0;
         }
         else
         {
           OSAPI_STRNCAT(objaclMacVlanListSummaryValue,",");
           OSAPI_STRNCAT(objaclMacVlanListSummaryValue, stat);
         }
      }
    }

  }
  else     
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objaclMacVlanListSummaryValue, sizeof (objaclMacVlanListSummaryValue));

  /* return the object value: aclMacVlanListSummary */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)  objaclMacVlanListSummaryValue,
                           strlen(objaclMacVlanListSummaryValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclIPInterfaceListSummary
*
* @purpose Get 'aclIPInterfaceListSummary'
 *@description  [aclIPInterfaceListSummary] The interface to which this ACL
* instance applies.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclIPInterfaceListSummary (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objaclIPInterfaceListSummaryValue,stat;

  xLibU32_t keyaclIdSummaryValue;
  xLibU32_t keyaclIdIfDirectionSummaryValue;
  
  xLibU32_t flag,i;
  L7_ACL_ASSIGNED_INTF_LIST_t intfList;
  FPOBJ_TRACE_ENTER (bufp);
  flag = 1;
 
  memset(objaclIPInterfaceListSummaryValue, 0,sizeof(objaclIPInterfaceListSummaryValue));
  /* retrieve key: aclIdSummary */
  owa.len = sizeof (keyaclIdSummaryValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIdSummary,
                          (xLibU8_t *) & keyaclIdSummaryValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIdSummaryValue, owa.len);

  /* retrieve key: aclIdIfDirectionSummary */
  owa.len = sizeof (keyaclIdIfDirectionSummaryValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIdIfDirectionSummary,
                          (xLibU8_t *) & keyaclIdIfDirectionSummaryValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIdIfDirectionSummaryValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbQosAclAssignedIntfDirListGet (L7_UNIT_CURRENT, keyaclIdSummaryValue,
                              keyaclIdIfDirectionSummaryValue, & intfList);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  if(intfList.count != 0)
  {
     for (i = 0; i < intfList.count; i++)
     {
        memset(stat, 0, sizeof(stat));
        osapiSnprintf(stat,sizeof(stat),"%d",intfList.intIfNum[i]);
         if(flag == 1)
        {
          OSAPI_STRNCAT(objaclIPInterfaceListSummaryValue,stat);
          flag =0;
        }
        else
        {
          OSAPI_STRNCAT(objaclIPInterfaceListSummaryValue,",");
          OSAPI_STRNCAT(objaclIPInterfaceListSummaryValue, stat);
        }
    }
  }

  FPOBJ_TRACE_VALUE (bufp, objaclIPInterfaceListSummaryValue,
                     strlen (objaclIPInterfaceListSummaryValue));

  /* return the object value: aclIPInterfaceListSummary */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objaclIPInterfaceListSummaryValue,
                           strlen(objaclIPInterfaceListSummaryValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclIPVlanListSummary
*
* @purpose Get 'aclIPVlanListSummary'
 *@description  [aclIPVlanListSummary] The interface to which this ACL instance
* applies.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclIPVlanListSummary (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objaclIPVlanListSummaryValue,stat;
  xLibU32_t flag =1,i;
  xLibU32_t keyaclIdSummaryValue;
  xLibU32_t keyaclIdIfDirectionSummaryValue;
  L7_ACL_ASSIGNED_VLAN_LIST_t vlanList;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIdSummary */
  owa.len = sizeof (keyaclIdSummaryValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIdSummary,
                          (xLibU8_t *) & keyaclIdSummaryValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
 memset(objaclIPVlanListSummaryValue,0,sizeof(objaclIPVlanListSummaryValue));
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIdSummaryValue, owa.len);

  /* retrieve key: aclIdIfDirectionSummary */
  owa.len = sizeof (keyaclIdIfDirectionSummaryValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIdIfDirectionSummary,
                          (xLibU8_t *) & keyaclIdIfDirectionSummaryValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIdIfDirectionSummaryValue, owa.len);

  /* get the value from application */
  if((owa.l7rc=usmDbQosAclAssignedVlanDirListGet(L7_UNIT_CURRENT, keyaclIdSummaryValue,
                              keyaclIdIfDirectionSummaryValue, &vlanList)) == L7_SUCCESS)
  {
    if(vlanList.count != 0)
    {
      for (i = 0; i < vlanList.count; i++)
      {
         memset(stat, 0x0, sizeof(stat));
         osapiSnprintf(stat, sizeof(stat),"%d",vlanList.vlanNum[i]);
         if(flag == 1)
         {
          OSAPI_STRNCAT(objaclIPVlanListSummaryValue,stat);
          flag =0;
         }
         else
         {
           OSAPI_STRNCAT(objaclIPVlanListSummaryValue,",");
           OSAPI_STRNCAT(objaclIPVlanListSummaryValue, stat);
         }
      }
    }

  }
  else 
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  

  FPOBJ_TRACE_VALUE (bufp, objaclIPVlanListSummaryValue, strlen(objaclIPVlanListSummaryValue));
  /* return the object value: aclIPVlanListSummary */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclIPVlanListSummaryValue,
                           strlen(objaclIPVlanListSummaryValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclIPv6InterfaceListSummary
*
* @purpose Get 'aclIPv6InterfaceListSummary'
 *@description  [aclIPv6InterfaceListSummary] The interface to which this ACL
* instance applies.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclIPv6InterfaceListSummary (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objaclIPv6InterfaceListSummaryValue, stat;

  xLibU32_t keyaclIdSummaryValue;
  xLibU32_t keyaclIdIfDirectionSummaryValue;

  xLibU32_t flag,i;
  L7_ACL_ASSIGNED_INTF_LIST_t intfList;
  FPOBJ_TRACE_ENTER (bufp);
  flag = L7_TRUE;

  memset(objaclIPv6InterfaceListSummaryValue, 0,sizeof(objaclIPv6InterfaceListSummaryValue));
  /* retrieve key: aclIdSummary */
  owa.len = sizeof (keyaclIdSummaryValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIdSummary,
                          (xLibU8_t *) & keyaclIdSummaryValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIdSummaryValue, owa.len);

  /* retrieve key: aclIdIfDirectionSummary */
  owa.len = sizeof (keyaclIdIfDirectionSummaryValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIdIfDirectionSummary,
                          (xLibU8_t *) & keyaclIdIfDirectionSummaryValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIdIfDirectionSummaryValue, owa.len);


  /* get the value from application */
  owa.l7rc = usmDbQosAclAssignedIntfDirListGet (L7_UNIT_CURRENT, keyaclIdSummaryValue,
                              keyaclIdIfDirectionSummaryValue, &intfList);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(intfList.count != 0)
  {
     for (i = 0; i < intfList.count; i++)
     {
        memset(stat, 0, sizeof(stat));
        osapiSnprintf(stat, sizeof(stat), "%d",intfList.intIfNum[i]);
         if(flag == L7_TRUE)
        {
          OSAPI_STRNCAT(objaclIPv6InterfaceListSummaryValue,stat);
          flag =L7_FALSE;
        }
        else
        {
          OSAPI_STRNCAT(objaclIPv6InterfaceListSummaryValue,",");
          OSAPI_STRNCAT(objaclIPv6InterfaceListSummaryValue, stat);
        }
    }
  }

  FPOBJ_TRACE_VALUE (bufp, &objaclIPv6InterfaceListSummaryValue,
                     strlen (objaclIPv6InterfaceListSummaryValue));

  /* return the object value: aclIPv6InterfaceListSummary */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclIPv6InterfaceListSummaryValue,
                           strlen (objaclIPv6InterfaceListSummaryValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclIPv6VlanListSummary
*
* @purpose Get 'aclIPv6VlanListSummary'
 *@description  [aclIPv6VlanListSummary] The interface to which this ACL
* instance applies.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclIPv6VlanListSummary (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objaclIPv6VlanListSummaryValue,stat;

  xLibU32_t flag = L7_TRUE, i;
  xLibU32_t keyaclIdSummaryValue;
  xLibU32_t keyaclIdIfDirectionSummaryValue;
  L7_ACL_ASSIGNED_VLAN_LIST_t vlanList;


  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIdSummary */
  owa.len = sizeof (keyaclIdSummaryValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIdSummary,
                          (xLibU8_t *) & keyaclIdSummaryValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIdSummaryValue, owa.len);

  memset (objaclIPv6VlanListSummaryValue,0,sizeof(objaclIPv6VlanListSummaryValue));
  /* retrieve key: aclIdIfDirectionSummary */
  owa.len = sizeof (keyaclIdIfDirectionSummaryValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIdIfDirectionSummary,
                          (xLibU8_t *) & keyaclIdIfDirectionSummaryValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIdIfDirectionSummaryValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbQosAclAssignedVlanDirListGet (L7_UNIT_CURRENT, keyaclIdSummaryValue,
                              keyaclIdIfDirectionSummaryValue, &vlanList);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(vlanList.count != 0)
    {
    for (i = 0; i < vlanList.count; i++)
      {
         memset(stat, 0x0, sizeof(stat));
         osapiSnprintf(stat, sizeof(stat), "%d",vlanList.vlanNum[i]);
         if(flag == L7_TRUE)
         {
          OSAPI_STRNCAT(objaclIPv6VlanListSummaryValue,stat);
          flag = L7_FALSE;
         }
         else
         {
           OSAPI_STRNCAT(objaclIPv6VlanListSummaryValue,",");
           OSAPI_STRNCAT(objaclIPv6VlanListSummaryValue, stat);
         }
      }
    }

  FPOBJ_TRACE_VALUE (bufp, objaclIPv6VlanListSummaryValue,
                     strlen (objaclIPv6VlanListSummaryValue));

  /* return the object value: aclIPv6VlanListSummary */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objaclIPv6VlanListSummaryValue,
                           strlen (objaclIPv6VlanListSummaryValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclTypeLabel (void *wap, void *bufp)
{
return XLIBRC_SUCCESS;
}
/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclIPNamedIndex
*
* @purpose Get 'aclIPNamedIndex'
 *@description  [aclIPNamedIndex] object to combine all IP Standard Extended and
* Named ACL's
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclIPNamedIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objaclIPNamedIndexValue;

  xLibU32_t keyaclIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIndex */
  owa.len = sizeof (keyaclIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                          (xLibU8_t *) & keyaclIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, owa.len);

  /* retrieve key: aclIPNamedIndex */
  /* check to see that if acl ID is valid Named ACL ID */
  if (usmDbQosAclNamedIndexCheckValid(L7_UNIT_CURRENT, L7_ACL_TYPE_IP, keyaclIndexValue) == L7_SUCCESS)
  {
     owa.l7rc= usmDbQosAclNameGet(L7_UNIT_CURRENT, keyaclIndexValue, objaclIPNamedIndexValue);
  }
  /* acl is not Named ACL hence convert the acl Number from integer  and assign it to */
  else
  {
     memset(objaclIPNamedIndexValue,0,sizeof(objaclIPNamedIndexValue));
     osapiSnprintf(objaclIPNamedIndexValue,sizeof(objaclIPNamedIndexValue),"%u",keyaclIndexValue);  
     owa.l7rc = L7_SUCCESS;
  }
    
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, objaclIPNamedIndexValue, owa.len);

  /* return the object value: aclIPNamedIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objaclIPNamedIndexValue,
                           strlen (objaclIPNamedIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclIPNamedType
*
* @purpose Get 'aclIPNamedType'
 *@description  [aclIPNamedIndex] Object to set the type of IP ACL's
* Named ACL's
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclIPNamedType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objaclIPNamedTypeValue;

  xLibU32_t keyaclIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIndex */
  owa.len = sizeof (keyaclIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                          (xLibU8_t *) & keyaclIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {    
    owa.rc = XLIBRC_SUCCESS;
    /* special case  no index hence return none*/
    objaclIPNamedTypeValue = L7_ACL_TYPE_IP_NONE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, owa.len);

  /* retrieve key: aclIPNamedIndex */
  /* check to see that if acl ID is valid Named ACL ID */
  if (usmDbQosAclNamedIndexCheckValid(L7_UNIT_CURRENT, L7_ACL_TYPE_IP, keyaclIndexValue) == L7_SUCCESS)
  {
     objaclIPNamedTypeValue = L7_ACL_TYPE_IP_NAME;
  }
  else
  {
       if((keyaclIndexValue >= L7_ACL_MIN_STD1_ID) && (keyaclIndexValue <=L7_ACL_MAX_STD1_ID))
        {
          objaclIPNamedTypeValue = L7_ACL_TYPE_IP_STD;   
        }
        else if((keyaclIndexValue >= L7_ACL_MIN_EXT1_ID) && (keyaclIndexValue <=L7_ACL_MAX_EXT1_ID))
        {
          objaclIPNamedTypeValue = L7_ACL_TYPE_IP_EXT;
        }
        else
        {
           objaclIPNamedTypeValue = L7_ACL_TYPE_IP_NONE; 
        }

  }
  FPOBJ_TRACE_NEW_KEY (bufp, &objaclIPNamedTypeValue, owa.len);

  /* return the object value: aclIPNamedType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objaclIPNamedTypeValue,
                           sizeof(objaclIPNamedTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclIPNamedIndex
*
* @purpose Set 'aclIPNamedIndex'
 *@description  [aclIPNamedIndex] object to combine all IP Standard Extended and
* Named ACL's
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclIPNamedIndex (void *wap, void *bufp)
{  
  /* Rename the Named ACL's for all other IP ACL's return Success */ 
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objaclIPNamedIndexValue;
  L7_BOOL flag;
  xLibU32_t keyaclIndexValue;
  
  FPOBJ_TRACE_ENTER (bufp);
/* retrieve object: aclIPNamedIndex */
  owa.len = sizeof (objaclIPNamedIndexValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objaclIPNamedIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objaclIPNamedIndexValue, owa.len);

  /* retrieve key: aclIndex */
  owa.len = sizeof (keyaclIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                          (xLibU8_t *) & keyaclIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
 /* This object renames the IP Named ACL Names */
 FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, owa.len);
  
  /* set the value in application */
  flag = fpObj_isNameACL(objaclIPNamedIndexValue);
  if(flag == L7_TRUE)
  {
      owa.l7rc = usmDbQosAclNameChange(L7_UNIT_CURRENT, keyaclIndexValue, objaclIPNamedIndexValue);
  }
  else
  {
    /* for other IP ACL's return success */
    owa.l7rc = L7_SUCCESS;
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    if(owa.l7rc == L7_ALREADY_CONFIGURED)
    {
      /* Name already exists */
      owa.rc = XLIBRC_ACL_NAME_ALREADY_EXISTS;  /* TODO: Change if required */
    }
    else
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    }
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclIPNamedType(void *wap, void *bufp)
{
   return XLIBRC_SUCCESS;
}
/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclRuleLogging
*
* @purpose Get 'aclRuleLogging'
 *@description  [aclRuleLogging] Flag to indicate that the ACL rule is being
* logged. A hardware count of the number of times this rule is hit is
* reported via the aclTrapRuleLogEvent notification.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclRuleLogging(void *wap, void *bufp)
{
   fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objaclRuleLoggingValue;

  xLibU32_t keyaclIndexValue;
  xLibU32_t keyaclRuleIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIndex */
  owa.len = sizeof (keyaclIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                          (xLibU8_t *) & keyaclIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, owa.len);

  /* retrieve key: aclRuleIndex */
  owa.len = sizeof (keyaclRuleIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                          (xLibU8_t *) & keyaclRuleIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbQosAclRuleLoggingGet (L7_UNIT_CURRENT, keyaclIndexValue,
                              keyaclRuleIndexValue, &objaclRuleLoggingValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objaclRuleLoggingValue, sizeof (objaclRuleLoggingValue));

  /* return the object value: aclRuleLogging */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclRuleLoggingValue,
                           sizeof (objaclRuleLoggingValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclRuleLogging
*
* @purpose Set 'aclRuleLogging'
 *@description  [aclRuleLogging] Flag to indicate that the ACL rule is being
* logged. A hardware count of the number of times this rule is hit is
* reported via the aclTrapRuleLogEvent notification.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclRuleLogging (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objaclRuleLoggingValue;

  xLibU32_t keyaclIndexValue;
  xLibU32_t keyaclRuleIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclRuleLogging */
  owa.len = sizeof (objaclRuleLoggingValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objaclRuleLoggingValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclRuleLoggingValue, owa.len);

  /* retrieve key: aclIndex */
  owa.len = sizeof (keyaclIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                          (xLibU8_t *) & keyaclIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, owa.len);

  /* retrieve key: aclRuleIndex */
  owa.len = sizeof (keyaclRuleIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                          (xLibU8_t *) & keyaclRuleIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, owa.len);

  /* set the ivalue in application */
  owa.l7rc = usmDbQosAclRuleLoggingAllowed(L7_UNIT_CURRENT, keyaclIndexValue, keyaclRuleIndexValue);
    if (owa.l7rc != L7_SUCCESS)
    {
       /* Reset any stray logging flag as it is not supported */
       objaclRuleLoggingValue = L7_FALSE;
    }

  owa.l7rc =  usmDbQosAclRuleLoggingAdd(L7_UNIT_CURRENT, keyaclIndexValue,
                              keyaclRuleIndexValue, objaclRuleLoggingValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclRuleTimeRangeName
*
* @purpose Get 'aclRuleTimeRangeName'
*
* @description [aclRuleTimeRangeName]:Name of time range associated with the IP ACL Rule
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclRuleTimeRangeName(void *wap,
                                                           void *bufp)
{
  xLibU32_t keyaclIndexValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclRuleIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objaclRuleTimeRangeNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIndex */
  kwa1.len = sizeof (keyaclIndexValue);
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                          (xLibU8_t *) & keyaclIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

   FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, owa.len);

  /* retrieve key: aclRuleIndex */
  kwa2.len = sizeof (keyaclRuleIndexValue);
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                          (xLibU8_t *) & keyaclRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, owa.len);


  /* get the value from application */
  owa.l7rc = usmDbQosAclRuleTimeRangeNameGet(L7_UNIT_CURRENT,keyaclIndexValue, keyaclRuleIndexValue, (L7_uchar8 *)objaclRuleTimeRangeNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objaclRuleTimeRangeNameValue, owa.len);
  /* return the object value: aclRuleTimeRangeName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objaclRuleTimeRangeNameValue,
                           strlen (objaclRuleTimeRangeNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclRuleTimeRangeName
*
* @purpose Set 'aclRuleTimeRangeName'
*
* @description [aclRuleTimeRangeName]:Name of time range associated with the IP ACL Rule
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclRuleTimeRangeName (void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objaclRuleTimeRangeNameValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclRuleIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclRuleTimeRangeName */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objaclRuleTimeRangeNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objaclRuleTimeRangeNameValue, owa.len);

  /* retrieve key: aclIndex */
  kwa1.len = sizeof (keyaclIndexValue);
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                          (xLibU8_t *) & keyaclIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, kwa1.len);

  /* retrieve key: aclRuleIndex */
  kwa2.len = sizeof (keyaclRuleIndexValue);
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                          (xLibU8_t *) & keyaclRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbQosAclRuleTimeRangeNameAdd(L7_UNIT_CURRENT, keyaclIndexValue,
                              keyaclRuleIndexValue,
                              (L7_uchar8 *)objaclRuleTimeRangeNameValue);
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
* @function fpObjList_fastPathQOSACLQOSACL_aclRuleTimeRangeName
*
* @purpose Get 'aclRuleTimeRangeName'
*
* @description [aclRuleTimeRangeName]:Name of time range associated with the IP ACL Rule
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_fastPathQOSACLQOSACL_aclRuleTimeRangeName(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
#ifdef L7_TIMERANGES_PACKAGE
  xLibStr256_t timeRangeNameValue;
  xLibStr256_t nextTimeRangeNameValue;
  xLibU32_t timeRangeIndexValue;
  xLibU32_t nextTimeRangeIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof (timeRangeNameValue);
  memset(timeRangeNameValue,0, sizeof(timeRangeNameValue));
  memset(nextTimeRangeNameValue, 0, sizeof(nextTimeRangeNameValue));
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleTimeRangeName,
                          (xLibU8_t *) timeRangeNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    timeRangeIndexValue =  0;
    nextTimeRangeIndexValue = 0;
    owa.l7rc = usmDbTimeRangeIndexFirstGet(L7_UNIT_CURRENT,
                  &nextTimeRangeIndexValue);
    if(owa.l7rc == L7_SUCCESS)
    {
          /* get the value from application */
          owa.l7rc = usmDbTimeRangeNameGet(L7_UNIT_CURRENT,
                                   nextTimeRangeIndexValue,
                                   (L7_uchar8 *) nextTimeRangeNameValue);

    }
  }
  else
  {

      owa.l7rc =usmDbTimeRangeNameToIndex(L7_UNIT_CURRENT, (L7_uchar8 *)timeRangeNameValue, &timeRangeIndexValue);
      if(owa.l7rc == L7_SUCCESS)
      {
          /* get next time range index and name */
          FPOBJ_TRACE_CURRENT_KEY(bufp, &timeRangeIndexValue, owa.len);
          owa.l7rc = usmDbTimeRangeIndexNextGet(L7_UNIT_CURRENT,  timeRangeIndexValue,
                  &nextTimeRangeIndexValue);
          if(owa.l7rc == L7_SUCCESS)
          {
             /* get the value from application */
              owa.l7rc = usmDbTimeRangeNameGet(L7_UNIT_CURRENT,
                                   nextTimeRangeIndexValue,
                                   (L7_uchar8 *) nextTimeRangeNameValue);
          }

      }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextTimeRangeNameValue, owa.len);

  /* return the object value: aclIfDirection */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextTimeRangeNameValue,
                           strlen(nextTimeRangeNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
#endif
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclRuleTimeRangeStatus
*
* @purpose Get 'aclRuleTimeRangeStatus'
* @description  [aclRuleTimeRangeStatus] Flag Indicates the IP ACL Rule Status.
* ACL rule Status is derived from the status of time range associated with ACL.
* If no time range is associated to an ACL rule then status of ACL rule is always active.
* If time range is associated to an ACL rule and time range is non existing in system,
* then ACL rule status is considered to be active
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclRuleTimeRangeStatus(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objaclRuleTimeRangeStatusValue;

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;

  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclRuleIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIndex */
  kwa1.len = sizeof (keyaclIndexValue);
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                          (xLibU8_t *) & keyaclIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, owa.len);

  /* retrieve key: aclRuleIndex */
  kwa2.len = sizeof (keyaclRuleIndexValue);
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                          (xLibU8_t *) & keyaclRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, owa.len);
  /* get the value from application */
  owa.l7rc = usmDbQosAclRuleStatusGet (L7_UNIT_CURRENT, keyaclIndexValue,
                              keyaclRuleIndexValue, &objaclRuleTimeRangeStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objaclRuleTimeRangeStatusValue, sizeof (objaclRuleTimeRangeStatusValue));

  /* return the object value: aclRuleTimeRangeStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objaclRuleTimeRangeStatusValue,
                           sizeof (objaclRuleTimeRangeStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclRuleProtocolKeyWord
*
* @purpose Get 'aclRuleProtocolKeyWord'
 *@description  [aclRuleProtocolKeyWord] icmp - 1 igmp - 2 ip - 4 tcp - 6 udp -
* 17 All values from 1 to 255 are valid.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclRuleProtocolKeyWord (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objaclRuleProtocolKeyWordValue;

  xLibU32_t keyaclIndexValue;
  xLibU32_t keyaclRuleIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIndex */
  owa.len = sizeof (keyaclIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                          (xLibU8_t *) & keyaclIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, owa.len);

  /* retrieve key: aclRuleIndex */
  owa.len = sizeof (keyaclRuleIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                          (xLibU8_t *) & keyaclRuleIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbQosAclRuleProtocolGet(L7_UNIT_CURRENT, keyaclIndexValue,
                              keyaclRuleIndexValue, &objaclRuleProtocolKeyWordValue);
  if ((owa.l7rc == L7_SUCCESS) && (objaclRuleProtocolKeyWordValue!=0 ))
      {
        switch (objaclRuleProtocolKeyWordValue)
        {
          case L7_ACL_PROTOCOL_ICMP:                    
          case L7_ACL_PROTOCOL_IGMP:
          case L7_ACL_PROTOCOL_IP:
          case L7_ACL_PROTOCOL_TCP:
          case L7_ACL_PROTOCOL_UDP:
          break;
          default:
          objaclRuleProtocolKeyWordValue = L7_ACL_PROTOCOL_OTHER;
          break;
        }
      }
  else
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objaclRuleProtocolKeyWordValue,
                     sizeof (objaclRuleProtocolKeyWordValue));

  /* return the object value: aclRuleProtocolKeyWord */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclRuleProtocolKeyWordValue,
                           sizeof (objaclRuleProtocolKeyWordValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclRuleProtocolKeyWord
*
* @purpose Set 'aclRuleProtocolKeyWord'
 *@description  [aclRuleProtocolKeyWord] icmp - 1 igmp - 2 ip - 4 tcp - 6 udp -
* 17 All values from 1 to 255 are valid.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclRuleProtocolKeyWord (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objaclRuleProtocolKeyWordValue;

  xLibU32_t keyaclIndexValue;
  xLibU32_t keyaclRuleIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclRuleProtocolKeyWord */
  owa.len = sizeof (objaclRuleProtocolKeyWordValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objaclRuleProtocolKeyWordValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclRuleProtocolKeyWordValue, owa.len);

  /* retrieve key: aclIndex */
  owa.len = sizeof (keyaclIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                          (xLibU8_t *) & keyaclIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, owa.len);

  /* retrieve key: aclRuleIndex */
  owa.len = sizeof (keyaclRuleIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                          (xLibU8_t *) & keyaclRuleIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, owa.len);

  /* set the value in application */
  if(objaclRuleProtocolKeyWordValue)
  {
       owa.l7rc = usmDbQosAclRuleProtocolAdd (L7_UNIT_CURRENT, keyaclIndexValue,
                              keyaclRuleIndexValue, objaclRuleProtocolKeyWordValue);
  
     if (owa.l7rc != L7_SUCCESS)
     {
         owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
     }
  }
  else
  {
      /* set prtocol number in protocol object */
      owa.rc = XLIBRC_SUCCESS;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclRuleServiceType
*
* @purpose Get 'aclRuleServiceType'
 *@description  [aclRuleServiceType] Specifies Service type IPDSCP, IP
* Precedence and IP TOS
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclRuleServiceType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objaclRuleServiceTypeValue;

  xLibU32_t keyaclIndexValue;
  xLibU32_t keyaclRuleIndexValue;
  L7_BOOL flag;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIndex */
  owa.len = sizeof (keyaclIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                          (xLibU8_t *) & keyaclIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

/* retrieve key: aclRuleIndex */
  owa.len = sizeof (keyaclRuleIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                          (xLibU8_t *) & keyaclRuleIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, owa.len);

  /* get the value from application */
  
  flag = usmDbQosAclIsFieldConfigured(L7_UNIT_CURRENT, keyaclIndexValue,
                              keyaclRuleIndexValue, ACL_IPDSCP);
  if(flag == L7_TRUE)
  {
     objaclRuleServiceTypeValue = L7_ACL_IPDSCP;
  }
  else 
  {
     flag = usmDbQosAclIsFieldConfigured(L7_UNIT_CURRENT, keyaclIndexValue,
                              keyaclRuleIndexValue, ACL_IPPREC);
     if(flag == L7_TRUE)
     {
        objaclRuleServiceTypeValue = L7_ACL_IPPREC;
     }
     else
     {
        flag = usmDbQosAclIsFieldConfigured(L7_UNIT_CURRENT, keyaclIndexValue,
                              keyaclRuleIndexValue, ACL_IPTOS);
        if(flag == L7_TRUE)
        {
            objaclRuleServiceTypeValue = L7_ACL_IPTOS;
        }
     }
  }
  /* if flag  is false set service type to none*/

  if(flag == L7_FALSE)
  {
     objaclRuleServiceTypeValue = L7_ACL_NONE;
  }

  FPOBJ_TRACE_VALUE (bufp, &objaclRuleServiceTypeValue, sizeof (objaclRuleServiceTypeValue));

  /* return the object value: aclRuleServiceType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclRuleServiceTypeValue,
                           sizeof (objaclRuleServiceTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclRuleServiceType
*
* @purpose Set 'aclRuleServiceType'
 *@description  [aclRuleServiceType] Specifies Service type IPDSCP, IP
* Precedence and IP TOS
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclRuleServiceType (void *wap, void *bufp)
{

  return XLIBRC_SUCCESS;
}

/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclRuleServiceTypeVal
*
* @purpose Get 'aclRuleServiceTypeVal'
 *@description  [aclRuleServiceTypeVal] Specifies Service type IPDSCP, IP
* Precedence and IP TOS
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclRuleServiceTypeVal (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objaclRuleServiceTypeValValue,stat,stat2;  
  L7_BOOL flag;
  xLibU32_t keyaclIndexValue;
  xLibU32_t keyaclRuleIndexValue;  
  xLibU32_t  val,val1=0,val2;

  FPOBJ_TRACE_ENTER (bufp);

  memset(objaclRuleServiceTypeValValue,0,sizeof(objaclRuleServiceTypeValValue));
  memset(stat,0,sizeof(stat));
  memset(stat2,0,sizeof(stat2));
  /* retrieve key: aclIndex */
  owa.len = sizeof (keyaclIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                          (xLibU8_t *) & keyaclIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, owa.len);

  /* retrieve key: aclRuleIndex */
  owa.len = sizeof (keyaclRuleIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                          (xLibU8_t *) & keyaclRuleIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, owa.len);
  flag = usmDbQosAclIsFieldConfigured(L7_UNIT_CURRENT, keyaclIndexValue,
                              keyaclRuleIndexValue, ACL_IPDSCP);
  if(flag == L7_TRUE)
  {     
     owa.l7rc = usmDbQosAclRuleIPDscpGet(L7_UNIT_CURRENT, keyaclIndexValue, keyaclRuleIndexValue, &val);
     if(owa.l7rc != L7_SUCCESS)
     {
       owa.rc = XLIBRC_FAILURE;
       FPOBJ_TRACE_EXIT (bufp, owa);
       return owa.rc;
     }

     osapiStrncpySafe( objaclRuleServiceTypeValValue,"IP DSCP:",sizeof(objaclRuleServiceTypeValValue));
     osapiSnprintf(stat,sizeof(stat),"%d",val);
     OSAPI_STRNCAT(objaclRuleServiceTypeValValue,stat);
     memset(stat, 0, sizeof(stat));
     /* If the val is multiple of 8 -- CS */
     if(((val%8) == 0) && (val <= 56))
     {
       osapiSnprintf(stat,sizeof(stat)," (CS-%d)",val/8);
       OSAPI_STRNCAT(objaclRuleServiceTypeValValue,stat);
     }  
     else if((val >= 10) && (val <=38) && (val%2 == 0))
     {
       switch(val)
       {
         case 10: val1=11;
                  break; 
         case 12: val1=12;
                  break; 
         case 14: val1=13;
                  break; 
         case 18: val1=21;
                  break; 
         case 20: val1=22;
                  break; 
         case 22: val1=23;
                  break; 
         case 26: val1=31;
                  break; 
         case 28: val1=32;
                  break; 
         case 30: val1=33;
                  break; 
         case 34: val1=41;
                  break; 
         case 36: val1=42;
                  break; 
         case 38: val1=43;
                  break; 
       }     
       osapiSnprintf(stat,sizeof(stat)," (AF-%d)", val1);
       OSAPI_STRNCAT(objaclRuleServiceTypeValValue,stat);
     }
     else
     {
       if(val == 46)
       {
         osapiSnprintf(stat,sizeof(stat)," (EF)");
         OSAPI_STRNCAT(objaclRuleServiceTypeValValue,stat);
       }
     }
  }
  else
  {
     flag = usmDbQosAclIsFieldConfigured(L7_UNIT_CURRENT, keyaclIndexValue, keyaclRuleIndexValue, ACL_IPPREC);
     if(flag == L7_TRUE)
     {
        owa.l7rc = usmDbQosAclRuleIPPrecedenceGet(L7_UNIT_CURRENT, keyaclIndexValue, keyaclRuleIndexValue,&val);
        osapiStrncpySafe( objaclRuleServiceTypeValValue,"IP PRECEDENCE:",sizeof(objaclRuleServiceTypeValValue));
        osapiSnprintf(stat,sizeof(stat),"%d",val);
        OSAPI_STRNCAT(objaclRuleServiceTypeValValue,stat); 
     }
     else
     {
        flag = usmDbQosAclIsFieldConfigured(L7_UNIT_CURRENT, keyaclIndexValue, keyaclRuleIndexValue, ACL_IPTOS);
        if(flag == L7_TRUE)
        {
            owa.l7rc = usmDbQosAclRuleIPTosGet(L7_UNIT_CURRENT, keyaclIndexValue, keyaclRuleIndexValue,&val,&val2);
            osapiStrncpySafe( objaclRuleServiceTypeValValue,"TOS BITS:" ,sizeof(objaclRuleServiceTypeValValue));
            osapiSnprintf(stat,sizeof(stat),"%2.2x",val);
            OSAPI_STRNCAT(objaclRuleServiceTypeValValue,stat);
            OSAPI_STRNCAT(objaclRuleServiceTypeValValue,"   TOS MASK:");
            val2 = ~val2 & 0x000000FF;
            osapiSnprintf(stat2,sizeof(stat2),"%2.2x",val2);
            OSAPI_STRNCAT(objaclRuleServiceTypeValValue,stat2);            
        }
     }
  }

  if(flag == L7_FALSE)
  {
     return XLIBRC_SUCCESS;
  }

  FPOBJ_TRACE_VALUE (bufp, objaclRuleServiceTypeValValue, strlen (objaclRuleServiceTypeValValue));

  /* return the object value: aclRuleServiceTypeVal */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objaclRuleServiceTypeValValue,
                           strlen (objaclRuleServiceTypeValValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}



/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclIpv6RuleProtocolKeyWord
*
* @purpose Get 'aclRuleProtocolKeyWord'
 *@description  [aclRuleProtocolKeyWord] icmp - 1 igmp - 2 ip - 4 tcp - 6 udp -
* 17 All values from 1 to 255 are valid.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_aclIpv6RuleProtocolKeyWord (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objaclIpv6RuleProtocolKeyWordValue;

  xLibU32_t keyaclIpv6IndexValue;
  xLibU32_t keyaclIpv6RuleIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIndex */
  owa.len = sizeof (keyaclIpv6IndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                          (xLibU8_t *) & keyaclIpv6IndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, owa.len);

  /* retrieve key: aclRuleIndex */
  owa.len = sizeof (keyaclIpv6RuleIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleIndex,
                          (xLibU8_t *) & keyaclIpv6RuleIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6RuleIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbQosAclRuleProtocolGet(L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                              keyaclIpv6RuleIndexValue, &objaclIpv6RuleProtocolKeyWordValue);
  if ((owa.l7rc == L7_SUCCESS) && (objaclIpv6RuleProtocolKeyWordValue!=0 ))
      {
        switch (objaclIpv6RuleProtocolKeyWordValue)
        {
          case L7_ACL_PROTOCOL_ICMPV6:                    
          case L7_ACL_PROTOCOL_IP:
          case L7_ACL_PROTOCOL_TCP:
          case L7_ACL_PROTOCOL_UDP:
          break;
          default:
          objaclIpv6RuleProtocolKeyWordValue = L7_ACL_PROTOCOL_OTHER;
          break;
        }
      }
  else
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objaclIpv6RuleProtocolKeyWordValue,
                     sizeof (objaclIpv6RuleProtocolKeyWordValue));

  /* return the object value: aclRuleProtocolKeyWord */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclIpv6RuleProtocolKeyWordValue,
                           sizeof (objaclIpv6RuleProtocolKeyWordValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACL_aclIpv6RuleProtocolKeyWord
*
* @purpose Set 'aclIpv6RuleIpv6ProtocolKeyWord'
 *@description  [aclIpv6RuleProtocolKeyWord] icmp - 1 igmp - 2 ip - 4 tcp - 6 udp -
* 17 All values from 1 to 255 are valid.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACL_aclIpv6RuleProtocolKeyWord (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objaclIpv6RuleProtocolKeyWordValue;

  xLibU32_t keyaclIpv6IndexValue;
  xLibU32_t keyaclIpv6RuleIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclRuleProtocolKeyWord */
  owa.len = sizeof (objaclIpv6RuleProtocolKeyWordValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objaclIpv6RuleProtocolKeyWordValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclIpv6RuleProtocolKeyWordValue, owa.len);

  /* retrieve key: aclIndex */
  owa.len = sizeof (keyaclIpv6IndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6Index,
                          (xLibU8_t *) & keyaclIpv6IndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6IndexValue, owa.len);

  /* retrieve key: aclRuleIndex */
  owa.len = sizeof (keyaclIpv6RuleIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIpv6RuleIndex,
                          (xLibU8_t *) & keyaclIpv6RuleIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIpv6RuleIndexValue, owa.len);

  /* set the value in application */
  if(objaclIpv6RuleProtocolKeyWordValue)
  {
       owa.l7rc = usmDbQosAclRuleProtocolAdd (L7_UNIT_CURRENT, keyaclIpv6IndexValue,
                              keyaclIpv6RuleIndexValue, objaclIpv6RuleProtocolKeyWordValue);
  
     if (owa.l7rc != L7_SUCCESS)
     {
         owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
     }
  }
  else
  {
      /* set prtocol number in protocol object */
      owa.rc = XLIBRC_SUCCESS;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_IPAclsExists
*
* @purpose Get 'IPAclsExists'
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_IPAclsExists (void *wap, void *bufp)
{
  xLibU32_t objaclIndexValue;
  xLibU32_t objAclsExist;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  objaclIndexValue = 0;
  owa.l7rc = usmDbQosAclNumGetFirst(L7_UNIT_CURRENT, &objaclIndexValue);
  if(owa.l7rc !=  L7_SUCCESS)
  {
     owa.l7rc =usmDbQosAclNamedIndexGetFirst(L7_UNIT_CURRENT, L7_ACL_TYPE_IP, &objaclIndexValue);
  }
  if (owa.l7rc == L7_SUCCESS)
  {
    objAclsExist = L7_XUI_TRUE;
  }
  else
  {
    objAclsExist = L7_XUI_FALSE;
  }

  /* return the object value: IPAclsExists */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAclsExist,
                           sizeof (objAclsExist));

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_MacAclsExists
*
* @purpose Get 'MacAclsExists'
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_MacAclsExists (void *wap, void *bufp)
{
  xLibU32_t objaclIndexValue;
  xLibU32_t objAclsExist;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  objaclIndexValue = 0;
  owa.l7rc = usmDbQosAclMacIndexGetFirst(L7_UNIT_CURRENT,&objaclIndexValue) ;
  if (owa.l7rc == L7_SUCCESS)
  {
    objAclsExist = L7_XUI_TRUE;
  }
  else
  {
    objAclsExist = L7_XUI_FALSE;
  }

  /* return the object value: MacAclsExists */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAclsExist,
                           sizeof (objAclsExist));


  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_IPv6AclsExists
*
* @purpose Get 'IPv6AclsExists'
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_IPv6AclsExists (void *wap, void *bufp)
{
   xLibU32_t objaclIndexValue;
  xLibU32_t objAclsExist;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  objaclIndexValue = 0;
  owa.l7rc =usmDbQosAclNamedIndexGetFirst(L7_UNIT_CURRENT, L7_ACL_TYPE_IPV6, &objaclIndexValue);
  if (owa.l7rc == L7_SUCCESS)
  {
    objAclsExist = L7_XUI_TRUE;
  }
  else
  {
    objAclsExist = L7_XUI_FALSE;
  }

  /* return the object value: IPv6AclsExists */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAclsExist,
                           sizeof (objAclsExist));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclMaxNum
*
* @purpose Get 'aclMaxNum'
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACLTABLE_aclMaxNum (void *wap, void *bufp)
{
  xLibU32_t objaclMaxNum;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  FPOBJ_TRACE_ENTER (bufp);

  owa.l7rc = usmDbQosAclMaxNumGet(L7_UNIT_CURRENT, &objaclMaxNum);
  if (owa.l7rc != L7_SUCCESS)
  {
     owa.rc = XLIBRC_FAILURE;
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
  }
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclMaxNum,
                           sizeof (objaclMaxNum));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_aclsConfigCurrent
*
* @purpose Get 'aclsConfigCurrent'
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACLTABLE_aclsConfigCurrent (void *wap, void *bufp)
{
  xLibU32_t objaclMaxNum;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  FPOBJ_TRACE_ENTER (bufp);

  owa.l7rc = usmDbQosAclCurrNumGet(L7_UNIT_CURRENT, &objaclMaxNum);
  if (owa.l7rc != L7_SUCCESS)
  {
     owa.rc = XLIBRC_FAILURE;
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
  }
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclMaxNum,
                           sizeof (objaclMaxNum));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACLTABLE_aclTableSizeAndMax
*
* @purpose Get 'aclTableSizeAndMax'
*
* @description [aclTableSizeAndMax]:  Current Size / Max Size of the ACL Table
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACLTABLE_aclTableSizeAndMax (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibStr256_t objaclTableSizeAndMaxValue;
  xLibU32_t tableSize, tableMax;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbQosAclCurrNumGet(L7_UNIT_CURRENT, &tableSize);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &tableSize, sizeof (tableSize));

  /* get the value from application */
  owa.l7rc = usmDbQosAclMaxNumGet(L7_UNIT_CURRENT, &tableMax);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &tableMax, sizeof (tableMax));

  /* format the string */
  osapiSnprintf(objaclTableSizeAndMaxValue, sizeof(objaclTableSizeAndMaxValue), 
                "%u / %u", tableSize, tableMax);

  /* return the object value: aclTableSizeAndMax */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclTableSizeAndMaxValue,
                           sizeof (objaclTableSizeAndMaxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_IsAclRuleMirrorIntfConfigured
*
* @purpose Get 'IsMirrorIntfConfigured'
*
* @description [IsMirrorIntfConfigured]: A non-zero value indicates that the
*              mirror interface is already configured for this particular
*              acl/rule combination and zero indicates no mirror interface is
*              configured.
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_IsAclRuleMirrorIntfConfigured(void *wap,
                                                              void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t value, intIfNum;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                           (xLibU8_t *) & keyaclIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, kwa1.len);

  /* retrieve key: aclRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                           (xLibU8_t *) & keyaclRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, kwa2.len);

  owa.l7rc = usmDbQosAclRuleMirrorIntfGet(L7_UNIT_CURRENT, keyaclIndexValue, 
                                             keyaclRuleIndexValue, &intIfNum);
  if (owa.l7rc != L7_SUCCESS)
  {
    value = L7_FALSE;
  }
  else
  {
    value = L7_TRUE;
  }

  /* return the object value: aclRuleRedirectIntf */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & value, sizeof (value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACL_IsAclRuleRedirectIntfConfigured
*
* @purpose Get 'IsRedirectIntfConfigured'
*
* @description [IsRedirectIntfConfigured]: A non-zero value indicates that the 
*              redirect interface is already configured for this particular
*              acl/rule combination and zero indicates no redirect interface is
*              configured. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACL_IsAclRuleRedirectIntfConfigured(void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyaclRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t value,intIfNum;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclIndex,
                           (xLibU8_t *) & keyaclIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIndexValue, kwa1.len);

  /* retrieve key: aclRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACL_aclRuleIndex,
                           (xLibU8_t *) & keyaclRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclRuleIndexValue, kwa2.len);

  owa.l7rc = usmDbQosAclRuleRedirectIntfGet(L7_UNIT_CURRENT, keyaclIndexValue,
                                             keyaclRuleIndexValue, &intIfNum);
  if (owa.l7rc != L7_SUCCESS)
  {
    value = L7_FALSE;
  }
  else
  {
    value = L7_TRUE;
  }

  /* return the object value: aclRuleRedirectIntf */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & value, sizeof (value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


