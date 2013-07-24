/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename support.c
*
* @purpose System Support (Debug) Manager
*
* @component system
*
* @comments none
*
* @create 08/01/2006
*
* @author wjacobs
* @end
*
**********************************************************************/

#include <string.h>                /* for memcpy() etc... */
#include "l7_common.h"
#include "sysapi.h"
#include "osapi.h"
#include "log.h"
#include "support_api.h"
#include "l7_cnfgr_api.h"
#include "unitmgr_api.h"
#include "buff_api.h"
#include "osapi.h"

extern L7_RC_t sysapiSupportCfgFileSave(void);
extern void osapiDebugMsgQueuePrint(void);
extern void osapiDebugMemoryStats (void);
extern void sysapiMbufDump(L7_int32 show_bufs);

void sysapiDebugRegisteredSysInfoDump(void);

/* Each list has a separate linked list of routines.
   Routines may be duplicated in multiple categories.
   However, only one instance of a routine may exist in each list */
static supportDebugCategory_t *supportDebugCategoryListHead[SUPPORT_CATEGORIES_MAX];
static supportDebugDescr_t      supportDebugDescrList[L7_LAST_COMPONENT_ID*2];

L7_uint32 supportDebugCategoryBufferPoolId = 0;

typedef struct supportCategoryNameListEntry_s
{
  SUPPORT_CATEGORIES_t  category;
  L7_char8              name[SUPPORT_CATEGORY_NAME_MAX_LEN];
  L7_uchar8             categoryDescr[SUPPORT_DEBUG_HELP_DESCR_SIZE];
} supportCategoryNameListEntry_t;

static supportCategoryNameListEntry_t supportCategoryNameList[] =
{
  /* category                                   name                descr*/
  /*-----------------------------------------------------------------------------------------*/
  {SUPPORT_CATEGORY_UNUSED,                 "UNUSED",             "\0"},
  {SUPPORT_CATEGORY_SYSTEM,                 "system",         "Dump general system info"},
  /* Collections of debug routines for a package */
  {SUPPORT_CATEGORY_SWITCHING,              "switching",      "Dump general switching info"},
  {SUPPORT_CATEGORY_ROUTING,                "routing",        "Dump general routing info"},
  {SUPPORT_CATEGORY_IPMCAST,                "ipmcast",        "Dump general multicast routing info"},
  {SUPPORT_CATEGORY_BGP,                    "bgp",            "Dump general BGP info"},
  {SUPPORT_CATEGORY_QOS,                    "qos",            "Dump general QOS info"},
  {SUPPORT_CATEGORY_WIRELESS,               "wireless",       "Dump general wireless info"},
  /* Debug routines specific to a component
      - An area does not have to be defined for the component.
      - The accessor routines will dump based on the component ID */
  {SUPPORT_CATEGORY_COMPONENT_SPECIFIC,      "component",      "Dump list of components with dump info"},

  /* Collections of debug routines to analyze common scenarios */
  {SUPPORT_CATEGORY_PORT_CONNECTIVITY,      "port",           "Dump port connectivity info"},
  {SUPPORT_CATEGORY_STACKING,               "stacking",       "Dump stacking info"},

  /*===============================================================*/
  /* This must be the last entry of the lists  ** Sentinel **        */
  /*===============================================================*/
  {SUPPORT_CATEGORY_LAST}
};

static L7_RC_t supportDebugCategoryEntryAdd(supportDebugCategory_t *supportDebugCategory);
static supportDebugCategory_t* supportDebugCategoryEntryDelete(supportDebugCategory_t *supportDebugCategory);
static L7_BOOL supportDebugCategoryEntryExists(supportDebugCategory_t *supportDebugCategory);


#define SUPPORT_DEBUG_CATEGORY_BUFFERS  (2*L7_LAST_COMPONENT_ID)
/*

supportDebugCategoryList


    */

/*====================================================================================*/
/*

            LINKER ASSIST ROUTINES
*/
/*====================================================================================*/

/*********************************************************************
* @purpose  Linker assist function for sysapi_support_debug.c
*
* @param    NONE
*
* @returns  NONE
*
* @notes
*
* @end
*********************************************************************/
extern void sysapiSupportCfgFileDump (void);
void sysapiSupportDebugLinkerAssist (void)
{
  sysapiSupportCfgFileDump();
}



/*====================================================================================*/
/*

            ROUTINES RELATED TO THE HIDDEN CLI "SUPPORT" COMMAND TREE
*/
/*====================================================================================*/


