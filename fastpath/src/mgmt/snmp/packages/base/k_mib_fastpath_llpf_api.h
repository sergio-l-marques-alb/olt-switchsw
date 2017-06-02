/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2009
*
**********************************************************************
*
* @filename src\mgmt\snmp\packages\base\k_mib_fastpath_llpf_api.h
*
* @purpose  Provide interface to LLPF MIB
*
* @component SNMP
*
* @comments
*
* @create 10/29/2009
*
* @author Vijayanand K(kvijayan)
* @end
*
**********************************************************************/
#ifndef __K_MIB_FASTPATH_LLPF_API_H__
#define __K_MIB_FASTPATH_LLPF_API_H__

#include <k_private_base.h>
#include "llpf_api.h"

/*********************************************************************
*
* @purpose  
*
* @param    
* @param    
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
* @notes
*
* @end
*
*********************************************************************/

L7_RC_t snmpLlpfIntfProtocolModeGet( L7_uint32 intIfNum,
                            L7_LLPF_BLOCK_TYPE_t protocolType, L7_uint32 *blockMode);

/*********************************************************************
*
* @purpose  Set the owner string for the Receiver
*
* @param    
* @param  
* @param    
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
* @notes
*
* @end
*
*********************************************************************/

L7_RC_t snmpLlpfIntfProtocolModeSet( L7_uint32 intIfNum,
                            L7_LLPF_BLOCK_TYPE_t protocolType, L7_uint32 blockMode);

/*********************************************************************
*
* @purpose Used to get the values of protocol index and interface number. 
*
* @param    extIfNum 
* @param    protoIndex
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t snmpLlpfIntfGet(L7_uint32 extIfNum,L7_uint32 protoIndex);
/*********************************************************************
*
* @purpose  
*
* @param  extIfNum
* @param  *nextExtIfNum
* @param  protoIndex
* @param  *nxtProtoIndex
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t  snmpLlpfIntfNextGet(L7_uint32 extIfNum, L7_uint32 *nextExtIfNum,
                    L7_uint32 protoIndex,L7_uint32 *nxtProtoIndex);

#endif /* endof __K_MIB_FASTPATH_LLPF_API_H__ */
