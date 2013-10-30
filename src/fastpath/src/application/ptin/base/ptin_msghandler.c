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

/* Message processing time measuring */

#define CCMSG_ID_MIN  0x9000
#define CCMSG_ID_MAX  0x90FF

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


/* Macro to set ACK OK on output message */
#define SETIPCACKOK(outbuf) {          \
  outbuf->infoDim = sizeof(L7_uint32); \
  *((L7_uint32 *) outbuf->info) = 0;   \
}

/* Macro to check infoDim consistency */
#define CHECK_INFO_SIZE_ATLEAST(msg_st) {             \
  if (inbuffer->infoDim < sizeof(msg_st)) {  \
    LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Data size inconsistent! Expecting at leat %u bytes; Received %u bytes!", sizeof(msg_st), inbuffer->infoDim);\
    res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE); \
    SetIPCNACK(outbuffer, res);               \
    break;                                    \
  }                                           \
}

/* Macro to check infoDim consistency */
#define CHECK_INFO_SIZE(msg_st) {             \
  if (inbuffer->infoDim != sizeof(msg_st)) {  \
    LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Data size inconsistent! Expecting %u bytes; Received %u bytes!", sizeof(msg_st), inbuffer->infoDim);\
    res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE); \
    SetIPCNACK(outbuffer, res);               \
    break;                                    \
  }                                           \
}

/* Macro to check infoDim consistency (including modulo match) */
#define CHECK_INFO_SIZE_MOD(msg_st) {             \
  if ((inbuffer->infoDim != sizeof(msg_st)) && ((inbuffer->infoDim % sizeof(msg_st)) != 0)) {  \
    LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Data size inconsistent! Expecting multiple of %u bytes; Received %u bytes", sizeof(msg_st), inbuffer->infoDim);\
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
      return ERROR_CODE_NOTALLOWED;

    case L7_NOT_IMPLEMENTED_YET:
      return ERROR_CODE_NOTIMPLEMENTED;

    case L7_NOT_EXIST:
      return ERROR_CODE_NOSUCHNAME;

    case L7_TABLE_IS_FULL:
      return ERROR_CODE_FULLTABLE;

    case L7_NOT_SUPPORTED:
      return ERROR_CODE_NOTSUPPORTED;
  }

  // Default error
  return ERROR_CODE_INVALIDPARAM;
}













#ifdef __802_1x__
static void seterror(ipc_msg *outbuff, const L7_ulong32 severity, const L7_ulong32 error) {
    outbuff->flags   = (IPCLIB_FLAGS_NACK);
    outbuff->infoDim = sizeof(int);
    *(int *)outbuff->info = SIR_ERROR(ERROR_FAMILY_HARDWARE,severity,error);
}









//Function for generic message reading/writing n STRUCT_SIZE structs*************************************************
//Uses the particular method for reading/writing each struct, "msg_generic_wrd_1struc"*******************************
static int msg_generic_wrd(int (*msg_generic_wrd_1struct)(ipc_msg *inbuff, ipc_msg *outbuff, L7_ulong32 i), ipc_msg *inbuff, ipc_msg *outbuff, L7_ulong32 STRUCT_SIZE_IN, L7_ulong32 STRUCT_SIZE_OUT)
{
  L7_ushort16 i,n;

  LOG_INFO(LOG_CTX_PTIN_MSGHANDLER, "Message received: 0x%04X", inbuff->msgId);

  //CHECK_INFO_SIZE_MOD(msg_ptin_pcs_prbs);
  if (inbuff->infoDim > IPCLIB_MAX_MSGSIZE     ||      inbuff->infoDim % STRUCT_SIZE_IN !=0)
  {
    seterror(outbuff, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE); //seterror(outbuff, ERROR_SEVERITY_DEBUG, HW_INVALID_MSG_SIZE);
    LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Data size inconsistent! Expecting multiple of %u bytes; Received %u bytes", STRUCT_SIZE_IN, inbuff->infoDim);
    return(0);
  }

  outbuff->flags   = (IPCLIB_FLAGS_ACK);

  n = inbuff->infoDim/STRUCT_SIZE_IN;

  if (STRUCT_SIZE_IN>=STRUCT_SIZE_OUT)  //0..n-1
  {
    for (i=0;    i<n;    i++)
    {
     if ((*msg_generic_wrd_1struct)((void*)inbuff, (void*)outbuff, i)) {
       outbuff->flags = (IPCLIB_FLAGS_NACK);
       LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error WRDing data");
     }
    }
  }
  else
  {
    if (n*STRUCT_SIZE_OUT>IPCLIB_MAX_MSGSIZE)
    {
     seterror(outbuff, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);   //seterror(outbuff, ERROR_SEVERITY_DEBUG, HW_INVALID_OUTPUT_MSG_SIZE);
     return(0);
    }
    for (i=n;  i;  i--) //i=n-1..0
    {
      if ((*msg_generic_wrd_1struct)((void*)inbuff, (void*)outbuff, i-1)) {
          outbuff->flags = (IPCLIB_FLAGS_NACK);
          LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error WRDing data");
      }
    }
  }

  outbuff->infoDim = n*STRUCT_SIZE_OUT;  //inbuff->infoDim;
  LOG_INFO(LOG_CTX_PTIN_MSGHANDLER, "Message processed: response with %d bytes", outbuff->infoDim);
  return(0);
}//msg_generic_wrd
#endif //__802_1x__










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
  int res = S_OK;
  L7_RC_t rc = L7_SUCCESS;
  L7_uint64 time_start, time_end;
  L7_uint32 time_delta;

  if (inbuffer == NULL)
  {
    LOG_WARNING(LOG_CTX_PTIN_MSGHANDLER, "NULL message received!");
    return SIR_ERROR(ERROR_FAMILY_IPC, ERROR_SEVERITY_ERROR, ERROR_CODE_EMPTYMSG);
  }

  /* Get initial time (in microseconds) */
  time_start = osapiTimeMicrosecondsGet();

  /* Prepare message header */
  SwapIPCHeader (inbuffer, outbuffer);

  /* If message is a ping, reply with PTin loading state, which can signal crash errors too */
  if (inbuffer->msgId == CCMSG_APPLICATION_IS_ALIVE)
  {
    LOG_TRACE(LOG_CTX_PTIN_MSGHANDLER,
              "Message received: CCMSG_APPLICATION_IS_ALIVE (0x%04X)", CCMSG_APPLICATION_IS_ALIVE);

    LOG_TRACE(LOG_CTX_PTIN_MSGHANDLER, "PTin state: %d", ptin_state);
    outbuffer->infoDim = sizeof(L7_uint32);
    *((L7_uint32 *) outbuffer->info) = ptin_state;

    LOG_TRACE(LOG_CTX_PTIN_MSGHANDLER,
              "Message processed: response with %d bytes", outbuffer->infoDim);

    return S_OK;
  }

  /* PTin module is still loading or crashed ? */
  if (ptin_state != PTIN_LOADED)
  {
    LOG_WARNING(LOG_CTX_PTIN_MSGHANDLER, "IPC message cannot be processed! PTin state = %d (msgId=%u)", ptin_state, inbuffer->msgId);
    res = SIR_ERROR(ERROR_FAMILY_IPC, ERROR_SEVERITY_ERROR, ERROR_CODE_NOTALLOWED);
    SetIPCNACK(outbuffer, res);
    return S_OK;
  }

  /* If reached here, means PTin module is loaded and ready to process messages */
  switch (inbuffer->msgId)
  {
    /************************************************************************** 
     * Application Utils Processing
     **************************************************************************/

    /* CCMSG_APP_CHANGE_STDOUT ************************************************/
    case CCMSG_APP_CHANGE_STDOUT:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_APP_CHANGE_STDOUT (0x%04X)", CCMSG_APP_CHANGE_STDOUT);
      LOG_NOTICE(LOG_CTX_PTIN_MSGHANDLER, "Redirecting stdout...");

      /* Validate message data */
      if (inbuffer->infoDim == 0)
      {
        res = SIR_ERROR(ERROR_FAMILY_IPC, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
        SetIPCNACK(outbuffer, res);
        break;
      }

      /* Apply change */
      ptin_PitHandler((char *) &inbuffer->info[0]);

      SETIPCACKOK(outbuffer);

      LOG_NOTICE(LOG_CTX_PTIN_MSGHANDLER, "...Stdout redirected to here :-)");
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_APP_CHANGE_STDOUT */
    }

    case CCMSG_APP_LOGGER_OUTPUT:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_APP_LOGGER_OUTPUT (0x%04X)", CCMSG_APP_LOGGER_OUTPUT);
      LOG_NOTICE(LOG_CTX_PTIN_MSGHANDLER, "Redirecting logger output...");

      /* If infodim is null, use stdout */
      if (inbuffer->infoDim==0)
      {
        log_redirect(LOG_OUTOUT_STDOUT, L7_NULLPTR);
        LOG_NOTICE(LOG_CTX_PTIN_MSGHANDLER, "...Logger redirected to stdout :-)");
      }
      /* Else if null length, use default filename */
      else if (inbuffer->info[0]=='\0')
      {
        log_redirect(LOG_OUTPUT_FILE, L7_NULLPTR);
        LOG_NOTICE(LOG_CTX_PTIN_MSGHANDLER, "...Logger redirected to \"%s\" :-)", LOG_OUTPUT_FILE_DEFAULT);
      }
      /* Otherwise, use the specified filename */
      else
      {
        log_redirect(LOG_OUTPUT_FILE, (char *) &inbuffer->info[0]);
        LOG_NOTICE(LOG_CTX_PTIN_MSGHANDLER, "...Logger redirected to \"%s\" :-)", (char *) &inbuffer->info[0]);
      }

      SETIPCACKOK(outbuffer);

      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_APP_CHANGE_STDOUT */
    }

    /* CCMSG_APP_SHELL_CMD_RUN ************************************************/
    case CCMSG_APP_SHELL_CMD_RUN:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_APP_SHELL_CMD_RUN (0x%04X)", CCMSG_APP_SHELL_CMD_RUN);

      if (inbuffer->infoDim == 0)
      {
        res = SIR_ERROR(ERROR_FAMILY_IPC, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
        SetIPCNACK(outbuffer, res);
        break;
      }

      if (ptin_msg_ShellCommand_run((L7_char8 *) &inbuffer->info[0]) != L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error on ptin_msg_ShellCommand_run()");
        res = SIR_ERROR(ERROR_FAMILY_IPC, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
        SetIPCNACK(outbuffer, res);

        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_APP_SHELL_CMD_RUN */
    }


    /************************************************************************** 
     * Misc Processing
     **************************************************************************/

    /* CCMSG_BOARD_SHOW *******************************************************/
    case CCMSG_BOARD_SHOW:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_BOARD_SHOW (0x%04X)", CCMSG_BOARD_SHOW);

      CHECK_INFO_SIZE(0);

      msg_FWFastpathInfo *fpInfo;
      fpInfo = (msg_FWFastpathInfo *) outbuffer->info;

      /* Get values */
      ptin_msg_FPInfo_get(fpInfo);  /* returns always success */

      /* Copy slot id information if available! */
      if (inbuffer->infoDim >= 1)
        outbuffer->info[0] = inbuffer->info[0];

      outbuffer->infoDim = sizeof(msg_FWFastpathInfo);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes (present=%d)", outbuffer->infoDim,fpInfo->BoardPresent);

      break;  /* CCMSG_BOARD_SHOW */
    }


    /* CCMSG_ALARMS_RESET *****************************************************/
    case CCMSG_ALARMS_RESET:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ALARMS_RESET (0x%04X)", CCMSG_ALARMS_RESET);

      CHECK_INFO_SIZE(msg_HwGenReq_t);

      /* Execute command */
      ptin_msg_alarms_reset();

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ALARMS_RESET */
    }

    /* CCMSG_APPLICATION_RESOURCES *********************************************/
    case CCMSG_APPLICATION_RESOURCES:
    {
      LOG_TRACE(LOG_CTX_PTIN_MSGHANDLER,
                "Message received: CCMSG_APPLICATION_RESOURCES (0x%04X)", CCMSG_APPLICATION_RESOURCES);

      CHECK_INFO_SIZE_MOD(msg_ptin_policy_resources);

      /* Copy inbuffer to outbuffer */
      memcpy(outbuffer->info,inbuffer->info,sizeof(msg_ptin_policy_resources));

      msg_ptin_policy_resources *resources;
      resources = (msg_ptin_policy_resources *) outbuffer->info;

      /* Get values */
      if ( L7_SUCCESS != (rc=ptin_msg_hw_resources_get(resources)) )
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while consulting hardware resources");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      outbuffer->infoDim = sizeof(msg_ptin_policy_resources);
      LOG_TRACE(LOG_CTX_PTIN_MSGHANDLER,
                "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_APPLICATION_RESOURCES */
    }

    /* CCMSG_DEFAULTS_RESET ***************************************************/
    case CCMSG_DEFAULTS_RESET:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_DEFAULTS_RESET (0x%04X)", CCMSG_DEFAULTS_RESET);

      CHECK_INFO_SIZE(msg_HwGenReq_t);

      /* Execute command */
      ptin_msg_defaults_reset();

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_DEFAULTS_RESET */
    }

    /* CCMSG_MULTICAST_MACHINE_RESET *******************************************/
    case CCMSG_MULTICAST_MACHINE_RESET:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_MULTICAST_MACHINE_RESET (0x%04X)", CCMSG_MULTICAST_MACHINE_RESET);

      CHECK_INFO_SIZE(msg_HwGenReq_t);

      msg_HwGenReq_t *ptr = (msg_HwGenReq_t *) &inbuffer->info[0];

      /* Reset multicast machine */
      rc = ptin_msg_multicast_reset(ptr);

      /* Error? */
      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error sending data");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }
      /* Success */
      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);
      break;  /* CCMSG_DEFAULTS_RESET */
    }

    /* CCMSG_TYPEB_PROT_SWITCH *******************************************/
    case CCMSG_TYPEB_PROT_SWITCH:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_TYPEB_PROT_SWITCH (0x%04X)", CCMSG_TYPEB_PROT_SWITCH);

      CHECK_INFO_SIZE(msg_HwTypeBprot_t);

      msg_HwTypeBprot_t *ptr = (msg_HwTypeBprot_t *) &inbuffer->info[0];

      /* TYPE B Protection Switching */
      rc = ptin_msg_typeBprotSwitch(ptr);

      /* Error? */
      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error sending data");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }
      /* Success */
      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);
      break;  /* CCMSG_DEFAULTS_RESET */
    }

    /************************************************************************** 
     * SLOT MODE CONFIGURATION
     **************************************************************************/

    case CCMSG_SLOT_MAP_MODE_GET:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_SLOT_MAP_MODE_GET (0x%04X)", inbuffer->msgId);

      CHECK_INFO_SIZE_ATLEAST(L7_uint32);
      msg_slotModeCfg_t *ptr;

      memcpy(outbuffer->info, inbuffer->info, sizeof(msg_slotModeCfg_t));
      ptr = (msg_slotModeCfg_t *) outbuffer->info;

      /* Execute command */
      rc = ptin_msg_slotMode_get(ptr);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error reading slot map");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      outbuffer->infoDim = sizeof(msg_slotModeCfg_t);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);
    }
    break;

    case CCMSG_SLOT_MAP_MODE_VALIDATE:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_SLOT_MAP_MODE_VALIDATE (0x%04X)", inbuffer->msgId);

      CHECK_INFO_SIZE_MOD(msg_slotModeCfg_t);

      msg_slotModeCfg_t *ptr = (msg_slotModeCfg_t *) inbuffer->info;

      /* Execute command */
      rc = ptin_msg_slotMode_validate(ptr);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error sending data");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);
    }
    break;

    case CCMSG_SLOT_MAP_MODE_APPLY:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_SLOT_MAP_MODE_APPLY (0x%04X)", inbuffer->msgId);

      //CHECK_INFO_SIZE_MOD(msg_slotModeCfg_t);

      //msg_slotModeCfg_t *ptr = (msg_slotModeCfg_t *) inbuffer->info;

      /* Execute command */
      rc = ptin_msg_slotMode_apply();

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error sending data");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);
    }
    break;

    /************************************************************************** 
     * PHY CONFIG Processing
     **************************************************************************/

    case CCMSG_ETH_PHY_STATUS_GET:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_PHY_STATUS_GET (0x%04X)", CCMSG_ETH_PHY_STATUS_GET);

      CHECK_INFO_SIZE_ATLEAST(L7_uint32);

      msg_HWEthPhyStatus_t *pin     = (msg_HWEthPhyStatus_t *) inbuffer->info;

      #if (PTIN_SYSTEM_N_PONS > 0 || PTIN_SYSTEM_N_ETH > 0)
      L7_uint i;
      msg_HWEthPhyStatus_t *pout    = (msg_HWEthPhyStatus_t *) outbuffer->info;

      /* Output info read */
      LOG_DEBUG(LOG_CTX_PTIN_MSG, "Requesting...");
      LOG_DEBUG(LOG_CTX_PTIN_MSG, " SlotId    = %u", pin->SlotId);
      LOG_DEBUG(LOG_CTX_PTIN_MSG, " BoardType = %u", pin->BoardType );
      LOG_DEBUG(LOG_CTX_PTIN_MSG, " PortId    = %u", pin->Port );

      /* Single port ? */
      if (pin->Port < max(PTIN_SYSTEM_N_PONS, PTIN_SYSTEM_N_ETH))
      {
        memcpy(pout, pin, sizeof(msg_HWEthPhyStatus_t));

        if (ptin_msg_PhyStatus_get(pout) != L7_SUCCESS)
        {
          LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while getting port status (port# %u)", pin->Port);
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
          SetIPCNACK(outbuffer, res);
          break;
        }

        outbuffer->infoDim = sizeof(msg_HWEthPhyStatus_t);
      }
      /* Swipe all ports */
      else
      {
        for (i = 0; i < PTIN_SYSTEM_N_ETH; i++)
        {
          memcpy(&pout[i], pin, sizeof(msg_HWEthPhyStatus_t));
          pout[i].Port = i;

          if (ptin_msg_PhyStatus_get(&pout[i]) != L7_SUCCESS)
            break;
        }

        /* Error? */
        if (i < PTIN_SYSTEM_N_ETH)
        {
          LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while getting port Status (port# %u)", pin->Port);
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
          SetIPCNACK(outbuffer, res);
          break;
        }

        outbuffer->infoDim = sizeof(msg_HWEthPhyStatus_t) * i;
      }
      #else
      LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while getting port Status (port# %u)", pin->Port);
      res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
      SetIPCNACK(outbuffer, res);
      break;
      #endif

      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_PHY_STATUS_GET */
    }

    /* CCMSG_ETH_PHY_CONFIG_SET ***********************************************/
    case CCMSG_ETH_PHY_CONFIG_SET:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_PHY_CONFIG_SET (0x%04X)", CCMSG_ETH_PHY_CONFIG_SET);

      CHECK_INFO_SIZE_MOD(msg_HWEthPhyConf_t);

      L7_uint i, n;
      msg_HWEthPhyConf_t *pi = (msg_HWEthPhyConf_t *) &inbuffer->info[0];

      /* Validate info size */
      if ( (inbuffer->infoDim < sizeof(msg_HWEthPhyConf_t)) || ((inbuffer->infoDim % sizeof(msg_HWEthPhyConf_t)) != 0) )
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Data size inconsistent! (%u)", inbuffer->infoDim);
        res = SIR_ERROR(ERROR_FAMILY_IPC, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
        SetIPCNACK(outbuffer, res);
        break;
      }

      n = inbuffer->infoDim / sizeof(msg_HWEthPhyConf_t);
      if (n > PTIN_SYSTEM_N_PORTS)
        n = PTIN_SYSTEM_N_PORTS;

      /* Apply config */
      for (i = 0; i < n; i++)
        if (L7_SUCCESS != ptin_msg_PhyConfig_set(&pi[i]))
          break;

      if (i != n)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while setting ports configuration (port# %u)", i);
        res = SIR_ERROR(ERROR_FAMILY_IPC, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_PHY_CONFIG_SET */
    }


    /* CCMSG_ETH_PHY_CONFIG_GET ***********************************************/
    case CCMSG_ETH_PHY_CONFIG_GET:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_PHY_CONFIG_GET (0x%04X)", CCMSG_ETH_PHY_CONFIG_GET);

      CHECK_INFO_SIZE(msg_HwGenReq_t);

      L7_uint i;
      msg_HwGenReq_t     *req  = (msg_HwGenReq_t *) &inbuffer->info[0];
      msg_HWEthPhyConf_t *pout = (msg_HWEthPhyConf_t *) &outbuffer->info[0];
      msg_HWEthPhyConf_t  pin[1];

      /* Reference structure */
      memset(pin,0x00,sizeof(msg_HWEthPhyConf_t));
      pin->SlotId = req->slot_id;
      pin->Port   = req->generic_id;
      pin->Mask   = 0xffff;

      /* Single port ? */
      if (pin->Port < PTIN_SYSTEM_N_PORTS)
      {
        memcpy(pout, pin, sizeof(msg_HWEthPhyConf_t));

        if (ptin_msg_PhyConfig_get(pout) != L7_SUCCESS)
        {
          LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while getting port configuration (port# %u)", pin->Port);
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
          SetIPCNACK(outbuffer, res);
          break;
        }

        outbuffer->infoDim = sizeof(msg_HWEthPhyConf_t);
      }
      /* Swipe all ports */
      else
      {
        for (i = 0; i < PTIN_SYSTEM_N_PORTS; i++)
        {
          memcpy(&pout[i], pin, sizeof(msg_HWEthPhyConf_t));
          pout[i].Port = i;

          if (ptin_msg_PhyConfig_get(&pout[i]) != L7_SUCCESS)
            break;
        }

        /* Error? */
        if (i != PTIN_SYSTEM_N_PORTS)
        {
          LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while getting port configuration (port# %u)", pin->Port);
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
          SetIPCNACK(outbuffer, res);
          break;
        }

        outbuffer->infoDim = sizeof(msg_HWEthPhyConf_t) * i;
      }

      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_PHY_CONFIG_GET */
    }


    /* CCMSG_ETH_PHY_STATE_GET ************************************************/
    case CCMSG_ETH_PHY_STATE_GET:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_PHY_STATE_GET (0x%04X)", CCMSG_ETH_PHY_STATE_GET);

      CHECK_INFO_SIZE(msg_HwGenReq_t);

      L7_uint i;
      msg_HwGenReq_t      *request = (msg_HwGenReq_t *) inbuffer->info;
      msg_HWEthPhyState_t *pout    = (msg_HWEthPhyState_t *) outbuffer->info;
      msg_HWEthPhyState_t  pin[1];

      /* Reference structure */
      memset(pin,0x00,sizeof(msg_HWEthPhyState_t));
      pin->SlotId = request->slot_id;
      pin->Port   = request->generic_id;
      pin->Mask   = 0xffff;

      /* Single port ? */
      if (pin->Port < PTIN_SYSTEM_N_PORTS)
      {
        memcpy(pout, pin, sizeof(msg_HWEthPhyState_t));

        if (ptin_msg_PhyState_get(pout) != L7_SUCCESS)
        {
          LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while getting port state (port# %u)", pin->Port);
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
          SetIPCNACK(outbuffer, res);
          break;
        }

        outbuffer->infoDim = sizeof(msg_HWEthPhyState_t);
      }
      /* Swipe all ports */
      else
      {
        for (i = 0; i < PTIN_SYSTEM_N_PORTS; i++)
        {
          memcpy(&pout[i], pin, sizeof(msg_HWEthPhyState_t));
          pout[i].Port = i;

          if (ptin_msg_PhyState_get(&pout[i]) != L7_SUCCESS)
            break;
        }

        /* Error? */
        if (i != PTIN_SYSTEM_N_PORTS)
        {
          LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while getting port state (port# %u)", pin->Port);
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
          SetIPCNACK(outbuffer, res);
          break;
        }

        outbuffer->infoDim = sizeof(msg_HWEthPhyState_t) * i;
      }

      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_PHY_STATE_GET */
    }

    /* CCMSG_ETH_PHY_ACTIVITY_GET ************************************************/
    case CCMSG_ETH_PHY_ACTIVITY_GET:
    {
      LOG_TRACE(LOG_CTX_PTIN_MSGHANDLER,
                "Message received: CCMSG_ETH_PHY_ACTIVITY_GET (0x%04X)", CCMSG_ETH_PHY_ACTIVITY_GET);

      CHECK_INFO_SIZE(msg_HWEthPhyActivity_t);

      msg_HWEthPhyActivity_t *pin  = (msg_HWEthPhyActivity_t *) inbuffer->info;
      msg_HWEthPhyActivity_t *pout = (msg_HWEthPhyActivity_t *) outbuffer->info;

      /* Reference structure */
      memcpy(pout, pin, sizeof(msg_HWEthPhyActivity_t));

      if (ptin_msg_PhyActivity_get(pout) != L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while getting port activity (slot=%u/%u)", pin->intf.slot, pin->intf.port);
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
        SetIPCNACK(outbuffer, res);
        break;
      }

      outbuffer->infoDim = sizeof(msg_HWEthPhyActivity_t);

      LOG_TRACE(LOG_CTX_PTIN_MSGHANDLER,
                "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_PHY_STATE_GET */
    }

    /************************************************************************** 
     * PHY COUNTERS Processing
     **************************************************************************/

    /* CCMSG_ETH_PHY_COUNTERS_GET *********************************************/
    case CCMSG_ETH_PHY_COUNTERS_GET:
    {
      LOG_TRACE(LOG_CTX_PTIN_MSGHANDLER,
                "Message received: CCMSG_ETH_PHY_COUNTERS_GET (0x%04X)", CCMSG_ETH_PHY_COUNTERS_GET);

      CHECK_INFO_SIZE_MOD(msg_HwGenReq_t);

      msg_HwGenReq_t                    *request;
      msg_HWEthRFC2819_PortStatistics_t *portStats;
      L7_uint nElems = inbuffer->infoDim / sizeof(msg_HwGenReq_t);

      request   = (msg_HwGenReq_t *) inbuffer->info;
      portStats = (msg_HWEthRFC2819_PortStatistics_t *) outbuffer->info;

      /* Get values */
      if (L7_SUCCESS != ptin_msg_PhyCounters_read(request,portStats,nElems))
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while getting counters (port# %u)", portStats->Port);
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
        SetIPCNACK(outbuffer, res);
        break;
      }

      outbuffer->infoDim = sizeof(msg_HWEthRFC2819_PortStatistics_t)*nElems;
      LOG_TRACE(LOG_CTX_PTIN_MSGHANDLER,
                "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_PHY_COUNTERS_GET */
    }


    /* CCMSG_ETH_PHY_COUNTERS_CLEAR *******************************************/
    case CCMSG_ETH_PHY_COUNTERS_CLEAR:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_PHY_COUNTERS_CLEAR (0x%04X)", CCMSG_ETH_PHY_COUNTERS_CLEAR);

      CHECK_INFO_SIZE(msg_HWEthRFC2819_PortStatistics_t);

      msg_HWEthRFC2819_PortStatistics_t *portStats;
      portStats = (msg_HWEthRFC2819_PortStatistics_t *) inbuffer->info;

      /* Execute command */
      if (L7_SUCCESS != ptin_msg_PhyCounters_clear(portStats))
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while clearing counters (port# %u)", portStats->Port);
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
                "Message processed: response with %d bytes", outbuffer->infoDim);

      break; /* CCMSG_ETH_PHY_COUNTERS_CLEAR */
    }

    /************************************************************************** 
     * Port Type Settings
     **************************************************************************/

    /* Set Port type (MEF extension) configuration */
    case CCMSG_ETH_PORT_EXT_SET:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_PORT_EXT_SET (0x%04X)", inbuffer->msgId);

      CHECK_INFO_SIZE_MOD(msg_HWPortExt_t);

      msg_HWPortExt_t *ptr = (msg_HWPortExt_t *) inbuffer->info;
      L7_uint nElems = inbuffer->infoDim/sizeof(msg_HWPortExt_t);

      /* Execute command */
      rc = ptin_msg_portExt_set(ptr, nElems);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error sending data");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);
    }
    break;

    /* Get Port type (MEF extension) configuration */
    case CCMSG_ETH_PORT_EXT_GET:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_PORT_TYPE_GET (0x%04X)", inbuffer->msgId);

      CHECK_INFO_SIZE(msg_HWPortExt_t);

      msg_HWPortExt_t *ptr;
      L7_uint          nElems;

      memcpy(outbuffer->info, inbuffer->info, sizeof(msg_HWPortExt_t));
      ptr = (msg_HWPortExt_t *) outbuffer->info;

      /* Execute command */
      rc = ptin_msg_portExt_get(ptr,&nElems);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error reading data");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      outbuffer->infoDim = sizeof(msg_HWPortExt_t)*nElems;
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);
    }
    break;

    /* Set MAC address */
    case CCMSG_ETH_PORT_MAC_SET:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_PORT_MAC_SET (0x%04X)", inbuffer->msgId);

      CHECK_INFO_SIZE_MOD(msg_HWPortMac_t);

      msg_HWPortMac_t *ptr = (msg_HWPortMac_t *) inbuffer->info;
      L7_uint nElems = inbuffer->infoDim/sizeof(msg_HWPortMac_t);

      /* Execute command */
      rc = ptin_msg_portMAC_set(ptr, nElems);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error sending data");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);
    }
    break;

    /* Get MAC address */
    case CCMSG_ETH_PORT_MAC_GET:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_PORT_MAC_GET (0x%04X)", inbuffer->msgId);

      CHECK_INFO_SIZE(msg_HWPortMac_t);

      msg_HWPortMac_t *ptr;
      L7_uint          nElems;

      memcpy(outbuffer->info, inbuffer->info, sizeof(msg_HWPortMac_t));
      ptr = (msg_HWPortMac_t *) outbuffer->info;

      /* Execute command */
      rc = ptin_msg_portMAC_get(ptr, &nElems);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error reading data");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      outbuffer->infoDim = sizeof(msg_HWPortMac_t)*nElems;
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);
    }
    break;

    /************************************************************************** 
     * QoS Processing
     **************************************************************************/

    /* Get CoS configuration */
    case CCMSG_ETH_PORT_COS_GET:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_PORT_COS_GET (0x%04X)", inbuffer->msgId);

      CHECK_INFO_SIZE(msg_QoSConfiguration_t);

      msg_QoSConfiguration_t *ptr;
      ptr = (msg_QoSConfiguration_t *) outbuffer->info;

      memcpy(outbuffer->info, inbuffer->info, sizeof(msg_QoSConfiguration_t));

      /* Execute command */
      rc = ptin_msg_CoS_get(ptr);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error getting data");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      outbuffer->infoDim = sizeof(msg_QoSConfiguration_t);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);
    }
    break;

    /* Set new CoS configuration */
    case CCMSG_ETH_PORT_COS_SET:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_PORT_COS_SET (0x%04X)", inbuffer->msgId);

      CHECK_INFO_SIZE(msg_QoSConfiguration_t);

      msg_QoSConfiguration_t *ptr;
      ptr = (msg_QoSConfiguration_t *) inbuffer->info;

      /* Execute command */
      rc = ptin_msg_CoS_set(ptr);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error sending data");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);
    }
    break;

    /************************************************************************** 
     * LAGs Processing
     **************************************************************************/

    /* CCMSG_ETH_LACP_LAG_GET *************************************************/
    case CCMSG_ETH_LACP_LAG_GET:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_LACP_LAG_GET (0x%04X)", CCMSG_ETH_LACP_LAG_GET);

      CHECK_INFO_SIZE(msg_LACPLagInfo_t);

      msg_LACPLagInfo_t *request = (msg_LACPLagInfo_t *) inbuffer->info;
      msg_LACPLagInfo_t *lagInfo = (msg_LACPLagInfo_t *) outbuffer->info;
      L7_uint            nElems;

      memcpy(lagInfo, request, sizeof(msg_LACPLagInfo_t));

      /* Execute command */
      if (L7_SUCCESS != ptin_msg_Lag_get(lagInfo, &nElems))
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while getting LAGs info");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
        SetIPCNACK(outbuffer, res);
        break;
      }

      outbuffer->infoDim = sizeof(msg_LACPLagInfo_t) * nElems;
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_LACP_LAG_GET */
    }


    /* CCMSG_ETH_LACP_LAG_ADD *************************************************/
    case CCMSG_ETH_LACP_LAG_ADD:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_LACP_LAG_ADD (0x%04X)", CCMSG_ETH_LACP_LAG_ADD);

      CHECK_INFO_SIZE(msg_LACPLagInfo_t);

      msg_LACPLagInfo_t *lagInfo;
      lagInfo = (msg_LACPLagInfo_t *) inbuffer->info;

      /* Execute command */
      if (L7_SUCCESS != ptin_msg_Lag_create(lagInfo))
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while creating a LAG");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_LACP_LAG_ADD */
    }


    /* CCMSG_ETH_LACP_LAG_REMOVE **********************************************/
    case CCMSG_ETH_LACP_LAG_REMOVE:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_LACP_LAG_REMOVE (0x%04X)", CCMSG_ETH_LACP_LAG_REMOVE);

      CHECK_INFO_SIZE(msg_LACPLagInfo_t);

      msg_LACPLagInfo_t *lagInfo;
      lagInfo = (msg_LACPLagInfo_t *) inbuffer->info;

      /* Execute command */
      if (L7_SUCCESS != ptin_msg_Lag_destroy(lagInfo))
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while destroying a LAG");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_LACP_LAG_REMOVE */
    }


    /* CCMSG_ETH_LACP_LAG_STATUS_GET ******************************************/
    case CCMSG_ETH_LACP_LAG_STATUS_GET:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_LACP_LAG_STATUS_GET (0x%04X)", CCMSG_ETH_LACP_LAG_STATUS_GET);

      CHECK_INFO_SIZE(msg_LACPLagStatus_t);

      msg_LACPLagStatus_t *request   = (msg_LACPLagStatus_t *) inbuffer->info;
      msg_LACPLagStatus_t *lagStatus = (msg_LACPLagStatus_t *) outbuffer->info;
      L7_uint              nElems=0;

      memcpy(lagStatus, request, sizeof(msg_LACPLagStatus_t));

      /* Execute command */
      if (L7_SUCCESS != ptin_msg_LagStatus_get(lagStatus, &nElems))
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while getting LAGs status");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
        SetIPCNACK(outbuffer, res);
        break;
      }

      outbuffer->infoDim = sizeof(msg_LACPLagStatus_t) * nElems;
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_LACP_LAG_STATUS_GET */
    }


    /* CCMSG_ETH_LACP_ADMINSTATE_SET ******************************************/
    case CCMSG_ETH_LACP_ADMINSTATE_SET:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_LACP_ADMINSTATE_SET (0x%04X)", CCMSG_ETH_LACP_ADMINSTATE_SET);

      CHECK_INFO_SIZE_MOD(msg_LACPAdminState_t);

      L7_uint nElems = inbuffer->infoDim / sizeof(msg_LACPAdminState_t);
      msg_LACPAdminState_t *lagAdminState = (msg_LACPAdminState_t *) inbuffer->info;

      /* Execute command */
      if (L7_SUCCESS != ptin_msg_LACPAdminState_set(lagAdminState, nElems))
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while setting LACP admin state");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_LACP_ADMINSTATE_SET */
    }


    /* CCMSG_ETH_LACP_ADMINSTATE_GET ******************************************/
    case CCMSG_ETH_LACP_ADMINSTATE_GET:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_LACP_ADMINSTATE_GET (0x%04X)", CCMSG_ETH_LACP_ADMINSTATE_GET);

      CHECK_INFO_SIZE(msg_LACPAdminState_t);

      L7_uint nElems;
      msg_LACPAdminState_t *request       = (msg_LACPAdminState_t *) inbuffer->info;
      msg_LACPAdminState_t *lagAdminState = (msg_LACPAdminState_t *) outbuffer->info;

      memcpy(lagAdminState,request,sizeof(msg_LACPAdminState_t));

      /* Execute command */
      if (L7_SUCCESS != ptin_msg_LACPAdminState_get(lagAdminState, &nElems))
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while getting LACP admin state");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
        SetIPCNACK(outbuffer, res);
        break;
      }

      outbuffer->infoDim = sizeof(msg_LACPAdminState_t) * nElems;
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_LACP_ADMINSTATE_GET */
    }


    /* CCMSG_ETH_LACP_STATS_GET ***********************************************/
    case CCMSG_ETH_LACP_STATS_GET:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_LACP_STATS_GET (0x%04X)", CCMSG_ETH_LACP_STATS_GET);

      CHECK_INFO_SIZE(msg_LACPStats_t);

      L7_uint nElems;
      msg_LACPStats_t  *request = (msg_LACPStats_t *) inbuffer->info;
      msg_LACPStats_t *lagStats = (msg_LACPStats_t *) outbuffer->info;

      /* Note: the index field provides the LAG nr (if out of range, all LAGs are returned) */
      memcpy(lagStats, request, sizeof(msg_LACPStats_t));

      /* Execute command */
      if (L7_SUCCESS != ptin_msg_LACPStats_get(lagStats, &nElems))
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while getting LACP stats");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
        SetIPCNACK(outbuffer, res);
        break;
      }

      outbuffer->infoDim = sizeof(msg_LACPStats_t) * nElems;
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_LACP_STATS_GET */
    }


    /* CCMSG_ETH_LACP_STATS_CLEAR *********************************************/
    case CCMSG_ETH_LACP_STATS_CLEAR:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_LACP_STATS_CLEAR (0x%04X)", CCMSG_ETH_LACP_STATS_CLEAR);

      CHECK_INFO_SIZE(msg_LACPStats_t);

      msg_LACPStats_t *lagStats;
      lagStats = (msg_LACPStats_t *) inbuffer->info;

      /* Execute command */
      if (L7_SUCCESS != ptin_msg_LACPStats_clear(lagStats))
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while clearing LACP stats");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_LACP_STATS_CLEAR */
    }

    /************************************************************************** 
     * L2 Table Processing
     **************************************************************************/

    /* L2 Aging get */
    case CCMSG_ETH_SWITCH_CONFIG_GET:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_SWITCH_CONFIG_GET (0x%04X)", CCMSG_ETH_SWITCH_CONFIG_GET);
      CHECK_INFO_SIZE(msg_switch_config_t);

      msg_switch_config_t *switch_config = (msg_switch_config_t *) &outbuffer->info[0];
      memcpy(outbuffer->info, inbuffer->info, sizeof(msg_switch_config_t));

      /* Execute command */
      rc = ptin_msg_l2_switch_config_get(switch_config);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while getting config");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      outbuffer->infoDim = sizeof(msg_HwEthMef10Evc_t);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);
    }
    break;

    /* L2 Aging set */
    case CCMSG_ETH_SWITCH_CONFIG_SET:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_SWITCH_CONFIG_SET (0x%04X)", CCMSG_ETH_SWITCH_CONFIG_SET);
      CHECK_INFO_SIZE(msg_switch_config_t);

      msg_switch_config_t *switch_config = (msg_switch_config_t *) &inbuffer->info[0];

      /* Execute command */
      rc = ptin_msg_l2_switch_config_set(switch_config);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while setting config");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);
    }
    break;

    /* Get L2 table */
    case CCMSG_ETH_MAC_TABLE_SHOW:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_MAC_TABLE_SHOW (0x%04X)", CCMSG_ETH_MAC_TABLE_SHOW);
      CHECK_INFO_SIZE(msg_switch_mac_intro_t);

      msg_switch_mac_table_t *mac_table = (msg_switch_mac_table_t *) outbuffer->info;
      memcpy(outbuffer->info, inbuffer->info, sizeof(msg_switch_mac_table_t));

      /* Execute command */
      rc = ptin_msg_l2_macTable_get(mac_table);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while getting MAC list");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      outbuffer->infoDim = sizeof(msg_switch_mac_table_t);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);
    }
    break;

    /* Remove an entry of the L2 table */
    case CCMSG_ETH_MAC_ENTRY_REMOVE:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_MAC_ENTRY_REMOVE (0x%04X)", CCMSG_ETH_MAC_ENTRY_REMOVE);
      CHECK_INFO_SIZE(msg_switch_mac_table_t);

      msg_switch_mac_table_t *mac_table = (msg_switch_mac_table_t *) inbuffer->info;

      /* Execute command */
      rc = ptin_msg_l2_macTable_remove(mac_table);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while removing MAC");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);
    }
    break;

    /* Add an entry to the L2 table */
    case CCMSG_ETH_MAC_ENTRY_ADD:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_MAC_ENTRY_ADD (0x%04X)", CCMSG_ETH_MAC_ENTRY_ADD);
      CHECK_INFO_SIZE(msg_switch_mac_table_t);

      msg_switch_mac_table_t *mac_table = (msg_switch_mac_table_t *) inbuffer->info;

      /* Execute command */
      rc = ptin_msg_l2_macTable_add(mac_table);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while adding MAC");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);
    }
    break;

    /************************************************************************** 
     * EVCs Processing
     **************************************************************************/

    /* CCMSG_ETH_EVC_GET ******************************************************/
    case CCMSG_ETH_EVC_GET:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_EVC_GET (0x%04X)", CCMSG_ETH_EVC_GET);
  
      CHECK_INFO_SIZE(msg_HwEthMef10Evc_t);

      msg_HwEthMef10Evc_t *evcConf;
      evcConf = (msg_HwEthMef10Evc_t *) outbuffer->info;

      memcpy(outbuffer->info, inbuffer->info, sizeof(msg_HwEthMef10Evc_t));

      /* Execute command */
      if (L7_SUCCESS != ptin_msg_EVC_get(evcConf))
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while getting EVC# %u config", evcConf->id);
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
        SetIPCNACK(outbuffer, res);
        break;
      }

      outbuffer->infoDim = sizeof(msg_HwEthMef10Evc_t);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_EVC_GET */
    }


    /* CCMSG_ETH_EVC_ADD ******************************************************/
    case CCMSG_ETH_EVC_ADD:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_EVC_ADD (0x%04X)", CCMSG_ETH_EVC_ADD);

      CHECK_INFO_SIZE(msg_HwEthMef10Evc_t);

      msg_HwEthMef10Evc_t *evcConf;
      evcConf = (msg_HwEthMef10Evc_t *) inbuffer->info;

      /* Execute command */
      if (L7_SUCCESS != ptin_msg_EVC_create(evcConf))
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while creating EVC# %u", evcConf->id);
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_EVC_ADD */
    }


    /* CCMSG_ETH_EVC_REMOVE ***************************************************/
    case CCMSG_ETH_EVC_REMOVE:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_EVC_REMOVE (0x%04X)", CCMSG_ETH_EVC_REMOVE);

      CHECK_INFO_SIZE_MOD(msg_HwEthMef10EvcRemove_t);

      msg_HwEthMef10EvcRemove_t *evcConf = (msg_HwEthMef10EvcRemove_t *) inbuffer->info;
      L7_uint16 n_structs = inbuffer->infoDim/sizeof(msg_HwEthMef10EvcRemove_t);

      /* Execute command */
      if (L7_SUCCESS != ptin_msg_EVC_delete(evcConf, n_structs))
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while deleting EVCs");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_EVC_REMOVE */
    }

    /* CCMSG_ETH_EVC_PORT_ADD ******************************************************/
    case CCMSG_ETH_EVC_PORT_ADD:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_EVC_PORT_ADD (0x%04X)", CCMSG_ETH_EVC_PORT_ADD);

      CHECK_INFO_SIZE_MOD(msg_HWevcPort_t);

      msg_HWevcPort_t *evcPort = (msg_HWevcPort_t *) inbuffer->info;
      L7_uint16        n_size  = inbuffer->infoDim/sizeof(msg_HWevcPort_t);

      /* Execute command */
      if (L7_SUCCESS != ptin_msg_evc_port(evcPort, n_size, PTIN_MSG_OPER_ADD))
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while adding port to EVC");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_EVC_PORT_ADD */
    }

    /* CCMSG_ETH_EVC_PORT_REMOVE ***************************************************/
    case CCMSG_ETH_EVC_PORT_REMOVE:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_EVC_PORT_REMOVE (0x%04X)", CCMSG_ETH_EVC_PORT_REMOVE);

      CHECK_INFO_SIZE_MOD(msg_HWevcPort_t);

      msg_HWevcPort_t *evcPort = (msg_HWevcPort_t *) inbuffer->info;
      L7_uint16        n_size  = inbuffer->infoDim/sizeof(msg_HWevcPort_t);

      /* Execute command */
      if (L7_SUCCESS != ptin_msg_evc_port(evcPort, n_size, PTIN_MSG_OPER_REMOVE))
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while removing port from EVC");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_EVC_PORT_REMOVE */
    }

    /* CCMSG_ETH_EVC_BRIDGE_ADD ***********************************************/
    case CCMSG_ETH_EVC_BRIDGE_ADD:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_EVC_BRIDGE_ADD (0x%04X)", CCMSG_ETH_EVC_BRIDGE_ADD);

      CHECK_INFO_SIZE(msg_HwEthEvcBridge_t);

      msg_HwEthEvcBridge_t *evcBridge;
      evcBridge = (msg_HwEthEvcBridge_t *) inbuffer->info;

      /* Execute command */
      if (L7_SUCCESS != ptin_msg_EVCBridge_add(evcBridge))
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while adding a bridge to EVC# %u", evcBridge->evcId);
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_EVC_BRIDGE_ADD */
    }


    /* CCMSG_ETH_EVC_BRIDGE_REMOVE ********************************************/
    case CCMSG_ETH_EVC_BRIDGE_REMOVE:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_EVC_BRIDGE_REMOVE (0x%04X)", CCMSG_ETH_EVC_BRIDGE_REMOVE);

      CHECK_INFO_SIZE(msg_HwEthEvcBridge_t);

      msg_HwEthEvcBridge_t *evcBridge;
      evcBridge = (msg_HwEthEvcBridge_t *) inbuffer->info;

      /* Execute command */
      rc = ptin_msg_EVCBridge_remove(evcBridge);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while removing a bridge to EVC# %u", evcBridge->evcId);
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_EVC_BRIDGE_REMOVE */
    }

    /* CCMSG_ETH_EVC_FLOW_ADD ***********************************************/
    case CCMSG_ETH_EVC_FLOW_ADD:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_EVC_FLOW_ADD (0x%04X)", CCMSG_ETH_EVC_FLOW_ADD);

      CHECK_INFO_SIZE(msg_HwEthEvcFlow_t);

      msg_HwEthEvcFlow_t *evcFlow;
      evcFlow = (msg_HwEthEvcFlow_t *) inbuffer->info;

      /* Execute command */
      if (L7_SUCCESS != ptin_msg_EVCFlow_add(evcFlow))
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while adding a flow to eEVC# %u", evcFlow->evcId);
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_EVC_FLOW_ADD */
    }

    /* CCMSG_ETH_EVC_FLOW_REMOVE ********************************************/
    case CCMSG_ETH_EVC_FLOW_REMOVE:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_EVC_FLOW_REMOVE (0x%04X)", CCMSG_ETH_EVC_FLOW_REMOVE);

      CHECK_INFO_SIZE(msg_HwEthEvcFlow_t);

      msg_HwEthEvcFlow_t *evcFlow;
      evcFlow = (msg_HwEthEvcFlow_t *) inbuffer->info;

      /* Execute command */
      rc = ptin_msg_EVCFlow_remove(evcFlow);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while removing a flow from eEVC# %u", evcFlow->evcId);
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_EVC_BRIDGE_REMOVE */
    }

    /* Add vlan to be flooded */
    case CCMSG_ETH_EVC_FLOOD_VLAN_ADD:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_EVC_FLOOD_VLAN_ADD (0x%04X)", CCMSG_ETH_EVC_FLOOD_VLAN_ADD);

      CHECK_INFO_SIZE_MOD(msg_HwEthEvcFloodVlan_t);

      msg_HwEthEvcFloodVlan_t *evcFlood;
      L7_uint32 n_clients;

      evcFlood  = (msg_HwEthEvcFloodVlan_t *) inbuffer->info;
      n_clients = inbuffer->infoDim / (sizeof(msg_HwEthEvcFloodVlan_t));

      /* Execute command */
      if (L7_SUCCESS != ptin_msg_EvcFloodVlan_add(evcFlood, n_clients))
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while adding a flood vlan to EVC# %u", evcFlood->evcId);
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_EVC_FLOOD_VLAN_ADD */
    }

    /* Remove vlan to be flooded */
    case CCMSG_ETH_EVC_FLOOD_VLAN_REMOVE:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_EVC_FLOOD_VLAN_REMOVE (0x%04X)", CCMSG_ETH_EVC_FLOOD_VLAN_REMOVE);

      CHECK_INFO_SIZE_MOD(msg_HwEthEvcFloodVlan_t);

      msg_HwEthEvcFloodVlan_t *evcFlood;
      L7_uint32 n_clients;

      evcFlood  = (msg_HwEthEvcFloodVlan_t *) inbuffer->info;
      n_clients = inbuffer->infoDim / (sizeof(msg_HwEthEvcFloodVlan_t));

      /* Execute command */
      if (L7_SUCCESS != ptin_msg_EvcFloodVlan_remove(evcFlood, n_clients))
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while removing a flood vlan to EVC# %u", evcFlood->evcId);
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_EVC_FLOOD_VLAN_ADD */
    }

    /************************************************************************** 
     * EVCs Counters config
     **************************************************************************/

    /* CCMSG_ETH_EVC_COUNTERS_GET *********************************************/
    case CCMSG_ETH_EVC_COUNTERS_GET:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_EVC_COUNTERS_GET (0x%04X)", CCMSG_ETH_EVC_COUNTERS_GET);

      CHECK_INFO_SIZE(msg_evcStats_t);

      msg_evcStats_t *evcstat_in, *evcstat_out;
      evcstat_in  = (msg_evcStats_t *) inbuffer->info;
      evcstat_out = (msg_evcStats_t *) outbuffer->info;

      memcpy(evcstat_out,evcstat_in,sizeof(msg_evcStats_t));

      /* Execute command */
      if (L7_SUCCESS != ptin_msg_evcStats_get(evcstat_out))
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while reading EVC stats");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
        SetIPCNACK(outbuffer, res);
        break;
      }

      outbuffer->infoDim = sizeof(msg_evcStats_t);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_EVC_COUNTERS_GET */
    }


    /* CCMSG_ETH_EVC_COUNTERS_ADD *********************************************/
    case CCMSG_ETH_EVC_COUNTERS_ADD:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_EVC_COUNTERS_ADD (0x%04X)", CCMSG_ETH_EVC_COUNTERS_ADD);

      CHECK_INFO_SIZE(msg_evcStats_t);

      msg_evcStats_t *evc_stat;
      evc_stat = (msg_evcStats_t *) inbuffer->info;

      /* Execute command */
      if (L7_SUCCESS != ptin_msg_evcStats_set(evc_stat))
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while adding EVC stats");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_EVC_COUNTERS_ADD */
    }

    /* CCMSG_ETH_EVC_COUNTERS_REMOVE ******************************************/
    case CCMSG_ETH_EVC_COUNTERS_REMOVE:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_EVC_COUNTERS_REMOVE (0x%04X)", CCMSG_ETH_EVC_COUNTERS_REMOVE);

      CHECK_INFO_SIZE(msg_evcStats_t);

      msg_evcStats_t *evc_stat;
      evc_stat = (msg_evcStats_t *) inbuffer->info;

      /* Execute command */
      rc = ptin_msg_evcStats_delete(evc_stat);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while removing EVC stats");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_EVC_COUNTERS_REMOVE */
    }


    /************************************************************************** 
     * Bandwidth profiles Config
     **************************************************************************/

    /* CCMSG_ETH_BW_PROFILE_GET ***********************************************/
    case CCMSG_ETH_BW_PROFILE_GET:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_BW_PROFILE_GET (0x%04X)", CCMSG_ETH_BW_PROFILE_GET);

      CHECK_INFO_SIZE(msg_HwEthBwProfile_t);

      msg_HwEthBwProfile_t *bwProfile_in, *bwProfile_out;
      bwProfile_in  = (msg_HwEthBwProfile_t *) inbuffer->info;
      bwProfile_out = (msg_HwEthBwProfile_t *) outbuffer->info;

      memcpy(bwProfile_out,bwProfile_in,sizeof(msg_HwEthBwProfile_t));

      /* Execute command */
      if (L7_SUCCESS != ptin_msg_bwProfile_get(bwProfile_out))
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while getting BW profile");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
        SetIPCNACK(outbuffer, res);
        break;
      }

      outbuffer->infoDim = sizeof(msg_HwEthBwProfile_t);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_BW_PROFILE_GET */
    }


    /* CCMSG_ETH_BW_PROFILE_SET ***********************************************/
    case CCMSG_ETH_BW_PROFILE_SET:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_BW_PROFILE_SET (0x%04X)", CCMSG_ETH_BW_PROFILE_SET);

      CHECK_INFO_SIZE(msg_HwEthBwProfile_t);

      msg_HwEthBwProfile_t *bwProfile;
      bwProfile = (msg_HwEthBwProfile_t *) inbuffer->info;

      /* Execute command */
      if (L7_SUCCESS != ptin_msg_bwProfile_set(bwProfile))
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while setting BW profile");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_BW_PROFILE_SET */
    }


    /* CCMSG_ETH_BW_PROFILE_DELETE ********************************************/
    case CCMSG_ETH_BW_PROFILE_DELETE:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_BW_PROFILE_DELETE (0x%04X)", CCMSG_ETH_BW_PROFILE_DELETE);

      CHECK_INFO_SIZE(msg_HwEthBwProfile_t);

      msg_HwEthBwProfile_t *bwProfile;
      bwProfile = (msg_HwEthBwProfile_t *) inbuffer->info;

      rc = ptin_msg_bwProfile_delete(bwProfile);

      /* Execute command */
      if ( L7_SUCCESS != rc )
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while removing BW profile");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_BW_PROFILE_DELETE */
    }

    /* CCMSG_ETH_STORM_CONTROL_GET ***********************************************/
    case CCMSG_ETH_STORM_CONTROL_GET:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_STORM_CONTROL_GET (0x%04X)", CCMSG_ETH_STORM_CONTROL_GET);

      CHECK_INFO_SIZE(msg_HwEthStormControl_t);

      msg_HwEthStormControl_t *stormControl_in, *stormControl_out;
      stormControl_in  = (msg_HwEthStormControl_t *) inbuffer->info;
      stormControl_out = (msg_HwEthStormControl_t *) outbuffer->info;

      memcpy(stormControl_out, stormControl_in, sizeof(msg_HwEthStormControl_t));

      /* Execute command */
      if (L7_SUCCESS != ptin_msg_stormControl_get(stormControl_out))
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while Storm Control profile");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
        SetIPCNACK(outbuffer, res);
        break;
      }

      outbuffer->infoDim = sizeof(msg_HwEthStormControl_t);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_STORM_CONTROL_GET */
    }

    /* CCMSG_ETH_STORM_CONTROL_SET ***********************************************/
    case CCMSG_ETH_STORM_CONTROL_SET:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_STORM_CONTROL_SET (0x%04X)", CCMSG_ETH_STORM_CONTROL_SET);

      CHECK_INFO_SIZE(msg_HwEthStormControl_t);

      msg_HwEthStormControl_t *stormControl;
      stormControl = (msg_HwEthStormControl_t *) inbuffer->info;

      /* Execute command */
      if (L7_SUCCESS != ptin_msg_stormControl_set(stormControl))
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while setting Storm Control profile");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_STORM_CONTROL_SET */
    }

    /* CCMSG_ETH_STORM_CONTROL_CLEAR ********************************************/
    case CCMSG_ETH_STORM_CONTROL_CLEAR:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_STORM_CONTROL_CLEAR (0x%04X)", CCMSG_ETH_STORM_CONTROL_CLEAR);

      CHECK_INFO_SIZE(msg_HwEthStormControl_t);

      msg_HwEthStormControl_t *stormControl;
      stormControl = (msg_HwEthStormControl_t *) inbuffer->info;

      rc = ptin_msg_stormControl_clear(stormControl);

      /* Execute command */
      if ( L7_SUCCESS != rc )
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while Storm Control profile");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_BW_PROFILE_DELETE */
    }

    /************************************************************************** 
     * inBand Config
     **************************************************************************/

    /* CCMSG_ETH_NTW_CONNECTIVITY_GET *****************************************/
    case CCMSG_ETH_NTW_CONNECTIVITY_GET:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_NTW_CONNECTIVITY_GET (0x%04X)", CCMSG_ETH_NTW_CONNECTIVITY_GET);

      CHECK_INFO_SIZE(msg_NtwConnectivity_t);

      msg_NtwConnectivity_t *ntwConn;
      ntwConn = (msg_NtwConnectivity_t *) outbuffer->info;

      memcpy(outbuffer->info, inbuffer->info, sizeof(msg_NtwConnectivity_t));

      /* Execute command */
      if (L7_SUCCESS != ptin_msg_ntw_connectivity_get(ntwConn))
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while getting Network Connectivity config");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
        SetIPCNACK(outbuffer, res);
        break;
      }

      outbuffer->infoDim = sizeof(msg_NtwConnectivity_t);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_NTW_CONNECTIVITY_GET */
    }

    /* CCMSG_ETH_NTW_CONNECTIVITY_SET *****************************************/
    case CCMSG_ETH_NTW_CONNECTIVITY_SET:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_NTW_CONNECTIVITY_SET (0x%04X)", CCMSG_ETH_NTW_CONNECTIVITY_SET);

      CHECK_INFO_SIZE(msg_NtwConnectivity_t);

      msg_NtwConnectivity_t *ntwConn;
      ntwConn = (msg_NtwConnectivity_t *) inbuffer->info;

      /* Execute command */
      if (L7_SUCCESS != ptin_msg_ntw_connectivity_set(ntwConn))
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while setting Network Connectivity config");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_NTW_CONNECTIVITY_SET */
    }

    /************************************************************************** 
     * DHCP Relay Agent
     **************************************************************************/

    /* Reconfigure Global DHCP EVC ****************************/
    case CCMSG_ETH_DHCP_EVC_RECONF:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_DHCP_EVC_RECONF (0x%04X)", inbuffer->msgId);

      CHECK_INFO_SIZE(msg_DhcpEvcReconf_t);

      msg_DhcpEvcReconf_t *ptr;
      ptr = (msg_DhcpEvcReconf_t *) outbuffer->info;

      memcpy(outbuffer->info, inbuffer->info, sizeof(msg_DhcpEvcReconf_t));

      /* Execute command */
      rc = ptin_msg_DHCP_evc_reconf(ptr);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error sending data");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      outbuffer->infoDim = sizeof(msg_DhcpEvcReconf_t);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);
    }
    break;

    /* Configure DHCP circuit-id global components ****************************/
    case CCMSG_ETH_DHCP_EVC_CIRCUITID_SET:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_DHCP_EVC_CIRCUITID_SET (0x%04X)", inbuffer->msgId);

      CHECK_INFO_SIZE(msg_AccessNodeCircuitId_t);

      msg_AccessNodeCircuitId_t *ptr;
      ptr = (msg_AccessNodeCircuitId_t *) outbuffer->info;

      memcpy(outbuffer->info, inbuffer->info, sizeof(msg_AccessNodeCircuitId_t));

      /* Execute command */
      rc = ptin_msg_DHCP_circuitid_set(ptr);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error sending data");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      outbuffer->infoDim = sizeof(msg_AccessNodeCircuitId_t);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);
    }
    break;

    /* Get DHCP profile data **************************************************/
    case CCMSG_ETH_DHCP_EVC_CIRCUITID_GET:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_DHCP_PROFILE_GET (0x%04X)", inbuffer->msgId);

      CHECK_INFO_SIZE(msg_AccessNodeCircuitId_t);

      msg_AccessNodeCircuitId_t *ptr;
      ptr = (msg_AccessNodeCircuitId_t *) outbuffer->info;

      memcpy(outbuffer->info, inbuffer->info, sizeof(msg_AccessNodeCircuitId_t));

      /* Execute command */
      rc = ptin_msg_DHCP_circuitid_get(ptr);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error getting data");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      outbuffer->infoDim = sizeof(msg_AccessNodeCircuitId_t);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);
    }
    break;

    /* Get DHCP profile data **************************************************/
    case CCMSG_ETH_DHCP_PROFILE_GET:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_DHCP_PROFILE_GET (0x%04X)", inbuffer->msgId);

      CHECK_INFO_SIZE(msg_HwEthernetDhcpOpt82Profile_t);

      msg_HwEthernetDhcpOpt82Profile_t *ptr;
      ptr = (msg_HwEthernetDhcpOpt82Profile_t *) outbuffer->info;

      memcpy(outbuffer->info, inbuffer->info, sizeof(msg_HwEthernetDhcpOpt82Profile_t));

      /* Execute command */
      rc = ptin_msg_DHCP_profile_get(ptr);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error getting data");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      outbuffer->infoDim = sizeof(msg_HwEthernetDhcpOpt82Profile_t);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);
    }
    break;

    /* Add a new DHCP profile **************************************************/
    case CCMSG_ETH_DHCP_PROFILE_ADD:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_DHCP_PROFILE_ADD (0x%04X)", inbuffer->msgId);

      CHECK_INFO_SIZE_MOD(msg_HwEthernetDhcpOpt82Profile_t);

      msg_HwEthernetDhcpOpt82Profile_t *ptr;
      ptr = (msg_HwEthernetDhcpOpt82Profile_t *) inbuffer->info;
      L7_uint32 n_clients = inbuffer->infoDim / (sizeof(msg_HwEthernetDhcpOpt82Profile_t));

      /* Execute command */
      rc = ptin_msg_DHCP_profile_add(ptr, n_clients);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error sending data");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);
    }
    break;

    /* Remove a DHCP profile ****************************************************/
    case CCMSG_ETH_DHCP_PROFILE_REMOVE:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_DHCP_PROFILE_REMOVE (0x%04X)", inbuffer->msgId);

      CHECK_INFO_SIZE_MOD(msg_HwEthernetDhcpOpt82Profile_t);

      msg_HwEthernetDhcpOpt82Profile_t *ptr;
      ptr = (msg_HwEthernetDhcpOpt82Profile_t *) inbuffer->info;
      L7_uint32 n_clients = inbuffer->infoDim /(sizeof(msg_HwEthernetDhcpOpt82Profile_t));

      rc = ptin_msg_DHCP_profile_remove(ptr, n_clients);

      /* Execute command */
      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error sending data");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);
    }
    break;

    /* Get client DHCP statistics */
    case CCMSG_ETH_DHCP_CLIENT_STATS_GET:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_DHCP_CLIENT_STATS_GET (0x%04X)", inbuffer->msgId);

      CHECK_INFO_SIZE(msg_DhcpClientStatistics_t);

      msg_DhcpClientStatistics_t *ptr;
      ptr = (msg_DhcpClientStatistics_t *) outbuffer->info;

      memcpy(outbuffer->info, inbuffer->info, sizeof(msg_DhcpClientStatistics_t));

      /* Execute command */
      rc = ptin_msg_DHCP_clientStats_get(ptr);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error getting data");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      outbuffer->infoDim = sizeof(msg_DhcpClientStatistics_t);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);
    }
    break;

    /* Clear client DHCP statistics */
    case CCMSG_ETH_DHCP_CLIENT_STATS_CLEAR:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_DHCP_CLIENT_STATS_CLEAR (0x%04X)", inbuffer->msgId);

      CHECK_INFO_SIZE(msg_DhcpClientStatistics_t);

      msg_DhcpClientStatistics_t *ptr;
      ptr = (msg_DhcpClientStatistics_t *) inbuffer->info;

      /* Execute command */
      rc = ptin_msg_DHCP_clientStats_clear(ptr);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error sending data");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);
    }
    break;

    /* Get interface DHCP statistics */
    case CCMSG_ETH_DHCP_INTF_STATS_GET:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_DHCP_INTF_STATS_GET (0x%04X)", inbuffer->msgId);

      CHECK_INFO_SIZE(msg_DhcpClientStatistics_t);

      msg_DhcpClientStatistics_t *ptr;
      ptr = (msg_DhcpClientStatistics_t *) outbuffer->info;

      memcpy(outbuffer->info, inbuffer->info, sizeof(msg_DhcpClientStatistics_t));

      /* Execute command */
      rc = ptin_msg_DHCP_intfStats_get(ptr);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error getting statistics");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      outbuffer->infoDim = sizeof(msg_DhcpClientStatistics_t);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);
    }
    break;

    /* Clear interface DHCP statistics */
    case CCMSG_ETH_DHCP_INTF_STATS_CLEAR:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_DHCP_INTF_STATS_CLEAR (0x%04X)", inbuffer->msgId);

      CHECK_INFO_SIZE(msg_DhcpClientStatistics_t);

      msg_DhcpClientStatistics_t *ptr;
      ptr = (msg_DhcpClientStatistics_t *) inbuffer->info;

      /* Execute command */
      rc = ptin_msg_DHCP_intfStats_clear(ptr);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error sending data");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);
    }
    break;

    /* Get DHCP Bind Table */
    case CCMSG_ETH_DHCP_BIND_TABLE_GET:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_DHCP_BIND_TABLE_GET (0x%04X)", CCMSG_ETH_DHCP_BIND_TABLE_GET);

      CHECK_INFO_SIZE(msg_DHCP_bind_table_request_t);

      msg_DHCP_bind_table_request_t *pin;
      msg_DHCPv4v6_bind_table_t     *pout;
      pin  = (msg_DHCP_bind_table_request_t*) inbuffer->info;
      pout = (msg_DHCPv4v6_bind_table_t *) outbuffer->info;

      /* Execute command */
      rc = ptin_msg_DHCPv4v6_bindTable_get(pin, pout);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error getting data");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      outbuffer->infoDim = sizeof(msg_DHCPv4v6_bind_table_t);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);
    }
    break;

    /* Remove a DHCP Bind Table entry */
    case CCMSG_ETH_DHCP_BIND_TABLE_REMOVE:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_DHCP_BIND_TABLE_CLEAR (0x%04X)", inbuffer->msgId);

      CHECK_INFO_SIZE(msg_DHCPv4v6_bind_table_t);

      msg_DHCPv4v6_bind_table_t *ptr;
      ptr = (msg_DHCPv4v6_bind_table_t *) inbuffer->info;

      /* Execute command */
      rc = ptin_msg_DHCP_bindTable_remove(ptr);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error sending data");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);
    }
    break;

    /************************************************************************** 
     * IGMP Proxy Config
     **************************************************************************/

    /* CCMSG_ETH_IGMP_PROXY_SET ***********************************************/
    case CCMSG_ETH_IGMP_PROXY_SET:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_IGMP_PROXY_SET (0x%04X)", CCMSG_ETH_IGMP_PROXY_SET);

      CHECK_INFO_SIZE(msg_IgmpProxyCfg_t);

      msg_IgmpProxyCfg_t *igmpProxy;
      igmpProxy = (msg_IgmpProxyCfg_t *) outbuffer->info;

      memcpy(outbuffer->info, inbuffer->info, sizeof(msg_IgmpProxyCfg_t));

      /* Execute command */
      rc = ptin_msg_igmp_proxy_set(igmpProxy);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while getting IGMP Proxy config");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_IGMP_PROXY_SET */
    }


    /* CCMSG_ETH_IGMP_PROXY_GET ***********************************************/
    case CCMSG_ETH_IGMP_PROXY_GET:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_IGMP_PROXY_GET (0x%04X)", CCMSG_ETH_IGMP_PROXY_GET);

      CHECK_INFO_SIZE(msg_IgmpProxyCfg_t);

      msg_IgmpProxyCfg_t *igmpProxy;
      igmpProxy = (msg_IgmpProxyCfg_t *) inbuffer->info;

      /* Execute command */
      rc = ptin_msg_igmp_proxy_get(igmpProxy);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while getting IGMP proxy configuration");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_IGMP_PROXY_GET */
    }


    /* CCMSG_ETH_IGMP_ENTRY_ADD ***********************************************/
    case CCMSG_ETH_IGMP_ENTRY_ADD:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_IGMP_ENTRY_ADD (0x%04X)", CCMSG_ETH_IGMP_ENTRY_ADD);

      CHECK_INFO_SIZE(msg_IgmpMultcastUnicastLink_t);

      msg_IgmpMultcastUnicastLink_t *igmpEntry;
      igmpEntry = (msg_IgmpMultcastUnicastLink_t *) inbuffer->info;

      /* Execute command */
      rc = ptin_msg_igmp_instance_add(igmpEntry);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while adding an IGMP entry");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_IGMP_ENTRY_ADD */
    }

    /* CCMSG_ETH_IGMP_ENTRY_REMOVE ********************************************/
    case CCMSG_ETH_IGMP_ENTRY_REMOVE:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_IGMP_ENTRY_REMOVE (0x%04X)", CCMSG_ETH_IGMP_ENTRY_REMOVE);

      CHECK_INFO_SIZE(msg_IgmpMultcastUnicastLink_t);

      msg_IgmpMultcastUnicastLink_t *igmpEntry;
      igmpEntry = (msg_IgmpMultcastUnicastLink_t *) inbuffer->info;

      /* Execute command */
      rc = ptin_msg_igmp_instance_remove(igmpEntry);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while removing an IGMP entry");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_IGMP_ENTRY_REMOVE */
    }

    /* CCMSG_ETH_IGMP_CLIENT_ADD **********************************************/
    case CCMSG_ETH_IGMP_CLIENT_ADD:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_IGMP_CLIENT_ADD (0x%04X)", CCMSG_ETH_IGMP_CLIENT_ADD);

      CHECK_INFO_SIZE_MOD(msg_IgmpClient_t);

      msg_IgmpClient_t *igmpClient;
      igmpClient = (msg_IgmpClient_t *) inbuffer->info;
      L7_uint32 n_clients = inbuffer->infoDim / sizeof(msg_IgmpClient_t);

      /* Execute command */
      rc = ptin_msg_igmp_client_add(igmpClient, n_clients);

      if (L7_SUCCESS != rc)
      {
       LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while adding an IGMP client");
       res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
       SetIPCNACK(outbuffer, res);
       break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_IGMP_CLIENT_ADD */
    }

    /* CCMSG_ETH_IGMP_CLIENT_REMOVE *******************************************/
    case CCMSG_ETH_IGMP_CLIENT_REMOVE:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_IGMP_CLIENT_REMOVE (0x%04X)", CCMSG_ETH_IGMP_CLIENT_REMOVE);

      CHECK_INFO_SIZE_MOD(msg_IgmpClient_t);

      msg_IgmpClient_t *igmpClient;
      igmpClient = (msg_IgmpClient_t *) inbuffer->info;
      L7_uint32 n_clients = inbuffer->infoDim / sizeof(msg_IgmpClient_t);

      rc = ptin_msg_igmp_client_delete(igmpClient, n_clients);

      /* Execute command */
      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while removing IGMP client");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_IGMP_CLIENT_REMOVE */
    }

    /* CCMSG_ETH_IGMP_CLIENT_STATS_GET ****************************************/
    case CCMSG_ETH_IGMP_CLIENT_STATS_GET:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_IGMP_CLIENT_STATS_GET (0x%04X)", CCMSG_ETH_IGMP_CLIENT_STATS_GET);

      CHECK_INFO_SIZE(msg_IgmpClientStatistics_t);

      msg_IgmpClientStatistics_t *igmpClientStats;
      igmpClientStats = (msg_IgmpClientStatistics_t *) outbuffer->info;

      memcpy(outbuffer->info, inbuffer->info, sizeof(msg_IgmpClientStatistics_t));

      /* Execute command */
      rc = ptin_msg_IGMP_clientStats_get(igmpClientStats);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while getting IGMP client statistics");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      outbuffer->infoDim = sizeof(msg_IgmpClientStatistics_t);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_IGMP_CLIENT_STATS_GET */
    }


    /* CCMSG_ETH_IGMP_CLIENT_STATS_CLEAR **************************************/
    case CCMSG_ETH_IGMP_CLIENT_STATS_CLEAR:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_IGMP_CLIENT_STATS_CLEAR (0x%04X)", CCMSG_ETH_IGMP_CLIENT_STATS_CLEAR);

      CHECK_INFO_SIZE(msg_IgmpClientStatistics_t);

      msg_IgmpClientStatistics_t *igmpClient;
      igmpClient = (msg_IgmpClientStatistics_t *) inbuffer->info;
      L7_uint32 n_clients = inbuffer->infoDim / sizeof(msg_IgmpClientStatistics_t);

      /* Execute command */
      rc = ptin_msg_IGMP_clientStats_clear(igmpClient, n_clients);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while clearing IGMP client statistics");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_IGMP_CLIENT_STATS_CLEAR */
    }

    /* CCMSG_ETH_IGMP_INTF_STATS_GET ******************************************/
    case CCMSG_ETH_IGMP_INTF_STATS_GET:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_IGMP_INTF_STATS_GET (0x%04X)", CCMSG_ETH_IGMP_INTF_STATS_GET);

      CHECK_INFO_SIZE(msg_IgmpClientStatistics_t);

      msg_IgmpClientStatistics_t *igmpPortStats;
      igmpPortStats = (msg_IgmpClientStatistics_t *) outbuffer->info;

      memcpy(outbuffer->info, inbuffer->info, sizeof(msg_IgmpClientStatistics_t));

      /* Execute command */
      rc = ptin_msg_IGMP_intfStats_get(igmpPortStats);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while getting IGMP port statistics");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      outbuffer->infoDim = sizeof(msg_IgmpClientStatistics_t);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_IGMP_INTF_STATS_GET */
    }


    /* CCMSG_ETH_IGMP_INTF_STATS_CLEAR ****************************************/
    case CCMSG_ETH_IGMP_INTF_STATS_CLEAR:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_IGMP_INTF_STATS_CLEAR (0x%04X)", CCMSG_ETH_IGMP_INTF_STATS_CLEAR);

      CHECK_INFO_SIZE(msg_IgmpClientStatistics_t);

      msg_IgmpClientStatistics_t *igmpIntf;
      igmpIntf = (msg_IgmpClientStatistics_t *) inbuffer->info;
      L7_uint32 n_ports = inbuffer->infoDim / sizeof(msg_IgmpClientStatistics_t);

      /* Execute command */
      rc = ptin_msg_IGMP_intfStats_clear(igmpIntf, n_ports);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error while clearing IGMP client statistics");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;  /* CCMSG_ETH_IGMP_INTF_STATS_CLEAR */
    }

    case CCMSG_ETH_IGMP_CHANNEL_ASSOC_GET:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_IGMP_CHANNEL_ASSOC_GET (0x%04X)", inbuffer->msgId);

      CHECK_INFO_SIZE(msg_MCAssocChannel_t);

      msg_MCAssocChannel_t *ptr;
      L7_uint16             n=0;

      memcpy(outbuffer->info, inbuffer->info, sizeof(msg_MCAssocChannel_t));
      ptr = (msg_MCAssocChannel_t *) outbuffer->info;

      /* Execute command */
      rc = ptin_msg_IGMP_ChannelAssoc_get(ptr, &n);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error getting MC channels");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      outbuffer->infoDim = sizeof(msg_MCAssocChannel_t)*n;
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);
    }
    break;

    case CCMSG_ETH_IGMP_CHANNEL_ASSOC_ADD:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_IGMP_CHANNEL_ASSOC_ADD (0x%04X)", inbuffer->msgId);

      CHECK_INFO_SIZE_MOD(msg_MCAssocChannel_t);

      msg_MCAssocChannel_t *ptr;
      L7_uint16             n;

      ptr = (msg_MCAssocChannel_t *) inbuffer->info;
      n = inbuffer->infoDim/sizeof(msg_MCAssocChannel_t);

      /* Execute command */
      rc  = ptin_msg_IGMP_ChannelAssoc_add(ptr, n);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error adding MC channels");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);
    }
    break;

    case CCMSG_ETH_IGMP_CHANNEL_ASSOC_REMOVE:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_IGMP_CHANNEL_ASSOC_REMOVE (0x%04X)", inbuffer->msgId);

      CHECK_INFO_SIZE_MOD(msg_MCAssocChannel_t);

      msg_MCAssocChannel_t *ptr;
      L7_uint16             n;

      ptr = (msg_MCAssocChannel_t *) inbuffer->info;
      n = inbuffer->infoDim/sizeof(msg_MCAssocChannel_t);

      /* Execute command */
      rc  = ptin_msg_IGMP_ChannelAssoc_remove(ptr, n);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error adding MC channels");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);
    }
    break;

    /* Add static multicast channel */
    case CCMSG_ETH_IGMP_STATIC_GROUP_ADD:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_IGMP_STATIC_GROUP_ADD (0x%04X)", inbuffer->msgId);

      CHECK_INFO_SIZE(msg_MCStaticChannel_t);

      msg_MCStaticChannel_t *ptr;
      ptr = (msg_MCStaticChannel_t *) inbuffer->info;

      /* Execute command */
      rc  = ptin_msg_IGMP_staticChannel_add(ptr);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error sending data");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);
    }
    break;

    /* Remove static multicast channel */
    case CCMSG_ETH_IGMP_STATIC_GROUP_REMOVE:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_IGMP_STATIC_GROUP_REMOVE (0x%04X)", inbuffer->msgId);

      CHECK_INFO_SIZE(msg_MCStaticChannel_t);

      msg_MCStaticChannel_t *ptr;
      ptr = (msg_MCStaticChannel_t *) inbuffer->info;

      /* Execute command */
      rc = ptin_msg_IGMP_channel_remove(ptr);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error sending data");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);
    }
    break;

    /* Get list of multicast channels */
    case CCMSG_ETH_IGMP_GROUPS_GET:
    {
      msg_MCActiveChannelsRequest_t *inputPtr;
      msg_MCActiveChannelsReply_t   *outputPtr;
      L7_uint16                     numberOfChannels;

      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_IGMP_GROUPS_GET (0x%04X)", inbuffer->msgId);

      CHECK_INFO_SIZE(msg_MCActiveChannelsRequest_t);

      inputPtr         = (msg_MCActiveChannelsRequest_t *) inbuffer->info;
      outputPtr        = (msg_MCActiveChannelsReply_t *)   outbuffer->info;
      numberOfChannels =  IPCLIB_MAX_MSGSIZE/sizeof(msg_MCActiveChannelsReply_t); //IPC buffer size / struct size

      /* Execute command */
      rc = ptin_msg_IGMP_channelList_get(inputPtr, outputPtr, &numberOfChannels);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error getting data");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      outbuffer->infoDim = numberOfChannels * sizeof(msg_MCActiveChannelsReply_t);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);
    }
    break;

    /* Get list of clients watching a multicast channel */
    case CCMSG_ETH_IGMP_CLIENT_GROUPS_GET:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_IGMP_CLIENT_GROUPS_GET (0x%04X)", inbuffer->msgId);

      CHECK_INFO_SIZE(msg_MCActiveChannelClients_t);

      msg_MCActiveChannelClients_t *ptr;
      ptr = (msg_MCActiveChannelClients_t *) outbuffer->info;

      memcpy(outbuffer->info, inbuffer->info, sizeof(msg_MCActiveChannelClients_t));

      /* Execute command */
      rc = ptin_msg_IGMP_clientList_get(ptr);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error getting data");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      outbuffer->infoDim = sizeof(msg_MCActiveChannelClients_t);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);
    }
    break;

    /* Set PRBS mode */
    case CCMSG_ETH_PCS_PRBS_ENABLE:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_PCS_PRBS_ENABLE (0x%04X)", inbuffer->msgId);

      CHECK_INFO_SIZE_MOD(msg_ptin_pcs_prbs);

      msg_ptin_pcs_prbs *ptr;
      L7_int n = inbuffer->infoDim/sizeof(msg_ptin_pcs_prbs);

      ptr = (msg_ptin_pcs_prbs *) inbuffer->info;

      /* Execute command */
      rc  = ptin_msg_pcs_prbs_enable(ptr,n);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error sending data");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);
    }
    break;

    /* Get PRBS status */
    case CCMSG_ETH_PCS_PRBS_STATUS:
    {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ETH_PCS_PRBS_STATUS (0x%04X)", inbuffer->msgId);

      CHECK_INFO_SIZE_MOD(msg_ptin_pcs_prbs);

      msg_ptin_pcs_prbs *ptr;
      L7_int n = inbuffer->infoDim/sizeof(msg_ptin_pcs_prbs);

      ptr = (msg_ptin_pcs_prbs *) outbuffer->info;
      memcpy(outbuffer->info, inbuffer->info, sizeof(msg_ptin_pcs_prbs)*n);

      /* Execute command */
      rc = ptin_msg_pcs_prbs_status(ptr,n);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error getting data");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      outbuffer->infoDim = sizeof(msg_ptin_pcs_prbs)*n;
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);
    }
    break;


    /************************************************************************** 
     * OAM MEPs Configuration
     **************************************************************************/
