/**
 * ptin_prot_erps.h
 *  
 * Implements ITU-T G.8032/Y.1344 Ethernet Ring Protection 
 * Switching 
 *
 * @author joaom (6/3/2013)
 * (joao-v-mateiro@ptinovacao.pt) 
 *  
 */


#include "ptin_globaldefs.h"
#ifdef PTIN_ENABLE_ERPS

#ifndef __PROT_ERPS_H__
#define __PROT_ERPS_H__

#include "datatypes.h"
#include "l7_packet.h"

//-------------------------------------------------------------------------
//  Constants Definitions
//-------------------------------------------------------------------------
#define PROT_ERPS_CALL_PROC_MS                10
#define PROT_ERPS_CALL_PROC_US                1000*PROT_ERPS_CALL_PROC_MS
#define PROT_ERPS_WAITING_STATES              3000

#define MAX_PROT_PROT_ERPS                    16
#define PROT_ERPS_UNUSEDIDX                   0

#define PROT_ERPS_ENTRY_FREE                  0
#define PROT_ERPS_ENTRY_BUSY                  1

#define PROT_ERPS_NON_REVERTIVE_OPERATION     0
#define PROT_ERPS_REVERTIVE_OPERATION         1

#define PROT_ERPS_PORT0                       0
#define PROT_ERPS_PORT1                       1

#define PROT_ERPS_SF_CLEAR                    0

#define PROT_ERPS_SWITCH_TO_PORT0             0
#define PROT_ERPS_SWITCH_TO_PORT1             1
#define PROT_ERPS_SWITCH_DISABLED             2

#define PROT_ERPS_MAC_SIZE                    L7_ENET_MAC_ADDR_LEN


//-------------------------------------------------------------------------
//  Error Codes
//-------------------------------------------------------------------------
#define PROT_ERPS_EXIT_OK                     0
#define PROT_ERPS_EXIT_NOK1                  -1
#define PROT_ERPS_EXIT_NOK2                  -2
#define PROT_ERPS_INDEX_VIOLATION            -3
#define PROT_ERPS_UNAVAILABLE                -4
#define PROT_ERPS_INDEX_IN_USE               -5


//-------------------------------------------------------------------------
//  Operator commands
//-------------------------------------------------------------------------
#define PROT_ERPS_OPCMD_NR                    0
#define PROT_ERPS_OPCMD_OC                    1
#define PROT_ERPS_OPCMD_LO                    2  //// TO BE DONE ///
#define PROT_ERPS_OPCMD_FS                    3
#define PROT_ERPS_OPCMD_MS                    4
#define PROT_ERPS_OPCMD_ReplaceRPL            5  //// TO BE DONE ///
#define PROT_ERPS_OPCMD_ExeSignal             6  //// TO BE DONE ///


//-------------------------------------------------------------------------
//  Local Request
//-------------------------------------------------------------------------
#define LReq_NONE                             100+10
#define LReq_CLEAR                            100+9
#define LReq_FS                               100+8
#define LReq_SF                               100+7
#define LReq_SFc                              100+6
#define LReq_MS                               100+5
#define LReq_WTRExp                           100+4
#define LReq_WTRRun                           100+3
#define LReq_WTBExp                           100+2
#define LReq_WTBRun                           100+1


//-------------------------------------------------------------------------
//  WTR/WTB/Guard Timers
//-------------------------------------------------------------------------
#define TIMER_CMD_STOP                        0
#define TIMER_CMD_START                       1

#define WTR_TIMER_CMD                         0
#define WTB_TIMER_CMD                         1
#define GUARD_TIMER_CMD                       2

//------------------------------------------------------------------------
//                                APS
//+----------------+-----------------+-----------------+-----------------+
//|       1                 2                 3                 4        |
//+----------------+-----------------+-----------------+-----------------+
//|8 7 6 5 4 3 2 1 | 8 7 6 5 4 3 2 1 | 8 7 6 5 4 3 2 1 | 8 7 6 5 4 3 2 1 |
//| Req/   | Sub-  |     Status      |        Node ID (6 octets)         |
//| State  | code  | R|D|B| Status   |                                   |
//|        |       | B|N|P| Reserved |                                   |
//|        |       |  |F|R|          |                                   |
//+----------------+-----------------+-----------------+-----------------+
//|                               Node ID                                |
//+----------------+-----------------+-----------------+-----------------+
//|                         Reserved 2 (24 octets)                       |
//+----------------+-----------------+-----------------+-----------------+


