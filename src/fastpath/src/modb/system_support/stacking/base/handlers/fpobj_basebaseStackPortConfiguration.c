/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_basebaseStackPortConfiguration.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to stacking-object.xml
*
* @create  16 January 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_baseStackPortConfiguration_obj.h"
#include "usmdb_spm_api.h"
#include "usmdb_util_api.h"
#include "usmdb_nim_api.h"
#include "usmdb_sim_api.h"
#include <ctype.h>
#include <stdlib.h>
/* For stack manager unit util routine */
#include "usmdb_unitmgr_api.h"
#include "fpobj_stackingRunifUtils.h"

/*******************************************************************************
* @function fpObjGet_baseStackPortConfiguration_StackPort 
*
* @purpose Get 'ifIndex'
 *@description  [ifIndex] interfaceIndex.    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStackPortConfiguration_StackPort (void *wap, void *bufp)
{
 
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStackPortValue;
  xLibU32_t nextStackPortValue;
  xLibU32_t u,s,p, portIndex;
  xLibU32_t unit =usmDbThisUnitGet();
  FPOBJ_TRACE_ENTER (bufp);
  
  owa.l7rc = L7_FAILURE;
  
  /* retrieve key: StackPort */
  owa.rc = xLibFilterGet (wap, XOBJ_baseStackPortConfiguration_StackPort,
                          (xLibU8_t *) & objStackPortValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
     
    owa.l7rc = usmDbIntIfNumTypeFirstGet(unit, USM_PHYSICAL_INTF | USM_LAG_INTF, 0, &nextStackPortValue);
    if ((owa.l7rc == L7_SUCCESS) && (usmDbUnitSlotPortGet(nextStackPortValue, &u, &s, &p) == L7_SUCCESS))
	{
     owa.l7rc = usmdbSpmFpsIndexFromUspGet (u, s, p, &portIndex);
     objStackPortValue = nextStackPortValue;  
	}
	else
	{
      owa.rc = XLIBRC_ENDOF_TABLE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
	}
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objStackPortValue, owa.len);
  }
  
  while( owa.l7rc != L7_SUCCESS)
  {
    owa.l7rc = usmDbIntIfNumTypeNextGet(unit, USM_PHYSICAL_INTF | USM_LAG_INTF, 0, objStackPortValue, &nextStackPortValue);
    if ((owa.l7rc == L7_SUCCESS) && (usmDbUnitSlotPortGet(nextStackPortValue, &u, &s, &p) == L7_SUCCESS))
	{
     owa.l7rc = usmdbSpmFpsIndexFromUspGet (u, s, p, &portIndex);
	}
	else
	{
	  break;
	}
    objStackPortValue = nextStackPortValue;  
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
 
  FPOBJ_TRACE_NEW_KEY (bufp, &nextStackPortValue, owa.len);
 
  /* return the object value: StackPort */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextStackPortValue, sizeof (nextStackPortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
 
}

/*******************************************************************************
* @function fpObjGet_baseSPM_ConfiguredStackMode
*
* @purpose Get 'ConfiguredStackMode'
*
* @description config mode of the stack port 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStackPortConfiguration_ConfiguredStackPortMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterface;
  xLibU32_t unit,slot,port, portIndex;
  xLibU32_t objConfiguredStackModeValue;
  SPM_STACK_PORT_ENTRY_t temp_entry;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StackPort */
  owa.rc = xLibFilterGet (wap, XOBJ_baseStackPortConfiguration_StackPort,
                          (xLibU8_t *) & keyInterface, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterface, owa.len);
  

  /* get the value from application */
  if((usmDbUnitSlotPortGet(keyInterface,&unit, &slot, &port) == L7_SUCCESS) && (usmdbSpmFpsIndexFromUspGet (unit, slot, port, &portIndex) == L7_SUCCESS))
  {
    owa.l7rc = usmdbSpmStackingCapablePortGet (portIndex, &temp_entry);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    if (temp_entry.fps_port == L7_TRUE)  /* Front Panel */
    {
      switch (temp_entry.port_info.config_mode)
      {
        case L7_TRUE:
             objConfiguredStackModeValue = L7_XUI_STACK;
             break;
        case L7_FALSE:
             objConfiguredStackModeValue = L7_XUI_ETHERNET;
             break;
        default:
             objConfiguredStackModeValue = L7_XUI_STACK;
             break;
      }/*End-of-Switch*/
    }
    else
    {
      objConfiguredStackModeValue = L7_XUI_NOT_APPLICABLE;
    }
  }
  else
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  

  /* return the object value: ConfiguredStackMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objConfiguredStackModeValue,
                           sizeof (objConfiguredStackModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSPM_ConfiguredStackMode
*
* @purpose Set 'ConfiguredStackMode'
*
* @description config mode of the stack port 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseStackPortConfiguration_ConfiguredStackPortMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objConfiguredStackModeValue;
  xLibU32_t keyInterface;
  xLibU32_t unit,slot,port, portIndex;
  xLibU32_t temp_val = 0;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ConfiguredStackMode */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objConfiguredStackModeValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objConfiguredStackModeValue, owa.len);

  /* retrieve key: keyInterface */
  owa.rc = xLibFilterGet (wap, XOBJ_baseStackPortConfiguration_StackPort,
                          (xLibU8_t *) & keyInterface, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterface, owa.len);

  if(usmDbUnitSlotPortGet(keyInterface, &unit, &slot, &port) == L7_SUCCESS)
  {
     owa.l7rc = usmdbSpmFpsIndexFromUspGet (unit, slot, port, &portIndex);
  }
  else
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
    switch (objConfiguredStackModeValue)
  {
      case L7_XUI_ETHERNET:
        temp_val = L7_FALSE;
        break;
  
      case L7_XUI_STACK:
        temp_val = L7_TRUE;
        break;
  
      default:
        /* unknown case */
        owa.l7rc = L7_FAILURE;
        break;
   }
   
  if (owa.l7rc != L7_SUCCESS)
  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* set the value in application */
  owa.l7rc =
    usmdbSpmFpsConfigStackingModeSet (portIndex,
                                      temp_val);
   
  if (owa.l7rc != L7_SUCCESS)
  { 
    
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

 

