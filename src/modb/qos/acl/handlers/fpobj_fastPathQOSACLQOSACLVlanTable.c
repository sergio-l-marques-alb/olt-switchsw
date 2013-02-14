
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_fastPathQOSACLQOSACLVlanTable.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to fastPathQOSACL-object.xml
*
* @create  24 July 2008, Thursday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_fastPathQOSACLQOSACLVlanTable_obj.h"
#include "usmdb_qos_acl_api.h"
#include "acl_exports.h"
#include "dot1q_exports.h"
#include "usmdb_util_api.h"
#include "usmdb_mib_vlan_api.h"

/*utility Functions */
L7_RC_t fpobjUtil_AclVlanAClIdTypeGet(L7_uint32 UnitIndex, L7_uint32 aclVlanIndex, L7_uint32 aclVlanDirection,
                   L7_uint32 aclVlanSequence, L7_uint32 *nextaclVlanAclType, L7_uint32 *nextaclVlanAclId);
L7_RC_t
fpObjUtil_QosAclVlanNext(L7_uint32 UnitIndex, L7_uint32 *aclVlanIndex, L7_uint32 *aclVlanDirection,
                   L7_uint32 *aclVlanSequence, L7_uint32 *aclVlanAclType, L7_uint32 *aclVlanAclId);
L7_RC_t fpobj_QosAclVlanAdd(L7_uint32 UnitIndex, L7_uint32 aclVlanIndex, L7_uint32 aclVlanDirection,
                   L7_uint32 aclVlanSequence, L7_uint32 aclVlanAclType, L7_uint32 aclVlanAclId);
L7_RC_t fpobj_QosAclVlanRemove(L7_uint32 UnitIndex, L7_uint32 aclVlanIndex, L7_uint32 aclVlanDirection,
                   L7_uint32 aclVlanSequence, L7_uint32 aclVlanAclType, L7_uint32 aclVlanAclId);
L7_RC_t
fpObjUtil_QosAclVlanNext(L7_uint32 UnitIndex, L7_uint32 *aclVlanIndex, L7_uint32 *aclVlanDirection,
                   L7_uint32 *aclVlanSequence, L7_uint32 *aclVlanAclType, L7_uint32 *aclVlanAclId)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 count = 0;
  L7_ACL_VLAN_DIR_LIST_t listInfo;
  L7_uint32 nextAclVlanIndex, nextAclVlanDirection, nextAclVlanSequence;
  L7_uint32 temp_direction;

  if(*aclVlanDirection  == -1)
  {
    temp_direction = L7_INBOUND_ACL;
  }
  else
  {
     temp_direction = *aclVlanDirection;
  }

  while(1)
  {
    rc = usmDbQosAclVlanDirSequenceGetNext(UnitIndex, *aclVlanIndex, temp_direction,
                                           *aclVlanSequence, &nextAclVlanSequence);
    if(rc == L7_SUCCESS)
    {
      *aclVlanSequence = nextAclVlanSequence;
      break;
    }
    if (rc != L7_SUCCESS)
    {
      /* Trying to get next (interface + direction) in use */
      rc = usmDbQosAclVlanDirGetNext(UnitIndex,
                                     *aclVlanIndex,
                                     temp_direction,
                                     &nextAclVlanIndex,
                                     &nextAclVlanDirection);

      if(rc != L7_SUCCESS)
      {
        break;
      }
      else if(rc == L7_SUCCESS && (usmDbQosAclIsVlanInUse(UnitIndex,
                                                          nextAclVlanIndex,
                                                          nextAclVlanDirection)) == L7_TRUE)
      {
          *aclVlanIndex = nextAclVlanIndex;
          temp_direction = nextAclVlanDirection;
          *aclVlanSequence = 0;
          rc = L7_SUCCESS;
      }
      else if( rc == L7_SUCCESS && (usmDbQosAclIsVlanInUse(UnitIndex,
                                                           nextAclVlanIndex,
                                                           nextAclVlanDirection)) != L7_TRUE)
      {
         *aclVlanIndex = nextAclVlanIndex;
         temp_direction = nextAclVlanDirection;
         *aclVlanSequence = 0;
         rc = L7_FAILURE;
         continue;
      }

      if (rc == L7_SUCCESS)
      {
        /* Get the first sequence for this interface and direction with input value of
           *aclVlanSequence = 0 */
        rc = usmDbQosAclVlanDirSequenceGetNext(UnitIndex,nextAclVlanIndex,nextAclVlanDirection,
 *aclVlanSequence,
                                               &nextAclVlanSequence);
        if(rc == L7_SUCCESS)
        {
          *aclVlanIndex = nextAclVlanIndex;
          temp_direction = nextAclVlanDirection;
          *aclVlanSequence = nextAclVlanSequence;
          break;
        }
      }
    } /* end of if (rc != L7_SUCCESS) */

  } /* end of while(1) */
  /* Once got the values of next intfIndex, Direction and sequence, now need to get
     next aclType and aclId. For a pair of (intfIndex+ Direction), there would be only one
     combination of (aclType + aclId) for a particular value of aclVlanSequence. */
  if(rc == L7_SUCCESS)
    rc = usmDbQosAclVlanDirAclListGet( UnitIndex,*aclVlanIndex,temp_direction,&listInfo);

  if(rc == L7_SUCCESS)
    for(count = 0;count < listInfo.count;count++)
    {
      rc = L7_FAILURE;
      if(listInfo.listEntry[count].seqNum == *aclVlanSequence)
      {
        rc = L7_SUCCESS;
        break;
      }
    }
 if(rc == L7_SUCCESS)
  {
    switch (temp_direction)
  {
      case L7_INBOUND_ACL:
        *aclVlanDirection = L7_INBOUND_ACL;
        break;

      case L7_OUTBOUND_ACL:
        *aclVlanDirection = L7_OUTBOUND_ACL;
        break;
       default:
        rc = L7_FAILURE;
        break;
    }
    *aclVlanAclType = listInfo.listEntry[count].aclType;
    *aclVlanAclId =   listInfo.listEntry[count].aclId;
  }

  return rc;
}

