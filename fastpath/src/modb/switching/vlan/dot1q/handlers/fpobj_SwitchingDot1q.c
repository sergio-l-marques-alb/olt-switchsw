/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_SwitchingDot1q.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to dot1q-object.xml
*
* @create  9 March 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_SwitchingDot1q_obj.h"
#include "usmdb_garp.h"
#include "usmdb_mib_vlan_api.h"

/*******************************************************************************
* @function fpObjGet_SwitchingDot1q_GvrpStatus
*
* @purpose Get 'GvrpStatus'
*
* @description [GvrpStatus]: The administrative status requested by management
*              for GVRP. The value enabled(1) indicates that GVRP should
*              be enabled on this device, on all ports for which it has
*              not been specifically disabled. When disabled(2), GVRP is
*              disabled on all ports and all GVRP packets will be forwarded
*              transparently. This object affects all GVRP Applicant and
*              Registrar state machines. A transition from disabled(2) to
*              enabled(1) will cause a reset of all GVRP state machines
*              on all ports. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1q_GvrpStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objGvrpStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbDot1qGvrpStatusGet (L7_UNIT_CURRENT, &objGvrpStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objGvrpStatusValue, sizeof (objGvrpStatusValue));

  /* return the object value: GvrpStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objGvrpStatusValue,
                           sizeof (objGvrpStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingDot1q_GvrpStatus
*
* @purpose Set 'GvrpStatus'
*
* @description [GvrpStatus]: The administrative status requested by management
*              for GVRP. The value enabled(1) indicates that GVRP should
*              be enabled on this device, on all ports for which it has
*              not been specifically disabled. When disabled(2), GVRP is
*              disabled on all ports and all GVRP packets will be forwarded
*              transparently. This object affects all GVRP Applicant and
*              Registrar state machines. A transition from disabled(2) to
*              enabled(1) will cause a reset of all GVRP state machines
*              on all ports. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDot1q_GvrpStatus (void *wap, void *bufp)
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

  /* set the value in application */
  owa.l7rc = usmDbDot1qGvrpStatusSet (L7_UNIT_CURRENT, objGvrpStatusValue);
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
* @function fpObjGet_SwitchingDot1q_NextFreeLocalVlanIndex
*
* @purpose Get 'NextFreeLocalVlanIndex'
*
* @description [NextFreeLocalVlanIndex]: The next available value for dot1qVlanIndex
*              of a local VLAN entry in dot1qVlanStaticTable. This
*              will report values >=4096 if a new Local VLAN may be created
*              or else the value 0 if this is not possible. A row creation
*              operation in this table for an entry with a local VlanIndex
*              value may fail if the current value of this object
*              is not used as the index. Even if the value read is used, there
*              is no guarantee that it will still be the valid index
*              when the create operation is attempted - another manager may
*              have already got in during the intervening time interval.
*              In this case, dot1qNextFreeLocalVlanIndex should be re-read
*              and the creation re-tried with the new value.This value will
*              automatically change when the current value is used to
*              create a new row. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1q_NextFreeLocalVlanIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNextFreeLocalVlanIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1qNextFreeLocalVlanIndexGet (L7_UNIT_CURRENT,
                                         &objNextFreeLocalVlanIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objNextFreeLocalVlanIndexValue,
                     sizeof (objNextFreeLocalVlanIndexValue));

  /* return the object value: NextFreeLocalVlanIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNextFreeLocalVlanIndexValue,
                           sizeof (objNextFreeLocalVlanIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
