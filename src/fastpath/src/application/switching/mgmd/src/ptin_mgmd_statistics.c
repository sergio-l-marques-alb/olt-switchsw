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

#include "ptin_mgmd_statistics.h"
#include "logger.h"
#include "ptin_mgmd_service_api.h"
#include "ptin_mgmd_defs.h"
#include "ptin_mgmd_osapi.h"

#include <string.h>

ptin_IGMP_Statistics_t mgmd_stat_port[PTIN_MGMD_MAX_PORTS]                            = {{0}};
ptin_IGMP_Statistics_t mgmd_stat_service[PTIN_MGMD_MAX_SERVICES][PTIN_MGMD_MAX_PORTS] = {{{0}}};
ptin_IGMP_Statistics_t mgmd_stat_client[PTIN_MGMD_MAX_PORTS][PTIN_MGMD_MAX_CLIENTS]   = {{{0}}};


/**
 * Use this static method to find the array idx with the desired service ID
 * 
 * @param serviceId : Service ID
 * @param arrayIdx  : Index in the array with the requested service ID
 * 
 * @return RC_t 
 */
static RC_t ptin_mgmd_stats_service_find(uint32 serviceId, int16 *arrayIdx)
{
  // Validations
  if ( PTIN_NULLPTR == arrayIdx)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Abnormal context [serviceId:%u arrayIdx:%p]", serviceId, arrayIdx);
    return FAILURE;
  }

  *arrayIdx = 0;
  for(*arrayIdx = 0; *arrayIdx < PTIN_MGMD_MAX_SERVICES; ++(*arrayIdx))
  {
    if(mgmd_stat_service[*arrayIdx][0].serviceId == serviceId)
    {
      return SUCCESS;
    }
  }

  return FAILURE;
}

/**
 * Use this static method to find the array idx with the desired service ID
 * 
 * @param serviceId : Service ID
 * @param arrayIdx  : Index in the array with the requested service ID
 * 
 * @return RC_t 
 *  
 * @note: If not found, returns an empty position 
 */
static RC_t ptin_mgmd_stats_service_getfree(uint32 serviceId, int16 *arrayIdx)
{
  // Validations
  if ( PTIN_NULLPTR == arrayIdx)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Abnormal context [serviceId:%u arrayIdx:%p]", serviceId, arrayIdx);
    return FAILURE;
  }

  *arrayIdx = 0;
  for(*arrayIdx = 0; *arrayIdx < PTIN_MGMD_MAX_SERVICES; ++(*arrayIdx))
  {
    if(mgmd_stat_service[*arrayIdx][0].used == FALSE)
    {
      uint32 portIdx;
     
      for(portIdx = 0; portIdx < PTIN_MGMD_MAX_PORTS; ++portIdx)
      {
        memset(&mgmd_stat_service[*arrayIdx][portIdx], 0x00, sizeof(ptin_IGMP_Statistics_t));
        mgmd_stat_service[*arrayIdx][portIdx].used      = TRUE;
        mgmd_stat_service[*arrayIdx][portIdx].serviceId = serviceId;
        return SUCCESS;
      }
    }
  }

  return FAILURE;
}

/**
 * Reset statistics for the requested service and "remove" it from the statistics array
 * 
 * @param serviceId : Service ID
 * 
 * @return RC_t 
 */
RC_t ptin_mgmd_stats_service_clear(uint32 serviceId)
{
  uint16 arrayIdx;
  uint32 portIdx;

  for(arrayIdx = 0; arrayIdx < PTIN_MGMD_MAX_SERVICES; ++arrayIdx)
  {
    if(mgmd_stat_service[arrayIdx][0].serviceId == serviceId)
    {
      for(portIdx = 0; portIdx < PTIN_MGMD_MAX_PORTS; ++portIdx)
      {
        memset(&mgmd_stat_service[arrayIdx][portIdx], 0x00, sizeof(ptin_IGMP_Statistics_t));
        mgmd_stat_service[arrayIdx][portIdx].used = FALSE;
      }
    }
  }

  return FAILURE;
}

/**
 * Increment MGMD statistics
 * 
 * @param portId    : interface where the packet entered
 * @param serviceId : Service ID
 * @param clientId  : Client ID
 * @param field     : field to increment
 * 
 * @return RC_t 
 */
