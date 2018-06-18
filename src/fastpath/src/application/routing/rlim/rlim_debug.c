/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename rlim_debug.c
*
* @purpose RLIM debugging functions
*
* @component Routing Logical Interface Manager
*
* @comments
*
* @create 02/21/2005
*
* @author eberge
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "osapi_support.h"
#include "osapi_sockdefs.h"
#include "defaultconfig.h"
#include "rlim.h"
#include "simapi.h"

#define TEST_ASSERT(exp, valgot, valexpected, passcnt, failcnt) {       \
  if (exp) {                                                            \
    (passcnt)++;                                                        \
  } else {                                                              \
    (failcnt)++;                                                        \
    sysapiPrintf("ASSERT FAILED: " #exp ", got %u, expected %u "        \
                 "(line %u)\n", (valgot), (valexpected), __LINE__);     \
  }                                                                     \
}

#define PRINT_FLAG(a,b,c) {         \
  if (((a) & (b)) != 0) {           \
    sysapiPrintf(c);                \
    (a) &= ~(b);                    \
    if ((a) != 0) {                 \
      sysapiPrintf(",");            \
    }                               \
  }                                 \
}

/*********************************************************************
* @purpose  Initialize debug trace flags 
*
* @end
*********************************************************************/
extern L7_uint32 rlimDebugTraceFlags;

/*********************************************************************
* @purpose  Initialize debug module
*
* @param    void
*
* @returns  void
*
* @notes    This is the hook to ensure that this object gets loaded
*
* @end
*********************************************************************/
void rlimDebugInit()
{
  return;
}

