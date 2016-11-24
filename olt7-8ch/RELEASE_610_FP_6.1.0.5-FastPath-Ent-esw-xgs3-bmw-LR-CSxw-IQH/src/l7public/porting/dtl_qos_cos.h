/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   dtl_qos_cos.h
*
* @purpose    COS component DTL API function prototypes
*
* @component  DTL
*
* @comments   none
*
* @create     03/16/2004
*
* @author     gpaussa
* @end
*
**********************************************************************/
#ifndef INCLUDE_DTL_QOS_COS_H
#define INCLUDE_DTL_QOS_COS_H 

#include "l7_common.h"
#include "l7_cos_api.h"


typedef struct
{
  /* see DAPI_QOS_COS_MAP_TABLE_t note in dapi.h for usage details */
  L7_uchar8     dot1pTrafficClass[L7_DOT1P_MAX_PRIORITY+1];     /* used for all modes               */
  L7_uchar8     ipPrecTrafficClass[L7_QOS_COS_MAP_NUM_IPPREC];  /* used for trust IP Prec mode only */
  L7_uchar8     ipDscpTrafficClass[L7_QOS_COS_MAP_NUM_IPDSCP];  /* used for trust IP DSCP mode only */

} DTL_QOS_COS_MAP_TABLE_t;


/*************************************************************************
* @purpose  Set the assigned traffic class (queue) for this IP precedence
*
* @param    intIfNum      @b{(input)}  Internal interface number     
* @param    precedence    @b{(input)}  IP precedence     
* @param    trafficClass  @b{(input)}  Traffic class value    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This should only be called when the interface is in 
*           trust IP precedence mode.
*
* @end
*********************************************************************/
L7_RC_t dtlQosCosMapIpPrecTrafficClassSet(L7_uint32 intIfNum, 
                                          L7_uint32 precedence,
                                          L7_uint32 trafficClass);

/*************************************************************************
* @purpose  Set the assigned traffic class (queue) for this IP DSCP
*
* @param    intIfNum      @b{(input)}  Internal interface number     
* @param    dscp          @b{(input)}  IP DSCP           
* @param    trafficClass  @b{(input)}  Traffic class value    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This should only be called when the interface is in 
*           trust IP DSCP mode.
*
* @end
*********************************************************************/
L7_RC_t dtlQosCosMapIpDscpTrafficClassSet(L7_uint32 intIfNum, 
                                          L7_uint32 dscp,
                                          L7_uint32 trafficClass);

/*************************************************************************
* @purpose  Set the COS trust mode for this interface
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    trustMode   @b{(input)}  Trust mode value    
* @param    pMapTable   @b{(input)}  Ptr to COS mapping table
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Sets the trust mode and supplies the contents of the active
*           COS map table.
*
* @end
*********************************************************************/
L7_RC_t dtlQosCosMapIntfTrustModeSet(L7_uint32 intIfNum, 
                                     L7_QOS_COS_MAP_INTF_MODE_t trustMode,
                                     DTL_QOS_COS_MAP_TABLE_t *pMapTable);

/*************************************************************************
* @purpose  Set the COS interface config parameters
*
* @param    intIfNum        @b{(input)}  Internal interface number     
* @param    intfShapingRate @b{(input)}  Interface shaping rate
* @param    qMgmtTypeIntf   @b{(input)}  Queue mgmt type (per-interface)
* @param    wredDecayExp    @b{(input)}  WRED decay exponent   
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The qMgmtTypeIntf value is set to 'reserved' if the device
*           supports per-queue management type configuration instead, 
*           which is set with the other queue parameters.
*
* @end
*********************************************************************/
L7_RC_t dtlQosCosIntfConfigSet(L7_uint32 intIfNum, 
                               L7_uint32 intfShapingRate,
                               L7_QOS_COS_QUEUE_MGMT_TYPE_t qMgmtTypeIntf,
                               L7_uint32 wredDecayExp);

/*************************************************************************
* @purpose  Get the COS interface config parameters
*
* @param    intIfNum             @b{(input)}  Internal interface number     
* @param    intfShapingRate      @b{(input)}  Interface shaping rate in kbps
* @param    intfShapingBurstSize @b{(input)}  Interface shaping burst size in kbits
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t dtlQosCosIntfStatusGet(L7_uint32 intIfNum, 
                               L7_uint32 *intfShapingRate,
                               L7_uint32 *intfShapingBurstSize);

/*************************************************************************
* @purpose  Set the COS queue scheduler parameters for the interface
*
* @param    intIfNum        @b{(input)}  Internal interface number     
* @param    *pMinBwList     @b{(input)}  Ptr to minimum bandwidth parm list
* @param    *pMaxBwList     @b{(input)}  Ptr to maximum bandwidth parm list
* @param    *pSchedTypeList @b{(input)}  Ptr to scheduler type list   
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Each of the 'list' style parameters contains a separate
*           value for each supported COS queue.
*
* @end
*********************************************************************/
L7_RC_t dtlQosCosQueueSchedConfigSet(L7_uint32 intIfNum, 
                                     L7_qosCosQueueBwList_t *pMinBwList,
                                     L7_qosCosQueueBwList_t *pMaxBwList,
                                     L7_qosCosQueueSchedTypeList_t *pSchedTypeList);

/*************************************************************************
* @purpose  Set the COS drop config (taildrop or WRED) parameters
*           on this interface
*
* @param    intIfNum      @b{(input)}  Internal interface number     
* @param    *pDropParms   @b{(input)}  Ptr to drop parms list
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Each of the 'list' style parameters contains a separate
*           value for each supported drop precedence level.
*
* @end
*********************************************************************/
L7_RC_t dtlQosCosQueueDropConfigSet(L7_uint32 intIfNum, 
                                    L7_qosCosDropParmsList_t *pDropParms);

#endif /* INCLUDE_DTL_QOS_COS_H */