RC_t ptin_mgmd_stat_increment_field(uint32 portId, uint32 serviceId, uint32 clientId, ptin_snoop_stat_enum_t field)
{
  int16 arrayIdx;

  // Validations
  if ( (portId >= PTIN_MGMD_MAX_PORTS) || (serviceId > PTIN_MGMD_MAX_SERVICE_ID) || ((clientId != ((uint32)-1)) && (clientId > PTIN_MGMD_MAX_CLIENTS)) )
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Abnormal context [portId:%u serviceId:%u clientId:%u]", portId, serviceId, clientId);
    return FAILURE;
  }

  // Get the index that holds the requested service ID. If not found, get a free index
  if(SUCCESS != ptin_mgmd_stats_service_find(serviceId, &arrayIdx))
  {
    if(SUCCESS != ptin_mgmd_stats_service_getfree(serviceId, &arrayIdx))
    {
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to find requested service[%u] and there are no free indexes left", serviceId);
      return FAILURE;
    }
  }

  switch (field) {
  case SNOOP_STAT_FIELD_ACTIVE_GROUPS:
    ++mgmd_stat_port[portId].active_groups;
    ++mgmd_stat_service[arrayIdx][portId].active_groups;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].active_groups;
    break;

  case SNOOP_STAT_FIELD_ACTIVE_CLIENTS:
    ++mgmd_stat_port[portId].active_clients;
    ++mgmd_stat_service[arrayIdx][portId].active_clients;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].active_clients;
    break;

  case SNOOP_STAT_FIELD_IGMP_SENT:
    ++mgmd_stat_port[portId].igmp_sent;
    ++mgmd_stat_service[arrayIdx][portId].igmp_sent;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmp_sent;
    break;

  case SNOOP_STAT_FIELD_IGMP_TX_FAILED:
    ++mgmd_stat_port[portId].igmp_tx_failed;
    ++mgmd_stat_service[arrayIdx][portId].igmp_tx_failed;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmp_tx_failed;
    break;

  case SNOOP_STAT_FIELD_IGMP_INTERCEPTED:
    ++mgmd_stat_port[portId].igmp_intercepted;
    ++mgmd_stat_service[arrayIdx][portId].igmp_intercepted;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmp_intercepted;
    break;

  case SNOOP_STAT_FIELD_IGMP_DROPPED:
    ++mgmd_stat_port[portId].igmp_dropped;
    ++mgmd_stat_service[arrayIdx][portId].igmp_dropped;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmp_dropped;
    break;

  case SNOOP_STAT_FIELD_IGMP_RECEIVED_VALID:
    ++mgmd_stat_port[portId].igmp_received_valid;
    ++mgmd_stat_service[arrayIdx][portId].igmp_received_valid;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmp_received_valid;
    break;

  case SNOOP_STAT_FIELD_IGMP_RECEIVED_INVALID:
    ++mgmd_stat_port[portId].igmp_received_invalid;
    ++mgmd_stat_service[arrayIdx][portId].igmp_received_invalid;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmp_received_invalid;
    break;

  case SNOOP_STAT_FIELD_GENERIC_QUERY_INVALID_RX:  
    ++mgmd_stat_port[portId].igmpquery.generic_query_invalid_rx;
    ++mgmd_stat_service[arrayIdx][portId].igmpquery.generic_query_invalid_rx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpquery.generic_query_invalid_rx;
    break;

  case SNOOP_STAT_FIELD_GENERAL_QUERY_TX:
    ++mgmd_stat_port[portId].igmpquery.general_query_tx;
    ++mgmd_stat_service[arrayIdx][portId].igmpquery.general_query_tx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpquery.general_query_tx;
    break;

  case SNOOP_STAT_FIELD_GENERAL_QUERY_TOTAL_RX:
    ++mgmd_stat_port[portId].igmpquery.general_query_total_rx;
    ++mgmd_stat_service[arrayIdx][portId].igmpquery.general_query_total_rx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpquery.general_query_total_rx;
    break;

  case SNOOP_STAT_FIELD_GENERAL_QUERY_VALID_RX:
    ++mgmd_stat_port[portId].igmpquery.general_query_valid_rx;
    ++mgmd_stat_service[arrayIdx][portId].igmpquery.general_query_valid_rx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpquery.general_query_valid_rx;
    break;

  case SNOOP_STAT_FIELD_GENERAL_QUERY_DROPPED_RX:
    ++mgmd_stat_port[portId].igmpquery.general_query_dropped_rx;
    ++mgmd_stat_service[arrayIdx][portId].igmpquery.general_query_dropped_rx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpquery.general_query_dropped_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_SPECIFIC_QUERY_TX:
    ++mgmd_stat_port[portId].igmpquery.group_query_tx;
    ++mgmd_stat_service[arrayIdx][portId].igmpquery.group_query_tx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpquery.group_query_tx;
    break;

  case SNOOP_STAT_FIELD_GROUP_SPECIFIC_QUERY_TOTAL_RX:
    ++mgmd_stat_port[portId].igmpquery.group_query_total_rx;
    ++mgmd_stat_service[arrayIdx][portId].igmpquery.group_query_total_rx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpquery.group_query_total_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_SPECIFIC_QUERY_VALID_RX:
    ++mgmd_stat_port[portId].igmpquery.group_query_valid_rx;
    ++mgmd_stat_service[arrayIdx][portId].igmpquery.group_query_valid_rx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpquery.group_query_valid_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_SPECIFIC_QUERY_DROPPED_RX:
    ++mgmd_stat_port[portId].igmpquery.group_query_dropped_rx;
    ++mgmd_stat_service[arrayIdx][portId].igmpquery.group_query_dropped_rx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpquery.group_query_dropped_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_AND_SOURCE_SPECIFIC_QUERY_TX:
    ++mgmd_stat_port[portId].igmpquery.source_query_tx;
    ++mgmd_stat_service[arrayIdx][portId].igmpquery.source_query_tx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpquery.source_query_tx;
    break;

  case SNOOP_STAT_FIELD_GROUP_AND_SOURCE_SPECIFIC_QUERY_TOTAL_RX:
    ++mgmd_stat_port[portId].igmpquery.source_query_total_rx;
    ++mgmd_stat_service[arrayIdx][portId].igmpquery.source_query_total_rx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpquery.source_query_total_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_AND_SOURCE_SPECIFIC_QUERY_VALID_RX:
    ++mgmd_stat_port[portId].igmpquery.source_query_valid_rx;
    ++mgmd_stat_service[arrayIdx][portId].igmpquery.source_query_valid_rx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpquery.source_query_valid_rx;
    break;

   case SNOOP_STAT_FIELD_GROUP_AND_SOURCE_SPECIFIC_QUERY_DROPPED_RX:
    ++mgmd_stat_port[portId].igmpquery.source_query_dropped_rx;
    ++mgmd_stat_service[arrayIdx][portId].igmpquery.source_query_dropped_rx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpquery.source_query_dropped_rx;
    break;

  case SNOOP_STAT_FIELD_JOINS_SENT:
    ++mgmd_stat_port[portId].joins_sent;
    ++mgmd_stat_service[arrayIdx][portId].joins_sent;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].joins_sent;
    break;

  case SNOOP_STAT_FIELD_JOINS_RECEIVED_SUCCESS:
    ++mgmd_stat_port[portId].joins_received_success;
    ++mgmd_stat_service[arrayIdx][portId].joins_received_success;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].joins_received_success;
    break;

  case SNOOP_STAT_FIELD_JOINS_RECEIVED_FAILED:
    ++mgmd_stat_port[portId].joins_received_failed;
    ++mgmd_stat_service[arrayIdx][portId].joins_received_failed;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].joins_received_failed;
    break;

  case SNOOP_STAT_FIELD_LEAVES_SENT:
    ++mgmd_stat_port[portId].leaves_sent;
    ++mgmd_stat_service[arrayIdx][portId].leaves_sent;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].leaves_sent;
    break;

  case SNOOP_STAT_FIELD_LEAVES_RECEIVED:
    ++mgmd_stat_port[portId].leaves_received;
    ++mgmd_stat_service[arrayIdx][portId].leaves_received;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].leaves_received;
    break;

  case SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_TX:
    ++mgmd_stat_port[portId].igmpv3.membership_report_tx;    
    ++mgmd_stat_service[arrayIdx][portId].igmpv3.membership_report_tx;    
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpv3.membership_report_tx;    
    break;

  case SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_TOTAL_RX:
    ++mgmd_stat_port[portId].igmpv3.membership_report_total_rx;    
    ++mgmd_stat_service[arrayIdx][portId].igmpv3.membership_report_total_rx;    
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpv3.membership_report_total_rx;    
    break;

  case SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_VALID_RX:
    ++mgmd_stat_port[portId].igmpv3.membership_report_valid_rx;    
    ++mgmd_stat_service[arrayIdx][portId].igmpv3.membership_report_valid_rx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpv3.membership_report_valid_rx;
    break;

  case SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_INVALID_RX:
    ++mgmd_stat_port[portId].igmpv3.membership_report_invalid_rx;
    ++mgmd_stat_service[arrayIdx][portId].igmpv3.membership_report_invalid_rx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpv3.membership_report_invalid_rx;
    break;

  case SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_DROPPED_RX:
    ++mgmd_stat_port->igmpv3.membership_report_dropped_rx;
    ++mgmd_stat_service[arrayIdx][portId].igmpv3.membership_report_dropped_rx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpv3.membership_report_dropped_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_ALLOW_NEW_SOURCES_TX:
    ++mgmd_stat_port[portId].igmpv3.group_record.allow_tx;
    ++mgmd_stat_service[arrayIdx][portId].igmpv3.group_record.allow_tx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpv3.group_record.allow_tx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_ALLOW_NEW_SOURCES_TOTAL_RX:
    ++mgmd_stat_port[portId].igmpv3.group_record.allow_total_rx;
    ++mgmd_stat_service[arrayIdx][portId].igmpv3.group_record.allow_total_rx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpv3.group_record.allow_total_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_ALLOW_NEW_SOURCES_VALID_RX:
    ++mgmd_stat_port[portId].igmpv3.group_record.allow_valid_rx;
    ++mgmd_stat_service[arrayIdx][portId].igmpv3.group_record.allow_valid_rx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpv3.group_record.allow_valid_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_ALLOW_NEW_SOURCES_INVALID_RX:
    ++mgmd_stat_port[portId].igmpv3.group_record.allow_invalid_rx;
    ++mgmd_stat_service[arrayIdx][portId].igmpv3.group_record.allow_invalid_rx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpv3.group_record.allow_invalid_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_ALLOW_NEW_SOURCES_DROPPED_RX:
    ++mgmd_stat_port[portId].igmpv3.group_record.allow_dropped_rx;
    ++mgmd_stat_service[arrayIdx][portId].igmpv3.group_record.allow_dropped_rx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpv3.group_record.allow_dropped_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_BLOCK_OLD_SOURCES_TX:
    ++mgmd_stat_port[portId].igmpv3.group_record.block_tx;
    ++mgmd_stat_service[arrayIdx][portId].igmpv3.group_record.block_tx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpv3.group_record.block_tx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_BLOCK_OLD_SOURCES_TOTAL_RX:
    ++mgmd_stat_port[portId].igmpv3.group_record.block_total_rx;
    ++mgmd_stat_service[arrayIdx][portId].igmpv3.group_record.block_total_rx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpv3.group_record.block_total_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_BLOCK_OLD_SOURCES_VALID_RX:
    ++mgmd_stat_port[portId].igmpv3.group_record.block_valid_rx;
    ++mgmd_stat_service[arrayIdx][portId].igmpv3.group_record.block_valid_rx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpv3.group_record.block_valid_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_BLOCK_OLD_SOURCES_INVALID_RX:
    ++mgmd_stat_port[portId].igmpv3.group_record.block_invalid_rx;
    ++mgmd_stat_service[arrayIdx][portId].igmpv3.group_record.block_invalid_rx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpv3.group_record.block_invalid_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_BLOCK_OLD_SOURCES_DROPPED_RX:
    ++mgmd_stat_port[portId].igmpv3.group_record.block_dropped_rx;
    ++mgmd_stat_service[arrayIdx][portId].igmpv3.group_record.block_dropped_rx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpv3.group_record.block_dropped_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_INCLUDE_TX:
    ++mgmd_stat_port[portId].igmpv3.group_record.to_include_tx;
    ++mgmd_stat_service[arrayIdx][portId].igmpv3.group_record.to_include_tx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpv3.group_record.to_include_tx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_INCLUDE_TOTAL_RX:
    ++mgmd_stat_port[portId].igmpv3.group_record.to_include_total_rx;
    ++mgmd_stat_service[arrayIdx][portId].igmpv3.group_record.to_include_total_rx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpv3.group_record.to_include_total_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_INCLUDE_VALID_RX:
    ++mgmd_stat_port[portId].igmpv3.group_record.to_include_valid_rx;
    ++mgmd_stat_service[arrayIdx][portId].igmpv3.group_record.to_include_valid_rx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpv3.group_record.to_include_valid_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_INCLUDE_INVALID_RX:
    ++mgmd_stat_port[portId].igmpv3.group_record.to_include_invalid_rx;
    ++mgmd_stat_service[arrayIdx][portId].igmpv3.group_record.to_include_invalid_rx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpv3.group_record.to_include_invalid_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_INCLUDE_DROPPED_RX:
    ++mgmd_stat_port[portId].igmpv3.group_record.to_include_dropped_rx;
    ++mgmd_stat_service[arrayIdx][portId].igmpv3.group_record.to_include_dropped_rx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpv3.group_record.to_include_dropped_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_EXCLUDE_TX:
    ++mgmd_stat_port[portId].igmpv3.group_record.to_exclude_tx;
    ++mgmd_stat_service[arrayIdx][portId].igmpv3.group_record.to_exclude_tx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpv3.group_record.to_exclude_tx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_EXCLUDE_TOTAL_RX:
    ++mgmd_stat_port[portId].igmpv3.group_record.to_exclude_total_rx;
    ++mgmd_stat_service[arrayIdx][portId].igmpv3.group_record.to_exclude_total_rx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpv3.group_record.to_exclude_total_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_EXCLUDE_VALID_RX:
    ++mgmd_stat_port[portId].igmpv3.group_record.to_exclude_valid_rx;
    ++mgmd_stat_service[arrayIdx][portId].igmpv3.group_record.to_exclude_valid_rx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpv3.group_record.to_exclude_valid_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_EXCLUDE_INVALID_RX:
    ++mgmd_stat_port[portId].igmpv3.group_record.to_exclude_invalid_rx;
    ++mgmd_stat_service[arrayIdx][portId].igmpv3.group_record.to_exclude_invalid_rx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpv3.group_record.to_exclude_invalid_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_EXCLUDE_DROPPED_RX:
    ++mgmd_stat_port[portId].igmpv3.group_record.to_exclude_dropped_rx;
    ++mgmd_stat_service[arrayIdx][portId].igmpv3.group_record.to_exclude_dropped_rx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpv3.group_record.to_exclude_dropped_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_INCLUDE_TX:
    ++mgmd_stat_port[portId].igmpv3.group_record.is_include_tx;
    ++mgmd_stat_service[arrayIdx][portId].igmpv3.group_record.is_include_tx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpv3.group_record.is_include_tx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_INCLUDE_TOTAL_RX:
    ++mgmd_stat_port[portId].igmpv3.group_record.is_include_total_rx;
    ++mgmd_stat_service[arrayIdx][portId].igmpv3.group_record.is_include_total_rx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpv3.group_record.is_include_total_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_INCLUDE_VALID_RX:
    ++mgmd_stat_port[portId].igmpv3.group_record.is_include_valid_rx;
    ++mgmd_stat_service[arrayIdx][portId].igmpv3.group_record.is_include_valid_rx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpv3.group_record.is_include_valid_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_INCLUDE_INVALID_RX:
    ++mgmd_stat_port[portId].igmpv3.group_record.is_include_invalid_rx;
    ++mgmd_stat_service[arrayIdx][portId].igmpv3.group_record.is_include_invalid_rx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpv3.group_record.is_include_invalid_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_INCLUDE_DROPPED_RX:
    ++mgmd_stat_port[portId].igmpv3.group_record.is_include_dropped_rx;
    ++mgmd_stat_service[arrayIdx][portId].igmpv3.group_record.is_include_dropped_rx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpv3.group_record.is_include_dropped_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_EXCLUDE_TX:
    ++mgmd_stat_port[portId].igmpv3.group_record.is_exclude_tx;
    ++mgmd_stat_service[arrayIdx][portId].igmpv3.group_record.is_exclude_tx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpv3.group_record.is_exclude_tx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_EXCLUDE_TOTAL_RX:
    ++mgmd_stat_port[portId].igmpv3.group_record.is_exclude_total_rx;
    ++mgmd_stat_service[arrayIdx][portId].igmpv3.group_record.is_exclude_total_rx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpv3.group_record.is_exclude_total_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_EXCLUDE_VALID_RX:
    ++mgmd_stat_port[portId].igmpv3.group_record.is_exclude_valid_rx;
    ++mgmd_stat_service[arrayIdx][portId].igmpv3.group_record.is_exclude_valid_rx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpv3.group_record.is_exclude_valid_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_EXCLUDE_INVALID_RX:
    ++mgmd_stat_port[portId].igmpv3.group_record.is_exclude_invalid_rx;
    ++mgmd_stat_service[arrayIdx][portId].igmpv3.group_record.is_exclude_invalid_rx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpv3.group_record.is_exclude_invalid_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_EXCLUDE_DROPPED_RX:
    ++mgmd_stat_port[portId].igmpv3.group_record.is_exclude_dropped_rx;
    ++mgmd_stat_service[arrayIdx][portId].igmpv3.group_record.is_exclude_dropped_rx;
    if((clientId != ((uint32)-1))) ++mgmd_stat_client[portId][clientId].igmpv3.group_record.is_exclude_dropped_rx;
    break;
 
  default:
    break;
  }

  return SUCCESS;
}

