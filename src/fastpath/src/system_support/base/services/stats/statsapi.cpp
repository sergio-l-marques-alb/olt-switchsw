/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename statsapi.cpp
*
* @purpose The purpose of this file is to implement the user interface 
*		   functions.
*
* @component Stats Manager
*
* @comments none
*
* @create 10/09/2000
*
* @author Archana Suthan
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#include <statsinclude.h>

const char * statsApiVersion = "HLD V1.0";

static StatsConfigurator *pStatsCfg = NULL;

static void *StatsLockSem = NULL;

#define pPRESENTER (pStatsCfg->pPresenter) 

/*
********************************************************************
*                           FUNCTIONS
********************************************************************
*/

/*
/------------------------------------------------------------------\
*                            ACCESS                                *
\------------------------------------------------------------------/
*/


/*
**********************************************************************
*
* @function     statsCfgInit
*
* @purpose      This function initializes the Statistics Manager and its 
*		        three major packages the Presenter, the MutantRegistry &
		        the Collector.  
*          
* @parameter    none
*
* @return       L7_SUCCESS     The Statistics Manager is initialized
* @return       L7_ERROR       Not Applicable
* @return       L7_FAILURE     The Statistics Manager could not be initialized
*
* @end
*
**********************************************************************
*/
EXT_API L7_RC_t statsCfgInit (void)
{

// Create a new StatsConfigurator
#ifndef STATS_STUB
	pStatsCfg = new StatsConfigurator();
#endif


	L7_RC_t l7rc = L7_FAILURE;
	
	if ((StatsLockSem = osapiSemaMCreate(OSAPI_SEM_Q_FIFO)) == NULL)
	{
		return (L7_FAILURE);
	}

	if (pStatsCfg != NULL)  // Sanity check that the StatsConfigurator 
	{						// exists
		// Call the StatsConfigurator's init function to instantiate
		// the Collector and the Presenter
		l7rc = pStatsCfg->statsCfgInit();
	}
    return l7rc;    
}
/*
**********************************************************************
*
* @function     statsCreate
*
* @purpose      This function allows the client to create one or more 
*		        counters  
*          
* @parameter    L7_uint32          listSize
*               pStatsParm_list_t  pStatsParmList
* @return       L7_SUCCESS     All the specified counters were successfully 
*					           created 
* @return       L7_ERROR       Not Applicable
* @return       L7_FAILURE     Counters could not be created
*
* @end
*
**********************************************************************
*/
EXT_API L7_RC_t statsCreate ( L7_uint32 listSize, 
					  pStatsParm_list_t pStatsParmList)
{
	L7_RC_t l7rc = L7_FAILURE;

    if(pStatsParmList == NULL)
      return l7rc;

	// Call the Presenter to create the required counters
	if (pStatsCfg != NULL) // Sanity check: Does the StatsConfigurator exist?
		if (pPRESENTER != NULL) 
        {
          if(osapiSemaTake(StatsLockSem, L7_WAIT_FOREVER) == L7_SUCCESS)
          {
			l7rc = pPRESENTER->statsCreate(listSize, pStatsParmList);
            osapiSemaGive(StatsLockSem);
          }
        }
    return l7rc;
}
/*
**********************************************************************
*
* @function     statsGet
*
* @purpose      This function allows the client to access counter information
*		        from the Statistics Manager
*          
* @parameter    L7_uint32               listSize
*	            pCounterValue_list_t    pCounterValueList
*
* @return       L7_SUCCESS  If all the counter(s) could be successfully 
*					        found and stored in the space provided by the
*						    client 
* @return       L7_ERROR    If the counter requested by the client does not 
*						    exist in the CounterFlyweight
*               L7_FAILURE  Catastrophic failure i.e. all the returned 
*						    values are invalid
*
* @end
*
**********************************************************************
*/
EXT_API L7_RC_t statsGet ( L7_uint32 listSize, 
				   pCounterValue_list_t pCounterValueList)
{
	L7_RC_t l7rc = L7_FAILURE;

    if(pCounterValueList == NULL)
      return l7rc;
	
	// Call the Presenter to get the values of the specified counters
	if (pStatsCfg != NULL) // Sanity check: Does the StatsConfigurator exist?
		if (pPRESENTER != NULL)
        {
          if(osapiSemaTake(StatsLockSem, L7_WAIT_FOREVER) == L7_SUCCESS)
          {
            l7rc = pPRESENTER->statsGet(listSize, pCounterValueList);
            osapiSemaGive(StatsLockSem);
          }
        }

	return l7rc;
}

