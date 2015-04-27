/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* Name:       dtl_qos_diffserv.h
*
* Purpose:    Function prototypes for QOS DiffServ DTL commands
*
* Component:  Device Transformation Layer (DTL)
*
* Commnets: 
*
* Created by: gpaussa 08/12/2002 
*
*********************************************************************/
#ifndef INCLUDE_DTL_QOS_DIFFSERV_H
#define INCLUDE_DTL_QOS_DIFFSERV_H 

#include "datatypes.h"
#include "commdefs.h"


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
* @notes    
*
* @end
*********************************************************************/
L7_RC_t dtlQosDiffServPolicyInstanceAdd(L7_uint32 intIfNum, 
                                        L7_BOOL outboundFlag,
                                        void *pTlv, 
                                        L7_uint32 tlvTotalSize);

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
                                           L7_uint32 tlvTotalSize);

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
                                     L7_ulong64 *pValue);

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
                                      L7_ulong64 *pValue);

#endif /* INCLUDE_DTL_QOS_DIFFSERV_H */
