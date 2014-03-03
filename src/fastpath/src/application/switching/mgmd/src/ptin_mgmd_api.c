/*********************************************************************
*
* (C) Copyright PT Inovação S.A. 2013-2013
*
**********************************************************************
*
* @create    21/10/2013
*
* @author    Daniel Filipe Figueira
* @author    Marcio Daniel Melo
*
**********************************************************************/

#include "ptin_mgmd_api.h"
#include "logger.h"
#include "ptin_mgmd_cnfgr.h"
#include "snooping.h"
#include "ptin_mgmd_specificquery.h"
#include "ptin_mgmd_db.h"
#include "ptin_utils_inet_addr_api.h"

#include "ptin_mgmd_eventqueue.h"

#include "ptin_mgmd_sourcetimer.h"
#include "ptin_mgmd_grouptimer.h"
#include "ptin_mgmd_proxytimer.h"
#include "ptin_mgmd_querytimer.h"
#include "ptin_mgmd_groupsourcespecifictimer.h"
#include "ptin_mgmd_routercmtimer.h"
#include "ptin_mgmd_proxycmtimer.h"

#include "ptin_mgmd_statistics.h"

#include "ptin_mgmd_cfg_api.h"
#include "ptin_mgmd_service_api.h"

#include "ptin_mgmd_whitelist.h"

#include <arpa/inet.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>


static void* ptin_mgmd_event_handle(void*);
static RC_t  ptin_mgmd_timers_create(void);
static RC_t  ptin_mgmd_memAlloc(void);
static RC_t  ptin_mgmd_threadInit(pthread_t *thread_id, pthread_attr_t *attr);


RC_t ptin_mgmd_threadInit(pthread_t *thread_id, pthread_attr_t *attr)
{
  if (0 != pthread_attr_init(attr))
  {
    PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to initialize thread attributes");
    return FAILURE;
  }

  if (0 != pthread_attr_setstacksize(attr, PTIN_MGMD_STACK_SIZE))
  {
    PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to set thread stack size to %u", PTIN_MGMD_STACK_SIZE);
    return FAILURE;
  }

  if (0 != pthread_create(thread_id, attr, &ptin_mgmd_event_handle, PTIN_NULLPTR))
  {
    PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to start MGMD thread");
    return FAILURE;
  }

  return SUCCESS;
}

