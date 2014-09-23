
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename    sim_debug_api.h
* @purpose     sim debug functions
* @component   SIM
* @comments    none
* @create      08/22/2006
* @author      ssuvarna
* @end
*             
**********************************************************************/


#include "l7_common.h"

/*********************************************************************
* @purpose  Get the current status of displaying ping packet debug info
*            
* @param    none
*                      
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    
*
* @end
*********************************************************************/
L7_BOOL pingDebugPacketTraceFlagGet();


/*********************************************************************
* @purpose  Turns on/off the displaying of ping packet debug info
*            
* @param    flag         new value of the Packet Debug flag
*                      
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t pingDebugPacketTraceFlagSet(L7_BOOL flag);

/*********************************************************************
*
* @purpose Trace ping packets received
*
* @param   intIfNum	@b{(input)} Internal Interface Number
* @param   *buff	@b{(input)} pointer to packet
*
* @returns void
*
* @notes
*
* @end
*
*********************************************************************/
void pingDebugPacketRxTrace(L7_uint32 intIfNum, L7_uchar8 *buff);

/*********************************************************************
*
* @purpose Trace ping packets received
*
* @param   intIfNum	@b{(input)} Internal Interface Number
* @param   *buff	@b{(input)} pointer to packet
*
* @returns void
*
* @notes
*
* @end
*
*********************************************************************/
void pingDebugPacketTxTrace(L7_uint32 intIfNum, L7_uchar8 *buff);
