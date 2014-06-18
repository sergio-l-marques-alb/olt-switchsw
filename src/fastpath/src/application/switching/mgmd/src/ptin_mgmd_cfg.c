/*********************************************************************
*
* (C) Copyright PT Inova��o S.A. 2013-2013
*
**********************************************************************
*
* @filename  ptin_mgmd_cfg.c
*
* @purpose   Contains definitions to support the configuration read
* @purpose   save and apply routines
*
* @component Mgmd
*
* @comments  none
*
* @create    18/10/2013
*
* @author    M�rcio Melo (marcio-d-melo@ptinovacao.pt)
* @end
*
**********************************************************************/


#include "ptin_mgmd_cfg.h"
#include "ptin_mgmd_logger.h"
#include "ptin_mgmd_cfg_api.h"
#include "ptin_mgmd_core.h"
#include "ptin_mgmd_util.h"
#include "ptin_mgmd_cnfgr.h"
#include "ptin_mgmd_statistics.h"
#include "ptin_mgmd_grouptimer.h"
#include "ptin_mgmd_sourcetimer.h"

ptin_IgmpProxyCfg_t     mgmdProxyCfg;
ptin_mgmd_externalapi_t ptin_mgmd_externalapi = {PTIN_NULLPTR};

static RC_t ptin_mgmd_igmp_proxy_config_validate(ptin_IgmpProxyCfg_t *igmpProxy);


void ptin_mgmd_cfg_memory_allocation(void)
{
  ptin_mgmd_memory_allocation+=sizeof(ptin_mgmd_externalapi);  
}
/**
 * Configure the external API.
 * 
 * @param externalApi
 * 
 * @return RC_t
 */
RC_t ptin_mgmd_externalapi_set(ptin_mgmd_externalapi_t* externalApi)
{
  if(PTIN_NULLPTR == externalApi)
  {
    return FAILURE;
  }

  ptin_mgmd_externalapi.igmp_admin_set = externalApi->igmp_admin_set;
  ptin_mgmd_externalapi.mld_admin_set  = externalApi->mld_admin_set;

  ptin_mgmd_externalapi.cos_set        = externalApi->cos_set;

  ptin_mgmd_externalapi.portList_get   = externalApi->portList_get;
  ptin_mgmd_externalapi.portType_get   = externalApi->portType_get;

  ptin_mgmd_externalapi.clientList_get = externalApi->clientList_get;

  ptin_mgmd_externalapi.port_open      = externalApi->port_open;
  ptin_mgmd_externalapi.port_close     = externalApi->port_close;

  ptin_mgmd_externalapi.tx_packet      = externalApi->tx_packet;

  return SUCCESS;
}

/**
 * Get the configured external API.
 * 
 * @param externalApi
 * 
 * @return RC_t
 */
RC_t ptin_mgmd_externalapi_get(ptin_mgmd_externalapi_t* externalApi)
{
  if(PTIN_NULLPTR == externalApi)
  {
    return FAILURE;
  }

  if( (PTIN_NULLPTR == ptin_mgmd_externalapi.igmp_admin_set) ||
      (PTIN_NULLPTR == ptin_mgmd_externalapi.mld_admin_set)  ||
      (PTIN_NULLPTR == ptin_mgmd_externalapi.cos_set)        ||
      (PTIN_NULLPTR == ptin_mgmd_externalapi.portList_get)   ||
      (PTIN_NULLPTR == ptin_mgmd_externalapi.portType_get)   ||
      (PTIN_NULLPTR == ptin_mgmd_externalapi.clientList_get) ||
      (PTIN_NULLPTR == ptin_mgmd_externalapi.port_open)      ||
      (PTIN_NULLPTR == ptin_mgmd_externalapi.port_close)     ||
      (PTIN_NULLPTR == ptin_mgmd_externalapi.tx_packet) )
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid callback found in external API!");
    return FAILURE;
  }

  externalApi->igmp_admin_set = ptin_mgmd_externalapi.igmp_admin_set; 
  externalApi->mld_admin_set  = ptin_mgmd_externalapi.mld_admin_set; 

  externalApi->cos_set        = ptin_mgmd_externalapi.cos_set; 
   
  externalApi->portList_get   = ptin_mgmd_externalapi.portList_get;   
  externalApi->portType_get   = ptin_mgmd_externalapi.portType_get;   

  externalApi->clientList_get = ptin_mgmd_externalapi.clientList_get;   
                                                 
  externalApi->port_open      = ptin_mgmd_externalapi.port_open;      
  externalApi->port_close     = ptin_mgmd_externalapi.port_close;  
     
  externalApi->tx_packet      = ptin_mgmd_externalapi.tx_packet;     

  return SUCCESS;
}