RC_t ptin_mgmd_timers_create(void)
{
  RC_t            res = SUCCESS;
  PTIN_MGMD_TIMER_CB_t timersCB; 
  uint32          num_timers = 0;

  //Source Timers
  num_timers = PTIN_MGMD_MAX_PORTS*PTIN_MGMD_MAX_SOURCES;
  if (SUCCESS == (res = ptin_mgmd_timer_createCB(PTIN_MGMD_TIMER_1MSEC, num_timers, 0, &timersCB)))
  {
    ptin_mgmd_sourcetimer_CB_set(timersCB);
    PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"\tTotal number of source timers: %u", num_timers);
  }
  else
  {
    return res;
  }

  //Group Timers
  num_timers = PTIN_MGMD_MAX_GROUPS*PTIN_MGMD_MAX_PORTS;
  if (SUCCESS == (res = ptin_mgmd_timer_createCB(PTIN_MGMD_TIMER_1MSEC, num_timers, 0, &timersCB)))
  {
    ptin_mgmd_grouptimer_CB_set(timersCB);
    PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"\tTotal number of group timers: %u", num_timers);
  }
  else
  {
    return res;
  }

  //Proxy Timers
  num_timers = PTIN_MGMD_MAX_GROUPS*PTIN_MGMD_MAX_PORTS;
  if (SUCCESS == (res = ptin_mgmd_timer_createCB(PTIN_MGMD_TIMER_1MSEC, num_timers, 0, &timersCB)))
  {
    ptin_mgmd_proxytimer_CB_set(timersCB);
    PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"\tTotal number of proxy timers: %u", num_timers);
  }
  else
  {
    return res;
  }

  //Query Timers
  num_timers = PTIN_MGMD_MAX_GROUPS*PTIN_MGMD_MAX_PORTS;
  if (SUCCESS == (res = ptin_mgmd_timer_createCB(PTIN_MGMD_TIMER_1MSEC, num_timers, 0, &timersCB)))
  {
    ptin_mgmd_querytimer_CB_set(timersCB);
    PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"\tTotal number of query timers: %u", num_timers);
  }
  else
  {
    return res;
  }

  //Group-Source Specific Timers
  num_timers = PTIN_MGMD_MAX_PORTS*PTIN_MGMD_MAX_CHANNELS;
  if (SUCCESS == (res = ptin_mgmd_timer_createCB(PTIN_MGMD_TIMER_1MSEC, num_timers, 0, &timersCB)))
  {
    ptin_mgmd_groupsourcespecifictimer_CB_set(timersCB);
    PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"\tTotal number of group-source specific timers: %u", num_timers);
  }
  else
  {
    return res;
  }

  //Router compatibility mode Timers
  num_timers = PTIN_MGMD_MAX_GROUPS*PTIN_MGMD_MAX_PORTS;
  if (SUCCESS == (res = ptin_mgmd_timer_createCB(PTIN_MGMD_TIMER_1MSEC, num_timers, 0, &timersCB)))
  {
    ptin_mgmd_routercmtimer_CB_set(timersCB);
    PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"\tTotal number of router compatibility mode timers: %u", num_timers);
  }
  else
  {
    return res;
  }

  //Proxy compatibility mode Timers
  num_timers = PTIN_MGMD_MAX_SERVICE_ID;
  if (SUCCESS == (res = ptin_mgmd_timer_createCB(PTIN_MGMD_TIMER_1MSEC, num_timers, 0, &timersCB)))
  {
    ptin_mgmd_proxycmtimer_CB_set(timersCB);
    PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"\tTotal number of proxy compatibility mode timers: %u", num_timers);
  }
  else
  {
    return res;
  }

  return res;
}


RC_t ptin_mgmd_memAlloc(void)
{
  RC_t res = SUCCESS;

  if(SUCCESS != (res = ptinMgmdEBInit()))
  {
    return res;
  }

  if(SUCCESS != (res = ptinMgmdGroupAVLTreeInit()))
  {
    return res;
  }

  if(SUCCESS != (res = ptinMgmdGroupRecordSourceAVLTreeInit()))
  {
    return res;
  }

  if(SUCCESS != (res = ptinMgmdGroupRecordGroupAVLTreeInit()))
  {
    return res;
  }

  if(SUCCESS != (res = ptinMgmdRootInterfaceAVLTreeInit()))
  {
    return res;
  }

  if(SUCCESS != (res = ptinMgmdSpecificQueryAVLTreeInit()))
  {
    return res;
  }

  if(SUCCESS != (res = ptinMgmdWhitelistInit()))
  {
    return res;
  }

  ptin_mgmd_memoryReport();

  return res;
}


/**
 * Used to initialize MGMD
 * 
 * @param thread_id[out]  : MGMD thread ID
 * @param externalApi[in] : Struct with API callbacks
 * @param logOutput[in]   : Output stream [LOG_STDERR; LOG_STDOUT; LOG_FILE]
 * @param logFile[in]     : System path plus file name for the log file
 *  
 * @return RC_t 
 *  
 * @note 'logFile' defaults to /var/log/mgmd.log if passed as PTIN_NULLPTR.
 * @note 'logFile' is ignored if 'logOutput' is not LOG_FILE
 */
