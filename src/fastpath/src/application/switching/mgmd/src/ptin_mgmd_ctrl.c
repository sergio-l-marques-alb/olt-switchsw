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

#include "ptin_mgmd_ctrl.h"
#include "ptin_mgmd_cfg.h"
#include "ptin_mgmd_statistics.h"
#include "ptin_utils_inet_addr_api.h"
#include "ptin_mgmd_db.h"
#include "ptin_mgmd_logger.h"
#include "ptin_timer_api.h"

#include <string.h>

static uint32                numberOfGroups  = 0;
static ptin_mgmd_groupInfo_t groupList[PTIN_MGMD_MAX_CHANNELS];
static uint16                numberOfClients = 0;
static uint8                 portClientList[PTIN_MGMD_MAX_PORT_ID][PTIN_MGMD_CLIENT_BITMAP_SIZE]; 
static uint16                numberOfClientsPerPort[PTIN_MGMD_MAX_PORT_ID];


/**
* @purpose Process a CTRL PTIN_MGMD_EVENT_CTRL_PROXY_CONFIG_GET message
*  
* @param  eventMsg[out] : Pointer to CTRL data
*
* @return RC_t
*
* @notes none
*/
RC_t ptin_mgmd_ctrl_mgmd_config_get(PTIN_MGMD_EVENT_CTRL_t *eventData)
{
  ptin_IgmpProxyCfg_t          ptinIgmpProxy = {0};
  PTIN_MGMD_CTRL_MGMD_CONFIG_t data          = {0};
  RC_t                         res           = SUCCESS; 

  ptinIgmpProxy.mask         = 0xFF;
  ptinIgmpProxy.querier.mask = 0xFFFF;
  ptinIgmpProxy.host.mask    = 0xFF;

  if (SUCCESS != (res = ptin_mgmd_igmp_proxy_config_get(&ptinIgmpProxy)))
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Error getting IGMP Proxy config");
    res = FAILURE;
  }

  if(SUCCESS == res)
  {
    //Copy data
    data.mask                                   = ptinIgmpProxy.mask;
    data.admin                                  = ptinIgmpProxy.admin;
    data.whiteList                              = ptinIgmpProxy.whitelist;
    data.networkVersion                         = ptinIgmpProxy.networkVersion;
    data.clientVersion                          = ptinIgmpProxy.clientVersion;
    data.ipv4Addr                               = ptinIgmpProxy.ipv4_addr;
    data.igmpCos                                = ptinIgmpProxy.igmp_cos;
    data.fastLeave                              = ptinIgmpProxy.fast_leave;
    data.querier.mask                           = ptinIgmpProxy.querier.mask;
    data.querier.flags                          = ptinIgmpProxy.querier.flags;  
    data.querier.robustness                     = ptinIgmpProxy.querier.robustness;
    data.querier.queryInterval                  = ptinIgmpProxy.querier.query_interval;
    data.querier.queryResponseInterval          = ptinIgmpProxy.querier.query_response_interval;
    data.querier.groupMembershipInterval        = ptinIgmpProxy.querier.group_membership_interval;
    data.querier.otherQuerierPresentInterval    = ptinIgmpProxy.querier.other_querier_present_interval;
    data.querier.startupQueryInterval           = ptinIgmpProxy.querier.startup_query_interval;
    data.querier.startupQueryCount              = ptinIgmpProxy.querier.startup_query_count;
    data.querier.lastMemberQueryInterval        = ptinIgmpProxy.querier.last_member_query_interval;
    data.querier.lastMemberQueryCount           = ptinIgmpProxy.querier.last_member_query_count;
    data.querier.olderHostPresentTimeout        = ptinIgmpProxy.querier.older_host_present_timeout;
    data.host.mask                              = ptinIgmpProxy.host.mask;
    data.host.flags                             = ptinIgmpProxy.host.flags;  
    data.host.robustness                        = ptinIgmpProxy.host.robustness;
    data.host.unsolicitedReportInterval         = ptinIgmpProxy.host.unsolicited_report_interval;
    data.host.olderQuerierPresentTimeout        = ptinIgmpProxy.host.older_querier_present_timeout;
    data.host.maxRecordsPerReport               = ptinIgmpProxy.host.max_records_per_report;
   
    memcpy(eventData->data, &data, sizeof(PTIN_MGMD_CTRL_MGMD_CONFIG_t)); 
  }

  return res;
}


