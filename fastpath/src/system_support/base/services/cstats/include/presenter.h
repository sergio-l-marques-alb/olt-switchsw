/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    presenter.h
* @purpose     presenter API's and data structures
* @component   stats
* @comments    
* @create      18/06/2007
* @author      YSaritha 
* @end
*             
**********************************************************************/


#ifndef INCLUDE_PRESENTER_H
#define INCLUDE_PRESENTER_H

/*
**********************************************************************
*
* @class        Presenter
*
* @purpose      The Presenter is in direct contact with the client. It is 
*               invoked every time the client requires statistical information
*               from the system. It also provides the client with the 
*               flexibility of creating and maintaining its own counters.
*
* @attribute    Collector *pCollector
*  
* @member       statsGet: Returns the value of the specified counter(s) 
* @member       statsReset: Resets the value of the specified counter(s)
* @member       statsCreate: Creates counter(s) as specified by the client
* @member       statsIncrement: Increments the value of LocalCounter(s) 
* @member       statsDecrement: Decrements the value of LocalCounter(s) 
*
* @note         Implemented in src\application\stats\private\collector.cpp
*
* @end
*
**********************************************************************
*/

typedef struct
{
  Collector *pCollector;

} Presenter;
/*
**********************************************************************
*
* @function     statsCreate
*
* @purpose      This function allows the client to create one or more 
*               counters  
*          
* @parameter    L7_uint32          listSize
* @parameter    pStatsParm_list_t  pStatsParmList
*
* @return       L7_SUCCESS     Most of the specified counters were successfully 
*               created 
* @return       L7_ERROR       Not Applicable
* @return       L7_FAILURE     Most of the Counters could not be created
*
* @notes        Implemented in src\application\stats\public\presenter.cpp
*
* @end
**********************************************************************
*/

L7_RC_t pre_statsCreate (Presenter * pStatsPre,
                         L7_uint32 listSize, pStatsParm_list_t pStatsParmList);

/*
**********************************************************************
*
* @function     statsGet
*
* @purpose      This function allows the client to access counter information
*               from the Statistics Manager
*          
* @parameter    L7_uint32         listSize
* @parameter    pCounterValue_list_t pCounterValueList
*
* @return       L7_SUCCESS      If most of the counter(s) could be successfully 
*               found and stored in the space provided by the client 
* @return       L7_FAILURE      If most of the counter(s) could not be 
*               successfully found and stored in the space provided by 
*               the client
*
* @note         Implemented in src\application\stats\public\presenter.cpp
*
* @end
*
**********************************************************************
*/

L7_RC_t pre_statsGet (Presenter * pStatsPre,
                      L7_uint32 listSize, pCounterValue_list_t pCounterValueList);

/*
**********************************************************************
*
* @function     statsReset
*
* @purpose      This function allows the client to reset counter information
*               in the Statistics Manager
*          
* @parameter    L7_uint32          listSize
* @parameter    pCounterValue_list_t pCounterValueList
*
* @return       L7_SUCCESS      If most of the specified counters could be 
*                               successfully reset
* @return       L7_FAILURE      If most of the specified counters could not 
*                               reset
*
* @note         Implemented in src\application\stats\public\presenter.cpp
*
* @end
**********************************************************************
*/

L7_RC_t pre_statsReset (Presenter * pStatsPre,
                        L7_uint32 listSize, pCounterValue_list_t pCounterValueList);


/*
**********************************************************************
*
* @function     statsIncrement
*
* @purpose      This function allows the client to increment the values of
*               LocalCounters
*          
* @parameter    L7_uint32        listSize
* @parameter    pCounterValue_list_t pCounterValueList
*
* @return       L7_SUCCESS      If most of the specified counter(s) were 
*                               successfully incremented 
* @return       L7_FAILURE      If most of the specified counter could not be 
*                               incremented
*
* @note         Implemented in src\application\stats\public\presenter.cpp
*
* @end
**********************************************************************
*/

L7_RC_t pre_statsIncrement (Presenter * pStatsPre,
                            L7_uint32 listSize, pCounterValue_list_t pCounterValueList);



/*
**********************************************************************
*
* @function     statsDecrement
*
* @purpose      This function allows the client to decrement the values of
*               LocalCounters
*          
* @parameter    L7_uint32        listSize
* @parameter    pCounterValue_list_t    pCounterValueList
*
* @return       L7_SUCCESS      If most of the specified counter(s) were 
*                               successfully decremented 
* @return       L7_FAILURE      If most of the specified counter could not be 
*                               decremented
*
* @note         Implemented in src\application\stats\public\presenter.cpp
*
* @end
**********************************************************************
*/

L7_RC_t pre_statsDecrement (Presenter * pStatsPre,
                            L7_uint32 listSize, pCounterValue_list_t pCounterValueList);

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
* @return       L7_SUCCESS      If all the specified counters were successfully
*                               added to the MutantSet(s) 
* @return       L7_FAILURE      If one or more of the specified counters could not be 
*                               added to any one of the mutantSet(s)
*
* @end
*
**********************************************************************
*/
L7_RC_t pre_statsMutlingsAdd (Presenter * pStatsPre,
                              L7_uint32 listSize, pStatsParm_list_t pStatsParmList);


/*
**********************************************************************
*
* @function     statsMutlingsDel
*                       
* @purpose      This function allows the client to delete mutlings from an 
*               existing mutantCounter. 
*          
* @parameter    L7_uint32          listSize
* @parameter    pStatsParm_list_t  pStatsParmList
*
* @return       L7_SUCCESS      If all the specified counters were successfully
*                               deleted from the mutantSet(s)
*               L7_FAILURE      If one or more of the specified counters could not be 
*                               deleted from the mutantSet(s)
*
* @end
*
**********************************************************************
*/
L7_RC_t pre_statsMutlingsDel (Presenter * pStatsPre,
                              L7_uint32 listSize, pStatsParm_list_t pStatsParmList);

/*
**********************************************************************
*
* @function     statsDelete
*
* @purpose      This function allows the client to delete one or more 
*               counters  
*          
* @parameter    L7_uint32          listSize
* @parameter    pStatsParm_list_t  pStatsParmList
*
* @return       L7_SUCCESS     All of the specified counters were successfully 
*                             deleted 
* @return       L7_ERROR       Not Applicable
* @return       L7_FAILURE     One or more of the Counters could not be deleted
*
* @end
*
**********************************************************************
*/
L7_RC_t pre_statsDelete (Presenter * pStatsPre,
                         L7_uint32 listSize, pStatsParm_list_t pStatsParmList);



/*
/------------------------------------------------------------------\
*            END OF PRESENTER DEFINITION                           *
\------------------------------------------------------------------/
*/

#endif /* INCLUDE_PRESENTER_H */
