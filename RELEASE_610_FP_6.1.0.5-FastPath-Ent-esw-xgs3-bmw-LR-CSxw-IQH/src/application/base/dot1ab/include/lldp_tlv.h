/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename lldp_tlv.h
*
* @purpose 802.1AB TLV defines and function prototypes
*
* @component 802.1AB
*
* @comments none
*
* @create 02/01/2005
*
* @author dfowler
* @end
*
**********************************************************************/

/*********************************************************************
 *
 ********************************************************************/
#ifndef LLDP_TLV_H
#define LLDP_TLV_H

#include "lldp_api.h"
#include "lldp.h"


#define LLDP_TLV_HEADER_SIZE            2  /* 7 bits type, 9 bits length */
#define LLDP_TLV_INFO_STRING_SIZE_MAX   511
#define LLDP_TLV_SIZE_MAX               (LLDP_TLV_HEADER_SIZE + \
                                         LLDP_TLV_INFO_STRING_SIZE_MAX)
#define LLDP_TLV_SUBTYPE_SIZE           1

#define LLDP_TLV_MGMT_STRING_SIZE_MAX       255
#define LLDP_TLV_MGMT_ADDR_SIZE_MAX         31
#define LLDP_TLV_MGMT_ADDR_OID_SIZE_MAX     128
#define LLDP_TLV_UNKNOWN_TLV_INFO_SIZE_MAX  511
#define LLDP_TLV_ORG_DEF_INFO_OUI_SIZE_MAX  3
#define LLDP_TLV_ORG_DEF_INFO_SIZE_MAX      507


#define LLDP_TLV_END_OF_LLDPDU_TLV_SIZE LLDP_TLV_HEADER_SIZE

#define LLDP_TLV_TTL_SIZE               2
#define LLDP_TLV_TTL_TLV_SIZE           (LLDP_TLV_HEADER_SIZE + \
                                         LLDP_TLV_TTL_SIZE)

#define LLDP_TLV_CHASSIS_ID_NTWK_ADDR_FAM_SIZE 1

#define LLDP_TLV_CHASSIS_ID_TLV_SIZE    (LLDP_TLV_HEADER_SIZE + \
                                         LLDP_TLV_SUBTYPE_SIZE + \
                                         LLDP_TLV_MGMT_STRING_SIZE_MAX)

#define LLDP_TLV_PORT_ID_TLV_SIZE       (LLDP_TLV_HEADER_SIZE + \
                                         LLDP_TLV_SUBTYPE_SIZE + \
                                         LLDP_TLV_MGMT_STRING_SIZE_MAX)

#define LLDP_TLV_PORT_DESC_TLV_SIZE     (LLDP_TLV_HEADER_SIZE + \
                                         LLDP_TLV_MGMT_STRING_SIZE_MAX)

#define LLDP_TLV_SYS_NAME_TLV_SIZE      (LLDP_TLV_HEADER_SIZE + \
                                         LLDP_TLV_MGMT_STRING_SIZE_MAX)

#define LLDP_TLV_SYS_DESC_TLV_SIZE      (LLDP_TLV_HEADER_SIZE + \
                                         LLDP_TLV_MGMT_STRING_SIZE_MAX)

#define LLDP_TLV_SYS_CAP_SIZE           2
#define LLDP_TLV_SYS_CAP_TLV_SIZE       (LLDP_TLV_HEADER_SIZE + \
                                         (2 * LLDP_TLV_SYS_CAP_SIZE))


#define LLDP_MED_TLV_MED_CAP_SIZE       3
#define LLDP_MED_TLV_MED_CAP_TLV_SIZE (LLDP_TLV_HEADER_SIZE +\
                     LLDP_TLV_ORG_DEF_INFO_OUI_SIZE_MAX +\
                     LLDP_TLV_SUBTYPE_SIZE +\
                     LLDP_MED_TLV_MED_CAP_SIZE)

#define LLDP_MED_TLV_NET_POLICY_SIZE    4
#define LLDP_MED_TLV_NET_POLICY_TLV_SIZE  (LLDP_TLV_HEADER_SIZE +\
                       LLDP_TLV_ORG_DEF_INFO_OUI_SIZE_MAX +\
                       LLDP_TLV_SUBTYPE_SIZE +\
                       LLDP_MED_TLV_NET_POLICY_SIZE)

#define LLDP_MED_TLV_NET_POLICY_TLVS_TX   1 /* Currently we transmit only voice vlan network policy */

