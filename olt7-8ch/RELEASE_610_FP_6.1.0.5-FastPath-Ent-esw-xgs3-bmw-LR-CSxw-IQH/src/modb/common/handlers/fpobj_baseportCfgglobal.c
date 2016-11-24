
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_baseportCfgglobal.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to base-object.xml
*
* @create  25 June 2008, Wednesday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_baseportCfgglobal_obj.h"
#include "usmdb_nim_api.h"
#include "usmdb_trapmgr_api.h"
#include "usmdb_util_api.h"
#include "usmdb_dot3ad_api.h"
#include "usmdb_dot1s_api.h"

/*******************************************************************************
* @function fpObjSet_baseportCfgglobal_GlobalSTPMode
*
* @purpose Set 'GlobalSTPMode'
 *@description  [GlobalSTPMode] ToDO: Add Help   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseportCfgglobal_GlobalSTPMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objGlobalSTPModeValue;
  xLibU32_t intifNum;

  FPOBJ_TRACE_ENTER (bufp);
  /* retrieve object: GlobalSTPMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objGlobalSTPModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objGlobalSTPModeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = L7_SUCCESS;
  for (intifNum = 1; intifNum < L7_MAX_INTERFACE_COUNT; intifNum++)
  {
    if (usmDbDot1sIsValidIntf(L7_UNIT_CURRENT, intifNum) == L7_TRUE)
    {
      if (usmDbDot1sPortStateSet(L7_UNIT_CURRENT, intifNum, objGlobalSTPModeValue) != L7_SUCCESS)
      {
        owa.l7rc = L7_FAILURE;
      }
    }
  }
	
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseportCfgglobal_GlobalAdminMode
*
* @purpose Set 'GlobalAdminMode'
 *@description  [GlobalAdminMode] ToDO: Add Help   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseportCfgglobal_GlobalAdminMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objGlobalAdminModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: GlobalAdminMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objGlobalAdminModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objGlobalAdminModeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbIfAdminStateSet (L7_UNIT_CURRENT, L7_ALL_INTERFACES, 
                                                           objGlobalAdminModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseportCfgglobal_GlobalLinkTrap
*
* @purpose Set 'GlobalLinkTrap'
 *@description  [GlobalLinkTrap] ToDO: Add Help   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseportCfgglobal_GlobalLinkTrap (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objGlobalLinkTrapValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: GlobalLinkTrap */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objGlobalLinkTrapValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objGlobalLinkTrapValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbIfLinkUpDownTrapEnableSet (L7_UNIT_CURRENT, L7_ALL_INTERFACES,
                                                      objGlobalLinkTrapValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseportCfgglobal_GlobalFrameSize
*
* @purpose Set 'GlobalFrameSize'
 *@description  [GlobalFrameSize] ToDO: Add Help   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseportCfgglobal_GlobalFrameSize (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objGlobalFrameSizeValue;

  xLibU32_t intfNum, intIfTypeFirst, intIfType;
  xLibU32_t  lag;
	
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: GlobalFrameSize */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objGlobalFrameSizeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objGlobalFrameSizeValue, owa.len);

  if( usmDbSocIs53115Check(L7_UNIT_CURRENT))
  {
     if(  ( objGlobalFrameSizeValue !=  L7_MIN_FRAME_SIZE) && ( objGlobalFrameSizeValue != PLAT_MAX_FRAME_SIZE ))
     {
       owa.rc = XLIBRC_INVALID_VALUE_GENERIC;
       FPOBJ_TRACE_EXIT (bufp, owa);
       return owa.rc;
     }
  }
  else
  {

     if ( objGlobalFrameSizeValue < L7_MIN_FRAME_SIZE || objGlobalFrameSizeValue > L7_MAX_FRAME_SIZE)
     {
       owa.rc = XLIBRC_INVALID_VALUE_GENERIC;
       FPOBJ_TRACE_EXIT (bufp, owa);
       return owa.rc;
     }

  }



  /* if row status object is specified and eual to delete return success */

  /* set the value in application */


  owa.l7rc = usmDbIntIfNumTypeFirstGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF, 0, &intfNum);
  if (owa.l7rc == L7_SUCCESS)
  {
    owa.l7rc = usmDbIfTypeGet(L7_UNIT_CURRENT, intfNum, &intIfTypeFirst);
    intIfType = intIfTypeFirst;
    while (1)
    {
      /* Only set if the interface type is the same as the first */
      if (intIfType == intIfTypeFirst)
      {
        if (usmDbDot3adIntfIsMemberGet(L7_UNIT_CURRENT, intfNum, &lag) == L7_SUCCESS)
        {
          /* Did not set the physical mode for lag members. */
          owa.l7rc = L7_FAILURE;
		break;
        }
        else
        {
          owa.l7rc = usmDbIfConfigMaxFrameSizeSet (intfNum, objGlobalFrameSizeValue);
		if(owa.l7rc != L7_SUCCESS)
		{
			  break;
		}
        }
      }
			
      owa.l7rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF, 0, intfNum, &intfNum);
      if (owa.l7rc == L7_SUCCESS)
      {
        owa.l7rc = usmDbIfTypeGet(L7_UNIT_CURRENT, intfNum, &intIfType);
      }
      else
      { 
        /* End of interface list */
        owa.l7rc = L7_SUCCESS;
        break;
      }
    }
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseportCfgglobal_GlobalPhysicalMode
*
* @purpose Set 'GlobalPhysicalMode'
 *@description  [GlobalPhysicalMode] ToDO: Add Help   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseportCfgglobal_GlobalPhysicalMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objGlobalPhysicalModeValue;

  xLibU32_t intfNum, intIfTypeFirst, intIfType;
  xLibU32_t portCapability, lag;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: GlobalPhysicalMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objGlobalPhysicalModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objGlobalPhysicalModeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */



  owa.l7rc = usmDbIntIfNumTypeFirstGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF, 0, &intfNum);
  if (owa.l7rc == L7_SUCCESS)
  {
        owa.l7rc = usmDbIfTypeGet(L7_UNIT_CURRENT, intfNum, &intIfTypeFirst);
        usmDbIntfPhyCapabilityGet(intfNum, &portCapability);
        intIfType = intIfTypeFirst;
        while (1)
        {
          /* Only set if the interface type is the same as the first */
          if (intIfType == intIfTypeFirst)
          {
            if (usmDbDot3adIntfIsMemberGet(L7_UNIT_CURRENT, intfNum, &lag) == L7_SUCCESS)
            {
              /* Did not set the physical mode for lag members. */
              owa.l7rc = L7_FAILURE;
				break;
            }
            else
            {
              if (objGlobalPhysicalModeValue == L7_PORTCTRL_PORTSPEED_AUTO_NEG)
              {
                owa.l7rc = usmDbIfAutoNegoStatusCapabilitiesSet(intfNum, L7_PORT_NEGO_CAPABILITY_ALL);
                if (owa.l7rc !=L7_SUCCESS)
                {
				    break;
                }
              }
              else
              {
                owa.l7rc = usmDbIfAutoNegoStatusCapabilitiesSet(intfNum, L7_DISABLE); 

                if (objGlobalPhysicalModeValue == L7_PORTCTRL_PORTSPEED_HALF_100TX &&
                    (portCapability & L7_PHY_CAP_PORTSPEED_HALF_100))
                {
                  owa.l7rc= usmDbIfSpeedSet(L7_UNIT_CURRENT, intfNum, objGlobalPhysicalModeValue);
                }
                else if (objGlobalPhysicalModeValue == L7_PORTCTRL_PORTSPEED_FULL_100TX &&
                         (portCapability & L7_PHY_CAP_PORTSPEED_FULL_100))
                {
                  owa.l7rc = usmDbIfSpeedSet(L7_UNIT_CURRENT, intfNum, objGlobalPhysicalModeValue);
                }
                else if (objGlobalPhysicalModeValue == L7_PORTCTRL_PORTSPEED_HALF_10T &&
                         (portCapability & L7_PHY_CAP_PORTSPEED_HALF_10))
                {
                  owa.l7rc = usmDbIfSpeedSet(L7_UNIT_CURRENT, intfNum, objGlobalPhysicalModeValue);
                }
                else if (objGlobalPhysicalModeValue == L7_PORTCTRL_PORTSPEED_FULL_10T &&
                         (portCapability & L7_PHY_CAP_PORTSPEED_FULL_10))
                {
                  owa.l7rc = usmDbIfSpeedSet(L7_UNIT_CURRENT, intfNum, objGlobalPhysicalModeValue);
                }
                else if (objGlobalPhysicalModeValue == L7_PORTCTRL_PORTSPEED_FULL_1000SX &&
                         (portCapability & L7_PHY_CAP_PORTSPEED_FULL_1000))
                {
                  owa.l7rc = usmDbIfSpeedSet(L7_UNIT_CURRENT, intfNum, objGlobalPhysicalModeValue);
                }
                else if (objGlobalPhysicalModeValue == L7_PORTCTRL_PORTSPEED_FULL_2P5FX &&
                         (portCapability & L7_PHY_CAP_PORTSPEED_FULL_2500))
                {
                  owa.l7rc = usmDbIfSpeedSet(L7_UNIT_CURRENT, intfNum, objGlobalPhysicalModeValue);
                }
                else if (objGlobalPhysicalModeValue == L7_PORTCTRL_PORTSPEED_FULL_10GSX &&
                         (portCapability & L7_PHY_CAP_PORTSPEED_FULL_10G))
                {
                  owa.l7rc = usmDbIfSpeedSet(L7_UNIT_CURRENT, intfNum, objGlobalPhysicalModeValue);
                }
                else
                {
                  owa.l7rc = L7_FAILURE;
                  break;
                }
								
              }
            }
          }
					
          owa.l7rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF, 0, intfNum, &intfNum);
          if (owa.l7rc == L7_SUCCESS)
          {
            owa.l7rc = usmDbIfTypeGet(L7_UNIT_CURRENT, intfNum, &intIfType);
            usmDbIntfPhyCapabilityGet(intfNum, &portCapability);
          }
          else
          { /* End of interface list */
            owa.l7rc = L7_SUCCESS;
            break;
          }
        }
  	}

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseportCfgglobal_GlobalLACPMode
*
* @purpose Set 'GlobalLACPMode'
 *@description  [GlobalLACPMode] ToDO: Add Help   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseportCfgglobal_GlobalLACPMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objGlobalLACPModeValue;

  xLibU32_t intfNum;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: GlobalLACPMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objGlobalLACPModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objGlobalLACPModeValue, owa.len);

  /* if row status object is specified and equal to delete return success */

  /* set the value in application */

  owa.l7rc = usmDbIntIfNumTypeFirstGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF, 0, &intfNum);
  if (owa.l7rc == L7_SUCCESS)
  {
    while (intfNum)
    {
      /* Set the LACP mode for the interface */
      if (usmDbDot3adAggPortLacpModeSet(L7_UNIT_CURRENT, intfNum, objGlobalLACPModeValue) != L7_SUCCESS)
      {
        /* Did not set the lacp mode for interface */
        owa.l7rc = L7_FAILURE;
        break; 
      }
      owa.l7rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF, 0, intfNum, &intfNum);
      if (owa.l7rc != L7_SUCCESS)
      {  /* End of interface list */
         owa.l7rc = L7_SUCCESS;
         break;   
      }
    }
  } 

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
