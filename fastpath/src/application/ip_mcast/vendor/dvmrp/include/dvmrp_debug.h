/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename dvmrp_debug.h
*
* @purpose Contains structures,constants,macros, for dvmrp protocol
*
* @component
*
* @comments
*
* @create 07/03/2006
*
* @author Prakash/Shashidhar
* @end
*
**********************************************************************/
#ifndef _DVMRP_DEBUG_H_
#define _DVMRP_DEBUG_H_
#include <dvmrp_common.h>
#define MCAST_MAP_MAX_MSG_SIZE 256

typedef struct igmp_group_info_s
{
  L7_uint32      interface_index;        /* Interface Index   */
  L7_uint32      mode;                         /* EXCLUDE or INCLUDE  */
  L7_inet_addr_t group;                 /*  Group address  */
  L7_uint32      no_of_sources;          /*  No of sources  */
  L7_inet_addr_t src[1];               /*  List of sources  */
} igmp_group_info_t;

#define DVMRP_DEBUG_PRINTF sysapiPrintf

/*********************************************************************
* @purpose  This function is used to print the interface table
*
* @param     addrFamily -@b{(input)}  FamilyType
* @returns  None
*
*
* @notes   None
* @end
*********************************************************************/
void dvmrpDebugInterfacesShow(L7_uint32 addrFamily);

/*********************************************************************
* @purpose  This function is used to print the neighbours on an interface
*
* @param     addrFamily -@b{(input)}  FamilyType
*
*                  intfindex -  @b{(input)} Interface for which the user is 
*                                     interested in the neighbours
* @returns  None
*
*
* @notes   None
* @end
*********************************************************************/
void dvmrpDebugNbrShow(L7_uint32 addrFamily,L7_int32 rtrIfNum);

/*********************************************************************
* @purpose  This function is used to print the route table
*
* @param    addrFamily -@b{(input)}  FamilyType
* @returns   None
*
*
* @notes   None
* @end
*********************************************************************/
void dvmrpDebugRoutesShow(L7_uint32 addrFamily);

/*********************************************************************
* @purpose  This function is used to print the prunes for the given netowrk
*
* @param       addrFamily -@b{(input)}  FamilyType
*                    src -  @b{(input)} source network
* 
* @returns  None
*
*
* @notes   None
* @end
*********************************************************************/
void dvmrpDebugPrunesShow(L7_uint32 addrFamily,L7_int32 src);

/*********************************************************************
* @purpose  This function is used to print the grafts sent
*
* @param    addrFamily -@b{(input)}  FamilyType
* @returns  None
*
*
* @notes   None
* @end
*********************************************************************/
void dvmrpDebugGraftsShow(L7_uint32 addrFamily);

/*************************************************************************************
* @purpose  This devshell command is provided to see the Operational state of DVMRP.
*
* @param   addrFamily -@b{(input)}  FamilyType
* @returns  None
*
*
* @notes   None
* @end
*************************************************************************************/
void dvmrpDebugOperStatusShow(L7_uint32 addrFamily);

/*********************************************************************
* @purpose  This function is used to print the Group membership table
*
* @param       addrFamily -@b{(input)}  FamilyType
* 
* @returns  None
*
*
* @notes   None
* @end
*********************************************************************/
void dvmrpDebugMgmdMembershipTableShow(L7_uint32 addrFamily,L7_int32 count);


#endif
