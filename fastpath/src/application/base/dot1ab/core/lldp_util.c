/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename lldp_util.c
*
* @purpose 802.1AB utility functions
*
* @component 802.1AB
*
* @comments none
*
* @create 02/01/2005
*
* @author dfowler
* @end
*
**********************************************************************/
/*********************************************************************
 *
 ********************************************************************/

#define L7_MAC_LLDP_PDU

#include <string.h>
#include "l7_common.h"
#include "log.h"
#include "nimapi.h"
#include "sysnet_api.h"
#include "buff_api.h"
#include "dtlapi.h"
#include "trapapi.h"
#include "lldp.h"
#include "lldp_util.h"
#include "lldp_api.h"
#include "lldp_tlv.h"
#include "lldp_debug.h"
#include "osapi_support.h"
#include "poe_api.h"
#include "lldp_med.h"
#include "trapapi.h"
#include "mirror_api.h"
#include "dot1x_api.h"

#ifdef L7_NSF_PACKAGE
#include "lldp_ckpt.h"
#endif /* L7_NSF_PACKAGE */

extern lldpCfgData_t       *lldpCfgData;
extern lldpCnfgrState_t     lldpCnfgrState;
extern L7_uint32           *lldpMapTbl;
extern void                *lldpQueue;
extern lldpIntfOprData_t   *lldpIntfTbl;
extern void                *lldpSemaphore;
extern lldpStats_t         *lldpStats;
extern L7_uchar8           *lldpPDUBuf;
extern L7_uint32            lldpMgmtAddrPoolId;
extern L7_uint32            lldpUnknownTLVPoolId;
extern L7_uint32            lldpOrgDefInfoPoolId;
extern L7_BOOL              lldpWarmRestart;
extern L7_BOOL              lldpActivateStartupDone;

PORTEVENT_MASK_t            lldpNimEventMask;

/* use one timer that wakes up every second, maintain a
   a reference count and delete the timer when we have no events */
static osapiTimerDescr_t *lldpTimer       = L7_NULLPTR;
static L7_uint32          lldpTimerCount  = 0;  /* reference count for lldpTimer */
static L7_uint32          lldpLastNotify  = 0;
L7_BOOL lldpMedDebugTxFlag = L7_FALSE;

L7_uchar8 *lldpCnfgrStateNames[LLDP_PHASE_UNCONFIG_2 + 1] =
{
  "P0", "P1", "P2", "WMU", "P3", "EXE", "U1", "U2"
};

extern const L7_uchar8 LLDP_MED_TIA_OUI_STRING[LLDP_TIA_OUI_STRING_LEN];
extern const L7_uchar8 LLDP_8023_TIA_OUI_STRING[LLDP_TIA_OUI_STRING_LEN];
extern const L7_uchar8 LLDP_8021_TIA_OUI_STRING[LLDP_TIA_OUI_STRING_LEN];
static L7_BOOL lldpMedTlvCapRx = L7_FALSE;
static L7_BOOL lldpMedTlvExtPwrRx = L7_FALSE;
static L7_BOOL lldpMedTlvHwRx = L7_FALSE;
static L7_BOOL lldpMedTlvFwRx = L7_FALSE;
static L7_BOOL lldpMedTlvSwRx = L7_FALSE;
static L7_BOOL lldpMedTlvSnoRx = L7_FALSE;
static L7_BOOL lldpMedTlvMfgRx = L7_FALSE;
static L7_BOOL lldpMedTlvMdlRx = L7_FALSE;
static L7_BOOL lldpMedTlvAssetRx = L7_FALSE;

static L7_BOOL lldpSkipDeleteRemEntry = L7_FALSE;

