/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   pimdm_main.h
*
* @purpose    
*
* @component  PIM-DM
*
* @comments   none
*
* @create     
*
* @author     Krishna Mohan CS
* @end
*
**********************************************************************/

#ifndef _PIMDM_MAIN_H
#define _PIMDM_MAIN_H

/*******************************************************************************
**                             Includes                                       **
*******************************************************************************/
#include "l7_common.h"
#include "l3_addrdefs.h"
#include "mcast_inet.h"
#include "avl_api.h"
#include "comm_mask.h"
#include "pimdm_api.h"
#include "l7apptimer_api.h"
#include "l7sll_api.h"
#include "l7handle_api.h"
#include "pimdm_defs.h"
#include "pimdm_map_vend_ctrl.h"

/*******************************************************************************
**                             General Definitions                            **
*******************************************************************************/

/* Miscellaneous Definitions */
#define PIMDM_UNUSED_PARAM(x)   ((void) x)

/*******************************************************************************
**                        Data Structure Definitions                          **
*******************************************************************************/

/*********************************************************************
*    PIM-DM Protocol Specific Control Message Type enumeration
*********************************************************************/
typedef enum
{
  PIMDM_CTRL_PKT_HELLO = 0,
  PIMDM_CTRL_PKT_JOIN,
  PIMDM_CTRL_PKT_PRUNE,
  PIMDM_CTRL_PKT_ASSERT,
  PIMDM_CTRL_PKT_GRAFT,
  PIMDM_CTRL_PKT_GRAFT_ACK,
  PIMDM_CTRL_PKT_STATE_REFRESH,
  PIMDM_CTRL_PKT_DATA_NO_CACHE,
  PIMDM_CTRL_PKT_DATA_WRONG_IF,
  PIMDM_CTRL_PKT_MAX
}PIMDM_CTRL_PKT_TYPE_t;

/*********************************************************************
*            PIM-DM Interface Statistics Type enumeration
*********************************************************************/
typedef enum
{
  PIMDM_STATS_RECEIVED = 0,
  PIMDM_STATS_SENT,
  PIMDM_STATS_DROPPED,
  PIMDM_STATS_TYPE_MAX
}PIMDM_STATS_TYPE_t;

/*********************************************************************
*              PIM-DM Control Block Parameters
*********************************************************************/
typedef struct pimdmCB_s
{
  L7_int32       addrFamily;     /* L7_AF_INET (or) L7_AF_INET6 */
  L7_BOOL        operMode;       /* PIM-DM Operational Mode */

  /************ Interface Info related Parameters ***************/
  struct pimdmIntfInfo_s *intfInfo; /* Space for Interface & Neighbors */
  interface_bitset_t     nbrBitmap; /* Bitmap of Interfaces with PIM-DM neighbors.
                                 * Represents pim_nbrs macro in sec.4.1.3 of RFC 3973 */

  /************ MRT (S,G) Database related parameters ***************/
  avlTree_t       mrtSGTree;    /* PIMDM MRT Tree - node - pimdmMrtEntry_t */
  avlTreeTables_t *mrtTreeHeap; /* Space for MRT Tree nodes */
  void            *mrtDataHeap; /* Space for MRT data */

  /************ Local Membership Database related parameters ***************/
  avlTree_t       localMembTree;       /* MGMD Membership info */
  avlTreeTables_t *pLocalMembTreeHeap; /* Space for Local Memb'ship Tree nodes */
  void            *pLocalMembDataHeap; /* Space for Local Memb'ship data */

  /************ Timers related parameters ***************/
  handle_list_t         *handleList; /* Handle list to store the  timer args */
  L7_APP_TMR_CTRL_BLK_t appTimer;    /* PIM-DM Application Timer Handler */
  L7_uint32             maxPimdmTimers; /* Maximum number of Timers needed */

  /************* RTO Specific Parameters **********************/
  L7_VOIDFUNCPTR_t   rtoCallback;  /* callback stored for reference purpose */
  L7_APP_TMR_HNDL_t  rtoTimer;  /* Timer to get the data in chunks in intervals */
  void               *rtoTimerSemId; /* Lock for RTO Timer as it will
                                      * be accessed both in RTO and
                                      * PIMDM-MAP Task contexts.*/

  /******** AdminScope Boundary Database Specific parameters ***************/
  L7_sll_t           asbList;  /* AdminScope Boundary List of type pimdmASBNode_t */

  /************ Buffer Pools related Parameters ***************/
  L7_uchar8       *routeChangeBuffer; /*space allocated for the route change buffer */

  /************ Control Packet Rx/Tx related parameters ***************/
  L7_int32       sockFD;         /* Used for Packet reception and transmisson */
  L7_inet_addr_t allRoutersAddr; /* PIM All Routers Address. */
  L7_uchar8      pktTxBuf[PIMDM_MAX_PDU]; /* Buffer used to Transmit PIM-DM Packets */
  L7_uchar8      pktRxBuf[PIMDM_MAX_PDU]; /* Buffer used to Receive PIM-DM Packets */

  /************ Join/Prune Bundle Timer ***************/
  L7_APP_TMR_HNDL_t pimdmJPBundleTimer;

}pimdmCB_t;


/*******************************************************************************
**                  Function Prototypes Declarations                          **
*******************************************************************************/

extern L7_RC_t
pimdmEventProcess (L7_uchar8 familyType,
                   L7_uint32 event,
                   void *msg);

extern L7_RC_t
pimdmGlobalAdminModeSet (MCAST_CB_HNDL_t pimdmCbHandle,
                         pimdmMapEvent_t *mapLayerInfo);

extern void
pimdmRTOTimerStart (MCAST_CB_HNDL_t cbHandle);

#endif  /* _PIMDM_MAIN_H */