/* Begin Function Declarations: support_api.h */
/*********************************************************************
*
* @purpose  Runtime registration routine for snapshots dynamically
*
* @param    category      @b((input)) one of SUPPORT_CATEGORIES_t
* @param    componentId   @b((input)) component ID
* @param    supportRoutine   @b((input)) address of routine to invoke
* @param    supportRoutineName   @b((input)) name of the routine
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  Allow for support engineer to add a snapshot routine
*
* @end
*
*********************************************************************/
L7_RC_t supportDebugCategoryRuntimeRegister(SUPPORT_CATEGORIES_t category,
                                            L7_COMPONENT_IDS_t componentId,
                                            void *supportRoutine,
                                            L7_uchar8 *supportRoutineName)
{
  supportDebugCategory_t supportDebugCategory;

  memset(&supportDebugCategory, 0x00, sizeof(supportDebugCategory));
  supportDebugCategory.category  = category;
  supportDebugCategory.componentId  = componentId;
  supportDebugCategory.info.supportRoutine  = supportRoutine;
  osapiStrncpySafe(supportDebugCategory.info.supportRoutineName, supportRoutineName,
          SUPPORT_DEBUG_HELP_NAME_SIZE);

  return (supportDebugCategoryRegister(supportDebugCategory));

}
/*********************************************************************
*
* @purpose  Register "support" hooks to gather initial debug information to address
*           specified classes of data
*
* @param    supportDebugCategory   @b((input)) a supportDebugCategory_t structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The same component ID can register multiple debug routines.
*           The same component ID and/or routine can be registered in multiple categories
*
* @end
*
*********************************************************************/
L7_RC_t supportDebugCategoryRegisterImpl(supportDebugCategory_t supportDebugCategory)
{
  L7_COMPONENT_IDS_t        componentId;
  SUPPORT_CATEGORIES_t      category;
  supportDebugCategory_t    *pEntry;
  void*                     supportRoutine;
  L7_RC_t rc;
  L7_char8 name[L7_COMPONENT_NAME_MAX_LEN];

  if (supportDebugCategoryBufferPoolId == 0)
  {
    /* Infrastructure is uninitialized */
    return L7_FAILURE;
  }

  category        = supportDebugCategory.category;
  componentId     = supportDebugCategory.componentId;
  supportRoutine  = supportDebugCategory.info.supportRoutine;

  if ((rc = cnfgrApiComponentNameGet(componentId, name)) != L7_SUCCESS)
  {
    osapiStrncpySafe(name, "Unknown", 8);
  }

  /* Validate the input */
  if ( (category == SUPPORT_CATEGORY_UNUSED) || (category >= SUPPORT_CATEGORY_LAST) )
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "Attempt to register invalid category %d\n",
            (L7_int32)category);
    return(L7_FAILURE);
  }

  if ( (componentId == L7_FIRST_COMPONENT_ID) || (componentId >= L7_LAST_COMPONENT_ID))
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "Attempt to register invalid component ID %d\n",
            (L7_int32)componentId);
    return(L7_FAILURE);
  }

  if (supportRoutine == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "supportRoutine is null for category %d and componentId %d, %s\n",
            (L7_int32) category,(L7_int32)componentId, name);
    return(L7_FAILURE);
  }

  /*Check for null strings */
  if(strlen(supportDebugCategory.info.supportRoutineName) == 0)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "Category %d  component %d, %s: Attempt to register null name\n",
            (L7_int32)category, (L7_int32)componentId, name);
    return(L7_FAILURE);
  }

  /* Copy into our data store.

    Do not depend upon the registering components to keep the data structure required
    for support routine. We do not want the complexities of a misbehaving component when
    we are trying to debug a problem. As such, use memory dedicated for this purpose.

   */

  if (bufferPoolAllocate(supportDebugCategoryBufferPoolId, (L7_uchar8 **)&pEntry) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "Unable to get buffer for category %d, component %d, %s, routine %s\n",
            supportDebugCategory.category, supportDebugCategory.componentId, name,
            supportDebugCategory.info.supportRoutineName);
    return L7_FAILURE;
  }


  memcpy((L7_uchar8 *)pEntry, &supportDebugCategory, sizeof(supportDebugCategory_t));

  /* Ensure that there is not garbage is the prev and next fields */
  pEntry->prev = L7_NULLPTR;
  pEntry->next = L7_NULLPTR;

  /* Check for pre-existing registration in that category
     Note that we are checking for a match on the address of the supportRoutine routine,
     not a match on component IDs.  This allows system routines with the same component ID
     to register different help routines */

  if (supportDebugCategoryEntryExists(pEntry) == L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "Debug Help routine %s (%x) already registered\n",
            supportDebugCategory.info.supportRoutineName,(L7_int32)supportRoutine);

    bufferPoolFree(supportDebugCategoryBufferPoolId, (L7_uchar8 *)pEntry);
    return(L7_FAILURE);
  }

  /* Routine was not previously registered. Add it */
  if (supportDebugCategoryEntryAdd(pEntry) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "Failed to register entry for category %d, component %d, %s, routine %s\n",
            supportDebugCategory.category, supportDebugCategory.componentId, name,
            supportDebugCategory.info.supportRoutineName);

    bufferPoolFree(supportDebugCategoryBufferPoolId, (L7_uchar8 *)pEntry);
    return L7_FAILURE;
  }

  return L7_SUCCESS;


}


