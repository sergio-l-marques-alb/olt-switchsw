/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename src\application\base\dos\dos.h
*
* @purpose Contains prototypes and Data Structures
*          to support the Denial of Service function      
*
* @component DOSCONTROL
*
* @comments
*
* @create 04/04/2005
*
* @author esmiley
* @end
*
**********************************************************************/

#ifndef INCLUDE_DOS_H
#define INCLUDE_DOS_H

#include "nimapi.h"
#include "default_cnfgr.h"
#include "dtlapi.h"
#include "l7_cnfgr_api.h"

typedef enum {
  DOS_PHASE_INIT_0 = 0,
  DOS_PHASE_INIT_1,
  DOS_PHASE_INIT_2,
  DOS_PHASE_WMU,
  DOS_PHASE_INIT_3,
  DOS_PHASE_EXECUTE,
  DOS_PHASE_UNCONFIG_1,
  DOS_PHASE_UNCONFIG_2,
} doSCnfgrState_t;

typedef enum
{
  DOSINTFPINGPARAM = 0,
  DOSINTFSYNACKPARAM,
  DOSINTFLASTPARAM,
}dosIntfCfgParams_t;

typedef struct
{
  nimConfigID_t      configId;
  L7_uint32 intfPingFlooding;
  L7_uint32 intfSmurfAttack;
  L7_uint32 intfSynAckFlooding;
  L7_uint32 intfparams[DOSINTFLASTPARAM];
}dosIntfCfgData_t;

typedef struct
{
  L7_fileHdr_t   cfgHdr; 
  L7_uchar8      systemSIPDIPMode;
  L7_uchar8      systemSMACDMACMode;
  L7_uchar8      systemFirstFragMode;
  L7_uchar8      systemTCPFragMode;
  L7_uchar8      systemTCPFlagMode;
  L7_uchar8      systemTCPFinUrgPshMode;
  L7_uchar8      systemTCPSynMode;
  L7_uchar8      systemTCPSynFinMode;
  L7_uchar8      systemTCPOffsetMode;
  L7_uchar8      systemL4PortMode;
  L7_uchar8      systemUDPPortMode;
  L7_uchar8      systemICMPMode;
  L7_uchar8      systemICMPFragMode;
  L7_uchar8      rsvd[3];
  L7_uint32      systemMaxICMPSize;
  L7_uint32      systemMaxICMPv6Size;
  L7_uint32      systemMinTCPHdrLength;
  L7_uint32      pingFlooding;
  L7_uint32      smurfAttack;
  L7_uint32      synAckFlooding;
  L7_uint32      params[DOSINTFLASTPARAM];
  dosIntfCfgData_t dosIntfCfgData[L7_MAX_PORT_COUNT+1];
  L7_uint32      checkSum;    /* check sum of config file NOTE: needs to be last entry */
} doSCfgData_t;

typedef struct dosIntfChangeParms_s
{
  L7_uint32 event;
  NIM_CORRELATOR_t correlator;
} dosIntfChangeParms_t;

typedef struct
{
  L7_uint32            intIfNum;
  dosIntfChangeParms_t dosIntfChangeParms;
} dosMgmtMsg_t;

#define DOSCONTROL_CFG_FILENAME     "dos.cfg"
#define DOSCONTROL_CFG_VER_1        0x1
#define DOSCONTROL_CFG_VER_2        0x2
#define DOSCONTROL_CFG_VER_CURRENT  DOSCONTROL_CFG_VER_2 

#define DOSCONTROL_IS_READY (((doSCnfgrState == DOS_PHASE_INIT_3) || \
                            (doSCnfgrState == DOS_PHASE_EXECUTE) || \
                            (doSCnfgrState == DOS_PHASE_UNCONFIG_1)) ? (L7_TRUE) : (L7_FALSE))

#define DOS_MSG_SIZE        sizeof(dosMgmtMsg_t)
#define DOS_MSG_COUNT       1024
#define DOS_QUEUE           "dos_Queue"

/*********************************************************************
* @purpose  Build default DoS config data  
*
* @param    ver   Software version of Config Data
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
void doSBuildDefaultConfigData(L7_uint32 ver);

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
void doSMigrateConfigData(L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer);

/*********************************************************************
* @purpose  Applies DoS config data
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
L7_RC_t doSApplyConfigData(void);


/*********************************************************************
* @purpose  Checks if DoS user config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_BOOL doSHasDataChanged(void);
void doSResetDataChanged(void);

/*********************************************************************
* @purpose  Saves DoS user config file to NVStore
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
L7_RC_t doSSave(void);

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
L7_RC_t doSCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
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
L7_RC_t doSCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
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
L7_RC_t doSCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
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
void doSCnfgrFiniPhase1Process();

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
void doSCnfgrFiniPhase2Process();

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
void doSCnfgrFiniPhase3Process();

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
L7_RC_t doSCnfgrNoopProcess( L7_CNFGR_RESPONSE_t *pResponse,
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
L7_RC_t doSCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                               L7_CNFGR_ERR_RC_t   *pReason );

/*********************************************************************
* @purpose  Restores DoS user config file to factore defaults
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
L7_uint32 doSRestore(void);

/*********************************************************************
* @purpose  Start Dos TASK
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
L7_RC_t dosStartTask();

/*********************************************************************
*
* @purpose task to handle all Port DOS management messages
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
void dosTask();

/*********************************************************************
* @purpose  Build default DoS config data for each interface
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
void dosBuildDefaultIntfConfigData(nimConfigID_t *configId, dosIntfCfgData_t *pCfg);

#endif /* INCLUDE_STD_POLICY_H */
