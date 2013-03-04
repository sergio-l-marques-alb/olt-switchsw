/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename defaultconfig.h
*
* @purpose All Factory default settings are in this file
*
* @component cnfgr
*
* @comments none
*
* @create 11/01/2004
*
* @author jpickering
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#ifndef INCLUDE_IPV6_DEFAULTCONFIG
#define INCLUDE_IPV6_DEFAULTCONFIG


#define FD_CNFGR_IPV6_MAX_MSGS_SERVICED      512


/* Maximum number of timer ticks to sleep when IP forwarder has serviced its max msgs */
#define FD_CNFGR_IPV6_MAX_MSGS_TASK_DELAY    4
#define FD_IPV6_DEFAULT_TTL                   64         /* hops    */
#define FD_RTRV6_DEFAULT_ADMIN_MODE            L7_DISABLE
#define FD_RTRV6_DEFAULT_FORWARDING            L7_ENABLE
#define FD_RTRV6_DEFAULT_ECMP_MODE             L7_DISABLE
#define FD_RTRV6_DEFAULT_TRACE_MODE            L7_DISABLE 

/* max age of IPv6 neighbor cache entry. Stale entries get garbage 
 * collected after this time. */
#define FD_NCE_STALE_TIME  1200

/*  Routing Protocol Default Preferences             */

#define    FD_RTR_RTOV6_PREF_UNSUPPORTED     255
#define    FD_RTR_RTOV6_PREF_LOCAL           0
#define    FD_RTR_RTOV6_PREF_STATIC          1
#define    FD_RTR_RTOV6_PREF_MPLS            FD_RTR_RTOV6_PREF_UNSUPPORTED
#define    FD_RTR_RTOV6_PREF_OSPF_INTRA      110
#define    FD_RTR_RTOV6_PREF_OSPF_INTER      110
#define    FD_RTR_RTOV6_PREF_OSPF_EXTERNAL   110
#define    FD_RTR_RTOV6_PREF_RIP             FD_RTR_RTOV6_PREF_UNSUPPORTED
#define    FD_RTR_RTOV6_PREF_IBGP            FD_RTR_RTOV6_PREF_UNSUPPORTED 
#define    FD_RTR_RTOV6_PREF_EBGP            FD_RTR_RTOV6_PREF_UNSUPPORTED 
   

/* Start of CONFIGURATOR DHCP6S Component's Factory Defaults */

/* dhcp6sCfgData_t default values */
#define FD_DHCP6S_DEFAULT_ADMIN_MODE                     L7_DISABLE
#define FD_DHCP6S_DEFAULT_RELAYOPT_TYPE                  54  /* TBD : draft-droms-dhc-v6-relayopt-00.txt */
#define FD_DHCP6S_DEFAULT_RELAYOPT_REMOTEID_TYPE         1

/*  poolCfgData_t default values */
#define FD_DHCP6S_DEFAULT_POOLCFG_TYPEOFBINDING          L7_DHCP6S_INACTIVE_POOL
#define FD_DHCP6S_DEFAULT_SERVER_PREF                    10

#define FD_CNFGR_DHCP6S_DEFAULT_STACK_SIZE       L7_IPV6_DEFAULT_STACK_SIZE
#define FD_CNFGR_DHCP6S_DEFAULT_TASK_SLICE       L7_IPV6_DEFAULT_TASK_SLICE
#define FD_CNFGR_DHCP6S_DEFAULT_TASK_PRI         L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)

#define DHCP6S_PROC_TASK "DHCPv6 Server Processing Task"
#define DHCP6S_PROC_QUEUE "DHCPv6 Server Processing Queue"

/* End of DHCP6S Server Factory Defaults */

/*------------------------------------------*/
/* DHCP6 Server/Relay Parameter Limits      */
/*------------------------------------------*/

/* Maximum number of interfaces to config DHCPv6 Server/Relay */
#define L7_DHCP6S_MAX_INTERFACES_NUMBER     24

/*------------------------------------------*/
/*  END DHCP6 Server/Relay Parameter Limits */
/*------------------------------------------*/

#define FD_IPV6_DEFAULT_INTERFACE_PREFIX_PREFER_LIFETIME              604800 
#define FD_IPV6_DEFAULT_INTERFACE_PREFIX_VALID_LIFETIME               2592000

/* END IPV6 */


#endif 