//10.3.  R-APS format
//   R-APS information is carried in an R-APS PDU, which is one of a suite of Ethernet OAM messages.

//The fields of R-APS specific information:
//  a)  Request/State (4 bits) - This field represents a request or state, and is encoded as described
#define RReq_NONE                       15
#define RReq_EVENT                      14    //1110 Event
#define RReq_FS                         13    //1101 Forced Switch
#define RReq_SF                         11    //1011 Signal Fail (SF)
#define RReq_MS                         7     //0111 Manual Switch (MS)
#define RReq_NR                         0     //0000 No Request (NR)

#define RReq_STAT_RB                    0x80
#define RReq_STAT_DNF                   0x40
#define RReq_STAT_BPR                   0x20
#define RReq_STAT_ZEROS                 0x00

#define APS_GET_REQ(aspReqStateRx)     ( (aspReqStateRx >> 4) & 0x0F )
#define APS_GET_STATUS(aspReqStatusRx) ( (aspReqStatusRx    ) & 0xE0 )

//-------------------------------------------------------------------------
//  Node state - The current state of the Ethernet Ring Node
//-------------------------------------------------------------------------
#define ERPS_STATE_Freeze               0x0
#define ERPS_STATE_Z_Init               0x1
#define ERPS_STATE_A_Idle               0x2
#define ERPS_STATE_B_Protection         0x3
#define ERPS_STATE_C_ManualSwitch       0x4
#define ERPS_STATE_D_ForcedSwitch       0x5
#define ERPS_STATE_E_Pending            0x6


#define ERPS_STATE_GetState(state)      ((state & 0x0F))
#define ERPS_STATE_SetLocal(state)      ((state & 0x0F))
#define ERPS_STATE_SetRemote(state)     ((state | 0x10))
#define ERPS_STATE_IsLocal(state)       ((state & 0x10)? 0:1)
#define ERPS_STATE_IsRemote(state)      ((state & 0x10)? 1:0)

//-------------------------------------------------------------------------
// ring port
//-------------------------------------------------------------------------
#define ERPS_PORT_FLUSHING              0
#define ERPS_PORT_BLOCKING              1


#define ERP_NODE_ID                    srcMacAddr

//-------------------------------------------------------------------------
//  DB Structures
//-------------------------------------------------------------------------

typedef struct {
   unsigned char slot;
   unsigned char type;
   unsigned char idx;
} erpsPort_t;

/// ERPS Parameters Configurations
typedef struct _erpsProtParam_t {
   L7_uint8   ringId;               ///< in the range [1, .., 239]
   L7_uint8   isOpenRing;
   L7_uint16  controlVid;
   L7_uint8   megLevel;

   erpsPort_t port0;
   erpsPort_t port1;
   L7_uint8   port0Role;
   L7_uint8   port1Role;
   L7_uint8   port0CfmIdx;
   L7_uint8   port1CfmIdx;

   L7_uint8   revertive;            ///< 0-Non-Revertive, 1-Revertive: Enables WaitToRestore Timer
   L7_uint16  guardTimer;           ///< [10, 2000] ms 10 ms steps [10 ms, 2 seconds], with a default value of 500 ms.
   L7_uint8   holdoffTimer;         ///< [0, 10] seconds in steps of 100 ms.
   L7_uint8   waitToRestoreTimer;   ///< [1, 12] minutes in steps of 1 min, default: 5 min.

   L7_uint32  continualTxInterval;  ///< APS
   L7_uint32  rapidTxInterval;      ///< APS

   ///service List
   L7_uint8   vid_bmp[(1<<12)/(sizeof(L7_uint8)*8)];
} erpsProtParam_t;