/**
* @purpose Process a CTRL PTIN_MGMD_EVENT_CTRL_PROXY_CONFIG_SET message
*  
* @param  eventMsg[in] : Pointer to CTRL data
*
* @return RC_t
*
* @notes none
*/
RC_t ptin_mgmd_ctrl_mgmd_config_set(PTIN_MGMD_EVENT_CTRL_t *eventData)
{
  RC_t                         res     = SUCCESS;
  ptin_IgmpProxyCfg_t          igmpCfg = {0};
  PTIN_MGMD_CTRL_MGMD_CONFIG_t data    = {0}; 

  memcpy(&data, eventData->data, sizeof(PTIN_MGMD_CTRL_MGMD_CONFIG_t)); 

  igmpCfg.mask                                   = data.mask;
  igmpCfg.admin                                  = data.admin;
  igmpCfg.networkVersion                         = data.networkVersion;
  igmpCfg.clientVersion                          = data.clientVersion;
  igmpCfg.ipv4_addr                              = data.ipv4Addr;
  igmpCfg.igmp_cos                               = data.igmpCos;
  igmpCfg.fast_leave                             = data.fastLeave;

  igmpCfg.querier.mask                           = data.querier.mask;
  igmpCfg.querier.flags                          = data.querier.flags;
  igmpCfg.querier.robustness                     = data.querier.robustness;
  igmpCfg.querier.query_interval                 = data.querier.queryInterval;
  igmpCfg.querier.query_response_interval        = data.querier.queryResponseInterval;
  igmpCfg.querier.group_membership_interval      = data.querier.groupMembershipInterval;
  igmpCfg.querier.other_querier_present_interval = data.querier.otherQuerierPresentInterval;
  igmpCfg.querier.startup_query_interval         = data.querier.startupQueryInterval;
  igmpCfg.querier.startup_query_count            = data.querier.startupQueryCount;
  igmpCfg.querier.last_member_query_interval     = data.querier.lastMemberQueryInterval;
  igmpCfg.querier.last_member_query_count        = data.querier.lastMemberQueryCount;
  igmpCfg.querier.older_host_present_timeout     = data.querier.olderHostPresentTimeout;

  igmpCfg.host.mask                              = data.host.mask;
  igmpCfg.host.flags                             = data.host.flags;
  igmpCfg.host.robustness                        = data.host.robustness;
  igmpCfg.host.unsolicited_report_interval       = data.host.unsolicitedReportInterval;
  igmpCfg.host.older_querier_present_timeout     = data.host.olderQuerierPresentTimeout;
  igmpCfg.host.max_records_per_report            = data.host.maxRecordsPerReport;

  igmpCfg.whitelist                              = data.whiteList;

  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "IGMP Proxy (mask=0x%08X)", igmpCfg.mask);
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, " Admin #                          = %u",          igmpCfg.admin);
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, " Network Version                  = %u",          igmpCfg.networkVersion);
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, " Client Version                   = %u",          igmpCfg.clientVersion);
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, " IP Addr                          = %u.%u.%u.%u", (igmpCfg.ipv4_addr>>24)&0xFF, (igmpCfg.ipv4_addr>>16)&0xFF, (igmpCfg.ipv4_addr>>8)&0xFF, igmpCfg.ipv4_addr&0xFF);
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, " COS                              = %u",          igmpCfg.igmp_cos);
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, " FastLeave                        = %s",          igmpCfg.fast_leave != 0 ? "ON" : "OFF");
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, " Querier (mask=0x%08X)", igmpCfg.querier.mask);
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "   Flags                          = 0x%04X",      igmpCfg.querier.flags);
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "   Robustness                     = %u",          igmpCfg.querier.robustness);
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "   Query Interval                 = %u",          igmpCfg.querier.query_interval);
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "   Query Response Interval        = %u",          igmpCfg.querier.query_response_interval);
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "   Group Membership Interval      = %u",          igmpCfg.querier.group_membership_interval);
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "   Other Querier Present Interval = %u",          igmpCfg.querier.other_querier_present_interval);
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "   Startup Query Interval         = %u",          igmpCfg.querier.startup_query_interval);
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "   Startup Query Count            = %u",          igmpCfg.querier.startup_query_count);
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "   Last Member Query Interval     = %u",          igmpCfg.querier.last_member_query_interval);
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "   Last Member Query Count        = %u",          igmpCfg.querier.last_member_query_count);
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "   Older Host Present Timeout     = %u",          igmpCfg.querier.older_host_present_timeout);
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, " Host (mask=0x%08X)", igmpCfg.host.mask);         
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "   Flags                          = 0x%02X",      igmpCfg.host.flags);
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "   Robustness                     = %u",          igmpCfg.host.robustness);
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "   Unsolicited Report Interval    = %u",          igmpCfg.host.unsolicited_report_interval);
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "   Older Querier Present Timeout  = %u",          igmpCfg.host.older_querier_present_timeout);
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "   Max Group Records per Packet   = %u",          igmpCfg.host.max_records_per_report);
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, " Whitelist                        = %s",          igmpCfg.whitelist != 0 ? "ON" : "OFF");

  if (SUCCESS != (res = ptin_mgmd_igmp_proxy_config_set(&igmpCfg)))
  {
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to configure MGMD proxy");
  }

  return res;
}


