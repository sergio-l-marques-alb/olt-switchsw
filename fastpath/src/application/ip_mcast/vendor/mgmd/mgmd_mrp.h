/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  mgmd_mrp.h
*
* @purpose   Defintions for MGMD <-> MRP interactions
*
* @component MGMD (IGMP+MLD)
*
* @comments  none
*
* @create    Feb 08, 2007
*
* @author    akamlesh
*
* @end
*
**********************************************************************/
#include "mgmd.h"

void mgmd_src_info_for_mrp_add(mgmdGroupInfo_t *group_info, L7_inet_addr_t sourceAddr,
                               L7_uchar8 mode, L7_uchar8 action);

void mgmd_frame_event_info_and_notify (mgmd_cb_t *mgmdCB,  
                                       mgmMrtEvents_t event, L7_uint32 intfNum, 
                                       mgmd_group_t *mgmd_group,mgmdGroupInfo_t *group_info);



