
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename     captive_portal_cnfgr.h
*
* @purpose      Captive portal configuration header
*
* @component    captive portal
*
* @comments     none
*
* @create       06/25/2003
*
* @author       dcaugherty
*
* @end
*
**********************************************************************/
#ifndef INCLUDE_CAPTIVE_PORTAL_COMMON_H
#define INCLUDE_CAPTIVE_PORTAL_COMMON_H

#include "clustering_commdefs.h"


/* message ID for osapi message posting */
#define CP_CLUSTER_MSG_ID_STATS_UPDATE    1

/* CP Statistics Update message */
typedef struct cpClusterStatsUpdateMsg_s
{
  L7_uchar8            buf[CLUSTER_MAX_DATAGRAM_MSG_SIZE];
  L7_uint32            msgLen;
  L7_enetMacAddr_t     swMacAddr;
  L7_enetMacAddr_t     macAddr;
  L7_uint64            bytesTx;
  L7_uint64            bytesRx;
  L7_uint64            pktsTx;
  L7_uint64            pktsRx;
} cpClusterStatsUpdateMsg_t;

typedef struct captivePortalTaskMsg_ 
{
  L7_uint32 data;
  L7_uint32 msgID;
  union
  {
    cpClusterStatsUpdateMsg_t statsMsg;
  } type;
} captivePortalTaskMsg;  

/*********************************************************************
* @purpose  This function returns the task ID of the Captive 
*           Portal task.
*
* @param    void
*
* @returns  L7_ERROR   - If task does not exist, could 
*                        not be started, or initialization failed.
* @returns  L7_int32 representing task ID (duh) otherwise.
*
* @end
*********************************************************************/
L7_int32    captivePortalTaskId(void);


/*********************************************************************
* @purpose  To force the Captive Portal task to quit.
*
* @param    int - a reason code (not yet used)
*
* @returns  void
*           
* @end
*********************************************************************/
void        captivePortalQuit(int ignored);


/*********************************************************************
* @purpose  To wake up the Captive Portal task to do immediate work.
*
* @param    void
*
* @returns  L7_RC_t - L7_ERROR or L7_SUCCESS
*           
* @end
*********************************************************************/
L7_RC_t     captivePortalTaskWake(void);

/*********************************************************************
* @purpose  To enable Captive Portal
*
* @param    void
*
* @returns  L7_RC_t - L7_ERROR or L7_SUCCESS
*           
* @end
*********************************************************************/
L7_RC_t     captivePortalEnable(void);

/*********************************************************************
* @purpose  To disable Captive Portal
*
* @param    void
*
* @returns  L7_RC_t - L7_ERROR or L7_SUCCESS
*           
* @end
*********************************************************************/
L7_RC_t     captivePortalDisable(void);

/*********************************************************************
*
* @purpose  To parse the configurator commands send to the Captive
*           Portal component
*
* @param    none
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
void captivePortalCnfgrParse(void);

/*********************************************************************
* @purpose  Handle CP client authentications
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void captivePortalClientAuthHandle(void);

#endif  /* INCLUDE_CAPTIVE_PORTAL_COMMON_H */
