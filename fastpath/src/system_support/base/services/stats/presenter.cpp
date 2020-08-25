/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename presenter.h
*
* @purpose The purpose of this file is to define the presenter class
*
* @component Statistics Manager
*
* @comments none
*
* @create 10/08/2000
*
* @author Archana Suthan
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/


#include <statsinclude.h>

Presenter :: ~Presenter()
{
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
* @parameter    pStatsParm_list_t  pStatsParmList
*
* @return       L7_SUCCESS     All of the specified counters were successfully 
*					          created 
* @return       L7_ERROR       Not Applicable
* @return       L7_FAILURE     One or more of the Counters could not be created
*
* @end
*
**********************************************************************
*/
L7_RC_t Presenter :: statsCreate ( L7_uint32 listSize, 
					  pStatsParm_list_t pStatsParmList)
{
	// Define and initialize curr to pStatsParmList
	pStatsParm_entry_t curr = pStatsParmList;
	// Define and initialize l7rc to L7_SUCCESS
	L7_RC_t l7rc = L7_SUCCESS;
    
	for (L7_uint32 i=0; i < listSize; i++)
	{
		// Call the Collector's create method
		pCollector->create(curr);

		// If the Collector returns status == L7_FAILURE
		// the counter could not be created
		if (curr->status == L7_FAILURE) l7rc = L7_FAILURE;
		// Increment curr to the next element in the list
        curr++;
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
* @parameter    L7_uint32         listSize
* @parameter    pCounterValue_list_t pCounterValueList
*
* @return       L7_SUCCESS      If all of the counter(s) could be successfully 
*					            found and stored in the space provided by the
*						        client 
* @return       L7_FAILURE      If one or more of the counter(s) could not be 
*					            successfully found and stored in the space 
*						        provided by the client
*
* @end
*
**********************************************************************
*/
L7_RC_t Presenter :: statsGet ( L7_uint32 listSize, 
				   pCounterValue_list_t pCounterValueList)
{
	// Define and initialize curr to pCounterValueList
	pCounterValue_entry_t curr = pCounterValueList;
	// Define and initialize l7rc to L7_SUCCESS
	L7_RC_t l7rc = L7_SUCCESS;

	for (L7_uint32 i = 0; i < listSize; i++)
	{
		// Call the Collector's get method
		pCollector->get(curr);
		// If the Collector returns status == L7_FAILURE
		// the counter could not be found
		if (curr->status == L7_FAILURE) l7rc = L7_FAILURE;
		// Increment curr to the next element in the list
		curr++;
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
* @parameter    L7_uint32               listSize
* @parameter    pCounterValue_list_t    pCounterValueList
* @return       L7_SUCCESS    If all of the specified counters could be 
*					          successfully reset
*               L7_FAILURE    If one or more of the specified counters could not 
*						      reset
*
* @end
*
**********************************************************************
*/
L7_RC_t Presenter :: statsReset ( L7_uint32 listSize, 
					 pCounterValue_list_t pCounterValueList)
{
	// Define and initialize curr to pCounterValueList
	pCounterValue_entry_t curr = pCounterValueList;
	// Define and initialize l7rc to L7_SUCCESS
	L7_RC_t l7rc = L7_SUCCESS;

	for (L7_uint32 i = 0; i < listSize; i++)
	{
		// Call the Collector's reset method
		pCollector->reset(curr);
		// If the Collector returns status == L7_FAILURE
		// the counter could not be reset
		if (curr->status == L7_FAILURE) l7rc = L7_FAILURE;
		// Increment curr to the next element in the list
		curr++;
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
*               pCounterValue_list_t    pCounterValueList
* @return       L7_SUCCESS      If all of the specified counter(s) were 
*					            successfully incremented 
* @return       L7_FAILURE      If one or more of the specified counter could not be 
*						        incremented
*
* @end
*
**********************************************************************
*/
L7_RC_t Presenter :: statsIncrement ( L7_uint32  listSize, 
						 pCounterValue_list_t  pCounterValueList)
{
	// Define and initialize curr to pCounterValueList
	pCounterValue_entry_t curr = pCounterValueList;
	// Define and initialize l7rc to L7_SUCCESS
	L7_RC_t l7rc = L7_SUCCESS;

	for (L7_uint32 i = 0; i < listSize; i++)
	{
		// Call the Collector's increment method
		pCollector->increment(curr);
		// If the Collector returns status == L7_FAILURE
		// the counter could not be incremented
		if (curr->status == L7_FAILURE) l7rc = L7_FAILURE;
		// Increment curr to the next element in the list
		curr++;
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
* @parameter    pCounterValue_list_t    pCounterValueList
*
* @return       L7_SUCCESS      If all the specified counters were successfully
*						        decremented 
* @return       L7_FAILURE      If one or more of the specified counters could not be 
*						        decremented
*
* @end
*
**********************************************************************
*/
L7_RC_t Presenter :: statsDecrement ( L7_uint32 listSize, 
						 pCounterValue_list_t pCounterValueList)
{
	// Define and initialize curr to pCounterValueList
	pCounterValue_entry_t curr = pCounterValueList;
	// Define and initialize l7rc to L7_SUCCESS
	L7_RC_t l7rc = L7_SUCCESS;

	for (L7_uint32 i = 0; i < listSize; i++)
	{
		// Call the Collector's decrement method
		pCollector->decrement(curr);
		// If the Collector returns status == L7_FAILURE
		// the counter could not be decremented
		if (curr->status == L7_FAILURE) l7rc = L7_FAILURE;
		// Increment curr to the next element in the list
		curr++;
	}
	return l7rc;
}

/*
**********************************************************************
*
* @function     statsMutlingsAdd
*                       
* @purpose      This function allows the client to add mutlings to an 
*               existing mutantCounter. 
*          
* @parameter    L7_uint32        listSize
* @parameter    pStatsParm_list_t pStatsParmList
*
* @return       L7_SUCCESS       If all the specified counters were successfully
*						        added to the MutantSet(s) 
* @return       L7_FAILURE       If one or more of the specified counters could not be 
*						        added to any one of the mutantSet(s)
*
* @end
*
**********************************************************************
*/
L7_RC_t Presenter :: statsMutlingsAdd ( L7_uint32 listSize, 
						 pStatsParm_list_t pStatsParmList)
{
	// Define and initialize curr to pCounterValueList
	pStatsParm_list_t curr = pStatsParmList;
	// Define and initialize l7rc to L7_SUCCESS
	L7_RC_t l7rc = L7_SUCCESS;

	for (L7_uint32 i = 0; i < listSize; i++)
	{
		// Call the Collector's decrement method
		pCollector->addMutlings(curr);
		// If the Collector returns status == L7_FAILURE
		// the counter could not be decremented
		if (curr->status == L7_FAILURE) l7rc = L7_FAILURE;
		// Increment curr to the next element in the list
		curr++;
	}
	return l7rc;
}

/*
**********************************************************************
*
* @function     statsMutlingsDel
*                       
* @purpose      This function allows the client to delete mutlings from an 
*               existing mutantCounter. 
*          
* @parameter    L7_uint32               listSize
* @parameter    pStatsParm_list_t       pStatsParmList
*
* @return       L7_SUCCESS     If all the specified counters were successfully
*						       deleted from the mutantSet(s)
* @return       L7_FAILURE     If one or more of the specified counters could not be 
*						       deleted from the mutantSet(s)
*
* @end
*
**********************************************************************
*/
L7_RC_t Presenter :: statsMutlingsDel ( L7_uint32 listSize, 
						 pStatsParm_list_t pStatsParmList)
{
	// Define and initialize curr to pCounterValueList
	pStatsParm_list_t curr = pStatsParmList;
	// Define and initialize l7rc to L7_SUCCESS
	L7_RC_t l7rc = L7_SUCCESS;

	for (L7_uint32 i = 0; i < listSize; i++)
	{
		// Call the Collector's decrement method
		pCollector->delMutlings(curr);
		// If the Collector returns status == L7_FAILURE
		// the counter could not be decremented
		if (curr->status == L7_FAILURE) l7rc = L7_FAILURE;
		// Increment curr to the next element in the list
		curr++;
	}
	return l7rc;
}

/*
**********************************************************************
*
* @function     statsDelete
*
* @purpose      This function allows the client to delete one or more 
*		        counters  
*          
* @parameter    L7_uint32          listSize
* @parameter    pStatsParm_list_t  pStatsParmList
*
* @return       L7_SUCCESS     All of the specified counters were successfully 
*					          deleted 
* @return       L7_ERROR       Not Applicable
* @return       L7_FAILURE     One or more of the Counters could not be deleted
*
* @end
*
**********************************************************************
*/
L7_RC_t Presenter :: statsDelete ( L7_uint32 listSize, 
					  pStatsParm_list_t pStatsParmList)
{
	// Define and initialize curr to pStatsParmList
	pStatsParm_entry_t curr = pStatsParmList;
	// Define and initialize l7rc to L7_SUCCESS
	L7_RC_t l7rc = L7_SUCCESS;
    
	for (L7_uint32 i=0; i < listSize; i++)
	{
		// Call the Collector's create method
		pCollector->counterDelete(curr);

		// If the Collector returns status == L7_FAILURE
		// the counter could not be created
		if (curr->status == L7_FAILURE) l7rc = L7_FAILURE;
		// Increment curr to the next element in the list
        curr++;
	}
	
	return l7rc;
}

