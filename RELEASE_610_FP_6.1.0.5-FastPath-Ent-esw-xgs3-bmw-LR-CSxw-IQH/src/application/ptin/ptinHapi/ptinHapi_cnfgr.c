/*
 * ptinHapi_cnfgr.c
 *
 *  Created on: 2010/04/08
 *      Author: Andre Brizido
 *
 *      Routines for Initialization adn Reinitialization
 */

/* global variables */
#include "ptinHapi_include.h"
#include "IPC.h"
#include "CHMessageHandler.h"
#include "dtlinclude.h"
#include "ptin_snoop_stat_api.h"
#include "snooping_api.h"
#include "usmdb_common.h"
#include "usmdb_mib_bridge_api.h"
#include "usmdb_dot3ad_api.h"
#include "usmdb_policy_api.h"
#include "ptin_flow.h"
#include "ptin_interface.h"
#include "nimapi.h"
#include "dtl_cnfgr.h"
#include "cli_txt_cfg_api.h"
#include "fw_shm.h"
#include <unistd.h>
#include <sys/time.h>

// Semaphore to indicate where module must start
void *ptin_module_l2_sem=L7_NULLPTR;
static void *ptin_module_ready_sem=L7_NULLPTR;

// PTin Task id
static L7_int32 ptin_mainTaskId = L7_ERROR;
static L7_int32 ptin_L2TaskId = L7_ERROR;
static L7_int32 ptin_L2ManagTaskId = L7_ERROR;

static L7_uint32 l2_time_ref = 0;


void *ptinL2Queue = L7_NULLPTR;

ptinHapiCnfgrState_t ptinHapiCnfgrState_g = PTINHAPI_PHASE_INIT_0;

ptinHapiDeregister_t ptinHapiDeregister = {L7_FALSE, L7_FALSE, L7_FALSE, L7_FALSE, L7_FALSE, L7_FALSE, L7_FALSE, L7_FALSE, L7_FALSE};
ptinHapiCfg_t                  *ptinHapiCfg         = L7_NULLPTR;
//extern void *ptinHapi_queue;

//extern void *g_AppIpcSemaphore;

L7_int    linkStatus[L7_SYSTEM_N_INTERF];
L7_int    lagActiveMembers[L7_SYSTEM_N_PORTS];

L7_RC_t ptin_ApplyConfigCompleteCb(L7_uint32 event);

