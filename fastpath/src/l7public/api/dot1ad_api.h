/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename dot1ad_api.h
*
* @purpose Contains dot1ad offerings 
*
* @component DOT1AD 
*
* @comments 
*
* @create 
*
* @author
*
* @end
*
**********************************************************************/
#ifndef INCLUDE_DOT1AD_API_H
#define INCLUDE_DOT1AD_API_H

#include "nimapi.h"

/* the following values are established here if not already defined */

/* There should be atleast one NNI port to make sense for the services on UNI ports. */
#define L7_DOT1AD_UNI_INTERFACE_COUNT_MAX       (L7_MAX_PORT_COUNT - 1) 

#define L7_DOT1AD_MAX_VLAN_ID 4095
#define L7_DOT1AD_MIN_VLAN_ID 1
#define L7_DOT1AD_NULL_VLAN_ID 0

#define L7_DOT1AD_INVALID_UNIT -1
#define L7_DOT1AD_INVALID_SLOT -1
#define L7_DOT1AD_INVALID_PORT -1

#define L7_DOT1AD_VLAN(vlanTag)           (vlanTag&0xfff)
#define L7_DOT1AD_GET_DOT1P_PRIORITY(vlanTag) ((vlanTag&0xf000) >> 13)
#define L7_DOT1AD_SET_DOT1P_PRIORITY(priority)(priority << 13)

#define L7_DOT1AD_SERVICES_MAX                  4096
#define L7_DOT1AD_MAX_DOT1P                     7

#define L7_DOT1AD_MIN_SERVICE_INSTANCES         1
#define L7_DOT1AD_MAX_PORTS_FOR_SUBSCRIPTION    2
#define L7_DOT1AD_MIN_NNI_PORTS_COUNT           1


/*** TLV definitions for dot1ad ***/
#define L7_DOT1AD_TLV_MAX_SIZE                 65536 /* 64 KB is TLV util limit */
                                                      
#define L7_DOT1AD_TLV_LIST_TYPE                0x0020
#define L7_DOT1AD_TLV_INST_DEL_LIST_TYPE       0x0021
                                                      
#define L7_DOT1AD_TLV_RULE_DEF                 0x0610
#define L7_DOT1AD_TLV_MATCH_COS_TYPE           0x0611
#define L7_DOT1AD_TLV_MATCH_COS2_TYPE          0x0612
#define L7_DOT1AD_TLV_MATCH_DSTIP_TYPE         0x0613
#define L7_DOT1AD_TLV_MATCH_DSTIPV6_TYPE       0x0614
#define L7_DOT1AD_TLV_MATCH_DSTL4PORT_TYPE     0x0615
#define L7_DOT1AD_TLV_MATCH_DSTMAC_TYPE        0x0616
#define L7_DOT1AD_TLV_MATCH_ETYPE_TYPE         0x0617
#define L7_DOT1AD_TLV_MATCH_EVERY_TYPE         0x0618
#define L7_DOT1AD_TLV_MATCH_FLOWLBLV6_TYPE     0x0619
#define L7_DOT1AD_TLV_MATCH_ICMPMSG_TYPE       0x061A
#define L7_DOT1AD_TLV_MATCH_IPTOS_TYPE         0x061B
#define L7_DOT1AD_TLV_MATCH_PROTOCOL_TYPE      0x061C
#define L7_DOT1AD_TLV_MATCH_SRCIP_TYPE         0x061D
#define L7_DOT1AD_TLV_MATCH_SRCIPV6_TYPE       0x061E
#define L7_DOT1AD_TLV_MATCH_SRCL4PORT_TYPE     0x061F
#define L7_DOT1AD_TLV_MATCH_SRCMAC_TYPE        0x0620
#define L7_DOT1AD_TLV_MATCH_VLANID_TYPE        0x0621
#define L7_DOT1AD_TLV_MATCH_VLANID2_TYPE       0x0622
#define L7_DOT1AD_TLV_MATCH_VLANTAG_TYPE       0x0623
#define L7_DOT1AD_TLV_SERVICE_TYPE             0x0624
#define L7_DOT1AD_TLV_NNI_INTERFACE_LIST       0x0625
#define L7_DOT1AD_TLV_MATCH_VLANID3_TYPE       0x0626

#define L7_DOT1AD_TLV_ATTR_MARK_VLAN_TYPE      0x0821
#define L7_DOT1AD_TLV_ATTR_MARK_VLAN2_TYPE      0x0822
#define L7_DOT1AD_TLV_ATTR_MARK_DVLAN_TYPE      0x0823

/* Police Conform/Exceed/Nonconform policy attribute TLV -- action flag values*/
#define L7_DOT1AD_TLV_ATTR_POLICE_ACT_DROP         1 /* discard all  */
#define L7_DOT1AD_TLV_ATTR_POLICE_ACT_MARKCOS      2 /* mark w/COS   */
#define L7_DOT1AD_TLV_ATTR_POLICE_ACT_MARKCOS2     3 /* mark w/COS2  */
#define L7_DOT1AD_TLV_ATTR_POLICE_ACT_MARKDSCP     4 /* mark w/DSCP  */
#define L7_DOT1AD_TLV_ATTR_POLICE_ACT_MARKPREC     5 /* mark w/Preced*/
#define L7_DOT1AD_TLV_ATTR_POLICE_ACT_SEND         6 /* no marking   */


#define DOT1AD_VLAN_ASIS    0x01
#define DOT1AD_VLAN_ASRX    0x02
#define DOT1AD_VLAN_REMOVE  0x04
#define DOT1AD_VLAN_REMARK  0x08