/// Hardware Abstraction Layer
typedef struct _erpsHAL_t {
    int     (*rd_alarms)            (L7_uint8 slot, L7_uint32 index);
    L7_RC_t (*aps_rxfields)         (L7_uint8 erps_idx, L7_uint8 *req_state, L7_uint8 *status, L7_uint8 *nodeid, L7_uint32 *rxport);
    L7_RC_t (*aps_txfields)         (L7_uint8 erps_idx, L7_uint8 req_state, L7_uint8 status);
    int     (*prot_proc)            (L7_uint8 prot_id);
} erpsHAL_t;

/// State Machine Parameters Configurations and States
typedef struct {
    L7_uint8 admin;             ///< ENTRY_FREE/ENTRY_BUSY

    erpsProtParam_t protParam;
    
    //++++++++++++++++++++++++++++++++++++++++++++++++
    // Internal variables
    //++++++++++++++++++++++++++++++++++++++++++++++++

    L7_uint8 status_SF[2];            ///< Ports SF (Physical or OAM)
    
    L7_uint32 wtr_timer;              ///< Timer
    L7_uint8  wtr_CMD;                ///< Timer
    L7_uint32 wtb_timer;              ///< Timer      //// TO BE DONE ///
    L7_uint8  wtb_CMD;                ///< Timer
    L7_uint32 guard_timer;            ///< Timer
    L7_uint32 guard_timer_previous;   ///< Timer
    L7_uint8  guard_CMD;              ///< Timer
    L7_uint16 holdoff_timer;          ///< Timer
    L7_uint16 holdoff_timer_previous; ///< Timer
    
    // Operator command
    L7_uint8 operator_cmd;            ///< Operator Command
    L7_uint8 operator_cmd_port;       ///< Operator Command Requested Port
    
    // Request    
    L7_uint8 localRequest;
    L7_uint8 remoteRequest;
    
    // APS    
    L7_uint16 apsReqStatusTx;
    L7_uint16 apsReqStatusRx[2];
    L7_uint8  apsNodeIdRx[2][PROT_ERPS_MAC_SIZE];
    L7_uint8  apsBprRx[2];

    // Ports State
    L7_uint8 portState[2];            ///< blocking or flushing
    L7_uint8 rplBlockedPortSide;      ///< current RPL side port
    L7_uint8 hwSync;                  ///< configures VLAN on switch
    L7_uint8 hwFdbFlush;              ///< FDB Flush by VLAN

    // FSM
    L7_uint8 state_machine;
    L7_uint8 state_machine_h;         ///< LSN: State
                                      ///< MSN: isLocalOrRemoteState
    L7_uint16 waitingstates;
    
    //++++++++++++++++++++++++++++++++++++++++++++++++
    // HAL
    //++++++++++++++++++++++++++++++++++++++++++++++++

    erpsHAL_t hal;                    ///< Functions Abstraction

    //++++++++++++++++++++++++++++++++++++++++++++++++

} tblErps_t;


/// Estrutura usada para leitura de alarmistica
typedef struct _erpsStatus_t {
    L7_uint8   rplBlockedPortSide;      ///< PROT_ERPS_PORT0 or PROT_ERPS_PORT1
    L7_uint8   port0_SF;                ///< SF or NO SF
    L7_uint8   port1_SF;
    L7_uint8   port0State;              ///< ERPS_PORT_BLOCKING or ERPS_PORT_FLUSHING
    L7_uint8   port1State;
    L7_uint16  guard_timer;             ///< elapsed time
    L7_uint32  wtr_timer;
    L7_uint32  wtb_timer;
    L7_uint16  holdoff_timer;
} erpsStatus_t;


//-------------------------------------------------------------------------
//  VLANs Inclusion List
//-------------------------------------------------------------------------

/// VLANs protected by this ERPS instance
typedef struct {
  L7_uint8  vid_bmp[(1<<12)/(sizeof(L7_uint8)*8)];          ///< VLAN ID
  L7_uint8  isOwnerVid_bmp[(1<<12)/(sizeof(L7_uint8)*8)];   ///< True if this is a UNI VLAN
} erpsVlanInclusionList_t;


//-------------------------------------------------------------------------
//  Variables
//-------------------------------------------------------------------------

