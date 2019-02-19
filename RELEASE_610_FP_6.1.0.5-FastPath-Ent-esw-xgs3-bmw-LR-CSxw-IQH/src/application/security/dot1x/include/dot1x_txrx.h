/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  dot1x_txrx.h
*
* @purpose   dot1x Transmit and Receive PDUs include file
*
* @component dot1x
*
* @comments  none
*
* @create    04/08/2003
*
* @author    mfiorito
*
* @end
*             
**********************************************************************/
#ifndef INCLUDE_DOT1X_TXRX_H
#define INCLUDE_DOT1X_TXRX_H

#define DOT1X_MAX_PDU_SIZE        (FD_NIM_DEFAULT_MTU_SIZE) /*bytes*/

/* EAPOL Packet types */
#define EAPOL_EAPPKT      0  /* 0000 0000B */
#define EAPOL_START       1  /* 0000 0001B */
#define EAPOL_LOGOFF      2  /* 0000 0010B */
#define EAPOL_KEY         3  /* 0000 0011B */
#define EAPOL_ENCASFALERT 4  /* 0000 0100B */

/* EAPOL packet header */
typedef struct L7_eapolPacket_s
{
  L7_uchar8   protocolVersion;
  L7_uchar8   packetType;
  L7_ushort16 packetBodyLength;
} L7_eapolPacket_t;

/* Length defines for EAPOL-Key frame */
#define EAPOL_KEY_REPLAY_COUNTER_LEN    8
#define EAPOL_KEY_IV_LEN                16
#define EAPOL_KEY_SIGNATURE_LEN         16

/* Bitmask defines for keyIndex field in EAPOL-Key frame */
#define EAPOL_KEY_UNICAST  0x80  /* first bit */
#define EAPOL_KEY_INDEX    0x7F  /* last 7 bits */

/* EAPOL-Key packet format */
typedef struct L7_eapolKeyPacket_s
{
  L7_uchar8   descriptorType;
  L7_ushort16 keyLength;
  L7_uchar8   replayCounter[EAPOL_KEY_REPLAY_COUNTER_LEN];
  L7_uchar8   keyIV[EAPOL_KEY_IV_LEN];
  L7_uchar8   keyIndex;
  L7_uchar8   keySignature[EAPOL_KEY_SIGNATURE_LEN];
} L7_eapolKeyPacket_t;

/* EAP Packet code types */
#define EAP_REQUEST   1
#define EAP_RESPONSE  2
#define EAP_SUCCESS   3
#define EAP_FAILURE   4

/* EAP packet header */
typedef struct L7_eapPacket_s
{
  L7_uchar8   code;
  L7_uchar8   id;
  L7_ushort16 length;
} L7_eapPacket_t;

/* Defines for 'type' field of EAP Request and Response frames */
#define EAP_RRIDENTITY  1  /* Request/Identity or Response/Identity */
#define EAP_RRNOTIF     2  /* Notification */
#define EAP_RRNAK       3  /* NAK (Response only) */
#define EAP_RRMD5       4  /* MD5-Challenge */
#define EAP_RROTP       5  /* One-Time Password */
#define EAP_RRGTK       6  /* Generic Token Card */
#define EAP_TLS         13 /* EAP/TLS */
#define EAP_TTLS        21 /* EAP/TTLS */
#define EAP_PEAP        25 /* EAP/PEAP */

/* EAP Request/Response packet header */
typedef struct L7_eapRrPacket_s
{
  L7_uchar8   type;
} L7_eapRrPacket_t;


extern L7_RC_t dot1xTxRxInit(void);
extern L7_RC_t dot1xPduReceive(L7_netBufHandle bufHandle, sysnet_pdu_info_t *pduInfo);
extern void dot1xTxRxHeaderTagRemove(L7_netBufHandle bufHandle);

extern void dot1xTxReqId(L7_uint32 intIfNum, dot1xPortType_t portType);
extern void dot1xTxReq(dot1xLogicalPortInfo_t *logicalPortInfo, L7_netBufHandle bufHandle);
extern void dot1xTxCannedSuccess(L7_uint32 intIfNum, dot1xPortType_t portType);
extern void dot1xTxCannedFail(L7_uint32 intIfNum, dot1xPortType_t portType);
extern void dot1xTxMd5Challenge(dot1xLogicalPortInfo_t *logicalPortInfo);
extern void dot1xFrameTransmit(L7_uint32 intIfNum, L7_netBufHandle bufHandle,dot1xPortType_t portType);
extern L7_RC_t dot1xTxRxHostConvert(L7_netBufHandle bufHandle);
extern L7_RC_t dot1xSendRespToServer(L7_uint32 intIfNum, L7_netBufHandle bufHandle);
extern L7_RC_t dot1xRadiusSendNotification(L7_uint32 intIfNum,
                                           L7_uchar8 *radiusPayload,
                                           L7_uint32 payloadLen);
extern L7_RC_t dot1xTxReqIdCheck(L7_uint32 intIfNum);
extern void dot1xTxStart(L7_uint32 intIfNum);
extern void dot1xTxSuppRsp(L7_uint32 intIfNum);

#endif /* INCLUDE_DOT1X_TXRX_H */
