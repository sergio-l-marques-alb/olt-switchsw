/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  hpc_util.c
*
* @purpose   Provide internal utility functions for the hpc component
*
* @component sysapi
*
* @create    02/12/2003
*
* @author    jeffr
* @end
*
*********************************************************************/

#include <string.h>
#include "l7_common.h"
#include "log.h"
#include "osapi.h"
#include "registry.h"
#include "osapi_support.h"
#include "sysapi.h"
#include "sysapi_hpc.h"
#include "buff_api.h"
#include "hpc.h"
#include "hpc_hw_api.h"
#include "nvstoreapi.h"
#include "unitmgr_api.h"
#include "l7_cnfgr_api.h" /* needed for the cnfgrApiComponentNameGet */

extern HPC_UNIT_DESCRIPTOR_t hpc_unit_descriptor_db[];
extern HPC_CARD_DESCRIPTOR_t hpc_card_descriptor_db[];

static hpcMessageReceiveNotifyList_t hpcMessageReceiveNotifyList[L7_LAST_COMPONENT_ID];
static hpcStackEventNotifyList_t     hpcStackEventNotifyList[HPC_EVENT_LAST][L7_LAST_COMPONENT_ID];

static hpcCfgData_t hpc_config_data;
static hpcOperationalData_t hpc_oper_data;

/* local function prototypes */
void hpcTransportMessageRecvHandler(L7_enetMacAddr_t src_key,
                                    L7_uint32 receive_id,
                                    L7_uchar8* buffer, L7_uint32 msg_len);
void hpcStackEventCallback(hpcStackEventMsg_t stack_event);
L7_RC_t hpcCfgDataRetrieve(void);
void hpcCfgDataSetDefault(void);
L7_RC_t hpcCfgDataRestore(void);
L7_RC_t hpcCfgDataApply(hpcCfgData_t *cfg_data);
void hpcDebugRemoteDevshellInit(void);
L7_RC_t hpcDriverAsfInit();
extern void hapiBroadSocFileLoad(char *file_name, L7_BOOL suppressFileNotAvail);


/**************************************************************************
*
* @purpose  Perform any needed runtime system unit and card descriptor database
*           initialization.
*
* @param    none
*
* @returns  L7_SUCCESS  operation completed with no problems
* @returns  L7_FAILURE  an error occurred while running the DAPI
*                       data initializer functions
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t hpcDescriptorDbInit(void)
{
  return(L7_SUCCESS);
}

/**************************************************************************
*
* @purpose  Initialize the HPC system service.
*
* @param    none
*
* @returns  L7_SUCCESS  no problems in initialization
* @returns  L7_ERROR    encountered error in initialization
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t hpcInit(void)
{

  bzero((L7_uchar8 *)&hpc_config_data, sizeof(hpcCfgData_t));
  bzero((L7_uchar8 *)&hpc_oper_data, sizeof(hpcOperationalData_t));
  bzero((L7_uchar8 *)hpcMessageReceiveNotifyList, sizeof(hpcMessageReceiveNotifyList_t));
  bzero((L7_uchar8 *)hpcStackEventNotifyList, sizeof(hpcStackEventNotifyList_t));

  /* check if platform has any hpc configuration data requirements, if so retrieve it */
  if (hpcHardwareCfgDataRequired() == L7_TRUE)
  {
    /* retrieve any saved config data from persistent storage */
    if (hpcCfgDataRetrieve() != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_BSP_COMPONENT_ID,
              "hpcInit: bad return code from hpcCfgDataRetrieve() call.\n");
    }
  }
  else
  {
    /* platform doesn't save variable data for HPC, use default settings */
    hpcCfgDataSetDefault();
  }

  /* apply configuration to hpc hardware specific layer */
  if (hpcCfgDataApply(&hpc_config_data) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_BSP_COMPONENT_ID,
            "hpcInit: bad return code from hpcApplyCfgData() call.\n");
    return(L7_ERROR);
  }

  /* initialize the platform specific part of hpc */
  if (hpcHardwareInit(hpcStackEventCallback, hpcTransportMessageRecvHandler) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_BSP_COMPONENT_ID,
            "hpcInit: bad return code from hpcHardwareInit() call.\n");
    return(L7_ERROR);
  }

  if (hpcDriverAsfInit() != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_BSP_COMPONENT_ID,
            "hpcInit: bad return code from hpcDriverAsfInit() call.\n");
    return(L7_ERROR);
  }

 /* special debug code */
  hpcDebugRemoteDevshellInit();

  /*
   * Load post-boot initialization SOC file.
   * This will allow the user to override properties from driver initialization.
   */
  hapiBroadSocFileLoad("sdk-postboot.soc", L7_TRUE);

  /* special debug code */
  return L7_SUCCESS;
}

/* PTin added: application control */
void hpcFini(void)
{
  hpcHardwareFini();
}

/**************************************************************************
*
* @purpose  Gets the MAC address allocation policy for the platform.  The
*           policy returned is one of the ENUM SYSAPI_MAC_POLICY_t.
*
* @param    none
*
* @returns  SYSAPI_MAC_POLICY_t
*
* @notes
*
* @end
*
*************************************************************************/
SYSAPI_MAC_POLICY_t hpcMacAllocationPolicyGet(void)
{
  return(hpcHardwareMacAllocationPolicyGet());
}

/**************************************************************************
*
* @purpose  Gets the MAC address for a given (slot, port) on the local
*           unit.
*
* @param    none
*
* @returns  L7_SUCCESS  no problems getting MAC address
* @returns  L7_ERROR    (slot, port) data invalid, or other error in MAC
*                       address retrieval
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t hpcIfaceMacGet(L7_INTF_TYPES_t type, L7_uint32 slot, L7_uint32 port, L7_uchar8 *l2_mac_addr, L7_uchar8 *l3_mac_addr)
{
  L7_RC_t rc = L7_ERROR;
  HPC_UNIT_DESCRIPTOR_t *pLocalUnitDescriptor;

  if ((pLocalUnitDescriptor = hpcLocalUnitDescriptorGet()) != L7_NULLPTR)
  {
    if(hpcHardwareIfaceMacGet(type, slot, port, l2_mac_addr, l3_mac_addr, pLocalUnitDescriptor) == L7_SUCCESS)
    {
      rc = L7_SUCCESS;
    }
  }
  return(rc);
}

/**************************************************************************
*
* @purpose  Local function registered with platform-specific part of HPC to
*           be used as callback when the interunit message transport becomes
*           ready or unready to handle messages.  Updates operational
*           data structure with current stack status.
*
* @param    none
*
* @returns  nothing
*
* @notes
*
* @end
*
*************************************************************************/
void hpcStackEventCallback(hpcStackEventMsg_t stack_event)
{
  HPC_EVENT_t event;
  L7_uint32   id;

  event = stack_event.hpcStackEvent;

  if (event >= HPC_EVENT_LAST)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_BSP_COMPONENT_ID,
            "hpcStackEventCallback: unknown event (%d)\n", event);
    return;
  }

  switch (event)
  {
    case HPC_EVENT_BCAST_TRANSPORT_UP:
    {
      hpc_oper_data.bcast_transport_is_ready = L7_TRUE;
      break;
    }
    case HPC_EVENT_BCAST_TRANSPORT_DOWN:
    {
      hpc_oper_data.bcast_transport_is_ready = L7_FALSE;
      break;
    }

    default:
      break;
  }


  /* Perform the callback to the registered handlers for the event */
  for (id = 0; id < L7_LAST_COMPONENT_ID; id++)
  {
    if (hpcStackEventNotifyList[event][id].notify)
    {
      hpcStackEventNotifyList[event][id].notify(stack_event);
    }
  }

  return;
}