/*********************************************************************
* @purpose  Mode printing helper function
*
* @param    tunnelMode
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
L7_uchar8 *rlimTunnelModeString(L7_uint32 tunnelMode)
{
  switch (tunnelMode)
  {
    case L7_TUNNEL_MODE_UNDEFINED:
      return "undefined";
    case L7_TUNNEL_MODE_6OVER4:
      return "6over4";
    case L7_TUNNEL_MODE_6TO4:
      return "6to4";
    case L7_TUNNEL_MODE_ISATAP:
      return "isatap";
    case L7_TUNNEL_MODE_IP6:
      return "ipv6";
    default:
      break;
  }

  return "invalid";
}

/*********************************************************************
* @purpose  address-type printing helper function
*
* @param    addrType
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
L7_uchar8 *rlimAddrTypeString(L7_uint32 addrType)
{
  switch (addrType)
  {
    case RLIM_ADDRTYPE_UNDEFINED:
      return "undefined";
    case RLIM_ADDRTYPE_IP4:
      return "ip4";
    case RLIM_ADDRTYPE_IP6:
      return "ip6";
    case RLIM_ADDRTYPE_INTERFACE:
      return "interface";
    default:
      break;
  }

  return "invalid";
}

/*********************************************************************
* @purpose  Ip4 Address printing helper function
*
* @param    addr      address
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void rlimIp4AddrPrint(L7_uint32 addr)
{
  L7_uchar8 addrbuf[OSAPI_INET_NTOA_BUF_SIZE];
  osapiInetNtoa(addr, addrbuf);
  sysapiPrintf("%s", addrbuf);
}

/*********************************************************************
* @purpose  Ip6 Address printing helper function
*
* @param    addr      address
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void rlimIp6AddrPrint(L7_in6_addr_t *addr)
{
  L7_uchar8 addrbuf[40];
  sysapiPrintf("%s", osapiInetNtop(L7_AF_INET6, addr->in6.addr8,
                                   addrbuf, sizeof(addrbuf)));
}

/*********************************************************************
* @purpose  Tunnel flags printing helper function
*
* @param    flags
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void rlimTunnelFlagsPrint(L7_uint32 flags)
{
  L7_uint32 f = flags;

  sysapiPrintf("<");

  PRINT_FLAG(f, RLIM_TNNLOP_FLAG_ENABLED, "Enabled");
  PRINT_FLAG(f, RLIM_TNNLOP_FLAG_IP4_ROUTING, "V4Routing");
  PRINT_FLAG(f, RLIM_TNNLOP_FLAG_IP6_ROUTING, "V6Routing");
  PRINT_FLAG(f, RLIM_TNNLOP_FLAG_REACHABLE, "Reachable");
  PRINT_FLAG(f, RLIM_TNNLOP_FLAG_LOCALIP4UP, "V4LocalUp");
  PRINT_FLAG(f, RLIM_TNNLOP_FLAG_LOCALIP6UP, "V6LocalUp");
  PRINT_FLAG(f, RLIM_TNNLOP_FLAG_OSAPI_INTF_CREATED, "OsapiCreated");
  PRINT_FLAG(f, RLIM_TNNLOP_FLAG_DTL_INTF_CREATED, "DtlCreated");
  PRINT_FLAG(f, RLIM_TNNLOP_FLAG_DISABLE_IN_PROGRESS, "DisableInProgress");
  PRINT_FLAG(f, RLIM_TNNLOP_FLAG_DELETE_IN_PROGRESS, "DeleteInProgress");
  PRINT_FLAG(f, RLIM_TNNLOP_FLAG_HAS_DUPLICATE, "HasDuplicate");
  PRINT_FLAG(f, RLIM_TNNLOP_FLAG_IS_DUPLICATE, "IsDuplicate");
  PRINT_FLAG(f, RLIM_TNNLOP_FLAG_PORT_ENABLED, "PortEnabled");
  PRINT_FLAG(f, RLIM_TNNLOP_FLAG_DTL_ROUTE_CREATED, "RouteCreated");
  if (f != 0)
    sysapiPrintf("%#x", f);

  sysapiPrintf(">");
}

/*********************************************************************
* @purpose  Print an interface Config ID
*
* @param    pCfgID
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void rlimNimConfigIdPrint(nimConfigID_t *pCfgID)
{
  switch (pCfgID->type)
  {
    case L7_LOGICAL_VLAN_INTF:
      sysapiPrintf("vlanId:%u", pCfgID->configSpecifier.vlanId);
      break;
    case L7_LAG_INTF:
      sysapiPrintf("dot3adIntf:%u", pCfgID->configSpecifier.dot3adIntf);
      break;
    case L7_LOOPBACK_INTF:
      sysapiPrintf("loopbackId:%u", pCfgID->configSpecifier.loopbackId);
      break;
    case L7_TUNNEL_INTF:
      sysapiPrintf("tunnelId:%u", pCfgID->configSpecifier.tunnelId);
      break;
    default:
      sysapiPrintf("usp:%d.%d.%d", pCfgID->configSpecifier.usp.unit,
                   pCfgID->configSpecifier.usp.slot,
                   pCfgID->configSpecifier.usp.port);
      break;
  }
}

/*********************************************************************
* @purpose  Address-printing helper function
*
* @param    addrType  type of the address
* @param    addr      address
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void rlimAddrPrint(rlimAddr_t *addr)
{
  switch (addr->addrType)
  {
    case RLIM_ADDRTYPE_UNDEFINED:
      sysapiPrintf("undefined");
      break;

    case RLIM_ADDRTYPE_IP4:
      rlimIp4AddrPrint(addr->un.ip4addr);
      break;

    case RLIM_ADDRTYPE_IP6:
      rlimIp6AddrPrint(&addr->un.ip6addr);
      break;

    case RLIM_ADDRTYPE_INTERFACE:
      rlimNimConfigIdPrint(&addr->un.intfConfigId);
      break;

    default:
      sysapiPrintf("invalid");
      break;
  }
}

/*********************************************************************
* @purpose  Dump RLIM state
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t rlimDebugShow()
{
  rlimLoopbackMask_t loopMask;
  rlimTunnelMask_t tnnlMask;
  L7_BOOL entryFound;
  L7_uint32 loopbackId = 0;
  L7_uint32 tunnelId = 0;
#ifdef L7_IPV6_PACKAGE
  rlim6to4Dest_t dummyDest, *tdest,*tdestNext;
#endif

#if 0
  /* This goes away now with text based config */
  sysapiPrintf("Config Filename: %s\n", rlimCfgData->cfgHdr.filename);
  sysapiPrintf("Version: %u\n", rlimCfgData->cfgHdr.version);
  sysapiPrintf("Type: %u\n", rlimCfgData->cfgHdr.type);
  sysapiPrintf("Length: %u\n", rlimCfgData->cfgHdr.length);
  sysapiPrintf("Checksum: %#x\n", rlimCfgData->checkSum);