/*
**********************************************************************
*
* @function     statsReset
*
* @purpose      This function allows the client to reset counter information
*		        in the Statistics Manager
*          
* @parameters   L7_uint32               listSize
*               pCounterValue_list_t    pCounterValueList
* @return       L7_SUCCESS              If all the specified counters could be 
*					                    successfully reset
* @return       L7_ERROR        If some counters could not be reset
* @return       L7_FAILURE      Catastrophic failure i.e. if none of the
*						        counters could be reset
*
* @end
*
**********************************************************************
*/
EXT_API L7_RC_t statsReset ( L7_uint32 listSize, 
					 pCounterValue_list_t pCounterValueList)
{
	L7_RC_t l7rc = L7_FAILURE;

    if(pCounterValueList == NULL)
      return l7rc;
    
	// Call the Presenter to reset the required counters
	if (pStatsCfg != NULL) // Sanity check: Does the StatsConfigurator exist?
		if (pPRESENTER != NULL)
        {
          if(osapiSemaTake(StatsLockSem, L7_WAIT_FOREVER) == L7_SUCCESS)
          {
			l7rc = pPRESENTER->statsReset(listSize, pCounterValueList);
            osapiSemaGive(StatsLockSem);
          }
        }

	return l7rc;
}

/*
**********************************************************************
*
* @function     statsIncrement
*
* @purpose      This function allows the client to increment the values of
*		        LocalCounters
*          
* @parameter    L7_uint32               listSize
* @parameter    pCounterValue_list_t    pCounterValueList
*
* @return       L7_SUCCESS      If the specified counter(s) was successfully
*						        incremented 
* @return       L7_ERROR        Not Applicable
* @return       L7_FAILURE      If the specified counter could not be 
*						        incremented
*
* @end
*
**********************************************************************
*/
EXT_API L7_RC_t statsIncrement ( L7_uint32  listSize, 
						 pCounterValue_list_t  pCounterValueList)
{
	L7_RC_t l7rc = L7_FAILURE;

    if(pCounterValueList == NULL)
      return l7rc;
	
	// Call the Presenter to increment the required counters
	if (pStatsCfg != NULL) // Sanity check: Does the StatsConfigurator exist?
		if (pPRESENTER != NULL) 
        {
          if(osapiSemaTake(StatsLockSem, L7_WAIT_FOREVER) == L7_SUCCESS)
          {
			l7rc = pPRESENTER->statsIncrement(listSize,pCounterValueList);
            osapiSemaGive(StatsLockSem);
          }
        }

	return l7rc;
}



/*
**********************************************************************
*
* @function     statsDecrement
*                       
* @purpose      This function allows the client to decrement the values of
*		        LocalCounters 
*          
* @parameter    L7_uint32               listSize
*               pCounterValue_list_t    pCounterValueList
*
* @return       L7_SUCCESS    If the specified counter(s) was successfully
*						      decremented 
* @return       L7_ERROR      Not Applicable
* @return       L7_FAILURE    If the specified counter could not be 
*						      decremented
*
* @end
*
**********************************************************************
*/
EXT_API L7_RC_t statsDecrement ( L7_uint32 listSize, 
						 pCounterValue_list_t pCounterValueList )
{
	L7_RC_t l7rc = L7_FAILURE;

    if(pCounterValueList == NULL)
      return l7rc;
    
	// Call the Presenter to decrement the required counters
	if (pStatsCfg != NULL) // Sanity check: Does the StatsConfigurator exist?
		if (pPRESENTER != NULL)
        {
          if(osapiSemaTake(StatsLockSem, L7_WAIT_FOREVER) == L7_SUCCESS)
          {
			l7rc = pPRESENTER->statsDecrement(listSize,pCounterValueList);
            osapiSemaGive(StatsLockSem);
          }
        }

	return l7rc;
}