/**************************************************************************
*
* @purpose  Function registered with HPC to get stack event callbacks.
*
* @param    registrarID  routine registrar ID (See L7_COMPONENT_IDS_t)
* @param    event        Stack event to register
* @param    *notify      Notification routine with the hpcStackEventMsg_t parm
*
* @returns  L7_SUCCESS callback successfully registered
* @returns  L7_FAILURE registrarID is not a valid component ID or invalid eventId
*
* @end
*
*************************************************************************/
L7_RC_t hpcStackEventRegisterCallback(L7_uint32 registrarId,
                                      HPC_EVENT_t event,
                                      void (*notify)(hpcStackEventMsg_t eventMsg))
{
  L7_RC_t rc = L7_SUCCESS;

  if ((registrarId >= L7_LAST_COMPONENT_ID) || (registrarId <= 0))
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_BSP_COMPONENT_ID,
            "%s: registrarId %d out of range\n", __FUNCTION__, registrarId);
    rc = L7_FAILURE;
  }
  else if ((event >= HPC_EVENT_LAST) || (event <= 0))
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_BSP_COMPONENT_ID,
            "%s: eventID %d out of range\n", __FUNCTION__, event);
    rc = L7_FAILURE;
  }
  else
  {
    hpcStackEventNotifyList[event][registrarId].notify =  notify;
  }

  return rc;
}


/**************************************************************************
*
* @purpose  Local function used to retrieve HPC's saved configuration
*           data.
*
* @param    none
*
* @returns  L7_SUCCESS  no problems configuration retrieval
* @returns  L7_ERROR    encountered error
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t hpcCfgDataRetrieve(void)
{
  L7_RC_t result=L7_SUCCESS;
  L7_uint32 crc;

  if (hpcHardwareCfgDataRetrieve(&hpc_config_data, sizeof(hpc_config_data)) == L7_SUCCESS)
 {
    /* check crc of retrieved data */
    crc = nvStoreCrc32((L7_uchar8 *)&hpc_config_data, (L7_uint32)(sizeof (hpc_config_data) - sizeof (hpc_config_data.crc)));
    if (hpc_config_data.crc == crc)
    {
      /* check version of retrieved data */
      if (hpc_config_data.version != HPC_CONFIG_VER_CURRENT)
      {
        /* version mismatch in retrieved data, abandon and generate default config data */
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_BSP_COMPONENT_ID,
                "hpcCfgDataRetrieve: version mismatch in retrieved config data.\n");
        result = hpcCfgDataRestore();
      }
    }
    else
    {
      /* bad crc in retrieved data, abandon and generate default config data */
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_BSP_COMPONENT_ID,
              "hpcCfgDataRetrieve: bad crc in retrieved config data.\n");
      result = hpcCfgDataRestore();
    }
  }
  else
  {
    /* error when retrieving data, abandon and generate default config data */
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_BSP_COMPONENT_ID,
            "hpcCfgDataRetrieve: error from hpcHardwareCfgDataRetrieve().\n");
    result = hpcCfgDataRestore();
  }
  return(result);
}

/*********************************************************************
* @purpose  Saves HPC's configuration data.
*
* @param    void
*
* @returns  L7_SUCCESS  no problems in save
* @returns  L7_ERROR    encountered error
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t hpcCfgDataSave(void)
{

  hpc_config_data.crc = nvStoreCrc32((L7_uchar8 *)&hpc_config_data,
                                     (L7_uint32)(sizeof (hpc_config_data) - sizeof (hpc_config_data.crc)));
  return(hpcHardwareCfgDataSave(&hpc_config_data, sizeof(hpc_config_data)));
}

/*********************************************************************
* @purpose  Restores hpc configuration to default values
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
void hpcCfgDataSetDefault(void)
{

  bzero((L7_uchar8 *)&hpc_config_data, sizeof(hpcCfgData_t));
  hpc_config_data.version = HPC_CONFIG_VER_CURRENT;
  hpc_config_data.role = SYSAPI_STACK_ROLE_UNASSIGNED;
  hpc_config_data.local_unit_number = 1;  /* if we don't know the past, initialize to be unit number 1 */
  hpc_config_data.admin_pref = L7_UNITMGR_MGMTFUNC_UNASSIGNED;

}

