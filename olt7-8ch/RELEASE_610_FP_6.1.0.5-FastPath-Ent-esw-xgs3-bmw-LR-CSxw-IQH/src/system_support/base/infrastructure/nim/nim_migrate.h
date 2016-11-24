
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename nim_migrate.h
*
* @purpose NIM Configuration Migration
*
* @component NIM
*
* @comments none
*
* @create  08/24/2004
*
* @author  Rama Sasthri, Kristipati 
* @end
*
**********************************************************************/

#ifndef NIM_MIGRATE_H
#define NIM_MIGRATE_H

#include "platform_config_migrate.h"
#include "sysapi_cfg_migrate.h"
#include "nim_data.h"
#include "nim_config.h"

#define MAX_IF_ALIAS_REL_4_0 64

/****************************************
*
*  NIM Configuration Data                    
*
*****************************************/

typedef struct
{
  L7_uchar8 LAAMacAddr[L7_MAC_ADDR_LEN];        /* Locally Administered MAC Address  */
  L7_uint32 addrType;           /* Using L7_BIA or L7_LAA */
  char ifAlias[MAX_IF_ALIAS_REL_4_0 + 1];       /* User-Defined Name */
  L7_uint32 nameType;           /* Using L7_SYSNAME or L7_ALIASNAME */
  L7_uint32 ifSpeed;            /* ifType (MIB II) duplex is included */
  L7_uint32 autoNegAdminStatus; /* agentPortAutoNegAdminStatus (private MIB): L7_ENABLE or L7_DISABLE */
  L7_uint32 adminState;         /* ifAdminStatus: L7_ENABLE or L7_DISABLE */
  L7_uint32 trapState;          /* link Trap status: L7_ENABLE or L7_DISABLE */
  L7_uint32 ipMtu;              /* Maximum size of data portion of a frame on the port */
  L7_uint32 encapsType;         /* Encapsulation Type of frame: L7_ENCAPSULATION_t */
  L7_uint32 cfgMaxFrameSize;    /* Maximum configurable frame size on the port */
} nimCfgPortV1_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  L7_uint32 cardIdRev;          /* Hardward id and version of config file */
  nimCfgPortV1_t *cfgPort;
  L7_uint32 checkSum;           /* check sum of config file NOTE: must be last entry */
} nimConfigDataV1_t;

/* type for the format of the configuration file */
typedef struct
{
  L7_fileHdr_t cfgHdr;
  L7_uint32 numOfInterfaces;    /* Number of created interfaces */
  NIM_INTF_MASK_REL_4_1_t configMaskBitmap;     /* a bitmap to determine the used maskoffsets */
  nimCfgPort_t *cfgPortStart;
  L7_uint32 checkSum;
  nimCfgPort_t cfgPort[L7_MAX_INTERFACE_COUNT_REL_4_1 + 1];     /* an array of the ports configuration */
  L7_uint32 checkSum2;           /* check sum of config file NOTE: must be last entry */
} nimConfigDataV3_t;

/* type for the format of the configuration file */
typedef struct
{
  L7_fileHdr_t cfgHdr;
  L7_uint32 numOfInterfaces;    /* Number of created interfaces */
  NIM_INTF_MASK_REL_4_3_t configMaskBitmap;     /* a bitmap to determine the used maskoffsets */
  nimCfgPort_t *cfgPortStart;
  L7_uint32 checkSum;
  nimCfgPort_t cfgPort[L7_MAX_INTERFACE_COUNT_REL_4_3 + 1];     /* an array of the ports configuration */
  L7_uint32 checkSum2;           /* check sum of config file NOTE: must be last entry */
} nimConfigDataV4_t;

/* type for the format of the configuration file */
typedef struct
{
  L7_fileHdr_t cfgHdr;
  L7_uint32 numOfInterfaces;    /* Number of created interfaces */
  NIM_INTF_MASK_REL_4_4_t configMaskBitmap;     /* a bitmap to determine the used maskoffsets */
  nimCfgPort_t *cfgPortStart;
  L7_uint32 checkSum;
  nimCfgPort_t cfgPort[L7_MAX_INTERFACE_COUNT_REL_4_4 + 1];     /* an array of the ports configuration */
  L7_uint32 checkSum2;           /* check sum of config file NOTE: must be last entry */
} nimConfigDataV5_t;

typedef nimConfigDataV5_t nimConfigDataCurrent_t;
#endif /* NIM_MIGRATE_H */
