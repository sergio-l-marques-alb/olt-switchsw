/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    mirror.h
* @purpose     Port mirroring  definitions
* @component   mirroring
* @comments    none
* @create      11/21/2001
* @author      skalyanam
* @end
*             
**********************************************************************/

#ifndef FILTER_H
#define FILTER_H                                    

#include "nimapi.h"
#include "defaultconfig.h"
#include "filter_api.h"
#include "dot1q_api.h"
#include "l7_cnfgr_api.h"


#define FILTER_CFG_FILENAME    "filterCfgData.cfg"
#define FILTER_CFG_VER_1        0x1
#define FILTER_CFG_VER_2        0x2
#define FILTER_CFG_VER_3        0x3
#define FILTER_CFG_VER_4        0x4
#define FILTER_CFG_VER_5        0x5

#define FILTER_CFG_VER_CURRENT  FILTER_CFG_VER_5

#define L7_MAX_STATIC_FILTER_ENTRIES (L7_MAX_FDB_STATIC_FILTER_ENTRIES+L7_MFDB_MAX_MAC_ENTRIES)

typedef struct 
{
  L7_uchar8      macAddr[L7_MAC_ADDR_LEN];
  L7_uint32      vlanId;
  L7_uint32      numSrc;
  L7_BOOL        inUse;
  L7_INTF_MASK_t srcIntfMask; /*uses intIfNum */
  L7_INTF_MASK_t dstIntfMask; /*uses intIfNum */
} filterData_t;

#define FILTER_IS_READY (((filterCnfgrState == FILTER_PHASE_INIT_3) || \
                  (filterCnfgrState == FILTER_PHASE_EXECUTE) || \
                  (filterCnfgrState == FILTER_PHASE_UNCONFIG_1 )) \
                        ? (L7_TRUE) : (L7_FALSE))

typedef struct 
{
  nimConfigID_t  configId;
} filterIntfCfgData_t;

typedef struct
{
  L7_ushort16 vlanId;
  L7_uchar8   mode;
} filterVlanCfgData_t;

typedef struct
{
  L7_fileHdr_t        cfgHdr;
  L7_uchar8           filteringEnabled;
  filterData_t        filterData[L7_MAX_STATIC_FILTER_ENTRIES];
  filterIntfCfgData_t filterIntfCfgData[L7_FILTER_INTF_MAX_COUNT];
  filterVlanCfgData_t filteringVlanCfgData[L7_MAX_VLANS];
  L7_uint32           checkSum;                 /* check sum of config file NOTE: needs to be last entry */
} filterCfgData_t;

typedef struct  filterInfoData_s
{
  L7_uchar8      vlanidmacAddr[L7_FDB_KEY_SIZE];/*include two bytes of vlanid */
  L7_uint32      numSrc;
  L7_INTF_MASK_t srcIntfMask;
  L7_INTF_MASK_t dstIntfMask; /* dstIntf forward Mask */
  L7_INTF_MASK_t dstIntfFilterMask;
  void*          next;
} filterInfoData_t;

typedef enum {
  FILTER_PHASE_INIT_0 = 0,
  FILTER_PHASE_INIT_1,
  FILTER_PHASE_INIT_2,
  FILTER_PHASE_WMU,
  FILTER_PHASE_INIT_3,
  FILTER_PHASE_EXECUTE,
  FILTER_PHASE_UNCONFIG_1,
  FILTER_PHASE_UNCONFIG_2
} filterCnfgrState_t;
  
typedef struct {
  L7_BOOL filterSave;
  L7_BOOL filterRestore;
  L7_BOOL filterHasDataChanged;
  L7_BOOL filterIntfChangeCallback;
  L7_BOOL filterVlanChangeCallback;
} filterDeregister_t;


#define FILTER_MAX_RESTRICTED_ADDR  34

/*********************************************************************
* @purpose  Saves filter user config file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t filterSave(void);
/*********************************************************************
* @purpose  Restores filter user config file to factore defaults
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t filterRestore(void);
/*********************************************************************
* @purpose  Checks if filter user config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_BOOL filterHasDataChanged(void);
void filterResetDataChanged(void);
/*********************************************************************
* @purpose  Apply static MAC Filtering Configuration Data
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t filterApplyConfigData(void);

/*********************************************************************
* @purpose  Apply Filter Interface Config Data
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t filterApplyIntfConfigData(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Build default filter config data  
*
* @param    ver   Software version of Config Data
*
* @returns  none
*
* @notes    none 
*
* @end
*********************************************************************/
void filterBuildDefaultConfigData(L7_uint32 ver);

