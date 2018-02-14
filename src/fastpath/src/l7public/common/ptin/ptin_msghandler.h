/**
 * CHMessageHandler.h 
 *  
 * Created on: 2010/04/09 
 * Author: Andre Brizido 
 *  
 * Modified on 2011/06/14 
 * By: Alexandre Santos (alexandre-r-santos@ptinovacao.pt) 
 * Notes: 
 *  - Structures used on the communication process are now defined in this
 *    module and used exclusively here
 */




#ifndef CHMESSAGEHANDLER_H_
#define CHMESSAGEHANDLER_H_

#include "ptin_include.h"
#include "ipc.h"

/*****************************************************************************
 * IPC Messages codes 
 *****************************************************************************/
#define CCMSG_APPLICATION_IS_ALIVE          0x9000  // (To be used with fastpath.cli)
#define CCMSG_BOARD_SHOW                    0x9001  // struct msg_FWFastpathInfo
#define CCMSG_ALARMS_RESET                  0x9002  // No struct
#define CCMSG_DEFAULTS_RESET                0x9003  // No struct

#define CCMSG_SWITCH_TEMPERATURE_GET        0x900A  // struct msg_ptin_temperature_monitor_t


#define CCMSG_APP_CHANGE_STDOUT             0x91F0  // (To be used with fastpath.cli)
#define CCMSG_APP_SHELL_CMD_RUN             0x91F1  // (To be used with fastpath.cli)
#define CCMSG_APP_LOGGER_OUTPUT             0x91F2  // (To be used with fastpath.cli)
#define CCMSG_APP_FW_STATE_SET              0x91F3  // (To be used with fastpath.cli)

#define CCMSG_LAST_MSG_ID                                  0x91FF



/*****************************************************************************
 * Structures exchanged on the messages
 * NOTE: By default they are 'packed' to avoid empty gaps
 *****************************************************************************/

typedef enum
{
  DEFAULT_RESET_MODE_FULL =     0x00,
  DEFAULT_RESET_MODE_PARTIAL =  0xFF
} defaults_reset_mode_t;

typedef struct {
  L7_uint8  slot_id;    // Slot ID
  L7_uint8  generic_id; // ID used for any purpose (context of each message)
  L7_uint8  type;       // Field used for any purpose
  L7_uint8  param;      // Parameter
} __attribute__((packed)) msg_HwGenReq_t;

typedef struct {
  L7_uint8  intf_type;  // Interface type: { 0-Physical, 1-Logical (LAG), 2-Routing }
  L7_uint8  intf_id;    // Interface Id# (phy ports / LAGs / Routing)
} __attribute__((packed)) msg_HwEthInterface_t;

/* Misc structs ***************************************************************/

// Board show
// Message CCMSG_BOARD_SHOW
typedef struct  {
  L7_uint8     SlotIndex;                // slot id
  L7_uint32    BoardPresent;             // 1 - means FastPath is loaded
  L7_uint32    BoardNInterfaces;         // (not used)                         
  L7_uint32    BoardVersion;             // (not used)              
  L7_uint32    BoardMode;                // (not used) 
  L7_uint8     BoardSerialNumber[20];    // (FastPath version)                   
  L7_uint32    FpgaFixoID;               // (not used)                
  L7_uint32    FpgaFixoVersion;          // (not used)                     
  L7_uint32    FpgaFlexID;               // (not used)                
  L7_uint32    FpgaFlexVersion;          // (not used)                    
  L7_uint32    PCBVersion;               // (not used)
} __attribute__((packed)) msg_FWFastpathInfo;

/***************************************************** 
 * PORT CONFIGURATIONS
 ****************************************************/

/* CCMSG_SWITCH_TEMPERATURE_GET */
typedef struct
{
  L7_uint8  SlotId;
  L7_uint16 index;
  L7_uint16 number_of_sensors;
  struct
  {
    L7_int16 curr_value;
    L7_int16 peak_value;
  } __attribute__((packed)) sensors_data[10];
} __attribute__((packed)) msg_ptin_temperature_monitor_t;


/***************************************************************************** 
 * Functions prototypes
 *****************************************************************************/


extern int ipc_msg_bytes_debug_enable(int disable0_enable1_read2);


extern int ipc_msg_bytes_debug_enable(int disable0_enable1_read2);


/**
 * Message handler for the PTin Module.
 * 
 * @param inbuffer Input IPC message
 * @param outbuffer Output IPC message
 * 
 * @return int Error code (SIR_ERROR list)
 */
extern int CHMessageHandler (ipc_msg *inbuffer, ipc_msg *outbuffer);

/**
 * Initializes message runtime meter 
 *  
 * @param msg_id : message id 
 */
extern void CHMessage_runtime_meter_init(L7_uint msg_id);

/**
 * Prints meter information
 */
extern void CHMessage_runtime_meter_print(void);

#endif /* CHMESSAGEHANDLER_H_ */
