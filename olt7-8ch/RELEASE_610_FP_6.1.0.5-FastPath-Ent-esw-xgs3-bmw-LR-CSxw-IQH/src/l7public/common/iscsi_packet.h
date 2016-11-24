/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename iscsi_packet.h
*
* @purpose iSCSI packet parsing definitions and prototypes
*
* @component iSCSI
*
* @comments none
*
* @create 05/18/2008
*
* @end
*
**********************************************************************/
#ifndef ISCSI_PKT_H
#define ISCSI_PKT_H

typedef struct L7_iscsi_basic_header_segment_s
{
  L7_uchar8      opcode;               /* 0 *//* Message opcode */
  L7_uchar8      opcodeSpecFields[3];  /* 1 */
  L7_uint32      totalAhsDataSegLength;/* 4 */
  L7_uchar8      isid[6];              /* 8 */
  L7_ushort16    tsih;                 /* 14 */
  L7_uint32      initiatorTaskTag;     /* 16 */
  L7_ushort16    cid;                  /* 20 */
  L7_uchar8      rsvd1[2];             /* 22 */
  L7_uint32      cmdSn;                /* 24 */
  L7_uchar8      rsvd2[20];            /* 28 */
  /* followed by DataSegment*/         /* 48 */
} L7_iscsi_basic_header_segment_t;

#define ISCSI_BHS_LENGTH   sizeof(L7_iscsi_basic_header_segment_t)

#define ISCSI_TOTAL_AHS_LENGTH(__iscsi_bhs_ptr__) \
    (((osapiNtohl(__iscsi_bhs_ptr__->totalAhsDataSegLength) & 0xff000000)>>24) * 4)    /* 32-bit word units (== 4 bytes) */

#define ISCSI_DATA_LENGTH(__iscsi_bhs_ptr__) \
    (osapiNtohl(__iscsi_bhs_ptr__->totalAhsDataSegLength) & 0x00ffffff)    

#define ISCSI_OPCODE_MASK(__iscsiOpcodeField__)  \
    (__iscsiOpcodeField__ & 0x3f)

#define ISCSI_LOGOUT_REQ_REASON_CODE_MASK(__iscsiReasonField__)  \
    (__iscsiReasonField__ & 0x7f)

#define ISCSI_OPCODE_LOGIN_REQ   0x03
#define ISCSI_OPCODE_LOGOUT_REQ  0x06

#define L7_ISCSI_LOGOUT_REQ_REASON_CLOSE_SESSION                  0
#define L7_ISCSI_LOGOUT_REQ_REASON_CLOSE_CONNECTION               1
#define L7_ISCSI_LOGOUT_REQ_REASON_CLOSE_CONNECTION_FOR_RECOVERY  2

#define L7_ISCSI_TARGET_NAME_KEY "TargetName="
#define L7_ISCSI_TARGET_NAME_KEY_LENGTH 11
#define L7_ISCSI_INITIATOR_NAME_KEY "InitiatorName="
#define L7_ISCSI_INITIATOR_NAME_KEY_LENGTH 14

#endif
