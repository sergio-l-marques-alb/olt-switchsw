/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    mirror_cfg.h
* @purpose     Port mirroring config data 
* @component   mirroring
* @comments    none
* @create      11/21/2001
* @author      skalyanam
* @end
*             
**********************************************************************/

#ifndef MIRROR_CFG_H
#define MIRROR_CFG_H

#include <nimapi.h>
#include "l7_common.h"
#include "comm_mask.h"
#include "mirror_exports.h"

/*********************************************************************
* @purpose  Migrate config date
*
* @param    oldVer older version number
* @param    ver current version number
* @param    pCfgBuffer older config data
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void mirrorMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver,
                                     L7_char8 * pCfgBuffer);

#define MIRROR_CFG_FILENAME  "mirrorCfgData.cfg"
#define MIRROR_CFG_VER_1      0x1
#define MIRROR_CFG_VER_2      0x2
#define MIRROR_CFG_VER_3      0x3
#define MIRROR_CFG_VER_4      0x4
#define MIRROR_CFG_VER_CURRENT MIRROR_CFG_VER_4

typedef struct
{
  L7_INTF_MASK_t        srcIngressMirrorType;
  L7_INTF_MASK_t        srcEgressMirrorType;
  L7_INTF_MASK_t        srcPorts; 
  nimConfigID_t         destID;   
  L7_uint32             mode;    
  L7_MIRROR_DIRECTION_t probeType;
} mirrorSession_t;

typedef struct
{
  L7_fileHdr_t   cfgHdr;
  mirrorSession_t sessionData[L7_MIRRORING_MAX_SESSIONS];
  L7_uint32      tracemode;     /*  Enable tracing of events */
  L7_uint32      checkSum;     /* check sum of config file NOTE: needs to be last entry */
} mirrorCfgData_t;

/* operational data */
typedef struct mirrorSessionInfo_s {
    L7_uint32      mode;
} mirrorSessionInfo_t;


#endif /*MIRROR_CFG_H*/