/*********************************************************************
* @purpose  Copies filter user config file from Info to the Cfg structure
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t filterDataCopy();

/*********************************************************************
* @purpose  Build default filter intf config data  
*
* @parms    configId  the id to be placed into intf config
* @parms    pCfg, a pointer to the interface structure
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
void filterBuildDefaultIntfConfigData(nimConfigID_t *configId, 
                                      filterIntfCfgData_t *pCfg);

/*********************************************************************
* @purpose  Obtain a pointer to the first free interface config struct
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to filter interface config structure
*                           or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled or not).
*
* @notes    
*
* @end
*********************************************************************/
L7_BOOL filterMapIntfConfigEntryGet(L7_uint32 intIfNum, filterIntfCfgData_t **pCfg);

/*********************************************************************
*
* @purpose  Get persistent configuration entry for a vlan
*
* @param    L7_ushort16   vlanId    @b((input))  VLAN identifier
* @param    filterVlanCfgData_t *vlanCfg  @b((output)) persistent cfg entry
*
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t filterVlanCfgEntryGet(L7_ushort16 vlanId, filterVlanCfgData_t **vlanCfg);

/*********************************************************************
* @purpose  Obtain a pointer to the specified interface configuration 
*           data for this interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr to filter interface config structure
*                                  or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled or not).
*
* @notes    The caller can set the pCfg parm to L7_NULL if it does not
*           want the value output from this function.
*
* @end
*********************************************************************/
L7_BOOL filterMapIntfIsConfigurable(L7_uint32 intIfNum, filterIntfCfgData_t **pCfg);

/*********************************************************************
* @purpose  Process the Callback for L7_CREATE
*
* @param    intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t filterIntfCreate(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Process the Callback for L7_DETACH
*
* @param    intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t filterIntfDetach(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Process the Callback for L7_DELETE
*
* @param    intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t filterIntfDelete(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Process NIM events
*
* @param    intIfNum    internal interface number
* @param    event       event, defined by L7_PORT_EVENTS_t
* @param    correlator  event, defined by L7_PORT_EVENTS_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t filterIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event, 
                                 NIM_CORRELATOR_t correlator);

/*********************************************************************
* @purpose  Propogate Startup notifications to static filter component
*
* @param    startupPhase
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
void filterStartupNotifyCallback(NIM_STARTUP_PHASE_t startupPhase);

/*********************************************************************
* @purpose  To process Vlan change events.
*
* @param    vlanID      vlanId 
* @param    intIfNum     internal interface whose state has changed
* @param    event       event, defined by L7_PORT_EVENTS_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t filterVlanChangeCallback(dot1qNotifyData_t *vlanData,L7_uint32 intIfNum,L7_uint32 event);

/*********************************************************************
* @purpose  This function process the configurator control commands/request 
*           pair Unconfigure Phase 1.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*     
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t filterCnfgrUconfigPhase1(L7_CNFGR_RESPONSE_t *pResponse,
                                 L7_CNFGR_ERR_RC_t   *pReason);

/*********************************************************************
* @purpose  Delete all configured filters
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t filterDeleteAllEntries(void);


/*********************************************************************/
/***********************filter_util.c prototypes**********************/
/*********************************************************************/
/*********************************************************************
*
* @purpose  Apply a change to global filtering configuration
*
* @param    L7_BOOL  enable
*
* @returns  L7_SUCCESS,
* @returns  L7_FAILURE,
*
* @end
*
*********************************************************************/
L7_RC_t filterAdminModeApply(L7_BOOL enable);

