
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename  sysapi_cfg_migrate.c
*
* @purpose
*
* @component sysapi
*
* @create       8/23/2004
*
* @author       Rama Sasthri, Kristipati
* @end
*
*********************************************************************/

#include <string.h>             /* for memcpy() etc... */
#include <stdarg.h>             /* for va_start, etc... */
#include "l7_common.h"
#include "l7_product.h"
#include "registry.h"
#include "osapi.h"
#include "log_api.h"
#include "log.h"
#include "sysapi.h"
#include "l7_ip_api.h"

#include "platform_config_migrate.h"
#include "sysapi_cfg_migrate.h"

#define SYSAPI_READ_LONG(addr) (*(L7_uint32 *) (addr))

extern SYSAPI_DIRECTORY_t *pSysapiCfgTree[SYSAPI_MAX_FILE_ENTRIES];
extern void *sysapiCfgSema;
extern L7_RC_t sysapiCfgFileIndexGet (const L7_char8 * fileName, L7_uint32 * pFileIndex);
extern void nimRel4_0SlotInfoRecord (L7_uint32 slot, L7_uchar8 * pSlotInfo, L7_uint32 cfgLen);
extern L7_RC_t ipMapMigrateRel4_0VlanInterfaces (L7_uchar8 * pIpMap, L7_uint32 ipMapLen, 
                                                 L7_uint32 min_vlan_intIfNum);
extern void dot1qMigrateRel4_0VlanIntfMask(L7_uint32 min_vlan_intIfNum);

/* The following definitions are purposfully re-defined to facilitate configuration
   migration release 4.0 to later releases */

#define ACL_CFG_FILENAME        "acl.cfg"
#define BWB_CFG_FILENAME     "bandwidthBucketCfgData.cfg"
#define L7_BGP_CFG_FILENAME  "bgpMap.cfg"
#define CLI_WEB_CFG_FILENAME  "cliWebCfgData.cfg"
#define L7_DHCPS_CFG_FILENAME       "dhcpsMap.cfg"
#define L7_DIFFSERV_CFG_FILENAME  "diffserv.cfg"
#define DOT1Q_CFG_FILENAME  "dot1q.cfg"
#define DOT1S_CFG_FILENAME     "dot1s.cfg"
#define DOT1X_CFG_FILENAME     "dot1x.cfg"
#define DOT3AD_CFG_FILENAME "dot3ad.cfg"
#define DVLANTAG_CFG_FILENAME     "dvlantag.cfg"
#define L7_DVMRP_CFG_FILENAME   "dvmrpMap.cfg"
#define FDB_CFG_FILENAME  "fdb.cfg"
#define FILTER_CFG_FILENAME  "filterCfgData.cfg"
#define GARP_CFG_FILENAME  "garp.cfg"
#define GMRP_CFG_FILENAME "gmrp.cfg"
#define SNOOP_CFG_FILENAME  "snoopCfgData.cfg"
#define L7_IGMP_CFG_FILENAME  "igmpMap.cfg"
#define L7_IP_CFG_FILENAME  "ipCfgData.cfg"
#define L7_IP_STATIC_ROUTES_CFG_FILENAME  "ipStaticRoutesCfg.cfg"
#define L7_IP_STATIC_ARP_CFG_FILENAME     "ipStaticArpCfg.cfg"
#define IPV6_PROVISIONING_CFG_FILENAME  "ipv6_provisioning.cfg"
#define L7_MCAST_CFG_FILENAME  "mcastMap.cfg"
#define MIRROR_CFG_FILENAME  "mirrorCfgData.cfg"
#define L7_OSPF_CFG_FILENAME      "ospfMap.cfg"
#define PBVLAN_CFG_FILENAME  "pbVlan.cfg"
#define L7_PIMDM_CFG_FILENAME   "pimdmMap.cfg"
#define L7_PIMSM_CFG_FILENAME   "pimsmMap.cfg"
#define POE_CFG_FILENAME     "poe.cfg"
#define RADIUS_CFG_FILENAME    "radius.cfg"
#define L7_DHCP_RELAY_CFG_FILENAME  "dhcpRelay.cfg"
#define L7_RIP_CFG_FILENAME       "ripMap.cfg"
#define L7_RTR_DISC_FILENAME     "rtrDiscCfg.cfg"
#define SIM_CFG_FILENAME  "simCfgData.cfg"
#define SNMP_CFG_FILENAME  "snmpCfgData.cfg"
#define SSHD_CFG_FILENAME    "sshd.cfg"
#define SSLT_CFG_FILENAME    "sslt.cfg"
#define POLICY_CFG_FILENAME  "policy.cfg"
#define TC_CFG_FILENAME     "trafficClassCfgData.cfg"
#define TRAP_CFG_FILENAME  "trapCfgData.cfg"
#define USER_MGR_CFG_FILENAME  "user_mgr.cfg"
#define L7_VRRP_CFG_FILENAME   "vrrpMap.cfg"

typedef struct
{
  L7_uint32 slot;
  L7_BOOL configIdKnown;
  L7_INTF_TYPES_t sysIntfType;
  nimConfigID_t configId;
}
rel4_0IntfTypeMap_t;

static rel4_0IntfTypeMap_t rel4_0IntfTypeMap[L7_MAX_INTERFACE_COUNT_REL_4_0];
static L7_uint32 rel4_0SlotPortMap[L7_MAX_SLOTS_PER_BOX_REL_4_0][L7_MAX_PORTS_PER_SLOT_REL_4_0 + 1];

