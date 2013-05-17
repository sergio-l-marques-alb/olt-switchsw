/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename cnfgr_sid.c
*
* @purpose Configurator Structural Initialization Database
*
* @component cnfgr
*
* @comments none
*
* @create 03/21/2003
*
* @author avasquez
*
* @end
*
**********************************************************************/

#include "cnfgr_sid.h"
#include "cnfgr_sid_const.h"
#include "cnfgr_sid_extern.h"

#define L7_SIM_COMPONENT_MNEMONIC                          "SIM"
#define L7_SIM_COMPONENT_NAME                              "SIM"
#define L7_NIM_COMPONENT_MNEMONIC                          "NIM"
#define L7_NIM_COMPONENT_NAME                              "NIM"
#define L7_TRAPMGR_COMPONENT_MNEMONIC                      "TRAPMGR"
#define L7_TRAPMGR_COMPONENT_NAME                          "TRAPMGR"
#define L7_CNFGR_COMPONENT_MNEMONIC                        "CNFGR"
#define L7_CNFGR_COMPONENT_NAME                            "CNFGR"
#define L7_DRIVER_COMPONENT_MNEMONIC                       "DRIVER"
#define L7_DRIVER_COMPONENT_NAME                           "DRIVER"
#define L7_POLICY_COMPONENT_MNEMONIC                       "POLICY"
#define L7_POLICY_COMPONENT_NAME                           "POLICY"
#define L7_DOT1Q_COMPONENT_MNEMONIC                        "DOT1Q"
#define L7_DOT1Q_COMPONENT_NAME                            "DOT1Q"
#define L7_PBVLAN_COMPONENT_MNEMONIC                       "PBVLAN"
#define L7_PBVLAN_COMPONENT_NAME                           "PBVLAN"
#define L7_DOT3AD_COMPONENT_MNEMONIC                       "DOT3AD"
#define L7_DOT3AD_COMPONENT_NAME                           "DOT3AD"
#define L7_STATSMGR_COMPONENT_MNEMONIC                     "STATSMGR"
#define L7_STATSMGR_COMPONENT_NAME                         "STATSMGR"
#define L7_LTS_COMPONENT_MNEMONIC                          "LTS"
#define L7_LTS_COMPONENT_NAME                              "LTS"
#define L7_LAG_COMPONENT_MNEMONIC                          "LAG"
#define L7_LAG_COMPONENT_NAME                              "LAG"
#define L7_USMDB_COMPONENT_MNEMONIC                        "USMDB"
#define L7_USMDB_COMPONENT_NAME                            "USMDB"
#define L7_LAYER3_COMPONENT_MNEMONIC                       "LAYER3"
#define L7_LAYER3_COMPONENT_NAME                           "LAYER3"
#define L7_CUSTOM_COMPONENT_MNEMONIC                       "CUSTOM"
#define L7_CUSTOM_COMPONENT_NAME                           "CUSTOM"
#define L7_FDB_COMPONENT_MNEMONIC                          "FDB"
#define L7_FDB_COMPONENT_NAME                              "FDB"
#define L7_GARP_COMPONENT_MNEMONIC                         "GARP"
#define L7_GARP_COMPONENT_NAME                             "GARP"
#define L7_SNMP_COMPONENT_MNEMONIC                         "SNMP"
#define L7_SNMP_COMPONENT_NAME                             "SNMP"
#define L7_DTL_COMPONENT_MNEMONIC                          "DTL"
#define L7_DTL_COMPONENT_NAME                              "DTL"
#define L7_USER_MGR_COMPONENT_MNEMONIC                     "USER_MGR"
#define L7_USER_MGR_COMPONENT_NAME                         "USER_MGR"
#define L7_CLI_WEB_COMPONENT_MNEMONIC                      "CLI_WEB"
#define L7_CLI_WEB_COMPONENT_NAME                          "CLI_WEB"
#define L7_CMD_LOGGER_COMPONENT_MNEMONIC                   "CMDLOGGER"
#define L7_CMD_LOGGER_COMPONENT_NAME                       "CMD_LOGGER"
#define L7_IP_MAP_COMPONENT_MNEMONIC                       "IP"
#define L7_IP_MAP_COMPONENT_NAME                           "IP"
#define L7_ARP_MAP_COMPONENT_MNEMONIC                      "ARP"
#define L7_ARP_MAP_COMPONENT_NAME                          "ARP"
#define L7_RTR_DISC_COMPONENT_MNEMONIC                     "RTR_DISC"
#define L7_RTR_DISC_COMPONENT_NAME                         "RTR_DISC"
#define L7_OSPF_MAP_COMPONENT_MNEMONIC                     "OSPF"
#define L7_OSPF_MAP_COMPONENT_NAME                         "OSPF"
#define L7_RIP_MAP_COMPONENT_MNEMONIC                      "RIP"
#define L7_RIP_MAP_COMPONENT_NAME                          "RIP"
#define L7_FLEX_MGMD_MAP_COMPONENT_MNEMONIC                "IGMP"
#define L7_FLEX_MGMD_MAP_COMPONENT_NAME                    "IGMP"
#define L7_FLEX_MCAST_MAP_COMPONENT_MNEMONIC               "MCAST"
#define L7_FLEX_MCAST_MAP_COMPONENT_NAME                   "MCAST"
#define L7_FLEX_PIMDM_MAP_COMPONENT_MNEMONIC               "PIMDM"
#define L7_FLEX_PIMDM_MAP_COMPONENT_NAME                   "PIMDM"
#define L7_FLEX_DVMRP_MAP_COMPONENT_MNEMONIC               "DVMRP"
#define L7_FLEX_DVMRP_MAP_COMPONENT_NAME                   "DVMRP"
#define L7_FLEX_PIMSM_MAP_COMPONENT_MNEMONIC               "PIMSM"
#define L7_FLEX_PIMSM_MAP_COMPONENT_NAME                   "PIMSM"
#define L7_VRRP_MAP_COMPONENT_MNEMONIC                     "VRRP"
#define L7_VRRP_MAP_COMPONENT_NAME                         "VRRP"
#define L7_FLEX_PACKAGES_MNEMONIC                          "PACKAGES"
#define L7_FLEX_PACKAGES_NAME                              "PACKAGES"
#define L7_FLEX_MPLS_MAP_COMPONENT_MNEMONIC                "MPLS"
#define L7_FLEX_MPLS_MAP_COMPONENT_NAME                    "MPLS"
#define L7_FLEX_BGP_MAP_COMPONENT_MNEMONIC                 "BGP"
#define L7_FLEX_BGP_MAP_COMPONENT_NAME                     "BGP"
#define L7_FLEX_QOS_ACL_COMPONENT_MNEMONIC                 "ACL"
#define L7_FLEX_QOS_ACL_COMPONENT_NAME                     "QOS_ACL"
#define L7_FLEX_QOS_COS_COMPONENT_MNEMONIC                 "COS"
#define L7_FLEX_QOS_COS_COMPONENT_NAME                     "QOS_COS"
#define L7_FLEX_QOS_DIFFSERV_COMPONENT_MNEMONIC            "DIFFSERV"
#define L7_FLEX_QOS_DIFFSERV_COMPONENT_NAME                "QOS_DIFFSERV"
#define L7_PORT_MIRROR_COMPONENT_MNEMONIC                  "MIRROR"
#define L7_PORT_MIRROR_COMPONENT_NAME                      "PORT_MIRROR"
#define L7_MFDB_COMPONENT_MNEMONIC                         "MFDB"
#define L7_MFDB_COMPONENT_NAME                             "MFDB"
#define L7_FILTER_COMPONENT_MNEMONIC                       "FILTER"
#define L7_FILTER_COMPONENT_NAME                           "FILTER"
#define L7_DHCP_RELAY_COMPONENT_MNEMONIC                   "IP_HELPER"
#define L7_DHCP_RELAY_COMPONENT_NAME                       "IP_HELPER"
#define L7_SNOOPING_COMPONENT_MNEMONIC                     "SNOOP"
#define L7_SNOOPING_COMPONENT_NAME                         "SNOOPING"
#define L7_GMRP_COMPONENT_MNEMONIC                         "GMRP"
#define L7_GMRP_COMPONENT_NAME                             "GMRP"
#define L7_SERVICE_PORT_PRESENT_MNEMONIC                   "SERV_PORT"
#define L7_SERVICE_PORT_PRESENT_NAME                       "SERVICE_PORT_PRESENT"
#define L7_CABLE_TEST_COMPONENT_MNEMONIC                   "CABLETEST"
#define L7_CABLE_TEST_COMPONENT_NAME                       "CABLE_TEST"
#define L7_DOT1S_COMPONENT_MNEMONIC                        "DOT1S"
#define L7_DOT1S_COMPONENT_NAME                            "DOT1S"
#define L7_RADIUS_COMPONENT_MNEMONIC                       "RADIUS"
#define L7_RADIUS_COMPONENT_NAME                           "RADIUS"
#define L7_TACACS_COMPONENT_MNEMONIC                       "TACACS"
#define L7_TACACS_COMPONENT_NAME                           "TACACS"
#define L7_DOT1X_COMPONENT_MNEMONIC                        "DOT1X"
#define L7_DOT1X_COMPONENT_NAME                            "DOT1X"
#define L7_DHCPS_MAP_COMPONENT_MNEMONIC                    "DHCPS"
#define L7_DHCPS_MAP_COMPONENT_NAME                        "DHCPS"
#define L7_DVLANTAG_COMPONENT_MNEMONIC                     "DVLANTAG"
#define L7_DVLANTAG_COMPONENT_NAME                         "DVLANTAG"
#define L7_IPV6_PROVISIONING_COMPONENT_MNEMONIC            "IPV6_PROV"
#define L7_IPV6_PROVISIONING_COMPONENT_NAME                "IPV6_PROVISIONING"
#define L7_FLEX_OPENSSL_COMPONENT_MNEMONIC                 "OPENSSL"
#define L7_FLEX_OPENSSL_COMPONENT_NAME                     "OPENSSL"
#define L7_FLEX_SSLT_COMPONENT_MNEMONIC                    "SSLT"
#define L7_FLEX_SSLT_COMPONENT_NAME                        "SSLT"
#define L7_FLEX_SSHD_COMPONENT_MNEMONIC                    "SSHD"
#define L7_FLEX_SSHD_COMPONENT_NAME                        "SSHD"
#define L7_FLEX_SSHC_COMPONENT_MNEMONIC                    "SSHC"
#define L7_FLEX_SSHC_COMPONENT_NAME                        "SSHC"
#define L7_DOT1P_COMPONENT_MNEMONIC                        "DOT1P"
#define L7_DOT1P_COMPONENT_NAME                            "DOT1P"
#define L7_UNITMGR_COMPONENT_MNEMONIC                      "UNITMGR"
#define L7_UNITMGR_COMPONENT_NAME                          "UNITMGR"
#define L7_CARDMGR_COMPONENT_MNEMONIC                      "CARDMGR"
#define L7_CARDMGR_COMPONENT_NAME                          "CARDMGR"
#define L7_FLEX_STACKING_COMPONENT_MNEMONIC                "STACKING"
#define L7_FLEX_STACKING_COMPONENT_NAME                    "STACKING"
#define L7_EDB_COMPONENT_MNEMONIC                          "EDB"
#define L7_EDB_COMPONENT_NAME                              "EDB"
#define L7_CDA_COMPONENT_MNEMONIC                          "CDA"
#define L7_CDA_COMPONENT_NAME                              "CDA"
#define L7_FFTP_COMPONENT_MNEMONIC                         "FFTP"
#define L7_FFTP_COMPONENT_NAME                             "FFTP"
#define L7_OSAPI_COMPONENT_MNEMONIC                        "OSAPI"
#define L7_OSAPI_COMPONENT_NAME                            "OSAPI"
#define L7_POE_COMPONENT_MNEMONIC                          "POE"
#define L7_POE_COMPONENT_NAME                              "POE"
#define L7_CONFIG_SCRIPT_COMPONENT_MNEMONIC                "SCRIPT"
#define L7_CONFIG_SCRIPT_COMPONENT_NAME                    "SCRIPT"
#define L7_LOG_COMPONENT_MNEMONIC                          "LOG"
#define L7_LOG_COMPONENT_NAME                              "LOG"
#define L7_SNTP_COMPONENT_MNEMONIC                         "SNTP"
#define L7_SNTP_COMPONENT_NAME                             "SNTP"
#define L7_OUTBOUND_TELNET_COMPONENT_MNEMONIC              "OUTB_TNET"
#define L7_OUTBOUND_TELNET_COMPONENT_NAME                  "OUTBOUND_TELNET"
#define L7_PORT_MACLOCK_COMPONENT_MNEMONIC                 "PORT_ML"
#define L7_PORT_MACLOCK_COMPONENT_NAME                     "PORT_MACLOCK"
#define L7_DHCP_FILTERING_COMPONENT_MNEMONIC               "DHCP_FLTR"
#define L7_DHCP_FILTERING_COMPONENT_NAME                   "DHCP_FILTERING"
#define L7_DOSCONTROL_COMPONENT_MNEMONIC                   "DOS"
#define L7_DOSCONTROL_COMPONENT_NAME                       "DENIAL_OF_SERVICE"
#define L7_SERVICES_SPECIFIC_COMPONENT_MNEMONIC            "SERVICES"
#define L7_SERVICES_SPECIFIC_COMPONENT_NAME                "SERVICES"
#define L7_ALARMMGR_COMPONENT_MNEMONIC                     "ALARMMGR"
#define L7_ALARMMGR_COMPONENT_NAME                         "ALARMMGR"
#define L7_POWER_UNIT_MGR_COMPONENT_MNEMONIC               "POWERMGR"
#define L7_POWER_UNIT_MGR_COMPONENT_NAME                   "POWERMGR"
#define L7_FAN_UNIT_MGR_COMPONENT_MNEMONIC                 "FANMGR"
#define L7_FAN_UNIT_MGR_COMPONENT_NAME                     "FANMGR"
#define L7_OSPFV3_MAP_COMPONENT_MNEMONIC                   "OSPFV3"
#define L7_OSPFV3_MAP_COMPONENT_NAME                       "OSPFV3"
#define L7_IPV6_MAP_COMPONENT_MNEMONIC                     "IPV6"
#define L7_IPV6_MAP_COMPONENT_NAME                         "IPV6"
#define L7_IPV6_MAP_COMPONENT_DEBUG_NAME                   "IPV6_MAP"
#define L7_RLIM_COMPONENT_MNEMONIC                         "RLIM"
#define L7_RLIM_COMPONENT_NAME                             "RLIM"
#define L7_DHCP6S_MAP_COMPONENT_MNEMONIC                   "DHCP6S"
#define L7_DHCP6S_MAP_COMPONENT_NAME                       "DHCP6S"
#define L7_LLDP_COMPONENT_MNEMONIC                         "LLDP"
#define L7_LLDP_COMPONENT_NAME                             "LLDP"
#define L7_VLAN_IPSUBNET_COMPONENT_MNEMONIC                "IPSUBVLAN"
#define L7_VLAN_IPSUBNET_COMPONENT_NAME                    "IP_SUBNET_VLAN"
#define L7_VLAN_MAC_COMPONENT_MNEMONIC                     "MACVLAN"
#define L7_VLAN_MAC_COMPONENT_NAME                         "MAC_VLAN"
#define L7_PROTECTED_PORT_COMPONENT_MNEMONIC               "PROT_PORT"
#define L7_PROTECTED_PORT_COMPONENT_NAME                   "PROTECTED_PORT"
#define L7_CHASSIS_ALARM_COMPONENT_MNEMONIC                "CHSS_ALARM"
#define L7_CHASSIS_ALARM_COMPONENT_NAME                    "CHASSIS_ALARM"
#define L7_DNS_CLIENT_COMPONENT_MNEMONIC                   "DNS_CLIENT"
#define L7_DNS_CLIENT_COMPONENT_NAME                       "DNS_CLIENT"
#define L7_FLEX_WIRELESS_COMPONENT_MNEMONIC                "WIRELESS"
#define L7_FLEX_WIRELESS_COMPONENT_NAME                    "WIRELESS"
#define L7_BSP_COMPONENT_MNEMONIC                          "BSP"
#define L7_BSP_COMPONENT_NAME                              "BSP"
#define L7_IPV6_MGMT_COMPONENT_MNEMONIC                    "IPV6_MGMT"
#define L7_IPV6_MGMT_COMPONENT_NAME                        "IPV6_MGMT"
#define L7_MFC_COMPONENT_MNEMONIC                          "MFC"
#define L7_MFC_COMPONENT_NAME                              "MFC"
#define L7_VOICE_VLAN_COMPONENT_MNEMONIC                   "VOICE_VLAN"
#define L7_VOICE_VLAN_COMPONENT_NAME                       "VOICE_VLAN"
#define L7_DHCP_SNOOPING_COMPONENT_MNEMONIC                "DHCP_SNP"
#define L7_DHCP_SNOOPING_COMPONENT_NAME                    "DHCP_SNOOPING"
#define L7_DAI_COMPONENT_MNEMONIC                          "DAI"
#define L7_DAI_COMPONENT_NAME                              "DYNAMIC_ARP_INSPECTION"
#define L7_FLEX_QOS_VOIP_COMPONENT_MNEMONIC                "VOIP"
#define L7_FLEX_QOS_VOIP_COMPONENT_NAME                    "QOS_VOIP"
#define L7_SFLOW_COMPONENT_MNEMONIC                        "SFLOW"
#define L7_SFLOW_COMPONENT_NAME                            "SFLOW"
#define L7_FLEX_CAPTIVE_PORTAL_COMPONENT_MNEMONIC          "CPTV_PRTL"
#define L7_FLEX_CAPTIVE_PORTAL_COMPONENT_NAME              "CAPTIVE_PORTAL"
#define L7_ISDP_COMPONENT_MNEMONIC                         "ISDP"
#define L7_ISDP_COMPONENT_NAME                             "ISDP"
#define L7_LLPF_COMPONENT_MNEMONIC                         "LLPF"
#define L7_LLPF_COMPONENT_NAME                             "LLPF"
#define L7_MGMT_ACAL_COMPONENT_MNEMONIC                    "MGMT_ACAL"
#define L7_MGMT_ACAL_COMPONENT_NAME                        "MGMT_ACAL"
#define L7_BOX_SERVICES_COMPONENT_MNEMONIC                 "BOXSERV"
#define L7_BOX_SERVICES_COMPONENT_NAME                     "BOX_SERVICES"
#define L7_LINK_DEPENDENCY_COMPONENT_MNEMONIC              "LINK_DEP"
#define L7_LINK_DEPENDENCY_COMPONENT_NAME                  "LINK_DEPENDENCY"
#define L7_FLEX_QOS_ISCSI_COMPONENT_MNEMONIC               "ISCSI"
#define L7_FLEX_QOS_ISCSI_COMPONENT_NAME                   "QOS_ISCSI"
#define L7_RMON_COMPONENT_MNEMONIC                         "RMON"
#define L7_RMON_COMPONENT_NAME                             "RMON"
#define L7_FLEX_METRO_DOT1AD_COMPONENT_MNEMONIC            "DOT1AD"
#define L7_FLEX_METRO_DOT1AD_COMPONENT_NAME                "DOT1AD"
#define L7_DOT3AH_COMPONENT_MNEMONIC                       "DOT3AH"
#define L7_DOT3AH_COMPONENT_NAME                           "DOT3AH/EFM-OAM"
#define L7_DOT1AG_COMPONENT_MNEMONIC                       "DOT1AG"
#define L7_DOT1AG_COMPONENT_NAME                           "DOT1AG/CFM-OAM"
#define L7_TR069_COMPONENT_MNEMONIC                        "TR069"
#define L7_TR069_COMPONENT_NAME                            "TR069"
#define L7_AUTO_INSTALL_COMPONENT_MNEMONIC                 "AUTO_INST"
#define L7_AUTO_INSTALL_COMPONENT_NAME                     "AUTO_INSTALL"
#define L7_DHCP_COMPONENT_MNEMONIC                         "DHCPCLIENT"
#define L7_DHCP_COMPONENT_NAME                             "DHCP_CLIENT"
#define L7_BOOTP_COMPONENT_MNEMONIC                        "BOOTP"
#define L7_BOOTP_COMPONENT_NAME                            "BOOTP_CLIENT"
#define L7_CP_WIO_COMPONENT_MNEMONIC                       "CP WIO"
#define L7_CP_WIO_COMPONENT_NAME                           "CAPTIVE_PORTAL_WIRED_INTF_OWNER"
#define L7_DHCP6C_COMPONENT_MNEMONIC                       "DHCP6C"
#define L7_DHCP6C_COMPONENT_NAME                           "DHCP6_CLIENT"
#define L7_CKPT_COMPONENT_MNEMONIC                         "CKPT"
#define L7_CKPT_COMPONENT_NAME                             "CHECKPOINT_SERVICE"
#define L7_PFC_COMPONENT_MNEMONIC                          "PFC"
#define L7_PFC_COMPONENT_NAME                              "PFC"
#define L7_DOT1X_AUTH_SERV_COMPONENT_MNEMONIC              "DOT1X_AS" 
#define L7_DOT1X_AUTH_SERV_COMPONENT_NAME                  "DOT1X_AUTH_SERV" 
#define L7_TIMERANGES_COMPONENT_MNEMONIC                   "TIMERANGES" 
#define L7_TIMERANGES_COMPONENT_NAME                       "TIMERANGES" 
#define L7_PTIN_COMPONENT_MNEMONIC                         "PTIN"         /* PTin added: ptin module */
#define L7_PTIN_COMPONENT_NAME                             "PTIN MODULE"  /* PTin added: ptin module */
#define L7_PTIN_PPPOE_COMPONENT_MNEMONIC                   "PPPOE"        /* PTin added: pppoe module */
#define L7_PTIN_PPPOE_COMPONENT_NAME                       "PPPOE MODULE" /* PTin added: pppoe module */

