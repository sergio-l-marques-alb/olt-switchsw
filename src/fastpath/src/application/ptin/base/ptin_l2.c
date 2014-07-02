#include "ptin_l2.h"
#include "fdb_api.h"
#include "ptin_intf.h"
#include "ptin_evc.h"
#include "nimapi.h"
#include "logger.h"
#include "usmdb_mib_bridge_api.h"

static L7_uint32              mac_table_entries=0;
static ptin_switch_mac_entry  mac_table[PLAT_MAX_FDB_MAC_ENTRIES];

/**
 * Get aging time
 * 
 * @param age : aging time in seconds (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_l2_mac_aging_get(L7_uint32 *age)
{
  /* Validate arguments */
  if (age==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_L2,"Null pointer");
    return L7_FAILURE;
  }

  /* Get aging time */
  if (usmDbDot1dTpAgingTimeGet(1, age)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_L2,"Error reading MAC aging");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Set aging time
 * 
 * @param age : aging time in seconds
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_l2_mac_aging_set(L7_uint32 age)
{
  /* Apply aging time */
  if (usmDbFDBAddressAgingTimeoutSet(1, FD_SIM_DEFAULT_MGMT_VLAN_ID, age)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_L2,"Error applying MAC aging of %u seconds",age);
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_L2,"Success applying MAC aging of %u seconds",age);

  return L7_SUCCESS;
}

/**
 * Load all MAC entries to a RAM table
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_l2_mac_table_load(void)
{
  L7_uchar8         keyNext[L7_FDB_KEY_SIZE];
  dot1dTpFdbData_t  fdbEntry;
  L7_uint32         index, i;
  ptin_intf_t       ptin_intf;
  L7_uint16         vlan;
  L7_INTF_TYPES_t   intfType;
  L7_RC_t           rc = L7_SUCCESS;
  L7_uint32         evc_ext_id;
  IfN_vp_entry_t    e;

  LOG_TRACE(LOG_CTX_PTIN_L2, "Loading MAC table...");

  memset( keyNext, 0x00, sizeof(L7_uchar8)*L7_FDB_KEY_SIZE );

  for (i=0, index=0; i<PLAT_MAX_FDB_MAC_ENTRIES && fdbFind(keyNext, L7_MATCH_GETNEXT, &fdbEntry)==L7_SUCCESS; i++)
  {
    memcpy(keyNext, fdbEntry.dot1dTpFdbAddress, L7_FDB_KEY_SIZE);

    /* Get interface type */
    if (nimGetIntfType(fdbEntry.dot1dTpFdbPort,&intfType)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_L2,"Error getting interface type of intIfNum=%u",fdbEntry.dot1dTpFdbPort);
      rc = L7_FAILURE;
      continue;
    }

    /* Only accept physical and logical interfaces */
    if (intfType!=L7_PHYSICAL_INTF && intfType!=L7_LAG_INTF && intfType!=L7_VLAN_PORT_INTF)
    {
      LOG_WARNING(LOG_CTX_PTIN_L2,"Invalid interface type (%u) of intIfNum=%u",intfType,fdbEntry.dot1dTpFdbPort);
      continue;
    }

    /* Convert to ptin interface format */
    if (intfType==L7_VLAN_PORT_INTF) {
        e.intIfNum_vport=   fdbEntry.dot1dTpFdbPort;
        if (IfN_vp_DB(3, &e)) {
            LOG_WARNING(LOG_CTX_PTIN_L2,"PON&GEMid for intIfNum=%lu not found",fdbEntry.dot1dTpFdbPort);
            continue;
        }
        else LOG_TRACE(LOG_CTX_PTIN_L2,"intIfNum_vport=%lu PON=%u/%u GEMid=%u",fdbEntry.dot1dTpFdbPort, e.pon.intf_type, e.pon.intf_id, e.gem_id);
        ptin_intf=  e.pon;
                    //=e.gem_id;
    }
    else
    if (ptin_intf_intIfNum2ptintf(fdbEntry.dot1dTpFdbPort,&ptin_intf)!=L7_SUCCESS)
    {
      LOG_WARNING(LOG_CTX_PTIN_L2,"Invalid intIfNum=%u",fdbEntry.dot1dTpFdbPort);
      continue;
    }

    // Extract vlan and validate it
    vlan = osapiNtohs((L7_uint16) *((L7_uint16 *) &fdbEntry.dot1dTpFdbAddress[0]));
    if (vlan>4095) {
      LOG_ERR(LOG_CTX_PTIN_L2, "Invalid vlanid (%u) on index %u",vlan,index);
      rc = L7_FAILURE;
      continue;
    }

    evc_ext_id = -1;
    ptin_evc_get_evcIdfromIntVlan(vlan, &evc_ext_id);

    // Fill mac-table entry
    mac_table[index].entryId      = index;
    mac_table[index].evcId        = evc_ext_id;
    mac_table[index].vlanId       = vlan;
    mac_table[index].intf         = ptin_intf;
    mac_table[index].gem_id       = L7_VLAN_PORT_INTF==intfType?    e.gem_id:   -1;
    mac_table[index].static_entry = (fdbEntry.dot1dTpFdbEntryType==L7_FDB_ADDR_FLAG_STATIC);
    memcpy(mac_table[index].addr, &fdbEntry.dot1dTpFdbAddress[L7_FDB_IVL_ID_LEN], sizeof(L7_uchar8)*L7_FDB_MAC_ADDR_LEN);

    index++;
  }

  /* Update total number of entries */
  mac_table_entries = index;
  LOG_TRACE(LOG_CTX_PTIN_L2, "%u MAC entries loaded.",index);

  return rc;
}