RC_t ptin_mgmd_init(pthread_t *thread_id, ptin_mgmd_externalapi_t* externalApi, uint8 logOutput, char8* logFile)
{
  pthread_attr_t attr;

  //Validation
  if( (PTIN_NULLPTR==thread_id) || (PTIN_NULLPTR==externalApi) || ((logOutput==MGMD_LOG_FILE) && (PTIN_NULLPTR==logFile)) )
  {
    PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Abnormal context [thread_id:%p externalApi:%p logFile:%p]", thread_id, externalApi, logFile);
    return FAILURE;
  }

  //Log initialization
  ptin_mgmd_log_init(PTIN_MGMD_LOG_OUTPUT_STDERR);
  ptin_mgmd_log_redirect(logOutput, logFile);

  //Timer initialization
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Starting timer initialization allocation");
  if(SUCCESS != ptin_mgmd_timers_create())
  {
    PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to finish timer initialization");
    return FAILURE;
  }

  //Memory allocation
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Starting memory allocation");
  if(SUCCESS != ptin_mgmd_memAlloc())
  {
    PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to finish memory allocation");
    return FAILURE;
  }

  //Set API callbacks
  ptin_mgmd_externalapi_set(externalApi);

  // We need to decide wether it makes sense to do this here  
  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Loadind Default MGMD Configs");  
  if(SUCCESS != ptin_mgmd_igmp_proxy_defaultcfg_load())
  {
    PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to load default configs");
    return FAILURE;
  }

  if (SUCCESS != ptin_mgmd_eventqueue_init())
  {
    PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to initialize message queue");
    return FAILURE;
  }

  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Starting thread initialization");
  if (SUCCESS != ptin_mgmd_threadInit(thread_id, &attr))
  {
    PTIN_MGMD_LOG_CRITICAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to initialize MGMD thread");
    return FAILURE;
  }

  //Set log to ERROR by default after the init phase has been completed
  ptin_mgmd_log_sev_set(1 << PTIN_MGMD_LOG_CTX_PTIN_IGMP, 4);

  return SUCCESS;
}


/**
 * Used to uninitialize MGMD
 * 
 * @param thread_id[out] : MGMD thread ID
 * 
 * @return RC_t 
 */
