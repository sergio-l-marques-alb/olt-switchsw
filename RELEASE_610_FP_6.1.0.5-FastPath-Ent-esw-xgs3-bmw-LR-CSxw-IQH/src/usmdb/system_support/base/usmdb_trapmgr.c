/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename src\usmdb\usmdb_trapmgr.c
*
* @purpose Provide interface to hardware API's for unitmgr components
*
* @component unitmgr
*
* @comments none
*
* @create 10/31/2000
*
* @author jjernigan
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/

#include "l7_common.h"
#include "usmdb_common.h"
#include "usmdb_registry_api.h"
#include "usmdb_trapmgr_api.h"
#include "usmdb_util_api.h"

#include "trapapi.h"
#include "snmp_api.h"
#include "snmp_trap_api.h"
#include "snmp_util_api.h"
#ifdef L7_CAPTIVE_PORTAL_PACKAGE
 #include "trap_captive_portal_api.h"
#endif

/*********************************************************************
*
* @purpose Get the Authentication Traps configuration mode.
*
* @param L7_uint32 UnitIndex  @b((input))  The unit for this operation
* @param L7_uint32 *val       @b((output)) Returned value (L7_ENABLE or L7_DISABLE)
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes Valid Authentication Trap Configurations returned:
*        @table{@row{@cell{L7_ENABLE}}
*               @row{@cell{L7_DISABLE}}}
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapAuthSwGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  *val = trapMgrGetTrapAuth();
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Get the Link State Traps configuration mode.
*
* @param L7_uint32 UnitIndex  @b((input))  The unit for this operation
* @param L7_uint32 *val       @b((output)) Returned value (L7_ENABLE or L7_DISABLE)
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes Valid Link Trap Configurations returned:
*        @table{@row{@cell{L7_ENABLE}}
*               @row{@cell{L7_DISABLE}}}
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapLinkStatusSwGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  *val = trapMgrGetTrapLink();
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Get the Multiusers Traps configuration mode.
*
* @param L7_uint32 UnitIndex  @b((input))  The unit for this operation
* @param L7_uint32 *val       @b((output)) Returned value (L7_ENABLE or L7_DISABLE)
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes Valid Mutliusers Trap Configurations returned:
*        @table{@row{@cell{L7_ENABLE}}
*               @row{@cell{L7_DISABLE}}}
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapMultiUsersSwGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  *val = trapMgrGetTrapMultiUsers();
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Get the Spanning Tree traps configuration mode.
*
* @param L7_uint32 UnitIndex  @b((input))  The unit for this operation
* @param L7_uint32 *val       @b((output)) Returned value (L7_ENABLE or L7_DISABLE)
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes Valid Spanning Tree Trap Configurations returned:
*        @table{@row{@cell{L7_ENABLE}}
*        @row{@cell{L7_DISABLE}}}
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapSpanningTreeSwGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  *val = trapMgrGetTrapSpanningTree();
  return L7_SUCCESS;
}

#ifdef L7_WIRELESS_PACKAGE
/*********************************************************************
*
* @purpose Get the Global Wireless traps configuration mode.
*
* @param L7_uint32 UnitIndex  @b((input))  The unit for this operation
* @param L7_uint32 *val       @b((output)) Returned value (L7_ENABLE or L7_DISABLE)
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes Valid Global Wireless Trap Configurations returned:
*        @table{@row{@cell{L7_ENABLE}}
*        @row{@cell{L7_DISABLE}}}
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapWirelessModeGet(L7_uint32 *val)
{
  return trapMgrWirelessTrapModeGet(val);
}
#endif

#ifdef L7_CAPTIVE_PORTAL_PACKAGE
/*********************************************************************
*
* @purpose Get the Captive Portal traps configuration mode.
*
* @param   L7_uint32  *val  @b((output)) Returned value (L7_ENABLE or L7_DISABLE)
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes   Valid Captive Portal Trap Configurations returned:
*          @table{@row{@cell{L7_ENABLE}}
*          @row{@cell{L7_DISABLE}}}
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapCaptivePortalGet(L7_uint32 *val)
{
  return trapMgrCaptivePortalTrapGet(val);
}
#endif

/*********************************************************************
*
* @purpose Set the Authentication related traps.
*
* @param L7_uint32 UnitIndex  @b((input)) The unit for this operation
* @param L7_uint32 val        @b((input)) Trap authentication state
*                             (L7_ENABLE or L7_DISABLE)
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes Valid Authentication Trap Configurations returned:
*        @table{@row{@cell{L7_ENABLE}}
*        @row{@cell{L7_DISABLE}}}
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapAuthSwSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  trapMgrSetTrapAuth(val);
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Set the Link State related traps.
*
* @param L7_uint32 UnitIndex  @b((input)) The unit for this operation
* @param L7_uint32 val        @b((input)) Link status (L7_ENABLE or L7_DISABLE)
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes Valid Link Trap Configurations returned:
*        @table{@row{@cell{L7_ENABLE}}
*        @row{@cell{L7_DISABLE}}}
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapLinkStatusSwSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  trapMgrSetTrapLink(val);
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Set the Multiusers related traps.
*
* @param L7_uint32 UnitIndex  @b((input)) The unit for this operation
* @param L7_uint32 val        @b((input)) Trap status (L7_ENABLE or L7_DISABLE)
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes Valid Multiusers Trap Configurations returned:
*        @table{@row{@cell{L7_ENABLE}}
*        @row{@cell{L7_DISABLE}}}
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapMultiUsersSwSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  trapMgrSetTrapMultiUsers(val);
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Set the Spanning Tree related traps.
*
* @param L7_uint32 UnitIndex  @b((input)) The unit for this operation
* @param L7_uint32 val        @b((input)) Trap status (L7_ENABLE or L7_DISABLE)
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes Valid Spanning Tree Trap Configurations returned:
*        @table{@row{@cell{L7_ENABLE}}
*        @row{@cell{L7_DISABLE}}}
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapSpanningTreeSwSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  trapMgrSetTrapMultiSpanningTree(val);
  return L7_SUCCESS;
}

/* END swDevTrapCtrl Family */