/*********************************************************************
* @purpose  Deregister a "support hook"
*
*
* @param    supportDebugCategory   @b((input)) a supportDebugCategory_t structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE

*
* @notes
*
* @end
*********************************************************************/
L7_RC_t supportDebugCategoryDeregisterImpl(supportDebugCategory_t supportDebugCategory)
{

  L7_COMPONENT_IDS_t        componentId;
  SUPPORT_CATEGORIES_t      category;
  void*                     supportRoutine;
  supportDebugCategory_t    *pEntry;
  L7_RC_t rc;
  L7_char8 name[L7_COMPONENT_NAME_MAX_LEN];

  category        = supportDebugCategory.category;
  componentId     = supportDebugCategory.componentId;
  supportRoutine  = supportDebugCategory.info.supportRoutine;

  if ((rc = cnfgrApiComponentNameGet(componentId, name)) != L7_SUCCESS)
  {
    osapiStrncpySafe(name, "Unknown", 8);
  }

  /* Validate the input */
  if ( (category == SUPPORT_CATEGORY_UNUSED) || (category >= SUPPORT_CATEGORY_LAST) )
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "Attempt to deregister invalid category %d\n",
            (L7_int32)category);
    return(L7_FAILURE);
  }

  if ( (componentId == L7_FIRST_COMPONENT_ID) || (componentId >= L7_LAST_COMPONENT_ID))
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "Attempt to deregister invalid component ID %d\n",
            (L7_int32)componentId);
    return(L7_FAILURE);
  }

  if (supportRoutine == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "supportRoutine is null for category %d and componentId %d, %s\n",
            (L7_int32) category,(L7_int32)componentId, name);
    return(L7_FAILURE);
  }


  /* Check for existance of a registration in that category */
  pEntry = supportDebugCategoryEntryDelete(&supportDebugCategory);

  if (pEntry == L7_NULLPTR)
  {
    /* Registration not found */
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "Deregistration failed: Debug Help routine %s (%x) not registered\n",
            supportDebugCategory.info.supportRoutineName,(L7_int32)supportRoutine);
    return(L7_FAILURE);
  }

  /* Free the buffer associated with the entry */
  bufferPoolFree(supportDebugCategoryBufferPoolId, (L7_uchar8 *)pEntry);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Obtain descriptions for first support registration
*
* @param    category         @b{(input)} support routine category
* @param    componentId      @b{(input)} component ID
* @param    *supportDebugCategory    @b{output}  Pointer to an output structure
*
* @returns  L7_RC_t  L7_SUCCESS or L7_FALIURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t supportDebugCategoryGetFirst(L7_uint32 category, L7_uint32 componentId,
                                     supportDebugCategory_t *supportDebugCategory)
{
  L7_int32 i;

  for (i=0; i < SUPPORT_CATEGORY_LAST; i++)
  {
    if (supportDebugCategoryListHead[i] != L7_NULL)
    {
      /* First entry was found */
      memcpy(supportDebugCategory, supportDebugCategoryListHead[i],
             sizeof(supportDebugCategory_t));

      return(L7_SUCCESS);
    }  /*  j < L7_LAST_COMPONENT_ID */
  } /*  i < SUPPORT_CATEGORY_LAST*/

  return L7_FAILURE;
}



/*********************************************************************
* @purpose  Obtain description for the next "support" registration
*
* Index values of the current entry:
* @param    category         @b{(input)} support routine category
* @param    componentId      @b{(input)} component ID
* @param    supportRoutine        @b{(input)} address of next support routine
* @param    *supportDebugCategory    @b{output}  Pointer to an output structure
*
*
* @returns  L7_RC_t  L7_SUCCESS or L7_FALIURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t supportDebugCategoryGetNext(SUPPORT_CATEGORIES_t category,
                                    L7_COMPONENT_IDS_t  componentId,
                                    void* supportRoutine,
                                    supportDebugCategory_t *supportDebugCategory)
{

  L7_COMPONENT_IDS_t        prevComponentId;
  SUPPORT_CATEGORIES_t      prevCategory;
  void*                     prevSupportRoutine;
  L7_int32                  i;
  L7_BOOL                   prevFound;
  supportDebugCategory_t    *curr;
  supportDebugCategory_t    *pEntry;

  prevComponentId     = componentId;
  prevCategory        = category;
  prevSupportRoutine  = supportRoutine;
  prevFound           = L7_FALSE;
  pEntry              = L7_NULLPTR;

  if (supportDebugCategory == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "Null value passed for supportDebugCategory\n");
    return L7_FAILURE;
  }
  /* Is this a getFirst */
  if ( (prevCategory == SUPPORT_CATEGORY_UNUSED) &&
       (prevComponentId == L7_FIRST_COMPONENT_ID) &&
       (prevSupportRoutine == L7_NULL) )
  {
    return(supportDebugCategoryGetFirst(category,componentId,supportDebugCategory));
  }

  /*-----------------------------------------------------------------------------*/
  /* Find entry associated with the category and supportRoutine.

     Note that the categories are ordered according to the value of the enum
     SUPPORT_CATEGORIES_t.

     However, within a category, entries are NOT stored in sequential
     order according to the value of the componentId. Instead, entries
     are stored in the order in which they were registered.

   */
  /*-----------------------------------------------------------------------------*/

  curr = supportDebugCategoryListHead[prevCategory];

  while (curr != L7_NULLPTR)
  {
    if (curr->info.supportRoutine == prevSupportRoutine)
    {
      /* Entry found.  Point to next entry */
      prevFound = L7_TRUE;
      curr = curr->next;
      break;
    }
    curr = curr->next;
  }  /* while */

  if (prevFound != L7_TRUE)
  {
    /* Do not log a message because this is an expected condition */
    /* Return failure because a matching entry was not found */
    return L7_FAILURE;
  }

  /*----------------------------------*/
  /* The previous entry was found.   */
  /*----------------------------------*/
  if (curr != L7_NULLPTR)
  {
    /* There was another entry in the same category.  Return that entry */
    pEntry = curr;
  }
  else
  {
    /* Return the first entry in the next category */

    for (i=(prevCategory+1); i < SUPPORT_CATEGORY_LAST; i++)
    {
      if (supportDebugCategoryListHead[i] != L7_NULLPTR)
      {
        /* First entry in next category was found */
        pEntry = supportDebugCategoryListHead[i];
        break;
      }
    } /*  i < SUPPORT_CATEGORY_LAST*/

    if (i== SUPPORT_CATEGORY_LAST)
    {
      /* There was no next entry */
      return L7_FAILURE;
    }


  }

  if (pEntry != L7_NULLPTR)
  {
    memcpy(supportDebugCategory, pEntry,sizeof(supportDebugCategory_t));
  }

  return L7_SUCCESS;

}