/**
* @purpose Process a CTRL PTIN_MGMD_EVENT_CTRL_CLIENT_STATS_GET message
*  
* @param  eventMsg[out] : Pointer to CTRL data
*
* @return RC_t
*
* @notes none
*/
RC_t ptin_mgmd_ctrl_clientstats_get(PTIN_MGMD_EVENT_CTRL_t *eventData)
{
  ptin_IGMP_Statistics_t          stats = {0};
  RC_t                            res   = SUCCESS;
  PTIN_MGMD_CTRL_STATS_REQUEST_t  request;
  PTIN_MGMD_CTRL_STATS_RESPONSE_t response  = {0}; 

  memcpy(&request, eventData->data, sizeof(PTIN_MGMD_CTRL_STATS_REQUEST_t));

  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Reading client statistics:");
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "  Port ID    = %u", request.portId);
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "  Client ID  = %u", request.clientId);

  //Get statistics 
  if (SUCCESS != (res = ptin_mgmd_stat_client_get(request.portId, request.clientId, &stats)) )
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Error getting client statistics");
    return res;
  }

  if(SUCCESS == res)
  {
    response.activeGroups                       = stats.active_groups;
    response.activeClients                      = stats.active_clients;

    response.igmpTx                             = stats.igmpv2.join_tx               +
                                                  stats.igmpv2.leave_tx              +
                                                  stats.igmpv3.membership_report_tx  +
                                                  stats.igmpquery.general_query_tx   +
                                                  stats.igmpquery.group_query_tx     +
                                                  stats.igmpquery.source_query_tx;

    response.igmpValidRx                        = stats.igmpv3.membership_report_valid_rx  +
                                                  stats.igmpv2.leave_valid_rx              +
                                                  stats.igmpv2.join_valid_rx               +
                                                  stats.igmpquery.general_query_valid_rx   +
                                                  stats.igmpquery.group_query_valid_rx     +
                                                  stats.igmpquery.source_query_valid_rx;

    response.igmpInvalidRx                      = stats.igmp_invalid_rx                +
                                                  stats.igmpv2.join_invalid_rx               +
                                                  stats.igmpv3.membership_report_invalid_rx  +
                                                  stats.igmpquery.generic_query_invalid_rx;

    response.igmpDroppedRx                      = stats.igmp_dropped_rx                         +
                                                  stats.igmpv3.membership_report_dropped_rx  +
                                                  stats.igmpquery.generic_query_dropped_rx;

    response.igmpTotalRx                        = response.igmpValidRx    +
                                                  response.igmpInvalidRx  +
                                                  response.igmpDroppedRx;

    response.v2.joinTx                          = stats.igmpv2.join_tx;
    response.v2.joinValidRx                     = stats.igmpv2.join_valid_rx;
    response.v2.joinInvalidRx                   = stats.igmpv2.join_invalid_rx;
    response.v2.leaveTx                         = stats.igmpv2.leave_tx;
    response.v2.leaveValidRx                    = stats.igmpv2.leave_valid_rx;

    response.v3.membershipReportTx              = stats.igmpv3.membership_report_tx;
    response.v3.membershipReportValidRx         = stats.igmpv3.membership_report_valid_rx;
    response.v3.membershipReportInvalidRx       = stats.igmpv3.membership_report_invalid_rx;
    response.v3.groupRecords.allowTx            = stats.igmpv3.group_record.allow_tx;
    response.v3.groupRecords.allowValidRx       = stats.igmpv3.group_record.allow_valid_rx;
    response.v3.groupRecords.allowInvalidRx     = stats.igmpv3.group_record.allow_invalid_rx;
    response.v3.groupRecords.blockTx            = stats.igmpv3.group_record.block_tx;
    response.v3.groupRecords.blockValidRx       = stats.igmpv3.group_record.block_valid_rx;
    response.v3.groupRecords.blockInvalidRx     = stats.igmpv3.group_record.block_invalid_rx;
    response.v3.groupRecords.isIncludeTx        = stats.igmpv3.group_record.is_include_tx;
    response.v3.groupRecords.isIncludeValidRx   = stats.igmpv3.group_record.is_include_valid_rx;
    response.v3.groupRecords.isIncludeInvalidRx = stats.igmpv3.group_record.is_include_invalid_rx;
    response.v3.groupRecords.isExcludeTx        = stats.igmpv3.group_record.is_exclude_tx;
    response.v3.groupRecords.isExcludeValidRx   = stats.igmpv3.group_record.is_exclude_valid_rx;
    response.v3.groupRecords.isExcludeInvalidRx = stats.igmpv3.group_record.is_exclude_invalid_rx;
    response.v3.groupRecords.toIncludeTx        = stats.igmpv3.group_record.to_include_tx;
    response.v3.groupRecords.toIncludeValidRx   = stats.igmpv3.group_record.to_include_valid_rx;
    response.v3.groupRecords.toIncludeInvalidRx = stats.igmpv3.group_record.to_include_invalid_rx;
    response.v3.groupRecords.toExcludeTx        = stats.igmpv3.group_record.to_exclude_tx;
    response.v3.groupRecords.toExcludeValidRx   = stats.igmpv3.group_record.to_exclude_valid_rx;
    response.v3.groupRecords.toExcludeInvalidRx = stats.igmpv3.group_record.to_exclude_invalid_rx;

    response.query.generalQueryTx               = stats.igmpquery.general_query_tx;
    response.query.generalQueryValidRx          = stats.igmpquery.general_query_valid_rx;
    response.query.groupQueryTx                 = stats.igmpquery.group_query_tx;
    response.query.groupQueryValidRx            = stats.igmpquery.group_query_valid_rx;
    response.query.sourceQueryTx                = stats.igmpquery.source_query_tx;
    response.query.sourceQueryValidRx           = stats.igmpquery.source_query_valid_rx;

    memcpy(eventData->data, &response, sizeof(PTIN_MGMD_CTRL_STATS_RESPONSE_t));
  }
  eventData->dataLength = sizeof(PTIN_MGMD_CTRL_STATS_RESPONSE_t);

  return res;
}


/**
* @purpose Process a CTRL PTIN_MGMD_EVENT_CTRL_CLIENT_STATS_CLEAR message
*  
* @param  eventMsg[in] : Pointer to CTRL data
*
* @return RC_t
*
* @notes none
*/
RC_t ptin_mgmd_ctrl_clientstats_clear(PTIN_MGMD_EVENT_CTRL_t *eventData)
{
  RC_t                           res  = SUCCESS;
  PTIN_MGMD_CTRL_STATS_REQUEST_t data = {0};

  memcpy(&data, eventData->data, sizeof(PTIN_MGMD_CTRL_STATS_REQUEST_t));

  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Resetting client statistics:");
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "  Port ID   = %u", data.portId);
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "  Client ID = %u", data.clientId);

  //Clear client stats
  if (SUCCESS != (res = ptin_mgmd_stat_client_clear(data.portId, data.clientId)))
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Error getting client statistics");
    return res;
  }

  return res;
}


