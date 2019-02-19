
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_SwitchingGarpGlobalPortConfig.c
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
#include "_xe_SwitchingGarpGlobalPortConfig_obj.h"
#include "usmdb_garp.h"
#include "usmdb_mib_vlan_api.h"
#include "usmdb_util_api.h"

/*******************************************************************************
* @function fpObjSet_SwitchingGarpGlobalPortConfig_GvrpStatus
*
* @purpose Set 'GvrpStatus'
 *@description  [GvrpStatus] The state of GVRP operation on this port. The value
* enabled(1) indicates that GVRP is enabled.  GVRP Status is still
* enabled for the device, GVRP    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingGarpGlobalPortConfig_GvrpStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objGvrpStatusValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: GvrpStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objGvrpStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objGvrpStatusValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbQportsEnableGVRPSet(L7_UNIT_CURRENT,L7_ALL_INTERFACES, objGvrpStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_SwitchingGarpGlobalPortConfig_GmrpStatus
*
* @purpose Set 'GmrpStatus'
 *@description  [GmrpStatus] The administrative status requested by management
* for GMRP. The value enabled(1) indicates that GMRP should be
* enabled on this device, in all VLANs, on all ports for which it has
* not been specifically disabled. When disabled(2), GMRP is disabled,
* in al   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingGarpGlobalPortConfig_GmrpStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objGmrpStatusValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: GmrpStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objGmrpStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objGmrpStatusValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbGarpGmrpPortEnableSet(L7_UNIT_CURRENT,L7_ALL_INTERFACES,objGmrpStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_SwitchingGarpGlobalPortConfig_JoinTime
*
* @purpose Set 'JoinTime'
 *@description  [JoinTime] The GARP Join time, in centiseconds.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingGarpGlobalPortConfig_JoinTime (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objJoinTimeValue;
  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;


  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: JoinTime */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objJoinTimeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objJoinTimeValue, owa.len);
  nextObjInterfaceValue = 0;
  owa.l7rc = usmDbValidIntIfNumFirstGet ( &nextObjInterfaceValue);
  while(owa.l7rc == L7_SUCCESS)
  {
    /* if row status object is specified and eual to delete return success */

    /* set the value in application */
    objInterfaceValue = nextObjInterfaceValue;
    (void)usmDbGarpJoinTimeSet (L7_UNIT_CURRENT, nextObjInterfaceValue,
                                 objJoinTimeValue);
    owa.l7rc = usmDbValidIntIfNumNext(objInterfaceValue,
                                        &nextObjInterfaceValue);
    
  }
#if 0
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
#endif
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_SwitchingGarpGlobalPortConfig_LeaveTime
*
* @purpose Set 'LeaveTime'
 *@description  [LeaveTime] The GARP Leave time, in centiseconds.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingGarpGlobalPortConfig_LeaveTime (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLeaveTimeValue;
  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: LeaveTime */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objLeaveTimeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLeaveTimeValue, owa.len);
  nextObjInterfaceValue = 0;
  owa.l7rc = usmDbValidIntIfNumFirstGet ( &nextObjInterfaceValue);
  while(owa.l7rc == L7_SUCCESS)
  {
    /* if row status object is specified and eual to delete return success */

    /* set the value in application */
    objInterfaceValue = nextObjInterfaceValue;
    (void)usmDbGarpLeaveTimeSet (L7_UNIT_CURRENT, nextObjInterfaceValue,
                                    objLeaveTimeValue);
    owa.l7rc = usmDbValidIntIfNumNext(objInterfaceValue,
                                        &nextObjInterfaceValue);

  }
#if 0
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
#endif
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_SwitchingGarpGlobalPortConfig_LeaveAllTime
*
* @purpose Set 'LeaveAllTime'
 *@description  [LeaveAllTime] The GARP LeaveAll time, in centiseconds   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingGarpGlobalPortConfig_LeaveAllTime (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLeaveAllTimeValue;
  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: LeaveAllTime */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objLeaveAllTimeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLeaveAllTimeValue, owa.len);
  nextObjInterfaceValue = 0;
  owa.l7rc = usmDbValidIntIfNumFirstGet ( &nextObjInterfaceValue);
  while(owa.l7rc == L7_SUCCESS)
  {
    /* if row status object is specified and eual to delete return success */

    /* set the value in application */
    objInterfaceValue = nextObjInterfaceValue;
    (void)usmDbGarpLeaveAllTimeSet(L7_UNIT_CURRENT, nextObjInterfaceValue,
                                    objLeaveAllTimeValue);
    owa.l7rc = usmDbValidIntIfNumNext(objInterfaceValue,
                                        &nextObjInterfaceValue);

  }
#if 0
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
#endif
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