/*********************************************************************
* @purpose  Restores hpc configuration to default values
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t hpcCfgDataRestore(void)
{
  hpcCfgDataSetDefault();
  return(hpcCfgDataSave());
}

/**************************************************************************
*
* @purpose  Local function used to apply HPC's saved configuration
*           data to the hardware.
*
* @param    none
*
* @returns  L7_SUCCESS  no problems configuration retrieval
* @returns  L7_ERROR    encountered error
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t hpcCfgDataApply(hpcCfgData_t *cfg_data)
{
  L7_RC_t rc;
  L7_uint32 stack_port_index;

  do
  {

    if ((rc = hpcHardwareAdminPrefSet(cfg_data->admin_pref)) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_BSP_COMPONENT_ID,
              "hpcCfgDataApply: error from hpcHardwareAdminPrefSet().\n");
    }
    if ((rc = hpcHardwareTopOfStackSet(cfg_data->role)) != L7_SUCCESS)
    {
      rc = L7_SUCCESS;
    }
    if ((rc = hpcHardwareUnitNumberSet(cfg_data->local_unit_number)) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_BSP_COMPONENT_ID,
              "hpcCfgDataApply: error from hpcHardwareUnitNumberSet().\n");
      break;
    }
    for (stack_port_index = 0; stack_port_index < L7_MAX_STACK_PORTS_PER_UNIT; stack_port_index++)
    {
      if (cfg_data->stack_ports[stack_port_index].inuse == L7_TRUE)
      {
        if ((rc = hpcHardwareStackPortSet(cfg_data->stack_ports[stack_port_index].slot,
                                          cfg_data->stack_ports[stack_port_index].port,
                                          cfg_data->stack_ports[stack_port_index].enabled )) != L7_SUCCESS)
        {
          L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_BSP_COMPONENT_ID,
                  "hpcCfgDataApply: error from hpcHardwareStackPortSet().\n");
          break;
        }
      }
    }
    if (rc != L7_SUCCESS)
    {
      break;
    }
  } while(0); /* for error handling */
  return(rc);
}
/**************************************************************************
*
* @purpose  Retrieves a card descriptor from the support-cards database.
*
* @param    cardTypeId   cardTypeId of entry used for lookup
*
* @returns  pointer to descriptor record, L7_NULLPTR if not found
*
* @notes
*
* @end
*
*************************************************************************/
HPC_CARD_DESCRIPTOR_t *hpcCardDbLookup(L7_uint32 cardTypeId)
{
  HPC_CARD_DESCRIPTOR_t *desc_ptr;
  L7_uint32 index;
  L7_uint32 found_index = 0;
  L7_BOOL   item_found = L7_FALSE;

  /* scan the table for desired data entry */
  for (index=0; index < hpcSupportedCardsNumGet(); index++)
  {
    if (hpc_card_descriptor_db[index].cardTypeDescriptor.cardTypeId == cardTypeId)
    {
      item_found = L7_TRUE;
      found_index = index;
      break;
    }
  }

  /* if we found an item meeting the lookup criteria, provide the data and set appropriate return code */
  if (item_found == L7_TRUE)
  {
    desc_ptr = &hpc_card_descriptor_db[found_index];
  }
  else
  {
    desc_ptr = (HPC_CARD_DESCRIPTOR_t *)L7_NULLPTR;
  }
  return(desc_ptr);
}

/**************************************************************************
*
* @purpose  Validates whether the provided index is a valid cardTypeIdIndex
*           value.
*
* @param    cardTypeIdIndex   index into the card descriptor table
*
* @returns  L7_SUCCESS  index is valid
* @returns  L7_FAILURE  index is invalid
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t hpcCardDbEntryIndexCheck(L7_uint32 cardTypeIdIndex)
{
  L7_RC_t rc = L7_FAILURE;

  /* Check that index is valid... index is 1 based */
  if (cardTypeIdIndex > 0 && cardTypeIdIndex <= hpcSupportedCardsNumGet())
  {
    rc = L7_SUCCESS;
  }
  return(rc);
}

/**************************************************************************
*
* @purpose  Returns the next valid cardTypeIdIndex of greater value
*           than the one provided if it exists.
*
* @param    cardTypeIdIndex   index into the card descriptor table
*
* @returns  L7_SUCCESS  an entry with a greater index value exists
* @returns  L7_FAILURE  an entry with a greater index value does not exist
* @returns  cardTypeIdIndex value of next index if found
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t hpcCardDbEntryIndexNextGet(L7_uint32 *cardTypeIdIndex)
{
  L7_RC_t rc = L7_FAILURE;

  /* Check that index is in range after being incremented... index is 1 based */
  if (*cardTypeIdIndex == 0 || *cardTypeIdIndex < hpcSupportedCardsNumGet())
  {
    /* we are in range of table, so increment index value to get to next valid */
    (*cardTypeIdIndex)++;
    rc = L7_SUCCESS;
  }
  return(rc);
}

/**************************************************************************
*
* @purpose  Returns card type identifier that corresponds to
*           the index provided.
*
* @param    cardTypeIdIndex   index into the card descriptor table
*
* @returns  L7_SUCCESS  the index is valid and the string retrieved
* @returns  L7_FAILURE  the index was not valid
* @returns  *cardTypeId  pointer to the identifier
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t hpcCardDbCardIdGet(L7_uint32 cardTypeIdIndex, L7_uint32 *cardTypeId)
{
  L7_RC_t rc = L7_FAILURE;

  /* Check that index is valid... index is 1 based */
  if (cardTypeIdIndex > 0 && cardTypeIdIndex <= hpcSupportedCardsNumGet())
  {
    *cardTypeId = hpc_card_descriptor_db[cardTypeIdIndex-1].cardTypeDescriptor.cardTypeId;
    rc = L7_SUCCESS;
  }
  return(rc);
}

/**************************************************************************
*
* @purpose  Retrieves a unit descriptor from the support-unit database.
*
* @param    unitTypeId   cardTypeId of entry used for lookup
*
* @returns  pointer to descriptor record, L7_NULLPTR if not found
*
* @notes
*
* @end
*
*************************************************************************/
HPC_UNIT_DESCRIPTOR_t *hpcUnitDbLookup(L7_uint32 unitTypeId)
{
  HPC_UNIT_DESCRIPTOR_t *desc_ptr;
  L7_uint32 index;
  L7_uint32 found_index = 0;
  L7_BOOL   item_found = L7_FALSE;

  /* scan the table for desired data entry */
  for (index=0; index < hpcSupportedUnitsNumGet(); index++)
  {
    if (hpc_unit_descriptor_db[index].unitTypeDescriptor.unitTypeId == unitTypeId)
    {
      item_found = L7_TRUE;
      found_index = index;
      break;
    }
  }

  /* if we found an item meeting the lookup criteria, provide the data and set appropriate return code */
  if (item_found == L7_TRUE)
  {
    desc_ptr = &hpc_unit_descriptor_db[found_index];
  }
  else
  {
    desc_ptr = (HPC_UNIT_DESCRIPTOR_t *)L7_NULLPTR;
  }
  return(desc_ptr);
}


/**************************************************************************
*
* @purpose  Retrieve the unit descriptor from the database
*           for the unit type of the local unit.
*
* @param    none
*
* @returns  pointer to descriptor record, L7_NULLPTR if problem retrieving
*
* @notes
*
* @end
*
*************************************************************************/
HPC_UNIT_DESCRIPTOR_t *hpcLocalUnitDescriptorGet(void)
{
  static L7_BOOL first_time = L7_TRUE;
  static HPC_UNIT_DESCRIPTOR_t *desc_ptr = L7_NULLPTR;
  L7_uint32              unit_desc_id;

  if (first_time == L7_TRUE)
  {
    first_time = L7_FALSE;

    /* find out what type of unit we are running on (this is determined */
    /* and stored in registry by init code in the bsp                   */
    if (sysapiRegistryGet(UNIT_TYPE_ID, U32_ENTRY, (void *) &unit_desc_id)
        == L7_SUCCESS)
    {
      desc_ptr = hpcUnitDbLookup(unit_desc_id);
    }
  }
  return(desc_ptr);
}