/*********************************************************************
* @purpose  Determine if any registration exists for any snapshot.
*
* @param    void
*
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL supportDebugCategoryRegistrationExists(void)
{
  /* A get first will let us know if any component is registered */
  supportDebugCategory_t supportDebugCategory;

  if (supportDebugCategoryGetFirst(0,0,&supportDebugCategory)== L7_SUCCESS)
  {
    return L7_TRUE;
  }

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Determine if any debug routine is registered for
*           the category and/or component
*
* Index values of the current entry:
* @param    category         @b{(input/output)} support routine category
* @param    componentId      @b{(input/output)} component ID
*
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL supportDebugCategoryIsPresent(SUPPORT_CATEGORIES_t category,
                                      L7_COMPONENT_IDS_t componentId)
{

  supportDebugCategory_t *curr;

  /* Validate the input */
  if ( (category == SUPPORT_CATEGORY_UNUSED)||
       (category >= SUPPORT_CATEGORY_LAST))
  {
    return L7_FALSE;
  }

  /* Handle component specific registrations first */
  if (category == SUPPORT_CATEGORY_COMPONENT_SPECIFIC)
  {

    if ( (componentId == L7_FIRST_COMPONENT_ID)||
         (componentId >= L7_LAST_COMPONENT_ID) )
    {
      return L7_FALSE;
    }

    /* Return true if there are any debug routines registered for this component */
    /* Note that the components are not stored in order */
    curr = supportDebugCategoryListHead[category];
    while (curr != L7_NULLPTR)
    {
      if (curr->componentId == componentId)
      {
        /* An entry was found */
        return L7_TRUE;
      }
      curr = curr->next;
    }  /* while */

  }
  else
  {
    if (supportDebugCategoryListHead[category] != L7_NULLPTR)
    {
      /* At least one entry was found */
      return L7_TRUE;
    }
  }

  return L7_FALSE;

}

/*********************************************************************
* @purpose  Invoke the debug dump routines registered in this category
*
* Index values of the current entry:
* @param    category         @b{(input/output)} support routine category
* @param    componentId      @b{(input/output)} component ID
*
*
* @returns  L7_RC_t  L7_SUCCESS or L7_FALIURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t supportDebugCategoryDump(SUPPORT_CATEGORIES_t category)
{

  supportDebugCategory_t    *curr;

  /* Validate the input */
  if ( (category == SUPPORT_CATEGORY_UNUSED)||
       (category >= SUPPORT_CATEGORY_LAST))
  {
    return L7_FAILURE;
  }

  curr = supportDebugCategoryListHead[category];
  while (curr != L7_NULLPTR)
  {
    if (curr->info.supportRoutine != L7_NULLPTR)
    {
      (*(curr->info.supportRoutine))();
    }
    curr = curr->next;
  }  /* while */


  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Get the name for the support category
*
* @param    category  @b{(input)} support routine category
* @param    *name     @b{(output)} pointer to a string of length
*                                 SUPPORT_CATEGORY_NAME_MAX_LEN
*
* @returns  L7_RC_t  L7_SUCCESS or L7_FALIURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t supportDebugCategoryNameGet(SUPPORT_CATEGORIES_t category,
                                    L7_char8 *name)
{

  /* Validate the input */
  if ( (category == SUPPORT_CATEGORY_UNUSED)||
       (category >= SUPPORT_CATEGORY_LAST))
  {
    osapiStrncpySafe(name,"\0",SUPPORT_CATEGORY_NAME_MAX_LEN);
    return L7_FAILURE;
  }

  osapiStrncpySafe( (L7_char8*)name,supportCategoryNameList[category].name,
           SUPPORT_CATEGORY_NAME_MAX_LEN);
  return L7_SUCCESS;
}

/*====================================================================================*/
/*
            ROUTINES RELATED TO GENERAL DEBUG INFRASTRUCTURE AND
                                    DEVSHELL ROUTINES
*/
/*====================================================================================*/