RC_t ptin_mgmd_deinit(pthread_t thread_id)
{
  _UNUSED_(thread_id);
#if 0
  void                     *res;
  snoopPTinL3InfoData_t    *avlTreeEntry;  
  snoopPTinL3InfoDataKey_t avlTreeKey;
  mgmd_eb_t                *pSnoopEB;
  mgmd_cb_t                *pMgmdCB = PTIN_NULLPTR; 

  if ((pSnoopEB = mgmdEBGet()) == PTIN_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopEBGet()");
    return FAILURE;
  }

  /* Run all cells in AVL tree */
  memset(&avlTreeKey,0x00,sizeof(snoopPTinL3InfoDataKey_t));
  while( ( avlTreeEntry = avlSearchLVL7(&pSnoopEB->snoopPTinL3AvlTree, &avlTreeKey, AVL_NEXT) ) != PTIN_NULLPTR )
  {
    char                  debug_buf[IPV6_DISP_ADDR_LEN] = {0};
    snoopPTinL3InfoData_t *snoopEntry;

    /* Prepare next key */
    memcpy(&avlTreeKey, &avlTreeEntry->snoopPTinL3InfoDataKey, sizeof(snoopPTinL3InfoDataKey_t));
    if(PTIN_NULLPTR != (snoopEntry = snoopPTinL3EntryFind(avlTreeEntry->snoopPTinL3InfoDataKey.serviceId, &avlTreeEntry->snoopPTinL3InfoDataKey.groupAddr, AVL_EXACT)))
    {
      uint32 ifIdx;

      printf("Group: %s       serviceId: %u\n", inetAddrPrint(&(snoopEntry->snoopPTinL3InfoDataKey.groupAddr), debug_buf), snoopEntry->snoopPTinL3InfoDataKey.serviceId);
      printf("-----------------------------------------\n");

      for (ifIdx=0; ifIdx<PTIN_MGMD_MAX_PORTS; ++ifIdx)
      {
        if (snoopEntry->interfaces[ifIdx].active == TRUE)
        {
          uint32 sourceIdx; 

          printf("Interface: %02u |\n", ifIdx);                
          printf("              |Static:             %s\n", snoopEntry->interfaces[ifIdx].isStatic?"Yes":"No");        
          printf("              |Filter-Mode:        %s\n", snoopEntry->interfaces[ifIdx].filtermode==PTIN_MGMD_FILTERMODE_INCLUDE?"Include":"Exclude");
          if(SNOOP_PTIN_PROXY_ROOT_INTERFACE_ID == ifIdx)
          {
            printf("              |proxyCM:            %u\n", pMgmdCB->proxyCM[snoopEntry->snoopPTinL3InfoDataKey.serviceId].compatibilityMode);
            printf("              |proxyCM-Timer:      %u\n", ptin_mgmd_proxycmtimer_timeleft(&pMgmdCB->proxyCM[snoopEntry->snoopPTinL3InfoDataKey.serviceId]));
          }
          else
          {
            printf("              |routerCM:           %u\n", snoopEntry->interfaces[ifIdx].groupCMTimer.compatibilityMode);
            printf("              |routerCM-Timer:     %u\n", ptin_mgmd_routercmtimer_timeleft(&snoopEntry->interfaces[ifIdx].groupCMTimer));
          }
          printf("              |Nbr of Sources:     %u\n", snoopEntry->interfaces[ifIdx].numberOfSources);        
          printf("              |Group-Timer:        %u\n", ptin_mgmd_grouptimer_timeleft(&snoopEntry->interfaces[ifIdx].groupTimer));                
          printf("              |Nbr of Clients:     %u\n", snoopEntry->interfaces[ifIdx].numberOfClients);        
          printf("              |Clients: ");
          int8 clientIdx;
          for (clientIdx=(PTIN_MGMD_CLIENT_BITMAP_SIZE-1); clientIdx>=0; --clientIdx)
          {
            printf("%02X", snoopEntry->interfaces[ifIdx].clients[clientIdx]);
          }                      
          printf("\n");
          for (sourceIdx=0; sourceIdx<PTIN_MGMD_MAX_SOURCES; ++sourceIdx)
          {
            if (snoopEntry->interfaces[ifIdx].sources[sourceIdx].status != PTIN_MGMD_SOURCESTATE_INACTIVE)
            {
              int8 clientIdx;
              printf("                       |Source: %s\n", inetAddrPrint(&(snoopEntry->interfaces[ifIdx].sources[sourceIdx].sourceAddr), debug_buf));
              printf("                                |Static:         %s\n", snoopEntry->interfaces[ifIdx].sources[sourceIdx].isStatic?"Yes":"No");
              printf("                                |status:         %s\n", snoopEntry->interfaces[ifIdx].sources[sourceIdx].status==PTIN_MGMD_SOURCESTATE_ACTIVE?"Active":"ToRemove");            
              printf("                                |Timer Running:  %s\n", ptin_mgmd_sourcetimer_isRunning(&snoopEntry->interfaces[ifIdx].sources[sourceIdx].sourceTimer)?"Yes":"No");
              printf("                                |Source-Timer:   %u\n", ptin_mgmd_sourcetimer_timeleft(&snoopEntry->interfaces[ifIdx].sources[sourceIdx].sourceTimer));
              printf("                                |Nbr of Clients: %u\n", snoopEntry->interfaces[ifIdx].sources[sourceIdx].numberOfClients);            
              printf("                                |Clients: ");            
              for (clientIdx=(PTIN_MGMD_CLIENT_BITMAP_SIZE-1); clientIdx>=0; --clientIdx)
              {
                printf("%02X", snoopEntry->interfaces[ifIdx].sources[sourceIdx].clients[clientIdx]);
              }
              printf("\n");
            }
          }
        }
      }
    }
  }

  pthread_cancel(thread_id);
  pthread_join(thread_id, &res);
#endif
  return SUCCESS;
}

/**
 * Used to initialize MGMD
 * 
 * @param context[in]  : Log context
 * @param severity[in] : Log severity level
 *  
 * @return RC_t 
 */
RC_t ptin_mgmd_logseverity_set(uint8 context, uint8 severity)
{
  return ptin_mgmd_log_sev_set(1 << context, severity);
}


