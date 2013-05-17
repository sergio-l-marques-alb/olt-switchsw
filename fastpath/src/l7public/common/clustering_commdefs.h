/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename clustering_commdefs.h
*
* @purpose The purpose of this file is to have a central location for
*          clustering constants.
*
* @component wireless (for now)
*
* @comments none
*
* @create 12/04/2007
*
* @author dcaugherty
* @end
*
**********************************************************************/

#ifndef INCLUDE_CLUSTERING_COMMDEFS
#define INCLUDE_CLUSTERING_COMMDEFS

#include "datatypes.h"
#include "comm_structs.h"
#include "l7_packet.h"
#include "l3_addrdefs.h"
#include "wireless_commdefs.h"

typedef enum
{
  CLUSTERING_PHASE_INIT_0 = 0,
  CLUSTERING_PHASE_INIT_1,
  CLUSTERING_PHASE_INIT_2,
  CLUSTERING_PHASE_WMU,
  CLUSTERING_PHASE_INIT_3
} clustering_CnfgrState_t;

typedef enum {
  CLUSTER_EVENT_LOCAL_SWITCH_JOINED = 0,
  CLUSTER_EVENT_LOCAL_SWITCH_LEFT,
  CLUSTER_EVENT_SWITCH_JOINED,
  CLUSTER_EVENT_SWITCH_LEFT,
  CLUSTER_EVENT_CONTROLLER_ELECTED,
  CLUSTER_EVENT_MAX =  CLUSTER_EVENT_CONTROLLER_ELECTED
} clusterEvent;

typedef enum {
  CLUSTER_MSG_DELIVERY_DATAGRAM = 0,
  CLUSTER_MSG_DELIVERY_RELIABLE 
} clusterMsgDeliveryMethod;

#define CLUSTER_MSG_RELIABLE_SIZE_MAX  L7_WIRELESS_TCP_MSG_SIZE_MAX
#define CLUSTER_MSG_DATAGRAM_SIZE_MAX  L7_WIRELESS_UDP_MSG_SIZE_MAX

/*
 * NB: We can only support 16 applications total!!!!!!!! 
 */


typedef L7_uint32 clusterCfgID;

/* In order to add an application to clustering support, you
 * must first READ THIS COMMENT COMPLETELY.
 *
 * You will note below that there is an enumeration constant
 * below called CLUSTER_CFG_BIT_LAST.  Your new application
 * must assume this value; that is, CLUSTER_CFG_BIT_yourApp
 * must be the last value in this enumeration, and 
 * CLUSTER_CFG_BIT_LAST must be set to it.  This means you 
 * WILL do the following OR ELSE:
 *
 * 1. You will add the following line DIRECTLY ABOVE the #if
 * below:
 *
 *   CLUSTER_CFG_BIT_yourApp, 
 *
 * 2. You will change the line in the first #else clause 
 * immediately below that line to:
 * 
 *   CLUSTER_CFG_BIT_LAST = CLUSTER_CFG_BIT_yourApp
 *
 * Adding your new application's enum value anywhere else 
 * will cause all sorts of havoc and breakage.  You will then
 * be summoned to fix it.
 *
 */

typedef enum
{
  CLUSTER_CFG_BIT_FIRST  = 16,
  CLUSTER_CFG_BIT_CAPTIVE_PORTAL = CLUSTER_CFG_BIT_FIRST,
  CLUSTER_CFG_BIT_RADIUS,
  CLUSTER_CFG_BIT_QOS_ACL,
  CLUSTER_CFG_BIT_QOS_DIFFSERV,

#if defined(CLUSTERING_UNIT_TESTING_ONLY)
  CLUSTER_CFG_BIT_CLUSTER_TEST,
  CLUSTER_CFG_BIT_LAST  =  CLUSTER_CFG_BIT_CLUSTER_TEST
#else
  CLUSTER_CFG_BIT_LAST  =  CLUSTER_CFG_BIT_QOS_DIFFSERV
#endif
} clusterCfgIDBit;

#define NUM_CLUSTER_CONFIG_IDS \
         (CLUSTER_CFG_BIT_LAST - CLUSTER_CFG_BIT_FIRST + 1)

#define CLUSTER_CFG_BIT2ID(bitnumber)  (1 << (bitnumber))

/*  Can be used to get the clusterConfigID for the application, e.g.: 
 *
 *  captivePortalID = CLUSTER_CFG_ID(CAPTIVE_PORTAL)
 */

#define CLUSTER_CFG_ID(bitname)        (1 << (CLUSTER_CFG_BIT_##bitname))


