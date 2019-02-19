/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src\policy\std_policy.h
*
* @purpose Contains prototypes and Data Structures
*          to support the FlowControl and BroadCastStorm
*
* @component
*
* @comments
*
* @create 04/02/2001
*
* @author rjindal
* @end
*
**********************************************************************/

#ifndef INCLUDE_STD_POLICY_H
#define INCLUDE_STD_POLICY_H

#include "nimapi.h"
#include "default_cnfgr.h"
#include "dtlapi.h"
#include "l7_cnfgr_api.h"

typedef enum {
  POLICY_PHASE_INIT_0 = 0,
  POLICY_PHASE_INIT_1,
  POLICY_PHASE_INIT_2,
  POLICY_PHASE_WMU,
  POLICY_PHASE_INIT_3,
  POLICY_PHASE_EXECUTE,
  POLICY_PHASE_UNCONFIG_1,
  POLICY_PHASE_UNCONFIG_2,
} policyCnfgrState_t;

typedef struct {
  L7_BOOL policySave;
  L7_BOOL policyRestore;
  L7_BOOL policyHasDataChanged;
  L7_BOOL policyIntfChangeCallback;
} policyDeregister_t;

typedef struct
{
  nimConfigID_t  configId;
  L7_uint32      flowControlMode;
  L7_uint32      bcastStormMode;
  L7_uint32      bcastStormThreshold;
  L7_RATE_UNIT_t bcastStormThresholdUnit;
  L7_uint32      mcastStormMode;
  L7_uint32      mcastStormThreshold;
  L7_RATE_UNIT_t mcastStormThresholdUnit;
  L7_uint32      ucastStormMode;
  L7_uint32      ucastStormThreshold;
  L7_RATE_UNIT_t ucastStormThresholdUnit;
} policyIntfCfgData_t;

typedef struct
{
  L7_fileHdr_t   cfgHdr;
  L7_uint32      systemFlowControlMode;
  L7_uint32      systemBcastStormMode;
  L7_uint32      systemBcastStormThreshold;
  L7_RATE_UNIT_t systemBcastStormThresholdUnit;
  L7_uint32      systemMcastStormMode;
  L7_uint32      systemMcastStormThreshold;
  L7_RATE_UNIT_t systemMcastStormThresholdUnit;
  L7_uint32      systemUcastStormMode;
  L7_uint32      systemUcastStormThreshold;
  L7_RATE_UNIT_t systemUcastStormThresholdUnit;
  policyIntfCfgData_t policyIntfCfgData[L7_POLICY_INTF_MAX_COUNT];
  L7_uint32      checkSum;    /* check sum of config file NOTE: needs to be last entry */
} policyCfgData_t;

#define POLICY_CFG_FILENAME     "policy.cfg"
#define POLICY_CFG_VER_1        0x1
#define POLICY_CFG_VER_2        0x2
#define POLICY_CFG_VER_3        0x3
#define POLICY_CFG_VER_4        0x4
#define POLICY_CFG_VER_5        0x5
#define POLICY_CFG_VER_CURRENT  POLICY_CFG_VER_5

#define POLICY_IS_READY (((policyCnfgrState == POLICY_PHASE_INIT_3) || \
                         (policyCnfgrState == POLICY_PHASE_EXECUTE) || \
                         (policyCnfgrState == POLICY_PHASE_UNCONFIG_1)) ? (L7_TRUE) : (L7_FALSE))

/*********************************************************************
* @purpose  Build default policy intf config data
*
* @parms    config Id, the config Id to be placed into the intf config
* @parms    pCfg, a pointer to the interface structure
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void policyBuildDefaultIntfConfigData(nimConfigID_t *configId, policyIntfCfgData_t *pCfg);

/*********************************************************************
* @purpose  Build default policy config data
*
* @param    ver   Software version of Config Data
*
* @returns  void
*
* @notes    Builds Garp, VLAN and port default data which will be
*           applied in dot1qApplyConfigData
*           Also inits the config file header
*
* @end
*********************************************************************/
void policyBuildDefaultConfigData(L7_uint32 ver);

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
void policyMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer);