#define LLDP_MED_TLV_LOCATION_SIZE_MAX  261
#define LLDP_MED_TLV_LOCATION_TLV_SIZE  (LLDP_TLV_HEADER_SIZE +\
                     LLDP_TLV_ORG_DEF_INFO_OUI_SIZE_MAX +\
                     LLDP_TLV_SUBTYPE_SIZE +\
                     LLDP_MED_TLV_LOCATION_SIZE_MAX)

#define LLDP_MED_TLV_EXT_POWER_SIZE   3
#define LLDP_MED_TLV_EXT_POWER_TLV_SIZE (LLDP_TLV_HEADER_SIZE +\
                     LLDP_TLV_ORG_DEF_INFO_OUI_SIZE_MAX +\
                     LLDP_TLV_SUBTYPE_SIZE +\
                     LLDP_MED_TLV_EXT_POWER_SIZE)

#define LLDP_MED_TLV_INVENTORY_SIZE   32
#define LLDP_MED_TLV_INVENTORY_TLV_SIZE (LLDP_TLV_HEADER_SIZE +\
                     LLDP_TLV_ORG_DEF_INFO_OUI_SIZE_MAX +\
                     LLDP_TLV_SUBTYPE_SIZE +\
                     LLDP_MED_TLV_INVENTORY_SIZE)
#define LLDP_MED_TLV_INVENTORY_TLV_TX   7


#define LLDP_TLV_MGMT_ADDR_SIZE_MAX     31
#define LLDP_TLV_MGMT_ADDR_OID_SIZE_MAX 128
#define LLDP_TLV_MGMT_ADDR_LEN_SIZE     1
#define LLDP_TLV_INTF_NUM_SIZE          4
#define LLDP_TLV_OID_LEN_SIZE           1
#define LLDP_TLV_MGMT_ADDR_TLV_SIZE     (LLDP_TLV_HEADER_SIZE + \
                                         LLDP_TLV_MGMT_ADDR_LEN_SIZE + \
                                         LLDP_TLV_SUBTYPE_SIZE + \
                                         LLDP_TLV_MGMT_ADDR_SIZE_MAX + \
                                         LLDP_TLV_SUBTYPE_SIZE + \
                                         LLDP_TLV_INTF_NUM_SIZE + \
                                         LLDP_TLV_OID_LEN_SIZE + \
                                         LLDP_TLV_MGMT_ADDR_OID_SIZE_MAX)


/* this is the largest LLDPDU we will transmit */
/* We currently do not support location or inventory TLVs hence their sizes are not included below */
#define LLDP_PDU_SIZE_MAX               (L7_ENET_HDR_SIZE + \
                                         L7_ENET_ENCAPS_HDR_SIZE + \
                                         LLDP_TLV_END_OF_LLDPDU_TLV_SIZE + \
                                         LLDP_TLV_TTL_TLV_SIZE + \
                                         LLDP_TLV_CHASSIS_ID_TLV_SIZE + \
                                         LLDP_TLV_PORT_ID_TLV_SIZE + \
                                         LLDP_TLV_PORT_DESC_TLV_SIZE + \
                                         LLDP_TLV_SYS_NAME_TLV_SIZE + \
                                         LLDP_TLV_SYS_DESC_TLV_SIZE + \
                                         LLDP_TLV_SYS_CAP_TLV_SIZE + \
                                         LLDP_TLV_MGMT_ADDR_TLV_SIZE + \
                     LLDP_MED_TLV_MED_CAP_TLV_SIZE + \
                     (LLDP_MED_TLV_NET_POLICY_TLV_SIZE * LLDP_MED_TLV_NET_POLICY_TLVS_TX) + \
                     LLDP_MED_TLV_LOCATION_TLV_SIZE + \
                     LLDP_MED_TLV_EXT_POWER_TLV_SIZE + \
                     (LLDP_MED_TLV_INVENTORY_TLV_SIZE * LLDP_MED_TLV_INVENTORY_TLV_TX))


