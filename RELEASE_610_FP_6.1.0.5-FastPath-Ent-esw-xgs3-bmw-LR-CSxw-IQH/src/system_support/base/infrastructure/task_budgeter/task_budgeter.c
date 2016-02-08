/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    task_budgeter.c
* @purpose     CPU Task Budgeter functions
* @component   Task Budgeter
* @comments    none
* @create      11/17/2003
* @author      betsyt
* @end
*             
**********************************************************************/
#define L7_TASKBUDGETER

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "task_budgeter.h"

static void (*extBudgeterTaskHandler)(sysBudgeterType_t type, taskInfo_t *taskInfo) = 0;

/*********************************************************************
* @purpose  Registers a task for Budgeter Control
*
* @parms    taskData - Task Information
*
* @returns  None
*
* @notes    Not defined for use.  Spec TBD
*
* @end
*********************************************************************/
/* Internal APIs */
void budgeterTaskRegister(taskInfo_t *taskData)
{
    return;
}

/*********************************************************************
* @purpose  DeRegisters a task for Budgeter Control
*
* @parms    taskData - Task Information
*
* @returns  None
*
* @notes    Not defined for use.  Spec TBD
*
* @end
*********************************************************************/
void budgeterTaskUnregister(taskInfo_t *taskData)
{
    return;
}

/*********************************************************************
* @purpose  Informs an external budgeter that a task desires to execute
*
* @parms    taskData - Task Information
*
* @returns  None
*
* @notes
*
* @end
*********************************************************************/
void budgeterTaskExecute(taskInfo_t *taskData)
{
    if (extBudgeterTaskHandler != L7_NULL) {
        extBudgeterTaskHandler(TASK_EXECUTE, taskData);
    }
    return;
}

/* External APIs */

/*********************************************************************
* @purpose  Registers an external budgeter for task management
*
* @parms    extBudgeterTaskHandler - External Task Scheduler
*
* @returns  None
*
* @notes
*
* @end
*********************************************************************/
void budgeterTaskHandlerRegister(void (*extBudgeterTaskHandler)(sysBudgeterType_t type, taskInfo_t *taskData)) {
    extBudgeterTaskHandler = extBudgeterTaskHandler;
    return;
}
