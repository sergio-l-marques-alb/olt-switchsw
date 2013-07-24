/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   nim_startup.h
*
* @purpose    The functions that handle the nim startup notification
*
* @component  NIM
*
* @comments   none
*
* @create     09/24/2008
*
* @author     bradyr
* @end
*
**********************************************************************/

#ifndef NIM_STARTUP_H
#define NIM_STARTUP_H

#include "l7_common.h"
/*#include "nim_data.h"*/
#include "avl_api.h"

/* Startup Tree Data structure */
typedef struct
{

  L7_uint32           priority;
  L7_COMPONENT_IDS_t  componentId;
  StartupNotifyFcn    startupFunction;
  void *avlData;

} nimStartUpTreeData_t;

/*********************************************************************
* @purpose  Allocate the memory for the startup AVL tree
*
* @param    void
*
* @returns  void
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t nimStartUpPhaseOneInit();

/*********************************************************************
* @purpose  Create an StartUp
*
* @param    componentId   @b{(input)}   Component ID of startup function
* @param    priority      @b{(input)}   priority to execute startup function
* @param    startupFcn    @b{(input)}   Function pointer to startup routine
*
* @notes   
*       
* @end
*
*********************************************************************/
void nimStartUpCreate(L7_COMPONENT_IDS_t componentId,
                      L7_uint32 priority,
                      StartupNotifyFcn startupFcn);

/*********************************************************************
* @purpose  Delete an StartUp
*
* @param    componentId   @b{(input)}   Component ID of startup function
* @param    priority      @b{(input)}   priority to execute startup function
*
* @notes   
*
*       
* @end
*
*********************************************************************/
void nimStartUpDelete(L7_COMPONENT_IDS_t componentId, L7_uint32 priority);

/*********************************************************************
* @purpose  Find the first startup function
*
* @param    pRetData     @b{(input)}  pointer to return data
*
* @returns  L7_SUCCESS if an entry exists
* @returns  L7_FAILURE if no entry exists
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t nimStartUpFirstGet(nimStartUpTreeData_t *pRetData);

/*********************************************************************
* @purpose  Find the next startup function
*
* @param    pRetData     @b{(input)}  pointer to search/return data
*
* @returns  L7_SUCCESS if a next entry exists
* @returns  L7_FAILURE if no next entry exists
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t nimStartUpNextGet(nimStartUpTreeData_t *pRetData);

/*********************************************************************
*
* @purpose  Waits for the component to complete its STARTUP processing,
*           then gives the nimStartUpEvSema signaling cardmgr to proceed
*           to the next component.
*
* @param    none
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
extern void nimStartupEventWait(void);

#endif /* NIM_STARTUP_H */
