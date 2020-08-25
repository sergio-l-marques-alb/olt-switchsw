/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  mgmd_mrp.c
*
* @purpose   Routines for MGMD <-> MRP interactions
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
#include "mgmd_mrp.h"
#include "l7_mgmdmap_include.h"

/*********************************************************************
* @purpose  Add source information in mgmdGroupInfo to be sent to MRPs
*
* @param    group_info  @b{ (output) } group in which source is to be added
* @param    sourceAddr  @b{ (input) }  address of the source to be added
* @param    mode        @b{ (input) }  Mode for the source (INCLUDE / EXCLUDE)
* @param    action      @b{ (input) }  Action to be taken for the source (ADD / DELETE)
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void mgmd_src_info_for_mrp_add(mgmdGroupInfo_t *group_info, L7_inet_addr_t sourceAddr,
                        L7_uchar8 mode, L7_uchar8 action)
{
  L7_uint32 numSrcs = group_info->numSrcs;
  mrp_source_record_t *mgmdMrpSrcRec = L7_NULLPTR;

  if (numSrcs >= MGMD_MAX_QUERY_SOURCES)
    return;

  /* Allocate for the Inner sourceList Pointers */
  if (bufferPoolAllocate (mcastMgmdEventsBufferPoolIdGet(sourceAddr.family),
                          (L7_uchar8**) &mgmdMrpSrcRec)
                       != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "MCAST MGMD Event Inner Node Alloc Failed");
    return;
  }

  inetCopy (&(mgmdMrpSrcRec->sourceAddr), &sourceAddr);
  mgmdMrpSrcRec->filterMode = mode;
  mgmdMrpSrcRec->action = action;

  group_info->sourceList[numSrcs] = mgmdMrpSrcRec;
  group_info->numSrcs++;
}

/*********************************************************************
* @purpose  Routine to frame GroupInfo and Notify MRP protocols of the 
*           Membership events.
*
* @param    mgmdCB         @b{ (input) }  MGMD control block
* @param    event          @b{ (input) }  event type.
* @param    intfNum        @b{ (input) }  interface number
* @param    group          @b{ (input) }  group IP Address
* @param    group_info     @b{ (input) }  pointer to intermodule data-structure
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void mgmd_frame_event_info_and_notify(mgmd_cb_t *mgmdCB, 
                                             mgmMrtEvents_t event, L7_uint32 intfNum, 
                                             mgmd_group_t *mgmd_group, mgmdGroupInfo_t *group_info)
{
  /* Update group_info fields */                               
  group_info->rtrIfNum = intfNum;
  group_info->mode = mgmd_group->filterMode;
  group_info->group = mgmd_group->group;

  MGMD_DEBUG(MGMD_DEBUG_EVENTS," Entered ");

  if((group_info->numSrcs == 0)&&(!((mgmd_group->mrpUpdated & MGMD_NEW_GROUP)||
       (mgmd_group->mrpUpdated & MGMD_GROUP_MODE_UPDATED)||
       (mgmd_group->mrpUpdated & MGMD_MRP_REQUESTED_GROUP)||
       (mgmd_group->mrpUpdated & MGMD_MRP_INFORM_FAILED))))
  {
    MGMD_DEBUG_ADDR (MGMD_DEBUG_EVENTS, "group already notified to MRP ",&mgmd_group->group);
    return;    
  }  
  if(mgmd_group->mrpUpdated & MGMD_NEW_GROUP)
  {
    MGMD_DEBUG_ADDR (MGMD_DEBUG_EVENTS,
                "New group created and an event being sent to MRP\n",&mgmd_group->group);
  }
  else if(mgmd_group->mrpUpdated & MGMD_GROUP_MODE_UPDATED)
  {
    MGMD_DEBUG(MGMD_DEBUG_EVENTS,
               "group mode is changed and an event being sent to MRP, \n",&mgmd_group->group);
  }
  else if(mgmd_group->mrpUpdated & MGMD_MRP_REQUESTED_GROUP)
  {
    MGMD_DEBUG (MGMD_DEBUG_EVENTS, "Group %s requested by the MRP and an \
                        event being sent to MRP \n",&mgmd_group->group);
  }
  
  if(mgmdMapProtocolMRPMembershipEventsNotify(mgmdCB->proto, event, group_info) == L7_SUCCESS)
  {
  /*reset both flags , as its updated to MRP */
  mgmd_group->mrpUpdated &= ~MGMD_NEW_GROUP;
  mgmd_group->mrpUpdated &= ~MGMD_GROUP_MODE_UPDATED;
  mgmd_group->mrpUpdated &= ~MGMD_MRP_REQUESTED_GROUP;
    mgmd_group->mrpUpdated &= ~MGMD_MRP_INFORM_FAILED;    
  }
  else
  {
    mgmd_group->mrpUpdated |= MGMD_MRP_INFORM_FAILED;
    if (mgmdUtilAppTimerSet(mgmdCB, L7_NULL, (void *)mgmdCB->mgmd_mrp_timer_handle,
                            &mgmdCB->mgmd_mrp_timer, MGMD_MRP_INFORM_TIMEOUT,
                            L7_MGMD_MRP_INFORM_TIMER) != L7_SUCCESS)
    {
      MGMD_DEBUG (MGMD_DEBUG_APIS, "Could not Start the L7_MGMD_MRP_INFORM_TIMER \n"); 
      return;
    }
  }
}


