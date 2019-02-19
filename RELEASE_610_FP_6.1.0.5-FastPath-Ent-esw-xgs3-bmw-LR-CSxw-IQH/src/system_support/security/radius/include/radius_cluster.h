/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 **********************************************************************
 *
 * @filename     radius_cluster.h
 *
 * @purpose      Radius Cluster Component.
 *
 * @component    Radius Cluster Functionality
 *
 * @comments     none
 *
 * @create       02/23/2008
 *
 * @author       mpolicharla
 *
 * @end
 *
 **********************************************************************/
#ifndef INCLUDE_RADIUS_CLUSTER_H
#define INCLUDE_RADIUS_CLUSTER_H

#include "datatypes.h"
#include "comm_structs.h"
#include "log.h"
#include "ipv6_commdefs.h"
#include "wireless_commdefs.h"
#include "clustering_commdefs.h"
#include "radius_api.h"
#include "radius_db.h"

#define RADIUS_CLUSTER_CONFIG_MSG_TYPE 0x1200

/* Msg header elements size in bytes */
#define RADIUS_MSG_TYPE_SIZE                  2
#define RADIUS_MSG_LEN_SIZE                   2
#define RADIUS_MSG_HDR_SIZE                   ( RADIUS_MSG_TYPE_SIZE + RADIUS_MSG_LEN_SIZE )

/* IE header size in bytes  */
#define    RADIUS_IE_ID_SIZE                  RADIUS_MSG_TYPE_SIZE
#define    RADIUS_IE_LEN_SIZE                 RADIUS_MSG_LEN_SIZE
#define    RADIUS_IE_HDR_SIZE                 RADIUS_MSG_HDR_SIZE

/*******************************************
 *
 * Radius client cluster message IE type enumeration
 *
 *******************************************/
typedef enum
{
  /* Global Radius Client Configuration */
  L7_IE_RADIUS_CLIENT_ACCT_ADMIN_MODE_TYPE	= 0xA000,
  L7_IE_RADIUS_CLIENT_MAX_NUM_RETRANS_TYPE	= 0xA001,
  L7_IE_RADIUS_CLIENT_TIMEOUT_DURATION_TYPE = 0xA002,
  L7_IE_RADIUS_CLIENT_DEADTIME_DURATION_TYPE = 0xA003,
  L7_IE_RADIUS_CLIENT_SRC_IP_ADDR_TYPE = 0xA004,
  L7_IE_RADIUS_CLIENT_SECRET_TYPE = 0xA005,
  L7_IE_RADIUS_CLIENT_NAS_IP_MODE_TYPE = 0xA006,
  L7_IE_RADIUS_CLIENT_NAS_IP_ADDR_TYPE = 0xA007,

  /* Server Configuration */
  L7_IE_RADIUS_CLIENT_SERVER_TYPE = 0xA010,
  L7_IE_RADIUS_CLIENT_SERVER_PORT_TYPE = 0xA011,
  L7_IE_RADIUS_CLIENT_GROUP_NAME_TYPE = 0xA012,
  L7_IE_RADIUS_CLIENT_SERVER_SECRET_TYPE = 0xA013,
  L7_IE_RADIUS_CLIENT_SERVER_PRIORITY_TYPE = 0xA014,
  L7_IE_RADIUS_CLIENT_MSG_AUTH_TYPE = 0xA015,
  L7_IE_RADIUS_CLIENT_PRIMARY_TYPE = 0xA016,
  L7_IE_RADIUS_CLIENT_SERVER_MAX_NUM_RETRANS_TYPE = 0xA017,
  L7_IE_RADIUS_CLIENT_SERVER_TIMEOUT_DURATION_TYPE = 0xA018,
  L7_IE_RADIUS_CLIENT_SERVER_DEADTIME_DURATION_TYPE = 0xA019,
  L7_IE_RADIUS_CLIENT_SERVER_SRC_IP_ADDR_TYPE = 0xA01A,
  L7_IE_RADIUS_CLIENT_SERVER_CONFIG_MASK_TYPE = 0xA01B,
  L7_IE_RADIUS_CLIENT_SERVER_USAGE_TYPE = 0xA01C               
}L7RadiusClientIEType_t;

