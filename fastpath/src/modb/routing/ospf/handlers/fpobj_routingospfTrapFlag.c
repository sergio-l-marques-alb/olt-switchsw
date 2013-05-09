/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingospfTrapFlag.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to ospfTrap-object.xml
*
* @create  30 March 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_routingospfTrapFlag_obj.h"
#include "usmdb_mib_ospf_api.h"
#include "usmdb_ospf_api.h"

/*******************************************************************************
* @function fpObjGet_routingospfTrapFlag_TrapFlag
*
* @purpose Get 'TrapFlag'
*
* @description [TrapFlag]: Allows a user to determine configured ospf trap
*              flags 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfTrapFlag_TrapFlag (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTrapFlagValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbOspfTrapFlagsGet (L7_UNIT_CURRENT, &objTrapFlagValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTrapFlagValue, sizeof (objTrapFlagValue));

  /* return the object value: TrapFlag */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTrapFlagValue,
                           sizeof (objTrapFlagValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfTrapFlag_TrapMode
*
* @purpose Get 'TrapMode'
*
* @description [TrapMode]: Allows a user to determine whether the specified
*              trap is enabled or disabled 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfTrapFlag_TrapMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTrapModeValue;
  xLibU32_t objTrapTypeValue = L7_OSPF_TRAP_ALL;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbOspfTrapModeGet (L7_UNIT_CURRENT, &objTrapModeValue, objTrapTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTrapModeValue, sizeof (objTrapModeValue));

  /* return the object value: TrapMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTrapModeValue,
                           sizeof (objTrapModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfTrapFlag_TrapMode
*
* @purpose Set 'TrapMode'
*
* @description [TrapMode]: Allows a user to determine whether the specified
*              trap is enabled or disabled 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfTrapFlag_TrapMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTrapModeValue;
  xLibU32_t objTrapTypeValue = L7_OSPF_TRAP_ALL;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TrapMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTrapModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTrapModeValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfTrapModeSet (L7_UNIT_CURRENT, objTrapModeValue, objTrapTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