#define DOT1AD_TLV_MATCH_EVERY_NONE 0
#define DOT1AD_TLV_MATCH_EVERY 1

/* dot1ad List Instance */
typedef struct dot1adTlvListType_s
{
  L7_uint32     instanceKey; /* dot1adnum */
} dot1adTlvListType_t;
#define L7_DOT1AD_TLV_LIST_TYPE_LEN (L7_uint32)sizeof(dot1adTlvListType_t)

/* dot1ad service type TLV */
typedef struct dot1adTlvServiceType_s
{
  L7_DOT1AD_SVCTYPE_t     srvType; /* dot1ad service type */
} dot1adTlvServiceType_t;
#define L7_DOT1AD_TLV_SERVICE_TYPE_LEN (L7_uint32)sizeof(dot1adTlvServiceType_t)

/* dot1ad NNI Interface List TLV */
typedef struct dot1adTlvNniInterfaceList_s
{
  L7_uint32 nniIntfCnt;
  nimUSP_t nniIntf[L7_DOT1AD_MAX_NNI_INTFS_PER_SUBSCRIPTION];
} dot1adTlvNniInterfaceList_t;
#define L7_DOT1AD_TLV_NNI_INTERFACE_LIST_LEN (L7_uint32)sizeof(dot1adTlvNniInterfaceList_t)

/* dot1ad Deletion List TLV */
typedef struct
{
  L7_uint32     keyCount;
  L7_uint32     instanceKey; /* may extend this later for a list */
} dot1adTlvInstDelList_t;
#define L7_DOT1AD_TLV_INST_DEL_LIST_LEN (L7_uint32)sizeof(dot1adTlvInstDelList_t)
/* special definition for minimum native length (useful for building, parsing)*/
#define L7_DOT1AD_TLV_INST_DEL_LIST_MIN_LEN (L7_uint32)sizeof(L7_uint32)


/* dot1ad Rule Definition TLV */
#define L7_DOT1AD_TLV_RULE_DEF_PERMIT 0
#define L7_DOT1AD_TLV_RULE_DEF_DENY   1 
#define L7_DOT1AD_TLV_RULE_CORR_NULL  0        /* 0 means no rule logging    */
typedef struct
{
  L7_uchar8     denyFlag;                       
  L7_uchar8     rsvd1[3];                       /* reserved -- for alignment  */
} dot1adTlvRuleDef_t;
#define L7_DOT1AD_TLV_RULE_DEF_LEN (L7_uint32)sizeof(dot1adTlvRuleDef_t)


/* Class of Service (CoS) match criterion TLV */
typedef struct
{
  L7_uchar8     cosValue;                       /* COS value (0-7)            */
  L7_uchar8     rsvd1[3];                       /* reserved -- for alignment  */
} dot1adTlvMatchCos_t;
#define L7_DOT1AD_TLV_MATCH_COS_LEN ((L7_uint32)sizeof(dot1adTlvMatchCos_t))


/* Secondary Class of Service (CoS2) match criterion TLV */
typedef struct
{
  L7_uchar8     cosValue;                       /* COS value (0-7)            */
  L7_uchar8     rsvd1[3];                       /* reserved -- for alignment  */
} dot1adTlvMatchCos2_t;
#define L7_DOT1AD_TLV_MATCH_COS2_LEN ((L7_uint32)sizeof(dot1adTlvMatchCos2_t))


/* Destination IP Address match criterion TLV */
typedef struct
{
  L7_uint32     ipAddr;
  L7_uint32     ipMask;
} dot1adTlvMatchDstIp_t;
#define L7_DOT1AD_TLV_MATCH_DSTIP_LEN (L7_uint32)sizeof(dot1adTlvMatchDstIp_t)


/* Destination IPv6 Address match criterion TLV */
typedef struct
{
  L7_uchar8     ipv6Addr[L7_IP6_ADDR_LEN];
  L7_uchar8     ipv6Mask[L7_IP6_ADDR_LEN];
} dot1adTlvMatchDstIpv6_t;
#define L7_DOT1AD_TLV_MATCH_DSTIPV6_LEN (L7_uint32)sizeof(dot1adTlvMatchDstIpv6_t)


/* Destination Layer-4 Port Range match criterion TLV */
#define L7_DOT1AD_TLV_MATCH_DSTL4PORT_MASK  0xFFFF
#define L7_DOT1AD_TLV_MATCH_DSTL4PORT_SHIFT 0
typedef struct
{
  L7_ushort16   portStart;
  L7_ushort16   portEnd;
  L7_ushort16   portMask;
  L7_ushort16   rsvd1;                          /* reserved -- for alignment  */
} dot1adTlvMatchDstL4Port_t;
#define L7_DOT1AD_TLV_MATCH_DSTL4PORT_LEN (L7_uint32)sizeof(dot1adTlvMatchDstL4Port_t)


/* Destination MAC Address match criterion TLV */
typedef struct
{
  L7_uchar8     macAddr[L7_MAC_ADDR_LEN];
  L7_uchar8     macMask[L7_MAC_ADDR_LEN];
} dot1adTlvMatchDstMac_t;
#define L7_DOT1AD_TLV_MATCH_DSTMAC_LEN ((L7_uint32)sizeof(dot1adTlvMatchDstMac_t))


/* Ethertype match criterion TLV */
typedef struct
{
  L7_ushort16   etypeValue1;
  L7_ushort16   etypeValue2;
  L7_uchar8     checkStdHdrFlag;                /* qualify using protocol hdr */
  L7_uchar8     rsvd1[3];                       /* reserved -- for alignment  */
} dot1adTlvMatchEtype_t;
#define L7_DOT1AD_TLV_MATCH_ETYPE_LEN ((L7_uint32)sizeof(dot1adTlvMatchEtype_t))


