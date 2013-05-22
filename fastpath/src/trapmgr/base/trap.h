/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename trap.h
*
* @purpose Trap Manager Constants and Data Structures
*
* @component trapmgr
*
* @comments none
*
* @create 08/30/2000
*
* @author bmutz
* @end
*
**********************************************************************/


#ifndef INCLUDE_TRAP_H
#define INCLUDE_TRAP_H

#include "dtlapi.h"
#include "l7_resources.h"
#include "trapmgr_exports.h"

#define TRAPMGR_DROP_TRAP  0
#define TRAPMGR_LOG_TRAP   1

#define TRAP_CFG_FILENAME  "trapCfgData.cfg"
#define TRAP_CFG_VER_1      0x1
#define TRAP_CFG_VER_2      0x2
#define TRAP_CFG_VER_3      0x3
#define TRAP_CFG_VER_4      0x4
#define TRAP_CFG_VER_5      0x5
#define TRAP_CFG_VER_6      0x6
#define TRAP_CFG_VER_7      0x7
#define TRAP_CFG_VER_8      0x8
#define TRAP_CFG_VER_CURRENT TRAP_CFG_VER_8

#define TRAPMGR_UNIT_INDEX  1

/****************************************
*
*  Trap Manager Configuration Data
*
*****************************************/

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
  L7_uint32      trapCaptivePortal;
  L7_uint32      checkSum;            /* check sum of config file Note: Must be last entry */

} trapMgrCfgData_t;


/****************************************
*
*  Trap Manager Data Structures
*
*****************************************/

typedef struct
{
  trapMgrTrapLogEntry_t   trapLog[TRAPMGR_TABLE_SIZE];
  L7_uint32               lastReceivedTrap;
  L7_uint32               currentAmountOfTraps;
  L7_uint32               amountOfUnDisplayedTraps;
  L7_uint32               totalAmountOfTraps;
} trapMgrTrapData_t;


/****************************************
*
*  TRAP Internal function defines
*
*****************************************/

/*********************************************************************
* @purpose  Build default trap config data
*
* @param    ver   Software version of Config Data
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void trapBuildDefaultConfigData(L7_uint32 ver);

/*********************************************************************
* @purpose  Apply trap manager's configuration data.
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t trapMgrApplyConfigData(void);

/*********************************************************************
* @purpose  Add a trap to the trap log
*
* @param    trapStringBuf    Message to log
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void trapMgrLogTrapToLocalLog(L7_char8 *trapStringBuf);

/*********************************************************************
* @purpose  If it exists, tell the OSPF component which of the 
*           OSPF trap flags are set.
*
* @param    ospfTrapFlags {(input)} bitmap indicating the OSPF trap flags
*           that are set.
*
* @returns  L7_SUCCESS if trap flags are successfully communicated to OSPF
* @returns  L7_NOT_EXIST if the routing package is not present 
*
* @comments None 
*       
* @end
*********************************************************************/
L7_RC_t trapMgrOspfTrapFlagsApply(L7_uint32 ospfTrapFlags);


/*********************************************************************
* @purpose  Migrate old config to new
*
* @param    oldVer      @b{(input)} version of old config definition
* @param    ver         @b{(input)} version of current config definition
* @param    pCfgBuffer  @b{(input)} ptr to location of read configuration
*
* @returns  void
*
* @notes    This is the callback function provided to the sysapiCfgFileGet
*           routine to handle cases where the config file is of an older
*           version.
*
* @notes
*
* @end
*********************************************************************/
void trapMigrateConfigData(L7_uint32 oldVer, L7_uint32 ver, L7_char8 *pCfgBuffer);

#endif
