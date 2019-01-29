/**********************************************************************
* @filename lldp_migrate.h
*
* @purpose LLDP MED configuration file migrate header
*
* @component 802.1AB
*
* @comments none
*
* @create 12/19/2006
*
* @author skalyanam
*
* @end
*
**********************************************************************/
#ifndef LLDP_MIGRATE_H
#define LLDP_MIGRATE_H

#include "platform_config_migrate.h"
#include "sysapi_cfg_migrate.h"
#include "lldp.h"


#define L7_LLDP_INTF_MAX_COUNT_REL_4_4 (L7_MAX_PORT_COUNT_REL_4_4 + 1)

typedef struct
{
  nimConfigID_t        cfgId;
  L7_uchar8            txEnabled;
  L7_uchar8            rxEnabled;
  L7_uchar8            notificationEnabled;
  L7_uchar8            optionalTLVsEnabled;
  L7_uchar8            mgmtAddrTxEnabled;
} lldpIntfCfgDataV1_t;
typedef struct
{
  L7_fileHdr_t         cfgHdr;
  L7_uint32            txInterval;
  L7_uint32            txHoldMultiplier;
  L7_uint32            reinitDelay;
  L7_uint32            notifyInterval;
  lldpIntfCfgDataV1_t  intfCfgData[L7_LLDP_INTF_MAX_COUNT_REL_4_4];
  L7_uint32            checkSum;
} lldpCfgDataV1_t;

typedef lldpCfgData_t lldpCfgDataV2_t;

void lldpMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer);

#endif /* LLDP_MIGRATE_H */
