/**
 * CHMessageHandler.c 
 *  
 * Created on: 
 * Author: 
 *  
 * Modified on 2011/06/14 
 * By: Alexandre Santos (alexandre-r-santos@ptinovacao.pt) 
 * Notes: major code refactoring
 *  
 */

#include "ptin_include.h"
#include "ptin_msg.h"
#include "tty_ptin.h"
#include "sirerrors.h"
#include "ptin_control.h"
#include "ptin_mgmd_api.h"
#include "ptin_debug.h"//Added by MMelo to use ptin_timer routines
#include "ipc.h"
#include <ptin_intf.h>
extern L7_RC_t ptin_ptp_fpga_entry(ptin_dtl_search_ptp_t *e, DAPI_CMD_GET_SET_t operation);
extern L7_RC_t ptin_oam_fpga_entry(ptin_dtl_search_oam_t *e, DAPI_CMD_GET_SET_t operation);

/* Message processing time measuring */

#define CCMSG_ID_MIN  0x9000
#define CCMSG_ID_MAX  0x91FF

typedef struct
{
  uint32 number_of_calls;
  uint32 total_runtime;
  uint32 last_runtime;
  uint32 min_runtime;
  uint32 max_runtime;
  uint32 average_runtime;
} struct_runtime_t;

static struct_runtime_t msg_runtime[CCMSG_ID_MAX-CCMSG_ID_MIN+1];

static void CHMessage_runtime_meter_update(L7_uint msg_id, L7_uint32 time_delta);

#define MSG_N_ELEMS(size)   (inbuffer->infoDim / (size))

#define SETIPC_INFODIM(size) { \
  outbuffer->infoDim = size;  \
}

/* Macro to set ACK OK on output message */
#define SETIPCACKOK(outbuffer) { \
  outbuffer->flags = IPCLIB_FLAGS_ACK; \
  outbuffer->infoDim = sizeof(L7_uint32); \
  *((L7_uint32 *) outbuffer->info) = ENDIAN_SWAP32(0);   \
}
#define SETIPCNACK(outbuffer, res) { \
  outbuffer->flags = IPCLIB_FLAGS_NACK; \
  outbuffer->infoDim = sizeof(unsigned int); \
  *(unsigned int*)outbuffer->info = ENDIAN_SWAP32((unsigned int) res); \
}


/* Macro to check infoDim consistency */
#define CHECK_INFO_SIZE_ATLEAST(msg_st) {             \
  if (inbuffer->infoDim < sizeof(msg_st)) {  \
    PT_LOG_ERR(LOG_CTX_MSGHANDLER, "Data size inconsistent! Expecting at least %u bytes; Received %u bytes!", sizeof(msg_st), inbuffer->infoDim);\
    res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE); \
    SetIPCNACK(outbuffer, res);               \
    break;                                    \
  }                                           \
}

/* Macro to check infoDim consistency */
#define CHECK_INFO_SIZE(msg_st) {             \
  if (inbuffer->infoDim != sizeof(msg_st)) {  \
    PT_LOG_ERR(LOG_CTX_MSGHANDLER, "Data size inconsistent! Expecting %u bytes; Received %u bytes!", sizeof(msg_st), inbuffer->infoDim);\
    res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE); \
    SetIPCNACK(outbuffer, res);               \
    break;                                    \
  }                                           \
}

/* Macro to check infoDim consistency (including modulo match) */
#define CHECK_INFO_SIZE_MOD(msg_st) {             \
  if ( ((inbuffer->infoDim % sizeof(msg_st)) != 0)) {  \
    PT_LOG_ERR(LOG_CTX_MSGHANDLER, "Data size inconsistent! Expecting multiple of %u bytes; Received %u bytes", sizeof(msg_st), inbuffer->infoDim);\
    res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE); \
    SetIPCNACK(outbuffer, res);               \
    break;                                    \
  }                                           \
}

/* Macro to check infoDim consistency */
#define CHECK_INFO_SIZE_ATLEAST_ABS(size_bytes) {             \
  if (inbuffer->infoDim < size_bytes) {  \
    PT_LOG_ERR(LOG_CTX_MSGHANDLER, "Data size inconsistent! Expecting at least %u bytes; Received %u bytes!", size_bytes, inbuffer->infoDim);\
    res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE); \
    SetIPCNACK(outbuffer, res);               \
    break;                                    \
  }                                           \
}

/* Macro to check infoDim consistency */
#define CHECK_INFO_SIZE_ABS(size_bytes) {             \
  if (inbuffer->infoDim != size_bytes) {  \
    PT_LOG_ERR(LOG_CTX_MSGHANDLER, "Data size inconsistent! Expecting %u bytes; Received %u bytes!", size_bytes, inbuffer->infoDim);\
    res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE); \
    SetIPCNACK(outbuffer, res);               \
    break;                                    \
  }                                           \
}


