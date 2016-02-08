/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    resource_manager.c
* @purpose     Resource Manager functions
* @component   Resource Manager
* @comments    none
* @create      11/14/2003
* @author      betsyt
* @end
*             
**********************************************************************/
#define L7_RESOURCEMANAGER

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "resource_manager.h"

static L7_BOOL (*RMHandler)(resourceOperationType optype, resourceType_t rtype, RM_Data_t *data) = L7_NULL;

/*********************************************************************
* @purpose  Obtains permission from external resource manager to create
*           resource  
*
* @parms    rtype - Resource Type Being Created
* @parms    *data - Pointer to applicable resource definition
*
* @returns  L7_TRUE - Resource can be created
*           L7_FALSE - Resource is not available
*
* @notes    
*
* @end
*********************************************************************/
L7_BOOL RM_Create(resourceType_t rtype, RM_Data_t *data)
{
    if (RMHandler != L7_NULL) {
        return RMHandler(ALLOCATE, rtype, data);
    }
    else {
        return L7_TRUE;
    }
}

/*********************************************************************
* @purpose  Notifies the external manager that a resource is being
*           deleted
*
* @parms    rtype - Resource Type Being Deleted
* @parms    *data - Pointer to applicable resource definition
*
* @returns  None
*
* @notes    
*
* @end
*********************************************************************/
void RM_Delete(resourceType_t rtype, RM_Data_t *data)
{
    if (RMHandler != L7_NULL) {
        RMHandler(DEALLOCATE, rtype, data);
    }
    return;
}

/*********************************************************************
* @purpose  Registers an External Resource Manager for notification
*
* @parms    extRMHandler - Function to be called for Resource Management
*
* @returns  None
*
* @notes    
*
* @end
*********************************************************************/
void RM_Register(L7_BOOL (*extRMHandler)(resourceOperationType optype, resourceType_t rtype, RM_Data_t* data) )
{
    RMHandler = extRMHandler;   
    return;
}
