/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2009
*
**********************************************************************
*
* @filename   cp_ckpt.h
*
* @purpose    Captive Portal Checkpoint Data Header File
*
* @component  Captive Portal
*
* @comments   None
*
* @create     04/22/2009
*
* @author     rjindal
*
* @end
*
*********************************************************************/

#ifndef INCLUDE_CP_CKPT_H
#define INCLUDE_CP_CKPT_H

#include "datatypes.h"
#include "ckpt_api.h"

/* CP Ckpt support message type */
typedef enum cpCkptMsgType_s
{
  CP_CKPT_MSG = 1
} cpCkptMsgType_t;

/* CP Ckpt element size in bytes */
#define CP_CKPT_IE_MAC_SIZE         6
#define CP_CKPT_IE_IPv4_SIZE        4
#define CP_CKPT_IE_INT32_SIZE       4
#define CP_CKPT_IE_INT8_SIZE        1

/* CP Ckpt support message header */
typedef struct cpCkptMsgHdr_s
{
  L7_ushort16   msgType;
  L7_ushort16   msgLen;
} cpCkptMsgHdr_t;

/* Message header size in bytes */
#define CP_CKPT_MSG_TYPE_SIZE     2
#define CP_CKPT_MSG_LEN_SIZE      2
#define CP_CKPT_MSG_HDR_SIZE      (CP_CKPT_MSG_TYPE_SIZE + CP_CKPT_MSG_LEN_SIZE)

/* CP Ckpt support message element header */
typedef struct cpCkptMsgElementHdr_s
{
  L7_ushort16   elementId;
  L7_ushort16   elementLen;
} cpCkptMsgElementHdr_t;

/* IE header size in bytes */
#define CP_CKPT_IE_ID_SIZE        CP_CKPT_MSG_TYPE_SIZE
#define CP_CKPT_IE_LEN_SIZE       CP_CKPT_MSG_LEN_SIZE
#define CP_CKPT_IE_HDR_SIZE       CP_CKPT_MSG_HDR_SIZE


/* CP-Ckpt IE type enumeration */
typedef enum
{
  CP_CKPT_IE_TYPE_CLIENT_MAC_ADDRESS = 1,
  CP_CKPT_IE_TYPE_DATA               = 2
} cpCkptIEType_t;

/* CP-Ckpt message IE data size in bytes */
#define CP_CKPT_IE_CLIENT_MAC_ADDRESS_SIZE  L7_ENET_MAC_ADDR_LEN
#define CP_CKPT_IE_DATA_SIZE                49

#define CP_CKPT_MSG_TOTAL_IE_COUNT          2

/* CP-Ckpt message IE total size in bytes */
#define CP_CKPT_MSG_TOTAL_IE_SIZE   ((CP_CKPT_IE_HDR_SIZE * CP_CKPT_MSG_TOTAL_IE_COUNT) +\
                                     (CP_CKPT_IE_CLIENT_MAC_ADDRESS_SIZE) +\
                                     (CP_CKPT_IE_DATA_SIZE))
/* CP-Ckpt message size in bytes */
#define CP_CKPT_MSG_SIZE            (CP_CKPT_MSG_HDR_SIZE + CP_CKPT_MSG_TOTAL_IE_SIZE)


/* CP Ckpt element and message header macros */

#define CP_CKPT_ELEMENT_HDR_SET(element, type, len, buffer, disp)   \
                do { \
                  element.elementId = osapiHtons(type); \
                  element.elementLen = osapiHtons(len); \
                  memcpy((buffer+disp), &element, sizeof(cpCkptMsgElementHdr_t)); \
                  disp += CP_CKPT_IE_HDR_SIZE; \
                } while (0);


/*********************************************************************
                MANAGEMENT UNIT FUNCTIONS
*********************************************************************/
void cpCkptManagerCallback(L7_CKPT_MGR_CALLBACK_t cbType,
                           L7_uchar8 *ckptData, L7_uint32 bufLen,
                           L7_uint32 *msgLen, L7_BOOL *moreData);
void cpCkptNewBackupManager(void);
void cpCkptMsgWrite(L7_uchar8 *ckptData, L7_uint32 bufLen,
                    L7_uint32 *msgLen, L7_BOOL *moreData);
void cpCkptBackupManagerGone(void);
void cpCkptClientInfoCkpt(void);


/*********************************************************************
                BACKUP UNIT FUNCTIONS
*********************************************************************/
void cpCkptBackupCallback(L7_CKPT_BACKUP_MGR_CALLBACK_t cbType,
                          L7_uchar8 *ckptData, L7_uint32 msgLen);
void cpCkptMsgProcess(L7_uchar8 *ckptData, L7_uint32 msgLen);
void cpCkptDataPurge(void);
void cpCkptDataApply(L7_BOOL *dataExists);

#endif

