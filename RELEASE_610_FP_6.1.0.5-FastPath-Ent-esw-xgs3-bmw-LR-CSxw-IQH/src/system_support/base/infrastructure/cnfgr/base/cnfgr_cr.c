/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* Name: cnfgr_cr.c
*
* Purpose: Configurator component repository functions.
*
* Component: Configurator (cnfgr)
*
* Comments:  The following is the function catalog for CR:
*
*            1. Configurator Interface (prototype in cnfgr_api.h)
*
*               - cnfgrApiQuery
*
*            2. CR Internal Use Only (prototype in this file)
*
*               - cnfgrCrComponentFind 
*
*            3. CR Interface (prototype in cnfgr_cr.h)
*
*               - cnfgrCrComponentFirstTake 
*               - cnfgrCrComponentGive
*               - cnfgrCrComponentNextTake 
*               - cnfgrCrComponentTake
*               - cnfgrCrFini
*               - cnfgrCrInitialize 
*
* Created by: avasquez 03/17/2003 
*
*********************************************************************/
#include "cnfgr_include.h"
#include "sysapi_hpc.h"
#include "unitmgr_api.h"

#if (CNFGR_MODULE_CR == CNFGR_PRESENT)


/*
 *********************************************************************
 *             Static (local) Variables
 *********************************************************************
*/

/* Component Table -
 *
*/
static struct
{
    L7_uint32  maxCount,
               currentCount;
    L7_dll_t  *pDll;

} componentRepository = { 0,0,0};


static void *crLock = L7_NULLPTR;

#define CNFGR_CR_COMPONENT_TABLE_END_f  cnfgrSidCrComponentTableEndGet()


/*
 *********************************************************************
 *             Configurator Interface Function Calls
 *********************************************************************
*/ 

 /*
    Configurator Access functions
 */