/*********************************************************************
*
* @purpose  Register "support" hooks to facilitate customer support personnel.
*           The registration of the "Debug Help" routines allow for easy identification
*           of useful devShell routines to be invoked by customer support engineers.
*
* @param    supportDebugDescr   @b((input)) a supportDebugDescr_t structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The same component ID can register multiple debug help routines.
*           If a null help routine is passed, only the user control data is saved.
*
* @end
*
*********************************************************************/
L7_RC_t supportDebugRegisterImpl(supportDebugDescr_t supportDebugDescr)
{
  L7_COMPONENT_IDS_t        componentId;
  void*                     debugHelp;
  L7_uint32                 i;
  L7_RC_t rc;
  L7_char8 name[L7_COMPONENT_NAME_MAX_LEN];

  componentId     = supportDebugDescr.componentId;
  debugHelp       = supportDebugDescr.internal.debugHelp;

  if ((rc = cnfgrApiComponentNameGet(componentId, name)) != L7_SUCCESS)
  {
    osapiStrncpySafe(name, "Unknown", 8);
  }

  /* Validate the input */
  if ( (componentId == L7_FIRST_COMPONENT_ID) || (componentId >= L7_LAST_COMPONENT_ID))
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "Attempt to register invalid component ID %d\n",
            (L7_int32)componentId);
    return(L7_FAILURE);
  }

  if (debugHelp != L7_NULLPTR)
  {
    /* Check for null strings */
    if ( (strncmp(supportDebugDescr.internal.debugHelpRoutineName, "\0",SUPPORT_DEBUG_HELP_NAME_SIZE)== 0) ||
         (strncmp(supportDebugDescr.internal.debugHelpDescr, "\0",SUPPORT_DEBUG_HELP_DESCR_SIZE)== 0))
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
              "Component %d, %s: Attempt to register null string for name or help description\n",
              (L7_int32)componentId, name);
      return(L7_FAILURE);
    }

  }


  /* Check for pre-existing registration for that component
     Note that we are checking for a match on the address of the debugHelp routine,
     and on component IDs.  This allows system routines with the same component ID
     to register different help routines */
  for (i=0; i < L7_LAST_COMPONENT_ID; i++)
  {
    /* This test will catch even the null debugHelp (user control) registrations */
    if (supportDebugDescrList[i].componentId == componentId)
    {
      if (supportDebugDescrList[i].internal.debugHelp == debugHelp)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
                "Debug Help routine %s (%x) already registered\n",
                supportDebugDescr.internal.debugHelpRoutineName,(L7_int32)debugHelp);
        return(L7_FAILURE);
      }
    }
  }

  /* Routine was not previously registered. Copy info into first empty entry */
  for (i=0; i < L7_LAST_COMPONENT_ID; i++)
  {
    if (supportDebugDescrList[i].componentId == L7_NULL)
    {

      memcpy(&supportDebugDescrList[i], &supportDebugDescr, sizeof(supportDebugDescr_t));
      break;
    }
  }

  return(L7_SUCCESS);

}


/*********************************************************************
* @purpose  Deregister a "support hook"
*
* @purpose  Register "support" hooks to facilitate customer support personnel.
*           The registration of the "Debug Help" routines allow for easy identification
*           of useful devShell routines to be invoked by customer support engineers.
*
* @param    supportDebugDescr   @b((input)) a supportDebugDescr_t structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t supportDebugDeregisterImpl(supportDebugDescr_t supportDebugDescr)
{

  L7_COMPONENT_IDS_t        componentId;
  void*                     debugHelp;
  L7_uint32                 i;
  L7_BOOL                   matchFound;
  L7_RC_t rc;
  L7_char8 name[L7_COMPONENT_NAME_MAX_LEN];

  componentId    = supportDebugDescr.componentId;
  debugHelp      = supportDebugDescr.internal.debugHelp;
  matchFound     = L7_FALSE;

  if ((rc = cnfgrApiComponentNameGet(componentId, name)) != L7_SUCCESS)
  {
    osapiStrncpySafe(name, "Unknown", 8);
  }

  /* Validate the input */
  if ( (componentId == L7_FIRST_COMPONENT_ID) || (componentId >= L7_LAST_COMPONENT_ID))
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "Attempt to deregister invalid component ID %d\n",
            (L7_int32)componentId);
    return(L7_FAILURE);
  }



  /* Check for pre-existing registration for that component
     Note that we are checking for a match on the address of the debugHelp routine,
     and on component IDs.  This allows system routines with the same component ID
     to register different help routines, or none at all (if registering user control only) */
  for (i=0; i < L7_LAST_COMPONENT_ID; i++)
  {
    /* This test will catch even the null debugHelp (user control) registrations */
    if (supportDebugDescrList[i].componentId == componentId)
    {
      if (supportDebugDescrList[i].internal.debugHelp == debugHelp)
      {
        matchFound = L7_TRUE;
        break;
      }
    }
  }

  /* Check for existance of a registration in that category */
  if (matchFound == L7_TRUE)
  {
    memset(&supportDebugDescrList[i], 0, sizeof(supportDebugDescr));
    return(L7_SUCCESS);
  }

  /* Registration not found */
  L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
          "Deregistration failed: Component %d, %s,  Debug Help routine %s (%x) not registered\n",
          componentId, name, supportDebugDescr.internal.debugHelpRoutineName,(L7_int32)debugHelp);
  return(L7_FAILURE);

}


