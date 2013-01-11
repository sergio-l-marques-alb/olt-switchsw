/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* Name: dtl_qos_diffserv.c
*
* Purpose: DiffServ TLV and statistics commands to the driver.
*
* Component: Device Transformation Layer (DTL)
*
* Commnets:
*
* Created by: gpaussa 08/12/2002 
*
*********************************************************************/
#include "dtlinclude.h"


/*********************************************************************
* @purpose  Add a DiffServ policy-class instance definition
*
* @param    intIfNum      @b{(input)} Internal interface number
* @param    outboundFlag  @b{(input)} traffic direction indicator
* @param    pTLV          @b{(input)} TLV pointer
* @param    tlvTotalSize  @b{(input)} Size of entire TLV
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The TLV contains the instance key.
*
* @end
*********************************************************************/
L7_RC_t dtlQosDiffServPolicyInstanceAdd(L7_uint32 intIfNum, 
                                        L7_BOOL outboundFlag,
                                        void *pTlv, 
                                        L7_uint32 tlvTotalSize)
{
  nimUSP_t        nimUsp;
  DAPI_USP_t      ddUsp;
  DAPI_QOS_CMD_t  dapiCmd;
  L7_RC_t   rc;

  if (nimGetUnitSlotPort(intIfNum, &nimUsp) != L7_SUCCESS)
    return L7_FAILURE;
  
  ddUsp.unit = nimUsp.unit;
  ddUsp.slot = nimUsp.slot;  
  ddUsp.port = nimUsp.port-1;
  
  dapiCmd.cmdData.diffServInstAdd.getOrSet = DAPI_CMD_SET;
  
  if (outboundFlag != L7_TRUE)
    dapiCmd.cmdData.diffServInstAdd.ifDirection = DAPI_QOS_INTF_DIR_IN;
  else
    dapiCmd.cmdData.diffServInstAdd.ifDirection = DAPI_QOS_INTF_DIR_OUT;

  dapiCmd.cmdData.diffServInstAdd.pTLV = pTlv;
  dapiCmd.cmdData.diffServInstAdd.tlvTotalSize = tlvTotalSize;

  rc = dapiCtl(&ddUsp, DAPI_CMD_QOS_DIFFSERV_INST_ADD, &dapiCmd);
  if ((rc == L7_SUCCESS) || (rc == L7_ASYNCH_RESPONSE))
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Delete one or more DiffServ policy-class instance definitions
*
* @param    intIfNum      @b{(input)} Internal interface number
* @param    outboundFlag  @b{(input)} traffic direction indicator
* @param    pTLV          @b{(input)} TLV pointer
* @param    tlvTotalSize  @b{(input)} Size of entire TLV
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The TLV contains a list of instance keys.
*
* @end
*********************************************************************/
L7_RC_t dtlQosDiffServPolicyInstanceDelete(L7_uint32 intIfNum, 
                                           L7_BOOL outboundFlag,
                                           void *pTlv, 
                                           L7_uint32 tlvTotalSize)
{
  nimUSP_t        nimUsp;
  DAPI_USP_t      ddUsp;
  DAPI_QOS_CMD_t  dapiCmd;
  L7_RC_t   rc;

  if (nimGetUnitSlotPort(intIfNum, &nimUsp) != L7_SUCCESS)
    return L7_FAILURE;
  
  ddUsp.unit = nimUsp.unit;
  ddUsp.slot = nimUsp.slot;  
  ddUsp.port = nimUsp.port-1;
  
  dapiCmd.cmdData.diffServInstDelete.getOrSet = DAPI_CMD_SET;
  
  if (outboundFlag != L7_TRUE)
    dapiCmd.cmdData.diffServInstDelete.ifDirection = DAPI_QOS_INTF_DIR_IN;
  else
    dapiCmd.cmdData.diffServInstDelete.ifDirection = DAPI_QOS_INTF_DIR_OUT;

  dapiCmd.cmdData.diffServInstDelete.pTLV = pTlv;
  dapiCmd.cmdData.diffServInstDelete.tlvTotalSize = tlvTotalSize;

  rc = dapiCtl(&ddUsp, DAPI_CMD_QOS_DIFFSERV_INST_DELETE, &dapiCmd);
  if ((rc == L7_SUCCESS) || (rc == L7_ASYNCH_RESPONSE))
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get an inbound DiffServ statistic from the device driver
*
* @param    intIfNum      @b{(input)}  Internal interface number
* @param    instanceKey   @b{(input)}  Key identifying an instance on this intf
* @param    ctrId         @b{(input)}  System counter ID
* @param    pValue        @b{(output)} Pointer to 64-bit counter value output
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  Counter not supported on this platform
* @returns  L7_ERROR          Counter not defined for this traffic direction
* @returns  L7_FAILURE
*
* @notes    The instance key is one that was previously issued to the 
*           device driver when a policy instance TLV was added to the 
*           interface.  The same instance key can be used for multiple
*           interfaces, so the key combined with the intIfNum uniquely
*           identifies the driver counter.
*
*           The ctrId passed into this function is the one defined for
*           the system (i.e., from platform_counters.h).  This is then
*           translated to the corresponding DAPI counter id.  
*
* @end
*********************************************************************/
L7_RC_t dtlQosDiffServDrvrStatsInGet(L7_uint32 intIfNum, 
                                     L7_uint32 instanceKey,
                                     L7_uint32 ctrId,
                                     L7_ulong64 *pValue)
{
  nimUSP_t        nimUsp;
  DAPI_USP_t      ddUsp;
  L7_uint32       ddCtrId;
  DAPI_QOS_CMD_t  dapiCmd;
  L7_RC_t   rc;

  /* initialize output counter value */
  pValue->high = pValue->low = 0;

  if (nimGetUnitSlotPort(intIfNum, &nimUsp) != L7_SUCCESS)
    return L7_FAILURE;
  
  ddUsp.unit = nimUsp.unit;
  ddUsp.slot = nimUsp.slot;  
  ddUsp.port = nimUsp.port-1;
  
  /* identify the corresponding driver counter name
   *
   * NOTE: Cannot use 'switch' statement here, since all unsupported
   *       counters have the same ID value (L7_PLATFORM_CTR_NOT_SUPPORTED).
   *
   * NOTE: MUST check for L7_PLATFORM_CTR_NOT_SUPPORTED first.
   */
  if (ctrId == L7_PLATFORM_CTR_NOT_SUPPORTED)
    return L7_NOT_SUPPORTED;            /* ctr not supported on this platform */

  else if (ctrId ==  L7_PLATFORM_CTR_DIFFSERV_POLICY_PERF_IN_OFFERED_OCTETS)
    ddCtrId = DAPI_QOS_DIFFSERV_STAT_IN_OFFERED_OCTETS;

  else if (ctrId == L7_PLATFORM_CTR_DIFFSERV_POLICY_PERF_IN_OFFERED_PACKETS)
    ddCtrId = DAPI_QOS_DIFFSERV_STAT_IN_OFFERED_PACKETS;

  else if (ctrId == L7_PLATFORM_CTR_DIFFSERV_POLICY_PERF_IN_DISCARDED_OCTETS)
    ddCtrId = DAPI_QOS_DIFFSERV_STAT_IN_DISCARDED_OCTETS;

  else if (ctrId == L7_PLATFORM_CTR_DIFFSERV_POLICY_PERF_IN_DISCARDED_PACKETS)
    ddCtrId = DAPI_QOS_DIFFSERV_STAT_IN_DISCARDED_PACKETS;

  else
    return L7_ERROR;                    /* ctr not defined for this direction */

  dapiCmd.cmdData.diffServIntfStatInGet.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.diffServIntfStatInGet.instanceKey = instanceKey;
  dapiCmd.cmdData.diffServIntfStatInGet.statistic = ddCtrId;
  dapiCmd.cmdData.diffServIntfStatInGet.value = pValue;
  
  rc = dapiCtl(&ddUsp, DAPI_CMD_QOS_DIFFSERV_INTF_STAT_IN_GET, &dapiCmd);
  if ((rc == L7_SUCCESS) || (rc == L7_ASYNCH_RESPONSE))
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get an outbound DiffServ statistic from the device driver
*
* @param    intIfNum      @b{(input)}  Internal interface number
* @param    instanceKey   @b{(input)}  Key identifying an instance on this intf
* @param    ctrId         @b{(input)}  System counter ID
* @param    pValue        @b{(output)} Pointer to 64-bit counter value output
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  Counter not supported on this platform
* @returns  L7_ERROR          Counter not defined for this traffic direction
* @returns  L7_FAILURE
*
* @notes    The instance key is one that was previously issued to the 
*           device driver when a policy instance TLV was added to the 
*           interface.  The same instance key can be used for multiple
*           interfaces, so the key combined with the intIfNum uniquely
*           identifies the driver counter.
*
*           The ctrId passed into this function is the one defined for
*           the system (i.e., from platform_counters.h).  This is then
*           translated to the corresponding DAPI counter id.  
*
* @end
*********************************************************************/
L7_RC_t dtlQosDiffServDrvrStatsOutGet(L7_uint32 intIfNum, 
                                      L7_uint32 instanceKey,
                                      L7_uint32 ctrId,
                                      L7_ulong64 *pValue)
{
  nimUSP_t        nimUsp;
  DAPI_USP_t      ddUsp;
  L7_uint32       ddCtrId;
  DAPI_QOS_CMD_t  dapiCmd;
  L7_RC_t   rc;

  /* initialize output counter value */
  pValue->high = pValue->low = 0;

  if (nimGetUnitSlotPort(intIfNum, &nimUsp) != L7_SUCCESS)
    return L7_FAILURE;
  
  ddUsp.unit = nimUsp.unit;
  ddUsp.slot = nimUsp.slot;  
  ddUsp.port = nimUsp.port-1;
  
  /* identify the corresponding driver counter name
   *
   * NOTE: Cannot use 'switch' statement here, since all unsupported
   *       counters have the same ID value (L7_PLATFORM_CTR_NOT_SUPPORTED).
   *
   * NOTE: MUST check for L7_PLATFORM_CTR_NOT_SUPPORTED first.
   */
  if (ctrId == L7_PLATFORM_CTR_NOT_SUPPORTED)
    return L7_NOT_SUPPORTED;            /* ctr not supported on this platform */

  else if (ctrId ==  L7_PLATFORM_CTR_DIFFSERV_POLICY_PERF_OUT_OFFERED_OCTETS)
    ddCtrId = DAPI_QOS_DIFFSERV_STAT_OUT_OFFERED_OCTETS;

  else if (ctrId == L7_PLATFORM_CTR_DIFFSERV_POLICY_PERF_OUT_OFFERED_PACKETS)
    ddCtrId = DAPI_QOS_DIFFSERV_STAT_OUT_OFFERED_PACKETS;

  else if (ctrId == L7_PLATFORM_CTR_DIFFSERV_POLICY_PERF_OUT_DISCARDED_OCTETS)
    ddCtrId = DAPI_QOS_DIFFSERV_STAT_OUT_DISCARDED_OCTETS;

  else if (ctrId == L7_PLATFORM_CTR_DIFFSERV_POLICY_PERF_OUT_DISCARDED_PACKETS)
    ddCtrId = DAPI_QOS_DIFFSERV_STAT_OUT_DISCARDED_PACKETS;

  else
    return L7_ERROR;                    /* ctr not defined for this direction */

  dapiCmd.cmdData.diffServIntfStatOutGet.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.diffServIntfStatOutGet.instanceKey = instanceKey;
  dapiCmd.cmdData.diffServIntfStatOutGet.statistic = ddCtrId;
  dapiCmd.cmdData.diffServIntfStatOutGet.value = pValue;
  
  rc = dapiCtl(&ddUsp, DAPI_CMD_QOS_DIFFSERV_INTF_STAT_OUT_GET, &dapiCmd);
  if ((rc == L7_SUCCESS) || (rc == L7_ASYNCH_RESPONSE))
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