#ifdef L7_WIRELESS_PACKAGE
/*********************************************************************
*
* @purpose  Allows a user to configure whether the specified trap
*           is enabled or disabled
*
* @param    *mode      L7_ENABLE/ L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbTrapWirelessModeSet(L7_uint32 val)
{
  return trapMgrWirelessTrapModeSet(val);
}
#endif

#ifdef L7_CAPTIVE_PORTAL_PACKAGE
/*********************************************************************
*
* @purpose Set the Captive Portal traps configuration mode.
*
* @param   L7_uint32  val  @b((input)) L7_ENABLE or L7_DISABLE
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapCaptivePortalSet(L7_uint32 val)
{
  return trapMgrCaptivePortalTrapSet(val);
}
#endif

/*********************************************************************
*
* @purpose Get the POE traps configuration mode.
*
* @param L7_uint32 UnitIndex  @b((input))  The unit for this operation
* @param L7_uint32 *val       @b((output)) Returned value (L7_ENABLE or L7_DISABLE)
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes Valid Broadcast Trap Configurations returned:
*        @table{@row{@cell{L7_ENABLE}}
*        @row{@cell{L7_DISABLE}}}
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapPoeGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  *val = trapMgrGetTrapPoe();
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Set the POE related traps.
*
* @param L7_uint32 UnitIndex  @b((input)) The unit for this operation
* @param L7_uint32 val        @b((input)) Trap status (L7_ENABLE or L7_DISABLE)
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes Valid Broadcast Trap Configurations returned:
*        @table{@row{@cell{L7_ENABLE}}
*        @row{@cell{L7_DISABLE}}}
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapPoeSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  trapMgrSetTrapPoe(val);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the MAC Locking Violation traps configuration mode.
*
* @param L7_uint32 UnitIndex  @b((input))  The unit for this operation
* @param L7_uint32 *val       @b((output)) Returned value (L7_ENABLE or L7_DISABLE)
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapMacLockViolationGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  *val = trapMgrGetTrapMacLockViolation();
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Set the MAC Locking Violation related traps.
*
* @param L7_uint32 UnitIndex  @b((input)) The unit for this operation
* @param L7_uint32 val        @b((input)) Trap status (L7_ENABLE or L7_DISABLE)
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapMacLockViolationSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  trapMgrSetTrapMacLockViolation(val);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the total number of traps recorded upto the current
*          instant.
*
* @param L7_uint32 UnitIndex  @b((input))  The unit for this operation
* @param L7_uint32 *val       @b((output)) Returned value
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbInfoTrapsSinceLastResetSwGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  *val = trapMgrGetTotalAmountOfTraps();
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Get the traps logged since the last Display.
*
* @param L7_uint32 UnitIndex  @b((input))  The unit for this operation
* @param L7_uint32 *val       @b((output)) Returned value
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapNumUnDisplayedTrapEntriesGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  *val = trapMgrGetAmountOfUnDisplayedTraps();
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Displays the traplog.
*
* @param L7_uint32           UnitIndex  @b((input))  The unit for this operation
* @param L7_uint32           user       @b((input))  User
* @param usmDbTrapLogEntry_t trapLog[]  @b((input))  Trap entries' data
* @param L7_uint32           *val       @b((output)) Returned value

* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapLogSwGet(L7_uint32 UnitIndex, L7_uint32 user, usmDbTrapLogEntry_t trapLog[], L7_uint32 *val)
{
  *val = trapMgrGetTrapLog(user, (trapMgrTrapLogEntry_t *) trapLog);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Returns Trap Manager's last received trap log entry index
*
* @returns  index of last received trap log entry

* @notes    none
*
* @end
*********************************************************************/
L7_uint32 usmDbTrapLogEntryLastReceivedGet(void)
{
  return trapMgrGetTrapLogEntryLastReceived();
}

/*********************************************************************
*
* @purpose Displays one entry of the traplog.
*
* @param L7_uint32           UnitIndex      @b((input))  The unit for this operation
* @param L7_uint32           user           @b((input))  User
* @param L7_uint32           index          @b((output)) Returned value
* @param usmDbTrapLogEntry_t trapLogEntry   @b((input))  Trap entries' data

* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapLogEntryGet(L7_uint32 UnitIndex, L7_uint32 user, L7_uint32 index, usmDbTrapLogEntry_t *trapLogEntry)
{
  return trapMgrGetTrapLogEntry(user, index, (trapMgrTrapLogEntry_t *) trapLogEntry);
}

/*********************************************************************
*
* @purpose Clears the Trap log.
*
* @param L7_uint32 UnitIndex  @b((input)) The unit for this operation
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes Valid Trap log Configurations returned:
*        @table{@row{@cell{L7_ENABLE}}
*        @row{@cell{L7_DISABLE}}}
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCtrlClearTrapLogSwSet(L7_uint32 UnitIndex)
{
  trapMgrClearTrapLog();
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Check to see if the entry exists
*
* @param L7_uint32 UnitIndex  @b((input)) The unit for this operation
* @param L7_uint32 Index     @b((input)) The index
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapManagerEntryIpGet(L7_uint32 UnitIndex, L7_uint32 Index)
{
  L7_uint32 status;
  /* check to see if it's a valid access entry */
  if (Index >= 0 && Index < L7_MAX_SNMP_COMM &&
      SnmpTrapMgrStatusGet(Index, &status) == L7_SUCCESS && status != L7_SNMP_TRAP_MGR_STATUS_DELETE)
    return L7_SUCCESS;

  return L7_FAILURE;
}