/* "Match Every" match criterion TLV */
#define L7_DOT1AD_TLV_MATCH_EVERY_LEN       0


/* IPv6 Flow Label match criterion TLV */
#define L7_DOT1AD_TLV_MATCH_FLOWLBLV6_MASK   0xFFFFF
#define L7_DOT1AD_TLV_MATCH_FLOWLBLV6_SHIFT  0
typedef struct
{
  L7_uint32     flowLabel;                      /* defined as 20-bit value    */
  L7_uint32     flowLabelMask;                  /* in case masking allowed    */
} dot1adTlvMatchFlowlblv6_t;
#define L7_DOT1AD_TLV_MATCH_FLOWLBLV6_LEN (L7_uint32)sizeof(dot1adTlvMatchFlowlblv6_t)


/* ICMP Message match criterion TLV */
/* NOTE: Currently used for implicit IPv6 deny all only (not user configurable) */
#define L7_DOT1AD_TLV_MATCH_ICMPMSG_MASK_OFF    0x00
#define L7_DOT1AD_TLV_MATCH_ICMPMSG_MASK_ON     0xFF
typedef struct
{
  L7_uchar8     msgType; 
  L7_uchar8     msgTypeMask;
  L7_uchar8     msgCode; 
  L7_uchar8     msgCodeMask;
} dot1adTlvMatchIcmpMsg_t;
#define L7_DOT1AD_TLV_MATCH_ICMPMSG_LEN (L7_uint32)sizeof(dot1adTlvMatchIcmpMsg_t)


/* IP Type of Service (ToS) match criterion TLV */
#define L7_DOT1AD_TLV_MATCH_IPDSCP_MASK         0xFC
#define L7_DOT1AD_TLV_MATCH_IPDSCP_SHIFT        2    
#define L7_DOT1AD_TLV_MATCH_IPPRECEDENCE_MASK   0xE0
#define L7_DOT1AD_TLV_MATCH_IPPRECEDENCE_SHIFT  5
typedef struct
{
  L7_uchar8     tosValue;
  L7_uchar8     tosMask;
  L7_uchar8     rsvd1[2];                       /* reserved -- for alignment  */
} dot1adTlvMatchIpTos_t;
#define L7_DOT1AD_TLV_MATCH_IPTOS_LEN (L7_uint32)sizeof(dot1adTlvMatchIpTos_t)


/* Protocol Number match criterion TLV */
#define L7_DOT1AD_TLV_MATCH_PROTOCOL_MASK    0xFF
#define L7_DOT1AD_TLV_MATCH_PROTOCOL_IP_MASK 0x00  /* 'match all' protocols */
#define L7_DOT1AD_TLV_MATCH_PROTOCOL_SHIFT   0
typedef struct
{
  L7_uchar8     protoNumValue;
  L7_uchar8     protoNumMask;
  L7_uchar8     rsvd1[2];                       /* reserved -- for alignment  */
} dot1adTlvMatchProtocol_t;
#define L7_DOT1AD_TLV_MATCH_PROTOCOL_LEN (L7_uint32)sizeof(dot1adTlvMatchProtocol_t)


/* Source IP Address match criterion TLV */
typedef struct
{
  L7_uint32     ipAddr;
  L7_uint32     ipMask;
} dot1adTlvMatchSrcIp_t;
#define L7_DOT1AD_TLV_MATCH_SRCIP_LEN (L7_uint32)sizeof(dot1adTlvMatchSrcIp_t)


/* Source IPv6 Address match criterion TLV */
typedef struct
{
  L7_uchar8     ipv6Addr[L7_IP6_ADDR_LEN];
  L7_uchar8     ipv6Mask[L7_IP6_ADDR_LEN];
} dot1adTlvMatchSrcIpv6_t;
#define L7_DOT1AD_TLV_MATCH_SRCIPV6_LEN (L7_uint32)sizeof(dot1adTlvMatchSrcIpv6_t)


/* Source Layer-4 Port Range match criterion TLV */
#define L7_DOT1AD_TLV_MATCH_SRCL4PORT_MASK  0xFFFF
#define L7_DOT1AD_TLV_MATCH_SRCL4PORT_SHIFT 0
typedef struct
{
  L7_ushort16   portStart;
  L7_ushort16   portEnd;
  L7_ushort16   portMask;
  L7_ushort16   rsvd1;                          /* reserved -- for alignment  */
} dot1adTlvMatchSrcL4Port_t;
#define L7_DOT1AD_TLV_MATCH_SRCL4PORT_LEN (L7_uint32)sizeof(dot1adTlvMatchSrcL4Port_t)


/* Source MAC Address match criterion TLV */
typedef struct
{
  L7_uchar8     macAddr[L7_MAC_ADDR_LEN];
  L7_uchar8     macMask[L7_MAC_ADDR_LEN];
} dot1adTlvMatchSrcMac_t;
#define L7_DOT1AD_TLV_MATCH_SRCMAC_LEN ((L7_uint32)sizeof(dot1adTlvMatchSrcMac_t))


/* VLAN ID match criterion TLV */
typedef struct
{
  L7_ushort16   vidStart;                       /* VLAN ID start              */
  L7_ushort16   vidEnd;                         /* VLAN ID end                */
} dot1adTlvMatchVlanId_t;
#define L7_DOT1AD_TLV_MATCH_VLANID_LEN ((L7_uint32)sizeof(dot1adTlvMatchVlanId_t))


