/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    resource_manager.h
* @purpose     Resource Manager  definitions
* @component   Resource Manager
* @comments    none
* @create      11/14/2003
* @author      betsyt
* @end
*             
**********************************************************************/
#ifndef _RESOURCEMANAGER_H_
#define _RESOURCEMANAGER_H_

#include "l7_common.h"

typedef enum
{
    RESOURCE_TYPE_VLAN
} resourceType_t;

typedef enum 
{
    ALLOCATE,
    DEALLOCATE
} resourceOperationType;

typedef union 
{
    struct 
    {       
        L7_int32 vlanID;                       
        L7_BOOL  isStatic;
    } RM_VLAN_t;

} RM_Data_t;

#ifdef L7_RESOURCEMANAGER

L7_BOOL RM_Create(resourceType_t rtype, RM_Data_t *data);
void RM_Delete(resourceType_t rtype, RM_Data_t *data);
void RM_Register(L7_BOOL (*extRMHandler)(resourceOperationType optype, resourceType_t rtype, RM_Data_t *data) );

#else

#define RM_Create(args...) (L7_TRUE)
#define RM_Delete(args...) {}
#define RM_Register(args...) {}

#endif /* L7_RESOURCEMANAGER */

#endif /* _RESOURCEMANAGER_H */
