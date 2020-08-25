
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_IPMCASTagentSnmpTrapFlagsConfigGroupMulticast.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to IPMCAST-object.xml
*
* @create  02 May 2008, Friday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_IPMCASTagentSnmpTrapFlagsConfigGroupMulticast_obj.h"
#include "usmdb_mib_dvmrp_api.h"
#include "usmdb_mib_pim_api.h"


/*******************************************************************************
* @function fpObjGet_IPMCASTagentSnmpTrapFlagsConfigGroupMulticast_agentSnmpDVMRPTrapFlag
*
* @purpose Get 'agentSnmpDVMRPTrapFlag'
*
* @description [agentSnmpDVMRPTrapFlag] This enables or disables DVMRP Traps on the system.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTagentSnmpTrapFlagsConfigGroupMulticast_agentSnmpDVMRPTrapFlag (void *wap,
                                                                                        void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objagentSnmpDVMRPTrapFlagValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbDvmrpTrapModeGet (L7_UNIT_CURRENT, L7_DVMRP_TRAP_ALL, &objagentSnmpDVMRPTrapFlagValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objagentSnmpDVMRPTrapFlagValue,
                     sizeof (objagentSnmpDVMRPTrapFlagValue));

  /* return the object value: agentSnmpDVMRPTrapFlag */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objagentSnmpDVMRPTrapFlagValue,
                           sizeof (objagentSnmpDVMRPTrapFlagValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_IPMCASTagentSnmpTrapFlagsConfigGroupMulticast_agentSnmpDVMRPTrapFlag
*
* @purpose Set 'agentSnmpDVMRPTrapFlag'
*
* @description [agentSnmpDVMRPTrapFlag] This enables or disables DVMRP Traps on the system.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_IPMCASTagentSnmpTrapFlagsConfigGroupMulticast_agentSnmpDVMRPTrapFlag (void *wap,
                                                                                        void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objagentSnmpDVMRPTrapFlagValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: agentSnmpDVMRPTrapFlag */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objagentSnmpDVMRPTrapFlagValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objagentSnmpDVMRPTrapFlagValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbDvmrpTrapModeSet(L7_UNIT_CURRENT, objagentSnmpDVMRPTrapFlagValue, L7_DVMRP_TRAP_ALL );
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_IPMCASTagentSnmpTrapFlagsConfigGroupMulticast_agentSnmpPIMTrapFlag
*
* @purpose Get 'agentSnmpPIMTrapFlag'
*
* @description [agentSnmpPIMTrapFlag] This enables or disables PIM Traps on the system.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTagentSnmpTrapFlagsConfigGroupMulticast_agentSnmpPIMTrapFlag (void *wap,
                                                                                      void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objagentSnmpPIMTrapFlagValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbPimTrapModeGet(L7_PIM_TRAP_ALL, &objagentSnmpPIMTrapFlagValue); 
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objagentSnmpPIMTrapFlagValue, sizeof (objagentSnmpPIMTrapFlagValue));

  /* return the object value: agentSnmpPIMTrapFlag */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objagentSnmpPIMTrapFlagValue,
                           sizeof (objagentSnmpPIMTrapFlagValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_IPMCASTagentSnmpTrapFlagsConfigGroupMulticast_agentSnmpPIMTrapFlag
*
* @purpose Set 'agentSnmpPIMTrapFlag'
*
* @description [agentSnmpPIMTrapFlag] This enables or disables PIM Traps on the system.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_IPMCASTagentSnmpTrapFlagsConfigGroupMulticast_agentSnmpPIMTrapFlag (void *wap,
                                                                                      void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objagentSnmpPIMTrapFlagValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: agentSnmpPIMTrapFlag */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objagentSnmpPIMTrapFlagValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objagentSnmpPIMTrapFlagValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbPimTrapModeSet (L7_UNIT_CURRENT, objagentSnmpPIMTrapFlagValue, L7_PIM_TRAP_ALL) ;
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
