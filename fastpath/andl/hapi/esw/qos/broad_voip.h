/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* Name: broad_voip.h
*
* Purpose: This file contains all the routines for Auto-voip feature
*
* Component: hapi
*
* Comments:
*
* Created by: Murali Krishna Peddireddy  4/07/07
*

*
*
**********************************************************************/
#ifndef INCLUDE_BROAD_VOIP_H
#define INCLUDE_BROAD_VOIP_H

#include "broad_qos_common.h"
/*********************************************************************
*
* @function hapiBroadQosVoipPortInit
*
* @purpose  Per-port AUTO-VOIP init
*
* @param    DAPI_PORT_t* dapiPortPtr - generic port instance
*
* @returns  L7_RC_t
*
* @notes    Invoked once per physical port
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosVoipPortInit(DAPI_PORT_t *dapiPortPtr);
/*********************************************************************
*
* @function hapiBroadVoipPolicyFind
*
* @purpose  To get the free index
*
* @param    L7_uint32  srcIpAddr - source ip address
* @param    L7_uint32  dstIpAddr - destination ip address
* @param    L7_uint32  srcL4Port - source l4 port
* @param    L7_uint32  dstL4Port - destination l4 port
* @param    L7_uint32 *index     - index of entry in the table
* @param    L7_uint32 *voipId    - policy id
* @returns  L7_RC_t
*
*
* @end
*
*********************************************************************/
L7_RC_t  hapiBroadVoipPolicyFind(L7_uint32  srcIpAddr,
                                  L7_uint32  dstIpAddr,
                                  L7_uint32  srcL4Port,
                                  L7_uint32  dstL4Port,
                                  L7_uint32 *index,
                                  L7_uint32 *voipId);
/*********************************************************************
*
* @function hapiBroadVoipPolicyUpdate
*
* @purpose  Update hapi VoIP table with call details
*
* @param    DAPI_QOS_CMD_t *voipCmd   -
* @param    BROAD_POLICY_t voipId     -
* @param    BROAD_POLICY_RULE_t rule1 -
* @param    BROAD_POLICY_RULE_t rule2 -
* @param    L7_uint32 index           -
*
* @returns  L7_RC_t
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadVoipPolicyUpdate(DAPI_QOS_CMD_t *voipCmd, 
                                  BROAD_POLICY_t voipId,
                                  BROAD_POLICY_RULE_t rule1, 
                                  BROAD_POLICY_RULE_t rule2,
                                  L7_uint32 index);
/*********************************************************************
*
* @function hapiBroadQosVoipSession
*
* @purpose  Per-port Per call session Create/Delete
*
* @param    DAPI_USP_t* dapiPortPtr - needs to be a valid usp
* @param    DAPI_CMD_t   cmd        - DAPI_CMD_QOS_VOIP_SESSION
* @param    void         *data      - voipSession
* @param    DAPI_t       *dapi_g    - the driver object
*
*
* @returns  L7_RC_t
*
* @notes
*
* @end
*
*********************************************************************/

L7_RC_t hapiBroadQosVoipSession(DAPI_USP_t *usp, 
                                DAPI_CMD_t cmd, 
                                void *data, 
                                DAPI_t *dapi_g);
/*********************************************************************
*
* @function hapiBroadVoipControlPacketAdd
*
* @purpose  Add policy to copy VOIP control packets to CPU
*
* @param    L7_uint32 protocol
* @param    L7_ushort16 port
*
* @returns  L7_RC_t
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadVoipControlPacketAdd(L7_uint32 protocol,L7_ushort16 port);
/*********************************************************************
*
* @function hapiBroadQosVoipProfile
*
* @purpose
*
* @param    DAPI_USP_t *usp - needs to be a valid usp
* @param    DAPI_CMD_t cmd  - DAPI_CMD_QOS_VOIP_PROFILE
* @param    void *data      - voipProfile
* @param    DAPI_t *dapi_g  - the driver object
*
*
* @returns  L7_RC_t
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosVoipProfile(DAPI_USP_t *usp,
                                DAPI_CMD_t cmd,
                                void *data,
                                DAPI_t *dapi_g);

L7_RC_t hapiBroadQosVoipInit(DAPI_t *dapi_g);
/**********************************************************************
* @function hapiBroadQosVoipStatsGet
*
* @purpose
*
* @param   DAPI_USP_t *usp - needs to be a valid usp
* @param   DAPI_CMD_t cmd  - DAPI_CMD_QOS_VOIP_STATS_GET
* @param   void *data      - voipStats
* @param   DAPI_t *dapi_g  - the driver object
*
* @returns  L7_RC_t
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosVoipStatsGet(DAPI_USP_t *usp,
                                 DAPI_CMD_t cmd,
                                 void *data,
                                 DAPI_t *dapi_g);


/*********************************************************************
*
* @purpose  Notifies the VoIP component that a physical port has been
*           added to a LAG port
*
* @param   *portUsp         @b{(input)} USP of member port
* @param   *lagUsp          @b{(input)} USP of LAG port
* @param   *dapi_g          @b{(input)} The driver object    
*
* @returns  L7_SUCCESS    Function executed normally
* @returns  L7_FAILURE    Function failed
*
* @comments   
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosVoIPPortLagAddNotify(DAPI_USP_t *portUsp, DAPI_USP_t *lagUsp, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose  Notifies the VoIP component that a physical port has been
*           removed from a LAG port
*
* @param   *portUsp         @b{(input)} USP of member port
* @param   *lagUsp          @b{(input)} USP of LAG port
* @param   *dapi_g          @b{(input)} The driver object    
*
* @returns  L7_SUCCESS    Function executed normally
* @returns  L7_FAILURE    Function failed
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosVoIPPortLagDeleteNotify(DAPI_USP_t *portUsp, DAPI_USP_t *lagUsp, DAPI_t *dapi_g);

#endif
