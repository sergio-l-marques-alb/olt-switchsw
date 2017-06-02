/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_SwitchingProtectedPortConfig.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to ProtectedPorts-object.xml
*
* @create  12 February 2008
*
* @author Radha K 
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_SwitchingProtectedPortConfig_obj.h"
#include "usmdb_protected_port_api.h"
#include "usmdb_common.h"
#include "usmdb_util_api.h"


extern int getPortFromMask(L7_INTF_MASK_t intfmask);

/*******************************************************************************
* @function fpObjGet_SwitchingProtectedPortConfig_GroupId
*
* @purpose Get 'GroupId'
*
* @description [GroupId]: The group that this port belongs to 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingProtectedPortConfig_GroupId (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objGroupIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: GroupId */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingProtectedPortConfig_GroupId,
                          (xLibU8_t *) & objGroupIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objGroupIdValue = 0;
    { 
      owa.l7rc =  L7_SUCCESS;
    } 
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objGroupIdValue, owa.len);

    objGroupIdValue++;
    if(objGroupIdValue < L7_PROTECTED_PORT_MAX_GROUPS)
    {
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
  FPOBJ_TRACE_NEW_KEY (bufp, &objGroupIdValue, owa.len);

  /* return the object value: GroupId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objGroupIdValue,
                           sizeof (objGroupIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingProtectedPortConfig_GroupName
*
* @purpose Get 'GroupName'
*
* @description [GroupName]: The name of the group 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingProtectedPortConfig_GroupName (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyGroupIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objGroupNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: GroupId */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingProtectedPortConfig_GroupId,
                          (xLibU8_t *) & keyGroupIdValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyGroupIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmdbProtectedPortGroupNameGet (L7_UNIT_CURRENT, keyGroupIdValue,
                                             objGroupNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: GroupName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objGroupNameValue,
                           strlen (objGroupNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingProtectedPortConfig_GroupName
*
* @purpose Set 'GroupName'
*
* @description [GroupName]: The name of the group 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingProtectedPortConfig_GroupName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objGroupNameValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyGroupIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: GroupName */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objGroupNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objGroupNameValue, owa.len);

  /* retrieve key: GroupId */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingProtectedPortConfig_GroupId,
                          (xLibU8_t *) & keyGroupIdValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyGroupIdValue, kwa.len);
    if( strlen(objGroupNameValue) > (L7_PROTECTED_PORT_GROUP_NAME_SIZE-1))
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if 
                                                       required */
   FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* set the value in application */
  owa.l7rc = usmdbProtectedPortGroupNameSet (L7_UNIT_CURRENT, keyGroupIdValue,
                                             objGroupNameValue);
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
* @function fpObjGet_SwitchingProtectedPortConfig_PortList
*
* @purpose Get 'PortList'
*
* @description [PortList]: The set of ports that are protected in this group
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingProtectedPortConfig_PortList (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyGroupIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof(L7_INTF_MASK_t) );
  L7_INTF_MASK_t interfaceMask;

  FPOBJ_TRACE_ENTER (bufp);
  /* retrieve key: GroupId */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingProtectedPortConfig_GroupId,
                          (xLibU8_t *) & keyGroupIdValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyGroupIdValue, kwa.len);

  memset(&interfaceMask, 0x00, sizeof(L7_INTF_MASK_t));
  /* get the value from application */
  owa.l7rc =
    usmdbProtectedPortGroupIntfMaskGet (L7_UNIT_CURRENT, keyGroupIdValue,
                                     &interfaceMask);
        
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &interfaceMask, sizeof (L7_INTF_MASK_t));
  /* return the object value: PortList */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &interfaceMask,
                           sizeof(L7_INTF_MASK_t));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjList_SwitchingProtectedPortConfig_PortList