/**
 * Load IGMP proxy default configuraion parameters
 * 
 * @return RC_t SUCCESS/FAILURE
 */
RC_t ptin_mgmd_igmp_proxy_defaultcfg_load(void)
{
  ptin_IgmpProxyCfg_t igmpProxy;
  RC_t rc;
  
  igmpProxy.mask                                   = 0xFF;
  igmpProxy.admin                                  = 0;
  igmpProxy.whitelist                              = PTIN_IGMP_DEFAULT_WHITELIST_MODE;
  igmpProxy.networkVersion                         = PTIN_IGMP_DEFAULT_VERSION;
  igmpProxy.clientVersion                          = PTIN_IGMP_DEFAULT_VERSION;
  igmpProxy.ipv4_addr                              = PTIN_IGMP_DEFAULT_IPV4;
  igmpProxy.igmp_cos                               = PTIN_IGMP_DEFAULT_COS;
  igmpProxy.fast_leave                             = PTIN_IGMP_DEFAULT_FASTLEAVEMODE;

  igmpProxy.querier.mask                           = 0xFFFF;
  igmpProxy.querier.flags                          = 0;
  igmpProxy.querier.robustness                     = PTIN_IGMP_DEFAULT_ROBUSTNESS;
  igmpProxy.querier.query_interval                 = PTIN_IGMP_DEFAULT_QUERYINTERVAL;
  igmpProxy.querier.query_response_interval        = PTIN_IGMP_DEFAULT_QUERYRESPONSEINTERVAL;
  igmpProxy.querier.group_membership_interval      = PTIN_IGMP_DEFAULT_GROUPMEMBERSHIPINTERVAL;
  igmpProxy.querier.other_querier_present_interval = PTIN_IGMP_DEFAULT_OTHERQUERIERPRESENTINTERVAL;
  igmpProxy.querier.startup_query_interval         = PTIN_IGMP_DEFAULT_STARTUPQUERYINTERVAL;
  igmpProxy.querier.startup_query_count            = PTIN_IGMP_DEFAULT_STARTUPQUERYCOUNT;
  igmpProxy.querier.last_member_query_interval     = PTIN_IGMP_DEFAULT_LASTMEMBERQUERYINTERVAL;
  igmpProxy.querier.last_member_query_count        = PTIN_IGMP_DEFAULT_LASTMEMBERQUERYCOUNT;
  igmpProxy.querier.older_host_present_timeout     = PTIN_IGMP_DEFAULT_OLDERHOSTPRESENTTIMEOUT;

  igmpProxy.host.mask                              = 0xFF;
  igmpProxy.host.flags                             = 0;
  igmpProxy.host.robustness                        = PTIN_IGMP_DEFAULT_ROBUSTNESS;
  igmpProxy.host.unsolicited_report_interval       = PTIN_IGMP_DEFAULT_UNSOLICITEDREPORTINTERVAL;
  igmpProxy.host.older_querier_present_timeout     = PTIN_IGMP_DEFAULT_OLDERQUERIERPRESENTTIMEOUT;
  igmpProxy.host.max_records_per_report            = PTIN_IGMP_DEFAULT_MAX_RECORDS_PER_REPORT;
  igmpProxy.host.tos_rtr_alert_check               = PTIN_MGMD_TOS_RTR_ALERT_CHECK;

  /* Apply default config */
  rc = ptin_mgmd_igmp_proxy_config_set(&igmpProxy);
  if (rc != SUCCESS)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "IGMP default config failed to be load");
    return FAILURE;
  }

  return SUCCESS;
}

/**
 * Validate IGMP Proxy configuration
 * 
 * @param igmpProxy Structure with config parameters
 * 
 * @return L7_RC_t SUCCESS/FAILURE
 */
