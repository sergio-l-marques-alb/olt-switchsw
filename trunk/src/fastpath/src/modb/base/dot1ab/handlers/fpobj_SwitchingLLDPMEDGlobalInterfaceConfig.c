
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_SwitchingLLDPMEDGlobalInterfaceConfig.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to Switching-object.xml
*
* @create  19 June 2008, Thursday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "usmdb_util_api.h"
#include "usmdb_lldp_api.h"

/*******************************************************************************
* @function fpObjSet_SwitchingLLDPMEDGlobalInterfaceConfig_LldpMedAdminMode
*
* @purpose Set 'LldpMedAdminMode'
 *@description  [LldpMedAdminMode] The MED admin mode for this interface   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingLLDPMEDGlobalInterfaceConfig_LldpMedAdminMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLldpMedAdminModeValue;
  xLibU32_t objLldpMedInterfaceValue;
  xLibU32_t nextObjLldpMedInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: LldpMedAdminMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objLldpMedAdminModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objLldpMedAdminModeValue, owa.len);
  owa.l7rc = usmDbValidIntIfNumFirstGet (&nextObjLldpMedInterfaceValue);

  while(owa.l7rc == L7_SUCCESS)
  {
    if (usmDbLldpXMedPortAdminModeSet (nextObjLldpMedInterfaceValue,
                                   objLldpMedAdminModeValue) != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

     objLldpMedInterfaceValue = nextObjLldpMedInterfaceValue;
     owa.l7rc = usmDbGetNextVisibleExtIfNumber ( objLldpMedInterfaceValue,
                                &nextObjLldpMedInterfaceValue);
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_SwitchingLLDPMEDGlobalInterfaceConfig_ConfigNotifEnabled
*
* @purpose Set 'ConfigNotifEnabled'
 *@description  [ConfigNotifEnabled] A value of 'true(1)' enables sending the
* topology change traps on this port. A value of 'false(2)' disables
* sending the topology change traps on this port.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingLLDPMEDGlobalInterfaceConfig_ConfigNotifEnabled (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objConfigNotifEnabledValue;
  xLibU32_t objLldpMedInterfaceValue;
  xLibU32_t nextObjLldpMedInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ConfigNotifEnabled */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objConfigNotifEnabledValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objConfigNotifEnabledValue, owa.len);

  owa.l7rc = usmDbValidIntIfNumFirstGet (&nextObjLldpMedInterfaceValue);
  while(owa.l7rc == L7_SUCCESS)
  {


     usmDbLldpXMedPortConfigNotifEnableSet (nextObjLldpMedInterfaceValue,
                                           objConfigNotifEnabledValue);


     objLldpMedInterfaceValue = nextObjLldpMedInterfaceValue;
     owa.l7rc = usmDbGetNextVisibleExtIfNumber ( objLldpMedInterfaceValue,
                                &nextObjLldpMedInterfaceValue);
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
