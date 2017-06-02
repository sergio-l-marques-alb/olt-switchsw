/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_SwitchingStaticMACFilterConfig.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to StaticMACFilter-object.xml
*
* @create  19 February 2008
*
* @author  Radha K
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_SwitchingStaticMACFilterConfig_obj.h"
#include "usmdb_filter_api.h"
#include "filter_exports.h"
#include "usmdb_mib_vlan_api.h"
#include "usmdb_dot1q_api.h"
#include "usmdb_util_api.h"
#include "usmdb_dot3ad_api.h"
#include "dot1q_api.h"

extern L7_RC_t usmDbNextVlanGet(L7_uint32 UnitIndex, L7_uint32 vid, L7_uint32 *nextvid);
/*******************************************************************************
* @function fpObjList_SwitchingStaticMACFilterConfig_StaticMacFilteringVlanId
*
* @purpose List 'StaticMacFilteringVlanId'
*
* @description [StaticMacFilteringVlanId]: The Static MAC Filter Vlan Id 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_SwitchingStaticMACFilterConfig_StaticMacFilteringVlanId (void
                                                                           *wap,
                                                                           void
                                                                           *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStaticMacFilteringVlanIdValue;
  xLibU32_t nextObjStaticMacFilteringVlanIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StaticMacFilteringVlanId */
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_SwitchingStaticMACFilterConfig_StaticMacFilteringVlanId,
                   (xLibU8_t *) & objStaticMacFilteringVlanIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, (char *)NULL, 0);
    nextObjStaticMacFilteringVlanIdValue = DOT1Q_DEFAULT_VLAN;
    owa.l7rc = usmDbVlanIDGet(L7_UNIT_CURRENT, nextObjStaticMacFilteringVlanIdValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.l7rc = usmDbNextVlanGet(L7_UNIT_CURRENT, nextObjStaticMacFilteringVlanIdValue,
                        &nextObjStaticMacFilteringVlanIdValue);
    }
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objStaticMacFilteringVlanIdValue, owa.len);
    owa.l7rc = usmDbNextVlanGet (L7_UNIT_CURRENT,
                                  objStaticMacFilteringVlanIdValue,
                                  &nextObjStaticMacFilteringVlanIdValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjStaticMacFilteringVlanIdValue, owa.len);

  /* return the object value: StaticMacFilteringVlanId */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & nextObjStaticMacFilteringVlanIdValue,
                    sizeof (objStaticMacFilteringVlanIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_SwitchingStaticMACFilterConfig_StaticMacFilteringVlanId
*
* @purpose Get 'StaticMacFilteringVlanId'
*
* @description [StaticMacFilteringVlanId]: The Static MAC Filter Vlan Id 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingStaticMACFilterConfig_StaticMacFilteringVlanId (void
                                                                           *wap,
                                                                           void
                                                                           *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStaticMacFilteringVlanIdValue=0;
  xLibU32_t nextObjStaticMacFilteringVlanIdValue=0;
  xLibStr6_t objStaticMacFilteringAddressValue={0};
  xLibStr6_t nextObjStaticMacFilteringAddressValue={0};
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StaticMacFilteringVlanId */
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_SwitchingStaticMACFilterConfig_StaticMacFilteringVlanId,
                   (xLibU8_t *) & objStaticMacFilteringVlanIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, (char *)NULL, 0);
    objStaticMacFilteringVlanIdValue = 0;

    owa.l7rc = usmDbFilterNextGet (L7_UNIT_CURRENT, objStaticMacFilteringAddressValue,
                            objStaticMacFilteringVlanIdValue,
                            nextObjStaticMacFilteringAddressValue,
                            &nextObjStaticMacFilteringVlanIdValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objStaticMacFilteringVlanIdValue, owa.len);
    
    do
    {
      owa.l7rc = usmDbFilterNextGet (L7_UNIT_CURRENT, objStaticMacFilteringAddressValue,
                                     objStaticMacFilteringVlanIdValue,
                                     nextObjStaticMacFilteringAddressValue,
                                     &nextObjStaticMacFilteringVlanIdValue);
      memcpy(objStaticMacFilteringAddressValue,nextObjStaticMacFilteringAddressValue,sizeof(objStaticMacFilteringAddressValue));
    }while((objStaticMacFilteringVlanIdValue == nextObjStaticMacFilteringVlanIdValue) && (owa.l7rc == L7_SUCCESS));  
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjStaticMacFilteringVlanIdValue, owa.len);

  /* return the object value: StaticMacFilteringVlanId */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & nextObjStaticMacFilteringVlanIdValue,
                    sizeof (objStaticMacFilteringVlanIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingStaticMACFilterConfig_StaticMacFilteringAddress
*
* @purpose Get 'StaticMacFilteringAddress'
*
* @description [StaticMacFilteringAddress]: The Static MAC Filter MAC address
*              
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingStaticMACFilterConfig_StaticMacFilteringAddress (void
                                                                            *wap,
                                                                            void
                                                                            *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t objStaticMacFilteringAddressValue;
  xLibStr6_t nextObjStaticMacFilteringAddressValue;
  xLibU32_t  MacFilteringVlanIdValue;
  xLibU32_t  nextMacFilteringVlanIdValue;

	xLibStr6_t tempObjStaticMacFilteringAddressValue;
  xLibU32_t  tempMacFilteringVlanIdValue;
	
  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof(MacFilteringVlanIdValue);

  owa.rc =
    xLibFilterGet (wap, XOBJ_SwitchingStaticMACFilterConfig_StaticMacFilteringVlanId,
                   (xLibU8_t *) & MacFilteringVlanIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  /* retrieve key: StaticMacFilteringAddress */
  owa.len = sizeof(objStaticMacFilteringAddressValue);

  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingStaticMACFilterConfig_StaticMacFilteringAddress,
                   (xLibU8_t *) objStaticMacFilteringAddressValue, &owa.len);

  memset(nextObjStaticMacFilteringAddressValue, 0x00, sizeof(nextObjStaticMacFilteringAddressValue));
  nextMacFilteringVlanIdValue = 0;
  memset(tempObjStaticMacFilteringAddressValue, 0x00, sizeof(tempObjStaticMacFilteringAddressValue));
  tempMacFilteringVlanIdValue = 0;

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, (char *)NULL, 0);

    do
    {
      owa.l7rc = usmDbFilterNextGet (L7_UNIT_CURRENT, tempObjStaticMacFilteringAddressValue,
                            tempMacFilteringVlanIdValue,
                            nextObjStaticMacFilteringAddressValue,
                            &nextMacFilteringVlanIdValue);

      memcpy(tempObjStaticMacFilteringAddressValue,nextObjStaticMacFilteringAddressValue,
              sizeof(tempObjStaticMacFilteringAddressValue));
      tempMacFilteringVlanIdValue = nextMacFilteringVlanIdValue;

    }while((MacFilteringVlanIdValue != nextMacFilteringVlanIdValue)
           && (owa.l7rc == L7_SUCCESS));
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objStaticMacFilteringAddressValue, owa.len);

    do
    {
      owa.l7rc = usmDbFilterNextGet (L7_UNIT_CURRENT, tempObjStaticMacFilteringAddressValue,
                            tempMacFilteringVlanIdValue,
                            nextObjStaticMacFilteringAddressValue,
                            &nextMacFilteringVlanIdValue);

      memcpy(tempObjStaticMacFilteringAddressValue,nextObjStaticMacFilteringAddressValue,
              sizeof(tempObjStaticMacFilteringAddressValue));
      tempMacFilteringVlanIdValue = nextMacFilteringVlanIdValue;

    }while((MacFilteringVlanIdValue != nextMacFilteringVlanIdValue)
           && (owa.l7rc == L7_SUCCESS));

    if(owa.l7rc == L7_SUCCESS)
    {

      while((MacFilteringVlanIdValue == nextMacFilteringVlanIdValue)
               &&( memcmp(objStaticMacFilteringAddressValue,
               nextObjStaticMacFilteringAddressValue,
               sizeof(nextObjStaticMacFilteringAddressValue)))
               && (owa.l7rc == L7_SUCCESS))
      {
           owa.l7rc = usmDbFilterNextGet (L7_UNIT_CURRENT, tempObjStaticMacFilteringAddressValue,
                              tempMacFilteringVlanIdValue,
                              nextObjStaticMacFilteringAddressValue,
                              &nextMacFilteringVlanIdValue);
          memcpy(tempObjStaticMacFilteringAddressValue,nextObjStaticMacFilteringAddressValue,
                  sizeof(tempObjStaticMacFilteringAddressValue));
          tempMacFilteringVlanIdValue = nextMacFilteringVlanIdValue;

       }
       if(owa.l7rc == L7_SUCCESS)
       {
          if(MacFilteringVlanIdValue == nextMacFilteringVlanIdValue)
          {
             owa.l7rc = usmDbFilterNextGet (L7_UNIT_CURRENT, tempObjStaticMacFilteringAddressValue,
                              tempMacFilteringVlanIdValue,
                              nextObjStaticMacFilteringAddressValue,
                              &nextMacFilteringVlanIdValue);
             if( MacFilteringVlanIdValue != nextMacFilteringVlanIdValue)
			 {
               owa.l7rc = L7_FAILURE;
			 }  
          }
          else
		  {
            owa.l7rc = L7_FAILURE;
		  }	
        }
    }
  }

  if (owa.l7rc != L7_SUCCESS )
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, nextObjStaticMacFilteringAddressValue, owa.len);

  /* return the object value: StaticMacFilteringAddress */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) nextObjStaticMacFilteringAddressValue,
                    sizeof (objStaticMacFilteringAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingStaticMACFilterConfig_StaticMacFilteringSourcePortMask
*
* @purpose Get 'StaticMacFilteringSourcePortMask'
 *@description  [StaticMacFilteringSourcePortMask] The Static MAC Filter source
* port mask. When setting this value, the system will ignore
* configuration for ports not between the first and last valid ports.
* Configuration of any port numbers between this range that are not
* valid ports return a failure mes   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingStaticMACFilterConfig_StaticMacFilteringSourcePortMask (void *wap,
                                                                                   void *bufp)
{

  fpObjWa_t kwaStaticMacFilteringVlanId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStaticMacFilteringVlanIdValue;
  fpObjWa_t kwaStaticMacFilteringAddress = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t keyStaticMacFilteringAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_INTF_MASK_t));
  L7_INTF_MASK_t objStaticMacFilteringSourcePortMaskValue;
  L7_INTF_MASK_t nextObjStaticMacFilteringSourcePortMaskValue;
  xLibU32_t intfList[L7_FILTER_MAX_INTF];
  xLibU32_t numIntf;
  xLibU32_t intfValue;
  xLibU32_t i = 0;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StaticMacFilteringVlanId */
  kwaStaticMacFilteringVlanId.rc =
    xLibFilterGet (wap, XOBJ_SwitchingStaticMACFilterConfig_StaticMacFilteringVlanId,
                   (xLibU8_t *) & keyStaticMacFilteringVlanIdValue,
                   &kwaStaticMacFilteringVlanId.len);
  if (kwaStaticMacFilteringVlanId.rc != XLIBRC_SUCCESS)
  {
    kwaStaticMacFilteringVlanId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaStaticMacFilteringVlanId);
    return kwaStaticMacFilteringVlanId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStaticMacFilteringVlanIdValue,
                           kwaStaticMacFilteringVlanId.len);

  /* retrieve key: StaticMacFilteringAddress */
  kwaStaticMacFilteringAddress.rc =
    xLibFilterGet (wap, XOBJ_SwitchingStaticMACFilterConfig_StaticMacFilteringAddress,
                   (xLibU8_t *) keyStaticMacFilteringAddressValue,
                   &kwaStaticMacFilteringAddress.len);
  if (kwaStaticMacFilteringAddress.rc != XLIBRC_SUCCESS)
  {
    kwaStaticMacFilteringAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaStaticMacFilteringAddress);
    return kwaStaticMacFilteringAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyStaticMacFilteringAddressValue,
                           kwaStaticMacFilteringAddress.len);

  owa.l7rc = usmDbFilterSrcIntfListGet(L7_UNIT_CURRENT,keyStaticMacFilteringAddressValue,keyStaticMacFilteringVlanIdValue,&numIntf,intfList);  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

 	memset(&nextObjStaticMacFilteringSourcePortMaskValue,0x00, sizeof (L7_INTF_MASK_t));

  while(numIntf != 0)
  {
    if(numIntf >= 1)
    {
      intfValue = intfList[i];
	  L7_INTF_SETMASKBIT(nextObjStaticMacFilteringSourcePortMaskValue, intfValue);
    } 
    i++;
    numIntf--;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjStaticMacFilteringSourcePortMaskValue, sizeof (L7_INTF_MASK_t));

  /* return the object value: StaticMacFilteringSourcePortMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjStaticMacFilteringSourcePortMaskValue,
                           sizeof(objStaticMacFilteringSourcePortMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjList_SwitchingStaticMACFilterConfig_StaticMacFilteringSourcePortMask
*
* @purpose List 'StaticMacFilteringSourcePortMask'
 *@description  [StaticMacFilteringSourcePortMask] The Static MAC Filter source
* port mask. When setting this value, the system will ignore
* configuration for ports not between the first and last valid ports.
* Configuration of any port numbers between this range that are not
* valid ports return a failure mes   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_SwitchingStaticMACFilterConfig_StaticMacFilteringSourcePortMask (void *wap,
                                                                                    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_INTF_MASK_t));
  L7_INTF_MASK_t interfaceMask;
  xLibU32_t nextObjInterfaceValue;
  xLibU32_t portChannelIf=0;

  memset(&interfaceMask, 0x00, sizeof (L7_INTF_MASK_t));
  FPOBJ_TRACE_ENTER (bufp);

  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingStaticMACFilterConfig_StaticMacFilteringSourcePortMask,
                          (xLibU8_t *) &interfaceMask, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjInterfaceValue=0;
    owa.l7rc = usmDbGetNextPhysicalIntIfNumber(nextObjInterfaceValue,&nextObjInterfaceValue);
    while (usmDbDot3adIntfIsMemberGet(L7_UNIT_CURRENT, nextObjInterfaceValue, &portChannelIf) == L7_SUCCESS)
    {
      owa.l7rc = usmDbGetNextPhysicalIntIfNumber(nextObjInterfaceValue,&nextObjInterfaceValue);
      if ( owa.l7rc != L7_SUCCESS )
      {
        break;
      }
    }
  }
  else
  {
    nextObjInterfaceValue = fpObjPortFromMaskGet(interfaceMask);
    if(nextObjInterfaceValue == -1)
    {
      owa.rc = XLIBRC_ENDOF_TABLE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_CURRENT_KEY (bufp, &nextObjInterfaceValue, owa.len);
    owa.l7rc = usmDbGetNextPhysicalIntIfNumber(nextObjInterfaceValue,&nextObjInterfaceValue);
    while (usmDbDot3adIntfIsMemberGet(L7_UNIT_CURRENT, nextObjInterfaceValue, &portChannelIf) == L7_SUCCESS)
    {
      owa.l7rc = usmDbGetNextPhysicalIntIfNumber(nextObjInterfaceValue,&nextObjInterfaceValue);
      if ( owa.l7rc != L7_SUCCESS )
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
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &interfaceMask,
                           sizeof (L7_INTF_MASK_t));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingStaticMACFilterConfig_StaticMacFilteringSourcePortMask
*
* @purpose Set 'StaticMacFilteringSourcePortMask'
 *@description  [StaticMacFilteringSourcePortMask] The Static MAC Filter source
* port mask. When setting this value, the system will ignore
* configuration for ports not between the first and last valid ports.
* Configuration of any port numbers between this range that are not
* valid ports return a failure mes   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingStaticMACFilterConfig_StaticMacFilteringSourcePortMask (void *wap,
                                                                                   void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_INTF_MASK_t));
  L7_INTF_MASK_t intfMask;
  memset(&intfMask,0x0,sizeof(intfMask));

  fpObjWa_t kwaStaticMacFilteringVlanId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStaticMacFilteringVlanIdValue;
  fpObjWa_t kwaStaticMacFilteringAddress = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t keyStaticMacFilteringAddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  L7_uint32 numIntf, intIfNum, numIntfPrev,intfCount,i, j, numIntfToRemove, numIntfToAdd;
 
  L7_uint32 intfList[L7_FILTER_MAX_INTF];        /*L7_MAX_PHYSICAL_PORTS_PER_SLOT*/
  L7_uint32 intfListPrev[L7_FILTER_MAX_INTF];
  L7_uint32 intfListToAdd[L7_FILTER_MAX_INTF];
  L7_uint32 intfListToRemove[L7_FILTER_MAX_INTF];
  L7_BOOL intfFound;
  L7_BOOL errorFound;
  L7_RC_t rc;
 
  errorFound = L7_FALSE;

  /* retrieve key: StaticMacFilteringVlanId */
  kwaStaticMacFilteringVlanId.rc =
    xLibFilterGet (wap, XOBJ_SwitchingStaticMACFilterConfig_StaticMacFilteringVlanId,
                   (xLibU8_t *) & keyStaticMacFilteringVlanIdValue,
                   &kwaStaticMacFilteringVlanId.len);
  if (kwaStaticMacFilteringVlanId.rc != XLIBRC_SUCCESS)
  {
    kwaStaticMacFilteringVlanId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaStaticMacFilteringVlanId);
    return kwaStaticMacFilteringVlanId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStaticMacFilteringVlanIdValue,
                           kwaStaticMacFilteringVlanId.len);

  /* retrieve key: StaticMacFilteringAddress */
  kwaStaticMacFilteringAddress.rc =
    xLibFilterGet (wap, XOBJ_SwitchingStaticMACFilterConfig_StaticMacFilteringAddress,
                   (xLibU8_t *) keyStaticMacFilteringAddressValue,
                   &kwaStaticMacFilteringAddress.len);
  if (kwaStaticMacFilteringAddress.rc != XLIBRC_SUCCESS)
  {
    kwaStaticMacFilteringAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaStaticMacFilteringAddress);
    return kwaStaticMacFilteringAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyStaticMacFilteringAddressValue,
                           kwaStaticMacFilteringAddress.len);

  /* retrieve object: InterfaceMask */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) &intfMask, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
 
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

  rc = usmDbFilterSrcIntfListGet(L7_UNIT_CURRENT,keyStaticMacFilteringAddressValue,keyStaticMacFilteringVlanIdValue,
                               &numIntfPrev, intfListPrev);

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
      rc = usmdbFilterSrcIntfAdd(L7_UNIT_CURRENT, keyStaticMacFilteringAddressValue, keyStaticMacFilteringVlanIdValue, intfListToAdd[i]);
      if (rc != L7_SUCCESS)
      {
        errorFound = L7_TRUE;
      }
    }
    
    for (i=0; i<numIntfToRemove; i++)
    {
      rc = usmdbFilterSrcIntfDelete(L7_UNIT_CURRENT,  keyStaticMacFilteringAddressValue,
                            keyStaticMacFilteringVlanIdValue, intfListToRemove[i]);
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
      rc = usmdbFilterSrcIntfDelete(L7_UNIT_CURRENT,  keyStaticMacFilteringAddressValue,
                            keyStaticMacFilteringVlanIdValue, intfListPrev[i]);
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
* @function fpObjGet_SwitchingStaticMACFilterConfig_StaticMacFilteringDestPortMask
*
* @purpose Get 'StaticMacFilteringDestPortMask'
 *@description  [StaticMacFilteringDestPortMask] The Static MAC Filter
* destination port . When setting this value, the system will ignore
* configuration for ports not between the first and last valid ports.
* Configuration of any port numbers between this range that are not
* valid ports return a failur   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingStaticMACFilterConfig_StaticMacFilteringDestPortMask (void *wap,
                                                                                 void *bufp)
{

  fpObjWa_t kwaStaticMacFilteringVlanId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStaticMacFilteringVlanIdValue;
  fpObjWa_t kwaStaticMacFilteringAddress = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t keyStaticMacFilteringAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_INTF_MASK_t));
  L7_INTF_MASK_t objStaticMacFilteringDestPortMaskValue;
  L7_INTF_MASK_t nextObjStaticMacFilteringDestPortMaskValue;
  xLibU32_t intfList[L7_FILTER_MAX_INTF];
  xLibU32_t numIntf;
  xLibS32_t intfValue;
  xLibU32_t i = 0;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StaticMacFilteringVlanId */
  kwaStaticMacFilteringVlanId.rc =
    xLibFilterGet (wap, XOBJ_SwitchingStaticMACFilterConfig_StaticMacFilteringVlanId,
                   (xLibU8_t *) & keyStaticMacFilteringVlanIdValue,
                   &kwaStaticMacFilteringVlanId.len);
  if (kwaStaticMacFilteringVlanId.rc != XLIBRC_SUCCESS)
  {
    kwaStaticMacFilteringVlanId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaStaticMacFilteringVlanId);
    return kwaStaticMacFilteringVlanId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStaticMacFilteringVlanIdValue,
                           kwaStaticMacFilteringVlanId.len);

  /* retrieve key: StaticMacFilteringAddress */
  kwaStaticMacFilteringAddress.rc =
    xLibFilterGet (wap, XOBJ_SwitchingStaticMACFilterConfig_StaticMacFilteringAddress,
                   (xLibU8_t *) keyStaticMacFilteringAddressValue,
                   &kwaStaticMacFilteringAddress.len);
  if (kwaStaticMacFilteringAddress.rc != XLIBRC_SUCCESS)
  {
    kwaStaticMacFilteringAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaStaticMacFilteringAddress);
    return kwaStaticMacFilteringAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyStaticMacFilteringAddressValue,
                           kwaStaticMacFilteringAddress.len);


  owa.l7rc = usmDbFilterDstIntfListGet(L7_UNIT_CURRENT,keyStaticMacFilteringAddressValue,
                                     keyStaticMacFilteringVlanIdValue,&numIntf,intfList);  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  memset(&nextObjStaticMacFilteringDestPortMaskValue,0x00, sizeof (L7_INTF_MASK_t));

  while(numIntf != 0)
  {
    if(numIntf >= 1)
    {
      intfValue = intfList[i];
      L7_INTF_SETMASKBIT(nextObjStaticMacFilteringDestPortMaskValue, intfValue);
    }   
    i++;
    numIntf--;
  }
 
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjStaticMacFilteringDestPortMaskValue, 
                            sizeof (L7_INTF_MASK_t));

  /* return the object value: StaticMacFilteringDestPortMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjStaticMacFilteringDestPortMaskValue,
                           sizeof(objStaticMacFilteringDestPortMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjList_SwitchingStaticMACFilterConfig_StaticMacFilteringDestPortMask
*
* @purpose List 'StaticMacFilteringDestPortMask'
 *@description  [StaticMacFilteringDestPortMask] The Static MAC Filter
* destination port . When setting this value, the system will ignore
* configuration for ports not between the first and last valid ports.
* Configuration of any port numbers between this range that are not
* valid ports return a failur   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_SwitchingStaticMACFilterConfig_StaticMacFilteringDestPortMask (void *wap,
                                                                                  void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_INTF_MASK_t));
  L7_INTF_MASK_t interfaceMask;
  xLibU32_t portChannelIf=0;
  xLibU32_t nextObjInterfaceValue;

  memset(&interfaceMask, 0x00, sizeof (L7_INTF_MASK_t));
  FPOBJ_TRACE_ENTER (bufp);


  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingStaticMACFilterConfig_StaticMacFilteringDestPortMask,
                          (xLibU8_t *) &interfaceMask, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjInterfaceValue = 0;
    owa.l7rc = usmDbGetNextPhysicalIntIfNumber(nextObjInterfaceValue,&nextObjInterfaceValue);
    while (usmDbDot3adIntfIsMemberGet(L7_UNIT_CURRENT, nextObjInterfaceValue, &portChannelIf) == L7_SUCCESS)
    {
      owa.l7rc = usmDbGetNextPhysicalIntIfNumber(nextObjInterfaceValue,&nextObjInterfaceValue);
      if ( owa.l7rc != L7_SUCCESS )
      {
        break;
      }
    }
  }
  else
  {
    nextObjInterfaceValue = fpObjPortFromMaskGet(interfaceMask);
    if(nextObjInterfaceValue == -1)
    {
      owa.rc = XLIBRC_ENDOF_TABLE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_CURRENT_KEY (bufp, &nextObjInterfaceValue, owa.len);
    owa.l7rc = usmDbGetNextPhysicalIntIfNumber(nextObjInterfaceValue,&nextObjInterfaceValue);
    while (usmDbDot3adIntfIsMemberGet(L7_UNIT_CURRENT, nextObjInterfaceValue, &portChannelIf) == L7_SUCCESS)
    {
      owa.l7rc = usmDbGetNextPhysicalIntIfNumber(nextObjInterfaceValue,&nextObjInterfaceValue);
      if ( owa.l7rc != L7_SUCCESS )
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
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &interfaceMask,
                           sizeof (L7_INTF_MASK_t));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingStaticMACFilterConfig_StaticMacFilteringDestPortMask
*
* @purpose Set 'StaticMacFilteringDestPortMask'
 *@description  [StaticMacFilteringDestPortMask] The Static MAC Filter
* destination port . When setting this value, the system will ignore
* configuration for ports not between the first and last valid ports.
* Configuration of any port numbers between this range that are not
* valid ports return a failur   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingStaticMACFilterConfig_StaticMacFilteringDestPortMask (void *wap,
                                                                                 void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_INTF_MASK_t));
  L7_INTF_MASK_t intfMask;
  memset(&intfMask,0x0,sizeof(intfMask));

  fpObjWa_t kwaStaticMacFilteringVlanId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStaticMacFilteringVlanIdValue;
  fpObjWa_t kwaStaticMacFilteringAddress = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t keyStaticMacFilteringAddressValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  L7_uint32 numIntf, intIfNum, numIntfPrev,intfCount,i, j, numIntfToRemove, numIntfToAdd;

  L7_uint32 intfList[L7_FILTER_MAX_INTF];        /*L7_MAX_PHYSICAL_PORTS_PER_SLOT*/
  L7_uint32 intfListPrev[L7_FILTER_MAX_INTF];
  L7_uint32 intfListToAdd[L7_FILTER_MAX_INTF];
  L7_uint32 intfListToRemove[L7_FILTER_MAX_INTF];
  L7_BOOL intfFound;
  L7_BOOL errorFound;
  L7_RC_t rc;

  errorFound = L7_FALSE;


  /* retrieve key: StaticMacFilteringVlanId */
  kwaStaticMacFilteringVlanId.rc =
    xLibFilterGet (wap, XOBJ_SwitchingStaticMACFilterConfig_StaticMacFilteringVlanId,
                   (xLibU8_t *) & keyStaticMacFilteringVlanIdValue,
                   &kwaStaticMacFilteringVlanId.len);
  if (kwaStaticMacFilteringVlanId.rc != XLIBRC_SUCCESS)
  {
    kwaStaticMacFilteringVlanId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaStaticMacFilteringVlanId);
    return kwaStaticMacFilteringVlanId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStaticMacFilteringVlanIdValue,
                           kwaStaticMacFilteringVlanId.len);

  /* retrieve key: StaticMacFilteringAddress */
  kwaStaticMacFilteringAddress.rc =
    xLibFilterGet (wap, XOBJ_SwitchingStaticMACFilterConfig_StaticMacFilteringAddress,
                   (xLibU8_t *) keyStaticMacFilteringAddressValue,
                   &kwaStaticMacFilteringAddress.len);
  if (kwaStaticMacFilteringAddress.rc != XLIBRC_SUCCESS)
  {
    kwaStaticMacFilteringAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaStaticMacFilteringAddress);
    return kwaStaticMacFilteringAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyStaticMacFilteringAddressValue,
                           kwaStaticMacFilteringAddress.len);

   /* retrieve object: InterfaceMask */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) &intfMask, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

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

  rc = usmDbFilterDstIntfListGet(L7_UNIT_CURRENT,keyStaticMacFilteringAddressValue,keyStaticMacFilteringVlanIdValue,
                               &numIntfPrev, intfListPrev);

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
      rc = usmdbFilterDstIntfAdd(L7_UNIT_CURRENT, keyStaticMacFilteringAddressValue, keyStaticMacFilteringVlanIdValue, intfListToAdd[i]);
      if (rc != L7_SUCCESS)
      {
        errorFound = L7_TRUE;
      }
    }

    for (i=0; i<numIntfToRemove; i++)
    {
      rc = usmdbFilterDstIntfDelete(L7_UNIT_CURRENT,  keyStaticMacFilteringAddressValue,
                            keyStaticMacFilteringVlanIdValue, intfListToRemove[i]);
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
      rc = usmdbFilterDstIntfDelete(L7_UNIT_CURRENT,  keyStaticMacFilteringAddressValue,
                            keyStaticMacFilteringVlanIdValue, intfListPrev[i]);
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
* @function fpObjGet_SwitchingStaticMACFilterConfig_StaticMacFilteringStatus
*
* @purpose Get 'StaticMacFilteringStatus'
*
* @description [StaticMacFilteringStatus]: The Static MAC Filter status. Supported
*              values: Remove - removes the entry 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingStaticMACFilterConfig_StaticMacFilteringStatus (void
                                                                           *wap,
                                                                           void
                                                                           *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStaticMacFilteringVlanIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t keyStaticMacFilteringAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStaticMacFilteringStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StaticMacFilteringVlanId */
  kwa1.rc =
    xLibFilterGet (wap,
                   XOBJ_SwitchingStaticMACFilterConfig_StaticMacFilteringVlanId,
                   (xLibU8_t *) & keyStaticMacFilteringVlanIdValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStaticMacFilteringVlanIdValue, kwa1.len);

  /* retrieve key: StaticMacFilteringAddress */
  kwa2.rc =
    xLibFilterGet (wap,
                   XOBJ_SwitchingStaticMACFilterConfig_StaticMacFilteringAddress,
                   (xLibU8_t *) keyStaticMacFilteringAddressValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyStaticMacFilteringAddressValue, kwa2.len);

  objStaticMacFilteringStatusValue = L7_ROW_STATUS_ACTIVE;

  /* return the object value: StaticMacFilteringStatus */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objStaticMacFilteringStatusValue,
                    sizeof (objStaticMacFilteringStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingStaticMACFilterConfig_StaticMacFilteringStatus
*
* @purpose Set 'StaticMacFilteringStatus'
*
* @description [StaticMacFilteringStatus]: The Static MAC Filter status. Supported
*              values: Remove - removes the entry 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingStaticMACFilterConfig_StaticMacFilteringStatus (void
                                                                           *wap,
                                                                           void
                                                                           *bufp)
{
  L7_BOOL brc;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStaticMacFilteringStatusValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStaticMacFilteringVlanIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t keyStaticMacFilteringAddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: StaticMacFilteringStatus */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objStaticMacFilteringStatusValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStaticMacFilteringStatusValue, owa.len);

  /* retrieve key: StaticMacFilteringVlanId */
  kwa1.rc =
    xLibFilterGet (wap,
                   XOBJ_SwitchingStaticMACFilterConfig_StaticMacFilteringVlanId,
                   (xLibU8_t *) & keyStaticMacFilteringVlanIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_VLANID_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStaticMacFilteringVlanIdValue, kwa1.len);

  /* retrieve key: StaticMacFilteringAddress */
  kwa2.rc =
    xLibFilterGet (wap,
                   XOBJ_SwitchingStaticMACFilterConfig_StaticMacFilteringAddress,
                   (xLibU8_t *) keyStaticMacFilteringAddressValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_MAC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyStaticMacFilteringAddressValue, kwa2.len);

  /* call the usmdb only for add and delete */
  if (objStaticMacFilteringStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {

    owa.l7rc = usmDbVlanIDGet(L7_UNIT_CURRENT, keyStaticMacFilteringVlanIdValue);
    if(owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_STATIC_MAC_FILTER_INVALID_VLAN; 
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    
    brc = usmDbFilterIsRestrictedFilterMac(keyStaticMacFilteringAddressValue);
    if(brc != L7_FALSE)
    {
      owa.rc = XLIBRC_STATIC_MAC_FILTER_INVALID_MAC; 
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

    /* Create a row */
    owa.l7rc = usmDbFilterCreate(L7_UNIT_CURRENT, 
                                  keyStaticMacFilteringAddressValue, 
                                  keyStaticMacFilteringVlanIdValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_MAC_FILTER_CREATE_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else if (objStaticMacFilteringStatusValue == L7_ROW_STATUS_DESTROY)
  {
    /* Delete the existing row */
    owa.l7rc = usmDbFilterRemove (L7_UNIT_CURRENT, 
                                  keyStaticMacFilteringAddressValue,
                                  keyStaticMacFilteringVlanIdValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_MAC_FILTER_DEL_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  return XLIBRC_SUCCESS;
}


/*******************************************************************************
* @function fpObjSet_SwitchingStaticMACFilterConfig_StaticMacFilteringDeleteAll
*
* @purpose Set 'StaticMacFilteringDeleteAll'
*
* @description [StaticMacFilteringStatus]: The Static MAC Filter status. Supported
*              values: Remove - removes the entry 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingStaticMACFilterConfig_StaticMacFilteringDeleteAll (void
                                                                           *wap,
                                                                           void
                                                                           *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);
  xLibU32_t  vlanID;
  xLibStr6_t macAddr;


  owa.l7rc = usmDbFilterFirstGet(L7_UNIT_CURRENT, macAddr, &vlanID);
  if (owa.l7rc == L7_SUCCESS)
  {
    owa.l7rc = usmDbFilterRemove(L7_UNIT_CURRENT, macAddr, vlanID);
  }

  while (owa.l7rc == L7_SUCCESS)
  {
    owa.l7rc = usmDbFilterNextGet(L7_UNIT_CURRENT, macAddr, vlanID, macAddr, &vlanID);
    if (owa.l7rc == L7_SUCCESS)
    {
      owa.l7rc = usmDbFilterRemove(L7_UNIT_CURRENT, macAddr, vlanID);
    }
  }

  return XLIBRC_SUCCESS;
}
/*********************************************************



********************************************************/

xLibRC_t fpObjGet_SwitchingStaticMACFilterConfig_VidMac (void
                                                         *wap,
                                                          void
                                                          *bufp)
{
  L7_uchar8 vidMac[L7_VID_MAC_ADDR_LEN];
  xLibU16_t vidMacLen = sizeof (vidMac);
  L7_uchar8 mac[L7_ENET_MAC_ADDR_LEN];
  L7_uint32 vlanid;
  L7_uchar8 nextmac[L7_ENET_MAC_ADDR_LEN];
  L7_uint32 nextvlanid;
  L7_ushort16 vid;

  memset (mac, 0, sizeof (mac));
  memset (&vlanid,0,sizeof(vlanid));
  memset (nextmac,0,sizeof(nextmac));
  memset (&nextvlanid,0,sizeof(nextvlanid));
  memset (&vid,0,sizeof(vid));
  if (XLIBRC_NO_FILTER == xLibFilterGet (wap, XOBJ_SwitchingStaticMACFilterConfig_VidMac, vidMac, &vidMacLen))
  {
    if (usmDbFilterFirstGet (1, mac,&vlanid) == L7_SUCCESS)
    {
      usmDbEntryVidMacCombine(vlanid,mac,vidMac);
      xLibBufDataSet (bufp, (xLibU8_t *) vidMac, sizeof (vidMac));
      return XLIBRC_SUCCESS;
    }
    else
    {
       return XLIBRC_ENDOF_TABLE;
    }
  }
  else
  {
    memcpy(&vid,vidMac,2);
    mac[0]=vidMac[2];
    mac[1]=vidMac[3];
    mac[2]=vidMac[4];
    mac[3]=vidMac[5];
    mac[4]=vidMac[6];
     mac[5]=vidMac[7];
    if (usmDbFilterNextGet (1,mac,vid,nextmac,&nextvlanid) == L7_SUCCESS)
    {
      usmDbEntryVidMacCombine(nextvlanid,nextmac,vidMac);
      xLibBufDataSet (bufp, (xLibU8_t *) vidMac, sizeof (vidMac));
      return XLIBRC_SUCCESS;
    }
    else
    {
      return XLIBRC_ENDOF_TABLE;
    }
  }
  return XLIBRC_SUCCESS;
}


/*******************************************************************************
* @function fpObjGet_SwitchingStaticMACFilterConfig_StaticMacFilteringAdminMode
*
* @purpose Get 'StaticMacFilteringAdminMode'
 *@description  [StaticMacFilteringAdminMode] Delete All Static MAC Filtering
* Entries   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingStaticMACFilterConfig_StaticMacFilteringAdminMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objStaticMacFilteringAdminModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbFilterAdminModeGet (L7_UNIT_CURRENT, &objStaticMacFilteringAdminModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objStaticMacFilteringAdminModeValue,
                     sizeof (objStaticMacFilteringAdminModeValue));

  /* return the object value: StaticMacFilteringAdminMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStaticMacFilteringAdminModeValue,
                           sizeof (objStaticMacFilteringAdminModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingStaticMACFilterConfig_StaticMacFilteringAdminMode
*
* @purpose Set 'StaticMacFilteringAdminMode'
 *@description  [StaticMacFilteringAdminMode] Delete All Static MAC Filtering
* Entries   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingStaticMACFilterConfig_StaticMacFilteringAdminMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objStaticMacFilteringAdminModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: StaticMacFilteringAdminMode */
  owa.len = sizeof (objStaticMacFilteringAdminModeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objStaticMacFilteringAdminModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStaticMacFilteringAdminModeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbFilterAdminModeSet (L7_UNIT_CURRENT, objStaticMacFilteringAdminModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjGet_SwitchingStaticMACFilterConfig_vlanForwardingMode
*
* @purpose Get 'vlanForwardingMode'
 *@description  [vlanForwardingMode] Set Static MAC address filtering
* configuration   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingStaticMACFilterConfig_vlanForwardingMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objvlanForwardingModeValue;

  xLibU32_t keyStaticMacFilteringVlanIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StaticMacFilteringVlanId */
  owa.len = sizeof (keyStaticMacFilteringVlanIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingStaticMACFilterConfig_StaticMacFilteringVlanId,
                          (xLibU8_t *) & keyStaticMacFilteringVlanIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStaticMacFilteringVlanIdValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmdbFilterVlanFilteringModeGet (L7_UNIT_CURRENT, keyStaticMacFilteringVlanIdValue,
                              &objvlanForwardingModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objvlanForwardingModeValue, sizeof (objvlanForwardingModeValue));

  /* return the object value: vlanForwardingMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objvlanForwardingModeValue,
                           sizeof (objvlanForwardingModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingStaticMACFilterConfig_vlanForwardingMode
*
* @purpose Set 'vlanForwardingMode'
 *@description  [vlanForwardingMode] Set Static MAC address filtering
* configuration   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingStaticMACFilterConfig_vlanForwardingMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objvlanForwardingModeValue;

  xLibU32_t keyStaticMacFilteringVlanIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: vlanForwardingMode */
  owa.len = sizeof (objvlanForwardingModeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objvlanForwardingModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objvlanForwardingModeValue, owa.len);

  /* retrieve key: StaticMacFilteringVlanId */
  owa.len = sizeof (keyStaticMacFilteringVlanIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingStaticMACFilterConfig_StaticMacFilteringVlanId,
                          (xLibU8_t *) & keyStaticMacFilteringVlanIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStaticMacFilteringVlanIdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmdbFilterVlanFilteringModeSet (L7_UNIT_CURRENT, keyStaticMacFilteringVlanIdValue,
                              objvlanForwardingModeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