L7_RC_t fpobjUtil_AclVlanAClIdTypeGet(L7_uint32 UnitIndex, L7_uint32 aclVlanIndex, L7_uint32 aclVlanDirection,
                   L7_uint32 aclVlanSequence, L7_uint32 *nextaclVlanAclType, L7_uint32 *nextaclVlanAclId)
{
  L7_RC_t rc;
  L7_uint32 count = 0;
  L7_ACL_VLAN_DIR_LIST_t listInfo;
  L7_uint32 aclVlanAclType = *nextaclVlanAclType;
  L7_uint32 aclVlanAclId = *nextaclVlanAclId;

  if((rc  =  usmDbQosAclVlanDirGet( UnitIndex,aclVlanIndex, aclVlanDirection ))== L7_SUCCESS)
   {
      if((rc = usmDbQosAclVlanDirSequenceGet(UnitIndex, aclVlanIndex, aclVlanDirection ,aclVlanSequence)) ==L7_SUCCESS)
      {
          rc =  usmDbQosAclVlanDirAclListGet( UnitIndex, aclVlanIndex, aclVlanDirection, &listInfo);
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
                 rc = L7_FAILURE;
          }
          else if(rc == L7_SUCCESS)
          {
                *nextaclVlanAclType = listInfo.listEntry[count].aclType;
                *nextaclVlanAclId = listInfo.listEntry[count].aclId;
                rc = L7_SUCCESS;
          }
       }
    }
    return rc;
}
L7_RC_t fpobj_QosAclVlanAdd(L7_uint32 UnitIndex, L7_uint32 aclVlanIndex, L7_uint32 aclVlanDirection,L7_uint32 aclVlanSequence, L7_uint32 aclVlanAclType, L7_uint32 aclVlanAclId)
{
  L7_RC_t rc = L7_SUCCESS;
  if((aclVlanDirection != L7_INBOUND_ACL) && (aclVlanDirection != L7_OUTBOUND_ACL))
  {
    rc = L7_FAILURE;
  }


  if (rc == L7_SUCCESS && aclVlanAclType == L7_ACL_TYPE_IP)
    rc = usmDbQosAclVlanDirectionAdd(UnitIndex, aclVlanIndex,
                                          aclVlanDirection,aclVlanAclId,aclVlanSequence);

  if (rc == L7_SUCCESS && aclVlanAclType == L7_ACL_TYPE_IPV6)
    rc = usmDbQosAclVlanDirectionAdd(UnitIndex,aclVlanIndex,
                                          aclVlanDirection,aclVlanAclId,aclVlanSequence);

  if (rc == L7_SUCCESS && aclVlanAclType == L7_ACL_TYPE_MAC)
    rc = usmDbQosAclMacVlanDirectionAdd(UnitIndex,aclVlanIndex,
                                          aclVlanDirection,aclVlanAclId,aclVlanSequence);

  return rc;

}
L7_RC_t fpobj_QosAclVlanRemove(L7_uint32 UnitIndex, L7_uint32 aclVlanIndex, L7_uint32 aclVlanDirection, L7_uint32 aclVlanSequence, L7_uint32 aclVlanAclType, L7_uint32 aclVlanAclId)
{
  L7_RC_t rc=L7_SUCCESS;
  if((aclVlanDirection != L7_INBOUND_ACL) && (aclVlanDirection != L7_OUTBOUND_ACL))
  {
    rc = L7_FAILURE;
  }


  if (rc == L7_SUCCESS && aclVlanAclType == L7_ACL_TYPE_IP)
    rc = usmDbQosAclVlanDirectionRemove(UnitIndex, aclVlanIndex,
                                          aclVlanDirection,aclVlanAclId);

  if (rc == L7_SUCCESS && aclVlanAclType == L7_ACL_TYPE_IPV6)
    rc = usmDbQosAclVlanDirectionRemove(UnitIndex,  aclVlanIndex,
                                          aclVlanDirection,aclVlanAclId);

  if (rc == L7_SUCCESS && aclVlanAclType == L7_ACL_TYPE_MAC)
    rc = usmDbQosAclMacVlanDirectionRemove(UnitIndex,  aclVlanIndex,
                                          aclVlanDirection,aclVlanAclId);

  return rc;
}


