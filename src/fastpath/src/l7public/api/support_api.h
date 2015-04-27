/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename support_api.h
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

#ifndef __SUPPORT_API_H__
#define __SUPPORT_API_H__

#include "l7_common.h"
#include "compdefs.h"

/* Support Category naming table.
*/
#define  SUPPORT_CATEGORY_NAME_MAX_LEN      20      /* includes end-of-string char */
#define  SUPPORT_DEBUG_HELP_NAME_SIZE       32
#define  SUPPORT_DEBUG_HELP_DESCR_SIZE      48


typedef struct supportDebugCategory_s
{
  struct supportDebugCategory_s   *prev;
  struct supportDebugCategory_s   *next;

  SUPPORT_CATEGORIES_t        category;          /* Used to position debug hook in the UI */
  L7_COMPONENT_IDS_t          componentId;       /* Used to identify layer registering the hook */ 

  /* Registrations for controls to the user interface, e.g. CLI command tree */
  struct
  {
    void                        (*supportRoutine)(void); /* xxxDebug routine to invoke to
                                                          gather info for this category */

    L7_uchar8                   supportRoutineName[SUPPORT_DEBUG_HELP_NAME_SIZE]; /* name */
    /* String which will appear on the debug dump help */
  } info; 

} supportDebugCategory_t;

typedef struct supportDebugDescr_s
{

  L7_COMPONENT_IDS_t          componentId;       /* Used to identify feature in the UI */ 

  /* Registrations for controls to the user interface, e.g. CLI command tree */
  struct
  {
    L7_RC_t (*notifySave)(void);        /* save routine, e.g. save trace configuration*/
    L7_BOOL (*hasDataChanged)(void);    /* has data changed routine */

    L7_RC_t (*clearFunc)(void);         /* clear routine to disable traces and other 
                                           support functions*/
  } userControl;

  struct
  {
    void                        (*debugHelp)(void); /* xxxDebugHelp routine 
                                                           - e.g. helps for debug dumps,
                                                                  config dumps, 
                                                                  trace control funcs, etc. */

    L7_uchar8                   debugHelpRoutineName[SUPPORT_DEBUG_HELP_NAME_SIZE]; /* name */
    L7_uchar8                   debugHelpDescr[SUPPORT_DEBUG_HELP_DESCR_SIZE]; 
    /* String which will appear on the debug dump help */
  } internal;


} supportDebugDescr_t;



/* Begin Function Prototypes */

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
                                            L7_uchar8 *supportRoutineName);

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
#ifdef L7_PRODUCT_SMARTPATH
#define supportDebugCategoryRegister(x) L7_SUCCESS
#else
#define supportDebugCategoryRegister(x) supportDebugCategoryRegisterImpl(x)
#endif
L7_RC_t supportDebugCategoryRegisterImpl(supportDebugCategory_t supportDebugCategory);  


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
#ifdef L7_PRODUCT_SMARTPATH
#define supportDebugCategoryDeregister(x) L7_SUCCESS
#else
#define supportDebugCategoryDeregister(x) supportDebugCategoryDeregisterImpl(x)
#endif
L7_RC_t supportDebugCategoryDeregisterImpl(supportDebugCategory_t supportDebugCategory);  

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
                                    supportDebugCategory_t *supportDebugCategory);


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
L7_BOOL supportDebugCategoryRegistrationExists(void);

/*********************************************************************
* @purpose  Determine if any debug routine is registered for 
*           the category and/or component
*
* Index values of the current entry:
* @param    category         @b; support routine category
* @param    componentId      @b; component ID
*
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes 
*                                 
* @end
*********************************************************************/
L7_BOOL supportDebugCategoryIsPresent(SUPPORT_CATEGORIES_t category,
                                      L7_COMPONENT_IDS_t componentId);


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
L7_RC_t supportDebugCategoryDump(SUPPORT_CATEGORIES_t category);