#define L7_COMPONENT_MNEMONIC_MAX_LEN   10      /* includes end-of-string char */
#define L7_COMPONENT_NAME_MAX_LEN       40      /* includes end-of-string char */


/* These components are started even when box hardware management
** capability is disabled. The components are required in order
** for the box to act as a stack member.
*/
static L7_COMPONENT_IDS_t cnfgrNonMgrComponentList [] =
{
  L7_TRAPMGR_COMPONENT_ID,
  L7_CARDMGR_COMPONENT_ID,
  L7_UNITMGR_COMPONENT_ID,
  L7_FLEX_STACKING_COMPONENT_ID,
  L7_FFTP_COMPONENT_ID,
  L7_CDA_COMPONENT_ID,
  L7_FLEX_OPENSSL_COMPONENT_ID,
  L7_CLI_WEB_COMPONENT_ID,
  L7_CMD_LOGGER_COMPONENT_ID,
  L7_EDB_COMPONENT_ID,
  L7_NIM_COMPONENT_ID,
  L7_DTL_COMPONENT_ID,
  L7_CKPT_COMPONENT_ID
};

/* Component naming table.
*/
static CNFGR_COMPONENT_NAME_LIST_ENTRY_t cnfgrComponentNameList[] =
{
  /* cid                         mnemonic                              name    */
  /*-----------------------------------------------------------------------------------------------------------------------------*/
  { L7_SIM_COMPONENT_ID,                 L7_SIM_COMPONENT_MNEMONIC,                 L7_SIM_COMPONENT_NAME},
  { L7_NIM_COMPONENT_ID,                 L7_NIM_COMPONENT_MNEMONIC,                 L7_NIM_COMPONENT_NAME},
  { L7_TRAPMGR_COMPONENT_ID,             L7_TRAPMGR_COMPONENT_MNEMONIC,             L7_TRAPMGR_COMPONENT_NAME},
  { L7_CNFGR_COMPONENT_ID,               L7_CNFGR_COMPONENT_MNEMONIC,               L7_CNFGR_COMPONENT_NAME},
  { L7_DRIVER_COMPONENT_ID,              L7_DRIVER_COMPONENT_MNEMONIC,              L7_DRIVER_COMPONENT_NAME},
  { L7_POLICY_COMPONENT_ID,              L7_POLICY_COMPONENT_MNEMONIC,              L7_POLICY_COMPONENT_NAME},
  { L7_DOT1Q_COMPONENT_ID,               L7_DOT1Q_COMPONENT_MNEMONIC,               L7_DOT1Q_COMPONENT_NAME},
  { L7_PBVLAN_COMPONENT_ID,              L7_PBVLAN_COMPONENT_MNEMONIC,              L7_PBVLAN_COMPONENT_NAME},
  { L7_DOT3AD_COMPONENT_ID,              L7_DOT3AD_COMPONENT_MNEMONIC,              L7_DOT3AD_COMPONENT_NAME},
  { L7_STATSMGR_COMPONENT_ID,            L7_STATSMGR_COMPONENT_MNEMONIC,            L7_STATSMGR_COMPONENT_NAME},
  { L7_LTS_COMPONENT_ID,                 L7_LTS_COMPONENT_MNEMONIC,                 L7_LTS_COMPONENT_NAME},
  { L7_USMDB_COMPONENT_ID,               L7_USMDB_COMPONENT_MNEMONIC,               L7_USMDB_COMPONENT_NAME},
  { L7_LAYER3_COMPONENT_ID,              L7_LAYER3_COMPONENT_MNEMONIC,              L7_LAYER3_COMPONENT_NAME},
  { L7_CUSTOM_COMPONENT_ID,              L7_CUSTOM_COMPONENT_MNEMONIC,              L7_CUSTOM_COMPONENT_NAME},
  { L7_FDB_COMPONENT_ID,                 L7_FDB_COMPONENT_MNEMONIC,                 L7_FDB_COMPONENT_NAME},
  { L7_GARP_COMPONENT_ID,                L7_GARP_COMPONENT_MNEMONIC,                L7_GARP_COMPONENT_NAME},
  { L7_SNMP_COMPONENT_ID,                L7_SNMP_COMPONENT_MNEMONIC,                L7_SNMP_COMPONENT_NAME},
  { L7_DTL_COMPONENT_ID,                 L7_DTL_COMPONENT_MNEMONIC,                 L7_DTL_COMPONENT_NAME},
  { L7_USER_MGR_COMPONENT_ID,            L7_USER_MGR_COMPONENT_MNEMONIC,            L7_USER_MGR_COMPONENT_NAME},
  { L7_CLI_WEB_COMPONENT_ID,             L7_CLI_WEB_COMPONENT_MNEMONIC,             L7_CLI_WEB_COMPONENT_NAME},
  { L7_CMD_LOGGER_COMPONENT_ID,          L7_CMD_LOGGER_COMPONENT_MNEMONIC,          L7_CMD_LOGGER_COMPONENT_NAME},
  { L7_IP_MAP_COMPONENT_ID,              L7_IP_MAP_COMPONENT_MNEMONIC,              L7_IP_MAP_COMPONENT_NAME},
  { L7_RTR_DISC_COMPONENT_ID,            L7_RTR_DISC_COMPONENT_MNEMONIC,            L7_RTR_DISC_COMPONENT_NAME},
  { L7_OSPF_MAP_COMPONENT_ID,            L7_OSPF_MAP_COMPONENT_MNEMONIC,            L7_OSPF_MAP_COMPONENT_NAME},
  { L7_RIP_MAP_COMPONENT_ID,             L7_RIP_MAP_COMPONENT_MNEMONIC,             L7_RIP_MAP_COMPONENT_NAME},
  { L7_FLEX_MGMD_MAP_COMPONENT_ID,       L7_FLEX_MGMD_MAP_COMPONENT_MNEMONIC,       L7_FLEX_MGMD_MAP_COMPONENT_NAME},
  { L7_FLEX_MCAST_MAP_COMPONENT_ID,      L7_FLEX_MCAST_MAP_COMPONENT_MNEMONIC,      L7_FLEX_MCAST_MAP_COMPONENT_NAME},
  { L7_FLEX_PIMDM_MAP_COMPONENT_ID,      L7_FLEX_PIMDM_MAP_COMPONENT_MNEMONIC,      L7_FLEX_PIMDM_MAP_COMPONENT_NAME},
  { L7_FLEX_DVMRP_MAP_COMPONENT_ID,      L7_FLEX_DVMRP_MAP_COMPONENT_MNEMONIC,      L7_FLEX_DVMRP_MAP_COMPONENT_NAME},
  { L7_FLEX_PIMSM_MAP_COMPONENT_ID,      L7_FLEX_PIMSM_MAP_COMPONENT_MNEMONIC,      L7_FLEX_PIMSM_MAP_COMPONENT_NAME},
  { L7_VRRP_MAP_COMPONENT_ID,            L7_VRRP_MAP_COMPONENT_MNEMONIC,            L7_VRRP_MAP_COMPONENT_NAME},
  { L7_FLEX_PACKAGES_ID,                 L7_FLEX_PACKAGES_MNEMONIC,                 L7_FLEX_PACKAGES_NAME},
  { L7_FLEX_MPLS_MAP_COMPONENT_ID,       L7_FLEX_MPLS_MAP_COMPONENT_MNEMONIC,       L7_FLEX_MPLS_MAP_COMPONENT_NAME},
  { L7_FLEX_BGP_MAP_COMPONENT_ID,        L7_FLEX_BGP_MAP_COMPONENT_MNEMONIC,        L7_FLEX_BGP_MAP_COMPONENT_NAME},
  { L7_FLEX_QOS_ACL_COMPONENT_ID,        L7_FLEX_QOS_ACL_COMPONENT_MNEMONIC,        L7_FLEX_QOS_ACL_COMPONENT_NAME},
  { L7_FLEX_QOS_COS_COMPONENT_ID,        L7_FLEX_QOS_COS_COMPONENT_MNEMONIC,        L7_FLEX_QOS_COS_COMPONENT_NAME},
  { L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,   L7_FLEX_QOS_DIFFSERV_COMPONENT_MNEMONIC,   L7_FLEX_QOS_DIFFSERV_COMPONENT_NAME},
  { L7_PORT_MIRROR_COMPONENT_ID,         L7_PORT_MIRROR_COMPONENT_MNEMONIC,         L7_PORT_MIRROR_COMPONENT_NAME},
  { L7_MFDB_COMPONENT_ID,                L7_MFDB_COMPONENT_MNEMONIC,                L7_MFDB_COMPONENT_NAME},
  { L7_FILTER_COMPONENT_ID,              L7_FILTER_COMPONENT_MNEMONIC,              L7_FILTER_COMPONENT_NAME},
  { L7_DHCP_RELAY_COMPONENT_ID,          L7_DHCP_RELAY_COMPONENT_MNEMONIC,          L7_DHCP_RELAY_COMPONENT_NAME},
  { L7_DHCP_FILTERING_COMPONENT_ID,      L7_DHCP_FILTERING_COMPONENT_MNEMONIC,      L7_DHCP_FILTERING_COMPONENT_NAME},
  { L7_SNOOPING_COMPONENT_ID,            L7_SNOOPING_COMPONENT_MNEMONIC,            L7_SNOOPING_COMPONENT_NAME},
  { L7_GMRP_COMPONENT_ID,                L7_GMRP_COMPONENT_MNEMONIC,                L7_GMRP_COMPONENT_NAME},
  { L7_SERVICE_PORT_PRESENT_ID,          L7_SERVICE_PORT_PRESENT_MNEMONIC,          L7_SERVICE_PORT_PRESENT_NAME},
  { L7_CABLE_TEST_COMPONENT_ID,          L7_CABLE_TEST_COMPONENT_MNEMONIC,          L7_CABLE_TEST_COMPONENT_NAME},
  { L7_DOT1S_COMPONENT_ID,               L7_DOT1S_COMPONENT_MNEMONIC,               L7_DOT1S_COMPONENT_NAME},
  { L7_RADIUS_COMPONENT_ID,              L7_RADIUS_COMPONENT_MNEMONIC,              L7_RADIUS_COMPONENT_NAME},
  { L7_TACACS_COMPONENT_ID,              L7_TACACS_COMPONENT_MNEMONIC,              L7_TACACS_COMPONENT_NAME},
  { L7_DOT1X_COMPONENT_ID,               L7_DOT1X_COMPONENT_MNEMONIC,               L7_DOT1X_COMPONENT_NAME},
  { L7_DHCPS_MAP_COMPONENT_ID,           L7_DHCPS_MAP_COMPONENT_MNEMONIC,           L7_DHCPS_MAP_COMPONENT_NAME},
  { L7_DVLANTAG_COMPONENT_ID,            L7_DVLANTAG_COMPONENT_MNEMONIC,            L7_DVLANTAG_COMPONENT_NAME},
  { L7_IPV6_PROVISIONING_COMPONENT_ID,   L7_IPV6_PROVISIONING_COMPONENT_MNEMONIC,   L7_IPV6_PROVISIONING_COMPONENT_NAME},
  { L7_FLEX_OPENSSL_COMPONENT_ID,        L7_FLEX_OPENSSL_COMPONENT_MNEMONIC,        L7_FLEX_OPENSSL_COMPONENT_NAME},
  { L7_FLEX_SSLT_COMPONENT_ID,           L7_FLEX_SSLT_COMPONENT_MNEMONIC,           L7_FLEX_SSLT_COMPONENT_NAME},
  { L7_FLEX_SSHD_COMPONENT_ID,           L7_FLEX_SSHD_COMPONENT_MNEMONIC,           L7_FLEX_SSHD_COMPONENT_NAME},
  { L7_DOT1P_COMPONENT_ID,               L7_DOT1P_COMPONENT_MNEMONIC,               L7_DOT1P_COMPONENT_NAME},
  { L7_UNITMGR_COMPONENT_ID,             L7_UNITMGR_COMPONENT_MNEMONIC,             L7_UNITMGR_COMPONENT_NAME},
  { L7_CARDMGR_COMPONENT_ID,             L7_CARDMGR_COMPONENT_MNEMONIC,             L7_CARDMGR_COMPONENT_NAME},
  { L7_FLEX_STACKING_COMPONENT_ID,       L7_FLEX_STACKING_COMPONENT_MNEMONIC,       L7_FLEX_STACKING_COMPONENT_NAME},
  { L7_EDB_COMPONENT_ID,                 L7_EDB_COMPONENT_MNEMONIC,                 L7_EDB_COMPONENT_NAME},
  { L7_CDA_COMPONENT_ID,                 L7_CDA_COMPONENT_MNEMONIC,                 L7_CDA_COMPONENT_NAME},
  { L7_FFTP_COMPONENT_ID,                L7_FFTP_COMPONENT_MNEMONIC,                L7_FFTP_COMPONENT_NAME},
  { L7_OSAPI_COMPONENT_ID,               L7_OSAPI_COMPONENT_MNEMONIC,               L7_OSAPI_COMPONENT_NAME},
  { L7_POE_COMPONENT_ID,                 L7_POE_COMPONENT_MNEMONIC,                 L7_POE_COMPONENT_NAME},
  { L7_CONFIG_SCRIPT_COMPONENT_ID,       L7_CONFIG_SCRIPT_COMPONENT_MNEMONIC,       L7_CONFIG_SCRIPT_COMPONENT_NAME},
  { L7_LOG_COMPONENT_ID,                 L7_LOG_COMPONENT_MNEMONIC,                 L7_LOG_COMPONENT_NAME},
  { L7_SNTP_COMPONENT_ID,                L7_SNTP_COMPONENT_MNEMONIC,                L7_SNTP_COMPONENT_NAME},
  { L7_TELNET_MAP_COMPONENT_ID,          L7_OUTBOUND_TELNET_COMPONENT_MNEMONIC,     L7_OUTBOUND_TELNET_COMPONENT_NAME},
  { L7_PORT_MACLOCK_COMPONENT_ID,        L7_PORT_MACLOCK_COMPONENT_MNEMONIC,        L7_PORT_MACLOCK_COMPONENT_NAME},
  { L7_FLEX_OSPFV3_MAP_COMPONENT_ID,     L7_OSPFV3_MAP_COMPONENT_MNEMONIC,          L7_OSPFV3_MAP_COMPONENT_NAME},
  { L7_FLEX_IPV6_MAP_COMPONENT_ID,       L7_IPV6_MAP_COMPONENT_MNEMONIC,            L7_IPV6_MAP_COMPONENT_NAME},
  { L7_RLIM_COMPONENT_ID,                L7_RLIM_COMPONENT_MNEMONIC,                L7_RLIM_COMPONENT_NAME},
  { L7_FLEX_DHCP6S_MAP_COMPONENT_ID,     L7_DHCP6S_MAP_COMPONENT_MNEMONIC,          L7_DHCP6S_MAP_COMPONENT_NAME},
  { L7_DOSCONTROL_COMPONENT_ID,          L7_DOSCONTROL_COMPONENT_MNEMONIC,          L7_DOSCONTROL_COMPONENT_NAME},
  { L7_LLDP_COMPONENT_ID,                L7_LLDP_COMPONENT_MNEMONIC,                L7_LLDP_COMPONENT_NAME},
  { L7_VLAN_IPSUBNET_COMPONENT_ID,       L7_VLAN_IPSUBNET_COMPONENT_MNEMONIC,       L7_VLAN_IPSUBNET_COMPONENT_NAME},
  { L7_VLAN_IPSUBNET_COMPONENT_ID,       L7_VLAN_IPSUBNET_COMPONENT_MNEMONIC,       L7_VLAN_IPSUBNET_COMPONENT_NAME},
  { L7_VLAN_MAC_COMPONENT_ID,            L7_VLAN_MAC_COMPONENT_MNEMONIC,            L7_VLAN_MAC_COMPONENT_NAME},
  { L7_PROTECTED_PORT_COMPONENT_ID,      L7_PROTECTED_PORT_COMPONENT_MNEMONIC,      L7_PROTECTED_PORT_COMPONENT_NAME},
  { L7_DNS_CLIENT_COMPONENT_ID,          L7_DNS_CLIENT_COMPONENT_MNEMONIC,          L7_DNS_CLIENT_COMPONENT_NAME},
  { L7_FLEX_WIRELESS_COMPONENT_ID,       L7_FLEX_WIRELESS_COMPONENT_MNEMONIC,       L7_FLEX_WIRELESS_COMPONENT_NAME},
  { L7_ALARMMGR_COMPONENT_ID,            L7_ALARMMGR_COMPONENT_MNEMONIC,            L7_ALARMMGR_COMPONENT_NAME},
  { L7_CHASSIS_ALARM_COMPONENT_ID,       L7_CHASSIS_ALARM_COMPONENT_MNEMONIC,       L7_CHASSIS_ALARM_COMPONENT_NAME},
  { L7_POWER_UNIT_MGR_COMPONENT_ID,      L7_POWER_UNIT_MGR_COMPONENT_MNEMONIC,      L7_POWER_UNIT_MGR_COMPONENT_NAME},
  { L7_FAN_UNIT_MGR_COMPONENT_ID,        L7_FAN_UNIT_MGR_COMPONENT_MNEMONIC,        L7_FAN_UNIT_MGR_COMPONENT_NAME},
  { L7_BSP_COMPONENT_ID,                 L7_BSP_COMPONENT_MNEMONIC,                 L7_BSP_COMPONENT_NAME},
  { L7_FLEX_IPV6_MGMT_COMPONENT_ID,      L7_IPV6_MGMT_COMPONENT_MNEMONIC,           L7_IPV6_MGMT_COMPONENT_NAME},
  { L7_MFC_COMPONENT_ID,                 L7_MFC_COMPONENT_MNEMONIC,                 L7_MFC_COMPONENT_NAME},
  { L7_VOICE_VLAN_COMPONENT_ID,          L7_VOICE_VLAN_COMPONENT_MNEMONIC,          L7_VOICE_VLAN_COMPONENT_NAME},
  { L7_DHCP_SNOOPING_COMPONENT_ID,       L7_DHCP_SNOOPING_COMPONENT_MNEMONIC,       L7_DHCP_SNOOPING_COMPONENT_NAME},
  { L7_DAI_COMPONENT_ID,                 L7_DAI_COMPONENT_MNEMONIC,                 L7_DAI_COMPONENT_NAME},
  { L7_FLEX_QOS_VOIP_COMPONENT_ID,       L7_FLEX_QOS_VOIP_COMPONENT_MNEMONIC,       L7_FLEX_QOS_VOIP_COMPONENT_NAME},
  { L7_ISDP_COMPONENT_ID,                L7_ISDP_COMPONENT_MNEMONIC,                L7_ISDP_COMPONENT_NAME},
  { L7_LLPF_COMPONENT_ID,                L7_LLPF_COMPONENT_MNEMONIC,                L7_LLPF_COMPONENT_NAME},
  { L7_SFLOW_COMPONENT_ID,               L7_SFLOW_COMPONENT_MNEMONIC,               L7_SFLOW_COMPONENT_NAME},
  { L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID, L7_FLEX_CAPTIVE_PORTAL_COMPONENT_MNEMONIC, L7_FLEX_CAPTIVE_PORTAL_COMPONENT_NAME},
  { L7_CP_WIO_COMPONENT_ID,              L7_CP_WIO_COMPONENT_MNEMONIC,              L7_CP_WIO_COMPONENT_NAME},
  { L7_FLEX_METRO_DOT1AD_COMPONENT_ID,   L7_FLEX_METRO_DOT1AD_COMPONENT_MNEMONIC,   L7_FLEX_METRO_DOT1AD_COMPONENT_NAME},
  { L7_DOT3AH_COMPONENT_ID,              L7_DOT3AH_COMPONENT_MNEMONIC,              L7_DOT3AH_COMPONENT_NAME},
  { L7_DOT1AG_COMPONENT_ID,              L7_DOT1AG_COMPONENT_MNEMONIC,              L7_DOT1AG_COMPONENT_NAME},
  { L7_MGMT_ACAL_COMPONENT_ID,           L7_MGMT_ACAL_COMPONENT_MNEMONIC,           L7_MGMT_ACAL_COMPONENT_NAME         },
  { L7_BOX_SERVICES_COMPONENT_ID,        L7_BOX_SERVICES_COMPONENT_MNEMONIC,        L7_BOX_SERVICES_COMPONENT_NAME},
  { L7_LINK_DEPENDENCY_COMPONENT_ID,     L7_LINK_DEPENDENCY_COMPONENT_MNEMONIC,     L7_LINK_DEPENDENCY_COMPONENT_NAME},
  { L7_RMON_COMPONENT_ID,                L7_RMON_COMPONENT_MNEMONIC,                L7_RMON_COMPONENT_NAME},
  { L7_TR069_COMPONENT_ID,               L7_TR069_COMPONENT_MNEMONIC,               L7_TR069_COMPONENT_NAME},
  { L7_AUTO_INSTALL_COMPONENT_ID,        L7_AUTO_INSTALL_COMPONENT_MNEMONIC,        L7_AUTO_INSTALL_COMPONENT_NAME},
  { L7_DHCP_CLIENT_COMPONENT_ID,         L7_DHCP_COMPONENT_MNEMONIC,                L7_DHCP_COMPONENT_NAME},
  { L7_BOOTP_COMPONENT_ID,               L7_BOOTP_COMPONENT_MNEMONIC,               L7_BOOTP_COMPONENT_NAME},
  { L7_FLEX_QOS_ISCSI_COMPONENT_ID,      L7_FLEX_QOS_ISCSI_COMPONENT_MNEMONIC,      L7_FLEX_QOS_ISCSI_COMPONENT_NAME},
  { L7_DHCP6C_COMPONENT_ID,              L7_DHCP6C_COMPONENT_MNEMONIC,              L7_DHCP6C_COMPONENT_NAME},
  { L7_CKPT_COMPONENT_ID,                L7_CKPT_COMPONENT_MNEMONIC,                L7_CKPT_COMPONENT_NAME},
  { L7_ARP_MAP_COMPONENT_ID,             L7_ARP_MAP_COMPONENT_MNEMONIC,             L7_ARP_MAP_COMPONENT_NAME},
  { L7_PFC_COMPONENT_ID,                 L7_PFC_COMPONENT_MNEMONIC,                 L7_PFC_COMPONENT_NAME},
  { L7_DOT1X_AUTH_SERV_COMPONENT_ID,     L7_DOT1X_AUTH_SERV_COMPONENT_MNEMONIC,     L7_DOT1X_AUTH_SERV_COMPONENT_NAME},
  { L7_TIMERANGES_COMPONENT_ID,          L7_TIMERANGES_COMPONENT_MNEMONIC,          L7_TIMERANGES_COMPONENT_NAME},

  /* PTin added: ptin module */
  { L7_PTIN_COMPONENT_ID,                L7_PTIN_COMPONENT_MNEMONIC,                L7_PTIN_COMPONENT_NAME},
  { PTIN_PPPOE_COMPONENT_ID,             L7_PTIN_PPPOE_COMPONENT_MNEMONIC,          L7_PTIN_PPPOE_COMPONENT_NAME},

/*===============================================================*/
/* This must be the last entry of the lists  ** Sentinel **      */
/*===============================================================*/
  {L7_LAST_COMPONENT_ID}
};