/* Service VLAN ID (VLAN ID2) match criterion TLV */
typedef struct
{
  L7_ushort16   vidStart;                       /* Secondary ID start         */
  L7_ushort16   vidEnd;                         /* Secondary ID end           */
} dot1adTlvMatchVlanId2_t;
#define L7_DOT1AD_TLV_MATCH_VLANID2_LEN ((L7_uint32)sizeof(dot1adTlvMatchVlanId2_t))

/* Isolated VLAN ID TLV */
typedef struct
{
  L7_ushort16   vid;                       /* VLAN ID */
} dot1adTlvMatchVlanId3_t;
#define L7_DOT1AD_TLV_MATCH_VLANID3_LEN ((L7_uint32)sizeof(dot1adTlvMatchVlanId3_t))

/* VLAN tag status match criterion TLV */
typedef struct
{
  L7_uchar8     vlanTagStatus;
  L7_uchar8     rsvd1[3];                       /* reserved -- for alignment  */
} dot1adTlvMatchVlanTag_t;
#define L7_DOT1AD_TLV_MATCH_VLANTAG_LEN (L7_uint32)sizeof(dot1adTlvMatchVlanTag_t)

typedef struct
{
  L7_ushort16   innerVlanOp;                       /* Inner ID operation         */
  L7_ushort16   innerVlan;                         /* Inner ID                   */
  L7_ushort16   outerVlanOp;                       /* Secondary ID operation     */
  L7_ushort16   outerVlan;                         /* Secondary ID               */
  /*specify egress port for EVT */
  L7_uint32     intfUnit;                       /* interface unit number      */
  L7_uint32     intfSlot;                       /* interface slot number      */
  L7_uint32     intfPort;                       /*interface port num (0-based)*/
} dot1adTlvAttrDVlan_t;
#define L7_DOT1AD_TLV_ATTR_MARK_DVLAN_LEN ((L7_uint32)sizeof(dot1adTlvAttrDVlan_t))

typedef struct dot1adSubscriptionTreeKey_s
{
  L7_uint32            intIfNum;       /* UNI Interface and index in AVL tree */
  L7_uint32            svid;          /* Service Vlan ID and index in AVL tree */
  L7_uint32            cvid;          /* Resultant CVID and index in AVL tree*/
} dot1adSubscriptionTreeKey_t;

typedef struct dot1adSubscriptionStruct_s
{
  dot1adSubscriptionTreeKey_t   subscrEntryKey;
#define L7_DOT1AD_IS_IN_CONFIG 0x01
#define L7_DOT1AD_IS_IN_HW     0x02
  L7_BOOL                       isConfigured;
  L7_uchar8                     subscrName[L7_DOT1AD_SUBSCRIPTION_NAME_LEN_MAX+1];      /* subscription name */
  L7_uint32                     configMask;
  L7_DOT1AD_PKTTYPE_t           pktType;       /* untagged or priority tagged or tagged */
  L7_uint32                     cvidToMatch;   /* inner vlan id: single value    */
  L7_uint32                     svidToMatch;
  L7_uint32                     priorityToMatch;
  L7_BOOL                       removeTag;
  L7_uint32                     nniInterface[L7_DOT1AD_MAX_NNI_INTFS_PER_SUBSCRIPTION];   /* NNI Interface(s) associated with this subscription */  
  L7_uint32                     nniIntfCount;
  L7_uint32                     refCount; /* Represents the number of instances this 
                                           subscription is applied on */
  void                          *next;        /* pointer needed by AVL library */
} dot1adSubscriptionStruct_t;


/***************************************************************************
****************************FUNCTION PROTOTYPES*****************************
****************************************************************************/