/* Global Radius Client Configuration IE Size */
#define  L7_IE_RADIUS_CLIENT_ACCT_ADMIN_MODE_SIZE      1
#define  L7_IE_RADIUS_CLIENT_MAX_NUM_RETRANS_SIZE      1
#define  L7_IE_RADIUS_CLIENT_TIMEOUT_DURATION_SIZE     2
#define  L7_IE_RADIUS_CLIENT_DEADTIME_DURATION_SIZE    2
#define  L7_IE_RADIUS_CLIENT_SRC_IP_ADDR_SIZE          4
#define  L7_IE_RADIUS_CLIENT_SECRET_SIZE               16
#define  L7_IE_RADIUS_CLIENT_NAS_IP_MODE_SIZE          1
#define  L7_IE_RADIUS_CLIENT_NAS_IP_ADDR_TYPE_SIZE     1
#define  L7_IE_RADIUS_CLIENT_NAS_IP_SIZE               16
#define  L7_IE_RADIUS_CLIENT_NAS_IP_ADDR_SIZE          ((L7_IE_RADIUS_CLIENT_NAS_IP_ADDR_TYPE_SIZE)+\
                                                        (L7_IE_RADIUS_CLIENT_NAS_IP_SIZE))


/* Server Configuration IE Size */
#define  L7_IE_RADIUS_CLIENT_SERVER_SIZE               258
#define  L7_IE_RADIUS_CLIENT_SERVER_PORT_SIZE          2
#define  L7_IE_RADIUS_CLIENT_GROUP_NAME_SIZE           32
#define  L7_IE_RADIUS_CLIENT_SERVER_SECRET_SIZE        16
#define  L7_IE_RADIUS_CLIENT_SERVER_PRIORITY_SIZE      4
#define  L7_IE_RADIUS_CLIENT_MSG_AUTH_SIZE             1
#define  L7_IE_RADIUS_CLIENT_PRIMARY_SIZE              1
#define  L7_IE_RADIUS_CLIENT_SERVER_MAX_NUM_RETRANS_SIZE    1
#define  L7_IE_RADIUS_CLIENT_SERVER_TIMEOUT_DURATION_SIZE   2
#define  L7_IE_RADIUS_CLIENT_SERVER_DEADTIME_DURATION_SIZE  2
#define  L7_IE_RADIUS_CLIENT_SERVER_SRC_IP_ADDR_SIZE        4
#define  L7_IE_RADIUS_CLIENT_SERVER_CONFIG_MASK_SIZE        1
#define  L7_IE_RADIUS_CLIENT_SERVER_USAGE_SIZE               1

#define L7_IE_RADIUS_CLIENT_SRVR_CONFIG_IE_TOTAL_COUNT   13 
#define L7_IE_RADIUS_CLIENT_SRVR_CONFIG_SIZE    (((RADIUS_IE_HDR_SIZE) * (L7_IE_RADIUS_CLIENT_SRVR_CONFIG_IE_TOTAL_COUNT))+ \
                                                  (L7_IE_RADIUS_CLIENT_SERVER_SIZE)+\
                                                  (L7_IE_RADIUS_CLIENT_SERVER_PORT_SIZE)+\
                                                  (L7_IE_RADIUS_CLIENT_GROUP_NAME_SIZE)+\
                                                  (L7_IE_RADIUS_CLIENT_SERVER_SECRET_SIZE)+\
                                                  (L7_IE_RADIUS_CLIENT_SERVER_PRIORITY_SIZE)+\
                                                  (L7_IE_RADIUS_CLIENT_MSG_AUTH_SIZE)+\
                                                  (L7_IE_RADIUS_CLIENT_PRIMARY_SIZE)+\
                                                  (L7_IE_RADIUS_CLIENT_SERVER_MAX_NUM_RETRANS_SIZE)+\
                                                  (L7_IE_RADIUS_CLIENT_SERVER_CONFIG_MASK_SIZE)+\
                                                  (L7_IE_RADIUS_CLIENT_SERVER_TIMEOUT_DURATION_SIZE)+\
                                                  (L7_IE_RADIUS_CLIENT_SERVER_DEADTIME_DURATION_SIZE)+\
                                                  (L7_IE_RADIUS_CLIENT_SERVER_USAGE_SIZE)+\
                                                  (L7_IE_RADIUS_CLIENT_SERVER_SRC_IP_ADDR_SIZE))
