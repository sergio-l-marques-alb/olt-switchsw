
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_SwitchingPbvlanInterfaceConfig.c
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
#include "_xe_SwitchingPbvlanInterfaceConfig_obj.h"
#include "usmdb_pbvlan_api.h"
#include "usmdb_util_api.h"

L7_RC_t
fpObjUtilProtocolGroupPortEntryGetNext(L7_uint32 UnitIndex, L7_uint32 *groupId, L7_uint32 *ifIndex)
{
  L7_uint32 intIfNum=0;

  if (usmDbComponentPresentCheck(UnitIndex, L7_PBVLAN_COMPONENT_ID) == L7_FALSE)
  {
      return L7_FAILURE;
  }

  /* if this group doesn't exist, get the next valid group */
  if (usmDbPbVlanGroupGetExact(UnitIndex, *groupId) != L7_SUCCESS &&
      usmDbPbVlanGroupGetNext(UnitIndex, *groupId, groupId) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* if the requested external interface isn't visible or doesn't have an internal interface*/
  if (usmDbVisibleExtIfNumberCheck(UnitIndex, *ifIndex) != L7_SUCCESS ||
      usmDbIntIfNumFromExtIfNum(*ifIndex, &intIfNum) != L7_SUCCESS)
  {
    /* find the next external interface */
    if (usmDbGetNextVisibleExtIfNumber(*ifIndex, ifIndex) != L7_SUCCESS ||
        usmDbIntIfNumFromExtIfNum(*ifIndex, &intIfNum) != L7_SUCCESS)
    {
      /* no more interface for this group */
      if (usmDbPbVlanGroupGetNext(UnitIndex, *groupId, groupId) != L7_SUCCESS)
        return L7_FAILURE;

      /* start at the begining of the group */
      *ifIndex = 0;
      intIfNum = 0;
    }
  }

   /* while the selected group is valid */
  while (usmDbPbVlanGroupGetExact(UnitIndex, *groupId) == L7_SUCCESS)
  {
    /* check to see if the current index is in the group */
    if (usmDbPbVlanGroupPortGetExact(UnitIndex, *groupId, intIfNum) != L7_SUCCESS)
    {
      /* find the next port in the group */
      if (usmDbPbVlanGroupPortGetNext(UnitIndex, *groupId, intIfNum, &intIfNum) != L7_SUCCESS)
      {
        /* no more ports in this group found, go on to next group */
        if (usmDbPbVlanGroupGetNext(UnitIndex, *groupId, groupId) == L7_SUCCESS)
        {
          intIfNum = 0;
          continue;
        }
        else
        {
          /* no more groups found */
          return L7_FAILURE;
        }
      }
    }

    /* we've found a port, convert to external*/
    return usmDbExtIfNumFromIntIfNum(intIfNum, ifIndex);
  }

  return L7_FAILURE;
}

/*******************************************************************************
* @function fpObjGet_SwitchingPbvlanInterfaceConfig_GroupId
*
* @purpose Get 'GroupId'
 *@description  [GroupId] Group identifier of the protocol-based VLAN entry   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingPbvlanInterfaceConfig_GroupId (void *wap, void *bufp)
{

  xLibU32_t objGroupIdValue;
  xLibU32_t nextObjGroupIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: GroupId */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingPbvlanInterfaceConfig_GroupId,
                          (xLibU8_t *) & objGroupIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&nextObjGroupIdValue, 0, sizeof (nextObjGroupIdValue));
    memset (&objGroupIdValue, 0, sizeof (objGroupIdValue));
    owa.l7rc = usmDbPbVlanGroupGetNext(L7_UNIT_CURRENT,objGroupIdValue,
                                      &nextObjGroupIdValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objGroupIdValue, owa.len);
    do
    {
      owa.l7rc = usmDbPbVlanGroupGetNext(L7_UNIT_CURRENT,objGroupIdValue,
                                      &nextObjGroupIdValue);
    }
    while ((objGroupIdValue == nextObjGroupIdValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
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
* @function fpObjGet_SwitchingPbvlanInterfaceConfig_Interface
*
* @purpose Get 'Interface'
 *@description  [Interface] The interface number of this instance   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingPbvlanInterfaceConfig_Interface (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_INTF_MASK_t));
  L7_INTF_MASK_t interfaceMask;
  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyGroupIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  memset(&interfaceMask, 0x00, sizeof (L7_INTF_MASK_t));
  /* retrieve key: GroupId */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingPbvlanInterfaceConfig_GroupId,
                           (xLibU8_t *) & keyGroupIdValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyGroupIdValue, kwa.len);

  memset(&interfaceMask, 0x00, sizeof (L7_INTF_MASK_t));
  /* retrieve key: Interface */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingPbvlanInterfaceConfig_Interface,
                          (xLibU8_t *) &interfaceMask , &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objInterfaceValue = 0;
  }
  else
  {
    objInterfaceValue = fpObjPortFromMaskGet(interfaceMask);
    if(objInterfaceValue == -1)
    {
      owa.rc = XLIBRC_ENDOF_TABLE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objInterfaceValue, owa.len);
  owa.l7rc = usmDbPbVlanGroupPortGetNext (L7_UNIT_CURRENT,
                                          keyGroupIdValue,
                                          objInterfaceValue,
                                          &nextObjInterfaceValue);
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  memset(&interfaceMask,0x00, sizeof (L7_INTF_MASK_t));
  L7_INTF_SETMASKBIT(interfaceMask, nextObjInterfaceValue);
  FPOBJ_TRACE_NEW_KEY (bufp, &interfaceMask, sizeof (L7_INTF_MASK_t));

  /* return the object value: Interface */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &interfaceMask,
                           sizeof (L7_INTF_MASK_t));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjList_SwitchingPbvlanInterfaceConfig_Interface