/*************************************************************************
*
* @purpose  Retrieve the min/max index values allowed for the services table
*
* @param    *pMin       @b{(output)} Pointer to index min output location
* @param    *pMax       @b{(output)} Pointer to index max output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t dot1adServiceIndexMinMaxGet(L7_uint32 *pMin, L7_uint32 *pMax);

/*********************************************************************
*
* @purpose  Get the first service created in the system.
*
* @param    *svid   @b{(output)} first service VlanID 
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    no services exist
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1adServiceIndexGetFirst(L7_uint32 *svid);

/*********************************************************************
*
* @purpose  Get the next sequential service VlanID
*
* @param    svid    @b{(input)}   service VlanID
* @param    *next   @b{(output)}  next service VlanID
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    no services exist, or svid is the last
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1adServiceIndexGetNext(L7_uint32 svid, L7_uint32 *next);

/*********************************************************************
*
* @purpose  Checks if svid is a valid, configured service
*
* @param    svid    @b{(input)}  service VlanID
*
* @returns  L7_SUCCESS  service VlanID is valid
* @returns  L7_FAILURE  service VlanID not valid
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1adServiceIndexValidCheck(L7_uint32 svid);

/*********************************************************************
*
* @purpose  Checks if the service VlanID is within proper range
*
* @param    svid    @b{(input)}  service VlanID 
*
* @returns  L7_SUCCESS  service VlanID is in range
* @returns  L7_FAILURE  service VlanID is out of range
* @returns  L7_DEPENDENCY_NOT_MET If the service VlanID is 
*                                 management vlan or default vlan.
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1adServiceSvidRangeCheck(L7_uint32 svid);

/*********************************************************************
*
* @purpose  To create a new dot1ad service.
*
* @param    svid    @b{(input)} service VlanID 
*
* @returns  L7_SUCCESS        service created
* @returns  L7_FAILURE        invalid service identifier, all other failures
* @returns  L7_ERROR          service already exists
* @returns  L7_TABLE_IS_FULL  maximum number of services already created
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1adServiceCreate(L7_uint32 svid);

/*********************************************************************
*
* @purpose  To delete an existing dot1ad service.
*
* @param    svid    @b{(input)} service VlanID
*
* @returns  L7_SUCCESS  service deleted
* @returns  L7_ERROR    service does not exist
* @returns  L7_FAILURE  invalid service , all other failures
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1adServiceDelete(L7_uint32 svid);

/*********************************************************************
*
* @purpose  To add the name to this service instance.
*
* @param    svid    @b{(input)} service index
* @param    *name   @b{(input)} service name 
*                   (1 to L7_DOT1AD_SERVICE_NAME_LEN_MAX chars)
*
* @returns  L7_SUCCESS  name is added
* @returns  L7_FAILURE  invalid service index, invalid name, or other failure
* @returns  L7_ERROR    service does not exist
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1adServiceNameAdd(L7_uint32 svid, L7_char8 *name);

/*********************************************************************
*
* @purpose  To set the service type for a given dot1ad service.
*
* @param    svid           @b{(input)}  service VlanID.
* @param    serviceType    @b{(input)}  Type of the dot1adService.
*                                       It can be either 
*                                       L7_DOT1AD_SVCTYPE_ELINE,
*                                       or L7_DOT1AD_SVCTYPE_ELAN,
*                                       or  L7_DOT1AD_SVCTYPE_ETREE,
*                                       or  L7_DOT1AD_SVCTYPE_TLS
*
* @returns  L7_SUCCESS      service Type is set sucessfully.
* @returns  L7_ERROR        service with specified service VlanID 
*                           does not exist.
* @returns  L7_FAILURE      invalid service VlanID, or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1adServiceTypeSet(L7_uint32 svid, L7_DOT1AD_SVCTYPE_t serviceType);

/*********************************************************************
*
* @purpose  To set the isolated VlanID for E-TREE service
*
* @param    svid        @b{(input)} service index
* @param    isoltaeVid  @b{(input)} isolated vlan id
*
* @returns  L7_SUCCESS      Isolate VlanID  is set.
* @returns  L7_ERROR        service with specified service VlanID 
*                           does not exist.
* @returns  L7_FAILURE      invalid service VlanID, or other failure

* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1adIsolateVidSet(L7_uint32 svid, L7_uint32 isolateVid);

/*********************************************************************
*
* @purpose  To get the isolated VLAN for E-TREE service
*
* @param    svid        @b{(input)}  service VlanID
* @param    isoltaeVid  @b{(output)} isolated VlanID
*
* @returns  L7_SUCCESS  Isolated VlanID is retrieved successfully.
* @returns  L7_ERROR    Service with specified service VlanID 
*                       does not exist.
* @returns  L7_FAILURE  Invalid service VlanID, or other failure

* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1adIsolateVidGet(L7_uint32 svid, L7_uint32 *isolateVid);

/*********************************************************************
*
* @purpose  To get the service type for a given dot1ad service.
*
* @param    svid           @b{(input)}  service VlanID.
* @param    serviceType    @b{(output)}  Type of the dot1adService.
*
* @returns  L7_SUCCESS      service Type is retrieved sucessfully.
* @returns  L7_ERROR        service with specified service VlanID 
*                           does not exist.
* @returns  L7_FAILURE      invalid service VlanID, or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1adServiceTypeGet(L7_uint32 svid, L7_DOT1AD_SVCTYPE_t *serviceType);


/*********************************************************************
*
* @purpose  To get the name of this service.
*
* @param    svid    @b{(input)}  service index
* @param    *name   @b{(output)} service name 
*                                (1 to L7_DOT1AD_SERVICE_NAME_LEN_MAX chars)
*
* @returns  L7_SUCCESS  name is retrieved
* @returns  L7_FAILURE  invalid service index, name ptr, or other failure
* @returns  L7_ERROR    service does not exist
*
* @comments Caller must provide a name buffer of at least
*           (L7_DOT1AD_SERVICE_NAME_LEN_MAX+1) characters.
*
* @end
*
*********************************************************************/
L7_RC_t dot1adServiceNameGet(L7_uint32 svid, L7_uchar8 *name);

/*********************************************************************
*
* @purpose  To set the NNI Interface List for a given dot1ad service.
*
* @param    svid           @b{(input)}  service VlanID of the 
*                                       given dot1ad service.
* @param    nniIntfList    @b{(input)}  NNI Interface List.
* @param    nniIntfCnt     @b{(input)}  Number of NNI interfaces
*                                       specified in the nniIntfList.
*
* @returns  L7_SUCCESS            If NNI Intf List is added
* @returns  L7_ERROR              If the service does not exist
* @returns  L7_TABLE_IS_FULL      If number of NNI interfaces specified is 
*                                 greater than the allowed number.
* @returns  L7_DEPENDENCY_NOT_MET If any one of the Interface in the specified
*                                 nniIntfList is not NNI 
* @returns  L7_FAILURE            If specified service VlanID is 
*                                 out of range or other failure
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t dot1adNniIntfListSet(L7_uint32 svid, L7_uint32 *nniIntfList, L7_uint32 nniIntfCnt);

/*********************************************************************
*
* @purpose  To get the NNI InterfaceList for a given dot1ad service.
*
* @param    svid          @b{(input)}  service VlanID of the 
*                                      given dot1ad service.
* @param    *nniInfList   @b{(output)} NNI Interface List
*                                      (1 to L7_CLI_MAX_STRING_LENGTH chars)
* @param    *nniIntfCnt   @b{(output)} Count of NNI Interfaces
*
* @returns  L7_SUCCESS    If  NNI InterfaceList is retrieved successfully.
* @returns  L7_ERROR      If Service with the specified service VlanID
*                         does not exist
* @returns  L7_FAILURE    If specified service VlanID is
*                         out of range or other failure
*
* @comments Caller must provide a nniIntfList buffer of at least
*           (L7_CLI_MAX_STRING_LENGTH+1) characters.
*
* @end
*
*********************************************************************/
L7_RC_t dot1adNniIntfListGet(L7_uint32 svid, L7_uint32 *nniIntfList, L7_uint32 *nniIntfCnt);

