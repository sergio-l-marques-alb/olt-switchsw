/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_SwitchingProtectedPortCfg.c
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
#include "_xe_SwitchingProtectedPortCfg_obj.h"
#include "usmdb_protected_port_api.h"
#include "usmdb_common.h"
#include "usmdb_util_api.h"


extern int getPortFromMask(L7_INTF_MASK_t intfmask);

/*******************************************************************************
* @function fpObjGet_SwitchingProtectedPortCfg_GroupId
*
* @purpose Get 'GroupId'
*
* @description [GroupId]: The group that this port belongs to 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_SwitchingProtectedPortCfg_GroupId (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objGroupIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: GroupId */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingProtectedPortCfg_GroupId,
                          (xLibU8_t *) & objGroupIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objGroupIdValue = L7_PROTECTED_PORT_MAX_GROUPS + 1;
    owa.l7rc =  L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objGroupIdValue, owa.len);
    
    if( objGroupIdValue == L7_PROTECTED_PORT_MAX_GROUPS + 1)
    {
      objGroupIdValue = 0;
    }
    else
    {
      objGroupIdValue++;
    }
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


xLibRC_t fpObjGet_SwitchingProtectedPortCfg_GroupId (void *wap, void *bufp)
{
  xLibU32_t objInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof ( objInterfaceValue));
  xLibU32_t objGroupIdValue=0;
  FPOBJ_TRACE_ENTER (bufp);
  /* retrieve key: ifIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingProtectedPortCfg_PortList,
                          (xLibU8_t *) & objInterfaceValue, &owa.len);

 
  FPOBJ_TRACE_ENTER (bufp);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objInterfaceValue, sizeof(objInterfaceValue));

  owa.l7rc =  usmdbProtectedPortGroupIntfGet (L7_UNIT_CURRENT, objInterfaceValue, &objGroupIdValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    objGroupIdValue = L7_PROTECTED_PORT_MAX_GROUPS + 1;
  }
  owa.len  = (sizeof ( xLibU32_t));
  /* return the object value: GroupId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objGroupIdValue,
                           sizeof (objGroupIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_SwitchingProtectedPortCfg_GroupName
*
* @purpose Get 'GroupName'
*
* @description [GroupName]: The name of the group 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingProtectedPortCfg_GroupName (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyGroupIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objGroupNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: GroupId */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingProtectedPortCfg_GroupId,
                          (xLibU8_t *) & keyGroupIdValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyGroupIdValue, kwa.len);

  memset(objGroupNameValue, 0x00, (sizeof (xLibStr256_t)));

  if( keyGroupIdValue <  L7_PROTECTED_PORT_MAX_GROUPS + 1)
  {
  /* get the value from application */
    owa.l7rc = usmdbProtectedPortGroupNameGet (L7_UNIT_CURRENT, keyGroupIdValue,
                                             objGroupNameValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }

  /* return the object value: GroupName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objGroupNameValue,
                           strlen (objGroupNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingProtectedPortCfg_GroupName
*
* @purpose Set 'GroupName'
*
* @description [GroupName]: The name of the group 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingProtectedPortCfg_GroupName (void *wap, void *bufp)
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
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingProtectedPortCfg_GroupId,
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
* @function fpObjGet_SwitchingProtectedPortCfg_PortList
*
* @purpose Get 'PortList'
*
* @description [PortList]: The set of ports that are protected in this group
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingProtectedPortCfg_PortList (void *wap, void *bufp)
{

 fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objifIndexValue;
  xLibU32_t nextObjifIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingProtectedPortCfg_PortList,
                          (xLibU8_t *) & objifIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbIntIfNumTypeFirstGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF,
                                 0, &nextObjifIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objifIndexValue, owa.len);
	 owa.l7rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF,
                                               0, objifIndexValue, &nextObjifIndexValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjifIndexValue, owa.len);

  /* return the object value: ifIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjifIndexValue,
                           sizeof (objifIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#if 0
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_INTF_MASK_t));
  L7_INTF_MASK_t interfaceMask;
  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;

  memset(&interfaceMask, 0x00, sizeof (L7_INTF_MASK_t));
  FPOBJ_TRACE_ENTER (bufp);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingProtectedPortCfg_PortList,
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
#endif

#if 0

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyGroupIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof(L7_INTF_MASK_t) );
  L7_INTF_MASK_t interfaceMask;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: GroupId */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingProtectedPortCfg_GroupId,
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
#endif
}

