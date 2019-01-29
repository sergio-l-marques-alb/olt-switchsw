/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename pml_api.c
*
* @purpose Port MAC Locking API functions
*
* @component Port MAC Locking
*
* @comments none
*
* @create 05/21/2004
*
* @author colinw
* @end
*
**********************************************************************/
/*********************************************************************
 *
 ********************************************************************/

#include <string.h>
#include "l7_common.h"
#include "nimapi.h"
#include "pml.h"
#include "pml_api.h"
#include "pml_util.h"
#include "osapi.h"
#include "osapi_support.h"
#include "dtlapi.h"
#include "trapapi.h"
#include "fdb_api.h"
#include "pml_exports.h"

#ifdef L7_DHCP_SNOOPING_PACKAGE
#include "dhcp_snooping_api.h"
#endif

extern void fdbInsert(char *mac, L7_uint32 intIfNum, L7_uint32 virtual_port, L7_uint32 vlanId, L7_ushort16 entryType);  /* PTin modified: virtual ports */
extern void fdbDelete(char *mac, L7_uint32 vlanId);

extern L7_BOOL pmlMapIntfIsAttached(L7_uint32 intIfNum);

extern pmlCfgData_t *pmlCfgData;

/*********************************************************************
*
* @purpose  Gets the Port MAC Locking Admin mode
*
* @param    L7_uint32  *adminMode   @b((output)) Port MAC Locking admin mode
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlAdminModeGet(L7_uint32 *adminMode)
{
  *adminMode = (pmlCfgData->globalLockEnabled == L7_TRUE) ? L7_ENABLE : L7_DISABLE;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Sets the Port MAC Locking Admin mode
*
* @param    L7_uint32  adminMode   @b((input)) Port MAC Locking admin mode
*
* @returns  L7_SUCCESS, if mode was set
* @returns  L7_FAILURE, if invalid mode was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlAdminModeSet(L7_uint32 adminMode)
{
  if (adminMode != L7_ENABLE && adminMode != L7_DISABLE)
    return L7_FAILURE;

  if (adminMode == pmlCfgData->globalLockEnabled)
    return L7_SUCCESS;

  pmlCfgData->globalLockEnabled = ((adminMode == L7_ENABLE) ? L7_TRUE : L7_FALSE);

  pmlSemaTake();
  pmlAdminModeApply(adminMode);
  pmlSemaGive();

  pmlCfgData->cfgHdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Sets the Port MAC Locking mode for the specified interface
*
* @param    L7_uint32  intIfNum  @b((output)) Internal interface number
* @param    L7_uint32  mode      @b((input))  Port MAC Locking intf mode
*
* @returns  L7_SUCCESS, if mode was set successfully
* @returns  L7_FAILURE, if interface was invalid or if invalid mode was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlIntfModeSet(L7_uint32 intIfNum, L7_uint32 mode)
{
  pmlIntfCfgData_t *pCfg;
  pmlIntfOprData_t *pOpr;

  if (mode != L7_ENABLE && mode != L7_DISABLE)
    return L7_FAILURE;

  pmlSemaTake();
  if (pmlMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    pmlSemaGive();
    return L7_FAILURE;
  }

  if (pmlMapIntfIsOperational(intIfNum, &pOpr) != L7_TRUE)
  {
    pmlSemaGive();
    return L7_FAILURE;
  }

  if (mode == pCfg->intfLockEnabled)
  {
    pmlSemaGive();
    return L7_SUCCESS;
  }

  pCfg->intfLockEnabled = (mode == L7_ENABLE) ? L7_TRUE : L7_FALSE;

  /* Apply the changes to hardware only if interface is not acquired */
  if(!pmlMapIntfIsAcquired(intIfNum))
  {
    /* Apply the config change */
    pmlIntfModeApply(intIfNum, mode, L7_FALSE);

    if (mode == L7_FALSE)
    {
      /* purge the violation tree for this port */
      pOpr->lastViolationAddr.vlanId = 0;
      memset(pOpr->lastViolationAddr.macAddr.addr, 0 ,sizeof(pOpr->lastViolationAddr.macAddr.addr));

      if (pCfg->violationTrapsEnabled == L7_TRUE)
      {
        avlPurgeAvlTree(&pOpr->violationAvlTree, PML_VIOLATION_TREE_SIZE);
      }
    }
  }

  pmlCfgData->cfgHdr.dataChanged = L7_TRUE;
  pmlSemaGive();
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets the configured Port MAC Locking mode for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))  Internal interface number
* @param    L7_uint32  *mode     @b((output)) Configured Port MAC Locking intf mode
*
*
* @returns  L7_SUCCESS, if mode was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlIntfModeGet(L7_uint32 intIfNum, L7_uint32 *mode)
{
  pmlIntfCfgData_t *pCfg;

  if (pmlIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  pmlSemaTake();
  if (pmlMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    pmlSemaGive();
    return L7_FAILURE;
  }

  *mode = (pCfg->intfLockEnabled == L7_TRUE) ? L7_ENABLE : L7_DISABLE;

  pmlSemaGive();
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets the Port MAC Locking dynamic limit for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))  Internal interface number
* @param    L7_uchar8  *limit    @b((output)) Dynamic Limit
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlIntfDynamicLimitGet(L7_uint32 intIfNum, L7_uint32 *limit)
{
  pmlIntfCfgData_t *pCfg;

  if (pmlIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  pmlSemaTake();
  if (pmlMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    pmlSemaGive();
    return L7_FAILURE;
  }

  *limit = pCfg->dynamicLimit;

  pmlSemaGive();
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets the Port MAC Locking maximum dynamic limit for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))  Internal interface number
* @param    L7_uchar8  *limit    @b((output)) Maximum Dynamic Limit
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlIntfDynamicMaxLimitGet(L7_uint32 intIfNum, L7_uint32 *limit)
{
  if (pmlIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  *limit = L7_MACLOCKING_MAX_DYNAMIC_ADDRESSES;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Sets the Port MAC Locking dynamic limit for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input)) Internal interface number
