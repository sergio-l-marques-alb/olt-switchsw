
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  mfc.h
*
* @purpose   The purpose of this file is to export the functionalities
*            implemented by the corresponding C file (mfc.c).
*
* @component Multicast Forwarding Cache (MFC)
*
* @comments  This file should not be included by any header or C files
*            other than those that belong to the MFC component itself.
*            All other components MUST use the functionalities exported
*            by the mfc_api.h file only.
*
* @create    January 18, 2006
*
* @author    ddevi
* @end
*
**********************************************************************/
#ifndef _MFC_MAP_H
#define _MFC_MAP_H

/**********************************************************************
       Include Files (only those required to compile this file)
**********************************************************************/
#include "l7_common.h"
#include "l3_mcast_commdefs.h"
#include "mfc_api.h"
#include "l3_addrdefs.h"

/*********************************************************************
         Typedefs
***********************************************************************/

typedef enum 
{
  MFC_EVENT_MIN = 0,
  MFC_ENTRY_EXPIRY_TIMER_EVENT,
  MFC_UPCALL_EXPIRY_TIMER_EVENT,
  MFC_WARM_RESTART_TIMER_EVENT,
  MFC_IPV6_DATA_PKT_RECV_EVENT,
  MFC_EVENT_MAX,
}mfcEventType_t;


/**********************************************************************
                  Function Prototypes
**********************************************************************/

extern L7_RC_t mfcEnable(L7_uchar8 family);
extern L7_RC_t mfcDisable(L7_uchar8 family);
extern L7_RC_t mfcCnfgrPhase1Init(void);
extern L7_RC_t mfcCnfgrPhase2Init(void);
extern L7_RC_t mfcCnfgrPhase3Init(void);
extern L7_RC_t mfcCnfgrPhase1InitUndo(void);
extern L7_RC_t mfcCnfgrPhase2InitUndo(void);
extern L7_RC_t mfcCnfgrPhase3InitUndo(void);
extern L7_RC_t mfcEventProcess(mfcEventType_t event, void *pMsg);
extern L7_RC_t mfcEntryCountGet(L7_uint32 *pCount);
extern L7_RC_t mfcWarmRestartTimerStart(void);
#endif /* _MFC_H */