void ptinHapiTask(uint32 numArgs, void *unit)
{
  L7_HWEth_Statistics_State counters_state;
  int port, lagPort, status/*, last_status*/;
  L7_uint32 adminState, linkState, link=L7_FALSE, intf, lagIntf;
  L7_int    isMember, isActiveMember;
  L7_uint32 lagIdList[L7_SYSTEM_N_PORTS];
  long tmp;

  // This task has started!
  osapiTaskInitDone(L7_PTINHAPI_TASK_SYNC);

  printf("starting my task\n");

  for (port=0; port<L7_SYSTEM_N_INTERF; port++)
  {
    linkStatus[port] = L7_TRUE;
  }
  for (port=0; port<L7_SYSTEM_N_PORTS; port++)
  {
    lagActiveMembers[port] = L7_TRUE;
  }
  memset(lagIdList,0x00,sizeof(L7_uint32)*L7_SYSTEM_N_PORTS);

  if (fw_shm_open())
  {
    LOG_MSG("PTIN: Unable to open shared memory\n");
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) ERROR\n", __FUNCTION__, __LINE__);
  } else
  {
    memset(pfw_shm,0x00,sizeof(t_fw_shm));
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) success\n", __FUNCTION__, __LINE__);
  }

  // Wait to receive a callback indicating that all configuration were executed
  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s: Waiting to start...",__FUNCTION__);
  osapiSemaTake(ptin_module_ready_sem,L7_WAIT_FOREVER);
  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s: PTin module is starting!",__FUNCTION__);

  // NIM is ready?
  while (!nimPhaseStatusCheck())
  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: NIM not ready!",__FUNCTION__);
    sleep(1);
  }  
  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s: NIM Ready!",__FUNCTION__);

  status = L7_FAILURE;
  do
  {
    //last_status = status;
    sleep(5);
    if ((status=dtlPtinInit())==L7_SUCCESS)
      printf("dtlPtinInit: success\n");
    else
      printf("dtlPtinInit: failure\n");
  } while (/*last_status!=L7_SUCCESS ||*/ status!=L7_SUCCESS);


  while (!snoopIsReady())
  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Snoop is NOT ready!",__FUNCTION__);
    sleep(1);
  }
  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s: Snoop is ready!",__FUNCTION__);

  // Wait 10 seconds for system stabilization
  sleep(10);

  while (OpenIPC()!=0)
  {
    printf("Error opening communications channel!\r\n");
    sleep(5);
  }
  printf("Communications channel opened!\r\n");

  // System ready
  global_var_system_ready=TRUE;

  // Send a trap with 30 seconds periodicity, until boardinfo is read!
  tmp = osapiTimeMillisecondsGet()-30000;
  while (!global_var_trap_sent)
  {
    if ((osapiTimeMillisecondsGet()-tmp)>=30000)
    {
      tmp = osapiTimeMillisecondsGet();
      printf("A enviar trap\n");
      if (send_trap(IPC_CHMSG_TRAP_PORT,TRAP_ARRANQUE,-1)<0)
      {
        printf("Falha a enviar trap\n");
      } else
      {
        printf("Trap enviado\n");
      }
    }
    sleep(1);
  }

  // 10 seconds should be enough to receive all configuration messages
  sleep(60);

  for (port=L7_SYSTEM_N_PORTS; port<L7_SYSTEM_N_INTERF; port++)
  {
    if (ptin_translate_port2intf(port,&intf)!=L7_SUCCESS || intf==0)
      continue;

    usmDbDot3adAdminModeSet(1,intf,L7_DISABLE);
    usleep(100000);
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Reenabling lag port %u", __FUNCTION__, __LINE__,intf);
    usmDbDot3adAdminModeSet(1,intf,L7_ENABLE);
    usleep(100000);
  }

  /* Time, for lags to link up */
  sleep(5);

  // Check if FP rules with redirectTrunk actions, need to be activated
  for (port=L7_SYSTEM_N_PORTS; port<L7_SYSTEM_N_INTERF; port++)
  {
    //printf("Stored link state for port=%u: link=%u\n",port,linkStatus[port]);
    // Only applying to existent lags, with its adminState active, and in link up
    if (ptin_translate_port2intf(port,&intf)!=L7_SUCCESS ||
        nimGetIntfAdminState(intf,&adminState)!=L7_SUCCESS ||
        nimGetIntfLinkState(intf,&linkState)!=L7_SUCCESS)
      continue;

    if (intf==0 || adminState!=L7_ENABLE || linkState!=L7_UP)
      continue;

    /* We have link */
    dtlPtinBitStreamLagRecheck(intf,L7_TRUE);
  }

  printf("Starting checking alarms!\n");

  while (1)
  {
    sleep(3);

    counters_state.status_mask = 0xFFFFFFFF;
    if (dtlPtinGetCountersState(&counters_state)!=L7_SUCCESS)
    {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Falha a ler o estado dos contadores\n", __FUNCTION__, __LINE__);
    } else
    {
      //DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Sucesso a ler o estado dos contadores\n", __FUNCTION__, __LINE__);
      for (port=0; port<L7_SYSTEM_N_INTERF; port++)
      {
        intf = 0;
        if (ptin_translate_port2intf(port,&intf)!=L7_SUCCESS || intf==0 ||
            nimGetIntfAdminState(intf,&adminState)!=L7_SUCCESS)
        {
          intf = 0;
          adminState = L7_DISABLE;
        }

        // Get Link status
        if (intf==0 || adminState==L7_DISABLE)
        {
          link = L7_TRUE;       // End of link down
        }
        else if (nimGetIntfLinkState(intf, &linkState)==L7_SUCCESS)
        {
          link = (linkState==L7_UP);
        }
        else
        {
          link = L7_FALSE;
        }

        if (port<L7_SYSTEM_N_PORTS)  pfw_shm->intf[port].link = link;
        if (linkStatus[port]!=link)
        {
          printf("Link state changed to port=%u: link=%u\n",port,link);
          if (port>=L7_SYSTEM_PON_PORTS &&
              send_trap_intf_alarm(((port<L7_SYSTEM_N_PORTS) ? port : (128+port-L7_SYSTEM_N_PORTS+1)),
                                   ((!link) ? TRAP_ALARM_LINK_DOWN_START : TRAP_ALARM_LINK_DOWN_END),
                                   TRAP_ALARM_STATUS_EVENT,0)==0)
            printf("Alarm sent: port=%u, link=%u\n",port,link);
          // For lag interfaces, verify if some FP rule, using redirect_trunk action, must be updated
          if (port>=L7_SYSTEM_N_PORTS)
          {
            dtlPtinBitStreamLagRecheck(intf,link);
          }
          linkStatus[port]=link;
        }
        if (port<L7_SYSTEM_N_PORTS)  pfw_shm->intf[port].counter_state = counters_state.status_value[port];

        // Only send lag active member traps, if interface is physical
        if (port<L7_SYSTEM_N_PORTS)
        {
          // Determine if is an active lag member
          if (intf!=0 &&
              usmDbDot3adIntfIsMemberGet(1,intf,&lagIntf)==L7_SUCCESS && lagIntf!=0 &&
              ptin_translate_intf2port(lagIntf,&lagPort)==L7_SUCCESS && lagPort>=L7_SYSTEM_N_PORTS && lagPort<L7_SYSTEM_N_INTERF)
          {
            /* This interface is a lag member */
            isMember = L7_TRUE;

            lagIdList[port] = lagPort-L7_SYSTEM_N_PORTS+1;

            /* Is this interface an active lag member? */
            if (adminState==L7_DISABLE)
              isActiveMember = L7_TRUE;
            else if (usmDbDot3adIsActiveMember(1,intf)==L7_SUCCESS)
              isActiveMember = L7_TRUE;
            else
              isActiveMember = L7_FALSE;
          } else
          {
            isMember = L7_FALSE;
            isActiveMember = L7_TRUE;   /* in order to close alarms, and to not open a new one */
          }
          // Check if there is a change in the active member state
          if ( lagActiveMembers[port]!=isActiveMember )
          {
            //if (pfw_shm->intf[port].admin && isMember)
            if (send_trap_intf_alarm( port,
                                      ((isActiveMember) ? TRAP_ALARM_LAG_INACTIVE_MEMBER_END : TRAP_ALARM_LAG_INACTIVE_MEMBER_START),
                                      TRAP_ALARM_STATUS_EVENT,
                                      lagIdList[port] ) == 0 )
            printf("Alarm sent: port=%u, activeMember=%u\n",port,isActiveMember);

            lagActiveMembers[port]=isActiveMember;
          }
        }
      }
    }
  }
}