RC_t ptin_mgmd_igmp_proxy_config_validate(ptin_IgmpProxyCfg_t *igmpProxy)
{
  ptin_mgmd_cb_t *pMgmdCB;

  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Verifying new config to IGMP Proxy...");

  /* Get Mgmd Control Block */
  if ((pMgmdCB = mgmdCBGet(PTIN_MGMD_AF_INET)) == PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Error getting pMgmdCB");
    return FAILURE;
  }

  /* *******************
   * IGMP general config
   * *******************/
  /* Admin state */
  if (igmpProxy->mask & PTIN_IGMP_PROXY_MASK_ADMIN && mgmdProxyCfg.admin != igmpProxy->admin)
  {    
    if(igmpProxy->admin!=PTIN_MGMD_ENABLE && igmpProxy->admin!=PTIN_MGMD_DISABLE)
    {
      PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "  Invalid Admin Value:                                   %u", mgmdProxyCfg.admin);
      return FAILURE;
    }    
  }

  /* Class-Of-Service (COS) */
  if (igmpProxy->mask & PTIN_IGMP_PROXY_MASK_COS && mgmdProxyCfg.igmp_cos != igmpProxy->igmp_cos)
  {
    if ( igmpProxy->igmp_cos > PTIN_IGMP_COS_MAX) 
    {
      PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "  Invalid CoS Value:                       %u", mgmdProxyCfg.igmp_cos);
      return FAILURE;
    } 
  }


  /* *******************
   * IGMP Querier config
   * *******************/
  /* Querier Robustness */
  if (igmpProxy->querier.mask & PTIN_IGMP_QUERIER_MASK_RV && mgmdProxyCfg.querier.robustness != igmpProxy->querier.robustness)
  {
     if(igmpProxy->querier.robustness < PTIN_MIN_ROBUSTNESS_VARIABLE || igmpProxy->querier.robustness > PTIN_MAX_ROBUSTNESS_VARIABLE)
     {
       PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid Robustness Variable :%u", igmpProxy->querier.robustness);
       return FAILURE;
     }
  }

  /* Query Interval */
  if (igmpProxy->querier.mask & PTIN_IGMP_QUERIER_MASK_QI && mgmdProxyCfg.querier.query_interval != igmpProxy->querier.query_interval)
  {
    if (igmpProxy->querier.query_interval < PTIN_IGMP_MIN_QUERYINTERVAL ||
        ((mgmdProxyCfg.networkVersion == PTIN_IGMP_VERSION_2 && igmpProxy->querier.query_interval > PTIN_IGMPv2_MAX_QUERYINTERVAL) ||
        (mgmdProxyCfg.networkVersion == PTIN_IGMP_VERSION_3 && igmpProxy->querier.query_interval > PTIN_IGMPv3_MAX_QUERYINTERVAL)))
    {
      PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid Query Interval:%u", igmpProxy->querier.query_interval);    
      return FAILURE;
    }
  }

  /* Query Response Interval */
  if (igmpProxy->querier.mask & PTIN_IGMP_QUERIER_MASK_QRI && mgmdProxyCfg.querier.query_response_interval != igmpProxy->querier.query_response_interval)
  {
    if (igmpProxy->querier.query_response_interval < PTIN_IGMP_MIN_QUERYRESPONSEINTERVAL ||
        ((mgmdProxyCfg.networkVersion == PTIN_IGMP_VERSION_2 && igmpProxy->querier.query_response_interval > PTIN_IGMPv2_MAX_QUERYRESPONSEINTERVAL) ||
        (mgmdProxyCfg.networkVersion == PTIN_IGMP_VERSION_3 && igmpProxy->querier.query_response_interval > PTIN_IGMPv3_MAX_QUERYRESPONSEINTERVAL)))
    {    
      PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid Query Response Interval:%u", igmpProxy->querier.query_response_interval);
      return FAILURE;
    }
  }

  /* *******************
   * IGMP Host config
   * *******************/
  /* Host Robustness */
  if (igmpProxy->host.mask & PTIN_IGMP_HOST_MASK_RV && mgmdProxyCfg.host.robustness != igmpProxy->host.robustness)
  {
    if (igmpProxy->host.robustness < PTIN_MIN_ROBUSTNESS_VARIABLE || igmpProxy->host.robustness > PTIN_MAX_ROBUSTNESS_VARIABLE)
    {
      PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid Robustness Variable value:%u, going to use existing value:%u", igmpProxy->host.robustness, mgmdProxyCfg.host.robustness);
      return FAILURE;
    }
  }

  /* Unsolicited Report Interval */
  if (igmpProxy->host.mask & PTIN_IGMP_HOST_MASK_URI && mgmdProxyCfg.host.unsolicited_report_interval != igmpProxy->host.unsolicited_report_interval)
  {
    if (igmpProxy->host.unsolicited_report_interval < PTIN_IGMP_MIN_UNSOLICITEDREPORTINTERVAL || igmpProxy->host.unsolicited_report_interval > PTIN_IGMP_MAX_UNSOLICITEDREPORTINTERVAL)
    {
      PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid Unsolicited Report Interval configured: %u, going to use existing value :%u", igmpProxy->host.unsolicited_report_interval, mgmdProxyCfg.host.unsolicited_report_interval);
      return FAILURE;
    }
  }

  /* Max Records per Report */
  if (igmpProxy->host.mask & PTIN_IGMP_HOST_MASK_MRPR && mgmdProxyCfg.host.max_records_per_report != igmpProxy->host.max_records_per_report)
  {
    if (igmpProxy->host.max_records_per_report < PTIN_IGMP_MIN_RECORDS_PER_REPORT || igmpProxy->host.max_records_per_report > PTIN_IGMP_MAX_RECORDS_PER_REPORT)
    {
      PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid Max Records per Report Value: %u, going to use existing value:%u", igmpProxy->host.max_records_per_report, mgmdProxyCfg.host.max_records_per_report);
      return FAILURE;
    }
  }
  
  return SUCCESS;
}


