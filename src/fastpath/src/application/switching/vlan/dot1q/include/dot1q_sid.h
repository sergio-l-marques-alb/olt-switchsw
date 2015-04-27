/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename dot1q_sid.c
*
* @purpose Structural data for dot1q
*
* @component dot1q
*
* @comments none
*
* @create 11/1/2005
*
* @author akulkarni
*
* @end
*             
**********************************************************************/
#ifndef __DOT1Q_SID_H
#define __DOT1Q_SID_H

#include "l7_common.h"

L7_uint32 dot1qSidTaskPriorityGet(void);
L7_uint32 dot1qSidTaskStackSizeGet(void);
L7_uint32 dot1qSidTaskSliceGet(void);
L7_uint32 dot1qSidMsgCountGet(void);


#endif