/* Component activation table.
*/
static CNFGR_COMPONENT_LIST_ENTRY_t cnfgrComponentList[] =

{ /* cid                              mode                        pComponentCmdFunc               hwApplyPhases */
  /*-------------------------------------------------------------------------------------------------------------*/
  {L7_CNFGR_COMPONENT_ID,                L7_CNFGR_COMPONENT_ENABLE,  L7_NULLPTR,                    0},
  {L7_SIM_COMPONENT_ID,                  L7_CNFGR_COMPONENT_ENABLE,  simApiCnfgrCommand,            0},
  {L7_NIM_COMPONENT_ID,                  L7_CNFGR_COMPONENT_ENABLE,  nimApiCnfgrCommand,            0},
  /*
  Although OpenSSL is technically a "flex" package, its library is depended on by
  several "base" components, if the OPENSSL package is present.  Since the OpenSSL
  component should be started up and initialized when the dependent components
  attempt to initialize the SSL library (by calling opensslInit), OpenSSL should
  be started as early as possible, otherwise the dependent components will be
  forced to wait until OpenSSL has properly initialized the SSL libraries.
  */
#ifdef L7_OPENSSL_PACKAGE
  {L7_FLEX_OPENSSL_COMPONENT_ID,         L7_CNFGR_COMPONENT_ENABLE,  opensslApiCnfgrCommand,        0},
#endif
  {L7_USER_MGR_COMPONENT_ID,             L7_CNFGR_COMPONENT_ENABLE,  userMgrApiCnfgrCommand,        0},
#if defined (L7_CLI_PACKAGE) || defined (L7_XCLI_PACKAGE) || defined(L7_XWEB_PACKAGE)
  {L7_CLI_WEB_COMPONENT_ID,              L7_CNFGR_COMPONENT_ENABLE,  cliWebApiCnfgrCommand,         0},
#endif
#ifndef L7_PRODUCT_SMARTPATH
#ifdef L7_XCLI_PACKAGE
  {L7_CMD_LOGGER_COMPONENT_ID,           L7_CNFGR_COMPONENT_DISABLE, cmdLoggerApiCnfgrCommand,      0},
#else
  {L7_CMD_LOGGER_COMPONENT_ID,           L7_CNFGR_COMPONENT_ENABLE,  cmdLoggerApiCnfgrCommand,      0},
#endif
#endif
  {L7_DTL_COMPONENT_ID,                  L7_CNFGR_COMPONENT_ENABLE,  dtlApiCnfgrCommand,            0},
  {L7_CARDMGR_COMPONENT_ID,              L7_CNFGR_COMPONENT_ENABLE,  cmgrApiCnfgrCommand,           0},
#ifdef L7_TIMERANGES_PACKAGE
  {L7_TIMERANGES_COMPONENT_ID,            L7_CNFGR_COMPONENT_ENABLE,  timeRangeApiCnfgrCommand,      0},
#endif  
#ifdef L7_POE_PACKAGE
  {L7_POE_COMPONENT_ID,                  L7_CNFGR_COMPONENT_ENABLE,  poeApiCnfgrCommand,            0},
#endif
  {L7_POLICY_COMPONENT_ID,               L7_CNFGR_COMPONENT_ENABLE,  policyApiCnfgrCommand,         L7_CNFGR_HW_APPLY_CONFIG},
  {L7_PORT_MIRROR_COMPONENT_ID,          L7_CNFGR_COMPONENT_ENABLE,  mirrorApiCnfgrCommand,         L7_CNFGR_HW_APPLY_CONFIG},
  {L7_FDB_COMPONENT_ID,                  L7_CNFGR_COMPONENT_ENABLE,  fdbApiCnfgrCommand,            L7_CNFGR_HW_APPLY_CONFIG},
#ifdef L7_MACLOCK_PACKAGE
  {L7_PORT_MACLOCK_COMPONENT_ID,         L7_CNFGR_COMPONENT_ENABLE,  pmlApiCnfgrCommand,            L7_CNFGR_HW_APPLY_CONFIG},
#endif
#ifdef L7_DHCP_FILTER_PACKAGE
  {L7_DHCP_FILTERING_COMPONENT_ID,       L7_CNFGR_COMPONENT_ENABLE,  dhcpFilterApiCnfgrCommand,     0},
#endif
#ifdef L7_DHCP_SNOOPING_PACKAGE
  {L7_DHCP_SNOOPING_COMPONENT_ID,        L7_CNFGR_COMPONENT_ENABLE,  dsApiCnfgrCommand,             L7_CNFGR_HW_APPLY_CONFIG},
#endif
#ifdef PTIN_PPPOE_PACKAGE
  {PTIN_PPPOE_COMPONENT_ID,              L7_CNFGR_COMPONENT_ENABLE,  pppoeApiCnfgrCommand,          L7_CNFGR_HW_APPLY_CONFIG},
#endif


#ifdef L7_DOT3AH_PACKAGE
  {L7_DOT3AH_COMPONENT_ID,               L7_CNFGR_COMPONENT_ENABLE,  dot3ahApiCnfgrCommand,         L7_CNFGR_HW_APPLY_CONFIG},
#endif
#ifdef L7_DOT1AG_PACKAGE
  {L7_DOT1AG_COMPONENT_ID,               L7_CNFGR_COMPONENT_ENABLE,  dot1agApiCnfgrCommand,         0},
#endif
#ifdef L7_DAI_PACKAGE
  {L7_DAI_COMPONENT_ID,                  L7_CNFGR_COMPONENT_ENABLE,  daiApiCnfgrCommand,            L7_CNFGR_HW_APPLY_CONFIG},
#endif
  {L7_SNMP_COMPONENT_ID,                 L7_CNFGR_COMPONENT_ENABLE,  snmpApiCnfgrCommand,           0},
#ifndef L7_PRODUCT_SMARTPATH
  {L7_EDB_COMPONENT_ID,                  L7_CNFGR_COMPONENT_ENABLE,  edbApiCnfgrCommand,            0},
#endif
  {L7_DOT1Q_COMPONENT_ID,                L7_CNFGR_COMPONENT_ENABLE,  dot1qApiCnfgrCommand,          L7_CNFGR_HW_APPLY_CONFIG},
#ifdef L7_PBVLAN_PACKAGE
  {L7_PBVLAN_COMPONENT_ID,               L7_CNFGR_COMPONENT_ENABLE,  pbVlanApiCnfgrCommand,         L7_CNFGR_HW_APPLY_CONFIG},
#endif
#ifdef L7_GARP_PACKAGE
  {L7_GARP_COMPONENT_ID,                 L7_CNFGR_COMPONENT_ENABLE,  garpApiCnfgrCommand,           0},
#endif
  {L7_DOT1S_COMPONENT_ID,                L7_CNFGR_COMPONENT_ENABLE,  dot1sApiCnfgrCommand,          L7_CNFGR_HW_APPLY_CONFIG},
  {L7_DOT1X_COMPONENT_ID,                L7_CNFGR_COMPONENT_ENABLE,  dot1xApiCnfgrCommand,          L7_CNFGR_HW_APPLY_CONFIG},
  {L7_MFDB_COMPONENT_ID,                 L7_CNFGR_COMPONENT_ENABLE,  mfdbApiCnfgrCommand,           0},
#ifdef L7_STATIC_FILTERING_PACKAGE
  {L7_FILTER_COMPONENT_ID,               L7_CNFGR_COMPONENT_ENABLE,  filterApiCnfgrCommand,         L7_CNFGR_HW_APPLY_CONFIG},
#endif
  {L7_RADIUS_COMPONENT_ID,               L7_CNFGR_COMPONENT_DISABLE,  radiusApiCnfgrCommand,         0},
  {L7_MGMT_ACAL_COMPONENT_ID,            L7_CNFGR_COMPONENT_ENABLE,  macalApiCnfgrCommand,          0},
#ifndef L7_PRODUCT_SMARTPATH
#endif
#ifdef L7_TACACS_PACKAGE
  {L7_TACACS_COMPONENT_ID,               L7_CNFGR_COMPONENT_ENABLE,  tacacsApiCnfgrCommand,         0},
#endif
#ifdef L7_METRO_FLEX_PACKAGE
#ifdef L7_DOT1AD_PACKAGE
  {L7_FLEX_METRO_DOT1AD_COMPONENT_ID,    L7_CNFGR_COMPONENT_ENABLE,  dot1adApiCnfgrCommand,         L7_CNFGR_HW_APPLY_CONFIG},
#endif
#endif
  {L7_UNITMGR_COMPONENT_ID,              L7_CNFGR_COMPONENT_ENABLE,  unitMgrApiCnfgrCommand,        0},
  {L7_SNOOPING_COMPONENT_ID,             L7_CNFGR_COMPONENT_ENABLE,  snoopApiCnfgrCommand,          L7_CNFGR_HW_APPLY_CONFIG},
  {L7_DOT3AD_COMPONENT_ID,               L7_CNFGR_COMPONENT_ENABLE,  dot3adApiCnfgrCommand,         L7_CNFGR_HW_APPLY_CONFIG},
#ifndef L7_PRODUCT_SMARTPATH
  {L7_TRAPMGR_COMPONENT_ID,              L7_CNFGR_COMPONENT_ENABLE,  trapMgrApiCnfgrCommand,        0},
#endif
  /* NOTE: intentionally commented out but left for historical purposes
  {L7_IPV6_PROVISIONING_COMPONENT_ID,    L7_CNFGR_COMPONENT_ENABLE,  ipv6ProvApiCnfgrCommand,       L7_NULLPTR},
  */
#ifdef L7_DVLAN_PACKAGE
  {L7_DVLANTAG_COMPONENT_ID,             L7_CNFGR_COMPONENT_ENABLE,  dvlantagApiCnfgrCommand,       L7_CNFGR_HW_APPLY_CONFIG},
#endif
#ifdef L7_DHCPS_PACKAGE
  {L7_DHCPS_MAP_COMPONENT_ID,            L7_CNFGR_COMPONENT_ENABLE,  dhcpsApiCnfgrCommand,          0},
#endif
  {L7_LOG_COMPONENT_ID,                  L7_CNFGR_COMPONENT_ENABLE,  logApiCnfgrCommand,            0},
  {L7_SNTP_COMPONENT_ID,                 L7_CNFGR_COMPONENT_ENABLE,  sntpApiCnfgrCommand,           0},
#ifdef L7_OUTBOUND_TELNET_PACKAGE
  {L7_TELNET_MAP_COMPONENT_ID,           L7_CNFGR_COMPONENT_ENABLE,  telnetApiCnfgrCommand,         0},
#endif
  {L7_DOSCONTROL_COMPONENT_ID,           L7_CNFGR_COMPONENT_ENABLE,  doSApiCnfgrCommand,            L7_CNFGR_HW_APPLY_CONFIG},
#ifdef L7_PROT_PORTS_PACKAGE
  {L7_PROTECTED_PORT_COMPONENT_ID,       L7_CNFGR_COMPONENT_ENABLE,  protectedPortApiCnfgrCommand,  L7_CNFGR_HW_APPLY_CONFIG},
#endif
#ifdef L7_IPVLAN_PACKAGE
  {L7_VLAN_IPSUBNET_COMPONENT_ID,        L7_CNFGR_COMPONENT_ENABLE,  vlanIpSubnetApiCnfgrCommand,   L7_CNFGR_HW_APPLY_CONFIG},
#endif
#ifdef L7_MACVLAN_PACKAGE
  {L7_VLAN_MAC_COMPONENT_ID,             L7_CNFGR_COMPONENT_ENABLE,  vlanMacApiCnfgrCommand,        L7_CNFGR_HW_APPLY_CONFIG},
#endif
  {L7_VOICE_VLAN_COMPONENT_ID,           L7_CNFGR_COMPONENT_DISABLE,  voiceVlanApiCnfgrCommand,      L7_CNFGR_HW_APPLY_CONFIG},
#ifdef L7_SFLOW_PACKAGE
  {L7_SFLOW_COMPONENT_ID,                L7_CNFGR_COMPONENT_ENABLE,  sFlowApiCnfgrCommand,          0},
#endif
#ifdef L7_TR069_PACKAGE
  {L7_TR069_COMPONENT_ID,                L7_CNFGR_COMPONENT_ENABLE,  tr069ApiCnfgrCommand,          0},
#endif
#ifdef L7_PFC_PACKAGE
  {L7_PFC_COMPONENT_ID,                   L7_CNFGR_COMPONENT_ENABLE,  pfcApiCnfgrCommand,          L7_CNFGR_HW_APPLY_CONFIG},
#endif
  /*===============================================================*/
  /* components that belong to flex packages                         */
  /*===============================================================*/

  /*       cid                        mode                        pComponentCmdFunc               enable_routine */
  /*-------------------------------------------------------------------------------------------------------------*/

#ifdef L7_MGMT_SECURITY_PACKAGE
  {L7_FLEX_SSLT_COMPONENT_ID,            L7_CNFGR_COMPONENT_ENABLE,  ssltApiCnfgrCommand,           0},  /* PTin changed: enable->disable */
  {L7_FLEX_SSHD_COMPONENT_ID,            L7_CNFGR_COMPONENT_ENABLE,  sshdApiCnfgrCommand,           0},  /* PTin changed: enable->disable */
#endif

#ifdef L7_STACKING_PACKAGE
  {L7_FLEX_STACKING_COMPONENT_ID,        L7_CNFGR_COMPONENT_DISABLE,  spmApiCnfgrCommand,            0},
  {L7_FFTP_COMPONENT_ID,                 L7_CNFGR_COMPONENT_DISABLE,  fftpApiCnfgrCommand,           0},
  {L7_CDA_COMPONENT_ID,                  L7_CNFGR_COMPONENT_DISABLE,  cdaApiCnfgrCommand,            0},
#ifdef L7_NSF_PACKAGE
  {L7_CKPT_COMPONENT_ID,                 L7_CNFGR_COMPONENT_DISABLE,  ckptApiCnfgrCommand,           0},
#endif
#endif

#ifdef L7_ROUTING_PACKAGE
  {L7_IP_MAP_COMPONENT_ID,               L7_CNFGR_COMPONENT_ENABLE,  ipMapApiCnfgrCommand,          L7_CNFGR_HW_APPLY_CONFIG | L7_CNFGR_HW_APPLY_L3},
  {L7_ARP_MAP_COMPONENT_ID,              L7_CNFGR_COMPONENT_ENABLE,  L7_NULLPTR,                    L7_CNFGR_HW_APPLY_L3},
#ifdef L7_RIP_PACKAGE
  {L7_RIP_MAP_COMPONENT_ID,              L7_CNFGR_COMPONENT_ENABLE,  ripMapApiCnfgrCommand,         0},
#endif
#ifdef L7_OSPF_PACKAGE
  {L7_OSPF_MAP_COMPONENT_ID,             L7_CNFGR_COMPONENT_ENABLE,  ospfMapApiCnfgrCommand,        0},
#endif
#ifdef L7_VRRP_PACKAGE
  {L7_VRRP_MAP_COMPONENT_ID,             L7_CNFGR_COMPONENT_ENABLE,  vrrpApiCnfgrCommand,           0},
#endif
  {L7_DHCP_RELAY_COMPONENT_ID,           L7_CNFGR_COMPONENT_ENABLE,  dhcpRelayApiCnfgrCommand,      0},
  {L7_RTR_DISC_COMPONENT_ID,             L7_CNFGR_COMPONENT_ENABLE,  rtrDiscApiCnfgrCommand,        0},
#ifdef L7_RLIM_PACKAGE
  {L7_RLIM_COMPONENT_ID,                 L7_CNFGR_COMPONENT_ENABLE,  rlimApiCnfgrCommand,           L7_CNFGR_HW_APPLY_L3},
#endif
#endif

#ifdef L7_BGP_PACKAGE
  {L7_FLEX_BGP_MAP_COMPONENT_ID,         L7_CNFGR_COMPONENT_DISABLE,  bgpApiCnfgrCommand,            0},
#endif
/* PTin modified: mcast is now disabled! */
#ifdef L7_MCAST_PACKAGE
  {L7_FLEX_MCAST_MAP_COMPONENT_ID,       L7_CNFGR_COMPONENT_DISABLE,  mcastApiCnfgrCommand,          L7_CNFGR_HW_APPLY_IPMCAST},
  {L7_FLEX_MGMD_MAP_COMPONENT_ID,        L7_CNFGR_COMPONENT_DISABLE,  mgmdApiCnfgrCommand,           L7_CNFGR_HW_APPLY_IPMCAST},
  {L7_FLEX_DVMRP_MAP_COMPONENT_ID,       L7_CNFGR_COMPONENT_DISABLE,  dvmrpApiCnfgrCommand,          0},
  {L7_FLEX_PIMDM_MAP_COMPONENT_ID,       L7_CNFGR_COMPONENT_DISABLE,  pimdmApiCnfgrCommand,          L7_CNFGR_HW_APPLY_IPMCAST},
  {L7_FLEX_PIMSM_MAP_COMPONENT_ID,       L7_CNFGR_COMPONENT_DISABLE,  pimsmApiCnfgrCommand,          L7_CNFGR_HW_APPLY_IPMCAST},
#endif

#ifdef L7_QOS_PACKAGE
#ifdef L7_QOS_FLEX_PACKAGE_DIFFSERV
  {L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,    L7_CNFGR_COMPONENT_ENABLE,  diffServApiCnfgrCommand,       L7_CNFGR_HW_APPLY_CONFIG},
#endif

#ifdef L7_QOS_FLEX_PACKAGE_ACL
  {L7_FLEX_QOS_ACL_COMPONENT_ID,         L7_CNFGR_COMPONENT_ENABLE,  aclApiCnfgrCommand,            L7_CNFGR_HW_APPLY_CONFIG},
#endif

#ifdef L7_QOS_FLEX_PACKAGE_COS
  {L7_FLEX_QOS_COS_COMPONENT_ID,         L7_CNFGR_COMPONENT_ENABLE,  cosApiCnfgrCommand,            L7_CNFGR_HW_APPLY_CONFIG},
#endif
#ifdef L7_QOS_FLEX_PACKAGE_VOIP
  {L7_FLEX_QOS_VOIP_COMPONENT_ID,        L7_CNFGR_COMPONENT_ENABLE,  voipApiCnfgrCommand,           L7_CNFGR_HW_APPLY_CONFIG},
#endif
#ifdef L7_QOS_FLEX_PACKAGE_ISCSI
  {L7_FLEX_QOS_ISCSI_COMPONENT_ID,       L7_CNFGR_COMPONENT_ENABLE,  iscsiApiCnfgrCommand,          L7_CNFGR_HW_APPLY_CONFIG},
#endif
#endif /* QOS */
#ifdef L7_ROUTING_PACKAGE
#ifdef L7_IPV6_PACKAGE
  {L7_FLEX_IPV6_MAP_COMPONENT_ID,        L7_CNFGR_COMPONENT_DISABLE,  ip6MapApiCnfgrCommand,         L7_CNFGR_HW_APPLY_CONFIG | L7_CNFGR_HW_APPLY_L3},
#ifdef L7_OSPFV3
  {L7_FLEX_OSPFV3_MAP_COMPONENT_ID,      L7_CNFGR_COMPONENT_DISABLE,  ospfv3MapApiCnfgrCommand,      0},
#endif
  {L7_FLEX_DHCP6S_MAP_COMPONENT_ID,      L7_CNFGR_COMPONENT_ENABLE,  dhcp6sApiCnfgrCommand,         0},
#endif
#endif
#ifdef L7_SERVICES_PACKAGE
  {L7_SERVICES_COMPONENT_ID,             L7_CNFGR_COMPONENT_DISABLE,  servicesApiCnfgrCommand,       0},
#endif
#ifdef L7_CHASSIS
  {L7_ALARMMGR_COMPONENT_ID,             L7_CNFGR_COMPONENT_DISABLE, alarmApiCnfgrCommand,          0},
  {L7_CHASSIS_ALARM_COMPONENT_ID,        L7_CNFGR_COMPONENT_DISABLE, chassisAlarmApiCnfgrCommand,   0},
  {L7_POWER_UNIT_MGR_COMPONENT_ID,       L7_CNFGR_COMPONENT_DISABLE, pumgrApiCnfgrCommand,          0},
  {L7_FAN_UNIT_MGR_COMPONENT_ID,         L7_CNFGR_COMPONENT_DISABLE, fumgrApiCnfgrCommand,          0},
#endif

  {L7_LLDP_COMPONENT_ID,                 L7_CNFGR_COMPONENT_ENABLE,  lldpApiCnfgrCommand,           L7_CNFGR_HW_APPLY_CONFIG},
  {L7_DOT1X_AUTH_SERV_COMPONENT_ID,      L7_CNFGR_COMPONENT_ENABLE,  dot1xAuthServApiCnfgrCommand,          0},
#ifndef L7_PRODUCT_SMARTPATH
  {L7_DNS_CLIENT_COMPONENT_ID,           L7_CNFGR_COMPONENT_ENABLE,  dnsApiCnfgrCommand,            0},
#endif
#ifdef L7_WIRELESS_PACKAGE
  {L7_FLEX_WIRELESS_COMPONENT_ID,        L7_CNFGR_COMPONENT_DISABLE,  wirelessApiCnfgrCommand,       0},
#endif
#ifdef L7_IPV6_MGMT_PACKAGE
  {L7_FLEX_IPV6_MGMT_COMPONENT_ID,       L7_CNFGR_COMPONENT_ENABLE,  L7_NULLPTR,                    0},
#endif
#ifdef L7_FLEX_CAPTIVE_PORTAL_PACKAGE
  {L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,  L7_CNFGR_COMPONENT_DISABLE,  captivePortalApiCnfgrCommand,  L7_CNFGR_HW_APPLY_CONFIG},
#endif
/* captive portal wired interface owner component only runs if captive portal runs */
#ifdef L7_CP_WIO_PACKAGE
    {L7_CP_WIO_COMPONENT_ID,             L7_CNFGR_COMPONENT_DISABLE, wioApiCnfgrCommand, L7_CNFGR_HW_APPLY_CONFIG},
#endif
#ifdef L7_AUTO_INSTALL_PACKAGE
  {L7_AUTO_INSTALL_COMPONENT_ID,         L7_CNFGR_COMPONENT_DISABLE,  autoInstallApiCnfgrCommand,    0},
#endif
  {L7_DHCP_CLIENT_COMPONENT_ID,          L7_CNFGR_COMPONENT_ENABLE,  dhcpClientApiCnfgrCommand,     0},
#ifdef L7_ISDP_PACKAGE
  {L7_ISDP_COMPONENT_ID,                 L7_CNFGR_COMPONENT_ENABLE,  isdpApiCnfgrCommand,           0},
#endif /* L7_ISDP_PACKAGE */
#ifdef L7_LLPF_PACKAGE
  {L7_LLPF_COMPONENT_ID,                 L7_CNFGR_COMPONENT_ENABLE,  llpfApiCnfgrCommand,          L7_CNFGR_HW_APPLY_CONFIG},
#endif /* L7_LLPF_PACKAGE */
#ifdef L7_RMON_PACKAGE
  {L7_RMON_COMPONENT_ID,                 L7_CNFGR_COMPONENT_ENABLE,  rmonApiCnfgrCommand,           0},
#endif /* L7_RMON_PACKAGE */
#ifndef L7_PRODUCT_SMARTPATH
  {L7_BOX_SERVICES_COMPONENT_ID,         L7_CNFGR_COMPONENT_ENABLE,  boxsApiCnfgrCommand,           0},
#if L7_FEAT_LINK_DEPENDENCY==1
  {L7_LINK_DEPENDENCY_COMPONENT_ID,      L7_CNFGR_COMPONENT_ENABLE,  linkDependencyApiCnfgrCommand, 0},
#endif
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  {L7_DHCP6C_COMPONENT_ID,               L7_CNFGR_COMPONENT_ENABLE,  dhcp6cApiCnfgrCommand,         0},
#endif

  /* PTin added: ptin module */  
  {L7_PTIN_COMPONENT_ID,                 L7_CNFGR_COMPONENT_ENABLE,  ptinApiCnfgrCommand,           0},

  {L7_LAST_COMPONENT_ID,                 L7_CNFGR_COMPONENT_ENABLE,  L7_NULLPTR,                    0}
#endif
};