/*********************************************************************
*
* @purpose Get the next entry's IP Address.
*
* @param L7_uint32 UnitIndex  @b((input))  The unit for this operation
* @param L7_uint32 *IpAddr    @b((output)) Pointer to the IP address
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapManagerEntryIpNext(L7_uint32 UnitIndex, L7_uint32 *Index)
{
  L7_uint32 status;

  for (*Index += 1; *Index < L7_MAX_SNMP_COMM; *Index += 1)
  {
    if (SnmpTrapMgrStatusGet(*Index, &status) == L7_SUCCESS && status != L7_SNMP_TRAP_MGR_STATUS_DELETE)
    return L7_SUCCESS;
  }
  return(L7_FAILURE);
}


/*********************************************************************
*
* @purpose Get the community name for the trap receiver.
*
* @param L7_uint32 UnitIndex  @b((input))  The unit for this operation
* @param L7_uint32 Index      @b((input))  Index
* @param L7_char8  *buf       @b((output)) Returned value
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapManagerCommIpGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_char8 *buf)
{
  return SnmpTrapMgrCommunityGet(Index, buf);
}


/*********************************************************************
*
* @purpose Get the SNMP mode to handle traps.
*
* @param L7_uint32 UnitIndex  @b((input))  The unit for this operation
* @param L7_uint32 Index      @b((input))  Index
* @param L7_uint32 *val       @b((output)) Returned value (L7_ENABLE or L7_DISABLE)
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes Valid SNMP TrapMgr status Configurations returned:
*        @table{@row{@cell{L7_ENABLE}}
*        @row{@cell{L7_DISABLE}}}
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapManagerStatusIpGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 *val)
{
  return SnmpTrapMgrStatusGet(Index, val);
}


/*********************************************************************
*
* @purpose Get the SNMP mode to handle traps.
*
* @param L7_uint32 UnitIndex  @b((input))  The unit for this operation
* @param L7_uint32 Index      @b((input))  Index
* @param L7_uint32 *val       @b((output)) Returned value (snmpTrapVer_t)
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes Valid SNMP TrapMgr versions returned:
*        @table{@row{@cell{L7_SNMP_TRAP_VER_SNMPV1}}
*        @row{@cell{L7_SNMP_TRAP_VER_SNMPV2}}}
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapManagerVersionGet(L7_uint32 UnitIndex, L7_uint32 Index, snmpTrapVer_t *val)
{
  return SnmpTrapMgrVersionGet(Index, val);
}


/*********************************************************************
*
* @purpose Set the SNMP Trap Manager Community mode.
*
* @param L7_uint32 UnitIndex  @b((input))  The unit for this operation
* @param L7_uint32 Index      @b((input))  Index
* @param L7_char8  *val       @b((output)) Pointer to community status
*                             (L7_ENABLE or L7_DISABLE)
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes Valid SNMP Trap Mgr Configurations returned:
*        @table{@row{@cell{L7_ENABLE}}
*        @row{@cell{L7_DISABLE}}}
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapManagerCommIpSet(L7_uint32 UnitIndex, L7_uint32 Index, L7_char8 *buf)
{
  return SnmpTrapMgrCommunitySet(Index, buf);
}


/*********************************************************************
*
* @purpose Set the SNMP trap handling mode.
*
* @param L7_uint32 UnitIndex  @b((input)) The unit for this operation
* @param L7_uint32 Index      @b((input)) The index
* @param L7_uint32 val        @b((input)) Snmptrap status (L7_ENABLE or L7_DISABLE)
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes Valid SNMP Trap Mgr status Configurations returned:
*        @table{@row{@cell{L7_ENABLE}}
*        @row{@cell{L7_DISABLE}}}
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapManagerStatusIpSet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 val)
{
  return SnmpTrapMgrStatusSet(Index, val);
}

/*********************************************************************
*
* @purpose Set the SNMP trap version.
*
* @param L7_uint32 UnitIndex  @b((input))  The unit for this operation
* @param L7_uint32 Index      @b((input))  Index
* @param L7_uint32 *val       @b((output)) SNMP Version (snmpTrapVer_t)
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes Valid SNMP TrapMgr versions returned:
*        @table{@row{@cell{L7_SNMP_TRAP_VER_SNMPV1}}
*        @row{@cell{L7_SNMP_TRAP_VER_SNMPV2}}}
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapManagerVersionSet(L7_uint32 UnitIndex, L7_uint32 Index, snmpTrapVer_t val)
{
  return SnmpTrapMgrVersionSet(Index, val);
}

/*********************************************************************
*
* @purpose Determines if trap log has been cleared.
*
* @param L7_uint32 UnitIndex  @b((input))  The unit for this operation
* @param L7_uint32 *val       @b((output)) Returned value
*
* @returns L7_SUCCESS
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCtrlClearTrapLogSwGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Set the SNMP IP Address per index.
*
* @param L7_uint32 UnitIndex           @b((input)) The unit for this operation
* @param L7_uint32 emptyTrapCommIndex  @b((input)) Index number
* @param L7_char8  *ip_address         @b((input)) The IP address
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapManagerIpAddrSet(L7_uint32 UnitIndex, L7_uint32 Index, L7_char8 *ip_address)
{
  L7_uint32 inetint;
  L7_RC_t rc;
  rc = usmDbInetAton(ip_address, &inetint);
  if ((rc == L7_FAILURE) || (inetint==0))
  {
    return L7_FAILURE;
  }
  else
    return SnmpTrapMgrIpAddrSet(Index, inetint);
}


/*********************************************************************
*
* @purpose Set the SNMP IPv6 Address per index.
*
* @param L7_uint32 UnitIndex           @b((input)) The unit for this operation
* @param L7_uint32 emptyTrapCommIndex  @b((input)) Index number
* @param L7_in6_addr_t  *ipv6_address  @b((input)) The IPv6 address
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapManagerIPv6AddrSet(L7_uint32 UnitIndex, L7_uint32 Index, L7_in6_addr_t *ipv6_address)
{
    return SnmpTrapMgrIPv6AddrSet(Index, ipv6_address);
}


/*********************************************************************
*
* @purpose Set the raw SNMP IP Address per index.
*
* @param L7_uint32 UnitIndex           @b((input)) The unit for this operation
* @param L7_uint32 emptyTrapCommIndex  @b((input)) Index number
* @param L7_uint32 ip_address          @b((input)) The IP address
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapManagerIpAddrRawSet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 ip_address)
{
  return SnmpTrapMgrIpAddrSet(Index, ip_address);
}


/*********************************************************************
*
* @purpose Get the SNMP IP Address per index.
*
* @param L7_uint32 UnitIndex    @b((input))  The unit for this operation
* @param L7_uint32 Index        @b((input))  Index
* @param L7_uint32 *ip_address  @b((output)) Returned IP address
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapManagerIpAddrGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 *ip_address)
{
  return SnmpTrapMgrIpAddrGet(Index, ip_address);
}

/*********************************************************************
*
* @purpose Get the SNMP IPv6 Address per index.
*
* @param L7_uint32 UnitIndex    @b((input))  The unit for this operation
* @param L7_uint32 Index        @b((input))  Index
* @param L7_in6_addr_t *ipv6_address  @b((output)) Returned IPv6 address
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapManagerIPv6AddrGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_in6_addr_t *ipv6_address)
{
  return SnmpTrapMgrIPv6AddrGet(Index, ipv6_address);
}


/* Trap Mgr cleanup */
/*********************************************************************
*
* @purpose Issue a link down trap.
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uint32 intIfNum  @b((input)) Internal interface number
*
* @returns L7_SUCCESS, if success
* @returns L7_NOT_EXISTS, if interface does not exist
* @returns L7_FAILURE, if other failure
*
* @notes Valid Link Trap Configurations returned:
*        @table{@row{@cell{L7_ENABLE}}
*        @row{@cell{L7_DISABLE}}}
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapMgrLinkDownTrapSend(L7_uint32 UnitIndex, L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_FAILURE;

  rc = trapMgrLinkDownLogTrap(intIfNum);

  return rc;
}


/*********************************************************************
*
* @purpose Issue a link up trap.
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uint32 intIfNum  @b((input)) Internal interface number
*
* @returns L7_SUCCESS, if success
* @returns L7_NOT_EXISTS, if interface does not exist
* @returns L7_FAILURE, if other failure
*
* @notes Valid Link Trap Configurations returned:
*        @table{@row{@cell{L7_ENABLE}}
*        @row{@cell{L7_DISABLE}}}
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapMgrLinkUpTrapSend(L7_uint32 UnitIndex, L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_FAILURE;

  rc = trapMgrLinkUpLogTrap(intIfNum);

  return rc;
}


/*********************************************************************
*
* @purpose Issue a coldstart trap.
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uint32 intIfNum  @b((input)) Internal interface number
*
* @returns L7_SUCCESS, if success
* @returns L7_NOT_EXISTS, if interface does not exist
* @returns L7_FAILURE, if other failure
*
* @notes Valid Cold Start Trap Configurations returned:
*        @table{@row{@cell{L7_ENABLE}}
*        @row{@cell{L7_DISABLE}}}
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapMgrColdStartTrapSend(L7_uint32 UnitIndex, L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_FAILURE;

  rc = trapMgrColdStartLogTrap(intIfNum);

  return rc;
}


/*********************************************************************
*
* @purpose Issue a warmstart trap.
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uint32 intIfNum  @b((input)) Internal interface number
*
* @returns L7_SUCCESS, if success
* @returns L7_NOT_EXISTS, if interface does not exist
* @returns L7_FAILURE, if other failure
*
* @notes Valid Warm Start Trap Configurations returned:
*        @table{@row{@cell{L7_ENABLE}}
*        @row{@cell{L7_DISABLE}}}
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapMgrWarmStartTrapSend(L7_uint32 UnitIndex, L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_FAILURE;

  rc = trapMgrWarmStartLogTrap(intIfNum);

  return rc;
}


/*********************************************************************
*
* @purpose Issue an Authentication Failure trap.
*
* @param L7_uint32 UnitIndex  @b((input))  The unit for this operation
* @param L7_uint32 ipAddress  @b((input)) The IP address
*
* @returns L7_SUCCESS, if success
* @returns L7_NOT_EXISTS, if interface does not exist
* @returns L7_FAILURE, if other failure
*
* @notes Valid Authentication Failure Trap Configurations returned:
*        @table{@row{@cell{L7_ENABLE}}
*        @row{@cell{L7_DISABLE}}}
*
* @end
*
*********************************************************************/
/* @p0457 start */
L7_RC_t usmDbTrapMgrAuthenFailureTrapSend(L7_uint32 UnitIndex, L7_uint32 ipAddress)
{
  L7_RC_t rc = L7_FAILURE;

  rc = trapMgrAuthenFailureLogTrap(ipAddress);
/* @p0457 end */

  return rc;
}