/**
* @purpose Process a CTRL PTIN_MGMD_EVENT_CTRL_INTF_STATS_GET message
*  
* @param  eventMsg[out] : Pointer to CTRL data
*
* @return RC_t
*
* @notes none
*/
RC_t ptin_mgmd_ctrl_intfstats_get(PTIN_MGMD_EVENT_CTRL_t *eventData)
{
  ptin_IGMP_Statistics_t          stats    = {0};
  RC_t                            res      = SUCCESS;
  PTIN_MGMD_CTRL_STATS_REQUEST_t  request;
  PTIN_MGMD_CTRL_STATS_RESPONSE_t response = {0}; 

  memcpy(&request, eventData->data, sizeof(PTIN_MGMD_CTRL_STATS_REQUEST_t));

  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Reading interface statistics:");
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "  Service ID = %u", request.serviceId);
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "  Port ID    = %u", request.portId);

  //Get statistics
  if (SUCCESS != (res = ptin_mgmd_stat_intf_get(request.serviceId, request.portId, &stats)) )
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Error getting interface statistics");
    return res;
  }

  if(SUCCESS == res)
  {
    response.activeGroups                       = stats.active_groups;
    response.activeClients                      = stats.active_clients;

    response.igmpTx                             = stats.igmpv2.join_tx               +
                                                  stats.igmpv2.leave_tx              +
                                                  stats.igmpv3.membership_report_tx  +
                                                  stats.igmpquery.general_query_tx   +
                                                  stats.igmpquery.group_query_tx     +
                                                  stats.igmpquery.source_query_tx;

    response.igmpValidRx                        = stats.igmpv3.membership_report_valid_rx  +
                                                  stats.igmpv2.leave_valid_rx              +
                                                  stats.igmpv2.join_valid_rx               +
                                                  stats.igmpquery.general_query_valid_rx   +
                                                  stats.igmpquery.group_query_valid_rx     +
                                                  stats.igmpquery.source_query_valid_rx;

    response.igmpInvalidRx                      = stats.igmp_invalid_rx                +
                                                  stats.igmpv2.join_invalid_rx               +
                                                  stats.igmpv3.membership_report_invalid_rx  +
                                                  stats.igmpquery.generic_query_invalid_rx;

    response.igmpDroppedRx                      = stats.igmp_dropped_rx                         +
                                                  stats.igmpv3.membership_report_dropped_rx  +
                                                  stats.igmpquery.generic_query_dropped_rx;

    response.igmpTotalRx                        = response.igmpValidRx    +
                                                  response.igmpInvalidRx  +
                                                  response.igmpDroppedRx;

    response.v2.joinTx                          = stats.igmpv2.join_tx;
    response.v2.joinValidRx                     = stats.igmpv2.join_valid_rx;
    response.v2.joinInvalidRx                   = stats.igmpv2.join_invalid_rx;
    response.v2.leaveTx                         = stats.igmpv2.leave_tx;
    response.v2.leaveValidRx                    = stats.igmpv2.leave_valid_rx;

    response.v3.membershipReportTx              = stats.igmpv3.membership_report_tx;
    response.v3.membershipReportValidRx         = stats.igmpv3.membership_report_valid_rx;
    response.v3.membershipReportInvalidRx       = stats.igmpv3.membership_report_invalid_rx;
    response.v3.groupRecords.allowTx            = stats.igmpv3.group_record.allow_tx;
    response.v3.groupRecords.allowValidRx       = stats.igmpv3.group_record.allow_valid_rx;
    response.v3.groupRecords.allowInvalidRx     = stats.igmpv3.group_record.allow_invalid_rx;
    response.v3.groupRecords.blockTx            = stats.igmpv3.group_record.block_tx;
    response.v3.groupRecords.blockValidRx       = stats.igmpv3.group_record.block_valid_rx;
    response.v3.groupRecords.blockInvalidRx     = stats.igmpv3.group_record.block_invalid_rx;
    response.v3.groupRecords.isIncludeTx        = stats.igmpv3.group_record.is_include_tx;
    response.v3.groupRecords.isIncludeValidRx   = stats.igmpv3.group_record.is_include_valid_rx;
    response.v3.groupRecords.isIncludeInvalidRx = stats.igmpv3.group_record.is_include_invalid_rx;
    response.v3.groupRecords.isExcludeTx        = stats.igmpv3.group_record.is_exclude_tx;
    response.v3.groupRecords.isExcludeValidRx   = stats.igmpv3.group_record.is_exclude_valid_rx;
    response.v3.groupRecords.isExcludeInvalidRx = stats.igmpv3.group_record.is_exclude_invalid_rx;
    response.v3.groupRecords.toIncludeTx        = stats.igmpv3.group_record.to_include_tx;
    response.v3.groupRecords.toIncludeValidRx   = stats.igmpv3.group_record.to_include_valid_rx;
    response.v3.groupRecords.toIncludeInvalidRx = stats.igmpv3.group_record.to_include_invalid_rx;
    response.v3.groupRecords.toExcludeTx        = stats.igmpv3.group_record.to_exclude_tx;
    response.v3.groupRecords.toExcludeValidRx   = stats.igmpv3.group_record.to_exclude_valid_rx;
    response.v3.groupRecords.toExcludeInvalidRx = stats.igmpv3.group_record.to_exclude_invalid_rx;

    response.query.generalQueryTx               = stats.igmpquery.general_query_tx;
    response.query.generalQueryValidRx          = stats.igmpquery.general_query_valid_rx;
    response.query.groupQueryTx                 = stats.igmpquery.group_query_tx;
    response.query.groupQueryValidRx            = stats.igmpquery.group_query_valid_rx;
    response.query.sourceQueryTx                = stats.igmpquery.source_query_tx;
    response.query.sourceQueryValidRx           = stats.igmpquery.source_query_valid_rx;

    memcpy(eventData->data, &response, sizeof(PTIN_MGMD_CTRL_STATS_RESPONSE_t));
  }
  eventData->dataLength = sizeof(PTIN_MGMD_CTRL_STATS_RESPONSE_t);

  return res;
}


