/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename nim.h
*
* @purpose Network Interface Manager Constants and Data Structures
*
* @component nim
*
* @comments none
*
* @create 06/26/2000
*
* @author wjacobs
* @end
*
**********************************************************************/
/*********************************************************************
 *                   
 ********************************************************************/

#ifndef INCLUDE_NIM
#define INCLUDE_NIM

#include "default_cnfgr.h"
#include "statsapi.h"

/*---------------------------------------------------------------------------*/
/* Constants dependent upon other definitions for port mapping */
/*---------------------------------------------------------------------------*/

/****************************************************************************************
 * Interfaces which can not be in L7_MAX_INTF_MASK must be at the higest end of the list.
 * Reference comments for L7_MAX_INTF_MASK.
 ****************************************************************************************/

    

#define L7_NIM_UNUSED_PARAMETER   0

/****************************************
*
*  NIM Constants                    
*
*****************************************/

/****************************************************************************************
*  Calculate the array size of L7_uint32 for a bit 
*  array for each port that acts as a network interface.
*  That is, each port that can be in a forwarding or not
*  forwarding state. This does not include:
* 
*   CPU ports, service ports 
 ****************************************************************************************/
#define L7_MAX_INTF_MASK   (L7_MAX_INTERFACE_COUNT/32 + 1)

#endif /* INCLUDE_NIM */