typedef enum
{
  RADIUS_CLUSTER_OPER_MSG_RXED  = 0,
  RADIUS_CLUSTER_CFG_SEND,
  RADIUS_CLUSTER_CFG_RX_START,
  RADIUS_CLUSTER_CFG_RX_END,
  RADIUS_CLUSTER_CFG_RX_ABORT,
  RADIUS_CLUSTER_CFG_RX_APPLY,
  RADIUS_CLUSTER_MEMBER_SELF_JOINED,
  RADIUS_CLUSTER_MEMBER_JOINED,
  RADIUS_CLUSTER_MEMBER_LEFT,
  RADIUS_CLUSTER_MEMBER_SELF_LEFT,
  RADIUS_CLUSTER_MEMBER_CONTROLLER_ELECTED
} radiusClusterMsgDesc_t;

typedef enum
{
  RADIUS_CLUSTER_WAIT_STATE,  /* wait for cluster membership to stabilize */
  RADIUS_CLUSTER_CFG_TX_STATE,
  RADIUS_CLUSTER_CFG_RX_START_STATE,
  RADIUS_CLUSTER_CFG_RX_VALIDATE_STATE,
  RADIUS_CLUSTER_CFG_RX_APPLY_STATE,
  RADIUS_CLUSTER_CFG_RX_ABORT_STATE,
  RADIUS_CLUSTER_READY_TO_START_STATE,
  RADIUS_CLUSTER_MAX_NUM_STATES
} radiusClusterState_t;


#define TX_WAIT_TIME_IN_MSECS     20000
#define RX_WAIT_TIME_IN_MSECS      5000

#define RADIUS_CLUSTER_IN_OPER_STATE(state) \
  ( ((state) != RADIUS_CLUSTER_CFG_TX_STATE) && \
    ((state) != RADIUS_CLUSTER_CFG_RX_START_STATE) && \
    ((state) != RADIUS_CLUSTER_CFG_RX_VALIDATE_STATE) && \
    ((state) != RADIUS_CLUSTER_CFG_RX_APPLY_STATE))

typedef struct radiusClusterMessage_s
{
  radiusClusterMsgDesc_t     mtype;
  clusterMemberID_t          cmid;
} radiusClusterMessage_t;

/* Store cluster member details */
typedef struct radiusClusterMember_s
{
  L7_BOOL              inuse;
  clusterMemberID_t    cmid;
} radiusClusterMember_t;


/* Radius cluster operational data */
typedef struct
{
  radiusServerConfigEntry_t serverConfig;
  L7_char8 serverName[L7_RADIUS_SERVER_NAME_LENGTH+1];
} radiusClientServer_t;

typedef struct
{
  L7_uint32 acctAdminMode;
  L7_uint32 maxNumRetrans;
  L7_uint32 timeOutDuration;
  L7_uint32 radiusServerDeadTime;
  L7_uint32 sourceIpAddress;
  L7_char8  radiusServerSecret[L7_RADIUS_MAX_SECRET + 1];
  L7_uint32 retryPrimaryTime;
  L7_BOOL	nasIpMode;
  L7_uint32	nasIpAddress;
  radiusClientServer_t authServer[L7_RADIUS_MAX_AUTH_SERVERS];
  radiusClientServer_t acctServer[L7_RADIUS_MAX_ACCT_SERVERS];

  L7_uint32 authServerCount;
  L7_uint32 acctServerCount;
} radiusClientClusterCfg_t;

#define L7_TCP_MSG_SIZE_MAX             8192

typedef struct radiusClusterOprData_s
{
  clusterMemberID_t         selfID;
  clusterMemberID_t         ctrID;
  radiusClusterState_t      state;

  L7_uchar8                 sendMsgBuf[L7_TCP_MSG_SIZE_MAX];
  radiusClientClusterCfg_t  radiusClusterCfg;
} radiusClusterOprData_t;