/**
* @purpose Process a CTRL PTIN_MGMD_EVENT_CTRL_INTF_STATS_CLEAR message
*  
* @param  eventMsg[in] : Pointer to CTRL data
*
* @return RC_t
*
* @notes none
*/
RC_t ptin_mgmd_ctrl_intfstats_clear(PTIN_MGMD_EVENT_CTRL_t *eventData)
{
  RC_t                           res  = SUCCESS;
  PTIN_MGMD_CTRL_STATS_REQUEST_t data = {0}; 

  memcpy(&data, eventData->data, sizeof(PTIN_MGMD_CTRL_STATS_REQUEST_t));

  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Resetting interface statistics:");
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "  Service ID = %u", data.serviceId);
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "  Port ID    = %u", data.portId);

  //Clear client stats
  if (SUCCESS != (res = ptin_mgmd_stat_intf_clear(data.serviceId, data.portId)) )
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Error getting interface statistics");
    return res;
  }

  return res;
}


/**
* @purpose Process a CTRL PTIN_MGMD_EVENT_CTRL_GROUPS_GET message
*  
* @param  eventMsg[in] : Pointer to CTRL data
*
* @return RC_t
*
* @notes none
*/
RC_t ptin_mgmd_ctrl_activegroups_get(PTIN_MGMD_EVENT_CTRL_t *eventData)
{
  RC_t                                   res               = SUCCESS;
  uint32                                 maxNumberOfEntries = PTIN_MGMD_EVENT_CTRL_DATA_SIZE_MAX / sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_RESPONSE_t);
  uint16                                 i;
  uint32                                 j;
  PTIN_MGMD_CTRL_ACTIVEGROUPS_REQUEST_t  request           = {0};
  PTIN_MGMD_CTRL_ACTIVEGROUPS_RESPONSE_t response          = {0}; 
  
  memcpy(&request, eventData->data, sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_REQUEST_t));

  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Reading channel list");
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP," Service ID = %u", request.serviceId);
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP," Entry ID   = %u (0x%04X)", request.entryId, request.entryId);

  request.clientId = PTIN_MGMD_MANAGEMENT_CLIENT_ID;

  //Update groupList
  if(PTIN_MGMD_CTRL_ACTIVEGROUPS_FIRST_ENTRY == request.entryId) 
  {
    memset(groupList, 0x00, sizeof(groupList));
    if (SUCCESS != (res = ptinMgmdactivegroups_get(request.serviceId, request.portId, request.clientId, groupList, &numberOfGroups)))
    {
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Unable to update groupList");
      res = FAILURE;
    }
  }

  if(SUCCESS == res)
  {
    if( (PTIN_MGMD_CTRL_ACTIVEGROUPS_FIRST_ENTRY != request.entryId) && (request.entryId > numberOfGroups) ) //Update groupList
    {
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Invalid entryId [Max: %u]", numberOfGroups);
      res = NOT_EXIST;
    }
  }

  if(SUCCESS == res)
  {
    for (i=request.entryId+1, j=0; i<numberOfGroups && j<maxNumberOfEntries; ++i)
    {
      PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Type[%u] FilterMode[%u] GroupTimer[%u] Group:[%08X] SourceTimer[%u] Source[%08X]", 
                groupList[i].staticType, groupList[i].filterMode, groupList[i].groupTimer, groupList[i].groupAddr.addr.ipv4.s_addr, 
                groupList[i].sourceTimer, groupList[i].sourceAddr.addr.ipv4.s_addr, groupList[i].staticType);

      response.entryId     = i;
      response.groupType   = groupList[i].staticType;
      response.filterMode  = groupList[i].filterMode;
      response.groupIP     = groupList[i].groupAddr.addr.ipv4.s_addr;
      response.groupTimer  = groupList[i].groupTimer;
      response.sourceIP    = groupList[i].sourceAddr.addr.ipv4.s_addr;
      response.sourceTimer = groupList[i].sourceTimer;

      memcpy(eventData->data+j*sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_RESPONSE_t), &response, sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_RESPONSE_t));
      ++j;
    }
    eventData->dataLength = j*sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_RESPONSE_t);
  }

  return res;
}