/**
 * Task responsible to process Learning requests
 * 
 * @param numArgs 
 * @param unit 
 */
void ptinL2Task(uint32 numArgs, void *unit)
{
  L7_RC_t status = L7_FAILURE;
  ptinL2Msg_t message;
  L7_uint16 counter=0;
  struct timeval tm;

  if (ptinL2Queue == L7_NULLPTR)
  {
    LOG_MSG("ptin L2 Task: Failed to create, exiting....\n");
  }
  else
  {
    while (1)
    {
      status = osapiMessageReceive(ptinL2Queue, (void *)&message, sizeof(ptinL2Msg_t), L7_WAIT_FOREVER);

      if (status == L7_FAILURE)
      {
        printf("%s(%d) Error receiving new message\r\n",__FUNCTION__,__LINE__);
        continue;
      }

      osapiSemaTake(ptin_module_l2_sem,L7_WAIT_FOREVER);

      /* Save current time (for ptinL2ManagementTask task) */
      gettimeofday(&tm, (struct timezone *)NULL);
      l2_time_ref = tm.tv_sec*1000 + tm.tv_usec/1000;

      /* Process message */
      if (ptinL2Process(message.msgsType, message.intIfNum, message.vlanId, message.mac_addr, message.entryType)!=L7_SUCCESS)
      {
        printf("%s(%d) Processing failed... reinserting in queue\r\n",__FUNCTION__,__LINE__);
        /* If some error have ocurred, insert again message in the queue */
        if (osapiMessageSend(ptinL2Queue, &message, sizeof(ptinL2Msg_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM ) != L7_SUCCESS)
        {
          printf("L2 address not processed: intIfNum %u, vid %u: mac=%02X:%02X:%02X:%02X:%02X:%02X\n",message.intIfNum,message.vlanId,
                 message.mac_addr[0],message.mac_addr[1],message.mac_addr[2],message.mac_addr[3],message.mac_addr[4],message.mac_addr[5]);
        }
      }

      osapiSemaGive(ptin_module_l2_sem);

      /* This is a pause, to prevent queues to full up */
      if ((++counter)%256==0)  sleep(1);
    }
  }

  return;
}


/**
 * Task responsible to process Learning requests
 * 
 * @param numArgs 
 * @param unit 
 */
void ptinL2ManagementTask(uint32 numArgs, void *unit)
{
//usmdbFdbEntry_t fdbEntry, fdbEntry2;
//fdbMeberInfo_t fdbMemberInfo;
//L7_RC_t rc;
//struct timeval tm;
//L7_uint32 time_old, time_now;
//
///* Clear entry */
//memset(&fdbEntry,0x00,sizeof(usmdbFdbEntry_t));
//
//gettimeofday(&tm, (struct timezone *)NULL);
//l2_time_ref = tm.tv_sec*1000 + tm.tv_usec/1000;

  while (1)
  {
    sleep(10);

//  gettimeofday(&tm, (struct timezone *)NULL);
//  time_now = tm.tv_sec*1000 + tm.tv_usec/1000;
//
//  /* Prevent time inversion */
//  if (time_now<l2_time_ref)
//  {
//    l2_time_ref = 0;
//  }
//
//  /* If no new MACs are being processed for more than 5 seconds, check present MACs */
//  if (time_now-l2_time_ref<5000)
//  {
//    continue;
//  }
//
//  osapiSemaTake(ptin_module_l2_sem,L7_WAIT_FOREVER);
//
//  gettimeofday(&tm, (struct timezone *)NULL);
//  time_old = tm.tv_sec*1000 + tm.tv_usec/1000;
//
//  /* If MAC search, reched the end, restart */
//  memset(&fdbEntry,0x00,sizeof(usmdbFdbEntry_t));
//  while (usmDbFDBEntryNextGet(0,&fdbEntry)!=L7_SUCCESS)
//  {
//    /* If MAC found is static... */
//    if (fdbEntry.usmdbFdbEntryType==L7_FDB_ADDR_FLAG_STATIC)
//    {
//      /* ... search for a dynamic entry with the same MAC */
//      rc = L7_SUCCESS;
//      memset(&fdbEntry2,0x00,sizeof(usmdbFdbEntry_t));
//      while ((rc=usmDbFDBEntryNextGet(0,&fdbEntry2))==L7_SUCCESS)
//      {
//        /* if found, abandon search */
//        if (fdbEntry2.usmdbFdbEntryType==L7_FDB_ADDR_FLAG_LEARNED &&
//            memcmp(&(fdbEntry2.usmdbFdbMac[2]),&(fdbEntry.usmdbFdbMac[2]),sizeof(L7_uchar8)*L7_MAC_ADDR_LEN)==0)
//          break;
//      }
//      /* Dynamic entry not found... going to delete static entry */
//      if (rc==L7_FAILURE)
//      {
//        fdbMemberInfo.entryType = fdbEntry.usmdbFdbEntryType;
//        fdbMemberInfo.intIfNum  = fdbEntry.usmdbFdbIntIfNum;
//        fdbMemberInfo.vlanId    = ((L7_uint32) fdbEntry.usmdbFdbMac[0]<<8) | ((L7_uint32) fdbEntry.usmdbFdbMac[1]);
//        memcpy(fdbMemberInfo.macAddr,&(fdbEntry.usmdbFdbMac[2]),sizeof(L7_uint8)*L7_MAC_ADDR_LEN);
//        fdbDelEntry(&fdbMemberInfo);
//      }
//      //usleep(1000);
//    }
//  }
//
//  gettimeofday(&tm, (struct timezone *)NULL);
//  time_now = tm.tv_sec*1000 + tm.tv_usec/1000;
//
//  printf("%s(%d) Took %u milliseconds to check all MACs\r\n",__FUNCTION__,__LINE__,time_now-time_old);
//
//  osapiSemaGive(ptin_module_l2_sem);
  }

  return;
}

/*********************************************************************
*
* @purpose  CNFGR System Initialization for PTIN HAPI component
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this
*                                             CNFGR request
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the PTIN HAPIs comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void ptinHapiApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

  L7_RC_t             rc     = L7_ERROR;
  L7_CNFGR_ERR_RC_t   reason = L7_CNFGR_ERR_RC_INVALID_PAIR;

  LOG_MSG("%s %u: %s: Initializing Ptin api\n",
          __FILE__, __LINE__, __FUNCTION__);

  response = L7_CNFGR_CMD_COMPLETE;

  /* validate command type */
  if (pCmdData != L7_NULL)
  {
    if (pCmdData->type == L7_CNFGR_RQST)
    {
      command    = pCmdData->command;
      request    = pCmdData->u.rqstData.request;
      correlator = pCmdData->correlator;
      if (request > L7_CNFGR_RQST_FIRST &&
          request < L7_CNFGR_RQST_LAST)
      {

        /* validate command/event pair */
        switch (command)
        {
        case L7_CNFGR_CMD_INITIALIZE:
          switch (request)
          {
          case L7_CNFGR_RQST_I_PHASE1_START:
            //("%s(%d) Phase 1\r\n",__FUNCTION__,__LINE__);
            if ((rc = ptinHapiCnfgrInitPhase1Process(&response, &reason)) != L7_SUCCESS)
            {
              ptinHapiCnfgrFiniPhase1Process();
            }
            break;
          case L7_CNFGR_RQST_I_PHASE2_START:
            //printf("%s(%d) Phase 2\r\n",__FUNCTION__,__LINE__);
            if ((rc = ptinHapiCnfgrInitPhase2Process(&response, &reason)) != L7_SUCCESS)
            {
              ptinHapiCnfgrFiniPhase2Process();
            }
            break;
          case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
            //printf("%s(%d) Phase MGMT\r\n",__FUNCTION__,__LINE__);
            rc = ptinHapiCnfgrNoopProccess(&response, &reason);
            ptinHapiCnfgrState_g = PTINHAPI_PHASE_WMU;
            break;
          case L7_CNFGR_RQST_I_PHASE3_START:
            //printf("%s(%d) Phase 3\r\n",__FUNCTION__,__LINE__);
            if ((rc = ptinHapiCnfgrInitPhase3Process(&response, &reason)) != L7_SUCCESS)
            {
              ptinHapiCnfgrFiniPhase3Process();
            }
            break;
          default:
            /* invalid command/request pair */
            break;
          } /* endswitch initialize requests */
          break;

        case L7_CNFGR_CMD_EXECUTE:
          //printf("%s(%d) Phase EXE\r\n",__FUNCTION__,__LINE__);
          switch (request)
          {
          case L7_CNFGR_RQST_E_START:
            //printf("%s(%d) Phase START\r\n",__FUNCTION__,__LINE__);
            ptinHapiCnfgrState_g = PTINHAPI_PHASE_EXECUTE;

            rc        = L7_SUCCESS;
            response  = L7_CNFGR_CMD_COMPLETE;
            reason    = 0;
            break;

          default:
            /* invalid command/request pair */
            break;
          }
          break;

        case L7_CNFGR_CMD_UNCONFIGURE:
          //printf("%s(%d) Phase UN\r\n",__FUNCTION__,__LINE__);
          switch (request)
          {
          case L7_CNFGR_RQST_U_PHASE1_START:
            //printf("%s(%d) Phase UN1\r\n",__FUNCTION__,__LINE__);
            rc = ptinHapiCnfgrNoopProccess(&response, &reason);
            ptinHapiCnfgrState_g = PTINHAPI_PHASE_UNCONFIG_1;
            break;

          case L7_CNFGR_RQST_U_PHASE2_START:
            //printf("%s(%d) Phase UN2\r\n",__FUNCTION__,__LINE__);
            rc = ptinHapiCnfgrUconfigPhase2(&response, &reason);
            ptinHapiCnfgrState_g = PTINHAPI_PHASE_UNCONFIG_2;
            /* we need to do something with the stats in the future */
            break;

          default:
            //printf("%s(%d) ?????\r\n",__FUNCTION__,__LINE__);
            /* invalid command/request pair */
            break;
          }
          break;

        case L7_CNFGR_CMD_TERMINATE:
        case L7_CNFGR_CMD_SUSPEND:
          //printf("%s(%d)\r\n",__FUNCTION__,__LINE__);
          rc = ptinHapiCnfgrNoopProccess(&response, &reason);
          break;

        default:
          //printf("%s(%d) xxxxxxx\r\n",__FUNCTION__,__LINE__);
          reason = L7_CNFGR_ERR_RC_INVALID_CMD;
          break;
        } /* endswitch command/event pair */

      } /* endif validate request */

      else
      {
        //printf("%s(%d) XXXXXX\r\n",__FUNCTION__,__LINE__);
        reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
      }

    } /* endif cnfgr request */

    else
    {
      //printf("%s(%d) CCCCCCCCCC\r\n",__FUNCTION__,__LINE__);
      reason = L7_CNFGR_ERR_RC_INVALID_CMD_TYPE;
    }

  } /* endif validate command type */

  else
  {
    //printf("%s(%d) AAAAAAAAAAAA\r\n",__FUNCTION__,__LINE__);
    correlator = L7_NULL;
    reason = L7_CNFGR_ERR_RC_INVALID_CMD;
  }

  /* return value to caller -
   * <prepare complesion response>
   * <callback the configurator>
   */
  cbData.correlator       = correlator;
  cbData.asyncResponse.rc = rc;
  if (rc == L7_SUCCESS)
    cbData.asyncResponse.u.response = response;
  else
    cbData.asyncResponse.u.reason   = reason;

  cnfgrApiCallback(&cbData);
}

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
L7_RC_t ptinHapiCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                        L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t ptinHapiRC;

  LOG_MSG("%s %u: %s: \n", __FILE__, __LINE__, __FUNCTION__);

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  ptinHapiRC     = L7_SUCCESS;

  ptinHapiCnfgrState_g = PTINHAPI_PHASE_INIT_1;

  ptinHapiApplyDebugConfigData();

  // Clear statistics
  (void)ptin_igmp_stat_init();
  (void)ptin_dhcp_stat_init();

  ptin_flow_init();
  printf("Fastpath initializations executed!\r\n");

  // Create Queues
  ptinL2Queue = (void *)osapiMsgQueueCreate(PTIN_L2_QUEUE, L7_MAX_FDB_MAC_ENTRIES, sizeof(ptinL2Msg_t));
  if (ptinL2Queue == L7_NULLPTR)
  {
    LOG_MSG("%s: Error creating ptin task\n",__FUNCTION__);
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    ptinHapiRC = L7_FAILURE;
    return ptinHapiRC;
  }
  /* L2 Task */
  if ((ptin_L2TaskId=osapiTaskCreate("PTIN L2 task", ptinL2Task, 0, 0,
                                       (32*1024),
                                       L7_DEFAULT_TASK_PRIORITY,
                                       L7_DEFAULT_TASK_SLICE)) == L7_ERROR)
  {
    printf("ERROR: Failed to start ptinHapiTask task\n");
    LOG_MSG("%s: Error creating ptin task\n",__FUNCTION__);
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    ptinHapiRC   = L7_ERROR;
    return ptinHapiRC;
  }

  /* L2 Management Task */
  if ((ptin_L2ManagTaskId=osapiTaskCreate("PTIN L2 task", ptinL2ManagementTask, 0, 0,
                                       (32*1024),
                                       L7_MEDIUM_TASK_PRIORITY,
                                       L7_DEFAULT_TASK_SLICE)) == L7_ERROR)
  {
    printf("ERROR: Failed to start ptinL2ManagementTask task\n");
    LOG_MSG("%s: Error creating ptin task\n",__FUNCTION__);
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    ptinHapiRC   = L7_ERROR;
    return ptinHapiRC;
  }

  // Initialize semaphores
  ptin_module_ready_sem = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_EMPTY);
  if (ptin_module_ready_sem==L7_NULLPTR)
  {
    LOG_MSG("%s: Error creating ptin_module_ready_sem\n",__FUNCTION__);
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    ptinHapiRC   = L7_ERROR;
    return ptinHapiRC;
  }
  ptin_module_l2_sem = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (ptin_module_l2_sem==L7_NULLPTR)
  {
    LOG_MSG("%s: Error creating ptin_module_l2_sem\n",__FUNCTION__);
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    ptinHapiRC   = L7_ERROR;
    return ptinHapiRC;
  }

  if ((ptin_mainTaskId=osapiTaskCreate("my task", ptinHapiTask, 0, 0,
                                       (32*1024),
                                       L7_DEFAULT_TASK_PRIORITY,
                                       L7_DEFAULT_TASK_SLICE)) == L7_ERROR)
  {
    printf("ERROR: Failed to start ptinHapiTask task\n");
    LOG_MSG("%s: Error creating ptin task\n",__FUNCTION__);
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    ptinHapiRC   = L7_ERROR;
    return ptinHapiRC;
  }

  /* wait for tasks initialization */
  if (osapiWaitForTaskInit(L7_PTINHAPI_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    LOG_MSG("%s: Error creating ptin task\n",__FUNCTION__);
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    ptinHapiRC   = L7_ERROR;
    return ptinHapiRC;
  }

  return ptinHapiRC;
}


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
L7_RC_t ptinHapiCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                        L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t ptinHapiRC;
  nvStoreFunctionList_t ptinHapiNotifyFunctionList;