/*********************************************************************
* @purpose  Function for updating radius cluster state
*
* @param    event    @b{(input)} Cluster event 
* @param    memId    @b{(input)} Cluster memeber ID asssociated with the event 
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void radisuClusterEventCallback(clusterEvent event, clusterMemberID_t * memID);

/*********************************************************************
* @purpose  Callback to initiate sending radius configuration to cluster memeber
*
* @param    pID    @b{(input)} Pointer to cluster member for sending radius configuration
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void radiusCfgSendCallback(clusterMemberID_t * pID);

/******************************************************************************
* @purpose  Function for validating received radius configuration from cluster memeber
*
* @param    cmd    @b{(input)} Command from cluster module for receiving radius configuration
*
* @returns  none
*
* @notes    none
*
* @end
******************************************************************************/
void radiusCfgRxCallback(clusterConfigCmd cmd);

/*********************************************************************
*
* @purpose  Radius Clustering main task
*
* @returns  void
*
* @comments none
*
* @end
*
*********************************************************************/
void radius_cluster_task(void);

/**********************************************************
*
*  Macro definition to get config type of server parameter 
*
**********************************************************/
#define RADIUS_SERVER_CONFIG_TYPE_GET(configMask, bitPos) \
                                    (((configMask) >> (bitPos)) & 1) 
                              
/**********************************************************
*
*  Wireless Element and Message Header Insertion Macros
*
**********************************************************/
#define RADIUS_PKT_MSG_HDR_SET(msg, buffer, disp)   \
                do { \
                  message = (radiusMsgHdr_t *) (buffer+disp); \
                  message->msgType = osapiHtons (message->msgType); \
                  message->msgLen = osapiHtons (message->msgLen); \
                  disp += RADIUS_MSG_HDR_SIZE; \
                } while (0);

#define RADIUS_PKT_ELEMENT_HDR_SET(element, type, len, buffer, disp)   \
                do { \
                  element = (radiusMsgElementHdr_t *) (buffer+disp); \
                  element->elementId = osapiHtons (type); \
                  element->elementLen = osapiHtons (len); \
                  disp += RADIUS_IE_HDR_SIZE; \
                } while (0);

#define RADIUS_PKT_ELEMENT_HDR_MEMCPY_SET(_elem, _type, _len, _buffer, _disp)   \
                do { \
                  (_elem).elementId = osapiHtons((_type)); \
                  (_elem).elementLen = osapiHtons((_len)); \
                  memcpy(((_buffer)+(_disp)), &(_elem), sizeof(radiusMsgElementHdr_t)); \
                  (_disp) += RADIUS_IE_HDR_SIZE; \
                } while (0);

#define RADIUS_PKT_INT32_SET(value, buffer, disp)    \
                do { \
                  value = osapiHtonl(value); \
                  memcpy (buffer+disp, &value, sizeof(value)); \
                  disp += sizeof(value); \
                } while (0);

#define RADIUS_PKT_INT16_SET(value, buffer, disp)    \
                do { \
                  value = osapiHtons(value); \
                  memcpy (buffer+disp, &value, sizeof(value)); \
                  disp += sizeof(value); \
                } while (0); 

#define RADIUS_PKT_INT8_SET(value, buffer, disp)     \
                do { \
                  memcpy (buffer+disp, &value, sizeof(value)); \
                  disp += sizeof(value); \
                } while (0);

#define RADIUS_PKT_MAC_ADDR_SET(value, buffer, disp)     \
                do { \
                  memcpy (buffer+disp,&value, L7_ENET_MAC_ADDR_LEN); \
                  disp += L7_ENET_MAC_ADDR_LEN; \
                } while (0);

#define RADIUS_PKT_STRING_SET(string, length, buffer, disp)    \
                do { \
                  memcpy (buffer+disp, string, length); \
                  disp += length; \
                } while (0);

#define RADIUS_PKT_MSG_HDR_GET(msg, buffer, disp)   \
                do { \
                  message = (radiusMsgHdr_t *) (buffer+disp); \
                  msg->msgType = osapiNtohs (msg->msgType); \
                  msg->msgLen = osapiNtohs (msg->msgLen); \
                  disp += RADIUS_MSG_HDR_SIZE; \
                } while (0);