/**
* @purpose Process a CTRL PTIN_MGMD_EVENT_CTRL_CLIENT_GROUPS_GET message
*  
* @param  eventMsg[in] : Pointer to CTRL data
*
* @return RC_t
*
* @notes none
*/
RC_t ptin_mgmd_ctrl_client_activegroups_get(PTIN_MGMD_EVENT_CTRL_t *eventData)
{
  RC_t                                   res               = SUCCESS;
  uint32                                 maxNumberOfEntries = PTIN_MGMD_EVENT_CTRL_DATA_SIZE_MAX / sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_RESPONSE_t);
  uint16                                 i;
  uint32                                 j;
  PTIN_MGMD_CTRL_ACTIVEGROUPS_REQUEST_t  request           = {0};
  PTIN_MGMD_CTRL_ACTIVEGROUPS_RESPONSE_t response          = {0}; 
  
  memcpy(&request, eventData->data, sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_REQUEST_t));

  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Reading channel list");
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP," Service ID = %u", request.serviceId);
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP," Port ID    = %u", request.portId);
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP," Client ID  = %u", request.clientId);
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP," Entry ID   = %u (0x%04X)", request.entryId, request.entryId);

  //Update groupList
  if(PTIN_MGMD_CTRL_ACTIVEGROUPS_FIRST_ENTRY == request.entryId) 
  {
    memset(groupList, 0x00, sizeof(groupList));
    if (SUCCESS != (res = ptinMgmdactivegroups_get(request.serviceId, request.portId, request.clientId, groupList, &numberOfGroups)))
    {
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Unable to update groupList");
      res = FAILURE;
    }
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Number of Groups read:%u",numberOfGroups);
  }

  if(SUCCESS == res)
  {
    if( (PTIN_MGMD_CTRL_ACTIVEGROUPS_FIRST_ENTRY != request.entryId) && (request.entryId > numberOfGroups) )
    {
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Invalid entryId [Max: %u]", numberOfGroups);
      res = NOT_EXIST;
    }
  }

  if(SUCCESS == res)
  {
    for (i=request.entryId+1, j=0; i<numberOfGroups && j<maxNumberOfEntries; ++i)
    {
      response.entryId     = i;
      response.groupType   = groupList[i].staticType;
      response.filterMode  = groupList[i].filterMode;
      response.groupIP     = groupList[i].groupAddr.addr.ipv4.s_addr;
      response.groupTimer  = groupList[i].groupTimer;
      response.sourceIP    = groupList[i].sourceAddr.addr.ipv4.s_addr;
      response.sourceTimer = groupList[i].sourceTimer;

      PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Copying...[Entry Id:%u groupIP:0x%04X groupType:%u filterMode:%u groupTimer:%u sourceIP:0x%04X sourceTimer:%u]", response.entryId, response.groupIP,response.groupType,
                          response.filterMode, response.groupTimer, response.sourceIP, response.sourceTimer);
      memcpy(eventData->data+j*sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_RESPONSE_t), &response, sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_RESPONSE_t));
      ++j;
    }
    eventData->dataLength = j*sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_RESPONSE_t);
  }

  return res;
}