#endif
  sysapiPrintf("Component ID: %u\n", rlimCfgData->cfgHdr.componentID);
  sysapiPrintf("Data changed: %u\n", rlimCfgData->cfgHdr.dataChanged);

  loopMask = rlimCfgData->loopbackMask;

  RLIM_MASK_FLBIT(loopMask, loopbackId);
  RLIM_MASK_NONZERO(loopMask, entryFound);

  sysapiPrintf("Configured Loopback interfaces:\n");
  while (entryFound)
  {
    sysapiPrintf("    loopback%u\n", loopbackId);

    RLIM_MASK_CLRBIT(loopMask, loopbackId);
    RLIM_MASK_FLBIT(loopMask, loopbackId);
    RLIM_MASK_NONZERO(loopMask, entryFound);
  }

  sysapiPrintf("Loopback Interface Internal Interface Numbers:\n");
  for (loopbackId = 0; loopbackId <= RLIM_MAX_LOOPBACK_ID; loopbackId++)
  {
    rlimLoopbackOpData_t *ip = &rlimLoopbackOpData[loopbackId];
    if (ip->loopbackIntIfNum != 0)
    {
      sysapiPrintf("    %u (loopback%u)\n", ip->loopbackIntIfNum, loopbackId);
    }
  }

  tnnlMask = rlimCfgData->tunnelMask;

  RLIM_MASK_FLBIT(tnnlMask, tunnelId);
  RLIM_MASK_NONZERO(tnnlMask, entryFound);

  sysapiPrintf("Configured Tunnel interfaces:\n");
  while (entryFound)
  {
    if (tunnelId > RLIM_MAX_TUNNEL_ID)
      break;

    rlimTunnelCfgData_t *tcp = &rlimCfgData->tunnelCfgData[tunnelId];

    sysapiPrintf("    tunnel%u\n", tunnelId);
    sysapiPrintf("        mode: %s\n", rlimTunnelModeString(tcp->tunnelMode));

    sysapiPrintf("        local addr: ");
    rlimAddrPrint(&tcp->localAddr);
    sysapiPrintf("\n");

    sysapiPrintf("        remote addr: ");
    rlimAddrPrint(&tcp->remoteAddr);
    sysapiPrintf("\n");

    RLIM_MASK_CLRBIT(tnnlMask, tunnelId);
    RLIM_MASK_FLBIT(tnnlMask, tunnelId);
    RLIM_MASK_NONZERO(tnnlMask, entryFound);
  }

  sysapiPrintf("Tunnel Interface Operational Data:\n");
  for (tunnelId = 0; tunnelId <= RLIM_MAX_TUNNEL_ID; tunnelId++)
  {
    rlimTunnelOpData_t *ip = &rlimTunnelOpData[tunnelId];
    if (ip->tunnelIntIfNum != 0)
    {
      sysapiPrintf("    tunnel%u\n", tunnelId);
      sysapiPrintf("        intIfNum: %u\n", ip->tunnelIntIfNum);

      sysapiPrintf("        flags: ");
      rlimTunnelFlagsPrint(ip->flags);
      sysapiPrintf("\n");

      sysapiPrintf("        localIp4Addr: ");
      rlimIp4AddrPrint(ip->localIp4Addr);
      sysapiPrintf("\n");

      sysapiPrintf("        localIp6Addr: ");
      rlimIp6AddrPrint(&ip->localIp6Addr);
      sysapiPrintf("\n");

      sysapiPrintf("        nextHopIp4Addr: ");
      rlimIp4AddrPrint(ip->nextHopIp4Addr);
      sysapiPrintf("\n");

      sysapiPrintf("        nextHopIp6Addr: ");
      rlimIp6AddrPrint(&ip->nextHopIp6Addr);
      sysapiPrintf("\n");

      sysapiPrintf("        localIntIfNum: %u\n", ip->localIntIfNum);
    }
  }