#define RADIUS_PKT_MSG_HDR_DIRECT_GET(_msg, _buffer, _disp)   \
                do { \
                  (_msg) = (radiusMsgHdr_t *) ((_buffer)+(_disp)); \
                  (_disp) += RADIUS_MSG_HDR_SIZE; \
                } while (0);

#define RADIUS_PKT_ELEMENT_HDR_GET(element, buffer, disp)   \
                do { \
                  element = (radiusMsgElementHdr_t *) (buffer+disp); \
                  element->elementId = osapiNtohs (element->elementId); \
                  element->elementLen = osapiNtohs (element->elementLen); \
                  disp += RADIUS_IE_HDR_SIZE; \
                } while (0);

#define RADIUS_PKT_ELEMENT_HDR_MEMCPY_GET(_elem, _buffer, _disp)   \
                do { \
                  memcpy(&(_elem), (_buffer)+(_disp), sizeof(radiusMsgElementHdr_t)); \
                  (_elem).elementId = osapiNtohs((_elem).elementId); \
                  (_elem).elementLen = osapiNtohs((_elem).elementLen); \
                  (_disp) += RADIUS_IE_HDR_SIZE; \
                } while (0);

/* macro for IE length check within a case statement */
#define RADIUS_PKT_ELEMENT_SIZE_CHECK(_elem, _size, _offset) \
  if ((_elem).elementLen != (_size)) \
  { \
    LOG_MSG("Element ID (%d) received with invalid length %d, expected %d.\n", (_elem).elementId, (_elem).elementLen, (_size)); \
    (_offset) += (_elem).elementLen; \
    break; \
  }

#define RADIUS_PKT_INT32_GET(value, buffer, disp)    \
                do { \
                  memcpy (&value, buffer+disp, sizeof(value)); \
                  value = osapiNtohl(value); \
                  disp += sizeof(value); \
                } while (0);

#define RADIUS_PKT_INT64_GET(value, buffer, disp)    \
                do { \
                  memcpy (&value, buffer+disp, sizeof(value)); \
                  value = osapiNtohll(value); \
                  disp += sizeof(value); \
                } while (0);

#define RADIUS_PKT_INT16_GET(value, buffer, disp)    \
                do { \
                  memcpy (&value, buffer+disp, sizeof(value)); \
                  value = osapiNtohs(value); \
                  disp += sizeof(value); \
                } while (0);

#define RADIUS_PKT_INT8_GET(value, buffer, disp)     \
                do { \
                  memcpy (&value, buffer+disp, sizeof(value)); \
                  disp += sizeof(value); \
                } while (0);

#define RADIUS_PKT_MAC_ADDR_GET(value, buffer, disp)     \
                do { \
                  memcpy (&value,buffer+disp, L7_ENET_MAC_ADDR_LEN); \
                  disp += L7_ENET_MAC_ADDR_LEN; \
                } while (0);

#define RADIUS_PKT_STRING_GET(string, length, buffer, disp)    \
                do { \
                  memcpy (string, buffer+disp, length); \
                  disp += length; \
                } while (0);

/****************************************
*
*  Wireless Message (TCP/UDP/L2) Header
*
*****************************************/
typedef struct radiusMsgHdr_s
{
   L7_ushort16   msgType; /* Message Type */
   L7_ushort16   msgLen;  /* Message Len */
}radiusMsgHdr_t;


/****************************************
*
*  PDU Handle
*
*****************************************/
typedef struct radiusPduHdl_s
{
   L7_uchar8      *bufPtr;
   L7_uint32      offset;
} radiusPduHdl_t;

/****************************************
*
*  Wireless Element (TCP/UDP/L2) Header
*
*****************************************/
typedef struct radiusMsgElementHdr_s
{
   L7_ushort16   elementId; /* Element Type */
   L7_ushort16   elementLen;  /* Element Len */
} radiusMsgElementHdr_t;

#endif  /* INCLUDE_RADIUS_CLUSTER_H */
