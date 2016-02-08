/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    task_budgeter.h
* @purpose     Task budgeter  definitions
* @component   Task budgeter
* @comments    none
* @create      11/14/2003
* @author      betsyt
* @end
*             
**********************************************************************/
#ifndef _TASKBUDGETER_H_
#define _TASKBUDGETER_H_

#include "l7_common.h"

typedef enum
{
    TASK_REGISTER,
    TASK_UNREGISTER,
    TASK_EXECUTE,
} sysBudgeterType_t;


typedef struct
{
    L7_int32 taskID;
    L7_int32 priority;
} taskInfo_t;

#ifdef L7_TASKBUDGETER

void budgeterTaskRegister(taskInfo_t *taskData);   
void budgeterTaskUnregister(taskInfo_t *taskData); 
void budgeterTaskExecute(taskInfo_t *taskData);             
void budgeterTaskHandlerRegister(void (*extBudgeterTaskHandler)(sysBudgeterType_t type, taskInfo_t *taskData));

#else

#define budgeterTaskRegister(args...) {}
#define budgeterTaskUnregister(args...) {} 
#define budgeterTaskExecute(args...) {}        
#define budgeterTaskHandlerRegister(args...) {}

#endif /* L7_TASKBUDGETER */

#endif /*_TASKBUDGETER_H_*/