/**************************************************************************
*
* @purpose  Register a callback function to be invoked when a card event
*           occurs.
*
* @param    notify       pointer to function
* @param    registrarID  one of enum L7_COMPONENT_IDS_t identifying the component
*                        to be called back
*
* @returns  L7_SUCCESS   if registrarID is valid
* @returns  L7_FAILURE   if registrarID is invalid
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t hpcCardEventCallbackRegister(void (*notify)(L7_uint32 slotNum, L7_uint32 cardTypeId, L7_uint32 event),
                                     L7_uint32 registrarID)
{
  L7_char8 name[L7_COMPONENT_NAME_MAX_LEN];

  if (cnfgrApiComponentNameGet(registrarID, name) != L7_SUCCESS)
  {
    osapiStrncpySafe(name, "Unknown", 8);
  }

  if ((registrarID >= L7_LAST_COMPONENT_ID) | (registrarID <= 0))
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_BSP_COMPONENT_ID,
            "hpcCardEventCallbackRegister: registrarID %d out of range\n", registrarID);
    return(L7_FAILURE);
  }
  else
  {
    /* call hardware specific function to generate card plug events as appropriate */
    if (hpcHardwareCardEventCallbackRegister(notify, registrarID) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_BSP_COMPONENT_ID,
              "hpcCardEventCallbackRegister: bad return code from hpcHardwareCardEventCallbackRegister() for registrarID %d, %s\n", registrarID, name);
      return(L7_FAILURE);
    }
    return(L7_SUCCESS);
  }
}

/**************************************************************************
*
* @purpose  Reports the local unit's MAC address for use in identifying
*           this unit across the distributed system.
*
* @param    mac   pointer to storage inwhich to store the mac address
*
* @returns  L7_SUCCESS  address retrieved with no problems
* @returns  L7_ERROR  problem encountered in platform specific retrieval function
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t hpcLocalUnitIdentifierMacGet(L7_enetMacAddr_t *mac)
{
  return(hpcHardwareLocalUnitIdentifierMacGet(mac));
}

/**************************************************************************
*
* @purpose  Reports the current state of interunit message transport.
*
* @param    none
*
* @returns  L7_TRUE  transport is ready to send messages
* @returns  L7_FALSE transport is not ready to send messages
*
* @notes
*
* @end
*
*************************************************************************/
L7_BOOL hpcBcastTransportIsReady(void)
{
  return(hpc_oper_data.bcast_transport_is_ready);
}

/**************************************************************************
*
* @purpose  Saves the unit number assigned to the local unit by unit manager.
*
* @param    lookup_type  one of enum HPC_LKUP_REQ_t indicating what type of lookup
*                        the caller wants
* @param    cardTypeId   cardTypeId of entry used for lookup
*
* @returns  pointer to descriptor record, L7_NULLPTR if not found
*
* @notes    This function is restricted to be called ONLY by sysapiHpcUnitNumberSet().
*
* @end
*
*************************************************************************/
L7_RC_t hpcUnitNumberSet(L7_uint32 unit)
{
  L7_RC_t rc = L7_SUCCESS;

  /* JPR_TODO - limit check unit parm ; what if hw set fails?*/

  /* check if this really news to us */
  if (hpc_config_data.local_unit_number != unit)
  {
    if ((rc = hpcHardwareUnitNumberSet(unit)) == L7_SUCCESS)
    /* if application of change to hardware found no problems... */
    {
      hpc_config_data.local_unit_number = unit;  /* update database */
      hpcCfgDataSave();
    }
  }
  return(rc);
}

/**************************************************************************
*
* @purpose  Gets the unit number assigned to the local unit by unit manager.
*
* @param    none
*
* @returns  pointer to descriptor record, L7_NULLPTR if not found
*
* @notes    This function is restricted to be called ONLY by sysapiHpcUnitNumberGet().
*
* @end
*
*************************************************************************/
L7_uint32 hpcUnitNumberGet(void)
{
  return(hpc_config_data.local_unit_number);
}

/**************************************************************************
*
* @purpose  Saves the management preference
*
* @param    admin_pref preference
*
* @returns  L7_SUCCESS, L7_FAILURE
*
* @notes    None
*
* @end
*
*************************************************************************/
L7_RC_t hpcAdminPrefSet(L7_uint32 admin_pref)
{
  L7_RC_t rc = L7_SUCCESS;

  /* JPR_TODO - limit check unit parm ; what if hw set fails?*/

  /* check if this really news to us */
  if (hpc_config_data.admin_pref != admin_pref)
  {
    if ((rc = hpcHardwareAdminPrefSet(admin_pref)) == L7_SUCCESS)
    /* if application of change to hardware found no problems... */
    {
      hpc_config_data.admin_pref = admin_pref;  /* update database */
      hpcCfgDataSave();
    }
  }
  return(rc);
}
/**************************************************************************
*
* @purpose  Gets the management preference
*
* @param    none
*
* @returns  management preference
*
* @notes    None
*
* @end
*
*************************************************************************/
L7_uint32 hpcAdminPrefGet(void)
{
  return (hpc_config_data.admin_pref);
}

/*********************************************************************
* @purpose  Given a unit, gets the unit's system identifier key
*
* @param    unit_number unit number
* @param    *key   pointer to unit key storage
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    will copy L7_HPC_UNIT_ID_KEY_LEN bytes into *key
*
* @end
*********************************************************************/
L7_RC_t hpcUnitIdentifierKeyGet(L7_uint32 unit, L7_enetMacAddr_t *key)
{
  return(hpcHardwareUnitIdentifierKeyGet(unit, key));
}

/**************************************************************************
*
* @purpose  Informs HPC of the local unit's role in the stack.
*           HPC should take any steps necessary based on this fact.
*
* @param    role  one of enum SYSAPI_STACK_ROLE_t indicating this unit's role
                  in the stack
*
* @returns  L7_SUCCESS  all necessary processing was successful
* @returns  L7_ERROR    some problem encountered
*
* @notes    This function is restricted to be called ONLY by sysapiHpcTopOfStackSet().
*
* @end
*
*************************************************************************/
L7_RC_t hpcTopOfStackSet(SYSAPI_STACK_ROLE_t role)
{
  L7_RC_t rc;

  /* JPR_TODO - limit check parms? */
  if ((rc = hpcHardwareTopOfStackSet(role)) == L7_SUCCESS)
  {
    hpc_config_data.role = role;
    hpcCfgDataSave();
  }
  return(rc);
}

