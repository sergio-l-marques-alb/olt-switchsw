/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename pml.h
*
* @purpose Port MAC Locking definitions
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

#ifndef PML_H
#define PML_H

#include "nimapi.h"
#include "defaultconfig.h"
#include "avl_api.h"
#include "pml_api.h"
#include "sysnet_api.h"
#include "l7_product.h"
#include "comm_mask.h"
#include "fdb_exports.h"
#include "l7_cnfgr_api.h"

#define PML_QUEUE           "pml_Queue"
#define PML_MSG_SIZE        sizeof(pmlMgmtMsg_t)

#define PML_CFG_FILENAME    "pmlCfgData.cfg"
#define PML_CFG_VER_1       0x1
#define PML_CFG_VER_2       0x2
#define PML_CFG_VER_CURRENT PML_CFG_VER_2

#define PML_IS_READY (((pmlCnfgrState == PML_PHASE_INIT_3) || \
                       (pmlCnfgrState == PML_PHASE_EXECUTE) ||          \
                       (pmlCnfgrState == PML_PHASE_UNCONFIG_1)) ? (L7_TRUE) : (L7_FALSE))

#define PML_VIOLATION_TREE_SIZE 100

typedef enum
{
  PML_PHASE_INIT_0 = 0,
  PML_PHASE_INIT_1,
  PML_PHASE_INIT_2,
  PML_PHASE_WMU,
  PML_PHASE_INIT_3,
  PML_PHASE_EXECUTE,
  PML_PHASE_UNCONFIG_1,
  PML_PHASE_UNCONFIG_2,
} pmlCnfgrState_t;

typedef struct
{
  L7_enetMacAddr_t macAddr;
  L7_ushort16      vlanId;
} pmlMac_t;

typedef struct
{
  nimConfigID_t      configId;
  L7_uint32          violationTrapsSeconds;
  L7_BOOL            intfLockEnabled;
  L7_BOOL            violationTrapsEnabled;
  L7_uint32          dynamicLimit;
  L7_uint32          staticLimit;
  pmlMac_t           staticMacEntry[L7_MACLOCKING_MAX_STATIC_ADDRESSES];
} pmlIntfCfgData_t;

typedef struct
{
  L7_uchar8           vlanIdMacAddr[L7_FDB_KEY_SIZE]; /* Include two bytes of VLAN ID */
  void               *next;
} pmlLockInfoData_t;

typedef struct
{
  L7_uint32          staticCount;
  L7_uint32          dynamicCount;
  avlTree_t          violationAvlTree;
  avlTreeTables_t   *violationTreeHeap;
  pmlLockInfoData_t *violationDataHeap;
  pmlMac_t           lastViolationAddr;
  L7_uint32          lastViolationTrap;
} pmlIntfOprData_t;

typedef struct
{
  L7_fileHdr_t       cfgHdr;
  L7_BOOL            globalLockEnabled;
  pmlIntfCfgData_t   pmlIntfCfgData[L7_PML_MAX_INTF];
  L7_uint32          checkSum;      /* check sum of config file NOTE: needs to be last entry */
} pmlCfgData_t;

typedef struct pmlIntfChangeParms_s
{
  L7_uint32 event;
  NIM_CORRELATOR_t correlator;
} pmlIntfChangeParms_t;

/*  Port MAC Locking Management Message IDs */
typedef enum
{
  pmlCnfgr = 1,
  pmlIntfChange
} pmlMgmtMessages_t;

typedef struct
{
  L7_uint32        msgId;    /* Of type pmlMgmtMessages_t */
  L7_uint32        intIfNum;
  L7_uint32        vlanId;
  union
  {
    L7_CNFGR_CMD_DATA_t  CmdData;
    pmlIntfChangeParms_t pmlIntfChangeParms;
  } u;
} pmlMgmtMsg_t;

/*********************************************************************
*
* @purpose  System Initialization for Port MAC Locking component
*
* @param    none
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlInit(L7_CNFGR_CMD_DATA_t *pCmdData);

/*********************************************************************
*
* @purpose  System Init Undo for Port MAC Locking component
*
* @param    none
*
* @notes    none
*
* @end
*********************************************************************/
void pmlInitUndo();

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
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @end
*********************************************************************/
L7_RC_t pmlCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason );

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 2.
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
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @end
*********************************************************************/
L7_RC_t pmlCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason );

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 3.
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
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @end
*********************************************************************/
L7_RC_t pmlCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason );

/*********************************************************************
* @purpose  This function undoes pmlCnfgrInitPhase1Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void pmlCnfgrFiniPhase1Process();

/*********************************************************************
* @purpose  This function undoes pmlCnfgrInitPhase2Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void pmlCnfgrFiniPhase2Process();

/*********************************************************************
* @purpose  This function undoes pmlCnfgrInitPhase2Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void pmlCnfgrFiniPhase3Process();

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
* @end
*********************************************************************/
L7_RC_t pmlCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
                              L7_CNFGR_ERR_RC_t   *pReason );

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Unconfigure Phase 2.
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
* @end
*********************************************************************/