*
* @purpose List 'PortList'
 *@description  [PortList] The set of ports that are protected in this group   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_SwitchingProtectedPortConfig_PortList (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_INTF_MASK_t));
  L7_INTF_MASK_t interfaceMask;
  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyGroupIdValue;
  xLibU32_t GroupId;
  L7_RC_t rv = L7_FAILURE;
  L7_RC_t rc;
  
  /* retrieve key: GroupId */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingProtectedPortConfig_GroupId,
                          (xLibU8_t *) & GroupId, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &GroupId, kwa.len);
  
  memset(&interfaceMask, 0x00, sizeof (L7_INTF_MASK_t));
  FPOBJ_TRACE_ENTER (bufp);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingProtectedPortConfig_PortList,
                          (xLibU8_t *) &interfaceMask, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbIntIfNumTypeFirstGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF,
                                 0, &nextObjInterfaceValue);
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
    owa.l7rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF,
                                               0, objInterfaceValue, &nextObjInterfaceValue);
    
  }
  
  if(owa.l7rc == L7_SUCCESS)
  {  
	 while(rv != L7_SUCCESS)
	 {
	   rc = usmdbProtectedPortGroupIntfGet(L7_UNIT_CURRENT, nextObjInterfaceValue, &keyGroupIdValue);
	   if(rc == L7_SUCCESS)
       {
         if(GroupId == keyGroupIdValue)
	     {
		   rv = L7_SUCCESS;
		   break;
		 }
		 else
		 {
		   rv = L7_FAILURE; 
		 }
       }
       else
       {
         rv = L7_SUCCESS;
		 break;
       }
       objInterfaceValue = nextObjInterfaceValue;
       owa.l7rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF,
                                             0, objInterfaceValue, &nextObjInterfaceValue);
	   if (owa.l7rc != L7_SUCCESS)
       {
         break;
       }
	 }
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
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)&interfaceMask,
                           sizeof (interfaceMask));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_SwitchingProtectedPortConfig_PortList