/*
**********************************************************************
*
* @function     statsAddMutlings
*                       
* @purpose      this function allows the client to dynamically add
*               mutlings to a mutant counter 
*          
* @parameter    L7_uint32           listSize
* @parameter    pStatsParm_list_t   pStatsParmList
* @return       L7_SUCCESS      If all the specified counter(s) were successfully
*						        added to the mutant set 
* @return       L7_ERROR        Not Applicable
* @return       L7_FAILURE      If at least one of the specified counters could
*                               not be added to the mutant set.
*
* @end
*
**********************************************************************
*/
EXT_API L7_RC_t statsMutlingsAdd (L7_uint32 listSize, 
                                  pStatsParm_list_t  pStatsParmList)
{
	L7_RC_t l7rc = L7_FAILURE;
    
    if(pStatsParmList == NULL)
      return l7rc;
    
	// Call the Presenter to decrement the required counters
	if (pStatsCfg != NULL) // Sanity check: Does the StatsConfigurator exist?
		if (pPRESENTER != NULL)
        {
          if(osapiSemaTake(StatsLockSem, L7_WAIT_FOREVER) == L7_SUCCESS)
          {
			l7rc = pPRESENTER->statsMutlingsAdd(listSize,pStatsParmList);
            osapiSemaGive(StatsLockSem);
          }
        }

	return l7rc;
}


/*
**********************************************************************
*
* @function     statsDelMutlings
*                       
* @purpose      This function allows the client to dynamically delete
*               mutlings to a mutant counter 
*          
* @parameter    L7_uint32        listSize
* @parameter    pStatsParm_list_t pStatsParmList
*
* @return       L7_SUCCESS      If all the specified counter(s) were successfully
*						        deleted from the mutant set 
* @return       L7_ERROR        Not Applicable
* @return       L7_FAILURE      If at least one of the specified counters could
*                               not be deleted from the mutant set.
*
* @end
*
**********************************************************************
*/
EXT_API L7_RC_t statsMutlingsDel (L7_uint32 listSize, 
                                  pStatsParm_list_t  pStatsParmList)
{
	L7_RC_t l7rc = L7_FAILURE;

    if(pStatsParmList == NULL)
      return l7rc;
    
	// Call the Presenter to decrement the required counters
	if (pStatsCfg != NULL) // Sanity check: Does the StatsConfigurator exist?
		if (pPRESENTER != NULL)
        {
          if(osapiSemaTake(StatsLockSem, L7_WAIT_FOREVER) == L7_SUCCESS)
          {
			l7rc = pPRESENTER->statsMutlingsDel(listSize,pStatsParmList);
            osapiSemaGive(StatsLockSem);
          }
        }

	return l7rc;
    
}


/*
**********************************************************************
*
* @function     statsCreate
*
* @purpose      This function allows the client to create one or more 
*		        counters  
*          
* @parameter    L7_uint32          listSize
*               pStatsParm_list_t  pStatsParmList
* @return       L7_SUCCESS     All the specified counters were successfully 
*					           created 
* @return       L7_ERROR       Not Applicable
* @return       L7_FAILURE     Counters could not be created
*
* @end
*
**********************************************************************
*/
EXT_API L7_RC_t statsDelete ( L7_uint32 listSize, 
					  pStatsParm_list_t pStatsParmList)
{
	L7_RC_t l7rc = L7_FAILURE;
	// Call the Presenter to create the required counters
	if (pStatsCfg != NULL) // Sanity check: Does the StatsConfigurator exist?
		if (pPRESENTER != NULL) 
        {
#ifdef _L7_OS_VXWORKS_
          if(osapiTaskLock() == L7_SUCCESS)
          {
#endif /*_L7_OS_VXWORKS_ */
			l7rc = pPRESENTER->statsDelete(listSize, pStatsParmList);
#ifdef _L7_OS_VXWORKS_
            osapiTaskUnlock();
          }
#endif /*_L7_OS_VXWORKS_ */
        }
    return l7rc;
}

