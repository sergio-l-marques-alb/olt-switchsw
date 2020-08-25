
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename trap_migrate.h
*
* @purpose trap Configuration Migration
*
* @component trap
*
* @comments none
*
* @create  08/24/2004
*
* @author  Rama Sasthri, Kristipati 
* @end
*
**********************************************************************/

#ifndef TRAP_MIGRATE_H
#define TRAP_MIGRATE_H

#include "platform_config_migrate.h"
#include "sysapi_cfg_migrate.h"
#include <trapapi.h>
#include <trap.h>

#define FD_TRAP_BCAST_STORM   L7_ENABLE

typedef struct
{
  L7_fileHdr_t cfgHdr;

  L7_uint32 trapAuth;
  L7_uint32 trapLink;
  L7_uint32 trapMultiUsers;
  L7_uint32 trapSpanningTree;
  L7_uint32 trapBcastStorm;
  L7_uint32 trapGeneralSystem;
  L7_uint32 trapDot1q;
  L7_uint32 trapOspf;
  L7_uint32 trapVrrp;
  L7_uint32 trapBgp;
  L7_uint32 trapPim;
  L7_uint32 trapDvmrp;
  /* L7_uint32 trapPoe; This is present in base REL_G but not in REL_G_4_0_0_2 */
  L7_uint32 checkSum;           /* check sum of config file Note: Must be last entry */
}
trapMgrCfgDataV1_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;

  L7_uint32 trapAuth;
  L7_uint32 trapLink;
  L7_uint32 trapMultiUsers;
  L7_uint32 trapSpanningTree;
  L7_uint32 trapBcastStorm;
  L7_uint32 trapGeneralSystem;
  L7_uint32 trapDot1q;
  L7_uint32 trapOspf;
  L7_uint32 trapVrrp;
  L7_uint32 trapBgp;
  L7_uint32 trapPim;
  L7_uint32 trapDvmrp;
  L7_uint32 trapInventory;
  L7_uint32 trapPoe;
  L7_uint32 checkSum;           /* check sum of config file Note: Must be last entry */

} trapMgrCfgDataV2_t;

typedef struct
{
  L7_fileHdr_t   cfgHdr;

  L7_uint32      trapAuth;
  L7_uint32      trapLink;
  L7_uint32      trapMultiUsers;
  L7_uint32      trapSpanningTree;
  L7_uint32      trapBcastStorm;
  L7_uint32      trapGeneralSystem;
  L7_uint32      trapDot1q;
  L7_uint32      trapOspf;
  L7_uint32      trapVrrp;
  L7_uint32      trapBgp;
  L7_uint32      trapPim;
  L7_uint32      trapDvmrp;
  L7_uint32      trapInventory;
  L7_uint32      trapPoe;
  L7_uint32      trapMacLockViolation;
  L7_uint32      checkSum;            /* check sum of config file Note: Must be last entry */

} trapMgrCfgDataV3_t;

typedef struct
{
  L7_fileHdr_t   cfgHdr;

  L7_uint32      trapAuth;
  L7_uint32      trapLink;
  L7_uint32      trapMultiUsers;
  L7_uint32      trapSpanningTree;
  L7_uint32      trapGeneralSystem;
  L7_uint32      trapDot1q;
  L7_uint32      trapOspf;
  L7_uint32      trapVrrp;
  L7_uint32      trapBgp;
  L7_uint32      trapPim;
  L7_uint32      trapDvmrp;
  L7_uint32      trapInventory;
  L7_uint32      trapPoe;
  L7_uint32      trapMacLockViolation;
  L7_uint32      trapAcl;
  L7_uint32      checkSum;            /* check sum of config file Note: Must be last entry */

} trapMgrCfgDataV4_t;

typedef struct
{
  L7_fileHdr_t   cfgHdr;

  L7_uint32      trapAuth;
  L7_uint32      trapLink;
  L7_uint32      trapMultiUsers;
  L7_uint32      trapSpanningTree;
  L7_uint32      trapGeneralSystem;
  L7_uint32      trapDot1q;
  L7_uint32      trapOspf;
  L7_uint32      trapVrrp;
  L7_uint32      trapBgp;
  L7_uint32      trapPim;
  L7_uint32      trapDvmrp;
  L7_uint32      trapInventory;
  L7_uint32      trapPoe;
  L7_uint32      trapMacLockViolation;
  L7_uint32      trapAcl;
  L7_uint32      trapWireless;
  L7_uint32      checkSum;            /* check sum of config file Note: Must be last entry */

} trapMgrCfgDataV5_t;

typedef struct
{
  L7_fileHdr_t   cfgHdr;

  L7_uint32      trapAuth;
  L7_uint32      trapLink;
  L7_uint32      trapMultiUsers;
  L7_uint32      trapSpanningTree;
  L7_uint32      trapGeneralSystem;
  L7_uint32      trapDot1q;
  L7_uint32      trapOspf;
  L7_uint32      trapOspfv3;
  L7_uint32      trapVrrp;
  L7_uint32      trapBgp;
  L7_uint32      trapPim;
  L7_uint32      trapDvmrp;
  L7_uint32      trapInventory;
  L7_uint32      trapPoe;
  L7_uint32      trapMacLockViolation;
  L7_uint32      trapAcl;
  L7_uint32      trapWireless;
  L7_uint32      checkSum;            /* check sum of config file Note: Must be last entry */

} trapMgrCfgDataV6_t;

typedef trapMgrCfgData_t trapMgrCfgDataV7_t;


#endif /* TRAP_MIGRATE_H */