* @param    L7_uchar8  limit     @b((input))  Dynamic Limit
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlIntfDynamicLimitSet(L7_uint32 intIfNum, L7_uint32 limit)
{
  pmlIntfCfgData_t *pCfg;
  pmlIntfOprData_t *pOpr;

  if (pmlIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  pmlSemaTake();
  if (pmlMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    pmlSemaGive();
    return L7_FAILURE;
  }

  if (pmlMapIntfIsOperational(intIfNum, &pOpr) != L7_TRUE)
  {
    pmlSemaGive();
    return L7_FAILURE;
  }

  if (limit == pCfg->dynamicLimit)
  {
    pmlSemaGive();
    return L7_SUCCESS;
  }

  if (limit > L7_MACLOCKING_MAX_DYNAMIC_ADDRESSES)
  {
    pmlSemaGive();
    return L7_FAILURE;
  }

  /* Only modify HW if intf is attached. */
  if (pmlMapIntfIsAttached(intIfNum))
  {
    if (limit < pCfg->dynamicLimit)
    {
      if ((pmlCfgData->globalLockEnabled == L7_TRUE) && (pCfg->intfLockEnabled == L7_TRUE))
      {
        /* flush the dynamic entries on this port */
        dtlDot1sFlush(intIfNum);
      }
    }
  }

  pCfg->dynamicLimit = limit;

  pmlCfgData->cfgHdr.dataChanged = L7_TRUE;

  pmlSemaGive();
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets the Port MAC Locking static limit for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))  Internal interface number
* @param    L7_uchar8  *limit    @b((output)) Static Limit
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlIntfStaticLimitGet(L7_uint32 intIfNum, L7_uint32 *limit)
{
  pmlIntfCfgData_t *pCfg;

  if (pmlIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  pmlSemaTake();
  if (pmlMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    pmlSemaGive();
    return L7_FAILURE;
  }

  *limit = pCfg->staticLimit;

  pmlSemaGive();
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets the Port MAC Locking maximum static limit for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))  Internal interface number
* @param    L7_uchar8  *limit    @b((output)) Maximum Static Limit
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlIntfStaticMaxLimitGet(L7_uint32 intIfNum, L7_uint32 *limit)
{
  if (pmlIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  *limit = L7_MACLOCKING_MAX_STATIC_ADDRESSES;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets the Port MAC Locking current static entries used count
*           for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))  Internal interface number
* @param    L7_uchar8  *count    @b((output)) Count
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlIntfStaticCountGet(L7_uint32 intIfNum, L7_uint32 *count)
{
  pmlIntfOprData_t *pOpr;

  if (pmlIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  pmlSemaTake();
  if (pmlMapIntfIsOperational(intIfNum, &pOpr) != L7_TRUE)
  {
    pmlSemaGive();
    return L7_FAILURE;
  }

  *count = pOpr->staticCount;

  pmlSemaGive();
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Sets the Port MAC Locking static limit for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input)) Internal interface number
* @param    L7_uchar8  limit     @b((input)) Static Limit
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlIntfStaticLimitSet(L7_uint32 intIfNum, L7_uint32 limit)
{
  pmlIntfCfgData_t  *pCfg;
  pmlIntfOprData_t  *pOpr;
  L7_uint32          count;
  L7_uint32          i;

  if (pmlIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  pmlSemaTake();
  if (pmlMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    pmlSemaGive();
    return L7_FAILURE;
  }

  if (pmlMapIntfIsOperational(intIfNum, &pOpr) != L7_TRUE)
  {
    pmlSemaGive();
    return L7_FAILURE;
  }

  if (limit == pCfg->staticLimit)
  {
    pmlSemaGive();
    return L7_SUCCESS;
  }

  if (limit > L7_MACLOCKING_MAX_STATIC_ADDRESSES)
  {
    pmlSemaGive();
    return L7_FAILURE;
  }

  /* if static limit has decreased, we need to remove some entries from HW */
  if (limit < pCfg->staticLimit)
  {
    count = 0;

    for (i = 0; i < L7_MACLOCKING_MAX_STATIC_ADDRESSES; i++)
    {
      if (pCfg->staticMacEntry[i].vlanId != 0)
      {
        count++;

        if (count > limit)
        {
          /* this entry is over the limit... remove it */
          pOpr->staticCount--;

          /* tell FDB component about this address */
          fdbDelete(pCfg->staticMacEntry[i].macAddr.addr, pCfg->staticMacEntry[i].vlanId);

          /* remove the entry from the HW */
          dtlFdbMacAddrDelete(pCfg->staticMacEntry[i].macAddr.addr, intIfNum, pCfg->staticMacEntry[i].vlanId, 0, L7_NULL);

          pCfg->staticMacEntry[i].vlanId = 0;
          memset(pCfg->staticMacEntry[i].macAddr.addr, 0, L7_ENET_MAC_ADDR_LEN);
        }
      }
    }
  }

  pCfg->staticLimit = limit;

  pmlCfgData->cfgHdr.dataChanged = L7_TRUE;

  pmlSemaGive();
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets the Port MAC Locking violation trap mode for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))  Internal interface number
* @param    L7_uchar8  *trapMode @b((output)) Violation Trap Mode
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlIntfViolationTrapModeGet(L7_uint32 intIfNum, L7_uint32 *trapMode)
{
  pmlIntfCfgData_t *pCfg;

  if (pmlIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  pmlSemaTake();
  if (pmlMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    pmlSemaGive();
    return L7_FAILURE;
  }

  *trapMode = (pCfg->violationTrapsEnabled == L7_TRUE) ? L7_ENABLE : L7_DISABLE;

  pmlSemaGive();
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Sets the Port MAC Locking violation trap mode for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input)) Internal interface number
* @param    L7_uchar8  trapMode  @b((input)) Violation Trap Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlIntfViolationTrapModeSet(L7_uint32 intIfNum, L7_uint32 trapMode)
{
  pmlIntfCfgData_t *pCfg;
  pmlIntfOprData_t *pOpr;

  if (trapMode != L7_ENABLE && trapMode != L7_DISABLE)
    return L7_FAILURE;

  pmlSemaTake();
  if (pmlMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    pmlSemaGive();
    return L7_FAILURE;
  }

  if (pmlMapIntfIsOperational(intIfNum, &pOpr) != L7_TRUE)
  {
    pmlSemaGive();
    return L7_FAILURE;
  }

  if (pCfg->violationTrapsEnabled == trapMode)
  {
    pmlSemaGive();
    return L7_SUCCESS;
  }

  pCfg->violationTrapsEnabled = (trapMode == L7_ENABLE) ? L7_TRUE : L7_FALSE;

  /* remove all entries from the violation tree */
  if (pCfg->violationTrapsEnabled == L7_FALSE)
  {
    avlPurgeAvlTree(&pOpr->violationAvlTree, PML_VIOLATION_TREE_SIZE);
  }

  pmlCfgData->cfgHdr.dataChanged = L7_TRUE;
  pmlSemaGive();
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Gets the Frequency interval between successive SNMP traps
*
* @param L7_uint32 intIfNum   @b((input)) internal interface number
* @param L7_uint32 seconds    @b((input)) Time interval in seconds
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t pmlIntfTrapFrequencyGet(L7_uint32 intIfNum, L7_uint32 *seconds)
{
  pmlIntfCfgData_t *pCfg;

  if (pmlIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  pmlSemaTake();
  if (pmlMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    pmlSemaGive();
    return L7_FAILURE;
  }

  *seconds = pCfg->violationTrapsSeconds;
  pmlSemaGive();
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Sets the Frequency interval between successive SNMP traps
*
* @param L7_uint32 intIfNum   @b((input)) internal interface number
* @param L7_uint32 seconds    @b((input)) Time interval in seconds
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t pmlIntfTrapFrequencySet(L7_uint32 intIfNum, L7_uint32 seconds)
{
  pmlIntfCfgData_t *pCfg;
  pmlIntfOprData_t *pOpr;

  if (seconds < L7_PML_VIOLATION_TRAPS_SECONDS_MIN ||
      seconds > L7_PML_VIOLATION_TRAPS_SECONDS_MAX)
  {
    return L7_FAILURE;
  }

  pmlSemaTake();
  if (pmlMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    pmlSemaGive();
    return L7_FAILURE;
  }

  if (pmlMapIntfIsOperational(intIfNum, &pOpr) != L7_TRUE)
  {
    pmlSemaGive();
    return L7_FAILURE;
  }

  if (pCfg->violationTrapsSeconds == seconds)
  {
    pmlSemaGive();
    return L7_SUCCESS;
  }

  pCfg->violationTrapsSeconds = seconds;
  pmlCfgData->cfgHdr.dataChanged = L7_TRUE;
  pmlSemaGive();
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Adds a statically locked entry to an interface.
*
* @param    L7_uint32        intIfNum  @b((input)) Internal interface number
* @param    L7_enetMacAddr_t macAddr   @b((input)) MAC address
* @param    L7_ushort16      vlanId    @b((input)) VLAN ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlIntfStaticEntryAdd(L7_uint32 intIfNum, L7_enetMacAddr_t macAddr, L7_ushort16 vlanId)
{
  pmlIntfCfgData_t  *pCfg;
  pmlIntfOprData_t  *pOpr;
  L7_uint32          i;
  L7_uchar8          key[L7_FDB_KEY_SIZE];
  pmlIntfOprData_t  *pTempOpr;
  L7_uint32          tempIntIfNum;
  dot1dTpFdbData_t   fdbEntry;
  L7_RC_t            rc;

  if (vlanId == 0)
  {
    return L7_FAILURE;
  }

  if ((macAddr.addr[0] & 0x01) == 0x01)
  {
    return L7_FAILURE;
  }

  pmlSemaTake();
  if (pmlMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    pmlSemaGive();
    return L7_FAILURE;
  }

  if (pmlMapIntfIsOperational(intIfNum, &pOpr) != L7_TRUE)
  {
    pmlSemaGive();
    return L7_FAILURE;
  }

  if ((pOpr->staticCount + 1) > pCfg->staticLimit)
  {
    pmlSemaGive();
    return L7_FAILURE;
  }

  /* search the FDB to determine if this MAC already exists.
     If it exists as a static entry, return failure. If it
     exists as a dynamic entry, convert it to static */
  memcpy(&key[0], &vlanId, sizeof(L7_ushort16));
  memcpy(&key[2], &macAddr.addr, L7_ENET_MAC_ADDR_LEN);

  rc = fdbFind(key, L7_MATCH_EXACT, &fdbEntry);

  if (rc == L7_SUCCESS)
  {
    if (fdbEntry.dot1dTpFdbEntryType == L7_FDB_ADDR_FLAG_STATIC)
    {
      pmlSemaGive();
      return L7_FAILURE;
    }
    else if (fdbEntry.dot1dTpFdbEntryType == L7_FDB_ADDR_FLAG_LEARNED)
    {
      tempIntIfNum = fdbEntry.dot1dTpFdbPort;

      if (pmlIsValidIntf(tempIntIfNum) == L7_FALSE)
      {
        pmlSemaGive();
        return L7_FAILURE;
      }

      if (pmlMapIntfIsOperational(tempIntIfNum, &pTempOpr) != L7_TRUE)
      {
        pmlSemaGive();
        return L7_FAILURE;
      }

      if (pTempOpr->dynamicCount > 0)
      {
        pTempOpr->dynamicCount--;
      }
    }
  }

  /* add the entry to the config structure */
  for (i = 0; i < L7_MACLOCKING_MAX_STATIC_ADDRESSES; i++)
  {
    if (pCfg->staticMacEntry[i].vlanId == 0)
    {
      pCfg->staticMacEntry[i].vlanId = vlanId;
      memcpy(pCfg->staticMacEntry[i].macAddr.addr, macAddr.addr, L7_ENET_MAC_ADDR_LEN);
      break;
    }
  }

  if (i == L7_MACLOCKING_MAX_STATIC_ADDRESSES)
  {
    /* shouldn't get here */
    L7_LOG_ERROR(0);
  }

  pOpr->staticCount++;

  /* tell FDB component about this address */
  fdbInsert(macAddr.addr, intIfNum, 0 /* Not used */, vlanId, L7_FDB_ADDR_FLAG_STATIC);

  /* Only modify HW if intf is attached. */
  if (pmlMapIntfIsAttached(intIfNum))
  {
    /* add the entry to the HW */
    dtlFdbMacAddrAdd(macAddr.addr, intIfNum, vlanId, L7_FDB_ADDR_FLAG_STATIC, L7_NULL);
  }

  pmlCfgData->cfgHdr.dataChanged = L7_TRUE;
  pmlSemaGive();
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Deletes a statically locked entry from an interface.
*
* @param    L7_uint32        intIfNum  @b((input)) Internal interface number
* @param    L7_enetMacAddr_t macAddr   @b((input)) MAC address
* @param    L7_ushort16      vlanId    @b((input)) VLAN ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlIntfStaticEntryDelete(L7_uint32 intIfNum, L7_enetMacAddr_t macAddr, L7_ushort16 vlanId)
{
  pmlIntfCfgData_t  *pCfg;
  pmlIntfOprData_t  *pOpr;
  L7_uint32          i;

  if (vlanId == 0)
  {
    return L7_FAILURE;
  }

  pmlSemaTake();
  if (pmlMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    pmlSemaGive();
    return L7_FAILURE;
  }

  if (pmlMapIntfIsOperational(intIfNum, &pOpr) != L7_TRUE)
  {
    pmlSemaGive();
    return L7_FAILURE;
  }

  /* remove the entry from the config structure */
  for (i = 0; i < L7_MACLOCKING_MAX_STATIC_ADDRESSES; i++)
  {
    if (pCfg->staticMacEntry[i].vlanId == vlanId)
    {
      if (memcmp(pCfg->staticMacEntry[i].macAddr.addr, macAddr.addr, L7_ENET_MAC_ADDR_LEN) == 0)
      {
        pCfg->staticMacEntry[i].vlanId = 0;
        memset(pCfg->staticMacEntry[i].macAddr.addr, 0, L7_ENET_MAC_ADDR_LEN);
        break;
      }
    }
  }

  if (i == L7_MACLOCKING_MAX_STATIC_ADDRESSES)
  {
    /* not found */
    pmlSemaGive();
    return L7_FAILURE;
  }

  pOpr->staticCount--;

  /* tell FDB component about this address */
  fdbDelete(macAddr.addr, vlanId);

  /* Only modify HW if intf is attached. */
  if (pmlMapIntfIsAttached(intIfNum))
  {
    /* remove the entry from the HW */
    dtlFdbMacAddrDelete(macAddr.addr, intIfNum, vlanId, 0, L7_NULL);
  }

  pmlCfgData->cfgHdr.dataChanged = L7_TRUE;
  pmlSemaGive();
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets the first static entry for an interface.
*
* @param    L7_uint32        intIfNum  @b((input)) Internal interface number
* @param    L7_enetMacAddr_t macAddr   @b((output)) MAC address
* @param    L7_ushort16      vlanId    @b((output)) VLAN ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlIntfStaticEntryGetFirst(L7_uint32 intIfNum, L7_enetMacAddr_t *macAddr, L7_ushort16 *vlanId)
{
  pmlIntfCfgData_t  *pCfg;
  L7_uint32          i;
  L7_BOOL            foundOne = L7_FALSE;
  L7_uchar8          lowestEntryValue[8];
  L7_uchar8          entryValue[8];

  pmlSemaTake();
  if (pmlMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    pmlSemaGive();
    return L7_FAILURE;
  }

  memset(lowestEntryValue, 0xff, 8);

  for (i = 0; i < L7_MACLOCKING_MAX_STATIC_ADDRESSES; i++)
  {
    if (pCfg->staticMacEntry[i].vlanId != 0)
    {
      memcpy(&entryValue[0], &pCfg->staticMacEntry[i].vlanId, 2);
      memcpy(&entryValue[2], pCfg->staticMacEntry[i].macAddr.addr, 6);

      if (memcmp(entryValue, lowestEntryValue, 8) < 0)
      {
        memcpy(lowestEntryValue, entryValue, 8);
        foundOne = L7_TRUE;
      }
    }
  }

  if (foundOne == L7_TRUE)
  {
    memcpy(macAddr->addr, &lowestEntryValue[2], 6);
    *vlanId = *(L7_ushort16 *)&lowestEntryValue[0];
    pmlSemaGive();
    return L7_SUCCESS;
  }

  vlanId = 0;
  memset(macAddr->addr, 0, L7_ENET_MAC_ADDR_LEN);

  pmlSemaGive();
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Gets the next static entry for an interface.
*
* @param    L7_uint32        intIfNum  @b((input)) Internal interface number
* @param    L7_enetMacAddr_t macAddr   @b((output)) MAC address
* @param    L7_ushort16      vlanId    @b((output)) VLAN ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlIntfStaticEntryGetNext(L7_uint32 intIfNum, L7_enetMacAddr_t *macAddr, L7_ushort16 *vlanId)
{
  pmlIntfCfgData_t  *pCfg;
  L7_uint32          i;
  L7_BOOL            foundOne = L7_FALSE;
  L7_uchar8          lowestEntryValue[8];
  L7_uchar8          entryValue[8];
  L7_uchar8          searchValue[8];

  pmlSemaTake();
  if (pmlMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    pmlSemaGive();
    return L7_FAILURE;
  }

  memset(lowestEntryValue, 0xff, 8);
  memcpy(&searchValue[0], vlanId, 2);
  memcpy(&searchValue[2], macAddr->addr, 6);

  for (i = 0; i < L7_MACLOCKING_MAX_STATIC_ADDRESSES; i++)
  {
    if (pCfg->staticMacEntry[i].vlanId != 0)
    {
      memcpy(&entryValue[0], &pCfg->staticMacEntry[i].vlanId, 2);
      memcpy(&entryValue[2], pCfg->staticMacEntry[i].macAddr.addr, 6);

      /* if this entry is greater than the one passed in */
      if (memcmp(entryValue, searchValue, 8) > 0)
      {
        /* if this entry is less than our temp entry */
        if (memcmp(entryValue, lowestEntryValue, 8) < 0)
        {
          memcpy(lowestEntryValue, entryValue, 8);
          foundOne = L7_TRUE;
        }
      }
    }
  }

  if (foundOne == L7_TRUE)
  {
    memcpy(macAddr->addr, &lowestEntryValue[2], 6);
    *vlanId = *(L7_ushort16 *)&lowestEntryValue[0];
    pmlSemaGive();
    return L7_SUCCESS;
  }

  vlanId = 0;
  memset(macAddr->addr, 0, L7_ENET_MAC_ADDR_LEN);

  pmlSemaGive();
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Gets the first dynamic entry for an interface.
*
* @param    L7_uint32        intIfNum  @b((input)) Internal interface number
* @param    L7_enetMacAddr_t macAddr   @b((output)) MAC address
* @param    L7_ushort16      vlanId    @b((output)) VLAN ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlIntfDynamicEntryGetFirst(L7_uint32 intIfNum, L7_enetMacAddr_t *macAddr, L7_ushort16 *vlanId)
{
  L7_uchar8          key[L7_FDB_KEY_SIZE] = {0};
  dot1dTpFdbData_t   fdbEntry;
  L7_RC_t            rc;
  L7_BOOL            found = L7_FALSE;
  pmlIntfCfgData_t  *pCfg;

  pmlSemaTake();
  if (pmlMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    vlanId = 0;
    memset(macAddr->addr, 0, L7_ENET_MAC_ADDR_LEN);

    pmlSemaGive();
    return L7_FAILURE;
  }
  pmlSemaGive();

  if ((pmlCfgData->globalLockEnabled == L7_TRUE) && (pCfg->intfLockEnabled == L7_TRUE))
  {
    do
    {
      rc = fdbFind(key, L7_MATCH_GETNEXT, &fdbEntry);

      if (rc == L7_SUCCESS)
      {
        memcpy(&key[0], fdbEntry.dot1dTpFdbAddress, L7_FDB_KEY_SIZE);

        if ((fdbEntry.dot1dTpFdbPort == intIfNum) && (fdbEntry.dot1dTpFdbEntryType == L7_FDB_ADDR_FLAG_LEARNED))
        {
          memcpy(macAddr->addr, &fdbEntry.dot1dTpFdbAddress[2], 6);
          memcpy(vlanId, &fdbEntry.dot1dTpFdbAddress[0], sizeof(L7_ushort16));
          *vlanId = osapiNtohs(*vlanId); /* convert back to host byte order */
          found = L7_TRUE;
        }
      }
      else
      {
        break;
      }
    } while(found == L7_FALSE);
  }

  if (found == L7_FALSE)
  {
    vlanId = 0;
    memset(macAddr->addr, 0, L7_ENET_MAC_ADDR_LEN);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets the next dynamic entry for an interface.
*
* @param    L7_uint32        intIfNum  @b((input)) Internal interface number
* @param    L7_enetMacAddr_t macAddr   @b((output)) MAC address
* @param    L7_ushort16      vlanId    @b((output)) VLAN ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlIntfDynamicEntryGetNext(L7_uint32 intIfNum, L7_enetMacAddr_t *macAddr, L7_ushort16 *vlanId)
{
  L7_uchar8          key[L7_FDB_KEY_SIZE] = {0};
  dot1dTpFdbData_t   fdbEntry;
  L7_RC_t            rc;
  L7_BOOL            found = L7_FALSE;
  pmlIntfCfgData_t  *pCfg;

  pmlSemaTake();
  if (pmlMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    vlanId = 0;
    memset(macAddr->addr, 0, L7_ENET_MAC_ADDR_LEN);

    pmlSemaGive();
    return L7_FAILURE;
  }
  pmlSemaGive();

  if ((pmlCfgData->globalLockEnabled == L7_TRUE) && (pCfg->intfLockEnabled == L7_TRUE))
  {
    memcpy(&key[0], vlanId, 2);
    memcpy(&key[2], macAddr->addr, 6);

    do
    {
      rc = fdbFind(key, L7_MATCH_GETNEXT, &fdbEntry);

      if (rc == L7_SUCCESS)
      {
        memcpy(&key[0], fdbEntry.dot1dTpFdbAddress, L7_FDB_KEY_SIZE);

        if ((fdbEntry.dot1dTpFdbPort == intIfNum) && (fdbEntry.dot1dTpFdbEntryType == L7_FDB_ADDR_FLAG_LEARNED))
        {
          memcpy(macAddr->addr, &fdbEntry.dot1dTpFdbAddress[2], 6);
          memcpy(vlanId, &fdbEntry.dot1dTpFdbAddress[0], sizeof(L7_ushort16));

          *vlanId = osapiNtohs(*vlanId); /* convert back to host byte order */

          found = L7_TRUE;
        }
      }
      else
      {
        break;
      }
    } while(found == L7_FALSE);
  }

  if (found == L7_FALSE)
  {
    vlanId = 0;
    memset(macAddr->addr, 0, L7_ENET_MAC_ADDR_LEN);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Converts dynamically locked entries to statically locked entries
*           on this interface.
*
* @param    L7_uint32        intIfNum  @b((input)) Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlIntfDynamicToStaticMove(L7_uint32 intIfNum)
{
  pmlIntfCfgData_t  *pCfg;
  pmlIntfOprData_t  *pOpr;
  L7_uint32          i;
  L7_uint32          staticCfgIndex;
  L7_enetMacAddr_t   macAddr;
  L7_ushort16        vlanId;
  L7_RC_t            rc;

  pmlSemaTake();
  if (pmlMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    pmlSemaGive();
    return L7_FAILURE;
  }

  if (pmlMapIntfIsOperational(intIfNum, &pOpr) != L7_TRUE)
  {
    pmlSemaGive();
    return L7_FAILURE;
  }

  memset(&macAddr.addr[0], 0, 6);
  vlanId = 0;

  /* take the first 'X' entries from the dynamic table and convert
     them to static. 'X' is the number of remaining static entries */
  for (i = pOpr->staticCount; i < pCfg->staticLimit; i++)
  {
    rc = pmlIntfDynamicEntryGetNext(intIfNum, &macAddr, &vlanId);
    if (rc != L7_SUCCESS)
    {
      break;
    }

    pOpr->dynamicCount--;
    pOpr->staticCount++;

    /* add the entry to the config structure */
    for (staticCfgIndex = 0; staticCfgIndex < L7_MACLOCKING_MAX_STATIC_ADDRESSES; staticCfgIndex++)
    {
      if (pCfg->staticMacEntry[staticCfgIndex].vlanId == 0)
      {
        pCfg->staticMacEntry[staticCfgIndex].vlanId = vlanId;
        pCfg->staticMacEntry[staticCfgIndex].macAddr = macAddr;
        break;
      }
    }

    if (staticCfgIndex == L7_MACLOCKING_MAX_STATIC_ADDRESSES)
    {
      /* shouldn't get here */
      L7_LOG_ERROR(0);
    }

    /* tell FDB component about this address */
    fdbInsert(pCfg->staticMacEntry[staticCfgIndex].macAddr.addr,
              intIfNum, 0 /* Not used */,
              pCfg->staticMacEntry[staticCfgIndex].vlanId,
              L7_FDB_ADDR_FLAG_STATIC);

    /* Only modify HW if intf is attached. */
    if (pmlMapIntfIsAttached(intIfNum))
    {
      /* add the entry to the HW as static */
      dtlFdbMacAddrAdd(pCfg->staticMacEntry[staticCfgIndex].macAddr.addr,
                       intIfNum,
                       pCfg->staticMacEntry[staticCfgIndex].vlanId,
                       L7_FDB_ADDR_FLAG_STATIC,
                       L7_NULL);
    }
  }

  pmlCfgData->cfgHdr.dataChanged = L7_TRUE;
  pmlSemaGive();
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets the last violation entry for an interface.
*
* @param    L7_uint32        intIfNum  @b((input)) Internal interface number
* @param    L7_enetMacAddr_t macAddr   @b((output)) MAC address
* @param    L7_ushort16      vlanId    @b((output)) VLAN ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlIntfLastViolationAddrGet(L7_uint32 intIfNum, L7_enetMacAddr_t *macAddr, L7_ushort16 *vlanId)
{
  pmlIntfOprData_t  *pOpr;

  pmlSemaTake();
  if (pmlMapIntfIsOperational(intIfNum, &pOpr) != L7_TRUE)
  {
    pmlSemaGive();
    return L7_FAILURE;
  }

  if (pOpr->lastViolationAddr.vlanId == 0)
  {
    pmlSemaGive();
    return L7_FAILURE;
  }

  *vlanId = pOpr->lastViolationAddr.vlanId;
  memcpy(&macAddr->addr, &pOpr->lastViolationAddr.macAddr.addr, L7_ENET_MAC_ADDR_LEN);

  pmlSemaGive();
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Callback from DTL informing about learned address
*
* @param    L7_uint32        intIfNum  @b((input)) Internal interface number
* @param    L7_enetMacAddr_t macAddr   @b((output)) MAC address
* @param    L7_ushort16      vlanId    @b((output)) VLAN ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlLearnEntryCallBack(L7_uint32 intIfNum, L7_enetMacAddr_t macAddr, L7_ushort16 vlanId)
{
  pmlIntfOprData_t  *pOpr;
  L7_uchar8          key[L7_FDB_KEY_SIZE];
  dot1dTpFdbData_t   fdbEntry;
  L7_uint32          tempIntIfNum;
  pmlIntfOprData_t  *pTempOpr;
  L7_RC_t            rc;

  if (vlanId == 0)
  {
    return L7_FAILURE;
  }

  /* search the FDB to determine if this MAC is being relearned */
  memcpy(&key[0], &vlanId, sizeof(L7_ushort16));
  memcpy(&key[2], &macAddr.addr, L7_ENET_MAC_ADDR_LEN);

  rc = fdbFind(key, L7_MATCH_EXACT, &fdbEntry);

  pmlSemaTake();

  if (rc == L7_SUCCESS)
  {
    tempIntIfNum = fdbEntry.dot1dTpFdbPort;

    /* if we already know about this address, just return */
    if (tempIntIfNum == intIfNum)
    {
      pmlSemaGive();
      return L7_SUCCESS;
    }

    /* otherwise we are learning this addr on a different port */
    if (pmlMapIntfIsOperational(tempIntIfNum, &pTempOpr) == L7_TRUE)
    {
      if (pTempOpr->dynamicCount > 0)
      {
        pTempOpr->dynamicCount--;
      }
    }
  }

  if (pmlMapIntfIsOperational(intIfNum, &pOpr) != L7_TRUE)
  {
    pmlSemaGive();
    return L7_FAILURE;
  }

  if(pOpr->dynamicCount < L7_MACLOCKING_MAX_DYNAMIC_ADDRESSES)
  {
    pOpr->dynamicCount++;
  }

  pmlSemaGive();
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Callback from DTL informing about aged address
*
* @param    L7_enetMacAddr_t macAddr   @b((output)) MAC address
* @param    L7_ushort16      vlanId    @b((output)) VLAN ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlAgeEntryCallBack(L7_enetMacAddr_t macAddr, L7_ushort16 vlanId)
{
  pmlIntfOprData_t  *pOpr;
  L7_uchar8          key[L7_FDB_KEY_SIZE];
  L7_RC_t            rc;
  dot1dTpFdbData_t   fdbEntry;
  L7_uint32          intIfNum;

  if (vlanId == 0)
  {
    return L7_FAILURE;
  }

  memcpy(&key[0], &vlanId, sizeof(L7_ushort16));
  memcpy(&key[2], &macAddr.addr, L7_ENET_MAC_ADDR_LEN);

  /* search the FDB to determine which interface this MAC was learned on */
  rc = fdbFind(key, L7_MATCH_EXACT, &fdbEntry);
  if (rc != L7_SUCCESS)
    return L7_FAILURE;

  intIfNum = fdbEntry.dot1dTpFdbPort;

  pmlSemaTake();
  if (pmlMapIntfIsOperational(intIfNum, &pOpr) != L7_TRUE)
  {
    pmlSemaGive();
    return L7_FAILURE;
  }
  
  if (pOpr->dynamicCount > 0)
  {
    pOpr->dynamicCount--;
  }

  pmlSemaGive();
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Callback from DTL informing about an unknown address
*
* @param    L7_uint32        intIfNum  @b((input)) Internal interface number
* @param    L7_enetMacAddr_t macAddr   @b((output)) MAC address
* @param    L7_ushort16      vlanId    @b((output)) VLAN ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlUnknownAddrCallBack(L7_uint32 intIfNum, L7_enetMacAddr_t macAddr, L7_ushort16 vlanId)
{
  pmlIntfCfgData_t  *pCfg;
  pmlIntfOprData_t  *pOpr;
  pmlLockInfoData_t *macEntry;
  pmlLockInfoData_t  newMacEntry;
  L7_uchar8          key[L7_FDB_KEY_SIZE];
  dot1dTpFdbData_t   fdbEntry;
  L7_uint32          tempIntIfNum;
  pmlIntfOprData_t  *pTempOpr;
  L7_RC_t            rc;
  L7_uint32          timestamp;

  if (vlanId == 0)
  {
    return L7_FAILURE;
  }

  pmlSemaTake();
  if (pmlMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    pmlSemaGive();
    return L7_FAILURE;
  }

  if (pmlMapIntfIsOperational(intIfNum, &pOpr) != L7_TRUE)
  {
    pmlSemaGive();
    return L7_FAILURE;
  }

  /* catch unwanted callbacks */
  if ((pCfg->intfLockEnabled == L7_FALSE) || (pmlCfgData->globalLockEnabled == L7_FALSE))
  {
    pmlSemaGive();
    return L7_FAILURE;
  }

  /* if we can learn the address, then do so, otherwise treat it as a violation */
  if ((pOpr->dynamicCount < pCfg->dynamicLimit)
#ifdef L7_DHCP_SNOOPING_PACKAGE
#ifdef L7_IPSG_PACKAGE
       &&
      ipsgClientAuthorized(&macAddr, vlanId, intIfNum)
#endif
#endif
)
  {
    /* search the FDB to determine if this MAC is being relearned */
    memcpy(&key[0], &vlanId, sizeof(L7_ushort16));
    memcpy(&key[2], &macAddr.addr, L7_ENET_MAC_ADDR_LEN);

    rc = fdbFind(key, L7_MATCH_EXACT, &fdbEntry);

    if (rc == L7_SUCCESS)
    {
      tempIntIfNum = fdbEntry.dot1dTpFdbPort;

      /* if we already know about this address, just return */
      if (tempIntIfNum == intIfNum)
      {
        pmlSemaGive();
        return L7_SUCCESS;
      }

      if (pmlMapIntfIsOperational(tempIntIfNum, &pTempOpr) == L7_TRUE)
      {
        if (pTempOpr->dynamicCount > 0)
        {
          pTempOpr->dynamicCount--;
        }
      }
    }

    pOpr->dynamicCount++;

    /* add the address to the HW */
    rc = dtlFdbMacAddrAdd(macAddr.addr, intIfNum, vlanId, L7_FDB_ADDR_FLAG_LEARNED, L7_NULL);

    if (rc == L7_SUCCESS)
    {
      /* tell the FDB component about this address */
      fdbLearnEntryCallBack(macAddr.addr,
                            intIfNum, 0 /* Not used */,
                            vlanId,
                            FDB_ADD);
    }
    else
    {
      /* since we couldn't add the entry to HW, clean up the count */
      pOpr->dynamicCount--;
    }

    pmlSemaGive();
  }
  else
  {
    /* Handle the case where we have many unknown address callbacks for the same address.
       In this case, the address may have already been accepted as a learned address. We
       need to ensure that further callbacks for this address are not treated as violations. */
    if (pOpr->dynamicCount >= pCfg->dynamicLimit)
    {
      memcpy(&key[0], &vlanId, sizeof(L7_ushort16));
      memcpy(&key[2], &macAddr.addr, L7_ENET_MAC_ADDR_LEN);

      rc = fdbFind(key, L7_MATCH_EXACT, &fdbEntry);

      if (rc == L7_SUCCESS)
      {
        pmlSemaGive();
        return L7_SUCCESS;
      }
    }

    pOpr->lastViolationAddr.macAddr = macAddr;
    pOpr->lastViolationAddr.vlanId  = vlanId;

    if (pCfg->violationTrapsEnabled == L7_FALSE)
    {
      pmlSemaGive();
      return L7_SUCCESS;
    }

    /* add the entry to the operational structure */
    memcpy(&newMacEntry.vlanIdMacAddr[0], &vlanId, sizeof(L7_ushort16));
    memcpy(&newMacEntry.vlanIdMacAddr[2], &macAddr.addr, L7_ENET_MAC_ADDR_LEN);

    macEntry = avlInsertEntry(&pOpr->violationAvlTree, (void *)&newMacEntry);
    pmlSemaGive();

    if (macEntry != NULL)
    {
      if (macEntry != &newMacEntry)
      {
        /* entry already exists, don't send another trap */
        return L7_FAILURE;
      }
    }

    /* send the trap to trapmgr */
    timestamp = osapiUpTimeRaw();
    if ((timestamp - pOpr->lastViolationTrap) > pCfg->violationTrapsSeconds)
    {
    trapMgrMacLockViolationLogTrap(intIfNum, macAddr, vlanId);
      pOpr->lastViolationTrap = timestamp;
      /* Customer trap */
      trapMgrLockPortLogTrap(intIfNum, macAddr);
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Callback from DTL indicating that the FDB sync is complete.
*
* @notes    none
*
* @end
*********************************************************************/
void pmlSyncCompleteCallback()
{
  pmlSyncCompleteSet(L7_TRUE);
}

/*********************************************************************
*
* @purpose  Returns the count of dynamcially learned Mac Addresses
*
* @param    L7_uint32        interface @b((input)) The interface for which count
*                            is to be determined
* @param    L7_uint32        learnedCount  @b((output)) Learned Dynamic addresses count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlGetDynamicLearnedAddrCount(L7_uint32 interface, L7_uint32 *learnedCount)
{
   pmlIntfOprData_t  *pOpr;

   pmlSemaTake();
   if(pmlMapIntfIsOperational(interface, &pOpr) != L7_TRUE)
   {
      pmlSemaGive();
      return L7_FAILURE;
   }

  *learnedCount=  pOpr->dynamicCount;
   pmlSemaGive();
   return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  Returns the count of static learned Mac Addresses
*
* @param    L7_uint32        interface @b((input)) The interface for which count
*                            is to be determined
* @param    L7_uint32        learnedCount  @b((output)) static addresses count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlGetStaticLearnedAddrCount(L7_uint32 interface, L7_uint32 *learnedCount)
{
   pmlIntfOprData_t  *pOpr;

   pmlSemaTake();
   if(pmlMapIntfIsOperational(interface, &pOpr) != L7_TRUE)
   {
      pmlSemaGive();
      return L7_FAILURE;
   }

  *learnedCount=  pOpr->staticCount;
   pmlSemaGive();
   return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Returns the count of static learned Mac Addresses
*
* @param    L7_uint32        vlanId @b((input))
* @param    L7_uint32        count  @b((output)) static addresses count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlGetStaticAddrCountByVlan(L7_uint32 vlanId,L7_uint32 *count)
{
  pmlIntfCfgData_t  *pCfg;
  L7_uint32 i, intIfNum;

  *count = 0;

  pmlSemaTake();
  for (intIfNum = 0; intIfNum < L7_MAX_INTERFACE_COUNT; intIfNum++)
  {
    if (pmlIsValidIntf(intIfNum))
    {
      if (pmlMapIntfIsConfigurable(intIfNum, &pCfg))
      {
        for (i = 0; i < L7_MACLOCKING_MAX_STATIC_ADDRESSES; i++)
        {
          if (pCfg->staticMacEntry[i].vlanId != 0)
          {
            if (pCfg->staticMacEntry[i].vlanId == vlanId)
            {
              (*count)++;
            }
          }
        }
      }
    }
  }
  pmlSemaGive();

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Returns the count of static learned Mac Addresses
*
* @param    L7_uint32        count  @b((output)) static addresses count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlGetStaticAddrCountGlobal(L7_uint32 *count)
{
  pmlIntfOprData_t  *pOpr;
  L7_uint32 intIfNum;

  *count = 0;

  pmlSemaTake();
  for (intIfNum = 0; intIfNum < L7_MAX_INTERFACE_COUNT; intIfNum++)
  {
    if (pmlIsValidIntf(intIfNum))
    {
      if (pmlMapIntfIsOperational(intIfNum, &pOpr))
      {
        (*count) += pOpr->staticCount;
      }
    }
  }
  pmlSemaGive();
   return L7_SUCCESS;
  return L7_SUCCESS;
}