/*********************************************************************
*
* @purpose  Checks if the service name is a valid string
*
* @param    *name       @b{(input)} service instance name string
*
* @returns  L7_SUCCESS  valid service name string
* @returns  L7_FAILURE  invalid service name string
*
* @comments This function only checks the name string syntax for a service.
*           It does not check if a service currently contains this name.
*
* @end
*
*********************************************************************/
L7_RC_t dot1adServiceNameStringCheck(L7_char8 *name);

/*********************************************************************
*
* @purpose  To get the VlanId of a service instance, given its name.
*
* @param    *name  @b{(input)} service instance name
* @param    *svid  @b{(output)} service VlanID
*
* @returns  L7_SUCCESS     service instance index is retrieved
* @returns  L7_FAILURE     invalid parms, or other failure
* @returns  L7_ERROR       service instance does not exist
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1adServiceNameToSvid(L7_uchar8 *name, L7_uint32 *svid);

/*********************************************************************
*
* @purpose  To get the information if passed vlanid 
*           is configured as isolateVlanId
*
* @param    vlanId  @b{(input)} vlan id
* @param    *flag @b{(output)}  flag 
*
* @returns  L7_SUCCESS     service instance index is retrieved
* @returns  L7_FAILURE     invalid parms, or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1adServiceIsolateVidIsConfigured(L7_uint32 vlanId, L7_BOOL *flag);

/*********************************************************************
*
* @purpose  To get the information if svid is configured.
*
* @param    svid  @b{(input)} service vlan id
* @param    *flag @b{(output)} flag 
*
* @returns  L7_SUCCESS     service instance index is retrieved
* @returns  L7_FAILURE     invalid parms, or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1adServiceServiceVidIsConfigured(L7_uint32 svid, L7_BOOL *flag);

/*********************************************************************
*
* @purpose  Reserve current subscription structure for given internal 
*           interface number and dot1ad service VlanID.
*
* @param    intIfNum   @b{(input)} internal interface num
* @param    svid       @b{(input)} dot1ad service VlanID
*
* @returns  L7_SUCCESS     If current subscription is reserved.
* @returns  L7_FAILURE     invalid parms, or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1adReserveCurrentSubscription(L7_uint32 intIfNum, L7_uint32 svid);

/*********************************************************************
*
* @purpose  Release current subscription structure from given 
*           internal interface number and dot1ad service VlanID.
*
* @param    intIfNum   @b{(input)} internal interface num
* @param    svid       @b{(input)} dot1ad service VlanID
*
* @returns  L7_SUCCESS     If current subscription is released.
* @returns  L7_FAILURE     invalid parms, or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1adReleaseCurrentSubscription(L7_uint32 intIfNum, L7_uint32 svid);

/*********************************************************************
*
* @purpose  To set the Service VlanID for a dot1ad subscription.
*
* @param    intIfNum   @b{(input)} internal interface num
* @param    svid       @b{(input)} dot1ad service VlanID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1adSubscriptionSvidSet(L7_uint32 intIfNum, L7_uint32 svid);

/*********************************************************************
*
* @purpose  To set the resulting customer VlanID for a dot1ad subscription.
*
* @param    intIfNum    @b{(input)} internal interface num
* @param    cvid        @b{(input)} resulting customer VlanID.
*                                   This is specified as part of 
*                                   assign-cvid or remark-cvid actions.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1adSubscriptionCvidSet(L7_uint32 intIfNum, L7_uint32 cvid);

/*********************************************************************
*
* @purpose  To set the name for a dot1ad service subscription.
*
* @param    intIfNum   @b{(input)} internal interface num
* @param    svid       @b{(input)} dot1ad service VlanID
* @param    name       @b{(input)} subscription  name
*
* @returns  L7_SUCCESS     If subscription name is set successfully
* @returns  L7_FAILURE     subscription name already exists, or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1adSubscriptionNameSet(L7_uint32 intIfNum, L7_uint32 svid, L7_char8 *name);

/*********************************************************************
*
* @purpose  To get the name of a  dot1ad service subscription.
*
* @param    intIfNum   @b{(input)} internal interface num
* @param    svid       @b{(input)} dot1ad service VlanID
* @param    cvid       @b{(input)} Customer VlanID
* @param    name       @b{(output)} subscription  name
*
* @returns  L7_SUCCESS     
* @returns  L7_FAILURE  
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1adSubscriptionNameGet(L7_uint32 intIfNum, L7_uint32 svid, L7_uint32 cvid, L7_char8 *name);

/*********************************************************************
*
* @purpose  To set the matching Packet type for a dot1ad subscription.
*
* @param    intIfNum   @b{(input)} internal interface num
* @param    svid       @b{(input)} dot1ad service VlanID
* @param    pktType    @b{(input)} Matching Packet type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1adSubscriptionPktTypeSet(L7_uint32 intIfNum, L7_uint32 svid, L7_DOT1AD_PKTTYPE_t pktType);

/*********************************************************************
*
* @purpose  To get the matching Packet type for a dot1ad subscription.
*
* @param    intIfNum   @b{(input)} internal interface num
* @param    svid       @b{(input)} dot1ad service VlanID
* @param    cvid       @b{(input)} customer VlanID
* @param    pktType    @b{(output)} Matching Packet type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1adSubscriptionPktTypeGet(L7_uint32 intIfNum, L7_uint32 svid, L7_uint32 cvid, L7_DOT1AD_PKTTYPE_t *pktType);

/*********************************************************************
*
* @purpose  To set the action remove CTAG  for a dot1ad subscription.
*
* @param    intIfNum    @b{(input)} internal interface num
* @param    svid        @b{(input)} dot1ad service VlanID
* @param    removeCtag  @b{(input)} removeCtag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1adSubscriptionRemoveCtagSet(L7_uint32 intIfNum, L7_uint32 svid, L7_BOOL removeCtag);

/*********************************************************************
*
* @purpose  To get the remove CTAG action for a dot1ad subscription.
*
* @param    intIfNum   @b{(input)} internal interface num
* @param    svid       @b{(input)} dot1ad service VlanID
* @param    cvid       @b{(input)} customer VlanID
* @param    removeCtag @b{(output)} removeCtag
*                                   (L7_TRUE or L7_FALSE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1adSubscriptionRemoveCtagGet(L7_uint32 intIfNum, L7_uint32 svid, L7_uint32 cvid, L7_BOOL *removeCtag);

/*********************************************************************
*
* @purpose  To set the matching customer VlanID for a dot1ad subscription.
*
* @param    intIfNum   @b{(input)} internal interface num
* @param    svid       @b{(input)} dot1ad service VlanID
* @param    matchCvid  @b{(input)} Matching customer VlanID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1adSubscriptionMatchCvidSet(L7_uint32 intIfNum, L7_uint32 svid, L7_uint32 matchCvid);

/*********************************************************************
*
* @purpose  To get the matching customer VlanID for a dot1ad subscription.
*
* @param    intIfNum   @b{(input)} internal interface num
* @param    svid       @b{(input)} dot1ad service VlanID
* @param    cvid       @b{(input)} customer VlanID
* @param    matchCvid  @b{(output)} Matching customer VlanID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1adSubscriptionMatchCvidGet(L7_uint32 intIfNum, L7_uint32 svid, L7_uint32 cvid, L7_uint32 *matchCvid);

/*********************************************************************
*
* @purpose  To set the matching Service VlanID for a dot1ad subscription.
*
* @param    intIfNum   @b{(input)} internal interface num
* @param    svid       @b{(input)} dot1ad service VlanID
* @param    matchSvid  @b{(input)} Matching service VlanID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1adSubscriptionMatchSvidSet(L7_uint32 intIfNum, L7_uint32 svid, L7_uint32 matchSvid);

/*********************************************************************
*
* @purpose  To get the matching service VlanID for a dot1ad subscription.
*
* @param    intIfNum   @b{(input)} internal interface num
* @param    svid       @b{(input)} dot1ad service VlanID
* @param    cvid       @b{(input)} customer VlanID
* @param    matchSvid  @b{(output)} Matching service VlanID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1adSubscriptionMatchSvidGet(L7_uint32 intIfNum, L7_uint32 svid, L7_uint32 cvid, L7_uint32 *matchSvid);

/*********************************************************************
*
* @purpose  To set the matching dot1p priority for a dot1ad subscription.
*
* @param    intIfNum      @b{(input)} internal interface num
* @param    svid          @b{(input)} dot1ad service VlanID
* @param    matchPriority @b{(input)} Matching dot1p priority
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1adSubscriptionMatchPrioritySet(L7_uint32 intIfNum, L7_uint32 svid, L7_uint32 matchPriority);

/*********************************************************************
*
* @purpose  To get the matching dot1p priority for a dot1ad subscription.
*
* @param    intIfNum      @b{(input)} internal interface num
* @param    svid          @b{(input)} dot1ad service VlanID
* @param    cvid          @b{(input)} customer VlanID
* @param    matchPriority @b{(input)} Matching dot1p priority
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1adSubscriptionMatchPriorityGet(L7_uint32 intIfNum, L7_uint32 svid, L7_uint32 cvid, L7_uint32 *matchPriority);

/*********************************************************************
*
* @purpose  To set the NNI Interface List for a given dot1ad service
*           subscription.
*
* @param    intIfNum      @b{(input)} internal interface num
* @param    svid          @b{(input)} dot1ad service VlanID
* @param    cvid          @b{(input)} customer VlanID
* @param    nniIntfList   @b{(input)} NNI Interface List.
* @param    nniIntfCnt    @b{(input)} Number of NNI interfaces
*                                     specified in the nniIntfList.
*
* @returns  L7_SUCCESS    If NNI Intferface List is added
* @returns  L7_FAILURE    If any one of the Interface in the specified
*                         nniIntfList is not NNI other failure
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t dot1adSubscriptionNNIIntfListSet(L7_uint32 intIfNum, L7_uint32 svid, L7_uint32 *nniIntfList, L7_uint32 nniIntfCnt);

/*********************************************************************
*
* @purpose  To get the NNI InterfaceList for a given dot1ad service
*           subscription.
*
* @param    intIfNum      @b{(input)}  internal interface num
* @param    svid          @b{(input)}  service VlanID of the 
*                                      given dot1ad service.
* @param    cvid          @b{(input)}  customer VlanID
* @param    *nniInfList   @b{(output)} NNI Interface List
* @param    *nniIntfCnt   @b{(output)} Count of NNI Interfaces
*
* @returns  L7_SUCCESS    If  NNI InterfaceList is retrieved successfully.
* @returns  L7_FAILURE    
*
* @comments None
*
* @end
*
*********************************************************************/
L7_RC_t dot1adSubscriptionNNIIntfListGet(L7_uint32 intIfNum, L7_uint32 svid, L7_uint32 cvid, L7_uint32 *nniIntfList, L7_uint32 *nniIntfCnt);

