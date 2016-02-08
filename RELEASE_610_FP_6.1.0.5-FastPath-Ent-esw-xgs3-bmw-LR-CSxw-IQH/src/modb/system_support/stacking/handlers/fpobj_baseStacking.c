/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_baseStacking.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to sntp-object.xml
*
* @create  11 January 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_baseStacking_obj.h"
#include "usmdb_unitmgr_api.h"
#include "usmdb_common.h"
#include "usmdb_sim_api.h"
#include "usmdb_dim_api.h"
#include "usmdb_cda_api.h"
#include "usmdb_file_api.h"
#include <ctype.h>
#include "usmdb_trapmgr_api.h"

/*******************************************************************************
* @function fpObjSet_baseStacking_ResetAllUnits
*
* @purpose Set 'ResetAllUnits'
 *@description  [ResetAllUnits] Reset All units   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseStacking_ResetAllUnits (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objResetAllUnitsValue;
  xLibU32_t temp, unit;
  usmDbUnitMgrStatus_t status;
  L7_RC_t rc;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ResetAllUnits */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objResetAllUnitsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objResetAllUnitsValue, owa.len);

  /* set the value in application */
  rc = usmDbUnitMgrMgrNumberGet(&unit);
  temp = 0;
  while (usmDbUnitMgrStackMemberGetNext(temp, &temp) == L7_SUCCESS)
  {
    rc = usmDbUnitMgrUnitStatusGet(temp, &status);
    if (temp != unit && status != L7_USMDB_UNITMGR_UNIT_NOT_PRESENT)
    {
      (void)usmDbUnitMgrResetUnit(temp);
    }
  }

  (void)usmDbUnitMgrResetUnit(unit);

  owa.l7rc = L7_SUCCESS;
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseStacking_SupportedUnitIndex
*
* @purpose Get 'SupportedUnitIndex'
*
* @description Index for Supported Unit Types 
*              
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_SupportedUnitIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSupportedUnitIndexValue;
  xLibU32_t nextObjSupportedUnitIndexValue;
  xLibU32_t tempUnitType;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SupportedUnitIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baseStacking_SupportedUnitIndex,
                          (xLibU8_t *) & objSupportedUnitIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjSupportedUnitIndexValue = 0;
    objSupportedUnitIndexValue = 0;
    tempUnitType =0;
  }
  
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objSupportedUnitIndexValue, owa.len);
  owa.l7rc = usmDbUnitMgrSupportedUnitIdNextGet (&objSupportedUnitIndexValue,
                                                 &tempUnitType);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  nextObjSupportedUnitIndexValue = objSupportedUnitIndexValue;
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjSupportedUnitIndexValue, owa.len);

  /* return the object value: SupportedUnitIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjSupportedUnitIndexValue,
                           sizeof (nextObjSupportedUnitIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseStacking_UnitType
*
* @purpose Get 'UnitType'
*
* @description Unit Type for a given Supported Unit Index 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_UnitType (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySupportedUnitIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnitTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SupportedUnitIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseStacking_SupportedUnitIndex,
                          (xLibU8_t *) & keySupportedUnitIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySupportedUnitIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbUnitMgrSupportedUnitIdGet (keySupportedUnitIndexValue,
                                    &objUnitTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: UnitType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUnitTypeValue,
                           sizeof (objUnitTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseStacking_UnitTypeID
*
* @purpose Get 'UnitTypeID'
*
* @description Unit Type for a given Unit 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_UnitTypeID (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySupportedUnitIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnitTypeIDValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SupportedUnitIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseStacking_SupportedUnitIndex,
                          (xLibU8_t *) & keySupportedUnitIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySupportedUnitIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbUnitMgrUnitTypeIdIndexGet (keySupportedUnitIndexValue,
                                    &objUnitTypeIDValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: UnitTypeID */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUnitTypeIDValue,
                           sizeof (objUnitTypeIDValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseStacking_SupportedModel
*
* @purpose Get 'SupportedModel'
*
* @description Model of the Supported Unit Type 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_SupportedModel (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySupportedUnitIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objSupportedModelValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SupportedUnitIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseStacking_SupportedUnitIndex,
                          (xLibU8_t *) & keySupportedUnitIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySupportedUnitIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbUnitMgrSupportedModelGet (keySupportedUnitIndexValue,
                                            objSupportedModelValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: SupportedModel */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objSupportedModelValue,
                           strlen (objSupportedModelValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseStacking_SupportedModel
*
* @purpose Set 'SupportedModel'
*
* @description Dummy Set function to be able to support a pseudo-key mechanism 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseStacking_SupportedModel (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySupportedUnitIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SupportedUnitIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseStacking_SupportedUnitIndex,
                          (xLibU8_t *) & keySupportedUnitIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySupportedUnitIndexValue, kwa.len);

  FPOBJ_TRACE_EXIT (bufp, kwa);
  return kwa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseStacking_Description
*
* @purpose Get 'Description'
*
* @description Description of the Supported Unit Type 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_Description (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySupportedUnitIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objDescriptionValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SupportedUnitIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseStacking_SupportedUnitIndex,
                          (xLibU8_t *) & keySupportedUnitIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySupportedUnitIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbUnitMgrSupportedDescriptionGet (keySupportedUnitIndexValue,
                                                   objDescriptionValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Description */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objDescriptionValue,
                           strlen (objDescriptionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseStacking_ManagementPreference
*
* @purpose Get 'ManagementPreference'
*
* @description Management preferance of the Supported Unit Type 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_ManagementPreference (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySupportedUnitIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objManagementPreferenceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SupportedUnitIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseStacking_SupportedUnitIndex,
                          (xLibU8_t *) & keySupportedUnitIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySupportedUnitIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbUnitMgrSupportedMgmtPrefGet ( keySupportedUnitIndexValue,
                                      &objManagementPreferenceValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ManagementPreference */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objManagementPreferenceValue,
                           sizeof (objManagementPreferenceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseStacking_TargetCodeVersion
*
* @purpose Get 'TargetCodeVersion'
*
* @description [TargetCodeVersion]: Code Target ID on the Supported Unit Type
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_TargetCodeVersion (void *wap, void *bufp)
{
  fpObjWa_t kwaSupportedUnitIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySupportedUnitIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objTargetCodeVersionValue;
  L7_uint32 temp_val;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SupportedUnitIndex */
  kwaSupportedUnitIndex.rc =
    xLibFilterGet (wap, XOBJ_baseStacking_SupportedUnitIndex,
                   (xLibU8_t *) & keySupportedUnitIndexValue,
                   &kwaSupportedUnitIndex.len);
  if (kwaSupportedUnitIndex.rc != XLIBRC_SUCCESS)
  {
    kwaSupportedUnitIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaSupportedUnitIndex);
    return kwaSupportedUnitIndex.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp,
                           &keySupportedUnitIndexValue,
                           kwaSupportedUnitIndex.len);

  owa.l7rc = usmDbUnitMgrSupportedCodeTargetIdGet(keySupportedUnitIndexValue, &temp_val);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  sprintf(objTargetCodeVersionValue, "0x%x", temp_val);

  /* return the object value: TargetCodeVersion */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objTargetCodeVersionValue,
                           strlen (objTargetCodeVersionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseStacking_StackMember
*
* @purpose Get 'StackMember'
*
* @description UnitID of the Stackmember 
*              
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_StackMember (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStackMemberValue;
  xLibU32_t nextObjStackMemberValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StackMember */
  owa.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                          (xLibU8_t *) & objStackMemberValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjStackMemberValue = 0;
    objStackMemberValue = 0;
  }
  
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objStackMemberValue, owa.len);
  owa.l7rc = usmDbUnitMgrStackMemberGetNext (objStackMemberValue,
                                             &nextObjStackMemberValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjStackMemberValue, owa.len);

  /* return the object value: StackMember */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjStackMemberValue,
                           sizeof (objStackMemberValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseStacking_StackMemberType
*
* @purpose Get 'StackMemberType'
*
* @description Type of the Stackmember 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_StackMemberType (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStackMemberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibStr256_t objStackMemberTypeValue;
  xLibU32_t unitType;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StackMember */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                          (xLibU8_t *) & keyStackMemberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbUnitMgrUnitTypeGet (keyStackMemberValue,
                                      &unitType);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  sprintf(objStackMemberTypeValue, "0x%x", unitType);
  /* return the object value: StackMemberType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStackMemberTypeValue,
                           strlen (objStackMemberTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseStacking_UnitMgrModelIdentifier
*
* @purpose Get 'UnitMgrModelIdentifier'
*
* @description Model Identifier of the unit 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_UnitMgrModelIdentifier (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStackMemberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objUnitMgrModelIdentifierValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StackMember */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                          (xLibU8_t *) & keyStackMemberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbUnitMgrModelIdentifierGet (keyStackMemberValue,
                                    objUnitMgrModelIdentifierValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: UnitMgrModelIdentifier */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objUnitMgrModelIdentifierValue,
                           strlen (objUnitMgrModelIdentifierValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseStacking_UnitMgrModelIdentiferPreCfg
*
* @purpose Get 'UnitMgrModelIdentiferPreCfg'
*
* @description Pre-configured Model Identifier 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_UnitMgrModelIdentiferPreCfg (void *wap,
                                                            void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStackMemberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objUnitMgrModelIdentiferPreCfgValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StackMember */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                          (xLibU8_t *) & keyStackMemberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbUnitMgrModelIdentiferPreCfgGet (keyStackMemberValue,
                                         objUnitMgrModelIdentiferPreCfgValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: UnitMgrModelIdentiferPreCfg */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) objUnitMgrModelIdentiferPreCfgValue,
                    strlen (objUnitMgrModelIdentiferPreCfgValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseStacking_UnitMgrUnitStatus
*
* @purpose Get 'UnitMgrUnitStatus'
*
* @description Unit Manager Status 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_UnitMgrUnitStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStackMemberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnitMgrUnitStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StackMember */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                          (xLibU8_t *) & keyStackMemberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbUnitMgrUnitStatusGet (keyStackMemberValue,
                                        &objUnitMgrUnitStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: UnitMgrUnitStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUnitMgrUnitStatusValue,
                           sizeof (objUnitMgrUnitStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseStacking_UnitMgrIsMgmtUnit
*
* @purpose Get 'UnitMgrIsMgmtUnit'
*
* @description Is this a Management Unit 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_UnitMgrIsMgmtUnit (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStackMemberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnitMgrIsMgmtUnitValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StackMember */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                          (xLibU8_t *) & keyStackMemberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbUnitMgrIsMgmtUnit ( keyStackMemberValue,
                                     &objUnitMgrIsMgmtUnitValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: UnitMgrIsMgmtUnit */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUnitMgrIsMgmtUnitValue,
                           sizeof (objUnitMgrIsMgmtUnitValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseStacking_UnitMgrStandbyStatus
*
* @purpose Get 'UnitMgrStandbyStatus'
*
* @description Get the Standby Status of the Unit 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_UnitMgrStandbyStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStackMemberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnitMgrStandbyStatusValue;
  L7_uint32 mgrUnit = 0, cfgStandby = 0, operStandby = 0;
  L7_BOOL   standbyMode = L7_FALSE;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StackMember */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                          (xLibU8_t *) & keyStackMemberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbUnitMgrStandbyGet(&operStandby, &standbyMode);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  owa.l7rc = usmDbUnitMgrCfgStandbyGet(&cfgStandby);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  owa.l7rc = usmDbUnitMgrMgrNumberGet(&mgrUnit);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  if(keyStackMemberValue != mgrUnit)
  {
    if ((operStandby == cfgStandby) && (cfgStandby == keyStackMemberValue))
    {
      objUnitMgrStandbyStatusValue = L7_XUI_StandbyStatus_CFG_STANDBY;
    }
    else if (operStandby != cfgStandby && cfgStandby == keyStackMemberValue)
    {
      objUnitMgrStandbyStatusValue = L7_XUI_StandbyStatus_CFG_STANDBY;
    }
    else if (operStandby != cfgStandby && operStandby == keyStackMemberValue)
    {
      objUnitMgrStandbyStatusValue = L7_XUI_StandbyStatus_OPER_STANDBY;
    }
    else
    {
      objUnitMgrStandbyStatusValue = L7_XUI_StandbyStatus_NONE;
    }
  }
  else
  {
    objUnitMgrStandbyStatusValue = L7_XUI_StandbyStatus_NONE;
  }

  /* return the object value: UnitMgrStandbyStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUnitMgrStandbyStatusValue,
                           sizeof (objUnitMgrStandbyStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseStacking_UnitMgrDeviceDescription
*
* @purpose Get 'UnitMgrDeviceDescription'
*
* @description Device Description of the Unit 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_UnitMgrDeviceDescription (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStackMemberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objUnitMgrDeviceDescriptionValue;
  FPOBJ_TRACE_ENTER (bufp);
  xLibU32_t unitIndex=0;

  /* retrieve key: StackMember */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                          (xLibU8_t *) & keyStackMemberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, kwa.len);

  /* get the value from application */
   if (usmDbUnitMgrUnitTypeIdIndexGet(keyStackMemberValue,&unitIndex) == L7_SUCCESS)
   {
        memset (objUnitMgrDeviceDescriptionValue, 0, sizeof(objUnitMgrDeviceDescriptionValue));

     owa.l7rc = usmDbUnitMgrSupportedDescriptionGet (unitIndex,objUnitMgrDeviceDescriptionValue);
   }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: UnitMgrDeviceDescription */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objUnitMgrDeviceDescriptionValue,
                           strlen (objUnitMgrDeviceDescriptionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseStacking_UnitMgrUserDescription
*
* @purpose Get 'UnitMgrUserDescription'
*
* @description User Description of the Unit 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_UnitMgrUserDescription (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStackMemberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objUnitMgrUserDescriptionValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StackMember */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                          (xLibU8_t *) & keyStackMemberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbUnitMgrUserDescriptionGet ( keyStackMemberValue,
                                      objUnitMgrUserDescriptionValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: UnitMgrUserDescription */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objUnitMgrUserDescriptionValue,
                           strlen (objUnitMgrUserDescriptionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseStacking_UnitMgrUnitIdKey
*
* @purpose Get 'UnitMgrUnitIdKey'
*
* @description Key of the Unit 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_UnitMgrUnitIdKey (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStackMemberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objUnitMgrUnitIdKeyValue;
  L7_enetMacAddr_t tempKeyIDType;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StackMember */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                          (xLibU8_t *) & keyStackMemberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbUnitMgrUnitIdKeyGet ( keyStackMemberValue,
           &tempKeyIDType);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  memcpy(objUnitMgrUnitIdKeyValue,tempKeyIDType.addr,strlen(tempKeyIDType.addr));

  /* return the object value: UnitMgrUnitIdKey */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objUnitMgrUnitIdKeyValue,
                           strlen (objUnitMgrUnitIdKeyValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseStacking_UnitMgrExpectedCodeVer
*
* @purpose Get 'UnitMgrExpectedCodeVer'
*
* @description Expected Code Version on the Unit 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_UnitMgrExpectedCodeVer (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStackMemberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objUnitMgrExpectedCodeVerValue;
  usmDbCodeVersion_t tempVerType;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StackMember */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                          (xLibU8_t *) & keyStackMemberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbUnitMgrExpectedCodeVerGet (keyStackMemberValue,
                                    &tempVerType);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objUnitMgrExpectedCodeVerValue[0] = 0;
  strncat(objUnitMgrExpectedCodeVerValue,&tempVerType.rel,sizeof(xLibU8_t));
  strncat(objUnitMgrExpectedCodeVerValue,(xLibU8_t *)'-',sizeof(xLibU8_t));

  strncat(objUnitMgrExpectedCodeVerValue,&tempVerType.ver,sizeof(xLibU8_t));
  strncat(objUnitMgrExpectedCodeVerValue,(xLibU8_t *)'-',sizeof(xLibU8_t));

  strncat(objUnitMgrExpectedCodeVerValue,&tempVerType.maint_level,sizeof(xLibU8_t));
  strncat(objUnitMgrExpectedCodeVerValue,(xLibU8_t *)'-',sizeof(xLibU8_t));

  strncat(objUnitMgrExpectedCodeVerValue,&tempVerType.build_num,sizeof(xLibU8_t));

  /* return the object value: UnitMgrExpectedCodeVer */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objUnitMgrExpectedCodeVerValue,
                           strlen (objUnitMgrExpectedCodeVerValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseStacking_UnitMgrUnitUpTime
*
* @purpose Get 'UnitMgrUnitUpTime'
*
* @description Number of Seconds since this is Unit is running 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_UnitMgrUnitUpTime (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStackMemberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnitMgrUnitUpTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StackMember */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                          (xLibU8_t *) & keyStackMemberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbUnitMgrUnitUpTimeGet ( keyStackMemberValue,
                                        &objUnitMgrUnitUpTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: UnitMgrUnitUpTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUnitMgrUnitUpTimeValue,
                           sizeof (objUnitMgrUnitUpTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseStacking_UnitMgrUnitUpTimeFormatted
*
* @purpose Get 'UnitMgrUnitUpTimeFormatted'
*
* @description Formatted Up time 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_UnitMgrUnitUpTimeFormatted (void *wap, void *bufp)
{
  xLibU32_t objUnitMgrUnitUpTimeFormattedValue;
  xLibU32_t keyStackMemberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StackMember */
  owa.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                          (xLibU8_t *) & keyStackMemberValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbUnitMgrUnitUpTimeGet (keyStackMemberValue, &objUnitMgrUnitUpTimeFormattedValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  FPOBJ_TRACE_VALUE (bufp, &objUnitMgrUnitUpTimeFormattedValue, sizeof (objUnitMgrUnitUpTimeFormattedValue));
	

  /* return the object value: UnitMgrUnitUpTimeFormatted */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUnitMgrUnitUpTimeFormattedValue,
                           sizeof (objUnitMgrUnitUpTimeFormattedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseStacking_UnitMgrHwMgmtPref
*
* @purpose Get 'UnitMgrHwMgmtPref'
*
* @description [UnitMgrHwMgmtPref]: Indicates the default preference assigned
*              to the unit. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_UnitMgrHwMgmtPref (void *wap, void *bufp)
{
  fpObjWa_t kwaStackMember = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStackMemberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnitMgrHwMgmtPrefValue;
  usmDbUnitMgrMgmtPref_t temp_val;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StackMember */
  kwaStackMember.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                                     (xLibU8_t *) & keyStackMemberValue,
                                     &kwaStackMember.len);
  if (kwaStackMember.rc != XLIBRC_SUCCESS)
  {
    kwaStackMember.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaStackMember);
    return kwaStackMember.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, kwaStackMember.len);

  /* get the value from application */
  owa.l7rc = usmDbUnitMgrHwMgmtPrefGet (keyStackMemberValue,
                              &temp_val);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  switch (temp_val)
  {
    case L7_USMDB_UNITMGR_MGMTPREF_DISABLED:
      objUnitMgrHwMgmtPrefValue = L7_XUI_MgmtPref_DISABLED;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_1:
    case L7_USMDB_UNITMGR_MGMTPREF_2:
    case L7_USMDB_UNITMGR_MGMTPREF_3:
    case L7_USMDB_UNITMGR_MGMTPREF_4:
    case L7_USMDB_UNITMGR_MGMTPREF_5:
    case L7_USMDB_UNITMGR_MGMTPREF_6:
    case L7_USMDB_UNITMGR_MGMTPREF_7:
    case L7_USMDB_UNITMGR_MGMTPREF_8:
    case L7_USMDB_UNITMGR_MGMTPREF_9:
    case L7_USMDB_UNITMGR_MGMTPREF_10:
    case L7_USMDB_UNITMGR_MGMTPREF_11:
    case L7_USMDB_UNITMGR_MGMTPREF_12:
    case L7_USMDB_UNITMGR_MGMTPREF_13:
    case L7_USMDB_UNITMGR_MGMTPREF_14:
    case L7_USMDB_UNITMGR_MGMTPREF_15:
      objUnitMgrHwMgmtPrefValue = L7_XUI_MgmtPref_ASSIGNED;
      break;
    case L7_USMDB_UNITMGR_MGMTFUNC_UNASSIGNED:
      objUnitMgrHwMgmtPrefValue = L7_XUI_MgmtPref_UNASSIGNED;
      break;
    default:
      owa.l7rc = L7_FAILURE;
      break;
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  /* return the object value: UnitMgrHwMgmtPref */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUnitMgrHwMgmtPrefValue,
                           sizeof (objUnitMgrHwMgmtPrefValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseStacking_UnitMgrHwMgmtPrefValue
*
* @purpose Get 'UnitMgrHwMgmtPrefValue'
*
* @description [UnitMgrHwMgmtPrefValue]: Indicates the default preference
*              value assigned to the unit. The preference value indicates
*              how likely this unit is to be chosen as the management unit.
*               A value of 0 indicates a disabled or unassigned preference.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_UnitMgrHwMgmtPrefValue (void *wap, void *bufp)
{
  fpObjWa_t kwaStackMember = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStackMemberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnitMgrHwMgmtPrefValueValue;
  usmDbUnitMgrMgmtPref_t temp_val;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StackMember */
  kwaStackMember.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                                     (xLibU8_t *) & keyStackMemberValue,
                                     &kwaStackMember.len);
  if (kwaStackMember.rc != XLIBRC_SUCCESS)
  {
    kwaStackMember.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaStackMember);
    return kwaStackMember.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, kwaStackMember.len);

  /* get the value from application */
  owa.l7rc = usmDbUnitMgrHwMgmtPrefGet ( keyStackMemberValue,
                              &temp_val);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  switch (temp_val)
  {
    case L7_USMDB_UNITMGR_MGMTPREF_1:
      objUnitMgrHwMgmtPrefValueValue = 1;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_2:
      objUnitMgrHwMgmtPrefValueValue = 2;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_3:
      objUnitMgrHwMgmtPrefValueValue = 3;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_4:
      objUnitMgrHwMgmtPrefValueValue = 4;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_5:
      objUnitMgrHwMgmtPrefValueValue = 5;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_6:
      objUnitMgrHwMgmtPrefValueValue = 6;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_7:
      objUnitMgrHwMgmtPrefValueValue = 7;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_8:
      objUnitMgrHwMgmtPrefValueValue = 8;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_9:
      objUnitMgrHwMgmtPrefValueValue = 9;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_10:
      objUnitMgrHwMgmtPrefValueValue = 10;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_11:
      objUnitMgrHwMgmtPrefValueValue = 11;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_12:
      objUnitMgrHwMgmtPrefValueValue = 12;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_13:
      objUnitMgrHwMgmtPrefValueValue = 13;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_14:
      objUnitMgrHwMgmtPrefValueValue = 14;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_15:
      objUnitMgrHwMgmtPrefValueValue = 15;
      break;
    default:
      objUnitMgrHwMgmtPrefValueValue = 0;
      break;
  }

  /* return the object value: UnitMgrHwMgmtPrefValue */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUnitMgrHwMgmtPrefValueValue,
                           sizeof (objUnitMgrHwMgmtPrefValueValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseStacking_UnitMgrAdminMgmtPref
*
* @purpose Get 'UnitMgrAdminMgmtPref'
*
* @description [UnitMgrAdminMgmtPref]: Indicates the configured preference
*              assigned to the unit. This object can not be set to assigned(3).
*               Setting this object to disabled(1), or unassigned(2)
*              will set agentInventoryUnitHWMgmtPrefValue to 0. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_UnitMgrAdminMgmtPref (void *wap, void *bufp)
{
  fpObjWa_t kwaStackMember = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStackMemberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnitMgrAdminMgmtPrefValue;
  usmDbUnitMgrMgmtPref_t temp_val;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StackMember */
  kwaStackMember.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                                     (xLibU8_t *) & keyStackMemberValue,
                                     &kwaStackMember.len);
  if (kwaStackMember.rc != XLIBRC_SUCCESS)
  {
    kwaStackMember.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaStackMember);
    return kwaStackMember.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, kwaStackMember.len);

  /* get the value from application */
  owa.l7rc = usmDbUnitMgrAdminMgmtPrefGet ( keyStackMemberValue,
                              &temp_val);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }  

  switch (temp_val)
  {
    case L7_USMDB_UNITMGR_MGMTPREF_DISABLED:
      objUnitMgrAdminMgmtPrefValue = L7_XUI_MgmtPref_DISABLED;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_1:
    case L7_USMDB_UNITMGR_MGMTPREF_2:
    case L7_USMDB_UNITMGR_MGMTPREF_3:
    case L7_USMDB_UNITMGR_MGMTPREF_4:
    case L7_USMDB_UNITMGR_MGMTPREF_5:
    case L7_USMDB_UNITMGR_MGMTPREF_6:
    case L7_USMDB_UNITMGR_MGMTPREF_7:
    case L7_USMDB_UNITMGR_MGMTPREF_8:
    case L7_USMDB_UNITMGR_MGMTPREF_9:
    case L7_USMDB_UNITMGR_MGMTPREF_10:
    case L7_USMDB_UNITMGR_MGMTPREF_11:
    case L7_USMDB_UNITMGR_MGMTPREF_12:
    case L7_USMDB_UNITMGR_MGMTPREF_13:
    case L7_USMDB_UNITMGR_MGMTPREF_14:
    case L7_USMDB_UNITMGR_MGMTPREF_15:
      objUnitMgrAdminMgmtPrefValue = L7_XUI_MgmtPref_ASSIGNED;
      break;
    case L7_USMDB_UNITMGR_MGMTFUNC_UNASSIGNED:
      objUnitMgrAdminMgmtPrefValue = L7_XUI_MgmtPref_UNASSIGNED;
      break;
  }

  /* return the object value: UnitMgrAdminMgmtPref */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUnitMgrAdminMgmtPrefValue,
                           sizeof (objUnitMgrAdminMgmtPrefValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseStacking_UnitMgrAdminMgmtPref
*
* @purpose Set 'UnitMgrAdminMgmtPref'
*
* @description [UnitMgrAdminMgmtPref]: Indicates the configured preference
*              assigned to the unit. This object can not be set to assigned(3).
*               Setting this object to disabled(1), or unassigned(2)
*              will set agentInventoryUnitHWMgmtPrefValue to 0. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseStacking_UnitMgrAdminMgmtPref (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnitMgrAdminMgmtPrefValue;
  fpObjWa_t kwaStackMember = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStackMemberValue;
  usmDbUnitMgrMgmtPref_t temp_val = 0;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: UnitMgrAdminMgmtPref */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objUnitMgrAdminMgmtPrefValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUnitMgrAdminMgmtPrefValue, owa.len);

  /* retrieve key: StackMember */
  kwaStackMember.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                                     (xLibU8_t *) & keyStackMemberValue,
                                     &kwaStackMember.len);
  if (kwaStackMember.rc != XLIBRC_SUCCESS)
  {
    kwaStackMember.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaStackMember);
    return kwaStackMember.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, kwaStackMember.len);

  switch (objUnitMgrAdminMgmtPrefValue)
  {
  case L7_XUI_MgmtPref_DISABLED:
    temp_val = L7_USMDB_UNITMGR_MGMTPREF_DISABLED;
    break;
  case L7_XUI_MgmtPref_UNASSIGNED:
    temp_val = L7_USMDB_UNITMGR_MGMTFUNC_UNASSIGNED;
    break;
  default:
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
  owa.l7rc = usmDbUnitMgrAdminMgmtPrefSet ( keyStackMemberValue,
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


/*******************************************************************************
* @function fpObjGet_baseStacking_UnitMgrAdminMgmtPrefValue
*
* @purpose Get 'UnitMgrAdminMgmtPrefValue'
*
* @description [UnitMgrAdminMgmtPrefValue]: Indicates the configured preference
*              value assigned to the unit. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_UnitMgrAdminMgmtPrefValue (void *wap, void *bufp)
{
  fpObjWa_t kwaStackMember = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStackMemberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnitMgrAdminMgmtPrefValueValue;
  usmDbUnitMgrMgmtPref_t temp_val;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StackMember */
  kwaStackMember.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                                     (xLibU8_t *) & keyStackMemberValue,
                                     &kwaStackMember.len);
  if (kwaStackMember.rc != XLIBRC_SUCCESS)
  {
    kwaStackMember.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaStackMember);
    return kwaStackMember.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, kwaStackMember.len);

  /* get the value from application */
  owa.l7rc = usmDbUnitMgrAdminMgmtPrefGet ( keyStackMemberValue,
                              &temp_val);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  switch (temp_val)
  {
    case L7_USMDB_UNITMGR_MGMTPREF_DISABLED:
      objUnitMgrAdminMgmtPrefValueValue = 0;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_1:
      objUnitMgrAdminMgmtPrefValueValue = 1;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_2:
      objUnitMgrAdminMgmtPrefValueValue = 2;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_3:
      objUnitMgrAdminMgmtPrefValueValue = 3;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_4:
      objUnitMgrAdminMgmtPrefValueValue = 4;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_5:
      objUnitMgrAdminMgmtPrefValueValue = 5;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_6:
      objUnitMgrAdminMgmtPrefValueValue = 6;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_7:
      objUnitMgrAdminMgmtPrefValueValue = 7;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_8:
      objUnitMgrAdminMgmtPrefValueValue = 8;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_9:
      objUnitMgrAdminMgmtPrefValueValue = 9;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_10:
      objUnitMgrAdminMgmtPrefValueValue = 10;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_11:
      objUnitMgrAdminMgmtPrefValueValue = 11;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_12:
      objUnitMgrAdminMgmtPrefValueValue = 12;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_13:
      objUnitMgrAdminMgmtPrefValueValue = 13;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_14:
      objUnitMgrAdminMgmtPrefValueValue = 14;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_15:
      objUnitMgrAdminMgmtPrefValueValue = 15;
      break;
    case L7_USMDB_UNITMGR_MGMTFUNC_UNASSIGNED:
      objUnitMgrAdminMgmtPrefValueValue = 16;
      break;
    default:
      objUnitMgrAdminMgmtPrefValueValue = 0;
      break;
  }

  /* return the object value: UnitMgrAdminMgmtPrefValue */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objUnitMgrAdminMgmtPrefValueValue,
                    sizeof (objUnitMgrAdminMgmtPrefValueValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseStacking_UnitMgrAdminMgmtPrefValue
*
* @purpose Set 'UnitMgrAdminMgmtPrefValue'
*
* @description [UnitMgrAdminMgmtPrefValue]: Indicates the configured preference
*              value assigned to the unit. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseStacking_UnitMgrAdminMgmtPrefValue (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnitMgrAdminMgmtPrefValueValue;
  fpObjWa_t kwaStackMember = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStackMemberValue;
  usmDbUnitMgrMgmtPref_t temp_val = 0;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: UnitMgrAdminMgmtPrefValue */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objUnitMgrAdminMgmtPrefValueValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUnitMgrAdminMgmtPrefValueValue, owa.len);

  /* retrieve key: StackMember */
  kwaStackMember.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                                     (xLibU8_t *) & keyStackMemberValue,
                                     &kwaStackMember.len);
  if (kwaStackMember.rc != XLIBRC_SUCCESS)
  {
    kwaStackMember.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaStackMember);
    return kwaStackMember.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, kwaStackMember.len);

  switch (objUnitMgrAdminMgmtPrefValueValue)
  {
  case 0:
    temp_val = L7_USMDB_UNITMGR_MGMTPREF_DISABLED;
    break;
  case 1:
    temp_val = L7_USMDB_UNITMGR_MGMTPREF_1;
    break;
  case 2:
    temp_val = L7_USMDB_UNITMGR_MGMTPREF_2;
    break;
  case 3:
    temp_val = L7_USMDB_UNITMGR_MGMTPREF_3;
    break;
  case 4:
    temp_val = L7_USMDB_UNITMGR_MGMTPREF_4;
    break;
  case 5:
    temp_val = L7_USMDB_UNITMGR_MGMTPREF_5;
    break;
  case 6:
    temp_val = L7_USMDB_UNITMGR_MGMTPREF_6;
    break;
  case 7:
    temp_val = L7_USMDB_UNITMGR_MGMTPREF_7;
    break;
  case 8:
    temp_val = L7_USMDB_UNITMGR_MGMTPREF_8;
    break;
  case 9:
    temp_val = L7_USMDB_UNITMGR_MGMTPREF_9;
    break;
  case 10:
    temp_val = L7_USMDB_UNITMGR_MGMTPREF_10;
    break;
  case 11:
    temp_val = L7_USMDB_UNITMGR_MGMTPREF_11;
    break;
  case 12:
    temp_val = L7_USMDB_UNITMGR_MGMTPREF_12;
    break;
  case 13:
    temp_val = L7_USMDB_UNITMGR_MGMTPREF_13;
    break;
  case 14:
    temp_val = L7_USMDB_UNITMGR_MGMTPREF_14;
    break;
  case 15:
    temp_val = L7_USMDB_UNITMGR_MGMTPREF_15;
    break;
  case 16:
    temp_val = L7_USMDB_UNITMGR_MGMTFUNC_UNASSIGNED;
    break;
  default:
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
  owa.l7rc = usmDbUnitMgrAdminMgmtPrefSet ( keyStackMemberValue,
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

/*******************************************************************************
* @function fpObjGet_baseStacking_UnitMgrDetectCodeVerRunning
*
* @purpose Get 'UnitMgrDetectCodeVerRunning'
*
* @description Detected Version of the Code on the unit 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_UnitMgrDetectCodeVerRunning (void *wap,
                                                            void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStackMemberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objUnitMgrDetectCodeVerRunningValue;
  usmDbCodeVersion_t ver;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StackMember */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                          (xLibU8_t *) & keyStackMemberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbUnitMgrDetectCodeVerRunningGet ( keyStackMemberValue,
                                         &ver);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  if (isalpha(ver.rel))
  {
    sprintf(objUnitMgrDetectCodeVerRunningValue, "%c.%d.%d.%d", ver.rel, ver.ver, ver.maint_level, ver.build_num);
  }
  else
  {
    sprintf(objUnitMgrDetectCodeVerRunningValue, "%d.%d.%d.%d", ver.rel, ver.ver, ver.maint_level, ver.build_num);
  }
  /* return the object value: UnitMgrDetectCodeVerRunning */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objUnitMgrDetectCodeVerRunningValue,
                    strlen (objUnitMgrDetectCodeVerRunningValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseStacking_UnitMgrSerialNumber
*
* @purpose Get 'UnitMgrSerialNumber'
*
* @description Serial Number of the unit 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_UnitMgrSerialNumber (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStackMemberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objUnitMgrSerialNumberValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StackMember */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                          (xLibU8_t *) & keyStackMemberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbUnitMgrSerialNumberGet ( keyStackMemberValue,
                                          objUnitMgrSerialNumberValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: UnitMgrSerialNumber */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objUnitMgrSerialNumberValue,
                           strlen (objUnitMgrSerialNumberValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseStacking_UnitMgrOsDescription
*
* @purpose Get 'UnitMgrOsDescription'
*
* @description Operating System Description of the Unit 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_UnitMgrOsDescription (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStackMemberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objUnitMgrOsDescriptionValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StackMember */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                          (xLibU8_t *) & keyStackMemberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbUnitMgrOsDescriptionGet (keyStackMemberValue,
                                           objUnitMgrOsDescriptionValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: UnitMgrOsDescription */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objUnitMgrOsDescriptionValue,
                           strlen (objUnitMgrOsDescriptionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseStacking_UnitMgrDetectCodeVerFlash
*
* @purpose Get 'UnitMgrDetectCodeVerFlash'
*
* @description Detected Version of the Code in the Flash 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_UnitMgrDetectCodeVerFlash (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStackMemberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objUnitMgrDetectCodeVerFlashValue;
  usmDbCodeVersion_t ver;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StackMember */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                          (xLibU8_t *) & keyStackMemberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbUnitMgrDetectCodeVerFlashGet (keyStackMemberValue,
                                       &ver);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
#if 0
  objUnitMgrDetectCodeVerFlashValue[0] = 0;
  strncat(objUnitMgrDetectCodeVerFlashValue,&tempVerType.rel,sizeof(xLibU8_t));
  strncat(objUnitMgrDetectCodeVerFlashValue,(xLibU8_t *)'-',sizeof(xLibU8_t));

  strncat(objUnitMgrDetectCodeVerFlashValue,&tempVerType.ver,sizeof(xLibU8_t));
  strncat(objUnitMgrDetectCodeVerFlashValue,(xLibU8_t *)'-',sizeof(xLibU8_t));

  strncat(objUnitMgrDetectCodeVerFlashValue,&tempVerType.maint_level,sizeof(xLibU8_t));
  strncat(objUnitMgrDetectCodeVerFlashValue,(xLibU8_t *)'-',sizeof(xLibU8_t));

  strncat(objUnitMgrDetectCodeVerFlashValue,&tempVerType.build_num,sizeof(xLibU8_t));
#endif
  if (isalpha(ver.rel))
  {
    sprintf(objUnitMgrDetectCodeVerFlashValue, "%c.%d.%d.%d", ver.rel, ver.ver, ver.maint_level, ver.build_num);
  }
  else
  {
    sprintf(objUnitMgrDetectCodeVerFlashValue, "%d.%d.%d.%d", ver.rel, ver.ver, ver.maint_level, ver.build_num);
  }
  /* return the object value: UnitMgrDetectCodeVerFlash */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objUnitMgrDetectCodeVerFlashValue,
                           strlen (objUnitMgrDetectCodeVerFlashValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseStacking_ResetUnit
*
* @purpose Set 'ResetUnit'
*
* @description Rest the unit 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseStacking_ResetUnit (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objResetUnitValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStackMemberValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ResetUnit */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objResetUnitValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objResetUnitValue, owa.len);

  /* retrieve key: StackMember */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                          (xLibU8_t *) & keyStackMemberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbUnitMgrResetUnit (keyStackMemberValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    objResetUnitValue = L7_FALSE;
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  objResetUnitValue = L7_TRUE;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseStacking_ResetUnitCfg
*
* @purpose Set 'ResetUnitCfg'
*
* @description Clear the configuration on the unit to factory defaults 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseStacking_ResetUnitCfg (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objResetUnitCfgValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ResetUnitCfg */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objResetUnitCfgValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objResetUnitCfgValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbUnitMgrResetUnitCfg ( );
  if (owa.l7rc != L7_SUCCESS)
  {
    objResetUnitCfgValue = L7_FALSE;
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objResetUnitCfgValue = L7_TRUE;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseStacking_UnitMgrNumber
*
* @purpose Set 'UnitMgrNumber'
*
* @description Unit number of the management unit 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseStacking_UnitMgrNumber (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnitMgrNumberValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStackMemberValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: UnitMgrNumber */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objUnitMgrNumberValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUnitMgrNumberValue, owa.len);

  /* retrieve key: StackMember */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                          (xLibU8_t *) & keyStackMemberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbUnitMgrUnitNumberChange ( keyStackMemberValue,
                                           objUnitMgrNumberValue);
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
* @function fpObjGet_baseStacking_UnitMgrEventLog
*
* @purpose Get 'UnitMgrEventLog'
*
* @description Number of Event log entries on the management unit 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_UnitMgrEventLog (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStackMemberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnitMgrEventLogValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StackMember */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                          (xLibU8_t *) & keyStackMemberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbUnitMgrEventLogGet( keyStackMemberValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    objUnitMgrEventLogValue = L7_FALSE;
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objUnitMgrEventLogValue = L7_TRUE;

  /* return the object value: UnitMgrEventLog */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUnitMgrEventLogValue,
                           sizeof (objUnitMgrEventLogValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseStacking_Slots
*
* @purpose Get 'Slots'
*
* @description Number of Slots on the management unit 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_Slots (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStackMemberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSlotsValue;
  xLibU32_t tempValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StackMember */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                          (xLibU8_t *) & keyStackMemberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbUnitDbEntrySlotsGet ( keyStackMemberValue,
                                       &objSlotsValue,&tempValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Slots */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSlotsValue,
                           sizeof (objSlotsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseStacking_UnitMgrActiveImageName
*
* @purpose Get 'UnitMgrActiveImageName'
*
* @description Image name of the Current Active Image 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_UnitMgrActiveImageName (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStackMemberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objUnitMgrActiveImageNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StackMember */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                          (xLibU8_t *) & keyStackMemberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbUnitMgrActiveImageNameGet ( keyStackMemberValue,
                                    objUnitMgrActiveImageNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: UnitMgrActiveImageName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objUnitMgrActiveImageNameValue,
                           strlen (objUnitMgrActiveImageNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseStacking_UnitMgrBackupImageName
*
* @purpose Get 'UnitMgrBackupImageName'
*
* @description Image name of the backup image 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_UnitMgrBackupImageName (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStackMemberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objUnitMgrBackupImageNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StackMember */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                          (xLibU8_t *) & keyStackMemberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbUnitMgrBackupImageNameGet ( keyStackMemberValue,
                                    objUnitMgrBackupImageNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: UnitMgrBackupImageName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objUnitMgrBackupImageNameValue,
                           strlen (objUnitMgrBackupImageNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseStacking_UnitMgrActivatedImageName
*
* @purpose Get 'UnitMgrActivatedImageName'
*
* @description Image name of the Activated Image 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_UnitMgrActivatedImageName (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStackMemberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objUnitMgrActivatedImageNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StackMember */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                          (xLibU8_t *) & keyStackMemberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbUnitMgrActivatedImageNameGet ( keyStackMemberValue,
                                       objUnitMgrActivatedImageNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: UnitMgrActivatedImageName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objUnitMgrActivatedImageNameValue,
                           strlen (objUnitMgrActivatedImageNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}



/*******************************************************************************
* @function fpObjSet_baseStacking_UnitMgrReplicateSTK
*
* @purpose Set 'UnitMgrReplicateSTK'
*
* @description [UnitMgrReplicateSTK]: Initiates the specified STK file copy
*              from management unit to this unit. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseStacking_UnitMgrReplicateSTK (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnitMgrReplicateSTKValue;
  fpObjWa_t kwaStackMember = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStackMemberValue;
  xLibStr256_t srcimage;
  xLibStr256_t destimage;

  FPOBJ_TRACE_ENTER (bufp);

  memset(srcimage,0x00,sizeof(srcimage));
  memset(destimage,0x00,sizeof(destimage));

  /* retrieve object: UnitMgrReplicateSTK */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objUnitMgrReplicateSTKValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUnitMgrReplicateSTKValue, owa.len);

  /* retrieve key: StackMember */
  kwaStackMember.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                                     (xLibU8_t *) & keyStackMemberValue,
                                     &kwaStackMember.len);
  if (kwaStackMember.rc != XLIBRC_SUCCESS)
  {
    kwaStackMember.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaStackMember);
    return kwaStackMember.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, kwaStackMember.len);


  switch (objUnitMgrReplicateSTKValue)
  {
  case L7_DISABLE:
    /* no-op */
    break;

  case L7_ENABLE:
    owa.l7rc = usmDbTransferFileNameLocalGet(L7_UNIT_CURRENT, srcimage);
    owa.l7rc = usmDbTransferFileNameRemoteGet(L7_UNIT_CURRENT, destimage);
    owa.l7rc = usmdbCdaCodeUpdate(keyStackMemberValue, srcimage, destimage);
    break;

  default:
    /* unknown value */
    owa.l7rc = L7_FAILURE;
    break;
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
* @function fpObjGet_baseStacking_UnitMgrRowStatus
*
* @purpose Set 'UnitMgrRowStatus'
*
* @description [UnitMgrRowStatus]: Status of this row. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_UnitMgrRowStatus (void *wap, void *bufp)
{
   return XLIBRC_SUCCESS;
}

/*******************************************************************************
* @function fpObjSet_baseStacking_UnitMgrRowStatus
*
* @purpose Set 'UnitMgrRowStatus'
*
* @description [UnitMgrRowStatus]: Status of this row. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseStacking_UnitMgrRowStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnitMgrRowStatusValue;
  fpObjWa_t kwaStackMember = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStackMemberValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySupportedUnitIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: UnitMgrRowStatus */
  owa.rc = xLibBufDataGet (bufp,(xLibU8_t *) & objUnitMgrRowStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUnitMgrRowStatusValue, owa.len);

  /* retrieve key: StackMember */
  kwaStackMember.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                                    (xLibU8_t *) & keyStackMemberValue,
                                    &kwaStackMember.len);
  if (kwaStackMember.rc != XLIBRC_SUCCESS)
  {
    kwaStackMember.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaStackMember);
    return kwaStackMember.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, kwaStackMember.len);

  /* call the usmdb only for add and delete */
  if (objUnitMgrRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    /* retrieve key: SupportedUnitIndex */
    kwa.rc = xLibFilterGet (wap, XOBJ_baseStacking_SupportedUnitIndex,
                           (xLibU8_t *) & keySupportedUnitIndexValue, &kwa.len);
    if (kwa.rc != XLIBRC_SUCCESS)
    {
      kwa.rc = XLIBRC_FILTER_MISSING; /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, kwa);
      return kwa.rc;
    }

    owa.l7rc = usmDbUnitMgrCreateUnit(keyStackMemberValue, keySupportedUnitIndexValue);
    if (owa.l7rc != L7_SUCCESS)
    {  
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
  }
  else if (objUnitMgrRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
    /* Delete the existing row */
    owa.l7rc = usmDbUnitMgrDeleteUnit (keyStackMemberValue);
    if (owa.l7rc != L7_SUCCESS)
    {

      if (owa.l7rc == L7_ERROR)
      {
         owa.rc = XLIBRC_ERROR_ACTIVE_SWITCH_REMOVE;
      }
      else
      {
         owa.rc = XLIBRC_ERROR_SWITCH_REMOVE;  /* TODO: Change if required */
      }
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
  }
  return XLIBRC_SUCCESS;
}

/*******************************************************************************
* @function fpObjGet_baseStacking_UnitMgrImage1Version
*
* @purpose Get 'UnitMgrImage1Version'
*
* @description [UnitMgrImage1Version]: Code version for Image1. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_UnitMgrImage1Version (void *wap, void *bufp)
{
  fpObjWa_t kwaStackMember = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStackMemberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objUnitMgrImage1VersionValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StackMember */
  kwaStackMember.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                                     (xLibU8_t *) & keyStackMemberValue,
                                     &kwaStackMember.len);
  if (kwaStackMember.rc != XLIBRC_SUCCESS)
  {
    kwaStackMember.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaStackMember);
    return kwaStackMember.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, kwaStackMember.len);

  /* get the value from application */
  owa.l7rc = usmDbImageVersionGet ( keyStackMemberValue, USMDB_IMAGE1_NAME,
                              objUnitMgrImage1VersionValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: UnitMgrImage1Version */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objUnitMgrImage1VersionValue,
                           strlen (objUnitMgrImage1VersionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseStacking_UnitMgrImage2Version
*
* @purpose Get 'UnitMgrImage2Version'
*
* @description [UnitMgrImage2Version]: Code version for Image2. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_UnitMgrImage2Version (void *wap, void *bufp)
{
  fpObjWa_t kwaStackMember = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStackMemberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objUnitMgrImage2VersionValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StackMember */
  kwaStackMember.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                                     (xLibU8_t *) & keyStackMemberValue,
                                     &kwaStackMember.len);
  if (kwaStackMember.rc != XLIBRC_SUCCESS)
  {
    kwaStackMember.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaStackMember);
    return kwaStackMember.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, kwaStackMember.len);

  /* get the value from application */
  owa.l7rc = usmDbImageVersionGet (keyStackMemberValue,USMDB_IMAGE2_NAME,
                              objUnitMgrImage2VersionValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: UnitMgrImage2Version */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objUnitMgrImage2VersionValue,
                           strlen (objUnitMgrImage2VersionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseStacking_UnitMgrSTKName
*
* @purpose Get 'UnitMgrSTKName'
*
* @description [UnitMgrSTKName]: STK file to be used for copy/delete/activate
*              operatiosn 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_UnitMgrSTKName (void *wap, void *bufp)
{
  fpObjWa_t kwaStackMember = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStackMemberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnitMgrSTKNameValue;
  xLibStr256_t imageNameStr;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StackMember */
  kwaStackMember.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                                     (xLibU8_t *) & keyStackMemberValue,
                                     &kwaStackMember.len);
  if (kwaStackMember.rc != XLIBRC_SUCCESS)
  {
    kwaStackMember.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaStackMember);
    return kwaStackMember.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, kwaStackMember.len);

  memset(imageNameStr,0x00,sizeof(imageNameStr));

  if (usmDbTransferFileNameLocalGet(L7_UNIT_CURRENT, imageNameStr) != L7_SUCCESS)
  {
      strcpy(imageNameStr, USMDB_IMAGE1_NAME);
  }

  if(strcmp(imageNameStr, USMDB_IMAGE2_NAME) == 0)
  {
      objUnitMgrSTKNameValue = L7_XUI_IMGTYPE_IMAGE2;
  }
  else
  {
      objUnitMgrSTKNameValue = L7_XUI_IMGTYPE_IMAGE1;
  }

  /* return the object value: UnitMgrSTKName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUnitMgrSTKNameValue,
                           sizeof (objUnitMgrSTKNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseStacking_UnitMgrSTKName
*
* @purpose Set 'UnitMgrSTKName'
*
* @description [UnitMgrSTKName]: STK file to be used for copy/delete/activate
*              operatiosn 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseStacking_UnitMgrSTKName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnitMgrSTKNameValue;
  fpObjWa_t kwaStackMember = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStackMemberValue;
    xLibStr256_t imageNameStr;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: UnitMgrSTKName */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objUnitMgrSTKNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUnitMgrSTKNameValue, owa.len);

  /* retrieve key: StackMember */
  kwaStackMember.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                                     (xLibU8_t *) & keyStackMemberValue,
                                     &kwaStackMember.len);
  if (kwaStackMember.rc != XLIBRC_SUCCESS)
  {
    kwaStackMember.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaStackMember);
    return kwaStackMember.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, kwaStackMember.len);

  memset(imageNameStr,0x00,sizeof(imageNameStr));

  if(objUnitMgrSTKNameValue == L7_XUI_IMGTYPE_IMAGE1)
  {
      strcpy(imageNameStr, USMDB_IMAGE1_NAME);
  }
  else
  {
      strcpy(imageNameStr, USMDB_IMAGE2_NAME);
  }

  /* set the value in application */
  owa.l7rc = usmDbTransferFileNameLocalSet (L7_UNIT_CURRENT, imageNameStr);
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
* @function fpObjSet_baseStacking_UnitMgrActivateSTK
*
* @purpose Set 'UnitMgrActivateSTK'
*
* @description [UnitMgrActivateSTK]: Activates the specified STK file on this
*              unit. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseStacking_UnitMgrActivateSTK (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnitMgrActivateSTKValue;
  fpObjWa_t kwaStackMember = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStackMemberValue;
  xLibStr256_t imageNameStr;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: UnitMgrActivateSTK */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objUnitMgrActivateSTKValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUnitMgrActivateSTKValue, owa.len);

  /* retrieve key: StackMember */
  kwaStackMember.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                                     (xLibU8_t *) & keyStackMemberValue,
                                     &kwaStackMember.len);
  if (kwaStackMember.rc != XLIBRC_SUCCESS)
  {
    kwaStackMember.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaStackMember);
    return kwaStackMember.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, kwaStackMember.len);

  if(objUnitMgrActivateSTKValue == L7_DISABLE)
  {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
  }

  memset(imageNameStr,0x00,sizeof(imageNameStr));

  owa.l7rc = usmDbTransferFileNameLocalGet(USMDB_UNIT_CURRENT, imageNameStr);

  if (owa.l7rc != L7_SUCCESS)
  {
    /* set the value in application */
    owa.l7rc = usmDbImageActivate (keyStackMemberValue,imageNameStr);
  }
  
  if (owa.l7rc  != L7_SUCCESS)
  {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseStacking_UnitMgrDeleteSTK
*
* @purpose Set 'UnitMgrDeleteSTK'
*
* @description [UnitMgrDeleteSTK]: Deletes the specified STK file on this
*              unit. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseStacking_UnitMgrDeleteSTK (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnitMgrDeleteSTKValue;
  fpObjWa_t kwaStackMember = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStackMemberValue;
  xLibStr256_t imageNameStr;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: UnitMgrDeleteSTK */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objUnitMgrDeleteSTKValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUnitMgrDeleteSTKValue, owa.len);

  /* retrieve key: StackMember */
  kwaStackMember.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                                     (xLibU8_t *) & keyStackMemberValue,
                                     &kwaStackMember.len);
  if (kwaStackMember.rc != XLIBRC_SUCCESS)
  {
    kwaStackMember.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaStackMember);
    return kwaStackMember.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, kwaStackMember.len);

  if(objUnitMgrDeleteSTKValue == L7_DISABLE)
  {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
  }

  memset(imageNameStr,0x00,sizeof(imageNameStr));

  owa.l7rc = usmDbTransferFileNameLocalGet(L7_UNIT_CURRENT, imageNameStr);
  if (owa.l7rc != L7_SUCCESS)
  {
    /* set the value in application */
    owa.l7rc = usmDbFileDelete (keyStackMemberValue,imageNameStr);
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
* @function fpObjGet_baseStacking_CodeVersion
*
* @purpose Get 'CodeVersion'
*
* @description 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_CodeVersion (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySupportedUnitIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objCodeVersionValue;
  usmDbCodeVersion_t ver;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SupportedUnitIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseStacking_SupportedUnitIndex,
                          (xLibU8_t *) & keySupportedUnitIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySupportedUnitIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbUnitMgrDetectCodeVerRunningGet(keySupportedUnitIndexValue, &ver);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  if (isalpha(ver.rel))
  {
    sprintf(objCodeVersionValue,"%c.%d.%d.%d",ver.rel, ver.ver, ver.maint_level, ver.build_num);
  }
  else
  {
    sprintf(objCodeVersionValue,"%d.%d.%d.%d",ver.rel, ver.ver, ver.maint_level, ver.build_num);
  }

  /* return the object value: SupportedModel */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objCodeVersionValue,
                           strlen (objCodeVersionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseStacking_DebugSupUnitsNextGetTest
*
* @purpose Set 'DebugSupUnitsNextGetTest'
*
* @description Enable Debug 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseStacking_DebugSupUnitsNextGetTest (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDebugSupUnitsNextGetTestValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DebugSupUnitsNextGetTest */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objDebugSupUnitsNextGetTestValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDebugSupUnitsNextGetTestValue, owa.len);

  /* set the value in application */
 /* owa.l7rc =  usmdbDebugSupUnitsNextGetTest ();*/
  if (owa.l7rc != L7_SUCCESS)
  {
    objDebugSupUnitsNextGetTestValue = L7_FALSE;
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objDebugSupUnitsNextGetTestValue = L7_TRUE;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseStacking_UnitMgrPropagateCfgSCFM2ACFM
*
* @purpose Set 'UnitMgrPropagateCfgSCFM2ACFM'
*
* @description Propagate configuration from Standby to Active unit 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseStacking_UnitMgrPropagateCfgSCFM2ACFM (void *wap,
                                                              void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnitMgrPropagateCfgSCFM2ACFMValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: UnitMgrPropagateCfg_SCFM2ACFM */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objUnitMgrPropagateCfgSCFM2ACFMValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUnitMgrPropagateCfgSCFM2ACFMValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbUnitMgrPropagateCfg_SCFM2ACFM ();
  if (owa.l7rc != L7_SUCCESS)
  {
    objUnitMgrPropagateCfgSCFM2ACFMValue = L7_FALSE;
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objUnitMgrPropagateCfgSCFM2ACFMValue = L7_TRUE;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseStacking_UnitMgrPropagateCfg
*
* @purpose Set 'UnitMgrPropagateCfg'
*
* @description Propagate configuration from Manager to Standby units 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseStacking_UnitMgrPropagateCfg (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnitMgrPropagateCfgValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: UnitMgrPropagateCfg */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objUnitMgrPropagateCfgValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUnitMgrPropagateCfgValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbUnitMgrPropagateCfg ();
  if (owa.l7rc != L7_SUCCESS)
  {
    objUnitMgrPropagateCfgValue = L7_FALSE;
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objUnitMgrPropagateCfgValue = L7_TRUE;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseStacking_StandbyCfmSlot
*
* @purpose Get 'StandbyCfmSlot'
*
* @description Physical number of the Slots on Standby 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_StandbyCfmSlot (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStandbyCfmSlotValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  usmDbStandbyCfmSlotGet(&objStandbyCfmSlotValue);
  owa.l7rc  = L7_SUCCESS;
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStandbyCfmSlotValue,
                     sizeof (objStandbyCfmSlotValue));

  /* return the object value: StandbyCfmSlot */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStandbyCfmSlotValue,
                           sizeof (objStandbyCfmSlotValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseStacking_UnitMgrUnitTypeClear
*
* @purpose Set 'UnitMgrUnitTypeClear'
*
* @description Reset the Unit Type to Factory default 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseStacking_UnitMgrUnitTypeClear (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnitMgrUnitTypeClearValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: UnitMgrUnitTypeClear */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objUnitMgrUnitTypeClearValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUnitMgrUnitTypeClearValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbUnitMgrUnitTypeClear ();
  if (owa.l7rc != L7_SUCCESS)
  {
    objUnitMgrUnitTypeClearValue = L7_FALSE;
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objUnitMgrUnitTypeClearValue = L7_TRUE;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseStacking_UnitMgrTransferMgmt
*
* @purpose Set 'UnitMgrTransferMgmt'
*
* @description Transfer the Unit Management 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseStacking_UnitMgrTransferMgmt (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnitMgrTransferMgmtValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: UnitMgrTransferMgmt */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objUnitMgrTransferMgmtValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUnitMgrTransferMgmtValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbUnitMgrTransferMgmt ( objUnitMgrTransferMgmtValue);
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
* @function fpObjSet_baseStacking_UnitMgrReplicateCfg
*
* @purpose Set 'UnitMgrReplicateCfg'
*
* @description Replicate the Config from Master to Member units 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseStacking_UnitMgrReplicateCfg (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnitMgrReplicateCfgValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: UnitMgrReplicateCfg */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objUnitMgrReplicateCfgValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUnitMgrReplicateCfgValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbUnitMgrReplicateCfg ( objUnitMgrReplicateCfgValue);
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
* @function fpObjSet_baseStacking_UnitMgrReplicateOpr
*
* @purpose Set 'UnitMgrReplicateOpr'
*
* @description Replicate the Code from Master to Member units 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseStacking_UnitMgrReplicateOpr (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnitMgrReplicateOprValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: UnitMgrReplicateOpr */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objUnitMgrReplicateOprValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUnitMgrReplicateOprValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbUnitMgrReplicateOpr (objUnitMgrReplicateOprValue);
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
* @function fpObjGet_baseStacking_UnitMgrMgrNumber
*
* @purpose Get 'UnitMgrMgrNumber'
*
* @description Unit number of the management unit 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_UnitMgrMgrNumber (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnitMgrMgrNumberValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbUnitMgrMgrNumberGet (&objUnitMgrMgrNumberValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUnitMgrMgrNumberValue,
                     sizeof (objUnitMgrMgrNumberValue));

  /* return the object value: UnitMgrMgrNumber */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUnitMgrMgrNumberValue,
                           sizeof (objUnitMgrMgrNumberValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseStacking_ChassisStandbyCMUnit
*
* @purpose Get 'ChassisStandbyCMUnit'
*
* @description Chassis model number of the Standby unit 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_ChassisStandbyCMUnit (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objChassisStandbyCMUnitValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmdbChassisStandbyCMUnitGet (&objChassisStandbyCMUnitValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objChassisStandbyCMUnitValue,
                     sizeof (objChassisStandbyCMUnitValue));

  /* return the object value: ChassisStandbyCMUnit */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objChassisStandbyCMUnitValue,
                           sizeof (objChassisStandbyCMUnitValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjList_baseStacking_StandbyStackMember
*
* @purpose Get 'StandbyStackMember'
*
* @description Lists all the UnitIDs for potential Standby Stack members
*              
* @note  This is a List Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseStacking_StandbyStackMember (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStackMemberValue;
  xLibU32_t nextObjStackMemberValue;
  L7_uint32 mgmtUnitId = 0;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StackMember */
  owa.rc = xLibFilterGet (wap, XOBJ_baseStacking_StandbyStackMember,
                          (xLibU8_t *) & objStackMemberValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
 nextObjStackMemberValue = L7_MAX_UNITS_PER_STACK + 1;
    objStackMemberValue = 0;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objStackMemberValue, owa.len);
    if(objStackMemberValue == L7_MAX_UNITS_PER_STACK + 1)
    {
      owa.l7rc = usmDbUnitMgrStackMemberGetFirst(&nextObjStackMemberValue);
      objStackMemberValue = 0;
    }
    else
    {
      owa.l7rc = usmDbUnitMgrStackMemberGetNext (objStackMemberValue,
                                                 &nextObjStackMemberValue);
    }
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_ENDOF_TABLE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }

  /* Skip over the Management Unit */
  owa.l7rc = usmDbUnitMgrMgrNumberGet(&mgmtUnitId);
  if (owa.l7rc != L7_SUCCESS)
  {
 owa.rc = XLIBRC_ENDOF_TABLE;
 FPOBJ_TRACE_EXIT (bufp, owa);
 return owa.rc;
  }
  if(mgmtUnitId == nextObjStackMemberValue)
  {
    objStackMemberValue = nextObjStackMemberValue;
    owa.l7rc = usmDbUnitMgrStackMemberGetNext (objStackMemberValue,
                                               &nextObjStackMemberValue);
  
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_ENDOF_TABLE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjStackMemberValue, owa.len);

  /* return the object value: StackMember */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjStackMemberValue,
                           sizeof (nextObjStackMemberValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseStacking_StandbyStackMember
*
* @purpose Get 'StandbyStackMember'
*
* @description UnitID of the current StandbyStackmember 
*              
* @note 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_StandbyStackMember (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCfgStandbyStackMemberValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve the object: current CfgStandbyStackMember */
  owa.l7rc = usmDbUnitMgrCfgStandbyGet(&objCfgStandbyStackMemberValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    objCfgStandbyStackMemberValue = L7_MAX_UNITS_PER_STACK + 1;
  }

  /* return the object value: objCfgStandbyStackMemberValue */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCfgStandbyStackMemberValue,
                           sizeof (objCfgStandbyStackMemberValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseStacking_StandbyStackMember
*
* @purpose Set 'StandbyStackMember'
*
* @description Set UnitID for the StandbyStackmember 
*              
* @note 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseStacking_StandbyStackMember (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStandbyStackMemberValue = 0;
  xLibU32_t objCfgStandbyStackMemberValue = 0;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: StandbyStackMember */
  owa.rc = xLibFilterGet (wap, XOBJ_baseStacking_StandbyStackMember,
                          (xLibU8_t *) & objStandbyStackMemberValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStandbyStackMemberValue, owa.len);

  /* set the value in application */
  if(objStandbyStackMemberValue == L7_MAX_UNITS_PER_STACK + 1)
  {
    owa.l7rc = usmDbUnitMgrCfgStandbyGet(&objCfgStandbyStackMemberValue);
    if(owa.l7rc == L7_SUCCESS)
    {
      owa.l7rc = usmDbUnitMgrStandbySet (objCfgStandbyStackMemberValue, L7_FALSE);
    }
    else
    {
      owa.l7rc = L7_SUCCESS;
    }
  }
  else
  {
    owa.l7rc = usmDbUnitMgrStandbySet (objStandbyStackMemberValue, L7_TRUE);
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
* @function fpObjGet_baseStacking_unitBootImageVersion
*
* @purpose Get 'unitBootImageVersion'
 *@description  [unitBootImageVersion] <HTML>Boot Image Version of the Unit in
* the stack   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_unitBootImageVersion (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objunitBootImageVersionValue;

  xLibU32_t keyStackMemberValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StackMember */
  owa.len = sizeof (keyStackMemberValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                          (xLibU8_t *) & keyStackMemberValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, owa.len);
  memset(objunitBootImageVersionValue,0x00,sizeof(objunitBootImageVersionValue));
  /* get the value from application */
  owa.l7rc = usmDbBootImageVersionGet (keyStackMemberValue, RUNNING_IMAGE,
                                       objunitBootImageVersionValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objunitBootImageVersionValue, strlen (objunitBootImageVersionValue));

  /* return the object value: unitBootImageVersion */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objunitBootImageVersionValue,
                           strlen (objunitBootImageVersionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseStacking_ServiceTag
*
* @purpose Get 'ServiceTag'
*
* @description 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_ServiceTag (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStackMemberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objServiceTagValue;
  memset(objServiceTagValue, 0x00, sizeof(objServiceTagValue));
 
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SupportedUnitIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                          (xLibU8_t *) & keyStackMemberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbUnitMgrServiceTagGet(keyStackMemberValue, objServiceTagValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objServiceTagValue,
                           strlen (objServiceTagValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseStacking_AssetTag
*
* @purpose Get 'AssetTag'
*
* @description 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_AssetTag(void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStackMemberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objAssetTagValue;
  memset(objAssetTagValue, 0x00, sizeof(objAssetTagValue));
 
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SupportedUnitIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                          (xLibU8_t *) & keyStackMemberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbUnitMgrAssetTagGet(keyStackMemberValue, objAssetTagValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objAssetTagValue,
                           strlen (objAssetTagValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseStacking_AssetTag
*
* @purpose Get 'AssetTag'
*
* @description 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseStacking_AssetTag(void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStackMemberValue;
 
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objAssetTagValue;
  memset(objAssetTagValue, 0x00, sizeof(objAssetTagValue));
 
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SupportedUnitIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                          (xLibU8_t *) & keyStackMemberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, kwa.len);

  /* retrieve object: objAssetTagValue */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objAssetTagValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objAssetTagValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbUnitMgrAssetTagSet (keyStackMemberValue,  objAssetTagValue);
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
* @function fpObjGet_baseStacking_DestinationTransferUnit
*
* @purpose Get 'DestinationTransferUnit'
*
* @description Index for Supported Unit Types 
*              
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_DestinationTransferUnit (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDestinationTransferUnitValue;
  xLibU32_t nextDestinationTransferUnitValue;
  xLibU32_t tempUnitNo;
 xLibU32_t masterUnitNo;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SupportedUnitIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baseStacking_DestinationTransferUnit,
                          (xLibU8_t *) & objDestinationTransferUnitValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextDestinationTransferUnitValue = 0;
    tempUnitNo =0;
 masterUnitNo =0;

 owa.l7rc = usmDbUnitMgrStackMemberGetFirst(&tempUnitNo);
 if(owa.l7rc == L7_SUCCESS)
 {
  usmDbUnitMgrNumberGet(&masterUnitNo);
     if (tempUnitNo == masterUnitNo)
     {
       owa.l7rc = usmDbUnitMgrStackMemberGetNext(tempUnitNo, &nextDestinationTransferUnitValue);
     }
 }
 
  }
  else
  {
 owa.l7rc = usmDbUnitMgrStackMemberGetNext(objDestinationTransferUnitValue, &tempUnitNo);
 if(owa.l7rc == L7_SUCCESS)
 {
     usmDbUnitMgrNumberGet(&masterUnitNo);
     if (tempUnitNo == masterUnitNo)
     {
       owa.l7rc = usmDbUnitMgrStackMemberGetNext(tempUnitNo, &nextDestinationTransferUnitValue);
     }
    else
    {
     nextDestinationTransferUnitValue = tempUnitNo;
    }
 }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
 
  FPOBJ_TRACE_NEW_KEY (bufp, &nextDestinationTransferUnitValue, owa.len);

  /* return the object value: SupportedUnitIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextDestinationTransferUnitValue,
                           sizeof (nextDestinationTransferUnitValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseStacking_CopyMasterImageToStackMember
*
* @purpose Set 'CopyMasterIamgeToStackMember'
*
* @description 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseStacking_CopyMasterImageToStackMember(void *wap, void *bufp)
{
 fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
 xLibU32_t keyStackMemberValue;

 fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
 xLibU32_t objCopyMasterImageToStackMemberValue;
 xLibU32_t masterUnitNo;

  L7_char8  localFileName[DIM_MAX_FILENAME_SIZE];
  
 masterUnitNo = 0;
 memset(localFileName, 0x00, sizeof(localFileName));

 FPOBJ_TRACE_ENTER (bufp);

 /* retrieve key: SupportedUnitIndex */
 kwa.rc = xLibFilterGet (wap, XOBJ_baseStacking_DestinationTransferUnit,
                         (xLibU8_t *) & keyStackMemberValue, &kwa.len);
 if (kwa.rc != XLIBRC_SUCCESS)
 {
   kwa.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
   FPOBJ_TRACE_EXIT (bufp, kwa);
   return kwa.rc;
 }
 FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, kwa.len);

 /* retrieve object: objAssetTagValue */
 owa.rc = xLibBufDataGet (bufp,
                          (xLibU8_t *) &objCopyMasterImageToStackMemberValue,
                          &owa.len);
 if (owa.rc != XLIBRC_SUCCESS)
 {
   FPOBJ_TRACE_EXIT (bufp, owa);
   return owa.rc;
 }
 FPOBJ_TRACE_VALUE (bufp, &objCopyMasterImageToStackMemberValue, owa.len);


 usmDbUnitMgrNumberGet(&masterUnitNo);
 if ((L7_SUCCESS != usmDbActiveImageNameGet(masterUnitNo, localFileName)) ||
   (L7_TRUE != usmDbImageExists(localFileName)))
 {
  usmDbTrapMgrCopyFailedTrapSend();
  owa.l7rc = L7_FAILURE;
 }
 else
 {
  if (L7_SUCCESS != usmDbFileCopy(keyStackMemberValue, localFileName, "backup", L7_TRUE))
  {
    usmDbTrapMgrCopyFailedTrapSend();
    owa.l7rc = L7_FAILURE;
  }
  else
  {
    usmDbTrapMgrCopyFinishedTrapSend();
    owa.l7rc = L7_SUCCESS;
  }
 } 

 /* set the value in application */
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
* @function fpObjGet_baseStacking_UnitMgrManagementStatus
*
* @purpose Get 'UnitMgrManagementStatus'
*
* @description [UnitMgrManagementStatus]: STK file to be used for copy/delete/activate
*              operatiosn 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_UnitMgrManagementStatus(void *wap, void *bufp)
{
  fpObjWa_t kwaStackMember = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStackMemberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnitMgrManagementStatusValue;
  xLibU32_t tempUnitno = 0;;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StackMember */
  kwaStackMember.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                                     (xLibU8_t *) & keyStackMemberValue,
                                     &kwaStackMember.len);
  if (kwaStackMember.rc != XLIBRC_SUCCESS)
  {
    kwaStackMember.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaStackMember);
    return kwaStackMember.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, kwaStackMember.len);

  /*check if the unit is the management unit */

  owa.l7rc = usmDbUnitMgrMgrNumberGet(&tempUnitno);
  if( (owa.l7rc == L7_SUCCESS) && (tempUnitno == keyStackMemberValue) )
  {
  objUnitMgrManagementStatusValue = L7_XUI_MANAGEMENTUNIT;
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUnitMgrManagementStatusValue,
                       sizeof (objUnitMgrManagementStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
  }

  /*check if the unit is standby unit */
  owa.l7rc = usmDbUnitMgrCfgStandbyGet(&tempUnitno);
  if( (owa.l7rc == L7_SUCCESS) && (tempUnitno == keyStackMemberValue) )
  {
  objUnitMgrManagementStatusValue = L7_XUI_STANDBY;
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUnitMgrManagementStatusValue,
                       sizeof (objUnitMgrManagementStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
  }

  objUnitMgrManagementStatusValue = L7_XUI_STACKMEMBER;
  /* return the object value: UnitMgrSTKName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUnitMgrManagementStatusValue,
                           sizeof (objUnitMgrManagementStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseStacking_UnitMgrManagementStatus
*
* @purpose Set 'UnitMgrManagementStatus'
*
* @description [UnitMgrManagementStatus]: STK file to be used for copy/delete/activate
*              operatiosn 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseStacking_UnitMgrManagementStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnitMgrManagementStatusValue;
  fpObjWa_t kwaStackMember = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStackMemberValue;
  xLibU32_t tempUnitno = 0;
  usmDbUnitMgrStatus_t unitStatus;
  L7_BOOL status;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: UnitMgrSTKName */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objUnitMgrManagementStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUnitMgrManagementStatusValue, owa.len);

  /* retrieve key: StackMember */
  kwaStackMember.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                                     (xLibU8_t *) & keyStackMemberValue,
                                     &kwaStackMember.len);
  if (kwaStackMember.rc != XLIBRC_SUCCESS)
  {
    kwaStackMember.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaStackMember);
    return kwaStackMember.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, kwaStackMember.len);

  switch(objUnitMgrManagementStatusValue)
 {
    case L7_XUI_MANAGEMENTUNIT:
   /*check if unit is already mgmt unit */
   owa.l7rc = usmDbUnitMgrMgrNumberGet(&tempUnitno);
   if( owa.l7rc != L7_SUCCESS )
   {
    owa.rc = XLIBRC_FAILED_SETTING_MGMTUNIT;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
   }
   else if(tempUnitno == keyStackMemberValue)
   {
    owa.rc = XLIBRC_SUCCESS;    /* already mgmt unit. Hence no action. */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
   }
   
   owa.l7rc = usmDbUnitMgrTransferMgmt(keyStackMemberValue);
   if(owa.l7rc != L7_SUCCESS)
   {
    owa.rc = XLIBRC_FAILED_SETTING_MGMTUNIT;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
   }
   break;
 case L7_XUI_STANDBY:
   owa.l7rc = usmDbUnitMgrStandbyGet(&tempUnitno, &status);
   if( (owa.l7rc == L7_SUCCESS) && (tempUnitno != keyStackMemberValue ) )
   {
     if (usmDbUnitMgrMgrNumberGet(&tempUnitno) != L7_SUCCESS) 
    {
     owa.rc = XLIBRC_STNDBY_SET_FAILED;    /* TODO: Change if required */
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
    }

    if ((tempUnitno == keyStackMemberValue) )
    {
     owa.rc = XLIBRC_STANDBYUNIT_CANNOTBE_MGMTUNIT;    /* cannot set mgmt unit as standby. */
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
    }

     /* check if the unit is pre-config/active 
      * then only set it as standby
      */
    if (usmDbUnitMgrUnitStatusGet(keyStackMemberValue, &unitStatus) != L7_SUCCESS)
    {
     owa.rc = XLIBRC_STNDBY_SET_FAILED;    /* TODO: Change if required */
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
    }

    if (unitStatus != L7_USMDB_UNITMGR_UNIT_NOT_PRESENT &&
        unitStatus != L7_USMDB_UNITMGR_UNIT_OK)
    {
     owa.rc = XLIBRC_STNDBY_SHDBE_VALIDUNIT;    /* TODO: Change if required */
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
    }

    if (usmDbUnitMgrStandbySet(keyStackMemberValue, L7_TRUE) != L7_SUCCESS)
    {
     owa.rc = XLIBRC_STNDBY_SET_FAILED;    /* TODO: Change if required */
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
    }
   }
   else
   {
    owa.rc = XLIBRC_SUCCESS;    /* already standby unit. Hence no action. */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
   }
   break;
 case L7_XUI_STACKMEMBER:
     /*ichk the current unit type. if its a mgmt unit */
    
   if (usmDbUnitMgrMgrNumberGet(&tempUnitno) != L7_SUCCESS) 
   {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
     }

   if ((tempUnitno == keyStackMemberValue) )
   {
       /* Cannot change status of management unit */
    owa.rc = XLIBRC_SET_STKMEMBER_TO_MGMTUNIT_FAILED;    /* cannot set mgmt unit as standby. */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
   }

   /*remove if its a standby unit */
   owa.l7rc = usmDbUnitMgrStandbyGet(&tempUnitno, &status);
   if( (owa.l7rc == L7_SUCCESS) && (tempUnitno == keyStackMemberValue ) )
   {
         if (usmDbUnitMgrStandbySet(tempUnitno, L7_FALSE) != L7_SUCCESS)
         {
      owa.rc = XLIBRC_STACKMEMBER_SET_FAILED;    /* cannot set  unit as standby. */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
         }
   }
     break;
 default :
   owa.rc = XLIBRC_FAILURE;   
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseStacking_UnitMgrManagementStandByStatus
*
* @purpose Get 'UnitMgrManagementStandByStatus'
*
* @description [UnitMgrManagementStandByStatus]: 
*              operatiosn 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_UnitMgrManagementStandByStatus(void *wap, void *bufp)
{
  fpObjWa_t kwaStackMember = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStackMemberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnitMgrManagementStatusValue;

  L7_uint32 operStandby = 0, cfgStandby = 0;
  L7_BOOL mode = L7_FALSE;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StackMember */
  kwaStackMember.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                                     (xLibU8_t *) & keyStackMemberValue,
                                     &kwaStackMember.len);
  if (kwaStackMember.rc != XLIBRC_SUCCESS)
  {
    kwaStackMember.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaStackMember);
    return kwaStackMember.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, kwaStackMember.len);

  usmDbUnitMgrStandbyGet(&operStandby,&mode);
  usmDbUnitMgrCfgStandbyGet(&cfgStandby);

  if (operStandby == cfgStandby && cfgStandby == keyStackMemberValue)
  {
     objUnitMgrManagementStatusValue = L7_XUI_CFG_STANDBY;
     owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUnitMgrManagementStatusValue,
                       sizeof (objUnitMgrManagementStatusValue));
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;  
  }
  else if (operStandby != cfgStandby && cfgStandby == keyStackMemberValue)
  {
     objUnitMgrManagementStatusValue = L7_XUI_CFG_STANDBY;
     owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUnitMgrManagementStatusValue,
                       sizeof (objUnitMgrManagementStatusValue));
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;  
  }
  else if (operStandby != cfgStandby && operStandby == keyStackMemberValue)
  {
     objUnitMgrManagementStatusValue = L7_XUI_OPR_STANDBY;
     owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUnitMgrManagementStatusValue,
                       sizeof (objUnitMgrManagementStatusValue));
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;  
  }

  objUnitMgrManagementStatusValue = L7_XUI_NONE_STANDBY;
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUnitMgrManagementStatusValue,
                           sizeof (objUnitMgrManagementStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