typedef enum {
  LLDP_TLV_TYPE_END_OF_LLDPDU = 0,
  LLDP_TLV_TYPE_CHASSIS_ID = 1,
  LLDP_TLV_TYPE_PORT_ID = 2,
  LLDP_TLV_TYPE_TIME_TO_LIVE = 3,
  LLDP_TLV_TYPE_PORT_DESC = 4,
  LLDP_TLV_TYPE_SYS_NAME = 5,
  LLDP_TLV_TYPE_SYS_DESC = 6,
  LLDP_TLV_TYPE_SYS_CAP = 7,
  LLDP_TLV_TYPE_MGMT_ADDR = 8,
  LLDP_TLV_TYPE_RESERVED_BEGIN = 9,
  LLDP_TLV_TYPE_RESERVED_END = 126,
  LLDP_TLV_TYPE_ORG_DEF = 127
} lldpTLVType_t;


#define LLDP_TIA_OUI_STRING_LEN 3


typedef enum {
  LLDP_MED_SUBTYPE_CAP = 1,
  LLDP_MED_SUBTYPE_NET_POLICY = 2,
  LLDP_MED_SUBTYPE_LOCATION = 3,
  LLDP_MED_SUBTYPE_EXT_POWER = 4,
  LLDP_MED_SUBTYPE_INVENTORY_HW = 5,
  LLDP_MED_SUBTYPE_INVENTORY_FW = 6,
  LLDP_MED_SUBTYPE_INVENTORY_SW = 7,
  LLDP_MED_SUBTYPE_INVENTORY_SNO = 8,
  LLDP_MED_SUBTYPE_INVENTORY_MFG_NAME = 9,
  LLDP_MED_SUBTYPE_INVENTORY_MODEL_NAME = 10,
  LLDP_MED_SUBTYPE_INVENTORY_ASSET_ID = 11

}lldpMedTLVSubtype_t;
typedef enum
{
  LLDP_8023_SUBTYPE_RESERVED = 0,
  LLDP_8023_SUBTYPE_MAC_PHY = 1,
  LLDP_8023_SUBTYPE_POWER_MDI = 2,
  LLDP_8023_SUBTYPE_LINK_AGG = 3,
  LLDP_8023_SUBTYPE_MAX_FRAME_SIZE = 4,
  LLDP_8023_SUBTYPE_POE_STATEFUL_CONTROL = 5,
  LLDP_8023_SUBTYPE_POE_ADDL_STATUS = 6,
  
}lldp8023TLVSubtype_t;
typedef enum
{
  LLDP_8021_SUBTYPE_RESERVED = 0,
  LLDP_8021_SUBTYPE_PORT_VLANID = 1,
  LLDP_8021_SUBTYPE_PORT_PROTOCOL_VLANID = 2,
  LLDP_8021_SUBTYPE_VLAN_NAME = 3,
  LLDP_8021_SUBTYPE_PROTOCOL_ID = 4,
  
}lldp8021TLVSubtype_t;


typedef struct {
  L7_uchar8  *buffer;
  L7_uint32   offset;
} lldpPDUHandle_t;

typedef struct 
{
  L7_uchar8 capabilities[2];
  L7_uchar8 deviceType[1];

}lldpMedCapTLV_t;

#define LLDP_MED_NETWORK_POLICY_DSCP    0x00003F
#define LLDP_MED_NETWORK_POLICY_L2PRI     0x0001C0
#define LLDP_MED_NETWORK_POLICY_L2PRI_SHIFT 6
#define LLDP_MED_NETWORK_POLICY_VID     0x1FFE00
#define LLDP_MED_NETWORK_POLICY_VID_SHIFT 9
#define LLDP_MED_NETWORK_POLICY_X     0x20
#define LLDP_MED_NETWORK_POLICY_X_SHIFT   21
#define LLDP_MED_NETWORK_POLICY_T     0x40
#define LLDP_MED_NETWORK_POLICY_T_SHIFT   22
#define LLDP_MED_NETWORK_POLICY_U     0x80
#define LLDP_MED_NETWORK_POLICY_U_SHIFT   23

typedef struct
{
  L7_uchar8 appType[1];
  L7_uchar8 policyBits[3];
}lldpMedNetPolicyTLV_t;

typedef struct
{
  L7_uchar8 dataFormat[1];
  L7_uchar8 locID[16];

}lldpMedLocCoOrdTLV_t;

typedef struct
{
  L7_uchar8 dataFormat[1];
  L7_uchar8 locID[256];

}lldpMedLocCivicTLV_t;

typedef struct
{
  L7_uchar8 dataFormat[1];
  L7_uchar8 locID[25];

}lldpMedLocELINTLV_t;