/*********************************************************************
*
* @purpose  To check if a field is enabled in a given mask
*
* @param    mask      @b{(input)}  mask
* @param    field     @b{(input)}  field 
*
* @returns  L7_TRUE     
* @returns  L7_FALSE     
*
* @comments
*
* @end
*
*********************************************************************/
L7_BOOL dot1adIsFieldConfigured(L7_uint32 mask, L7_uint32 field);
    
/*********************************************************************
*
* @purpose  To add a subscription to a given dot1ad service on a 
*           given interface.
*
* @param    svid          @b{(input)}  service VlanID of the
*                                      given dot1ad service.
* @param    intIfNum      @b{(input)}  Internal Interface number.
*
* @returns  L7_SUCCESS  If adding a subscription is successfull.
* @returns  L7_NOT_SUPPORTED If NNI interface list is specified at
*                            subscription level for service
*                            subscriptions whose service type is not
*                            E-LINE.
* @returns   L7_DEPENDENCY_NOT_MET If any one of the Interface in the 
*                                 specified nniIntfList is not NNI.
* @returns  L7_ERROR    If service with the specified service VlanID
*                       does not exist.
* @returns  L7_REQUEST_DENIED If hardware update failed
* @returns  L7_TABLE_IS_FULL If number of specified NNI interfaces is greater
*                            than the allowed number.
* @returns  L7_FAILURE  For all other failures
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1adSubscriptionAdd(L7_uint32 intIfNum, L7_uint32 svid);

/*********************************************************************
*
* @purpose  To delete a dot1ad service subscription on an 
*           given interface  
*
* @param    intIfNum   @b{(input)} internal interface num
* @param    svid       @b{(input)} service VlanID of the
*                                  given dot1ad service.
* @param    subscrName @b{(input)} Name of the dot1ad service 
*                                  subscription.
*
* @returns  L7_SUCCESS  If the dot1ad service subscription is deleted. 
* @returns  L7_FAILURE  invalid service identifier, all other failures.
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1adSubscriptionDelete(L7_uint32 intIfNum, L7_uint32 svid, L7_uchar8* subscrName);

/*********************************************************************
*
* @purpose  To get the name of a dot1ad service subscription based on 
*           NNI interface.
*
* @param    intIfNum   @b{(input)} internal interface num
* @param    svid       @b{(input)} dot1ad service VlanID
* @param    cvid       @b{(input)} Customer VlanID
* @param    name       @b{(output)} subscription  name
*
* @returns  L7_SUCCESS     
* @returns  L7_FAILURE  
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1adNniSubscriptionNameGet(L7_uint32 intIfNum, L7_uint32 svid, L7_uint32 cvid, L7_char8 *name);

/*********************************************************************
*
* @purpose  To check if the subscription name is already configured on an interface
*
* @param    name            @b{(input)} subscription name
* @param    intIfNum        @b{(input)} interface num
*
* @returns  L7_TRUE         Subscription name is present
* @returns  L7_FALSE        Subscription name is not present
*
* @comments
*
* @end
*
*********************************************************************/
L7_BOOL dot1adSubscriptionNamePresent(L7_uint32 intIfNum, L7_uchar8 *name);