/**
 * Increment client MGMD statistics
 * 
 * @param portId    : interface where the packet entered
 * @param clientId  : Client ID
 * @param field     : field to increment
 * 
 * @return RC_t 
 */
RC_t ptin_mgmd_stat_increment_clientOnly(uint32 portId, uint32 clientId, ptin_snoop_stat_enum_t field)
{
  // Validations
  if ( (portId >= PTIN_MGMD_MAX_PORTS) || ((clientId != ((uint32)-1)) && (clientId > PTIN_MGMD_MAX_CLIENTS)) || (clientId == ((uint32)-1)) )
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Abnormal context [portId:%u clientId:%u]", portId, clientId);
    return FAILURE;
  }

  switch (field) {
  case SNOOP_STAT_FIELD_ACTIVE_GROUPS:
    ++mgmd_stat_client[portId][clientId].active_groups;
    break;

  case SNOOP_STAT_FIELD_ACTIVE_CLIENTS:
    ++mgmd_stat_client[portId][clientId].active_clients;
    break;

  case SNOOP_STAT_FIELD_IGMP_SENT:
    ++mgmd_stat_client[portId][clientId].igmp_sent;
    break;

  case SNOOP_STAT_FIELD_IGMP_TX_FAILED:
    ++mgmd_stat_client[portId][clientId].igmp_tx_failed;
    break;

  case SNOOP_STAT_FIELD_IGMP_INTERCEPTED:
    ++mgmd_stat_client[portId][clientId].igmp_intercepted;
    break;

  case SNOOP_STAT_FIELD_IGMP_DROPPED:
    ++mgmd_stat_client[portId][clientId].igmp_dropped;
    break;

  case SNOOP_STAT_FIELD_IGMP_RECEIVED_VALID:
    ++mgmd_stat_client[portId][clientId].igmp_received_valid;
    break;

  case SNOOP_STAT_FIELD_IGMP_RECEIVED_INVALID:
    ++mgmd_stat_client[portId][clientId].igmp_received_invalid;
    break;

  case SNOOP_STAT_FIELD_GENERIC_QUERY_INVALID_RX:  
    ++mgmd_stat_client[portId][clientId].igmpquery.generic_query_invalid_rx;
    break;

  case SNOOP_STAT_FIELD_GENERAL_QUERY_TX:
    ++mgmd_stat_client[portId][clientId].igmpquery.general_query_tx;
    break;

  case SNOOP_STAT_FIELD_GENERAL_QUERY_TOTAL_RX:
    ++mgmd_stat_client[portId][clientId].igmpquery.general_query_total_rx;
    break;

  case SNOOP_STAT_FIELD_GENERAL_QUERY_VALID_RX:
    ++mgmd_stat_client[portId][clientId].igmpquery.general_query_valid_rx;
    break;

  case SNOOP_STAT_FIELD_GENERAL_QUERY_DROPPED_RX:
    ++mgmd_stat_client[portId][clientId].igmpquery.general_query_dropped_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_SPECIFIC_QUERY_TX:
    ++mgmd_stat_client[portId][clientId].igmpquery.group_query_tx;
    break;

  case SNOOP_STAT_FIELD_GROUP_SPECIFIC_QUERY_TOTAL_RX:
    ++mgmd_stat_client[portId][clientId].igmpquery.group_query_total_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_SPECIFIC_QUERY_VALID_RX:
    ++mgmd_stat_client[portId][clientId].igmpquery.group_query_valid_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_SPECIFIC_QUERY_DROPPED_RX:
    ++mgmd_stat_client[portId][clientId].igmpquery.group_query_dropped_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_AND_SOURCE_SPECIFIC_QUERY_TX:
    ++mgmd_stat_client[portId][clientId].igmpquery.source_query_tx;
    break;

  case SNOOP_STAT_FIELD_GROUP_AND_SOURCE_SPECIFIC_QUERY_TOTAL_RX:
    ++mgmd_stat_client[portId][clientId].igmpquery.source_query_total_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_AND_SOURCE_SPECIFIC_QUERY_VALID_RX:
    ++mgmd_stat_client[portId][clientId].igmpquery.source_query_valid_rx;
    break;

   case SNOOP_STAT_FIELD_GROUP_AND_SOURCE_SPECIFIC_QUERY_DROPPED_RX:
    ++mgmd_stat_client[portId][clientId].igmpquery.source_query_dropped_rx;
    break;

  case SNOOP_STAT_FIELD_JOINS_SENT:
    ++mgmd_stat_client[portId][clientId].joins_sent;
    break;

  case SNOOP_STAT_FIELD_JOINS_RECEIVED_SUCCESS:
    ++mgmd_stat_client[portId][clientId].joins_received_success;
    break;

  case SNOOP_STAT_FIELD_JOINS_RECEIVED_FAILED:
    ++mgmd_stat_client[portId][clientId].joins_received_failed;
    break;

  case SNOOP_STAT_FIELD_LEAVES_SENT:
    ++mgmd_stat_client[portId][clientId].leaves_sent;
    break;

  case SNOOP_STAT_FIELD_LEAVES_RECEIVED:
    ++mgmd_stat_client[portId][clientId].leaves_received;
    break;

  case SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_TX:
    ++mgmd_stat_client[portId][clientId].igmpv3.membership_report_tx;    
    break;

  case SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_TOTAL_RX:
    ++mgmd_stat_client[portId][clientId].igmpv3.membership_report_total_rx;    
    break;

  case SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_VALID_RX:
    ++mgmd_stat_client[portId][clientId].igmpv3.membership_report_valid_rx;
    break;

  case SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_INVALID_RX:
    ++mgmd_stat_client[portId][clientId].igmpv3.membership_report_invalid_rx;
    break;

  case SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_DROPPED_RX:
    ++mgmd_stat_client[portId][clientId].igmpv3.membership_report_dropped_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_ALLOW_NEW_SOURCES_TX:
    ++mgmd_stat_client[portId][clientId].igmpv3.group_record.allow_tx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_ALLOW_NEW_SOURCES_TOTAL_RX:
    ++mgmd_stat_client[portId][clientId].igmpv3.group_record.allow_total_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_ALLOW_NEW_SOURCES_VALID_RX:
    ++mgmd_stat_client[portId][clientId].igmpv3.group_record.allow_valid_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_ALLOW_NEW_SOURCES_INVALID_RX:
    ++mgmd_stat_client[portId][clientId].igmpv3.group_record.allow_invalid_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_ALLOW_NEW_SOURCES_DROPPED_RX:
    ++mgmd_stat_client[portId][clientId].igmpv3.group_record.allow_dropped_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_BLOCK_OLD_SOURCES_TX:
    ++mgmd_stat_client[portId][clientId].igmpv3.group_record.block_tx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_BLOCK_OLD_SOURCES_TOTAL_RX:
    ++mgmd_stat_client[portId][clientId].igmpv3.group_record.block_total_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_BLOCK_OLD_SOURCES_VALID_RX:
    ++mgmd_stat_client[portId][clientId].igmpv3.group_record.block_valid_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_BLOCK_OLD_SOURCES_INVALID_RX:
    ++mgmd_stat_client[portId][clientId].igmpv3.group_record.block_invalid_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_BLOCK_OLD_SOURCES_DROPPED_RX:
    ++mgmd_stat_client[portId][clientId].igmpv3.group_record.block_dropped_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_INCLUDE_TX:
    ++mgmd_stat_client[portId][clientId].igmpv3.group_record.to_include_tx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_INCLUDE_TOTAL_RX:
    ++mgmd_stat_client[portId][clientId].igmpv3.group_record.to_include_total_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_INCLUDE_VALID_RX:
    ++mgmd_stat_client[portId][clientId].igmpv3.group_record.to_include_valid_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_INCLUDE_INVALID_RX:
    ++mgmd_stat_client[portId][clientId].igmpv3.group_record.to_include_invalid_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_INCLUDE_DROPPED_RX:
    ++mgmd_stat_client[portId][clientId].igmpv3.group_record.to_include_dropped_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_EXCLUDE_TX:
    ++mgmd_stat_client[portId][clientId].igmpv3.group_record.to_exclude_tx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_EXCLUDE_TOTAL_RX:
    ++mgmd_stat_client[portId][clientId].igmpv3.group_record.to_exclude_total_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_EXCLUDE_VALID_RX:
    ++mgmd_stat_client[portId][clientId].igmpv3.group_record.to_exclude_valid_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_EXCLUDE_INVALID_RX:
    ++mgmd_stat_client[portId][clientId].igmpv3.group_record.to_exclude_invalid_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_TO_EXCLUDE_DROPPED_RX:
    ++mgmd_stat_client[portId][clientId].igmpv3.group_record.to_exclude_dropped_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_INCLUDE_TX:
    ++mgmd_stat_client[portId][clientId].igmpv3.group_record.is_include_tx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_INCLUDE_TOTAL_RX:
    ++mgmd_stat_client[portId][clientId].igmpv3.group_record.is_include_total_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_INCLUDE_VALID_RX:
    ++mgmd_stat_client[portId][clientId].igmpv3.group_record.is_include_valid_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_INCLUDE_INVALID_RX:
    ++mgmd_stat_client[portId][clientId].igmpv3.group_record.is_include_invalid_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_INCLUDE_DROPPED_RX:
    ++mgmd_stat_client[portId][clientId].igmpv3.group_record.is_include_dropped_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_EXCLUDE_TX:
    ++mgmd_stat_client[portId][clientId].igmpv3.group_record.is_exclude_tx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_EXCLUDE_TOTAL_RX:
    ++mgmd_stat_client[portId][clientId].igmpv3.group_record.is_exclude_total_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_EXCLUDE_VALID_RX:
    ++mgmd_stat_client[portId][clientId].igmpv3.group_record.is_exclude_valid_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_EXCLUDE_INVALID_RX:
    ++mgmd_stat_client[portId][clientId].igmpv3.group_record.is_exclude_invalid_rx;
    break;

  case SNOOP_STAT_FIELD_GROUP_RECORD_IS_EXCLUDE_DROPPED_RX:
    ++mgmd_stat_client[portId][clientId].igmpv3.group_record.is_exclude_dropped_rx;
    break;
 
  default:
    break;
  }

  return SUCCESS;
}