/*********************************************************************
*
* @purpose  To process the Callback for L7_CREATE
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
static L7_RC_t lldpIntfCreate(L7_uint32 intIfNum)
{
  nimConfigID_t      cfgId;
  lldpIntfCfgData_t *pCfg = L7_NULL;
  L7_uint32          i;
  L7_uint32          index = 0;

  if (lldpIsValidIntf(intIfNum) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  if (nimConfigIdGet(intIfNum, &cfgId) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  for (i = 1; i < L7_LLDP_INTF_MAX_COUNT; i++)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&lldpCfgData->intfCfgData[i].cfgId, &cfgId))
    {
      lldpMapTbl[intIfNum] = i;
      index = i;
      break;
    }
  }

  /* If an interface configuration entry is not already
     assigned to the interface, assign one */
  if (index == 0)
  {
    if(lldpMapIntfCfgEntryGet(intIfNum, &pCfg) != L7_TRUE)
    {
      return L7_FAILURE;
    }
    /* build the default configuration */
    lldpBuildDefaultIntfConfigData(&cfgId, pCfg);
  }

  /* Device natively supports the capabilities and network policy TLV
   * for the other TLVs we need to make specific checks. FastPath is
   * architected to run on network connectivity devices hence it can only be
   * be PSE PoE device hence make a check w/ poe if this interface can
   * support PSE function. Since this port does not need power from an
   * external source the extendedPD bit will not be set. Since currently
   * we do not have any Location application, that bit will also not be set.
   * Since we do not support the Entity MIB we will be setting the inventory
   * TLV bit either.
   * In the future if we do support any of the TLVs mentioned above we can
   * set their respective bits here.
   */
  /* Check with poe whether this port can support PSE function */
  if (lldpPoeIsValidPSEIntf(intIfNum) == L7_SUCCESS)
  {
    pCfg->medTLVsSupported.bitmap[1] |= LLDP_MED_CAP_EXT_PSE_BITMASK;
  }
  else if(lldpPoeIsValidPDIntf(intIfNum) == L7_SUCCESS)
  {
    pCfg->medTLVsSupported.bitmap[1] |= LLDP_MED_CAP_EXT_PD_BITMASK;
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To process the Callback for L7_DELETE
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
static L7_RC_t lldpIntfDelete(L7_uint32 intIfNum)
{
  L7_uint32 index = 0;

  /* delete the configuration entry and all related tables for this interface */
  if (lldpMapIntfIndexGet(intIfNum, &index) == L7_TRUE)
  {
    lldpRemDataInterfaceDelete(intIfNum,L7_FALSE);
    memset((void *)&lldpCfgData->intfCfgData[index], 0, sizeof(lldpIntfCfgData_t));
    memset((void *)&lldpMapTbl[intIfNum], 0, sizeof(L7_uint32));
    memset((void *)&lldpStats->intfStats[index], 0, sizeof(lldpIntfStats_t));
    memset((void *)&lldpIntfTbl[index], 0, sizeof(lldpIntfOprData_t));
    lldpCfgData->cfgHdr.dataChanged = L7_TRUE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To activate the RX path
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
static L7_RC_t lldpIntfRxActive(L7_uint32 intIfNum)
{
  L7_uint32 index = 0;

  /* get the configuration entry for this interface */
  if (lldpMapIntfIndexGet(intIfNum, &index) == L7_TRUE)
  {
    if (lldpIntfTbl[index].rxActive != L7_TRUE)
    {
      lldpIntfTbl[index].rxActive = L7_TRUE;
      return lldpIntfConfigDataApply(index);
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To activate the TX path
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
static L7_RC_t lldpIntfTxActive(L7_uint32 intIfNum)
{
  L7_uint32 index = 0;

  /* get the configuration entry for this interface */
  if (lldpMapIntfIndexGet(intIfNum, &index) == L7_TRUE)
  {
    if (lldpIntfTbl[index].txActive != L7_TRUE)
    {
      lldpIntfTbl[index].txActive = L7_TRUE;
      return lldpIntfConfigDataApply(index);
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To process the Callback for L7_ACTIVE (WarmRestart)
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
static L7_RC_t lldpWarmRestartIntfActive(L7_uint32 intIfNum)
{
  L7_uint32 index = 0;

  /* get the configuration entry for this interface */
  if (lldpMapIntfIndexGet(intIfNum, &index) == L7_TRUE)
  {
    if(lldpIntfTbl[index].activeWarmRestart != L7_TRUE)
    {
      lldpIntfTbl[index].activeWarmRestart = L7_TRUE;
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To deactivate the RX path
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
static L7_RC_t lldpIntfRxInactive(L7_uint32 intIfNum)
{
  L7_uint32 index = 0;

  /* get the configuration entry for this interface */
  if (lldpMapIntfIndexGet(intIfNum, &index) == L7_TRUE)
  {
    if (lldpIntfTbl[index].rxActive != L7_FALSE)
    {
      lldpIntfTbl[index].rxActive = L7_FALSE;
      return lldpIntfConfigDataApply(index);
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To deactivate the TX path
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
static L7_RC_t lldpIntfTxInactive(L7_uint32 intIfNum)
{
  L7_uint32 index = 0;

  /* get the configuration entry for this interface */
  if (lldpMapIntfIndexGet(intIfNum, &index) == L7_TRUE)
  {
    if (lldpIntfTbl[index].txActive != L7_FALSE)
    {
      lldpIntfTbl[index].txActive = L7_FALSE;
      return lldpIntfConfigDataApply(index);
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Maintain one timer for the component
*
* @returns  L7_SUCCESS,
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
void lldpTimerAdd(void)
{
  if (lldpTimerCount == 0)
  {
    osapiTimerAdd((void *)lldpTimerCallback, L7_NULL, L7_NULL,
                  LLDP_TIMER_INTERVAL, &lldpTimer);
  }
  lldpTimerCount++;

  return;
}

/*********************************************************************
*
* @purpose  Maintain one timer for the component
*
* @returns  L7_SUCCESS,
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
void lldpTimerDelete(void)
{
  lldpTimerCount--;
  if (lldpTimerCount == 0)
  {
    osapiTimerFree(lldpTimer);
    lldpTimer = L7_NULLPTR;
  }

  return;
}

/*********************************************************************
* @purpose  Allocate a management address entry from the buffer pool
*
* @param    lldpMgmtAddrEntry_t **list  @b((input)) current interface list
*
* @returns  lldpMgmtAddrEntry_t *
* @returns  L7_NULLPTR, no buffer available
*
* @notes    none
*
* @end
*********************************************************************/
static lldpMgmtAddrEntry_t *lldpMgmtAddrBufferGet(lldpMgmtAddrEntry_t **list)
{
  L7_uchar8 *buffer = L7_NULLPTR;

  if ((bufferPoolAllocate(lldpMgmtAddrPoolId, &buffer) != L7_SUCCESS) ||
      (buffer == L7_NULLPTR))
  {
    return L7_NULLPTR;
  }

  memset((void *)buffer, 0, sizeof(lldpMgmtAddrEntry_t));

  if (*list == L7_NULLPTR)
  {
    *list = (lldpMgmtAddrEntry_t *)buffer;
  } else
  {
    lldpMgmtAddrEntry_t *p = L7_NULLPTR;
    for (p = *list; p->next; p = p->next);
    p->next = (lldpMgmtAddrEntry_t *)buffer;
  }

  return (lldpMgmtAddrEntry_t *)buffer;
}

/*********************************************************************
* @purpose  Allocate an unknown TLV entry from the buffer pool
*
* @param    lldpUnknownTLVEntry_t **list  @b((input)) current interface list
*
* @returns  lldpUnknownTLVEntry_t *
* @returns  L7_NULLPTR, no buffer available
*
* @notes    none
*
* @end
*********************************************************************/
static lldpUnknownTLVEntry_t *lldpUnknownTLVBufferGet(lldpUnknownTLVEntry_t **list)
{
  L7_uchar8 *buffer = L7_NULLPTR;

  if ((bufferPoolAllocate(lldpUnknownTLVPoolId, &buffer) != L7_SUCCESS) ||
      (buffer == L7_NULLPTR))
  {
    return L7_NULLPTR;
  }

  memset((void *)buffer, 0, sizeof(lldpUnknownTLVEntry_t));

  if (*list == L7_NULLPTR)
  {
    *list = (lldpUnknownTLVEntry_t *)buffer;
  }
  else
  {
    lldpUnknownTLVEntry_t *p = L7_NULLPTR;
    for (p = *list; p->next; p = p->next);
    p->next = (lldpUnknownTLVEntry_t *)buffer;
  }

  return (lldpUnknownTLVEntry_t *)buffer;
}

/*********************************************************************
* @purpose  Allocate an organization defined entry from the buffer pool
*
* @param    lldpOrgDefInfoEntry_t **list  @b((input)) current interface list
*
* @returns  lldpOrgDefInfoEntry_t *
* @returns  L7_NULLPTR, no buffer available
*
* @notes    none
*
* @end
*********************************************************************/
static lldpOrgDefInfoEntry_t *lldpOrgDefInfoBufferGet(lldpOrgDefInfoEntry_t **list)
{
  L7_uchar8 *buffer = L7_NULLPTR;

  if ((bufferPoolAllocate(lldpOrgDefInfoPoolId, &buffer) != L7_SUCCESS) ||
      (buffer == L7_NULLPTR))
  {
    return L7_NULLPTR;
  }

  memset((void *)buffer, 0, sizeof(lldpOrgDefInfoEntry_t));

  if (*list == L7_NULLPTR)
  {
    *list = (lldpOrgDefInfoEntry_t *)buffer;
  }
  else
  {
    lldpOrgDefInfoEntry_t *p = L7_NULLPTR;
    for (p = *list; p->next; p = p->next);
    p->next = (lldpOrgDefInfoEntry_t *)buffer;
  }

  return (lldpOrgDefInfoEntry_t *)buffer;
}

/*********************************************************************
* @purpose  Checks to see if a network address is zero
*
* @param    lldpRemDataEntry_t current  @b((input)) current remote data
* @param    lldpRemDataEntry_t update   @b((input)) new LLDPDU data
*
* @returns  L7_SUCCESS, network address is zero
* @returns  L7_FAILURE, network address is not zero
* @returns  L7_ERROR,   chassis id is not a network address
*
* @notes    assumes chassisIdSubType as LLDP_CHASSIS_ID_SUBTYPE_NET_ADDR
*
* @end
*********************************************************************/
static L7_BOOL lldpChassisIdNetAddrIsZero(lldpRemDataEntry_t *current)
{
  L7_uint32 i;


  for (i = LLDP_TLV_CHASSIS_ID_NTWK_ADDR_FAM_SIZE; i < current->chassisIdLength; i++)
  {
    if (current->chassisId[i] != 0)
    {
      return L7_FALSE;
    }
  }

  return L7_TRUE;
}

/*********************************************************************
* @purpose  Compare the chassis ids of two remote data entries
*
* @param    lldpRemDataEntry_t current  @b((input)) current remote data
* @param    lldpRemDataEntry_t update   @b((input)) new LLDPDU data
*
* @returns  L7_SUCCESS, entries match
* @returns  L7_FAILURE, entries do not match, update remote data
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t lldpChassisIdCompare(lldpRemDataEntry_t *current,
                                    lldpRemDataEntry_t *update)
{
  /*
   * If the chassis ID subtype is Network Address and the stored value is all zeros, the
   * stored chassis ID should be updated.  Otherwise, ensure that the stored chassis ID
   * matches the current chassis ID.
   */
  if ((LLDP_CHASSIS_ID_SUBTYPE_NET_ADDR == current->chassisIdSubtype) &&
      (L7_TRUE == lldpChassisIdNetAddrIsZero(current)))
  {
    memcpy(current->chassisId, update->chassisId, update->chassisIdLength);
    current->chassisIdLength = update->chassisIdLength;
    return L7_SUCCESS;
  }

  if ((current->chassisIdLength != update->chassisIdLength) ||
      (0 != memcmp(current->chassisId, update->chassisId, update->chassisIdLength)))
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Compare two remote data entries to determine if there is an update
*
* @param    lldpRemDataEntry_t current  @b((input)) current remote data
* @param    lldpRemDataEntry_t update   @b((input)) new LLDPDU data
*
* @returns  L7_SUCCESS, entries match
* @returns  L7_FAILURE, entries do not match, update remote data
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t lldpRemEntryCompare(lldpRemDataEntry_t *current,
                                   lldpRemDataEntry_t *update)
{
  void *entry1 = L7_NULLPTR;
  void *entry2 = L7_NULLPTR;
  lldpMgmtAddrEntry_t *addr1 = L7_NULLPTR;
  lldpMgmtAddrEntry_t *addr2 = L7_NULLPTR;

  /* compare everything but the timestamps and list pointers */
  if ((current->portIdSubtype != update->portIdSubtype) ||
      (current->portIdLength != update->portIdLength) ||
      (memcmp(current->portId, update->portId, update->portIdLength) != 0) ||
      (strcmp(current->portDesc, update->portDesc) != 0) ||
      (strcmp(current->sysName, update->sysName) != 0) ||
      (strcmp(current->sysDesc, update->sysDesc) != 0) ||
      (current->sysCapEnabled != update->sysCapEnabled) ||
      (current->sysCapSupported != update->sysCapSupported) ||
      (current->chassisIdSubtype != update->chassisIdSubtype) ||
      (L7_SUCCESS != lldpChassisIdCompare(current, update)))
  {
    return L7_FAILURE;
  }

  /* walk through each of the buffer lists and compare contents */
  addr1 = current->mgmtAddrList;
  addr2 = update->mgmtAddrList;
  /*
   * If no management address is present in the current entry and at least one address is
   * present in the updated entry, then add all the addresses.
   */
  if ((L7_NULLPTR == addr1) &&
      (L7_NULLPTR != addr2))
  {
    do
    {
      /* need a mgmt address buffer */
      addr1 = lldpMgmtAddrBufferGet(&current->mgmtAddrList);

      if (L7_NULLPTR == addr1)
      {
        break;
      }

      memcpy(current->mgmtAddrList, addr2, sizeof(*addr2));
      addr1->next = L7_NULLPTR;         /* New mgmt addr does not (yet) have a next. */
      addr2 = addr2->next;
    } while (L7_NULLPTR != addr2);
  }
  else                                  /* Compare the mgmt address lists */
  {
    while ((addr1 != L7_NULLPTR) && (addr2 != L7_NULLPTR))
    {
      /* Do not compare the pointer to the next entry */
      if (memcmp(addr1, addr2, (sizeof(*addr1) - sizeof(addr1->next))) != 0)
      {
        return L7_FAILURE;
      }
      addr1 = addr1->next;
      addr2 = addr2->next;
    }
    if (addr1 != addr2)
    {
      return L7_FAILURE;
    }
  }

  entry1 = (void *)current->unknownTLVList;
  entry2 = (void *)update->unknownTLVList;
  while ((entry1 != L7_NULLPTR) && (entry2 != L7_NULLPTR))
  {
    /* Do not compare the pointer to the next entry */
    if (memcmp(entry1, entry2, (sizeof(lldpUnknownTLVEntry_t)-sizeof(L7_uint32))) != 0)
    {
      return L7_FAILURE;
    }
    entry1 = (void *)((lldpUnknownTLVEntry_t *)entry1)->next;
    entry2 = (void *)((lldpUnknownTLVEntry_t *)entry2)->next;
  }
  if (entry1 != entry2)
  {
    return L7_FAILURE;
  }

  entry1 = (void *)current->orgDefInfoList;
  entry2 = (void *)update->orgDefInfoList;
  while ((entry1 != L7_NULLPTR) && (entry2 != L7_NULLPTR))
  {
    /* Do not compare the pointer to the next entry */
    if (memcmp(entry1, entry2, (sizeof(lldpOrgDefInfoEntry_t)-sizeof(L7_uint32))) != 0)
    {
      return L7_FAILURE;
    }
    entry1 = (void *)((lldpOrgDefInfoEntry_t *)entry1)->next;
    entry2 = (void *)((lldpOrgDefInfoEntry_t *)entry2)->next;
  }
  if (entry1 != entry2)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Create LLDPDU for given interface
*
* @param    L7_uint32       index     @b((input)) component index
* @param    L7_uint32       intIfNum  @b((input)) internal interface number
* @param    lldpPDUHandle_t pduHdl    @b((input)) pointer to LLDP PDU type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
static L7_RC_t lldpLocalDataPduWrite(L7_uint32 index,
                                     L7_uint32 intIfNum,
                                     lldpPDUHandle_t *pduHdl)
{
  lldpChassisIdSubtype_t chassisIdSubtype = 0;
  lldpPortIdSubtype_t    portIdSubtype = 0;
  L7_uchar8              buffer[LLDP_MGMT_STRING_SIZE_MAX];
  L7_ushort16            length = 0;
  L7_uint32              ttl = 0;

  if (lldpPDUStart(pduHdl, intIfNum) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Chassis ID, Port ID, TTL are mandatory in that order */
  if (lldpLocChassisIdSubtypeGet(&chassisIdSubtype) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  memset(buffer, 0, sizeof(buffer));
  if (lldpLocChassisIdGet(buffer, &length) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if (lldpTLVChassisIdWrite(chassisIdSubtype, buffer, length, pduHdl) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (lldpLocPortIdSubtypeGet(intIfNum, &portIdSubtype) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  memset(buffer, 0, sizeof(buffer));
  if (lldpLocPortIdGet(intIfNum, buffer, &length) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if (lldpTLVPortIdWrite(portIdSubtype, buffer, length, pduHdl) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  ttl = lldpCfgData->txInterval * lldpCfgData->txHoldMultiplier;
  if (lldpTLVTTLWrite(ttl, pduHdl) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* test for all the optional TLVs */
  if (lldpCfgData->intfCfgData[index].optionalTLVsEnabled & LLDP_TX_TLV_PORT_DESC_BIT)
  {
    memset(buffer, 0, sizeof(buffer));
    if (lldpLocPortDescGet(intIfNum, (L7_char8 *)buffer) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    if (lldpTLVPortDescWrite((L7_char8 *)buffer, pduHdl) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }

  if (lldpCfgData->intfCfgData[index].optionalTLVsEnabled & LLDP_TX_TLV_SYS_NAME_BIT)
  {
    memset(buffer, 0, sizeof(buffer));
    if (lldpLocSysNameGet((L7_char8 *)buffer) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    if (lldpTLVSysNameWrite((L7_char8 *)buffer, pduHdl) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }

  if (lldpCfgData->intfCfgData[index].optionalTLVsEnabled & LLDP_TX_TLV_SYS_DESC_BIT)
  {
    memset(buffer, 0, sizeof(buffer));
    if (lldpLocSysDescGet((L7_char8 *)buffer) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    if (lldpTLVSysDescWrite((L7_char8 *)buffer, pduHdl) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }

  if (lldpCfgData->intfCfgData[index].optionalTLVsEnabled & LLDP_TX_TLV_SYS_CAP_BIT)
  {
    L7_ushort16 supported = 0;
    L7_ushort16 enabled = 0;

    if (lldpLocSysCapSupportedGet(&supported) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    if (lldpLocSysCapEnabledGet(&enabled) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    if (lldpTLVSysCapWrite(supported, enabled, pduHdl) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }

  if (lldpCfgData->intfCfgData[index].mgmtAddrTxEnabled == L7_TRUE)
  {
    L7_uchar8  addr[LLDP_MGMT_ADDR_SIZE_MAX];
    L7_char8   oid[LLDP_MGMT_ADDR_OID_SIZE_MAX];
    L7_uchar8  length = 0;
    L7_uint32  ifId = 0;
    lldpManAddrIfSubtype_t ifSubtype = 0;
    lldpIANAAddrFamilyNumber_t family = 0;

    if (lldpLocManAddrGet(&family, addr, &length) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    if (lldpLocManAddrIfSubtypeGet(&ifSubtype) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    if (lldpLocManAddrIfIdGet(&ifId) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    if (lldpLocManAddrOIDGet(oid) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    if (lldpTLVMgmtAddrWrite(family, addr, length,
                             ifSubtype, ifId, oid, pduHdl) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }

#ifdef L7_POE_AT_PACKAGE
  if(poeIsValidIntf(intIfNum) == L7_SUCCESS)
  {
   if(lldp8023PowerMDITLVWrite(index, intIfNum, pduHdl) != L7_SUCCESS)
   {
     return L7_FAILURE;
   }
  }
#endif

  /* We will transmit LLDP-MED TLVs only if we received them in the first place */
  if ((lldpIntfTbl[index].medTransmitEnabled > 0) ||
      (lldpIntfTbl[index].pdEndpointDevice   == L7_TRUE))
  {
    lldpMedTLVsWrite(index,intIfNum,pduHdl);
  }

  if (lldpPDUEnd(pduHdl) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Create shutdown LLDPDU for given interface
*
* @param    L7_uint32       index     @b((input)) component index
* @param    L7_uint32       intIfNum  @b((input)) internal interface number
* @param    lldpPDUHandle_t pduHdl    @b((input)) pointer to LLDP PDU type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
static L7_RC_t lldpShutdownPduWrite(L7_uint32 index,
                                    L7_uint32 intIfNum,
                                    lldpPDUHandle_t *pduHdl)
{
  lldpChassisIdSubtype_t chassisIdSubtype = 0;
  lldpPortIdSubtype_t    portIdSubtype = 0;
  L7_uchar8              buffer[LLDP_MGMT_STRING_SIZE_MAX];
  L7_ushort16            length = 0;

  if (lldpPDUStart(pduHdl, intIfNum) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Chassis ID, Port ID, TTL are mandatory in that order */
  if (lldpLocChassisIdSubtypeGet(&chassisIdSubtype) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  memset(buffer, 0, sizeof(buffer));
  if (lldpLocChassisIdGet(buffer, &length) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if (lldpTLVChassisIdWrite(chassisIdSubtype, buffer, length, pduHdl) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (lldpLocPortIdSubtypeGet(intIfNum, &portIdSubtype) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  memset(buffer, 0, sizeof(buffer));
  if (lldpLocPortIdGet(intIfNum, buffer, &length) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if (lldpTLVPortIdWrite(portIdSubtype, buffer, length, pduHdl) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* shutdown includes a TTL of 0, indicate to remote device to delete data */
  if (lldpTLVTTLWrite(0, pduHdl) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (lldpPDUEnd(pduHdl) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Record a remote table change and handle notifications.
*
* @param    L7_uint32  index  @b((input))  component internal index
*
* @returns  L7_SUCCESS,
*           L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t lldpRemLastChangeTimeUpdate(L7_uint32 index)
{
  lldpStats->remTblLastChangeTime = osapiUpTimeRaw();
  /* If notifications are enabled on this port and we have not sent a
     notify within the configured interval, send change notification */
  if ((lldpCfgData->intfCfgData[index].notificationEnabled == L7_TRUE) &&
      ((lldpStats->remTblLastChangeTime - lldpLastNotify) > lldpCfgData->notifyInterval))
  {
    lldpLastNotify = lldpStats->remTblLastChangeTime;
    if (trapMgrLldpRemTablesChangeTrap(lldpStats->remTblInserts,
                                       lldpStats->remTblDeletes,
                                       lldpStats->remTblDrops,
                                       lldpStats->remTblAgeouts) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_LLDP_COMPONENT_ID, "lldpRemLastChangeTimeUpdate failed to send change notification.");
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Parse a received LLDPDU
*
* @param    L7_uint32  index  @b((input))  component internal index
* @param    L7_uchar8  *pdu   @b((input))  pointer to the lldpdu received
* @param    L7_uint32  length @b((input))  length of lldpdu
*
* @returns  L7_SUCCESS, LLDPDU parsed and stored successfully
* @returns  L7_FAILURE, error parsing LLDPDU
* @returns  L7_REQUEST_DENIED, client is not authorized
* @returns  L7_TABLE_IS_FULL, out of buffers
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t lldpRemoteDataPduRead(L7_uint32  index,
                                     L7_uchar8 *pdu,
                                     L7_uint32  pduLength,
                                     L7_uchar8  *srcMac)
{
  lldpRemDataEntry_t entry,*currEntry;
  lldpRemDataKey_t  entryKey;
  lldpPDUHandle_t    pduHdl;
  lldpTLVType_t      type = LLDP_TLV_TYPE_END_OF_LLDPDU;
  L7_ushort16        length = 0;
  L7_uint32          ttl = 0;
  L7_BOOL      sendMedTrap = L7_FALSE;
  lldpMedCapTLV_t    medinfo;
  L7_uint32      tlvDiscard = 0;
  L7_uint32      tlvMed = 0;
  L7_uint32      tlv8021 = 0;
  L7_uint32      tlv8023 = 0;
  L7_uint32      tlvUnrecog = 0;
  L7_BOOL        voiceDevice=L7_FALSE;
  L7_uchar8      deviceType;
  L7_BOOL        poeDevice=L7_FALSE;
  L7_RC_t rc = L7_ERROR;
  L7_BOOL   medTransmitEnabled = L7_FALSE;
  L7_uint32 medFastStart = 0;

  /* Set the flag to false for every received pdu */
  lldpMedTlvCapRx = L7_FALSE;
  lldpMedTlvExtPwrRx = L7_FALSE;
  lldpMedTlvHwRx = L7_FALSE;
  lldpMedTlvFwRx = L7_FALSE;
  lldpMedTlvSwRx = L7_FALSE;
  lldpMedTlvSnoRx = L7_FALSE;
  lldpMedTlvMfgRx = L7_FALSE;
  lldpMedTlvMdlRx = L7_FALSE;
  lldpMedTlvAssetRx = L7_FALSE;

  memset(&entry, 0, sizeof(lldpRemDataEntry_t));
  entry.intIntfNum = lldpIntfTbl[index].intIfNum;

  pduHdl.buffer = pdu;
  pduHdl.offset = 0;

  /* Chassis ID, Port ID, TTL are mandatory in that order */
  if ((lldpTLVHeaderRead(&type, &length, &pduHdl, pduLength) != L7_SUCCESS) ||
      (type != LLDP_TLV_TYPE_CHASSIS_ID) ||
      (lldpTLVChassisIdRead(&entry.chassisIdSubtype, entry.chassisId, &entry.chassisIdLength,
                            &pduHdl, length) != L7_SUCCESS))
  {
    LLDP_TRACE(LLDP_DEBUG_PROTO_VERBOSE, "Failure reading mandatory TLVs on intf %d", lldpIntfTbl[index].intIfNum);
    return L7_FAILURE;
  }

  if ((lldpTLVHeaderRead(&type, &length, &pduHdl, pduLength) != L7_SUCCESS) ||
      (type != LLDP_TLV_TYPE_PORT_ID) ||
      (lldpTLVPortIdRead(&entry.portIdSubtype, entry.portId, &entry.portIdLength,
                         &pduHdl, length) != L7_SUCCESS))
  {
    LLDP_TRACE(LLDP_DEBUG_PROTO_VERBOSE, "Failure reading port ID TLV on intf %d", lldpIntfTbl[index].intIfNum);
    return L7_FAILURE;
  }

  if ((lldpTLVHeaderRead(&type, &length, &pduHdl, pduLength) != L7_SUCCESS) ||
      (type != LLDP_TLV_TYPE_TIME_TO_LIVE) ||
      (lldpTLVTTLRead(&ttl, &pduHdl, length) != L7_SUCCESS))
  {
    LLDP_TRACE(LLDP_DEBUG_PROTO_VERBOSE, "Failure reading TTL TLV on intf %d", lldpIntfTbl[index].intIfNum);
    return L7_FAILURE;
  }
  memcpy(&entryKey,&entry,sizeof(lldpRemDataKey_t));

  if (ttl == 0)
  {
    /* find the entry */
    currEntry = lldpRemDataEntryGet(&entryKey);
    /* TTL of zero indicates we should delete a matching remote entry */
    if (currEntry != L7_NULLPTR)
    {
      lldpMedGetRemEntryVoiceInfo(currEntry,&voiceDevice,&deviceType);
      /* delete the matching entry */
      if (currEntry->rxTTL != 0)
      {
        currEntry->rxTTL = 0;
        /*lldpTimerDelete();*/
      }
      LLDP_TRACE(LLDP_DEBUG_MED, "Received TTL 0, intf %d, check for device removal", currEntry->intIntfNum);
      lldpMedRemDeviceRemoveCheck(currEntry);
      if (L7_TRUE == currEntry->medDevice)
      {
        /* Set the MED operational  parameters to default */
        if (lldpIntfTbl[index].medTransmitEnabled > 0)
        {
          lldpIntfTbl[index].medTransmitEnabled--;
        }
        /* if no MED devices after above decrement */
        if (lldpIntfTbl[index].medTransmitEnabled == 0)
        {
          lldpIntfTbl[index].medFastStart = 0;
        }
      }
      lldpRemEntryDelete(currEntry);

      lldpStats->remTblDeletes++;
      lldpRemLastChangeTimeUpdate(index);
    }
    /* we never want to store a record with TTL of zero, so return here
       regardless of whether we found a matching entry */
    lldpStats->intfStats[index].rxFramesDiscarded++;
    LLDP_TRACE(LLDP_DEBUG_PROTO_VERBOSE, "Discarding PDU because TTL == 0 on intf %d", lldpIntfTbl[index].intIfNum);
    return L7_SUCCESS;
  }

  /* read any optional TLVs until END_OF_LLDPDU or end of buffer */
  while ((lldpTLVHeaderRead(&type, &length, &pduHdl, pduLength) == L7_SUCCESS) &&
         (type != LLDP_TLV_TYPE_END_OF_LLDPDU))
  {
    switch (type)
    {
      case LLDP_TLV_TYPE_PORT_DESC:
        rc = lldpTLVPortDescRead(entry.portDesc, &pduHdl, length);
        break;
      case LLDP_TLV_TYPE_SYS_NAME:
        rc = lldpTLVSysNameRead(entry.sysName, &pduHdl, length);
        break;
      case LLDP_TLV_TYPE_SYS_DESC:
        rc = lldpTLVSysDescRead(entry.sysDesc, &pduHdl, length);
        break;
      case LLDP_TLV_TYPE_SYS_CAP:
        rc = lldpTLVSysCapRead(&entry.sysCapSupported, &entry.sysCapEnabled,
                               &pduHdl, length);
        break;
      case LLDP_TLV_TYPE_MGMT_ADDR:
        {
          /* need a mgmt address buffer */
          lldpMgmtAddrEntry_t *addr = lldpMgmtAddrBufferGet(&entry.mgmtAddrList);
          if (addr == L7_NULLPTR)
          {
            rc = L7_TABLE_IS_FULL;
            break;
          }
          rc = lldpTLVMgmtAddrRead(addr, &pduHdl, length);
          break;
        }
      case LLDP_TLV_TYPE_ORG_DEF:
        {
          lldpOrgDefInfoEntry_t *info;
          /* If this TLV is MED then we need to validate it
           * Since we may need to discard this TLV do the validation before
           * getting a buffer. i.e. get the buffer only if this tlv
           * passes the validation.
           */
          if (memcmp(&pduHdl.buffer[pduHdl.offset], LLDP_MED_TIA_OUI_STRING,
                              LLDP_TLV_ORG_DEF_INFO_OUI_SIZE_MAX) == 0)
          {
              /*this is a MED TLV so return SUCCESS */
          rc = lldpMedTLVValidate(&entry, index, &pduHdl, length);
          if (rc != L7_SUCCESS)
          {
            /* This TLV has to be discarded, but we need to continue
             * reading the next TLV so set rc = SUCCESS and then break
             */
            tlvDiscard++;
            rc = L7_SUCCESS;
            break;
          }
          }
          /* need an org def info buffer */
          info = lldpOrgDefInfoBufferGet(&entry.orgDefInfoList);
          if (info == L7_NULLPTR)
          {
            /* log buffer message? */
            rc = L7_TABLE_IS_FULL;
            break;
          }
          rc = lldpTLVOrgDefInfoRead(info, &pduHdl, length);
          if (rc == L7_SUCCESS)
          {
            if (memcmp(info->oui,LLDP_MED_TIA_OUI_STRING, LLDP_TIA_OUI_STRING_LEN)==0)
            {
              if (lldpIntfTbl[index].medTransmitEnabled == 0)
              {
                /* Send out a trap if the toplogy change trap flag is enabled */
                sendMedTrap = L7_TRUE;
                medFastStart = lldpCfgData->fastStartRepeatCount;
              }
              else
              {
                medFastStart = lldpIntfTbl[index].medFastStart;
              }
              entry.medDevice    = L7_TRUE;
              medTransmitEnabled = L7_TRUE;
              tlvMed++;
            }
            else if (memcmp(info->oui,LLDP_8023_TIA_OUI_STRING, LLDP_TIA_OUI_STRING_LEN)==0)
            {
              tlv8023++;
            }
            else if (memcmp(info->oui,LLDP_8021_TIA_OUI_STRING, LLDP_TIA_OUI_STRING_LEN)==0)
            {
              tlv8021++;
            }
            else
              tlvUnrecog++;
          }
          break;
        }
      default:
        if (type >= LLDP_TLV_TYPE_RESERVED_BEGIN &&
            type <= LLDP_TLV_TYPE_RESERVED_END)
        {
          /* need an unrecognized TLV buffer */
          lldpUnknownTLVEntry_t *tlv = lldpUnknownTLVBufferGet(&entry.unknownTLVList);
          if (tlv == L7_NULLPTR)
          {
            /* log buffer message? */
            rc = L7_TABLE_IS_FULL;
            break;
          }
          tlv->type = type;  /* save type for unknown tlv */
          rc = lldpTLVUnknownRead(tlv, &pduHdl, length);
          if (rc == L7_SUCCESS)
          {
            tlvUnrecog++;
          }
        }
        else
        {
          rc = L7_FAILURE;
        }
        break;
    }
    if (rc != L7_SUCCESS)
    {
      lldpRemEntryDelete(&entry); /* make sure we return any allocated buffers */
      LLDP_TRACE(LLDP_DEBUG_PROTO_VERBOSE, "Failure reading optional TLVs on intf %d", lldpIntfTbl[index].intIfNum);
      return rc;
    }
  }

  /* find the remote data node */
  currEntry = lldpRemDataEntryGet(&entryKey);
  if (currEntry == L7_NULLPTR)
  {
    currEntry = lldpRemDataEntryMacCheck(entry.intIntfNum, srcMac);
  }

  if (currEntry != L7_NULLPTR)
  {
    if (lldpRemEntryCompare(currEntry, &entry) == L7_SUCCESS)
    {
      rc = lldpMedGetRemEntryVoiceInfo(currEntry,&voiceDevice,&deviceType);
      if(rc == L7_SUCCESS)
      {
        LLDP_TRACE(LLDP_DEBUG_MED, "Compare successful, intf %d, process voice device PDU", currEntry->intIntfNum);
        lldpMedProcessVoiceDevicePdu(currEntry,voiceDevice,deviceType);
      }
      currEntry->rxTTL = ttl;
      /* Even though Nothing is Changed Send it to State Machine,Useful in case of NACK */
      rc = lldpMedGetRemEntryPoeInfo(&entry,&poeDevice,&deviceType);
      if(rc == L7_SUCCESS)
      {
        lldpMedProcessPoeDevicePdu(&entry,poeDevice,L7_FALSE);
      }
      lldpRemEntryDelete(&entry);
      LLDP_TRACE(LLDP_DEBUG_PROTO_VERBOSE, "Compare successful, updating TTL on intf %d", lldpIntfTbl[index].intIfNum);
      return L7_SUCCESS;
    }
    else
    {
      LLDP_TRACE(LLDP_DEBUG_MED, "Compare unsuccessful, intf %d, delete old remote entry and add new", currEntry->intIntfNum);
    }
    /* delete current remote entry, buffers, and timer */
    if (currEntry->rxTTL != 0)
    {
      currEntry->rxTTL = 0;
     /* lldpTimerDelete(); */
    }

    /* Set the MED operational parameters to what was determined when we read this PDU for
     * the first time */
    if (L7_TRUE == currEntry->medDevice)
    {
      /* Set the MED operational  parameters to default */
      if (lldpIntfTbl[index].medTransmitEnabled > 0)
      {
        lldpIntfTbl[index].medTransmitEnabled--;
      }
      /* if no MED devices after above decrement */
      if (lldpIntfTbl[index].medTransmitEnabled == 0)
      {
        lldpIntfTbl[index].medFastStart = 0;
      }
    }
    lldpRemEntryDelete(currEntry);
  }
  else
  {
    LLDP_TRACE(LLDP_DEBUG_MED, "Entry not found, intf %d, Add remote entry",
               lldpIntfTbl[index].intIfNum);
  }

  /* copy the src address */
  memcpy(entry.srcMac, srcMac, sizeof(entry.srcMac));

  /* Need to process this before adding the entry in case of unauthenticated voice vlan */
  LLDP_TRACE(LLDP_DEBUG_MED, "intf %d, process voice device PDU", entry.intIntfNum);
  lldpMedProcessVoiceDevicePdu(&entry, voiceDevice, deviceType);

  if (L7_FALSE == dot1xPortClientAuthenticationGet(lldpIntfTbl[index].intIfNum, srcMac))
  {
    lldpRemEntryDelete(&entry); /* make sure we return any allocated buffers */
    LLDP_TRACE(LLDP_DEBUG_PROTO, "Client not authorized by dot1x, intf %d",
               lldpIntfTbl[index].intIfNum);
    return L7_REQUEST_DENIED;
  }

  if (L7_TRUE == medTransmitEnabled)
  {
    lldpIntfTbl[index].medFastStart = medFastStart;
    lldpIntfTbl[index].medTransmitEnabled++;
  }

  /* Update stats here after we are commited to retaining this information */
  lldpStats->intfStats[index].rxTLVsDiscarded += tlvDiscard;
  lldpStats->intfStats[index].rxTLVsMED += tlvMed;
  lldpStats->intfStats[index].rxTLVs8021 += tlv8021;
  lldpStats->intfStats[index].rxTLVs8023 += tlv8023;
  lldpStats->intfStats[index].rxTLVsUnrecognized += tlvUnrecog;

  /* Add the new entry */
  currEntry = lldpRemEntryAdd(&entry);
  if(currEntry == L7_NULLPTR)
  {
    LLDP_TRACE(LLDP_DEBUG_MED, "Could not add entry, intf %d, return failure", entry.intIntfNum);
    return L7_FAILURE;
  }
  /* update timestamps, "start" TTL timer, and record remote table change */
  currEntry->rxTTL = ttl;
  /*lldpTimerAdd();*/
  lldpStats->remTblInserts++;
  lldpRemLastChangeTimeUpdate(index);

  /* Send the med trap */
  if (sendMedTrap == L7_TRUE &&
      lldpCfgData->intfCfgData[index].medNotifyEnabled == L7_ENABLE)
  {
    memset(&medinfo, 0x00, sizeof(lldpMedCapTLV_t));
    if (lldpOrgDefTLVGet(currEntry,
                         (L7_uchar8*)LLDP_MED_TIA_OUI_STRING,
                         LLDP_MED_SUBTYPE_CAP,
                         (L7_uchar8*)&medinfo,
                         sizeof(lldpMedCapTLV_t)) == L7_SUCCESS)
    {
      (void)trapMgrLldpXMedTopologyChangeDetectedTrap((L7_uint32)currEntry->chassisIdSubtype,
                                                      currEntry->chassisId,
                                                      (L7_uint32)currEntry->chassisIdLength,
                                                      (L7_uint32)medinfo.deviceType[0]);
    }
  }

  LLDP_TRACE(LLDP_DEBUG_MED, "Added new entry, ttl %d, intf %d", currEntry->rxTTL, currEntry->intIntfNum);
  rc = lldpMedGetRemEntryPoeInfo(currEntry,&poeDevice,&deviceType);
  if(rc == L7_SUCCESS)
  {
    lldpMedProcessPoeDevicePdu(currEntry,poeDevice,L7_TRUE);
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Transmit PDU on a given interface
*
* @param    L7_uint32       index     @b((input)) component index
* @param    L7_uint32       intIfNum  @b((input)) internal interface number
* @param    lldpPDUHandle_t pduHdl    @b((input)) pointer to LLDP PDU type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
static L7_RC_t lldpPduTransmit(L7_uint32 index,
                               L7_uint32 intIfNum,
                               lldpPDUHandle_t *pduHdl)
{
  DTL_CMD_TX_INFO_t  dtlCmd;
  L7_netBufHandle    bufHandle;
  L7_uchar8         *dataStart;


  if (L7_TRUE == mirrorIsActiveProbePort(intIfNum))
  {
    return L7_SUCCESS;
  }

  /* get a network buffer handle */
  SYSAPI_NET_MBUF_GET(bufHandle);
  if (bufHandle == L7_NULL)
  {
    return L7_FAILURE;
  }

  /* point to the start of the data */
  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, dataStart);
  /* set the length of the data */
  SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, pduHdl->offset);
  /* copy the data */
  memcpy(dataStart, pduHdl->buffer, pduHdl->offset);

  /* send the data */
  bzero((char *)&dtlCmd,sizeof(DTL_CMD_TX_INFO_t));
  dtlCmd.intfNum = intIfNum;
  dtlCmd.priority = 1;    /* TBD - double check this priority */
  dtlCmd.typeToSend = DTL_L2RAW_UNICAST;

  if (dtlPduTransmit(bufHandle, DTL_CMD_TX_L2, &dtlCmd) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  LLDP_TRACE(LLDP_DEBUG_PROTO, "Sent a LLDP PDU on interface %d",intIfNum);
  
  /* DEBUG */
  /*
  if (lldpRemoteDataPduRead(1, pduHdl.buffer, pduHdl.offset) != L7_SUCCESS)
  {
    printf("lldpRemoteDataPduRead failed\n");
  }
  */
  /* DEBUG */

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Transmit Local System Data for given interface
*
* @param    L7_uint32  index  @b((input)) component index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
static L7_RC_t lldpLocalDataPduTransmit(L7_uint32 index)
{
  L7_uint32          intIfNum = 0;
  lldpPDUHandle_t    pduHdl;

  intIfNum = lldpIntfTbl[index].intIfNum;

  pduHdl.buffer = lldpPDUBuf; /* for now, we only use one buffer */
  if (lldpLocalDataPduWrite(index, intIfNum, &pduHdl) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return lldpPduTransmit(index, intIfNum, &pduHdl);
}

/*********************************************************************
* @purpose  Transmit shutdown LLDPDU for given interface
*
* @param    L7_uint32  index  @b((input)) component index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
static L7_RC_t lldpShutdownPduTransmit(L7_uint32 index)
{
  L7_uint32          intIfNum = 0;
  lldpPDUHandle_t    pduHdl;

  intIfNum = lldpIntfTbl[index].intIfNum;

  pduHdl.buffer = lldpPDUBuf; /* for now, we only use one buffer */
  if (lldpShutdownPduWrite(index, intIfNum, &pduHdl) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return lldpPduTransmit(index, intIfNum, &pduHdl);
}

/*********************************************************************
* @purpose  Obtain a pointer to the interface configuration data
*           for this interface
*
* @param    intIfNum @b{(input)}   Internal Interface Number
* @param    *index   @b{(output)}  local component index
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @notes    Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled
*           or not).
*
* @notes    The caller can set the pCfg parm to L7_NULL if it does not
*           want the value output from this function.
*
* @end
*********************************************************************/
L7_BOOL lldpMapIntfIndexGet(L7_uint32 intIfNum, L7_uint32 *index)
{
  L7_uint32     i = 0;
  nimConfigID_t cfgId;

  if (!(LLDP_IS_READY))
  {
    return L7_FALSE;
  }

  /* check valid interface range */
  if (intIfNum <= 0 || intIfNum >= platIntfMaxCountGet())
  {
    return L7_FALSE;
  }

  i = lldpMapTbl[intIfNum];
  if (i == 0)
  {
    return L7_FALSE;
  }

  /* verify that the cfgId in the config data table
   * entry matches the cfgId that NIM maps to
   * the intIfNum we are considering
   */
  if (nimConfigIdGet(intIfNum, &cfgId) == L7_SUCCESS)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&cfgId,
                               &(lldpCfgData->intfCfgData[i].cfgId)) == L7_FALSE)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

      /* if we get here, either we have a table management error between lldpCfgData and lldpMapTbl or
      ** there is synchronization issue between NIM and components w.r.t. interface creation/deletion
      */
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_LLDP_COMPONENT_ID, "Error accessing LLDP config data for interface %d, %s in lldpMapIntIfNum.\n", intIfNum, ifName);
      return L7_FALSE;
    }
    /* make sure we have the intIfNum saved correctly */
    lldpIntfTbl[i].intIfNum = intIfNum;
  }

  *index = i;

  return L7_TRUE;
}

/*********************************************************************
* @purpose  Obtain a pointer to the first free interface config struct
*
* @param    intIfNum @b{(input)}   Internal Interface Number
* @param    **pCfg   @b{(output)}  ptr to lldp nterface config structure
*                           or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @notes    Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled
*           or not).
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL lldpMapIntfCfgEntryGet(L7_uint32 intIfNum, lldpIntfCfgData_t **pCfg)
{
  L7_uint32         i;
  nimConfigID_t     cfgId;
  nimConfigID_t     cfgIdNull;
  static L7_uint32  nextIndex = 1;

  memset(&cfgIdNull, 0, sizeof(nimConfigID_t));

  if (!(LLDP_IS_READY))
  {
    return L7_FALSE;
  }

  /* check valid interface range */
  if (intIfNum <= 0 || intIfNum >= platIntfMaxCountGet())
  {
    return L7_FALSE;
  }

  if (nimConfigIdGet(intIfNum, &cfgId) != L7_SUCCESS)
  {
    return L7_FALSE;
  }

  /* Avoid N^2 processing when interfaces created at startup */
  if (nextIndex < L7_LLDP_INTF_MAX_COUNT)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&lldpCfgData->intfCfgData[nextIndex].cfgId, &cfgIdNull))
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_LLDP_COMPONENT_ID,
              "lldpMapIntfConfigEntryGet: Found empty config ID 1 %d for interface %d, %s\n",
              nextIndex, intIfNum, ifName);
      lldpMapTbl[intIfNum] = nextIndex;
      lldpIntfTbl[nextIndex].intIfNum = intIfNum;
      memset(&lldpIntfTbl[intIfNum].acquiredList,0,sizeof(AcquiredMask));
      *pCfg = &lldpCfgData->intfCfgData[nextIndex];
      nextIndex++;
      return L7_TRUE;
    }
  }

  /* Cached nextIndex is in use. Resort to search from beginning. N^2. */
  for (i = 1; i < L7_LLDP_INTF_MAX_COUNT; i++)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&lldpCfgData->intfCfgData[i].cfgId, &cfgIdNull))
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_LLDP_COMPONENT_ID,
              "lldpMapIntfConfigEntryGet: Found empty config ID 2 %d for interface %d, %s\n",
              i, intIfNum, ifName);
      lldpMapTbl[intIfNum] = i;
      lldpIntfTbl[i].intIfNum = intIfNum;
      *pCfg = &lldpCfgData->intfCfgData[i];
      nextIndex = i + 1;
      return L7_TRUE;
    }
  }

  return L7_FALSE;
}

/*********************************************************************
*
* @purpose  Apply Tx and Rx mode per interface
*
* @param    L7_uint32  index  component interface index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t lldpIntfConfigDataApply(L7_uint32 index)
{
  if (lldpIntfTxModeApply(lldpIntfTbl[index].intIfNum) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (lldpIntfRxModeApply(lldpIntfTbl[index].intIfNum) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Applies the 802.1AB transmit mode per interface
*
* @param    L7_uint32  intIfNum  @b((input))internal Interface number
*
* @returns  L7_SUCCESS, if interface mode was successfully applied
* @returns  L7_FAILURE
*
* @notes    This function assumes mode parameter is valid
*
* @end
*********************************************************************/
L7_RC_t lldpIntfTxModeApply(L7_uint32 intIfNum)
{
  L7_uint32 index=0;

  if (lldpMapIntfIndexGet(intIfNum, &index) != L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_LLDP_COMPONENT_ID, "lldpIntfTxModeApply(): \
                   Invalid Internal Interface number %d \n",intIfNum);
    return L7_FAILURE;
  }

  /* If we are enabled, port is up, and we are not waiting on
     reinitialization, start transmitting */
  if (lldpCfgData->intfCfgData[index].txEnabled == L7_TRUE &&
      lldpIntfTbl[index].txActive == L7_TRUE &&
      lldpIntfTbl[index].txReinitDelay == 0)
  {
    if (lldpIntfTbl[index].txTTR == 0)
    {
      if (lldpLocalDataPduTransmit(index) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_LLDP_COMPONENT_ID, "lldpIntfTxModeApply(): LLDPDU \
                       Transmit Failed!. Unable to reserve network buffer for transmission\n");
      }
      lldpIntfTbl[index].txTTR = lldpCfgData->txInterval;
      lldpIntfTbl[index].txDelayWhile = lldpCfgData->txDelay;

     /* lldpTimerAdd();*/
    }
  }
  else
  {
    /* if disabled, port is up, and we are transmitting,
       send the shutdown LLDPDU */
    if (lldpCfgData->intfCfgData[index].txEnabled == L7_FALSE &&
        lldpIntfTbl[index].txActive == L7_TRUE &&
        lldpIntfTbl[index].txTTR != 0)
    {
      lldpShutdownPduTransmit(index);
    }

    /* if disabled or port went down, and we are transmitting,
       initialize the reinit delay */
    if ((lldpCfgData->intfCfgData[index].txEnabled == L7_FALSE ||
         lldpIntfTbl[index].txActive == L7_FALSE) &&
        lldpIntfTbl[index].txTTR != 0 &&
        lldpIntfTbl[index].txReinitDelay == 0)
    {
      lldpIntfTbl[index].txReinitDelay = lldpCfgData->reinitDelay;
      /*lldpTimerAdd();*/
    }

    /* any other condition means we should not be transmitting */
    if (lldpIntfTbl[index].txTTR != 0)
    {
      lldpIntfTbl[index].txTTR = 0;
     /* lldpTimerDelete(); */
    }
    /* Set the MED parameters to defaults */
    if (lldpIntfTbl[index].medTransmitEnabled > 0)
    {
      lldpIntfTbl[index].medFastStart = 0;
      lldpIntfTbl[index].medTransmitEnabled = 0;
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Applies the 802.1AB receive mode per interface
*
* @param    L7_uint32  intIfNum  @b((input)) internal Interface number
*
* @returns  L7_SUCCESS, if interface mode was successfully applied
* @returns  L7_FAILURE, if interface number is invalid
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpIntfRxModeApply(L7_uint32 intIfNum)
{
  L7_uint32 index=0;

  if (lldpMapIntfIndexGet(intIfNum, &index) != L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_LLDP_COMPONENT_ID, "lldpIntfRxModeApply():"
                   "Invalid Internal Interface number %d \n",intIfNum);
    return L7_FAILURE;
  }

  /* if enabled, port is up, tell the driver we want to see PDUs */
  if (lldpCfgData->intfCfgData[index].rxEnabled == L7_TRUE &&
      lldpIntfTbl[index].rxActive == L7_TRUE)
  {
    if (lldpIntfTbl[index].rxFrame == L7_FALSE)
    {
      /* configure the driver to accept LLDP frames for this port */
      dtlLldpModeSet(lldpIntfTbl[index].intIfNum, L7_ENABLE);
      lldpIntfTbl[index].rxFrame = L7_TRUE;
    }
  }
  else
  {
    /* for any other condition, configure the driver to drop PDUs */
    if (lldpIntfTbl[index].rxFrame == L7_TRUE)
    {
      /* configure the driver to drop LLDP frames for this port */
      dtlLldpModeSet(lldpIntfTbl[index].intIfNum, L7_DISABLE);
      lldpIntfTbl[index].rxFrame = L7_FALSE;
    }

    /* I believe once we are disabled, by admin disabled we delete the remote data */
    /* If the port is only linked down then let it ageout on it's own
     * the flag capture this condition
     */

    if ( lldpSkipDeleteRemEntry == L7_FALSE)
    {
      if (lldpRemDataInterfaceDelete(lldpIntfTbl[index].intIfNum,L7_TRUE)!= L7_FAILURE)
      {
        /* Set the MED operational  parameters to default */
        if (lldpIntfTbl[index].medTransmitEnabled > 0)
        {
          lldpIntfTbl[index].medTransmitEnabled = 0;
          lldpIntfTbl[index].medFastStart = 0;
        }
        lldpRemLastChangeTimeUpdate(index);
      }
    }
    else
    {
      /* Set the MED operational  parameters to default  Allow entry to be aged out as this is a link down event*/
      if (lldpIntfTbl[index].medTransmitEnabled > 0)
      {
        lldpIntfTbl[index].medTransmitEnabled = 0;
        lldpIntfTbl[index].medFastStart = 0;
      }
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Propogate Interface notifications to lldpTask
*
* @param    L7_uint32         intIfNum    @b((input))  Interface number
* @param    L7_uint32         event       @b((input))  Event type
* @param    NIM_CORRELATOR_t  correlator  @b((input))  Correlator for event
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpIntfChangeCallback(L7_uint32 intIfNum,
                               L7_uint32 event,
                               NIM_CORRELATOR_t correlator)
{
  lldpMgmtMsg_t msg;

  msg.msgId = lldpMsgIntfChange;

  msg.u.lldpIntfChangeParms.intIfNum = intIfNum;
  msg.u.lldpIntfChangeParms.event = event;
  msg.u.lldpIntfChangeParms.correlator = correlator;

  if (osapiMessageSend(lldpQueue,
                       &msg,
                       LLDP_MSG_SIZE,
                       L7_NO_WAIT,
                       L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_LLDP_COMPONENT_ID,
            "lldpIntfChangeCallback(): message send failed\n");
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Handle NIM startup callback
*
* @param    startupPhase    Activate/Create
*
* @returns  void
*
* @notes    Transfer to lldp thread
*
* @end
*********************************************************************/
void lldpStartupCallback(NIM_STARTUP_PHASE_t startupPhase)
{
  lldpMgmtMsg_t   msg;
  L7_RC_t         rc;

  memset(&msg, 0, sizeof(lldpMgmtMsg_t));
  msg.msgId    = lldpMsgStartup;
  msg.u.lldpStartupParms.startupPhase = startupPhase;

  rc = osapiMessageSend(lldpQueue, &msg, LLDP_MSG_SIZE,
                        L7_NO_WAIT, L7_MSG_PRIORITY_NORM);

  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_LLDP_COMPONENT_ID,
            "Failed to send NIM startup phase %d to LLDP thread",
            startupPhase);
  }

  return;
}

/*********************************************************************
* @purpose  Do NIM create startup
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes    LLDP has no configuration at this point
*
* @end
*********************************************************************/
L7_RC_t lldpNimCreateStartup(void)
{
  L7_RC_t          rc, rc2;
  L7_uint32        intIfNum;

  LLDP_TRACE(LLDP_DEBUG_CHECKPOINT, "LLDP Create Startup beginning");

  rc = nimFirstValidIntfNumber(&intIfNum);

  while (rc == L7_SUCCESS)
  {
    osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
    rc2 = lldpIntfCreate(intIfNum);
    osapiSemaGive(lldpSemaphore);

    rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
  }

  /* Register with NIM to receive port CREATE and DELETE events */
  memset(&lldpNimEventMask, 0, sizeof(PORTEVENT_MASK_t));
  PORTEVENT_SETMASKBIT(lldpNimEventMask, L7_CREATE);
  PORTEVENT_SETMASKBIT(lldpNimEventMask, L7_DELETE);
  nimRegisterIntfEvents(L7_LLDP_COMPONENT_ID, lldpNimEventMask);

  nimStartupEventDone(L7_LLDP_COMPONENT_ID);

  LLDP_TRACE(LLDP_DEBUG_CHECKPOINT, "LLDP Create Startup done");
  L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_LLDP_COMPONENT_ID,
            "LLDP Create Startup done");
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Do NIM activate startup
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpNimActivateStartup(void)
{
  L7_RC_t          rc, rc2;
  L7_uint32        intIfNum;
  L7_BOOL          probeSetup;
  L7_uint32        activeState = L7_INACTIVE;
  L7_uint32        linkState;

  LLDP_TRACE(LLDP_DEBUG_CHECKPOINT, "LLDP Activate Startup beginning");

  rc = nimFirstValidIntfNumber(&intIfNum);

  while (rc == L7_SUCCESS)
  {
    if (nimGetIntfLinkState(intIfNum, &linkState) != L7_SUCCESS)
    {
      rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
      continue;
    }

    probeSetup = mirrorIsActiveProbePort(intIfNum);

    LLDP_TRACE(LLDP_DEBUG_CHECKPOINT_VERBOSE, "LLDP warm restart check: intIfNum %d, probeSetup %d, activeState %d",
               intIfNum, probeSetup, activeState);

    if (probeSetup != L7_TRUE)
    {
      if (activeState == L7_ACTIVE)
      {
        osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
        rc2 = lldpWarmRestartIntfActive(intIfNum);
        osapiSemaGive(lldpSemaphore);
      }
    }

    rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
  }

  /* If this is a warm restart, apply checkpointed data. */
  if (lldpWarmRestart == L7_TRUE)
  {
#ifdef L7_NSF_PACKAGE
    lldpCheckpointDataApply();
#endif /* L7_NSF_PACKAGE */
  }

  rc = nimFirstValidIntfNumber(&intIfNum);

  while (rc == L7_SUCCESS)
  {
    if ((nimGetIntfActiveState(intIfNum, &activeState) != L7_SUCCESS) ||
        (nimGetIntfLinkState(intIfNum, &linkState)     != L7_SUCCESS))
    {
      rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
      continue;
    }

    probeSetup = mirrorIsActiveProbePort(intIfNum);

    LLDP_TRACE(LLDP_DEBUG_CHECKPOINT_VERBOSE, "LLDP intIfNum %d, probeSetup %d, activeState %d",
               intIfNum, probeSetup, activeState);

    if (probeSetup != L7_TRUE)
    {
      if (L7_UP == linkState)
      {
        osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
        rc2 = lldpIntfRxActive(intIfNum);
        osapiSemaGive(lldpSemaphore);

        if (activeState == L7_ACTIVE)
        {
          /*
            If activeState is set to L7_ACTIVE, then either dot1xPortIsAuthorized is true,
            or the intf is not a valid dot1x port
          */
          osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
          rc2 = lldpIntfTxActive(intIfNum);
          osapiSemaGive(lldpSemaphore);
        }
      }
    }

    rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
  }

  /* Now ask NIM to send any future changes for these event types */
  memset(&lldpNimEventMask, 0, sizeof(PORTEVENT_MASK_t));
  PORTEVENT_SETMASKBIT(lldpNimEventMask, L7_CREATE);
  PORTEVENT_SETMASKBIT(lldpNimEventMask, L7_DELETE);
  PORTEVENT_SETMASKBIT(lldpNimEventMask, L7_UP);
  PORTEVENT_SETMASKBIT(lldpNimEventMask, L7_DOWN);
  PORTEVENT_SETMASKBIT(lldpNimEventMask, L7_PROBE_SETUP);
  PORTEVENT_SETMASKBIT(lldpNimEventMask, L7_PROBE_TEARDOWN);
  PORTEVENT_SETMASKBIT(lldpNimEventMask, L7_ACTIVE);
  PORTEVENT_SETMASKBIT(lldpNimEventMask, L7_INACTIVE);

  nimRegisterIntfEvents(L7_LLDP_COMPONENT_ID, lldpNimEventMask);

  nimStartupEventDone(L7_LLDP_COMPONENT_ID);

  lldpActivateStartupDone = L7_TRUE;

  cnfgrApiComponentHwUpdateDone(L7_LLDP_COMPONENT_ID, L7_CNFGR_HW_APPLY_CONFIG);
  LLDP_TRACE(LLDP_DEBUG_CHECKPOINT, "LLDP Activate Startup done");
  L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_LLDP_COMPONENT_ID,
            "LLDP Activate Startup done");

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Handle Interface notifications
*
* @param    L7_uint32         intIfNum    @b((input))  Interface number
* @param    L7_uint32         event       @b((input))  Event type
* @param    NIM_CORRELATOR_t  correlator  @b((input))  Correlator for event
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpIntfChangeProcess(L7_uint32 intIfNum,
                              L7_uint32 event,
                              NIM_CORRELATOR_t correlator)
{
  L7_RC_t                    rc = L7_SUCCESS;
  NIM_EVENT_COMPLETE_INFO_t  status;
  L7_BOOL     prevAcquired = L7_FALSE;
  L7_uint32   index=0;



  status.intIfNum     = intIfNum;
  status.component    = L7_LLDP_COMPONENT_ID;
  status.event        = event;
  status.correlator   = correlator;

  if (!(LLDP_IS_READY))
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_LLDP_COMPONENT_ID, "Received an interface change callback while not ready to receive it");
    status.response.rc  = rc;
    nimEventStatusCallback(status);
    return L7_FAILURE;
  }

  if (lldpIsValidIntf(intIfNum) != L7_TRUE)
  {
    status.response.rc  = L7_SUCCESS;
    nimEventStatusCallback(status);
    return L7_SUCCESS;
  }

  if(event != L7_CREATE)
  {
    if(lldpMapIntfIndexGet(intIfNum, &index) != L7_TRUE)
    {
       L7_LOGF(L7_LOG_SEVERITY_INFO, L7_LLDP_COMPONENT_ID,
               "Received an interface change callback for a invalid interface %d \n",intIfNum); 
       status.response.rc	= L7_SUCCESS;
       nimEventStatusCallback(status);
       return L7_SUCCESS;
    }
  }

  LLDP_TRACE(LLDP_DEBUG_PROTO, "Event recv for Intf - %d and event - %d \n",
             intIfNum,event);

  switch (event)
  {
    /* we are only interested in when the port becomes
       configurable and a change in operational status */

    case L7_PROBE_TEARDOWN:
    case L7_UP:
      osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
      if(event ==L7_PROBE_TEARDOWN)
      {
        COMPONENT_ACQ_CLRMASKBIT(lldpIntfTbl[index].acquiredList,L7_PORT_MIRROR_COMPONENT_ID);
      }
      COMPONENT_ACQ_NONZEROMASK(lldpIntfTbl[index].acquiredList,prevAcquired);
      if(prevAcquired == L7_FALSE)
      {
        rc = lldpIntfRxActive(intIfNum);
      }
      osapiSemaGive(lldpSemaphore);
      break;

    case L7_ACTIVE:
      osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
      COMPONENT_ACQ_NONZEROMASK(lldpIntfTbl[index].acquiredList,prevAcquired);
      if(prevAcquired == L7_FALSE)
      {
        rc = lldpIntfTxActive(intIfNum);
      }
      osapiSemaGive(lldpSemaphore);
      break;

    case L7_DOWN:
      lldpSkipDeleteRemEntry = L7_TRUE;
      /* fall through */

    case L7_PROBE_SETUP:
      osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
      if(event == L7_PROBE_SETUP)
      {
        COMPONENT_ACQ_SETMASKBIT(lldpIntfTbl[index].acquiredList,L7_PORT_MIRROR_COMPONENT_ID);
      }
      rc = lldpIntfRxInactive(intIfNum);
      rc = lldpIntfTxInactive(intIfNum);
      osapiSemaGive(lldpSemaphore);
      break;

    case L7_INACTIVE:
      osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
      rc = lldpIntfTxInactive(intIfNum);
      /* Set the flag to false even if there was an early return. This way we
       * can ensure that this flag is not set for ever and can be accurate for the next
       * interface.
       */
      lldpSkipDeleteRemEntry = L7_FALSE;
      osapiSemaGive(lldpSemaphore);
      break;

    case L7_CREATE:
      osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
      rc = lldpIntfCreate(intIfNum);
      osapiSemaGive(lldpSemaphore);
      break;

    case L7_DELETE:
      osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
      rc = lldpIntfDelete(intIfNum);
      osapiSemaGive(lldpSemaphore);
      break;

    default:
      break;
  }

  status.response.rc  = rc;
  nimEventStatusCallback(status);

  return rc;
}

/*********************************************************************
* @purpose  Send a message to the lldpTask to process incoming PDU
*
* @param    bufHandle          buffer  @b((input)) handle to the lldpdu received
* @param    sysnet_pdu_info_t *pduInfo @b((input)) pointer to pdu info structure
*
* @returns  L7_SUCCESS  if the message was put on the queue
* @returns  L7_FAILURE  if the message was not put on the queue
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpPduReceiveCallback(L7_netBufHandle bufHandle,
                               sysnet_pdu_info_t *pduInfo)
{
  L7_uchar8        *data = L7_NULLPTR;
  L7_uint32         index = 0;
  lldpMgmtMsg_t     msg;
  L7_enet_encaps_t *encap = L7_NULLPTR;
  L7_ushort16       ethertype = 0;

  if (!lldpQueue)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_LLDP_COMPONENT_ID, "lldpPduReceiveCallback(): lldpQueue has not been created!\n");
    return L7_FAILURE;
  }

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);

  encap = (L7_enet_encaps_t *)(data + L7_ENET_HDR_SIZE);
  ethertype = osapiNtohs(encap->type);
  if ((memcmp(data, L7_ENET_LLDP_MULTICAST_MAC_ADDR.addr, L7_MAC_ADDR_LEN) != 0) ||
      (ethertype != L7_ETYPE_LLDP))
  {
    return L7_FAILURE;
  }

  if ((lldpIsValidIntf(pduInfo->intIfNum) != L7_TRUE) ||
      (lldpMapIntfIndexGet(pduInfo->intIfNum, &index) != L7_TRUE) ||
      (lldpCfgData->intfCfgData[index].rxEnabled != L7_TRUE))
  {
    return L7_FAILURE;
  }

  msg.msgId = lldpMsgPduReceive;
  msg.u.lldpPduReceiveParms.intIfNum = pduInfo->intIfNum;
  msg.u.lldpPduReceiveParms.bufHandle = bufHandle;
  if (osapiMessageSend(lldpQueue,
                       &msg,
                       LLDP_MSG_SIZE,
                       L7_NO_WAIT,
                       L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_LLDP_COMPONENT_ID, "lldpPduReceiveCallback(): "
                                      "message send failed. Message Queue is full\n");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process an incoming packet
*
* @param    L7_uint32       intIfNum   @b((input)) NIM internal interface number
* @param    L7_netBufHandle bufHandle  @b((input)) pointer to the ethernet packet received
*
* @returns  L7_SUCCESS  if the message was put on the queue
* @returns  L7_FAILURE  if the message was not put on the queue
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpPduReceiveProcess(L7_uint32 intIfNum,
                              L7_netBufHandle bufHandle)
{
  L7_uchar8 *data = L7_NULLPTR;
  L7_uint32  index = 0;
  L7_uint32  length = 0;
  L7_uint32  offset = 0;
  L7_RC_t    rc = L7_FAILURE;
  L7_uchar8 srcMac[L7_MAC_ADDR_LEN];

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);

  /*
   * these checks are done in lldpPduReceiveCallback,
   * but this is an async call should verify again
   * with the semaphore locked.
   */
  if ((lldpIsValidIntf(intIfNum) != L7_TRUE) ||
      (lldpMapIntfIndexGet(intIfNum, &index) != L7_TRUE) ||
      (lldpCfgData->intfCfgData[index].rxEnabled != L7_TRUE))
  {
    SYSAPI_NET_MBUF_FREE(bufHandle);
    LLDP_TRACE(LLDP_DEBUG_PROTO_VERBOSE, "Received PDU on invalid intf %d", intIfNum);
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, length);
  offset = sysNetDataOffsetGet(data);

  /* Copy the src mac of the PDU */
  memcpy(srcMac, &data[6],L7_MAC_ADDR_LEN);

  rc = lldpRemoteDataPduRead(index, data + offset, length - offset,srcMac);
  if (rc == L7_SUCCESS)
  {
    lldpStats->intfStats[index].rxFramesTotal++;
    LLDP_TRACE(LLDP_DEBUG_PROTO_VERBOSE, "Received PDU and processed successfully on intf %d", intIfNum);
  }
  else if (rc == L7_TABLE_IS_FULL)
  {
    /* log buffer message? */
    lldpStats->remTblDrops++;
    LLDP_TRACE(LLDP_DEBUG_PROTO_VERBOSE, "Received PDU but table full on intf %d", intIfNum);
  }
  else if (rc == L7_REQUEST_DENIED)
  {
    /* do nothing -- frame does not count since port is not authorized by dot1x */
    LLDP_TRACE(LLDP_DEBUG_MED, "Received PDU on unauthorized intf %d", intIfNum);
  }
  else
  {
    lldpStats->intfStats[index].rxFramesDiscarded++;
    lldpStats->intfStats[index].rxFramesErrors++;
    LLDP_TRACE(LLDP_DEBUG_PROTO_VERBOSE, "Received PDU and discarded on intf %d", intIfNum);
  }
  SYSAPI_NET_MBUF_FREE(bufHandle);
  osapiSemaGive(lldpSemaphore);
  return rc;
}

/*********************************************************************
*
* @purpose callback to handle the 802.1AB timer events
*
* @param void
*
* @returns void
*
* @notes none
*
* @end
*
*********************************************************************/
void lldpTimerCallback()
{
  lldpMgmtMsg_t msg;

  if (!lldpQueue)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_LLDP_COMPONENT_ID, "lldpTimerCallback(): lldpQueue has not been created!\n");
  }

  msg.msgId = lldpMsgTimer;
  if (osapiMessageSend(lldpQueue,
                       &msg,
                       LLDP_MSG_SIZE,
                       L7_NO_WAIT,
                       L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_LLDP_COMPONENT_ID, "lldpTimerCallback(): timer send failed\n");
    osapiTimerAdd((void *)lldpTimerCallback, L7_NULL, L7_NULL,
                  LLDP_TIMER_INTERVAL, &lldpTimer);
  }
}

/*********************************************************************
* @purpose  Handle timer notifications
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
void lldpTimerProcess(void)
{
  L7_uint32 index = 0,tempAgeout;
  lldpRemDataEntry_t *entry,*prevEntry=L7_NULLPTR;

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  for (index = 1; index < L7_LLDP_INTF_MAX_COUNT; index++)
  {
    if (lldpIntfTbl[index].medFastStart != 0)
    {
      lldpIntfTbl[index].txDelayWhile = 1;
    }

    if (lldpIntfTbl[index].txDelayWhile != 0)
    {
      lldpIntfTbl[index].txDelayWhile--;
    }

    /* check for expired txTTRs */
    if (lldpIntfTbl[index].txTTR != 0)
    {
      lldpIntfTbl[index].txTTR--;
      if ((lldpIntfTbl[index].txDelayWhile == 0) &&
          ((lldpIntfTbl[index].txTTR == 0) ||
           (lldpIntfTbl[index].localDbChanged == L7_TRUE) ||
           (lldpIntfTbl[index].medFastStart != 0)))
      {
        if (lldpLocalDataPduTransmit(index) != L7_SUCCESS)
        {
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_LLDP_COMPONENT_ID, "lldpTimerProcess(): LLDPDU "
                     "Transmit Failed! Unable to reserve network buffer for transmission\n");
        }
        else
        {
          /* update tx stats */
          lldpStats->intfStats[index].txFramesTotal++;
          lldpIntfTbl[index].localDbChanged = L7_FALSE;
        }
        lldpIntfTbl[index].txTTR = lldpCfgData->txInterval;
        lldpIntfTbl[index].txDelayWhile = lldpCfgData->txDelay;
      }
    }

    if (lldpIntfTbl[index].medFastStart > 0)
    {
      lldpIntfTbl[index].medFastStart--;
    }

    tempAgeout = lldpStats->remTblAgeouts;
    while((entry=lldpRemDataNeighborGetNext(lldpIntfTbl[index].intIfNum,prevEntry))!=L7_NULLPTR)
    {
      entry->rxTTL--;
      /* check for expired rxTTLs */
      if(entry->rxTTL == 0)
      {
        /* delete remote entry and record stats */
        LLDP_TRACE(LLDP_DEBUG_MED, "TTL now 0, intf %d, check for device removal", entry->intIntfNum);
        lldpMedRemDeviceRemoveCheck(entry);
        if (L7_TRUE == entry->medDevice)
        {
          /* Set the MED operational  parameters to default */
          if (lldpIntfTbl[index].medTransmitEnabled > 0)
          {
            lldpIntfTbl[index].medTransmitEnabled--;
          }
          /* if no MED devices after above decrement */
          if (lldpIntfTbl[index].medTransmitEnabled == 0)
          {
            lldpIntfTbl[index].medFastStart = 0;
          }
        }
        lldpRemEntryDelete(entry);

        lldpStats->remTblAgeouts++;
        lldpStats->intfStats[index].rxAgeouts++;
      }
      else
      {
        prevEntry = entry;
      }
    }

    /* send the notification if any of the remote nodes were timedout*/
    if(tempAgeout != lldpStats->remTblAgeouts)
    {
      lldpRemLastChangeTimeUpdate(index);
    }

    /* check for expired txReinitDelay */
    if (lldpIntfTbl[index].txReinitDelay != 0)
    {
      lldpIntfTbl[index].txReinitDelay--;
      if (lldpIntfTbl[index].txReinitDelay == 0)
      {
        /*lldpTimerDelete();*/
        /* apply tx mode if it has been enabled
           since we started the timer */
        if (lldpCfgData->intfCfgData[index].txEnabled == L7_TRUE)
        {
          lldpIntfTxModeApply(lldpIntfTbl[index].intIfNum);
        }
      }
    }
  }
  osapiSemaGive(lldpSemaphore);
  osapiTimerAdd((void *)lldpTimerCallback, L7_NULL, L7_NULL,
                LLDP_TIMER_INTERVAL, &lldpTimer);

  return;
}

/*********************************************************************
* @purpose  Verify a remote index entry is still valid
*
* @param    L7_uint32       intIfNum   @b((input)) NIM internal interface number
* @param    L7_uint32       index      @b((input)) component index
* @param    L7_uint32       timestamp  @b((input)) remote entry timestamp
*
* @returns  the Remote node
*
* @notes    none
*
* @end
*********************************************************************/
lldpRemDataEntry_t *lldpIsRemEntryValid(L7_uint32 intIfNum,
                                        L7_uint32 index,
                                        L7_uint32 timestamp)
{
  lldpRemDataEntry_t *remEntry,*prevEntry=L7_NULLPTR;

  while((remEntry=lldpRemDataNeighborGetNext(intIfNum,prevEntry))!=L7_NULLPTR)
  {
    if(remEntry->remIndex == index && remEntry->timestamp == timestamp)
    {
      return remEntry;
    }
    prevEntry = remEntry;
  }
  return L7_NULLPTR;
}

/*********************************************************************
* @purpose  Propogate Interface Rx Apply to lldpTask
*
* @param    L7_uint32       intIfNum   @b((input))  Interface number
* @param    L7_BOOL         rxApply    @b((input))  Apply Rx only
* @param    L7_BOOL         txApply    @b((input))  Apply Tx only
* *
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpIntfModeApplyPostMsg(L7_uint32 intIfNum,
                                 L7_BOOL   rxApply,
                                 L7_BOOL   txApply)
{
  lldpMgmtMsg_t msg;

  memset(&msg,0,sizeof(lldpMgmtMsg_t));
  msg.msgId = lldpMsgIntfModeApply;
  msg.u.lldpMsgIntfModeApplyParms.intIfNum = intIfNum;
  msg.u.lldpMsgIntfModeApplyParms.rxApply = rxApply;
  msg.u.lldpMsgIntfModeApplyParms.txApply = txApply;


  if (osapiMessageSend(lldpQueue,
                       &msg,
                       LLDP_MSG_SIZE,
                       L7_NO_WAIT,
                       L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_LLDP_COMPONENT_ID,
            "lldpIntfModeApplyPostMsg(): message send failed\n");
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Propogate Remote Database  Clear Message to lldpTask
*
* @param    None
* *
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpRemoteDBClearPostMsg(void)
{
  lldpMgmtMsg_t msg;

  memset(&msg,0,sizeof(lldpMgmtMsg_t));
  msg.msgId = lldpMsgRemDbClear;


  if (osapiMessageSend(lldpQueue,
                       &msg,
                       LLDP_MSG_SIZE,
                       L7_NO_WAIT,
                       L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_LLDP_COMPONENT_ID,
            "lldpRemoteDBClearPostMsg(): message send failed\n");
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Print the current 802.1AB config values to serial port
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpCfgDump(void)
{
  L7_uint32 i;
  nimConfigID_t cfgIdNull;

  memset(&cfgIdNull, 0, sizeof(nimConfigID_t));

  printf("\n");
  printf("802.1AB Configuration\n");
  printf("=====================\n\n");

  printf("Transmit Interval: %i\n", lldpCfgData->txInterval);
  printf("Transmit Hold Multiplier: %i\n", lldpCfgData->txHoldMultiplier);
  printf("Notification Interval: %i\n", lldpCfgData->notifyInterval);
  printf("Reinit Delay: %i\n", lldpCfgData->reinitDelay);

  printf("\nInterfaces Configured:\n");
  for (i = 1; i < L7_LLDP_INTF_MAX_COUNT; i++)
  {
    L7_uint32 intIfNum, extIfNum;
    L7_uint32 index = 0;

    if (NIM_CONFIG_ID_IS_EQUAL(&lldpCfgData->intfCfgData[i].cfgId, &cfgIdNull))
    {
      continue;
    }
    if (nimIntIfFromConfigIDGet(&(lldpCfgData->intfCfgData[i].cfgId),
                                &intIfNum) != L7_SUCCESS)
    {
      continue;
    }
    if (lldpMapIntfIndexGet(intIfNum, &index) == L7_TRUE)
    {
      lldpIntfCfgData_t *pCfg = &lldpCfgData->intfCfgData[index];

      if (nimGetIntfIfIndex(intIfNum, &extIfNum) == L7_SUCCESS)
      {
        printf("Interface: %d\n", extIfNum);
        printf("Transmit Mode: %s\n", pCfg->txEnabled ? "Enabled" : "Disabled");
        printf("Receive Mode: %s\n", pCfg->rxEnabled ? "Enabled" : "Disabled");
        printf("Notification Mode: %s\n", pCfg->notificationEnabled ? "Enabled" : "Disabled");
        printf("Transmit TLVs:\n");
        printf("  Port Description: %s\n",
               (pCfg->optionalTLVsEnabled & LLDP_TX_TLV_PORT_DESC_BIT) ? "Y" : "N");
        printf("  System Name: %s\n",
               (pCfg->optionalTLVsEnabled & LLDP_TX_TLV_SYS_NAME_BIT) ? "Y" : "N");
        printf("  System Description: %s\n",
               (pCfg->optionalTLVsEnabled & LLDP_TX_TLV_SYS_DESC_BIT) ? "Y" : "N");
        printf("  System Capability: %s\n",
               (pCfg->optionalTLVsEnabled & LLDP_TX_TLV_SYS_CAP_BIT) ? "Y" : "N");
        printf("  Mgmt Addr: %s\n", pCfg->mgmtAddrTxEnabled ? "Y" : "N");
      }
    }
  }
  printf("\n");

  printf("\n=====================\n");

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Print the current 802.1AB interface status to serial port
*
* @param    L7_uint32    intIfNum   @b((input)) NIM internal interface number
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpIntfDump(L7_uint32 intIfNum)
{
  L7_uint32 index = 0;
  L7_uint32 extIfNum = 0;

  printf("\n");
  printf("802.1AB Configuration\n");
  printf("=====================\n\n");

  if (lldpMapIntfIndexGet(intIfNum, &index) == L7_TRUE)
  {
    if (nimGetIntfIfIndex(intIfNum, &extIfNum) == L7_SUCCESS)
    {
      printf("Interface: %d\n", extIfNum);
      printf("Rx Link: %s\n", lldpIntfTbl[index].rxActive ? "Up" : "Down");
      printf("Tx Link: %s\n", lldpIntfTbl[index].txActive ? "Up" : "Down");
      printf("txTTR: %i\n", lldpIntfTbl[index].txTTR);
      printf("rxFrame: %s\n", lldpIntfTbl[index].rxFrame ? "Y" : "N");
    }
  }
  printf("\n");
  printf("\n=====================\n");

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Print the current 802.1AB interface config values to serial port
*
* @param    L7_uint32    intIfNum   @b((input)) NIM internal interface number
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpCfgIntfDump(L7_uint32 intIfNum)
{
  L7_uint32 index = 0;
  L7_uint32 extIfNum = 0;

  printf("\n");
  printf("802.1AB Configuration\n");
  printf("=====================\n\n");

  printf("Transmit Interval: %i\n", lldpCfgData->txInterval);
  printf("Transmit Hold Multiplier: %i\n", lldpCfgData->txHoldMultiplier);
  printf("Notification Interval: %i\n", lldpCfgData->notifyInterval);
  printf("Reinit Delay: %i\n", lldpCfgData->reinitDelay);

  printf("\nInterface Configuration:\n\n");

  if (lldpMapIntfIndexGet(intIfNum, &index) == L7_TRUE)
  {
    lldpIntfCfgData_t *pCfg = &lldpCfgData->intfCfgData[index];

    if (nimGetIntfIfIndex(intIfNum, &extIfNum) == L7_SUCCESS)
    {
      printf("Interface: %d\n", extIfNum);
      printf("Transmit Mode: %s\n", pCfg->txEnabled ? "Enabled" : "Disabled");
      printf("Receive Mode: %s\n", pCfg->rxEnabled ? "Enabled" : "Disabled");
      printf("Notification Mode: %s\n", pCfg->notificationEnabled ? "Enabled" : "Disabled");
      printf("Transmit TLVs:\n");
      printf("  Port Description: %s\n",
             (pCfg->optionalTLVsEnabled & LLDP_TX_TLV_PORT_DESC_BIT) ? "Y" : "N");
      printf("  System Name: %s\n",
             (pCfg->optionalTLVsEnabled & LLDP_TX_TLV_SYS_NAME_BIT) ? "Y" : "N");
      printf("  System Description: %s\n",
             (pCfg->optionalTLVsEnabled & LLDP_TX_TLV_SYS_DESC_BIT) ? "Y" : "N");
      printf("  System Capability: %s\n",
             (pCfg->optionalTLVsEnabled & LLDP_TX_TLV_SYS_CAP_BIT) ? "Y" : "N");
      printf("  Mgmt Addr: %s\n", pCfg->mgmtAddrTxEnabled ? "Y" : "N");
    }
  }
  printf("\n");

  printf("\n=====================\n");

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Print the current 802.1AB remote data count to serial port
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpRemDataCountDump(void)
{
  lldpRemDataKey_t key;
  lldpRemDataEntry_t *remEntry;
  L7_uint32            i=0;

  memset(&key,0,sizeof(lldpRemDataKey_t));
  printf("\n");
  while((remEntry=lldpRemDataEntryGetNext(&key))!=L7_NULLPTR)
  {
    i++;
    /* copy the entry to the key */
    memcpy(&key,remEntry,sizeof(lldpRemDataKey_t));
  }
  printf("Number of Remote entries = %d \n",i);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Print the current 802.1AB remote data to serial port
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpRemDataDump(void)
{
  lldpRemDataKey_t key;
  lldpRemDataEntry_t *remEntry;

  memset(&key,0,sizeof(lldpRemDataKey_t));
  printf("\n");
  printf("802.1AB Remote Data\n");
  printf("===================\n\n");

  while((remEntry=lldpRemDataEntryGetNext(&key))!=L7_NULLPTR)
  {
    L7_uchar8           *id = L7_NULLPTR;
    L7_uint32            i;

    printf("Local Interface:     %d\n", remEntry->intIntfNum);
    printf("Remote Data\n");
    printf("-------------------------------------------------\n");

    printf("Time to Live:        %i\n", remEntry->rxTTL);
    printf("Chassis Id Length:   %i\n", remEntry->chassisIdLength);
    printf("Chassis Id Subtype:  %i\n", remEntry->chassisIdSubtype);
    printf("Chassis Id:         ");
    id = remEntry->chassisId;
    for (i = 0; i < remEntry->chassisIdLength; i++)
    {
        printf("%02x:", id[i]);
    }
    printf("\n");
    printf("Port Id Length:      %i\n", remEntry->portIdLength);
    printf("Port Id Subtype:     %i\n", remEntry->portIdSubtype);
    id = remEntry->portId;
    for (i = 0; i < remEntry->portIdLength; i++)
    {
        printf("%02x:", id[i]);
    }
    printf("\n");
    printf("Port Description:    %s\n", remEntry->portDesc);
    printf("System Name:         %s\n", remEntry->sysName);
    printf("System Description:  %s\n", remEntry->sysDesc);
    printf("System Capabilities: %i %i\n",
           remEntry->sysCapSupported, remEntry->sysCapEnabled);
    printf("-------------------------------------------------\n\n");

    /* copy the entry to the key */
    memcpy(&key,remEntry,sizeof(lldpRemDataKey_t));
  }
  printf("\n");
  printf("\n=====================\n");
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Print the current 802.1AB remote data to serial port
*
* @param    L7_uint32   intIfNum   @b((input)) NIM internal interface number
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpRemDataIntfDump(L7_uint32 intIfNum)
{
  lldpRemDataEntry_t *remEntry,*prevEntry=L7_NULLPTR;

  printf("\n");
  printf("802.1AB Remote Data\n");
  printf("=====================\n\n");

  while((remEntry=lldpRemDataNeighborGetNext(intIfNum,prevEntry))!=L7_NULLPTR)
  {
    L7_uchar8             *id = L7_NULLPTR;
    L7_uint32             i;
    lldpMgmtAddrEntry_t   *entry = L7_NULLPTR;
    lldpOrgDefInfoEntry_t *orgDefEntry = L7_NULLPTR;
    lldpUnknownTLVEntry_t *unkTlv = L7_NULLPTR;

    printf("Local Interface:     %d\n", remEntry->intIntfNum);
    printf("Remote Data\n");
    printf("-------------------------------------------------\n");

    printf("Time to Live:        %i\n", remEntry->rxTTL);
    printf("TimeStamp:           %u\n", remEntry->timestamp);
    printf("remote index:        %u\n", remEntry->remIndex);
    printf("Chassis Id Length:   %i\n", remEntry->chassisIdLength);
    printf("Chassis Id Subtype:  %i\n", remEntry->chassisIdSubtype);
    printf("Chassis Id:         ");
    id = remEntry->chassisId;
    for (i = 0; i < remEntry->chassisIdLength; i++)
    {
        printf("%02x:", id[i]);
    }
    printf("\n");
    printf("Port Id Length:      %i\n", remEntry->portIdLength);
    printf("Port Id Subtype:     %i\n", remEntry->portIdSubtype);
    id = remEntry->portId;
    for (i = 0; i < remEntry->portIdLength; i++)
    {
        printf("%02x:", id[i]);
    }
    printf("\n");
    printf("Port Description:    %s\n", remEntry->portDesc);
    printf("System Name:         %s\n", remEntry->sysName);
    printf("System Description:  %s\n", remEntry->sysDesc);
    printf("System Capabilities: %i %i\n",
           remEntry->sysCapSupported, remEntry->sysCapEnabled);

    entry = remEntry->mgmtAddrList;
    while (entry != L7_NULLPTR)
    {
      switch (entry->family)
      {
        case LLDP_IANA_ADDR_FAMILY_NUMBER_IPV4:
          printf("Mgmt Address Family: IPv4\n");
          printf("Mgmt Address:        %u.%u.%u.%u\n", entry->address[0],
                 entry->address[1],
                 entry->address[2],
                 entry->address[3]);
          break;
        case LLDP_IANA_ADDR_FAMILY_NUMBER_802:
          printf("Mgmt Address Family: 802\n");
          break;
        case LLDP_IANA_ADDR_FAMILY_NUMBER_IPV6:
          printf("Mgmt Address Family: IPv6\n");
          break;
        default:
          printf("Mgmt Address Family: %u\n", entry->family);
          break;
      }
      entry = entry->next;
    }
    orgDefEntry = remEntry->orgDefInfoList;
    while (orgDefEntry != L7_NULLPTR)
    {
      printf("Org Def OUI          0x%2x %2x %2x\n", orgDefEntry->oui[0],orgDefEntry->oui[1],orgDefEntry->oui[2]);
      printf("Subtype:             0x%2x\n",orgDefEntry->subtype);
      printf("Info is:             ");
      for (i = 0; i < orgDefEntry->length; i++)
      {
        printf(" %2x", orgDefEntry->info[i]);
      }
      printf("\n");
      orgDefEntry = orgDefEntry->next;
    }
    unkTlv = remEntry->unknownTLVList;
    while (unkTlv != L7_NULLPTR)
    {
      printf("Unknown TLV Type     0x%x:\n", unkTlv->type);
      printf("Info is:             ");
      for (i = 0; i < unkTlv->length; i++)
      {
        printf(" %2x", unkTlv->info[i]);
      }
      printf("\n");
      unkTlv = unkTlv->next;
    }
    printf("-------------------------------------------------\n\n");
    prevEntry = remEntry;
  }
  printf("\n");
  printf("\n=====================\n");
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Print the current 802.1AB statistics to serial port
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpStatsDump(void)
{
  L7_uint32 i;
  nimConfigID_t cfgIdNull;

  memset(&cfgIdNull, 0, sizeof(nimConfigID_t));

  printf("\n");
  printf("802.1AB Statistics\n");
  printf("=====================\n\n");

  /* Last change time... */
  printf("Remote Table Inserts: %i\n", lldpStats->remTblInserts);
  printf("Remote Table Ageouts: %i\n", lldpStats->remTblAgeouts);
  printf("Remote Table Deletes: %i\n", lldpStats->remTblDeletes);
  printf("Remote Table Drops: %i\n", lldpStats->remTblDrops);

  for (i = 1; i < L7_LLDP_INTF_MAX_COUNT; i++)
  {
    L7_uint32 intIfNum, extIfNum;
    L7_uint32 index = 0;

    if (NIM_CONFIG_ID_IS_EQUAL(&lldpCfgData->intfCfgData[i].cfgId, &cfgIdNull))
    {
      continue;
    }
    if (nimIntIfFromConfigIDGet(&(lldpCfgData->intfCfgData[i].cfgId),
                                &intIfNum) != L7_SUCCESS)
    {
      continue;
    }
    if (lldpMapIntfIndexGet(intIfNum, &index) == L7_TRUE)
    {
      if (nimGetIntfIfIndex(intIfNum, &extIfNum) == L7_SUCCESS)
      {
        printf("Interface Statistics : %d\n", extIfNum);
        printf("-------------------------------------------------\n");
        printf("Tx Frames Total: %i\n", lldpStats->intfStats[index].txFramesTotal);
        printf("Rx Frames Total: %i\n", lldpStats->intfStats[index].rxFramesTotal);
        printf("Rx Ageouts: %i\n", lldpStats->intfStats[index].rxAgeouts);
        printf("Rx Frames Discarded: %i\n", lldpStats->intfStats[index].rxFramesDiscarded);
        printf("Rx Frames Errors: %i\n", lldpStats->intfStats[index].rxFramesErrors);
        printf("Rx TLVs Discarded: %i\n", lldpStats->intfStats[index].rxTLVsDiscarded);
        printf("Rx TLVs Unrecognized: %i\n", lldpStats->intfStats[index].rxTLVsUnrecognized);
        printf("Rx TLVs MED: %i\n", lldpStats->intfStats[index].rxTLVsMED);
        printf("Rx TLVs 8023: %i\n", lldpStats->intfStats[index].rxTLVs8023);
        printf("Rx TLVs 8021: %i\n", lldpStats->intfStats[index].rxTLVs8021);

        printf("-------------------------------------------------\n\n");
      }
    }
  }
  printf("\n");

  printf("\n=====================\n");

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Print the current 802.1AB statistics to serial port
*
* @param    L7_uint32   intIfNum   @b((input)) NIM internal interface number
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpStatsIntfDump(L7_uint32 intIfNum)
{
  L7_uint32 index = 0;
  L7_uint32 extIfNum = 0;

  printf("\n");
  printf("802.1AB Statistics\n");
  printf("=====================\n\n");

  /* Last change time... */
  printf("Remote Table Inserts: %i\n", lldpStats->remTblInserts);
  printf("Remote Table Ageouts: %i\n", lldpStats->remTblAgeouts);
  printf("Remote Table Deletes: %i\n", lldpStats->remTblDeletes);
  printf("Remote Table Drops: %i\n", lldpStats->remTblDrops);

  if (lldpMapIntfIndexGet(intIfNum, &index) == L7_TRUE)
  {
    if (nimGetIntfIfIndex(intIfNum, &extIfNum) == L7_SUCCESS)
    {
      printf("Interface Statistics : %d\n", extIfNum);
      printf("-------------------------------------------------\n");
      printf("Tx Frames Total: %i\n", lldpStats->intfStats[index].txFramesTotal);
      printf("Rx Frames Total: %i\n", lldpStats->intfStats[index].rxFramesTotal);
      printf("Rx Ageouts: %i\n", lldpStats->intfStats[index].rxAgeouts);
      printf("Rx Frames Discarded: %i\n", lldpStats->intfStats[index].rxFramesDiscarded);
      printf("Rx Frames Errors: %i\n", lldpStats->intfStats[index].rxFramesErrors);
      printf("Rx TLVs Discarded: %i\n", lldpStats->intfStats[index].rxTLVsDiscarded);
      printf("Rx TLVs Unrecognized: %i\n", lldpStats->intfStats[index].rxTLVsUnrecognized);
      printf("Rx TLVs MED: %i\n", lldpStats->intfStats[index].rxTLVsMED);
      printf("Rx TLVs 8023: %i\n", lldpStats->intfStats[index].rxTLVs8023);
      printf("Rx TLVs 8021: %i\n", lldpStats->intfStats[index].rxTLVs8021);

      printf("-------------------------------------------------\n\n");
    }
  }

  printf("\n");
  printf("\n=====================\n");

  return L7_SUCCESS;
}

void lldpMedDebugTxSet(L7_uint32 intIfNum)
{
  L7_uint32 index;

  if (lldpMapIntfIndexGet(intIfNum, &index) == L7_TRUE)
  {
    if (lldpIntfTbl[index].medTransmitEnabled == 0)
    {
      lldpIntfTbl[index].medTransmitEnabled = 1;
      lldpIntfTbl[index].medFastStart = lldpCfgData->fastStartRepeatCount;
      printf("lldp-MED Tx enabled interface %d fast start set to %d\n",  intIfNum, lldpIntfTbl[index].medFastStart);
    }
    else
    {
      lldpIntfTbl[index].medTransmitEnabled = 0;
      lldpIntfTbl[index].medFastStart = 0;
      printf("lldp-MED Tx disabled interface %d fast start set to 0\n",  intIfNum);
    }
  }
  return;
}
void lldpMedDebugTxFlagSet()
{
  if (lldpMedDebugTxFlag == L7_FALSE)
  {
    lldpMedDebugTxFlag = L7_TRUE;
    printf("LLDP-MED Debug Tx Flag Set\n");
  }
  else
  {
    lldpMedDebugTxFlag = L7_FALSE;
    printf("LLDP-MED Debug Tx Flag Reset\n");
  }
  return;
}

/*********************************************************************
* @purpose  Validate a received MED TLV
*
* @param    lldpPDUHandle_t       *pduHdl    @b((input))  pointer to LLDP PDU type.
* @param    L7_uint32             length     @b((input))  length of information string.
*
* @returns  L7_SUCCESS, TLV is valid
* @returns  L7_FAILURE, Invalid TLV
*
* @notes    This routine will only validate MED TLVs all other OUIs TLVs will return a success.
*
* @end
*********************************************************************/
L7_RC_t lldpMedTLVValidate(lldpRemDataEntry_t *entry,
                           L7_uint32           index,
                           lldpPDUHandle_t    *pduHdl,
                           L7_uint32           length)
{
  L7_uint32 offset;
  lldpMedTLVSubtype_t subType;
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 extPwrOffset, pwrDevice;

  /*Perform Validation only for MED TLV */
  if (memcmp(&pduHdl->buffer[pduHdl->offset], LLDP_MED_TIA_OUI_STRING, LLDP_TLV_ORG_DEF_INFO_OUI_SIZE_MAX) != 0)
  {
    /*this is not a MED TLV so return SUCCESS */
    return L7_SUCCESS;
  }
  /* This is an MED TLV */
  if (lldpCfgData->intfCfgData[index].rxEnabled != L7_TRUE ||
      lldpCfgData->intfCfgData[index].txEnabled != L7_TRUE)
  {
    /* We should not process any MED TLVs if tx or rx is disabled*/
    return  L7_FAILURE;
  }

  /* Keep a local copy to navigate insde the pdu */
  offset = pduHdl->offset;

  /* The offset now points to the 3 byte oui advance the offset to subtype */
  offset += LLDP_TLV_ORG_DEF_INFO_OUI_SIZE_MAX;

  subType = (L7_uint32) pduHdl->buffer[offset];
  extPwrOffset = offset;

  if (lldpMedTlvCapRx == L7_TRUE && subType == LLDP_MED_SUBTYPE_CAP)
  {
    /* This is a duplicate cap tlvs discard this tlv */
    return L7_FAILURE;
  }
  if (lldpMedTlvCapRx == L7_FALSE && subType != LLDP_MED_SUBTYPE_CAP)
  {
    /* We have not yet received a med cap tlv, discard this tlv
     * bump up the counters
     */
    return L7_FAILURE;
  }
  /* If lldpMedTlvCapRx is false then the first MED frame has to be
   * a med capabilities TLV
   */
  if (lldpMedTlvCapRx == L7_FALSE && subType == LLDP_MED_SUBTYPE_CAP)
  {
    lldpMedTlvCapRx = L7_TRUE;
  }
  /* The case lldpMedTlvCapRx == L7_TRUE && subType != LLDP_MED_SUBTYPE_CAP
   * is a valid one
   */

  /* Based on the sub type individual rules have to be checked */
  switch (subType)
  {
    case LLDP_MED_SUBTYPE_CAP:
      if (!(entry->medCapCurrent[1] & LLDP_MED_CAP_CAPABILITIES_BITMASK))
      {
        entry->medCapCurrent[1] |= LLDP_MED_CAP_CAPABILITIES_BITMASK;
      }
      /* No specific validation */
      rc = L7_SUCCESS;
      break;
    case LLDP_MED_SUBTYPE_NET_POLICY:
      if (!(entry->medCapCurrent[1] & LLDP_MED_CAP_NETWORKPOLICY_BITMASK))
      {
        entry->medCapCurrent[1] |= LLDP_MED_CAP_NETWORKPOLICY_BITMASK;
      }
      /* No specific validation */
      rc = L7_SUCCESS;
      break;
    case LLDP_MED_SUBTYPE_LOCATION:
      if (!(entry->medCapCurrent[1] & LLDP_MED_CAP_LOCATION_BITMASK))
      {
        entry->medCapCurrent[1] |= LLDP_MED_CAP_LOCATION_BITMASK;
      }
      /* No specific validation */
      rc = L7_SUCCESS;
      break;

    case LLDP_MED_SUBTYPE_EXT_POWER:
      if (lldpMedTlvExtPwrRx == L7_FALSE)
      {
        lldpMedTlvExtPwrRx = L7_TRUE;

        extPwrOffset++;
        pwrDevice = (L7_uint32) (pduHdl->buffer[extPwrOffset] & LLDP_MED_EXT_POWER_TYPE);

        if (!(entry->medCapCurrent[1] & LLDP_MED_CAP_EXT_PSE_BITMASK) && pwrDevice == POWER_DEVICE_TYPE_PSE)
        {
          entry->medCapCurrent[1] |= LLDP_MED_CAP_EXT_PSE_BITMASK;
        }
        else if (!(entry->medCapCurrent[1] & LLDP_MED_CAP_EXT_PD_BITMASK) && pwrDevice == POWER_DEVICE_TYPE_PD)
        {
          entry->medCapCurrent[1] |= LLDP_MED_CAP_EXT_PD_BITMASK;
        }
        rc = L7_SUCCESS;
      }
      else
      {
        /* Duplicate tlv discard */
        rc = L7_FAILURE;
      }
      break;
    case LLDP_MED_SUBTYPE_INVENTORY_HW:
      if (lldpMedTlvHwRx == L7_FALSE)
      {
        lldpMedTlvHwRx = L7_TRUE;
        if (!(entry->medCapCurrent[1] & LLDP_MED_CAP_INVENTORY_BITMASK))
        {
          entry->medCapCurrent[1] |= LLDP_MED_CAP_INVENTORY_BITMASK;
        }
        rc = L7_SUCCESS;
      }
      else
      {
        /* Duplicate tlv discard */
        rc = L7_FAILURE;
      }
      break;
    case LLDP_MED_SUBTYPE_INVENTORY_FW:
      if (lldpMedTlvFwRx == L7_FALSE)
      {
        lldpMedTlvFwRx = L7_TRUE;
        if (!(entry->medCapCurrent[1] & LLDP_MED_CAP_INVENTORY_BITMASK))
        {
          entry->medCapCurrent[1] |= LLDP_MED_CAP_INVENTORY_BITMASK;
        }
        rc = L7_SUCCESS;
      }
      else
      {
        /* Duplicate tlv discard */
        rc = L7_FAILURE;
      }
      break;
    case LLDP_MED_SUBTYPE_INVENTORY_SW:
      if (lldpMedTlvSwRx == L7_FALSE)
      {
        lldpMedTlvSwRx = L7_TRUE;
        if (!(entry->medCapCurrent[1] & LLDP_MED_CAP_INVENTORY_BITMASK))
        {
          entry->medCapCurrent[1] |= LLDP_MED_CAP_INVENTORY_BITMASK;
        }
        rc = L7_SUCCESS;
      }
      else
      {
        /* Duplicate tlv discard */
        rc = L7_FAILURE;
      }
      break;
    case LLDP_MED_SUBTYPE_INVENTORY_SNO:
      if (lldpMedTlvSnoRx == L7_FALSE)
      {
        lldpMedTlvSnoRx = L7_TRUE;
        if (!(entry->medCapCurrent[1] & LLDP_MED_CAP_INVENTORY_BITMASK))
        {
          entry->medCapCurrent[1] |= LLDP_MED_CAP_INVENTORY_BITMASK;
        }
        rc = L7_SUCCESS;
      }
      else
      {
        /* Duplicate tlv discard */
        rc = L7_FAILURE;
      }
      break;
    case LLDP_MED_SUBTYPE_INVENTORY_MFG_NAME:
      if (lldpMedTlvMfgRx == L7_FALSE)
      {
        lldpMedTlvMfgRx = L7_TRUE;
        if (!(entry->medCapCurrent[1] & LLDP_MED_CAP_INVENTORY_BITMASK))
        {
          entry->medCapCurrent[1] |= LLDP_MED_CAP_INVENTORY_BITMASK;
        }
        rc = L7_SUCCESS;
      }
      else
      {
        /* Duplicate tlv discard */
        rc = L7_FAILURE;
      }
      break;
    case LLDP_MED_SUBTYPE_INVENTORY_MODEL_NAME:
      if (lldpMedTlvMdlRx == L7_FALSE)
      {
        lldpMedTlvMdlRx = L7_TRUE;
        if (!(entry->medCapCurrent[1] & LLDP_MED_CAP_INVENTORY_BITMASK))
        {
          entry->medCapCurrent[1] |= LLDP_MED_CAP_INVENTORY_BITMASK;
        }
        rc = L7_SUCCESS;
      }
      else
      {
        /* Duplicate tlv discard */
        rc = L7_FAILURE;
      }
      break;
    case LLDP_MED_SUBTYPE_INVENTORY_ASSET_ID:
      if (lldpMedTlvAssetRx == L7_FALSE)
      {
        lldpMedTlvAssetRx = L7_TRUE;
        if (!(entry->medCapCurrent[1] & LLDP_MED_CAP_INVENTORY_BITMASK))
        {
          entry->medCapCurrent[1] |= LLDP_MED_CAP_INVENTORY_BITMASK;
        }
        rc = L7_SUCCESS;
      }
      else
      {
        /* Duplicate tlv discard */
        rc = L7_FAILURE;
      }
      break;
    default:
      /* The above are the valid MED subtypes any reserved ones are discarded */
      rc = L7_FAILURE;
      break;
  }

  return rc;
}

/*********************************************************************
* @purpose  Check If a Voice device is about to be removed, if so send out a notification
*
* @param    L7_uint32             index     @b((input))  the interface index
*
* @returns  L7_SUCCESS, TLV is valid
* @returns  L7_FAILURE, Invalid TLV
*
* @notes
*
* @end
*********************************************************************/
void lldpMedRemDeviceRemoveCheck(lldpRemDataEntry_t *remEntry)
{
  lldpXMedNotify_t notifyMsg;
  lldpMedCapTLV_t info;
  L7_uint32 index;
  lldp8023PowerMdiTLV_t powerMdiTlvInfo;

  memset(&info,       0x00,  sizeof(lldpMedCapTLV_t));
  memset(&notifyMsg,  0x00,  sizeof(lldpXMedNotify_t));
  memset(&powerMdiTlvInfo, 0x00, sizeof(lldp8023PowerMdiTLV_t));

  if (lldpOrgDefTLVGet(remEntry,
                       (L7_uchar8*)LLDP_MED_TIA_OUI_STRING,
                       LLDP_MED_SUBTYPE_CAP,
                       (L7_uchar8*)&info,
                       sizeof(lldpMedCapTLV_t)) == L7_SUCCESS)
  {
    if ((L7_uint32)info.deviceType[0] == endpointClass3)
    {
      /* We do see a previously detected voice device endpoint class III
       * send out a REMOVE notification
       */
      LLDP_TRACE(LLDP_DEBUG_MED, "Previously detected MED device type: %d, send Remove", info.deviceType[0]);
      notifyMsg.evData.med.deviceType = (L7_uint32)info.deviceType[0];
      notifyMsg.event = LLDP_MED_REMOVE_EVENT;
      memcpy(notifyMsg.evData.med.mac, remEntry->srcMac, L7_MAC_ADDR_LEN);
      lldpXMedNotifyRegisteredUsers(remEntry->intIntfNum, &notifyMsg);
    }

    /* Send out the topology change trap */
    if (lldpMapIntfIndexGet(remEntry->intIntfNum,&index)==L7_TRUE &&
        lldpCfgData->intfCfgData[index].medNotifyEnabled == L7_ENABLE)
    {
      (void)trapMgrLldpXMedTopologyChangeDetectedTrap((L7_uint32)remEntry->chassisIdSubtype,
                                                      remEntry->chassisId,
                                                      (L7_uint32)remEntry->chassisIdLength,
                                                      (L7_uint32)info.deviceType[0]);
    }
  }

  if ((lldpOrgDefTLVGet(remEntry,





                        (L7_uchar8*)LLDP_8023_TIA_OUI_STRING,
                        LLDP_8023_SUBTYPE_POWER_MDI,
                        (L7_uchar8*)&powerMdiTlvInfo,
                        sizeof(lldp8023PowerMdiTLV_t)) == L7_SUCCESS))
  {
    LLDP_TRACE(LLDP_DEBUG_POE_MED, "Previously detected POE device on Interface 0x%x  send Remove", notifyMsg.event);
    notifyMsg.event = LLDP_POE_REMOVE_EVENT;

    lldpXMedNotifyRegisteredUsers(remEntry->intIntfNum, &notifyMsg);
  }
  return;
}

#ifdef L7_POE_PACKAGE
void lldpDebugDumpIntfOperData(L7_uint32 ifNum)
{
  L7_uint32 index;
  if (lldpMapIntfIndexGet(ifNum, &index) == L7_TRUE)
  {
    sysapiPrintf("LLDP INTERFACE OPERATIONAL DATA DUMP FOR INDEX=%d\n",index);
    sysapiPrintf("\n=====================\n");
    sysapiPrintf("INTF NUM       :%8x\n",lldpIntfTbl[index].intIfNum);
    sysapiPrintf("INTF ACTIVE RX :%8x\n",lldpIntfTbl[index].rxActive);
    sysapiPrintf("INTF ACTIVE TX :%8x\n",lldpIntfTbl[index].txActive);
    sysapiPrintf("INTF RX FRAME  :%8x\n",lldpIntfTbl[index].rxFrame);
    sysapiPrintf("INTF TX TTR    :%8x\n",lldpIntfTbl[index].txTTR);
    sysapiPrintf("INTF TX DELAY  :%8x\n",lldpIntfTbl[index].txDelayWhile);
    sysapiPrintf("INTF TX RINIT  :%8x\n",lldpIntfTbl[index].txReinitDelay);
    sysapiPrintf("INTF MED TX ENA:%8x\n",lldpIntfTbl[index].medTransmitEnabled);
    sysapiPrintf("INTF MED FSTART:%8x\n",lldpIntfTbl[index].medFastStart);
    return;
  }
  sysapiPrintf(" INVALID LLDP INTERFACE\n");
}

#endif