//   L7_uint32 eventMask = 0;

  LOG_MSG("%s %u: %s: \n", __FILE__, __LINE__, __FUNCTION__);

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  ptinHapiRC     = L7_SUCCESS;


  memset((void *) &ptinHapiNotifyFunctionList, 0, sizeof(ptinHapiNotifyFunctionList));
  ptinHapiNotifyFunctionList.registrar_ID   = L7_PTINHAPI_COMPONENT_ID;
  ptinHapiNotifyFunctionList.notifySave     = ptinHapiSave;
  ptinHapiNotifyFunctionList.hasDataChanged = ptinHapiHasDataChanged;
  ptinHapiNotifyFunctionList.notifyConfigDump     = ptinHapiCfgDump;
  ptinHapiNotifyFunctionList.resetDataChanged = ptinHapiResetDataChanged;

  ptinHapiDeregister.ptinHapiSave                     = L7_FALSE;
  ptinHapiDeregister.ptinHapiRestore                  = L7_FALSE;
  ptinHapiDeregister.ptinHapiHasDataChanged           = L7_FALSE;
  ptinHapiDeregister.ptinHapiCfgDump                  = L7_FALSE;

  ptinHapiDeregister.ptinHapiIntfChangeCallback       = L7_FALSE;

  /*initilize debug*/

  //TODO ADB remove this