/* ===================================================================
 *  Configurator General--
 * ===================================================================
*/

/*********************************************************************
* @purpose  Make sure all component mnemonic and name strings are terminated
*
* @param    void
*
* @returns  void
*
* @notes    This guards against any names defined in excess of the maximum
*           allowed length.
*
* @end
*********************************************************************/
void cnfgrSidComponentNameCleanup(void)
{
  CNFGR_COMPONENT_NAME_LIST_ENTRY_t *p = cnfgrComponentNameList;

  while (p->cid != L7_LAST_COMPONENT_ID)
  {
#ifndef L7_PRODUCT_SMARTPATH
    p->mnemonic[L7_COMPONENT_MNEMONIC_MAX_LEN-1] = L7_EOS;
    p->name[L7_COMPONENT_NAME_MAX_LEN-1] = L7_EOS;
#endif
    p++;
  }
}

/*********************************************************************
* @purpose  Get options for semaphores
*
* @param    void
*
* @returns  CNFGR_SEMAPHORE_OPTIONS
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 cnfgrSidSemaphoreOptionsGet()
{
  return (CNFGR_SEMAPHORE_OPTIONS);
}

/* ===================================================================
 *  Component Control (CCTRL)--
 * ===================================================================
*/

/*********************************************************************
* @purpose  Get maximum number of callback supported by the configurator.
*
* @param    void
*
* @returns  CNFGR_CTRL_CALLBACK_MAX
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 cnfgrSidCtrlCallbackMaxGet()
{
  return (CNFGR_CTRL_CALLBACK_MAX);
}

/* ===================================================================
 * Message Handler --
 * ===================================================================
*/

