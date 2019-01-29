
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_baseIsdpInterfaceTable.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to base-object.xml
*
* @create  05 June 2008, Thursday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_baseIsdpInterfaceTable_obj.h"
#include "usmdb_isdp_api.h"
#include "usmdb_util_api.h"
/*******************************************************************************
* @function fpObjGet_baseIsdpInterfaceTable_IsdpInterfaceIfIndex
*
* @purpose Get 'IsdpInterfaceIfIndex'
 *@description  [IsdpInterfaceIfIndex] The ifIndex value of the local interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIsdpInterfaceTable_IsdpInterfaceIfIndex (void *wap, void *bufp)
{
 
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIsdpInterfaceIfIndexValue;
  xLibU32_t nextObjIsdpInterfaceIfIndexValue;
 
  FPOBJ_TRACE_ENTER (bufp);
 
  /* retrieve key: IsdpInterfaceIfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baseIsdpInterfaceTable_IsdpInterfaceIfIndex,
                          (xLibU8_t *) & objIsdpInterfaceIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbIntIfNumTypeFirstGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF,
                                 0, &nextObjIsdpInterfaceIfIndexValue);                
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objIsdpInterfaceIfIndexValue, owa.len);
    owa.l7rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF,
                                               0, objIsdpInterfaceIfIndexValue, &nextObjIsdpInterfaceIfIndexValue); 
  }
 
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
 
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjIsdpInterfaceIfIndexValue, owa.len);
 
  /* return the object value: IsdpInterfaceIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjIsdpInterfaceIfIndexValue,
                           sizeof (nextObjIsdpInterfaceIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseIsdpInterfaceTable_IsdpInterfaceEnable
*
* @purpose Get 'IsdpInterfaceEnable'
 *@description  [IsdpInterfaceEnable] An indication of whether the Industry
* Standard Discovery Protocol is currently running on this interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIsdpInterfaceTable_IsdpInterfaceEnable (void *wap, void *bufp)
{

  fpObjWa_t kwaIsdpInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIsdpInterfaceIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIsdpInterfaceEnableValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IsdpInterfaceIfIndex */
  kwaIsdpInterfaceIfIndex.rc = xLibFilterGet (wap, XOBJ_baseIsdpInterfaceTable_IsdpInterfaceIfIndex,
                                              (xLibU8_t *) & keyIsdpInterfaceIfIndexValue,
                                              &kwaIsdpInterfaceIfIndex.len);
  if (kwaIsdpInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaIsdpInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaIsdpInterfaceIfIndex);
    return kwaIsdpInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIsdpInterfaceIfIndexValue, kwaIsdpInterfaceIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmdbIsdpIntfModeGet (keyIsdpInterfaceIfIndexValue,
                                                      &objIsdpInterfaceEnableValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IsdpInterfaceEnable */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIsdpInterfaceEnableValue,
                           sizeof (objIsdpInterfaceEnableValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseIsdpInterfaceTable_IsdpInterfaceEnable
*
* @purpose Set 'IsdpInterfaceEnable'
 *@description  [IsdpInterfaceEnable] An indication of whether the Industry
* Standard Discovery Protocol is currently running on this interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseIsdpInterfaceTable_IsdpInterfaceEnable (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIsdpInterfaceEnableValue;

  fpObjWa_t kwaIsdpInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIsdpInterfaceIfIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: IsdpInterfaceEnable */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objIsdpInterfaceEnableValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objIsdpInterfaceEnableValue, owa.len);

  /* retrieve key: IsdpInterfaceIfIndex */
  kwaIsdpInterfaceIfIndex.rc = xLibFilterGet (wap, XOBJ_baseIsdpInterfaceTable_IsdpInterfaceIfIndex,
                                                                  (xLibU8_t *) & keyIsdpInterfaceIfIndexValue,
                                                                  &kwaIsdpInterfaceIfIndex.len);
  if (kwaIsdpInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaIsdpInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaIsdpInterfaceIfIndex);
    return kwaIsdpInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIsdpInterfaceIfIndexValue, kwaIsdpInterfaceIfIndex.len);

  /* set the value in application */
  owa.l7rc = usmdbIsdpIntfModeSet (keyIsdpInterfaceIfIndexValue, objIsdpInterfaceEnableValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