//   (void)ptinHapiDebugInit();

// // Register callback to process L2 address changes
// if (dtlCallbackRegistration(DTL_FAMILY_INTF_MGMT, dtlPTinIntfCallback) != L7_SUCCESS)
// {
//   LOG_MSG("PTIN: Unable to register to DTL for link changes\n");
//   DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d)\n", __FUNCTION__, __LINE__);
//   //return L7_FAILURE;
// }
// else  {
//   DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) success\n", __FUNCTION__, __LINE__);
// }

  // Register callback to process L2 address changes
  // Don't do this:
  // Module FDB makes the same registration... doing this disables him to catch events
  // More inteligent way: Call ptin callback inside FDB callback
  //
  /*
  if (dtlCallbackRegistration(DTL_FAMILY_ADDR_MGMT, dtlPTinL2Callback) != L7_SUCCESS)
  {
   LOG_MSG("PTIN: Unable to register to DTL for address changes\n");
   DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) ERROR\n", __FUNCTION__, __LINE__);
   //return L7_FAILURE;
  }
  else  {
   DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) success\n", __FUNCTION__, __LINE__);
  } 
  */ 

  // Register Callback to know when configurations are complete
#ifdef L7_CLI_PACKAGE
  if (txtCfgApplyCompletionNotifyRegister(L7_PTINHAPI_COMPONENT_ID, ptin_ApplyConfigCompleteCb))
  {
    LOG_MSG("%s: PTIN failed to register for apply config completions\n",__FUNCTION__);
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    ptinHapiRC  = L7_ERROR;
    return ptinHapiRC;
  }
