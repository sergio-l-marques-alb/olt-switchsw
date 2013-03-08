/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   cos_config.h
*
* @purpose    COS component structures and config data
*
* @component  cos
*
* @comments   none
*
* @create     03/12/2004
*
* @author     gpaussa
* @end
*
**********************************************************************/
#ifndef INCLUDE_COS_CONFIG_H
#define INCLUDE_COS_CONFIG_H

#include "l7_common.h"
#include "defaultconfig.h"
#include "default_cnfgr.h"
#include "l7_cos_api.h"

#define L7_COS_CFG_FILENAME             "qos_cos.cfg"
#define L7_COS_CFG_VER_1                0x1
#define L7_COS_CFG_VER_2                0x2
#define L7_COS_CFG_VER_CURRENT          L7_COS_CFG_VER_2

/* global COS config parameters */
typedef struct
{
  L7_cosCfgParms_t        cfg;          /* all globally configurable parms */

} L7_cosCfgGlobalParms_t;

/* struct defining all COS parms on an interface */
typedef struct 
{
  nimConfigID_t           configId;     /* must be first in intf cfg struct */
  L7_cosCfgParms_t        cfg;          /* all intf configurable parms */

} L7_cosCfgIntfParms_t;

typedef struct
{
  L7_fileHdr_t            cfgHdr;

  L7_uint32               msgLvl;       /* debug message level (saved config) */
  L7_cosCfgGlobalParms_t  cosGlobal;
  L7_cosCfgIntfParms_t    cosIntf[L7_COS_INTF_MAX_COUNT];

  L7_uint32               checkSum;
  
} L7_cosCfgData_t;


extern L7_cosCfgData_t    *pCosCfgData_g;

#define L7_COS_CFG_DATA_SIZE  sizeof(L7_cosCfgData_t)


/****************************************
*
*  COS deregistered functions
*
*  This struct will be deprecated when
*  component deregistration is supported
*
*****************************************/

typedef struct 
{
  L7_BOOL cosSave;
  L7_BOOL cosRestore;
  L7_BOOL cosHasDataChanged;
  L7_BOOL cosIntfChange;
} cosDeregister_t;

extern cosDeregister_t    cosDeregister_g;


#endif /* INCLUDE_COS_CONFIG_H */
