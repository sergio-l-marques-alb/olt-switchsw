/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename defaultconfig.h
*
* @purpose All Factory default settings are in this file
*
* @component none
*
* @comments Only Factory config files should include this header
*           (ie. nim_config.c, sim_config.c)
*
* @create 07/06/2001
*
* @author diogenes
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/


#ifndef INCLUDE_CONFIG_DEFAULTS_PKG
#define INCLUDE_CONFIG_DEFAULTS_PKG

#include "commdefs_pkg.h"

typedef struct
{
  L7_char8       mibArray[L7_MIB_DESCR_LEN]; 
} mibs_t;

typedef struct
{
  mibs_t       mibSupported[L7_MIB_SIZE];
} mibDescr_t;


#ifdef L7_ROUTING_PACKAGE

/* Start of SIM Component's Factory Defaults */
#define FD_SIM_DEFAULT_SYSTEM_SERVICES      6   /* Refer to rfc1213 */
#define FD_SIM_DEFAULT_SYSTEM_DESCR         "Broadcom FASTPATH Routing"   
#define FD_CLI_WEB_COMMAND_PROMPT           "Broadcom FASTPATH Routing"


/* Start of Sim Factory Defaults */
/* Start of Sim Factory Defaults */
/* Start of Sim Factory Defaults */
/* Start of Sim Factory Defaults */
/* Start of Sim Factory Defaults */

#ifdef SIM_FACTORY_DEFAULT
static mibDescr_t FD_mibDescr[] = 
{
{ 
  { { MIB_CAPABILITY_DESCR_RFC_1 } }
},
{ 
  { { MIB_CAPABILITY_DESCR_RFC_2 } }
},
{ 
  { { MIB_CAPABILITY_DESCR_RFC_3 } }
},
{ 
  { { MIB_CAPABILITY_DESCR_RFC_4 } }
},
{ 
  { { MIB_CAPABILITY_DESCR_RFC_5 } }
},
{ 
  { { MIB_CAPABILITY_DESCR_RFC_6 } }
},
{ 
  { { MIB_CAPABILITY_DESCR_RFC_7 } }
},
{ 
  { { MIB_CAPABILITY_DESCR_RFC_8 } }
}
};

  #define FD_SIM_MIB_DESCRIPTION   8

#endif /* SIM_FACTORY_DEFAULT */  

/* End of SIM Component's Factory Defaults */
 
#endif /* L7_ROUTING_PACKAGE */




#ifdef L7_SWITCHING_PACKAGE

/* Start of SIM Component's Factory Defaults */
#define FD_SIM_DEFAULT_SYSTEM_SERVICES      2   /* Refer to rfc1213 */

#ifdef L7_PRODUCT_SMARTPATH
#define FD_SIM_DEFAULT_SYSTEM_DESCR         "Broadcom SmartPATH"   
#else
#define FD_SIM_DEFAULT_SYSTEM_DESCR         "Broadcom FASTPATH Switching"   
#endif
#define FD_CLI_WEB_COMMAND_PROMPT           "Broadcom FASTPATH Switching" 

/* Start of Sim Factory Defaults */
/* Start of Sim Factory Defaults */
/* Start of Sim Factory Defaults */
/* Start of Sim Factory Defaults */
/* Start of Sim Factory Defaults */

#ifdef SIM_FACTORY_DEFAULT
static mibDescr_t FD_mibDescr[] = 
{
{ 
  { { MIB_CAPABILITY_DESCR_RFC_1 } }
},
{ 
  { { MIB_CAPABILITY_DESCR_RFC_2 } }
},
{ 
  { { MIB_CAPABILITY_DESCR_RFC_3 } }
},
{ 
  { { MIB_CAPABILITY_DESCR_RFC_4 } }
},
{ 
  { { MIB_CAPABILITY_DESCR_RFC_5 } } 
},
{ 
  { { MIB_CAPABILITY_DESCR_RFC_6 } }
}
};

  #define FD_SIM_MIB_DESCRIPTION   6

#endif /* SIM_FACTORY_DEFAULT */  

/* End of SIM Component's Factory Defaults */

#endif /* L7_SWITCHING_PACKAGE */

#define L7_FASTPATH_FLEX_BGP_PACKAGE_STR       "FASTPATH BGP-4"
#define L7_FASTPATH_FLEX_QOS_PACKAGE_STR       "FASTPATH QOS"
#define L7_FASTPATH_FLEX_MCAST_PACKAGE_STR     "FASTPATH Multicast"
#define L7_FASTPATH_FLEX_IPV6_PACKAGE_STR      "FASTPATH IPv6"
#define L7_FASTPATH_FLEX_IPV6_MGMT_PACKAGE_STR "FASTPATH IPv6 Management"
#define L7_FASTPATH_FLEX_WIRELESS_PACKAGE_STR  "FASTPATH Wireless"
#define L7_FASTPATH_FLEX_STACKING_PACKAGE_STR  "FASTPATH Stacking"
#define L7_FASTPATH_FLEX_METRO_PACKAGE_STR     "FASTPATH Metro"


/* TBD: This customer specific checks need to be replaced with the following
   once the CChelper changes are appropriately done:
   #ifdef INCLUDE_L7_DEFAULTCONFIG_PKG_OVERRIDES
*/
#if defined(FEAT_METRO_CPE_V1_0) 
#include "defaultconfig_pkg_overrides.h"
#endif

#endif /* include_config_defaults */