#define LLDP_MED_EXT_POWER_PRIORITY         0x0F
#define LLDP_MED_EXT_POWER_PRIORITY_SHIFT      0
#define LLDP_MED_EXT_POWER_SOURCE           0x30
#define LLDP_MED_EXT_POWER_SOURCE_SHIFT        4
#define LLDP_MED_EXT_POWER_TYPE             0xC0
#define LLDP_MED_EXT_POWER_TYPE_SHIFT          6

typedef struct
{
  L7_uchar8 powerBits[1]; /*7:6 Power Type,5:4 Power Source,3:0 Power Priority */
  L7_uchar8 powerValue[2];
}lldpMedExtPwrTLV_t;

/* Device Type is a 2 bit Value -- Maximum Value is 3. If Spec changes adjust the shift accordingly */
typedef enum
{
  LLDP_MED_EXT_PWR_DEVICE_TYPE_PSE = 0,
  LLDP_MED_EXT_PWR_DEVICE_TYPE_PD = 1,
  LLDP_MED_EXT_PWR_DEVICE_TYPE_RSVD1 = 2,
  LLDP_MED_EXT_PWR_DEVICE_TYPE_RSVD2 = 3
}lldpMedExtPwrTLVDeviceType_t;
#define POWER_DEVICE_TYPE_PSE       (LLDP_MED_EXT_PWR_DEVICE_TYPE_PSE << LLDP_MED_EXT_POWER_TYPE_SHIFT)
#define POWER_DEVICE_TYPE_PD        (LLDP_MED_EXT_PWR_DEVICE_TYPE_PD << LLDP_MED_EXT_POWER_TYPE_SHIFT)
#define POWER_DEVICE_TYPE_RSVD1     (LLDP_MED_EXT_PWR_DEVICE_TYPE_RSVD1 << LLDP_MED_EXT_POWER_TYPE_SHIFT)
#define POWER_DEVICE_TYPE_RSVD2     (LLDP_MED_EXT_PWR_DEVICE_TYPE_RSVD2 << LLDP_MED_EXT_POWER_TYPE_SHIFT)

/* Power Source Type is a 2 bit Value -- Maximum Value is 3. If Spec changes adjust the shift accordingly */
typedef enum 
{
  LLDP_MED_EXT_PWR_SOURCE_UNKNOWN = 0,
  LLDP_MED_EXT_PWR_SOURCE_PSE = 1,
  LLDP_MED_EXT_PWR_SOURCE_PRIMARY = 1,
  LLDP_MED_EXT_PWR_SOURCE_LOCAL =2,
  LLDP_MED_EXT_PWR_SOURCE_BACKUP =2,
  LLDP_MED_EXT_PWR_SOURCE_PSE_AND_LOCAL = 3
}lldpMedExtPwrTLVSourceType_t;
#define POWER_SOURCE_UNKNOWN        (LLDP_MED_EXT_PWR_SOURCE_UNKNOWN << LLDP_MED_EXT_POWER_SOURCE_SHIFT)
#define POWER_SOURCE_PSE            (LLDP_MED_EXT_PWR_SOURCE_PSE << LLDP_MED_EXT_POWER_SOURCE_SHIFT)
#define POWER_SOURCE_PRIMARY        (LLDP_MED_EXT_PWR_SOURCE_PRIMARY << LLDP_MED_EXT_POWER_SOURCE_SHIFT)
#define POWER_SOURCE_LOCAL          (LLDP_MED_EXT_PWR_SOURCE_LOCAL << LLDP_MED_EXT_POWER_SOURCE_SHIFT)
#define POWER_SOURCE_BACKUP         (LLDP_MED_EXT_PWR_SOURCE_BACKUP << LLDP_MED_EXT_POWER_SOURCE_SHIFT)
#define POWER_SOURCE_PSE_AND_LOCAL  (LLDP_MED_EXT_PWR_SOURCE_PSE_AND_LOCAL << LLDP_MED_EXT_POWER_SOURCE_SHIFT)

