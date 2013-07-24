/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename snmp_sid.c
*
* @purpose Structural data for snmp component
*
* @component snmp
*
* @comments adapted from fdb_sid_const.h
*
* @create 07/08/2003
*
* @author cpverne
*
* @end
*             
**********************************************************************/
#ifndef SNMP_SID_CONST_H
#define SNMP_SID_CONST_H

#include "default_cnfgr.h"
#include "l7_product.h"
#include "defaultconfig.h"

#define SNMP_SID_DEFAULT_TASK_PRI            L7_TASK_PRIORITY_LEVEL(FD_CNFGR_SNMP_DEFAULT_TASK_PRI)

#define SNMP_SID_DEFAULT_STACK_SIZE          FD_CNFGR_SNMP_DEFAULT_STACK_SIZE

#define SNMP_SID_DEFAULT_TASK_SLICE          FD_CNFGR_SNMP_DEFAULT_TASK_SLICE

#define SNMP_SID_DEFAULT_MSG_SIZE            FD_CNFGR_SNMP_DEFAULT_MSG_SIZE

#endif
