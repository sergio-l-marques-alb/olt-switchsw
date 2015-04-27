/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename dot3ad_sid_const.h
*
* @purpose Structural data for dot3ad
*
* @component dot3ad
*
* @comments none
*
* @create 06/20/2003
*
* @author 
*
* @end
*             
**********************************************************************/
#ifndef DOT3AD_SID_CONST_H
#define DOT3AD_SID_CONST_H

#include "default_cnfgr.h"
#include "l7_product.h"
#include "defaultconfig.h"

#define DOT3AD_SID_DEFAULT_TASK_PRI          L7_TASK_PRIORITY_LEVEL(L7_MEDIUM_TASK_PRIORITY)

#define DOT3AD_SID_DEFAULT_STACK_SIZE        FD_CNFGR_FDB_DEFAULT_STACK_SIZE

#define DOT3AD_SID_DEFAULT_TASK_SLICE        FD_CNFGR_FDB_DEFAULT_TASK_SLICE

#define DOT3AD_SID_LAG_MAX_FRAME_SIZE        FD_NIM_DEFAULT_MAX_FRAME_SIZE

#endif