/* Power Priority is a 4 bit Value -- Maximum Value is 15. If Spec changes adjust the shift accordingly */
typedef enum
{
  LLDP_MED_EXT_PWR_PRIORITY_UNKNOWN = 0,
  LLDP_MED_EXT_PWR_PRIORITY_CRITICAL = 1,
  LLDP_MED_EXT_PWR_PRIORITY_HIGH = 2,
  LLDP_MED_EXT_PWR_PRIORITY_LOW = 3,
  LLDP_MED_EXT_PWR_PRIORITY_RSVDL = 4,
  LLDP_MED_EXT_PWR_PRIORITY_RSVDH = 15,
}lldpMedExtPwrTLVPriority_t;
#define POWER_PRIORITY_UNKNOWN      (LLDP_MED_EXT_PWR_PRIORITY_UNKNOWN << LLDP_MED_EXT_POWER_PRIORITY_SHIFT)
#define POWER_PRIORITY_CRITICAL     (LLDP_MED_EXT_PWR_PRIORITY_CRITICAL << LLDP_MED_EXT_POWER_PRIORITY_SHIFT)
#define POWER_PRIORITY_HIGH         (LLDP_MED_EXT_PWR_PRIORITY_HIGH << LLDP_MED_EXT_POWER_PRIORITY_SHIFT)
#define POWER_PRIORITY_LOW          (LLDP_MED_EXT_PWR_PRIORITY_LOW << LLDP_MED_EXT_POWER_PRIORITY_SHIFT)

#define LLDP_8023_STATEFUL_CNTRL_ACKNOWLEDGE              0xFF
#define LLDP_8023_STATEFUL_CNTRL_ACKNOWLEDGE_SHIFT           0
typedef enum 
{
  LLDP_8023_POE_STATEFUL_CNTRL_ACKNOWLEDGE_NOTPART = 0,
  LLDP_8023_POE_STATEFUL_CNTRL_ACKNOWLEDGE_ACK = 1,
  LLDP_8023_POE_STATEFUL_CNTRL_ACKNOWLEDGE_NACK = 2,
}lldp8023StatusCntlTLVAckType_t;
#define POWER_TLVACK_IGNORE (LLDP_8023_POE_STATEFUL_CNTRL_ACKNOWLEDGE_NOTPART << LLDP_8023_STATEFUL_CNTRL_ACKNOWLEDGE_SHIFT )
#define POWER_TLVACK_ACK (LLDP_8023_POE_STATEFUL_CNTRL_ACKNOWLEDGE_ACK << LLDP_8023_STATEFUL_CNTRL_ACKNOWLEDGE_SHIFT)
#define POWER_TLVACK_NACK (LLDP_8023_POE_STATEFUL_CNTRL_ACKNOWLEDGE_NACK << LLDP_8023_STATEFUL_CNTRL_ACKNOWLEDGE_SHIFT)

typedef struct
{
  L7_uchar8 data[32];
}lldpMedInventoryTLV_t;

#define LLDP_8023_MAC_PHY_AUTO_NEG_SUP    0x01
#define LLDP_8023_MAC_PHY_AUTO_NEG_STATUS   0x02

typedef struct 
{
  L7_uchar8 autoNegSupStatus[1];
  L7_uchar8 autoNegAdvt[2];
  L7_uchar8 operMAUType[2];

}lldp8023MacPhyTLV_t;

typedef struct 
{
  L7_uchar8 reqPowerBits[1];    /* 7:6- Power Type, 5:4- Power source, 3:0- Power priority */
  L7_uchar8 reqPowerValue[2];   /* Requested Power Value */
  L7_uchar8 powerAck[1];        /* Power Acknowledge */

}lldp8023StatefulCntlTLV_t;


