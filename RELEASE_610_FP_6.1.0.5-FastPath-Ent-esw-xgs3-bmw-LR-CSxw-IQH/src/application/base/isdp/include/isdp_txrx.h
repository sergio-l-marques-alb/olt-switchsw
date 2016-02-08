/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2007
*
**********************************************************************
* @filename  isdp_txrx.h
*
* @purpose   ISDP Transmit and Receive PDUs
*
* @component isdp
*
* @comments
*
* @create    12/11/2007
*
* @author    dgaryachy
*
* @end
*
**********************************************************************/
#ifndef ISDP_TXRX_H
#define ISDP_TXRX_H

#include "sysnet_api.h"
#include "datatypes.h"

#define ISDP_VERSION_1              1
#define ISDP_VERSION_2              2
#define ISDP_VERSION_CURRENT        ISDP_VERSION_2

#define ISDP_MAX_PDU_SIZE          (1024) /*bytes*/

#define ISDP_DEVICE_ID_TLV_TYPE     1
#define ISDP_ADDRESS_TLV_TYPE       2
#define ISDP_PORT_ID_TLV_TYPE       3
#define ISDP_CAPABILITIES_TLV_TYPE  4
#define ISDP_VERSION_TLV_TYPE       5
#define ISDP_PLATFORM_TLV_TYPE      6

/*LLC defines*/
#define ISDP_UNNUMBERED_FRAME       0x03
#define ISDP_DSAP_SNAP              0xaa
#define ISDP_SSAP_SNAP              0xaa

#define ISDP_NLPID                  1
#define ISDP_IP                     0xcc
#define ISDP_PROTO_LEN              1

typedef struct isdpHeader_s
{
  L7_uchar8               version;                  /* packet version */
  L7_uchar8               holdTime;                 /* packet ttl */
  L7_ushort16             checksum;                 /* IP checksum */
}isdpHeader_t;

typedef struct isdpTlvAddressByte_s
{
  L7_uchar8          protocol_type;         /* protocol type, always equals to 1 for FASTPATH */
  L7_uchar8          protocol_length;       /* protocol field length, always equals to 1 for FASTPATH*/
  L7_uchar8          protocol;              /* protocol, always equals to 0xCC — IP */
  L7_uchar8          address_length[2];     /* address field length */
  L7_uchar8          address[4];            /* IP v4 address */
}isdpTlvAddressByte_t;

typedef struct isdpTlvAddress_s
{
  L7_uchar8          protocol_type;         /* protocol type, always equals to 1 for FASTPATH */
  L7_uchar8          protocol_length;       /* protocol field length, always equals to 1 for FASTPATH*/
  L7_uchar8          protocol;              /* protocol, always equals to 0xCC — IP */
  L7_ushort16        address_length;        /* address field length */
  L7_uint32          address;               /* IP v4 address */
}isdpTlvAddress_t;

typedef struct isdpTlvTypeLength_s
{
  L7_ushort16        tlv_type;               /* TLV type */
  L7_ushort16        tlv_length;             /* TLV Length*/
}isdpTlvTypeLength_t;

/* Pool of buffers for managing IP addresses associated with a particular entry */
typedef struct isdpIpAddressBuffer_s
{
  L7_IP_ADDR_t                  ipAddr;
  struct isdpIpAddressBuffer_s  *next;
}isdpIpAddressBuffer_t;

typedef struct isdpPacket_s
{
  L7_enetHeader_t         ether;                               /* Ethernet header */
  L7_802_encaps_t         llc;                                 /* 802 header */
  isdpHeader_t            isdp_header;                         /* packet header */
  L7_uchar8               isdp_device_id[L7_ISDP_DEVICE_ID_LEN];    /* device id in ASCII, max length 30 */
  L7_uint32               isdp_address_number;                 /* Number of address TLVs */
  L7_uchar8               isdp_port_id[L7_ISDP_PORT_ID_LEN];      /* port id in ASCII, max length 30, populated with ifName */
  L7_uint32               isdp_capabilities;                   /* capabilities, populated only with 0x01 or 0x02 */
  L7_uchar8               isdp_version[L7_ISDP_VERSION_LEN];      /* version in ASCII */
  L7_uchar8               isdp_platform[L7_ISDP_PLATFORM_LEN];    /* platform in ASCII */
  isdpIpAddressBuffer_t   *ipAddressList;
}isdpPacket_t;

/***************************************************************************
 **************************FUNCTION PROTOTYPES******************************
 ***************************************************************************
 */
L7_RC_t isdpPduTransmit(L7_netBufHandle bufHandle,
              L7_uint32 intIfNum);
L7_RC_t isdpPduReceive(L7_netBufHandle bufHandle,
             sysnet_pdu_info_t *PduInfo);
L7_RC_t isdpTcnTx(L7_uint32 intIfNum, L7_uchar8 version);
L7_RC_t isdpTxRxInit(void);
L7_RC_t isdpTxRxDeinit(void);
L7_RC_t isdpTxRxHeaderTagRemove(L7_netBufHandle bufHandle);
L7_RC_t isdpTxRxHostConvert(L7_netBufHandle bufHandle, L7_uchar8 ** buff,
    L7_uint32 intIfNum);
L7_netBufHandle isdpTxRxNetworkConvert(L7_netBufHandle bufHandle,
    L7_uint32 intIfNum);
L7_RC_t isdpChecksum(void* data, size_t length, L7_ushort16* checksum);
L7_RC_t isdpV2CheckSum(void* data, size_t length, L7_ushort16* checksum);
L7_RC_t isdpTxRxHeaderTagRemove(L7_netBufHandle bufHandle);

#endif /*ISDP_TXRX_H*/