#endif

  ptinHapiCnfgrState_g = PTINHAPI_PHASE_INIT_2;

  return ptinHapiRC;
}



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
L7_RC_t ptinHapiCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                        L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t ptinHapiRC;

  LOG_MSG("%s %u: %s: \n", __FILE__, __LINE__, __FUNCTION__);

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  ptinHapiRC     = L7_SUCCESS;

//  if (sysapiCfgFileGet(L7_DOT1S_COMPONENT_ID, DOT1S_CFG_FILENAME,
//                   (L7_char8 *)dot1sCfg, sizeof(dot1sCfg_t),
//                   &dot1sCfg->checkSum, DOT1S_CFG_VER_CURRENT,
//                   dot1sBuildConfigData, dot1sMigrateConfigData) != L7_SUCCESS)
//  {
//    *pResponse  = 0;
//    *pReason    = L7_CNFGR_ERR_RC_FATAL;
//    dot1sRC     = L7_ERROR;
//
//    return dot1sRC;
//  }

  if (ptinHapiCopyConfigData() != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    ptinHapiRC     = L7_ERROR;

    return ptinHapiRC;
  }

  ptinHapiCnfgrState_g = PTINHAPI_PHASE_INIT_3;

  return ptinHapiRC;
}

/*********************************************************************
* @purpose  This function undoes ptinHapiCnfgrInitPhase1Process
*
* @param    void
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void ptinHapiCnfgrFiniPhase1Process(void)
{
  /* deallocate anything that was allocated */

  LOG_MSG("%s %u: %s: \n", __FILE__, __LINE__, __FUNCTION__);

