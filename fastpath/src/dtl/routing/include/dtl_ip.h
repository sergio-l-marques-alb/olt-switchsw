/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename dtl_ip.h
*
* @purpose This header file contains definitions to support the
*          L3 IP component.
*
* @component Device Transformation Layer
*
* @comments none
*
* @author   Shekhar Kalyanam 3/14/2001
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/



#ifndef INCLUDE_DTLCTRL_IP_H
#define INCLUDE_DTLCTRL_IP_H

#include "dtl_l3_api.h"
#include "dapi.h"
#include "sysapi.h"
#include "sysapi.h"
/*
***********************************************************************
*                           CONSTANTS
***********************************************************************
*/


/*
***********************************************************************
*                           DATA TYPES
***********************************************************************
*/


/*
**********************************************************************
*                           GLOBAL DATA
**********************************************************************
*/
#ifdef  DTLCTRL_IP_GLOBALS
  #define DTLCTRL_IP_EXT
#else
  #define DTLCTRL_IP_EXT extern
#endif  /*DTLCTRL_IP_GLOBALS*/



/*
***************************************
*
*  DEBUGGING MACROS, FUNCTION PROTOTYPES
*
****************************************
*/

/* Usage Note:
   To toggle between logging and printf modes,
   comment or uncomment the following #define.
*/
#define DTL_IPV4_DEBUG_LOG


#ifdef DTL_IPV4_DEBUG_LOG


  #define DTL_IP_TRACE(format,args...)                      \
{                                                           \
   if ( ipMapTraceModeGet() == L7_ENABLE)                   \
   {                                                        \
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DTL_COMPONENT_ID, format,##args); \
   }                                                        \
}

  #define DTL_IPV4_ERROR(format,args...)                      \
{                                                           \
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DTL_COMPONENT_ID, format,##args); \
}

#else

  #define DTL_IP_TRACE(format,args...)                      \
{                                                           \
   if ( ipMapTraceModeGet() == L7_ENABLE)                   \
   {                                                        \
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP,format,##args);    \
   }                                                        \
}

  #define DTL_IPV4_ERROR(format,args...)                          \
{                                                               \
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP_ERROR,format,##args);  \
}

#endif

/*
***********************************************************************
*                       FUNCTION PROTOTYPES -- DTL INTERNAL USE ONLY
***********************************************************************
*/


L7_RC_t dtlRoutingIntfCmdBuild(dtlRtrIntf_t *ipCircuit,
                               DAPI_ROUTING_INTF_MGMT_CMD_t *dapiCmd);
L7_RC_t dtlIpv4RoutingIntfModifyCmdBuild(dtlRtrIntfDesc_t *ipCircDesc,
                                         DAPI_ROUTING_INTF_MGMT_CMD_t *dapiCmd);
L7_RC_t dtlIpv4AddrCmdBuild(L7_uint32 intIfNum, L7_IP_ADDR_t ipAddr,
                            L7_IP_MASK_t ipMask,
                            DAPI_ROUTING_INTF_MGMT_CMD_t *dapiCmd);


#endif      /*INCLUDE_DTLCTRL_IP_H*/