/**************************************************************************
*
* @purpose  Retrieves local unit's stacking role.
*
* @returns    One of enum SYSAPI_STACK_ROLE_t indicating this unit's role
              in the stack
*
*
* @notes    This function is restricted to be called ONLY by sysapiHpcTopOfStackGet().
*
* @end
*
*************************************************************************/
SYSAPI_STACK_ROLE_t hpcTopOfStackGet(void)
{
  return(hpc_config_data.role);
}
/*********************************************************************
* @purpose  Gets whether network port is used for stacking.
*
* @param    slot - slot number of port be configured for stacking
* @param    port - port number to be configured for stacking
* @param    stacking_enable - OUT: boolean to enable (L7_TRUE) or disable (L7_FALSE)
*
* @returns  L7_SUCCESS - if all goes well
* @returns  L7_FAILURE - if specified port is not found in the database.
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t hpcStackPortGet(L7_uint32 slot, L7_uint32 port, L7_BOOL *stacking_enable)
{
  L7_uint32 i;

  /* Check whether port with specified slot number and port number
  ** is already in the configuration.
  */
  for (i = 0; i < L7_MAX_STACK_PORTS_PER_UNIT; i++)
  {
    if ((hpc_config_data.stack_ports[i].inuse == L7_TRUE) &&
        (hpc_config_data.stack_ports[i].slot == slot) &&
        (hpc_config_data.stack_ports[i].port == port))
    {
      *stacking_enable = hpc_config_data.stack_ports[i].enabled;
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Enables or disables a network port for use in stacking.
*           This information is saved in the HPC component's persistent
*           storage configuration so it is available after reboots.
*
* @param    slot - slot number of port be configured for stacking
* @param    port - port number to be configured for stacking
* @param    stacking_enable - boolean to enable (L7_TRUE) or disable (L7_FALSE)
*
* @returns  L7_SUCCESS - if all goes well
* @returns  L7_FAILURE - if problem occurs
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t hpcStackPortSet(L7_uint32 slot, L7_uint32 port, L7_BOOL stacking_enable)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 i;

  /* Check whether port with specified slot number and port number
  ** is already in the configuration.
  */
  for (i = 0; i < L7_MAX_STACK_PORTS_PER_UNIT; i++)
  {
    if ((hpc_config_data.stack_ports[i].inuse == L7_TRUE) &&
        (hpc_config_data.stack_ports[i].slot == slot) &&
        (hpc_config_data.stack_ports[i].port == port))
    {
      hpc_config_data.stack_ports[i].enabled = stacking_enable;
      hpcCfgDataSave();
      return L7_SUCCESS;
    }
  }

  /* scan config data store for unused storage location
  */
  for (i=0; i < L7_MAX_STACK_PORTS_PER_UNIT; i++)
  {
    if (hpc_config_data.stack_ports[i].inuse == L7_FALSE)
    {
      if ((rc = hpcHardwareStackPortSet(slot, port, stacking_enable)) == L7_SUCCESS)
      {
        hpc_config_data.stack_ports[i].slot = slot;
        hpc_config_data.stack_ports[i].port = port;
        hpc_config_data.stack_ports[i].enabled = stacking_enable;
        hpc_config_data.stack_ports[i].inuse = L7_TRUE;
        hpcCfgDataSave();
      }
      break;
    }
  }
  /* check if we couldn't find a location for storing this data */
  if (i == L7_MAX_STACK_PORTS_PER_UNIT)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_BSP_COMPONENT_ID,
            "hpcStackPortSet: Tried to add more than L7_MAX_STACK_PORTS_PER_UNIT stack ports.\n");
    rc = L7_FAILURE;
  }
  return(rc);
}

/*********************************************************************
* @purpose  Looks up Stack Port by internal unit/port index
*
* @param    bcm_unit - internal unit number
* @param    bcm_port - internal port number
* @param    slot - slot number of port be configured for stacking
* @param    port - port number to be configured for stacking
*
* @returns  L7_SUCCESS - if port is found
* @returns  L7_FAILURE - if specified port is not found in the database.
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t hpcStackPortLookup(L7_uint32 bcm_unit, L7_uint32 bcm_port, L7_uint32 *slot, L7_uint32 *port)
{
  HPC_UNIT_DESCRIPTOR_t       *lclUnitDesc;
  L7_uint32 i;

  if ((lclUnitDesc = hpcLocalUnitDescriptorGet()) == L7_NULLPTR)
  {
    return L7_FAILURE;;
  }

  for (i=0; i < lclUnitDesc->unitTypeDescriptor.num_stack_ports; i++)
  {
    if (bcm_unit == lclUnitDesc->unitTypeDescriptor.stack_port_list[i].h1 &&
        bcm_port == lclUnitDesc->unitTypeDescriptor.stack_port_list[i].h2)
    {
      *slot = lclUnitDesc->unitTypeDescriptor.stack_port_list[i].slot;
      *port = lclUnitDesc->unitTypeDescriptor.stack_port_list[i].port;
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Queries the platform component as to the largest message
*           size in bytes that the transport can handle.
*
* @param    none
*
* @returns  the max size of message payload in bytes
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 hpcTransportMaxMessageLengthGet(void)
{
  return(hpcHardwareTransportMaxMessageLengthGet());
}


/*********************************************************************
* @purpose  Register a routine to be called when an intrastack message
*           is received for the registrar.
*
* @param    *notify      Notification routine with the following parm
*                        @param    src_key {(input)}   Key (mac-address) of the
                                                       unit that sent the msg
*                        @param    *buffer             pointer to buffer
                                                       containing message
*                        @param    msg_len             length in bytes of message
* @param    registrarID  routine registrar ID (See L7_COMPONENT_IDS_t)
*
* @returns  L7_SUCCESS callback successfully registered
* @returns  L7_FAILURE registrarID is not a valid component ID
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t hpcReceiveCallbackRegister(void (*notify)(L7_enetMacAddr_t src_key,
                                                  L7_uchar8* buffer,
                                                  L7_uint32 msg_len),
                                   L7_uint32 registrarID)
{
  if ((registrarID >= L7_LAST_COMPONENT_ID) | (registrarID <= 0))
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_BSP_COMPONENT_ID,
            "hpcReceiveCallbackRegister: registrarID %d out of range\n", registrarID);
    return(L7_FAILURE);
  }
  else
  {
    hpcMessageReceiveNotifyList[registrarID].notify =  notify;
    hpcMessageReceiveNotifyList[registrarID].registrarID = registrarID;
    hpcMessageReceiveNotifyList[registrarID].flags = 0;
    return(L7_SUCCESS);
  }
}


/*********************************************************************
* @purpose  Set the flags for a HPC registrar
*
* @param    registrarID {(input)} routine registrar ID (See L7_COMPONENT_IDS_t)
* @param    flags       {(input)} Flag values
*
* @returns  L7_SUCCESS callback successfully registered
* @returns  L7_FAILURE registrarID is not a valid component ID
*
* @comments Caller must be registered with HPC
*
* @end
*********************************************************************/
L7_RC_t hpcRegistrarFlagsSet(L7_uint32 registrarID,
                             HPC_REGISTRAR_FLAGS_t flags)