//   if (g_AppIpcSemaphore != L7_NULLPTR)
//   {
//      osapiSemaDelete(g_AppIpcSemaphore);
//   }

  // Close communications channel
  CloseIPC();

  // Delete Queues
  if (ptinL2Queue != L7_NULLPTR)
  {
    osapiMsgQueueDelete(ptinL2Queue);
    ptinL2Queue = L7_NULLPTR;
  }

  // Delete tasks
  osapiTaskDelete(ptin_L2TaskId);
  osapiTaskDelete(ptin_L2ManagTaskId);
  osapiTaskDelete(ptin_mainTaskId);

  // Delete semaphores
  osapiSemaDelete(ptin_module_l2_sem);
  osapiSemaDelete(ptin_module_ready_sem);

  // Delete tasks

  ptinHapiCnfgrState_g = PTINHAPI_PHASE_INIT_0;
}

/*********************************************************************
* @purpose  This function undoes cosCnfgrInitPhase2Process
*
* @param    void
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void ptinHapiCnfgrFiniPhase2Process(void)
{
  /*
   * Currently there is no real way of deregistering with other
   * components this will be a work item for the future. For now,
   * a temporary solution is set in place, wherein a registered
   * function can execute its callback only if its corresponding
   * member in the cosDeregister_g struct is set to L7_FALSE;
   */
  LOG_MSG("%s %u: %s: \n", __FILE__, __LINE__, __FUNCTION__);
  ptinHapiDeregister.ptinHapiSave = L7_TRUE;
  ptinHapiDeregister.ptinHapiRestore = L7_TRUE;
  ptinHapiDeregister.ptinHapiHasDataChanged = L7_TRUE;
  ptinHapiDeregister.ptinHapiIntfChangeCallback = L7_TRUE;

  // Deregister callbacks
  txtCfgApplyCompletionNotifyDeregister(L7_PTINHAPI_COMPONENT_ID);

  ptinHapiCnfgrState_g = PTINHAPI_PHASE_INIT_1;

