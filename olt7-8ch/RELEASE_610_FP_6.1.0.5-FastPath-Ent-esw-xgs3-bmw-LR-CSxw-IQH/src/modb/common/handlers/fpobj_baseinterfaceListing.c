
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_baseinterfaceListing.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to base-object.xml
*
* @create  29 December 2008, Monday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_baseinterfaceListing_obj.h"
#include "usmdb_unitmgr_api.h"
#include "usmdb_util_api.h"
#include "usmdb_mib_vlan_api.h"
#include "usmdb_nim_api.h"

#ifdef L7_RLIM_PACKAGE
#include "usmdb_rlim_api.h"
#endif

L7_RC_t fpObjUtil_baseinterfaceListing_NextPhysicalIntfInterface(
                   xLibU32_t unit, xLibU32_t intfNum,  xLibU32_t *nextIntfNum)
{
   L7_uint32 u, s, p;
 L7_RC_t rc; 
 xLibU32_t tempIntfNum;
 if(intfNum == 0 ) /* get thefirst intf number */
 {
  if(unit == 0) /*get first intf for all interfaces */
  {
     rc = usmDbIntIfNumTypeFirstGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF,
                                 0, nextIntfNum);
  }
  else /* get first intf for given unit */
  {
   /*Loop till unit of the intf no is != Unit */
   rc = usmDbIntIfNumTypeFirstGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF,
                                 0, &tempIntfNum);
   if(rc == L7_SUCCESS)
   {
     if (usmDbUnitSlotPortGet(tempIntfNum, &u, &s, &p) != L7_SUCCESS)
     {
        return L7_FAILURE;;
     }
     if(u == unit)
     {
       *nextIntfNum = tempIntfNum;
     }
     else
     {
       do
       {
        if (usmDbUnitSlotPortGet(tempIntfNum, &u, &s, &p) == L7_SUCCESS)
        {/* Get the usp of the newly obtained interface number */
         if (u == unit)/* If it passes to the next unit. Thats all we want. Stop here.*/
         {
             *nextIntfNum = tempIntfNum;
          return L7_SUCCESS; /*You have the next interface of the specified unit*/
         }
        }
       }while(usmDbGetNextPhysicalIntIfNumber(tempIntfNum, &tempIntfNum) == L7_SUCCESS);
        return L7_FAILURE;
      }
     }
     else
     {
         return L7_FAILURE;
     }
  }
 }
 else /* get the next phy intf */
 {
  if(unit == 0) /*all units */
  {
    rc = usmDbGetNextPhysicalIntIfNumber(intfNum, nextIntfNum);
  }
  else /*for selected unit */
  {
    if (usmDbIntIfNumTypeNextGet(unit, USM_PHYSICAL_INTF, 0, intfNum, nextIntfNum) == L7_SUCCESS)
    {/* Get the next valid interface  */
      if (usmDbUnitSlotPortGet(*nextIntfNum, &u, &s, &p) == L7_SUCCESS)
      {/* Get the usp of the newly obtained interface number */
        if (u!= unit)/* If it passes to the next unit. Thats all we want. Stop here.*/
          return L7_FAILURE;
        else
          return L7_SUCCESS; /*You have the next interface of the specified unit*/
      }
      else
      {
        return L7_FAILURE;
      }
    }
    else
    {
      return L7_FAILURE;
    }
  }
 }
 
 return rc;
}
/*******************************************************************************
* @function fpObjGet_baseinterfaceListing_UnitIndex
*
* @purpose Get 'UnitIndex'
 *@description  [UnitIndex] Index for Supported Unit Types
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseinterfaceListing_UnitIndex2 (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objUnitIndexValue;
  xLibU32_t nextObjUnitIndexValue;
  xLibU32_t tempUnitType;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UnitIndex */
  owa.len = sizeof (objUnitIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseinterfaceListing_UnitIndex2,
                          (xLibU8_t *) & objUnitIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
   objUnitIndexValue = 0;
   nextObjUnitIndexValue = 0;
   tempUnitType = 0;
  }

  owa.l7rc = usmDbUnitMgrStackMemberGetNext (objUnitIndexValue,
                                             &nextObjUnitIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjUnitIndexValue, owa.len);

  /* return the object value: UnitIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjUnitIndexValue,
                           sizeof (nextObjUnitIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseinterfaceListing_PhysicalInterfaces
*
* @purpose Get 'PhysicalInterfaces'
 *@description  [PhysicalInterfaces] Physical Interface List
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseinterfaceListing_PhysicalInterfaces2 (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objPhysicalInterfacesValue;
  xLibU32_t nextObjPhysicalInterfacesValue;

  xLibU32_t keyUnitIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UnitIndex */
  owa.len = sizeof (keyUnitIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseinterfaceListing_UnitIndex2,
                          (xLibU8_t *) & keyUnitIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUnitIndexValue, owa.len);

  /* retrieve key: PhysicalInterfaces */
  owa.len = sizeof (objPhysicalInterfacesValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseinterfaceListing_PhysicalInterfaces2,
                          (xLibU8_t *) & objPhysicalInterfacesValue, &owa.len);
   if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
   objPhysicalInterfacesValue = 0;
   nextObjPhysicalInterfacesValue = 0;
    owa.l7rc = fpObjUtil_baseinterfaceListing_NextPhysicalIntfInterface (keyUnitIndexValue,
                                    objPhysicalInterfacesValue,  &nextObjPhysicalInterfacesValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objPhysicalInterfacesValue, owa.len);
    owa.l7rc = fpObjUtil_baseinterfaceListing_NextPhysicalIntfInterface ( keyUnitIndexValue,
                                    objPhysicalInterfacesValue, &nextObjPhysicalInterfacesValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjPhysicalInterfacesValue, owa.len);

  /* return the object value: PhysicalInterfaces */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjPhysicalInterfacesValue,
                           sizeof (nextObjPhysicalInterfacesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_baseinterfaceListing_UnitIndex
*
* @purpose Get 'UnitIndex'
 *@description  [UnitIndex] Index for Supported Unit Types   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseinterfaceListing_UnitIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnitIndexValue;
  xLibU32_t nextObjUnitIndexValue;

	
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UnitIndex */

  owa.rc = xLibFilterGet (wap, XOBJ_baseinterfaceListing_UnitIndex,
                          (xLibU8_t *) & objUnitIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objUnitIndexValue = 0;
    nextObjUnitIndexValue = 0;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objUnitIndexValue, owa.len);

  owa.l7rc = usmDbUnitMgrStackMemberGetNext (objUnitIndexValue,
                                                 &nextObjUnitIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjUnitIndexValue, owa.len);

  /* return the object value: UnitIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjUnitIndexValue,
                           sizeof (nextObjUnitIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseinterfaceListing_PhysicalInterfaces
*
* @purpose Get 'PhysicalInterfaces'
 *@description  [PhysicalInterfaces] Physical Interface List   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseinterfaceListing_PhysicalInterfaces (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objPhysicalInterfacesValue;
  xLibU32_t nextObjPhysicalInterfacesValue;

  xLibU32_t keyUnitIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UnitIndex */
  owa.len = sizeof (keyUnitIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseinterfaceListing_UnitIndex,
                          (xLibU8_t *) & keyUnitIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_SUCCESS;
    keyUnitIndexValue = 0;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUnitIndexValue, owa.len);

  /* retrieve key: PhysicalInterfaces */
  owa.len = sizeof (objPhysicalInterfacesValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseinterfaceListing_PhysicalInterfaces,
                          (xLibU8_t *) & objPhysicalInterfacesValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
	 objPhysicalInterfacesValue = 0;
	 nextObjPhysicalInterfacesValue = 0;
    owa.l7rc = fpObjUtil_baseinterfaceListing_NextPhysicalIntfInterface (keyUnitIndexValue,
                                    objPhysicalInterfacesValue,  &nextObjPhysicalInterfacesValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objPhysicalInterfacesValue, owa.len);
    owa.l7rc = fpObjUtil_baseinterfaceListing_NextPhysicalIntfInterface ( keyUnitIndexValue,
                                    objPhysicalInterfacesValue, &nextObjPhysicalInterfacesValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjPhysicalInterfacesValue, owa.len);

  /* return the object value: PhysicalInterfaces */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjPhysicalInterfacesValue,
                           sizeof (nextObjPhysicalInterfacesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
 33 * @function fpObjGet_baseinterfaceListing_fiberInterface
 34 *
 35 * @purpose Get 'fiberInterface'
 36  *@description  [fiberInterface] <HTML>Fiber Interface
 37 * @notes
 38 *
 39 * @return
 40 *******************************************************************************/
xLibRC_t fpObjGet_baseinterfaceListing_fiberInterface (void *wap, void *bufp)
 {
   fpObjWa_t owa = FPOBJ_INIT_WA2 ();
   xLibU32_t objfiberInterfaceValue;
   xLibU32_t nextObjfiberInterfaceValue;
   xLibU32_t connType;
   xLibU32_t keyUnitIndexValue;
   FPOBJ_TRACE_ENTER (bufp);

   /* retrieve key: UnitIndex */
   owa.len = sizeof (keyUnitIndexValue);
   owa.rc = xLibFilterGet (wap, XOBJ_baseinterfaceListing_UnitIndex,
                          (xLibU8_t *) & keyUnitIndexValue, &owa.len);
   if (owa.rc != XLIBRC_SUCCESS)
   {
     owa.rc = XLIBRC_SUCCESS;
     keyUnitIndexValue = 0;
   }

   FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUnitIndexValue, owa.len);
   
   /* retrieve key: fiberInterface */
   owa.len = sizeof (objfiberInterfaceValue);
   owa.rc = xLibFilterGet (wap, XOBJ_baseinterfaceListing_fiberInterface,
                           (xLibU8_t *) & objfiberInterfaceValue, &owa.len);
   if (owa.rc != XLIBRC_SUCCESS)
   {
     FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
     objfiberInterfaceValue = 0;
	 nextObjfiberInterfaceValue = 0;

    owa.l7rc = fpObjUtil_baseinterfaceListing_NextPhysicalIntfInterface (keyUnitIndexValue,
                                    objfiberInterfaceValue,  &nextObjfiberInterfaceValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objfiberInterfaceValue, owa.len);
    owa.l7rc = fpObjUtil_baseinterfaceListing_NextPhysicalIntfInterface ( keyUnitIndexValue,
                                    objfiberInterfaceValue, &nextObjfiberInterfaceValue);
  }
   

   /* Loop through all the interfaces and skip interfaces other than Copper */
   while(owa.l7rc == L7_SUCCESS)
   {
     if (L7_SUCCESS == usmDbPhysicalIntIfNumberCheck(0, nextObjfiberInterfaceValue))
     {
     if (( usmDbIntfPhyCapabilityGet(nextObjfiberInterfaceValue, &connType) == L7_SUCCESS ) )
     {
       if (connType & L7_PHY_CAP_PORTSPEED_SFP)
       {
         break;
       }
         else if (( usmDbIntfConnectorTypeGet(nextObjfiberInterfaceValue, &connType) == L7_SUCCESS ) )
         {
           if (L7_XAUI == connType)
           {
             break;
           }
     }
       } 
     }
     objfiberInterfaceValue =nextObjfiberInterfaceValue;
     owa.l7rc = usmDbValidIntIfNumNext (objfiberInterfaceValue,
                                     &nextObjfiberInterfaceValue);
   }

   if (owa.l7rc != L7_SUCCESS)
   {
     owa.rc = XLIBRC_ENDOF_TABLE;
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
   }

   FPOBJ_TRACE_NEW_KEY (bufp, &nextObjfiberInterfaceValue, owa.len);

   /* return the object value: fiberInterface */
   owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjfiberInterfaceValue,
                         sizeof (nextObjfiberInterfaceValue));
   FPOBJ_TRACE_EXIT (bufp, owa);
   return owa.rc;

 }


/*******************************************************************************
* @function fpObjGet_baseinterfaceListing_LagInterfaces2
*
* @purpose Get 'LagInterfaces'
 *@description  [LagInterfaces] Lag Interface List
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseinterfaceListing_LagInterfaces2 (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objLagInterfacesValue;
  xLibU32_t nextObjLagInterfacesValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: LagInterfaces */
  owa.len = sizeof (objLagInterfacesValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseinterfaceListing_LagInterfaces2,
                          (xLibU8_t *) & objLagInterfacesValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
  owa.l7rc = usmDbIntIfNumTypeFirstGet(L7_UNIT_CURRENT,
                                   USM_LAG_INTF, 0, &nextObjLagInterfacesValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objLagInterfacesValue, owa.len);
   owa.l7rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT,
                   USM_LAG_INTF, 0, objLagInterfacesValue, &nextObjLagInterfacesValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjLagInterfacesValue, owa.len);

  /* return the object value: LagInterfaces */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjLagInterfacesValue,
                           sizeof (nextObjLagInterfacesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseinterfaceListing_LagInterfaces
*
* @purpose Get 'LagInterfaces'
 *@description  [LagInterfaces] Lag Interface List   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseinterfaceListing_LagInterfaces (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objLagInterfacesValue;
  xLibU32_t nextObjLagInterfacesValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: LagInterfaces */
  owa.len = sizeof (objLagInterfacesValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseinterfaceListing_LagInterfaces,
                          (xLibU8_t *) & objLagInterfacesValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
	owa.l7rc = usmDbIntIfNumTypeFirstGet(L7_UNIT_CURRENT,
		                               USM_LAG_INTF, 0, &nextObjLagInterfacesValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objLagInterfacesValue, owa.len);
	 owa.l7rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT, 
	 	               USM_LAG_INTF, 0, objLagInterfacesValue, &nextObjLagInterfacesValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjLagInterfacesValue, owa.len);

  /* return the object value: LagInterfaces */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjLagInterfacesValue,
                           sizeof (nextObjLagInterfacesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseinterfaceListing_VLANRoutingInterfacesList
*
* @purpose Get 'VLANRoutingInterfacesList'
 *@description  [VLANRoutingInterfacesList] VLAN Interface List
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseinterfaceListing_VLANRoutingInterfacesList (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  owa.rc = XLIBRC_FAILURE;
  return owa.rc;
}

/*******************************************************************************
* @function fpObjList_baseinterfaceListing_VLANRoutingInterfacesList
*
* @purpose Get 'VLANRoutingInterfacesList'
 *@description  [VLANRoutingInterfacesList] VLAN Interface List
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseinterfaceListing_VLANRoutingInterfacesList (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
#ifdef L7_ROUTING_PACKAGE
  xLibU32_t objvLANInterfacesValue;
  xLibU32_t nextObjvLANInterfacesValue;
  xLibU32_t vlanId;

  FPOBJ_TRACE_ENTER (bufp);
  /* retrieve key: vLANInterfaces */
  owa.len = sizeof (objvLANInterfacesValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseinterfaceListing_VLANRoutingInterfacesList,
                          (xLibU8_t *) & objvLANInterfacesValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjvLANInterfacesValue = 0;
  }
  else
  {
     nextObjvLANInterfacesValue = objvLANInterfacesValue;
  }
  if ((owa.l7rc =usmDbNextIntIfNumberByTypeGet(L7_LOGICAL_VLAN_INTF, nextObjvLANInterfacesValue, &nextObjvLANInterfacesValue )) == L7_SUCCESS)
  {
    owa.l7rc=usmDbIpVlanRtrIntIfNumToVlanId(L7_UNIT_CURRENT, nextObjvLANInterfacesValue, &vlanId);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjvLANInterfacesValue, owa.len);

  /* return the object value: vLANInterfaces */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjvLANInterfacesValue,
                           sizeof (nextObjvLANInterfacesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
#endif

  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseinterfaceListing_VLANRoutingInterfacesList
*
* @purpose Get 'VLANRoutingInterfacesList'
 *@description  [VLANRoutingInterfacesList] VLAN Interface List
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseinterfaceListing_VLANRoutingInterfacesList(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseinterfaceListing_vLANInterfaces
*
* @purpose Get 'vLANInterfaces'
 *@description  [vLANInterfaces] VLAN Interface List   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseinterfaceListing_VLANInterfaces (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
#ifdef L7_ROUTING_PACKAGE
  xLibU32_t objvLANInterfacesValue;
  xLibU32_t nextObjvLANInterfacesValue;
  xLibU32_t vlanId;
  
  FPOBJ_TRACE_ENTER (bufp);
  /* retrieve key: vLANInterfaces */
  owa.len = sizeof (objvLANInterfacesValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseinterfaceListing_VLANInterfaces,
                          (xLibU8_t *) & objvLANInterfacesValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjvLANInterfacesValue = 0;
  }
  else
  {
     nextObjvLANInterfacesValue = objvLANInterfacesValue;
  }
  if ((owa.l7rc =usmDbNextIntIfNumberByTypeGet(L7_LOGICAL_VLAN_INTF, nextObjvLANInterfacesValue, &nextObjvLANInterfacesValue )) == L7_SUCCESS)
  {
    owa.l7rc=usmDbIpVlanRtrIntIfNumToVlanId(L7_UNIT_CURRENT, nextObjvLANInterfacesValue, &vlanId);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjvLANInterfacesValue, owa.len);

  /* return the object value: vLANInterfaces */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjvLANInterfacesValue,
                           sizeof (nextObjvLANInterfacesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
#endif

  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseinterfaceListing_VlanIndex2
*
* @purpose Get 'VlanIndex'
*
* @description [VlanIndex]: The VLAN-ID or other identifier refering to this
*              VLAN.
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseinterfaceListing_VlanIndex2 (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVlanIndexValue, nextVlanIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baseinterfaceListing_VlanIndex2,
                          (xLibU8_t *) & objVlanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objVlanIndexValue = 0;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objVlanIndexValue, owa.len);

  owa.l7rc = usmDbNextVlanGet(L7_UNIT_CURRENT, objVlanIndexValue, &nextVlanIndexValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextVlanIndexValue, owa.len);

  /* return the object value: VlanIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextVlanIndexValue,
                           sizeof (nextVlanIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}



/*******************************************************************************
* @function fpObjGet_baseinterfaceListing_VlanIndex
*
* @purpose Get 'VlanIndex'
*
* @description [VlanIndex]: The VLAN-ID or other identifier refering to this
*              VLAN. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseinterfaceListing_VlanIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVlanIndexValue, nextVlanIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baseinterfaceListing_VlanIndex,
                          (xLibU8_t *) & objVlanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objVlanIndexValue = 0;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objVlanIndexValue, owa.len);

  owa.l7rc = usmDbNextVlanGet(L7_UNIT_CURRENT, objVlanIndexValue, &nextVlanIndexValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextVlanIndexValue, owa.len);

  /* return the object value: VlanIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextVlanIndexValue,
                           sizeof (nextVlanIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/* Serves the purpose of Routing and IPv6 Modules */

/*******************************************************************************
* @function fpObjGet_baseinterfaceListing_VlanRtngIntfsLoopbacks
*
* @purpose Get 'VlanRtngIntfsLoopbacks'
 *@description  [VlanRtngIntfsLoopbacks] VLAN Routing Interfaces and Loopbacks List
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseinterfaceListing_VlanRtngIntfsLoopbacks (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
#ifdef L7_ROUTING_PACKAGE
  xLibU32_t objVlanRtngIntfsLoopbacks;
  xLibU32_t nextObjVlanRtngIntfsLoopbacks;
  xLibU32_t intfID;

  FPOBJ_TRACE_ENTER (bufp);
  /* retrieve key: VlanRtngIntfsLoopbacks */
  owa.len = sizeof (objVlanRtngIntfsLoopbacks);
  owa.rc = xLibFilterGet (wap, XOBJ_baseinterfaceListing_VlanRtngIntfsLoopbacks,
                          (xLibU8_t *) & objVlanRtngIntfsLoopbacks, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjVlanRtngIntfsLoopbacks = 0;
  }
  else
  {
    nextObjVlanRtngIntfsLoopbacks = objVlanRtngIntfsLoopbacks;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objVlanRtngIntfsLoopbacks, owa.len);
  intfID = 0;
#ifdef L7_RLIM_PACKAGE
    /* If the filter value is Loopback interface then should directly get the next Loopback Interface.
       Otherwise get the next VLAN Routing Interface.
    */
    if (usmDbRlimLoopbackIdGet(nextObjVlanRtngIntfsLoopbacks, &intfID) != L7_SUCCESS)  
    {
#endif
      if ((owa.l7rc =usmDbNextIntIfNumberByTypeGet(L7_LOGICAL_VLAN_INTF, nextObjVlanRtngIntfsLoopbacks, &nextObjVlanRtngIntfsLoopbacks)) == L7_SUCCESS)
      {
        owa.l7rc=usmDbIpVlanRtrIntIfNumToVlanId(L7_UNIT_CURRENT, nextObjVlanRtngIntfsLoopbacks, &intfID);
      }

#ifdef L7_RLIM_PACKAGE
      if(owa.l7rc != L7_SUCCESS)
      {
          nextObjVlanRtngIntfsLoopbacks = 0;
          owa.l7rc =usmDbNextIntIfNumberByTypeGet(L7_LOOPBACK_INTF, nextObjVlanRtngIntfsLoopbacks, &nextObjVlanRtngIntfsLoopbacks);
      }
    }
    else
    {
         owa.l7rc =usmDbNextIntIfNumberByTypeGet(L7_LOOPBACK_INTF, nextObjVlanRtngIntfsLoopbacks, &nextObjVlanRtngIntfsLoopbacks);
    }
#endif

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjVlanRtngIntfsLoopbacks, owa.len);

  /* return the object value: VlanRtngIntfsLoopbacks */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjVlanRtngIntfsLoopbacks,
      sizeof (nextObjVlanRtngIntfsLoopbacks));
  FPOBJ_TRACE_EXIT (bufp, owa);
#endif
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseinterfaceListing_VlanRtngIntfsTunnels
*
* @purpose Get 'VlanRtngIntfsTunnels'
 *@description  [VlanRtngIntfsTunnels] VLAN Routing Interfaces and Tunnels List
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseinterfaceListing_VlanRtngIntfsTunnels (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
#ifdef L7_IPV6_PACKAGE
  xLibU32_t objVlanRtngIntfsTunnels;
  xLibU32_t nextObjVlanRtngIntfsTunnels;
  xLibU32_t intfID;

  FPOBJ_TRACE_ENTER (bufp);
  /* retrieve key: VlanRtngIntfsLoopbacks */
  owa.len = sizeof (objVlanRtngIntfsTunnels);
  owa.rc = xLibFilterGet (wap, XOBJ_baseinterfaceListing_VlanRtngIntfsTunnels,
                          (xLibU8_t *) & objVlanRtngIntfsTunnels, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjVlanRtngIntfsTunnels = 0;
  }
  else
  {
    nextObjVlanRtngIntfsTunnels = objVlanRtngIntfsTunnels;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objVlanRtngIntfsTunnels, owa.len);
  intfID = 0;
#ifdef L7_RLIM_PACKAGE
    /* If the filter value is Tunnel interface then should directly get the next Tunnel Interface.
       Otherwise get the next VLAN Routing Interface.
    */
    if (usmDbRlimTunnelIdGet(nextObjVlanRtngIntfsTunnels, &intfID) != L7_SUCCESS)
    {
#endif
      if ((owa.l7rc =usmDbNextIntIfNumberByTypeGet(L7_LOGICAL_VLAN_INTF, nextObjVlanRtngIntfsTunnels, &nextObjVlanRtngIntfsTunnels)) == L7_SUCCESS)
      {
        owa.l7rc=usmDbIpVlanRtrIntIfNumToVlanId(L7_UNIT_CURRENT, nextObjVlanRtngIntfsTunnels, &intfID);
      }
#ifdef L7_RLIM_PACKAGE
      if(owa.l7rc != L7_SUCCESS)
      { 
         nextObjVlanRtngIntfsTunnels = 0;
         owa.l7rc =usmDbNextIntIfNumberByTypeGet(L7_TUNNEL_INTF, nextObjVlanRtngIntfsTunnels, &nextObjVlanRtngIntfsTunnels);
      }
    }/* end if Tunnel intf get */
    else
    {
      owa.l7rc =usmDbNextIntIfNumberByTypeGet(L7_TUNNEL_INTF, nextObjVlanRtngIntfsTunnels, &nextObjVlanRtngIntfsTunnels);
    }
#endif
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjVlanRtngIntfsTunnels, owa.len);

  /* return the object value: VlanRtngIntfsTunnels */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjVlanRtngIntfsTunnels,
      sizeof (nextObjVlanRtngIntfsTunnels));
  FPOBJ_TRACE_EXIT (bufp, owa);
#endif
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseinterfaceListing_VlanRtngIntfsLoopbacksTunnels
*
* @purpose Get 'VlanRtngIntfsLoopbacksTunnels'
 *@description  [VlanRtngIntfsLoopbacksTunnels] VLAN Routing Interfaces, Loopbacks and Tunnels List
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseinterfaceListing_VlanRtngIntfsLoopbacksTunnels (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
#ifdef L7_IPV6_PACKAGE
  xLibU32_t objVlanRtngIntfsLoopbacksTunnels;
  xLibU32_t nextObjVlanRtngIntfsLoopbacksTunnels;
  xLibU32_t intfID;

  FPOBJ_TRACE_ENTER (bufp);
  /* retrieve key: VlanRtngIntfsLoopbacksTunnels */
  owa.len = sizeof (objVlanRtngIntfsLoopbacksTunnels);
  owa.rc = xLibFilterGet (wap, XOBJ_baseinterfaceListing_VlanRtngIntfsLoopbacksTunnels,
                          (xLibU8_t *) & objVlanRtngIntfsLoopbacksTunnels, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjVlanRtngIntfsLoopbacksTunnels = 0;
    /* Get First Valid Vlan Routing Interface.If none are there then 
     ** get First Valid Loopback if none are there then get first valid Tunnel Interface 
     */
  }
  else
  {
    nextObjVlanRtngIntfsLoopbacksTunnels = objVlanRtngIntfsLoopbacksTunnels;
  }
   FPOBJ_TRACE_CURRENT_KEY (bufp, &nextObjVlanRtngIntfsLoopbacksTunnels, owa.len);
   intfID = 0;
#ifdef L7_RLIM_PACKAGE
    /* If the filter value is Tunnel interface then should directly get the next Tunnel Interface.
       Otherwise get the next VLAN Routing Interface.
    */
    if ((usmDbRlimTunnelIdGet(nextObjVlanRtngIntfsLoopbacksTunnels, &intfID) != L7_SUCCESS) && 
        (usmDbRlimLoopbackIdGet(nextObjVlanRtngIntfsLoopbacksTunnels,&intfID) != L7_SUCCESS))
    {
#endif
        if ((owa.l7rc =usmDbNextIntIfNumberByTypeGet(L7_LOGICAL_VLAN_INTF, nextObjVlanRtngIntfsLoopbacksTunnels, &nextObjVlanRtngIntfsLoopbacksTunnels)) == L7_SUCCESS)
        {
          owa.l7rc=usmDbIpVlanRtrIntIfNumToVlanId(L7_UNIT_CURRENT, nextObjVlanRtngIntfsLoopbacksTunnels, &intfID);
        }
#ifdef L7_RLIM_PACKAGE
      if(owa.l7rc != L7_SUCCESS)
      {
         nextObjVlanRtngIntfsLoopbacksTunnels = 0;
         owa.l7rc =usmDbNextIntIfNumberByTypeGet(L7_LOOPBACK_INTF, nextObjVlanRtngIntfsLoopbacksTunnels, &nextObjVlanRtngIntfsLoopbacksTunnels);
         if (owa.l7rc != L7_SUCCESS)
         {
            nextObjVlanRtngIntfsLoopbacksTunnels = 0;
            owa.l7rc =usmDbNextIntIfNumberByTypeGet(L7_TUNNEL_INTF, nextObjVlanRtngIntfsLoopbacksTunnels, &nextObjVlanRtngIntfsLoopbacksTunnels);
         }
      }
    }/* end if Tunnel intf or Loopback intf get */
    else if (usmDbRlimLoopbackIdGet(nextObjVlanRtngIntfsLoopbacksTunnels, &intfID) == L7_SUCCESS)
    {
       owa.l7rc =usmDbNextIntIfNumberByTypeGet(L7_LOOPBACK_INTF, nextObjVlanRtngIntfsLoopbacksTunnels, &nextObjVlanRtngIntfsLoopbacksTunnels);
         if (owa.l7rc != L7_SUCCESS)
         {
            nextObjVlanRtngIntfsLoopbacksTunnels = 0;
            owa.l7rc =usmDbNextIntIfNumberByTypeGet(L7_TUNNEL_INTF, nextObjVlanRtngIntfsLoopbacksTunnels, &nextObjVlanRtngIntfsLoopbacksTunnels);
         }

    }
    else if (usmDbRlimTunnelIdGet(objVlanRtngIntfsLoopbacksTunnels, &intfID) == L7_SUCCESS)
    {
      owa.l7rc =usmDbNextIntIfNumberByTypeGet(L7_TUNNEL_INTF, nextObjVlanRtngIntfsLoopbacksTunnels, &nextObjVlanRtngIntfsLoopbacksTunnels);

    }
#endif

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
 
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjVlanRtngIntfsLoopbacksTunnels, owa.len);

  /* return the object value: VlanRtngIntfsTunnels */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjVlanRtngIntfsLoopbacksTunnels,
      sizeof (nextObjVlanRtngIntfsLoopbacksTunnels));
  FPOBJ_TRACE_EXIT (bufp, owa);
#endif
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseinterfaceListing_VlanRtngIntfsLoopbacksTunnels
*
* @purpose Get 'VlanRtngIntfsLoopbacksTunnels'
 *@description  [VlanRtngIntfsLoopbacksTunnels] VLAN Routing Interfaces, Loopbacks and Tunnels List
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseinterfaceListing_PopupTunnels (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  owa.rc = XLIBRC_SUCCESS;
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseinterfaceListing_VlanRtngIntfsLoopbacksTunnels
*
* @purpose Get 'VlanRtngIntfsLoopbacksTunnels'
 *@description  [VlanRtngIntfsLoopbacksTunnels] VLAN Routing Interfaces, Loopbacks and Tunnels List
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseinterfaceListing_AllRtrIntfVlansLoopbacksTunnels (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
#ifdef L7_ROUTING_PACKAGE
  xLibU32_t objAllRtrIntfVlansLoopbacksTunnels;
  xLibU32_t nextObjAllRtrIntfVlansLoopbacksTunnels;
  xLibU32_t objVlanIndexValue=0;
  xLibU32_t nextVlanIndexValue=0;
  xLibU32_t intfID;
  xLibU32_t popupTunnels = L7_TRUE;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: popupTunnels*/
  owa.len = sizeof (popupTunnels);
  owa.rc = xLibFilterGet (wap, XOBJ_baseinterfaceListing_PopupTunnels,
                          (xLibU8_t *) & popupTunnels, &owa.len);

  /* retrieve key: AllRtrIntfVlansLoopbacksTunnels */
  owa.len = sizeof (objAllRtrIntfVlansLoopbacksTunnels);
  owa.rc = xLibFilterGet (wap, XOBJ_baseinterfaceListing_AllRtrIntfVlansLoopbacksTunnels,
                          (xLibU8_t *) & objAllRtrIntfVlansLoopbacksTunnels, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjAllRtrIntfVlansLoopbacksTunnels = 0;
    /* Get First Valid Vlan Routing Interface.If none are there then
     ** get First Valid Loopback if none are there then get first valid Tunnel Interface
     */
  }
  
  else
  {
    nextObjAllRtrIntfVlansLoopbacksTunnels = objAllRtrIntfVlansLoopbacksTunnels;
  }
   FPOBJ_TRACE_CURRENT_KEY (bufp, &nextObjAllRtrIntfVlansLoopbacksTunnels, owa.len);
   intfID = 0;
#ifdef L7_RLIM_PACKAGE
    /* If the filter value is Tunnel interface then should directly get the next Tunnel Interface.
       Otherwise get the next VLAN Routing Interface.
    */
    if (
#ifdef L7_IPV6_PACKAGE
(usmDbRlimTunnelIdGet(nextObjAllRtrIntfVlansLoopbacksTunnels, &intfID) != L7_SUCCESS) &&
#endif
        (usmDbRlimLoopbackIdGet(nextObjAllRtrIntfVlansLoopbacksTunnels,&intfID) != L7_SUCCESS))
    {
#endif
           if (nextObjAllRtrIntfVlansLoopbacksTunnels > 8000)
       {
          owa.l7rc = L7_FAILURE;
          objVlanIndexValue = nextObjAllRtrIntfVlansLoopbacksTunnels-8000;
          if (usmDbNextVlanGet(L7_UNIT_CURRENT, objVlanIndexValue, &nextVlanIndexValue)== L7_SUCCESS)
          {
            owa.l7rc = L7_SUCCESS;
            if (usmDbIpVlanRtrVlanIdToIntIfNum(L7_UNIT_CURRENT, nextVlanIndexValue, &nextObjAllRtrIntfVlansLoopbacksTunnels) != L7_SUCCESS)
            {
              nextObjAllRtrIntfVlansLoopbacksTunnels = 8000+nextVlanIndexValue;
            }
          }
       }
       else
       {
          owa.l7rc = L7_FAILURE;
          if (owa.rc != XLIBRC_SUCCESS) /* This is the first time */
          {
            if (usmDbNextVlanGet(L7_UNIT_CURRENT, intfID, &nextVlanIndexValue)== L7_SUCCESS)
            {
               owa.l7rc = L7_SUCCESS;
               if (usmDbIpVlanRtrVlanIdToIntIfNum(L7_UNIT_CURRENT, nextVlanIndexValue, &nextObjAllRtrIntfVlansLoopbacksTunnels) != L7_SUCCESS)
               {
                  nextObjAllRtrIntfVlansLoopbacksTunnels = 8000+nextVlanIndexValue;
               }
             }
           }
           else if (usmDbIpVlanRtrIntIfNumToVlanId(L7_UNIT_CURRENT, nextObjAllRtrIntfVlansLoopbacksTunnels, &intfID) == L7_SUCCESS)
          {
            if (usmDbNextVlanGet(L7_UNIT_CURRENT, intfID, &nextVlanIndexValue)== L7_SUCCESS)
            {
               owa.l7rc = L7_SUCCESS;
               if (usmDbIpVlanRtrVlanIdToIntIfNum(L7_UNIT_CURRENT, nextVlanIndexValue, &nextObjAllRtrIntfVlansLoopbacksTunnels) != L7_SUCCESS)
               {
                 nextObjAllRtrIntfVlansLoopbacksTunnels = 8000+nextVlanIndexValue;
               }

            }
          }
       }
       #ifdef L7_RLIM_PACKAGE
      if(owa.l7rc != L7_SUCCESS)
      {
         nextObjAllRtrIntfVlansLoopbacksTunnels = 0;
         owa.l7rc =usmDbNextIntIfNumberByTypeGet(L7_LOOPBACK_INTF, nextObjAllRtrIntfVlansLoopbacksTunnels, &nextObjAllRtrIntfVlansLoopbacksTunnels);
#ifdef L7_IPV6_PACKAGE
         if ((owa.l7rc != L7_SUCCESS) && (popupTunnels == L7_TRUE))
         {
            nextObjAllRtrIntfVlansLoopbacksTunnels = 0;
            owa.l7rc =usmDbNextIntIfNumberByTypeGet(L7_TUNNEL_INTF, nextObjAllRtrIntfVlansLoopbacksTunnels, &nextObjAllRtrIntfVlansLoopbacksTunnels);
         }
#endif
      }
    }/* end if Tunnel intf or Loopback intf get */
    else if (usmDbRlimLoopbackIdGet(nextObjAllRtrIntfVlansLoopbacksTunnels, &intfID) == L7_SUCCESS)
    {
       owa.l7rc =usmDbNextIntIfNumberByTypeGet(L7_LOOPBACK_INTF, nextObjAllRtrIntfVlansLoopbacksTunnels, &nextObjAllRtrIntfVlansLoopbacksTunnels);
#ifdef L7_IPV6_PACKAGE
         if ((owa.l7rc != L7_SUCCESS) && (popupTunnels == L7_TRUE))
         {
            nextObjAllRtrIntfVlansLoopbacksTunnels = 0;
            owa.l7rc =usmDbNextIntIfNumberByTypeGet(L7_TUNNEL_INTF, nextObjAllRtrIntfVlansLoopbacksTunnels, &nextObjAllRtrIntfVlansLoopbacksTunnels);
         }
#endif

    }
#ifdef L7_IPV6_PACKAGE
    else if ((popupTunnels == L7_TRUE) &&(usmDbRlimTunnelIdGet(objAllRtrIntfVlansLoopbacksTunnels, &intfID) == L7_SUCCESS))
    {
            owa.l7rc =usmDbNextIntIfNumberByTypeGet(L7_TUNNEL_INTF, nextObjAllRtrIntfVlansLoopbacksTunnels, &nextObjAllRtrIntfVlansLoopbacksTunnels);

    }
#endif
#endif

     if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjAllRtrIntfVlansLoopbacksTunnels, owa.len);
  /* return the object value: AllRtrIntfVlansLoopbacksTunnels */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjAllRtrIntfVlansLoopbacksTunnels,
      sizeof (nextObjAllRtrIntfVlansLoopbacksTunnels));
  FPOBJ_TRACE_EXIT (bufp, owa);
#endif
  return owa.rc;

}
/*******************************************************************************
n fpObjGet_baseinterfaceListing_IsRoutingIntf
*
* @purpose Get 'IsRoutingIntfIsRoutingIntf'
 *@description  [IsRoutingIntfIsRoutingIntf]
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseinterfaceListing_IsRoutingIntf (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objIsRoutingIntf;

  xLibU32_t keyAllRtrIntfVlansLoopbacksTunnels;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AllRtrIntfVlansLoopbacksTunnels*/
  owa.len = sizeof (keyAllRtrIntfVlansLoopbacksTunnels);
  owa.rc = xLibFilterGet (wap, XOBJ_baseinterfaceListing_AllRtrIntfVlansLoopbacksTunnels,
                          (xLibU8_t *) & keyAllRtrIntfVlansLoopbacksTunnels, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  if (keyAllRtrIntfVlansLoopbacksTunnels > 8000)
  {
    objIsRoutingIntf=L7_FALSE;
  }
  else
  {
    objIsRoutingIntf=L7_TRUE;
  }

  /* return the object value: IsRoutingIntf*/
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIsRoutingIntf,
      sizeof (objIsRoutingIntf));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_baseinterfaceListing_RoutingIntfValue
*
* @purpose Get 'RoutingIntfValue"
*@description  [RoutingIntfValue]
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseinterfaceListing_RoutingIntfValue (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objRoutingIntfValue;

  xLibU32_t keyAllRtrIntfVlansLoopbacksTunnels;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AllRtrIntfVlansLoopbacksTunnels*/
  owa.len = sizeof (keyAllRtrIntfVlansLoopbacksTunnels);
  owa.rc = xLibFilterGet (wap, XOBJ_baseinterfaceListing_AllRtrIntfVlansLoopbacksTunnels,
                          (xLibU8_t *) & keyAllRtrIntfVlansLoopbacksTunnels, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (keyAllRtrIntfVlansLoopbacksTunnels > 8000)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else
  {
    objRoutingIntfValue=keyAllRtrIntfVlansLoopbacksTunnels;
  }

  /* return the object value: IsRoutingIntfIsRoutingIntf*/
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRoutingIntfValue,
      sizeof (objRoutingIntfValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjGet_baseinterfaceListing_VlanIdValue
*
* @purpose Get "VlanIdValue"
*@description  [VlanIdValue]
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseinterfaceListing_VlanIdValue (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
#ifdef L7_ROUTING_PACKAGE
  xLibU32_t objVlanIdValue;

  xLibU32_t keyAllRtrIntfVlansLoopbacksTunnels;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AllRtrIntfVlansLoopbacksTunnels*/
  owa.len = sizeof (keyAllRtrIntfVlansLoopbacksTunnels);
  owa.rc = xLibFilterGet (wap, XOBJ_baseinterfaceListing_AllRtrIntfVlansLoopbacksTunnels,
                          (xLibU8_t *) & keyAllRtrIntfVlansLoopbacksTunnels, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  if (keyAllRtrIntfVlansLoopbacksTunnels <= 8000)
  {
     if (usmDbIpVlanRtrIntIfNumToVlanId(L7_UNIT_CURRENT, keyAllRtrIntfVlansLoopbacksTunnels, &objVlanIdValue) != L7_SUCCESS)
     {
       owa.rc = XLIBRC_FAILURE;
       FPOBJ_TRACE_EXIT (bufp, owa);
       return owa.rc;
     }
  }
  else
  {
    objVlanIdValue=keyAllRtrIntfVlansLoopbacksTunnels-8000;
  }

  /* return the object value: VlanIdValue*/
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objVlanIdValue,
      sizeof (objVlanIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
#endif
  return owa.rc;
}
/*******************************************************************************
* @function fpObjGet_baseinterfaceListing_LoopbackID
*
* @purpose Get "VlanIdValue"
*@description  [VlanIdValue]
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseinterfaceListing_LoopbackID (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objIdValue = 0;

  xLibU32_t keyAllRtrIntfVlansLoopbacksTunnels;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AllRtrIntfVlansLoopbacksTunnels*/
  owa.len = sizeof (keyAllRtrIntfVlansLoopbacksTunnels);
  owa.rc = xLibFilterGet (wap, XOBJ_baseinterfaceListing_AllRtrIntfVlansLoopbacksTunnels,
                          (xLibU8_t *) & keyAllRtrIntfVlansLoopbacksTunnels, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  owa.len = sizeof (objIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseinterfaceListing_LoopbackID,
                          (xLibU8_t *) & objIdValue, &owa.len);
  if (owa.rc == XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.l7rc = L7_FAILURE;
  if (keyAllRtrIntfVlansLoopbacksTunnels <= 8000)
  {
#ifdef L7_ROUTING_PACKAGE
#ifdef L7_RLIM_PACKAGE
     owa.l7rc = usmDbRlimLoopbackIdGet(keyAllRtrIntfVlansLoopbacksTunnels,&objIdValue);
#endif
#endif
  }
  if (owa.l7rc != L7_SUCCESS)
  {
       owa.rc = XLIBRC_FAILURE;
       FPOBJ_TRACE_EXIT (bufp, owa);
       return owa.rc;
  }

  /* return the object value: VlanIdValue*/
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIdValue,
      sizeof (objIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseinterfaceListing_LoopbackID
*
* @purpose Get "VlanIdValue"
*@description  [VlanIdValue]
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseinterfaceListing_TunnelID (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objIdValue = 0;

  xLibU32_t keyAllRtrIntfVlansLoopbacksTunnels;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AllRtrIntfVlansLoopbacksTunnels*/
  owa.len = sizeof (keyAllRtrIntfVlansLoopbacksTunnels);
  owa.rc = xLibFilterGet (wap, XOBJ_baseinterfaceListing_AllRtrIntfVlansLoopbacksTunnels,
                          (xLibU8_t *) & keyAllRtrIntfVlansLoopbacksTunnels, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  owa.len = sizeof (objIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseinterfaceListing_TunnelID,
                          (xLibU8_t *) & objIdValue, &owa.len);
  if (owa.rc == XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  owa.l7rc = L7_FAILURE;
  if (keyAllRtrIntfVlansLoopbacksTunnels <= 8000)
  {
#ifdef L7_IPV6_PACKAGE
#ifdef L7_RLIM_PACKAGE
     owa.l7rc = usmDbRlimTunnelIdGet(keyAllRtrIntfVlansLoopbacksTunnels,&objIdValue);
#endif
#endif
  }
  if (owa.l7rc != L7_SUCCESS)
  {
       owa.rc = XLIBRC_ENDOF_TABLE;
       FPOBJ_TRACE_EXIT (bufp, owa);
       return owa.rc;
  }

  /* return the object value: TunnelIdValue*/
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIdValue,
      sizeof (objIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
       return owa.rc;
}