static L7_char8 sysapiUncombinedCfgFileList[][L7_MAX_FILENAME] = {
  ACL_CFG_FILENAME,
  BWB_CFG_FILENAME,
  L7_BGP_CFG_FILENAME,
  CLI_WEB_CFG_FILENAME,
  L7_DHCPS_CFG_FILENAME,
  L7_DIFFSERV_CFG_FILENAME,
  DOT1Q_CFG_FILENAME,
  DOT1S_CFG_FILENAME,
  DOT1X_CFG_FILENAME,
  DOT3AD_CFG_FILENAME,
  DVLANTAG_CFG_FILENAME,
  L7_DVMRP_CFG_FILENAME,
  FDB_CFG_FILENAME,
  FILTER_CFG_FILENAME,
  GARP_CFG_FILENAME,
  GMRP_CFG_FILENAME,
  SNOOP_CFG_FILENAME,
  L7_IGMP_CFG_FILENAME,
  L7_IP_CFG_FILENAME,
  L7_IP_STATIC_ROUTES_CFG_FILENAME,
  L7_IP_STATIC_ARP_CFG_FILENAME,
  IPV6_PROVISIONING_CFG_FILENAME,
  L7_MCAST_CFG_FILENAME,
  MIRROR_CFG_FILENAME,
  L7_OSPF_CFG_FILENAME,
  PBVLAN_CFG_FILENAME,
  L7_PIMDM_CFG_FILENAME,
  L7_PIMSM_CFG_FILENAME,
  POE_CFG_FILENAME,
  RADIUS_CFG_FILENAME,
  L7_DHCP_RELAY_CFG_FILENAME,
  L7_RIP_CFG_FILENAME,
  L7_RTR_DISC_FILENAME,
  SIM_CFG_FILENAME,
  SNMP_CFG_FILENAME,
  SSHD_CFG_FILENAME,
  SSLT_CFG_FILENAME,
  POLICY_CFG_FILENAME,
  TC_CFG_FILENAME,
  TRAP_CFG_FILENAME,
  USER_MGR_CFG_FILENAME,
  L7_VRRP_CFG_FILENAME
};

static L7_char8 sysapiDeleteCfgFileList[][L7_MAX_FILENAME] = {
  "cliWebInitParms.cfg",
  "dot1xInitParms.cfg",
  "pimsmInitParms.cfg",
  "dvmrpInitParms.cfg",
  "pimdmInitParms.cfg",
  "mcastInitParms.cfg",
  "igmpInitParms.cfg",
  "igmpSnoopingInitParms.cfg",
  "relayMap.cfg",
  "relayInitParms.cfg",
  "vrrpInitParms.cfg",
  "ripInitParms.cfg",
  "ospfInitParms.cfg",
  "ipInitParms.cfg",
  "pbVlanInitParms.cfg",
  "fdbInitParms.cfg",
  "snmpInitParms.cfg",
  "simInitParms.cfg",
  "dtlCfgData.cfg",
  "dtlInitParms.cfg",
  "dhcpsLeases.cfg",
  "trafficClassCfgData.cfg",
  "bandwidthBucketCfgData.cfg"
};

static L7_RC_t sysapiCfgFileMapInterfaces (L7_uchar8 * pIpMap, L7_uint32 ipMapLen);

/*********************************************************************
* @purpose  Read config from buffer and fill the config tree
*
* @param    oldCfgFile  (@input)Small config File Name
* @param    pCfgData    (@input)Config Buffer
* @param    cfgLen      (@input)Config Length
*
* @returns  SYSAPI_DIRECTORY_t pointer
*
* @notes
*
* @end
*********************************************************************/

static SYSAPI_DIRECTORY_t *
sysapiCfgFileRel4_0CfgEntryAdd (L7_char8 * oldCfgFile, L7_uchar8 * pCfgData, L7_uint32 cfgLen)
{
  L7_RC_t rc;
  L7_uint32 checksum;
  L7_uint32 newCfgLen;
  L7_uint32 fileindex;
  L7_fileHdr_t *pNewCfgFileHdr;
  L7_fileHdrV1_t *pOldCfgFileHdr;
  L7_uchar8 *data_area;
  SYSAPI_DIRECTORY_t *pSysapiCfgTreeEntry;

  if (pCfgData == L7_NULL || cfgLen == 0)
  {
    LOG_MSG ("Failed to get sizeof Config file %s.\n", oldCfgFile);
    return L7_NULL;
  }

  if (sizeof (L7_fileHdrV1_t) > cfgLen)
  {
    LOG_MSG ("Config file %s size %u is too small.\n", oldCfgFile, cfgLen);
    return L7_NULL;
  }

  /* Get the index of the current file in the config tree */
  fileindex = 0;
  rc = sysapiCfgFileIndexGet (oldCfgFile, &fileindex);
  if (L7_ERROR == rc)
  {
    LOG_MSG ("sysapiCfgFileIndexGet (%s) failed\n", oldCfgFile);
    return L7_NULL;
  }

  pSysapiCfgTreeEntry = pSysapiCfgTree[fileindex];
  if (L7_NULLPTR == pSysapiCfgTreeEntry)
  {
    LOG_MSG ("pSysapiCfgTreeEntry is NULL\n");
    return L7_NULL;
  }

  newCfgLen = cfgLen;
  newCfgLen -= sizeof (L7_fileHdrV1_t); /*  adjust new header length */
  newCfgLen += sizeof (L7_fileHdr_t);   /*  adjust new header length */

  /* Allocate memory for the data storage if not sufficient */
  if (pSysapiCfgTreeEntry->file_location)
    osapiFree (L7_SIM_COMPONENT_ID, pSysapiCfgTreeEntry->file_location);

  data_area = osapiMalloc (L7_SIM_COMPONENT_ID, newCfgLen);
  pSysapiCfgTreeEntry->file_location = data_area;
  if (data_area == L7_NULL)
  {
    LOG_MSG ("osapiMalloc (%u) failed\n", newCfgLen);
    return L7_NULL;
  }

  /* convert the header */
  pNewCfgFileHdr = (L7_fileHdr_t *) data_area;
  pOldCfgFileHdr = (L7_fileHdrV1_t *) pCfgData;
  memset (pNewCfgFileHdr, 0, sizeof (L7_fileHdr_t));
  osapiStrncpySafe (pNewCfgFileHdr->filename, oldCfgFile, sizeof (pNewCfgFileHdr->filename));
  pNewCfgFileHdr->version = pOldCfgFileHdr->version;
  pNewCfgFileHdr->componentID = pOldCfgFileHdr->componentID;
  pNewCfgFileHdr->type = pOldCfgFileHdr->type;
  pNewCfgFileHdr->length = newCfgLen;
  pNewCfgFileHdr->dataChanged = pOldCfgFileHdr->dataChanged;
  pNewCfgFileHdr->fileHdrVersion = L7_FILE_HDR_VER_CURRENT;

  memcpy (data_area + sizeof (L7_fileHdr_t),
          pCfgData + sizeof (L7_fileHdrV1_t), cfgLen - sizeof (L7_fileHdrV1_t));

  /* Fill entry details */
  strncpy (pSysapiCfgTreeEntry->filename, oldCfgFile, L7_MAX_FILENAME);
  pSysapiCfgTreeEntry->filetype = L7_CFG_DATA;
  pSysapiCfgTreeEntry->filesize = newCfgLen;

  newCfgLen = newCfgLen - 4;
  checksum = nvStoreCrc32 (data_area, newCfgLen);
  memcpy (data_area + newCfgLen, &checksum, 4);

  return pSysapiCfgTreeEntry;
}

