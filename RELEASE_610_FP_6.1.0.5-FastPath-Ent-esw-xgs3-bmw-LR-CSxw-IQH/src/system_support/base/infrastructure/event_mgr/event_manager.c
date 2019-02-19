/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    event_manager.c
* @purpose     Event Manager functions
* @component   Event Manager
* @comments    none
* @create      11/17/2003
* @author      betsyt
* @end
*             
**********************************************************************/
#define L7_EVENTMANAGER

#include "l7_common.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "osapi.h"
#include "event_manager.h"

struct em_Registry {
    void (*EMHandler)(sysEventType_t event, L7_char8 *eventMessage);
    struct em_Registry *next;
}; 

static struct em_Registry *head = L7_NULL;

/*********************************************************************
* @purpose  Sends Event Notification to external managers
*
* @parms    event - Event Type being notified
* @parms    *strEvent - Pointer to string describing event
*
* @returns  None
*
* @notes    
*
* @end
*********************************************************************/
void EM_LOG(sysEventType_t event, L7_char8 *strEvent)
{
    struct em_Registry *em_ptr;
  
    for (em_ptr = head; em_ptr != L7_NULL; em_ptr = em_ptr->next) {
        em_ptr->EMHandler(event, strEvent);
    }
    return;
}

/*********************************************************************
* @purpose  Registers an external manager for event notification
*
* @parms    extEMHandler - Pointer to external manager
*
* @returns  None
*
* @notes    
*
* @end
*********************************************************************/
void EM_Register (void (*extEMHandler)(sysEventType_t event, L7_char8 *eventMessage) )
{
    struct em_Registry *em_new;
    struct em_Registry *em_ptr;

    if (extEMHandler == L7_NULL) {
        return;
    }
  
    em_new = osapiMalloc(L7_SIM_COMPONENT_ID, sizeof(struct em_Registry));
    if (em_new == L7_NULL) {
        return;
    }

    em_new->EMHandler = extEMHandler;
  
    if (head == L7_NULL) {
        head = em_new;
        em_new->next = L7_NULL;
    }
    else {
        for (em_ptr = head; em_ptr != L7_NULL; em_ptr = em_ptr->next) {
            if (em_ptr->EMHandler == extEMHandler) {
                osapiFree(L7_SIM_COMPONENT_ID, em_new);
                return;
            }
        }
        em_new->next = head;
        head = em_new;
    }
    return;
}

/*********************************************************************
* @purpose  De-Registers an external manager for event notification
*
* @parms    extEMHandler - Pointer to external manager
*
* @returns  None
*
* @notes    
*
* @end
*********************************************************************/
void EM_DeRegister (void (*extEMHandler)(sysEventType_t event, char *eventMessage) )
{
    struct em_Registry *em_ptr;
    struct em_Registry *save_ptr;
  
    if (extEMHandler == L7_NULL) {
        return;
    }

    if (extEMHandler == head->EMHandler) {
        em_ptr = head;
        head = em_ptr->next;
        osapiFree(L7_SIM_COMPONENT_ID, em_ptr);

        return;
    }

    for (em_ptr = head; em_ptr->next != L7_NULL; em_ptr = em_ptr->next) {
        if (em_ptr->next->EMHandler == extEMHandler) {
            save_ptr =  em_ptr->next;
            em_ptr->next =  em_ptr->next->next;
            osapiFree(L7_SIM_COMPONENT_ID, save_ptr);
            return;
        }
    }
    return;
}