/**
 * Decrement MGMD statistics
 * 
 * @param portId    : interface where the packet entered
 * @param serviceId : Service ID
 * @param clientId  : Client ID
 * @param field     : field to increment
 * 
 * @return RC_t 
 */
RC_t ptin_mgmd_stat_decrement_field(uint32 portId, uint32 serviceId, uint32 clientId, ptin_snoop_stat_enum_t field)
{
  int16 arrayIdx;

  // Validations
  if ( (portId >= PTIN_MGMD_MAX_PORTS) || (serviceId > PTIN_MGMD_MAX_SERVICE_ID) || ((clientId != ((uint32)-1)) && (clientId > PTIN_MGMD_MAX_CLIENTS)) )
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Abnormal context [portId:%u serviceId:%u clientId:%u]", portId, serviceId, clientId);
    return FAILURE;
  }

  // Get the index that holds the requested service ID. If not found, get a free index
  if(SUCCESS != ptin_mgmd_stats_service_find(serviceId, &arrayIdx))
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to find requested service[%u]", serviceId);
    return FAILURE;
  }

  switch (field)
  {
    case SNOOP_STAT_FIELD_ACTIVE_GROUPS:
      --mgmd_stat_port[portId].active_groups;
      --mgmd_stat_service[arrayIdx][portId].active_groups;
      if((clientId != ((uint32)-1))) --mgmd_stat_client[portId][clientId].active_groups;
      break;

    case SNOOP_STAT_FIELD_ACTIVE_CLIENTS:
      --mgmd_stat_port[portId].active_clients;
      --mgmd_stat_service[arrayIdx][portId].active_clients;
      if((clientId != ((uint32)-1))) --mgmd_stat_client[portId][clientId].active_clients;
      break;

    default:
      break;
  }

  return SUCCESS;
}