/*********************************************************************
* @purpose  Read the Small config files and copy to malloc buffer
*
* @param    oldCfgFile  (@input) Small config File Name
* @param    ppCfgData   (@output) Return Buffer Address
* @param    pCfgLen     (@output) Return Length
*
* @returns
*       L7_SUCCESS
*       L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
static L7_RC_t
sysapiCfgFileRel4_0BufferCopy (L7_char8 * cfgFileName, L7_uchar8 ** ppCfgData,
                               L7_uint32 * pCfgLen)
{
  L7_uint32 length;
  L7_uchar8 *pCfgData;
  L7_char8 *cfgFile;

  *ppCfgData = L7_NULLPTR;
  *pCfgLen = 0;

  if (0 == strcmp ("dhcpRelay.cfg", cfgFileName))
  {
    cfgFile = "relayMap.cfg";
  }
  else
  {
    cfgFile = cfgFileName;
  }

  /* get the size of the data area, if file exists */
  length = 0;
  if (L7_SUCCESS != osapiFsFileSizeGet (cfgFile, &length))
  {
     MIGRATE_TRACE ("Failed to get sizeof Config file %s.\n", cfgFile); 
    return L7_ERROR;
  }

  pCfgData = osapiMalloc (L7_SIM_COMPONENT_ID, length);
  if (L7_NULL == pCfgData)
  {
    LOG_MSG ("osapiMalloc (%u) failed\n", length);
    return L7_ERROR;
  }

  if (L7_SUCCESS != osapiFsRead (cfgFile, pCfgData, length))
  {
    LOG_MSG ("Failed to read config file %s data %u\n", cfgFile, length);
    return L7_ERROR;
  }

  *ppCfgData = pCfgData;
  *pCfgLen = length;

  LOG_MSG ("File %s: size = %u\n", cfgFile, length);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Converts uncombined configuration files to combined configuration files.