#ifdef __Y1731_802_1ag_OAM_ETH__
    case CCMSG_WR_MEP:
    case CCMSG_FLUSH_MEP:
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_WR_MEP/CCMSG_FLUSH_MEP (0x%04X)", inbuffer->msgId);
    
      CHECK_INFO_SIZE_MOD(msg_bd_mep_t);

      rc = ptin_msg_wr_MEP(inbuffer, outbuffer, 0);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error sending data");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);
    
      break;
    case CCMSG_RM_MEP:
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_RM_MEP (0x%04X)", inbuffer->msgId);

      CHECK_INFO_SIZE_MOD(msg_bd_mep_t);

      rc = ptin_msg_del_MEP(inbuffer, outbuffer, 0);

      if (L7_SUCCESS != rc) {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error sending data");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;
    case CCMSG_WR_RMEP:
    case CCMSG_FLUSH_RMEP:
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_WR_RMEP/CCMSG_FLUSH_RMEP (0x%04X)", inbuffer->msgId);

      CHECK_INFO_SIZE_MOD(msg_bd_rmep_t);

      rc = ptin_msg_wr_RMEP(inbuffer, outbuffer, 0);

      if (L7_SUCCESS != rc) {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error sending data");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;
    case CCMSG_RM_RMEP:
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_RM_RMEP (0x%04X)", inbuffer->msgId);

      CHECK_INFO_SIZE_MOD(msg_bd_rmep_t);

      rc = ptin_msg_del_RMEP(inbuffer, outbuffer, 0);

      if (L7_SUCCESS != rc) {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error sending data");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;
    case CCMSG_DUMP_MEPs:
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
             "Message received: CCMSG_DUMP_MEPs (0x%04X)", inbuffer->msgId);

      CHECK_INFO_SIZE_MOD(msg_generic_prefix_t);

      rc = ptin_msg_dump_MEPs(inbuffer, outbuffer);
      if (L7_SUCCESS != rc) {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error sending data");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;

    case CCMSG_DUMP_MEs:
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_DUMP_MEs (0x%04X)", inbuffer->msgId);

      CHECK_INFO_SIZE_MOD(msg_bd_me_t);

      rc = ptin_msg_dump_MEs(inbuffer, outbuffer);

      if (L7_SUCCESS != rc) {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error sending data");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;

    case CCMSG_DUMP_LUT_MEPs:
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_DUMP_LUT_MEPs (0x%04X)", inbuffer->msgId);

      CHECK_INFO_SIZE_MOD(msg_generic_prefix_t);

      rc = ptin_msg_dump_LUT_MEPs(inbuffer, outbuffer);

      if (L7_SUCCESS != rc) {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error sending data");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);

      break;
#endif //__Y1731_802_1ag_OAM_ETH__

    /************************************************************************** 
    * ERPS Configuration
    **************************************************************************/

    case CCMSG_ERPS_SET:
      {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ERPS_SET (0x%04X)", inbuffer->msgId);
    
      CHECK_INFO_SIZE_MOD(msg_erps_t);

      msg_erps_t *ptr;
      ptr = (msg_erps_t *) outbuffer->info;

      memcpy(outbuffer->info, inbuffer->info, sizeof(msg_erps_t));

      /* Execute command */
      rc = ptin_msg_erps_set(ptr);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error sending data");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);
      }
      break;

    case CCMSG_ERPS_DEL:
      {
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message received: CCMSG_ERPS_DEL (0x%04X)", inbuffer->msgId);
    
      CHECK_INFO_SIZE_MOD(msg_erps_t);

      msg_erps_t *ptr;
      ptr = (msg_erps_t *) outbuffer->info;

      memcpy(outbuffer->info, inbuffer->info, sizeof(msg_erps_t));

      /* Execute command */
      rc = ptin_msg_erps_del(ptr);

      if (L7_SUCCESS != rc)
      {
        LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error sending data");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
        SetIPCNACK(outbuffer, res);
        break;
      }

      SETIPCACKOK(outbuffer);
      LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
               "Message processed: response with %d bytes", outbuffer->infoDim);
      }
      break;

    case CCMSG_ERPS_CONF:
      {
        LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
                 "Message received: CCMSG_ERPS_CONF (0x%04X)", inbuffer->msgId);
      
        CHECK_INFO_SIZE_MOD(msg_erps_t);

        msg_erps_t *ptr;
        ptr = (msg_erps_t *) outbuffer->info;

        memcpy(outbuffer->info, inbuffer->info, sizeof(msg_erps_t));

        /* Execute command */
        rc = ptin_msg_erps_config(ptr);

        if (L7_SUCCESS != rc)
        {
          LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error sending data");
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
          SetIPCNACK(outbuffer, res);
          break;
        }

        SETIPCACKOK(outbuffer);
        LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
                 "Message processed: response with %d bytes", outbuffer->infoDim);
      
      }
      break;

    case CCMSG_ERPS_STATUS:
      {
        LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
                 "Message received: CCMSG_ERPS_STATUS (0x%04X)", inbuffer->msgId);
      
        CHECK_INFO_SIZE_MOD(msg_erps_status_t);

        msg_erps_status_t *ptr;
        ptr = (msg_erps_status_t *) outbuffer->info;

        memcpy(outbuffer->info, inbuffer->info, sizeof(msg_erps_status_t));

        /* Execute command */
        rc = ptin_msg_erps_status(ptr);

        if (L7_SUCCESS != rc)
        {
          LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error sending data");
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
          SetIPCNACK(outbuffer, res);
          break;
        }

        SETIPCACKOK(outbuffer);
        outbuffer->infoDim = sizeof(msg_erps_status_t);
        LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
                 "Message processed: response with %d bytes", outbuffer->infoDim);
      
      }
      break;

    case CCMSG_ERPS_STATUS_NEXT:
      {
        LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
                 "Message received: CCMSG_ERPS_STATUS_NEXT (0x%04X)", inbuffer->msgId);
      
        CHECK_INFO_SIZE_MOD(msg_erps_status_t);

        msg_erps_status_t *ptr;
        L7_int            n;

        ptr = (msg_erps_status_t *) outbuffer->info;

        memcpy(outbuffer->info, inbuffer->info, sizeof(msg_erps_status_t));

        /* Execute command */
        rc = ptin_msg_erps_status_next(ptr, &n);

        if (L7_SUCCESS != rc)
        {
          LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error sending data");
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
          SetIPCNACK(outbuffer, res);
          break;
        }

        SETIPCACKOK(outbuffer);
        outbuffer->infoDim = sizeof(msg_erps_status_t)*n;
        LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
                 "Message processed: response with %d bytes", outbuffer->infoDim);
      
      }
      break;

      case CCMSG_ERPS_OPERATOR_CMD:
      {
        LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
                 "Message received: CCMSG_ERPS_OPERATOR_CMD (0x%04X)", inbuffer->msgId);
      
        CHECK_INFO_SIZE_MOD(msg_erps_cmd_t);

        msg_erps_cmd_t *ptr;

        ptr = (msg_erps_cmd_t *) outbuffer->info;

        memcpy(outbuffer->info, inbuffer->info, sizeof(msg_erps_cmd_t));

        /* Execute command */
        rc = ptin_msg_erps_cmd(ptr);

        if (L7_SUCCESS != rc)
        {
          LOG_ERR(LOG_CTX_PTIN_MSGHANDLER, "Error sending data");
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, SIRerror_get(rc));
          SetIPCNACK(outbuffer, res);
          break;
        }

        SETIPCACKOK(outbuffer);
        LOG_INFO(LOG_CTX_PTIN_MSGHANDLER,
                 "Message processed: response with %d bytes", outbuffer->infoDim);
      
      }
      break;


