/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename dvmrp_mfc.h
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
#ifndef _DVMRP_IGMP_H_
#define _DVMRP_IGMP_H_
#include <dvmrp_common.h>

struct igmp
{
  u_char   igmp_type;             /* IGMP type */
  u_char   igmp_code;             /* routing code */
  u_short  igmp_cksum;           /* checksum */
  L7_in_addr_t igmp_group;      /* group address */
};

/*********************************************************************
* @purpose  This function is used to update the cache table when IGMP group 
*           membership info is recieved
*           
* @param       dvmrpcb   - @b{(input)} Pointer to the dvmrp control block 
*                    grp_info   -  @b{(input)} Pointer toIGMP membership info
*
* @returns  None
*
* @notes    None
* @end
*********************************************************************/
void dvmrpCacheTableUpdateForGrpUpdateEvent(dvmrp_t *dvmrpcb, mgmdMrpEventInfo_t *grp_info);
/*********************************************************************
* @purpose  This function is to test the igmp group membership
*
* @param     dvmrpcb   - @b{(input)} Pointer to the dvmrp control block 
* @param     group  -  @b{(input)}  group address
* @param     source -  @b{(input)}  source address
* @param     index  -  @b{(input)}  interfac index
* 
* @returns    L7_SUCCESS/L7_FAILURE
*
* @notes   None
* @end
*********************************************************************/
L7_RC_t  dvmrpMemberShipTest(dvmrp_t *dvmrpcb, L7_inet_addr_t *group, L7_inet_addr_t *source, 
                             L7_int32 index);

#endif
