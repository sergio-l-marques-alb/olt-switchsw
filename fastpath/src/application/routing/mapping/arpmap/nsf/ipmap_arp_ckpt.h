/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
**********************************************************************
*
* @filename   ipmap_arp_ckpt.h
*
* @purpose    ARP layer internal function prototypes for checkpoint functions
*
* @component  ARP MAP
*
* @comments   none
*
* @create     02/05/2009
*
* @author     Kiran Kumar Kella
* @end
*
**********************************************************************/


#ifndef INCLUDE_IPMAP_ARP_CKPT_H
#define INCLUDE_IPMAP_ARP_CKPT_H

#include "datatypes.h"

L7_RC_t arpMapCkptTableCreate(void);
L7_RC_t arpMapCkptCallbacksRegister(void);
void arpMapNewBackupManager(void);
void arpMapBackupManagerGone(void);
void arpMapCheckpointMsgWrite(L7_uchar8  *checkpointData,
                             L7_uint32 maxDataLen,
                             L7_uint32 *msgLen,
                             L7_BOOL  *moreData);
L7_RC_t arpMapCheckpointDataApply(void);
void arpMapCheckpointMsgProcess(L7_uchar8 *ckptData, L7_uint32 dataLen);
void arpMapCheckpointDataClear(void);
void arpMapCheckpointTablePopulate(void);

#endif   /* INCLUDE_IPMAP_ARP_CKPT_H */