/*********************************************************************
*
* @purpose Get the link trap configuration of the specified interface.
*
* @param L7_uint32 UnitIndex  @b((input))  The unit for this operation
* @param L7_uint32 intIfNum   @b((input))  The internal interface number
* @param L7_uint32 *val       @b((output)) Pointer to link trap configuration
*
* @returns L7_SUCCESS, if success
* @returns L7_NOT_EXISTS, if interface does not exist
* @returns L7_FAILURE, if other failure
*
* @notes Valid Link Trap Configurations returned:
*        @table{@row{@cell{L7_ENABLE}}
*        @row{@cell{L7_DISABLE}}}
*
* @end
*
*********************************************************************/
L7_RC_t usmDbIfLinkUpDownTrapEnableGet(L7_uint32 UnitIndex,
                                       L7_uint32 intIfNum,
                                       L7_uint32 *val)
{
  return(nimGetIntfLinkTrap(intIfNum, val));
}


/*********************************************************************
*
* @purpose Set the link trap configuration of the specified interface.
*
* @param L7_uint32 UnitIndex  @b((input)) The unit for this operation
* @param L7_uint32 intIfNum   @b((input)) The internal interface number,
*                             L7_ALL_INTERFACES to apply to all
* @param L7_uint32 val        @b((input)) Link Trap Config
*
* @returns L7_SUCCESS, if success
* @returns L7_NOT_EXISTS, if interface does not exist
* @returns L7_FAILURE, if other failure
*
* @notes Valid Link Trap Configurations:
*        @table{@row{@cell{L7_ENABLE}}
*        @row{@cell{L7_DISABLE}}}
*
* @end
*
*********************************************************************/
L7_RC_t usmDbIfLinkUpDownTrapEnableSet(L7_uint32 UnitIndex,
                                       L7_uint32 intIfNum,
                                       L7_uint32 val)
{
  L7_uint32 current, next;
  L7_RC_t rc;

  if (intIfNum == L7_ALL_INTERFACES)
  {
    rc = nimFirstValidIntfNumber(&current);
    if (rc != L7_SUCCESS) return rc;
    rc = usmDbtrapMgrPortParmCanSet(UnitIndex, current);
    if (rc == L7_SUCCESS)
      rc = nimSetIntfLinkTrap(current, val);

    while (1)
    {
      if (nimNextValidIntfNumber(current, &next) != L7_SUCCESS)
        break;
      current = next;

      rc = usmDbtrapMgrPortParmCanSet(UnitIndex, current);
      if (rc== L7_SUCCESS)
      {
        rc = nimSetIntfLinkTrap(current, val);
        if (rc != L7_SUCCESS) break;
      }
    }
  }
  else
  {
    rc = usmDbtrapMgrPortParmCanSet(UnitIndex, intIfNum);
    if (rc == L7_SUCCESS)
    {
      rc = nimSetIntfLinkTrap(intIfNum, val);
    }
  }


  return rc;
}