static L7_uint16 SIRerror_get(L7_RC_t error_code)
{
  switch ((int) error_code)
  {
    case L7_FAILURE:
    case L7_ERROR:
      return ERROR_CODE_INVALIDPARAM;

    case L7_REQUEST_DENIED:
      return ERROR_CODE_USED;

    case L7_NOT_IMPLEMENTED_YET:
      return ERROR_CODE_NOTIMPLEMENTED;

    case L7_NOT_EXIST:
      return ERROR_CODE_NOSUCHNAME;

    case L7_TABLE_IS_FULL:
      return ERROR_CODE_FULLTABLE;

    case L7_NOT_SUPPORTED:
      return ERROR_CODE_NOTSUPPORTED;

    case L7_DEPENDENCY_NOT_MET:
      return ERROR_CODE_FP_EVC_UNKNOWN;
    case L7_ADDR_INUSE:
      return ERROR_COUNTER_USED;
    case L7_NO_VALUE:
      return ERROR_NO_MORE_VLAN_AVAILABLE;
  }

  // Default error
  return ERROR_CODE_INVALIDPARAM;
}













int ipc_msg_bytes_debug_enable(int disable0_enable1_read2) {
static int enable=0;

 switch (disable0_enable1_read2) {
 case 0:
 case 1: enable=disable0_enable1_read2; break;
 }

 return enable;
}










/**
 * Message handler for the PTin Module.
 * 
 * @param inbuffer Input IPC message
 * @param outbuffer Output IPC message
 * 
 * @return int Error code (SIR_ERROR list)
 */
