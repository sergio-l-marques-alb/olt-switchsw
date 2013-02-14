
/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename clustering_api.h
 *
 * @purpose  APIs for clustering support
 *
 * @component wireless (for now)
 *
 * @comments none
 *
 * @create 12/04/2007
 *
 * @author dcaugherty
 *
 * @end
 *
 **********************************************************************/


#ifndef SYSAPI_CLUSTERING_H
#define SYSAPI_CLUSTERING_H

#include "l7_product.h"
#include "log.h"
#include "comm_structs.h"
#include "ipv6_commdefs.h"
#include "clustering_commdefs.h"


/********************************************************************
 ********************************************************************
 *
 *                   API for Clustering Support
 *
 ********************************************************************
 ********************************************************************/
          


/*********************************************************************
* @purpose  To provide functions for sysapi-level clustering functions
*
* @param    clusterFunctionSet_t * @{(input)} set of entry points
*
* @notes    See description of input data type above.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/

L7_RC_t   clusterProviderRegister(clusterFunctionSet_t * cfs);


/*********************************************************************
* @purpose  To indicate if clustering is supported on this switch
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED
*
* @end
*********************************************************************/

L7_RC_t   clusterSupportGet(void);
          
/*********************************************************************
* @purpose  To provide the max number of cluster members supported
*           on this switch
*
* @param    none
*
* @returns  L7_uint32, max number of members (what else?)
*
* @end
*********************************************************************/

L7_uint32  clusterMaxMembersGet(void);
          

/*********************************************************************
* @purpose  To allow an application to register for notification 
*           of various cluster events by supplying a callback function
*
* @param    clusterMemberEventCallback_t * @b{(input)} callback
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/

L7_RC_t    clusterMemberNotifyRegister(clusterMemberEventCallback_t *);

/*********************************************************************
* @purpose  To retreive the IPv4 and/or IPv6 addresses of the
*           cluster member denoted by the given switch ID
*           of various cluster events by supplying a callback function
*
* @param    clusterMemberID_t * @b{(input)} switch ID
* @param    L7_IP_ADDR_t      * @b{(output)) IPv4 address 
* @param    L7_in6_addr_t     * @b{(output)) IPv6 address 
*
* @notes    Output parameters will contain null values (e.g. 0.0.0.0)
*           if the address in question cannot be obtained.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/


L7_RC_t 
clusterMemberIdentityGet(clusterMemberID_t * switchID,
                         L7_IP_ADDR_t      * ipV4_addr,
                         L7_in6_addr_t     * ipV6_addr);


/*********************************************************************
* @purpose  To retreive the maximum size for cluster messages
*           on this switch, given the specified delivery method
*
* @param    clusterMsgDeliveryMethod @b{(input)} delivery method
*
* @returns  Maximum size in bytes.  Note that this value will not
*           change, hence the caller can cache the result indefinitely
*
* @end
*********************************************************************/

L7_uint32 
clusterMsgMaxSizeGet(clusterMsgDeliveryMethod method);