/*******************************************************************************
* @function fpObjList_SwitchingProtectedPortCfg_PortList
*
* @purpose List 'PortList'
 *@description  [PortList] The set of ports that are protected in this group   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_SwitchingProtectedPortCfg_PortList (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_INTF_MASK_t));
  L7_INTF_MASK_t interfaceMask;
  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;

  memset(&interfaceMask, 0x00, sizeof (L7_INTF_MASK_t));
  FPOBJ_TRACE_ENTER (bufp);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingProtectedPortCfg_PortList,
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
* @function fpObjSet_SwitchingProtectedPortCfg_PortList
*
* @purpose Set 'PortList'
*
* @description [PortList]: The set of ports that are protected in this group
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingProtectedPortCfg_PortList (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_INTF_MASK_t));
  L7_INTF_MASK_t newIfMask;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyGroupIdValue;
  L7_RC_t rc;
  L7_uint32 intIfNum;
  L7_INTF_MASK_t oldIfMask;
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
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingProtectedPortCfg_GroupId,
                          (xLibU8_t *) & keyGroupIdValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyGroupIdValue, kwa.len);
 
  /* set the value in application */
  memset(&oldIfMask, 0x00, L7_INTF_INDICES);

  if(usmdbProtectedPortGroupIntfMaskGet(L7_UNIT_CURRENT,
                                        keyGroupIdValue,
                                        &oldIfMask) != L7_SUCCESS)
  {
    owa.l7rc = L7_FAILURE;
  }

  L7_INTF_MASKEXOREQ(newIfMask,oldIfMask);
  rc = usmDbIntIfNumTypeFirstGet(USMDB_UNIT_CURRENT, USM_PHYSICAL_INTF, 0,
                                 &intIfNum);

  while( rc == L7_SUCCESS)
  {
    if (L7_INTF_ISMASKBITSET(newIfMask,intIfNum))
    {
      if(L7_INTF_ISMASKBITSET(oldIfMask, intIfNum))
      {
        /*Already protected, need to delete it */
        if (usmdbProtectedPortGroupIntfDelete(L7_UNIT_CURRENT,
                                              keyGroupIdValue,
                                              intIfNum) != L7_SUCCESS)
        {
          owa.l7rc = L7_FAILURE;
        }
      }
      else
      {
        /* Newly added, so need to add to protected list */
        if (usmdbProtectedPortGroupIntfAdd(L7_UNIT_CURRENT,
                                           keyGroupIdValue,
                                           intIfNum) != L7_SUCCESS)
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
* @function fpObjGet_SwitchingProtectedPortCfg_PortList
*
* @purpose Get 'PortList'
*
* @description [PortList] The set of ports that are protected in this group
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingProtectedPortCfg_PortList (void *wap, void *bufp)
{

  fpObjWa_t kwaGroupId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyGroupIdValue, nextintIfNum, nextgroupId, i, flag;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPortListValue, stat;
  FPOBJ_TRACE_ENTER (bufp);

  flag =1;

  /* retrieve key: GroupId */
  kwaGroupId.rc = xLibFilterGet (wap, XOBJ_SwitchingProtectedPortCfg_GroupId,
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
* @function fpObjSet_SwitchingProtectedPortCfg_PortList
*
* @purpose Set 'PortList'
*
* @description [PortList] The set of ports that are protected in this group
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingProtectedPortCfg_PortList (void *wap, void *bufp)
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
  kwaGroupId.rc = xLibFilterGet (wap, XOBJ_SwitchingProtectedPortCfg_GroupId,
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

#if 0
/*******************************************************************************
* @function fpObjSet_SwitchingProtectedPortCfg_AppendPort
*
* @purpose Set 'AppendPort'
*
* @description [AppendPort] Append the given port to already existing port list in a group
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingProtectedPortCfg_AppendPort (void *wap, void *bufp)
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
  kwaGroupId.rc = xLibFilterGet (wap, XOBJ_SwitchingProtectedPortCfg_GroupId,
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
* @function fpObjSet_SwitchingProtectedPortCfg_DeletePortt
*
* @purpose Set 'DeletePortt'
*
* @description [DeletePortt] Delete the given port to already existing port list in a group
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingProtectedPortCfg_DeletePortt (void *wap, void *bufp)
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
  kwaGroupId.rc = xLibFilterGet (wap, XOBJ_SwitchingProtectedPortCfg_GroupId,
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
#endif
            

/*******************************************************************************
* @function fpObjSet_MetroProtocolTunnelingEntry_TunnelingRowStatus
*
* @purpose Set 'TunnelingRowStatus'
 *@description  [TunnelingRowStatus] The association status of an Interface,
* TunnelingMAC, ProtocolID, TunnelingAction in protocol tunneling
* table.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingProtectedPortCfg_RowStatusCfg (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objPortConfigRowStatusValue;

  xLibU32_t keyInterfaceValue;
  xLibU32_t keyGroupIdValue;


  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TunnelingRowStatus */
  owa.len = sizeof ( objPortConfigRowStatusValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objPortConfigRowStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPortConfigRowStatusValue, owa.len);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingProtectedPortCfg_PortList,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* retrieve key: TunnelingMAC */
  owa.len = sizeof ( keyGroupIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingProtectedPortCfg_GroupId,
                          (xLibU8_t *) &keyGroupIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp,  keyGroupIdValue, owa.len);


  if ( objPortConfigRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    if( keyGroupIdValue == L7_PROTECTED_PORT_MAX_GROUPS + 1)
    { 
      owa.l7rc  = usmdbProtectedPortGroupIntfGet (L7_UNIT_CURRENT, keyInterfaceValue, &keyGroupIdValue);
      if( owa.l7rc == L7_SUCCESS) 
      {
        owa.l7rc = usmdbProtectedPortGroupIntfDelete (L7_UNIT_CURRENT,keyGroupIdValue, keyInterfaceValue); 
      }
    }  
    else
    {
      owa.l7rc = usmdbProtectedPortGroupIntfAdd(L7_UNIT_CURRENT,keyGroupIdValue, keyInterfaceValue);
    }
  }
  else if ( objPortConfigRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
    owa.l7rc = usmdbProtectedPortGroupIntfDelete (L7_UNIT_CURRENT,keyGroupIdValue, keyInterfaceValue); 
  } 


  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

xLibRC_t fpObjGet_SwitchingProtectedPortCfg_RowStatusCfg (void *wap, void *bufp)
{
  return XLIBRC_SUCCESS;
}
xLibRC_t fpObjSet_SwitchingProtectedPortCfg_GroupId  (void *wap, void *bufp)
{
  return XLIBRC_SUCCESS;
}