*
* @purpose List 'Interface'
 *@description  [Interface] The interface number of this instance   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_SwitchingPbvlanInterfaceConfig_Interface (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_INTF_MASK_t));
  L7_INTF_MASK_t interfaceMask;
  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;

  memset(&interfaceMask, 0x00, sizeof (L7_INTF_MASK_t));
  FPOBJ_TRACE_ENTER (bufp);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingPbvlanInterfaceConfig_Interface,
                          (xLibU8_t *) &interfaceMask, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbValidIntIfNumFirstGet(&nextObjInterfaceValue);
  }
  else
  {
    objInterfaceValue = fpObjPortFromMaskGet(interfaceMask);
    if(objInterfaceValue == -1)
    { 
      owa.rc = XLIBRC_ENDOF_TABLE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objInterfaceValue, sizeof(objInterfaceValue));
    owa.l7rc = usmDbValidIntIfNumNext(objInterfaceValue, &nextObjInterfaceValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  memset(&interfaceMask, 0x00, sizeof (L7_INTF_MASK_t));
  L7_INTF_SETMASKBIT(interfaceMask,nextObjInterfaceValue);
  FPOBJ_TRACE_NEW_KEY (bufp, &interfaceMask, sizeof (L7_INTF_MASK_t));

  /* return the object value: Interface */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &interfaceMask,
                           sizeof (L7_INTF_MASK_t));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_SwitchingPbvlanInterfaceConfig_PortStatus
*
* @purpose Get 'PortStatus'
 *@description  [PortStatus] The status of this entry. active(1) - the port is
* associated with this group createAndGo(4) - add the port to this
* group destroy(6) - remove the port from this group   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingPbvlanInterfaceConfig_PortStatus (void *wap, void *bufp)
{
  fpObjWa_t kwaGroupId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyGroupIdValue;
  fpObjWa_t kwaInterfaceMask = FPOBJ_INIT_WA (sizeof(L7_INTF_MASK_t));
  L7_INTF_MASK_t interfaceMask;
#if 0
  xLibU32_t keyInterfaceValue;
#endif
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPortStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: GroupId */
  kwaGroupId.rc = xLibFilterGet (wap, XOBJ_SwitchingPbvlanInterfaceConfig_GroupId,
                                 (xLibU8_t *) & keyGroupIdValue,
                                 &kwaGroupId.len);
  if (kwaGroupId.rc != XLIBRC_SUCCESS)
  {
    kwaGroupId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaGroupId);
    return kwaGroupId.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyGroupIdValue, kwaGroupId.len);

  /* retrieve key: Interface */
  kwaInterfaceMask.rc =
    xLibFilterGet (wap, XOBJ_SwitchingPbvlanInterfaceConfig_Interface,
                   (xLibU8_t *) &interfaceMask, &kwaInterfaceMask.len);
  if (kwaInterfaceMask.rc != XLIBRC_SUCCESS)
  {
    kwaInterfaceMask.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterfaceMask);
    return kwaInterfaceMask.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &interfaceMask, kwaInterfaceMask.len);

  objPortStatusValue = L7_ROW_STATUS_ACTIVE;

  owa.l7rc = L7_SUCCESS;
  
  #if 0
  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keyGroupIdValue,
                              keyInterfaceValue, &objPortStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  #endif 

  /* return the object value: PortStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPortStatusValue, sizeof (objPortStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingPbvlanInterfaceConfig_PortStatus
*
* @purpose Set 'PortStatus'
 *@description  [PortStatus] The status of this entry. active(1) - the port is
* associated with this group createAndGo(4) - add the port to this
* group destroy(6) - remove the port from this group   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingPbvlanInterfaceConfig_PortStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPortStatusValue;
  fpObjWa_t kwaGroupId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyGroupIdValue;
  fpObjWa_t kwaInterfaceMask = FPOBJ_INIT_WA (sizeof(L7_INTF_MASK_t));
  L7_INTF_MASK_t keyInterfaceMask;
  xLibU32_t keyInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PortStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objPortStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPortStatusValue, owa.len);

  /* retrieve key: GroupId */
  kwaGroupId.rc = xLibFilterGet (wap, XOBJ_SwitchingPbvlanInterfaceConfig_GroupId,
                                 (xLibU8_t *) & keyGroupIdValue,
                                 &kwaGroupId.len);
  if (kwaGroupId.rc != XLIBRC_SUCCESS)
  {
    kwaGroupId.rc = XLIBRC_PBVLAN_GROUPID_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, kwaGroupId);
    return kwaGroupId.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyGroupIdValue, kwaGroupId.len);

  /* retrieve key: Interface */
  kwaInterfaceMask.rc =
    xLibFilterGet (wap, XOBJ_SwitchingPbvlanInterfaceConfig_Interface,
                   (xLibU8_t *) & keyInterfaceMask, &kwaInterfaceMask.len);
  if (kwaInterfaceMask.rc != XLIBRC_SUCCESS)
  {
    kwaInterfaceMask.rc = XLIBRC_PBVLAN_INTFMASK_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, kwaInterfaceMask);
    return kwaInterfaceMask.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceMask, kwaInterfaceMask.len);
  keyInterfaceValue = 0;
  while((keyInterfaceValue = fpObjNextIntfGet(&keyInterfaceMask,keyInterfaceValue))!= -1)
  { 
    /* call the usmdb only for add and delete */
  if (objPortStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
      /* Create a row */
      owa.l7rc = usmDbPbVlanGroupPortAdd (L7_UNIT_CURRENT, 
                                          keyGroupIdValue,
                                          keyInterfaceValue);
      if (owa.l7rc != L7_SUCCESS)
      {
        owa.rc = XLIBRC_GROUP_PORT_ADD_FAILURE;  /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
  }
  else if (objPortStatusValue == L7_ROW_STATUS_DESTROY)
  {
      /* Delete the existing row */
      owa.l7rc = usmDbPbVlanGroupPortDelete (L7_UNIT_CURRENT, 
                                           keyGroupIdValue,
                                           keyInterfaceValue);
      if (owa.l7rc != L7_SUCCESS)
      {
        owa.rc = XLIBRC_GROUP_PORT_DEL_FAILURE;  /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
 } 
  return XLIBRC_SUCCESS;
}