{
  if (hpcMessageReceiveNotifyList[registrarID].registrarID != registrarID)
  {
    return L7_FAILURE;
  }

  hpcMessageReceiveNotifyList[registrarID].flags |= flags;

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Send a message to all other units in the stack.  Message is delivered
*           to corresponding registrarID in remote system.  If there is
*           no registration of the registrarID in the local system, an
*           error is returned.  If there is no registration in any of the
*           remote systems, the message is silently discarded there and
*           no notification is sent to the sending caller.
*
*           If the message payload exceeds the platform transport's
*           maximum message size, this function will return an error
*           without sending anything.
*
* @param    registrarID  routine registrar ID (See L7_COMPONENT_IDS_t)
* @param    msg_length  number of bytes in buffer
* @param    buffer  pointer to the payload to be sent
*
* @returns  L7_SUCCESS message successfully dispatched to harware transport
* @returns  L7_FAILURE problem occured in transport
* @returns  L7_ERROR registrar is not known to HPC message transport
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t hpcBroadcastMessageSend(L7_uint32 registrarID, L7_uint32 msg_length, L7_uchar8* buffer)
{
  L7_RC_t rc = L7_ERROR;

  if ((registrarID >= L7_LAST_COMPONENT_ID) | (registrarID <= 0))
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_BSP_COMPONENT_ID,
            "hpcBroadcastMessageSend: registrarID %d out of range\n", registrarID);
  }
  else
  {
    /* check if this registrar has registered with the transport */
    if (hpcMessageReceiveNotifyList[registrarID].registrarID == registrarID)
    {
      /* validate message payload size does not exceed platform limits */
      if (msg_length <= hpcHardwareTransportMaxMessageLengthGet())
      {
        rc = hpcHardwareBroadcastMessageSend(registrarID, msg_length, buffer);
        hpcMessageReceiveNotifyList[registrarID].tx++;
      }
    }
  }
  return(rc);
}

/*********************************************************************
* @purpose  Send a message to a specific unit in the stack.  Message is delivered
*           to corresponding registrarID in remote system.  If there is
*           no registration of the registrarID in the local system, an
*           error is returned.  If there is no registration in the
*           remote systems, the message is silently discarded there and
*           an error is returned to the sending caller.  This function will
*           hold the caller's thread until the message is either successfully
*           acknowledged or the send times out.
*
*           If the message payload exceeds the platform transport's
*           maximum message size, this function will return an error
*           without sending anything.
*
* @param    registrarID  routine registrar ID (See L7_COMPONENT_IDS_t)
* @param    unit         destination unit ID
* @param    msg_length  number of bytes in payload buffer
* @param    buffer  pointer to the payload to be sent
*
* @returns  L7_SUCCESS message successfully dispatched to harware transport
* @returns  L7_FAILURE problem occured in transport, message not acknowledged
* @returns  L7_ERROR registrar is not known to HPC message transport
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t hpcMessageSend(L7_uint32 registrarID, L7_uint32 unit,
                       L7_uint32 msg_length,  L7_uchar8* buffer)
{
  L7_RC_t rc = L7_ERROR;
  L7_char8 name[L7_COMPONENT_NAME_MAX_LEN];

  if (cnfgrApiComponentNameGet(registrarID, name) != L7_SUCCESS)
  {
    osapiStrncpySafe(name, "Unknown", 8);
  }

  if ((registrarID >= L7_LAST_COMPONENT_ID) | (registrarID <= 0))
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_BSP_COMPONENT_ID,
            "hpcMessageSend: registrarID %d out of range\n", registrarID);
  }
  else
  {
    /* check if this registrar has registered with the transport */
    if (hpcMessageReceiveNotifyList[registrarID].registrarID == registrarID)
    {
      if (msg_length <= hpcHardwareTransportMaxMessageLengthGet())
      {
        /* send it to platform specific send function */
        rc = hpcHardwareMessageSend(registrarID, unit, msg_length, buffer,
                                    0, L7_NULLPTR,
                                    LVL7_RELIABLE_TRANSPORT_PORT_NUMBER);
        hpcMessageReceiveNotifyList[registrarID].tx++;
      }
      else
      {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_BSP_COMPONENT_ID,
                "Mesg len %d more than HPC can send from registrarID %d, %s\n", msg_length, registrarID, name);
      }

    }
  }
  return(rc);
}

/*********************************************************************
* @purpose  Resets the Driver to a known state
*
* @param    void
*
* @returns  L7_SUCCESS successfully reset the driver
* @returns  L7_FAILURE problem occurred while resetting the driver
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t hpcDriverReset(void)
{
  return(hpcHardwareDriverReset());
}

/*********************************************************************
* @purpose  Determine whether the driver is responsible for resyncing new devices
*
* @param    void
*
* @returns  L7_TRUE   if the driver is responsible
* @returns  L7_FALSE  if the driver is not responsible
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL hpcDriverSyncSupported(void)
{
  return(hpcHardwareDriverSyncSupported());
}

/**************************************************************************
*
* @purpose  Local function registered with platform-specific part of HPC to
*           be used as callback when the interunit message transport receives
*           message.  Uses
*
* @param    none
*
* @returns  nothing
*
* @notes
*
* @end
*
*************************************************************************/
void hpcTransportMessageRecvHandler(L7_enetMacAddr_t src_key,
                                    L7_uint32 receive_id,
                                    L7_uchar8* buffer, L7_uint32 msg_len)
{
  L7_uint32 unit_number;
  L7_RC_t   rc;
  L7_BOOL   known_src_unit = L7_TRUE;

  rc = unitMgrKeyUnitIdGet(src_key, &unit_number);
  if (rc != L7_SUCCESS)
  {
    known_src_unit = L7_FALSE;
  }

  if ((receive_id >= L7_LAST_COMPONENT_ID) | (receive_id <= 0))
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_BSP_COMPONENT_ID,
            "hpcTransportMessageRecvHandler: receive_id %d out of range\n", receive_id);
  }
  else
  {
    if (hpcMessageReceiveNotifyList[receive_id].registrarID == receive_id)
    {
      if (hpcMessageReceiveNotifyList[receive_id].notify != L7_NULLPTR)
      {
        if ((known_src_unit == L7_TRUE) ||
            ((known_src_unit == L7_FALSE) &&
             (hpcMessageReceiveNotifyList[receive_id].flags &
                HPC_REGISTRAR_RECEIVE_MSGS_FROM_UNKNOWN_UNIT)))
        {
          hpcMessageReceiveNotifyList[receive_id].notify(src_key, buffer, msg_len);
          hpcMessageReceiveNotifyList[receive_id].rx++;
        }
      }
    }
  }

  return;
}