/**
 * Get IGMP statistics for a given client
 * 
 * @param portId      : Port Id
 * @param clientId    : Client Id
 * @param clientStats : Statistics
 * 
 * @return RC_t
 */
RC_t ptin_mgmd_stat_client_get(uint32 portId, uint32 clientId, ptin_IGMP_Statistics_t *clientStats)
{
  // Validations
  if ( (portId >= PTIN_MGMD_MAX_PORTS) || (clientId > PTIN_MGMD_MAX_CLIENTS) )
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Abnormal context [portId:%u clientId:%u]", portId, clientId);
    return FAILURE;
  }

  memcpy(clientStats, &mgmd_stat_client[portId][clientId], sizeof(ptin_IGMP_Statistics_t));

  return SUCCESS;
}


/**
 * Clear IGMP statistics for a given client
 * 
 * @param portId   : Port Id
 * @param clientId : Client Id
 * 
 * @return RC_t
 */
RC_t ptin_mgmd_stat_client_clear(uint32 portId, uint32 clientId)
{
  uint32 activeGroups, activeClients;

  // Validations
  if ( (portId >= PTIN_MGMD_MAX_PORTS) || (clientId > PTIN_MGMD_MAX_CLIENTS) )
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Abnormal context [portId:%u clientId:%u]", portId, clientId);
    return FAILURE;
  }

  activeGroups  = mgmd_stat_client[portId][clientId].active_groups;
  activeClients = mgmd_stat_client[portId][clientId].active_clients;
  memset(&mgmd_stat_client[portId][clientId], 0x00, sizeof(ptin_IGMP_Statistics_t));
  mgmd_stat_client[portId][clientId].active_groups  = activeGroups;
  mgmd_stat_client[portId][clientId].active_clients = activeClients;

  return SUCCESS;
}


