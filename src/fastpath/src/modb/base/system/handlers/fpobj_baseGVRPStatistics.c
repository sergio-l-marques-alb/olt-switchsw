
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_baseGVRPStatistics.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to base-object.xml
*
* @create  19 October 2008, Sunday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_baseGVRPStatistics_obj.h"
#include "usmdb_util_api.h"
#include "usmdb_mib_vlan_api.h"

/*******************************************************************************
* @function fpObjGet_baseGVRPStatistics_ifIndex
*
* @purpose Get 'ifIndex'
 *@description  [ifIndex]  A unique value, greater than zero, for each
* interface. It is recommended that values are assigned contiguously
* starting from 1. The value for each interface sub-layer must remain
* constant at least from one re-initial   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseGVRPStatistics_ifIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objifIndexValue;
  xLibU32_t nextObjifIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  owa.len = sizeof (objifIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseGVRPStatistics_ifIndex,
                          (xLibU8_t *) & objifIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbIntIfNumTypeFirstGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF, 
                                         0, &nextObjifIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objifIndexValue, owa.len);
    owa.l7rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF,
                                        0, objifIndexValue, &nextObjifIndexValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjifIndexValue, owa.len);

  /* return the object value: ifIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjifIndexValue, sizeof (nextObjifIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseGVRPStatistics_GVRPStatsAttributeType
*
* @purpose Get 'GVRPStatsAttributeType'
 *@description  [GVRPStatsAttributeType] This object used to identify a GVRP
* attribute.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseGVRPStatistics_GVRPStatsAttributeType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objGVRPStatsAttributeTypeValue;
  xLibU32_t nextObjGVRPStatsAttributeTypeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: GVRPStatsAttributeType */
  owa.len = sizeof (objGVRPStatsAttributeTypeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseGVRPStatistics_GVRPStatsAttributeType,
                          (xLibU8_t *) & objGVRPStatsAttributeTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjGVRPStatsAttributeTypeValue = L7_XUI_GARP_JOIN_EMPTY;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objGVRPStatsAttributeTypeValue, owa.len);
    if ((objGVRPStatsAttributeTypeValue < L7_XUI_GARP_LEAVEALL) && (objGVRPStatsAttributeTypeValue > 0))
    {
      nextObjGVRPStatsAttributeTypeValue = objGVRPStatsAttributeTypeValue + 1;
      owa.l7rc = L7_SUCCESS;
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

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjGVRPStatsAttributeTypeValue, owa.len);

  /* return the object value: GVRPStatsAttributeType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjGVRPStatsAttributeTypeValue,
                           sizeof (nextObjGVRPStatsAttributeTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseGVRPStatistics_GVRPStatsAttributeRx
*
* @purpose Get 'GVRPStatsAttributeRx'
 *@description  [GVRPStatsAttributeRx] This object indicates the number of
* packets received with the given attribute and on a given interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseGVRPStatistics_GVRPStatsAttributeRx (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objGVRPStatsAttributeRxValue;
              
  xLibU32_t keyGVRPStatsAttributeTypeValue;
  xLibU32_t keyifIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  owa.len = sizeof(keyifIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseGVRPStatistics_ifIndex,
                        (xLibU8_t *) &keyifIndexValue, 
                        &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keyifIndexValue, 
           owa.len);

  /* retrieve key: GVRPStatsAttributeType */
  owa.len = sizeof(keyGVRPStatsAttributeTypeValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseGVRPStatistics_GVRPStatsAttributeType,
                         (xLibU8_t *) &keyGVRPStatsAttributeTypeValue, 
                         &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keyGVRPStatsAttributeTypeValue, 
                          owa.len);

  /* get the value from application */
  owa.l7rc = usmDbGarpRxStatisticGet(keyifIndexValue, keyGVRPStatsAttributeTypeValue, 
                                     &objGVRPStatsAttributeRxValue);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;  
  }

  FPOBJ_TRACE_VALUE (bufp, &objGVRPStatsAttributeRxValue, sizeof(objGVRPStatsAttributeRxValue));

  /* return the object value: GVRPStatsAttributeRx */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objGVRPStatsAttributeRxValue,
                      sizeof(objGVRPStatsAttributeRxValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}



/*******************************************************************************
* @function fpObjGet_baseGVRPStatistics_GVRPStatsAttributeTx
*
* @purpose Get 'GVRPStatsAttributeTx'
 *@description  [GVRPStatsAttributeTx] This object indicates the number of
* packets transmitted with the given attribute and on a given interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseGVRPStatistics_GVRPStatsAttributeTx (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objGVRPStatsAttributeTxValue;

  xLibU32_t keyifIndexValue;
  xLibU32_t keyGVRPStatsAttributeTypeValue;


  FPOBJ_TRACE_ENTER(bufp);



  /* retrieve key: ifIndex */
  owa.len = sizeof(keyifIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseGVRPStatistics_ifIndex,
                         (xLibU8_t *) &keyifIndexValue, 
                         &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keyifIndexValue, 
            owa.len);

  /* retrieve key: GVRPStatsAttributeType */
  owa.len = sizeof(keyGVRPStatsAttributeTypeValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseGVRPStatistics_GVRPStatsAttributeType,
                         (xLibU8_t *) &keyGVRPStatsAttributeTypeValue, 
                         &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keyGVRPStatsAttributeTypeValue, 
                          owa.len);

  /* get the value from application */
  owa.l7rc = usmDbGarpTxStatisticGet(keyifIndexValue, keyGVRPStatsAttributeTypeValue, 
                                     &objGVRPStatsAttributeTxValue);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;  
  }

  FPOBJ_TRACE_VALUE (bufp, &objGVRPStatsAttributeTxValue, sizeof(objGVRPStatsAttributeTxValue));

  /* return the object value: GVRPStatsAttributeTx */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objGVRPStatsAttributeTxValue,
                      sizeof(objGVRPStatsAttributeTxValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}