/*********************************************************************
* @purpose  To register for clustering messages of a given type
*           and delivery method
*
* @param    L7_ushort16                  @b{(input)} message type
* @param    clusterMsgDeliveryMethod * @b{(input)) method
* @param    clusterMsgCallback_t     * @b{(input)) callback
*
* @notes    Callback function will be invoked when message with
*           given type and delivery method is received by the
*           clustering subsystem.  Note that a function of this
*           type should never introduce significant latency when
*           handling the reception of messages.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/

L7_RC_t
clusterMsgRegister(L7_ushort16 msgType,
                   clusterMsgDeliveryMethod method,
                   clusterMsgCallback_t * callback);

           
/*********************************************************************
* @purpose  To send a message to a cluster member
*
* @param    clusterMemberID_t    @b{(input)} intended recipient
* @param    clusterMsgDeliveryMethod * @b{(input)) method
* @param    L7_ushort16                  @b{(input)} message type
* @param    L7_uint32                  @b{(input)} message length
* @param    L7_uchar8 *                @b{(input)} contents
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/

L7_RC_t   
clusterMsgSend(clusterMemberID_t * memberID,
               clusterMsgDeliveryMethod method,
               L7_ushort16 msgType,
               L7_uint32   msgLength,
               L7_uchar8 * msgContents);


/*********************************************************************
* @purpose  To register to handle for configuration messages
*
* @param    clusterCfgID  @b{(input)} config ID
* @param    clusterConfigSendCallback_t * @b{(input)) send callback
* @param    clusterConfigRxCallback_t *   @b{(input)) recv callback
*
* @notes    Tells the cluster API that, for the application 
*           corresponding to the configuration ID, to invoke the
*           send and receive routines given when configuration info
*           is "pushed" by or from a cluster controller.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/

L7_RC_t   
clusterConfigRegister(clusterCfgID cfgID,
                      clusterConfigSendCallback_t * sendCB,
                      clusterConfigRxCallback_t * rxCB);


/*********************************************************************
*
* @purpose  To retrieve the bit mask of apps registered for clustering
*
* @params   none
*
* @returns  L7_uint32
*
* @comments Return value of 0 implies no registrants
*
* @end
*
*********************************************************************/
L7_uint32 clusterConfigRegistrationsGet(void);

/*********************************************************************
*
* @purpose  To retrieve the callbacks for a given config ID
*
* @params   clusterCfgID cfgID @b{(input)} config ID to retrieve
* @params   clusterConfigSendCallback_t ** sendCB @b{(output)}
* @params   clusterConfigRxCallback_t ** rxCB @b{(output)}
*
* @returns  L7_SUCCESS if cfgID has been registered
* @returns  L7_FAILURE otherwise
*
* @comments Any of the output parameters can be NULL, 
*           if the caller is uninterested in the callback in question. 
*           (All output parameters set to NULL makes this function
*            equivalent to a "clusterCallbackGet(cfgID)" routine. )
*
* @end
*
*********************************************************************/
L7_RC_t
clusterConfigCallbackGet(clusterCfgID                cfgID,
                         clusterConfigSendCallback_t **sendCB,
                         clusterConfigRxCallback_t   **rxCB);

/*********************************************************************
* @purpose  To tell the clustering component that all configuration
*           information has been sent to the specified switch.
*
* @param    clusterCfgID cfgID @b{(input)} config ID to retrieve
* @param    L7_RC_t @b{(input)} status 
*
* @notes    parameter value equals either L7_SUCCESS or L7_FAILURE 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/

L7_RC_t
clusterConfigSendDone(clusterCfgID  cfgID,
                      L7_RC_t       status);

/*********************************************************************
* @purpose  To tell the clustering component that all configuration
*           information has been processed for the specified switch.
*
* @param    L7_RC_t @b{(input)} status ap
*
* @notes    parameter value equals either L7_SUCCESS or L7_FAILURE 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/

L7_RC_t
clusterConfigRxDone(clusterCfgID  cfgID,
                    L7_RC_t       status);

/*********************************************************************
* @purpose  To tell the clustering component that all configuration
*           information has been processed for the specified switch.
*
* @param    clusterCfgID cfgID @b{(input)} config ID to retrieve
*
* @notes    parameter value equals either L7_SUCCESS or L7_FAILURE 
*
* @returns  nothing
*
* @end
*********************************************************************/

L7_RC_t
clusterConfigRxApplyDone(clusterCfgID  cfgID);
                          


/*********************************************************************
* @purpose  This function returns the current configurator state
*
* @param   none
*
* @returns  clustering_CnfgrState_t value
*
* @end
*********************************************************************/
clustering_CnfgrState_t clusteringStateGet(void);

#endif  /* SYSAPI_CLUSTERING_H */