/*********************************************************************
* @purpose  Obtain descriptions for first support registration
*
* Index values of the current entry:
* @param    componentId      @b{(input/output)} component ID
* @param    debugHelp        @b{(input/output)} address of debug help routine
* @param    debugHelpRoutineName    @b{output}  Name of next debugHelp routine
* @param    debugHelpDescr          @b{output}  help description of next entry
*
*
* @returns  L7_RC_t  L7_SUCCESS or L7_FALIURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t supportDebugHelpGetFirst(L7_COMPONENT_IDS_t* componentId,
                                 void** debugHelp,
                                 L7_uchar8* debugHelpRoutineName,
                                 L7_uchar8* debugHelpDescr)
{
  L7_int32                  i;

  for (i=0; i < L7_LAST_COMPONENT_ID; i++)
  {
    if (supportDebugDescrList[i].internal.debugHelp != L7_NULL)
    {
      /* First entry was found */
      *componentId    = supportDebugDescrList[i].componentId;
      *debugHelp     = supportDebugDescrList[i].internal.debugHelp;
      osapiStrncpySafe(debugHelpRoutineName,supportDebugDescrList[i].internal.debugHelpRoutineName,
              SUPPORT_DEBUG_HELP_NAME_SIZE);
      osapiStrncpySafe(debugHelpDescr,supportDebugDescrList[i].internal.debugHelpDescr,
              SUPPORT_DEBUG_HELP_DESCR_SIZE);

      return(L7_SUCCESS);
    }  /*  debugHelp != L7_NULL */
  }  /*  i < L7_LAST_COMPONENT_ID */

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Obtain description for the next "support debug" registration
*
* Index values of the current entry:
* @param    componentId      @b{(input/output)} component ID
* @param    debugHelp        @b{(input/output)} address of debug help routine
* @param    debugHelpRoutineName    @b{output}  Name of next debugHelp routine
* @param    debugHelpDescr          @b{output}  help description of next entry
*
*
* @returns  L7_RC_t  L7_SUCCESS or L7_FALIURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t supportDebugHelpGetNext(L7_COMPONENT_IDS_t* componentId,
                                void** debugHelp,
                                L7_uchar8* debugHelpRoutineName,
                                L7_uchar8* debugHelpDescr)
{

  L7_COMPONENT_IDS_t        prevComponentId;
  void*                     prevDebugHelp;
  L7_int32                  i, prevEntryIndex;
  L7_BOOL                   prevFound;

  prevComponentId     = *componentId;
  prevDebugHelp       = *debugHelp;
  prevEntryIndex      = 0;
  prevFound           = L7_FALSE;

  /* Is this a getFirst */
  if ( (prevComponentId == L7_FIRST_COMPONENT_ID) &&
       (prevDebugHelp == L7_NULL) )
  {
    return(supportDebugHelpGetFirst(componentId,debugHelp,
                                    debugHelpRoutineName,debugHelpDescr));
  }

  /* Find entry associated with the debugHelpRoutineName and component ID.

     Because multiple entries may exist with the same component ID,
     entries are NOT stored in sequential order according to the value
     of the componentId. Instead, entries are stored in the order in which
     they were registered.

    */
  for (i =0; i < L7_LAST_COMPONENT_ID; i++)
  {
    if (supportDebugDescrList[i].componentId == prevComponentId)
    {
      if (supportDebugDescrList[i].internal.debugHelp == prevDebugHelp)
      {
        prevFound = L7_TRUE;
        break;
      }
    }
  }

  if (prevFound != L7_TRUE)
  {
    /* Do not log a message because this is an expected condition */
    /* Return failure because a matching entry was not found */
    return L7_FAILURE;
  }


  /*  Now, find the next entry in this area, or the next entry period */
  prevEntryIndex = i;
  for (i= (prevEntryIndex+1); i < L7_LAST_COMPONENT_ID; i++)
  {
    if (supportDebugDescrList[i].internal.debugHelp != L7_NULL)
    {
      /* First entry was found */
      *componentId    = supportDebugDescrList[i].componentId;
      *debugHelp      = supportDebugDescrList[i].internal.debugHelp;
      osapiStrncpySafe(debugHelpRoutineName,supportDebugDescrList[i].internal.debugHelpRoutineName,
              SUPPORT_DEBUG_HELP_NAME_SIZE);
      osapiStrncpySafe(debugHelpDescr,supportDebugDescrList[i].internal.debugHelpDescr,
              SUPPORT_DEBUG_HELP_DESCR_SIZE);

      return(L7_SUCCESS);
    }  /*  debugHelp != L7_NULL */
  }  /*  i < L7_LAST_COMPONENT_ID */


  /* If we reach here, no entry was found */

  return L7_FAILURE;

}


/*********************************************************************
* @purpose  Call all registered non-volitale debug save routines
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t supportDebugSave(void)
{
  L7_uint32 i;
  L7_RC_t rc;

  rc = L7_SUCCESS;

  for (i=0; i < L7_LAST_COMPONENT_ID; i++)
  {
    if (supportDebugDescrList[i].userControl.notifySave != L7_NULL)
    {
      if ((*supportDebugDescrList[i].userControl.notifySave)() != L7_SUCCESS)
      {
        L7_RC_t rc2;
        L7_char8 name[L7_COMPONENT_NAME_MAX_LEN];

        if ((rc2 = cnfgrApiComponentNameGet(supportDebugDescrList[i].componentId, name)) != L7_SUCCESS)
        {
          osapiStrncpySafe(name, "Unknown", 8);
        }

        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
                "support/debug Save call failed for component %d, %s\n",
                (L7_int32)supportDebugDescrList[i].componentId, name);
        LOG_EVENT(supportDebugDescrList[i].componentId);
      }
    }  /*  notifySave != L7_NULL */
  }  /*  j < L7_LAST_COMPONENT_ID */


  if (sysapiSupportCfgFileSave() != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "Failed to save support cfg file\n");
    return L7_FAILURE;
  }

  osapiFsSync (); /* Wait until all config files are saved to the file system */

  /* propagate configuration files to connected units incase of failover */
  unitMgrPropagateCfg(UNITMGR_CFG_TYPE_SUPPORT);


  return(rc);
}