/**
* @purpose Process a CTRL PTIN_MGMD_EVENT_CTRL_GROUP_CLIENTS_GET message
*  
* @param  eventMsg[in] : Pointer to CTRL data
*
* @return RC_t
*
* @notes none
*/
RC_t ptin_mgmd_ctrl_clientList_get(PTIN_MGMD_EVENT_CTRL_t *eventData)
{
  RC_t                                   res                = SUCCESS;  
  uint16                                 portId;
  uint16                                 numberOfClientsAux = 0;
  uint32                                 maxNumberOfEntries = PTIN_MGMD_EVENT_CTRL_DATA_SIZE_MAX / sizeof(PTIN_MGMD_CTRL_GROUPCLIENTS_RESPONSE_t);
  uint32                                 clientId, entryId, numOfClientsInMsg;
  PTIN_MGMD_CTRL_GROUPCLIENTS_REQUEST_t  request            = {0};
  PTIN_MGMD_CTRL_GROUPCLIENTS_RESPONSE_t response           = {0};
  ptin_mgmd_inet_addr_t                  groupAddr, sourceAddr;
  char                                   debugBuf[PTIN_MGMD_IPV6_DISP_ADDR_LEN]; 
  
  memcpy(&request, eventData->data, sizeof(PTIN_MGMD_CTRL_GROUPCLIENTS_REQUEST_t));

  ptin_mgmd_inetAddressSet(PTIN_MGMD_AF_INET, &request.groupIP,  &groupAddr);
  ptin_mgmd_inetAddressSet(PTIN_MGMD_AF_INET, &request.sourceIP, &sourceAddr);

  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Reading client list");
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP," Service ID = %u", request.serviceId);
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP," Group IP   = %s", ptin_mgmd_inetAddrPrint(&groupAddr,  debugBuf));
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP," Source IP  = %s", ptin_mgmd_inetAddrPrint(&sourceAddr, debugBuf));
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP," Entry ID   = %u (0x%04X)", request.entryId, request.entryId);  

  //Update groupList
  if(PTIN_MGMD_CTRL_GROUPCLIENTS_FIRST_ENTRY == request.entryId)
  {
    numberOfClients=0;
    memset(portClientList, 0x00, sizeof(portClientList));
    memset(numberOfClientsPerPort, 0x00, sizeof(numberOfClientsPerPort));
    for (portId=1;portId<=PTIN_MGMD_MAX_PORT_ID;portId++)
    {
      if (ptin_mgmd_loop_trace) 
        PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Iterating over portId:%u | PTIN_MGMD_MAX_PORT_ID:%u", portId, PTIN_MGMD_MAX_PORT_ID);  

      if (SUCCESS != (res = ptinMgmdgroupclients_get(request.serviceId, portId, &groupAddr, &sourceAddr, portClientList[portId], &numberOfClientsPerPort[portId])))
      {
        PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Unable to update clientList");
        return SUCCESS;//FAILURE;        
      }
      numberOfClients+=numberOfClientsPerPort[portId];
    }
  }
  else
  {
    if(request.entryId > numberOfClients) 
    {
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Invalid entryId [Max: %u]", numberOfClients);
      return SUCCESS;//NOT_EXIST;
    }
  }


  ++request.entryId; //Start searching the ID next to the requested
  numOfClientsInMsg  = 0;
  numberOfClientsAux = 0;
  entryId            = 0;
  for (portId=1; portId<=PTIN_MGMD_MAX_PORT_ID; portId++)
  {
    if (ptin_mgmd_loop_trace) 
      PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Iterating over portId:%u | PTIN_MGMD_MAX_PORT_ID:%u", portId, PTIN_MGMD_MAX_PORT_ID);  

    //Ignore this interface if the requested entryId is higher than the number os clients for this interface
    numberOfClientsAux += numberOfClientsPerPort[portId];
    if(numberOfClientsAux < request.entryId)
    {
      entryId += numberOfClientsPerPort[portId];
      continue;
    }      
    
    //
    //Check each client in this interface
    for (clientId=0; entryId<maxNumberOfEntries && entryId<numberOfClients && clientId<PTIN_MGMD_MAX_CLIENTS && numberOfClientsPerPort[portId]!=0; ++clientId)
    {      
      if (ptin_mgmd_extended_debug) 
        PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Iterating over clientId:%u | numberOfClients:%u | PTIN_MGMD_MAX_CLIENTS:%u",clientId, numberOfClients, PTIN_MGMD_MAX_CLIENTS);  

      
      //Move forward 8 bits if this byte is 0 (no clients)
      if(! (PTIN_MGMD_CLIENT_IS_MASKBYTESET(portClientList[portId],clientId)))
      {
        clientId += PTIN_MGMD_CLIENT_MASK_UNIT -1; //Less one, because of the For cycle that increments also 1 unit.
        continue;
      } 
                  
      //Move forward 1 bit if the requested entryId is still higher
      ++numberOfClientsAux;
      if(numberOfClientsAux <= request.entryId)
      {
        entryId += numberOfClientsPerPort[portId];
        continue;
      } 
      
      //Add a new client to the response if the bit is set
      if(TRUE == PTIN_MGMD_CLIENT_IS_MASKBITSET(portClientList[portId], clientId))
      {    
        response.entryId  = entryId;
        response.portId   = portId;
        response.clientId = clientId;      
        memcpy(eventData->data+entryId*sizeof(PTIN_MGMD_CTRL_GROUPCLIENTS_RESPONSE_t), &response, sizeof(PTIN_MGMD_CTRL_GROUPCLIENTS_RESPONSE_t));

        ++entryId;
        ++numOfClientsInMsg;
        //All Clients have been already found
        if(numOfClientsInMsg>=numberOfClientsPerPort[portId])
        {
          break;
        }
      }
    }
  }
  eventData->dataLength = numOfClientsInMsg*sizeof(PTIN_MGMD_CTRL_GROUPCLIENTS_RESPONSE_t);
  
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Number of Clients (read:%u and in the message:%u)",numberOfClients,numOfClientsInMsg);
  return SUCCESS;
}

/**
* @purpose Process a CTRL PTIN_MGMD_EVENT_CTRL_STATIC_GROUP_ADD message
*  
* @param  eventMsg[in] : Pointer to CTRL data
*
* @return RC_t
*
* @notes none
*/
RC_t ptin_mgmd_ctrl_staticChannel_add(PTIN_MGMD_EVENT_CTRL_t *eventData)
{
  RC_t                         res       = SUCCESS;
  PTIN_MGMD_CTRL_STATICGROUP_t request   = {0};
  ptin_mgmd_inet_addr_t             groupAddr;
  char                         debugBuf[PTIN_MGMD_IPV6_DISP_ADDR_LEN]; 
  
  memcpy(&request, eventData->data, sizeof(PTIN_MGMD_CTRL_STATICGROUP_t));

  ptin_mgmd_inetAddressSet(PTIN_MGMD_AF_INET, &request.groupIp, &groupAddr);

  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Adding static group");
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP," Service ID = %u", request.serviceId);
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP," Group IP   = %s", ptin_mgmd_inetAddrPrint(&groupAddr, debugBuf));

  if (SUCCESS != (res = ptinMgmdAddStaticGroup(request.serviceId, &groupAddr, 0, PTIN_NULLPTR)))
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Error adding static group");
    return res;
  }

  return res;
}

/**
* @purpose Process a CTRL PTIN_MGMD_EVENT_CTRL_STATIC_GROUP_REMOVE message
*  
* @param  eventMsg[in] : Pointer to CTRL data
*
* @return RC_t
*
* @notes none
*/
RC_t ptin_mgmd_ctrl_staticChannel_remove(PTIN_MGMD_EVENT_CTRL_t *eventData)
{
  RC_t                         res       = SUCCESS;
  PTIN_MGMD_CTRL_STATICGROUP_t request   = {0};
  ptin_mgmd_inet_addr_t             groupAddr;
  char                         debugBuf[PTIN_MGMD_IPV6_DISP_ADDR_LEN]; 
  
  memcpy(&request, eventData->data, sizeof(PTIN_MGMD_CTRL_STATICGROUP_t));

  ptin_mgmd_inetAddressSet(PTIN_MGMD_AF_INET, &request.groupIp, &groupAddr);

  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Removing static group");
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP," Service ID = %u", request.serviceId);
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP," Group IP   = %s", ptin_mgmd_inetAddrPrint(&groupAddr, debugBuf));

  if (SUCCESS != (res = ptinMgmdRemoveStaticGroup(request.serviceId, &groupAddr, 0, PTIN_NULLPTR)))
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Error adding static group");
    return res;
  }

  return res;
}