#ifdef __802_1x__
    case CCMSG_WR_802_1X_ADMINMODE:
    case CCMSG_WR_802_1X_TRACE:
    case CCMSG_WR_802_1X_VLANASSGNMODE:
    case CCMSG_WR_802_1X_MONMODE:
    case CCMSG_WR_802_1X_DYNVLANMODE:
      msg_generic_wrd(msg_wr_802_1x_Genrc, inbuffer, outbuffer, sizeof(msg_802_1x_Genrc), sizeof(msg_802_1x_Genrc));
      break;

    case CCMSG_WR_802_1X_ADMINCONTROLLEDDIRECTIONS:
    case CCMSG_WR_802_1X_PORTCONTROLMODE:
    case CCMSG_WR_802_1X_QUIETPERIOD:
    case CCMSG_WR_802_1X_TXPERIOD:
    case CCMSG_WR_802_1X_SUPPTIMEOUT:
    case CCMSG_WR_802_1X_SERVERTIMEOUT:
    case CCMSG_WR_802_1X_MAXREQ:
    case CCMSG_WR_802_1X_REAUTHPERIOD:
    case CCMSG_WR_802_1X_KEYTXENABLED:
    case CCMSG_WR_802_1X_GUESTVLANID:
    case CCMSG_WR_802_1X_GUSTVLANPERIOD:
    case CCMSG_WR_802_1X_MAXUSERS:
    case CCMSG_WR_802_1X_UNAUTHENTICATEDVLAN:
      msg_generic_wrd(msg_wr_802_1x_Genrc2, inbuffer, outbuffer, sizeof(msg_802_1x_Genrc2), sizeof(msg_generic_prefix_t));
      break;

    case CCMSG_WR_802_1X_AUTHSERV:
      msg_generic_wrd(msg_wr_802_1x_AuthServ, inbuffer, outbuffer, sizeof(msg_802_1x_AuthServ), sizeof(msg_generic_prefix_t));
      break;
