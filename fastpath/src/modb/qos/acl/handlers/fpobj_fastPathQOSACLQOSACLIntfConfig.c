
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_fastPathQOSACLQOSACLIntfConfig.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to fastPathQOSACL-object.xml
*
* @create  23 July 2008, Wednesday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_fastPathQOSACLQOSACLIntfConfig_obj.h"
#include "usmdb_qos_acl_api.h"
#include "acl_exports.h"
#include "usmdb_util_api.h"
#include "usmdb_mib_vlan_api.h"

L7_RC_t fpobjUtil_QosAclIntfIfNext(L7_uint32 UnitIndex, L7_uint32 *aclIfIndex, L7_uint32 *aclIfDirection,
                   L7_uint32 *aclIfSequence, L7_uint32 *aclIfAclType, L7_uint32 *aclIfAclId);

L7_RC_t fpobj_QosAclIntfIfRemove(L7_uint32 UnitIndex, L7_uint32 aclIfIndex, L7_uint32 aclIfDirection,
                        L7_uint32 aclIfSequence, L7_uint32 aclIfAclType, L7_uint32 aclIfAclId);

L7_RC_t fpobj_QosAclIntfIfAdd(L7_uint32 UnitIndex, L7_uint32 aclIfIndex, L7_uint32 aclIfDirection,
                  L7_uint32 aclIfSequence, L7_uint32 aclIfAclType, L7_uint32 aclIfAclId, L7_uint32 *aclDiffservFlag);
L7_RC_t fpobjUtil_QosAclIntfIfACLIDTypeNext(L7_uint32 UnitIndex, L7_uint32 aclIfIndex, L7_uint32 aclIfDirection,
                  L7_uint32 aclIfSequence, L7_uint32 *nextaclIfAclType, L7_uint32 *nextaclIfAclId);