/*********************************************************************
* @purpose  Applies std policy config data
*
* @param    void
*
* @returns  L7_SUCCESS
* @return   L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t policyApplyConfigData(void);

/*********************************************************************
*
* @purpose  To process the Callback for L7_PORT_INSERT
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
L7_RC_t policyApplyIntfConfigData(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Apply flow control for an interface
*
* @param    intIfNum    intIfNum
*
* @param    mode        admin mode,
*                       (@b{  L7_ENABLE or
*                             L7_DISABLE})
*
* @returns  L7_RC_T
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t policyIntfFlowCtrlModeApply(L7_uint32 intIfNum, L7_uint32 mode);

/*********************************************************************
* @purpose  Apply the broadcast rate limiting for an interface
*
* @param    intIfNum    intIfNum
*
* @param    mode        admin mode,
*                       (@b{  L7_ENABLE or
*                             L7_DISABLE})
*
* @param    threshold   threshold value
*
* @param    rate_unit   L7_RATE_UNIT_PERCENT or L7_RATE_UNIT_PPS or L7_RATE_UNIT_KBPS
*
* @returns  L7_RC_T
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t policyIntfBcastCtrlModeApply(L7_uint32 intIfNum, L7_uint32 mode, L7_int32 threshold,
                                     L7_RATE_UNIT_t rate_unit);

/*********************************************************************
* @purpose  Apply the multicast rate limiting for an interface
*
* @param    intIfNum    intIfNum
*
* @param    mode        admin mode,
*                       (@b{  L7_ENABLE or
*                             L7_DISABLE})
*
* @param    threshold   threshold value
*
* @param    rate_unit   L7_RATE_UNIT_PERCENT or L7_RATE_UNIT_PPS or L7_RATE_UNIT_KBPS
*
* @returns  L7_RC_T
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t policyIntfMcastCtrlModeApply(L7_uint32 intIfNum, L7_uint32 mode, L7_int32 threshold,
                                     L7_RATE_UNIT_t rate_unit);


/*********************************************************************
* @purpose  Apply the destination lookup failure rate limiting for an interface
*
* @param    intIfNum    intIfNum
*
* @param    mode        admin mode,
*                       (@b{  L7_ENABLE or
*                             L7_DISABLE})
*
* @param    threshold   threshold value
*
* @param    rate_unit   L7_RATE_UNIT_PERCENT or L7_RATE_UNIT_PPS or L7_RATE_UNIT_KBPS
*
* @returns  L7_RC_T
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t policyIntfUcastCtrlModeApply(L7_uint32 intIfNum, L7_uint32 mode, L7_int32 threshold,
                                     L7_RATE_UNIT_t rate_unit);


/*********************************************************************
* @purpose  Checks if policy user config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL policyHasDataChanged(void);
void policyResetDataChanged(void);

/*********************************************************************
* @purpose  Saves policy user config file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t policySave(void);

/*********************************************************************
* @purpose  Restores dot1d user config file to factore defaults
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 policyRestore(void);

/*********************************************************************
* @purpose  Register all policy counters with the Statistics Manager
*
* @param    None
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @end
*********************************************************************/
L7_RC_t policyStatsCreate();

/*********************************************************************
* @purpose  Register all policy counters with the Statistics Manager
*           for the specified interface.
*
* @param    L7_uint32 intIfNum internal interface number
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @end
*********************************************************************/
L7_RC_t policyStatsCreateIntf(L7_uint32 intfNum);

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
L7_RC_t policyIntfCreate(L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  To process the Callback for L7_DETACH
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
L7_RC_t policyIntfDetach(L7_uint32 intIfNum);

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
L7_RC_t policyIntfDelete(L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  To process NIM events.
*
* @param    L7_uint32  intIfNum  internal interface number
* @param    L7_uint32  event     event, defined by L7_PORT_EVENTS_t
* @param    L7_uint32  correlator event, defined by L7_PORT_EVENTS_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t policyIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event, NIM_CORRELATOR_t correlator);

/*********************************************************************
* @purpose  Obtain a pointer to the specified interface configuration data
*           for this interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to policy interface config structure
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
* @notes    The caller can set the pCfg parm to L7_NULL if it does not
*           want the value output from this function.
*
* @end
*********************************************************************/
L7_BOOL policyMapIntfIsConfigurable(L7_uint32 intIfNum, policyIntfCfgData_t **pCfg);

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
L7_RC_t policyCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
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
L7_RC_t policyCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
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
L7_RC_t policyCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason );

/*********************************************************************
* @purpose  This function undoes policyCnfgrInitPhase1Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void policyCnfgrFiniPhase1Process();

/*********************************************************************
* @purpose  This function undoes policyCnfgrInitPhase2Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void policyCnfgrFiniPhase2Process();

/*********************************************************************
* @purpose  This function undoes policyCnfgrInitPhase3Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void policyCnfgrFiniPhase3Process();

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
L7_RC_t policyCnfgrNoopProcess( L7_CNFGR_RESPONSE_t *pResponse,
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
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*
* @end
*********************************************************************/
L7_RC_t policyCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                  L7_CNFGR_ERR_RC_t   *pReason );

#endif /* INCLUDE_STD_POLICY_H */