/*------------------- DEBUG FUNCTIONS -----------------------*/

/* register with the HPC transport using a poached IDs (DVMRP and VRRP will not need these) */
static L7_uint32  test_registrarID = L7_FLEX_DVMRP_MAP_COMPONENT_ID;

static L7_uint32  rmt_devshell_registrarID = L7_VRRP_MAP_COMPONENT_ID;

void hpcDebugTransportTestCb(L7_enetMacAddr_t src_key,
                             L7_uchar8* buffer, L7_uint32 msg_len)
{
  printf("hpcDebugTransportTestCb rec'd message: '%s'\n", buffer);
}

void hpcDebugTransportInit(void)
{
  /* register with the HPC transport using a poached ID */
  hpcReceiveCallbackRegister(hpcDebugTransportTestCb, test_registrarID);
}

void hpcDebugBcastMsgSend(void)
{
  L7_uchar8 payload[] = "hpcDebugBcastMsgSend test msg";

  /* Send off the packet to all CPUs */
  hpcBroadcastMessageSend(test_registrarID, sizeof(payload), payload);
}

void hpcDebugMsgSend(unit)
{
  L7_uchar8 payload[] = "hpcDebugMsgSend test msg";

  /* Send off the packet to selected CPU */
  hpcMessageSend(test_registrarID, unit, sizeof(payload), payload);
}


#define RMT_DEVSHELL_CMD_MAX_LENGTH 255

void hpcDebugRemoteDevshell(L7_uint32 unit, char *command)
{
  if (L7_SUCCESS != hpcMessageSend(rmt_devshell_registrarID, unit, (strlen(command) + 1), command))
  {
    sysapiPrintf("Error in call to hpcMessageSend for unit %d.\n",unit);
  }
}

void hpcDebugRemoteDevshellTask(int numArgs, void * arg)
{
  /* execute the requested command */
  (void)osapiDevShellExec((L7_char8 *) arg);

  /* free the storage for the command string */
  osapiFree(rmt_devshell_registrarID, arg);

  return;
}

void hpcDebugRemoteDevshellCb(L7_enetMacAddr_t src_key,
                              L7_uchar8* buffer, L7_uint32 msg_len)
{
  void *command_text;
  L7_uint32 thread_id;

  /* do some checks, reserve the last byte of command_text to hold \0 */
  if (RMT_DEVSHELL_CMD_MAX_LENGTH < msg_len)
  {
    sysapiPrintf("hpcDebugRemoteDevshellCb(): received message too long.\n");
    return;
  }
  if (L7_NULLPTR == (command_text = osapiMalloc(rmt_devshell_registrarID, msg_len+1)))
  {
    sysapiPrintf("hpcDebugRemoteDevshellCb(): could not allocate command buffer memory.\n");
    return;
  }

  /* make copy of the received command string */
  strncpy((L7_char8 *)command_text, buffer, msg_len+1);

  /* dispatch the command to the devshell function on it's own thread */
  thread_id = osapiTaskCreate("rmtdevshell", hpcDebugRemoteDevshellTask, 1, command_text,
                              L7_DEFAULT_STACK_SIZE, L7_DEFAULT_TASK_PRIORITY, L7_DEFAULT_TASK_SLICE);
  return;
}

void hpcDebugRemoteDevshellInit(void)
{
  /* register with the HPC transport using a poached ID */
  hpcReceiveCallbackRegister(hpcDebugRemoteDevshellCb, rmt_devshell_registrarID);
}

void hpcDebugIfaceMacGet(L7_INTF_TYPES_t type, L7_uint32 slot, L7_uint32 port)
{
  L7_uchar8 l2_mac[6];
  L7_uchar8 l3_mac[6];
  L7_uint32 i;

  if (hpcIfaceMacGet(type, slot, port, l2_mac, l3_mac) == L7_SUCCESS)
  {
    printf("The resulting L2 mac for (%d, %d) is ", slot, port);

    for (i=0; i<5; i++)
    {
      printf("%02x:", l2_mac[i]);
    }
    printf("%02x\n", l2_mac[5]);
  }
  else
  {
    printf("Bad return code from hpcIfaceMacGet().\n");
  }
}

void hpcDebugDataShow(void)
{
  L7_uint32 i;
  L7_BOOL no_stack_ports = L7_TRUE;
  L7_RC_t rc;
  L7_char8 name[L7_COMPONENT_NAME_MAX_LEN+1];

  printf("HPC operational data (hpc_oper_data)\n");
  printf("  hpc_oper_data.bcast_transport_is_ready = %s\n",
         hpc_oper_data.bcast_transport_is_ready ? "L7_TRUE":"L7_FALSE");
  printf("HPC configuration data (hpc_config_data)\n");
  printf("  hpc_config_data.admin_pref = %d\n", hpc_config_data.admin_pref);
  printf("  hpc_config_data.local_unit_number = %d\n", hpc_config_data.local_unit_number);
  printf("  hpc_config_data.role = %d\n", hpc_config_data.role);
  printf("  hpc_config_data.version = %d\n", hpc_config_data.version);
  for (i=0; i<L7_MAX_STACK_PORTS_PER_UNIT; i++)
  {
    if (hpc_config_data.stack_ports[i].inuse == L7_TRUE)
    {
      if (no_stack_ports == L7_TRUE)
      {
        printf("\n  Front panel stacking configured on (s.p enabled/disabled):\n");
      }
      printf("    %d.%d - %s\n",
             hpc_config_data.stack_ports[i].slot,
             hpc_config_data.stack_ports[i].port,
             hpc_config_data.stack_ports[i].enabled ? "enabled":"disabled");
      no_stack_ports = L7_FALSE;
    }
  }
  if (no_stack_ports == L7_TRUE)
  {
    printf("\n  No front panel ports configured for stacking.\n");
  }

  printf("\nPer Component Packets Tx/Rx using HPC\n");
  for (i = 0; i <  L7_LAST_COMPONENT_ID;i++)
  {
    if (hpcMessageReceiveNotifyList[i].registrarID == i)
    {
      if (hpcMessageReceiveNotifyList[i].notify != L7_NULLPTR)
      {
         rc = cnfgrApiComponentNameGet(i,name);
         if (rc == L7_SUCCESS)
          {
            printf("%-32s flags %x tx=%10u  rx=%10u\n",
                   name, hpcMessageReceiveNotifyList[i].flags,
                   hpcMessageReceiveNotifyList[i].tx,
                   hpcMessageReceiveNotifyList[i].rx);
          }
      }
    }
  }
}