typedef enum {
  CLUSTER_CONFIG_CMD_START_TRANSFER = 0,
  CLUSTER_CONFIG_CMD_END_TRANSFER,
  CLUSTER_CONFIG_CMD_ABORT,
  CLUSTER_CONFIG_CMD_APPLY
} clusterConfigCmd;


#define CLUSTER_MIN_RELIABLE_MSG_SIZE  (1  * 1024) /* in bytes */
#define CLUSTER_MIN_DATAGRAM_MSG_SIZE  (1  * 1024) /* in bytes */
#define CLUSTER_MAX_RELIABLE_MSG_SIZE  L7_WIRELESS_TCP_MSG_SIZE_MAX
#define CLUSTER_MAX_DATAGRAM_MSG_SIZE  L7_WIRELESS_UDP_MSG_SIZE_MAX

#define CLUSTER_MAX_CFG_APPLY_WAIT       (30)  /* in seconds */
#define CLUSTER_CFG_APPLY_WAIT_INTERVAL  (500) /* in milliseconds */


/********************************************************************
 *
 *   Type definitions
 *
 ********************************************************************/

typedef L7_enetMacAddr_t clusterMemberID_t;

/* These next two macros are probably overkill, but they offer some
 * protection against future changes to the clusterMemberID_t type.
 */
#define CLUSTER_MEMBER_ID_COERCE(pID) ((L7_enetMacAddr_t *) (pID))
#define CLUSTER_MEMBER_MAC_COERCE(pMac) ((clusterMemberID_t *) (pMac))

#define CLUSTER_MEMBER_ID_COPY(pDest, pSrc) \
  (memcpy((void *)(pDest), (void *)(pSrc), sizeof(clusterMemberID_t)))
#define CLUSTER_MEMBER_ID_CLEAR(pDest) \
  (memset((void *)(pDest), 0, sizeof(clusterMemberID_t)))
#define CLUSTER_MEMBER_ID_CMP(pID1, pID2)  \
  (memcmp((void *)(pID1), (void *)(pID2), sizeof(clusterMemberID_t)))
#define CLUSTER_MEMBER_ID_EQL(pID1, pID2)  \
  (0 == CLUSTER_MEMBER_ID_CMP((pID1), (pID2)))

/* Definitions to support common message element format */

typedef struct clusterMsgElementHdr_s
{
  L7_ushort16  elementId;   /* Element Type */
  L7_ushort16  elementLen;  /* Element Len */
} clusterMsgElementHdr_t;

#define CLUSTER_PKT_ELEMENT_HDR_SET(_type, _len, _buffer, _disp)   \
          do { \
            clusterMsgElementHdr_t _elem; \
            _elem.elementId = osapiHtons((_type)); \
            _elem.elementLen = osapiHtons((_len)); \
            memcpy(((_buffer)+(_disp)), &_elem, sizeof(_elem)); \
            (_disp) += sizeof(_elem); \
          } while (0);

#define CLUSTER_PKT_INT32_SET(_value, _buffer, _disp)    \
          do { \
            L7_uint32 _tmp32 = (L7_uint32)(_value); \
            _tmp32 = osapiHtonl(_tmp32); \
            memcpy((_buffer)+(_disp), &_tmp32, sizeof(_tmp32)); \
            (_disp) += sizeof(_tmp32); \
          } while (0);

#define CLUSTER_PKT_INT16_SET(_value, _buffer, _disp)    \
          do { \
            L7_ushort16 _tmp16 = (L7_ushort16)(_value); \
            _tmp16 = osapiHtons(_tmp16); \
            memcpy((_buffer)+(_disp), &_tmp16, sizeof(_tmp16)); \
            (_disp) += sizeof(_tmp16); \
          } while (0); 

#define CLUSTER_PKT_INT8_SET(_value, _buffer, _disp)     \
          do { \
            L7_uchar8 _tmp8 = (L7_uchar8)(_value); \
            memcpy((_buffer)+(_disp), &_tmp8, sizeof(_tmp8)); \
            (_disp) += sizeof(_tmp8); \
          } while (0);

#define CLUSTER_PKT_MAC_ADDR_SET(_value, _buffer, _disp)     \
          do { \
            memcpy((_buffer)+(_disp),&(_value), L7_ENET_MAC_ADDR_LEN); \
            (_disp) += L7_ENET_MAC_ADDR_LEN; \
          } while (0);

#define CLUSTER_PKT_STRING_SET(_string, _length, _buffer, _disp)    \
          do { \
            memcpy((_buffer)+(_disp), (_string), (_length)); \
            (_disp) += (_length); \
          } while (0);

