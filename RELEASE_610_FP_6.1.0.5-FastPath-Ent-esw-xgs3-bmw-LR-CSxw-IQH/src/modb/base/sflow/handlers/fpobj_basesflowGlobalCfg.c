
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_basesflowGlobalCfg.c
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
#include "_xe_basesflowGlobalCfg_obj.h"
#include "usmdb_sflow.h"
#include "usmdb_util_api.h"

/*******************************************************************************
* @function fpObjGet_basesflowGlobalCfg_Version
*
* @purpose Get 'Version'
 *@description  [Version] Uniquely identifies the version and implementation of
* this MIB.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesflowGlobalCfg_Version (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objVersionValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmdbsFlowAgentVersionGet (L7_UNIT_CURRENT, objVersionValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objVersionValue, strlen (objVersionValue));

  /* return the object value: Version */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objVersionValue, strlen (objVersionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basesflowGlobalCfg_AgentAddressType
*
* @purpose Get 'AgentAddressType'
 *@description  [AgentAddressType] The address type of the address associated
* with this agent.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesflowGlobalCfg_AgentAddressType (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAgentAddressTypeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmdbsFlowAgentAddressTypeGet (L7_UNIT_CURRENT, &objAgentAddressTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objAgentAddressTypeValue, sizeof (objAgentAddressTypeValue));

  /* return the object value: AgentAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAgentAddressTypeValue,
                           sizeof (objAgentAddressTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basesflowGlobalCfg_AgentAddress
*
* @purpose Get 'AgentAddress'
 *@description  [AgentAddress] The IP address associated with this agent.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesflowGlobalCfg_AgentAddress (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t objAgentAddressValue;

  xLibStr256_t objAgentAddressStrValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmdbsFlowAgentAddressGet (L7_UNIT_CURRENT, objAgentAddressStrValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.l7rc = usmDbParseInetAddrFromStr (objAgentAddressStrValue, &objAgentAddressValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objAgentAddressValue, strlen (objAgentAddressValue));

  /* return the object value: AgentAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objAgentAddressValue, sizeof (objAgentAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