/**
 * Get IGMP statistics for a given interface/service
 * 
 * @param serviceId      : Service Id
 * @param interfaceId    : Interface Id
 * @param interfaceStats : Statistics
 * 
 * @return RC_t
 */
RC_t ptin_mgmd_stat_intf_get(uint32 serviceId, uint32 interfaceId, ptin_IGMP_Statistics_t *interfaceStats)
{
  int16 arrayIdx;

  // Validations
  if ( (serviceId > PTIN_MGMD_MAX_SERVICE_ID) || (interfaceId >= PTIN_MGMD_MAX_PORTS) )
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Abnormal context [serviceId:%u interfaceId:%u]", serviceId, interfaceId);
    return FAILURE;
  }

  // Get the index that holds the requested service ID. If not found, get a free index
  if(SUCCESS != ptin_mgmd_stats_service_find(serviceId, &arrayIdx))
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to find requested service[%u]", serviceId);
    memset(interfaceStats, 0x00, sizeof(ptin_IGMP_Statistics_t));
    return SUCCESS;
  }

  memcpy(interfaceStats, &mgmd_stat_service[arrayIdx][interfaceId], sizeof(ptin_IGMP_Statistics_t));

  return SUCCESS;
}


/**
 * Clear IGMP statistics for a given interface/service
 * 
 * @param serviceId      : Service Id
 * @param portId    : Interface Id
 * @param interfaceStats : Statistics
 * 
 * @return RC_t
 */
RC_t ptin_mgmd_stat_intf_clear(uint32 serviceId, uint32 portId)
{
  uint32 activeGroups, activeClients;
  int16 arrayIdx;

  // Validations
  if ( (serviceId > PTIN_MGMD_MAX_SERVICE_ID) || (portId >= PTIN_MGMD_MAX_PORTS) )
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Abnormal context [serviceId:%u interfaceId:%u]", serviceId, portId);
    return FAILURE;
  }

  // Get the index that holds the requested service ID. If not found, get a free index
  if(SUCCESS != ptin_mgmd_stats_service_find(serviceId, &arrayIdx))
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to find requested service[%u]", serviceId);
    return SUCCESS;
  }

  activeGroups  = mgmd_stat_service[arrayIdx][portId].active_groups;
  activeClients = mgmd_stat_service[arrayIdx][portId].active_clients;
  memset(&mgmd_stat_service[arrayIdx][portId], 0x00, sizeof(ptin_IGMP_Statistics_t));
  mgmd_stat_service[arrayIdx][portId].active_groups  = activeGroups;
  mgmd_stat_service[arrayIdx][portId].active_clients = activeClients;

  return SUCCESS;
}

