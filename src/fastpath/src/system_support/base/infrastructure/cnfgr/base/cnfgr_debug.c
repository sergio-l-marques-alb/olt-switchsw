/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* Name: cnfgr_debug.c
*
* Purpose: Configurator component debug functions.
*
* Component: Configurator (cnfgr)
*
* Comments:
*
* Created by: msmith 06/26/2003
*
*********************************************************************/
#include "cnfgr_include.h"
#include "l7_product.h"
#include "platform_config.h"

static L7_BOOL bannerPrinted = L7_FALSE;

/*********************************************************************
* @purpose  This function initializes Debug module. The use of this
*           function is mandatory. This function is a Debug interface.
*
* @param    None
*
* @returns  L7_SUCCESS - always
*
*
* @notes    None
*
*
* @end
*********************************************************************/
L7_RC_t cnfgrDebugInitialize(void)
{
  cnfgrTraceInit(CNFGR_TRACE_ENTRY_MAX, CNFGR_TRACE_ENTRY_SIZE_MAX);
  cnfgrProfileInit();
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function terminates the Debug module. The use of this
*           function is optional. This function is a Debug interface.
*
* @param    None
*
* @returns  None.
*
* @notes    None
*
*
* @end
*********************************************************************/
void cnfgrDebugFini(void)
{
  L7_RC_t rc;
  L7_CNFGR_CMD_DATA_t cmdData,
           *pCmdData = &cmdData;

  cnfgrApiSystemStartupReasonSet(L7_STARTUP_AUTO_COLD);

  /* Initiate the unconfiguration cycle.
  */
  pCmdData->cbHandle         = L7_CNFGR_NO_HANDLE;
  pCmdData->command          = L7_CNFGR_CMD_TERMINATE;
  pCmdData->correlator       = L7_NULL;
  pCmdData->type             = L7_CNFGR_EVNT;
  pCmdData->u.evntData.event = L7_CNFGR_EVNT_T_START;
  pCmdData->u.evntData.data  = L7_LAST_COMPONENT_ID;

  rc = cnfgrApiCommand(pCmdData);
  if (rc != L7_SUCCESS)
  {
    L7_LOG_ERROR(rc);
  }
}

/*********************************************************************
* @purpose  This function dumps information about a component.
*
* @param    cid - @a{(input)} Component id
*
* @returns  nothing
*
* @notes    none
*
* @end
*********************************************************************/
void cnfgrDebugCnfgrDataShow(L7_COMPONENT_IDS_t cid)
{
  L7_CNFGR_QUERY_DATA_t queryData;
  L7_CNFGR_QUERY_RQST_t request;
  L7_RC_t rc;
  L7_uchar8 *component_name;
  L7_CNFGR_QUERY_DATA_t *query = &queryData;

  memset((void *) query, 0, sizeof(L7_CNFGR_QUERY_DATA_t));

  if ( bannerPrinted == L7_FALSE )
  {
    sysapiPrintf("\n");
    sysapiPrintf("Component            CurS NxtS SvdS Mode Stat Cmd Rqst RC aRsp \n");
    sysapiPrintf("-------------------- ---- ---- ---- ---- ---- --- ---- -- -----\n");
    bannerPrinted = L7_TRUE;
  }

  query->type = L7_CNFGR_QRY_TYPE_SINGLE;
  query->args.cid = cid;
  query->request = L7_CNFGR_QRY_RQST_PRESENT;
  rc = cnfgrApiQuery(query);

  component_name = cnfgrSidComponentNameGet(cid);

  if ( rc == L7_SUCCESS && query->data.present == L7_FALSE )
  {
    sysapiPrintf("*** %s (%d) Not Present ***", component_name, cid);
  }
  else if ( rc == L7_FAILURE )
  {
    sysapiPrintf("*** Error fetching metadata for %s (%d):  ", component_name, cid);
    switch ( query->reason )
    {
      case L7_CNFGR_ERR_RC_INTERNAL:
        sysapiPrintf("internal error");
        break;
      case L7_CNFGR_ERR_RC_INVALID_ID:
        sysapiPrintf("invalid id");
        break;
      case L7_CNFGR_ERR_RC_INVALID_RSQT:
        sysapiPrintf("invalid request");
        break;
      case L7_CNFGR_ERR_RC_INVALID_DATA:
        sysapiPrintf("invalid data");
        break;
      case L7_CNFGR_ERR_RC_NOT_FOUND:
        sysapiPrintf("no component");
        break;
      default:
        sysapiPrintf("?");
        break;
    }
    sysapiPrintf(" ***");
  }
  else
  {
    memset((void *) query, 0, sizeof(L7_CNFGR_QUERY_DATA_t));
    sysapiPrintf("%-21s", component_name);

    for ( request = L7_CNFGR_QRY_RQST_FIRST+1; request < L7_CNFGR_QRY_RQST_LAST; request++ )
    {
      query->type = L7_CNFGR_QRY_TYPE_SINGLE;
      query->args.cid = cid;
      query->request = request;
      query->data.name = component_name;
      if ( cnfgrApiQuery(query) == L7_SUCCESS )
      {
        if ( &(query->data) == L7_NULLPTR )
        {
          continue;
        }
        switch ( request )
        {
          case L7_CNFGR_QRY_RQST_STATE:
            {
              L7_uint32 i;
              #define CNFGR_DBG_NUM_STATES sizeof(states) / sizeof(L7_CNFGR_STATE_t)
              const L7_CNFGR_STATE_t states[] =
              { query->data.state.current,
                query->data.state.next,
                query->data.state.saved
              };
              for ( i = 0; i < CNFGR_DBG_NUM_STATES; i++ )
              {
                if ( states[i] > L7_CNFGR_STATE_NULL && states[i] < L7_CNFGR_STATE_LAST )
                {
                  switch ( states[i] )
                  {
                    case L7_CNFGR_STATE_IDLE:
                      sysapiPrintf("%-5s", "Idl");
                      break;
                    case L7_CNFGR_STATE_P1:
                      sysapiPrintf("%-5s", "P1");
                      break;
                    case L7_CNFGR_STATE_P2:
                      sysapiPrintf("%-5s", "P2");
                      break;
                    case L7_CNFGR_STATE_P3:
                      sysapiPrintf("%-5s", "P3");
                      break;
                    case L7_CNFGR_STATE_WMU:
                      sysapiPrintf("%-5s", "WMU");
                      break;
                    case L7_CNFGR_STATE_E:
                      sysapiPrintf("%-5s", "E");
                      break;
                    case L7_CNFGR_STATE_U1:
                      sysapiPrintf("%-5s", "U1");
                      break;
                    case L7_CNFGR_STATE_U2:
                      sysapiPrintf("%-5s", "U2");
                      break;
                    case L7_CNFGR_STATE_T:
                      sysapiPrintf("%-5s", "T");
                      break;
                    case L7_CNFGR_STATE_S:
                      sysapiPrintf("%-5s", "S");
                      break;
                    case L7_CNFGR_STATE_R:
                      sysapiPrintf("%-5s", "R");
                      break;
                    default:
                      sysapiPrintf("%-5s", "?");
                      break;
                  }
                }
                else
                {
                  sysapiPrintf("%-5s", "!");
                }
              }
            }
            break;

          case L7_CNFGR_QRY_RQST_PRESENT:
            break;

          case L7_CNFGR_QRY_RQST_MODE:
            switch ( query->data.mode )
            {
              case L7_CNFGR_COMPONENT_ENABLE:
                sysapiPrintf("%-5s", "E");
                break;
              case L7_CNFGR_COMPONENT_DISABLE:
                sysapiPrintf("%-5s", "D");
                break;
              default:
                sysapiPrintf("%-5s", "?");
                break;
            }
            break;

          case L7_CNFGR_QRY_RQST_STATUS:
            switch ( query->data.status )
            {
              case L7_CNFGR_COMPONENT_ACTIVE:
                sysapiPrintf("%-5s", "A");
                break;
              case L7_CNFGR_COMPONENT_INACTIVE:
                sysapiPrintf("%-5s", "I");
                break;
              default:
                sysapiPrintf("%-5s", "?");
                break;
            }
            break;

          case L7_CNFGR_QRY_RQST_CMDRQST:
            {
              if ( query->data.cmdRqst.cmd > L7_CNFGR_CMD_FIRST && query->data.cmdRqst.cmd < L7_CNFGR_CMD_LAST )
              {
                switch ( query->data.cmdRqst.cmd )
                {
                  case L7_CNFGR_CMD_INITIALIZE:
                    sysapiPrintf("%-4s", "I");
                    break;
                  case L7_CNFGR_CMD_EXECUTE:
                    sysapiPrintf("%-4s", "E");
                    break;
                  case L7_CNFGR_CMD_UNCONFIGURE:
                    sysapiPrintf("%-4s", "U");
                    break;
                  case L7_CNFGR_CMD_TERMINATE:
                    sysapiPrintf("%-4s", "T");
                    break;
                  case L7_CNFGR_CMD_SUSPEND:
                    sysapiPrintf("%-4s", "S");
                    break;
                  case L7_CNFGR_CMD_RESUME:
                    sysapiPrintf("%-4s", "R");
                    break;
                  default:
                    sysapiPrintf("%-4s", "?");
                    break;
                }
              }
              else
              {
                sysapiPrintf("%-4s", "!");
              }
              if ( query->data.cmdRqst.rqst > L7_CNFGR_RQST_FIRST && query->data.cmdRqst.rqst < L7_CNFGR_RQST_LAST )
              {
                switch ( query->data.cmdRqst.rqst )
                {
                  case L7_CNFGR_RQST_I_PHASE1_START:
                    sysapiPrintf("%-5s", "IP1");
                    break;
                  case L7_CNFGR_RQST_I_PHASE2_START:
                    sysapiPrintf("%-5s", "IP2");
                    break;
                  case L7_CNFGR_RQST_I_PHASE3_START:
                    sysapiPrintf("%-5s", "IP3");
                    break;
                  case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                    sysapiPrintf("%-5s", "WMU");
                    break;
                  case L7_CNFGR_RQST_E_START:
                    sysapiPrintf("%-5s", "E");
                    break;
                  case L7_CNFGR_RQST_U_PHASE1_START:
                    sysapiPrintf("%-5s", "UP1");
                    break;
                  case L7_CNFGR_RQST_U_PHASE2_START:
                    sysapiPrintf("%-5s", "UP2");
                    break;
                  case L7_CNFGR_RQST_T_START:
                    sysapiPrintf("%-5s", "T");
                    break;
                  case L7_CNFGR_RQST_S_START:
                    sysapiPrintf("%-5s", "S");
                    break;
                  case L7_CNFGR_RQST_R_START:
                    sysapiPrintf("%-5s", "R");
                    break;
                  default:
                    sysapiPrintf("%-5s", "?");
                    break;
                }
              }
              else
              {
                sysapiPrintf("%-5s", "!");
              }
              if ( query->data.cmdRqst.aRsp.rc == L7_SUCCESS )
              {
                sysapiPrintf("%-3s", "S");
                switch ( query->data.cmdRqst.aRsp.u.response )
                {
                  case L7_CNFGR_CMD_COMPLETE:
                    sysapiPrintf("%-5s", "Cmd");
                    break;
                  case L7_CNFGR_INIT_COMPLETE:
                    sysapiPrintf("%-5s", "Init");
                    break;
                  default:
                    sysapiPrintf("%-5s", "?");
                    break;
                }
              }
              else
              {
                sysapiPrintf("%-3s", "E");
                if ( query->data.cmdRqst.aRsp.u.reason > L7_CNFGR_ERR_RC_FIRST && query->data.cmdRqst.aRsp.u.reason < L7_CNFGR_ERR_RC_LAST )
                {
                  switch ( query->data.cmdRqst.aRsp.u.reason )
                  {
                    case L7_CNFGR_ERR_RC_FATAL:
                      sysapiPrintf("%-5s", "Fatal");
                      break;
                    case L7_CNFGR_ERR_RC_LACK_OF_RESOURCES:
                      sysapiPrintf("%-5s", "LOR");
                      break;
                    case L7_CNFGR_ERR_RC_NOT_AVAILABLE:
                      sysapiPrintf("%-5s", "NA");
                      break;
                    case L7_CNFGR_ERR_RC_NOT_FOUND:
                      sysapiPrintf("%-5s", "NF");
                      break;
                    case L7_CNFGR_ERR_RC_BUSY:
                      sysapiPrintf("%-5s", "Busy");
                      break;
                    case L7_CNFGR_ERR_RC_IGNORED:
                      sysapiPrintf("%-5s", "Ignrd");
                      break;
                    case L7_CNFGR_ERR_RC_INTERNAL:
                      sysapiPrintf("%-5s", "Ntrnl");
                      break;
                    case L7_CNFGR_ERR_RC_INVALID_ID:
                      sysapiPrintf("%-5s", "IID");
                      break;
                    case L7_CNFGR_ERR_RC_INVALID_DATA:
                      sysapiPrintf("%-5s", "IData");
                      break;
                    case L7_CNFGR_ERR_RC_INVALID_RSQT:
                      sysapiPrintf("%-5s", "IRqst");
                      break;
                    case L7_CNFGR_ERR_RC_INVALID_EVNT:
                      sysapiPrintf("%-5s", "IEvnt");
                      break;
                    case L7_CNFGR_ERR_RC_INVALID_HANDLE:
                      sysapiPrintf("%-5s", "IHdl");
                      break;
                    case L7_CNFGR_ERR_RC_INVALID_CMD:
                      sysapiPrintf("%-5s", "ICmd");
                      break;
                    case L7_CNFGR_ERR_RC_INVALID_CMD_TYPE:
                      sysapiPrintf("%-5s", "ICMDT");
                      break;
                    case L7_CNFGR_ERR_RC_INVALID_PAIR:
                      sysapiPrintf("%-5s", "IPair");
                      break;
                    case L7_CNFGR_ERR_RC_OUT_OF_SEQUENCE:
                      sysapiPrintf("%-5s", "OOS");
                      break;
                    default:
                      sysapiPrintf("%-5s", "?");
                      break;
                  }
                }
                else
                {
                  sysapiPrintf("%-5s", "!");
                }
              }
            }
            break;

          default:
            break;
        }
      }
    }

    memset((void *) query, 0, sizeof(L7_CNFGR_QUERY_DATA_t));
  }

  sysapiPrintf("\n");
  bannerPrinted = L7_FALSE;
}

/*********************************************************************
* @purpose  This function dumps information about all components.
*
* @param    nothing
*
* @returns  nothing
*
* @notes    none
*
* @end
*********************************************************************/
void cnfgrDebugCnfgrDataShowAll()
{
  L7_COMPONENT_IDS_t component;

  for ( component = L7_FIRST_COMPONENT_ID+1; component < L7_LAST_COMPONENT_ID; component++ )
  {
    cnfgrDebugCnfgrDataShow(component);
    bannerPrinted = L7_TRUE;
  }

  bannerPrinted = L7_FALSE;
}

/*********************************************************************
* @purpose  This function puts the system into the unconfigured state.
*
* @param    nothing
*
* @returns  nothing
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t cnfgrDebugUnconfigure()
{
 L7_RC_t rc;
 L7_CNFGR_CMD_DATA_t cmdData,
          *pCmdData = &cmdData;

 cnfgrApiSystemStartupReasonSet(L7_STARTUP_AUTO_COLD);

 /* Initiate the unconfiguration cycle.
 */
 pCmdData->cbHandle         = L7_CNFGR_NO_HANDLE;
 pCmdData->command          = L7_CNFGR_CMD_UNCONFIGURE;
 pCmdData->correlator       = L7_NULL;
 pCmdData->type             = L7_CNFGR_EVNT;
 pCmdData->u.evntData.event = L7_CNFGR_EVNT_U_START;
 pCmdData->u.evntData.data  = L7_LAST_COMPONENT_ID;

 rc = cnfgrApiCommand(pCmdData);
 if (rc != L7_SUCCESS)
 {
   L7_LOG_ERROR(rc);
 }

 /* The system will come back up without further intervention.
 */

 return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Show values from l7_product.h. This will show any overrides that may
*           be in effect.
*
* @param    nothing
*
* @returns  nothing
*
* @notes    none
*
* @end
*********************************************************************/
void cnfgrDebugProductShow()
{
  sysapiPrintf("L7_MAX_STACK_PORTS_PER_UNIT: %d\n", L7_MAX_STACK_PORTS_PER_UNIT);
  sysapiPrintf("L7_MAX_UNITS_PER_STACK: %d\n", L7_MAX_UNITS_PER_STACK);
  sysapiPrintf("L7_MAX_PORT_COUNT: %d\n", L7_MAX_PORT_COUNT);
  sysapiPrintf("L7_MIN_FRAME_SIZE: %d\n", L7_MIN_FRAME_SIZE);
  sysapiPrintf("L7_MAX_NUM_LAG_INTF: %d\n", L7_MAX_NUM_LAG_INTF);
  sysapiPrintf("L7_MFDB_MAX_MAC_ENTRIES: %d\n", L7_MFDB_MAX_MAC_ENTRIES);
  sysapiPrintf("L7_MAX_FDB_STATIC_FILTER_ENTRIES: %d\n", L7_MAX_FDB_STATIC_FILTER_ENTRIES);
  sysapiPrintf("L7_VLAN_IPSUBNET_MAX_VLANS: %d\n", L7_VLAN_IPSUBNET_MAX_VLANS);
  sysapiPrintf("L7_VLAN_MAC_MAX_VLANS: %d\n", L7_VLAN_MAC_MAX_VLANS);
  sysapiPrintf("L7_MAX_VLANS: %d\n", L7_MAX_VLANS);
  sysapiPrintf("L7_DOT1P_NUM_TRAFFIC_CLASSES: %d\n", L7_DOT1P_NUM_TRAFFIC_CLASSES);
  sysapiPrintf("L7_MAX_MEMBERS_PER_LAG: %d\n", L7_MAX_MEMBERS_PER_LAG);
  sysapiPrintf("L7_MAX_FRAME_SIZE: %d\n", L7_MAX_FRAME_SIZE);
  sysapiPrintf("L7_MAX_FDB_MAC_ENTRIES: %d\n", L7_MAX_FDB_MAC_ENTRIES);
  sysapiPrintf("L7_DOT1S_ASYNC_STATE_SET: %d\n", L7_DOT1S_ASYNC_STATE_SET);
  sysapiPrintf("L7_DHCP_SNOOPING_MAX_STATIC_ENTRIES: %d\n", L7_DHCP_SNOOPING_MAX_STATIC_ENTRIES);
  sysapiPrintf("L7_DOT1X_MAX_USERS: %d\n", L7_DOT1X_MAX_USERS);
  sysapiPrintf("L7_FDB_IVL_STORE_SELF_ADDRESSES: %d\n", L7_FDB_IVL_STORE_SELF_ADDRESSES);
  sysapiPrintf("L7_FDB_MIN_AGING_TIMEOUT: %d\n", L7_FDB_MIN_AGING_TIMEOUT);
  sysapiPrintf("L7_FDB_MAX_AGING_TIMEOUT: %d\n", L7_FDB_MAX_AGING_TIMEOUT);
  sysapiPrintf("L7_ACL_MAX_LISTS: %d\n", L7_ACL_MAX_LISTS);
  sysapiPrintf("L7_ACL_MAX_RULES: %d\n", L7_ACL_MAX_RULES);
  sysapiPrintf("L7_ACL_MAX_RULES_PER_LIST: %d\n", L7_ACL_MAX_RULES_PER_LIST);
  sysapiPrintf("L7_ACL_VLAN_MAX_COUNT: %d\n", L7_ACL_VLAN_MAX_COUNT);
  sysapiPrintf("L7_ACL_MAX_RULES_PER_MULTILIST: %d\n", L7_ACL_MAX_RULES_PER_MULTILIST);
  sysapiPrintf("L7_ACL_LOG_RULE_LIMIT: %d\n", L7_ACL_LOG_RULE_LIMIT);
  sysapiPrintf("L7_ACL_LOG_RULE_PER_INTF_DIR_LIMIT: %d\n", L7_ACL_LOG_RULE_PER_INTF_DIR_LIMIT);
  sysapiPrintf("L7_DIFFSERV_RULE_PER_CLASS_LIM: %d\n", L7_DIFFSERV_RULE_PER_CLASS_LIM);
  sysapiPrintf("L7_DIFFSERV_INST_PER_POLICY_LIM: %d\n", L7_DIFFSERV_INST_PER_POLICY_LIM);
  sysapiPrintf("L7_DIFFSERV_ATTR_PER_INST_LIM: %d\n", L7_DIFFSERV_ATTR_PER_INST_LIM);
  sysapiPrintf("L7_DIFFSERV_SERVICE_INTF_LIM: %d\n", L7_DIFFSERV_SERVICE_INTF_LIM);
  sysapiPrintf("L7_DIFFSERV_CLASS_LIM: %d\n", L7_DIFFSERV_CLASS_LIM);
  sysapiPrintf("L7_DIFFSERV_POLICY_LIM: %d\n", L7_DIFFSERV_POLICY_LIM);
  sysapiPrintf("L7_MAX_CFG_QUEUES_PER_PORT: %d\n", L7_MAX_CFG_QUEUES_PER_PORT);
  sysapiPrintf("L7_MAX_CFG_DROP_PREC_LEVELS: %d\n", L7_MAX_CFG_DROP_PREC_LEVELS);
  sysapiPrintf("L7_IPV6_NHRES_MAX: %d\n", L7_IPV6_NHRES_MAX);
  sysapiPrintf("L7_L3_NUM_IP_ADDRS: %d\n", L7_L3_NUM_IP_ADDRS);
  sysapiPrintf("L7_L3_NUM_SECONDARIES: %d\n", L7_L3_NUM_SECONDARIES);
  sysapiPrintf("L7_MAX_6TO4_NEXTHOPS: %d\n", L7_MAX_6TO4_NEXTHOPS);
  sysapiPrintf("L7_MAX_NUM_LOOPBACK_INTF: %d\n", L7_MAX_NUM_LOOPBACK_INTF);
  sysapiPrintf("L7_MAX_NUM_TUNNEL_INTF: %d\n", L7_MAX_NUM_TUNNEL_INTF);
  sysapiPrintf("L7_MAX_NUM_VLAN_INTF: %d\n", L7_MAX_NUM_VLAN_INTF);
  sysapiPrintf("L7_MAX_NUM_WIRELESS_INTF: %d\n", L7_MAX_NUM_WIRELESS_INTF);
  sysapiPrintf("L7_MAX_NUM_CAPWAP_TUNNEL_INTF: %d\n", L7_MAX_NUM_CAPWAP_TUNNEL_INTF);
  sysapiPrintf("L7_MAX_NUM_ROUTER_INTF: %d\n", L7_MAX_NUM_ROUTER_INTF);
  sysapiPrintf("Maximum ARP cache entries: %d\n", platRtrArpMaxEntriesGet());
#ifdef   PROD_L3_ARP_CACHE_STATIC_MAX
  sysapiPrintf("L7_L3_ARP_CACHE_STATIC_MAX: %d\n", L7_L3_ARP_CACHE_STATIC_MAX);
#endif
#ifdef L7_IPV6_PACKAGE
  sysapiPrintf("L7_IPV6_ROUTE_TBL_SIZE_TOTAL: %d\n", platRtrIpv6RouteMaxEntriesGet());
  sysapiPrintf("Maximum IPv6 NDP entries: %d\n", platRtrIpv6NdpMaxEntriesGet());
#endif
  sysapiPrintf("Maximum IPv4 routes: %d\n", platRtrRouteMaxEntriesGet());
  sysapiPrintf("Maximum ECMP next hops: %d\n", platRtrRouteMaxEqualCostEntriesGet());
  sysapiPrintf("L7_MULTICAST_V4_HEAP_SIZE: %d\n", (L7_int32)L7_MULTICAST_V4_HEAP_SIZE);
  sysapiPrintf("L7_MULTICAST_V6_HEAP_SIZE: %d\n", (L7_int32)L7_MULTICAST_V6_HEAP_SIZE);
  sysapiPrintf("L7_PIMSM_MAX_STATIC_RP_NUM: %d\n", L7_PIMSM_MAX_STATIC_RP_NUM);
  sysapiPrintf("L7_PIMSM_MAX_SSM_RANGE: %d\n", L7_PIMSM_MAX_SSM_RANGE);
  sysapiPrintf("L7_PIMSM_MAX_CAND_RP_NUM: %d\n", L7_PIMSM_MAX_CAND_RP_NUM);
  sysapiPrintf("L7_PIMSM_MAX_RP_GRP_ENTRIES: %d\n", L7_PIMSM_MAX_RP_GRP_ENTRIES);
  sysapiPrintf("L7_PIMSM_MAX_PER_SCOPE_BSR_NODES: %d\n", L7_PIMSM_MAX_PER_SCOPE_BSR_NODES);
  sysapiPrintf("L7_PIMSM_MAX_NBR: %d\n", L7_PIMSM_MAX_NBR);
  sysapiPrintf("L7_PIMDM_MAX_NBR: %d\n", L7_PIMDM_MAX_NBR);
  sysapiPrintf("L7_DVMRP_MAX_NBR: %d\n", L7_DVMRP_MAX_NBR);
  sysapiPrintf("L7_DVMRP_MAX_L3_TABLE_SIZE: %d\n", L7_DVMRP_MAX_L3_TABLE_SIZE);
  sysapiPrintf("L7_MGMD_MAX_QUERY_SOURCES: %d\n", L7_MGMD_MAX_QUERY_SOURCES);
  sysapiPrintf("L7_MULTICAST_FIB_MAX_ENTRIES: %d\n", L7_MULTICAST_FIB_MAX_ENTRIES);
  sysapiPrintf("L7_PIMSM_MAX_S_G_RPT_IPV4_TABLE_SIZE: %d\n", platIpv4McastRoutesMaxGet());
  sysapiPrintf("L7_PIMSM_MAX_S_G_RPT_IPV6_TABLE_SIZE: %d\n", platIpv6McastRoutesMaxGet());
  sysapiPrintf("L7_PIMSM_MAX_S_G_IPV4_TABLE_SIZE: %d\n", platIpv4McastRoutesMaxGet());
  sysapiPrintf("L7_PIMSM_MAX_S_G_IPV6_TABLE_SIZE: %d\n", platIpv6McastRoutesMaxGet());
  sysapiPrintf("L7_PIMSM_MAX_STAR_G_IPV4_TABLE_SIZE: %d\n", platIpv4McastRoutesMaxGet());
  sysapiPrintf("L7_PIMSM_MAX_STAR_G_IPV6_TABLE_SIZE: %d\n", platIpv6McastRoutesMaxGet());
  sysapiPrintf("L7_PIMDM_MAX_MRT_IPV4_TABLE_SIZE: %d\n", platIpv4McastRoutesMaxGet());
  sysapiPrintf("L7_PIMDM_MAX_MRT_IPV6_TABLE_SIZE: %d\n", platIpv6McastRoutesMaxGet());
  sysapiPrintf("L7_DVMRP_MAX_MRT_IPV4_TABLE_SIZE: %d\n", L7_DVMRP_MAX_MRT_IPV4_TABLE_SIZE);
  sysapiPrintf("L7_MGMD_V4_HEAP_SIZE: %d\n", L7_MGMD_V4_HEAP_SIZE);
  sysapiPrintf("L7_MGMD_V6_HEAP_SIZE: %d\n", (L7_int32)L7_MGMD_V6_HEAP_SIZE);
  sysapiPrintf("L7_MGMD_GROUPS_MAX_ENTRIES: %d\n", L7_MGMD_GROUPS_MAX_ENTRIES);
  sysapiPrintf("L7_MGMD_PROXY_MRT_MAX_IPV4_ROUTE_ENTRIES: %d\n", platIpv4McastRoutesMaxGet());
  sysapiPrintf("L7_MGMD_PROXY_MRT_MAX_IPV6_ROUTE_ENTRIES: %d\n", platIpv6McastRoutesMaxGet());
  sysapiPrintf("L7_PIM_NUM_OPTIMAL_OUTGOING_INTERFACES: %d\n", L7_PIM_NUM_OPTIMAL_OUTGOING_INTERFACES);
  sysapiPrintf("L7_MULTICAST_MAX_IP_MTU: %d\n", L7_MULTICAST_MAX_IP_MTU);
  sysapiPrintf("L7_RTR_MAX_STATIC_MROUTES: %d\n", L7_RTR_MAX_STATIC_MROUTES);
  sysapiPrintf("L7_MCAST_MAX_ADMINSCOPE_ENTRIES: %d\n", L7_MCAST_MAX_ADMINSCOPE_ENTRIES);
  sysapiPrintf("L7_DOT1S_BPDUFLOOD_SET: %d\n", L7_DOT1S_BPDUFLOOD_SET);
  sysapiPrintf("L7_LOGICAL_UNIT: %d\n", L7_LOGICAL_UNIT);
  sysapiPrintf("L7_MAX_LOGICAL_PORTS_PER_SLOT: %d\n", L7_MAX_LOGICAL_PORTS_PER_SLOT);
  sysapiPrintf("L7_MAX_LOGICAL_SLOTS_PER_UNIT: %d\n", L7_MAX_LOGICAL_SLOTS_PER_UNIT);
  sysapiPrintf("L7_MAX_CPU_PORTS_PER_SLOT: %d\n", L7_MAX_CPU_PORTS_PER_SLOT);
  sysapiPrintf("L7_MAX_CPU_SLOTS_PER_UNIT: %d\n", L7_MAX_CPU_SLOTS_PER_UNIT);
  sysapiPrintf("L7_MAX_NUM_STACK_INTF: %d\n", L7_MAX_NUM_STACK_INTF);
  sysapiPrintf("L7_MAX_SUPPORTED_PHYSICAL_CARD_TYPES: %d\n", L7_MAX_SUPPORTED_PHYSICAL_CARD_TYPES);
  sysapiPrintf("L7_PERSISTENT_LOG_SUPPORTED: %d\n", L7_PERSISTENT_LOG_SUPPORTED);
  sysapiPrintf("L7_SYSAPI_FEATURE_KEYING_SUPPORTED: %d\n", L7_SYSAPI_FEATURE_KEYING_SUPPORTED);
  sysapiPrintf("L7_DHCPS_MAX_POOL_NUMBER: %d\n", L7_DHCPS_MAX_POOL_NUMBER);
  sysapiPrintf("L7_DHCPS_MAX_LEASE_NUMBER: %d\n", L7_DHCPS_MAX_LEASE_NUMBER);
  sysapiPrintf("L7_DHCPS_MAX_ASYNC_MSGS: %d\n", L7_DHCPS_MAX_ASYNC_MSGS);
  sysapiPrintf("FD_SSHD_MAX_SESSIONS: %d\n", FD_SSHD_MAX_SESSIONS);
  sysapiPrintf("FD_TELNET_DEFAULT_MAX_SESSIONS: %d\n", FD_TELNET_DEFAULT_MAX_SESSIONS);
  sysapiPrintf("FD_HTTP_DEFAULT_MAX_CONNECTIONS: %d\n", FD_HTTP_DEFAULT_MAX_CONNECTIONS);
  sysapiPrintf("FD_CLI_WEB_DEFAULT_NUM_SESSIONS: %d\n", FD_CLI_WEB_DEFAULT_NUM_SESSIONS);
  sysapiPrintf("FD_SECURE_HTTP_DEFAULT_MAX_CONNECTIONS: %d\n", FD_SECURE_HTTP_DEFAULT_MAX_CONNECTIONS);
  sysapiPrintf("L7_DOT1P_NUM_TRAFFIC_CLASSES_INIT: %d\n", L7_DOT1P_NUM_TRAFFIC_CLASSES_INIT);
  sysapiPrintf("L7_DOT1P_DEFAULT_USER_PRIORITY: %d\n", L7_DOT1P_DEFAULT_USER_PRIORITY);
  sysapiPrintf("L7_MAX_NUM_DYNAMIC_LAG: %d\n", L7_MAX_NUM_DYNAMIC_LAG);
  sysapiPrintf("L7_MAX_MULTIPLE_STP_INSTANCES: %d\n", L7_MAX_MULTIPLE_STP_INSTANCES);
  sysapiPrintf("L7_MAX_VLAN_PER_BRIDGE: %d\n", L7_MAX_VLAN_PER_BRIDGE);
  sysapiPrintf("L7_MACLOCKING_MAX_STATIC_ADDRESSES: %d\n", L7_MACLOCKING_MAX_STATIC_ADDRESSES);
  sysapiPrintf("L7_MACLOCKING_MAX_DYNAMIC_ADDRESSES: %d\n", L7_MACLOCKING_MAX_DYNAMIC_ADDRESSES);
  sysapiPrintf("L7_PROTECTED_PORT_MAX_GROUPS: %d\n", L7_PROTECTED_PORT_MAX_GROUPS);
  sysapiPrintf("L7_LLDP_MAX_ENTRIES_PER_PORT: %d\n", L7_LLDP_MAX_ENTRIES_PER_PORT);
  sysapiPrintf("L7_ISCSI_MAX_TARGET_TCP_PORTS: %d\n", L7_ISCSI_MAX_TARGET_TCP_PORTS);
  sysapiPrintf("L7_ISCSI_MAX_SESSIONS: %d\n", L7_ISCSI_MAX_SESSIONS);
  sysapiPrintf("L7_ISCSI_MAX_CONNECTIONS: %d\n", L7_ISCSI_MAX_CONNECTIONS);
  sysapiPrintf("L7_PLATFORM_ROUTING_VRRP_MIN_VRID: %d\n", L7_PLATFORM_ROUTING_VRRP_MIN_VRID);
  sysapiPrintf("L7_PLATFORM_ROUTING_VRRP_MAX_VRID: %d\n", L7_PLATFORM_ROUTING_VRRP_MAX_VRID);
  sysapiPrintf("L7_RTR_MAX_SUBNETS: %d\n", L7_RTR_MAX_SUBNETS);
  sysapiPrintf("L7_RTR_MAX_RTR_INTERFACES: %d\n", L7_RTR_MAX_RTR_INTERFACES);
  sysapiPrintf("L7_NUM_RESERVED_VLANS: %d\n", L7_NUM_RESERVED_VLANS);
  sysapiPrintf("L7_PLATFORM_MAX_VLAN_ID: %d\n", L7_PLATFORM_MAX_VLAN_ID);
  sysapiPrintf("L7_MULTICAST_FIB_ENTRY_LIFETIME: %d\n", L7_MULTICAST_FIB_ENTRY_LIFETIME);
  sysapiPrintf("L7_MULTICAST_FIB_UPCALL_LIFETIME: %d\n", L7_MULTICAST_FIB_UPCALL_LIFETIME);
  sysapiPrintf("L7_WIRELESS_MAX_PEER_GROUP_ACCESS_POINTS: %d\n", L7_WIRELESS_MAX_PEER_GROUP_ACCESS_POINTS);
  sysapiPrintf("L7_WIRELESS_MAX_PEER_GROUP_PARTICIPATION_APS: %d\n", L7_WIRELESS_MAX_PEER_GROUP_PARTICIPATION_APS);
  sysapiPrintf("L7_WIRELESS_MAX_PEER_SWITCHES: %d\n", L7_WIRELESS_MAX_PEER_SWITCHES);
  sysapiPrintf("L7_WIRELESS_MAX_ACCESS_POINTS: %d\n", L7_WIRELESS_MAX_ACCESS_POINTS);
  sysapiPrintf("L7_WIRELESS_MAX_CLIENTS: %d\n", L7_WIRELESS_MAX_CLIENTS);
  sysapiPrintf("L7_WIRELESS_MAX_CLIENTS_PER_RADIO: %d\n", L7_WIRELESS_MAX_CLIENTS_PER_RADIO);
  sysapiPrintf("L7_WIRELESS_NV_IMAGES_NVRAM_SIZE: %d\n", L7_WIRELESS_NV_IMAGES_NVRAM_SIZE);
  sysapiPrintf("L7_CAPTIVE_PORTAL_IMAGES_NVRAM_SIZE: %d\n", L7_CAPTIVE_PORTAL_IMAGES_NVRAM_SIZE);
  sysapiPrintf("L7_L3_TUNNEL_MAX_COUNT: %d\n", L7_L3_TUNNEL_MAX_COUNT);
  sysapiPrintf("L7_WIRELESS_TSPEC_TS_POOL_MAX: %d\n", L7_WIRELESS_TSPEC_TS_POOL_MAX);
  sysapiPrintf("L7_MAX_LINE_MODULES_PER_UNIT: %d\n", L7_MAX_LINE_MODULES_PER_UNIT);
  sysapiPrintf("L7_MAX_POWER_MODULES_PER_UNIT: %d\n", L7_MAX_POWER_MODULES_PER_UNIT);
  sysapiPrintf("L7_MAX_FAN_MODULES_PER_UNIT: %d\n", L7_MAX_FAN_MODULES_PER_UNIT);
  sysapiPrintf("L7_MAX_CONTROL_MODULES_PER_UNIT: %d\n", L7_MAX_CONTROL_MODULES_PER_UNIT);
  sysapiPrintf("L7_MAX_PHYSICAL_SLOTS_PER_CHASSIS: %d\n", L7_MAX_PHYSICAL_SLOTS_PER_CHASSIS);
  sysapiPrintf("L7_MAX_SLOTS_PER_CHASSIS: %d\n", L7_MAX_SLOTS_PER_CHASSIS);
  sysapiPrintf("L7_MAX_STACK_PORTS_PER_CFM: %d\n", L7_MAX_STACK_PORTS_PER_CFM);
  sysapiPrintf("L7_TS_MPLS_FTN_TBL_SIZE_TOTAL: %d\n", L7_TS_MPLS_FTN_TBL_SIZE_TOTAL);
  sysapiPrintf("L7_TS_MPLS_ILM_TBL_SIZE_TOTAL: %d\n", L7_TS_MPLS_ILM_TBL_SIZE_TOTAL);
  sysapiPrintf("L7_MAX_SLOTS_PER_UNIT: %d\n", L7_MAX_SLOTS_PER_UNIT);
  sysapiPrintf("L7_LAG_SLOT_NUM: %d\n", L7_LAG_SLOT_NUM);
  sysapiPrintf("L7_VLAN_SLOT_NUM: %d\n", L7_VLAN_SLOT_NUM);
  sysapiPrintf("L7_CPU_SLOT_NUM: %d\n", L7_CPU_SLOT_NUM);
  sysapiPrintf("L7_LOOPBACK_SLOT_NUM: %d\n", L7_LOOPBACK_SLOT_NUM);
  sysapiPrintf("L7_TUNNEL_SLOT_NUM: %d\n", L7_TUNNEL_SLOT_NUM);
  sysapiPrintf("L7_WIRELESS_SLOT_NUM: %d\n", L7_WIRELESS_SLOT_NUM);
  sysapiPrintf("L7_CAPWAP_TUNNEL_SLOT_NUM: %d\n", L7_CAPWAP_TUNNEL_SLOT_NUM);
  sysapiPrintf("L7_NUM_WIRELESS_INTFS: %d\n", L7_NUM_WIRELESS_INTFS);
  sysapiPrintf("L7_MAX_INTERFACE_COUNT: %d\n", L7_MAX_INTERFACE_COUNT);
  sysapiPrintf("L7_ALL_UNITS: %d\n", L7_ALL_UNITS);
  sysapiPrintf("L7_ALL_PHYSICAL_INTERFACES: %d\n", L7_ALL_PHYSICAL_INTERFACES);
  sysapiPrintf("DEFAULT_MSG_COUNT: %d\n", DEFAULT_MSG_COUNT);
  sysapiPrintf("DEFAULT_MSG_COUNT_MED: %d\n", DEFAULT_MSG_COUNT_MED);
  sysapiPrintf("DEFAULT_MSG_COUNT_HIGH: %d\n", DEFAULT_MSG_COUNT_HIGH);
  sysapiPrintf("USER_MGR_MSG_COUNT: %d\n", USER_MGR_MSG_COUNT);
  sysapiPrintf("FD_DOT1X_MSG_COUNT: %d\n", FD_DOT1X_MSG_COUNT);
  sysapiPrintf("SNOOP_MSG_COUNT: %d\n", SNOOP_MSG_COUNT);
  sysapiPrintf("DOT3AD_SID_MSG_COUNT: %d\n", DOT3AD_SID_MSG_COUNT);
  sysapiPrintf("LLDP_MSG_COUNT: %d\n", LLDP_MSG_COUNT);
  sysapiPrintf("PML_MSG_COUNT: %d\n", PML_MSG_COUNT);
  sysapiPrintf("FD_CNFGR_RADIUS_MSG_COUNT: %d\n", FD_CNFGR_RADIUS_MSG_COUNT);
  sysapiPrintf("FD_CNFGR_TACACS_MSG_COUNT: %d\n", FD_CNFGR_TACACS_MSG_COUNT);
  sysapiPrintf("DHCPSMAP_QUEUE_MAX_MSG_COUNT: %d\n", DHCPSMAP_QUEUE_MAX_MSG_COUNT);
  sysapiPrintf("DTL_MSG_COUNT: %d\n", DTL_MSG_COUNT);
  sysapiPrintf("DTL_ADDR_MSG_COUNT: %d\n", DTL_ADDR_MSG_COUNT);
  sysapiPrintf("HAPI_BROAD_LAG_ASYNC_CMD_QUEUE_SIZE: %d\n", HAPI_BROAD_LAG_ASYNC_CMD_QUEUE_SIZE);
  sysapiPrintf("GARP_MSG_COUNT: %d\n", GARP_MSG_COUNT);
  sysapiPrintf("GARP_PDU_MSG_COUNT: %d\n", GARP_PDU_MSG_COUNT);
  sysapiPrintf("DOT1S_MSG_COUNT: %d\n", DOT1S_MSG_COUNT);
  sysapiPrintf("DOT1AD_MSG_COUNT: %d\n", DOT1AD_MSG_COUNT);
#ifdef L7_AUTO_INSTALL_PACKAGE
  sysapiPrintf("AUTO_INSTALL_MSG_COUNT: %d\n", AUTO_INSTALL_MSG_COUNT);
#endif
  sysapiPrintf("L7_TRAP_MSG_QUEUE_SIZE: %d\n", L7_TRAP_MSG_QUEUE_SIZE);
  sysapiPrintf("ISDP_MSG_COUNT: %d\n", ISDP_MSG_COUNT);
  sysapiPrintf("BOXS_MSG_COUNT: %d\n", BOXS_MSG_COUNT);
  sysapiPrintf("DOT1S_MAX_BPDU_BUF_COUNT: %d\n", DOT1S_MAX_BPDU_BUF_COUNT);
  sysapiPrintf("DOT3AD_NUM_BUFFERS: %d\n", DOT3AD_NUM_BUFFERS);
  sysapiPrintf("L7_LLDP_REM_MGMT_ADDR_BUFFERS: %d\n", L7_LLDP_REM_MGMT_ADDR_BUFFERS);
  sysapiPrintf("L7_LLDP_REM_UNKNOWN_TLV_BUFFERS: %d\n", L7_LLDP_REM_UNKNOWN_TLV_BUFFERS);
  sysapiPrintf("L7_LLDP_REM_ORG_DEF_INFO_BUFFERS: %d\n", L7_LLDP_REM_ORG_DEF_INFO_BUFFERS);
  sysapiPrintf("RADIUS_SM_BUF_NUM: %d\n", RADIUS_SM_BUF_NUM);
  sysapiPrintf("RADIUS_LG_BUF_NUM: %d\n", RADIUS_LG_BUF_NUM);
  sysapiPrintf("DAPI_DEBUG_NUM_BUFFER_MSGS: %d\n", DAPI_DEBUG_NUM_BUFFER_MSGS);
  sysapiPrintf("L7_MAX_NETWORK_BUFF_PER_BOX: %d\n", L7_MAX_NETWORK_BUFF_PER_BOX);
  sysapiPrintf("L7_MBUF_RESERVED_TX_BUFFERS: %d\n", L7_MBUF_RESERVED_TX_BUFFERS);
  sysapiPrintf("L7_MBUF_RESERVED_RX_HI_PRIO_BUFFERS: %d\n", L7_MBUF_RESERVED_RX_HI_PRIO_BUFFERS);
  sysapiPrintf("L7_MBUF_RESERVED_RX_MID0_PRIO_BUFFERS: %d\n", L7_MBUF_RESERVED_RX_MID0_PRIO_BUFFERS);
  sysapiPrintf("L7_MBUF_RESERVED_RX_MID1_PRIO_BUFFERS: %d\n", L7_MBUF_RESERVED_RX_MID1_PRIO_BUFFERS);
  sysapiPrintf("L7_MBUF_RESERVED_RX_MID2_PRIO_BUFFERS: %d\n", L7_MBUF_RESERVED_RX_MID2_PRIO_BUFFERS);
  sysapiPrintf("L7_DEFAULT_STACK_SIZE: %d\n", L7_DEFAULT_STACK_SIZE);
  sysapiPrintf("L7_DEFAULT_TASK_SLICE: %d\n", L7_DEFAULT_TASK_SLICE);
  sysapiPrintf("L7_DEFAULT_TASK_PRIORITY: %d\n", L7_DEFAULT_TASK_PRIORITY);
  sysapiPrintf("L7_MEDIUM_TASK_PRIORITY: %d\n", L7_MEDIUM_TASK_PRIORITY);
  sysapiPrintf("L7_LOG_IN_MEMORY_LOG_COUNT: %d\n", L7_LOG_IN_MEMORY_LOG_COUNT);
  sysapiPrintf("OSAPI_MAX_TIMERS: %d\n", OSAPI_MAX_TIMERS);
  sysapiPrintf("L7_MAX_BCM_DEVICES_PER_UNIT: %d\n", L7_MAX_BCM_DEVICES_PER_UNIT);
  sysapiPrintf("L7_MAX_BCM_PORTS_PER_DEVICE: %d\n", L7_MAX_BCM_PORTS_PER_DEVICE);
  sysapiPrintf("SNOOP_QUERIER_VLAN_MAX: %d\n", SNOOP_QUERIER_VLAN_MAX);
  sysapiPrintf("FD_RTR_MAX_STATIC_ROUTES: %d\n", FD_RTR_MAX_STATIC_ROUTES);
  sysapiPrintf("FD_RTR6_MAX_STATIC_ROUTES: %d\n", FD_RTR6_MAX_STATIC_ROUTES);
  sysapiPrintf("FD_RTR_MAX_STATIC_ARP_ENTRIES: %d\n", FD_RTR_MAX_STATIC_ARP_ENTRIES);
  sysapiPrintf("SCROLL_BUF_LINES: %d\n", SCROLL_BUF_LINES);
  sysapiPrintf("L7_MAX_RMON_INTERFACE_COUNT: %d\n", L7_MAX_RMON_INTERFACE_COUNT);
  sysapiPrintf("L7_RMON_ENTRY_NUM: %d\n", L7_RMON_ENTRY_NUM);
  sysapiPrintf("DAPI_TRACE_ENTRY_MAX: %d\n", DAPI_TRACE_ENTRY_MAX);
  sysapiPrintf("DAPI_TRACE_ENTRY_SIZE_MAX: %d\n", DAPI_TRACE_ENTRY_SIZE_MAX);
  sysapiPrintf("SNOOP_MAX_MRTR_TIMERS: %d\n", SNOOP_MAX_MRTR_TIMERS);
  sysapiPrintf("FD_SIM_NETWORK_CONFIG_MODE: %d\n", FD_SIM_NETWORK_CONFIG_MODE);
  sysapiPrintf("FD_SIM_SERVPORT_CONFIG_MODE: %d\n", FD_SIM_SERVPORT_CONFIG_MODE);
  sysapiPrintf("NIM_TRACE_ENTRY_MAX: %d\n", NIM_TRACE_ENTRY_MAX);
  sysapiPrintf("NIM_TRACE_ENTRY_SIZE_MAX: %d\n", NIM_TRACE_ENTRY_SIZE_MAX);
  sysapiPrintf("L7_TLV_TRACE_ENTRY_MAX: %d\n", L7_TLV_TRACE_ENTRY_MAX);
  sysapiPrintf("L7_TLV_TRACE_ENTRY_SIZE_MAX: %d\n", L7_TLV_TRACE_ENTRY_SIZE_MAX);
  sysapiPrintf("DOT1X_TRACE_ENTRY_MAX: %d\n", DOT1X_TRACE_ENTRY_MAX);
  sysapiPrintf("DOT1X_TRACE_ENTRY_SIZE_MAX: %d\n", DOT1X_TRACE_ENTRY_SIZE_MAX);
  sysapiPrintf("DOT3AD_TRACE_ENTRY_MAX: %d\n", DOT3AD_TRACE_ENTRY_MAX);
  sysapiPrintf("DOT3AD_TRACE_ENTRY_SIZE_MAX: %d\n", DOT3AD_TRACE_ENTRY_SIZE_MAX);
  sysapiPrintf("DOT3AH_TRACE_ENTRY_MAX: %d\n", DOT3AH_TRACE_ENTRY_MAX);
  sysapiPrintf("DOT3AH_TRACE_ENTRY_SIZE_MAX: %d\n", DOT3AH_TRACE_ENTRY_SIZE_MAX);
  sysapiPrintf("L7_MAX_BUFFER_POOLS: %d\n", L7_MAX_BUFFER_POOLS);
  sysapiPrintf("TRACEROUTE_MAX_SESSIONS: %d\n", TRACEROUTE_MAX_SESSIONS);
  sysapiPrintf("TRAPMGR_TABLE_SIZE: %d\n", TRAPMGR_TABLE_SIZE);
  sysapiPrintf("L7_DOT1S_STACK_SIZE: %d\n", L7_DOT1S_STACK_SIZE);
  sysapiPrintf("L7_ISDP_STACK_SIZE: %d\n", L7_ISDP_STACK_SIZE);
  sysapiPrintf("L7_CMGR_CARD_INSERT_REMOVE_QUEUE_MSG_COUNT: %d\n", L7_CMGR_CARD_INSERT_REMOVE_QUEUE_MSG_COUNT);
}