/**
 * Applies IGMP Proxy configuration
 * 
 * @param igmpProxy Structure with config parameters
 * 
 * @return L7_RC_t SUCCESS/FAILURE
 */
RC_t ptin_mgmd_igmp_proxy_config_set(ptin_IgmpProxyCfg_t *igmpProxy)
{
  ptin_mgmd_externalapi_t  externalApi;
  ptin_mgmd_cb_t          *pMgmdCB;

  /* Ensure the requested configurations are valid. No other validations are performed later in this method */
  if(SUCCESS != ptin_mgmd_igmp_proxy_config_validate(igmpProxy))
  {
    return FAILURE;
  }

  /* Get MGMD external API */
  if(SUCCESS != ptin_mgmd_externalapi_get(&externalApi))
  {
    return FAILURE;
  }

  /* Get Mgmd Control Block */
  if ((pMgmdCB = mgmdCBGet(PTIN_MGMD_AF_INET)) == PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Error getting pMgmdCB");
    return FAILURE;
  }


  /* *******************
   * IGMP general config
   * *******************/
  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Applying new config to IGMP Proxy...");

  /* Output admin state, but only apply changes in the end... */
  if (igmpProxy->mask & PTIN_IGMP_PROXY_MASK_ADMIN
      && mgmdProxyCfg.admin != igmpProxy->admin)
  { 
    mgmdProxyCfg.admin=igmpProxy->admin;   
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "  Admin:                                   %s", mgmdProxyCfg.admin != 0 ? "ON" : "OFF");     
   
    if(mgmdProxyCfg.admin==PTIN_MGMD_ENABLE)
    {
      //Reset All Statistics
      ptin_mgmd_statistics_reset_all();

      //Open Ports for Static Groups Pre-Configured
      ptinMgmdStaticGroupPortOpen();

      //Start All Existing General Queries
      ptinMgmdStartAllGeneralQuery();      
    }
    else
    {
      //Stop All Existing General Queries
      ptinMgmdGeneralQueryStopAll();

      //Clean All Group Specific Query Entries
      ptinMgmdGroupSpecificQueriesRemoveAll();

#if 0//Once the admin mode is just for enabling the trap of packets  it does not make sense to remove all existing entries group entries
      //Clean All Group Entries
      ptinMgmdGroupRemoveAll();       
#else 
      //Remove Only Dynamic Entries
      ptinMgmdStaticOrDynamicGroupRemoveAll(FALSE);
#endif
      
      //Clean All Group Record Entries
      ptinMgmdGroupRecordRemoveAll();      
    }

    externalApi.igmp_admin_set(mgmdProxyCfg.admin); 
  }

  /* White-list mode */
  if ( (igmpProxy->mask & PTIN_IGMP_PROXY_MASK_WHITELIST) && (mgmdProxyCfg.whitelist != igmpProxy->whitelist) )
  {
    mgmdProxyCfg.whitelist = igmpProxy->whitelist;
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "  White-List mode:                         %s", mgmdProxyCfg.whitelist != 0 ? "ON" : "OFF");    
  }

  /* Network Version */
  if (igmpProxy->mask & PTIN_IGMP_PROXY_MASK_NETWORKVERSION && mgmdProxyCfg.networkVersion != igmpProxy->networkVersion)
  {    
    mgmdProxyCfg.networkVersion = igmpProxy->networkVersion;
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "  IGMP Network Version:                     %u", mgmdProxyCfg.networkVersion);   

    uint32 i;   
    for(i=0; i<PTIN_MGMD_MAX_SERVICES; ++i)
    {
     if (ptin_mgmd_loop_trace) 
       PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Iterating over i:%u | PTIN_MGMD_MAX_SERVICES:%u", i, PTIN_MGMD_MAX_SERVICES);

     pMgmdCB->proxyCM[i].compatibilityMode = igmpProxy->networkVersion;
    }
    
  }

  /* Client Version */
  if (igmpProxy->mask & PTIN_IGMP_PROXY_MASK_CLIENTVERSION && mgmdProxyCfg.clientVersion != igmpProxy->clientVersion)
  {
    mgmdProxyCfg.clientVersion = igmpProxy->clientVersion;
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "  IGMP Client Version:                      %u", mgmdProxyCfg.clientVersion);
  }

  /* Proxy IP */
  if (igmpProxy->mask & PTIN_IGMP_PROXY_MASK_IPV4 && mgmdProxyCfg.ipv4_addr != igmpProxy->ipv4_addr)
  {    
    mgmdProxyCfg.ipv4_addr = igmpProxy->ipv4_addr;
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "  IPv4:                                    %u.%u.%u.%u",
              (mgmdProxyCfg.ipv4_addr >> 24) & 0xFF, (mgmdProxyCfg.ipv4_addr >> 16) & 0xFF,
              (mgmdProxyCfg.ipv4_addr >>  8) & 0xFF,  mgmdProxyCfg.ipv4_addr        & 0xFF);
  }

  /* Class-Of-Service (COS) */
  if (igmpProxy->mask & PTIN_IGMP_PROXY_MASK_COS && mgmdProxyCfg.igmp_cos != igmpProxy->igmp_cos)
  {    
    mgmdProxyCfg.igmp_cos = igmpProxy->igmp_cos;
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "  IGMP COS:                                %u", mgmdProxyCfg.igmp_cos);
    externalApi.cos_set(mgmdProxyCfg.igmp_cos); 
  }

  /* Fast-Leave mode */
  if (igmpProxy->mask & PTIN_IGMP_PROXY_MASK_FASTLEAVE && mgmdProxyCfg.fast_leave != igmpProxy->fast_leave)
  {    
    mgmdProxyCfg.fast_leave = igmpProxy->fast_leave;
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "  Fast-Leave mode:                         %s", mgmdProxyCfg.fast_leave != 0 ? "ON" : "OFF");
  }

  /* Router Alert Check */
  if (igmpProxy->mask & PTIN_IGMP_HOST_MASK_RTR_ALERT && mgmdProxyCfg.host.tos_rtr_alert_check != igmpProxy->host.tos_rtr_alert_check)
  {    
    mgmdProxyCfg.host.tos_rtr_alert_check = igmpProxy->host.tos_rtr_alert_check;
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "  Router Alert Check:                         %s", mgmdProxyCfg.host.tos_rtr_alert_check != 0 ? "True" : "False");
  }


  /* *******************
   * IGMP Querier config
   * *******************/
  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "  Querier config:");

  /* Querier Robustness */
  if (igmpProxy->querier.mask & PTIN_IGMP_QUERIER_MASK_RV && mgmdProxyCfg.querier.robustness != igmpProxy->querier.robustness)
  {    
    mgmdProxyCfg.querier.robustness = igmpProxy->querier.robustness;
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "    Robustness:                            %u", mgmdProxyCfg.querier.robustness);
  }

  /* Query Interval */
  if (igmpProxy->querier.mask & PTIN_IGMP_QUERIER_MASK_QI && mgmdProxyCfg.querier.query_interval != igmpProxy->querier.query_interval)
  {    
    //Save Query Interval
    uint16 old_query_interval=mgmdProxyCfg.querier.query_interval;

    mgmdProxyCfg.querier.query_interval = igmpProxy->querier.query_interval;        
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "    Query Interval:                        %u (s)", mgmdProxyCfg.querier.query_interval);
    
    if(old_query_interval > mgmdProxyCfg.querier.query_interval &&  mgmdProxyCfg.admin==PTIN_MGMD_ENABLE)
    {
      //Restart all General Queries with a different Query Interval
      ptinMgmdReStartAllGeneralQuery(); 
    }
  }

  /* Query Response Interval */
  if (igmpProxy->querier.mask & PTIN_IGMP_QUERIER_MASK_QRI && mgmdProxyCfg.querier.query_response_interval != igmpProxy->querier.query_response_interval)
  {    
    mgmdProxyCfg.querier.query_response_interval = igmpProxy->querier.query_response_interval;
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "    Query Response Interval:               %u (1/10s)", mgmdProxyCfg.querier.query_response_interval);
  }

  /* Group Membership Interval */
  if (igmpProxy->querier.flags & PTIN_IGMP_QUERIER_MASK_AUTO_GMI)
  {    
    mgmdProxyCfg.querier.group_membership_interval = PTIN_IGMP_AUTO_GMI(mgmdProxyCfg.querier.robustness,
                                                                        mgmdProxyCfg.querier.query_interval,
                                                                        mgmdProxyCfg.querier.query_response_interval);
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "    Group Membership Interval (AUTO):      %u (s)", mgmdProxyCfg.querier.group_membership_interval);
  }
  else if (igmpProxy->querier.mask & PTIN_IGMP_QUERIER_MASK_GMI && mgmdProxyCfg.querier.group_membership_interval != igmpProxy->querier.group_membership_interval)
  {    
    mgmdProxyCfg.querier.group_membership_interval = igmpProxy->querier.group_membership_interval;
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "    Group Membership Interval:             %u (s)", mgmdProxyCfg.querier.group_membership_interval);
  }

  /* Other Querier Present Interval */
  if (igmpProxy->querier.flags & PTIN_IGMP_QUERIER_MASK_AUTO_OQPI)
  {    
    mgmdProxyCfg.querier.other_querier_present_interval = PTIN_IGMP_AUTO_OQPI(mgmdProxyCfg.querier.robustness,
                                                                              mgmdProxyCfg.querier.query_interval,
                                                                              mgmdProxyCfg.querier.query_response_interval);
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "    Other Querier Present Interval (AUTO): %u (s)", mgmdProxyCfg.querier.other_querier_present_interval);
  }
  else if (igmpProxy->querier.mask & PTIN_IGMP_QUERIER_MASK_OQPI && mgmdProxyCfg.querier.other_querier_present_interval != igmpProxy->querier.other_querier_present_interval)
  {    
    mgmdProxyCfg.querier.other_querier_present_interval = igmpProxy->querier.other_querier_present_interval;
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "    Other Querier Present Interval:        %u (s)", mgmdProxyCfg.querier.other_querier_present_interval);
  }

  /* Startup Query Interval */
  /*if (igmpProxy->querier.flags & PTIN_IGMP_QUERIER_MASK_AUTO_SQI)
  {    
    mgmdProxyCfg.querier.startup_query_interval = PTIN_IGMP_AUTO_SQI(mgmdProxyCfg.querier.query_interval);
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "    Startup Query Interval (AUTO):         %u (s)", mgmdProxyCfg.querier.startup_query_interval);
  }
  else*/ if (igmpProxy->querier.mask & PTIN_IGMP_QUERIER_MASK_SQI && mgmdProxyCfg.querier.startup_query_interval != igmpProxy->querier.startup_query_interval)
  {    
    mgmdProxyCfg.querier.startup_query_interval = igmpProxy->querier.startup_query_interval;
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "    Startup Query Interval:                %u (s)", mgmdProxyCfg.querier.startup_query_interval);
  }

  /* Startup Query Count */
  /*if (igmpProxy->querier.flags & PTIN_IGMP_QUERIER_MASK_AUTO_SQC)
  {    
    mgmdProxyCfg.querier.startup_query_count = PTIN_IGMP_AUTO_SQC(mgmdProxyCfg.querier.robustness);
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "    Startup Query Count (AUTO):            %u (s)", mgmdProxyCfg.querier.startup_query_count);
  }
  else*/ if (igmpProxy->querier.mask & PTIN_IGMP_QUERIER_MASK_SQC && mgmdProxyCfg.querier.startup_query_count != igmpProxy->querier.startup_query_count)
  {    
    mgmdProxyCfg.querier.startup_query_count = igmpProxy->querier.startup_query_count;
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "    Startup Query Count:                   %u (s)", mgmdProxyCfg.querier.startup_query_count);
  }

  /* Last Member Query Interval */
  if (igmpProxy->querier.mask & PTIN_IGMP_QUERIER_MASK_LMQI && mgmdProxyCfg.querier.last_member_query_interval != igmpProxy->querier.last_member_query_interval)
  {    
    PTIN_MGMD_TIMER_CB_t timerCB;

    mgmdProxyCfg.querier.last_member_query_interval = igmpProxy->querier.last_member_query_interval;
    ptin_mgmd_grouptimer_CB_get(&timerCB);
    ptin_mgmd_timer_controlblock_optThr_set(timerCB, 2*(mgmdProxyCfg.querier.last_member_query_interval*mgmdProxyCfg.querier.last_member_query_count)*100);
    ptin_mgmd_sourcetimer_CB_get(&timerCB);
    ptin_mgmd_timer_controlblock_optThr_set(timerCB, 2*(mgmdProxyCfg.querier.last_member_query_interval*mgmdProxyCfg.querier.last_member_query_count)*100);
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "    Last Member Query Interval:            %u (1/10s)", mgmdProxyCfg.querier.last_member_query_interval);
  }

  /* Last Member Query Count */
  /*if (igmpProxy->querier.flags & PTIN_IGMP_QUERIER_MASK_AUTO_LMQC)
  {    
    mgmdProxyCfg.querier.last_member_query_count = PTIN_IGMP_AUTO_LMQC(mgmdProxyCfg.querier.robustness);
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "    Last Member Query Count (AUTO):        %u (s)", mgmdProxyCfg.querier.last_member_query_count);
  }
  else */if (igmpProxy->querier.mask & PTIN_IGMP_QUERIER_MASK_LMQC && mgmdProxyCfg.querier.last_member_query_count != igmpProxy->querier.last_member_query_count)
  {    
    PTIN_MGMD_TIMER_CB_t timerCB;
    mgmdProxyCfg.querier.last_member_query_count = igmpProxy->querier.last_member_query_count;
    ptin_mgmd_grouptimer_CB_get(&timerCB);
    ptin_mgmd_timer_controlblock_optThr_set(timerCB, 2*(mgmdProxyCfg.querier.last_member_query_interval*mgmdProxyCfg.querier.last_member_query_count)*100);
    ptin_mgmd_sourcetimer_CB_get(&timerCB);
    ptin_mgmd_timer_controlblock_optThr_set(timerCB, 2*(mgmdProxyCfg.querier.last_member_query_interval*mgmdProxyCfg.querier.last_member_query_count)*100);
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "    Last Member Query Count:               %u (s)", mgmdProxyCfg.querier.last_member_query_count);
  }

  /* Older Host Present Timeout */
  if (igmpProxy->querier.flags & PTIN_IGMP_QUERIER_MASK_AUTO_OHPT)
  {    
    mgmdProxyCfg.querier.older_host_present_timeout = PTIN_IGMP_AUTO_OHPT(mgmdProxyCfg.querier.robustness,
                                                                          mgmdProxyCfg.querier.query_interval,
                                                                          mgmdProxyCfg.querier.query_response_interval);
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "    Older Host Present Timeout (AUTO):     %u (s)", mgmdProxyCfg.querier.older_host_present_timeout);
  }
  else if (igmpProxy->querier.mask & PTIN_IGMP_QUERIER_MASK_LMQC && mgmdProxyCfg.querier.older_host_present_timeout != igmpProxy->querier.older_host_present_timeout)
  {  
    mgmdProxyCfg.querier.older_host_present_timeout = igmpProxy->querier.older_host_present_timeout;
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "    Older Host Present Timeout:            %u (s)", mgmdProxyCfg.querier.older_host_present_timeout);
  }

  /* *******************
   * IGMP Host config
   * *******************/
  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "  Host config:");

  /* Host Robustness */
  if (igmpProxy->host.mask & PTIN_IGMP_HOST_MASK_RV && mgmdProxyCfg.host.robustness != igmpProxy->host.robustness)
  {    
    mgmdProxyCfg.host.robustness = igmpProxy->host.robustness;
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "    Robustness:                            %u", mgmdProxyCfg.host.robustness);

    //Once this parameter is currently not configured on the Manager, we use the parameter configured for the host
    mgmdProxyCfg.querier.robustness = igmpProxy->host.robustness;
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "    Robustness:                            %u", mgmdProxyCfg.querier.robustness);
  }

  /* Unsolicited Report Interval */
  if (igmpProxy->host.mask & PTIN_IGMP_HOST_MASK_URI && mgmdProxyCfg.host.unsolicited_report_interval != igmpProxy->host.unsolicited_report_interval)
  {    
    mgmdProxyCfg.host.unsolicited_report_interval = igmpProxy->host.unsolicited_report_interval;
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "    Unsolicited Report Interval:           %u (s)", mgmdProxyCfg.host.unsolicited_report_interval);
  }

  /* Older Querier Present Timeout */
  if (igmpProxy->host.flags & PTIN_IGMP_QUERIER_MASK_AUTO_OHPT)
  {    
    mgmdProxyCfg.host.older_querier_present_timeout = PTIN_IGMP_AUTO_OQPT(mgmdProxyCfg.host.robustness,
                                                                          mgmdProxyCfg.querier.query_interval,
                                                                          mgmdProxyCfg.querier.query_response_interval);
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "    Older Querier Present Timeout (AUTO):  %u (s)", mgmdProxyCfg.host.older_querier_present_timeout);
  }
  else if (igmpProxy->host.mask & PTIN_IGMP_HOST_MASK_OQPT && mgmdProxyCfg.host.older_querier_present_timeout != igmpProxy->host.older_querier_present_timeout)
  {    
    mgmdProxyCfg.host.older_querier_present_timeout = igmpProxy->host.older_querier_present_timeout;
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "    Older Querier Present Timeout:         %u (s)", mgmdProxyCfg.host.older_querier_present_timeout);
  }

  /* Max Records per Report */
  if (igmpProxy->host.mask & PTIN_IGMP_HOST_MASK_MRPR && mgmdProxyCfg.host.max_records_per_report != igmpProxy->host.max_records_per_report)
  {    
    mgmdProxyCfg.host.max_records_per_report = igmpProxy->host.max_records_per_report;
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "    Max Records per Report:                %u ", mgmdProxyCfg.host.max_records_per_report);
  }

  /* Update AUTO flags */
  mgmdProxyCfg.querier.flags = igmpProxy->querier.flags;
  mgmdProxyCfg.host.flags    = igmpProxy->host.flags;

  /* Finally, (de)activate IGMP module */
  if (igmpProxy->mask & PTIN_IGMP_PROXY_MASK_ADMIN && mgmdProxyCfg.admin != igmpProxy->admin)
  {
    mgmdProxyCfg.admin = igmpProxy->admin;
  }
  
  return SUCCESS;
}