/*********************************************************************
* @purpose  Check if all components support/debug user config data has changed
*
* @param    void
*
* @returns  L7_BOOL  L7_TRUE or L7_FALSE
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL supportDebugHasDataChanged(void)
{
  L7_uint32 i;

  for (i=0; i < L7_LAST_COMPONENT_ID; i++)
  {
    if (supportDebugDescrList[i].userControl.hasDataChanged != L7_NULL)
    {
      if ((*supportDebugDescrList[i].userControl.hasDataChanged)() == L7_TRUE)
      {
        return L7_TRUE;
      }

    }  /*  debugHelp != L7_NULL */
  }  /*  i < L7_LAST_COMPONENT_ID */

  return(L7_FALSE);
}


/*********************************************************************
* @purpose  Call all registered debug clear routines
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes
*
* @end
*********************************************************************/
void supportDebugClear(void)
{
  L7_uint32 i;

  for (i=0; i < L7_LAST_COMPONENT_ID; i++)
  {
    if (supportDebugDescrList[i].userControl.clearFunc != L7_NULL)
    {
      if ((*supportDebugDescrList[i].userControl.clearFunc)() != L7_SUCCESS)
      {
        L7_RC_t rc2;
        L7_char8 name[L7_COMPONENT_NAME_MAX_LEN];

        if ((rc2 = cnfgrApiComponentNameGet(supportDebugDescrList[i].componentId, name)) != L7_SUCCESS)
        {
          osapiStrncpySafe(name, "Unknown", 8);
        }

        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
                "Clear routine failed for component %d, %s\n",
                (L7_int32)supportDebugDescrList[i].componentId, name);
        LOG_EVENT(supportDebugDescrList[i].componentId);
      }
    }  /*  clearFunc != L7_NULL */
  }  /*  j < L7_LAST_COMPONENT_ID */

}
/*********************************************************************
* @purpose  Print all components where user config data has changed
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void supportDebugHasDataChangedDump(void)
{
  L7_uint32 i;
  L7_char8 name[L7_COMPONENT_NAME_MAX_LEN];
  L7_COMPONENT_IDS_t component_id;

  for (i=0; i < L7_LAST_COMPONENT_ID; i++)
  {
    if (supportDebugDescrList[i].userControl.hasDataChanged != L7_NULL)
    {
      if ((*supportDebugDescrList[i].userControl.hasDataChanged)() == L7_TRUE)
      {
        bzero(name, L7_COMPONENT_NAME_MAX_LEN);
        component_id = supportDebugDescrList[i].componentId;
        if (cnfgrApiComponentNameGet(component_id, name) == L7_SUCCESS)
        {
          sysapiPrintf("\n%25s (%d) has data changed flag set.",name,
                       (L7_int32)component_id);
        }
        else
          sysapiPrintf("\nComponent %d has data changed flag set.",
                       (L7_int32)component_id);
      }

    }  /*  hasDataChanged != L7_NULL */
  }  /*  i < L7_LAST_COMPONENT_ID */

}

