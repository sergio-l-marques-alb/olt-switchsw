/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename sflow_outcalls.h
*
* @purpose   Prototypes of sFlow outcalls
*
* @component sflow
*
* @comments  
*
* @create 20-Nov-2007
*
* @author drajendra
* @end
*
**********************************************************************/
#ifndef SFLOW_OUTCALLS_H
#define SFLOW_OUTCALLS_H
/*********************************************************************
* @purpose  Determine if the interface type is valid for Snooping
*
* @param    sysIntfType  @b{(input)} interface type
*
* @returns  L7_TRUE   Valid interface type
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL sFlowIsValidIntfType(L7_uint32 sysIntfType);
/*********************************************************************
* @purpose  Determine if the interface is valid for Snooping
*
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_TRUE   Supported interface
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL sFlowIsValidIntf(L7_uint32 intIfNum);
/*********************************************************************
* @purpose  Checks if the interface/VLAN ID can be enabled for sFlow
*
* @param    dsIndex  @b{(input)} Data source
*
* @returns  L7_TRUE, if datasource can be enabled
* @returns  L7_FALSE, if datasource cannot be enabled
*
* @notes    Acceptability criteria
*              1. If data source is interface, it must be in attached 
*                 state
*
*
* @end
*********************************************************************/
L7_BOOL sFlowIsDataSourceReady(L7_uint32 dsIndex);
/*********************************************************************
* @purpose  Get the requested counter from stats collector
*
* @param     intIfNum    @b{(input)}  internal interface number
* @param     counterType @b{(input)}  enum specifying the counter
*                                     SFLOW_ETH_COUNTERS_t
*                                     SFLOW_IF_COUNTERS_t
* @param    *val         @b{(output)} ptr to 32bit counter where the
*                                     value will be saved
* @param    *longval     @b{(output)} ptr to 64bit counter where the
*                                     value will be saved
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
void sFlowCounterValGet(L7_uint32 intIfNum, L7_uint32 counterType,
                        L7_uint32 *val, L7_uint64 *longval);
/********************************************************************* 
* @purpose  This function is used to get the Agent Address . 
* 
* @param    strIpAddr   Agent Address in Printable format 
* 
* 
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE 
* 
* @notes    none 
* 
* @end 
*********************************************************************/
L7_RC_t sFlowAgentAddrGet(L7_inet_addr_t *strIpAddr);
#endif /* SFLOW_OUTCALLS_H */
