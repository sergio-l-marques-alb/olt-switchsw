/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_cos.h
*
* @purpose   This file contains all the routines for Class of Service
*
* @component hapi-broad
*
* @comments
*
* @create    7/24/02
*
* @author    colinw
*
* @end
*
**********************************************************************/
#ifndef INCLUDE_BROAD_COS_H
#define INCLUDE_BROAD_COS_H

#include "broad_qos_common.h"

/*********************************************************************
*
* @purpose  Determines that minimum weight that can be programmed
*           into HW based on the chip type.
*
* @returns  Minimum HW queue weight
*
* @comments For chips that support WDRR mode, we convert the weight
*           to kbytes by multiplying the min HW weight by the
*           MTU quanta. 
*
* @end
*
*********************************************************************/
L7_uint32 hapiBroadCosQueueWeightMin();

/*********************************************************************
*
* @purpose  Determines that maximum weight that can be programmed
*           into HW based on the chip type.
*
* @returns  Maximum HW queue weight
*
* @comments For chips that support WDRR mode, we convert the weight
*           to kbytes by multiplying the max HW weight by the
*           MTU quanta. 
*
* @end
*
*********************************************************************/
L7_uint32 hapiBroadCosQueueWeightMax();

/*********************************************************************
*
* @purpose  Determines if weighted deficit round robin (WDRR)
*           is supported based on the chip type.
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_BOOL hapiBroadCosQueueWDRRSupported();

/*********************************************************************
*
* @purpose  Initialize COS component of QOS package
*
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
extern L7_RC_t hapiBroadQosCosInit(DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose  Per-port COS init
*
* @param   *dapiPortPtr     @b{(input)} Generic port instance
*
* @returns  L7_SUCCESS    Function executed normally
* @returns  L7_FAILURE    Function failed
*
* @comments    Invoked once per physical port
*
* @end
*
*********************************************************************/
extern L7_RC_t hapiBroadQosCosPortInit(DAPI_PORT_t *dapiPortPtr);

