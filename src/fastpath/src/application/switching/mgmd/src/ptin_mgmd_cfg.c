/*********************************************************************
*
* (C) Copyright PT Inovação S.A. 2013-2013
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
* @author    Márcio Melo (marcio-d-melo@ptinovacao.pt)
* @end
*
**********************************************************************/


#include "ptin_mgmd_cfg.h"
#include "logger.h"
#include "ptin_mgmd_cfg_api.h"
#include "snooping.h"

ptin_IgmpProxyCfg_t     igmpProxyCfg;
ptin_mgmd_externalapi_t ptin_mgmd_externalapi = {PTIN_NULLPTR};


/**
 * Configure the external API.
 * 
 * @param externalApi
 * 
 * @return RC_t
 */
RC_t ptin_mgmd_externalapi_set(ptin_mgmd_externalapi_t* externalApi)
{
  ptin_mgmd_externalapi.igmp_admin_set = externalApi->igmp_admin_set;
  ptin_mgmd_externalapi.mld_admin_set  = externalApi->mld_admin_set;

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
 * Applies IGMP Proxy configuration
 * 
 * @param igmpProxy Structure with config parameters
 * 
 * @return L7_RC_t SUCCESS/FAILURE
 */
RC_t ptin_mgmd_igmp_proxy_config_set(ptin_IgmpProxyCfg_t *igmpProxy)
{
  ptin_mgmd_externalapi_t externalApi;

  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Applying new config to IGMP Proxy...");

  if(SUCCESS != ptin_mgmd_externalapi_get(&externalApi))
  {
    return FAILURE;
  }

  /* *******************
   * IGMP general config
   * *******************/
  /* Output admin state, but only apply changes in the end... */
  if (igmpProxy->mask & PTIN_IGMP_PROXY_MASK_ADMIN
      && igmpProxyCfg.admin != igmpProxy->admin)
  {    
    if(igmpProxy->admin==PTIN_MGMD_ENABLE || igmpProxy->admin==PTIN_MGMD_DISABLE)
    {
      PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "  Admin:                                   %s", igmpProxyCfg.admin != 0 ? "ON" : "OFF");     
      igmpProxyCfg.admin=igmpProxy->admin;
      externalApi.igmp_admin_set(igmpProxyCfg.admin); 
    }
    else
    {
      PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "  Invalid Admin Value:                                   %u", igmpProxyCfg.admin);
    }    
  }

  /* White-list mode */
  if ( (igmpProxy->mask & PTIN_IGMP_PROXY_MASK_WHITELIST) && (igmpProxyCfg.whitelist != igmpProxy->whitelist) )
  {
    igmpProxyCfg.whitelist = igmpProxy->whitelist;
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "  White-List mode:                         %s", igmpProxyCfg.whitelist != 0 ? "ON" : "OFF");
  }

  /* Network Version */
  if (igmpProxy->mask & PTIN_IGMP_PROXY_MASK_NETWORKVERSION
      && igmpProxyCfg.networkVersion != igmpProxy->networkVersion)
  {
    igmpProxyCfg.networkVersion = igmpProxy->networkVersion;
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "  IGMP Network Version:                     %u", igmpProxyCfg.networkVersion);
  }

  /* Client Version */
  if (igmpProxy->mask & PTIN_IGMP_PROXY_MASK_CLIENTVERSION
      && igmpProxyCfg.clientVersion != igmpProxy->clientVersion)
  {
    igmpProxyCfg.clientVersion = igmpProxy->clientVersion;
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "  IGMP Client Version:                      %u", igmpProxyCfg.clientVersion);
  }

  /* Proxy IP */
  if (igmpProxy->mask & PTIN_IGMP_PROXY_MASK_IPV4
      && igmpProxyCfg.ipv4_addr != igmpProxy->ipv4_addr)
  {
    igmpProxyCfg.ipv4_addr = igmpProxy->ipv4_addr;
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "  IPv4:                                    %u.%u.%u.%u",
              (igmpProxyCfg.ipv4_addr >> 24) & 0xFF, (igmpProxyCfg.ipv4_addr >> 16) & 0xFF,
              (igmpProxyCfg.ipv4_addr >>  8) & 0xFF,  igmpProxyCfg.ipv4_addr        & 0xFF);
  }

  /* Class-Of-Service (COS) */
  if (igmpProxy->mask & PTIN_IGMP_PROXY_MASK_COS
      && igmpProxyCfg.igmp_cos != igmpProxy->igmp_cos)
  {
    igmpProxyCfg.igmp_cos = igmpProxy->igmp_cos;
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "  IGMP COS:                                %u", igmpProxyCfg.igmp_cos);
  }

  /* Fast-Leave mode */
  if (igmpProxy->mask & PTIN_IGMP_PROXY_MASK_FASTLEAVE
      && igmpProxyCfg.fast_leave != igmpProxy->fast_leave)
  {
    igmpProxyCfg.fast_leave = igmpProxy->fast_leave;
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "  Fast-Leave mode:                         %s", igmpProxyCfg.fast_leave != 0 ? "ON" : "OFF");
  }

  /* Router Alert Check */
  if (igmpProxy->mask & PTIN_IGMP_HOST_MASK_RTR_ALERT
      && igmpProxyCfg.host.tos_rtr_alert_check != igmpProxy->host.tos_rtr_alert_check)
  {
    igmpProxyCfg.host.tos_rtr_alert_check = igmpProxy->host.tos_rtr_alert_check;
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "  Router Alert Check:                         %s", igmpProxyCfg.host.tos_rtr_alert_check != 0 ? "True" : "False");
  }


  /* *******************
   * IGMP Querier config
   * *******************/
  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "  Querier config:");

  /* Querier Robustness */
  if (igmpProxy->querier.mask & PTIN_IGMP_QUERIER_MASK_RV
      && igmpProxyCfg.querier.robustness != igmpProxy->querier.robustness)
  {
     if(igmpProxy->querier.robustness >= PTIN_MIN_ROBUSTNESS_VARIABLE && igmpProxy->querier.robustness <= PTIN_MAX_ROBUSTNESS_VARIABLE)
     {
      igmpProxyCfg.querier.robustness = igmpProxy->querier.robustness;
      PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "    Robustness:                            %u", igmpProxyCfg.querier.robustness);
     }
     else
     {
       PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid Robustness Variable :%u", igmpProxy->querier.robustness);
     }
  }


  /* Query Interval */
  if (igmpProxy->querier.mask & PTIN_IGMP_QUERIER_MASK_QI
      && igmpProxyCfg.querier.query_interval != igmpProxy->querier.query_interval)
  {
    if (igmpProxy->querier.query_interval >= PTIN_IGMP_MIN_QUERYINTERVAL &&
        ((igmpProxyCfg.networkVersion == PTIN_IGMP_VERSION_2 && igmpProxy->querier.query_interval <= PTIN_IGMPv2_MAX_QUERYINTERVAL) ||
        (igmpProxyCfg.networkVersion == PTIN_IGMP_VERSION_3 && igmpProxy->querier.query_interval <= PTIN_IGMPv3_MAX_QUERYINTERVAL)))
    {  
#if 0 //We have moved the fp encoding to when the general is required to be sent.         
      if (igmpProxyCfg.networkVersion == PTIN_IGMP_VERSION_3 && igmpProxy->querier.query_interval >= 128)
      {
        snoop_fp_encode(AF_INET, igmpProxy->querier.query_interval, &igmpProxyCfg.querier.query_interval);        
      }
      else
#endif
      {
        igmpProxyCfg.querier.query_interval = igmpProxy->querier.query_interval;        
      }
      PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "    Query Interval:                        %u (s)", igmpProxyCfg.querier.query_interval);
    }
    else
    {
      PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid Query Interval:%u", igmpProxy->querier.query_interval);    
    }
  }

  /* Query Response Interval */
  if (igmpProxy->querier.mask & PTIN_IGMP_QUERIER_MASK_QRI
      && igmpProxyCfg.querier.query_response_interval != igmpProxy->querier.query_response_interval)
  {
    if (igmpProxy->querier.query_response_interval >= PTIN_IGMP_MIN_QUERYRESPONSEINTERVAL &&
        ((igmpProxyCfg.networkVersion == PTIN_IGMP_VERSION_2 && igmpProxy->querier.query_response_interval <= PTIN_IGMPv2_MAX_QUERYRESPONSEINTERVAL) ||
        (igmpProxyCfg.networkVersion == PTIN_IGMP_VERSION_3 && igmpProxy->querier.query_response_interval <= PTIN_IGMPv3_MAX_QUERYRESPONSEINTERVAL)))
    {
      igmpProxyCfg.querier.query_response_interval = igmpProxy->querier.query_response_interval;
      PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "    Query Response Interval:               %u (1/10s)", igmpProxyCfg.querier.query_response_interval);
    }
    else
    {    
      PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid Query Response Interval:%u", igmpProxy->querier.query_response_interval);
    }
  }

  /* Group Membership Interval */
  if (igmpProxy->querier.flags & PTIN_IGMP_QUERIER_MASK_AUTO_GMI)
  {
    igmpProxyCfg.querier.group_membership_interval = PTIN_IGMP_AUTO_GMI(igmpProxyCfg.querier.robustness,
                                                                        igmpProxyCfg.querier.query_interval,
                                                                        igmpProxyCfg.querier.query_response_interval);
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "    Group Membership Interval (AUTO):      %u (s)", igmpProxyCfg.querier.group_membership_interval);
  }
  else if (igmpProxy->querier.mask & PTIN_IGMP_QUERIER_MASK_GMI
      && igmpProxyCfg.querier.group_membership_interval != igmpProxy->querier.group_membership_interval)
  {
    igmpProxyCfg.querier.group_membership_interval = igmpProxy->querier.group_membership_interval;
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "    Group Membership Interval:             %u (s)", igmpProxyCfg.querier.group_membership_interval);
  }

  /* Other Querier Present Interval */
  if (igmpProxy->querier.flags & PTIN_IGMP_QUERIER_MASK_AUTO_OQPI)
  {
    igmpProxyCfg.querier.other_querier_present_interval = PTIN_IGMP_AUTO_OQPI(igmpProxyCfg.querier.robustness,
                                                                              igmpProxyCfg.querier.query_interval,
                                                                              igmpProxyCfg.querier.query_response_interval);
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "    Other Querier Present Interval (AUTO): %u (s)", igmpProxyCfg.querier.other_querier_present_interval);
  }
  else if (igmpProxy->querier.mask & PTIN_IGMP_QUERIER_MASK_OQPI
      && igmpProxyCfg.querier.other_querier_present_interval != igmpProxy->querier.other_querier_present_interval)
  {
    igmpProxyCfg.querier.other_querier_present_interval = igmpProxy->querier.other_querier_present_interval;
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "    Other Querier Present Interval:        %u (s)", igmpProxyCfg.querier.other_querier_present_interval);
  }

  /* Startup Query Interval */
  if (igmpProxy->querier.flags & PTIN_IGMP_QUERIER_MASK_AUTO_SQI)
  {
    igmpProxyCfg.querier.startup_query_interval = PTIN_IGMP_AUTO_SQI(igmpProxyCfg.querier.query_interval);
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "    Startup Query Interval (AUTO):         %u (s)", igmpProxyCfg.querier.startup_query_interval);
  }
  else if (igmpProxy->querier.mask & PTIN_IGMP_QUERIER_MASK_SQI
      && igmpProxyCfg.querier.startup_query_interval != igmpProxy->querier.startup_query_interval)
  {
    igmpProxyCfg.querier.startup_query_interval = igmpProxy->querier.startup_query_interval;
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "    Startup Query Interval:                %u (s)", igmpProxyCfg.querier.startup_query_interval);
  }

  /* Startup Query Count */
  if (igmpProxy->querier.flags & PTIN_IGMP_QUERIER_MASK_AUTO_SQC)
  {
    igmpProxyCfg.querier.startup_query_count = PTIN_IGMP_AUTO_SQC(igmpProxyCfg.querier.robustness);
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "    Startup Query Count (AUTO):            %u (s)", igmpProxyCfg.querier.startup_query_count);
  }
  else if (igmpProxy->querier.mask & PTIN_IGMP_QUERIER_MASK_SQC
      && igmpProxyCfg.querier.startup_query_count != igmpProxy->querier.startup_query_count)
  {
    igmpProxyCfg.querier.startup_query_count = igmpProxy->querier.startup_query_count;
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "    Startup Query Count:                   %u (s)", igmpProxyCfg.querier.startup_query_count);
  }

  /* Last Member Query Interval */
  if (igmpProxy->querier.mask & PTIN_IGMP_QUERIER_MASK_LMQI
      && igmpProxyCfg.querier.last_member_query_interval != igmpProxy->querier.last_member_query_interval)
  {
    igmpProxyCfg.querier.last_member_query_interval = igmpProxy->querier.last_member_query_interval/10;
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "    Last Member Query Interval:            %u (1/10s)", igmpProxyCfg.querier.last_member_query_interval);
  }

  /* Last Member Query Count */
  if (igmpProxy->querier.flags & PTIN_IGMP_QUERIER_MASK_AUTO_LMQC)
  {
    igmpProxyCfg.querier.last_member_query_count = PTIN_IGMP_AUTO_LMQC(igmpProxyCfg.querier.robustness);
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "    Last Member Query Count (AUTO):        %u (s)", igmpProxyCfg.querier.last_member_query_count);
  }
  else if (igmpProxy->querier.mask & PTIN_IGMP_QUERIER_MASK_LMQC
      && igmpProxyCfg.querier.last_member_query_count != igmpProxy->querier.last_member_query_count)
  {
    igmpProxyCfg.querier.last_member_query_count = igmpProxy->querier.last_member_query_count;
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "    Last Member Query Count:               %u (s)", igmpProxyCfg.querier.last_member_query_count);
  }

  /* Older Host Present Timeout */
  if (igmpProxy->querier.flags & PTIN_IGMP_QUERIER_MASK_AUTO_LMQC)
  {
    igmpProxyCfg.querier.older_host_present_timeout = PTIN_IGMP_AUTO_OHPT(igmpProxyCfg.querier.robustness,
                                                                          igmpProxyCfg.querier.query_interval,
                                                                          igmpProxyCfg.querier.query_response_interval);
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "    Older Host Present Timeout (AUTO):     %u (s)", igmpProxyCfg.querier.older_host_present_timeout);
  }
  else if (igmpProxy->querier.mask & PTIN_IGMP_QUERIER_MASK_LMQC
      && igmpProxyCfg.querier.older_host_present_timeout != igmpProxy->querier.older_host_present_timeout)
  {
    igmpProxyCfg.querier.older_host_present_timeout = igmpProxy->querier.older_host_present_timeout;
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "    Older Host Present Timeout:            %u (s)", igmpProxyCfg.querier.older_host_present_timeout);
  }

  /* *******************
   * IGMP Host config
   * *******************/
  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "  Host config:");

  /* Host Robustness */
  if (igmpProxy->host.mask & PTIN_IGMP_HOST_MASK_RV
      && igmpProxyCfg.host.robustness != igmpProxy->host.robustness)
  {
    if (igmpProxy->host.robustness < PTIN_MIN_ROBUSTNESS_VARIABLE || igmpProxy->host.robustness > PTIN_MAX_ROBUSTNESS_VARIABLE)
    {
      PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid Robustness Variable value:%u, going to use existing value:%u", igmpProxy->host.robustness, igmpProxyCfg.host.robustness);
    }
    else
    {
      igmpProxyCfg.host.robustness = igmpProxy->host.robustness;
      PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "    Robustness:                            %u", igmpProxyCfg.host.robustness);
    }
  }

  /* Unsolicited Report Interval */
  if (igmpProxy->host.mask & PTIN_IGMP_HOST_MASK_URI
      && igmpProxyCfg.host.unsolicited_report_interval != igmpProxy->host.unsolicited_report_interval)
  {
    if (igmpProxy->host.unsolicited_report_interval < PTIN_IGMP_MIN_UNSOLICITEDREPORTINTERVAL || igmpProxy->host.unsolicited_report_interval > PTIN_IGMP_MAX_UNSOLICITEDREPORTINTERVAL)
    {
      PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid Unsolicited Report Interval configured: %u, going to use existing value :%u", igmpProxy->host.unsolicited_report_interval, igmpProxyCfg.host.unsolicited_report_interval);
    }
    else
    {
      igmpProxyCfg.host.unsolicited_report_interval = igmpProxy->host.unsolicited_report_interval;
      PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "    Unsolicited Report Interval:           %u (s)", igmpProxyCfg.host.unsolicited_report_interval);
    }
  }

  /* Older Querier Present Timeout */
  if (igmpProxy->host.flags & PTIN_IGMP_HOST_MASK_OQPT)
  {
    igmpProxyCfg.host.older_querier_present_timeout = PTIN_IGMP_AUTO_OQPT(igmpProxyCfg.host.robustness,
                                                                          igmpProxyCfg.querier.query_interval,
                                                                          igmpProxyCfg.querier.query_response_interval);
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "    Older Querier Present Timeout (AUTO):  %u (s)", igmpProxyCfg.host.older_querier_present_timeout);
  }
  else if (igmpProxy->host.mask & PTIN_IGMP_HOST_MASK_OQPT
      && igmpProxyCfg.host.older_querier_present_timeout != igmpProxy->host.older_querier_present_timeout)
  {
    igmpProxyCfg.host.older_querier_present_timeout = igmpProxy->host.older_querier_present_timeout;
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "    Older Querier Present Timeout:         %u (s)", igmpProxyCfg.host.older_querier_present_timeout);
  }

  /* Max Records per Report */
  if (igmpProxy->host.mask & PTIN_IGMP_HOST_MASK_MRPR && igmpProxyCfg.host.max_records_per_report != igmpProxy->host.max_records_per_report)
  {
    if (igmpProxy->host.max_records_per_report < PTIN_IGMP_MIN_RECORDS_PER_REPORT || igmpProxy->host.max_records_per_report > PTIN_IGMP_MAX_RECORDS_PER_REPORT)
    {
      PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid Max Records per Report Value: %u, going to use existing value:%u", igmpProxy->host.max_records_per_report, igmpProxyCfg.host.max_records_per_report);
    }
    else
    {
      igmpProxyCfg.host.max_records_per_report = igmpProxy->host.max_records_per_report;
      PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "    Max Records per Report:                %u ", igmpProxyCfg.host.max_records_per_report);
    }
  }

  /* Update AUTO flags */
  igmpProxyCfg.querier.flags = igmpProxy->querier.flags;
  igmpProxyCfg.host.flags    = igmpProxy->host.flags;

  /* Finally, (de)activate IGMP module */
  if (igmpProxy->mask & PTIN_IGMP_PROXY_MASK_ADMIN
      && igmpProxyCfg.admin != igmpProxy->admin)
  {
    // TODO apply changes
//  rc = snoopIGMPAdminModeApply();

    igmpProxyCfg.admin = igmpProxy->admin;
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

  *igmpProxy = igmpProxyCfg;

  igmpProxy->mask         = 0xFF;
  igmpProxy->querier.mask = 0xFFFF;
  igmpProxy->host.mask    = 0xFF;

  return SUCCESS;
}




