/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/cli_show_running_config_filter.c
 *
 * @purpose show running config commands for static filter for the the cli
 *
 * @component user interface
 *
 * @comments
 *
 * @create  04/20/2008
 *
 * @author  akulkarn
 * @end
 *
 **********************************************************************/
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "l7_common.h"
#include "filter_exports.h"
#include "cli_web_exports.h"
#include "usmdb_util_api.h"
#include "comm_mask.h"
#include "cliapi.h"
#include "config_script_api.h"
#include "datatypes.h"
#include "cli_show_running_config.h"

#include "clicommands_filter.h"
#include "usmdb_filter_api.h"



/*********************************************************************
 * @purpose  To print the running configuration of macfilter Info
 *
 * @param    EwsContext ewsContext
 * @param    L7_uint32 unit
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 @end
 *********************************************************************/

L7_RC_t cliRunningConfigMacfilterInfo(EwsContext ewsContext, L7_uint32 unit)
{
  static L7_uint32 vlanId;
  static L7_uchar8 mac[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];

  memset ( mac, 0, sizeof(mac) );
  /* Configuring static filters for multicast addresses with destionation ports is supported
   * on all platforms */
  if ( usmDbFilterFirstGet(unit, mac, &vlanId) == L7_SUCCESS)
  {
    do
    {
      osapiSnprintf(buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X",
          mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

      osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_Macfilter_1, buf, vlanId);
      EWSWRITEBUFFER(ewsContext, stat);
    }
    while(usmDbFilterNextGet(unit, mac, vlanId, mac, &vlanId) == L7_SUCCESS);
  }

  return L7_SUCCESS;
}
/*********************************************************************
 * @purpose  To print the running configuration of macfilter Info for Physical Intf
 *
 * @param    EwsContext ewsContext
 * @param    L7_uint32 unit
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 @end
 *********************************************************************/

L7_RC_t cliRunningConfigMacfilterPhyIntfInfo(EwsContext ewsContext, L7_uint32 unit, L7_uint32 interface)
{
  static L7_uint32 vlanId;
  static L7_uchar8 mac[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 srcCount = 0;
  L7_uint32 destCount = 0;
  L7_uint32 srcIntfList[L7_FILTER_MAX_INTF];
  L7_uint32 destIntfList[L7_FILTER_MAX_INTF];
  L7_RC_t rc1 = L7_FAILURE;
  L7_RC_t rc2 = L7_FAILURE;
  L7_BOOL foundIntfInList = L7_FALSE;
  L7_uint32 i=0;

  memset ( mac, 0, sizeof(mac) );
  if ( usmDbFilterFirstGet(unit, mac, &vlanId) == L7_SUCCESS)
  {
    if(usmDbFilterIsIntfInAnyFilter(unit, interface) == L7_TRUE)
    {
      do
      {
        rc1 = usmDbFilterSrcIntfListGet(unit, mac, vlanId, &srcCount, srcIntfList);
        rc2 = usmDbFilterDstIntfListGet(unit, mac, vlanId, &destCount, destIntfList);

        osapiSnprintf(buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X",
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

        if (usmDbFeaturePresentCheck(unit, L7_FILTER_COMPONENT_ID, L7_FILTER_SOURCE_PORT_FILTERING_FEATURE_ID) == L7_TRUE)
        {
          if( rc1 == L7_SUCCESS && srcCount != L7_NULL)
          {
            foundIntfInList = L7_FALSE;
            for(i=0; i<srcCount; i++)
            {
              if (srcIntfList[i] == interface)
              {
                foundIntfInList = L7_TRUE;
                break;
              }
            }
            if (foundIntfInList == L7_TRUE)
            {
              osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_MacfilterAddsrc, buf, vlanId);
              EWSWRITEBUFFER(ewsContext, stat);
            }
          }
        }

        /*destination port configuration for multicast mac addresses is supported for all */
        if( rc2 == L7_SUCCESS && destCount != L7_NULL)
        {
          foundIntfInList = L7_FALSE;
          for(i=0; i<destCount; i++)
          {
            if (destIntfList[i] == interface)
            {
              foundIntfInList = L7_TRUE;
              break;
            }
          }
          if (foundIntfInList == L7_TRUE)
          {
            osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_MacfilterAdddest, buf, vlanId);
            EWSWRITEBUFFER(ewsContext, stat);
          }
        }
      }
      while(usmDbFilterNextGet(unit, mac, vlanId, mac, &vlanId) == L7_SUCCESS);
    }
  }
  return L7_SUCCESS;
}
