/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename clustering_util.c
*
* @purpose   implementation for clustering support
*
* @component base
*
* @comments none
*
* @create 12/06/2007
*
* @author dcaugherty
*
* @end
*
**********************************************************************/

#include "clustering_util.h"


/*********************************************************************
 *********************************************************************
 * 
 *   Default, bogus, do-nothing implementations of clustering
 *   functions.  (Do you really want prologues for these?)
 * 
 *********************************************************************
 *********************************************************************/

L7_RC_t 
noClusterMemberNotifyRegister(clusterMemberEventCallback_t * ignored)
{
  return L7_FAILURE;
}

L7_RC_t 
noClusterMemberIdentityGet(clusterMemberID_t *ignored,
			   L7_IP_ADDR_t      *unused,
			   L7_in6_addr_t     *unloved)
{
  return L7_FAILURE;
}

L7_uint32 
noClusterMsgMaxSizeGet(clusterMsgDeliveryMethod unused)
{
  return 0;
}

L7_RC_t
noClusterMsgRegister(L7_ushort16 ignored, 
		     clusterMsgDeliveryMethod unused,
		     clusterMsgCallback_t *unwanted)
{
  return L7_FAILURE;
}

L7_RC_t   
noClusterMsgSend(clusterMemberID_t *ignored, 
		 clusterMsgDeliveryMethod unused,
		 L7_ushort16       unwanted, 
		 L7_uint32         unloved, 
                 L7_uchar8 *       abandoned)
{
  return L7_FAILURE;
}

L7_RC_t
noClusterConfigRegister(clusterCfgID ignored,
                        clusterConfigSendCallback_t * unused,
                        clusterConfigRxCallback_t *  unwanted)
{
  return L7_FAILURE;
}


L7_uint32 
noClusterConfigRegistrationsGet(void)
{
  return 0;
}

L7_RC_t
noClusterConfigCallbackGet(clusterCfgID                cfgID,
                           clusterConfigSendCallback_t **sendCB,
                           clusterConfigRxCallback_t   **rxCB)
{
  return L7_FAILURE;
}


L7_RC_t  
noClusterConfigSendDone(clusterCfgID cfgID,
			L7_RC_t ignored)
{
  return L7_FAILURE;
}


L7_RC_t  
noClusterConfigRxDone(clusterCfgID cfgID,
		      L7_RC_t ignored)
{
  return L7_FAILURE;
}

void
noClusterConfigRxApplyDone(clusterCfgID cfgID)
{
  cfgID = cfgID;    /* note to compiler: shut up! */
}