/*********************************************************************
* @purpose  Removes a node entry from the registry
*          
* @param    L7_uchar8*  macAddr  @b((input)) MAC Address of the filter
* @param    L7_uint32   vlanId   @b{(input)} Vlan ID for the Filter
*   
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t filterEntryDelete(L7_uchar8* macAddr, 
                          L7_uint32 vlanId);
/*********************************************************************
* @purpose  adds a node entry from the registry
*          
* @param    L7_uchar8*  macAddr  @b((input)) MAC Address of the filter
* @param    L7_uint32   vlanId   @b{(input)} Vlan ID for the Filter
*   
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t filterEntryAdd(L7_uchar8* macAddr,
                       L7_uint32 vlanId);

/*********************************************************************
* @purpose  Finds a node entry from the registry
*          
* @param    L7_uchar8*  macAddr  @b((input)) MAC Address of the filter
* @param    L7_uint32   vlanId   @b{(input)} Vlan ID for the Filter
* @param    L7_uint32   flag     @b{(input)} Flag type for search
*                                            L7_MATCH_EXACT   - Exact match
*                                            L7_MATCH_GETNEXT - Next entry greater than this one
*   
* @returns  filter entry pointer on success
* @returns  null pointer on failure
*
* @notes none
*
* @end
*
*********************************************************************/
filterInfoData_t *filterEntryFind(L7_uchar8* macAddr,
                                  L7_uint32 vlanId,
                                  L7_uint32 flag);

/*********************************************************************
*
* @purpose  Is a particular interface present in the list of interfaces 
*
* @param    L7_uint32   list[]  @b((input))  List of interface numbers
* @param    L7_uint32   listSize@b{(input)}  Number of valid entries in the list
* @param    L7_uint32   intf    @b{(input)}  The interface number to search for
* @param    L7_uint32*  index   @b{(output)} The index into the List provided if interface found
*
* @returns  L7_TRUE, if the interface exists in the list
* @returns  L7_FALSE, if the interface in not in the list
*
* @notes    
*
* @end
*********************************************************************/
L7_BOOL filterIsIntfPresent(L7_uint32 list[], 
                            L7_uint32 listSize,
                            L7_uint32 intf, 
                            L7_uint32* index);
/*********************************************************************
*
* @purpose  Checks if the MAC Address is not within a set restricted MAC Addresses 
*
* @param    L7_uchar8*  macAddr      @b((input))  MAC Address of the filter
*
* @returns  L7_SUCCESS, if mac address in not restricted
* @returns  L7_FAILURE, if mac address is one of the restricted addresses
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t filterMacAddrCheck(L7_uchar8* macAddr);

/*********************************************************************
*
* @purpose  To check the mac address type, unicast or multicast.
*
* @param    L7_uchar8  *macAddr   @b((input)) MAC Address
*
* @returns  L7_TRUE, if MAC address is of type multicast
* @returns  L7_FALSE, if MAC address is of type unicast
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL filterIsMacAddrTypeMulticast(L7_uchar8 *macAddr);

/*********************************************************************
*
* @purpose  Extracts the vlan Id and Mac address from the 8 byte combination
*
* @param    L7_uchar8*  combined  @b{(input)}  Combined MAC and vlanId 
* @param    L7_uchar8*  macAddr   @b((output)) MAC Address of the filter
* @param    L7_uint32*  vlanId    @b{(output)} Vlan ID for the Filter
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if there is no MAC Filter
*
* @notes    First 2 bytes are the vlanId and the next 6 bytes the MAC address
*
* @end
*********************************************************************/
L7_RC_t filterMacVlanIdExtract(L7_uchar8* combined, 
                               L7_uchar8* macAddr, 
                               L7_uint32* vlanId);
/*********************************************************************
*
* @purpose  Checks if the Interface type is valid for filters
*
* @param    L7_uint32 intfType  @b((input))  Interface Type
*
* @returns  L7_TRUE, if interface type is acceptable
* @returns  L7_FALSE, if interface type is not acceptable
*
* @end
*********************************************************************/
L7_BOOL filterIsValidIntfType(L7_INTF_TYPES_t intfType);

/*********************************************************************
*
* @purpose  Checks if the Interface is valid for filters 
*
* @param    L7_uint32 intIfNum  @b((input))  Internal Interface Number
*
* @returns  L7_SUCCESS, if interface is acceptable
* @returns  L7_FAILURE, if interface is not acceptable
*
* @notes    Acceptability criteria
*           1. Must be a valid configured interface number &&
*           2. Must be a physical or LAG interface         &&
*           3. Must not be a LAG member
*           
*
* @end
*********************************************************************/
L7_RC_t filterIntfCheck(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 1.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*     
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t filterCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason );

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 1.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*     
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t filterCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason );

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 1.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*     
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t filterCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason );

/*********************************************************************
* @purpose  This function undoes filterCnfgrInitPhase1Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void filterCnfgrFiniPhase1Process();

/*********************************************************************
* @purpose  This function undoes filterCnfgrInitPhase2Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void filterCnfgrFiniPhase2Process();

/*********************************************************************
* @purpose  This function undoes filterCnfgrInitPhase3Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void filterCnfgrFiniPhase3Process();


/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair as a NOOP.   
*
* @param    pResponse - @b{(output)}  Response always command complete.
*     
* @param    pReason   - @b{(output)}  Always 0                    
*
* @returns  L7_SUCCESS - Always return this value. onse is available.
*
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           None.
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t filterCnfgrNoopProcess( L7_CNFGR_RESPONSE_t *pResponse,
                                 L7_CNFGR_ERR_RC_t   *pReason );

/*********************************************************************
*
* @purpose  Make dtl calls for adding/removing interfaces from mac filters
*
* @param    L7_uchar8*  macAddr   @b((input)) Static MAC Address for the filter
* @param    L7_uint32   vlanId    @b{(input)} Vlan ID for the Filter  
* @param    filterInfoData_t   filterEntry  @b{(input)} Filter entry structure
*               containing list of source and destination ports that are part 
*               of the filter
*                            
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ALREADY_CONFIGURED, if the filter already exists
* @returns  L7_FAILURE, if other failure
*
* @notes    This sole purpose of this function is to parse the list of ALL 
*           interfaces that are assigned to the mac filter and make a dtl call 
*           with only the SUBSET of this list which comprises of ATTACHED or 
*           ATTACHING interfaces
*
* @end
*********************************************************************/
L7_RC_t filterIntfModify(L7_uchar8* macAddr, L7_uint32 vlanId, filterInfoData_t  *filterEntry);


/* filter_migrate.c */
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
void filterMigrateConfigData(L7_uint32 oldVer , L7_uint32 ver, L7_char8 *pCfgBuffer);

/*********************************************************************
* @purpose  Adds an entry to mfdb . If the entry has already been
*           added, add the passed set of ports to the existing entry.
*          
* @param    L7_uint32   vlanId   @b{(input)} Vlan ID for the Filter
* @param    L7_uchar8*  macAddr  @b((input)) MAC Address of the filter
* @param    L7_INTF_MASK_t   dstIntfMask     @b{(input)} The list of ports
*                                            configured for the entry
*	
* @returns  L7_SUCCESS on succes
* @returns  L7_FAILURE otherwise
*
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t filterMfdbEntryAdd(L7_uint32 vlanId,L7_uchar8* macAddr, 
                           L7_INTF_MASK_t *dstIntfMask, L7_INTF_MASK_t *filterIntfMask);

/*********************************************************************
* @purpose  Deletes an entry from the mfdb. 
*          
* @param    L7_uint32   vlanId   @b{(input)} Vlan ID for the Filter
* @param    L7_uchar8*  macAddr  @b((input)) MAC Address of the filter
* @param    L7_INTF_MASK_t   dstIntfMask     @b{(input)} The list of ports
*                                            configured for the entry
*           L7_BOOL  entryPresent  @b((input)) True if the entry has already 
*                                             been added to the database
*	
* @returns  L7_SUCCESS on succes
* @returns  L7_FAILURE otherwise
*
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t filterMfdbEntryDelete(L7_uint32 vlanId,L7_uchar8* macAddr);

/*********************************************************************
* @purpose  Deletes the set of ports from an mfdb entry
*          
* @param    L7_uint32   vlanId   @b{(input)} Vlan ID for the Filter
* @param    L7_uchar8*  macAddr  @b((input)) MAC Address of the filter
* @param    L7_INTF_MASK_t   dstIntfMask     @b{(input)} The list of ports
*                                            configured for the entry
*	
* @returns  L7_SUCCESS on succes
* @returns  L7_FAILURE otherwise
*
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t filterMfdbEntryPortsDelete(L7_uint32 vlanId,L7_uchar8* macAddr, L7_INTF_MASK_t *dstIntfMask,
                                   L7_INTF_MASK_t *dstIntfFilterMask);


#endif /*FILTER_H*/