//   CloseIPC();
}

/*********************************************************************
* @purpose  This function undoes cosCnfgrInitPhase3Process
*
* @param    void
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void ptinHapiCnfgrFiniPhase3Process(void)
{
  L7_CNFGR_RESPONSE_t response;
  L7_CNFGR_ERR_RC_t   reason;

  LOG_MSG("%s %u: %s: \n", __FILE__, __LINE__, __FUNCTION__);
  /* this func will place cosCnfgrState_g in WMU */
  if (ptinHapiCnfgrUconfigPhase2(&response, &reason) != L7_SUCCESS)
  {
    LOG_MSG("%s %u: %s: Error unconfiguring COS phase 2\n",
            __FILE__, __LINE__, __FUNCTION__);
    /* keep going */
  }
  ptinHapiCnfgrState_g = PTINHAPI_PHASE_INIT_2;
}


/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair as a NOOP
*
* @param    pResponse   @b{(output)} Response always command complete
* @param    pReason     @b{(output)} Always 0
*
* @returns  L7_SUCCESS  Always return this value (pResponse is available)
*
* @comments The following responses are valid:
*             L7_CNFGR_CMD_COMPLETE
*
* @comments The following error reason codes are valid:
*             (none used)
*
* @comments This function runs in the configurator's thread, which MUST NOT
*           be blocked!
*
* @end
*********************************************************************/
L7_RC_t ptinHapiCnfgrNoopProccess(L7_CNFGR_RESPONSE_t *pResponse,
                                  L7_CNFGR_ERR_RC_t   *pReason)
{
  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;

  return L7_SUCCESS;
}

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

L7_RC_t ptinHapiCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t ptinHapiRC, rc;
//  L7_uint32 i;

  LOG_MSG("%s %u: %s: \n", __FILE__, __LINE__, __FUNCTION__);

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  ptinHapiRC     = L7_SUCCESS;
  rc          = L7_SUCCESS;

  /* In Unconfig pahse 2 need to delete all the instances that have been
   * created. Interfaces will be taken care of by card manager and dot3ad
   * when they will be detached and deleted as a part of unconfigure phase 1
   * and the ptinHapiIhProcessIntfChange will do the unconfiguration for interfaces.
   * So ptinHapi is responsible only for the instances. Do not delete the CIST as
   * the driver cannot delete the CIST
   */
//  for (i = 1; i <= L7_MAX_MULTIPLE_STP_INSTANCES; i++)
//  {
//    if (ptinHapiInstanceMap[i].inUse == L7_TRUE &&
//                ptinHapiInstanceMap[i].instNumber != ptinHapi_CIST_ID)
//        {
//          rc = ptinHapiInstanceDelete(ptinHapiInstanceMap[i].instNumber);
//          if (rc != L7_SUCCESS)
//          {
//                break;
//          }
//        }
//  }
//  if (rc != L7_SUCCESS)
//  {
//        LOG_MSG("Unable to delete MSTP Instance %u in the hardware", i);
//        *pResponse  = 0;
//    *pReason    = L7_CNFGR_ERR_RC_FATAL;
//        ptinHapiRC = L7_ERROR;
//  }

  memset(ptinHapiCfg, 0, sizeof(ptinHapiCfg_t));

//  CloseIPC();

  ptinHapiCnfgrState_g = PTINHAPI_PHASE_WMU;

  return ptinHapiRC;
}


/*********************************************************************
*
* @purpose  To parse the configurator commands send to ptinHapiTask
*
* @param    none
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ptinHapiCfgDump()
{

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Ptin Task only starts execution, when this callback is received
*
* @param    event  : event from txt config
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    Right now we will handle only one event
*
* @end
*********************************************************************/
L7_RC_t ptin_ApplyConfigCompleteCb(L7_uint32 event)
{
  /* Ignore events types we don't care about.  */
  if ( (event != TXT_CFG_APPLY_FAILURE) &&
       (event != TXT_CFG_APPLY_SUCCESS) )
  {
    return L7_SUCCESS;
  }

  if (event==TXT_CFG_APPLY_FAILURE)
  {
    LOG_MSG("%s: Configurations failed\n",__FUNCTION__);
    return L7_FAILURE;
  }

  // After this point, PTin module must start execution
  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s: I'm going to release the semaphore!",__FUNCTION__);
  osapiSemaGive(ptin_module_ready_sem);

  return L7_SUCCESS;
}