/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACLVlanTable_aclVlanIndex
*
* @purpose Get 'aclVlanIndex'
 *@description  [aclVlanIndex] The Vlan to which this ACL instance applies   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACLVlanTable_aclVlanIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objaclVlanIndexValue;
  xLibU32_t nextObjaclVlanIndexValue;
  xLibU32_t objaclVlanDirectionValue;
  xLibU32_t nextObjaclVlanDirectionValue;
  xLibU32_t objaclVlanSequenceValue;
  xLibU32_t nextObjaclVlanSequenceValue;
  xLibU32_t objaclVlanAclTypeValue;
  xLibU32_t nextObjaclVlanAclTypeValue;
  xLibU32_t objaclVlanAclIdValue;
  xLibU32_t nextObjaclVlanAclIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclVlanIndex */
  owa.len = sizeof (objaclVlanIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLVlanTable_aclVlanIndex,
                          (xLibU8_t *) & objaclVlanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    FPOBJ_CLR_U32 (objaclVlanIndexValue);
    FPOBJ_CLR_U32 (objaclVlanDirectionValue);
    FPOBJ_CLR_U32 (objaclVlanSequenceValue);
    FPOBJ_CLR_U32 (objaclVlanAclTypeValue);
    FPOBJ_CLR_U32 (objaclVlanAclIdValue);
    objaclVlanDirectionValue = L7_INBOUND_ACL;
    
    owa.l7rc = fpObjUtil_QosAclVlanNext(L7_UNIT_CURRENT,
                                     &objaclVlanIndexValue,
                                     &objaclVlanDirectionValue,
                                     &objaclVlanSequenceValue,
                                     &objaclVlanAclTypeValue,
                                     &objaclVlanAclIdValue);
    if(owa.l7rc == L7_SUCCESS)
   {
       nextObjaclVlanIndexValue = objaclVlanIndexValue;
   }
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclVlanIndexValue, owa.len);
    FPOBJ_CLR_U32 (objaclVlanDirectionValue);
    FPOBJ_CLR_U32 (objaclVlanSequenceValue);
    FPOBJ_CLR_U32 (objaclVlanAclTypeValue);
    FPOBJ_CLR_U32 (objaclVlanAclIdValue);
    nextObjaclVlanIndexValue = objaclVlanIndexValue;
    FPOBJ_CLR_U32 (nextObjaclVlanDirectionValue);
    FPOBJ_CLR_U32 (nextObjaclVlanSequenceValue);
    objaclVlanDirectionValue = L7_INBOUND_ACL;
    do
    {
      owa.l7rc = fpObjUtil_QosAclVlanNext(L7_UNIT_CURRENT,
                                      &nextObjaclVlanIndexValue,
                                      &nextObjaclVlanDirectionValue, &nextObjaclVlanSequenceValue,
                                      &nextObjaclVlanAclTypeValue, &nextObjaclVlanAclIdValue);
      FPOBJ_CPY_U32 (objaclVlanDirectionValue, nextObjaclVlanDirectionValue);
      FPOBJ_CPY_U32 (objaclVlanSequenceValue, nextObjaclVlanSequenceValue);
      FPOBJ_CPY_U32 (objaclVlanAclTypeValue, nextObjaclVlanAclTypeValue);
      FPOBJ_CPY_U32 (objaclVlanAclIdValue, nextObjaclVlanAclIdValue);
    }
    while (FPOBJ_CMP_U32 (objaclVlanIndexValue, nextObjaclVlanIndexValue)
           && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjaclVlanIndexValue, owa.len);

  /* return the object value: aclVlanIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjaclVlanIndexValue,
                           sizeof (nextObjaclVlanIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjList_fastPathQOSACLQOSACLVlanTable_aclVlanIndex
*
* @purpose List 'aclVlanIndex'
 *@description  [aclVlanIndex] The Vlan to which this ACL instance applies   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_fastPathQOSACLQOSACLVlanTable_aclVlanIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objaclVlanIndexValue;
  xLibU32_t nextObjaclVlanIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof (objaclVlanIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLVlanTable_aclVlanIndex,
                          (xLibU8_t *) & objaclVlanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjaclVlanIndexValue = L7_ACL_MIN_VLAN_ID;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclVlanIndexValue, owa.len);
    if((objaclVlanIndexValue >= L7_ACL_MIN_VLAN_ID) && (objaclVlanIndexValue < L7_ACL_MAX_VLAN_ID))
    {
       nextObjaclVlanIndexValue = objaclVlanIndexValue +1;
       owa.l7rc = L7_SUCCESS;
    }
    else
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

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjaclVlanIndexValue, owa.len);

  /* return the object value: aclVlanIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjaclVlanIndexValue,
                           sizeof (nextObjaclVlanIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACLVlanTable_aclVlanDirection
*
* @purpose Get 'aclVlanDirection'
 *@description  [aclVlanDirection] The Vlan direction to which this ACL instance
* applies.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACLVlanTable_aclVlanDirection (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objaclVlanIndexValue;
  xLibU32_t nextObjaclVlanIndexValue;
  xLibU32_t objaclVlanDirectionValue;
  xLibU32_t nextObjaclVlanDirectionValue;
  xLibU32_t objaclVlanSequenceValue;
  xLibU32_t nextObjaclVlanSequenceValue;
  xLibU32_t objaclVlanAclTypeValue;
  xLibU32_t nextObjaclVlanAclTypeValue;
  xLibU32_t objaclVlanAclIdValue;
  xLibU32_t nextObjaclVlanAclIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclVlanIndex */
  owa.len = sizeof (objaclVlanIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLVlanTable_aclVlanIndex,
                          (xLibU8_t *) & objaclVlanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclVlanIndexValue, owa.len);

  /* retrieve key: aclVlanDirection */
  owa.len = sizeof (objaclVlanDirectionValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLVlanTable_aclVlanDirection,
                          (xLibU8_t *) & objaclVlanDirectionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    FPOBJ_CLR_U32 (objaclVlanDirectionValue);
    FPOBJ_CLR_U32 (objaclVlanSequenceValue);
    FPOBJ_CLR_U32 (objaclVlanAclTypeValue);
    FPOBJ_CLR_U32 (objaclVlanAclIdValue);    
    FPOBJ_CLR_U32 (nextObjaclVlanDirectionValue);
    FPOBJ_CLR_U32 (nextObjaclVlanSequenceValue);
    FPOBJ_CLR_U32 (nextObjaclVlanAclTypeValue);
    FPOBJ_CLR_U32 (nextObjaclVlanAclIdValue);    
    nextObjaclVlanIndexValue = objaclVlanIndexValue;
    nextObjaclVlanDirectionValue = -1;
    objaclVlanDirectionValue =-1;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclVlanDirectionValue, owa.len);
    FPOBJ_CLR_U32 (objaclVlanSequenceValue);
    FPOBJ_CLR_U32 (objaclVlanAclTypeValue);
    FPOBJ_CLR_U32 (objaclVlanAclIdValue);
    FPOBJ_CLR_U32 (nextObjaclVlanSequenceValue);
    nextObjaclVlanIndexValue = objaclVlanIndexValue;
    nextObjaclVlanDirectionValue = objaclVlanDirectionValue;
  }
    do
    {
      owa.l7rc = fpObjUtil_QosAclVlanNext(L7_UNIT_CURRENT,
                                      &nextObjaclVlanIndexValue,
                                      &nextObjaclVlanDirectionValue, &nextObjaclVlanSequenceValue,
                                      &nextObjaclVlanAclTypeValue, &nextObjaclVlanAclIdValue);
      FPOBJ_CPY_U32 (objaclVlanSequenceValue, nextObjaclVlanSequenceValue);
      FPOBJ_CPY_U32 (objaclVlanAclTypeValue, nextObjaclVlanAclTypeValue);
      FPOBJ_CPY_U32 (objaclVlanAclIdValue, nextObjaclVlanAclIdValue);
    }
    while (FPOBJ_CMP_U32 (objaclVlanIndexValue, nextObjaclVlanIndexValue)
           && FPOBJ_CMP_U32 (objaclVlanDirectionValue, nextObjaclVlanDirectionValue)
           && (owa.l7rc == L7_SUCCESS));

  if ((!FPOBJ_CMP_U32 (objaclVlanIndexValue, nextObjaclVlanIndexValue)) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjaclVlanDirectionValue, owa.len);

  /* return the object value: aclVlanDirection */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjaclVlanDirectionValue,
                           sizeof (nextObjaclVlanDirectionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjList_fastPathQOSACLQOSACLVlanTable_aclVlanDirection
*
* @purpose List 'aclVlanDirection'
 *@description  [aclVlanDirection] The Vlan direction to which this ACL instance
* applies.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_fastPathQOSACLQOSACLVlanTable_aclVlanDirection (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objaclVlanDirectionValue;
  xLibU32_t nextObjaclVlanDirectionValue;

  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof (objaclVlanDirectionValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLVlanTable_aclVlanDirection,
                          (xLibU8_t *) & objaclVlanDirectionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjaclVlanDirectionValue =  L7_INBOUND_ACL;
    owa.l7rc = L7_SUCCESS;

  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclVlanDirectionValue, owa.len);
    /*TODO: check the feature outbound */
    if(objaclVlanDirectionValue == L7_INBOUND_ACL)
    {
       owa.l7rc = L7_FAILURE;
       if( usmDbFeaturePresentCheck(L7_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_INTF_DIRECTION_OUTBOUND_FEATURE_ID) == L7_TRUE )
       {
          nextObjaclVlanDirectionValue = L7_OUTBOUND_ACL;
          owa.l7rc = L7_SUCCESS;
       }
    }
    else{
      owa.l7rc = L7_FAILURE;
   }

  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjaclVlanDirectionValue, owa.len);

  /* return the object value: aclVlanDirection */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjaclVlanDirectionValue,
                           sizeof (nextObjaclVlanDirectionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACLVlanTable_aclVlanSequence
*
* @purpose Get 'aclVlanSequence'
 *@description  [aclVlanSequence] The relative evaluation sequence of this ACL
* for this Vlan and direction. When multiple ACLs are allowed for a
* given Vlan and direction, the sequence number determines the
* order in which the list of ACLs are evaluated,with lower sequence
* numbers given hi   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACLVlanTable_aclVlanSequence (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objaclVlanIndexValue;
  xLibU32_t nextObjaclVlanIndexValue;
  xLibU32_t objaclVlanDirectionValue;
  xLibU32_t nextObjaclVlanDirectionValue;
  xLibU32_t objaclVlanSequenceValue;
  xLibU32_t nextObjaclVlanSequenceValue;
  xLibU32_t objaclVlanAclTypeValue;
  xLibU32_t nextObjaclVlanAclTypeValue;
  xLibU32_t objaclVlanAclIdValue;
  xLibU32_t nextObjaclVlanAclIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclVlanIndex */
  owa.len = sizeof (objaclVlanIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLVlanTable_aclVlanIndex,
                          (xLibU8_t *) & objaclVlanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclVlanIndexValue, owa.len);

  /* retrieve key: aclVlanDirection */
  owa.len = sizeof (objaclVlanDirectionValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLVlanTable_aclVlanDirection,
                          (xLibU8_t *) & objaclVlanDirectionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclVlanDirectionValue, owa.len);

  /* retrieve key: aclVlanSequence */
  owa.len = sizeof (objaclVlanSequenceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLVlanTable_aclVlanSequence,
                          (xLibU8_t *) & objaclVlanSequenceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    FPOBJ_CLR_U32 (objaclVlanSequenceValue);
    FPOBJ_CLR_U32 (objaclVlanAclTypeValue);
    FPOBJ_CLR_U32 (objaclVlanAclIdValue);
    FPOBJ_CLR_U32 (nextObjaclVlanSequenceValue);
    FPOBJ_CLR_U32 (nextObjaclVlanAclTypeValue);
    FPOBJ_CLR_U32 (nextObjaclVlanAclIdValue);
    nextObjaclVlanIndexValue = objaclVlanIndexValue;
    nextObjaclVlanDirectionValue = objaclVlanDirectionValue;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclVlanSequenceValue, owa.len);
    FPOBJ_CLR_U32 (objaclVlanAclTypeValue);
    FPOBJ_CLR_U32 (objaclVlanAclIdValue);
    nextObjaclVlanIndexValue = objaclVlanIndexValue;
    nextObjaclVlanDirectionValue = objaclVlanDirectionValue;
    nextObjaclVlanSequenceValue = objaclVlanSequenceValue;
  }
    do
    {
      owa.l7rc = fpObjUtil_QosAclVlanNext(L7_UNIT_CURRENT,
                                      &nextObjaclVlanIndexValue,
                                      &nextObjaclVlanDirectionValue, &nextObjaclVlanSequenceValue,
                                      &nextObjaclVlanAclTypeValue, &nextObjaclVlanAclIdValue);
      FPOBJ_CPY_U32 (objaclVlanAclTypeValue, nextObjaclVlanAclTypeValue);
      FPOBJ_CPY_U32 (objaclVlanAclIdValue, nextObjaclVlanAclIdValue);
    }
    while (FPOBJ_CMP_U32 (objaclVlanIndexValue, nextObjaclVlanIndexValue)
           && FPOBJ_CMP_U32 (objaclVlanDirectionValue, nextObjaclVlanDirectionValue)
           && FPOBJ_CMP_U32 (objaclVlanSequenceValue, nextObjaclVlanSequenceValue)
           && (owa.l7rc == L7_SUCCESS));  

  if ((!FPOBJ_CMP_U32 (objaclVlanIndexValue, nextObjaclVlanIndexValue))
      || (!FPOBJ_CMP_U32 (objaclVlanDirectionValue, nextObjaclVlanDirectionValue))
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjaclVlanSequenceValue, owa.len);

  /* return the object value: aclVlanSequence */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjaclVlanSequenceValue,
                           sizeof (nextObjaclVlanSequenceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACLVlanTable_aclVlanAclType
*
* @purpose Get 'aclVlanAclType'
 *@description  [aclVlanAclType] The type of this ACL, which is used to
* interpret theaclVlanId object value. Each type of ACL uses its own
* numbering scheme for identification (see aclVlanAclId object for
* details). The aclVlanAclId object must be specified along with this
* object   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACLVlanTable_aclVlanAclType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();

  xLibU32_t objaclVlanIndexValue;
  xLibU32_t nextObjaclVlanIndexValue;
  xLibU32_t objaclVlanDirectionValue;
  xLibU32_t nextObjaclVlanDirectionValue;
  xLibU32_t objaclVlanSequenceValue;
  xLibU32_t nextObjaclVlanSequenceValue;
  xLibU32_t objaclVlanAclTypeValue;
  xLibU32_t nextObjaclVlanAclTypeValue;
  xLibU32_t objaclVlanAclIdValue;
  xLibU32_t nextObjaclVlanAclIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclVlanIndex */
  owa.len = sizeof (objaclVlanIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLVlanTable_aclVlanIndex,
                          (xLibU8_t *) & objaclVlanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclVlanIndexValue, owa.len);

  /* retrieve key: aclVlanDirection */
  owa.len = sizeof (objaclVlanDirectionValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLVlanTable_aclVlanDirection,
                          (xLibU8_t *) & objaclVlanDirectionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclVlanDirectionValue, owa.len);

  /* retrieve key: aclVlanSequence */
  owa.len = sizeof (objaclVlanSequenceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLVlanTable_aclVlanSequence,
                          (xLibU8_t *) & objaclVlanSequenceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclVlanSequenceValue, owa.len);

  /* retrieve key: aclVlanAclType */
  owa.len = sizeof (objaclVlanAclTypeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLVlanTable_aclVlanAclType,
                          (xLibU8_t *) & objaclVlanAclTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    FPOBJ_CLR_U32 (objaclVlanAclTypeValue);
    FPOBJ_CLR_U32 (objaclVlanAclIdValue);
    FPOBJ_CLR_U32 (nextObjaclVlanAclTypeValue);
    FPOBJ_CLR_U32 (nextObjaclVlanAclIdValue);
    nextObjaclVlanIndexValue = objaclVlanIndexValue;
    nextObjaclVlanDirectionValue = objaclVlanDirectionValue;
    nextObjaclVlanSequenceValue = objaclVlanSequenceValue;
  }  
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclVlanAclTypeValue, owa.len);
    FPOBJ_CLR_U32 (objaclVlanAclIdValue);
    FPOBJ_CLR_U32 (nextObjaclVlanAclIdValue);
    nextObjaclVlanIndexValue = objaclVlanIndexValue;
    nextObjaclVlanDirectionValue = objaclVlanDirectionValue;
    nextObjaclVlanSequenceValue = objaclVlanSequenceValue;
    nextObjaclVlanAclTypeValue = objaclVlanAclTypeValue;
  }
    do
    {
      owa.l7rc = fpobjUtil_AclVlanAClIdTypeGet(L7_UNIT_CURRENT,
                                      nextObjaclVlanIndexValue,
                                      nextObjaclVlanDirectionValue, nextObjaclVlanSequenceValue,
                                      &nextObjaclVlanAclTypeValue, &nextObjaclVlanAclIdValue);
      if(owa.l7rc != L7_SUCCESS)
      {
          owa.l7rc = fpObjUtil_QosAclVlanNext(L7_UNIT_CURRENT,
                                      &nextObjaclVlanIndexValue,
                                      &nextObjaclVlanDirectionValue, &nextObjaclVlanSequenceValue,
                                      &nextObjaclVlanAclTypeValue, &nextObjaclVlanAclIdValue);
      }
       FPOBJ_CPY_U32 (objaclVlanAclIdValue, nextObjaclVlanAclIdValue);
    }
    while (FPOBJ_CMP_U32 (objaclVlanIndexValue, nextObjaclVlanIndexValue)
           && FPOBJ_CMP_U32 (objaclVlanDirectionValue, nextObjaclVlanDirectionValue)
           && FPOBJ_CMP_U32 (objaclVlanSequenceValue, nextObjaclVlanSequenceValue)
           && FPOBJ_CMP_U32 (objaclVlanAclTypeValue, nextObjaclVlanAclTypeValue)
           && (owa.l7rc == L7_SUCCESS));  

  if ((!FPOBJ_CMP_U32 (objaclVlanIndexValue, nextObjaclVlanIndexValue))
      || (!FPOBJ_CMP_U32 (objaclVlanDirectionValue, nextObjaclVlanDirectionValue))
      || (!FPOBJ_CMP_U32 (objaclVlanSequenceValue, nextObjaclVlanSequenceValue))
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjaclVlanAclTypeValue, owa.len);

  /* return the object value: aclVlanAclType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjaclVlanAclTypeValue,
                           sizeof (nextObjaclVlanAclTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjList_fastPathQOSACLQOSACLVlanTable_aclVlanAclType
*
* @purpose List 'aclVlanAclType'
 *@description  [aclVlanAclType] The type of this ACL, which is used to
* interpret theaclVlanId object value. Each type of ACL uses its own
* numbering scheme for identification (see aclVlanAclId object for
* details). The aclVlanAclId object must be specified along with this
* object   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_fastPathQOSACLQOSACLVlanTable_aclVlanAclType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objaclVlanAclTypeValue;
  xLibU32_t nextObjaclVlanAclTypeValue;

  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof (objaclVlanAclTypeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLVlanTable_aclVlanAclType,
                          (xLibU8_t *) & objaclVlanAclTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjaclVlanAclTypeValue = L7_ACL_TYPE_NONE;
     owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclVlanAclTypeValue, owa.len);
    if(objaclVlanAclTypeValue == L7_ACL_TYPE_NONE)
    {
       nextObjaclVlanAclTypeValue = L7_ACL_TYPE_IP;
       owa.l7rc = L7_SUCCESS;
    }
    else if (objaclVlanAclTypeValue == L7_ACL_TYPE_IP)
    {
       nextObjaclVlanAclTypeValue = L7_ACL_TYPE_MAC;
       owa.l7rc = L7_SUCCESS;
    }
    else if(objaclVlanAclTypeValue == L7_ACL_TYPE_MAC)
    {
        nextObjaclVlanAclTypeValue = L7_ACL_TYPE_IPV6;
        owa.l7rc = L7_SUCCESS;

    }
    else if(objaclVlanAclTypeValue == L7_ACL_TYPE_IPV6)
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

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjaclVlanAclTypeValue, owa.len);

  /* return the object value: aclVlanAclType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjaclVlanAclTypeValue,
                           sizeof (nextObjaclVlanAclTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACLVlanTable_aclVlanAclId
*
* @purpose Get 'aclVlanAclId'
 *@description  [aclVlanAclId] The ACL identifier value, which is interpreted
* based on the aclVlanType object. For the IP ACLs, the actual ACL
* number is its identifier as follows: IP standard ranges from 1-99,
* while IP extended ranges from 100-199. Here, aclVlanAclId
* represents aclI   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACLVlanTable_aclVlanAclId (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();

  xLibU32_t objaclVlanIndexValue;
  xLibU32_t nextObjaclVlanIndexValue;
  xLibU32_t objaclVlanDirectionValue;
  xLibU32_t nextObjaclVlanDirectionValue;
  xLibU32_t objaclVlanSequenceValue;
  xLibU32_t nextObjaclVlanSequenceValue;
  xLibU32_t objaclVlanAclTypeValue;
  xLibU32_t nextObjaclVlanAclTypeValue;
  xLibU32_t objaclVlanAclIdValue;
  xLibU32_t nextObjaclVlanAclIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclVlanIndex */
  owa.len = sizeof (objaclVlanIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLVlanTable_aclVlanIndex,
                          (xLibU8_t *) & objaclVlanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclVlanIndexValue, owa.len);

  /* retrieve key: aclVlanDirection */
  owa.len = sizeof (objaclVlanDirectionValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLVlanTable_aclVlanDirection,
                          (xLibU8_t *) & objaclVlanDirectionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclVlanDirectionValue, owa.len);

  /* retrieve key: aclVlanSequence */
  owa.len = sizeof (objaclVlanSequenceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLVlanTable_aclVlanSequence,
                          (xLibU8_t *) & objaclVlanSequenceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclVlanSequenceValue, owa.len);

  /* retrieve key: aclVlanAclType */
  owa.len = sizeof (objaclVlanAclTypeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLVlanTable_aclVlanAclType,
                          (xLibU8_t *) & objaclVlanAclTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclVlanAclTypeValue, owa.len);

  /* retrieve key: aclVlanAclId */
  owa.len = sizeof (objaclVlanAclIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLVlanTable_aclVlanAclId,
                          (xLibU8_t *) & objaclVlanAclIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    FPOBJ_CLR_U32 (objaclVlanAclIdValue);
    FPOBJ_CLR_U32 (nextObjaclVlanAclIdValue);
    nextObjaclVlanIndexValue = objaclVlanIndexValue;
    nextObjaclVlanDirectionValue = objaclVlanDirectionValue;
    nextObjaclVlanSequenceValue = objaclVlanSequenceValue;
    nextObjaclVlanAclTypeValue = objaclVlanAclTypeValue;    
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclVlanAclIdValue, owa.len);
    nextObjaclVlanIndexValue = objaclVlanIndexValue;
    nextObjaclVlanDirectionValue = objaclVlanDirectionValue;
    nextObjaclVlanSequenceValue = objaclVlanSequenceValue;
    nextObjaclVlanAclTypeValue = objaclVlanAclTypeValue;
    nextObjaclVlanAclIdValue = objaclVlanAclIdValue;

  }

      owa.l7rc = fpobjUtil_AclVlanAClIdTypeGet(L7_UNIT_CURRENT,
                                      nextObjaclVlanIndexValue,
                                      nextObjaclVlanDirectionValue, nextObjaclVlanSequenceValue,
                                      &nextObjaclVlanAclTypeValue, &nextObjaclVlanAclIdValue);
      if(owa.l7rc != L7_SUCCESS)
      {
          owa.l7rc = fpObjUtil_QosAclVlanNext(L7_UNIT_CURRENT,
                                      &nextObjaclVlanIndexValue,
                                      &nextObjaclVlanDirectionValue, &nextObjaclVlanSequenceValue,
                                      &nextObjaclVlanAclTypeValue, &nextObjaclVlanAclIdValue);
      }

  if ((!FPOBJ_CMP_U32 (objaclVlanIndexValue, nextObjaclVlanIndexValue))
      || (!FPOBJ_CMP_U32 (objaclVlanDirectionValue, nextObjaclVlanDirectionValue))
      || (!FPOBJ_CMP_U32 (objaclVlanSequenceValue, nextObjaclVlanSequenceValue))
      || (!FPOBJ_CMP_U32 (objaclVlanAclTypeValue, nextObjaclVlanAclTypeValue))
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjaclVlanAclIdValue, owa.len);

  /* return the object value: aclVlanAclId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjaclVlanAclIdValue,
                           sizeof (nextObjaclVlanAclIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjList_fastPathQOSACLQOSACLVlanTable_aclVlanAclId
*
* @purpose Get 'aclVlanAclId'
 *@description  [aclVlanAclId] The ACL identifier value, which is interpreted
* based on the aclVlanType object. For the IP ACLs, the actual ACL
* number is its identifier as follows: IP standard ranges from 1-99,
* while IP extended ranges from 100-199. Here, aclVlanAclId
* represents aclI
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_fastPathQOSACLQOSACLVlanTable_aclVlanAclId (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objaclVlanAclIdValue;
  xLibU32_t nextObjaclVlanAclIdValue;
  xLibU32_t aclType;
  xLibU16_t aclTypeSize;
  xLibU32_t objaclIpv6IndexValue,nextObjaclIpv6IndexValue; 
  /* retrieve key: aclVlanAclId */  

  aclTypeSize = sizeof (aclType);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLVlanTable_aclVlanAclType,
                          (xLibU8_t *) & aclType, &aclTypeSize);
  
  owa.len = sizeof (objaclVlanAclIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLVlanTable_aclVlanAclId,
                          (xLibU8_t *) & objaclVlanAclIdValue, &owa.len);

 
  if(aclType == L7_ACL_TYPE_IP)
  {
      if(owa.rc != XLIBRC_SUCCESS)
      {
             objaclVlanAclIdValue =0;
             FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
             owa.l7rc = usmDbQosAclNumGetFirst(L7_UNIT_CURRENT, &objaclVlanAclIdValue);
             if(owa.l7rc !=  L7_SUCCESS)
             { 
                owa.l7rc =usmDbQosAclNamedIndexGetFirst(L7_UNIT_CURRENT, L7_ACL_TYPE_IP, &objaclVlanAclIdValue);
             }
             nextObjaclVlanAclIdValue = objaclVlanAclIdValue;
      }
      else
      {
             FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclVlanAclIdValue, owa.len);
             owa.l7rc = usmDbQosAclNumGetNext(L7_UNIT_CURRENT,objaclVlanAclIdValue,&nextObjaclVlanAclIdValue);
             if(owa.l7rc != L7_SUCCESS)
             {
                    owa.l7rc = usmDbQosAclNamedIndexGetNext(L7_UNIT_CURRENT, L7_ACL_TYPE_IP,objaclVlanAclIdValue, &nextObjaclVlanAclIdValue);
             }
      }
  }
  else if(aclType == L7_ACL_TYPE_MAC)
  {
      if (owa.rc != XLIBRC_SUCCESS)
     {
            FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
            objaclVlanAclIdValue =0;
            nextObjaclVlanAclIdValue =0;
            owa.l7rc = usmDbQosAclMacIndexGetFirst(L7_UNIT_CURRENT,&nextObjaclVlanAclIdValue) ;

     }
     else
     {
             FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclVlanAclIdValue, owa.len);
             owa.l7rc =  usmDbQosAclMacIndexGetNext( L7_UNIT_CURRENT,objaclVlanAclIdValue,&nextObjaclVlanAclIdValue);                                                            
     }

  }
  else if(aclType == L7_ACL_TYPE_IPV6)
  {
       
      if (owa.rc != XLIBRC_SUCCESS)
      {
         FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
         nextObjaclIpv6IndexValue =0;
          owa.l7rc = usmDbQosAclNamedIndexGetFirst(L7_UNIT_CURRENT,L7_ACL_TYPE_IPV6,&nextObjaclIpv6IndexValue);
       }
      else
      {
          FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclIpv6IndexValue, owa.len);
          owa.l7rc = usmDbQosAclNamedIndexGetNext(L7_UNIT_CURRENT,L7_ACL_TYPE_IPV6,objaclVlanAclIdValue,&nextObjaclIpv6IndexValue);
      }
      nextObjaclVlanAclIdValue = nextObjaclIpv6IndexValue;
  }
  else
  {
   owa.l7rc = L7_FAILURE;
  }                  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, nextObjaclVlanAclIdValue, owa.len);

  /* return the object value: aclVlanAclId */
   owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjaclVlanAclIdValue,
                           sizeof (nextObjaclVlanAclIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACLVlanTable_aclVlanAclIdOrName
*
* @purpose Get 'aclVlanAclIdOrName'
 *@description  [aclVlanAclIdOrName] The ACL identifier value, which can also be
* name for MAC and IP Named ACL's  is interpreted based on the
* aclID object.Here aclVlanAclId represents aclIndex.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACLVlanTable_aclVlanAclIdOrName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objaclVlanAclIdOrNameValue;
  xLibU32_t aclType;
  xLibU16_t aclTypeSize = sizeof(aclType);

  xLibU32_t keyaclVlanAclIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclVlanAclId */
  owa.len = sizeof (keyaclVlanAclIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLVlanTable_aclVlanAclId,
                          (xLibU8_t *) & keyaclVlanAclIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclVlanAclIdValue, owa.len);
  /* get the value from application */
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLVlanTable_aclVlanAclType,(xLibU8_t *) & aclType,&aclTypeSize);
  /* get the value from application */
  if(aclType == L7_ACL_TYPE_IP)
  {
      if (usmDbQosAclNamedIndexCheckValid(L7_UNIT_CURRENT, L7_ACL_TYPE_IP, keyaclVlanAclIdValue) == L7_SUCCESS)
     {
         owa.l7rc= usmDbQosAclNameGet(L7_UNIT_CURRENT,keyaclVlanAclIdValue,objaclVlanAclIdOrNameValue);
     }
     /* acl is not Named ACL hence convert the acl Number from integer  and assign it to */
     else
     {
         memset(objaclVlanAclIdOrNameValue,0,sizeof(objaclVlanAclIdOrNameValue));
         sprintf(objaclVlanAclIdOrNameValue,"%u",keyaclVlanAclIdValue);
         owa.l7rc = L7_SUCCESS;
     }

  }
  else if(aclType == L7_ACL_TYPE_MAC)
  {
     owa.l7rc = usmDbQosAclMacNameGet(L7_UNIT_CURRENT, keyaclVlanAclIdValue,
                              (L7_uchar8 *)objaclVlanAclIdOrNameValue);
  }
  else if(aclType == L7_ACL_TYPE_IPV6)
  {
     if (usmDbQosAclNamedIndexCheckValid(L7_UNIT_CURRENT, L7_ACL_TYPE_IPV6, keyaclVlanAclIdValue) == L7_SUCCESS)
     {
        owa.l7rc= usmDbQosAclNameGet(L7_UNIT_CURRENT,keyaclVlanAclIdValue,objaclVlanAclIdOrNameValue);
     }
     else
     {
       memset(objaclVlanAclIdOrNameValue,0,sizeof(objaclVlanAclIdOrNameValue));
       sprintf(objaclVlanAclIdOrNameValue,"%u",keyaclVlanAclIdValue);
       owa.l7rc = L7_SUCCESS;
     }
  }
  else
  {
      memset(objaclVlanAclIdOrNameValue,0,sizeof(objaclVlanAclIdOrNameValue));
      owa.l7rc = L7_SUCCESS;
  }


  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objaclVlanAclIdOrNameValue, strlen (objaclVlanAclIdOrNameValue));

  /* return the object value: aclVlanAclIdOrName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objaclVlanAclIdOrNameValue,
                           strlen (objaclVlanAclIdOrNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACLVlanTable_aclVlanAclIdOrName
*
* @purpose Set 'aclVlanAclIdOrName'
 *@description  [aclVlanAclIdOrName] The ACL identifier value, which can also be
* name for MAC and IP Named ACL's  is interpreted based on the
* aclID object.Here aclVlanAclId represents aclIndex.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACLVlanTable_aclVlanAclIdOrName (void *wap, void *bufp)
{
  return XLIBRC_SUCCESS;
#if 0
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objaclVlanAclIdOrNameValue;

  xLibU32_t keyaclVlanAclIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclVlanAclIdOrName */
  owa.len = sizeof (objaclVlanAclIdOrNameValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objaclVlanAclIdOrNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objaclVlanAclIdOrNameValue, owa.len);

  /* retrieve key: aclVlanAclId */
  owa.len = sizeof (keyaclVlanAclIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLVlanTable_aclVlanAclId,
                          (xLibU8_t *) & keyaclVlanAclIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclVlanAclIdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, keyaclVlanAclIdValue, objaclVlanAclIdOrNameValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif
}
/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACLVlanTable_aclVlanAclAssociated
*
* @purpose Get 'aclVlanAclAssociated'
 *@description  [aclVlanAclAssociated] 
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACLVlanTable_aclVlanAclAssociated (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objaclVlanAclAssociated;

  xLibU32_t keyaclVlanIndexValue;
  xLibU32_t keyaclVlanDirectionValue;
  xLibU32_t keyaclVlanAclTypeValue;
  xLibU32_t keyaclVlanAclIdValue;
  xLibU8_t filter = L7_TRUE;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclVlanIndex */
  owa.len = sizeof (keyaclVlanIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLVlanTable_aclVlanIndex,
                          (xLibU8_t *) & keyaclVlanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    filter = L7_FALSE;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclVlanIndexValue, owa.len);

  /* retrieve key: aclVlanDirection */
  owa.len = sizeof (keyaclVlanDirectionValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLVlanTable_aclVlanDirection,
                          (xLibU8_t *) & keyaclVlanDirectionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    filter = L7_FALSE;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclVlanDirectionValue, owa.len);

  /* retrieve key: aclVlanAclType */
  owa.len = sizeof (keyaclVlanAclTypeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLVlanTable_aclVlanAclType,
                          (xLibU8_t *) & keyaclVlanAclTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    filter = L7_FALSE;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclVlanAclTypeValue, owa.len);

  /* retrieve key: aclVlanAclId */
  owa.len = sizeof (keyaclVlanAclIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLVlanTable_aclVlanAclId,
(xLibU8_t *) & keyaclVlanAclIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    filter = L7_FALSE;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclVlanAclIdValue, owa.len);
  if (filter != L7_TRUE)
  {
     objaclVlanAclAssociated = L7_FALSE;
     owa.rc = XLIBRC_SUCCESS;
     /* return the object value: aclVlanStatus */
     owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclVlanAclAssociated,
                            sizeof (objaclVlanAclAssociated));
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
 
  }
  /* get the value from application */
  objaclVlanAclAssociated = L7_FALSE;
  if (L7_ACL_TYPE_MAC == keyaclVlanAclTypeValue)
  {
     if (usmDbQosMacAclVlanDirCheckValid(L7_UNIT_CURRENT, keyaclVlanAclIdValue,
         keyaclVlanIndexValue, keyaclVlanDirectionValue) == L7_SUCCESS)
     {
        objaclVlanAclAssociated = L7_TRUE;
     }
  }
  else if ((L7_ACL_TYPE_IP == keyaclVlanAclTypeValue)||(L7_ACL_TYPE_IPV6 == keyaclVlanAclTypeValue))
  {
     if (usmDbQosAclVlanDirCheckValid(L7_UNIT_CURRENT, keyaclVlanAclIdValue,
         keyaclVlanIndexValue, keyaclVlanDirectionValue) == L7_SUCCESS)
     {
        objaclVlanAclAssociated = L7_TRUE;
     }
    
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclVlanAclAssociated, sizeof (objaclVlanAclAssociated));

  /* return the object value: aclVlanStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclVlanAclAssociated,
                           sizeof (objaclVlanAclAssociated));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACLVlanTable_aclVlanStatus
*
* @purpose Get 'aclVlanStatus'
 *@description  [aclVlanStatus] Status of this instance. active(1) - this ACL
* Vlan instance is active createAndGo(4) - set to this value to
* assign an ACL to a Vlan and direction destroy(6) - set to this value
* to remove an ACL from a Vlan and direction   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACLVlanTable_aclVlanStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objaclVlanStatusValue;

  xLibU32_t keyaclVlanIndexValue;
  xLibU32_t keyaclVlanDirectionValue;
  xLibU32_t keyaclVlanSequenceValue;
  xLibU32_t keyaclVlanAclTypeValue;
  xLibU32_t keyaclVlanAclIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclVlanIndex */
  owa.len = sizeof (keyaclVlanIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLVlanTable_aclVlanIndex,
                          (xLibU8_t *) & keyaclVlanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclVlanIndexValue, owa.len);

  /* retrieve key: aclVlanDirection */
  owa.len = sizeof (keyaclVlanDirectionValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLVlanTable_aclVlanDirection,
                          (xLibU8_t *) & keyaclVlanDirectionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclVlanDirectionValue, owa.len);

  /* retrieve key: aclVlanSequence */
  owa.len = sizeof (keyaclVlanSequenceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLVlanTable_aclVlanSequence,
                          (xLibU8_t *) & keyaclVlanSequenceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclVlanSequenceValue, owa.len);

  /* retrieve key: aclVlanAclType */
  owa.len = sizeof (keyaclVlanAclTypeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLVlanTable_aclVlanAclType,
                          (xLibU8_t *) & keyaclVlanAclTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclVlanAclTypeValue, owa.len);

  /* retrieve key: aclVlanAclId */
  owa.len = sizeof (keyaclVlanAclIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLVlanTable_aclVlanAclId,
                          (xLibU8_t *) & keyaclVlanAclIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclVlanAclIdValue, owa.len);

  /* get the value from application */
  objaclVlanStatusValue = L7_ROW_STATUS_ACTIVE;

  FPOBJ_TRACE_VALUE (bufp, &objaclVlanStatusValue, sizeof (objaclVlanStatusValue));

  /* return the object value: aclVlanStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclVlanStatusValue,
                           sizeof (objaclVlanStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACLVlanTable_aclVlanStatus
*
* @purpose Set 'aclVlanStatus'
 *@description  [aclVlanStatus] Status of this instance. active(1) - this ACL
* Vlan instance is active createAndGo(4) - set to this value to
* assign an ACL to a Vlan and direction destroy(6) - set to this value
* to remove an ACL from a Vlan and direction
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACLVlanTable_aclVlanStatus (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objaclVlanStatusValue;

  xLibU32_t keyaclVlanIndexValue;
  xLibU32_t keyaclVlanDirectionValue;
  xLibU32_t keyaclVlanSequenceValue;
  xLibU32_t keyaclVlanAclTypeValue;
  xLibU32_t keyaclVlanAclIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclVlanStatus */
  owa.len = sizeof (objaclVlanStatusValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objaclVlanStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclVlanStatusValue, owa.len);

  /* retrieve key: aclVlanIndex */
  owa.len = sizeof (keyaclVlanIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLVlanTable_aclVlanIndex,
                          (xLibU8_t *) & keyaclVlanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(keyaclVlanIndexValue == 1)
  {
	owa.rc = XLIBRC_ERR_BIND_ACL_MGMT_VLAN;
	FPOBJ_TRACE_EXIT (bufp, owa);
	return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclVlanIndexValue, owa.len);

  /* retrieve key: aclVlanDirection */
  owa.len = sizeof (keyaclVlanDirectionValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLVlanTable_aclVlanDirection,
                          (xLibU8_t *) & keyaclVlanDirectionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclVlanDirectionValue, owa.len);

  /* retrieve key: aclVlanSequence */
  owa.len = sizeof (keyaclVlanSequenceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLVlanTable_aclVlanSequence,
                          (xLibU8_t *) & keyaclVlanSequenceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclVlanSequenceValue, owa.len);

  /* retrieve key: aclVlanAclType */
  owa.len = sizeof (keyaclVlanAclTypeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLVlanTable_aclVlanAclType,
                          (xLibU8_t *) & keyaclVlanAclTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclVlanAclTypeValue, owa.len);

  /* retrieve key: aclVlanAclId */
  owa.len = sizeof (keyaclVlanAclIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLVlanTable_aclVlanAclId,
                          (xLibU8_t *) & keyaclVlanAclIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_NO_ACLS_CONFIGURED;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclVlanAclIdValue, owa.len);

  owa.l7rc = L7_SUCCESS;
  if (objaclVlanStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {

    owa.l7rc =  fpobj_QosAclVlanAdd(L7_UNIT_CURRENT, keyaclVlanIndexValue,
                                keyaclVlanDirectionValue,
                                keyaclVlanSequenceValue,
                                keyaclVlanAclTypeValue,
                                keyaclVlanAclIdValue);
  }
 else if (objaclVlanStatusValue == L7_ROW_STATUS_DESTROY)
  {
    owa.l7rc = fpobj_QosAclVlanRemove(L7_UNIT_CURRENT, keyaclVlanIndexValue,
                                   keyaclVlanDirectionValue,
                                   keyaclVlanSequenceValue,
                                   keyaclVlanAclTypeValue,
                                   keyaclVlanAclIdValue);
  }
  
  if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_ACL_ASSIGN_FAILED;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

