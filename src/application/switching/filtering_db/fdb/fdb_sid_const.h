/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename fdb_sid.c
*
* @purpose Structural data for fdb
*
* @component fdb
*
* @comments none
*
* @create 06/20/2003
*
* @author mbaucom
*
* @end
*             
**********************************************************************/
#ifndef FDB_SID_CONST_H
#define FDB_SID_CONST_H

#include "default_cnfgr.h"
#include "l7_product.h"
#include "defaultconfig.h"

#define FDB_SID_DEFAULT_TASK_PRI          FD_CNFGR_FDB_DEFAULT_TASK_PRI

#define FDB_SID_DEFAULT_STACK_SIZE        FD_CNFGR_FDB_DEFAULT_STACK_SIZE

#define FDB_SID_DEFAULT_TASK_SLICE        FD_CNFGR_FDB_DEFAULT_TASK_SLICE

#define FDB_SID_MAX_VLAN_PER_BRIDGE       L7_MAX_VLAN_PER_BRIDGE

#define FDB_SID_DEFAULT_AGING_TIMEOUT     FD_FDB_DEFAULT_AGING_TIMEOUT

#define FDB_SID_MAX_FILTERING_DATABASES   L7_MAX_FILTERING_DATABASES

#define FDB_SID_VLAN_LEARNING_TYPE        L7_FDB_TYPE

#endif