/*********************************************************************
* @purpose  Initialize the list
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t supportDebugCategoryListInit(void)
{
  memset(supportDebugCategoryListHead,0x00,
         (sizeof(supportDebugCategory_t *)*SUPPORT_CATEGORIES_MAX));

  if (bufferPoolInit(SUPPORT_DEBUG_CATEGORY_BUFFERS, sizeof(supportDebugCategory_t),
                     "Sup Dbg Cat Bufs",&supportDebugCategoryBufferPoolId) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "Cannot allocate memory for support debug category buffers");
    return L7_FAILURE;
  }

  return L7_SUCCESS;

}


/*********************************************************************
* @purpose  Inserts entry in the list
*
*
* @param    *supportDebugCategory @b{{input}} pointer to a supportDebugCategory_t structure
*
* @returns  L7_SUCCESS on success
*           L7_FAILURE otherwise
*
* @notes    Inserts the element at the end of the list
*
* @end
*********************************************************************/
static L7_RC_t supportDebugCategoryEntryAdd(supportDebugCategory_t *supportDebugCategory)
{
  supportDebugCategory_t *curr;
  L7_int32  category;

  if (supportDebugCategory == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  category = supportDebugCategory->category;

  curr = supportDebugCategoryListHead[category];

  if (curr == L7_NULL)
  {
    /* First element to populate list, thus automatically becomes the head */
    supportDebugCategoryListHead[category]  = supportDebugCategory;
    supportDebugCategory->next              = L7_NULL;

  }
  else
  {
    /* Append to end of list */

    while (curr->next != L7_NULLPTR)
    {
      curr = curr->next;
    }

    curr->next                  = supportDebugCategory;
    supportDebugCategory->next  = L7_NULL;

  }

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Deletes an element from the list
*
* @param    *supportDebugCategory @b{{input}} pointer to a supportDebugCategory_t structure
*
* @returns  address of buffer for entry deleted
*
* @notes
*
* @end
*********************************************************************/
static supportDebugCategory_t* supportDebugCategoryEntryDelete(supportDebugCategory_t *supportDebugCategory)
{

  supportDebugCategory_t *curr;
  supportDebugCategory_t *prev;
  supportDebugCategory_t *pEntry;
  L7_int32  category;

  if (supportDebugCategory == L7_NULLPTR)
  {
    return L7_NULLPTR;
  }

  category = supportDebugCategory->category;

  curr = supportDebugCategoryListHead[category];
  prev = supportDebugCategoryListHead[category];
  pEntry = L7_NULLPTR;

  if (curr == L7_NULL)
  {
    /* List is empty */
    pEntry =  L7_NULLPTR;
  }
  else
  {
    /* Remove from list */

    while (curr != L7_NULLPTR)
    {
      if (curr->info.supportRoutine == supportDebugCategory->info.supportRoutine)
      {
        /* Match found */
        pEntry = curr;
        prev->next = curr->next;
        break;
      }
      prev = curr;
      curr = curr->next;
    }  /* curr != L7_NULLPTR */

  }  /* head != L7_NULLPTR */

  return pEntry;

}

/*********************************************************************
* @purpose  Determine if the entry is already present in the list
*
* @param    *supportDebugCategory @b{{input}} pointer to a supportDebugCategory_t structure
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    L7_LOGF on error.
*
* @end
*********************************************************************/
static L7_BOOL supportDebugCategoryEntryExists(supportDebugCategory_t *supportDebugCategory)
{

  supportDebugCategory_t *pEntry;


  pEntry = supportDebugCategoryListHead[supportDebugCategory->category];
  if (pEntry == L7_NULLPTR)
  {
    return L7_FALSE;
  }
  while (pEntry != L7_NULLPTR)
  {
    if (pEntry->info.supportRoutine == supportDebugCategory->info.supportRoutine)
    {
      return L7_TRUE;
    }

    pEntry = pEntry->next;
  }

  /* No support routine found */
  return L7_FALSE;

}


/* End Function Declarations */

/*====================================================================================*/
/*
            ROUTINES RELATED TO CAPTURING SPECIFIC AREAS OF DEBUGGING INFO
*/
/*====================================================================================*/

/*********************************************************************
* @purpose  Registered support debug dump routine for general system issues
*
* @param    void
*
* @returns  L7_RC_t  L7_SUCCESS or L7_FALIURE
*
* @notes
*
* @end
*********************************************************************/
void sysapiDebugSysInfoDumpRegister(void)
{
  supportDebugCategory_t supportDebugCategory;

  memset(&supportDebugCategory, 0x00, sizeof(supportDebugCategory));
  supportDebugCategory.componentId = L7_SIM_COMPONENT_ID;

  /*-----------------------------*/
  /* Register sysInfo debug dump */
  /*-----------------------------*/
  supportDebugCategory.category = SUPPORT_CATEGORY_SYSTEM;
  supportDebugCategory.info.supportRoutine = sysapiDebugRegisteredSysInfoDump;
  osapiStrncpySafe(supportDebugCategory.info.supportRoutineName, "sysapiDebugRegisteredSysInfoDump",
          SUPPORT_DEBUG_HELP_NAME_SIZE);
  (void) supportDebugCategoryRegister(supportDebugCategory);

}


/*********************************************************************
* @purpose  Registered support debug dump routine for general system issues
*
* @param    void
*
* @returns  L7_RC_t  L7_SUCCESS or L7_FALIURE
*
* @notes
*
* @end
*********************************************************************/
void sysapiDebugRegisteredSysInfoDump(void)
{
  sysapiPrintf("\r\n");
  sysapiPrintf("/*=====================================================================*/\n");
  sysapiPrintf("/*                  SYSTEM  INFORMATION                                     /\n");
  sysapiPrintf("/*=====================================================================*/\n");

  sysapiPrintf("\r\n");

  sysapiPrintf("/*------------------------------*/\n");
  sysapiPrintf("/* Output of sysapiMbufDump():  */\n");
  sysapiPrintf("/*------------------------------*/\n");
  sysapiPrintf("\r\n");

  sysapiMbufDump(0);

  sysapiPrintf("\r\n");
  sysapiPrintf("/*---------------------------------------*/\n");
  sysapiPrintf("/* Output of osapiDebugMsgQueuePrint():  */\n");
  sysapiPrintf("/*---------------------------------------*/\n");
  sysapiPrintf("\r\n");

  osapiDebugMsgQueuePrint();

#ifdef OSAPI_MEM_DEBUG
  sysapiPrintf("\r\n");
  sysapiPrintf("/*---------------------------------------*/\n");
  sysapiPrintf("/* Output of osapiDebugMallocSummary():  */\n");
  sysapiPrintf("/*---------------------------------------*/\n");
  sysapiPrintf("\r\n");
  osapiDebugMallocSummary(0);
#endif

#ifdef OSAPI_MEM_LIST
  sysapiPrintf("\r\n");
  sysapiPrintf("/*--------------------------------------*/\n");
  sysapiPrintf("/* Output of osapiDebugMemoryStats():   */\n");
  sysapiPrintf("/*--------------------------------------*/\n");
  sysapiPrintf("\r\n");

  osapiDebugMemoryStats();
#endif
  sysapiPrintf("\r\n");
}



