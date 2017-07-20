/**
 * ptin_prot_uplink.h
 *  
 * Implements the uplink protection for LAGs
 *
 * Created on: 2017/04/21 Author: Milton Ruas 
 *  
 * Notes:
 */

#ifndef _PTIN_PROT_UPLINK__H
#define _PTIN_PROT_UPLINK__H

#include "ptin_include.h"

#define MAX_UPLINK_PROT   16

/* Protection port type */
typedef enum
{
  PORT_WORKING    = 0,
  PORT_PROTECTION = 1,
  PORT_ALL        = 254,
  PORT_NONE       = 255
} PROT_PortType_t;

/*-------------------------------------------------------------------------*/
/*  WTR                                                                    */
/*-------------------------------------------------------------------------*/
typedef enum
{
  WTR_CMD_STOP  = 0,
  WTR_CMD_START = 1
} PROT_WTR_CMD_t;


typedef enum
{
  MASK_ALARMS_NONE  = 0x0000,  // No alarms
  MASK_LC_FAILURE   = 0x0001,  // HighPriority:    Line Card Failure [on/off]
  MASK_PORT_LOS     = 0x0002,  // HighPriority:    PON LOS [on/off]
  MASK_MEP_LOC      = 0x0004,  // MediumPriority:  MEP Loss Of Continuity
  MASK_MEP_RDI      = 0x0008,  // MediumPriority:  MEP Remote Defect Indication
  MASK_PORT_SD      = 0x0010,  // MediumPriority:  Port Signal Degrade (based on CRC)
  MASK_MEP_SD       = 0x0020,  // MediumPriority:  MEP Signal Degrade (based on FLR)
  MASK_PORT_LINK    = 0x0040,  // High priority:   Link-Down alarm
  MASK_PON_NOT_SYNC = 0x0080,  // LowPriority:     Failure In Sync Confs with Gpon Line Cards.
  MASK_PORT_BW      = 0x0100,  // LowPriority:     Lower Bandwidth relatively to other interface
  MASK_ALARMS_ALL   = 0xFFFF
} PROT_PortAlarmsMask_t;


/*-------------------------------------------------------------------------*/
/*  Operator commands                                                      */
/*-------------------------------------------------------------------------*/

typedef enum
{
  OPCMD_NR = 0x00,  /* No Request */
  OPCMD_OC = 0x01,  /* Clear Command */
  OPCMD_LO = 0x02,  /* Lockout protection */
  OPCMD_FS = 0x04,  /* Forced Switch */
  OPCMD_MS = 0x08   /* Manual Switch */
} PROT_OPCMD_t;

/*-------------------------------------------------------------------------*/
/*  Local Request                                                          */
/*-------------------------------------------------------------------------*/
typedef enum
{
  PROT_LReq_NONE       = 16,    /* No Request */
  PROT_LReq_OC         = 15,   /* Clear previous request */
  PROT_LReq_LO         = 14,   /* Lockout protection */
  PROT_LReq_FS         = 12,   /* Forced Switch */
  PROT_LReq_LC_FAILURE = 11,   /* Linecard failure */
  PROT_LReq_SF         = 10,   /* Signal Fail */
  PROT_LReq_LINK       = 9,    /* LINK-Down  */
  PROT_LReq_BW         = 8,    /* Bandwidth differential */
  PROT_LReq_SD         = 7,    /* Signal Degrade */
  PROT_LReq_SFc        = 6,    /* Signal-fail cleared */
  PROT_LReq_MS         = 5,    /* Manual-Switch */
  PROT_LReq_WTRExp     = 4     /* Wait-to-Restore Expiration */
} PROT_LReq_t;

/*-------------------------------------------------------------------------*/
/*  State machine                                                          */
/*-------------------------------------------------------------------------*/
typedef enum
{
  PROT_STATE_Disabled   = 0,
  PROT_STATE_Undefined  = 1,    
  PROT_STATE_Normal     = 2,
  PROT_STATE_Protection = 3,
  PROT_STATE_WorkAdmin  = 4,
  PROT_STATE_ProtAdmin  = 5,
  PROT_STATE_FailRecover= 6
} PROT_STATE_t;

/*-------------------------------------------------------------------------*/
/*  Structures Definition                                                  */
/*-------------------------------------------------------------------------*/

/// Protection Parameters
typedef struct {
    L7_BOOL   revert2working;       /// < 0-Non-Revertive, 1-Revertive -> Enables WaitToRestore Timer

    L7_uint16 HoldOffTimer;         ///< [0, 10] seconds in steps of 100 ms.
    L7_uint16 WaitToRestoreTimer;   ///< in seconds

    L7_uint32 alarmsEnFlag;         ///< UPLINK_PortAlarmsMaskEn_t
    L7_uint32 flags;                /// Other flags

    L7_uint32 intIfNumW;
    L7_uint32 intIfNumP;
} uplinkprotParams_st;