/*********************************************************************
* @purpose  Get the name for the support category
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
L7_RC_t supportDebugCategoryNameGet(SUPPORT_CATEGORIES_t category,
                                    L7_char8 *name);


/*********************************************************************
* @purpose  Get the name for the support category
*
* @param    category      @b{(input)} support routine category
* @param    componentId   @b{(input/output)} support routine category
* @param    *name         @b{(output)} pointer to a string of length
*                          SUPPORT_CATEGORY_NAME_MAX_LEN
*
*
* @returns  L7_RC_t  L7_SUCCESS or L7_FALIURE
*
* @notes 
*                                 
* @end
*********************************************************************/
L7_RC_t supportDebugCategoryListInfoGet(SUPPORT_CATEGORIES_t category,
                                        L7_COMPONENT_IDS_t *componentId,
                                        void *supportRoutine,
                                        L7_char8 *supportRoutineName,
                                        L7_char8 *supportRoutineDescr);


/*********************************************************************
*
* @purpose  Register help hooks 
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
#ifdef L7_PRODUCT_SMARTPATH
#define supportDebugRegister(x) L7_SUCCESS
#else
#define supportDebugRegister(x) supportDebugRegisterImpl(x)
#endif
L7_RC_t supportDebugRegisterImpl(supportDebugDescr_t supportDebugDescr);  


/*********************************************************************
* @purpose  Deregister help hooks
*
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
#ifdef L7_PRODUCT_SMARTPATH
#define supportDebugDeregister(x) L7_SUCCESS
#else
#define supportDebugDeregister(x) supportDebugDeregisterImpl(x)
#endif
L7_RC_t supportDebugDeregisterImpl(supportDebugDescr_t supportDebugDescr);  


/*********************************************************************
* @purpose  Obtain description for the next "support debug" registration
*
* Index values of the current entry:
* @param    componentId      @b{(input/output)} component ID
* @param    help        @b{(input/output)} address of debug help routine
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
                                void** help, 
                                L7_uchar8* helpRoutineName,
                                L7_uchar8* helpDescr);

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
L7_RC_t supportDebugSave(void);

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
L7_BOOL supportDebugHasDataChanged(void);


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
void supportDebugClear(void);


/* End Function Prototypes */



/*********************************************************************
*
* @purpose Combine all `*.cfg` files into one Big Config file and
*          save it as SYSAPI_CONFIG_FILENAME on the ramdisk
*
* @param   Void
*
* @returns L7_SUCCESS
* @returns L7_ERROR
*
* @end
*
*********************************************************************/
L7_RC_t sysapiSupportCfgFileSave(void);


/*********************************************************************
*
* @purpose Re-read configuration from NVRAM into run-time storage.
*
* @returns none
*
* @end
*
*********************************************************************/
void sysapiSupportCfgFileReload(void);


/*********************************************************************
* @purpose  Read configuration data from the data location pointer to
*           by the config file management system
*
* @param    component_id   component ID of the file owner.
* @param    fileName       name of file to load
* @param    buffer         pointer to location file will be placed
* @param    bufferSize     size of buffer
* @param    checkSum       pointer to location of checksum in buffer
* @param    version        expected software version of file (current version)
* @param    defaultBuild   function ptr to factory default build routine
*                          if L7_NULL do not build default file
* @param    migrateBuild   function ptr to config migration routine
*                          if config is outdated
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Routine assumes checkSum is last 4 bytes of buffer.
*           Routine assumes first structure in the buffer is L7_fileHdr_t
*
* @end
*********************************************************************/
L7_RC_t sysapiSupportCfgFileGet( L7_COMPONENT_IDS_t component_id, L7_char8 *fileName,
                                L7_char8 *buffer, L7_uint32 bufferSize,
                                L7_uint32 *checkSum, L7_uint32 version,
                                void (*defaultBuild)(L7_uint32),
                                void (*migrateBuild)(L7_uint32, L7_uint32, L7_char8 *));


/*********************************************************************
* @purpose  Write data to the file. Create/Open file if does not exist.
*
* @param    component_id  Component ID of the file owner.
* @param    filename      file to write data to
* @param    buffer        actual data to write to file
* @param    nbytes        number of bytes to write to file
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sysapiSupportCfgFileWrite( L7_COMPONENT_IDS_t component_id, L7_char8 *filename,
                                  L7_char8 *buffer, L7_uint32 nbytes);

#endif /* SUPPORT_API_H */