/**
 * Get MAC entries loaded with ptin_l2_mac_table_load
 * 
 * @param startId : starting entry id
 * @param numEntries : maximum number of entries to be read 
 *                   (input/output)
 * @param entries : pointer to a list of entries (output) 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_l2_mac_table_get( L7_uint32 startId, L7_uint32 *numEntries, ptin_switch_mac_entry **entries )
{
  /* Validate arguments */
  if (startId>=PLAT_MAX_FDB_MAC_ENTRIES)
  {
    LOG_ERR(LOG_CTX_PTIN_L2,"startId %u out of range [0,%u[",startId,PLAT_MAX_FDB_MAC_ENTRIES);
    return L7_FAILURE;
  }
  if (numEntries==L7_NULLPTR || entries==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_L2,"Null pointer");
    return L7_FAILURE;
  }

  /* Check if startId is inside valid entries */
  if (startId >= mac_table_entries)
  {
    LOG_WARNING(LOG_CTX_PTIN_L2,"startId %u higher than numElems=%u",startId,mac_table_entries);
    *numEntries = 0;
    *entries    = L7_NULLPTR;
    return L7_SUCCESS;
  }

  /* Majorate number of entries to be returned */
  if ((startId+(*numEntries)) > mac_table_entries)
  {
    *numEntries = mac_table_entries - startId;
  }

  /* Pointer to the first element */
  *entries = &mac_table[startId];

  LOG_TRACE(LOG_CTX_PTIN_L2, "%u MAC entries retrieved (starting from startId=%u)",*numEntries,startId);

  return L7_SUCCESS;
}