/*********************************************************************
*
* @purpose  Start an output LLDPDU buffer
*
* @param    lldpPDUHandle_t pduHdl    @b((input)) pointer to LLDP PDU type.
* @param    L7_uint32       intIfNum  @b((input)) NIM internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpPDUStart(lldpPDUHandle_t *pduHdl, L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  End an output LLDPDU buffer
*
* @param    lldpPDUHandle_t pduHdl  @b((input)) pointer to LLDP PDU type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpPDUEnd(lldpPDUHandle_t *pduHdl);

/*********************************************************************
*
* @purpose  Write a Time to Live TLV to a LLDPPDU
*
* @param    L7_ushort16        ttl       @b((input)) time to live seconds
* @param    lldpPDUHandle_t    *pduHdl   @b((input)) pointer to LLDP PDU type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad arg or not enough room for TLV
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpTLVTTLWrite(L7_ushort16      ttl,
                        lldpPDUHandle_t *pduHdl);

/*********************************************************************
*
* @purpose  Write a Chassis ID TLV to a LLDPPDU
*
* @param    L7_uchar8         *subtype @b((input)) Chassis ID subtype
* @param    L7_uchar8         *id      @b((input)) The Chassis ID
* @param    L7_ushort16       length   @b((input)) The Chassis ID length
* @param    lldpPDUHandle_t   *pduHdl  @b((input)) pointer to LLDP PDU type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad arg or not enough room for TLV
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpTLVChassisIdWrite(L7_uchar8          subtype,
                              L7_uchar8         *id,
                              L7_ushort16        length,
                              lldpPDUHandle_t   *pduHdl);

/*********************************************************************
*
* @purpose  Write a Port ID TLV to a LLDPPDU
*
* @param    L7_uchar8         subtype  @b((input)) Port ID subtype
* @param    L7_uchar8         *id      @b((input)) The Port ID
* @param    L7_ushort16       length   @b((input)) The Port ID length
* @param    lldpPDUHandle_t   *pduHdl  @b((input)) pointer to LLDP PDU type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad arg or not enough room for TLV
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpTLVPortIdWrite(L7_uchar8              subtype,
                           L7_uchar8             *id,
                           L7_ushort16            length,
                           lldpPDUHandle_t       *pduHdl);

/*********************************************************************
*
* @purpose  Write a Port Description TLV to a LLDPPDU
*
* @param    L7_char8         *desc   @b((input)) The Port Description
* @param    lldpPDUHandle_t  *pduHdl @b((input)) pointer to LLDP PDU type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad arg or not enough room for TLV
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpTLVPortDescWrite(L7_char8             *desc,
                             lldpPDUHandle_t      *pduHdl);

/*********************************************************************
*
* @purpose  Write a System Name TLV to a LLDPPDU
*
* @param    L7_char8           *name   @b((input)) The system name
* @param    lldpPDUHandle_t    *pduHdl @b((input)) pointer to LLDP PDU type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad arg or not enough room for TLV
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpTLVSysNameWrite(L7_char8        *name,
                            lldpPDUHandle_t *pduHdl);

/*********************************************************************
*
* @purpose  Write a System Description TLV to a LLDPPDU
*
* @param    L7_char8           *desc   @b((input)) The system description
* @param    L7_ushort16        length  @b((input)) system description length
* @param    lldpPDUHandle_t    *pduHdl @b((input)) pointer to LLDP PDU type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad arg or not enough room for TLV
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpTLVSysDescWrite(L7_char8        *desc,
                            lldpPDUHandle_t *pduHdl);

/*********************************************************************
*
* @purpose  Write a System Capabilities TLV to a LLDPPDU
*
* @param    L7_ushort16      supported  @b((input)) system capabilities supported mask
* @param    L7_ushort16      enabled    @b((input)) system capabilities enabled mask
* @param    lldpPDUHandle_t  *pduHdl    @b((input)) pointer to LLDP PDU type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad arg or not enough room for TLV
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpTLVSysCapWrite(L7_ushort16      supported,
                           L7_ushort16      enabled,
                           lldpPDUHandle_t *pduHdl);

/*********************************************************************
*
* @purpose  Write a Management Address TLV to a LLDPPDU
*
* @param    lldpIANAAddrFamilyNumber_t  addrFamily  @b((input)) IANA address family (IPv4, etc.)
* @param    L7_uchar8                   *addr       @b((input)) management address
* @param    L7_ushort16                 *addrLen    @b((input)) management address length
* @param    lldpManAddrIfSubtype_t      intfSubtype @b((input)) management intf subtype
* @param    L7_uint32                   intfNum     @b((input)) management interface number
* @param    L7_char8                    oid         @b((input)) management OID
* @param    lldpPDUHandle_t             *pduHdl     @b((input)) pointer to LLDP PDU type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad arg or not enough room for TLV
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpTLVMgmtAddrWrite(lldpIANAAddrFamilyNumber_t addrFamily,
                             L7_uchar8                 *addr,
                             L7_ushort16                addrLen,
                             lldpManAddrIfSubtype_t     intfSubtype,
                             L7_uint32                  intfNum,
                             L7_char8                  *oid,
                             lldpPDUHandle_t           *pduHdl);

/*********************************************************************
*
* @purpose  Read TLV Header from buffer
*
* @param    lldpTLVType_t   *type       @b((output)) LLDP TLV type
* @param    L7_ushort16     *length     @b((output)) length of information string
* @param    lldpPDUHandle_t *pduHdl     @b((input))  pointer to LLDP PDU type.
* @param    L7_uint32        pduLength  @b((input))  total length of PDU buffer.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad TLV, or end of LLDPDU
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpTLVHeaderRead(lldpTLVType_t   *type,
                          L7_ushort16     *length,
                          lldpPDUHandle_t *pduHdl,
                          L7_uint32        pduLength);

/*********************************************************************
*
* @purpose  Read Chassis ID TLV
*
* @param    L7_uchar8             *subtype  @b((output)) remote Chassis ID subtype
* @param    L7_uchar8             *id       @b((output)) remote Chassis ID
* @param    L7_ushort16           *idLength @b((output)) remote Chassis ID length
* @param    lldpPDUHandle_t       *pduHdl   @b((input))  pointer to LLDP PDU type.
* @param    L7_uint32             tlvLength @b((input))  length of information string.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad TLV
*
* @comments id buffer must be at least LLDP_MGMT_STRING_SIZE_MAX
*
* @end
*
*********************************************************************/
L7_RC_t lldpTLVChassisIdRead(L7_uchar8         *subtype,
                             L7_uchar8         *id,
                             L7_ushort16       *idLength,
                             lldpPDUHandle_t   *pduHdl,
                             L7_uint32          tlvLength);