*
* @param    void    @b{(input)} void
*
* @returns  void
*
* @notes   This routine is used to convert individual release 4.0 configuration files to
*          the combined format used by subsequent releases.  The converted files are
*          to be saved
*
* @end
*********************************************************************/
L7_RC_t
sysapiCfgFileUncombinedToCombinedCfgFileConvert (void)
{
  L7_uint32 i;
  L7_RC_t rc;
  L7_uint32 length;
  L7_uint32 fileindex;
  L7_uint32 cfgFileCount;
  L7_uchar8 *pCfgData;
  L7_fileHdr_t *pHdr;
  L7_char8 cfgFile[L7_MAX_FILENAME];
  SYSAPI_DIRECTORY_t *pSysapiCfgTreeEntry;

  /* take semaphore to ensure that this operation completes without interruption */
  (void) osapiSemaTake (sysapiCfgSema, L7_WAIT_FOREVER);

  /* Check if SYSAPI_CONFIG_FILENAME file exists   */
  strcpy (cfgFile, SYSAPI_CONFIG_FILENAME);
  if (L7_SUCCESS == osapiFsFileSizeGet (cfgFile, &length))
  {
    LOG_MSG ("Config file %s already exists\n", cfgFile);
    (void) osapiSemaGive (sysapiCfgSema);
    return L7_ERROR;
  }

  /*  If the interface mapping cannot be done from a release 4.0 to current version,
     return without attempting to convert any files.  Either this platform was not
     supported in release 4.0, so no conversion needs to be done.  Or, an error exists
     such that the conversion cannot successfully be done.
   */
  length = 0;
  pCfgData = L7_NULL;
  strcpy (cfgFile, L7_IP_CFG_FILENAME);
  rc = sysapiCfgFileRel4_0BufferCopy (cfgFile, &pCfgData, &length);
  if (L7_SUCCESS != rc)
  {
    MIGRATE_TRACE ("Failed to read config file %s header\n", cfgFile);
    length = 0;
  }

  rc = sysapiCfgFileMapInterfaces (pCfgData, length);
  if (L7_NULL != pCfgData)
    osapiFree (L7_SIM_COMPONENT_ID, pCfgData);  /* Allocated in BufferCopy */
  if (L7_SUCCESS != rc)
  {
    LOG_MSG ("sysapiCfgFileMapInterfaces failed\n");
    (void) osapiSemaGive (sysapiCfgSema);
    return L7_ERROR;
  }

  cfgFileCount = (sizeof (sysapiUncombinedCfgFileList) / sizeof (sysapiUncombinedCfgFileList[0]));
  if (SYSAPI_MAX_FILE_ENTRIES < cfgFileCount)
  {
    LOG_MSG ("cfgFileCount > SYSAPI_MAX_FILE_ENTRIES\n");
    (void) osapiSemaGive (sysapiCfgSema);
    return L7_ERROR;
  }

  /* Handle all End User Configuration Files */
  for (i = 0; i < cfgFileCount; i++)
  {
    strcpy (cfgFile, sysapiUncombinedCfgFileList[i]);
    rc = sysapiCfgFileRel4_0BufferCopy (cfgFile, &pCfgData, &length);
    if (L7_SUCCESS == rc)
    {
      (void) sysapiCfgFileRel4_0CfgEntryAdd (cfgFile, pCfgData, length);
      osapiFree (L7_SIM_COMPONENT_ID, pCfgData);        /* Allocated in BufferCopy */
    }
  }

  /* Handle all NIM Configuration Files */
  for (i = 0; i < L7_MAX_SLOTS_PER_BOX_REL_4_0; i++)
  {
    sprintf (cfgFile, "nimSlot%u.cfg", i);
    (void) sysapiCfgFileRel4_0BufferCopy (cfgFile, &pCfgData, &length);
    nimRel4_0SlotInfoRecord (i, pCfgData, length);
    /* nim_migrate will free slotInfo which is allocated while Copy */
  }

  /* give semaphore back */
  (void) osapiSemaGive (sysapiCfgSema);

  /* We will setup tree entry for nimStack here so that the
     nim_migrate is called later */
  fileindex = 0;
  strcpy (cfgFile, "nimStack.cfg");
  rc = sysapiCfgFileIndexGet (cfgFile, &fileindex);
  if (L7_ERROR == rc)
  {
    LOG_MSG ("sysapiCfgFileIndexGet (%s) failed\n", cfgFile);
    return L7_ERROR;
  }

  MIGRATE_TRACE ("nimStack.cfg index = %u\n", fileindex);

  pSysapiCfgTreeEntry = pSysapiCfgTree[fileindex];
  if (L7_NULLPTR == pSysapiCfgTreeEntry)
  {
    LOG_MSG ("pSysapiCfgTreeEntry is NULL\n");
    return L7_ERROR;
  }

  if (L7_NULLPTR != pSysapiCfgTreeEntry->file_location)
  {
    osapiFree (L7_SIM_COMPONENT_ID, pSysapiCfgTreeEntry->file_location);
    pSysapiCfgTreeEntry->file_location = L7_NULLPTR;
  }
  pHdr = osapiMalloc (L7_SIM_COMPONENT_ID, sizeof (L7_fileHdr_t));
  if (pHdr == NULL)
  {
    LOG_MSG ("pHdr is NULL\n");
    return L7_ERROR;
  }
  memset (pHdr, 0, sizeof (L7_fileHdr_t));
  pHdr->length = sizeof (L7_fileHdr_t);
  pSysapiCfgTreeEntry->file_location = pHdr;
  strncpy (pSysapiCfgTreeEntry->filename, cfgFile, L7_MAX_FILENAME);
  pSysapiCfgTreeEntry->filetype = L7_CFG_DATA;
  pSysapiCfgTreeEntry->filesize = sizeof (L7_fileHdr_t);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Create mapping for interfaces from release 4.0 to subsequent release
*
* @param    index           (@input)index of interface entry in release 4.0 config file
* @param    configId        (@output)pointer to current release nimConfigID_t structure
* @param    configIdOffset  (@output)pointer to current release configIdOffset for interface
* @param    intfType        (@output)pointer to interface type (one of L7_INTF_TYPES_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t
sysapiCfgFileRel4_0IndexToIntfInfoGet (L7_uint32 index, nimConfigID_t * configId,
                                       L7_uint32 * configIdOffset, L7_INTF_TYPES_t * intfType)
{
  if (index == 0)
    return L7_FAILURE;

  if (index >= L7_MAX_INTERFACE_COUNT_REL_4_0)
  {
    LOG_MSG ("invalid index %u MAX = %d\n", index, L7_MAX_INTERFACE_COUNT_REL_4_0);
    return L7_ERROR;
  }

  if (L7_NULL != configId)
  {
    (void) nimConfigIdCopy (&rel4_0IntfTypeMap[index].configId, configId);
    if (configId->type == L7_LAG_INTF)
    {
      configId->configSpecifier.dot3adIntf =
        rel4_0IntfTypeMap[index].configId.configSpecifier.usp.port;
    }
  }

  if (L7_NULL != configIdOffset)
  {
    *configIdOffset = index;
  }

  if (L7_NULL != intfType)
  {
    *intfType = rel4_0IntfTypeMap[index].sysIntfType;
  }

  if (rel4_0IntfTypeMap[index].configIdKnown == L7_TRUE)
    return L7_SUCCESS;

  /* LOG_MSG ("unknown configId for index %u\n", index); */
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Create mapping for slot/port from release 4.0 to subsequent release
*
* @param    index           (@input)index of interface entry in release 4.0 config file
* @param    configId        (@output)pointer to current release nimConfigID_t structure
* @param    configIdOffset  (@output)pointer to current release configIdOffset for interface
* @param    intfType        (@output)pointer to interface type (one of L7_INTF_TYPES_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t
sysapiCfgFileRel4_0SlotPortToIntfInfoGet (L7_uint32 slot, L7_uint32 port,
                                          nimConfigID_t * configId,
                                          L7_uint32 * configIdOffset, L7_INTF_TYPES_t * intfType)
{
  if (slot >= L7_MAX_SLOTS_PER_BOX_REL_4_0 || port > L7_MAX_PORTS_PER_SLOT_REL_4_0)
  {
    LOG_MSG (" Invalid slot %u OR port %u\n", slot, port);
    return L7_ERROR;
  }

  return sysapiCfgFileRel4_0IndexToIntfInfoGet (rel4_0SlotPortMap[slot][port],
                                                configId, configIdOffset, intfType);
}

/*********************************************************************
* @purpose  Create mapping for interfaces from release 4.0 to subsequent release
*
* @param    void   void
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
* @returns  L7_FAILURE
*
* @notes    Unsuccessful conversions should result in the configuration reverting
*           to default configuration.
*
*
* @end
*********************************************************************/
static L7_RC_t
sysapiCfgFileMapInterfaces (L7_uchar8 * pIpMap, L7_uint32 ipMapLen)
{
  L7_uint32 i;
  L7_uint32 unit = 1;
  L7_uint32 slot;
  L7_uint32 port;

  L7_BASE_TECHNOLOGY_TYPES_t techType;

  L7_uint32 min_vlan_intIfNum;
  L7_uint32 max_vlan_intIfNum;
  L7_uint32 lag_slot_number;
  L7_uint32 unused_slot_number;
  L7_uint32 vlan_slot_number;
  L7_uint32 cpu_slot_number;

  L7_uint32 min_phys_intIfNum;
  L7_uint32 max_phys_intIfNum;
  L7_uint32 min_cpu_intIfNum;
  L7_uint32 max_cpu_intIfNum;
  L7_uint32 min_lag_intIfNum;
  L7_uint32 max_lag_intIfNum;
  L7_uint32 min_logical_intIfNum;
  L7_uint32 max_logical_intIfNum;
  L7_uint32 min_servport_intIfNum;
  L7_uint32 max_servport_intIfNum;

  L7_uint32 max_phys_port_count;
  L7_uint32 max_phys_slots;
  L7_uint32 max_num_lag_intf;
  L7_uint32 max_cpu_slots;
  L7_uint32 max_num_vlan_intf;
  L7_uint32 max_num_logical_intf;
  L7_uint32 max_num_servport_intf;

  memset (rel4_0IntfTypeMap, 0, sizeof (rel4_0IntfTypeMap));
  memset (rel4_0SlotPortMap, 0, sizeof (rel4_0SlotPortMap));

  techType = cnfgrBaseTechnologyTypeGet ();

    /*----------------------------------------------------------------------------
     The values for intIfNum and slot number in release 4.0 are being
     explicitly mapped. The registry is not being used to avoid the danger
     of constants having been changed in a subsequent release.

     1. Determine intIfNum range for each type of interface.

     Interface ranges were determined by
     FD_CNFGR_NIM_MIN_*_INTF_NUM and FD_CNFGR_NIM_MAX_*_INTF_NUM
     for a particular interface type in release 4.0.
     These are/were defined in default_cnfgr.

     2. Determine value of slot number for each type of interface.

     Slot numbers are determined by the value of L7_*_SLOT_NUM.
     These are/were defined in platform.h.
    ----------------------------------------------------------------------------*/

  switch (techType)
  {
  case L7_BASE_TECHNOLOGY_TYPE_MASTER_DRIVER:

    /* Setup to get intIfNum ranges */
    max_phys_port_count = 16;   /* L7_MAX_PORT_COUNT */
    max_phys_slots = 3;         /* L7_MAX_PHYSICAL_SLOTS_PER_BOX - Slots are 0-based, unlike most other array indices */
    max_num_lag_intf = 8;       /* L7_MAX_NUM_LAG_INTF */
    max_cpu_slots = 1;          /* L7_MAX_CPU_SLOTS_PER_BOX  */
    max_num_vlan_intf = 16;     /* L7_MAX_NUM_ROUTER_INTF   - VLAN interfaces were always router interfaces in release 4.0 */
    max_num_logical_intf = 16;  /* L7_MAX_NUM_VLAN_INTF - unused in base FASTPATH in release 4.0   */
    max_num_servport_intf = 1;  /* L7_MAX_NUM_SERV_PORT_INTF - unused in base FASTPATH in release 4.0   */

    /* The driver indexes the slot by ZERO base */
    /* and the application code is ONE base     */

    lag_slot_number = max_phys_slots + 0;       /* L7_LAG_SLOT_NUM */
    unused_slot_number = lag_slot_number + 1;   /* L7_UNUSED_SLOT_NUM */
    vlan_slot_number = unused_slot_number + 1;  /* L7_VLAN_SLOT_NUM */
    cpu_slot_number = vlan_slot_number + 1;     /* L7_CPU_SLOT_NUM */

    break;

  case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_STRATA:        /* fallthrough */

    /* Setup to get intIfNum ranges */
    max_phys_port_count = 26;   /* L7_MAX_PORT_COUNT */
    max_phys_slots = 2;         /* L7_MAX_PHYSICAL_SLOTS_PER_BOX - Slots are 0-based, unlike most other array indices */
    max_num_lag_intf = 6;       /* L7_MAX_NUM_LAG_INTF */
    max_cpu_slots = 1;          /* L7_MAX_CPU_SLOTS_PER_BOX  */
    max_num_vlan_intf = 16;     /* L7_MAX_NUM_ROUTER_INTF   - VLAN interfaces were always router interfaces in release 4.0 */
    max_num_logical_intf = 6;   /* L7_MAX_NUM_VLAN_INTF - unused in base FASTPATH in release 4.0   */
    max_num_servport_intf = 1;  /* L7_MAX_NUM_SERV_PORT_INTF - unused in base FASTPATH in release 4.0   */

    /* Slot number assignments are the same across both the zeta and omega */
    /* The driver indexes the slot by ZERO base */
    /* and the application code is ONE base     */

    lag_slot_number = max_phys_slots + 0;       /* L7_LAG_SLOT_NUM */
    unused_slot_number = lag_slot_number + 1;   /* L7_UNUSED_SLOT_NUM */
    vlan_slot_number = unused_slot_number + 1;  /* L7_VLAN_SLOT_NUM */
    cpu_slot_number = vlan_slot_number + 1;     /* L7_CPU_SLOT_NUM */

    break;

  case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS:   /* fallthrough */
  default:                     /* for alpha we will assume XGS for default */

    /* Setup to get intIfNum ranges */

#if (LVL7_NUM_PORTS == 48)
    max_phys_port_count = 48;   /* L7_MAX_PORT_COUNT */
#else
    max_phys_port_count = 24;   /* L7_MAX_PORT_COUNT */
#endif

    max_phys_slots = 1;         /* L7_MAX_PHYSICAL_SLOTS_PER_BOX - Slots are 0-based, unlike most other array indices */
    max_num_lag_intf = 6;       /* L7_MAX_NUM_LAG_INTF */
    max_cpu_slots = 1;          /* L7_MAX_CPU_SLOTS_PER_BOX  */
    max_num_vlan_intf = 24;     /* L7_MAX_NUM_ROUTER_INTF   - VLAN interfaces were always router interfaces in release 4.0 */
    max_num_logical_intf = 24;  /* L7_MAX_NUM_VLAN_INTF - unused in base FASTPATH in release 4.0   */
    max_num_servport_intf = 1;  /* L7_MAX_NUM_SERV_PORT_INTF - unused in base FASTPATH in release 4.0   */

    /* Slot number assignments are the same across both the zeta and omega */
    /* The driver indexes the slot by ZERO base */
    /* and the application code is ONE base     */

    lag_slot_number = max_phys_slots + 0;       /* L7_LAG_SLOT_NUM */
    unused_slot_number = lag_slot_number + 1;   /* L7_UNUSED_SLOT_NUM */
    vlan_slot_number = unused_slot_number + 1;  /* L7_VLAN_SLOT_NUM */
    cpu_slot_number = vlan_slot_number + 1;     /* L7_CPU_SLOT_NUM */

    break;
  }

    /*------------------------------------------------------------------------------
    The release 4.0 methodology of assigning interface numbers is the same
     across all reference platform implementations.
    ------------------------------------------------------------------------------*/

  /* Populate data structure in accordance with FD_CNFGR_NIM*  values in release 4_0 */
  min_phys_intIfNum = 1;
  max_phys_intIfNum = max_phys_port_count;
  min_cpu_intIfNum = max_phys_intIfNum + 1;
  max_cpu_intIfNum = min_cpu_intIfNum + (max_cpu_slots - 1);
  min_lag_intIfNum = max_cpu_intIfNum + 1;
  max_lag_intIfNum = min_lag_intIfNum + (max_num_lag_intf - 1);
  min_vlan_intIfNum = max_lag_intIfNum + 1;
  max_vlan_intIfNum = min_vlan_intIfNum + (max_num_vlan_intf - 1);
  min_logical_intIfNum = max_vlan_intIfNum + 1;
  max_logical_intIfNum = min_logical_intIfNum + (max_num_logical_intf - 1);
  min_servport_intIfNum = max_logical_intIfNum + 1;
  max_servport_intIfNum = min_servport_intIfNum + (max_num_servport_intf - 1);

  /* Physical ports */
  slot = 0;
  MIGRATE_TRACE ("MAP Physical ports (slot %u) from %u to %u\n", slot,
                 min_phys_intIfNum, max_phys_intIfNum);
  for (i = min_phys_intIfNum; i <= max_phys_intIfNum; i++)
  {
    port = (i - min_phys_intIfNum) + 1;
    rel4_0IntfTypeMap[i].sysIntfType = L7_PHYSICAL_INTF;
    rel4_0IntfTypeMap[i].slot = slot;

    rel4_0IntfTypeMap[i].configIdKnown = L7_TRUE;
    rel4_0IntfTypeMap[i].configId.type = L7_PHYSICAL_INTF;

    rel4_0IntfTypeMap[i].configId.configSpecifier.usp.unit = unit;
    rel4_0IntfTypeMap[i].configId.configSpecifier.usp.slot = slot;
    rel4_0IntfTypeMap[i].configId.configSpecifier.usp.port = port;

    rel4_0SlotPortMap[slot][port] = i;
  }

  /* CPU ports */
  slot = cpu_slot_number;
  /* Check array bounds before proceeding... */
  if (L7_MAX_SLOTS_PER_BOX_REL_4_0 <= slot)
    return L7_FAILURE;

  MIGRATE_TRACE ("MAP CPU ports (slot %u) from %u to %u\n", slot, 
                 min_cpu_intIfNum, max_cpu_intIfNum);
  for (i = 1; i <= (max_cpu_intIfNum - min_cpu_intIfNum); i++)
  {
    port = i;

    rel4_0IntfTypeMap[i].sysIntfType = L7_CPU_INTF;
    rel4_0IntfTypeMap[i].slot = slot;

    rel4_0IntfTypeMap[i].configIdKnown = L7_TRUE;
    rel4_0IntfTypeMap[i].configId.type = L7_CPU_INTF;

    rel4_0IntfTypeMap[i].configId.configSpecifier.usp.unit = unit;
    rel4_0IntfTypeMap[i].configId.configSpecifier.usp.slot = slot;
    rel4_0IntfTypeMap[i].configId.configSpecifier.usp.port = port;

    rel4_0SlotPortMap[slot][port] = i;
  }

  /* LAG ports */
  slot = lag_slot_number;
  MIGRATE_TRACE ("MAP LAG ports (slot %u) from %u to %u\n", slot, 
                 min_lag_intIfNum, max_lag_intIfNum);
  for (i = 1; i <= (max_lag_intIfNum - min_lag_intIfNum); i++)
  {
    port = i;

    rel4_0IntfTypeMap[i].sysIntfType = L7_LAG_INTF;
    rel4_0IntfTypeMap[i].slot = slot;

    rel4_0IntfTypeMap[i].configIdKnown = L7_TRUE;
    rel4_0IntfTypeMap[i].configId.type = L7_LAG_INTF;

    rel4_0IntfTypeMap[i].configId.configSpecifier.usp.unit = unit;
    rel4_0IntfTypeMap[i].configId.configSpecifier.usp.slot = slot;
    rel4_0IntfTypeMap[i].configId.configSpecifier.usp.port = port;
    /* TBD: Test outcome to validate */

    rel4_0SlotPortMap[slot][port] = i;
  }

  /* VLAN ports */
  slot = vlan_slot_number;
  /* Check array bounds before proceeding... */
  if (L7_MAX_SLOTS_PER_BOX_REL_4_0 <= slot)
    return L7_FAILURE;
  MIGRATE_TRACE ("MAP VLAN ports (slot %u) from %u to %u\n", slot, 
                 min_vlan_intIfNum, max_vlan_intIfNum);
  for (i = 1; i <= (max_vlan_intIfNum - min_vlan_intIfNum); i++)
  {
    port = i;

    rel4_0IntfTypeMap[i].sysIntfType = L7_LOGICAL_VLAN_INTF;
    rel4_0IntfTypeMap[i].slot = slot;

    /* logical interfaces do not always exist */
    rel4_0IntfTypeMap[i].configIdKnown = L7_FALSE;
    rel4_0IntfTypeMap[i].configId.type = L7_LOGICAL_VLAN_INTF;

    rel4_0IntfTypeMap[i].configId.configSpecifier.usp.unit = unit;
    rel4_0IntfTypeMap[i].configId.configSpecifier.usp.slot = slot;
    rel4_0IntfTypeMap[i].configId.configSpecifier.usp.port = port;
    /* TBD: Test outcome to validate */

    rel4_0SlotPortMap[slot][port] = i;
  }

  /* Logical ports - Not used in release 4.0 */

  /* Service Port ports - Not used in release 4.0 */

  /* Get values for VLAN interfaces */
  /* From release 4.0 to release 4.1, the values stored for VLAN interfaces
     moved from the IP Map configuration to the 802.1Q configuration */
  if (ipMapLen > 0)
  {
    (void) ipMapMigrateRel4_0VlanInterfaces (pIpMap, ipMapLen, min_vlan_intIfNum);
    dot1qMigrateRel4_0VlanIntfMask(min_vlan_intIfNum);
    
  }


  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose To fill VLAN interface mapping
*
* @param   mappingIndex  Mapping Internal Interface Number 
* @param   vlanId
* @param   slot
* @param   port
*
* @returns L7_ERROR
* @returns L7_SUCCESS
*
* @end
*
*********************************************************************/
L7_RC_t
sysapiCfgFileMapVlanInterfaces (L7_uint32 mappingIndex, L7_uint32 vlanId,
                                L7_uint32 slot, L7_uint32 port)
{
  if (mappingIndex >= L7_MAX_INTERFACE_COUNT_REL_4_0)
  {
    LOG_MSG ("Map index %u is out of range\n", mappingIndex);
    return L7_ERROR;
  }
  
  rel4_0IntfTypeMap[mappingIndex].configIdKnown = L7_TRUE;
  rel4_0IntfTypeMap[mappingIndex].configId.configSpecifier.vlanId = vlanId;
  rel4_0SlotPortMap[slot][port] = mappingIndex;
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose To fill VLAN interface mapping
*
* @param   mappingIndex  Mapping Rel 4.0 intIfNum
* @param   *vlanId       Pointer to VLAN interface associated with intIfNum
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t
sysapiCfgFileRel4_0VlanIntfGet(L7_uint32 mappingIndex, L7_uint32 *vlanId)
{
  if (mappingIndex >= L7_MAX_INTERFACE_COUNT_REL_4_0)
  {
    LOG_MSG ("Map index %u is out of range\n", mappingIndex);
    return L7_FAILURE;
  }

  if (rel4_0IntfTypeMap[mappingIndex].configId.type == L7_LOGICAL_VLAN_INTF)
  {

      if ( rel4_0IntfTypeMap[mappingIndex].configIdKnown == L7_TRUE)
      {
          *vlanId = rel4_0IntfTypeMap[mappingIndex].configId.configSpecifier.vlanId;
          return L7_SUCCESS;
      }
  }
  return L7_FAILURE;
}


/*********************************************************************
*
* @purpose 
*
* @param    
*
* @returns L7_TRUE
* @returns L7_FALSE
*
* @end
*
*********************************************************************/
static L7_BOOL
sysapiUncombinedCfgFileListSearch (L7_char8 * filename)
{
  L7_uint32 i;
  L7_uint32 cfgFileCount;

  cfgFileCount = (sizeof (sysapiUncombinedCfgFileList) / sizeof (sysapiUncombinedCfgFileList[0]));

  for (i = 0; i < cfgFileCount; i++)
  {
    if (0 == strcmp (sysapiUncombinedCfgFileList[i], filename))
      return L7_TRUE;
  }

  for (i = 0; i < L7_MAX_SLOTS_PER_BOX_REL_4_0; i++)
  {
    char file[20];

    sprintf (file, "nimSlot%u.cfg", i);
    if (0 == strcmp (file, filename))
      return L7_TRUE;
  }

  return L7_FALSE;
}

/*********************************************************************
*
* @purpose 
*
* @param    
*
* @returns L7_TRUE
* @returns L7_FALSE
*
* @end
*
*********************************************************************/
void
sysapiUncombinedCfgRemove (void)
{
  L7_uint32 i;
  L7_uint32 cfgFileCount;

  cfgFileCount = (sizeof (sysapiUncombinedCfgFileList) / sizeof (sysapiUncombinedCfgFileList[0]));

  for (i = 0; i < cfgFileCount; i++)
  {
    (void) osapiFsDeleteFile (sysapiUncombinedCfgFileList[i]);
  }

  for (i = 0; i < L7_MAX_SLOTS_PER_BOX_REL_4_0; i++)
  {
    char file[20];

    sprintf (file, "nimSlot%u.cfg", i);
    (void) osapiFsDeleteFile (file);
  }

  cfgFileCount = (sizeof (sysapiDeleteCfgFileList) / sizeof (sysapiDeleteCfgFileList[0]));

  for (i = 0; i < cfgFileCount; i++)
  {
    (void) osapiFsDeleteFile (sysapiDeleteCfgFileList[i]);
  }
}

/*********************************************************************
*
* @purpose Read the Big Config file from the ramdisk and distribute
*          the `*.cfg` files into a set of memory areas on the ramdisk
*
* @param    cfgFile  the big config file
*
* @returns L7_SUCCESS
* @returns L7_ERROR
* @returns L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t
sysapiCfgFileRel4_0Separate (L7_char8 * bigCfn)
{
  L7_char8 filename[L7_MAX_FILENAME + 1];
  L7_uint32 i;
  L7_uint32 file_size;
  L7_uint32 checksum;
  L7_uint32 read_checksum;
  L7_uint32 bufsz;
  L7_uint32 num_cfg_files;
  L7_char8 *buf;
  L7_char8 *buf_ptr;

  bufsz = 0;

  if (L7_SUCCESS != osapiFsFileSizeGet (bigCfn, &bufsz))
  {
    LOG_MSG ("File size call not successful.\n");
    return (L7_ERROR);
  }

  if (0 == bufsz)
  {
    LOG_MSG ("File %s not found.\n", bigCfn);
    return (L7_ERROR);
  }

  LOG_MSG ("sysapiCfgFileRel4_0Separate: Allocating %u\n", bufsz);
  buf = osapiMalloc (L7_SIM_COMPONENT_ID, bufsz);
  if (L7_NULLPTR == buf)
  {
    LOG_MSG ("Malloc %u Failed.\n", bufsz);
    return (L7_ERROR);
  }
  memset (buf, 0x00, (size_t) bufsz);

  if (L7_SUCCESS != osapiFsRead (bigCfn, buf, (L7_int32) bufsz))
  {
    osapiFree (L7_SIM_COMPONENT_ID, buf);
    return L7_ERROR;
  }

  /* Verify the crc first */
  checksum = nvStoreCrc32 ((L7_uchar8 *) (buf + 4), (L7_uint32) (bufsz - 4));
  read_checksum = *(L7_uint32 *) buf;

  if (checksum != read_checksum)
  {
    LOG_MSG ("CRC check failed. 0x%x read and 0x%x calculated\n", read_checksum, checksum);
    osapiFree (L7_SIM_COMPONENT_ID, buf);
    return L7_ERROR;
  }

  /* Get the number of files in the bigc */
  num_cfg_files = SYSAPI_READ_LONG (buf + 4);

  /* Get the ipMapCfg, read it and create interface map */
  buf_ptr = buf + 4 + sizeof (L7_uint32);
  for (i = 0; i < num_cfg_files; i++)
  {
    memset (filename, 0x00, sizeof (filename));
    osapiStrncpySafe (filename, buf_ptr, sizeof (filename));

    buf_ptr += L7_MAX_FILENAME;
    file_size = SYSAPI_READ_LONG (buf_ptr);
    buf_ptr += sizeof (L7_uint32);

    if (0 == strcmp (filename, L7_IP_CFG_FILENAME))
    {
      if (L7_SUCCESS != sysapiCfgFileMapInterfaces (buf_ptr, file_size))
      {
        LOG_MSG ("sysapiCfgFileMapInterfaces failed\n");
        return L7_ERROR;
      }
    }
    buf_ptr += file_size;
  }

  /* Parse the buffer into multiple cfg files */
  buf_ptr = buf + 4 + sizeof (L7_uint32);
  for (i = 0; i < num_cfg_files; i++)
  {
    memset (filename, 0x00, sizeof (filename));
    osapiStrncpySafe (filename, buf_ptr, sizeof (filename));
    buf_ptr += L7_MAX_FILENAME;
    file_size = SYSAPI_READ_LONG (buf_ptr);
    buf_ptr += sizeof (L7_uint32);

    if (L7_TRUE == sysapiUncombinedCfgFileListSearch (filename))
    {
      LOG_MSG ("Config %s is separated from %s length = %u\n", filename, bigCfn, file_size);
      if (L7_NULL == sysapiCfgFileRel4_0CfgEntryAdd (filename, buf_ptr, file_size))
      {
        LOG_MSG ("sysapiCfgFileRel4_0CfgEntryAdd(%s) Failed.\n", filename);
      }
    }
    else
    {
      LOG_MSG ("Config %s is not separated from %s as we don't need\n", filename, bigCfn);
    }
    buf_ptr += file_size;
  }
  osapiFree (L7_SIM_COMPONENT_ID, buf);
  return (L7_SUCCESS);
}



/*********************************************************************
*
* @purpose Dump config migration info
*
* @param    void
*
* @returns void
*
* @end
*
*********************************************************************/
void
sysapiCfgMigrateDataDump (void)
{
  nimConfigID_t *idp;
  L7_uint32 intfIndex;
  L7_uint32 slot, port;
  rel4_0IntfTypeMap_t *intfTypeMap;
  char *intfName;

  sysapiPrintf ("\n--------------------------------------------------\n");
  sysapiPrintf ("               SLOT-PORT MAPPING                     ");
  sysapiPrintf ("\n--------------------------------------------------\n");

  for (slot = 0; slot < L7_MAX_SLOTS_PER_BOX_REL_4_0; slot++)
  {
    sysapiPrintf ("\nSLOT %d -> ", slot);
    for (port = 0; port <= L7_MAX_PORTS_PER_SLOT_REL_4_0; port++)
    {
      sysapiPrintf ("%02d  ", rel4_0SlotPortMap[slot][port]);
    }
  }
  sysapiPrintf ("\n--------------------------------------------------\n");

  sysapiPrintf ("\n--------------------------------------------------\n");
  sysapiPrintf ("               INTERFACE TYPE MAPPING                 ");
  sysapiPrintf ("\n--------------------------------------------------\n");
  sysapiPrintf ("intfIndex sysIntfType   USP       vlanId    lagId");
  sysapiPrintf ("\n--------------------------------------------------\n");

  for (intfIndex = 0; intfIndex < L7_MAX_INTERFACE_COUNT_REL_4_0; intfIndex++)
  {
    intfTypeMap = &rel4_0IntfTypeMap[intfIndex];
    idp = &intfTypeMap->configId;

    switch (intfTypeMap->sysIntfType)
    {
    case L7_PHYSICAL_INTF:
      intfName = "Physical";
      break;
    case L7_CPU_INTF:
      intfName = "CPU     ";
      break;
    case L7_LAG_INTF:
      intfName = "LAG     ";
      break;
    case L7_LOGICAL_VLAN_INTF:
      intfName = "VLAN";
      break;
    default:
      intfName = "unknown ";
      break;
    }
    sysapiPrintf ("    %02d    %s      %02d/%02d/%02d    %d      %d\n",
                  intfIndex, intfName, idp->configSpecifier.usp.unit,
                  idp->configSpecifier.usp.slot, idp->configSpecifier.usp.port,
                  idp->configSpecifier.vlanId, idp->configSpecifier.dot3adIntf);
  }
  sysapiPrintf ("\n--------------------------------------------------\n");
}


/*********************************************************************
*
* @purpose Determine if this is a valid unit for migration
*
* @param   void
*
* @returns L7_TRUE
* @returns L7_FALSE
*
* @notes    Between release 4.2 and 4.3, the number of valid ports
*           for non-stacking builds decreased due to a reduction
*           in the number of allowable unit IDs. Do not migrate
*           the ports from unsupported boxes.
*
* @end
*
*********************************************************************/
L7_BOOL sysapiCfgFileRel4_3ValidUnit(L7_uint32 unit)
{
  if (unit < L7_MAX_UNITS_PER_STACK) 
      return L7_TRUE;

  return L7_FALSE;

}


/*************************** IFINDEX MIGRATION  ************************************/
/*************************** IFINDEX MIGRATION  ************************************/
/*************************** IFINDEX MIGRATION  ************************************/
/*************************** IFINDEX MIGRATION  ************************************/
/*************************** IFINDEX MIGRATION  ************************************/

#define NIM_UNIT_MASK_REL_4_0 0x0ff00000
#define NIM_SLOT_MASK_REL_4_0 0x000ff000
#define NIM_PORT_MASK_REL_4_0 0x00000fff

#define NIM_UNIT_SHIFT_REL_4_0 20
#define NIM_SLOT_SHIFT_REL_4_0 12
#define NIM_PORT_SHIFT_REL_4_0  0

 /* Shifts for creating unique IF_INDICES */
#define NIM_UNIT_MASK_REL_4_1 0xff000000
#define NIM_SLOT_MASK_REL_4_1 0x00ff0000
#define NIM_PORT_MASK_REL_4_1 0x0000ffff

#define NIM_UNIT_SHIFT_REL_4_1 24
#define NIM_SLOT_SHIFT_REL_4_1 16
#define NIM_PORT_SHIFT_REL_4_1  0



/*********************************************************************
* @purpose  Converts the external interface number from release 4.0 
*           to release 4.1 versions
*
* @param    extIfNum        release 4.0 external interface number (ifIndex)
* @param    *unit           pointer to unit parameter
* @param    *slot           pointer to slot parameter
* @param    *port           pointer to port parameter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
void sysapiCfgFileMigrateExtIfNumToUnitSlotPortRel4_0Get (L7_uint32 extIfNum,  
                                                          L7_uint32 *unit,
                                                          L7_uint32 *slot,
                                                          L7_uint32 *port)
{
    
    *unit = ((extIfNum & NIM_UNIT_MASK_REL_4_0) >> NIM_UNIT_SHIFT_REL_4_0);
    *slot = ((extIfNum & NIM_SLOT_MASK_REL_4_0) >> NIM_SLOT_SHIFT_REL_4_0);
    *port = ((extIfNum & NIM_PORT_MASK_REL_4_0) >> NIM_PORT_SHIFT_REL_4_0);

    return;
     
}



