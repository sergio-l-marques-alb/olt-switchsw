/*
 * ptin_cnfgr.c
 *
 * Created on: 2010/04/08
 * Author: Andre Brizido
 *
 * Routines for Initialization adn Reinitialization
 * 
 * Modified on 2011/06/14 
 * By: Alexandre Santos (alexandre-r-santos@ptinovacao.pt) 
 * Notes:
 *   The implementation of this module is different from the standard
 *   defined for the fastpath modules.
 *   We do not create a task to handle messages because this is already
 *   achieved through 'ptin_msghandler' module.
 *   Instead we initialize everything using the fastpath thread (by directly
 *   invoking the initialization functions without passing through a message
 *   queue. This violates the rules defined on the 'Getting Started' doc, but
 *   it doesn't consume much CPU on those tasks.
 *   The ptinTask will actually work as a postponed configuration setting task.
 */

#include <unistd.h>
#include "cli_txt_cfg_api.h"
#include "ptin_cnfgr.h"

#include "ptin_msghandler.h"
#include "ptin_control.h"
#include "ptin_evc.h"
#include "ptin_igmp.h"
#include "ptin_dhcp.h"
#include "ptin_pppoe.h"
#include "ptin_prot_typeb.h"
#include "ptin_routing.h"
#include "ptin_ssm.h"
#include "ptin_prot_erps.h"
#include "ptin_ipdtl0_packet.h"
#include "ptin_rfc2819.h"
#include <ptin_prot_oam_eth.h>
#include <ptin_oam_packet.h>

#include "ipc.h"

#include <signal.h>

#if ( !PTIN_BOARD_IS_MATRIX )
#include "ptin_packet.h"
#endif

#if ( PTIN_BOARD_IS_STANDALONE || defined(SYNC_SSM_IS_SUPPORTED))
#include "fw_shm.h"
#endif

//#include "sirerrors.h"
//#include "traces.h"
//#include "ptin_cfg.h"
//#include "ptin_cnfgr.h"
//#include "ptin_intf.h"
//#include "ptin_msghandler.h"

/* Semaphore to synchronize PTin task execution */
void *ptin_ready_sem = L7_NULLPTR;

/* MGMD TxQueueId */
L7_int32 ptinMgmdTxQueueId = -1;

/* Get MGMD TxQueueId */
inline L7_int32 ptin_mgmd_txqueue_id_get(void){return ptinMgmdTxQueueId;};
/* Reinit MGMD TxQueue */
inline RC_t ptin_mgmd_txqueue_reinit(void) {return ptin_mgmd_txqueue_create(MGMD_TXQUEUE_KEY, &ptinMgmdTxQueueId);};


#if (PTIN_BOARD_IS_MATRIX)
void *ptin_switchover_sem  = L7_NULLPTR;
void *ptin_boardaction_sem = L7_NULLPTR;
#endif

static ptinCnfgrState_t ptinCnfgrState = PTIN_PHASE_INIT_0;


/**
 * Signal trap callback
 * 
 * @param signum 
 */
void main_sig_caught(int signum)
{
  LOG_PT_INFO(LOG_CTX_STARTUP, "firmware recebeu o sinal %d",signum);

  /* Close IPC channel */
  CloseIPC();

  /* Initialize IPC message runtime measurements */
  CHMessage_runtime_meter_init((L7_uint) -1);

  /* Open channel to communicate with the Manager */
  if (OpenIPC() != S_OK)
  {
    LOG_PT_ERR(LOG_CTX_CNFGR, "Error opening IPC channel!");
  }
  LOG_PT_INFO(LOG_CTX_CNFGR, "IPC Communications channel OK");
}