int CHMessageHandler (ipc_msg *inbuffer, ipc_msg *outbuffer)
{
  L7_uint32 msgId, infoDim;
  int ret = IPC_OK, res = S_OK;
  L7_RC_t rc = L7_SUCCESS;
  L7_uint64 time_start, time_end;
  L7_uint32 time_delta;

  if (inbuffer == NULL)
  {
    PT_LOG_WARN(LOG_CTX_MSGHANDLER, "NULL message received!");
    return SIR_ERROR(ERROR_FAMILY_IPC, ERROR_SEVERITY_ERROR, ERROR_CODE_EMPTYMSG);
  }

  /* Get initial time (in microseconds) */
  time_start = osapiTimeMicrosecondsGet();

  /* Prepare message header */
  SwapIPCHeader(inbuffer, outbuffer);

  msgId   = inbuffer->msgId;
  infoDim = inbuffer->infoDim;

  /* Commands allowed to be run, even if switchdrvr is busy */
  /* If reached here, means PTin module is loaded and ready to process messages */
  switch (msgId)
  {
    /************************************************************************** 
     * Application Utils Processing
     **************************************************************************/

    /* If message is a ping, reply with PTin loading state, which can signal crash errors too */
    case CCMSG_APPLICATION_IS_ALIVE:
    {
      PT_LOG_TRACE(LOG_CTX_MSGHANDLER, "Message received: CCMSG_APPLICATION_IS_ALIVE (0x%04X)", msgId);

      PT_LOG_TRACE(LOG_CTX_MSGHANDLER, "PTin state: %d", ptin_state);
      *((L7_uint32 *) outbuffer->info) = ENDIAN_SWAP32((L7_uint32) ptin_state);
      SETIPC_INFODIM(sizeof(L7_uint32));

      PT_LOG_TRACE(LOG_CTX_MSGHANDLER,
                  "Message processed: response with %d bytes", ENDIAN_SWAP32(outbuffer->infoDim));
      return IPC_OK;
    }

    /* CCMSG_APP_CHANGE_STDOUT ************************************************/
    case CCMSG_APP_CHANGE_STDOUT:
    {
      PT_LOG_INFO(LOG_CTX_MSGHANDLER, "Message received: CCMSG_APP_CHANGE_STDOUT (0x%04X)", msgId);
      PT_LOG_NOTICE(LOG_CTX_MSGHANDLER, "Redirecting stdout...");

      /* Validate message data */
      if (infoDim == 0)
      {
        res = SIR_ERROR(ERROR_FAMILY_IPC, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
        SetIPCNACK(outbuffer, res);
        return IPC_OK;
      }

      /* Apply change */
      ptin_PitHandler((char *) &inbuffer->info[0]);

      SETIPCACKOK(outbuffer);

      PT_LOG_NOTICE(LOG_CTX_MSGHANDLER, "...Stdout redirected to here :-)");
      return IPC_OK;  /* CCMSG_APP_CHANGE_STDOUT */
    }

    case CCMSG_APP_LOGGER_OUTPUT:
    {
      PT_LOG_INFO(LOG_CTX_MSGHANDLER, "Message received: CCMSG_APP_LOGGER_OUTPUT (0x%04X)", msgId);

      L7_uint8 output;
      char *filename;

      /* If infodim is null, use stdout */
      if (infoDim == 0)
      {
        PT_LOG_NOTICE(LOG_CTX_MSGHANDLER, "Redirecting logger output (0) to \"%s\"...", LOG_OUTPUT_FILE_DEFAULT);
        logger_redirect(LOG_OUTPUT_FILE, LOG_OUTPUT_FILE_DEFAULT);
        ptin_mgmd_logredirect(MGMD_LOG_FILE, LOG_OUTPUT_FILE_DEFAULT);
        PT_LOG_NOTICE(LOG_CTX_MSGHANDLER, "...Logger output (0) redirected to \"%s\" :-)", LOG_OUTPUT_FILE_DEFAULT);
      }
      /* Otherwise, use the specified filename */
      else if (infoDim == 1 || inbuffer->info[1] == '\0')
      {
        output = LOG_OUTPUT_FILE + inbuffer->info[0];

        PT_LOG_NOTICE(LOG_CTX_MSGHANDLER, "Redirecting logger output (%u) to \"%s\"...", output, LOG_OUTPUT_FILE_DEFAULT);
        logger_redirect(output, LOG_OUTPUT_FILE_DEFAULT);
        if (output == LOG_OUTPUT_FILE)
        {
          ptin_mgmd_logredirect(MGMD_LOG_FILE, LOG_OUTPUT_FILE_DEFAULT); 
        }
        PT_LOG_NOTICE(LOG_CTX_MSGHANDLER, "...Logger output (%u) redirected to \"%s\" :-)", output, LOG_OUTPUT_FILE_DEFAULT);
      }
      else
      {
        output = LOG_OUTPUT_FILE + inbuffer->info[0];
        filename = (char *) &inbuffer->info[1];

        PT_LOG_NOTICE(LOG_CTX_MSGHANDLER, "Redirecting logger output (%u) to \"%s\"...", output, filename);
        logger_redirect(output, filename);
        if (output == LOG_OUTPUT_FILE)
        {
          ptin_mgmd_logredirect(MGMD_LOG_FILE, filename);
        }
        PT_LOG_NOTICE(LOG_CTX_MSGHANDLER, "...Logger output (%u) redirected to \"%s\" :-)", output, filename);
      }
      SETIPCACKOK(outbuffer);
      return IPC_OK;  /* CCMSG_APP_CHANGE_STDOUT */
    }

    /* CCMSG_APP_SHELL_CMD_RUN ************************************************/
    case CCMSG_APP_SHELL_CMD_RUN:
    {
      PT_LOG_INFO(LOG_CTX_MSGHANDLER, "Message received: CCMSG_APP_SHELL_CMD_RUN (0x%04X)", msgId);

      if (infoDim == 0)
      {
        res = SIR_ERROR(ERROR_FAMILY_IPC, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
        SetIPCNACK(outbuffer, res);
        return IPC_OK;
      }
      if (ptin_msg_ShellCommand_run((L7_char8 *) &inbuffer->info[0]) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSGHANDLER, "Error on ptin_msg_ShellCommand_run()");
        res = SIR_ERROR(ERROR_FAMILY_IPC, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
        SetIPCNACK(outbuffer, res);
        return IPC_OK;
      }
      SETIPCACKOK(outbuffer);
      return IPC_OK;  /* CCMSG_APP_SHELL_CMD_RUN */
    }


    /* CCMSG_BOARD_SHOW *******************************************************/
    case CCMSG_BOARD_SHOW:
    {
      PT_LOG_INFO(LOG_CTX_MSGHANDLER, "Message received: CCMSG_BOARD_SHOW (0x%04X)", msgId);

      CHECK_INFO_SIZE(0);

      msg_FWFastpathInfo *fpInfo;
      fpInfo = (msg_FWFastpathInfo *) outbuffer->info;

      /* Get values */
      ptin_msg_FPInfo_get(fpInfo);  /* returns always success */

      /* Copy slot id information if available! */
      if (infoDim >= 1)
        outbuffer->info[0] = inbuffer->info[0];

      SETIPC_INFODIM(sizeof(msg_FWFastpathInfo));
      PT_LOG_INFO(LOG_CTX_MSGHANDLER,
                  "Message processed: response with %d bytes (present=%d)", outbuffer->infoDim, ENDIAN_SWAP32(fpInfo->BoardPresent));

      return IPC_OK;  /* CCMSG_BOARD_SHOW */
    }
  }

  /* If switchdrvr is busy, return FP_BUSY code error */
  if (ptin_state == PTIN_STATE_BUSY)
  {
    res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_FP_BUSY);
    SetIPCNACK(outbuffer, res);
    PT_LOG_WARN(LOG_CTX_MSGHANDLER, "IPC message cannot be processed! PTin state = %d (msgId=0x%x) -> error 0x%08x", ptin_state, msgId, res);
    return IPC_OK;
  }
  /* PTin module is still loading or crashed ? */
  else if (ptin_state != PTIN_STATE_READY)
  {
    res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_NOTALLOWED);
    SetIPCNACK(outbuffer, res);
    PT_LOG_WARN(LOG_CTX_MSGHANDLER, "IPC message cannot be processed! PTin state = %d (msgId=0x%x) -> error 0x%08x", ptin_state, msgId, res);
    return IPC_OK;
  }

  if (ipc_msg_bytes_debug_enable(2))
  {
    L7_uint i;

    if (inbuffer == NULL)
    {
        PT_LOG_WARN(LOG_CTX_MSGHANDLER, "NULL message received!");
        return SIR_ERROR(ERROR_FAMILY_IPC, ERROR_SEVERITY_ERROR, ERROR_CODE_EMPTYMSG);
    }
    printf("\n\rmsgId=0x%04x inbuffer->infoDim=%u:", inbuffer->msgId, inbuffer->infoDim);
    for(i=0; i<inbuffer->infoDim; i++)
    {
      if ((i % 32) == 0)
      {
        printf("\r\n%04x:", i);
      }
      printf(" %02x", inbuffer->info[i]);
    }
    printf("\n\r");
  }

  /* If reached here, means PTin module is loaded and ready to process messages */
  switch (msgId)
  {
    /************************************************************************** 
     * Misc Processing
     **************************************************************************/

    /* CCMSG_ALARMS_RESET *****************************************************/
    case CCMSG_ALARMS_RESET:
    {
      PT_LOG_INFO(LOG_CTX_MSGHANDLER, "Message received: CCMSG_ALARMS_RESET (0x%04X)", msgId);

      CHECK_INFO_SIZE(msg_HwGenReq_t);

      /* Execute command */
      ptin_msg_alarms_reset();

      break;  /* CCMSG_ALARMS_RESET */
    }

    /* CCMSG_DEFAULTS_RESET ***************************************************/
    case CCMSG_DEFAULTS_RESET:
    {
      PT_LOG_INFO(LOG_CTX_MSGHANDLER, "Message received: CCMSG_DEFAULTS_RESET (0x%04X)", msgId);

      CHECK_INFO_SIZE(msg_HwGenReq_t);
      #if 0
      msg_HwGenReq_t *config;
      config = (msg_HwGenReq_t *) inbuffer->info;
      ptin_msg_defaults_reset(config->param);
      SETIPCACKOK(outbuffer);
      #else
      /* Execute command */      
      ptin_msg_task_process(msgId, (void*)(inbuffer->info), infoDim, 3000);
      SETIPCACKOK(outbuffer);
      #endif

      break;  /* CCMSG_DEFAULTS_RESET */
    }


    case CCMSG_SWITCH_TEMPERATURE_GET:
    {
      PT_LOG_TRACE(LOG_CTX_MSGHANDLER, "Message received: CCMSG_SWITCH_TEMPERATURE_GET (0x%04X)", msgId);

      msg_ptin_temperature_monitor_t *ptr;

      ptr = (msg_ptin_temperature_monitor_t *) outbuffer->info;
      memset(ptr, 0x00, sizeof(msg_ptin_temperature_monitor_t));

      /* Execute command */
      rc = ptin_msg_switch_temperature_get(ptr);

      if (L7_SUCCESS != rc)
      {
        PT_LOG_ERR(LOG_CTX_MSGHANDLER, "Error reading temperature");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPC_INFODIM(sizeof(msg_ptin_temperature_monitor_t));
    }
    break;



    default:
    {
      PT_LOG_WARN(LOG_CTX_MSGHANDLER, "Message received: UNKNOWN! (0x%04X)\n", msgId);
      PT_LOG_WARN(LOG_CTX_MSGHANDLER,
                  "The received message is not supported!");
      SetIPCNACK (outbuffer, SIR_ERROR(ERROR_FAMILY_IPC, ERROR_SEVERITY_WARNING, ERROR_CODE_NOSUCHMSG));

      rc = L7_FAILURE;
      break;
    }

  }

  /* Save final time */
  time_end = osapiTimeMicrosecondsGet();
  time_delta = time_end - time_start;

  
  if( 0)//msgId == CCMSG_ETH_PHY_ACTIVITY_GET || 
      //msgId == CCMSG_HW_INTF_INFO_GET     ||
      //msgId == CCMSG_ETH_PHY_COUNTERS_GET ||
      //msgId == CCMSG_SWITCH_TEMPERATURE_GET ||
      //msgId == CCMSG_ETH_LACP_MATRIXES_SYNC2 || 
      //msgId == CCMSG_MGMD_PORT_SYNC || 
      //msgId == CCMSG_APPLICATION_RESOURCES)
    PT_LOG_TRACE(LOG_CTX_MSGHANDLER,"Message processed: 0x%04X in %lu usec [response:%u (bytes) rc=%u res=0x%08x]", msgId, outbuffer->infoDim, time_delta, rc, res);
  else
    PT_LOG_INFO(LOG_CTX_MSGHANDLER,"Message processed: 0x%04X in %lu usec  [response:%u (bytes) rc=%u res=0x%08x]", msgId, outbuffer->infoDim, time_delta, rc, res);

  /* Message Runtime Meter */
  /* Only for successfull messages */
  if (rc==L7_SUCCESS)
  {
    CHMessage_runtime_meter_update(msgId, time_delta);
  }

  return ret;
}


/**
 * Update message runtime meter
 * 
 * @param msg_id : message id
 * @param time_delta : time taken to process message
 */
static void CHMessage_runtime_meter_update(L7_uint msg_id, L7_uint32 time_delta)
{
  struct_runtime_t *msg_runtime_ptr;

  if (msg_id<CCMSG_ID_MIN && msg_id>CCMSG_ID_MAX)
  {
    return;
  }

  msg_runtime_ptr = &msg_runtime[msg_id-CCMSG_ID_MIN];

  /* If overflow, reset all data */
  if ( (msg_runtime_ptr->number_of_calls==(L7_uint32)-1) ||
       (msg_runtime_ptr->total_runtime + time_delta < msg_runtime_ptr->total_runtime) )
  {
    CHMessage_runtime_meter_init(msg_id);
  }

  /* Update database */
  msg_runtime_ptr->number_of_calls++;
  msg_runtime_ptr->last_runtime    = time_delta;
  msg_runtime_ptr->total_runtime  += time_delta;
  if ( (msg_runtime_ptr->number_of_calls)==1 || time_delta<(msg_runtime_ptr->min_runtime) )
    msg_runtime_ptr->min_runtime = time_delta;
  if ( time_delta > msg_runtime_ptr->max_runtime )
    msg_runtime_ptr->max_runtime = time_delta;
  msg_runtime_ptr->average_runtime = msg_runtime_ptr->total_runtime/msg_runtime_ptr->number_of_calls;
}

/**
 * Initializes message runtime meter 
 *  
 * @param msg_id : message id 
 */
void CHMessage_runtime_meter_init(L7_uint msg_id)
{
  if (msg_id<CCMSG_ID_MIN || msg_id>CCMSG_ID_MAX)
  {
    memset(msg_runtime,0x00,sizeof(msg_runtime));
  }
  else
  {
    memset(&msg_runtime[msg_id-CCMSG_ID_MIN],0x00,sizeof(struct_runtime_t));
  }
}

/**
 * Prints meter information
 */
void CHMessage_runtime_meter_print(void)
{
  L7_uint i;

  printf(" --------------------------------------------------------------------------------------------------------- \r\n");
  printf("| Message id |   #Calls   | Total runtime |  Last runtime |  Min. runtime |  Max. runtime | Avrg. runtime |\r\n");
  printf("|------------|------------|---------------|---------------|---------------|---------------|---------------|\r\n");

  for (i=0; i<CCMSG_ID_MAX-CCMSG_ID_MIN+1; i++)
  {
    if (msg_runtime[i].number_of_calls==0)  continue;

    printf("|   0x%04X   | %10u | %10u us | %10u us | %10u us | %10u us | %10u us |\r\n",
           i+CCMSG_ID_MIN,
           msg_runtime[i].number_of_calls,
           msg_runtime[i].total_runtime,
           msg_runtime[i].last_runtime,
           msg_runtime[i].min_runtime,
           msg_runtime[i].max_runtime,
           msg_runtime[i].average_runtime);
  }

  printf(" --------------------------------------------------------------------------------------------------------- \r\n");

  fflush(stdout);
}

