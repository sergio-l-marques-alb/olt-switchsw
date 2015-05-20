
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_basepowerSupplyStatusTable.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to base-object.xml
*
* @create  25 September 2008, Thursday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_basepowerSupplyStatusTable_obj.h"
#include "usmdb_boxs_api.h"

/*******************************************************************************
* @function fpObjGet_basepowerSupplyStatusTable_unitIndex
*
* @purpose Get 'unitIndex'
 *@description  [unitIndex] unitIndex is the value of valid stack number in the
* stack   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basepowerSupplyStatusTable_unitIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objunitIndexValue;
  xLibU32_t nextObjunitIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: unitIndex */
  owa.len = sizeof (objunitIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basepowerSupplyStatusTable_unitIndex,
                          (xLibU8_t *) & objunitIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbUnitMgrStackMemberGetFirst(&nextObjunitIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objunitIndexValue, owa.len);
    owa.l7rc = usmDbUnitMgrStackMemberGetNext(objunitIndexValue, &nextObjunitIndexValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjunitIndexValue, owa.len);

  /* return the object value: unitIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjunitIndexValue,
                           sizeof (nextObjunitIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basepowerSupplyStatusTable_powerSupplyIndex
*
* @purpose Get 'powerSupplyIndex'
 *@description  [powerSupplyIndex] Index value used to identify the power supply
* for the given stack member.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basepowerSupplyStatusTable_powerSupplyIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objpowerSupplyIndexValue;
  xLibU32_t nextObjpowerSupplyIndexValue;

  xLibU32_t keyunitIndexValue;
  xLibU32_t numPowSupplies;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: unitIndex */
  owa.len = sizeof (keyunitIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basepowerSupplyStatusTable_unitIndex,
                          (xLibU8_t *) & keyunitIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyunitIndexValue, owa.len);

  /* retrieve key: powerSupplyIndex */
  owa.len = sizeof (objpowerSupplyIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basepowerSupplyStatusTable_powerSupplyIndex,
                          (xLibU8_t *) & objpowerSupplyIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjpowerSupplyIndexValue = 0;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpowerSupplyIndexValue, owa.len);
    if (usmDbBoxsNumOfPowSuppliesGet(keyunitIndexValue, &numPowSupplies) == L7_SUCCESS)
    {
      if (objpowerSupplyIndexValue < numPowSupplies)
      {
         nextObjpowerSupplyIndexValue = objpowerSupplyIndexValue + 1;
         owa.l7rc = L7_SUCCESS;
      }
      else
      {
        owa.l7rc = L7_FAILURE;
      }
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

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpowerSupplyIndexValue, owa.len);

  /* return the object value: powerSupplyIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjpowerSupplyIndexValue,
                           sizeof (nextObjpowerSupplyIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basepowerSupplyStatusTable_powerSupplyType
*
* @purpose Get 'powerSupplyType'
 *@description  [powerSupplyType] value indicating whether the power supply
* module is fixed or removable.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basepowerSupplyStatusTable_powerSupplyType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objpowerSupplyTypeValue;
  xLibU32_t objpowerSupplyStateValue;

  xLibU32_t keyunitIndexValue;
  xLibU32_t keypowerSupplyIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: unitIndex */
  owa.len = sizeof (keyunitIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basepowerSupplyStatusTable_unitIndex,
                          (xLibU8_t *) & keyunitIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyunitIndexValue, owa.len);

  /* retrieve key: powerSupplyIndex */
  owa.len = sizeof (keypowerSupplyIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basepowerSupplyStatusTable_powerSupplyIndex,
                          (xLibU8_t *) & keypowerSupplyIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypowerSupplyIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbBoxsPowSupplyStatusGet(keyunitIndexValue, keypowerSupplyIndexValue, 
                                         &objpowerSupplyTypeValue, &objpowerSupplyStateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objpowerSupplyTypeValue, sizeof (objpowerSupplyTypeValue));

  /* return the object value: powerSupplyType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpowerSupplyTypeValue,
                           sizeof (objpowerSupplyTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basepowerSupplyStatusTable_powerSupplyState
*
* @purpose Get 'powerSupplyState'
 *@description  [powerSupplyState] Status indicating whether the power supply is
* supplying power or not.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basepowerSupplyStatusTable_powerSupplyState (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objpowerSupplyStateValue;
  xLibU32_t objpowerSupplyTypeValue;

  xLibU32_t keyunitIndexValue;
  xLibU32_t keypowerSupplyIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: unitIndex */
  owa.len = sizeof (keyunitIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basepowerSupplyStatusTable_unitIndex,
                          (xLibU8_t *) & keyunitIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyunitIndexValue, owa.len);

  /* retrieve key: powerSupplyIndex */
  owa.len = sizeof (keypowerSupplyIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basepowerSupplyStatusTable_powerSupplyIndex,
                          (xLibU8_t *) & keypowerSupplyIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypowerSupplyIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbBoxsPowSupplyStatusGet(keyunitIndexValue, keypowerSupplyIndexValue,
                                         &objpowerSupplyTypeValue, &objpowerSupplyStateValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objpowerSupplyStateValue, sizeof (objpowerSupplyStateValue));

  /* return the object value: powerSupplyState */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpowerSupplyStateValue,
                           sizeof (objpowerSupplyStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