/*********************************************************************
* @purpose  Get component name for the specified component ID.
*
* @param    cid   - Component ID.
* @param    comp_name   - Component name.
*
* @returns  L7_SUCCESS   - function completed succesfully. 
* @returns  L7_FAILURE   - Component not found.
*
* @notes    This function excecute synchronously.                     
*       
* @end
*********************************************************************/
L7_RC_t cnfgrApiComponentNameGet( L7_COMPONENT_IDS_t cid, L7_char8 *comp_name)
{
  L7_char8 *name;

  name = cnfgrSidComponentNameGet (cid);
  if (name == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  strcpy (comp_name, name);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get component mnemonic for the specified component ID.
*
* @param    cid   - Component ID.
* @param    comp_mnemonic   - Component mnemonic.
*
* @returns  L7_SUCCESS   - function completed succesfully. 
* @returns  L7_FAILURE   - Component not found.
*
* @notes    This function excecute synchronously.                     
*       
* @end
*********************************************************************/
L7_RC_t cnfgrApiComponentMnemonicGet( L7_COMPONENT_IDS_t cid, L7_char8 *comp_mnemonic)
{
  L7_char8 *mnemonic;

  mnemonic = cnfgrSidComponentMnemonicGet (cid);
  if (mnemonic == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  strcpy (comp_mnemonic, mnemonic);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function provides information about a component.       
*           The use of this function is optional.
*
* @param    pQueryData   - @b{(inputoutput)}pointer to data where the 
*                                           component will place
*                                           the requested information
*                                           for a component.
*
* @returns  L7_SUCCESS   - function completed succesfully. pQueryData
*                          contains valid information
* @returns  L7_ERROR     - function failed. pQueryData contain reason
*                        - for failure. 
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_CB_ERR_RC_INTERNAL      : could be fatal
*           L7_CNFGR_CB_ERR_RC_INVALID_ID    : component ID out of range
*           L7_CNFGR_CB_ERR_RC_INVALID_RQST
*           L7_CNFGR_CB_ERR_RC_INVALID_DATA  : invalid parameter
*           L7_CNFGR_CB_ERR_RC_NOT_FOUND     : component not found
*
* @notes    This function excecute synchronously.                     
*       
* @end
*********************************************************************/
L7_RC_t cnfgrApiQuery( CNFGR_INOUT L7_CNFGR_QUERY_DATA_t *pQueryData )
{  
    /* set up variables and structures */
    CNFGR_CR_HANDLE_t     crHandle;
    L7_COMPONENT_IDS_t    cid;
                    
    CNFGR_CR_COMPONENT_t  component,
                         *pComponent = &component;
    L7_RC_t               cnfgrRC    = L7_ERROR;
                                        
    L7_uint32 index=0; /* index into component array */

    /* validate input data --
     *
     * 1. data pointer 
    */
    if (pQueryData != L7_NULLPTR ) {

      /* 2. argument type */
      if (pQueryData->type == L7_CNFGR_QRY_TYPE_SINGLE) { /* single cid */

        /* 3. component identifier */
        cid = pQueryData->args.cid;
        if ( cid > L7_FIRST_COMPONENT_ID && cid < L7_LAST_COMPONENT_ID ) {

            /* get component and process request */
            cnfgrRC = cnfgrCrComponentTake( cid, &crHandle, &pComponent );
            if ( cnfgrRC == L7_SUCCESS ) {

                pQueryData->reason = L7_CNFGR_ERR_RC_FIRST;
                switch ( pQueryData->request ) {
                case L7_CNFGR_QRY_RQST_STATE:
                    if (pComponent != L7_NULLPTR) {

                        pQueryData->data.state.current = pComponent->currentState;
                        pQueryData->data.state.next    = pComponent->nextState;
                        pQueryData->data.state.saved   = pComponent->saveState;
                        cnfgrRC = L7_SUCCESS;

                    } else {

                        pQueryData->reason = L7_CNFGR_ERR_RC_NOT_FOUND;
                        cnfgrRC = L7_ERROR;

                    } /* endif component found */
                    break;

                case L7_CNFGR_QRY_RQST_PRESENT:
                    if ( pComponent != L7_NULLPTR )
                        pQueryData->data.present = pComponent->present;
                    else
                        pQueryData->data.present = L7_FALSE;

                    cnfgrRC = L7_SUCCESS;
                    break;

                case L7_CNFGR_QRY_RQST_MODE:
                    if ( pComponent != L7_NULLPTR ) {

                        pQueryData->data.mode = pComponent->mode;
                        cnfgrRC = L7_SUCCESS;

                    } else {

                        pQueryData->reason = L7_CNFGR_ERR_RC_NOT_FOUND;
                        cnfgrRC = L7_ERROR;

                    } /* endif component found */
                    break;

                case L7_CNFGR_QRY_RQST_STATUS:
                    if ( pComponent != L7_NULLPTR ) {

                        pQueryData->data.status = pComponent->status;
                        cnfgrRC = L7_SUCCESS;

                    } else {

                        pQueryData->reason = L7_CNFGR_ERR_RC_NOT_FOUND;
                        cnfgrRC = L7_ERROR;

                    } /* endif component found */
                    break;

                case L7_CNFGR_QRY_RQST_CMDRQST:
                    if ( pComponent != L7_NULLPTR ) {
                        pQueryData->data.cmdRqst.cmd     = pComponent->cmd;
                        pQueryData->data.cmdRqst.rqst    = pComponent->rqst;
                        pQueryData->data.cmdRqst.aRsp.rc = pComponent->aRsp.rc;
                        if ( pComponent->aRsp.rc == L7_SUCCESS )
                            pQueryData->data.cmdRqst.aRsp.u.response = pComponent->aRsp.u.response;
                        else
                            pQueryData->data.cmdRqst.aRsp.u.reason = pComponent->aRsp.u.reason;

                        cnfgrRC = L7_SUCCESS;

                    } else {

                        pQueryData->reason = L7_CNFGR_ERR_RC_NOT_FOUND;
                        cnfgrRC = L7_ERROR;

                    } /* endif component found */
                    break;

                case L7_CNFGR_QRY_RQST_MNEMONIC:
                    if ( pComponent != L7_NULLPTR && pComponent->mnemonic != L7_NULLPTR) {

                        strcpy(pQueryData->data.name, pComponent->mnemonic);
                        cnfgrRC = L7_SUCCESS;

                    } else {

                        pQueryData->reason = L7_CNFGR_ERR_RC_NOT_FOUND;
                        cnfgrRC = L7_ERROR;

                    } /* endif component found */
                    break;

                case L7_CNFGR_QRY_RQST_NAME:
                    if ( pComponent != L7_NULLPTR && pComponent->name != L7_NULLPTR) {

                        strcpy(pQueryData->data.name, pComponent->name);
                        cnfgrRC = L7_SUCCESS;

                    } else {

                        pQueryData->reason = L7_CNFGR_ERR_RC_NOT_FOUND;
                        cnfgrRC = L7_ERROR;

                    } /* endif component found */
                    break;

                case L7_CNFGR_QRY_RQST_ENABLE_ROUTINE:
                    if ( pComponent != L7_NULLPTR ) {

                        pQueryData->data.enable_routine = pComponent->enable_routine;
                        cnfgrRC = L7_SUCCESS;

                    } else {

                        pQueryData->reason = L7_CNFGR_ERR_RC_NOT_FOUND;
                        cnfgrRC = L7_ERROR;

                    } /* endif component found */
                    break;

                default:
                    /* 4. invalid request */
                    pQueryData->reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
                    cnfgrRC = L7_ERROR;
                    break;

                } /* endswitch request */

                if ( pComponent != L7_NULLPTR && crHandle != L7_NULLPTR )
                    (void)cnfgrCrComponentGive( crHandle );

            } else {

                pQueryData->reason = L7_CNFGR_ERR_RC_INTERNAL;

            } /* endif get component */ 
        } else {

            pQueryData->reason = L7_CNFGR_ERR_RC_INVALID_ID;

        } /* endif valid component ID */
      } else if (pQueryData->type == L7_CNFGR_QRY_TYPE_LIST) { /* multiple cids */

        pQueryData->reason = L7_CNFGR_ERR_RC_FIRST;

        switch ( pQueryData->request ) {
        case L7_CNFGR_QRY_RQST_ALL_PRESENT:

          for (cid = L7_FIRST_COMPONENT_ID+1; cid < L7_LAST_COMPONENT_ID; cid++) {

            /* get component and process request */
            cnfgrRC = cnfgrCrComponentTake( cid, &crHandle, &pComponent );

            if ( cnfgrRC == L7_SUCCESS ) {

              if ( pComponent != L7_NULLPTR && crHandle != L7_NULLPTR ) {

                if (pComponent->present == L7_TRUE) {

                  pQueryData->args.cid_list[index] = cid;
                  index++; /* incriment component index */

                } /* endif component present */

                (void)cnfgrCrComponentGive( crHandle );

              } /* endif component found */

            } /* endif get component */

          }

          pQueryData->args.cid_list[index] = 0; /* indicate end of dense list with 0 */
          cnfgrRC = L7_SUCCESS;
          break;

        default:
          /* 4. invalid request */
          pQueryData->reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
          cnfgrRC = L7_ERROR;
          break;
        } /* endswitch request */

      } else {

        pQueryData->reason = L7_CNFGR_ERR_RC_INVALID_DATA;

      }

    } else {

        /* cannot set pQueryData->reason = L7_CNFGR_ERR_RC_INVALID_DATA since
         * ptr is null
         */

    } /* endif valid data pointer */

    /* Return to caller */
    return (cnfgrRC);
}


/*
 *********************************************************************
 *                      Component Repository intenal functions 
 *********************************************************************
*/

/*
    Internal function prototypes
*/

L7_RC_t cnfgrCrComponentFind( CNFGR_IN  L7_COMPONENT_IDS_t cid, 
                              CNFGR_OUT L7_BOOL *pFound,
                              CNFGR_OUT CNFGR_CR_HANDLE_t *pCrHandle, 
                              CNFGR_OUT CNFGR_CR_COMPONENT_t **ppComponent);

/*
  Internal Funcions 
*/


/*********************************************************************
* @purpose  This function searchs for component (cid) in CR.            
*           This is a CR internal function.
*
* @param    cid          - @b{(input)}  Search key - component ID. 
*                                       
* @param    pFound       - @b{(output)} indicates if component is in CR.
*                                       
* @param    pCrHandle    - @b{(output)} location of unique identifier.
*                                       NULL if component not found.
*                                      
* @param    ppComponent  - @b{(output)} location of for the ComponentData
*                                       NULL if component not found.
*
* @returns  L7_SUCCESS   - function completed succesfully. Caller can update
*                          component data.
* @returns  L7_ERROR     - function failed. This COULD be a fatal error.
*
* @notes    None.                                                                         
*       
* @end
*********************************************************************/
L7_RC_t cnfgrCrComponentFind( CNFGR_IN  L7_COMPONENT_IDS_t cid, 
                              CNFGR_OUT L7_BOOL *pFound,
                              CNFGR_OUT CNFGR_CR_HANDLE_t *pCrHandle, 
                              CNFGR_OUT CNFGR_CR_COMPONENT_t **ppComponent)
{
    /* set up variables and structures */
    L7_RC_t               crRC,
                          crRC1;
    CNFGR_CR_COMPONENT_t *pComponent;
    L7_dll_member_t      *pDLLMember;
    L7_COMPONENT_IDS_t    searchCid;
    L7_BOOL               exit = L7_FALSE;

    *ppComponent = *pCrHandle = L7_NULLPTR;      /* output */
    *pFound      = L7_FALSE;                     /* output */

    /* Lock the CR */
    crRC1 = osapiSemaTake( crLock, L7_WAIT_FOREVER );
    if (crRC1 == L7_SUCCESS)
    {
        /* get first component in CR */
        crRC  = DLLFirstGet( componentRepository.pDll, &pDLLMember );
        if (crRC == L7_SUCCESS)
        {   
            do {
                pComponent = (CNFGR_CR_COMPONENT_t *)pDLLMember->data;
                searchCid  = pComponent->cid;

                if ( searchCid == cid )
                {
                    /* Found it! return the component to caller */
                    *ppComponent = pComponent;                        /* output */
                    *pCrHandle   = (CNFGR_CR_HANDLE_t)pDLLMember;     /* output */
                    *pFound      = L7_TRUE;                           /* output */

                    /* set return value and exit */
                    crRC         = L7_SUCCESS;
                    exit         = L7_TRUE;
                }
                else
                {
                    if ( searchCid == L7_LAST_COMPONENT_ID )
                    {
                        /* Not Found! return L7_SUCCESS (crRC) and NULL component to caller */
                        crRC         = L7_SUCCESS;
                        exit         = L7_TRUE;
                    }
                    else
                    {
                        crRC  = DLLNextGet( &pDLLMember );
                        if ( crRC != L7_SUCCESS )
                        {
                            /* log message and return to caller */
                            LOG_MSG("cnfgrCrComponentFind: Could not get next component from CR... exiting \n");

                            /* set error return value and exit */
                            crRC = L7_ERROR;
                            exit = L7_TRUE;

                        } /* endif next component */
                    } /* endif last component */
                } /* endif found */

            } while ( exit == L7_FALSE );

        }
        else
        {
            /* log message and return to caller */
            LOG_MSG("cnfgrCrComponentFind: Could not get first component from CR... exiting \n");

            /* set error return value */
            crRC = L7_ERROR;

        } /* endif find first component */

        /* unlock the CR */
        crRC1 = osapiSemaGive( crLock );
        if (crRC1 != L7_SUCCESS)
        {
            /* log message and return to caller */
            LOG_MSG("cnfgrCrComponentFind: failed to unlock CR... exiting \n");

            /* set error return value */
            crRC = L7_ERROR;

        } /* endif unlock CR */
    }
    else
    {
        /* log message and return to caller */
        LOG_MSG("cnfgrCrComponentFind: failed to lock CR... exiting \n");

        /* set error return value */
        crRC = L7_ERROR;

    } /* endif lock the CR */


   /* Return value to caller */
   return (crRC);
}


/*
 *********************************************************************
 *                      CR  Interface functions 
 *********************************************************************
*/

/* Component Accessors */

/*********************************************************************
* @purpose  This function get the first component in CR.                
*           This is a CR interface function.
*
* @param    pCrHandle    - @b{(output)} location of unique identifier.
*                                       NULL if cid not found.     
*                                      
* @param    ppComponent  - @b{(output)} location of for the ComponentData
*                                       NULL if cid not found. 
*
* @returns  L7_SUCCESS   - function completed succesfully. Caller can update
*                          component data.
* @returns  L7_ERROR     - function failed. This COULD be a fatal error.
*
* @notes    Queries can be done to the while the component is locked.                     
*       
* @end
*********************************************************************/
L7_RC_t cnfgrCrComponentFirstTake( CNFGR_OUT CNFGR_CR_HANDLE_t *pCrHandle,
                                   CNFGR_OUT CNFGR_CR_COMPONENT_t **ppComponent )
{
    /* set up variables and structures */
    L7_RC_t               crRC,
                          crRC1;
    CNFGR_CR_COMPONENT_t *pComponent;
    L7_dll_member_t      *pDLLMember;

    *ppComponent = *pCrHandle = L7_NULLPTR;                  /* output */

    /* Lock the CR */
    crRC1 = osapiSemaTake( crLock, L7_WAIT_FOREVER );
    if (crRC1 == L7_SUCCESS)
    {
        /* get the component */
        crRC  = DLLFirstGet( componentRepository.pDll, &pDLLMember );
        crRC1 = osapiSemaGive( crLock );
        if (crRC1 == L7_SUCCESS)
        {
            if (crRC == L7_SUCCESS)
            {
                pComponent = (CNFGR_CR_COMPONENT_t *)pDLLMember->data;
                crRC = osapiSemaTake ( pComponent->pLock, L7_WAIT_FOREVER );
                if (crRC == L7_SUCCESS)
                {
                    /* At this point return L7_SUCCESS (crRC) and the component to caller */
                    *ppComponent = pComponent;                           /* output */
                    *pCrHandle   = (CNFGR_CR_HANDLE_t)pDLLMember;        /* output */
                }
                else
                {
                    /* log message and return to caller */
                    LOG_MSG("cnfgrCrComponentFirstTake: failed to lock component... exiting \n");

                    /* set error return value */
                    crRC = L7_ERROR;

                } /* endif lock component */
            }
            else
            {
                /* log message and return to caller */
                LOG_MSG("cnfgrCrComponentFirstTake: failed to obtain member from CR... exiting \n");

                /* set error return value */
                crRC = L7_ERROR;

            } /* endif get member from CR */
        } 
        else
        {
            /* log message and return to caller */
            LOG_MSG("cnfgrCrComponentFirstTake: failed to unlock CR... exiting \n");

            /* set error return value */
            crRC = L7_ERROR;

        }/* end if unlock CR */
    }
    else
    {
        /* log message and return to caller */
        LOG_MSG("cnfgrCrComponentFirstTake: failed to lock CR... exiting \n");

        /* set error return value */
        crRC = L7_ERROR;

    } /* endif lock the CR */


   /* Return value to caller */
   return (crRC);
   
}

/*********************************************************************
* @purpose  This function puts the component to the repository.
*           User can get it to update content. This is a CR interface
*           function.
*
* @param    crHandle     - @b{(input)} unique identifier for component
*                                      to allow read acces only.
*
* @returns  L7_SUCCESS   - function completed succesfully. 
* @returns  L7_ERROR     - function failed. This COULD be a fatal error.
*
* @notes    None.                
*       
* @end
*********************************************************************/
L7_RC_t cnfgrCrComponentGive( CNFGR_IN CNFGR_CR_HANDLE_t crHandle )
{
    /* set up variables and structures */
    L7_RC_t               crRC;
    CNFGR_CR_COMPONENT_t *pComponent;
    L7_dll_member_t      *pDLLMember;

    /* validate handle */
    if ( crHandle != L7_NULLPTR ) 
    {
        /* Lock the CR */
        crRC = osapiSemaTake( crLock, L7_WAIT_FOREVER );
        if (crRC == L7_SUCCESS)
        {
            /* return the component - unlock component */
            pDLLMember = (L7_dll_member_t *)crHandle;
            pComponent = (CNFGR_CR_COMPONENT_t *)pDLLMember->data;
            crRC = osapiSemaGive( pComponent->pLock );
            if (crRC == L7_SUCCESS)
            {
                /* unlock CR */
                crRC = osapiSemaGive( crLock );
                if (crRC != L7_SUCCESS)
                {
                    /* log message and return to caller */
                    LOG_MSG("cnfgrCrComponentGive: failed to unlock CR... exiting \n");

                    /* set error return value */
                    crRC = L7_ERROR;

                } /* endif unlock CR */
            }
            else
            {
                /* log message and return to caller */
                LOG_MSG("cnfgrCrComponentGive: failed to unlock component... exiting \n");

                /* set error return value */
                crRC = L7_ERROR;
            } /* endif unlock the component */
        }
        else
        {
            /* log message and return to caller */
            LOG_MSG("cnfgrCrComponentGive: failed to lock CR... exiting \n");

            /* set error return value */
            crRC = L7_ERROR;

        } /* endif lock CR */
    }
    else
    {
        /* log message and return to caller */
        LOG_MSG("cnfgrCrComponentGive: invalid Handle... exiting \n");

        /* set error return value */
        crRC = L7_ERROR;

    } /* endif valid handle */

    /* Return value to caller */
    return (crRC);
}

/*********************************************************************
* @purpose  This function get the next component in CR.                
*           This is a CR interface function.
*
* @param    pCrHandle    - @b{(output)} location of unique identifier.
*                                       NULL if cid not found.     
*                                      
* @param    ppComponent  - @b{(output)} location of for the ComponentData
*                                       NULL if cid not found. 
*
* @returns  L7_SUCCESS   - function completed succesfully. Caller can update
*                          component data.
* @returns  L7_ERROR     - function failed. This COULD be a fatal error.
*
* @notes    Queries can be done to the while the component is locked.                     
*       
* @end
*********************************************************************/
L7_RC_t cnfgrCrComponentNextTake( CNFGR_INOUT CNFGR_CR_HANDLE_t *pCrHandle,
                                  CNFGR_OUT   CNFGR_CR_COMPONENT_t **ppComponent )
{
    /* set up variables and structures */
    L7_RC_t               crRC,
                          crRC1;
    CNFGR_CR_COMPONENT_t *pComponent;
    L7_dll_member_t      *pDLLMember = (L7_dll_member_t *)*pCrHandle;

    *ppComponent = *pCrHandle = L7_NULLPTR;                    /* output */

    /* Lock the CR */
    crRC1 = osapiSemaTake( crLock, L7_WAIT_FOREVER );
    if (crRC1 == L7_SUCCESS)
    {
        /* get the component */
        crRC  = DLLNextGet( &pDLLMember );
        crRC1 = osapiSemaGive( crLock );
        if (crRC1 == L7_SUCCESS)
        {
            if (crRC == L7_SUCCESS)
            {
                pComponent = (CNFGR_CR_COMPONENT_t *)pDLLMember->data;
                crRC = osapiSemaTake ( pComponent->pLock, L7_WAIT_FOREVER );
                if (crRC == L7_SUCCESS)
                {
                    /* At this point return L7_SUCCESS (crRC) and the component to caller */
                    *ppComponent = pComponent;                /* output */
                    *pCrHandle   = pDLLMember;                /* output */
                }
                else
                {
                    /* log message and return to caller */
                    LOG_MSG("cnfgrCrComponentFirstTake: failed to lock component... exiting \n");

                    /* set error return value */
                    crRC = L7_ERROR;

                } /* endif lock component */
            }
            else
            {
                /* log message and return to caller */
                LOG_MSG("cnfgrCrComponentFirstTake: failed to obtain member from CR... exiting \n");

                /* set error return value */
                crRC = L7_ERROR;

            } /* endif get member from CR */
        } 
        else
        {
            /* log message and return to caller */
            LOG_MSG("cnfgrCrComponentFirstTake: failed to unlock CR... exiting \n");

            /* set error return value */
            crRC = L7_ERROR;

        }/* end if unlock CR */
    }
    else
    {
        /* log message and return to caller */
        LOG_MSG("cnfgrCrComponentFirstTake: failed to lock CR... exiting \n");

        /* set error return value */
        crRC = L7_ERROR;

    } /* endif lock the CR */


   /* Return value to caller */
   return (crRC);
   
}

/*********************************************************************
* @purpose  This function gets a component from CR using CID key.       
*           This is a CR interface function.
*
* @param    cid          - @b{(input)}  Search key - component ID. 
*                                       
* @param    pCrHandle    - @b{(output)} location of unique identifier.
*                                       NULL if cid not found.     
*                                      
* @param    ppComponent  - @b{(output)} location of for the ComponentData
*                                       NULL if cid not found. 
*
* @returns  L7_SUCCESS   - function completed succesfully. Caller can update
*                          component data.
* @returns  L7_ERROR     - function failed. This COULD be a fatal error.
*
* @notes    Queries can be done to the while the component is locked.                     
*       
* @end
*********************************************************************/
L7_RC_t cnfgrCrComponentTake( CNFGR_IN  L7_COMPONENT_IDS_t cid, 
                              CNFGR_OUT CNFGR_CR_HANDLE_t *pCrHandle,
                              CNFGR_OUT CNFGR_CR_COMPONENT_t **ppComponent )
{
    /* set up variables and structures */
    L7_RC_t               crRC,
                          crRC1;
    CNFGR_CR_COMPONENT_t *pComponent;
    CNFGR_CR_HANDLE_t     crHandle;
    L7_BOOL               found;

    *ppComponent = *pCrHandle = L7_NULLPTR;                /* output */

    /* Lock the CR */
    crRC1 = osapiSemaTake( crLock, L7_WAIT_FOREVER );
    if ( crRC1 == L7_SUCCESS )
    {
        /* get the component */
        crRC  = cnfgrCrComponentFind( cid, &found, &crHandle, &pComponent);
        crRC1 = osapiSemaGive( crLock );
        if ( crRC1 == L7_SUCCESS )
        {
            if ( crRC == L7_SUCCESS )
            {
                if ( found == L7_TRUE )
                {
                    crRC = osapiSemaTake ( pComponent->pLock, L7_WAIT_FOREVER );
                    if (crRC == L7_SUCCESS)
                    {
                        /* At this point return L7_SUCCESS (crRC) and the component to caller */
                        *ppComponent = pComponent;                    /* output */
                        *pCrHandle   = crHandle;                      /* output */
                    }
                    else
                    {
                        /* log message and return to caller */
                        LOG_MSG("cnfgrCrComponentTake: failed to lock component... exiting \n");

                        /* set error return value */
                        crRC = L7_ERROR;

                    } /* endif lock component */
                } /* endif found */
            }
            else
            {
                /* log message and return to caller */
                LOG_MSG("cnfgrCrComponentTake: failed to obtain member from CR... exiting \n");

                /* set error return value */
                crRC = L7_ERROR;

            } /* endif get member from CR */
        } 
        else
        {
            /* log message and return to caller */
            LOG_MSG("cnfgrCrComponentTake: failed to unlock CR... exiting \n");

            /* set error return value */
            crRC = L7_ERROR;

        }/* end if unlock CR */
    }
    else
    {
        /* log message and return to caller */
        LOG_MSG("cnfgrCrComponentTake: failed to lock CR... exiting \n");

        /* set error return value */
        crRC = L7_ERROR;

    } /* endif lock the CR */

   /* Return value to caller */
   return (crRC);
   
}

/*********************************************************************
* @purpose  This function will attempt to return all CR resources.      
*           This is a CR interface function.
*
* @param    None.                                                   
*                                       
*
* @returns  None.                                                    
*
* @notes    None.                     
*       
* @end
*********************************************************************/
void cnfgrCrFini()
{
    /* set up variables and structures */
    L7_dll_member_t              *pDLLMember;

    if ( crLock != L7_NULLPTR )
    {
        (void)osapiSemaDelete(crLock);
        crLock = L7_NULLPTR;

    } /* endif crLock exist */

    /* 1. check if repository exists
     * 2. remove member from DLL
     * 3. delete the semaphore
     * 4. free the component data
     * 5. destroy the dll member
     * 6. reset repository init values to zero
    */
    if ( componentRepository.pDll != L7_NULLPTR ) 
    {
        while ( DLLIsEmpty( componentRepository.pDll ) == L7_FALSE )
        {
            (void)DLLTopDelete( componentRepository.pDll, &pDLLMember );
            (void)osapiSemaDelete( ((CNFGR_CR_COMPONENT_t *)(pDLLMember->data))->pLock );
            (void)osapiFree( L7_CNFGR_COMPONENT_ID, pDLLMember->data );
            (void)DLLMemberDestroy( pDLLMember ); 

        } /* endwhile CR is not empty */


        (void)DLLDestroy( componentRepository.pDll );
        
    } /* endif component repository exist */

    (void)bzero((char *)&componentRepository, sizeof( componentRepository ));

    /* return to caller */
    return;
}


/*********************************************************************
* @purpose  This function initializes CR  module. The use of this  
*           function is mandatory. This function is a CR interface.                          
*
* @param    None
*
* @returns  L7_SUCCESS - CR initialized successfully and it is         
*                        ready.
* @returns  L7_ERROR   - CR had problems and did not initialized.
*                        This is a fatal error.
*
*
* @notes    None                                              
*
*       
* @end
*********************************************************************/
L7_RC_t cnfgrCrInitialize()
{
    /* set up variables and structures */
    L7_RC_t                       crRC;
    CNFGR_CR_COMPONENT_t         *pComponentData = 0;
    L7_dll_member_t              *pDLLMember;
    CNFGR_COMPONENT_LIST_ENTRY_t *cil;
    L7_int32                      i, 
                                  options = CNFGR_SEMAPHORE_OPTIONS_f;
    HPC_UNIT_DESCRIPTOR_t        *local_unit=L7_NULLPTR;
    L7_BOOL                       skip_component;


    local_unit = hpcLocalUnitDescriptorGet ();
    if (local_unit == L7_NULLPTR)
    {
        /* FATAL ERROR: log message and return to caller */
        LOG_MSG("cnfgrCrInitialize: failed to obtain LocalUnitDescriptor... exiting \n");

        /* return error value to caller */
        return (L7_ERROR);
    }

    /* initialize CR -
     *
    */
    i = componentRepository.currentCount = 0;
    componentRepository.maxCount         = CNFGR_CR_COMPONENT_TABLE_END_f;

    cnfgrSidComponentNameCleanup();

    cil = cnfgrSidCrComponentListAddrGet();

    if ((crLock = osapiSemaMCreate( options )) == L7_NULLPTR)
    {
        /* FATAL ERROR: log message and return to caller */
        LOG_MSG("cnfgrCrInitialize: failed to obtain crLock... exiting \n");

        /* return error value to caller */
        return (L7_ERROR); 
    }

    /* <create the table> */
    crRC = DLLCreate(&componentRepository.pDll);
    if (crRC == L7_SUCCESS)
    {
        /* For all the component in the imput list
         * build an entry in the CR
        */ 
        do {
          skip_component = L7_FALSE;
          if (local_unit->unitTypeDescriptor.managementPreference == L7_UNITMGR_MGMTPREF_DISABLED)
          {
            if ((cil[i].cid != L7_LAST_COMPONENT_ID) &&
                (cnfgrSidNonMgrComponentCheck (cil[i].cid) != L7_TRUE))
            {
              skip_component = L7_TRUE;
            }
          }

          if (skip_component == L7_FALSE)
          {
            /* 1.-set up a component entry -
             *
             * <get component information from the input list (cnfgr_sid.c)>
             * <create and setup the component data>
            */
            pComponentData = (CNFGR_CR_COMPONENT_t *)osapiMalloc( L7_CNFGR_COMPONENT_ID, sizeof( CNFGR_CR_COMPONENT_t ) );
            if (pComponentData != L7_NULLPTR) 
            {
              pComponentData->status             = L7_CNFGR_COMPONENT_INACTIVE;
              pComponentData->currentState       = L7_CNFGR_STATE_IDLE;
              pComponentData->nextState          =
              pComponentData->saveState          = L7_CNFGR_STATE_NULL;
              pComponentData->cmd                = L7_CNFGR_CMD_FIRST;
              pComponentData->rqst               = L7_CNFGR_RQST_FIRST;
              pComponentData->aRsp.rc            = L7_ERROR;
              pComponentData->aRsp.u.reason      = L7_CNFGR_ERR_RC_FIRST;
              pComponentData->present            = L7_TRUE;
              pComponentData->mode               = cil[i].mode;              /* input */
              pComponentData->pCommandFunction   = cil[i].pComponentCmdFunc; /* input */
              pComponentData->cid                = cil[i].cid;               /* input */
              pComponentData->mnemonic           = cnfgrSidComponentMnemonicGet (cil[i].cid);
              pComponentData->name               = cnfgrSidComponentNameGet (cil[i].cid);
              pComponentData->enable_routine     = cil[i].enable_routine;
              pComponentData->pLock              = osapiSemaMCreate( options );
              if ( pComponentData->pLock != L7_NULLPTR )
              {
                  /* 2.- set up CR entry -
                   *
                   * <create a CR member and set up>
                  */
                  crRC = DLLMemberCreate( &pDLLMember, (void *)pComponentData );
                  if (crRC == L7_SUCCESS)
                  {
                      /* 3.- add member to the CR -
                       *
                      */
                      crRC = DLLBottomInsert( componentRepository.pDll, pDLLMember );
                      if (crRC != L7_SUCCESS)
                      {
                          /* FATAL ERROR: log message and return to caller */
                          LOG_MSG("cnfgrCrInitialize: failed to obtain table member... exiting \n");

                          /* return error to caller
                           * 
                           * NOTE: caller may need to clean up memory, crFini() should be called.
                           */
                          crRC = L7_ERROR;

                      } /* endif add member to the CR */
                  }
                  else
                  {
                      /* FATAL ERROR: log message and return to caller */
                      LOG_MSG("cnfgrCrInitialize: failed to obtain table member... exiting \n");

                      /* return error to caller
                       * 
                       * NOTE: caller may need to clean up memory, crFini() should be called.
                       */
                      crRC = L7_ERROR;

                  } /* endif create table member */
              }
              else
              {
                  /* FATAL ERROR: log message and return to caller */
                  LOG_MSG("cnfgrCrInitialize: failed to obtain componentData lock... exiting \n");

                  /* return error to caller
                   * 
                   * NOTE: caller may need to clean up memory, crFini() should be called.
                   */
                  crRC = L7_ERROR;

              } /* endif componentData Lock */
            }
            else
            {
                /* FATAL ERROR: log message and return to caller */
                LOG_MSG("cnfgrCrInitialize: failed to obtain componentData... exiting \n");

                /* return error to caller
                 * 
                 * NOTE: caller may need to clean up memory, crFini() should be called.
                 */
                crRC = L7_ERROR;

            } /* endif component data */
          }

         /* 4.- increment component count -
          *
         */
         i = ++componentRepository.currentCount;

        } while ( (skip_component == L7_TRUE) ||
		  ((pComponentData->cid != L7_LAST_COMPONENT_ID) && (crRC == L7_SUCCESS)) );
    } 
    else
    {
        /* FATAL ERROR: log message and return to caller */
        LOG_MSG("cnfgrCrInitialize: failed to obtain CR (DLL)... exiting \n");

        /* return error to caller */
        crRC = L7_ERROR;
    }

    /* return value to caller */
    return (crRC);
}


/*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/*
 *********************************************************************
 *                      Stub Functions
 *********************************************************************
*/

#else

L7_RC_t cnfgrApiQuery(CNFGR_INOUT L7_CNFGR_QUERY_DATA_t *pQueryData)
{  
    return (L7_ERROR);
}

#endif /* end cnfgr_cr module */

