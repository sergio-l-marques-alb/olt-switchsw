/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename     wids_cluster_api.h
*
* @purpose      APIs to other modules for future Cluster Component.
*
* @component    Cluster Functionality
*
* @comments     none
*
* @create       12/06/2007
*
* @author       dcaugherty
*
* @end
*
**********************************************************************/
#ifndef INCLUDE_CLUSTER_UTIL_H
#define INCLUDE_CLUSTER_UTIL_H

#include "datatypes.h"
#include "log.h"
#include "comm_structs.h"
#include "ipv6_commdefs.h"
#include "clustering_commdefs.h"


/*******************************************************************
 *
 * Prototypes for bogus, do-nothing entry points that are only used
 * inside clustering_util.c.  Note how underdocumented they are.
 * That could be a sign that you shouldn't be calling them.
 * 
 ******************************************************************/

L7_RC_t   noClusterMemberNotifyRegister(clusterMemberEventCallback_t);
L7_RC_t   noClusterMemberIdentityGet(clusterMemberID_t *,
				     L7_IP_ADDR_t *,
				     L7_in6_addr_t *);
L7_uint32 noClusterMsgMaxSizeGet(clusterMsgDeliveryMethod);
L7_RC_t   noClusterMsgRegister(L7_ushort16,
			     clusterMsgDeliveryMethod,
			     clusterMsgCallback_t *);
L7_RC_t   noClusterMsgSend(clusterMemberID_t *,
			   clusterMsgDeliveryMethod,
			   L7_ushort16, L7_uint32, L7_uchar8 *);
L7_RC_t   noClusterConfigRegister(clusterCfgID,
				  clusterConfigSendCallback_t *,
				  clusterConfigRxCallback_t *);
L7_uint32 noClusterConfigRegistrationsGet(void);
L7_RC_t   noClusterConfigCallbackGet(clusterCfgID,
				     clusterConfigSendCallback_t **,
				     clusterConfigRxCallback_t **);
L7_RC_t   noClusterConfigSendDone(clusterCfgID, L7_RC_t);
L7_RC_t   noClusterConfigRxDone(clusterCfgID, L7_RC_t);
void      noClusterConfigRxApplyDone(clusterCfgID);

#endif /* INCLUDE_CLUSTER_UTIL_H */