*
* @purpose Set 'PortList'
*
* @description [PortList]: The set of ports that are protected in this group
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingProtectedPortConfig_PortList (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_INTF_MASK_t));
  L7_INTF_MASK_t newIfMask;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyGroupIdValue, GroupIdValue;
  L7_RC_t rc;
  L7_uint32 intIfNum;
  FPOBJ_TRACE_ENTER (bufp);
  
  memset(&newIfMask, 0x00, L7_INTF_INDICES);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) &newIfMask, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, newIfMask, owa.len);

  /* retrieve key: GroupId */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingProtectedPortConfig_GroupId,
                          (xLibU8_t *) & keyGroupIdValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyGroupIdValue, kwa.len);
 
  /* set the value in application */
  rc = usmDbIntIfNumTypeFirstGet(USMDB_UNIT_CURRENT, USM_PHYSICAL_INTF, 0,
                                 &intIfNum);

  while( rc == L7_SUCCESS)
  {
    if (L7_INTF_ISMASKBITSET(newIfMask,intIfNum))
    {
      /* Newly added, so need to add to protected list */
      if (usmdbProtectedPortGroupIntfAdd(L7_UNIT_CURRENT,
                                         keyGroupIdValue,
                                         intIfNum) != L7_SUCCESS)
      {
        usmdbProtectedPortGroupIntfGet(L7_UNIT_CURRENT, intIfNum, &GroupIdValue);
        if(GroupIdValue == keyGroupIdValue)
        {
          owa.l7rc = L7_FAILURE;
        }
      }
    }
    
    rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT,USM_PHYSICAL_INTF, 0,
                                  intIfNum, &intIfNum);
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
#if 0
/*******************************************************************************
* @function fpObjGet_SwitchingProtectedPortConfig_PortList
*
* @purpose Get 'PortList'
*
* @description [PortList] The set of ports that are protected in this group
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingProtectedPortConfig_PortList (void *wap, void *bufp)
{

  fpObjWa_t kwaGroupId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyGroupIdValue, nextintIfNum, nextgroupId, i, flag;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPortListValue, stat;
  FPOBJ_TRACE_ENTER (bufp);

  flag =1;

  /* retrieve key: GroupId */
  kwaGroupId.rc = xLibFilterGet (wap, XOBJ_SwitchingProtectedPortConfig_GroupId,
                                 (xLibU8_t *) & keyGroupIdValue, &kwaGroupId.len);
  if (kwaGroupId.rc != XLIBRC_SUCCESS)
  {
    kwaGroupId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaGroupId);
    return kwaGroupId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyGroupIdValue, kwaGroupId.len);
  
  i = 0;
  memset(objPortListValue, 0x0, sizeof(objPortListValue)); 

  while (usmdbProtectedPortGroupIntfGetNext(L7_UNIT_CURRENT, keyGroupIdValue, i,
                                            &nextgroupId, &nextintIfNum) == L7_SUCCESS)
  {
     if(keyGroupIdValue == nextgroupId)
     {
        memset(stat, 0x0, sizeof(stat)); 
        sprintf(stat, "%d", nextintIfNum);
        if(flag == 1)
        {
          OSAPI_STRNCAT(objPortListValue,stat); 
          flag = 0;
        } 
        else
        {
          OSAPI_STRNCAT(objPortListValue, ",");
          OSAPI_STRNCAT(objPortListValue, stat); 
        }
     }

     i = nextintIfNum;
  }   

  /* return the object value: PortList */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPortListValue, strlen (objPortListValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjSet_SwitchingProtectedPortConfig_PortList
*
* @purpose Set 'PortList'
*
* @description [PortList] The set of ports that are protected in this group
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingProtectedPortConfig_PortList (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPortListValue;
  xLibU8_t *temp;
  fpObjWa_t kwaGroupId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyGroupIdValue, interface;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PortList */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objPortListValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objPortListValue, owa.len);

  /* retrieve key: GroupId */
  kwaGroupId.rc = xLibFilterGet (wap, XOBJ_SwitchingProtectedPortConfig_GroupId,
                                 (xLibU8_t *) & keyGroupIdValue, &kwaGroupId.len);
  if (kwaGroupId.rc != XLIBRC_SUCCESS)
  {
    kwaGroupId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaGroupId);
    return kwaGroupId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyGroupIdValue, kwaGroupId.len);

  temp = strtok(objPortListValue, ",");

  while (temp  != NULL)
  {
     sscanf (temp, "%d", &interface);

     /* set the value in application */
     owa.l7rc = usmdbProtectedPortGroupIntfAdd (L7_UNIT_CURRENT, keyGroupIdValue, interface);

     if (owa.l7rc != L7_SUCCESS)
     {
       owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
     }
     
     temp = strtok(NULL, ",");
  } 
  
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
#endif
/*******************************************************************************
* @function fpObjSet_SwitchingProtectedPortConfig_AppendPort
*
* @purpose Set 'AppendPort'
*
* @description [AppendPort] Append the given port to already existing port list in a group
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingProtectedPortConfig_AppendPort (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAppendPortValue;

  fpObjWa_t kwaGroupId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyGroupIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AppendPort */

  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAppendPortValue, &owa.len);
 
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAppendPortValue, owa.len);

  /* retrieve key: GroupId */
  kwaGroupId.rc = xLibFilterGet (wap, XOBJ_SwitchingProtectedPortConfig_GroupId,
                                 (xLibU8_t *) & keyGroupIdValue, &kwaGroupId.len);
  if (kwaGroupId.rc != XLIBRC_SUCCESS)
  {
    kwaGroupId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaGroupId);
    return kwaGroupId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyGroupIdValue, kwaGroupId.len);

  /* set the value in application */
  owa.l7rc = usmdbProtectedPortGroupIntfAdd(L7_UNIT_CURRENT, keyGroupIdValue, objAppendPortValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingProtectedPortConfig_DeletePortt
*
* @purpose Set 'DeletePortt'
*
* @description [DeletePortt] Delete the given port to already existing port list in a group
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingProtectedPortConfig_DeletePortt (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDeletePorttValue;

  fpObjWa_t kwaGroupId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyGroupIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DeletePortt */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objDeletePorttValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDeletePorttValue, owa.len);

  /* retrieve key: GroupId */
  kwaGroupId.rc = xLibFilterGet (wap, XOBJ_SwitchingProtectedPortConfig_GroupId,
                                 (xLibU8_t *) & keyGroupIdValue, &kwaGroupId.len);
  if (kwaGroupId.rc != XLIBRC_SUCCESS)
  {
    kwaGroupId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaGroupId);
    return kwaGroupId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyGroupIdValue, kwaGroupId.len);

  /* set the value in application */
  owa.l7rc = usmdbProtectedPortGroupIntfDelete(L7_UNIT_CURRENT, keyGroupIdValue, objDeletePorttValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_SwitchingProtectedPortConfig_RowStatus
*
* @purpose Get 'RowStatus'
*
* @description [RowStatus] Add or Remove an interface to protected port list for a group
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingProtectedPortConfig_RowStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyGroupIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objGroupNameValue;

  xLibU32_t objRowStatusValue = L7_NULL;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: GroupId */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingProtectedPortConfig_GroupId,
                          (xLibU8_t *) & keyGroupIdValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyGroupIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmdbProtectedPortGroupNameGet (L7_UNIT_CURRENT, keyGroupIdValue,
                                             objGroupNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  /* get the value from application */
  objRowStatusValue = L7_ROW_STATUS_ACTIVE;

  /* return the object value: RowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRowStatusValue, sizeof (objRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingProtectedPortConfig_RowStatus
*
* @purpose Set 'RowStatus'
*
* @description [RowStatus] Add or Remove an interface to protected port list for a group
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingProtectedPortConfig_RowStatus (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRowStatusValue;

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyGroupIdValue, GroupIdValue;

  fpObjWa_t pwa = FPOBJ_INIT_WA (sizeof (L7_INTF_MASK_t));
  L7_INTF_MASK_t newIfMask;

  L7_uint32 intIfNum = L7_NULL;
  L7_RC_t rc = L7_FAILURE;

  FPOBJ_TRACE_ENTER (bufp);


  /* retrieve object: RowStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) &objRowStatusValue, &owa.len); 
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRowStatusValue, owa.len);


  /* retrieve key: GroupId */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingProtectedPortConfig_GroupId,
                          (xLibU8_t *) & keyGroupIdValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyGroupIdValue, kwa.len);

  /* retrieve key: portlist */
  memset(&newIfMask, 0x00, L7_INTF_INDICES);
  pwa.rc = xLibFilterGet (wap, XOBJ_SwitchingProtectedPortConfig_PortList,
                                    (xLibU8_t *) &newIfMask, &pwa.len);
  if (pwa.rc != XLIBRC_SUCCESS)
  {
    pwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, pwa);
    return pwa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, newIfMask, pwa.len);

  owa.l7rc = L7_SUCCESS;

  rc = usmDbIntIfNumTypeFirstGet(USMDB_UNIT_CURRENT, USM_PHYSICAL_INTF, 0,
                                 &intIfNum);

  while ( rc == L7_SUCCESS)
  {
    if (L7_INTF_ISMASKBITSET(newIfMask,intIfNum))
    {

      if (objRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
      {

        if (usmdbProtectedPortGroupIntfAdd(L7_UNIT_CURRENT,
                                           keyGroupIdValue,
                                           intIfNum) != L7_SUCCESS)
        {
          owa.l7rc = L7_FAILURE;
        }
      }
      else if (objRowStatusValue == L7_ROW_STATUS_DESTROY)
      {
        if (usmdbProtectedPortGroupIntfDelete(L7_UNIT_CURRENT,
                                              keyGroupIdValue,
                                              intIfNum) != L7_SUCCESS)
        {
          owa.l7rc = L7_FAILURE;
        }
      }
      if(owa.l7rc == L7_FAILURE)
      {
        usmdbProtectedPortGroupIntfGet(L7_UNIT_CURRENT, intIfNum, &GroupIdValue); 
        if(GroupIdValue != keyGroupIdValue)
        {
          owa.l7rc = L7_SUCCESS;
        }
      }
    }
    rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT,USM_PHYSICAL_INTF, 0,
                                  intIfNum, &intIfNum);
    
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

