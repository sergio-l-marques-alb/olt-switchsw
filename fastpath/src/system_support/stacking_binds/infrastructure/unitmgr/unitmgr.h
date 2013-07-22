/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename unitmgr.h
*
* @purpose Unit manager private defines
*
* @component unitmgr
*
* @comments none
*
* @create 12/06/2002
*
* @author djohnson
*
* @end
*             
**********************************************************************/

#ifndef INCLUDE_UNITMGR_H
#define INCLUDE_UNITMGR_H
#include "unitmgr_api.h"
#include "l7_cnfgr_api.h"
#include "nimapi.h"

/* Private defines */
#define UNITMGR_QUEUE_NAME  "unitMgrQueue"
#define UNITMGR_MSG_COUNT	64
#define UNITMGR_TASK_NAME   "unitMgrTask"

#define L7_UNITMGR_CFG_FILENAME "unitmgr.cfg"
#define L7_UNITMGR_CFG_VER_1 1

/* !!!!!!!! MOVE !!!!!!!!!! */
#define L7_UNITMGR_MAX_PRECFGUNITS 8
#define L7_MAX_UNIT_TYPES      4
#define UNITMGR_COMPONENT_ID   63

/* Time interval for sending discovery message */
#define L7_UNITMGR_TX_SEC 2

/* Max time to wait for a discovery message */
#define L7_UNITMGR_RX_SEC 10

/* Time to wait to become a management unit */
#define L7_UNITMGR_TRANSFER_SEC 20

/* Time to wait to become a management unit after power up */
#define L7_UNITMGR_IPL_SEC 300

/* Check the unit list last received discovery message this often */
#define L7_UNITMGR_RX_EXPIRE_SEC 1

/* Discovery message */
typedef struct
{
  L7_uint32         unit_number;  
  L7_uint32         unit_type;      
  unitMgrMgmtFlag_t mgmt_unit;
  unitMgrMgmtPref_t admin_management_pref;
  unitMgrMgmtPref_t hw_management_pref; 
  L7_uchar8         model_id[16];          
  L7_BOOL           unit_discovery;        
  L7_BOOL           chassis_discovery;     
  
  L7_enetMacAddr_t  unit_mac;              /* sender's mac address */
} discovery_message_t;

/* Queue message */
typedef struct
{
  L7_uint32 event; /* major event id */
  L7_CNFGR_CMD_DATA_t cmdData;
  
  union
  {
    struct /* discovery message */
    {
      discovery_message_t discovery_message;
    } dm;
    
    struct /* request or response */
    {
      L7_uint32 tounit;
      L7_uint32 fromunit;
      L7_uint32 correlator;
      L7_BOOL   request;
      L7_RC_t   returncode;
      union
      {
        struct
        {
          L7_INTF_TYPES_t type;
          nimUSP_t usp;
          L7_enetMacAddr_t macaddr;
        } mac;

        struct
        {
          L7_uint32 event;
          L7_uchar8 trap[64];
        } trap;

        struct
        {
          L7_uchar8 message[LOG_MSG_MAX_MSG_SIZE];
        } msglog;
      } data;
    } r;
  } msgData;
} unitMgrMsg_t;

/* Preconfigured unit(s) and current unit information     */
/*   contains stack_member_t fields and additional fields */
/*   a copy exists as a node in the stack member tree     */
typedef struct
{
  L7_uint32         unit_number;  
  L7_uint32         unit_type;      
  L7_uchar8         model_id_hpc[16];          
  L7_uchar8         model_id_precfg[16];
  unitMgrStatus_t   unit_status;
  unitMgrMgmtFlag_t mgmt_unit;
  L7_uint32         unit_uptime;
  L7_uchar8         unit_description[256]; 
  unitMgrMgmtPref_t hw_management_pref; 
  unitMgrMgmtPref_t admin_management_pref;
  L7_uint32         expected_code_version;
  L7_uint32         detected_code_version_running;
  L7_uint32         detected_code_version_flash;
  L7_enetMacAddr_t  unit_mac;   
  L7_uint32         last_received;
  
  L7_BOOL           unit_discovery;    /* run the unit discovery protocol    */
  L7_BOOL           chassis_discovery; /* run the chassis discovery protocol */
  L7_uint32         storage_size;      /* space to store switch config       */
} unitmgr_info_t;

/* Each unit maintains a list of units currently in the stack */
typedef struct
{
  L7_uint32         unit_number;  
  L7_uint32         unit_type;      
  L7_uchar8         model_id_hpc[16];          
  L7_uchar8         model_id_precfg[16];
  unitMgrStatus_t   unit_status;
  unitMgrMgmtFlag_t mgmt_unit;
  L7_uint32         unit_uptime;
  L7_uchar8         unit_description[256]; 
  unitMgrMgmtPref_t hw_management_pref; 
  unitMgrMgmtPref_t admin_management_pref;
  L7_uint32         expected_code_version;
  L7_uint32         detected_code_version_running;
  L7_uint32         detected_code_version_flash;
  L7_enetMacAddr_t  unit_mac;   
  L7_uint32         last_received;
  L7_BOOL           local_unit;
} stack_member_t;