/*********************************************************************
* @purpose  PTin Task only starts execution when this callback is attended
*
* @param    event  : event from txt config
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    Right now we will handle only one event
*
* @end
*********************************************************************/
L7_RC_t ptinApplyConfigCompleteCb(L7_uint32 event)
{
  /* Ignore events types we don't care about.  */
  if ( (event != TXT_CFG_APPLY_FAILURE) &&
       (event != TXT_CFG_APPLY_SUCCESS) )
  {
    return L7_SUCCESS;
  }

  if (event == TXT_CFG_APPLY_FAILURE)
  {
    LOG_PT_FATAL(LOG_CTX_CNFGR, "OLTSWITCH configurations failed!");
    return L7_FAILURE;
  }

  /* After this point, PTin task must start execution */
  osapiSemaGive(ptin_ready_sem);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  CNFGR System Initialization for PTIN component
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this
*                                             CNFGR request
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the PTIN comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void ptinApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

  L7_RC_t             ptinRC = L7_ERROR;
  L7_CNFGR_ERR_RC_t   reason   = L7_CNFGR_ERR_RC_INVALID_PAIR;

  /* validate command type */
  if ( pCmdData != L7_NULL )
  {
    if (pCmdData->type == L7_CNFGR_RQST)
    {
      command    = pCmdData->command;
      request    = pCmdData->u.rqstData.request;
      correlator = pCmdData->correlator;
      if ( request > L7_CNFGR_RQST_FIRST &&
           request < L7_CNFGR_RQST_LAST )
      {
        /* validate command/event pair */
        switch ( command )
        {
          case L7_CNFGR_CMD_INITIALIZE:
            switch ( request )
            {
              case L7_CNFGR_RQST_I_PHASE1_START:
                if ((ptinRC = ptinCnfgrInitPhase1Process( &response, &reason )) != L7_SUCCESS)
                {
                  ptinCnfgrFiniPhase1Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                if ((ptinRC = ptinCnfgrInitPhase2Process( &response, &reason )) != L7_SUCCESS)
                {
                  ptinCnfgrFiniPhase2Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE3_START:
                if ((ptinRC = ptinCnfgrInitPhase3Process( &response, &reason )) != L7_SUCCESS)
                {
                  ptinCnfgrFiniPhase3Process();
                }
                break;
              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                ptinRC = ptinCnfgrNoopProccess( &response, &reason );
                ptinCnfgrState = PTIN_PHASE_WMU;
                break;
              default:
                /* invalid command/request pair */
                break;
            } /* endswitch initialize requests */
            break;

          case L7_CNFGR_CMD_EXECUTE:
            switch ( request )
            {
              case L7_CNFGR_RQST_E_START:
                ptinCnfgrState = PTIN_PHASE_EXECUTE;
                ptinRC  = L7_SUCCESS;
                response  = L7_CNFGR_CMD_COMPLETE;
                reason    = 0;
                break;

              default:
                /* invalid command/request pair */
                break;
            }
            break;

          case L7_CNFGR_CMD_UNCONFIGURE:
            switch ( request )
            {
              case L7_CNFGR_RQST_U_PHASE1_START:
                ptinRC = ptinCnfgrNoopProccess( &response, &reason );
                ptinCnfgrState = PTIN_PHASE_UNCONFIG_1;
                break;

              case L7_CNFGR_RQST_U_PHASE2_START:
                ptinRC = ptinCnfgrUconfigPhase2( &response, &reason );
                ptinCnfgrState = PTIN_PHASE_UNCONFIG_2;
                /* we need to do something with the stats in the future */
                break;

              default:
                /* invalid command/request pair */
                break;
            }
            break;

          case L7_CNFGR_CMD_TERMINATE:
          case L7_CNFGR_CMD_SUSPEND:
            ptinRC = ptinCnfgrNoopProccess( &response, &reason );
            break;

          default:
            reason = L7_CNFGR_ERR_RC_INVALID_CMD;
            break;
        } /* endswitch command/event pair */


      } else
      {
        reason = L7_CNFGR_ERR_RC_INVALID_RSQT;


      } /* endif validate request */


    } else
    {
      reason = L7_CNFGR_ERR_RC_INVALID_CMD_TYPE;


    } /* endif validate command type */


  } else
  {
    correlator = L7_NULL;
    reason = L7_CNFGR_ERR_RC_INVALID_CMD;


  } /* check for command valid pointer */

  /* return value to caller -
   * <prepare complesion response>
   * <callback the configurator>
   */
  cbData.correlator       = correlator;
  cbData.asyncResponse.rc = ptinRC;
  if (ptinRC == L7_SUCCESS)
    cbData.asyncResponse.u.response = response;
  else
    cbData.asyncResponse.u.reason   = reason;

  cnfgrApiCallback(&cbData);


  return;
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
L7_RC_t ptinCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t rc = L7_SUCCESS;

  LOG_PT_TRACE(LOG_CTX_STARTUP,"Phase 1...");

  /* Phase 1:
   *  - Create queues, tasks, semaphores, timers
   *  - Allocate memory
   *  - Memset to zero data structures
   */

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;

  /* Create an empty binary semphore (mutex) to hold ptinTask to apply
   * configurations without being sure that all the other fastpath modules
   * have started and are ready */
  ptin_ready_sem = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_EMPTY);
  if (ptin_ready_sem == L7_NULLPTR)
  {
    LOG_PT_FATAL(LOG_CTX_CNFGR, "Failed to create ptin_ready semaphore!");

    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_FAILURE;
  }

  /* Initialize EVC data structures */
  ptin_evc_init();

  /* Initialize DHCP data structures */
  ptin_dhcp_init();

  /* Initialize PPPoE data structures */
  ptin_pppoe_init();

  /* Initialize Type-B Protection data structures */
  ptin_prottypeb_init();

#if (PTIN_BOARD_IS_STANDALONE || defined(SYNC_SSM_IS_SUPPORTED))
  /* Open shared memory to communicate with the GPON application */
  if (fw_shm_open() != 0)
  {
    LOG_PT_FATAL(LOG_CTX_CNFGR, "Error initializing shared memory!");
    return L7_FAILURE;
  }
  else
  {
    memset(pfw_shm, 0x00, sizeof(t_fw_shm));
    LOG_PT_INFO(LOG_CTX_CNFGR, "Shared memory OK");
  }
#endif

  /* Create a new TxQueue to handle responses from MGMD */
  ptin_mgmd_txqueue_create(MGMD_TXQUEUE_KEY, &ptinMgmdTxQueueId);

  ptinCnfgrState = PTIN_PHASE_INIT_1;

  LOG_PT_INFO(LOG_CTX_CNFGR, "PTIN Phase 1 initialization OK");

  LOG_PT_TRACE(LOG_CTX_STARTUP,"End of Phase 1: rc=%d",rc);

  return rc;
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
L7_RC_t ptinCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                     L7_CNFGR_ERR_RC_t   *pReason )
{
  /* Phase 2:
  *  - Register for callbacks with other components
  */
  
  LOG_PT_TRACE(LOG_CTX_STARTUP,"Phase 2...");

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  
  /* Register Callback to know when configurations are complete */
#ifdef L7_CLI_PACKAGE
  if (txtCfgApplyCompletionNotifyRegister(L7_PTIN_COMPONENT_ID, ptinApplyConfigCompleteCb))
  {
    LOG_PT_FATAL(LOG_CTX_CNFGR, "Failed to register for config completion callback!");

    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    return L7_FAILURE;
  }
#else
  LOG_PT_FATAL(LOG_CTX_CNFGR, "L7_CLI_PACKAGE is not present!");

  *pResponse  = 0;
  *pReason    = L7_CNFGR_ERR_RC_FATAL;
  return L7_FAILURE;
#endif

#if (PTIN_BOARD_IS_MATRIX)
  /* Semaphore to control board insertion/remotion */
  ptin_boardaction_sem = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (ptin_boardaction_sem == L7_NULLPTR)
  {
    LOG_PT_FATAL(LOG_CTX_CNFGR, "Failed to create ptin_boardaction_sem semaphore!");

    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_FAILURE;
  }
#endif

  /* Initialize IGMP data structures (includes semaphores) */
  ptin_igmp_proxy_init();

  /* Create PTin task */
  if (osapiTaskCreate("PTin task", ptinTask, 0, 0,
                      L7_DEFAULT_STACK_SIZE*10,
                      L7_DEFAULT_TASK_PRIORITY,
                      L7_DEFAULT_TASK_SLICE) == L7_ERROR)
  {
    LOG_PT_FATAL(LOG_CTX_CNFGR, "Failed to create PTin task!");

    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_FAILURE;
  }
  /* Wait for task to be launched */
  if (osapiWaitForTaskInit (L7_PTIN_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    LOG_PT_FATAL(LOG_CTX_CNFGR, "Failed to start PTin task!");

    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_FAILURE;
  }
  LOG_PT_INFO(LOG_CTX_CNFGR, "PTin task launch OK");

  /* Initialize OAM data structures (includes task and timer) */
  ptin_oam_eth_init();

  /* Allocate SSM resources */
  if (ssm_init() != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_FAILURE;
  }
  LOG_PT_INFO(LOG_CTX_CNFGR, "SSM init OK");

    /* Initialize ERPS data structures (includes semaphores and timer) */
#ifdef PTIN_ENABLE_ERPS
  ptin_prot_erps_init();
#endif

#ifdef COMMON_APS_CCM_CALLBACKS__ETYPE_REG
  {
#ifdef PTIN_ENABLE_ERPS
   unsigned long i;

   for (i=0; i<MAX_PROT_PROT_ERPS; i++) ptin_aps_packet_init(i);//Initialize message queues
#endif

   common_aps_ccm_packetRx_callback_register(); //must be after OAM ETH and ERP queues init: ptin_ccm_packet_init(-1) and ptin_aps_packet_init()
  }
#endif

  /* IP dtl0 module initialization. */
#ifdef PTIN_ENABLE_DTL0TRAP
  ptin_ipdtl0_init();
#endif

#if ( !PTIN_BOARD_IS_MATRIX )
  if (ptin_packet_init() != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    return L7_ERROR;
  }
  LOG_PT_INFO(LOG_CTX_CNFGR, "ptin_packet initialized!");
#endif

/* Only make interface state management, if CXO board */
#ifdef PTIN_LINKSCAN_CONTROL
#if (PTIN_BOARD_IS_MATRIX)
#if (PTIN_BOARD == PTIN_BOARD_CXO640G || PTIN_BOARD == PTIN_BOARD_CXO160G)
  ptin_switchover_sem = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_EMPTY);
  if (ptin_switchover_sem == L7_NULLPTR)
  {
    LOG_PT_FATAL(LOG_CTX_CNFGR, "Failed to create ptin_switchover_sem semaphore!");

    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_FAILURE;
  }

  /* Create ptinSwitchoverTask */
  if (osapiTaskCreate("PTinSwitchover task", ptinSwitchoverTask, 0, 0,
                      L7_DEFAULT_STACK_SIZE,
                      L7_DEFAULT_TASK_PRIORITY,
                      L7_DEFAULT_TASK_SLICE) == L7_ERROR)
  {
    LOG_PT_FATAL(LOG_CTX_CONTROL, "Failed to create ptinSwitchoverTask!");
    return L7_FAILURE;
  }
  LOG_PT_INFO(LOG_CTX_CONTROL, "ptinSwitchoverTask created");

  /* Wait for task to be launched */
  if (osapiWaitForTaskInit (L7_PTIN_SWITCHOVER_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    LOG_PT_FATAL(LOG_CTX_CONTROL, "Failed to start ptinSwitchoverTask!");
    return L7_FAILURE;
  }
  LOG_PT_INFO(LOG_CTX_CONTROL, "ptinSwitchoverTask launch OK");
#endif
#endif
#endif

  /* Initialize rfc2819 monitoring (includes structures and tasks) */
  ptin_rfc2819_init();

  ptinCnfgrState = PTIN_PHASE_INIT_2;

  LOG_PT_INFO(LOG_CTX_CNFGR, "PTIN Phase 2 initialization OK");

  LOG_PT_TRACE(LOG_CTX_STARTUP,"End of Phase 2 (Success)");

  return L7_SUCCESS;
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
L7_RC_t ptinCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                     L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t rc = L7_SUCCESS;

  LOG_PT_TRACE(LOG_CTX_STARTUP,"Phase 3...");

  /* Phase 3:
   *  - Reset all status structures to initial state 
   *  - Read configuration
   *  - Apply global and NON-interface configuration
   */

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;

  /* Initialize IPC message runtime measurements */
  CHMessage_runtime_meter_init((L7_uint) -1);

  /* Open channel to communicate with the Manager */
  while (OpenIPC() != S_OK)
  {
    LOG_PT_CRITIC(LOG_CTX_CNFGR, "Error opening IPC channel! Retrying in %d seconds...", PTIN_T_RETRY_IPC);
    sleep(PTIN_T_RETRY_IPC);
  }
  LOG_PT_INFO(LOG_CTX_CNFGR, "IPC Communications channel OK");

  /* Define signal trap callback */
  if ( signal(SIGUSR1, main_sig_caught)==SIG_ERR )
  {
    LOG_PT_FATAL(LOG_CTX_STARTUP,"Create SIGTERM Handler [ERROR]");
    PTIN_CRASH();
  }

  /* Initialize Routing data structures */
  ptin_routing_init();

  ptinCnfgrState = PTIN_PHASE_INIT_3;

  LOG_PT_INFO(LOG_CTX_CNFGR, "PTIN Phase 3 initialization OK");

  LOG_PT_TRACE(LOG_CTX_STARTUP,"End of Phase 3: rc=%d", rc);

  return rc;
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
void ptinCnfgrFiniPhase1Process(void)
{
  /* deallocate anything that was allocated */

  /* Deconfigure IGMP proxy */
  ptin_igmp_proxy_deinit();

  LOG_PT_INFO(LOG_CTX_CNFGR, "SSM fini OK");

  ptinCnfgrState = PTIN_PHASE_INIT_0;
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
void ptinCnfgrFiniPhase2Process(void)
{
  /*
   * Currently there is no real way of deregistering with other
   * components this will be a work item for the future. For now,
   * a temporary solution is set in place, wherein a registered
   * function can execute its callback only if its corresponding
   * member in the cosDeregister_g struct is set to L7_FALSE;
   */

#if ( !PTIN_BOARD_IS_MATRIX )
  /* Deinit ptin packet module */
  ptin_packet_deinit();
#endif

  /* Deallocate SSM resources */
  ssm_fini();

   ptinCnfgrState = PTIN_PHASE_INIT_1;
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
void ptinCnfgrFiniPhase3Process(void)
{
  L7_CNFGR_RESPONSE_t response;
  L7_CNFGR_ERR_RC_t   reason;

  /* this func will place ptinCnfgrState in WMU */
  if (ptinCnfgrUconfigPhase2(&response, &reason) != L7_SUCCESS)
  {
    /* keep going */
  }

  /* Open channel to communicate with the Manager */
  CloseIPC();

  ptinCnfgrState = PTIN_PHASE_INIT_2;
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
L7_RC_t ptinCnfgrNoopProccess(L7_CNFGR_RESPONSE_t *pResponse,
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

L7_RC_t ptinCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                 L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t rc = L7_SUCCESS;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;

  ptinCnfgrState = PTIN_PHASE_WMU;

  return rc;
}


