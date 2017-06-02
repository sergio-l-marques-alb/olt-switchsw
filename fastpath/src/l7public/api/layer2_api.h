/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename layer2.h
 *
 * @purpose layer2 Defines, Data Structures, and Prototypes
 *
 * @component layer2
 *
 * @comments none
 *
 * @create 10/10/2000
 *
 * @author wjacobs
 * @end
 *
 **********************************************************************/

#ifndef _LAYER2_H_
#define _LAYER2_H_


/*--------------------------------------*/
/*  PLATFORM CONFIGURATION              */
/*--------------------------------------*/


#define LAYER2_PLATFORM_FILENAME  "layer2.plt"
#define LAYER2_PLATFORM_VER_1      0x1

typedef struct {
  L7_uint32   functionMap;            
#define LAYER2_CAPABLE          0x0001      /* Supports DOT1Q Bridging */
#define LAYER2_GVRP_CAPABLE     0x0002      /* Supports GVRP */
#define LAYER2_GMRP_CAPABLE     0x0004      /* Supports GMRP */
#define LAYER2_IVL_ONLY         0x0008      /* Supports IVL Only */
#define LAYER2_IVL_HYBRID       0x0010      /* Supports IVL/SVL Hybrid */
#define LAYER2_8021S_CAPABLE    0x0020      /* Supports 802.1S Multiple Spanning Tree */ 
#define LAYER2_8021V_CAPABLE    0x0040      /* Supports 802.1V Port and Protocol VLANs*/ 
#define LAYER2_8021W_CAPABLE    0x0080      /* Supports 802.1W Rapid Reconfiguration  */ 
#include "transfer_exports.h"
  L7_uchar8   defaultCfgFile[L7_MAX_FILENAME];    /* Platform-specific default cfg  */
  L7_uint32   priorityType;               
} layer2Platform_t;

typedef struct {
  L7_fileHdr_t        hdr;
  layer2Platform_t    platform;
  L7_uint32           checkSum;
}layer2PlatformFile_t;

#endif 