L7_RC_t pmlCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                               L7_CNFGR_ERR_RC_t   *pReason );

/*********************************************************************
*
* @purpose  To parse the configurator commands send to pmlTask
*
* @param    none
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
void pmlCnfgrParse(L7_CNFGR_CMD_DATA_t *pCmdData);

/*********************************************************************
* @purpose  Saves Port MAC Locking file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 pmlSave(void);

/*********************************************************************
* @purpose  Checks if Port MAC Locking user config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL pmlHasDataChanged(void);
void pmlResetDataChanged(void);
/*********************************************************************
* @purpose  Apply static Port MAC Locking Configuration Data
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlApplyConfigData(void);

/*********************************************************************
* @purpose  Build default Port MAC Locking Intf config data
*
* @parms    config Id, the config Id to be placed into the intf config
* @parms    pCfg, a pointer to the interface structure
*
* @returns  none
*
*
* @end
*********************************************************************/
void pmlBuildDefaultIntfConfigData(nimConfigID_t *configId, pmlIntfCfgData_t *pCfg);

/*********************************************************************
* @purpose  Build default Port MAC Locking config data
*
* @param    ver   Software version of Config Data
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void pmlBuildDefaultConfigData(L7_uint32 ver);

/*********************************************************************
* @purpose  Get all currently configured VLANs
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlAllVlansGet();

/*********************************************************************
*
* @purpose  Create a Multicast MAC entry
*
* @param    L7_uchar8   *macAddr   @b((input)) IP Multicast MAC Address
* @param    L7_uint32   vlanId     @b{(input)} Vlan ID
*
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlEntryCreate(L7_uchar8 *macAddr, L7_uint32 vlanId);

/*********************************************************************
*
* @purpose  Adds a interface to the port list of the multicast address entry
*
* @param    L7_uchar8   *macAddr   @b((input)) IP Multicast MAC Address
* @param    L7_uint32   vlanId     @b{(input)} Vlan ID for the MAC Address
* @param    L7_uint32   intIfNum   @b{(input)} Internal Interface number
*
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    adds interface at the end of densely packed list
*
* @end
*********************************************************************/
L7_RC_t pmlIntfAdd(L7_uchar8 *macAddr, L7_uint32 vlanId, L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  Deletes an interface from the port list of the multicast address entry
*
* @param    L7_uchar8*  macAddr   @b((input)) Multicast MAC Address
* @param    L7_uint32   vlanId    @b{(input)} Vlan ID for the MAC Address
* @param    L7_uint32   intIfNum  @b{(input)} Internal Interface number
*
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    deletes intf from list and reorders list densely
*
* @end
*********************************************************************/
L7_RC_t pmlIntfRemove(L7_uchar8* macAddr,
                        L7_uint32 vlanId,
                        L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  Removes an entry from the multicast address table
*
* @param    L7_uchar8*  macAddr   @b((input)) Multicast MAC Address
* @param    L7_uint32   vlanId    @b{(input)} Vlan ID for the MAC Address
*
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 pmlEntryRemove(L7_uchar8 *macAddr, L7_uint32 vlanId);

/*********************************************************************
*
* @purpose  Extracts the vlan Id and Mac address from the 8 byte combination
*
* @param    L7_uchar8   *combined  @b{(input)}  Combined MAC and vlanId
* @param    L7_uchar8   *macAddr   @b((output)) Multicast MAC Address
* @param    L7_uint32   *vlanId    @b{(output)} Vlan ID for the MAC Address
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if there is no
*
* @notes    First 2 bytes are the vlanId and the next 6 bytes the MAC address
*
* @end
*********************************************************************/
L7_RC_t pmlMacVlanIdExtract(L7_uchar8* combined,
                              L7_uchar8* macAddr,
                              L7_uint32* vlanId);

/*********************************************************************
* @purpose  Start Port MAC Locking task
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlStartTask();

/*********************************************************************
*
* @purpose task to handle all Port MAC Locking management messages
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
void pmlTask();

/*********************************************************************
*
* @functions pmlInfoShow
*
* @purpose   Displays all config and nonconfig info for Port MAC Locking
*
* @param     none
*
* @returns   L7_SUCCESS
*
* @comments  Used for debug
*
* @end
*
*********************************************************************/
L7_RC_t pmlInfoShow();

/*********************************************************************
* @purpose  Migrate old Port MAC Locking config to new
*
* @param   oldVer  @b{(input)} Old software version of Config Data
* @param   ver     @b{(input)} Expected software version of Config Data
* @param   *buffer @b{(input)} Outdated config buffer
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void pmlMigrateConfigData(L7_uint32 oldVer , L7_uint32 ver, L7_char8 *pCfgBuffer);

#endif /* PML_H */