/**
 * Flush the MAC table entries
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_l2_mac_table_flush( void )
{
  L7_uint32 i;
  L7_RC_t   rc = L7_SUCCESS;

  LOG_WARNING(LOG_CTX_PTIN_L2,"MAC table will be cleared!");

  /* Reload MAC table */
  if (ptin_l2_mac_table_load()==L7_SUCCESS)
  {
    LOG_TRACE(LOG_CTX_PTIN_L2,"MAC table reloaded");

    /* Remove all entries */
    for (i=0; i<mac_table_entries; i++)
    {
      if (ptin_l2_mac_table_entry_remove(&mac_table[i])!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_L2,"Error clearing MAC entry of index %u",i);
        rc = L7_FAILURE;
      }
    }
    if (rc!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_L2,"An error ocurred while clearing MAC entries");
    }
    else
    {
      LOG_TRACE(LOG_CTX_PTIN_L2,"Success clearing all MAC entries");
    }
  }
  else
  {
    LOG_ERR(LOG_CTX_PTIN_L2,"An error ocurred while reloading MAC table");
    return L7_FAILURE;
  }

  /* Flush all dynamic entries */
  if (fdbFlush()!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_L2,"An error ocurred while flushing all dynamic MAC entries");
    rc = L7_FAILURE;
  }
  else
  {
    LOG_TRACE(LOG_CTX_PTIN_L2,"Success flushing all dynamic MAC entries");
  }

  /* Reload MAC table */
  if (ptin_l2_mac_table_load()!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_L2,"An error ocurred while reloading MAC table");
    rc = L7_FAILURE;
  }
  else
  {
    LOG_TRACE(LOG_CTX_PTIN_L2,"Success reloading MAC table");
  }

  return rc;
}


/**
 * Remove a MAC entry
 * 
 * @param entry : entry to be removed
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_l2_mac_table_entry_remove( ptin_switch_mac_entry *entry )
{
  L7_uint16             vlanId;
  L7_uchar8             keyToFind[L7_FDB_KEY_SIZE];
  dot1dTpFdbData_t      fdbEntry;
  fdbMeberInfo_t        fdbMemberInfo;

  /* Check if there are items to be removed */
  if ( entry==L7_NULLPTR )
  {
    LOG_WARNING(LOG_CTX_PTIN_L2,"No items to be removed");
    return L7_SUCCESS;
  }

  LOG_TRACE(LOG_CTX_PTIN_L2,"Entry to be removed:");
  LOG_TRACE(LOG_CTX_PTIN_L2," EVCid  = %u",entry->evcId);
  LOG_TRACE(LOG_CTX_PTIN_L2," Vlan   = %u",entry->vlanId);
  LOG_TRACE(LOG_CTX_PTIN_L2," MacAddr=%02X:%02X:%02X:%02X:%02X:%02X",
            entry->addr[0],entry->addr[1],entry->addr[2],entry->addr[3],entry->addr[4],entry->addr[5]);
  LOG_TRACE(LOG_CTX_PTIN_L2," Intf   = %u/%u",entry->intf.intf_type,entry->intf.intf_id);
  LOG_TRACE(LOG_CTX_PTIN_L2," Static = %u",entry->static_entry);

  /* If vlan is not given */
  if (entry->vlanId>4095)
  {
    /* TODO: EVC id should be 32 bits instead of 16! */
//  /* Check if EVC id is valid */
//  if (entry->evcId>=PTIN_SYSTEM_N_EXTENDED_EVCS)
//  {
//    LOG_ERR(LOG_CTX_PTIN_L2,"Vlan %u and evcId %u are invalid",entry->vlanId,entry->evcId);
//    return L7_FAILURE;
//  }

    /* Get root vlan */
    if (ptin_evc_intRootVlan_get(entry->evcId,&vlanId)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_L2,"Can't get root vlan for evcId %u",entry->evcId);
      return L7_FAILURE;
    }

    /* Validate vlan */
    if (vlanId<PTIN_VLAN_MIN || vlanId>PTIN_VLAN_MAX)
    {
      LOG_ERR(LOG_CTX_PTIN_L2,"Vlan %u (extracted from evcId %u) is invalid",vlanId,entry->evcId);
      return L7_FAILURE;
    }
  }
  else
  {
    vlanId = entry->vlanId;
    LOG_TRACE(LOG_CTX_PTIN_L2,"Vlan to be used will be the provided one: %u",vlanId);
  }

  LOG_TRACE(LOG_CTX_PTIN_L2,"Entry of Vlan=%u and MAC=%02x:%02x:%02x:%02x:%02x:%02x will be removed",
            vlanId,entry->addr[0],entry->addr[1],entry->addr[2],entry->addr[3],entry->addr[4],entry->addr[5]);

  /* Vlan+MAC to search for */
  memcpy(&keyToFind[0],&vlanId,sizeof(L7_uint16));
  memcpy(&keyToFind[L7_FDB_IVL_ID_LEN],entry->addr,sizeof(L7_uint8)*L7_FDB_MAC_ADDR_LEN);

  /* Search for this key: if not found, return success */
  if (fdbFind(keyToFind, L7_MATCH_EXACT, &fdbEntry)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_L2,"Entry of Vlan=%u and MAC=%02x:%02x:%02x:%02x:%02x:%02x not found",
            vlanId,entry->addr[0],entry->addr[1],entry->addr[2],entry->addr[3],entry->addr[4],entry->addr[5]);
    return L7_SUCCESS;
  }

  /* Build structure to remove entry */
  fdbMemberInfo.vlanId = vlanId;
  memcpy(fdbMemberInfo.macAddr, entry->addr, sizeof(L7_uint8)*L7_MAC_ADDR_LEN);
  fdbMemberInfo.intIfNum   = fdbEntry.dot1dTpFdbPort;
  fdbMemberInfo.entryType  = fdbEntry.dot1dTpFdbEntryType;

  /* Remove Entry */
  if (fdbDelEntry(&fdbMemberInfo)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_L2,"Error removing entry of Vlan=%u and MAC=%02x:%02x:%02x:%02x:%02x:%02x",
            vlanId,entry->addr[0],entry->addr[1],entry->addr[2],entry->addr[3],entry->addr[4],entry->addr[5]);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}