/*********************************************************************
*
* @purpose  Read Port ID TLV
*
* @param    L7_uchar8             *subtype  @b((output)) remote Port ID subtype
* @param    L7_uchar8             *id       @b((output)) remote Chassis ID
* @param    L7_ushort16           *idLength @b((output)) remote Chassis ID length
* @param    lldpPDUHandle_t       *pduHdl   @b((input))  pointer to LLDP PDU type.
* @param    L7_uint32             tlvLength @b((input))  length of information string.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad TLV
*
* @comments id buffer must be at least LLDP_MGMT_STRING_SIZE_MAX
*
* @end
*
*********************************************************************/
L7_RC_t lldpTLVPortIdRead(L7_uchar8            *subtype,
                          L7_char8             *id,
                          L7_ushort16          *idLength,
                          lldpPDUHandle_t      *pduHdl,
                          L7_uint32             tlvLength);

/*********************************************************************
*
* @purpose  Read Time-to-live TLV
*
* @param    L7_uint32             *ttl      @b((output)) remote time to live
* @param    lldpPDUHandle_t       *pduHdl   @b((input))  pointer to LLDP PDU type.
* @param    L7_uint32             length    @b((input))  length of information string.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad TLV
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpTLVTTLRead(L7_uint32        *ttl,
                       lldpPDUHandle_t  *pduHdl,
                       L7_uint32         length);

/*********************************************************************
*
* @purpose  Read port description TLV
*
* @param    L7_char8              *portDesc @b((output)) remote port description
* @param    lldpPDUHandle_t       *pduHdl   @b((input))  pointer to LLDP PDU type.
* @param    L7_uint32             length    @b((input))  length of information string.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad TLV
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpTLVPortDescRead(L7_char8         *portDesc,
                            lldpPDUHandle_t  *pduHdl,
                            L7_uint32         length);

/*********************************************************************
*
* @purpose  Read system name TLV
*
* @param    L7_char8              *sysName  @b((output)) remote system name
* @param    lldpPDUHandle_t       *pduHdl   @b((input))  pointer to LLDP PDU type.
* @param    L7_uint32             length    @b((input))  length of information string.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad TLV
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpTLVSysNameRead(L7_char8         *sysName,
                           lldpPDUHandle_t  *pduHdl,
                           L7_uint32         length);

/*********************************************************************
*
* @purpose  Read system description TLV
*
* @param    L7_char8              *sysDesc  @b((output)) remote system description
* @param    lldpPDUHandle_t       *pduHdl   @b((input))  pointer to LLDP PDU type.
* @param    L7_uint32             length    @b((input))  length of information string.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad TLV
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpTLVSysDescRead(L7_char8         *sysDesc,
                           lldpPDUHandle_t  *pduHdl,
                           L7_uint32         length);

/*********************************************************************
*
* @purpose  Read system capabilities TLV
*
* @param    L7_ushort16        *supported @b((output)) system capabilities supported
* @param    L7_ushort16        *enabled   @b((output)) system capabilities enabled
* @param    lldpPDUHandle_t    *pduHdl    @b((input))  pointer to LLDP PDU type.
* @param    L7_uint32           length    @b((input))  length of information string.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad TLV
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpTLVSysCapRead(L7_ushort16      *supported,
                          L7_ushort16      *enabled,
                          lldpPDUHandle_t  *pduHdl,
                          L7_uint32         length);

/*********************************************************************
*
* @purpose  Read management address TLV
*
* @param    lldpMgmtAddrEntry_t *entry     @b((output))  remote management addr entry
* @param    lldpPDUHandle_t     *pduHdl    @b((input))   pointer to LLDP PDU type.
* @param    L7_uint32            length    @b((input))   length of information string.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad TLV
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpTLVMgmtAddrRead(lldpMgmtAddrEntry_t *entry,
                            lldpPDUHandle_t     *pduHdl,
                            L7_uint32            length);

/*********************************************************************
*
* @purpose  Read organizationally defined TLV
*
* @param    lldpOrgDefInfoEntry_t *entry     @b((output)) remote organizationally defined TLV
* @param    lldpPDUHandle_t       *pduHdl    @b((input))  pointer to LLDP PDU type.
* @param    L7_uint32             length     @b((input))  length of information string.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad TLV
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpTLVOrgDefInfoRead(lldpOrgDefInfoEntry_t *entry,
                              lldpPDUHandle_t       *pduHdl,
                              L7_uint32              length);

/*********************************************************************
*
* @purpose  Read unknown TLV
*
* @param    lldpUnknownTLVEntry_t *entry     @b((output)) remote unknown TLV
* @param    lldpPDUHandle_t       *pduHdl    @b((input))  pointer to LLDP PDU type.
* @param    L7_uint32             length     @b((input))  length of information string.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad TLV
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpTLVUnknownRead(lldpUnknownTLVEntry_t *entry,
                           lldpPDUHandle_t       *pduHdl,
                           L7_uint32              length);
void lldpMedTLVsWrite(L7_uint32 index,
            L7_uint32 intIfNum,
            lldpPDUHandle_t *pduHdl);
L7_RC_t lldpMedTLVWrite(lldpTLVType_t      type,
            L7_uchar8          subtype,
            L7_uchar8         *id,
            L7_ushort16        length,
            lldpPDUHandle_t   *pduHdl);
L7_RC_t lldpMedTLVHeaderWrite(lldpTLVType_t    type,
                              L7_ushort16      length,
                              lldpPDUHandle_t *pduHdl);
L7_RC_t lldp8023TLVsWrite(L7_uint32 index, L7_uint32 intIfNum, lldpPDUHandle_t* pduHdl);
L7_RC_t lldp8023MacPhyTLVWrite(L7_uint32 index, L7_uint32 intIfNum, lldpPDUHandle_t* pduHdl);
L7_RC_t lldpMedCapTLVWrite(L7_uint32 index, L7_uint32 intIfNum, lldpPDUHandle_t* pduHdl);
L7_RC_t lldpMedNetPolicyTLVWrite(L7_uint32 index, L7_uint32 intIfNum, lldpPDUHandle_t* pduHdl);
L7_RC_t lldpMedLocTLVWrite(L7_uint32 index, L7_uint32 intIfNum, lldpPDUHandle_t* pduHdl);
L7_RC_t lldpMedPSETLVWrite(L7_uint32 index, L7_uint32 intIfNum, lldpPDUHandle_t* pduHdl);
L7_RC_t lldpMedPDTLVWrite(L7_uint32 index, L7_uint32 intIfNum, lldpPDUHandle_t* pduHdl);
L7_RC_t lldpMedInventoryTLVsWrite(L7_uint32 index, L7_uint32 intIfNum, lldpPDUHandle_t* pduHdl);
L7_RC_t lldp8023TLVWrite(lldpTLVType_t      type,
             L7_uchar8          subtype,
             L7_uchar8         *id,
             L7_ushort16        length,
             lldpPDUHandle_t   *pduHdl);
L7_RC_t lldp8023TLVHeaderWrite(lldpTLVType_t    type,
                               L7_ushort16      length,
                               lldpPDUHandle_t *pduHdl);
/*********************************************************************
*
* @purpose  Write 8023 Stateful Control TLV
*
* @param    L7_uint32       index @((input)) index to the port
* @param    L7_uint32       intIfNum @((input)) Internal Interface Number
* @param    lldpPDUHandle_t *pduHdl @b((input)) pointer to LLDP PDU type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldp8023StatefulCntlTLVWrite(L7_uint32 index, L7_uint32 intIfNum, 
                                     lldpPDUHandle_t* pduHdl);

#endif /* LLDP_TLV_H */
