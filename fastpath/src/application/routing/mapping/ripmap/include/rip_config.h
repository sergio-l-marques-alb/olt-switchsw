
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   rip_config.h
*
* @purpose    RIP Mapping layer structures and config data
*
* @component  RIP Mapping Layer
*
* @comments   none
*
* @create     05/21/2001
*
* @author     gpaussa
* @end
*
**********************************************************************/

#ifndef _RIP_CONFIG_H
#define _RIP_CONFIG_H

#include "l7_common_l3.h"
#include "l3_default_cnfgr.h"
#include "nimapi.h"

#define L7_RIP_MAP_INTF_MAX_COUNT        L7_IPMAP_INTF_MAX_COUNT

#define L7_RIP_MAP_CFG_FILENAME       "ripMap.cfg"
#define L7_RIP_MAP_CFG_VER_1          0x1
#define L7_RIP_MAP_CFG_VER_2          0x2
#define L7_RIP_MAP_CFG_VER_3          0x3
#define L7_RIP_MAP_CFG_VER_4          0x4
#define L7_RIP_MAP_CFG_VER_5          0x5
#define L7_RIP_MAP_CFG_VER_6          0x6
#define L7_RIP_MAP_CFG_VER_7          0x7
#define L7_RIP_MAP_CFG_VER_CURRENT    L7_RIP_MAP_CFG_VER_7


/****************************************
*
*  RIP Interface Configuration Data         
*
*****************************************/

typedef struct ripMapCfgIntf_s
{
  nimConfigID_t configId;
  L7_uint32     adminMode;                /* Enable RIP for interface   */
  L7_uint32     authType;                 /* L7_AUTH_TYPES_t            */
  L7_char8      authKey[L7_AUTH_MAX_KEY_RIP]; /* authentication key     */
  L7_uint32     authKeyLen;               /* original length of auth key*/
  L7_uint32     authKeyId;                /* auth key identifier (MD5)  */
  L7_uint32     verSend;                  /* RIP version to send        */
  L7_uint32     verRecv;                  /* RIP version to receive     */
  L7_uint32     defaultMetric;            /* default route metric       */
  L7_uint32     intfCost;                 /* interface cost (FUTURE USE)*/

} ripMapCfgIntf_t;
typedef struct ripRtRedistFilterCfg_s
{
  L7_uint32                  mode;               /* Filter mode               */
  L7_uint32                  filter;             /* ACL to be used            */
} ripRtRedistFilterCfg_t;

typedef struct ripRtRedistCfg_s
{
  L7_uint32                  redistribute;      /* redistribute flag         */
  L7_uint32                  metric;            /* Metric for redistribution */ 
  L7_OSPF_METRIC_TYPES_t     matchType;         /* Type of OSPF route        */
  void                       *routeMap;          /* FUTURE use                */
  ripRtRedistFilterCfg_t     ripRtRedistFilter;
} ripRtRedistCfg_t;

typedef struct ripDefRtAdvCfg_s
{
  L7_uint32             defRtAdv;                 /* Default route adv flag     */
  void                  *routeMap;                 /* Future use                 */
} ripDefRtAdvCfg_t;

/****************************************
*
*  RIP Configuration File                
*
*****************************************/

typedef struct ripMapCfg_s 
{
  L7_fileHdr_t     cfgHdr;
  L7_uint32        ripAdminMode;             /* Enable RIP                 */
  L7_uint32        ripSplitHorizon;          /* Split Horizon mode         */
  L7_uint32        ripAutoSummarization;     /* Auto Summarization mode    */
  L7_uint32        ripHostRoutesAccept;      /* Host routes accept mode    */ 
  L7_uint32           ripDefaultMetric;             /* Default metric             */
  ripRtRedistCfg_t    ripRtRedist[REDIST_RT_LAST];  /* Route Redistribution Config*/
  ripDefRtAdvCfg_t    ripDefRtAdv;              /* Default Route Adv Config   */

  ripMapCfgIntf_t  intf[L7_RIP_MAP_INTF_MAX_COUNT];
  L7_uint32        checkSum;

} ripMapCfg_t;


/****************************************
*
*  Function Prototypes
*
*****************************************/
extern void ripMapMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer);


#endif /* _RIP_CONFIG_H */
