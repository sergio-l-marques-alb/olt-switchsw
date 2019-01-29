/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename edb_sid.c
*
* @purpose Structural data for edb component
*
* @component edb
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
#ifndef EDB_SID_CONST_H
#define EDB_SID_CONST_H

#include "default_cnfgr.h"
#include "l7_product.h"
#include "defaultconfig.h"

#define EDB_SID_DEFAULT_TASK_PRI            L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)

#define EDB_SID_DEFAULT_STACK_SIZE          L7_DEFAULT_STACK_SIZE

#define EDB_SID_DEFAULT_TASK_SLICE          L7_DEFAULT_TASK_SLICE

#define EDB_SID_DEFAULT_MSG_SIZE            100

#define EDB_SID_DEFAULT_TRAP_THROTTLE_TIME  5

#endif
