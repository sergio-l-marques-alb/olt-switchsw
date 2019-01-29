/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename commdefs_pkg.h
*
* @purpose The purpose of this file is to have a central location for
*          stub #defines needed by both 201 and 300 projects.
*
* @component sysapi
*
* @comments none
*
* @create 7/27/2001
*
* @author gaunce
* @end
*
**********************************************************************/

/**************************@null{*************************************
                     
 *******************************}*************************************}
**********************************************************************/

#ifndef INCLUDE_COMMDEFS_PKG
#define INCLUDE_COMMDEFS_PKG

#include "flex.h"
#ifdef L7_ROUTING_LM_PACKAGE
#define L7_ROUTING_PACKAGE 1
#endif

#ifdef L7_SWITCHING_LM_PACKAGE
#define L7_SWITCHING_PACKAGE 1
#endif


/*--------------------------------------*/
/*  MIB CAPABILITY CONSTANTS            */                          
/*--------------------------------------*/

#ifdef L7_ROUTING_PACKAGE

#define L7_MIB_DESCR_LEN     36     /* 201/300 */
#define L7_MIB_SIZE           8     /* 201/300 */

#define MIB_CAPABILITY_DESCR_RFC_1            "RFC 1213 MIB2"                   /* 201/300 */
#define MIB_CAPABILITY_DESCR_RFC_2            "RFC 1493 dot1dBRIDGE"            /* 201/300 */
#define MIB_CAPABILITY_DESCR_RFC_3            "RFC 1643 802.3"                  /* 201/300 */
#define MIB_CAPABILITY_DESCR_RFC_4            "RFC 1757 RMON"                   /* 201/300 */
#define MIB_CAPABILITY_DESCR_RFC_5            "RFC 2233 Interfaces Group MIB"   /* 201/300 */
#define MIB_CAPABILITY_DESCR_RFC_6            "RFC 2674 VLAN MIB"               /* 201/300 */

#define MIB_CAPABILITY_DESCR_RFC_7            "RFC 1724 RIP V2"                 /* 300 */
#define MIB_CAPABILITY_DESCR_RFC_8            "RFC 1850 OSPF"                   /* 300 */

#endif


#ifdef L7_SWITCHING_PACKAGE

#define L7_MIB_DESCR_LEN     36     /* 201/300 */
#define L7_MIB_SIZE           6     /* 201/300 */

#define MIB_CAPABILITY_DESCR_RFC_1            "RFC 1213 MIB2"                   /* 201/300 */
#define MIB_CAPABILITY_DESCR_RFC_2            "RFC 1493 dot1dBRIDGE"            /* 201/300 */
#define MIB_CAPABILITY_DESCR_RFC_3            "RFC 1643 802.3"                  /* 201/300 */
#define MIB_CAPABILITY_DESCR_RFC_4            "RFC 1757 RMON"                   /* 201/300 */
#define MIB_CAPABILITY_DESCR_RFC_5            "RFC 2233 Interfaces Group MIB"   /* 201/300 */
#define MIB_CAPABILITY_DESCR_RFC_6            "RFC 2674 VLAN MIB"               /* 201/300 */

#endif


#endif