void* ptin_mgmd_event_handle(void *param)
{
  _UNUSED_(param);
  PTIN_MGMD_EVENT_t   eventMsg;
  ptin_IgmpProxyCfg_t igmpCfg;

  while (1)
  {
    if (SUCCESS != ptin_mgmd_eventQueue_rx(&eventMsg))
    {
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to read from rxEventQueue");
      continue; //Do not abort here..Instead, just continue to the next event
    }
    PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "");
    PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "###################################################################################");

    if (ptin_mgmd_igmp_proxy_config_get(&igmpCfg) != SUCCESS)
    {
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Error getting MGMD Proxy configurations");
    }

    switch (eventMsg.type)
    {
      case PTIN_MGMD_EVENT_CODE_PACKET:
      {
        PTIN_MGMD_EVENT_PACKET_t eventData = {0};

        if(PTIN_MGMD_ENABLE != igmpCfg.admin)
        {
          PTIN_MGMD_LOG_NOTICE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "MGMD is disabled: Packet event discarded");
          break;
        }

        ptin_mgmd_event_packet_parse(&eventMsg, &eventData);
        ptin_mgmd_event_packet(&eventData);

        break;
      }
      case PTIN_MGMD_EVENT_CODE_TIMER:
      {
        PTIN_MGMD_EVENT_TIMER_t eventData = {0};

        ptin_mgmd_event_timer_parse(&eventMsg, &eventData);
        ptin_mgmd_event_timer(&eventData);

        break;
      }
      case PTIN_MGMD_EVENT_CODE_CTRL:
      {
        PTIN_MGMD_EVENT_CTRL_t eventData = {0};

        ptin_mgmd_event_ctrl_parse(&eventMsg, &eventData);
        ptin_mgmd_event_ctrl(&eventData);

        //Send the result to the CTRL
        ptin_mgmd_event_ctrl_create(&eventMsg, eventData.msgCode, eventData.msgId, eventData.res, eventData.msgQueueId, eventData.data, eventData.dataLength);
        if (SUCCESS != ptin_mgmd_messageQueue_send(eventData.msgQueueId, &eventMsg))
        {
          PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to write to txEventQueue");
          continue; //Do not abort here..Instead, just continue to the next event
        }

        break;
      }
      case PTIN_MGMD_EVENT_CODE_DEBUG:
      {
        PTIN_MGMD_EVENT_DEBUG_t eventData = {0};

        ptin_mgmd_event_debug_parse(&eventMsg, &eventData);
        ptin_mgmd_event_debug(&eventData);
        break;
      }
      default:
      {
        PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unknown event type received");
      }
    }
  }

  return PTIN_NULLPTR;
}


#ifdef _COMPILE_AS_BINARY_

pthread_t thread_id;
volatile sig_atomic_t fatal_error_in_progress = 0;

void signal_handler(int sig) 
{
  if(!fatal_error_in_progress)
  {
    pthread_cancel(thread_id);
    exit(0);
  }

  raise(sig);
}

int main(int argc, char **argv)
{
  void                    *res;
  ptin_mgmd_externalapi_t externalApi = {0}; 

  _UNUSED_(argc);
  _UNUSED_(argv);

  signal(SIGINT,signal_handler); //Register CTRL+C signal

  externalApi.igmp_admin_set = &ptin_mgmd_cfg_igmp_admin_set;
  externalApi.mld_admin_set  = &ptin_mgmd_cfg_mld_admin_set;
  externalApi.cos_set        = &ptin_mgmd_cfg_cos_set;
  externalApi.portList_get   = &ptin_mgmd_port_getList;
  externalApi.portType_get   = &ptin_mgmd_port_getType;
  externalApi.clientList_get = &ptin_mgmd_client_getList;
  externalApi.port_open      = &ptin_mgmd_port_open;
  externalApi.port_close     = &ptin_mgmd_port_close;
  externalApi.tx_packet      = &ptin_mgmd_tx_packet;

  if(SUCCESS != ptin_mgmd_init(&thread_id, &externalApi, MGMD_LOG_STDOUT, PTIN_NULLPTR))
  {
    PTIN_MGMD_LOG_FATAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Catastrophic Failure! Run as fast as you can and don't look back!");
    return -1;
  }

  ptin_mgmd_logseverity_set(PTIN_MGMD_LOG, MGMD_LOG_TRACE);

  pthread_join(thread_id, &res); //Wait forever..

  ptin_mgmd_log_deinit();

  return 0;
}
#endif //_COMPILE_AS_BINARY_