/*********************************************************************
*
* @purpose Set the link trap configuration of all interfaces in the
*          specified slot.
*
* @param L7_uint32 UnitIndex  @b((input)) The unit for this operation
* @param L7_uint32 SlotNum    @b((input)) The slot number
* @param L7_uint32 val        @b((input)) Link Trap Config
*
* @returns L7_SUCCESS, if success
* @returns L7_NOT_EXISTS, if interface does not exist
* @returns L7_FAILURE, if other failure
*
* @notes Valid Link Trap Configurations:
*        @table{@row{@cell{L7_ENABLE}}
*        @row{@cell{L7_DISABLE}}}
*
* @end
*
*********************************************************************/
L7_RC_t usmDbIfLinkUpDownTrapEnableSlotSet(L7_uint32 UnitIndex, L7_uint32 SlotNum, L7_uint32 val)
{
  L7_uint32 intIfNum, numPorts, i;
  L7_RC_t rc;

  rc = usmDbPortsPerSlotGet(UnitIndex, SlotNum, &numPorts);
  if (rc != L7_SUCCESS)
  {
    return rc;
  }

  rc = usmDbIntIfNumFromUSPGet(UnitIndex, SlotNum, 1, &intIfNum);
  if (rc != L7_SUCCESS)
  {
    return rc;
  }

  for (i = intIfNum; i < (intIfNum+numPorts); i++)
  {
    rc = nimSetIntfLinkTrap(i, val);
    if (rc != L7_SUCCESS) break;
  }

  return rc;
}


