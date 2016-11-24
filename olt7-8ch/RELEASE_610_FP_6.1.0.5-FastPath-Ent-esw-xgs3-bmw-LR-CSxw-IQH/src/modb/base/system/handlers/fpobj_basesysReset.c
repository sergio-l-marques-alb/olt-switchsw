/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_basesysReset.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to sntp-object.xml
*
* @create  14 January 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_basesysReset_obj.h"
#include "usmdb_sim_api.h"
#include "usmdb_common.h"
#include "usmdb_unitmgr_api.h"
#include "usmdb_util_api.h"

#include "usmdb_asf_api.h"
/*******************************************************************************
* @function fpObjSet_basesysReset_sysDeviceReset
*
* @purpose Set 'sysDeviceReset'
*
* @description Reboot the switch 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basesysReset_sysDeviceReset (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysDeviceResetValue=0;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysDeviceReset */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objsysDeviceResetValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysDeviceResetValue, owa.len);
 
  if (usmDbComponentPresentCheck(L7_UNIT_CURRENT, L7_FLEX_STACKING_COMPONENT_ID))
  {
  /* Reset All units in the stack */
  owa.l7rc = usmDbUnitMgrResetSystem ();
  }
  else
  {
    /* When there is no stacking there is only one unit. Reset it. */
    owa.l7rc = usmDbSwDevCtrlResetSet (L7_UNIT_CURRENT, objsysDeviceResetValue);
  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_basesysReset_sysResetUnitIndex
*
* @purpose Get 'sysResetUnitIndex'
 *@description  [sysResetUnitIndex] Stacked Unit numbers   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesysReset_sysResetUnitIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objsysResetUnitIndexValue;
  xLibU32_t nextObjsysResetUnitIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: sysResetUnitIndex */
  owa.len = sizeof (objsysResetUnitIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basesysReset_sysResetUnitIndex,
                          (xLibU8_t *) & objsysResetUnitIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbUnitMgrStackMemberGetFirst (&nextObjsysResetUnitIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objsysResetUnitIndexValue, owa.len);
    owa.l7rc = usmDbUnitMgrStackMemberGetNext (objsysResetUnitIndexValue,
                                    &nextObjsysResetUnitIndexValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjsysResetUnitIndexValue, owa.len);

  /* return the object value: sysResetUnitIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjsysResetUnitIndexValue,
                           sizeof (nextObjsysResetUnitIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_basesysReset_sysDeviceResetUnit
*
* @purpose Set 'sysDeviceResetUnit'
 *@description  [sysDeviceResetUnit] Reboot the specified unit   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basesysReset_sysDeviceResetUnit (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objsysDeviceResetUnitValue;

  xLibU32_t keysysResetUnitIndexValue;

  FPOBJ_TRACE_ENTER (bufp);
  /* retrieve object: sysDeviceResetUnit */
  owa.len = sizeof (objsysDeviceResetUnitValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objsysDeviceResetUnitValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysDeviceResetUnitValue, owa.len);

  /* retrieve key: sysResetUnitIndex */
  owa.len = sizeof (keysysResetUnitIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basesysReset_sysResetUnitIndex,
                          (xLibU8_t *) & keysysResetUnitIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keysysResetUnitIndexValue, owa.len);

  if (usmDbComponentPresentCheck(keysysResetUnitIndexValue, L7_FLEX_STACKING_COMPONENT_ID))
  {
  /* set the value in application */
  owa.l7rc = usmDbUnitMgrResetUnit (keysysResetUnitIndexValue);
  }
  else
  {
    /* When there is no stacking there is only one unit. Reset it. */
    owa.l7rc = usmDbSwDevCtrlResetSet (L7_UNIT_CURRENT, objsysDeviceResetUnitValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_basesysReset_sysDeviceResetConfig
*
* @purpose Set 'sysDeviceResetConfig'
*
* @description Set device configuraation to defaults 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basesysReset_sysDeviceResetConfig (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysDeviceResetConfigValue=0;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysDeviceResetConfig */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objsysDeviceResetConfigValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysDeviceResetConfigValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbResetConfigActionSet (L7_UNIT_CURRENT, objsysDeviceResetConfigValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  else
  {
    owa.rc = XLIBRC_RESTART_CONFIG_TO_DEFAULTS_SUCCESS;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjSet_basesysReset_sysDeviceEraseStartupConfig
*
* @purpose Set 'sysDeviceEraseStartupConfig'
*
* @description Set device configuraation to defaults 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basesysReset_sysDeviceEraseStartupConfig (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysDeviceEraseStartupConfigValue=0;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysDeviceEraseStartupConfig */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objsysDeviceEraseStartupConfigValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysDeviceEraseStartupConfigValue, owa.len);

  /* set the value in application */


  owa.l7rc =
    usmdbEraseStartupConfig ();
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ERASE_STRARTUP_CONFIG_ERROR;    /* TODO: Change if required */
  }
  else
  {
     owa.rc = XLIBRC_ERASE_STRARTUP_CONFIG_SUCCESS; 
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basesysReset_sysTimeSinceLastSwitchStatsReset
*
* @purpose Get 'sysTimeSinceLastSwitchStatsReset'
*
* @description time since the last time the switch status was reset 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesysReset_sysTimeSinceLastSwitchStatsReset (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objsysTimeSinceLastSwitchStatsResetValue={0};
  usmDbTimeSpec_t ts;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbTimeSinceLastSwitchStatsResetGet (L7_UNIT_CURRENT, &ts);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
 
  sprintf(objsysTimeSinceLastSwitchStatsResetValue,"%d day %d hr %d min %d sec",
                   ts.days, ts.hours, ts.minutes, ts.seconds);
 
  FPOBJ_TRACE_VALUE (bufp, objsysTimeSinceLastSwitchStatsResetValue,
                     strlen (objsysTimeSinceLastSwitchStatsResetValue));

  /* return the object value: sysTimeSinceLastSwitchStatsReset */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) objsysTimeSinceLastSwitchStatsResetValue,
                    strlen (objsysTimeSinceLastSwitchStatsResetValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjSet_basesysReset_sysDeviceResetAll
*
* @purpose Set 'sysDeviceResetAll'
 *@description  [sysDeviceResetAll] Reboot all switches in stack
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basesysReset_sysDeviceResetAll (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objsysDeviceResetAllValue;
  L7_RC_t rc;
  L7_uint32 unit, temp, val;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysDeviceResetAll */
  owa.len = sizeof (objsysDeviceResetAllValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objsysDeviceResetAllValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysDeviceResetAllValue, owa.len)

  rc = usmDbUnitMgrMgrNumberGet(&unit);
  temp = 0;

  while (usmDbUnitMgrStackMemberGetNext(temp, &temp) == L7_SUCCESS)
  {
     rc = usmDbUnitMgrUnitStatusGet(temp, &val);
     if (temp != unit && val != L7_USMDB_UNITMGR_UNIT_NOT_PRESENT)
     {
        (void)usmDbUnitMgrResetUnit(temp);
     }
  }
  (void)usmDbUnitMgrResetUnit(unit);

  owa.l7rc = L7_SUCCESS;
  /* set the value in application */
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjGet_basesysReset_ASFMode
*
* @purpose Get 'ASFMode'
* @description  [ASFMode] <HTML>(Enable/Disable) the ASF mode on a unit   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesysReset_ASFMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objASFModeValue;
  xLibU32_t objASFModeASFModeConfigMode;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbAsfModeGet (L7_UNIT_CURRENT, &objASFModeValue,&objASFModeASFModeConfigMode);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objASFModeValue, sizeof (objASFModeValue));

  /* return the object value: ASFModeSet */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objASFModeValue, sizeof (objASFModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_basesysReset_ASFMode
*
* @purpose Set 'ASFMode'
 *@description  [ASFMode] <HTML>(Enable/Disable) the ASF mode on a unit   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basesysReset_ASFMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objASFModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ASFModeSet */
  owa.len = sizeof (objASFModeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objASFModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objASFModeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbAsfModeSet (L7_UNIT_CURRENT, objASFModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjGet_basesysReset_ASFModeConfigMode
*
* @purpose Get 'ASFModeConfigMode'
 *@description  [ASFModeConfigMode] <HTML>(Enable/Disable) the ASF mode on a unit   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesysReset_ASFModeConfigMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objASFModeValue;
  xLibU32_t objASFModeASFModeConfigMode;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbAsfModeGet (L7_UNIT_CURRENT, &objASFModeValue,&objASFModeASFModeConfigMode);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objASFModeASFModeConfigMode, sizeof (objASFModeASFModeConfigMode));

  /* return the object value: ASFModeSet */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objASFModeASFModeConfigMode, sizeof (objASFModeASFModeConfigMode));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