/**
 * Gets IGMP Proxy configuration
 * 
 * @param igmpProxy Structure with config parameters
 * 
 * @return L7_RC_t SUCCESS/FAILURE
 */
RC_t ptin_mgmd_igmp_proxy_config_get(ptin_IgmpProxyCfg_t *igmpProxy)
{
  if(PTIN_NULLPTR == igmpProxy)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid parameters [igmpProxy=%p]", igmpProxy);
    return FAILURE;
  }

  *igmpProxy = mgmdProxyCfg;

  igmpProxy->mask         = 0xFF;
  igmpProxy->querier.mask = 0xFFFF;
  igmpProxy->host.mask    = 0xFF;

  if(igmpProxy->networkVersion == PTIN_IGMP_VERSION_2 && igmpProxy->querier.query_response_interval>PTIN_IGMPv2_MAX_QUERYRESPONSEINTERVAL)
  {
    PTIN_MGMD_LOG_NOTICE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid Query Response Interval when Operating in IGMPv2[%u]. Using Instead [%u]", igmpProxy->querier.query_response_interval, PTIN_IGMPv2_MAX_QUERYRESPONSEINTERVAL);
    igmpProxy->querier.query_response_interval = PTIN_IGMPv2_MAX_QUERYRESPONSEINTERVAL;
  }

  return SUCCESS;
}




