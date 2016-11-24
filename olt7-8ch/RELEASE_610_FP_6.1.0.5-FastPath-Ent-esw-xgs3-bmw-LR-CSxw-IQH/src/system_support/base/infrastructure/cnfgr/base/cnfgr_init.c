/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* Name: cnfgr_init.c
*
* Purpose: Configurator component initialization and cleanup functions.
*
* Component: Configurator (cnfgr)
*
* Commnets:
*
* Created by: avasquez 03/17/2003 
*
*********************************************************************/
/* Enable configurator Global Data */
#define CNFGR_INIT_GLOBALS
#include "cnfgr_include.h"


#if (CNFGR_MODULE_INIT == CNFGR_PRESENT)


/*********************************************************************
* @purpose  Initialize the configurator
*
* @param    pInitData     @b{(input)} Pointer to the configuratorn
*                                     initialization Data.
*
* @returns  L7_SUCCESS    The configurator initialized and is ready to 
*                         accept commands.
* @returns  L7_ERROR      The configurator failed to initialized. 
*
* @notes    NOTE: This function only initializes the configurator. The
*           Components are not initialized at the completion of this  
*           function.
*   
* @end
*********************************************************************/
L7_RC_t cnfgrApiInit (CNFGR_IN L7_CNFGR_INIT_DATA_t *pInitData)
{
    L7_RC_t     cnfgrRC;


    /* If this configurator context has been initialized, then
     * a reconfiguration is required. Thus, Configurator MUST
     * be terminated.
     *
     * NOTE: Design supports 1 instance of configurator.
     */

    if ( cnfgrReadyAndInitialized == L7_TRUE ) 
    {

        (void)cnfgrApiFini();
        /* recall the cnfgrApi Init 
         * 
         * NOTE: care should be taken in calling this function more
         *       than one.
        */
        cnfgrRC = cnfgrApiInit (pInitData);

    }
    else 
    {

        /* At this point all configurator modules are not been
         * initialized nor are ready. Start the initialization
         * of all configurator modules
         *
         * NOTE: There are no interdependency between the modules
         *       during initialization. Thus sequencing is not
         *       necessary.
         */
    
        /* Create and initialize global variables for this context */
        cnfgrReadyAndInitialized = L7_FALSE;
    
        /* Initialize configurator Modules */
        if ( cnfgrCrInitialize()    == L7_SUCCESS &&
             cnfgrCCtlrInitialize() == L7_SUCCESS &&
             cnfgrMsgInitialize()   == L7_SUCCESS &&
             cnfgrTallyInitialize() == L7_SUCCESS &&
             cnfgrDebugInitialize() == L7_SUCCESS )
        {
            /* At this point all configurator modules have been
             * initialized, as well as resources has been adquired.
             * The configurator is ready to receive command/events
             * pair.
             *
             * set the configurator initialized and ready variables 
            */
            cnfgrReadyAndInitialized = L7_TRUE;
            cnfgrRC = L7_SUCCESS;
    
        }
        else
        {
    
            /* There has been an error, ensure that all resources are returned
             * back to system before returning to caller.
             *
             * NOTE: The "fini" functions are "self-checked", i.e. if the module
             *       has been already "destroyed", the function has completed its job.
             *
             * NOTE: Order is not important. Thus "fini" functions CAN be called in
             *       any order.
             */
            cnfgrReadyAndInitialized = L7_FALSE;

            cnfgrTallyFini();
            cnfgrCCtlrFini();
            cnfgrCrFini();
            cnfgrMsgFini();
            cnfgrDebugFini();
    
            /* reset static variables for this context: None.   */
    
            /* release global variables for this context: None. */

            /* set return value */
            cnfgrRC = L7_ERROR;
    
        } /* endif initialize Modules */

    } /* endif check for initialize */

    /* return to caller */
    return (cnfgrRC);

}


/*********************************************************************
* @purpose  Terminate the configurator and all components controlled by it.
*
* @param    NONE          
*
* @returns  L7_SUCCESS    Configurator has susscefully terminated all software
*                         components, including itself.
* @returns  L7_ERROR      Configurator could not terminate one or more      
*                         components or itself. Fatal System Error.
*
* @notes    If return value is L7_ERROR the caller MUST assume that there
*           are resources allocated and components running. The configurator
*           is in a state that the only function call can receive is
*           cnfgrApiFini. No component in the system SHOULD be trusted.
*
* @end
*********************************************************************/
L7_RC_t cnfgrApiFini(void) 
{
    /* terminate all components TBD */

    /* terminate the configurator components */
    if ( cnfgrReadyAndInitialized == L7_TRUE ) {

        /* issue terminate to all components.
         * Issue a terminate to all components that are
         * not in IDLE state.
         *
         * TBD.
         */

        /* now... destroy the configurator */
        cnfgrReadyAndInitialized = L7_FALSE;
        cnfgrTallyFini();
        cnfgrMsgFini();
        cnfgrCrFini();
        cnfgrCCtlrFini();
        cnfgrDebugFini();

    } /* endif terminate configurator */

    /* return value to caller */
    return (L7_SUCCESS);
}


/*
 *********************************************************************
 *                      Configurator Internal Functions
 *********************************************************************
*/


/*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

#endif /* end cnfgr_init module */