/*********************************************************************
*
* @purpose  Issue Falling Alarm trap
*
* @param    L7_uint32 UnitIndex   @b((input))  The unit for this operation
* @param    L7_uint32 alarmIndex  @b((input)) Alarm index.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbtrapMgrFallingAlarmLogTrap(L7_uint32 UnitIndex, L7_uint32 alarmIndex)
{
  L7_RC_t rc = L7_FAILURE;

  rc = trapMgrFallingAlarmLogTrap(alarmIndex);

  return rc;
}


/*********************************************************************
*
* @purpose  Issue Rising Alarm trap
*
* @param    L7_uint32 UnitIndex   @b((input))  The unit for this operation
* @param    L7_uint32 alarmIndex  @b((input)) Alarm index.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbtrapMgrRisingAlarmLogTrap(L7_uint32 UnitIndex, L7_uint32 alarmIndex)
{
  L7_RC_t rc = L7_FAILURE;

  rc = trapMgrRisingAlarmLogTrap(alarmIndex);

  return rc;
}

/*********************************************************************
* @purpose  Check to see if the port is the appropriate type on which
*           the parm can be set
*
* @param    UnitIndex           The unit for this operation
* @param    intIfNum            Internal interface number
*
* @returns  L7_SUCCESS          The parm can be set on this port type
* @returns  L7_NOT_SUPPORTED    The port does not support this parm to be set
*
* @notes    presently this function tests only for logical vlan intf.
*
* @end
*********************************************************************/
L7_RC_t usmDbtrapMgrPortParmCanSet(L7_uint32 UnitIndex, L7_uint32 intIfNum)
{
  L7_INTF_TYPES_t ifType;

  if (nimGetIntfType(intIfNum,&ifType) == L7_SUCCESS)
  {
    if (ifType == L7_LOGICAL_VLAN_INTF)
    {
      return L7_NOT_SUPPORTED;
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Issue a Config Chaged trap.
*
* @param L7_char8*  trapSource @b((input)) source of the change such as the
*                   unit, slot, port, VLAN, LAG, etc
* @param L7_char8*  trapInfo @b((input)) additional information regarding the
*                   change
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @notes None
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapMgrConfigChangedTrapSend(L7_char8 *trapSource,
                                          L7_char8 *trapInfo)
{
  return trapMgrConfigChangedLogTrap(trapSource, trapInfo);
}

/*********************************************************************
*
* @purpose Issue a TFTP End trap.
*
* @param L7_int32   exitCode @b((input)) exitcode of the TFTP
* @param L7_char8*  fName @b((input)) name of the file
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @notes None
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapMgrTFTPEndTrapSend(L7_int32 exitCode, L7_char8 *fName)
{
  return trapMgrTFTPEndLogTrap(exitCode, fName);
}

/*********************************************************************
*
* @purpose Issue a TFTP Abort trap.
*
* @param L7_int32   exitCode @b((input)) exitcode of the TFTP
* @param L7_char8*  fName @b((input)) name of the file
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @notes None
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapMgrTFTPAbortTrapSend(L7_int32 exitCode, L7_char8 *fName)
{
  return trapMgrTFTPAbortLogTrap(exitCode, fName);
}

/*********************************************************************
*
* @purpose Issue a TFTP Start trap.
*
* @param L7_char8*  fName @b((input)) name of the file
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @notes None
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapMgrTFTPStartTrapSend(L7_char8 *fName)
{
  return trapMgrTFTPStartLogTrap(fName);
}

/*********************************************************************
*
* @purpose Issue a Link Failure trap.
*
* @param L7_int32 ifIndex @b((input)) The if index of the failed link
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @notes None
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapMgrLinkFailureTrapSend(L7_int32 ifIndex)
{
  return trapMgrLinkFailureLogTrap(ifIndex);
}

/*********************************************************************
*
* @purpose Issue a VlanDynPortAdded trap.
*
* @param L7_int32 dot1qVlanIndex @b((input)) VLan index
* @param L7_int32 port @b((input)) Port that was added dynamically to the VLan
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @notes None
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapMgrVlanDynPortAddedTrapSend(L7_int32 dot1qVlanIndex,
                                             L7_int32 port)
{
  return trapMgrVlanDynPortAddedLogTrap(dot1qVlanIndex, port);
}

/*********************************************************************
*
* @purpose Issue a VlanDynPortRemoved trap.
*
* @param L7_int32 dot1qVlanIndex @b((input)) VLan index
* @param L7_int32 port @b((input)) Port that was removed dynamically from the
*                                  VLan
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @notes None
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapMgrVlanDynPortRemovedTrapSend(L7_int32 dot1qVlanIndex,
                                               L7_int32 port)
{
  return trapMgrVlanDynPortRemovedLogTrap(dot1qVlanIndex, port);
}

/*********************************************************************
*
* @purpose Issue a StackMasterFailed trap.
*
* @param L7_int32 oldUnitNumber @b((input)) Old Stack master unit number
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @notes None
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapMgrStackMasterFailedTrapSend(L7_int32 oldUnitNumber)
{
  return trapMgrStackMasterFailedLogTrap(oldUnitNumber);
}

/*********************************************************************
*
* @purpose Issue a StackNewMasterElected trap.
*
* @param L7_int32 oldUnitNumber @b((input)) Old Stack master unit number
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @notes None
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapMgrStackNewMasterElectedTrapSend(L7_int32 oldUnitNumber)
{
  return trapMgrStackNewMasterElectedLogTrap(oldUnitNumber);
}

/*********************************************************************
*
* @purpose Issue a StackMemberUnitFailed trap.
*
* @param L7_int32 failedUnitNumber @b((input)) Failed member unit number
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @notes None
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapMgrStackMemberUnitFailedTrapSend(L7_int32 failedUnitNumber)
{
  return trapMgrStackMemberUnitFailedLogTrap(failedUnitNumber);
}

/*********************************************************************
*
* @purpose Issue a StackNewMemberUnitAdded trap.
*
* @param L7_int32 newUnitNumber @b((input)) New member unit number
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @notes None
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapMgrStackNewMemberUnitAddedTrapSend(L7_int32 newUnitNumber)
{
  return trapMgrStackNewMemberUnitAddedLogTrap(newUnitNumber);
}

/*********************************************************************
*
* @purpose Issue a StackMemberUnitRemoved trap.
*
* @param L7_int32 removedUnitNumber @b((input)) Removed member unit number
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @notes None
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapMgrStackMemberUnitRemovedTrapSend(L7_int32 removedUnitNumber)
{
  return trapMgrStackMemberUnitRemovedLogTrap(removedUnitNumber);
}

/*********************************************************************
*
* @purpose Issue a StackSplitMasterReport trap.
*
* @param L7_uchar8* trapInfo @b((input)) Comma seperated list of unit numbers
*                   of the units that are split from the main stack
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @notes None
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapMgrStackSplitMasterReportTrapSend(L7_uchar8 *trapInfo)
{
  return trapMgrStackSplitMasterReportLogTrap(trapInfo);
}

/*********************************************************************
*
* @purpose Issue a StackSplitNewMasterReport trap.
*
* @param L7_int32 oldMasterUnitNumber @b((input)) Old Stack master unit number
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @notes None
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapMgrStackSplitNewMasterReportTrapSend(L7_int32 oldMasterUnitNumber)
{
  return trapMgrStackSplitNewMasterReportLogTrap(oldMasterUnitNumber);
}

/*********************************************************************
*
* @purpose Issue a StackRejoined trap.
*
* @param None
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @notes None
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapMgrStackRejoinedTrapSend()
{
  return trapMgrStackRejoinedLogTrap();
}

/*********************************************************************
*
* @purpose Issue a StackLinkFailed trap.
*
* @param L7_int32   identifiedUnit @b((input)) The unit that identified the
*                   link failure
* @param L7_uchar8* info @b((input)) additional information about the stack
*                   state after the failure
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @notes None
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapMgrStackLinkFailedTrapSend(L7_int32 identifiedUnit,
                                            L7_uchar8 *info)
{
  return trapMgrStackLinkFailedLogTrap(identifiedUnit, info);
}

/*********************************************************************
*
* @purpose Issue a Dot1dStpPortStateForwarding trap.
*
* @param L7_int32 port @b((input)) Port number
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @notes None
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapMgrDot1dStpPortStateForwardingTrapSend(L7_int32 port, L7_uint32 instanceIdx)
{
  return trapMgrDot1dStpPortStateForwardingLogTrap(port, instanceIdx);
}

/*********************************************************************
*
* @purpose Issue a Dot1dStpPortStateNotForwarding trap.
*
* @param L7_int32 port @b((input)) Port number
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @notes None
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapMgrDot1dStpPortStateNotForwardingTrapSend(L7_int32 port, L7_uint32 instanceIdx)
{
  return trapMgrDot1dStpPortStateNotForwardingLogTrap(port, instanceIdx);
}

/*********************************************************************
*
* @purpose Issue a TrunkPortAdded trap.
*
* @param L7_int32 trunkIfIndex @b((input)) If Index of the trunk
* @param L7_int32 port @b((input)) Port number
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @notes None
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapMgrTrunkPortAddedTrapSend(L7_int32 trunkIfIndex, L7_int32 port)
{
  return trapMgrTrunkPortAddedLogTrap(trunkIfIndex, port);
}

/*********************************************************************
*
* @purpose Issue a TrunkPortRemoved trap.
*
* @param L7_int32 trunkIfIndex @b((input)) If Index of the trunk
* @param L7_int32 port @b((input)) Port number
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @notes None
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapMgrTrunkPortRemovedTrapSend(L7_int32 trunkIfIndex, L7_int32 port)
{
  return trapMgrTrunkPortRemovedLogTrap(trunkIfIndex, port);
}

/*********************************************************************
*
* @purpose Issue a LockPort trap.
*
* @param L7_int32 port @b((input)) Port number
* @param L7_enetMacAddr_t macAddr @b((input)) New MAC address received
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @notes None
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapMgrLockPortTrapSend(L7_int32 port, L7_enetMacAddr_t macAddr)
{
  return trapMgrLockPortLogTrap(port, macAddr);
}

/*********************************************************************
*
* @purpose Issue a VlanDynVlanAdded trap.
*
* @param L7_int32 dot1qVlanIndex @b((input)) Index of the VLan being added
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @notes None
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapMgrVlanDynVlanAddedTrapSend(L7_int32 dot1qVlanIndex)
{
  return trapMgrVlanDynVlanAddedLogTrap(dot1qVlanIndex);
}

/*********************************************************************
*
* @purpose Issue a VlanDynVlanRemoved trap.
*
* @param L7_int32 dot1qVlanIndex @b((input)) Index of the VLan being removed
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @notes None
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapMgrVlanDynVlanRemovedTrapSend(L7_int32 dot1qVlanIndex)
{
  return trapMgrVlanDynVlanRemovedLogTrap(dot1qVlanIndex);
}

/*********************************************************************
*
* @purpose Issue a EnvMonFanStateChange trap.
*
* @param L7_int32 fanIndex @b((input)) Fan Index
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @notes None
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapMgrEnvMonFanStateChangeTrapSend(L7_uint32 unit, L7_int32 fanIndex, L7_BOOL status)
{
  return trapMgrEnvMonFanStateChangeLogTrap(unit, fanIndex, status);
}

/*********************************************************************
*
* @purpose Issue a EnvMonPowerSupplyStateChange trap.
*
* @param L7_int32 envMonSupplyIndex @b((input)) Power supply Index
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @notes None
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapMgrEnvMonPowerSupplyStateChangeTrapSend(L7_uint32 unit, L7_int32 envMonSupplyIndex, L7_BOOL status)
{
  return trapMgrEnvMonPowerSupplyStateChangeLogTrap(unit, envMonSupplyIndex, status);
}

/*********************************************************************
*
* @purpose Issue a EnvMonTemperatureRisingAlarm trap.
*
* @param None
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @notes None
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapMgrEnvMonTemperatureRisingAlarmTrapSend(L7_uint32 unit)
{
  return trapMgrEnvMonTemperatureRisingAlarmLogTrap(unit);
}

/*********************************************************************
*
* @purpose Issue a CopyFinished trap.
*
* @param None
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @notes None
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapMgrCopyFinishedTrapSend()
{
  return trapMgrCopyFinishedLogTrap();
}

/*********************************************************************
*
* @purpose Issue a CopyFailed trap.
*
* @param None
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @notes None
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapMgrCopyFailedTrapSend()
{
  return trapMgrCopyFailedLogTrap();
}

/*********************************************************************
*
* @purpose Issue a Dot1xPortStatusAuthorized trap.
*
* @param L7_int32 port @b((input)) Port ifIndex
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @notes None
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapMgrDot1xPortStatusAuthorizedTrapSend(L7_int32 port)
{
  return trapMgrDot1xPortStatusAuthorizedLogTrap(port);
}

/*********************************************************************
*
* @purpose Issue a Dot1xPortStatusUnauthorized trap.
*
* @param L7_int32 port @b((input)) Port ifIndex
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @notes None
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapMgrDot1xPortStatusUnauthorizedTrapSend(L7_int32 port)
{
  return trapMgrDot1xPortStatusUnauthorizedLogTrap(port);
}

/*********************************************************************
*
* @purpose Issue a StpElectedAsRoot trap.
*
* @param None
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @notes None
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapMgrStpElectedAsRootTrapSend()
{
  return trapMgrStpElectedAsRootLogTrap();
}

/*********************************************************************
*
* @purpose Issue a StpNewRootElected trap.
*
* @param L7_int32 instanceID @b((input)) Instance ID
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @notes None
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapMgrStpNewRootElectedTrapSend(L7_int32 instanceID, L7_uchar8 *rootId)
{
  return trapMgrStpNewRootElectedLogTrap(instanceID, rootId);
}

/*********************************************************************
*
* @purpose Issue a InvalidUserLoginAttempted trap.
*
* @param L7_uchar8* uiMode @b((input)) The mode used to logged in, string with
*                   value of either "Web" or "CLI"
* @param L7_uchar8* fromIpAddress @b((input)) IP address from where the
*                   attempt was made
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @notes None
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapMgrInvalidUserLoginAttemptedTrapSend(L7_uchar8 *uiMode,
                                                      L7_uchar8 *fromIpAddress)
{
  return trapMgrInvalidUserLoginAttemptedLogTrap(uiMode, fromIpAddress);
}

/*********************************************************************
*
* @purpose Issue a ManagementACLViolation trap.
*
* @param L7_uchar8* uiMode @b((input)) The mode used to logged in, string with
*                   value of "SNMP" or "Web" or "CLI"
* @param L7_uchar8* fromIpAddress @b((input)) IP address from where the
*                   attempt was made
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @notes None
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapMgrManagementACLViolationTrapSend(L7_uchar8 *uiMode,
                                                   L7_uchar8 *fromIpAddress)
{
  return trapMgrManagementACLViolationLogTrap(uiMode, fromIpAddress);
}

/*********************************************************************
*
* @purpose Issue a SFP insertion/removal trap.
*
* @param L7_int32 unitNumber @b((input)) Unit where the SFP was inserted/removed
*
* @param L7_uint32 intIfNum   @b((input))  The internal interface number
*
* @param L7_BOOL status   @b((input))  Indicates whether the SFP was inserted
*                                      or removed
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @notes None
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapMgrSFPInsertionRemovalTrapSend(L7_int32 unitNumber,
                                                L7_int32 intIfNum,
                                                L7_BOOL status)
{
  return trapMgrSFPInsertionRemovalTrap(unitNumber, intIfNum, status);
}

/*********************************************************************
*
* @purpose Issue a XFP insertion/removal trap.
*
* @param L7_int32 unitNumber @b((input)) Unit where the XFP was inserted/removed
*
* @param L7_uint32 intIfNum   @b((input))  The internal interface number
*
* @param L7_BOOL status   @b((input))  Indicates whether the XFP was inserted
*                                      or removed
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @notes None
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTrapMgrXFPInsertionRemovalTrapSend(L7_int32 unitNumber,
                                                L7_int32 intIfNum,
                                                L7_BOOL status)
{
  return trapMgrXFPInsertionRemovalTrap(unitNumber, intIfNum, status);
}
