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

#include <string.h>
#include "defaultconfig.h"
#include "clustering_defaultconfig.h" /* for default max peers */
#include "clustering_util.h"

/*********************************************************************
 *
 *    Definition of data NOT to be exported outside this
 *    translation unit
 *
 *********************************************************************/


/* Now, we *could* implement the provider registration with a 
 * Boolean flag that every sysapi..() routine would check.
 * But with a little extra work, we can create some dummy
 * do-nothing functions instead.  An interesting side effect
 * of this implementation is that, if we can guarantee that
 * the "registration" (which is practically just the assigning
 * a pointer of type clusterFunctionSet_t) is atomic, there's
 * no need to use a semaphore. 
 */

static clusterFunctionSet_t clusterFuncs =  {
  noClusterMemberNotifyRegister,
  noClusterMemberIdentityGet,
  noClusterMsgMaxSizeGet,
  noClusterMsgRegister,
  noClusterMsgSend,
  noClusterConfigRegister,
  noClusterConfigRegistrationsGet,
  noClusterConfigCallbackGet,
  noClusterConfigSendDone,
  noClusterConfigRxDone,
  noClusterConfigRxApplyDone  
};


#define NUM_CLUSTER_FUNCTION_ENTRIES \
  ( sizeof(clusterFunctionSet_t) / sizeof(void *))



/*********************************************************************
* @purpose  To provide functions for clustering
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



L7_RC_t   clusterProviderRegister(clusterFunctionSet_t * cfs)
{
  L7_RC_t rc = L7_FAILURE;

  do 
  {
    void ** ppTmp;
    L7_uint32 ctr = 0;
    if (L7_NULLPTR == cfs)
    {
      break;
    }

    /* Check the contents of the passed-in structure to see if 
     * any entries are NULL pointers.  If so, refuse registration.
     * (Since all pointers will have the same size, cheat here
     *  and treat the structure like an array.)
     */
    ppTmp = (void **) cfs;

    while ( ctr < NUM_CLUSTER_FUNCTION_ENTRIES )
    {
      if (L7_NULLPTR == ppTmp[ctr])
      {
        break; /* break out of inner while */
      }
      ctr++;
    }

    if ( NUM_CLUSTER_FUNCTION_ENTRIES > ctr )
    {
      break; /* an entry didn't pass muster */
    }

    /* If we get here, everything's okay  */
    memcpy(&clusterFuncs, cfs, sizeof(clusterFunctionSet_t)); 
    rc = L7_SUCCESS;
  } while (0);

  return rc;
}


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

L7_RC_t   clusterSupportGet(void)
{
  if ((L7_TRUE == 
       cnfgrIsComponentPresent(L7_FLEX_WIRELESS_COMPONENT_ID)) &&
      (1 < L7_WIRELESS_MAX_PEER_SWITCHES))
  {
    return L7_SUCCESS;
  }
  return L7_NOT_SUPPORTED;
}
          

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

L7_uint32  clusterMaxMembersGet(void)
{
  return FD_CLUSTER_MAX_PEER_SWITCHES;
}

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

L7_RC_t 
clusterMemberNotifyRegister(clusterMemberEventCallback_t * cmec)
{
  return (clusterFuncs.cmnr)(cmec);
}

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
clusterMemberIdentityGet(clusterMemberID_t *switchID,
                         L7_IP_ADDR_t      *ipV4_addr,
                         L7_in6_addr_t     *ipV6_addr)
{
  return (clusterFuncs.cmig)(switchID, ipV4_addr, ipV6_addr);
}



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
clusterMsgMaxSizeGet(clusterMsgDeliveryMethod method)
{
  return (clusterFuncs.cmmsg)(method);
}


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
                   clusterMsgCallback_t * callback)
{
  return (clusterFuncs.cmr)(msgType, method, callback);
}

           
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
clusterMsgSend(clusterMemberID_t        *memberID,
               clusterMsgDeliveryMethod method,
               L7_ushort16              msgType,
               L7_uint32                msgLength,
               L7_uchar8                *msgContents)
{
  return (clusterFuncs.cms)(memberID, method, msgType,
          msgLength, msgContents);
}


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
                      clusterConfigRxCallback_t * rxCB)
{
  return (clusterFuncs.ccr)(cfgID, sendCB, rxCB);
}


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
L7_uint32 
clusterConfigRegistrationsGet(void)
{
  return (clusterFuncs.ccrg)();
}


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
                         clusterConfigRxCallback_t   **rxCB)
{
  return (clusterFuncs.cg)(cfgID, sendCB, rxCB);
}

/*********************************************************************
* @purpose  To tell the clustering component that all configuration
*           information has been sent to the specified switch.
*
* @param    clusterCfgID @b{(input)} cfg ID of application
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
clusterConfigSendDone(clusterCfgID cfgID,
                      L7_RC_t status)
{
  return (clusterFuncs.ccsd)(cfgID, status);
}


/*********************************************************************
* @purpose  To tell the clustering component that all configuration
*           information has been processed for the specified switch.
*
* @param    clusterCfgID @b{(input)} cfg ID of application
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
clusterConfigRxDone(clusterCfgID cfgID,
                    L7_RC_t status)
{
  return (clusterFuncs.ccrd)(cfgID, status);
}


/*********************************************************************
* @purpose  To tell the clustering component that all configuration
*           information has been applied for the specified switch.
*
* @param    clusterCfgID @b{(input)} cfg ID of application
*
* @notes    
*
* @returns  nothing
*
* @end
*********************************************************************/

void
clusterConfigRxApplyDone(clusterCfgID cfgID)
{
  (clusterFuncs.ccrad)(cfgID);
}
