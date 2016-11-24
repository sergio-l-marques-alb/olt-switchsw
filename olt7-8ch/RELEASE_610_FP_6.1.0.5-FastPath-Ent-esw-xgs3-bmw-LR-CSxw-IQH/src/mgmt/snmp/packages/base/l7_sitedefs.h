/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename l7_sitedefs.h
*
* @purpose The purpose of this file is to include common includes
*          in different snmp packages
*
* @component SNMP
*
* @comments none
*
* @create 12/12/2003
*
* @author ragrawal
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/

#ifndef L7_SITEDEFS_H
#define L7_SITEDEFS_H

/* Switching package specific defines */
#include "../switching/common/switching_sitedefs.h"

#ifdef SNMP_QOS
  /* QoS package specific defines */
  #include "../qos/qos_sitedefs.h"
#endif

#include "base_sitedefs.h"

/* Security package specific defines */
#include "../security/security_sitedefs.h"

/* Defines for SNMP Undo support for RFC 1213 MIB families */
#define SR_system_UNDO

/* DO NOT PUT ANYTHING AFTER THIS #endif */
#endif /* L7_SITEDEFS_H */