/**
 * Add a MAC entry
 * 
 * @param entry : entry to be added
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_l2_mac_table_entry_add( ptin_switch_mac_entry *entry )
{
  L7_uint32             intIfNum;
  L7_uint16             vlanId;
  L7_uchar8             keyToFind[L7_FDB_KEY_SIZE];
  dot1dTpFdbData_t      fdbEntry;
  fdbMeberInfo_t        fdbMemberInfo;

  /* Check if there are items to be removed */
  if ( entry==L7_NULLPTR )
  {
    LOG_WARNING(LOG_CTX_PTIN_L2,"No items to be removed");
    return L7_SUCCESS;
  }

  LOG_TRACE(LOG_CTX_PTIN_L2,"Entry to be added:");
  LOG_TRACE(LOG_CTX_PTIN_L2," EVCid  = %u",entry->evcId);
  LOG_TRACE(LOG_CTX_PTIN_L2," Vlan   = %u",entry->vlanId);
  LOG_TRACE(LOG_CTX_PTIN_L2," MacAddr=%02X:%02X:%02X:%02X:%02X:%02X",
            entry->addr[0],entry->addr[1],entry->addr[2],entry->addr[3],entry->addr[4],entry->addr[5]);
  LOG_TRACE(LOG_CTX_PTIN_L2," Intf   = %u/%u",entry->intf.intf_type,entry->intf.intf_id);
  LOG_TRACE(LOG_CTX_PTIN_L2," Static = %u",entry->static_entry);

  /* If vlan is not given */
  if (entry->vlanId>4095)
  {
    /* TODO: EVC id should be 32 bits instead of 16! */
//  /* Check if EVC id is valid */
//  if (entry->evcId>=PTIN_SYSTEM_N_EXTENDED_EVCS)
//  {
//    LOG_ERR(LOG_CTX_PTIN_L2,"Vlan %u and evcId %u are invalid",entry->vlanId,entry->evcId);
//    return L7_FAILURE;
//  }

    /* Get root vlan */
    if (ptin_evc_intRootVlan_get(entry->evcId,&vlanId)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_L2,"Can't get root vlan for evcId %u",entry->evcId);
      return L7_FAILURE;
    }

    /* Validate vlan */
    if (vlanId<PTIN_VLAN_MIN || vlanId>PTIN_VLAN_MAX)
    {
      LOG_ERR(LOG_CTX_PTIN_L2,"Vlan %u (extracted from evcId %u) is invalid",vlanId,entry->evcId);
      return L7_FAILURE;
    }
  }
  else
  {
    vlanId = entry->vlanId;
    LOG_TRACE(LOG_CTX_PTIN_L2,"Vlan to be used will be the provided one: %u",vlanId);
  }

  /* Calculate intIfNum */
  if (ptin_intf_ptintf2intIfNum(&entry->intf,&intIfNum)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_L2,"Interface %u/%u is invalid",entry->intf.intf_type,entry->intf.intf_id);
    return L7_FAILURE;
  }

  /* Check if this Vlan+MAC already exists */
  memcpy(&keyToFind[0],&vlanId,sizeof(L7_uint16));
  memcpy(&keyToFind[L7_FDB_IVL_ID_LEN],entry->addr,sizeof(L7_uint8)*L7_FDB_MAC_ADDR_LEN);

  /* Search for this key: if not found, return success */
  if (fdbFind(keyToFind, L7_MATCH_EXACT, &fdbEntry)==L7_SUCCESS)
  {
    LOG_WARNING(LOG_CTX_PTIN_L2,"Entry of Vlan=%u and MAC=%02x:%02x:%02x:%02x:%02x:%02x exists (attached to intIfNum=%u)",
            vlanId,entry->addr[0],entry->addr[1],entry->addr[2],entry->addr[3],entry->addr[4],entry->addr[5],fdbEntry.dot1dTpFdbPort);

    /* If interface and entry type matches, there is nothing to be done */
    if (fdbEntry.dot1dTpFdbPort==intIfNum &&
        (fdbEntry.dot1dTpFdbEntryType==L7_FDB_ADDR_FLAG_STATIC)==(entry->static_entry!=0))
    {
      LOG_WARNING(LOG_CTX_PTIN_L2,"Found entry matches the one to be added... nothing to be done.");
      return L7_SUCCESS;
    }
    /* Otherwise, delete it to add it again */
    else
    {
      /* Build structure to remove entry */
      fdbMemberInfo.vlanId = vlanId;
      memcpy(fdbMemberInfo.macAddr, entry->addr, sizeof(L7_uint8)*L7_MAC_ADDR_LEN);
      fdbMemberInfo.intIfNum   = fdbEntry.dot1dTpFdbPort;
      fdbMemberInfo.entryType  = fdbEntry.dot1dTpFdbEntryType;

      /* Remove existent entry */
      if (fdbDelEntry(&fdbMemberInfo)!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_L2,"Error removing entry of Vlan=%u and MAC=%02x:%02x:%02x:%02x:%02x:%02x",
                vlanId,entry->addr[0],entry->addr[1],entry->addr[2],entry->addr[3],entry->addr[4],entry->addr[5]);
        return L7_FAILURE;
      }
    }
  }

  LOG_TRACE(LOG_CTX_PTIN_L2,"Entry of Vlan=%u and MAC=%02x:%02x:%02x:%02x:%02x:%02x (%s) will be added (attached to interface %u/%u)",
            vlanId,entry->addr[0],entry->addr[1],entry->addr[2],entry->addr[3],entry->addr[4],entry->addr[5],
            ((entry->static_entry) ? "Static" : "Dynamic"),
            entry->intf.intf_type,entry->intf.intf_id);

  /* Build structure to remove entry */
  fdbMemberInfo.vlanId = vlanId;
  memcpy(fdbMemberInfo.macAddr, entry->addr, sizeof(L7_uint8)*L7_MAC_ADDR_LEN);
  fdbMemberInfo.intIfNum   = intIfNum;
  fdbMemberInfo.entryType  = (entry->static_entry) ? L7_FDB_ADDR_FLAG_STATIC : L7_FDB_ADDR_FLAG_LEARNED;

  /* Add Entry */
  if (fdbAddEntry(&fdbMemberInfo)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_L2,"Error adding entry of Vlan=%u and MAC=%02x:%02x:%02x:%02x:%02x:%02x to intf=%u/%u",
            vlanId,entry->addr[0],entry->addr[1],entry->addr[2],entry->addr[3],entry->addr[4],entry->addr[5],
            entry->intf.intf_type,entry->intf.intf_id);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