/*********************************************************************
*
* @purpose  This command maps an individual IP Precedence value to one 
*           of the port's available traffic classes (transmit queues).
*
* @param   *usp     @b{(input)} Needs to be a valid usp
* @param    cmd     @b{(input)} DAPI_CMD_QOS_COS_IP_PRECEDENCE_TO_TC_MAP
* @param   *data    @b{(input)} DAPI_QOS_CMD_t.cmdData.ipPrecedenceToTcMap
* @param   *dapi_g  @b{(input)} The driver object    
*
* @returns  L7_SUCCESS    Function executed normally
* @returns  L7_FAILURE    Function failed
*
* @comments This table is used when port trust mode is set to 
*           DAPI_QOS_COS_INTF_MODE_TRUST_IPPREC.
*
* @end
*
*********************************************************************/
/* DAPI_CMD_QOS_COS_IP_PRECEDENCE_TO_TC_MAP */
extern L7_RC_t hapiBroadQosCosIpPrecedenceToTcMap(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose  This command maps all eight IP Precedence values to the
*           port's available traffic classes (transmit queues).
*
* @param   *usp     @b{(input)} Needs to be a valid usp
* @param    cmd     @b{(input)} DAPI_CMD_QOS_COS_IP_PRECEDENCE_TABLE_CONFIG
* @param   *data    @b{(input)} DAPI_QOS_CMD_t.cmdData.ipPrecedenceTableConfig
* @param   *dapi_g  @b{(input)} The driver object    
*
* @returns  L7_SUCCESS    Function executed normally
* @returns  L7_FAILURE    Function failed
*
* @comments This COS mapping table is used when port trust mode is set to 
*           DAPI_QOS_COS_INTF_MODE_TRUST_IPPREC.
*
* @comments This function configures the entire IP Precedence COS mapping 
*           table all at once.
*
* @end
*
*********************************************************************/
/* DAPI_CMD_QOS_COS_IP_PRECEDENCE_TABLE_CONFIG */
extern L7_RC_t hapiBroadQosCosIpPrecedenceTableConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose  This command maps an individual IP DSCP value to one of the
*           port's available traffic classes (transmit queues).
*
* @param   *usp     @b{(input)} Needs to be a valid usp
* @param    cmd     @b{(input)} DAPI_CMD_QOS_COS_IP_DSCP_TO_TC_MAP
* @param   *data    @b{(input)} DAPI_QOS_CMD_t.cmdData.ipDscpToTcMap
* @param   *dapi_g  @b{(input)} The driver object    
*
* @returns  L7_SUCCESS    Function executed normally
* @returns  L7_FAILURE    Function failed
*
* @comments This table is used when port trust mode is set to 
*           DAPI_QOS_COS_INTF_MODE_TRUST_IPDSCP.
*
* @end
*
*********************************************************************/
/* DAPI_CMD_QOS_COS_IP_DSCP_TO_TC_MAP */
extern L7_RC_t hapiBroadQosCosIpDscpToTcMap(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose  This command maps all sixty-four IP DSCP values to the
*           port's available traffic classes (transmit queues).
*
* @param   *usp     @b{(input)} Needs to be a valid usp
* @param    cmd     @b{(input)} DAPI_CMD_QOS_COS_IP_DSCP_TABLE_CONFIG
* @param   *data    @b{(input)} DAPI_QOS_CMD_t.cmdData.ipDscpTableConfig
* @param   *dapi_g  @b{(input)} The driver object    
*
* @returns  L7_SUCCESS    Function executed normally
* @returns  L7_FAILURE    Function failed
*
* @comments This COS mapping table is used when port trust mode is set to 
*           DAPI_QOS_COS_INTF_MODE_TRUST_IPDSCP.
*
* @comments This function configures the entire IP DSCP COS mapping 
*           table all at once.
*
* @end
*
*********************************************************************/
/* DAPI_CMD_QOS_COS_IP_DSCP_TABLE_CONFIG */
extern L7_RC_t hapiBroadQosCosIpDscpTableConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose  Configure COS trust mode for the specified interface
*
* @param   *usp     @b{(input)} Needs to be a valid usp
* @param    cmd     @b{(input)} DAPI_CMD_QOS_COS_INTF_TRUST_MODE_CONFIG
* @param   *data    @b{(input)} DAPI_QOS_COS_CMD_t.cmdData.intfTrustModeConfig
* @param   *dapi_g  @b{(input)} The driver object    
*
* @returns  L7_SUCCESS    Function executed normally
* @returns  L7_FAILURE    Function failed
*
* @comments The intf trust mode setting determines which COS 
*           mapping table is used for handling ingress packets.
*           If the intf mode is untrusted, all ingress packets
*           are assigned based on the port default priority.
*
* @end
*
*********************************************************************/
/* DAPI_CMD_QOS_COS_INTF_TRUST_MODE_CONFIG */
extern L7_RC_t hapiBroadQosCosIntfTrustModeConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose  Configure COS interface-level parameters for the specified port
*
* @param   *usp     @b{(input)} Needs to be a valid usp
* @param    cmd     @b{(input)} DAPI_CMD_QOS_COS_INTF_CONFIG
* @param   *data    @b{(input)} DAPI_QOS_CMD_t.cmdData.intfConfig
* @param   *dapi_g  @b{(input)} The driver object    
*
* @returns  L7_SUCCESS    Function executed normally
* @returns  L7_FAILURE    Function failed
*
* @comments This command affects the port egress queues.
*
* @end
*
*********************************************************************/
/* DAPI_CMD_QOS_COS_INTF_CONFIG */
extern L7_RC_t hapiBroadQosCosIntfConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose  Retrieve status about COS interface-level parameters for the specified port
*
* @param   *usp     @b{(input)} Needs to be a valid usp
* @param    cmd     @b{(input)} DAPI_CMD_QOS_COS_INTF_STATUS
* @param   *data    @b{(input)} DAPI_QOS_CMD_t.cmdData.intfStatus
* @param   *dapi_g  @b{(input)} The driver object    
*
* @returns  L7_SUCCESS    Function executed normally
* @returns  L7_FAILURE    Function failed
*
* @end
*
*********************************************************************/
/* DAPI_CMD_QOS_COS_INTF_STATUS */
extern L7_RC_t hapiBroadQosCosIntfStatus(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose  Configure COS queue scheduler parameters for the specified port
*
* @param   *usp     @b{(input)} Needs to be a valid usp
* @param    cmd     @b{(input)} DAPI_CMD_QOS_COS_QUEUE_SCHED_CONFIG
* @param   *data    @b{(input)} DAPI_QOS_CMD_t.cmdData.queueSchedCfg
* @param   *dapi_g  @b{(input)} The driver object    
*
* @returns  L7_SUCCESS    Function executed normally
* @returns  L7_FAILURE    Function failed
*
* @comments This command affects the port egress queues.
*
* @end
*
*********************************************************************/
/* DAPI_CMD_QOS_COS_QUEUE_SCHED_CONFIG */
extern L7_RC_t hapiBroadQosCosQueueSchedConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose  Configure COS taildrop or WRED parameters for the specified queue
*
* @param   *usp     @b{(input)} Needs to be a valid usp
* @param    cmd     @b{(input)} DAPI_CMD_QOS_COS_QUEUE_DROP_CONFIG
* @param   *data    @b{(input)} DAPI_QOS_CMD_t.cmdData.queueDropCfg
* @param   *dapi_g  @b{(input)} The driver object    
*
* @returns  L7_SUCCESS    Function executed normally
* @returns  L7_FAILURE    Function failed
*
* @comments    This command affects the egress queues on a physical port.
*
* @end
*
*********************************************************************/
/* DAPI_CMD_QOS_COS_QUEUE_DROP_CONFIG */
extern L7_RC_t hapiBroadQosCosQueueDropConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose  Notifies the COS component that a physical port has been
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
extern L7_RC_t hapiBroadQosCosPortLagAddNotify(DAPI_USP_t *portUsp, DAPI_USP_t *lagUsp, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose  Notifies the COS component that a physical port has been
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
extern L7_RC_t hapiBroadQosCosPortLagDeleteNotify(DAPI_USP_t *portUsp, DAPI_USP_t *lagUsp, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose  Notifies the COS component that a physical port has link.
*
* @param   *portUsp         @b{(input)} USP of member port
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
extern L7_RC_t hapiBroadQosCosPortLinkUpNotify(DAPI_USP_t *portUsp, DAPI_t *dapi_g);

#endif