/// SW Data Base containing ERPS instance information
extern tblErps_t tbl_erps[MAX_PROT_PROT_ERPS];

/// SW Data Base VLANs Inclusion List
extern erpsVlanInclusionList_t tbl_erps_vlanList[MAX_PROT_PROT_ERPS];

//-------------------------------------------------------------------------


//-------------------------------------------------------------------------
//  Functions to export
//-------------------------------------------------------------------------


/**
 * @addtogroup functions ERPS (G.8032) Module functions
 */
/** @{ */


/**
 * ERPS Instaces initialization
 * @author joaom (6/5/2013)
 * @return int
 */
int ptin_erps_init(void);

/**
 * Allocate an ERPS instance
 * @author joaom (6/5/2013)
 * @param erps_idx 
 * @param new_group 
 * @return int 
 */
int ptin_erps_add_entry( L7_uint8 erps_idx, erpsProtParam_t *new_group);

/**
 * Used to reconfigure an existing entry
 * @author joaom (6/5/2013)
 * @param erps_idx 
 * @param conf 
 * @return int 
 */
int ptin_erps_conf_entry(L7_uint8 erps_idx, erpsProtParam_t *conf);

/**
 * Delete ERPS# instance
 * @author joaom (6/5/2013)
 * @param erps_idx 
 * @return int 
 */
int ptin_erps_remove_entry(L7_uint8 erps_idx);

/**
 * Operator Command
 * @author joaom (6/5/2013)
 * @param erps_idx 
 * @param cmd_port 
 * @return int 
 */
int ptin_erps_cmd_force(L7_uint8 erps_idx, L7_uint8 cmd_port);

/**
 * Operator Command
 * @author joaom (6/5/2013)
 * @param erps_idx 
 * @param cmd_port 
 * @return int 
 */
int ptin_erps_cmd_manual(L7_uint8 erps_idx, L7_uint8 cmd_port);

/**
 * Operator Command
 * @author joaom (6/5/2013)
 * @param erps_idx 
 * @return int 
 */
int ptin_erps_cmd_clear(L7_uint8 erps_idx);

/**
 * Operator Command
 * @author joaom (6/5/2013)
 * @param erps_idx 
 * @return int 
 */
int ptin_erps_cmd_lockout(L7_uint8 erps_idx);

/**
 * Delete all ERPS
 * @author joaom (6/5/2013)
 * @return int 
 */
int ptin_erps_clear(void);

/**
 * Get ERPS# Protection Parameters configuration
 * @author joaom (6/5/2013)
 * @param erps_idx 
 * @param group 
 * @return int 
 */
int ptin_erps_get_entry(L7_uint8 erps_idx, erpsProtParam_t *group);

/**
 * Get ERPS Status
 * @author joaom (6/5/2013)
 * @param erps_idx 
 * @param status 
 * @return int 
 */
int ptin_erps_get_status(L7_uint8 erps_idx, erpsStatus_t *status);

/**
 * ERPS Debug: Print configurations and Status information
 * @author joaom (6/5/2013)
 * @param erps_idx 
 * @return int 
 */
int ptin_erps_rd_entry(L7_uint8 erps_idx);

/**
 * ERPS Debug: Print configurations and Status information
 * @author joaom (6/11/2013)
 * @return int 
 */
int ptin_erps_rd_allentry(void);

/**
 * ERPS Debug: Print configurations and Status information
 * @author joaom (6/5/2013)
 * @param erps_idx 
 * @return int 
 */
int ptin_erps_dump(L7_uint8 erps_idx);

/**
 * Set condition of either failed (i.e., signal fail (SF)) or 
 * non-failed (OK) 
 * 
 * @author joaom (6/5/2013)
 * 
 * @param erps_idx 
 * @param path 
 * 
 * @return int 
 */
int ptin_erps_force_alarms(L7_uint8 erps_idx, L7_uint8 port, L7_uint8 sf);

/****************************************************************************** 
 * ERPS Task Init
 ******************************************************************************/

/**
 * Initializes ERPS module
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_prot_erps_init(void);

/** @} */
//-------------------------------------------------------------------------

#endif //__ERPS_H__

#endif  // PTIN_ENABLE_ERPS

