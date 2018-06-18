/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_baseStackFirmwareSynchronization.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to stacking-object.xml
*
* @create  27 Nov 2009
*
* @author  Vamshi Krishna Madap
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_baseStackFirmwareSynchronization_obj.h"
#include "usmdb_spm_api.h"
#include "usmdb_util_api.h"
#include "usmdb_nim_api.h"
#include <ctype.h>
#include <stdlib.h>
/* For stack manager unit util routine */
#include "usmdb_unitmgr_api.h"
#include "fpobj_stackingRunifUtils.h"

/*******************************************************************************
* @purpose Get the SFS mode
*
* @param    wap  work area pointer
* @param   bufp  pointer to store the result
*
* @description
*
* @return XLIBRC_SUCCESS/XLIBRC_FAILURE
*
* @notes
*
*******************************************************************************/
xLibRC_t fpObjGet_baseStackFirmwareSynchronization_autoUpgrade(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAutoUpgradeMode;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbUnitMgrStackFirmwareSyncModeGet(&objAutoUpgradeMode);
   
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAutoUpgradeMode,sizeof (objAutoUpgradeMode));
      
  /* return the object value: StackFirmwareSync */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAutoUpgradeMode,
                           sizeof(objAutoUpgradeMode));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @purpose Set the SFS mode
*
* @param    wap  work area pointer
* @param   bufp  pointer to store the result
*
* @description
*
* @return XLIBRC_SUCCESS/XLIBRC_FAILURE
*
* @notes
*
*******************************************************************************/
xLibRC_t fpObjSet_baseStackFirmwareSynchronization_autoUpgrade(void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAutoUpgradeMode;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: StackFirmwareSync */
  owa.rc = xLibBufDataGet (bufp,(xLibU8_t *) &objAutoUpgradeMode, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAutoUpgradeMode, owa.len);
  
  owa.l7rc = usmDbUnitMgrStackFirmwareSyncModeSet(objAutoUpgradeMode);
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
* @purpose Get the allow downgrade mode
*
* @param    wap  work area pointer
* @param   bufp  pointer to store the result
*   
* @description
*
* @return XLIBRC_SUCCESS/XLIBRC_FAILURE
*
* @notes
*
*******************************************************************************/
xLibRC_t fpObjGet_baseStackFirmwareSynchronization_allowDowngrade (void *wap, void *bufp)
{
	fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t allow_downgrade;

	FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbUnitMgrStackFirmwareSyncAllowDowngradeModeGet(&allow_downgrade);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &allow_downgrade,
                     sizeof (allow_downgrade));

  /* return the object value: Reboot */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &allow_downgrade,
                           sizeof (allow_downgrade));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @purpose Get the traps mode
*
* @param    wap  work area pointer
* @param   bufp  pointer to store the result
*
* @description
*
* @return XLIBRC_SUCCESS/XLIBRC_FAILURE
*
* @notes
*
*******************************************************************************/
xLibRC_t fpObjGet_baseStackFirmwareSynchronization_autoUpgradeTrap (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t trap_mode;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbUnitMgrStackFirmwareSyncTrapModeGet(&trap_mode);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &trap_mode,
                     sizeof (trap_mode));
    
  /* return the object value: Reboot */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &trap_mode,
                           sizeof (trap_mode));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @purpose Set the allow downgrade mode
*
* @param    wap  work area pointer
* @param   bufp  pointer to store the result
*
* @description
*
* @return XLIBRC_SUCCESS/XLIBRC_FAILURE
*
* @notes
*
*******************************************************************************/
xLibRC_t fpObjSet_baseStackFirmwareSynchronization_allowDowngrade (void *wap, void *bufp)
{
	fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t allow_downgrade;
 
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Reboot */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) &allow_downgrade, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &allow_downgrade, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbUnitMgrStackFirmwareSyncAllowDowngradeModeSet(allow_downgrade);
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
* @purpose Set the SFS trap mode
*
* @param    wap  work area pointer
* @param   bufp  pointer to store the result
*
* @description
*
* @return XLIBRC_SUCCESS/XLIBRC_FAILURE
*
* @notes
*
*******************************************************************************/
xLibRC_t fpObjSet_baseStackFirmwareSynchronization_autoUpgradeTrap (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t trap_mode;
 
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Reboot */
  owa.rc = xLibBufDataGet (bufp,(xLibU8_t *)&trap_mode, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &trap_mode, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbUnitMgrStackFirmwareSyncTrapModeSet(trap_mode);
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
* @purpose Get the stack member unit number
*
* @param    wap  work area pointer
* @param   bufp  pointer to store the result
*
* @description
*
* @return XLIBRC_SUCCESS/XLIBRC_FAILURE
*
* @notes
*
*******************************************************************************/
xLibRC_t fpObjGet_baseStackFirmwareSynchronization_memberUnit (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t memberUnit;
  xLibU32_t nextMemberUnit;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StackMember */
  owa.rc = xLibFilterGet (wap, XOBJ_baseStackFirmwareSynchronization_memberUnit,
                          (xLibU8_t *) &memberUnit, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextMemberUnit = 0;
    memberUnit = 0;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &memberUnit, owa.len);
  owa.l7rc = usmDbUnitMgrStackMemberGetNext (memberUnit,&nextMemberUnit);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextMemberUnit, owa.len);

  /* return the object value: StackMember */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextMemberUnit,
                           sizeof (nextMemberUnit));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @purpose Get the SFS status for the unit
*
* @param    wap  work area pointer
* @param   bufp  pointer to store the result
*
* @description
*
* @return XLIBRC_SUCCESS/XLIBRC_FAILURE
*
* @notes
*
*******************************************************************************/
xLibRC_t fpObjGet_baseStackFirmwareSynchronization_sfsStatus (void *wap, void *bufp)
{
	fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t memberUnit;
  xLibU32_t sfsStatus;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StackMember */
  owa.rc = xLibFilterGet (wap, XOBJ_baseStackFirmwareSynchronization_memberUnit,
                          (xLibU8_t *) & memberUnit, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
     owa.rc = XLIBRC_MISSING_KEY;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &memberUnit, owa.len);
  owa.l7rc = usmDbUnitMgrStackFirmwareSyncMemberStatusGet(memberUnit,&sfsStatus);

  FPOBJ_TRACE_NEW_KEY (bufp, &sfsStatus, owa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    return owa.rc;
  }
 
  /* return the object value: StackMember */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & sfsStatus,
                           sizeof (sfsStatus));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @purpose Get the SFS Last Attempt Status for a given unit
*
* @param    wap  work area pointer
* @param   bufp  pointer to store the result
*
* @description
*
* @return XLIBRC_SUCCESS/XLIBRC_FAILURE
*
* @notes
*
*******************************************************************************/
xLibRC_t fpObjGet_baseStackFirmwareSynchronization_lastAttemptStatus(void *wap, void *bufp)
{
	fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t memberUnit;
  xLibU32_t lastAttemptStatus;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StackMember */
  owa.rc = xLibFilterGet (wap, XOBJ_baseStackFirmwareSynchronization_memberUnit,
                          (xLibU8_t *) & memberUnit, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_MISSING_KEY;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &memberUnit, owa.len);
  owa.l7rc = usmDbUnitMgrStackFirmwareSyncMemberLastAttemptStatusGet(memberUnit,
                                                          &lastAttemptStatus);
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  } 
  FPOBJ_TRACE_NEW_KEY (bufp, &lastAttemptStatus, owa.len);

  /* return the object value: StackMember */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & lastAttemptStatus,
                           sizeof (lastAttemptStatus));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