L7_RC_t fpobjUtil_QosAclIntfIfNext(L7_uint32 UnitIndex, L7_uint32 *aclIfIndex, L7_uint32 *aclIfDirection,
                   L7_uint32 *aclIfSequence, L7_uint32 *aclIfAclType, L7_uint32 *aclIfAclId)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 count = 0;

  L7_ACL_INTF_DIR_LIST_t listInfo;

  L7_uint32 nextAclIfIndex,nextAclIfDirection,nextAclIfSequence;

  L7_uint32 temp_direction;
  if(*aclIfDirection  == -1)
  {
    temp_direction = L7_INBOUND_ACL;
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

        if((rc == L7_SUCCESS) )
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
L7_RC_t fpobjUtil_QosAclIntfIfACLIDTypeNext(L7_uint32 UnitIndex, L7_uint32 aclIfIndex, L7_uint32 aclIfDirection,
                  L7_uint32 aclIfSequence, L7_uint32 *nextaclIfAclType, L7_uint32 *nextaclIfAclId)
{
   L7_RC_t rc;
   L7_ACL_INTF_DIR_LIST_t listInfo;
   L7_uint32 aclIfAclType = *nextaclIfAclType;
   L7_uint32 aclIfAclId = *nextaclIfAclId;
   L7_uint32 count=0;
    
   if((rc  = usmDbQosAclIntfDirGet( UnitIndex, aclIfIndex, aclIfDirection ))== L7_SUCCESS)
   {
      if((rc = usmDbQosAclIntfDirSequenceGet(UnitIndex, aclIfIndex, aclIfDirection ,aclIfSequence)) ==L7_SUCCESS)
      {
          rc =  usmDbQosAclIntfDirAclListGet( UnitIndex,aclIfIndex,aclIfDirection,&listInfo); 
          if(rc == L7_SUCCESS)
          for(count = 0;count < listInfo.count;count++)
          {
               rc = L7_FAILURE;
               if(listInfo.listEntry[count].seqNum == aclIfSequence)
               {
                    rc = L7_SUCCESS;
                    break;
               }
          }
          if((rc == L7_SUCCESS) && (listInfo.listEntry[count].aclType == aclIfAclType)
                        && (listInfo.listEntry[count].aclId == aclIfAclId))
          {
                 rc = L7_FAILURE;
          }
          else if(rc == L7_SUCCESS)
          {
                *nextaclIfAclType = listInfo.listEntry[count].aclType;
                *nextaclIfAclId = listInfo.listEntry[count].aclId;
                rc = L7_SUCCESS;
          }      
       }
    }
    return rc;
}
L7_RC_t fpobj_QosAclIntfIfAdd(L7_uint32 UnitIndex, L7_uint32 aclIfIndex, L7_uint32 aclIfDirection,
                  L7_uint32 aclIfSequence, L7_uint32 aclIfAclType, L7_uint32 aclIfAclId,  L7_uint32 *aclDiffservFlag)
{
  L7_RC_t rc = L7_SUCCESS;


  if((aclIfDirection != L7_INBOUND_ACL) && (aclIfDirection != L7_OUTBOUND_ACL))
  {
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    if (usmDbQosAclIsDiffServIntfInUse(UnitIndex, aclIfIndex, aclIfDirection) == L7_TRUE &&
    usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                               L7_ACL_INTF_DIFFSERV_COEXIST_FEATURE_ID) == L7_FALSE)
    {
      rc = L7_FAILURE;
      *aclDiffservFlag = 1;
    }
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

L7_RC_t fpobj_QosAclIntfIfRemove(L7_uint32 UnitIndex, L7_uint32 aclIfIndex, L7_uint32 aclIfDirection,
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
* @function fpObjGet_fastPathQOSACLQOSACLIntfConfig_aclIntfIfIndex
*
* @purpose Get 'aclIntfIfIndex'
 *@description  [aclIntfIfIndex] The interface to which this ACL instance
* applies.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACLIntfConfig_aclIntfIfIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objaclIntfIfIndexValue;
  xLibU32_t nextObjaclIntfIfIndexValue;

  xLibU32_t objaclIntfIfDirectionValue;
  xLibU32_t nextObjaclIntfIfDirectionValue;
  xLibU32_t objaclIntfIfSequenceValue;
  xLibU32_t nextObjaclIntfIfSequenceValue;
  xLibU32_t objaclIntfIfAclTypeValue;
  xLibU32_t nextObjaclIntfIfAclTypeValue;
  xLibU32_t objaclIntfIfAclIdValue;
  xLibU32_t nextObjaclIntfIfAclIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIntfIfIndex */
  owa.len = sizeof (objaclIntfIfIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLIntfConfig_aclIntfIfIndex,
                          (xLibU8_t *) & objaclIntfIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    FPOBJ_CLR_U32 (objaclIntfIfIndexValue);
    FPOBJ_CLR_U32 (objaclIntfIfDirectionValue);
    FPOBJ_CLR_U32 (nextObjaclIntfIfIndexValue);
    FPOBJ_CLR_U32 (nextObjaclIntfIfDirectionValue);
    objaclIntfIfDirectionValue = L7_INBOUND_ACL;
    objaclIntfIfDirectionValue = L7_INBOUND_ACL;
    FPOBJ_CLR_U32 (objaclIntfIfSequenceValue);
    FPOBJ_CLR_U32 (objaclIntfIfAclTypeValue);
    FPOBJ_CLR_U32 (objaclIntfIfAclIdValue);
    FPOBJ_CLR_U32 (nextObjaclIntfIfSequenceValue);
    FPOBJ_CLR_U32 (nextObjaclIntfIfAclTypeValue);
    FPOBJ_CLR_U32 (nextObjaclIntfIfAclIdValue);
    owa.l7rc = fpobjUtil_QosAclIntfIfNext(L7_UNIT_CURRENT,&objaclIntfIfIndexValue,&objaclIntfIfDirectionValue,
                  &objaclIntfIfSequenceValue,&objaclIntfIfAclTypeValue,
                  &objaclIntfIfAclIdValue); 
    if(owa.l7rc == L7_SUCCESS)
   {
      nextObjaclIntfIfIndexValue= objaclIntfIfIndexValue;

   }

  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclIntfIfIndexValue, owa.len);
    nextObjaclIntfIfIndexValue = objaclIntfIfIndexValue;
    FPOBJ_CLR_U32 (objaclIntfIfDirectionValue);
    objaclIntfIfDirectionValue = L7_INBOUND_ACL;
    FPOBJ_CLR_U32 (objaclIntfIfSequenceValue);
    FPOBJ_CLR_U32 (objaclIntfIfAclTypeValue);
    FPOBJ_CLR_U32 (objaclIntfIfAclIdValue);
    FPOBJ_CLR_U32 (nextObjaclIntfIfSequenceValue);
    FPOBJ_CLR_U32 (nextObjaclIntfIfDirectionValue);
    do
    {
      owa.l7rc = fpobjUtil_QosAclIntfIfNext(L7_UNIT_CURRENT,&nextObjaclIntfIfIndexValue,&nextObjaclIntfIfDirectionValue,
                  &nextObjaclIntfIfSequenceValue,&nextObjaclIntfIfAclTypeValue,
                  &nextObjaclIntfIfAclIdValue);
      FPOBJ_CPY_U32 (objaclIntfIfDirectionValue, nextObjaclIntfIfDirectionValue);
      FPOBJ_CPY_U32 (objaclIntfIfSequenceValue, nextObjaclIntfIfSequenceValue);
      FPOBJ_CPY_U32 (objaclIntfIfAclTypeValue, nextObjaclIntfIfAclTypeValue);
      FPOBJ_CPY_U32 (objaclIntfIfAclIdValue, nextObjaclIntfIfAclIdValue);
    }
    while (FPOBJ_CMP_U32 (objaclIntfIfIndexValue, nextObjaclIntfIfIndexValue)
           && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjaclIntfIfIndexValue, owa.len);

  /* return the object value: aclIntfIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjaclIntfIfIndexValue,
                           sizeof (nextObjaclIntfIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjList_fastPathQOSACLQOSACLIntfConfig_aclIntfIfIndex
*
* @purpose List 'aclIntfIfIndex'
 *@description  [aclIntfIfIndex] The interface to which this ACL instance
* applies.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_fastPathQOSACLQOSACLIntfConfig_aclIntfIfIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objaclIntfIfIndexValue;
  xLibU32_t nextObjaclIntfIfIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof (objaclIntfIfIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLIntfConfig_aclIntfIfIndex,
                          (xLibU8_t *) & objaclIntfIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbIntIfNumTypeFirstGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF,
                                 0, &nextObjaclIntfIfIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclIntfIfIndexValue, owa.len);
    owa.l7rc =usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT,  USM_PHYSICAL_INTF | USM_LAG_INTF,
                                  0,objaclIntfIfIndexValue, &nextObjaclIntfIfIndexValue);    
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjaclIntfIfIndexValue, owa.len);

  /* return the object value: aclIntfIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjaclIntfIfIndexValue,
                           sizeof (nextObjaclIntfIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACLIntfConfig_aclIntfIfDirection
*
* @purpose Get 'aclIntfIfDirection'
 *@description  [aclIntfIfDirection] The interface direction to which this ACL
* instance applies.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACLIntfConfig_aclIntfIfDirection (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objaclIntfIfDirectionValue;
  xLibU32_t nextObjaclIntfIfDirectionValue;

  xLibU32_t objaclIntfIfIndexValue;
  xLibU32_t nextObjaclIntfIfIndexValue;
  xLibU32_t objaclIntfIfSequenceValue;
  xLibU32_t nextObjaclIntfIfSequenceValue;
  xLibU32_t objaclIntfIfAclTypeValue;
  xLibU32_t nextObjaclIntfIfAclTypeValue;
  xLibU32_t objaclIntfIfAclIdValue;
  xLibU32_t nextObjaclIntfIfAclIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIntfIfIndex */
  owa.len = sizeof (objaclIntfIfIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLIntfConfig_aclIntfIfIndex,
                          (xLibU8_t *) & objaclIntfIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclIntfIfIndexValue, owa.len);

  /* retrieve key: aclIntfIfDirection */
  owa.len = sizeof (objaclIntfIfDirectionValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLIntfConfig_aclIntfIfDirection,
                          (xLibU8_t *) & objaclIntfIfDirectionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    FPOBJ_CLR_U32 (objaclIntfIfDirectionValue);
    FPOBJ_CLR_U32 (nextObjaclIntfIfDirectionValue);
    FPOBJ_CLR_U32 (objaclIntfIfSequenceValue);
    FPOBJ_CLR_U32 (objaclIntfIfAclTypeValue);
    FPOBJ_CLR_U32 (objaclIntfIfAclIdValue);    
    FPOBJ_CLR_U32 (nextObjaclIntfIfSequenceValue);
    FPOBJ_CLR_U32 (nextObjaclIntfIfAclTypeValue);
    FPOBJ_CLR_U32 (nextObjaclIntfIfAclIdValue);
    nextObjaclIntfIfIndexValue =  objaclIntfIfIndexValue;    
    nextObjaclIntfIfDirectionValue = -1;
    objaclIntfIfDirectionValue = -1;
    #if 0
    owa.l7rc = fpobjUtil_QosAclIntfIfNext(L7_UNIT_CURRENT,&objaclIntfIfIndexValue,&objaclIntfIfDirectionValue,
                  &objaclIntfIfSequenceValue,&objaclIntfIfAclTypeValue,
                  &objaclIntfIfAclIdValue); 
    if( owa.l7rc == L7_SUCCESS)
    {
        nextObjaclIntfIfDirectionValue = objaclIntfIfDirectionValue;
    }    
    #endif
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclIntfIfDirectionValue, owa.len);
    FPOBJ_CLR_U32 (objaclIntfIfSequenceValue);
    FPOBJ_CLR_U32 (objaclIntfIfAclTypeValue);
    FPOBJ_CLR_U32 (objaclIntfIfAclIdValue);
    FPOBJ_CLR_U32 (nextObjaclIntfIfSequenceValue);
    nextObjaclIntfIfIndexValue = objaclIntfIfIndexValue;
    nextObjaclIntfIfDirectionValue = objaclIntfIfDirectionValue;
  }
  do
  {
      owa.l7rc = fpobjUtil_QosAclIntfIfNext(L7_UNIT_CURRENT,
                                      &nextObjaclIntfIfIndexValue,
                                      &nextObjaclIntfIfDirectionValue,
                                      &nextObjaclIntfIfSequenceValue, &nextObjaclIntfIfAclTypeValue,
                                      &nextObjaclIntfIfAclIdValue);
      FPOBJ_CPY_U32 (objaclIntfIfSequenceValue, nextObjaclIntfIfSequenceValue);
      FPOBJ_CPY_U32 (objaclIntfIfAclTypeValue, nextObjaclIntfIfAclTypeValue);
      FPOBJ_CPY_U32 (objaclIntfIfAclIdValue, nextObjaclIntfIfAclIdValue);
  }
    while (FPOBJ_CMP_U32 (objaclIntfIfIndexValue, nextObjaclIntfIfIndexValue)
           && FPOBJ_CMP_U32 (objaclIntfIfDirectionValue, nextObjaclIntfIfDirectionValue)
           && (owa.l7rc == L7_SUCCESS));  

  if ((!FPOBJ_CMP_U32 (objaclIntfIfIndexValue, nextObjaclIntfIfIndexValue))
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjaclIntfIfDirectionValue, owa.len);

  /* return the object value: aclIntfIfDirection */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjaclIntfIfDirectionValue,
                           sizeof (nextObjaclIntfIfDirectionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjList_fastPathQOSACLQOSACLIntfConfig_aclIntfIfDirection
*
* @purpose Get 'aclIntfIfDirection'
 *@description  [aclIntfIfDirection] The interface direction to which this ACL
* instance applies.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_fastPathQOSACLQOSACLIntfConfig_aclIntfIfDirection (void *wap, void *bufp)
{
  
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objaclIntfIfDirectionValue;
  xLibU32_t nextaclIntfIfDirectionValue;

  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof (objaclIntfIfDirectionValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLIntfConfig_aclIntfIfDirection,
                          (xLibU8_t *) & objaclIntfIfDirectionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextaclIntfIfDirectionValue =  L7_INBOUND_ACL;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclIntfIfDirectionValue, owa.len);
   /*TODO: check the feature outbound */
    if(objaclIntfIfDirectionValue == L7_INBOUND_ACL)
    {
       owa.l7rc = L7_FAILURE;
       if( usmDbFeaturePresentCheck(L7_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_INTF_DIRECTION_OUTBOUND_FEATURE_ID) == L7_TRUE )
       { 
          nextaclIntfIfDirectionValue = L7_OUTBOUND_ACL;
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

  FPOBJ_TRACE_NEW_KEY (bufp, &nextaclIntfIfDirectionValue, owa.len);

  /* return the object value: aclIntfIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextaclIntfIfDirectionValue,
                           sizeof (nextaclIntfIfDirectionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACLIntfConfig_aclIntfIfSequence
*
* @purpose Get 'aclIntfIfSequence'
 *@description  [aclIntfIfSequence] The relative evaluation sequence of this ACL
* for this interface and direction. When multiple ACLs are allowed
* for a given interface and direction, the sequence number
* determines the order in which the list of ACLs are evaluated,with lower
* sequence number   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACLIntfConfig_aclIntfIfSequence (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objaclIntfIfSequenceValue;
  xLibU32_t nextObjaclIntfIfSequenceValue;

  xLibU32_t objaclIntfIfIndexValue;
  xLibU32_t nextObjaclIntfIfIndexValue;
  xLibU32_t objaclIntfIfDirectionValue;
  xLibU32_t nextObjaclIntfIfDirectionValue;
  xLibU32_t objaclIntfIfAclTypeValue;
  xLibU32_t nextObjaclIntfIfAclTypeValue;
  xLibU32_t objaclIntfIfAclIdValue;
  xLibU32_t nextObjaclIntfIfAclIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIntfIfIndex */
  owa.len = sizeof (objaclIntfIfIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLIntfConfig_aclIntfIfIndex,
                          (xLibU8_t *) & objaclIntfIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclIntfIfIndexValue, owa.len);

  /* retrieve key: aclIntfIfDirection */
  owa.len = sizeof (objaclIntfIfDirectionValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLIntfConfig_aclIntfIfDirection,
                          (xLibU8_t *) & objaclIntfIfDirectionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclIntfIfDirectionValue, owa.len);

  /* retrieve key: aclIntfIfSequence */
  owa.len = sizeof (objaclIntfIfSequenceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLIntfConfig_aclIntfIfSequence,
                          (xLibU8_t *) & objaclIntfIfSequenceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    FPOBJ_CLR_U32 (objaclIntfIfSequenceValue);
    FPOBJ_CLR_U32 (objaclIntfIfAclTypeValue);
    FPOBJ_CLR_U32 (objaclIntfIfAclIdValue);
    FPOBJ_CLR_U32 (nextObjaclIntfIfSequenceValue);
    FPOBJ_CLR_U32 (nextObjaclIntfIfAclTypeValue);
    FPOBJ_CLR_U32 (nextObjaclIntfIfAclIdValue);
    nextObjaclIntfIfIndexValue =  objaclIntfIfIndexValue;   
    nextObjaclIntfIfDirectionValue = objaclIntfIfDirectionValue ;
    #if 0
    owa.l7rc = fpobjUtil_QosAclIntfIfNext(L7_UNIT_CURRENT,&objaclIntfIfIndexValu,&objaclIntfIfDirectionValue,
                  &objaclIntfIfSequenceValue,&objaclIntfIfAclTypeValue,
                  &objaclIntfIfAclIdValue); 
   if(owa.l7rc == L7_SUCCESS)
   {
      nextObjaclIntfIfSequenceValue= objaclIntfIfSequenceValue;

   }
   #endif
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclIntfIfSequenceValue, owa.len);
    FPOBJ_CLR_U32 (objaclIntfIfAclTypeValue);
    FPOBJ_CLR_U32 (objaclIntfIfAclIdValue);
    nextObjaclIntfIfIndexValue = objaclIntfIfIndexValue;
    nextObjaclIntfIfDirectionValue = objaclIntfIfDirectionValue;
    nextObjaclIntfIfSequenceValue = objaclIntfIfSequenceValue;
  }
    do
    {
      owa.l7rc = fpobjUtil_QosAclIntfIfNext (L7_UNIT_CURRENT,
                                      &nextObjaclIntfIfIndexValue,
                                      &nextObjaclIntfIfDirectionValue,
                                      &nextObjaclIntfIfSequenceValue, &nextObjaclIntfIfAclTypeValue,
                                      &nextObjaclIntfIfAclIdValue);
      FPOBJ_CPY_U32 (objaclIntfIfAclTypeValue, nextObjaclIntfIfAclTypeValue);
      FPOBJ_CPY_U32 (objaclIntfIfAclIdValue, nextObjaclIntfIfAclIdValue);
    }
    while (FPOBJ_CMP_U32 (objaclIntfIfIndexValue, nextObjaclIntfIfIndexValue)
           && FPOBJ_CMP_U32 (objaclIntfIfDirectionValue, nextObjaclIntfIfDirectionValue)
           && FPOBJ_CMP_U32 (objaclIntfIfSequenceValue, nextObjaclIntfIfSequenceValue)
           && (owa.l7rc == L7_SUCCESS));  

  if ((!FPOBJ_CMP_U32 (objaclIntfIfIndexValue, nextObjaclIntfIfIndexValue))
      || (!FPOBJ_CMP_U32 (objaclIntfIfDirectionValue, nextObjaclIntfIfDirectionValue))
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjaclIntfIfSequenceValue, owa.len);

  /* return the object value: aclIntfIfSequence */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjaclIntfIfSequenceValue,
                           sizeof (nextObjaclIntfIfSequenceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACLIntfConfig_aclIntfIfAclType
*
* @purpose Get 'aclIntfIfAclType'
 *@description  [aclIntfIfAclType] The type of this ACL, which is used to
* interpret the aclIfId object value. Each type of ACL uses its own
* numbering scheme for identification (see aclIfAclId object for
* details). The aclIfAclId object must be specified along with this object.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACLIntfConfig_aclIntfIfAclType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objaclIntfIfAclTypeValue;
  xLibU32_t nextObjaclIntfIfAclTypeValue;

  xLibU32_t objaclIntfIfIndexValue;
  xLibU32_t nextObjaclIntfIfIndexValue;
  xLibU32_t objaclIntfIfDirectionValue;
  xLibU32_t nextObjaclIntfIfDirectionValue;
  xLibU32_t objaclIntfIfSequenceValue;
  xLibU32_t nextObjaclIntfIfSequenceValue;
  xLibU32_t objaclIntfIfAclIdValue;
  xLibU32_t nextObjaclIntfIfAclIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIntfIfIndex */
  owa.len = sizeof (objaclIntfIfIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLIntfConfig_aclIntfIfIndex,
                          (xLibU8_t *) & objaclIntfIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclIntfIfIndexValue, owa.len);

  /* retrieve key: aclIntfIfDirection */
  owa.len = sizeof (objaclIntfIfDirectionValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLIntfConfig_aclIntfIfDirection,
                          (xLibU8_t *) & objaclIntfIfDirectionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclIntfIfDirectionValue, owa.len);

  /* retrieve key: aclIntfIfSequence */
  owa.len = sizeof (objaclIntfIfSequenceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLIntfConfig_aclIntfIfSequence,
                          (xLibU8_t *) & objaclIntfIfSequenceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclIntfIfSequenceValue, owa.len);

  /* retrieve key: aclIntfIfAclType */
  owa.len = sizeof (objaclIntfIfAclTypeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLIntfConfig_aclIntfIfAclType,
                          (xLibU8_t *) & objaclIntfIfAclTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    FPOBJ_CLR_U32 (objaclIntfIfAclTypeValue);
    FPOBJ_CLR_U32 (objaclIntfIfAclIdValue);    
    FPOBJ_CLR_U32 (nextObjaclIntfIfAclTypeValue);
    FPOBJ_CLR_U32 (nextObjaclIntfIfAclIdValue);
    nextObjaclIntfIfIndexValue = objaclIntfIfIndexValue;
    nextObjaclIntfIfDirectionValue = objaclIntfIfDirectionValue;
    nextObjaclIntfIfSequenceValue = objaclIntfIfSequenceValue;    
    #if 0
    owa.l7rc = fpobjUtil_QosAclIntfIfNext(L7_UNIT_CURRENT,&objaclIntfIfIndexValue,&objaclIntfIfDirectionValue,
 
                  &objaclIntfIfSequenceValue,&objaclIntfIfAclTypeValue,
                  &objaclIntfIfAclIdValue); 
   if(owa.l7rc == L7_SUCCESS)
   {
      nextObjaclIntfIfAclTypeValue=objaclIntfIfAclTypeValue;

   } 
   #endif
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclIntfIfAclTypeValue, owa.len);
    FPOBJ_CLR_U32 (objaclIntfIfAclIdValue);
    FPOBJ_CLR_U32 (nextObjaclIntfIfAclIdValue);
    nextObjaclIntfIfIndexValue = objaclIntfIfIndexValue;
    nextObjaclIntfIfDirectionValue = objaclIntfIfDirectionValue;
    nextObjaclIntfIfSequenceValue = objaclIntfIfSequenceValue;
    nextObjaclIntfIfAclTypeValue = objaclIntfIfAclTypeValue;
  }
    do
    {
      
      owa.l7rc = fpobjUtil_QosAclIntfIfACLIDTypeNext(L7_UNIT_CURRENT,nextObjaclIntfIfIndexValue,nextObjaclIntfIfDirectionValue,nextObjaclIntfIfSequenceValue,&nextObjaclIntfIfAclTypeValue,&nextObjaclIntfIfAclIdValue);
      if(owa.l7rc != L7_SUCCESS)
      {
               owa.l7rc = fpobjUtil_QosAclIntfIfNext(L7_UNIT_CURRENT,
                                      &nextObjaclIntfIfIndexValue,
                                      &nextObjaclIntfIfDirectionValue,
                                      &nextObjaclIntfIfSequenceValue, &nextObjaclIntfIfAclTypeValue,
                                      &nextObjaclIntfIfAclIdValue);
      }
      FPOBJ_CPY_U32 (objaclIntfIfAclIdValue, nextObjaclIntfIfAclIdValue);
    }
    while (FPOBJ_CMP_U32 (objaclIntfIfIndexValue, nextObjaclIntfIfIndexValue)
           && FPOBJ_CMP_U32 (objaclIntfIfDirectionValue, nextObjaclIntfIfDirectionValue)
           && FPOBJ_CMP_U32 (objaclIntfIfSequenceValue, nextObjaclIntfIfSequenceValue)
           && FPOBJ_CMP_U32 (objaclIntfIfAclTypeValue, nextObjaclIntfIfAclTypeValue)
           && (owa.l7rc == L7_SUCCESS));

  if ((!FPOBJ_CMP_U32 (objaclIntfIfIndexValue, nextObjaclIntfIfIndexValue))
      || (!FPOBJ_CMP_U32 (objaclIntfIfDirectionValue, nextObjaclIntfIfDirectionValue))
      || (!FPOBJ_CMP_U32 (objaclIntfIfSequenceValue, nextObjaclIntfIfSequenceValue))
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjaclIntfIfAclTypeValue, owa.len);

  /* return the object value: aclIntfIfAclType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjaclIntfIfAclTypeValue,
                           sizeof (nextObjaclIntfIfAclTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjList_fastPathQOSACLQOSACLIntfConfig_aclIntfIfAclType
*
* @purpose List 'aclIntfIfAclType'
 *@description  [aclIntfIfAclType] The type of this ACL, which is used to
* interpret the aclIfId object value. Each type of ACL uses its own
* numbering scheme for identification (see aclIfAclId object for
* details). The aclIfAclId object must be specified along with this object.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_fastPathQOSACLQOSACLIntfConfig_aclIntfIfAclType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objaclIntfIfAclTypeValue;
  xLibU32_t nextObjaclIntfIfAclTypeValue;

  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof (objaclIntfIfAclTypeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLIntfConfig_aclIntfIfAclType,
                          (xLibU8_t *) & objaclIntfIfAclTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjaclIntfIfAclTypeValue = L7_ACL_TYPE_NONE;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclIntfIfAclTypeValue, owa.len);
    if(objaclIntfIfAclTypeValue == L7_ACL_TYPE_NONE)
    {
       nextObjaclIntfIfAclTypeValue = L7_ACL_TYPE_IP;
       owa.l7rc = L7_SUCCESS;
    }
    else if (objaclIntfIfAclTypeValue == L7_ACL_TYPE_IP)
    {
       nextObjaclIntfIfAclTypeValue = L7_ACL_TYPE_MAC;
       owa.l7rc = L7_SUCCESS;
    }
    else if(objaclIntfIfAclTypeValue == L7_ACL_TYPE_MAC)
    {
        if (usmDbFeaturePresentCheck(L7_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_TYPE_IPV6_FEATURE_ID) == L7_TRUE)
        {
        nextObjaclIntfIfAclTypeValue = L7_ACL_TYPE_IPV6;
        owa.l7rc = L7_SUCCESS;
        }
        else
        {
            owa.l7rc = L7_FAILURE;
        }

    }
    else if(objaclIntfIfAclTypeValue == L7_ACL_TYPE_IPV6)
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

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjaclIntfIfAclTypeValue, owa.len);

  /* return the object value: aclIntfIfAclType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjaclIntfIfAclTypeValue,
                           sizeof (nextObjaclIntfIfAclTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACLIntfConfig_aclIntfIfAclId
*
* @purpose Get 'aclIntfIfAclId'
 *@description  [aclIntfIfAclId] The ACL identifier value, which is interpreted
* based on the aclIfType object. For the IP ACLs, the actual ACL
* number is its identifier as follows: IP standard ranges from 1-99,
* while IP extended ranges from 100-199. Here, aclIfAclId
* represents aclIndex.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACLIntfConfig_aclIntfIfAclId (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();

  xLibU32_t objaclIntfIfIndexValue;
  xLibU32_t nextObjaclIntfIfIndexValue;
  xLibU32_t objaclIntfIfDirectionValue;
  xLibU32_t nextObjaclIntfIfDirectionValue;
  xLibU32_t objaclIntfIfSequenceValue;
  xLibU32_t nextObjaclIntfIfSequenceValue;
  xLibU32_t objaclIntfIfAclTypeValue;
  xLibU32_t nextObjaclIntfIfAclTypeValue;
  xLibU32_t objaclIntfIfAclIdValue;
  xLibU32_t nextObjaclIntfIfAclIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIntfIfIndex */
  owa.len = sizeof (objaclIntfIfIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLIntfConfig_aclIntfIfIndex,
                          (xLibU8_t *) & objaclIntfIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclIntfIfIndexValue, owa.len);

  /* retrieve key: aclIntfIfDirection */
  owa.len = sizeof (objaclIntfIfDirectionValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLIntfConfig_aclIntfIfDirection,
                          (xLibU8_t *) & objaclIntfIfDirectionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclIntfIfDirectionValue, owa.len);

  /* retrieve key: aclIntfIfSequence */
  owa.len = sizeof (objaclIntfIfSequenceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLIntfConfig_aclIntfIfSequence,
                          (xLibU8_t *) & objaclIntfIfSequenceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclIntfIfSequenceValue, owa.len);

  /* retrieve key: aclIntfIfAclType */
  owa.len = sizeof (objaclIntfIfAclTypeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLIntfConfig_aclIntfIfAclType,
                          (xLibU8_t *) & objaclIntfIfAclTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclIntfIfAclTypeValue, owa.len);

  /* retrieve key: aclIntfIfAclId */
  owa.len = sizeof (objaclIntfIfAclIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLIntfConfig_aclIntfIfAclId,
                          (xLibU8_t *) & objaclIntfIfAclIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    FPOBJ_CLR_U32 (objaclIntfIfAclIdValue);
    FPOBJ_CLR_U32 (nextObjaclIntfIfAclIdValue);
    #if 0
    owa.l7rc = fpobjUtil_QosAclIntfIfNext(L7_UNIT_CURRENT,&objaclIntfIfIndexValue,&objaclIntfIfDirectionValue,
                  &objaclIntfIfSequenceValue,&objaclIntfIfAclTypeValue,
                  &objaclIntfIfAclIdValue); 
    if(owa.l7rc == L7_SUCCESS)
   {
      nextObjaclIntfIfAclIdValue = objaclIntfIfAclTypeValue;

   }
   #endif
    nextObjaclIntfIfIndexValue = objaclIntfIfIndexValue;
    nextObjaclIntfIfDirectionValue = objaclIntfIfDirectionValue;
    nextObjaclIntfIfSequenceValue = objaclIntfIfSequenceValue;
    nextObjaclIntfIfAclTypeValue = objaclIntfIfAclTypeValue;
  
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclIntfIfAclIdValue, owa.len);
    nextObjaclIntfIfIndexValue = objaclIntfIfIndexValue;
    nextObjaclIntfIfDirectionValue = objaclIntfIfDirectionValue;
    nextObjaclIntfIfSequenceValue = objaclIntfIfSequenceValue;
    nextObjaclIntfIfAclTypeValue = objaclIntfIfAclTypeValue;
    nextObjaclIntfIfAclIdValue = objaclIntfIfAclIdValue;
  }
    owa.l7rc = fpobjUtil_QosAclIntfIfACLIDTypeNext(L7_UNIT_CURRENT,nextObjaclIntfIfIndexValue,nextObjaclIntfIfDirectionValue,nextObjaclIntfIfSequenceValue,&nextObjaclIntfIfAclTypeValue,&nextObjaclIntfIfAclIdValue);
    if(owa.l7rc != L7_SUCCESS)
    {
           owa.l7rc = fpobjUtil_QosAclIntfIfNext(L7_UNIT_CURRENT,
                                      &nextObjaclIntfIfIndexValue,
                                      &nextObjaclIntfIfDirectionValue,
                                      &nextObjaclIntfIfSequenceValue, &nextObjaclIntfIfAclTypeValue,
                                      &nextObjaclIntfIfAclIdValue);
    }

  if ((!FPOBJ_CMP_U32 (objaclIntfIfIndexValue, nextObjaclIntfIfIndexValue))
      || (!FPOBJ_CMP_U32 (objaclIntfIfDirectionValue, nextObjaclIntfIfDirectionValue))
      || (!FPOBJ_CMP_U32 (objaclIntfIfSequenceValue, nextObjaclIntfIfSequenceValue))
      || (!FPOBJ_CMP_U32 (objaclIntfIfAclTypeValue, nextObjaclIntfIfAclTypeValue))
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjaclIntfIfAclIdValue, owa.len);

  /* return the object value: aclIntfIfAclId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjaclIntfIfAclIdValue,
                           sizeof (nextObjaclIntfIfAclIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjList_fastPathQOSACLQOSACLIntfConfig_aclIntfIfAclId
*
* @purpose Get 'aclIntfIfAclId'
 *@description  [aclIntfIfAclId] The ACL identifier value, which is interpreted
* based on the aclIfType object. For the IP ACLs, the actual ACL
* number is its identifier as follows: IP standard ranges from 1-99,
* while IP extended ranges from 100-199. Here, aclIfAclId
* represents aclIndex.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_fastPathQOSACLQOSACLIntfConfig_aclIntfIfAclId (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objaclIntfIfAclIdValue;
  xLibU32_t nextObjaclIntfIfAclIdValue;
  xLibU32_t aclType;
  xLibU16_t aclTypeSize;
  xLibU32_t objaclIpv6IndexValue,nextObjaclIpv6IndexValue;
  /* retrieve key: aclVlanAclId */

  aclTypeSize = sizeof (aclType);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLIntfConfig_aclIntfIfAclType,
                          (xLibU8_t *) & aclType, &aclTypeSize);

  owa.len = sizeof (objaclIntfIfAclIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLIntfConfig_aclIntfIfAclId,
                          (xLibU8_t *) & objaclIntfIfAclIdValue, &owa.len);


  if(aclType == L7_ACL_TYPE_IP)
  {
      if(owa.rc != XLIBRC_SUCCESS)
      {
             objaclIntfIfAclIdValue =0;
             FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
             owa.l7rc = usmDbQosAclNumGetFirst(L7_UNIT_CURRENT, &objaclIntfIfAclIdValue);
             if(owa.l7rc !=  L7_SUCCESS)
             {
                owa.l7rc =usmDbQosAclNamedIndexGetFirst(L7_UNIT_CURRENT, L7_ACL_TYPE_IP, &objaclIntfIfAclIdValue);
             }
             nextObjaclIntfIfAclIdValue = objaclIntfIfAclIdValue;
      }
      else
      {
             FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclIntfIfAclIdValue, owa.len);
             owa.l7rc = usmDbQosAclNumGetNext(L7_UNIT_CURRENT,objaclIntfIfAclIdValue,&nextObjaclIntfIfAclIdValue);
             if(owa.l7rc != L7_SUCCESS)
             {
                    owa.l7rc = usmDbQosAclNamedIndexGetNext(L7_UNIT_CURRENT, L7_ACL_TYPE_IP,objaclIntfIfAclIdValue, &nextObjaclIntfIfAclIdValue);
             }
      }
  }
  else if(aclType == L7_ACL_TYPE_MAC)
  {
      if (owa.rc != XLIBRC_SUCCESS)
     {
            FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
            objaclIntfIfAclIdValue =0;
            nextObjaclIntfIfAclIdValue=0;
            owa.l7rc = usmDbQosAclMacIndexGetFirst(L7_UNIT_CURRENT,&nextObjaclIntfIfAclIdValue) ;

     }
     else
     {
             FPOBJ_TRACE_CURRENT_KEY (bufp, &objaclIntfIfAclIdValue, owa.len);
             owa.l7rc =  usmDbQosAclMacIndexGetNext( L7_UNIT_CURRENT,objaclIntfIfAclIdValue,&nextObjaclIntfIfAclIdValue);         
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
          objaclIpv6IndexValue = objaclIntfIfAclIdValue;          
          owa.l7rc = usmDbQosAclNamedIndexGetNext(L7_UNIT_CURRENT,L7_ACL_TYPE_IPV6,objaclIpv6IndexValue,&nextObjaclIpv6IndexValue);
      }
      nextObjaclIntfIfAclIdValue = nextObjaclIpv6IndexValue;
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
  FPOBJ_TRACE_NEW_KEY (bufp, nextObjaclIntfIfAclIdValue, owa.len);

  /* return the object value: aclVlanAclId */
   owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjaclIntfIfAclIdValue,
                           sizeof (nextObjaclIntfIfAclIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACLIntfConfig_aclIntfIfAclIdOrName
*
* @purpose Get 'aclIntfIfAclIdOrName'
 *@description  [aclIntfIfAclIdOrName] The ACL identifier value, which can also
* be name for MAC and IP Named ACL's  is interpreted based on the
* aclID object.Here, aclIfAclId represents aclIndex.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACLIntfConfig_aclIntfIfAclIdOrName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objaclIntfIfAclIdOrNameValue;
  xLibU32_t aclType;
  xLibU16_t aclTypeSize = sizeof(aclType);

  xLibU32_t keyaclIntfIfAclIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIntfIfAclId */
  owa.len = sizeof (keyaclIntfIfAclIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLIntfConfig_aclIntfIfAclId,
                          (xLibU8_t *) & keyaclIntfIfAclIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIntfIfAclIdValue, owa.len);
  
  /* get the value from application */  
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLIntfConfig_aclIntfIfAclType,(xLibU8_t *) & aclType,&aclTypeSize);
  /* get the value from application */
  if(aclType == L7_ACL_TYPE_IP)
  {
      if (usmDbQosAclNamedIndexCheckValid(L7_UNIT_CURRENT, L7_ACL_TYPE_IP, keyaclIntfIfAclIdValue) == L7_SUCCESS)
     {
         owa.l7rc= usmDbQosAclNameGet(L7_UNIT_CURRENT,keyaclIntfIfAclIdValue,objaclIntfIfAclIdOrNameValue);
     }
     /* acl is not Named ACL hence convert the acl Number from integer  and assign it to */
     else
     {
         memset(objaclIntfIfAclIdOrNameValue,0,sizeof(objaclIntfIfAclIdOrNameValue));
         sprintf(objaclIntfIfAclIdOrNameValue,"%u",keyaclIntfIfAclIdValue);
         owa.l7rc = L7_SUCCESS;
     }

  }
  else if(aclType == L7_ACL_TYPE_MAC)
  {
     owa.l7rc = usmDbQosAclMacNameGet(L7_UNIT_CURRENT, keyaclIntfIfAclIdValue,
                              (L7_uchar8 *)objaclIntfIfAclIdOrNameValue);
  }
  else if(aclType == L7_ACL_TYPE_IPV6)
  {
      
     if (usmDbQosAclNamedIndexCheckValid(L7_UNIT_CURRENT, L7_ACL_TYPE_IPV6, keyaclIntfIfAclIdValue) == L7_SUCCESS)
     {
         owa.l7rc= usmDbQosAclNameGet(L7_UNIT_CURRENT,keyaclIntfIfAclIdValue,objaclIntfIfAclIdOrNameValue);
     }
     else
     {
       memset(objaclIntfIfAclIdOrNameValue,0,sizeof(objaclIntfIfAclIdOrNameValue));
       sprintf(objaclIntfIfAclIdOrNameValue,"%u",keyaclIntfIfAclIdValue);
       owa.l7rc = L7_SUCCESS;
     }
  }
  else
  {
      memset(objaclIntfIfAclIdOrNameValue,0,sizeof(objaclIntfIfAclIdOrNameValue));      
      owa.l7rc = L7_SUCCESS;
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objaclIntfIfAclIdOrNameValue, strlen (objaclIntfIfAclIdOrNameValue));

  /* return the object value: aclIntfIfAclIdOrName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objaclIntfIfAclIdOrNameValue,
                           strlen (objaclIntfIfAclIdOrNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACLIntfConfig_aclIntfIfAclIdOrName
*
* @purpose Set 'aclIntfIfAclIdOrName'
 *@description  [aclIntfIfAclIdOrName] The ACL identifier value, which can also
* be name for MAC and IP Named ACL's  is interpreted based on the
* aclID object.Here, aclIfAclId represents aclIndex.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACLIntfConfig_aclIntfIfAclIdOrName (void *wap, void *bufp)
{
  return XLIBRC_SUCCESS;
#if 0
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objaclIntfIfAclIdOrNameValue;

  xLibU32_t keyaclIntfIfAclIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclIntfIfAclIdOrName */
  owa.len = sizeof (objaclIntfIfAclIdOrNameValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objaclIntfIfAclIdOrNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objaclIntfIfAclIdOrNameValue, owa.len);

  /* retrieve key: aclIntfIfAclId */
  owa.len = sizeof (keyaclIntfIfAclIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLIntfConfig_aclIntfIfAclId,
                          (xLibU8_t *) & keyaclIntfIfAclIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIntfIfAclIdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, keyaclIntfIfAclIdValue,
                              objaclIntfIfAclIdOrNameValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif
}
/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACLIntfConfig_aclIntfAclAssociated
*
* @purpose Get 'aclIntfAclAssociated'
 *@description  [aclIntfAclAssociated] Status of this instance.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACLIntfConfig_aclIntfAclAssociated (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objaclIntfAclAssociated;

  xLibU32_t keyaclIntfIfIndexValue;
  xLibU32_t keyaclIntfIfDirectionValue;
  xLibU32_t keyaclIntfIfAclTypeValue;
  xLibU32_t keyaclIntfIfAclIdValue;
  xLibU8_t filter = L7_TRUE;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIntfIfIndex */
  owa.len = sizeof (keyaclIntfIfIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLIntfConfig_aclIntfIfIndex,
                          (xLibU8_t *) & keyaclIntfIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    filter = L7_FALSE;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIntfIfIndexValue, owa.len);

  /* retrieve key: aclIntfIfDirection */
  owa.len = sizeof (keyaclIntfIfDirectionValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLIntfConfig_aclIntfIfDirection,
                          (xLibU8_t *) & keyaclIntfIfDirectionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    filter = L7_FALSE;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIntfIfDirectionValue, owa.len);

  /* retrieve key: aclIntfIfAclType */
  owa.len = sizeof (keyaclIntfIfAclTypeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLIntfConfig_aclIntfIfAclType,
                          (xLibU8_t *) & keyaclIntfIfAclTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    filter = L7_FALSE;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIntfIfAclTypeValue, owa.len);

  /* retrieve key: aclIntfIfAclId */
  owa.len = sizeof (keyaclIntfIfAclIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLIntfConfig_aclIntfIfAclId,
                          (xLibU8_t *) & keyaclIntfIfAclIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
   {
    filter = L7_FALSE;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIntfIfAclIdValue, owa.len);
  if (filter != L7_TRUE)
  {
    objaclIntfAclAssociated = L7_FALSE;
    owa.rc = XLIBRC_SUCCESS;
    /* return the object value: aclIntfAclAssociated */
    owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclIntfAclAssociated,
                           sizeof (objaclIntfAclAssociated));
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  /* get the value from application */
  objaclIntfAclAssociated = L7_FALSE;
  if (L7_ACL_TYPE_MAC == keyaclIntfIfAclTypeValue)
  {
     if (usmDbQosMacAclIntfDirCheckValid(L7_UNIT_CURRENT, keyaclIntfIfAclIdValue, 
                                   keyaclIntfIfIndexValue, keyaclIntfIfDirectionValue) == L7_SUCCESS)
     {
        objaclIntfAclAssociated = L7_TRUE; 
     }
  }
  else if ((L7_ACL_TYPE_IP == keyaclIntfIfAclTypeValue) ||
           (L7_ACL_TYPE_IPV6 == keyaclIntfIfAclTypeValue))
  {
    if (usmDbQosAclIntfDirCheckValid(L7_UNIT_CURRENT, keyaclIntfIfAclIdValue,
                                   keyaclIntfIfIndexValue, keyaclIntfIfDirectionValue) == L7_SUCCESS)
    {
       objaclIntfAclAssociated = L7_TRUE;
    } 
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclIntfAclAssociated, sizeof (objaclIntfAclAssociated));

  owa.rc = XLIBRC_SUCCESS;
  /* return the object value: aclIntfAclAssociated */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclIntfAclAssociated,
                           sizeof (objaclIntfAclAssociated));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSACLQOSACLIntfConfig_aclIntfIfStatus
*
* @purpose Get 'aclIntfIfStatus'
 *@description  [aclIntfIfStatus] Status of this instance.. It could be eitherr
* active, createAndGo or destroy.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSACLQOSACLIntfConfig_aclIntfIfStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objaclIntfIfStatusValue;

  xLibU32_t keyaclIntfIfIndexValue;
  xLibU32_t keyaclIntfIfDirectionValue;
  xLibU32_t keyaclIntfIfSequenceValue;
  xLibU32_t keyaclIntfIfAclTypeValue;
  xLibU32_t keyaclIntfIfAclIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: aclIntfIfIndex */
  owa.len = sizeof (keyaclIntfIfIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLIntfConfig_aclIntfIfIndex,
                          (xLibU8_t *) & keyaclIntfIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIntfIfIndexValue, owa.len);

  /* retrieve key: aclIntfIfDirection */
  owa.len = sizeof (keyaclIntfIfDirectionValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLIntfConfig_aclIntfIfDirection,
                          (xLibU8_t *) & keyaclIntfIfDirectionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIntfIfDirectionValue, owa.len);

  /* retrieve key: aclIntfIfSequence */
  owa.len = sizeof (keyaclIntfIfSequenceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLIntfConfig_aclIntfIfSequence,
                          (xLibU8_t *) & keyaclIntfIfSequenceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIntfIfSequenceValue, owa.len);

  /* retrieve key: aclIntfIfAclType */
  owa.len = sizeof (keyaclIntfIfAclTypeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLIntfConfig_aclIntfIfAclType,
                          (xLibU8_t *) & keyaclIntfIfAclTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIntfIfAclTypeValue, owa.len);

  /* retrieve key: aclIntfIfAclId */
  owa.len = sizeof (keyaclIntfIfAclIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLIntfConfig_aclIntfIfAclId,
                          (xLibU8_t *) & keyaclIntfIfAclIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIntfIfAclIdValue, owa.len);

  /* get the value from application */
  objaclIntfIfStatusValue = L7_ROW_STATUS_ACTIVE;
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objaclIntfIfStatusValue, sizeof (objaclIntfIfStatusValue));

  /* return the object value: aclIntfIfStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objaclIntfIfStatusValue,
                           sizeof (objaclIntfIfStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_fastPathQOSACLQOSACLIntfConfig_aclIntfIfStatus
*
* @purpose Set 'aclIntfIfStatus'
 *@description  [aclIntfIfStatus] Status of this instance.. It could be eitherr
* active, createAndGo or destroy.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSACLQOSACLIntfConfig_aclIntfIfStatus (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objaclIntfIfStatusValue;

  xLibU32_t keyaclIntfIfIndexValue;
  xLibU32_t keyaclIntfIfDirectionValue;
  xLibU32_t keyaclIntfIfSequenceValue;
  xLibU32_t keyaclIntfIfAclTypeValue;
  xLibU32_t keyaclIntfIfAclIdValue;
  L7_uint32 aclDiffservFlag = 0;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: aclIntfIfStatus */
  owa.len = sizeof (objaclIntfIfStatusValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objaclIntfIfStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objaclIntfIfStatusValue, owa.len);

  /* retrieve key: aclIntfIfIndex */
  owa.len = sizeof (keyaclIntfIfIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLIntfConfig_aclIntfIfIndex,
                          (xLibU8_t *) & keyaclIntfIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIntfIfIndexValue, owa.len);

  /* retrieve key: aclIntfIfDirection */
  owa.len = sizeof (keyaclIntfIfDirectionValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLIntfConfig_aclIntfIfDirection,
                          (xLibU8_t *) & keyaclIntfIfDirectionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIntfIfDirectionValue, owa.len);

  /* retrieve key: aclIntfIfSequence */
  owa.len = sizeof (keyaclIntfIfSequenceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLIntfConfig_aclIntfIfSequence,
                          (xLibU8_t *) & keyaclIntfIfSequenceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_ACL_INVALID_SEQUENCE_NUM;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIntfIfSequenceValue, owa.len);

  /* retrieve key: aclIntfIfAclType */
  owa.len = sizeof (keyaclIntfIfAclTypeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLIntfConfig_aclIntfIfAclType,
                          (xLibU8_t *) & keyaclIntfIfAclTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIntfIfAclTypeValue, owa.len);

  /* retrieve key: aclIntfIfAclId */
  owa.len = sizeof (keyaclIntfIfAclIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSACLQOSACLIntfConfig_aclIntfIfAclId,
                          (xLibU8_t *) & keyaclIntfIfAclIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_NO_ACLS_CONFIGURED;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyaclIntfIfAclIdValue, owa.len);

  owa.l7rc = L7_SUCCESS;
  if (objaclIntfIfStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
/* Create a row */
    owa.l7rc = fpobj_QosAclIntfIfAdd (L7_UNIT_CURRENT, keyaclIntfIfIndexValue,
                                keyaclIntfIfDirectionValue,
                                keyaclIntfIfSequenceValue,
                                keyaclIntfIfAclTypeValue,
                                keyaclIntfIfAclIdValue, &aclDiffservFlag);
  }
  else if (objaclIntfIfStatusValue == L7_ROW_STATUS_DESTROY)
  {
    owa.l7rc = fpobj_QosAclIntfIfRemove (L7_UNIT_CURRENT, keyaclIntfIfIndexValue,
                                   keyaclIntfIfDirectionValue,
                                   keyaclIntfIfSequenceValue,
                                   keyaclIntfIfAclTypeValue,
                                   keyaclIntfIfAclIdValue);

  }
  if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = (aclDiffservFlag) ? XLIBRC_ACL_DIFFSERV_FAILED : XLIBRC_ACL_ASSIGN_FAILED;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
