/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2007
*
**********************************************************************
* @filename  boxs_cfg.h
*
* @purpose   Box Services configuration header
*
* @component boxs
*
* @comments
*
* @create    01/15/2008
*
* @author    vkozlov
*
* @end
*
**********************************************************************/
#ifndef BOXS_CFG_H
#define BOXS_CFG_H


#define BOXS_CFG_FILENAME     "boxs.cfg"
#define BOXS_CFG_VER_1        0x1
#define BOXS_CFG_VER_CURRENT  BOXS_CFG_VER_1

#define BOXS_DEBUG_CFG_FILENAME "boxs_debug.cfg"

typedef struct boxsGlobalCfgData_s
{
  L7_int32        minTemp;
  L7_int32        maxTemp;
  L7_BOOL 	      trapTempStatusEnabled;
  L7_BOOL 	      trapFanStatusEnabled;
  L7_BOOL 	      trapPowSupplyStatusEnabled;
  L7_BOOL 	      trapSfpStatusEnabled;
  L7_BOOL 	      trapXfpStatusEnabled;
} boxsGlobalCfgData_t;


typedef struct boxsCfgData_s
{
  boxsGlobalCfgData_t  globalCfgData;
} boxsCfgData_t;

typedef struct boxsCfg_s
{
  L7_fileHdr_t hdr;
  boxsCfgData_t cfg;
  L7_uint32 checkSum;
} boxsCfg_t;

/***************************************************************************
 **************************FUNCTION PROTOTYPES******************************
 ***************************************************************************
 */
L7_RC_t boxsSave (void);
L7_RC_t boxsRstore (void);
L7_BOOL boxsHasDataChanged (void);
void boxsResetDataChanged(void);
void boxsMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer);
void boxsBuildDefaultConfigData(L7_uint32 ver);

#endif /* BOXS_CFG_H */
