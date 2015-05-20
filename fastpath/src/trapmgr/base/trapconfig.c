/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename trapconfig.c
*
* @purpose Trap Manager Factory Defaults File
*
* @component trapmgr
*
* @comments none
*
* @create 08/30/2000
*
* @author bmutz
*
* @end
*             
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#include <l7_common.h>
#include <nvstoreapi.h>
#include <osapi.h>
#include <defaultconfig.h> 
#include <trapapi.h>
#include <trap.h>

extern trapMgrCfgData_t trapMgrCfgData;

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
void trapBuildDefaultConfigData(L7_uint32 ver)
{
  memset(( void * )&trapMgrCfgData, 0, sizeof( trapMgrCfgData_t)); 

  strcpy(trapMgrCfgData.cfgHdr.filename, TRAP_CFG_FILENAME);
  trapMgrCfgData.cfgHdr.version = ver;
  trapMgrCfgData.cfgHdr.componentID = L7_TRAPMGR_COMPONENT_ID;
  trapMgrCfgData.cfgHdr.type = L7_CFG_DATA;
  trapMgrCfgData.cfgHdr.length = sizeof(trapMgrCfgData);
  trapMgrCfgData.cfgHdr.dataChanged = L7_FALSE;

  trapMgrCfgData.trapAuth = FD_TRAP_AUTH;
  trapMgrCfgData.trapLink = FD_TRAP_LINK_STATUS;  
  trapMgrCfgData.trapMultiUsers = FD_TRAP_MULTI_USERS;
  trapMgrCfgData.trapSpanningTree = FD_TRAP_SPAN_TREE;
  trapMgrCfgData.trapDot1q = FD_TRAP_DOT1Q;
  trapMgrCfgData.trapOspf = FD_TRAP_OSPF_TRAP;
  trapMgrCfgData.trapOspfv3 = FD_TRAP_OSPFV3_TRAP;
  trapMgrCfgData.trapVrrp = FD_TRAP_VRRP;
  trapMgrCfgData.trapPim = FD_TRAP_PIM;
  trapMgrCfgData.trapDvmrp = FD_TRAP_DVMRP;
  trapMgrCfgData.trapPoe = FD_TRAP_POE;
  trapMgrCfgData.trapMacLockViolation = FD_TRAP_MACLOCK_VIOLATION;
  trapMgrCfgData.trapAcl = FD_TRAP_ACL;
  trapMgrCfgData.trapWireless = FD_TRAP_WIRELESS;
  trapMgrCfgData.trapCaptivePortal = FD_TRAP_CAPTIVE_PORTAL;
}

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
L7_RC_t trapMgrApplyConfigData(void)
{
    L7_RC_t rc;
    rc = trapMgrOspfTrapFlagsApply(trapMgrCfgData.trapOspf);
    if (rc == L7_NOT_EXIST)
    {
        /* not a routing build. Everything ok */
        return L7_SUCCESS;
    }
    else
    {
        return rc;
    }
}