/**
* @purpose Process a CTRL PTIN_MGMD_EVENT_CTRL_WHITELIST_ADD message
*  
* @param  eventMsg[in] : Pointer to CTRL data
*
* @return RC_t
*
* @notes none
*/
RC_t ptin_mgmd_ctrl_whitelist_add(PTIN_MGMD_EVENT_CTRL_t *eventData)
{
  RC_t                              res       = SUCCESS;
  PTIN_MGMD_CTRL_WHITELIST_CONFIG_t request   = {0};
  ptin_mgmd_inet_addr_t             groupAddr;
  ptin_mgmd_inet_addr_t             sourceAddr;  
  char                              debugBuf[PTIN_MGMD_IPV6_DISP_ADDR_LEN]; 
  
  memcpy(&request, eventData->data, sizeof(PTIN_MGMD_CTRL_WHITELIST_CONFIG_t));

  ptin_mgmd_inetAddressSet(PTIN_MGMD_AF_INET, &request.groupIp, &groupAddr);
  ptin_mgmd_inetAddressSet(PTIN_MGMD_AF_INET, &request.sourceIp, &sourceAddr);

  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Adding channel to white-list");
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP," Service ID = %u", request.serviceId);
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP," Group IP   = %s", ptin_mgmd_inetAddrPrint(&groupAddr, debugBuf));
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP," Source IP  = %s", ptin_mgmd_inetAddrPrint(&sourceAddr, debugBuf));

  if (SUCCESS != ptinMgmdWhitelistAdd(request.serviceId, &groupAddr,request.groupMaskLen, &sourceAddr,request.sourceMaskLen))
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Error adding entry to white-list");
    return ERROR;
  }

  return res;
}

/**
* @purpose Process a CTRL PTIN_MGMD_EVENT_CTRL_WHITELIST_REMOVE message
*  
* @param  eventMsg[in] : Pointer to CTRL data
*
* @return RC_t
*
* @notes none
*/
RC_t ptin_mgmd_ctrl_whitelist_remove(PTIN_MGMD_EVENT_CTRL_t *eventData)
{
  RC_t                              res       = SUCCESS;
  PTIN_MGMD_CTRL_WHITELIST_CONFIG_t request   = {0};
  ptin_mgmd_inet_addr_t             groupAddr;
  ptin_mgmd_inet_addr_t             sourceAddr;
  char                              debugBuf[PTIN_MGMD_IPV6_DISP_ADDR_LEN]; 
  
  memcpy(&request, eventData->data, sizeof(PTIN_MGMD_CTRL_WHITELIST_CONFIG_t));

  ptin_mgmd_inetAddressSet(PTIN_MGMD_AF_INET, &request.groupIp, &groupAddr);
  ptin_mgmd_inetAddressSet(PTIN_MGMD_AF_INET, &request.sourceIp, &sourceAddr);

  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Removing channel from white-list");
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP," Service ID = %u", request.serviceId);
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP," Group IP   = %s", ptin_mgmd_inetAddrPrint(&groupAddr, debugBuf));
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP," Source IP  = %s", ptin_mgmd_inetAddrPrint(&sourceAddr, debugBuf));

  if (SUCCESS != (res = ptinMgmdWhitelistRemove(request.serviceId, &groupAddr,request.groupMaskLen, &sourceAddr, request.sourceMaskLen)))
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Error removing entry from white-list");
    return res;
  }

  return res;
}

/**
* @purpose Process a CTRL PTIN_MGMD_EVENT_CTRL_SERVICE_REMOVE message
*  
* @param  eventMsg[in] : Pointer to CTRL data
*
* @return RC_t
*
* @notes none
*/
RC_t ptin_mgmd_ctrl_service_remove(PTIN_MGMD_EVENT_CTRL_t *eventData)
{
  RC_t                            res       = SUCCESS;
  PTIN_MGMD_CTRL_SERVICE_REMOVE_t request   = {0};
  
  memcpy(&request, eventData->data, sizeof(PTIN_MGMD_CTRL_SERVICE_REMOVE_t));

  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Removing service from MGMD");
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP," Service ID = %u", request.serviceId);

  if (SUCCESS != (res = ptinMgmdServiceRemove(request.serviceId)))
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Error removing service from MGMD");
    return res;
  }

  return res;
}

/**
* @purpose Process a CTRL PTIN_MGMD_EVENT_CTRL_RESET_DEFAULTS message
*  
* @param  eventMsg[in] : Pointer to CTRL data
*
* @return RC_t
*
* @notes none
*/
RC_t ptin_mgmd_ctrl_reset_defaults(PTIN_MGMD_EVENT_CTRL_t *eventData)
{
  RC_t                            res = SUCCESS;
  PTIN_MGMD_CTRL_RESET_DEFAULTS_t request = {0};
  
  memcpy(&request, eventData->data, sizeof(PTIN_MGMD_CTRL_RESET_DEFAULTS_t));

  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Resetting MGMD to default settings");
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP," Family = %u", request.family);

  if (SUCCESS != (res = ptinMgmdResetDefaults(request.family)))
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Error resetting MGMD to default settings");
    return res;
  }

  return res;
}