/*********************************************************************
* @purpose  Get maximum number of Log Messages to issued.
*
* @param    void
*
* @returns  CNFGR_MSG_LOG_MAX_COUNT
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 cnfgrSidMsgLogMaxCountGet()
{
  return (CNFGR_MSG_LOG_MAX_COUNT);
}

/* Message Queues */
/*********************************************************************
* @purpose  Get maximum number of noop to be processed.
*
* @param    void
*
* @returns  CNFGR_MSG_NOOP_COUNT
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 cnfgrSidMsgNoopCountGet()
{
  return ( CNFGR_MSG_NOOP_COUNT);
}

/* Message Q1 - Receive messages when !EIP.
 *
 */
/*********************************************************************
* @purpose  Get size of Q1.
*
* @param    void
*
* @returns  CNFGR_MSG_Q1_COUNT
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 cnfgrSidMsgQ1CountGet()
{
  return ( CNFGR_MSG_Q1_COUNT);
}

/* Message Q2 - Receive messages when EIP.
 *
 */
/*********************************************************************
* @purpose  Get size of Q2.
*
* @param    void
*
* @returns  CNFGR_MSG_Q2_COUNT
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 cnfgrSidMsgQ2CountGet()
{
  return ( CNFGR_MSG_Q2_COUNT);
}

/* Message Handler Thread -
 *
*/
/*********************************************************************
* @purpose  Get thread priority value.
*
* @param    void
*
* @returns  CNFGR_MSG_HANDLER_THREAD_PRIO
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 cnfgrSidMsgHandlerThreadPrioGet()
{
  return ( CNFGR_MSG_HANDLER_THREAD_PRIO);
}

/*********************************************************************
* @purpose  Get thread priority value.
*
* @param    void
*
* @returns  CNFGR_MSG_HANDLER_THREAD_STACK_SIZE
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 cnfgrSidMsgHandlerThreadStackSizeGet()
{
  return ( CNFGR_MSG_HANDLER_THREAD_STACK_SIZE );
}

/*********************************************************************
* @purpose  Get thread time slice value.
*
* @param    void
*
* @returns  CNFGR_MSG_HANDLER_THREAD_SLICE
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 cnfgrSidMsgHandlerThreadSliceGet()
{
  return ( CNFGR_MSG_HANDLER_THREAD_SLICE );
}

/* ===================================================================
 * Callback Tally --
 * ===================================================================
*/