typedef struct
{
  L7_fileHdr_t    hdr;
  unitmgr_info_t  cfg;
  stack_member_t  preCfg[L7_MAX_UNITS_PER_STACK];
  L7_uint32       checkSum;
} unitMgrCfgFile_t;

typedef enum
{
  INIT = 1,
  ISOLATED,
  CONN_UNIT,
  CONN_MGR,
  CONN_STBY
} unitMgrStates_t;

typedef enum
{
  POLL_HSSL_FOR_READY = 1,
  
  COLLECT_HSSL_MSGS_START,
  COLLECT_HSSL_MSGS_STOP,
  
  RECEIVE_DISCOVERY_MSG,
  SEND_DISCOVERY_MSG,
  
  EXPIRED_T_IPL,
  EXPIRED_T_TRANS,
  EXPIRED_T_RX,

  CHECK_RX_TIMERS,
  PROCEED_TO_STEADY_STATE,
  RESET_MYSELF,
  CANT_ASSIGN_UNIT_NUM,
  MOVE_MGMT_FUNC,
  ADMIN_MODE_ENABLE,
  ADMIN_MODE_DISABLE,
  SEND_EVENT_LOG,
  REASSIGN_UNIT_NUMBER,

  L7_UNITMGR_PHASE_1,
  L7_UNITMGR_PHASE_2,
  L7_UNITMGR_SAVE,
  L7_UNITMGR_RESTORE,

  MAC,
  TRAP,
  LOG,
  RESET
} unitmgr_events_t;

void umHPCLReceiveMsg(unitMgrMsg_t *msg_from_hssl, L7_uint32 length);
void umStopCollection(L7_uint32 nullParm1, L7_uint32 nullParm2);
void umDetermineMgmtUnitAndNum(void);
void unitMgrTask(void);
void umSetState(L7_uint32 new_state);
void umPollHPCL(L7_uint32 nullParm1, L7_uint32 nullParm2);
void umAddNewStackMember(unitMgrMsg_t *msg);
void umSendDiscoveryMessageEvent(L7_uint32 nullParm1, L7_uint32 nullParm2);
void umSendDiscoveryMessage(void);
L7_RC_t umGetNewUnitNum(L7_uint32 *num);
void umSendTIPLMessageEvent(L7_uint32 nullParm1, L7_uint32 nullParm2);
void umSendTTransMessageEvent(L7_uint32 nullParm1, L7_uint32 nullParm2);

void umCheckRxTimers(void);
void umSendCheckRxTimersEvent(L7_uint32 nullParm1, L7_uint32 nullParm2);
void umResolveMgmtConflict(unitMgrMsg_t *msg);
void umResolveUnitNumberConflict(unitMgrMsg_t *msg);
void umDecideIsoToMgrTransition(L7_BOOL *trans);
L7_RC_t umMgmtFuncSet(unitMgrMgmtFlag_t mgmt_func);
L7_RC_t umPreConfigUnitNumber(L7_uint32 unit_number);
L7_RC_t umUnitNumberChange(L7_uint32 unit_number, L7_uint32 new_unit_number);
L7_RC_t umUnitTypeAssign(L7_uint32 unit_number, L7_uint32 unit_type);
L7_RC_t umUnitNumberChangeEvent(L7_uint32 unit_number, L7_uint32 new_unit_number);

/* data */
L7_RC_t umStackMemberGetFirst(L7_uint32 *first_unit);
L7_RC_t umStackMemberGetNext(L7_uint32 start_unit, L7_uint32 *next_unit);
L7_RC_t umStackMemberGet(L7_uint32 unit, stack_member_t **p);
L7_RC_t umStackMemberDelete(L7_uint32 unit);
void umUpdateStackMember(unitMgrMsg_t *msg);

/* cfg */
L7_RC_t umSave(void);
L7_RC_t umRestore(void);
L7_BOOL umHasDataChanged(void);
void umApplyConfigData(void);
void umBuildDefaultConfigData(L7_uint32 ver);

L7_RC_t unitMgrApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData);
L7_RC_t umPhase1Init(L7_CNFGR_CMD_DATA_t *pCmdData);
L7_RC_t umPhase2Init(L7_CNFGR_CMD_DATA_t *pCmdData);
L7_RC_t umPhase3Init(L7_CNFGR_CMD_DATA_t *pCmdData);
L7_RC_t umUnconfigPhase2Handle(L7_CNFGR_CMD_DATA_t *pCmdData);
void umConfigPhaseDone(L7_CNFGR_CMD_DATA_t cmdData, L7_RC_t rc);


L7_RC_t umMgrNumberGet(L7_uint32 *unit_number);
L7_RC_t umNumberGet(L7_uint32 *unit_number);
L7_BOOL umIsLocal(L7_uint32 unit_num);
L7_RC_t umTransferMgmt(L7_uint32 target_unit);
void umTrapReceiveEvent(L7_uint32 trapnum);
void umMsgLogReceiveEvent(L7_uint32 msglog /* parms TBD */);
L7_RC_t umIfaceMacGet(L7_INTF_TYPES_t type, L7_uint32 unit,L7_uint32 slot,L7_uint32 port, L7_uchar8 *l2_mac_addr, L7_uchar8 *l3_mac_addr);
L7_RC_t umResetRequest(L7_uint32 unit);

#endif