#endif //__802_1x__

//CCMSG_ETH_IGMP_STATIC_GROUP_ADD
//CCMSG_ETH_IGMP_STATIC_GROUP_REMOVE
//CCMSG_ETH_IGMP_GROUPS_GET
//CCMSG_ETH_IGMP_CLIENT_GROUPS_GET

//CCMSG_ETH_PORT_COS_GET
//CCMSG_ETH_PORT_COS_SET
//
//CCMSG_ETH_MAC_AGING_GET
//CCMSG_ETH_MAC_AGING_SET
//CCMSG_ETH_MAC_TABLE_SHOW
//CCMSG_ETH_MAC_ENTRY_REMOVE
//
//CCMSG_ETH_DHCP_PROFILE_GET
//CCMSG_ETH_DHCP_PROFILE_ADD
//CCMSG_ETH_DHCP_PROFILE_REMOVE
//CCMSG_ETH_DHCP_CLIENT_STATS_GET
//CCMSG_ETH_DHCP_CLIENT_STATS_CLEAR
//CCMSG_ETH_DHCP_INTF_STATS_GET
//CCMSG_ETH_DHCP_INTF_STATS_CLEAR
//CCMSG_ETH_DHCP_BIND_TABLE_GET
//CCMSG_ETH_DHCP_BIND_TABLE_CLEAR


    default:
    {
      LOG_WARNING(LOG_CTX_PTIN_MSGHANDLER,
                  "Message received: UNKNOWN! (0x%04X)\n", inbuffer->msgId);
      LOG_WARNING(LOG_CTX_PTIN_MSGHANDLER,
                  "The received message is not supported!");
      SetIPCNACK (outbuffer, SIR_ERROR(ERROR_FAMILY_IPC, ERROR_SEVERITY_WARNING, ERROR_CODE_NOSUCHMSG));

      rc = L7_FAILURE;
      break;
    }
  }

  /* Save slot id parameter */
  if (inbuffer->infoDim>=1 &&
      inbuffer->info[0]!=(L7_uint8)-1 &&
      ptin_board_slotId!=inbuffer->info[0])
  {
    ptin_board_slotId = inbuffer->info[0];
  }

  /* Save final time */
  time_end = osapiTimeMicrosecondsGet();
  time_delta = time_end - time_start;

  LOG_TRACE(LOG_CTX_PTIN_MSGHANDLER,"Message 0x%04X was processed in %lu usec, with rc=%u", inbuffer->msgId, time_delta, rc);

  /* Message Runtime Meter */
  /* Only for successfull messages */
  if (rc==L7_SUCCESS)
  {
    CHMessage_runtime_meter_update(inbuffer->msgId, time_delta);
  }

  return S_OK;
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
}