void dumpDebugIntfConfig(L7_uint32 intIfNum, L7_uint32 svid, L7_uint32 cvid);

L7_BOOL dot1adServiceSubscriptionCompatCheck(void *newSubscrEntry);
L7_RC_t dot1adSubscriptionEntryTreeSearch( L7_uint32 intfNum,
                             L7_uint32 svid,
                             L7_uint32 cvid,
                             L7_uint32 matchType,
                             dot1adSubscriptionStruct_t **subscrEntry);
L7_RC_t dot1adSubscriptionEntryTreeServiceTypeSearch(L7_uint32 intIfNum,
                             L7_uint32 svid,
                             L7_uint32 cvid,
                             L7_uint32 matchType,
                             dot1adSubscriptionStruct_t **subscrEntry);
L7_RC_t dot1adSubscriptionTreeFirstIntfEntryGet(L7_uint32 intIfNum, dot1adSubscriptionStruct_t **subscrEntry);
L7_RC_t dot1adSubscriptionTreeNextIntfEntryGet(dot1adSubscriptionStruct_t *subscrEntry, dot1adSubscriptionStruct_t **nextSubscrEntry);
L7_RC_t dot1adSubscriptionTreeNextEntryGet(dot1adSubscriptionStruct_t *subscrEntry, dot1adSubscriptionStruct_t **nextSubscrEntry);
L7_RC_t dot1adSubscriptionEntryTreeSearchSubName(L7_uint32 intIfNum,
                                                 L7_uint32 svid,
                                                 L7_uchar8 *subscrName, 
                                                 dot1adSubscriptionStruct_t **subscrEntry);
L7_RC_t dot1adSubscriptionEntryTreeSearchIntfSubName(L7_uint32 intIfNum,
                                                    L7_uchar8 *subscrName);

L7_RC_t dot1adNniSubscriptionEntryTreeSearch(L7_uint32 intIfNum,
                             L7_uint32 svid,
                             L7_uint32 cvid,
                             L7_uint32 matchType,
                             dot1adSubscriptionStruct_t **subscrEntry);

L7_RC_t dot1adSubscriptionEntryTreeSearchOnSvid(
                                  L7_uint32 svid,
                                  dot1adSubscriptionStruct_t **subscrEntry);
#endif