#ifdef L7_IPV6_PACKAGE
  sysapiPrintf("6to4 Tunnel Destination Operational Data:\n");
  memset(&dummyDest, 0, sizeof(dummyDest));

  /* Get first entry in the tree. */
  tdest = avlSearchLVL7(&rlim6to4TunnelDestTree, &dummyDest, AVL_EXACT);
  if (tdest == L7_NULLPTR)
  {
      tdest = avlSearchLVL7(&rlim6to4TunnelDestTree, &dummyDest, AVL_NEXT);
  }
  while (tdest)
  {
    tdestNext = avlSearchLVL7(&rlim6to4TunnelDestTree, tdest, AVL_NEXT);

    sysapiPrintf("    tunnel%u\n", tdest->tunnelId);
    sysapiPrintf("        remoteIp4Addr:  ");
    rlimIp4AddrPrint(tdest->remoteAddr);
    sysapiPrintf("\n");
    sysapiPrintf("        nextHopIp4Addr: ");
    rlimIp4AddrPrint(tdest->nextHopIp4Addr);
    sysapiPrintf("\n");
    sysapiPrintf("        flags:          ");
    rlimTunnelFlagsPrint(tdest->flags);
    sysapiPrintf("\n");

    tdest = tdestNext;
  }
#endif
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Test mask utilities
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t rlimDebugMaskTest()
{
  L7_uint32 i, val;
  struct {
    L7_uchar8 value[1];
  } mask1;
#define MASK1_MAX_BIT ((sizeof(mask1.value)*8)-1)
  struct {
    L7_uchar8 value[9];
  } mask9;
#define MASK9_MAX_BIT ((sizeof(mask9.value)*8)-1)
  L7_uint32 tests_passed = 0;
  L7_uint32 tests_failed = 0;
  L7_BOOL result;

  memset(&mask1, 0, sizeof(mask1));
  RLIM_MASK_NONZERO(mask1, result);
  TEST_ASSERT(!result, 0, 1, tests_passed, tests_failed);

  for (i = 0; i <= MASK1_MAX_BIT; i++)
  {
    TEST_ASSERT(!RLIM_MASK_ISBITSET(mask1, i), 0, 1,
                tests_passed, tests_failed);
    RLIM_MASK_FHBIT(mask1, val);
    TEST_ASSERT(val == RLIM_MASK_INVALID_BIT(mask1), val, i,
                tests_passed, tests_failed);
    RLIM_MASK_FLBIT(mask1, val);
    TEST_ASSERT(val == RLIM_MASK_INVALID_BIT(mask1), val, i,
                tests_passed, tests_failed);
  }

  for (i = 0; i <= MASK1_MAX_BIT; i++)
  {
    RLIM_MASK_SETBIT(mask1, i);
    TEST_ASSERT(RLIM_MASK_ISBITSET(mask1, i), 0, 1,
                tests_passed, tests_failed);
    RLIM_MASK_FHBIT(mask1, val);
    TEST_ASSERT(val == i, val, i, tests_passed, tests_failed);
    RLIM_MASK_FLBIT(mask1, val);
    TEST_ASSERT(val == i, val, i, tests_passed, tests_failed);
    RLIM_MASK_CLRBIT(mask1, i);
    RLIM_MASK_NONZERO(mask1, result);
    TEST_ASSERT(!result, 0, 1, tests_passed, tests_failed);
    TEST_ASSERT(!RLIM_MASK_ISBITSET(mask1, i), 0, 1,
                tests_passed, tests_failed);
    RLIM_MASK_FHBIT(mask1, val);
    TEST_ASSERT(val == RLIM_MASK_INVALID_BIT(mask1), val,
                RLIM_MASK_INVALID_BIT(mask1), tests_passed, tests_failed);
    RLIM_MASK_FLBIT(mask1, val);
    TEST_ASSERT(val == RLIM_MASK_INVALID_BIT(mask1), val,
                RLIM_MASK_INVALID_BIT(mask1), tests_passed, tests_failed);
  }

  memset(&mask9, 0, sizeof(mask9));
  RLIM_MASK_NONZERO(mask9, result);
  TEST_ASSERT(!result, 0, 1, tests_passed, tests_failed);

  for (i = 0; i <= MASK9_MAX_BIT; i++)
  {
    TEST_ASSERT(!RLIM_MASK_ISBITSET(mask9, i), 0, 1,
                tests_passed, tests_failed);
    RLIM_MASK_FHBIT(mask9, val);
    TEST_ASSERT(val == RLIM_MASK_INVALID_BIT(mask9), val, i,
                tests_passed, tests_failed);
    RLIM_MASK_FLBIT(mask9, val);
    TEST_ASSERT(val == RLIM_MASK_INVALID_BIT(mask9), val, i,
                tests_passed, tests_failed);
  }

  for (i = 0; i <= MASK9_MAX_BIT; i++)
  {
    RLIM_MASK_SETBIT(mask9, i);
    RLIM_MASK_NONZERO(mask9, result);
    TEST_ASSERT(result, 0, 1, tests_passed, tests_failed);
    TEST_ASSERT(RLIM_MASK_ISBITSET(mask9, i), 0, 1,
                tests_passed, tests_failed);
    RLIM_MASK_FHBIT(mask9, val);
    TEST_ASSERT(val == i, val, i, tests_passed, tests_failed);
    RLIM_MASK_FLBIT(mask9, val);
    TEST_ASSERT(val == i, val, i, tests_passed, tests_failed);
    RLIM_MASK_CLRBIT(mask9, i);
    RLIM_MASK_NONZERO(mask9, result);
    TEST_ASSERT(!result, 0, 1, tests_passed, tests_failed);
    TEST_ASSERT(!RLIM_MASK_ISBITSET(mask9, i), 0, 1,
                tests_passed, tests_failed);
    RLIM_MASK_FHBIT(mask9, val);
    TEST_ASSERT(val == RLIM_MASK_INVALID_BIT(mask9), val,
                RLIM_MASK_INVALID_BIT(mask9), tests_passed, tests_failed);
    RLIM_MASK_FLBIT(mask9, val);
    TEST_ASSERT(val == RLIM_MASK_INVALID_BIT(mask9), val,
                RLIM_MASK_INVALID_BIT(mask9), tests_passed, tests_failed);
  }

  sysapiPrintf("%u tests passed, %u tests failed\n",
               tests_passed, tests_failed);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Test get loopbackId utility
*
* @param    intIfNum
*
* @returns  L7_SUCCESS  if intIfNum refers to a valid loopback interface
* @returns  L7_FAILURE  otherwise
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t rlimDebugLoopbackIdGet(L7_uint32 intIfNum)
{
  L7_uint32 loopbackId;

  if (rlimLoopbackIdGet(intIfNum, &loopbackId) != L7_SUCCESS)
  {
    sysapiPrintf("Could not get loopbackId\n");
    return L7_FAILURE;
  }

  sysapiPrintf("loopbackId = %u\n", loopbackId);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Test get loopback intIfNum utility
*
* @param    loopbackId
*
* @returns  L7_SUCCESS  if loopbackId is valid
* @returns  L7_FAILURE  otherwise
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t rlimDebugLoopbackIntIfNumGet(L7_uint32 loopbackId)
{
  L7_uint32 intIfNum;

  if (rlimLoopbackIntIfNumGet(loopbackId, &intIfNum) != L7_SUCCESS)
  {
    sysapiPrintf("Could not get internal interface identifer\n");
    return L7_FAILURE;
  }

  sysapiPrintf("intIfNum = %u\n", intIfNum);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Test get tunnelId utility
*
* @param    intIfNum  if intIfNum refers to a valid tunnel interface
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t rlimDebugTunnelIdGet(L7_uint32 intIfNum)
{
  L7_uint32 tunnelId;

  if (rlimTunnelIdGet(intIfNum, &tunnelId) != L7_SUCCESS)
  {
    sysapiPrintf("Could not get tunnelId\n");
    return L7_FAILURE;
  }

  sysapiPrintf("tunnelId = %u\n", tunnelId);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Test get tunnel intIfNum utility
*
* @param    tunnelId  is tunnelId is valid
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t rlimDebugTunnelIntIfNumGet(L7_uint32 tunnelId)
{
  L7_uint32 intIfNum;

  if (rlimTunnelIntIfNumGet(tunnelId, &intIfNum) != L7_SUCCESS)
  {
    sysapiPrintf("Could not get internal interface identifer\n");
    return L7_FAILURE;
  }

  sysapiPrintf("intIfNum = %u\n", intIfNum);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Test setting of local/remote addresses on a tunnel
*
* @param    tunnelId
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t rlimDebugTunnelAddrSet(L7_uint32 intIfNum, L7_uchar8 *addrStr,
                               L7_uint32 local)
{
  L7_uchar8 *s;
  L7_BOOL isv4 = L7_TRUE;

  for (s = addrStr; (*s) != '\0'; s++)
  {
    if (*s == ':')
    {
      isv4 = L7_FALSE;
      break;
    }
  }

  if (isv4)
  {
    L7_uint32 ip4addr = osapiInet_addr(addrStr);
    if (local)
    {
      return rlimTunnelLocalIp4AddrSet(intIfNum, ip4addr);
    }
    else
    {
      return rlimTunnelRemoteIp4AddrSet(intIfNum, ip4addr);
    }
  }
  else
  {
    L7_in6_addr_t ip6addr;
    if (osapiInetPton(L7_AF_INET6, addrStr, ip6addr.in6.addr8) != L7_SUCCESS)
    {
      sysapiPrintf("failed to convert IPv6 address: %s\n", addrStr);
      return L7_FAILURE;
    }

    if (local)
    {
      return rlimTunnelLocalIp6AddrSet(intIfNum, &ip6addr);
    }
    else
    {
      return rlimTunnelRemoteIp6AddrSet(intIfNum, &ip6addr);
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Test setting of an Ip4 address
*
* @param    tunnelId
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t rlimDebugTunnelGet(L7_uint32 intIfNum)
{
  L7_RC_t rc;
  L7_uint32 tunnelMode;
  L7_uint32 addrType;
  L7_uint32 ip4addr;
  L7_in6_addr_t ip6addr;
  L7_uint32 localIntIfNum;
  L7_uint32 hopLimit;
  L7_uint32 security;
  L7_uint32 tos;
  L7_uint32 flowLabel;
  L7_uint32 encapsLimit;

  sysapiPrintf("Tunnel Mode: ");
  rc = rlimTunnelModeGet(intIfNum, &tunnelMode);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("call failed (%u)", rc);
  }
  else
  {
    sysapiPrintf("%s", rlimTunnelModeString(tunnelMode));
  }
  sysapiPrintf("\n");

  sysapiPrintf("Cfg Local AddrType: ");
  rc = rlimTunnelCfgLocalAddrTypeGet(intIfNum, &addrType);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("call failed (%u)", rc);
  }
  else
  {
    sysapiPrintf("%s", rlimAddrTypeString(addrType));
  }
  sysapiPrintf("\n");

  sysapiPrintf("Local AddrType: ");
  rc = rlimTunnelLocalAddrTypeGet(intIfNum, &addrType);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("call failed (%u)", rc);
  }
  else
  {
    sysapiPrintf("%s", rlimAddrTypeString(addrType));
  }
  sysapiPrintf("\n");

  sysapiPrintf("Remote AddrType: ");
  rc = rlimTunnelRemoteAddrTypeGet(intIfNum, &addrType);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("call failed (%u)", rc);
  }
  else
  {
    sysapiPrintf("%s", rlimAddrTypeString(addrType));
  }
  sysapiPrintf("\n");

  sysapiPrintf("Local IP4 Addr: ");
  rc = rlimTunnelLocalIp4AddrGet(intIfNum, &ip4addr);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("call failed (%u)", rc);
  }
  else
  {
    rlimIp4AddrPrint(ip4addr);
  }
  sysapiPrintf("\n");

  sysapiPrintf("Remote IP4 Addr: ");
  rc = rlimTunnelRemoteIp4AddrGet(intIfNum, &ip4addr);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("call failed (%u)", rc);
  }
  else
  {
    rlimIp4AddrPrint(ip4addr);
  }
  sysapiPrintf("\n");

  sysapiPrintf("Local IP6 Addr: ");
  rc = rlimTunnelLocalIp6AddrGet(intIfNum, &ip6addr);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("call failed (%u)", rc);
  }
  else
  {
    rlimIp6AddrPrint(&ip6addr);
  }
  sysapiPrintf("\n");

  sysapiPrintf("Local Interface: ");
  rc = rlimTunnelLocalIntfGet(intIfNum, &localIntIfNum);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("call failed (%u)", rc);
  }
  else
  {
    sysapiPrintf("%u", localIntIfNum);
  }
  sysapiPrintf("\n");

  sysapiPrintf("Remote IP6 Addr: ");
  rc = rlimTunnelRemoteIp6AddrGet(intIfNum, &ip6addr);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("call failed (%u)", rc);
  }
  else
  {
    rlimIp6AddrPrint(&ip6addr);
  }
  sysapiPrintf("\n");

  sysapiPrintf("HopLimit: ");
  rc = rlimTunnelHopLimitGet(intIfNum, &hopLimit);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("call failed (%u)", rc);
  }
  else
  {
    sysapiPrintf("%u", hopLimit);
  }
  sysapiPrintf("\n");

  sysapiPrintf("Security: ");
  rc = rlimTunnelSecurityGet(intIfNum, &security);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("call failed (%u)", rc);
  }
  else
  {
    sysapiPrintf("%u", security);
  }
  sysapiPrintf("\n");

  sysapiPrintf("TOS: ");
  rc = rlimTunnelTOSGet(intIfNum, &tos);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("call failed (%u)", rc);
  }
  else
  {
    sysapiPrintf("%u", tos);
  }
  sysapiPrintf("\n");

  sysapiPrintf("FlowLabel: ");
  rc = rlimTunnelFlowLabelGet(intIfNum, &flowLabel);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("call failed (%u)", rc);
  }
  else
  {
    sysapiPrintf("%u", flowLabel);
  }
  sysapiPrintf("\n");

  sysapiPrintf("EncapsLimit: ");
  rc = rlimTunnelEncapsLimitGet(intIfNum, &encapsLimit);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("call failed (%u)", rc);
  }
  else
  {
    sysapiPrintf("%u", encapsLimit);
  }
  sysapiPrintf("\n");

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Test iterating through valid loopbacks and tunnels
*
* @param    tunnelId
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t rlimDebugNextGet(L7_uint32 intIfNum)
{
  L7_uint32 id;
  L7_RC_t rc;

  sysapiPrintf("Valid loopback interfaces:\n");
  rc = rlimLoopbackIdFirstGet(&id);
  while (rc == L7_SUCCESS)
  {
    sysapiPrintf("    loopback%u\n", id);
    rc = rlimLoopbackIdNextGet(id, &id);
  }

  sysapiPrintf("Valid tunnel interfaces:\n");
  rc = rlimTunnelIdFirstGet(&id);
  while (rc == L7_SUCCESS)
  {
    sysapiPrintf("    tunnel%u\n", id);
    rc = rlimTunnelIdNextGet(id, &id);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Enable debug output
*
* @param    enable - whether to enable/disable output
*
* @returns  L7_SUCCESS;
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t rlimDebugEnable(L7_uint32 enable)
{
  rlimDebug = enable;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Enable debug output
*
* @param    enable - whether to enable/disable output
*
* @returns  void 
*
* @notes    none
*
* @end
*********************************************************************/
void rlimDebugTrace(L7_uint32 flags)
{
  rlimDebugTraceFlags = flags;
}

/*********************************************************************
* @purpose  To print the debug Trace 
*
* @param    buffer that stores debug messages 
*
* @returns  void 
*
* @notes    none
*
* @end
*********************************************************************/
void rlimDebugTraceWrite(L7_char8 *traceMsg)
{
  L7_uint32 stackUptime;
  L7_uint32 secs;
  L7_uint32 msecs;
  L7_uchar8 debugMsg[512];

  if (traceMsg == NULL)
    return;

  stackUptime = simSystemUpTimeMsecGet();
  secs = stackUptime / 1000;
  msecs = stackUptime % 1000;

  /* For now, just print the message with a timestamp. */
  osapiSnprintf(debugMsg, sizeof(debugMsg), "\n%u.%03u: %s", secs, msecs, traceMsg);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,debugMsg);
}
