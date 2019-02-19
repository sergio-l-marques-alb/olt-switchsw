/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename vrrp_config.h
*
* @purpose VRRP header file
*
* @component VRRP
*
* @comments 
*
* @create  08/11/2001
*
* @author  TCS 
* @end
*
**********************************************************************/
/*********************************************************************
 *                   
 ********************************************************************/
#ifndef _VRRP_CONFIG_H
#define _VRRP_CONFIG_H

#include "l7_common.h"
#include "nimapi.h"
#include "l3_commdefs.h"
#include "l7_vrrp_api.h"
#include "default_cnfgr.h"

/* Since the max VRs supported in the system is L7_VRRP_MAX_VIRT_ROUTERS,
 * we also set the maximum number of interfaces that can be enabled for
 * atleast 1 VR to L7_VRRP_MAX_VIRT_ROUTERS.
 *
 * NOTE: Set the L7_VRRP_INTF_MAX_COUNT value to the minimum value of
 *       L7_VRRP_MAX_VIRT_ROUTERS and L7_RTR_MAX_RTR_INTERFACES */

#define L7_VRRP_INTF_MAX_COUNT        L7_VRRP_MAX_VIRT_ROUTERS

#define VRRP_MIN_PACKET_LENGTH  ( sizeof(vrrpPkt_t) - sizeof(L7_IP_ADDR_t) * ( L7_L3_NUM_IP_ADDRS-1) )

#define L7_VRRP_CFG_FILENAME   "vrrpMap.cfg"
#define L7_VRRP_CFG_VER_1       0x1
#define L7_VRRP_CFG_VER_2       0x2
#define L7_VRRP_CFG_VER_3       0x3
#define L7_VRRP_CFG_VER_CURRENT     L7_VRRP_CFG_VER_3

#define VRRP_DEBUG_CFG_FILENAME "vrrp_debug.cfg"
#define VRRP_DEBUG_CFG_VER_1     0x1
#define VRRP_DEBUG_CFG_VER_CURRENT    VRRP_DEBUG_CFG_VER_1


/****************************************
*
*  VRRP Global Configuration Data                
*
*****************************************/

typedef struct vrrpCfgData_s
{
  L7_BOOL          vrrpAdminMode;             /* Enable VRRP                 */

} vrrpCfgData_t;     

/****************************************
*
*  VRRP Interface Tracking Configuration Data
*
*****************************************/

typedef struct vrrpTrackIntf_s
{
  L7_uint32 intIfNum;
  L7_uchar8 prio_dec;
  L7_BOOL stateUp;
  
} vrrpTrackIntf_t;

/****************************************
*
*  VRRP Route Tracking Configuration Data
*
*****************************************/
typedef struct vrrpTrackRoute_s
{
  L7_uint32 ipAddr;
  L7_uint32 subnetMask;
  L7_uchar8 prio_dec;
  L7_BOOL reachable;
  
} vrrpTrackRoute_t;

/****************************************
*
*  VRRP Virtual Router Configuration Data         
*
*****************************************/

typedef struct vrrpVirtRouterCfg_s
{
  nimConfigID_t       configId;
  L7_uchar8           vrid;                  /* Virtual Router ID           */
  L7_uchar8           priority;              /* Priority                    */
  L7_BOOL             preempt_mode;          /* Pre-Empt mode               */
  L7_uchar8           adver_int;             /* Advertisement Interval      */
  L7_AUTH_TYPES_t     auth_types;
  L7_uchar8           auth_data[L7_VRRP_MAX_AUTH_DATA];
  L7_IP_ADDR_t        ipaddress[L7_L3_NUM_IP_ADDRS];	         /* Virtual router's IP address */
  L7_vrrpStartState_t vr_start_state; 
  vrrpTrackIntf_t  vr_track_intf[L7_VRRP_MAX_TRACK_INTF_PER_VR];
  vrrpTrackRoute_t vr_track_route[L7_VRRP_MAX_TRACK_RT_PER_VR];
} vrrpVirtRouterCfg_t;


/****************************************
*
*  VRRP Configuration File                
*
*****************************************/

typedef struct L7_vrrpMapCfg_s 
{
  L7_fileHdr_t        cfgHdr;
  vrrpCfgData_t       rtr;
  vrrpVirtRouterCfg_t virtRouter[L7_VRRP_MAX_VIRT_ROUTERS];
  L7_uint32           checkSum;

} L7_vrrpMapCfg_t;
/****************************************
*
*  VRRP Debug Configuration File
*
*****************************************/
typedef struct vrrpDebugCfgData_s
{
  L7_BOOL vrrpDebugPacketTraceFlag;
} vrrpDebugCfgData_t;

typedef struct vrrpDebugCfg_s
{
  L7_fileHdr_t          hdr;
  vrrpDebugCfgData_t  cfg;
  L7_uint32             checkSum;
} vrrpDebugCfg_t;


#endif /* _VRRP_CONFIG_H */
