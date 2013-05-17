/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    ospf_vend_ctrl.h
* @purpose     OSPF vendor-specific internal definitions
* @component   OSPF Mapping Layer
* @comments    none
* @create      04/12/2001
* @author      gpaussa
* @end
*             
**********************************************************************/

/*************************************************************
                    
*************************************************************/


#ifndef _OSPF_VEND_CTRL_H_
#define _OSPF_VEND_CTRL_H_

/* headers */
#include "std.h"
#include "routing_init.ext"
#include "xx.ext"
#include "frame.ext"
#include "buffer.ext"
#include "spapi.ext"
#include "tcpbind.ext"
#include "iptypes.h"
#include "spobj.h"
#include "ospfmib.ext"

/* general IP Mapping headers */
#include "l7_ospfinclude.h"
#include "ospf_vend_exten.h"

/* Common global externs */
extern struct ospfMapCtrl_s  ospfMapCtrl_g;
extern struct ospfMapMibCtrl_s  ospfMapMibCtrl_g;

/* literal definitions */
#define OSPF_MAP_RTR_INTF_MAX   (L7_RTR_MAX_RTR_INTERFACES +1)

#define OSPF_MAP_MCAST_ALL_RTRS   0xe0000005  /* 224.0.0.5 */
#define OSPF_MAP_MCAST_DES_RTRS   0xe0000006  /* 224.0.0.6 */

/* OSPF control context */
typedef struct ospfMapCtrl_s
{
  t_Handle          RTO_Id;
  t_Handle          ospfThread;

  /* OSPF socket and bind structure */
  t_Handle ospfTcpUdpObject;
  t_Handle ospfConn;

  /* semaphore for message queue between mapping layer and engine */
  void *ospfSyncSema;

  /* semaphore to synchronize access to configuration data */
  void *cfgSema;

} ospfMapCtrl_t;

/* OSPF mib control context */
typedef struct ospfMapMibCtrl_s
{
  t_Handle          p_ospfMibObj;
} ospfMapMibCtrl_t;

/* local manager definition. Used to associate an internal interface 
 * number with a protocol interface object. */
typedef struct ospfMapLocalMgr_s
{
  L7_uint32     index;                  /* local mgr array index */
  L7_uint32     intIfNum;               /* router internal intf number */

  /* On numbered interfaces, the primary IP address and mask. On 
   * unnumbered interfaces, 0/0. */
  L7_uint32     ipAddr;                 
  L7_uint32     ipMask;                 

  t_Handle      IFO_Id;                 /* intf object identifier */
} ospfMapLocalMgr_t, t_lm;

typedef struct ospfVlinkInfo_s     
{
    t_Handle        IFO_Id;
    L7_uint32       areaId;
    L7_uint32       neighbor;
    L7_BOOL         vlinkCreated;
} ospfVlinkInfo_t;


/*--------------------------------------------------------*/
/* Internal OSPF MAP vendor component function prototypes */
/*--------------------------------------------------------*/

/*------------------*/
/* ospf_vend_ctrl.c */
/*------------------*/

/*********************************************************************
* @purpose  Determines the local manager element from an internal
*           interface number.
*
* @param    intIfNum    router internal interface number
* @param    ppLm        output area to store Local Mgr element pointer
*
* @returns  L7_SUCCESS  Local Mgr element found
* @returns  L7_FAILURE  could not find Local Mgr element for interface
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ospfMapLmLookup(L7_uint32 intIfNum, ospfMapLocalMgr_t **ppLm);

/*********************************************************************
* @purpose  Determines the local manager index value from an internal 
*           interface number.
*
* @param    intIfNum    router internal interface number
*
* @returns  L7_uint32   Local Mgr index, or 0 if match not found.
*
* @notes    none
*       
* @end
*********************************************************************/
L7_uint32 ospfMapLmIndexGet(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Determines the internal interface number from an local 
*           index value
*
* @param    ifIndex     Local manager index value
*
* @returns  L7_uint32   Internal interface value, or 0 if match not found.
*
* @notes    none
*       
* @end
*********************************************************************/
L7_uint32 ospfMapLmIntIfNumGet(L7_uint32 ifIndex);

/*********************************************************************
* @purpose  Get the internal interface number for a IFO handle.      
*
* @param    IFO_Id      IFO object Id
*
* @returns  L7_uint32   Internal interface value, or 0 if match not found.
*
* @notes    none
*       
* @end
*********************************************************************/
L7_uint32 ospfMapLmIfoIntIfNumGet(t_Handle IFO_Id);

#endif /* _OSPF_VEND_CTRL_H_ */