#define CLUSTER_PKT_ELEMENT_HDR_GET(_elem, _buffer, _disp)   \
          do { \
            memcpy(&(_elem), (_buffer)+(_disp), sizeof(clusterMsgElementHdr_t)); \
            (_elem).elementId = osapiNtohs((_elem).elementId); \
            (_elem).elementLen = osapiNtohs((_elem).elementLen); \
            (_disp) += sizeof(clusterMsgElementHdr_t); \
          } while (0);

/* macro for IE length check within a case statement */
#define CLUSTER_PKT_ELEMENT_SIZE_CHECK(_compId, _debug, _elem, _size, _offset) \
          if ((_elem).elementLen != (_size)) \
          { \
            if ((_debug) == L7_TRUE) \
            { \
              L7_LOGF(L7_LOG_SEVERITY_DEBUG, (_compId), "%s: Element ID (%d) received with invalid length %d, expected %d.\n", __FUNCTION__, (_elem).elementId, (_elem).elementLen, (_size)); \
            } \
            (_offset) += (_elem).elementLen; \
            break; \
          }

#define CLUSTER_PKT_INT32_GET(_value, _buffer, _disp)    \
          do { \
            L7_uint32 _tmp32; \
            memcpy(&_tmp32, (_buffer)+(_disp), sizeof(_tmp32)); \
            (_value) = osapiNtohl(_tmp32); \
            (_disp) += sizeof(_tmp32); \
          } while (0);

#define CLUSTER_PKT_INT64_GET(_value, _buffer, _disp)    \
          do { \
            L7_uint64 _tmp64; \
            memcpy(&_tmp64, (_buffer)+(_disp), sizeof(_tmp64)); \
            (_value) = osapiNtohll(_tmp64); \
            (_disp) += sizeof(_tmp64); \
          } while (0);

#define CLUSTER_PKT_INT16_GET(_value, _buffer, _disp)    \
          do { \
            L7_ushort16 _tmp16; \
            memcpy(&_tmp16, (_buffer)+(_disp), sizeof(_tmp16)); \
            (_value) = osapiNtohs(_tmp16); \
            (_disp) += sizeof(_tmp16); \
          } while (0);

#define CLUSTER_PKT_INT8_GET(_value, _buffer, _disp)     \
          do { \
            L7_uchar8 _tmp8; \
            memcpy(&_tmp8, (_buffer)+(_disp), sizeof(_tmp8)); \
            (_value) = _tmp8; \
            (_disp) += sizeof(_tmp8); \
          } while (0);

#define CLUSTER_PKT_MAC_ADDR_GET(_value, _buffer, _disp)     \
          do { \
            memcpy (&(_value),(_buffer)+(_disp), L7_ENET_MAC_ADDR_LEN); \
            (_disp) += L7_ENET_MAC_ADDR_LEN; \
          } while (0);

#define CLUSTER_PKT_STRING_GET(_string, _length, _buffer, _disp)    \
          do { \
            memcpy((_string), (_buffer)+(_disp), (_length)); \
            (_disp) += (_length); \
          } while (0);

/********************************************************************
 ********************************************************************
 *
 *   Function pointer type definitions used indirectly in various
 *   function calls, complete with documentation
 *
 ********************************************************************
 ********************************************************************/

/*********************************************************************
* @purpose  Handle cluster events
*
* @param    clusterEvent @b{(input)} type of event
* @param    clusterMemberID_t * @b{(input)} event originator
*
* @returns  nothing
*
* @notes    Used in the clusterMemberNotifyRegister()
*           call, and in the clusterMemberNotifyRegister_t 
*           function type.
*
* @end
*********************************************************************/

typedef 
void clusterMemberEventCallback_t(clusterEvent      event,
                                  clusterMemberID_t *memberID);

 
/*********************************************************************
* @purpose  Handle messages received from cluster controller
*
* @param    L7_ushort16 @b{(input)} message type
* @param    L7_uint32 @b{(input)} message length
* @param    clusterMsgDeliveryMethod @b{(input)} delivery method
* @param    L7_uchar8 @b{(input)} the contents of the message
*
* @returns  nothing
*
* @notes    This function should NOT introduce significant amounts 
*           of latency (e.g. by blocking), since it will be called
*           in the context of the cluster controller.  It should
*           copy the message (at worst) and signal another task to
*           deal with the message's reception.
* @end
*********************************************************************/

typedef 
void clusterMsgCallback_t(L7_ushort16              msgType,
                          clusterMsgDeliveryMethod method,
                          L7_uint32                msgLength,
                          L7_uchar8                *msgBuf);