/**************************************************************************
*
* @purpose  Return the physical slot count for the local unit's base CPU board.
*
* @param    none
*
* @returns  L7_uint32   Number of physical slots for this base CPU card,
*                       zero indicates error in retrieval
*
* @comments
*
* @end
*
*************************************************************************/
L7_uint32 hpcLocalUnitSlotCountGet(void)
{
  HPC_UNIT_DESCRIPTOR_t *desc_ptr;
  L7_uint32 physSlotCount = 0;

  if ((desc_ptr = hpcLocalUnitDescriptorGet()) != L7_NULLPTR)
  {
    physSlotCount = desc_ptr->unitTypeDescriptor.numPhysSlots;
  }
  return(physSlotCount);
}
/*********************************************************************
* @purpose  Set (Enable/Disable) the ASF mode.
*
* @param    mode        @b{(input)} mode L7_ENABLE/L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t hpcDriverAsfInit()
{
  L7_RC_t   rc = L7_SUCCESS;
  hpcAsfCfg_t asfCfg;
  L7_uint32 crc;

  /* Set default mode. */
  hpc_oper_data.asfConfiguredMode = L7_DISABLE;

  /* Check for ASF mode and configure HW appropriately. */
  if (osapiFsRead(HPC_ASF_CFG_FILENAME, (L7_uchar8 *)&asfCfg, sizeof(asfCfg)) == L7_SUCCESS)
  {
    /* Check CRC */
    crc = nvStoreCrc32((L7_uchar8 *)&asfCfg, (L7_uint32)(sizeof (hpcAsfCfg_t) - sizeof (asfCfg.crc)));
    if (crc == asfCfg.crc)
    {
      if (asfCfg.version == HPC_ASF_CONFIG_VER_CURRENT)
      {
        hpc_oper_data.asfConfiguredMode = asfCfg.configAsfMode;
      }
      else
      {
        /* If the cfg file version changes, need to handle config migration here. */
      }
    }
  }

  hpc_oper_data.asfCurrentMode = hpc_oper_data.asfConfiguredMode;

  rc = hpcHardwareAsfInit(hpc_oper_data.asfCurrentMode);

  return rc;
}

/*********************************************************************
* @purpose  Set (Enable/Disable) the ASF mode.
*
* @param    mode        @b{(input)} mode L7_ENABLE/L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t hpcDriverAsfModeSet(L7_uint32 mode)
{
  hpc_oper_data.asfConfiguredMode = mode;

  return hpcHardwareAsfModeSet(mode);
}

/*********************************************************************
* @purpose  Get (Enable/Disable) the ASF mode.
*
* @param    currMode    @b{(input)} Current mode L7_ENABLE/L7_DISABLE
* @param    configMode  @b{(input)} Configured mode L7_ENABLE/L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t hpcDriverAsfModeGet(L7_uint32 *currMode, L7_uint32 *configMode)
{
  if ((currMode == NULL) || (configMode == NULL))
  {
    return L7_FAILURE;
  }

  *currMode   = hpc_oper_data.asfCurrentMode;
  *configMode = hpc_oper_data.asfConfiguredMode;

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  This routine determines if there are any stack ports.
*           If no stack ports, then unit manager can use this
*           info to speed up it's state machines.
*
* @returns  L7_BOOL
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL hpcNoStackPorts()
{
  return hpcHardwareNoStackPorts();
}

/*********************************************************************
* @purpose  Inform the driver that this unit is the standby of the stack
*
* @param    stby_present {(input)} L7_TRUE: There is a valid stby
*                                L7_FALSE: There is no valid stby
* @param
* @param    stby_key {(input)} Key of the stby unit
*
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t hpcDriverNotifyStandbyStatus(L7_BOOL stby_present,
                                     L7_enetMacAddr_t stby_key)
{
  return hpcHardwareNotifyStandbyStatus(stby_present, stby_key);
}

/*********************************************************************
* @purpose  UM on each unit informs the driver that the current manager
*           of the stack is no longer present. Driver tries to remove
*           the manager key from ATP/Next-hop transport layers so that
*           pending/future communication with the failed manager is
*           prevented.
*
* @param    managerKey: CPU key of the Manager
*
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
void hpcDriverRemoveStackManager(L7_enetMacAddr_t managerKey)
{
  return hpcHardwareRemoveStackManager(managerKey);
}

/*********************************************************************
* @purpose  Instruct the driver to shutdown all the front-panel ports
*           for a unit.
*
* @param    unitNumber {(input)} Unit for which the ports are to be shut.
*                                L7_ALL_UNITS indicates all the valid
*                                stack members.
*
*
* @returns  L7_RC_t
*
* @comments Called by UM before move-management
*
* @end
*********************************************************************/
L7_RC_t hpcDriverShutdownPorts(L7_uint32 unitNumber)
{
  return hpcHardwareShutdownPorts(unitNumber);
}



/*********************************************************************
* @purpose  Routine for application helpers to send packets on the local
*           unit.
*
* @param    frame              @b{(input)}
* @param    frameSize          @b{(input)}
* @param    priority           @b{(input)}
* @param    slot               @b{(input)}
* @param    port               @b{(input)}
* @param    ignoreEgressRules  @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t hpcDriverHelperSend(L7_uchar8   *frame,
                            L7_uint32    frameSize,
                            L7_uint8     priority,
                            L7_uint8     slot,
                            L7_ushort16  port,
                            L7_BOOL      ignoreEgressRules)
{
  return hpcHardwareHelperSend(frame, frameSize, priority, slot, port, ignoreEgressRules);
}

/*********************************************************************
*
* @purpose Allocate memory for the CPU send stats for HPC helper.
*          Zero out the stat values.
*
* @returns
*
* @notes   none
*
* @end
*
*********************************************************************/
void hpcDriverHelperStatsInit()
{
  hpcHardwareHelperStatsInit();
}