/* Tally handle -
 *
*/
/*********************************************************************
* @purpose  Get maximun number of handles.
*
* @param    void
*
* @returns  CNFGR_TALLY_HANDLE_MAX
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 cnfgrSidTallyHandleMaxGet()
{
  return ( CNFGR_TALLY_HANDLE_MAX );
}

/*********************************************************************
* @purpose  Get maximun number of response list entries.
*
* @param    void
*
* @returns  CNFGR_TALLY_RSP_LIST_MAX
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 cnfgrSidTallyRsplMaxGet()
{
  return ( CNFGR_TALLY_RSP_LIST_MAX );
}

/* Correlator pool -
 *
*/
/*********************************************************************
* @purpose  Get maximun number of correlators.
*
* @param    void
*
* @returns  CNFGR_TALLY_CORRELATOR_MAX
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 cnfgrSidTallyCorrelatorMaxGet()
{
  return ( CNFGR_TALLY_CORRELATOR_MAX );
}


/* Tally Watchdog Timer -
 *
*/
/*********************************************************************
* @purpose  Get watchdog time value.
*
* @param    void
*
* @returns  CNFGR_TALLY_WATCHDOG_TIME
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 cnfgrSidTallyWatchdogTimeGet()
{
  return ( CNFGR_TALLY_WATCHDOG_TIME );
}


/*********************************************************************
* @purpose  Get watchdog timer retry coout value.
*
* @param    void
*
* @returns  CNFGR_TALLY_WATCHDOG_RETRY
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 cnfgrSidTallyWatchdogRetryGet()
{
  return ( CNFGR_TALLY_WATCHDOG_RETRY );
}

/* ===================================================================
 * Component Repository --
 * ===================================================================
*/