/*********************************************************************
* @purpose  Send configuration information to a cluster member
*
* @param    clusterMemberID_t * @b{(input)} recipient of information
*
* @returns  nothing
*
* @notes    Used in the clusterConfigRegister() call and the
*           clusterConfigRegister_t type. 
*           
* @end
*********************************************************************/
typedef 
void  clusterConfigSendCallback_t(clusterMemberID_t *);

/*********************************************************************
* @purpose  To handle received configuration events
*
* @param    clusterConfigCmd @b{(input)} configuration command
*
* @returns  nothing
*
* @notes    Used in the clusterConfigRegister() call and the
*           clusterConfigRegister_t type. 
*           
* @end
*********************************************************************/
typedef 
void  clusterConfigRxCallback_t(clusterConfigCmd);



/********************************************************************
 ********************************************************************
 *
 *   Function pointer type definitions used directly in API
 *   function calls, complete with documentation
 *
 ********************************************************************
 ********************************************************************/
             

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
typedef 
L7_RC_t clusterMemberNotifyRegister_t(clusterMemberEventCallback_t *);

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

typedef 
L7_RC_t clusterMemberIdentityGet_t(clusterMemberID_t *switchID,
                                   L7_IP_ADDR_t      *ipV4_addr,
                                   L7_in6_addr_t     *ipV6_addr);


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

typedef L7_uint32 clusterMsgMaxSizeGet_t(clusterMsgDeliveryMethod method);


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

typedef 
L7_RC_t   clusterMsgRegister_t(L7_ushort16 msgType,
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

typedef
L7_RC_t   clusterMsgSend_t(clusterMemberID_t *memberID,
                           clusterMsgDeliveryMethod method,
                           L7_ushort16 msgType,
                           L7_uint32 msgLength,
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

typedef
L7_RC_t   clusterConfigRegister_t(clusterCfgID cfgID,
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
typedef
L7_uint32 clusterConfigRegistrationsGet_t(void);


/*********************************************************************
*
* @purpose  To retrieve the callbacks for a given config ID
*
* @params   clusterCfgID cfgID @b{(input)} config ID to retrieve
* @params   clusterConfigSendCallback_t ** sendCB @b{(output)}
* @params   clusterConfigRxCallback_t ** rxCB @b{(output)}
*
* @returns  L7_TRUE if cfgID has been registered
* @returns  L7_FALSE otherwise
*
* @comments Any of the output parameters can be NULL, 
*           if the caller is uninterested in the callback in question. 
*           (All output parameters set to NULL makes this function
*            equivalent to a "clusterCallbackGet(cfgID)" routine. )
*
* @end
*
*********************************************************************/
typedef 
L7_RC_t   clusterConfigCallbackGet_t(clusterCfgID                cfgID,
                                     clusterConfigSendCallback_t **sendCB,
                                     clusterConfigRxCallback_t   **rxCB);


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

typedef
L7_RC_t   clusterConfigSendDone_t(clusterCfgID cfgID,
				  L7_RC_t status);

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

typedef
L7_RC_t   clusterConfigRxDone_t(clusterCfgID cfgID,
				L7_RC_t status);
  


/*********************************************************************
* @purpose  To tell the clustering component that all configuration
*           information has been applied for the specified switch.
*
* @param    clusterCfgID @b{(input)} cfg ID of application
*
* @returns  nothing
*
* @end
*********************************************************************/

typedef
void      clusterConfigRxApplyDone_t(clusterCfgID cfgID);
				
  

/*********************************************************************
* 
* The following structure type is used by the appropriate component
* to fill our clustering-specific set of sysapi..() calls with 
* the appropriate entry points, which (surprise!) look a lot like
* most of the function type definitions above. 
*
* We do this because, as of right now, Clustering is not its own
* component within FastPath.  This can, should, and will change,
* but for now we allow the appropriate component to supply entry
* points during "config phase 1".
*
*********************************************************************/

typedef struct clusterFunctionSet_s
{
  clusterMemberNotifyRegister_t   * cmnr;
  clusterMemberIdentityGet_t      * cmig;
  clusterMsgMaxSizeGet_t          * cmmsg;
  clusterMsgRegister_t            * cmr;
  clusterMsgSend_t                * cms;
  clusterConfigRegister_t         * ccr;
  clusterConfigRegistrationsGet_t * ccrg;
  clusterConfigCallbackGet_t      * cg;
  clusterConfigSendDone_t         * ccsd;
  clusterConfigRxDone_t           * ccrd;
  clusterConfigRxApplyDone_t      * ccrad;
} clusterFunctionSet_t;

#endif
