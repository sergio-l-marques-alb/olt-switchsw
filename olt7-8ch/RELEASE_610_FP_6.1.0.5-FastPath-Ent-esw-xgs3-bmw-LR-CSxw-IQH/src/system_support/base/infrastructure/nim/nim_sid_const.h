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

#define NIM_SID_DEFAULT_TASK_PRI          L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)

#define NIM_SID_DEFAULT_STACK_SIZE        FD_CNFGR_FDB_DEFAULT_STACK_SIZE

#define NIM_SID_DEFAULT_TASK_SLICE        FD_CNFGR_FDB_DEFAULT_TASK_SLICE

#endif