/*********************************************************************
* @purpose  Get component table maximum size value.
*
* @param    void
*
* @returns  CNFGR_CR_COMPONENT_TABLE_END
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 cnfgrSidCrComponentTableEndGet()
{
  return ( CNFGR_CR_COMPONENT_TABLE_END );
}

/*********************************************************************
* @purpose  Get component list address.
*
* @param    void
*
* @returns  CNFGR_COMPONENT_LIST_ENTRY_t *
*
* @notes    none
*
* @end
*********************************************************************/
CNFGR_COMPONENT_LIST_ENTRY_t * cnfgrSidCrComponentListAddrGet(void )
{
  return (cnfgrComponentList);
}

/*********************************************************************
* @purpose  Determine if specified component should be started even
*           when unit can't become manager.
*
* @param    component_id
*
* @returns  L7_TRUE - Component should be started.
*           L7_FALSE - Component should not be started.
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL cnfgrSidNonMgrComponentCheck(L7_COMPONENT_IDS_t component_id)
{
  L7_uint32 i;
  L7_uint32 max_index;

  max_index = sizeof (cnfgrNonMgrComponentList) / sizeof (L7_COMPONENT_IDS_t);

  for (i = 0; i < max_index; i++)
  {
    if (cnfgrNonMgrComponentList[i] == component_id)
    {
      return L7_TRUE;
    }
  }

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Determine component name for the specified component ID.
*
* @param    component_id
*
* @returns  component name - Name of the component, if component is found.
*           L7_NULLPTR - Component is not found.
*
* @notes    none
*
* @end
*********************************************************************/
L7_char8 *cnfgrSidComponentNameGet(L7_COMPONENT_IDS_t component_id)
{
  L7_uint32 i;
  L7_uint32 max_index;

  max_index = sizeof (cnfgrComponentNameList) / sizeof (CNFGR_COMPONENT_NAME_LIST_ENTRY_t);

  for (i = 0; i < max_index; i++)
  {
    if (cnfgrComponentNameList[i].cid == component_id)
    {
      return cnfgrComponentNameList[i].name;
    }
  }

  return L7_NULLPTR;
}

/*********************************************************************
* @purpose  Determine component mnemonic for the specified component ID.
*
* @param    component_id
*
* @returns  component name - Name of the component, if component is found.
*           L7_NULLPTR - Component is not found.
*
* @notes    none
*
* @end
*********************************************************************/
L7_char8 *cnfgrSidComponentMnemonicGet(L7_COMPONENT_IDS_t component_id)
{
  L7_uint32 i;
  L7_uint32 max_index;

  max_index = sizeof (cnfgrComponentNameList) / sizeof (CNFGR_COMPONENT_NAME_LIST_ENTRY_t);

  for (i = 0; i < max_index; i++)
  {
    if (cnfgrComponentNameList[i].cid == component_id)
    {
      return cnfgrComponentNameList[i].mnemonic;
    }
  }

  return L7_NULLPTR;
}