/// Protection DB
typedef struct {
    L7_BOOL             admin;

    uplinkprotParams_st protParams;

    PROT_PortType_t     activePortType;
    PROT_PortType_t     activePortType_h;

    L7_uint32           hAlarms[2];
    L7_uint8            statusSF[2];
    L7_uint8            statusSD[2];
    L7_uint32           LAG_members_total[2];
    L7_uint32           LAG_members_active[2];
    L7_uint32           LAG_bw_total[2];
    L7_uint32           LAG_bw_active[2];
   
    // Operator Command
    PROT_OPCMD_t        operator_cmd;
    PROT_PortType_t     operator_switchToPortType;

    // Request
    PROT_LReq_t         localRequest;
    PROT_LReq_t         lastSwitchoverCause;

    // FSM
    PROT_STATE_t        state_machine;
    PROT_STATE_t        state_machine_h;

    // Timers
    PROT_WTR_CMD_t      wait_restore_timer_CMD;
    L7_uint32           wait_restore_timer;

} uplinkprot_st;

/// Used to get Status
typedef struct {
    PROT_PortType_t     activePortType;
    L7_uint32           alarmsW;
    L7_uint32           alarmsP;
    L7_uint32           alarmsMaskW;
    L7_uint32           alarmsMaskP;
    PROT_LReq_t         lastSwitchoverCause;
    L7_uint16           WaitToRestoreTimer;     ///< elapsed time in seconds
    L7_uint16           HoldOffTimer;           ///< elapsed time in seconds
} uplinkprot_status_st;

/// DB for Uplink Protection
extern uplinkprot_st uplinkprot[MAX_UPLINK_PROT];


/**
 * Initialize Module 
 *  
 * @return L7_RC_t   
 */
extern L7_RC_t ptin_prot_uplink_init(void);

/**
 * Search for a protection group with the specified intIfNum
 * 
 * @author mruas (19/07/17)
 * 
 * @param intIfNum 
 * @param protIdx
 * @param portType 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_NOT_EXIST
 */
extern L7_RC_t ptin_prot_uplink_index_find(L7_uint32 intIfNum, L7_uint8 *protIdx, L7_uint8 *portType);

/**
 * Blocking mechanism implemented here
 * 
 * @param intIfNum 
 * @param block_state 
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t ptin_prot_uplink_intf_block(L7_uint32 intIfNum, L7_int block_state);

/**
 * Create a protection group
 * 
 * @param protIdx : Protection group
 * @param intf1 : First interface
 * @param intf2 : Second interface
 * @param restore_time : Restoration time
 * @param operationMode : Should revert to working?
 * @param alarmFlagsEn : Alarm flags enable 
 * @param flags : Other flags 
 * @param force : Force (re)creation if group already exists 
 *  
 * @return L7_RC_t   
 */
extern L7_RC_t ptin_prot_uplink_create(L7_uint8 protIdx, ptin_intf_t *intf1, ptin_intf_t *intf2,
                                       L7_uint32 restore_time, L7_uint8 operationMode, L7_uint32 alarmFlagsEn, L7_uint32 flags, L7_BOOL force);

/**
 * Remove protection group
 * 
 * @param protIdx 
 *  
 * @return L7_RC_t  
 */
extern L7_RC_t ptin_prot_uplink_clear(L7_uint8 protIdx);

/**
 * Get protection group configuration
 * 
 * @param protIdx 
 * @param config
 *  
 * @return L7_RC_t  
 */
extern L7_RC_t ptin_prot_uplink_config_get(L7_uint8 protIdx, uplinkprotParams_st *config);

/**
 * Get protection group status
 * 
 * @param protIdx 
 * @param status 
 *  
 * @return L7_RC_t  
 */
extern L7_RC_t ptin_prot_uplink_status(L7_uint8 protIdx, uplinkprot_status_st *status);

/**
 * Update alarmFlagsEn value
 * 
 * @param protIdx 
 * @param alarmFlagsEn 
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t ptin_prot_uplink_alarmFlagsEn_set(L7_uint8 protIdx, L7_uint32 alarmFlagsEn);

/**
 * Update operationMode value
 * 
 * @param protIdx 
 * @param operationMode 
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t ptin_prot_uplink_operationMode_set(L7_uint8 protIdx, L7_uint32 operationMode);

/**
 * Update restore_time value
 * 
 * @param protIdx 
 * @param restore_time 
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t ptin_prot_uplink_restoreTime_set(L7_uint8 protIdx, L7_uint32 restore_time);

/**
 * Apply a command to a protection group
 * 
 * @param protIdx : Protection group
 * @param cmd : Command
 * @param portType : port type
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t ptin_prot_uplink_command(L7_uint8 protIdx, PROT_OPCMD_t cmd, PROT_PortType_t portType);

/**
 * State Machine Reset.
 *  
 * @param protIdx 
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t uplinkprotResetStateMachine(L7_uint16 protIdx);

/**
 * Process event and update state machine
 * 
 * @author mruas (5/10/2017)
 * 
 * @param intIfNum 
 * @param event 
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t uplinkProtEventProcess(L7_uint32 intIfNum, L7_uint16 event);

#endif /* _PTIN_PROT_UPLINK__H */

