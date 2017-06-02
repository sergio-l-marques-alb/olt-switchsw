/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    presenter.c
* @purpose     Implements the presenter functionality of stats comp
* @component   stats
* @comments    Provides an interface to collector from client
* @create      18/06/2007
* @author      YSaritha 
* @end
*             
**********************************************************************/

#include <stdio.h>

#include "commdefs.h"           /* lvl7 common definitions   */
#include "datatypes.h"          /* lvl7 data types definition */
#include "osapi.h"              /* lvl7 operating system apis */
#include "statsapi.h"           /* stats public methods */
#include "counter64.h"
#include "collector.h"
#include "presenter.h"
#include "statsconfig.h"
#include "log.h"


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
* @return       L7_SUCCESS     All of the specified counters were successfully 
*                             created 
* @return       L7_ERROR       Not Applicable
* @return       L7_FAILURE     One or more of the Counters could not be created
*
* @end
*
**********************************************************************
*/
L7_RC_t pre_statsCreate (Presenter * pStatsPre,
                         L7_uint32 listSize, pStatsParm_list_t pStatsParmList)
{
  L7_uint32 i;
  pStatsParm_entry_t curr = pStatsParmList;
  L7_RC_t l7rc = L7_SUCCESS;

  for (i = 0; i < listSize; i++)
  {
    /* Call the Collector's create method */
    collector_create (pStatsPre->pCollector, curr);

    /* If the Collector returns status == L7_FAILURE
       the counter could not be created */
    if (curr->status == L7_FAILURE)
    {
      printf ("ERROR!! failure in pre_statsCreate\n");
      l7rc = L7_FAILURE;
    }
    /* Increment curr to the next element in the list */
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
*               from the Statistics Manager
*          
* @parameter    L7_uint32         listSize
* @parameter    pCounterValue_list_t pCounterValueList
*
* @return       L7_SUCCESS      If all of the counter(s) could be successfully 
*                               found and stored in the space provided by the
*                               client 
* @return       L7_FAILURE      If one or more of the counter(s) could not be 
*                               successfully found and stored in the space 
*                               provided by the client
*
* @end
*
**********************************************************************
*/
L7_RC_t pre_statsGet (Presenter * pStatsPre,
                      L7_uint32 listSize, pCounterValue_list_t pCounterValueList)
{
  L7_uint32 i;
  L7_RC_t l7rc = L7_SUCCESS;

  /* Define and initialize curr to pCounterValueList */
  pCounterValue_entry_t curr = pCounterValueList;
  /* Define and initialize l7rc to L7_SUCCESS */

  for (i = 0; i < listSize; i++)
  {
    /* Call the Collector's get method */
    collector_get (pStatsPre->pCollector, curr);
    /* If the Collector returns status == L7_FAILURE
       the counter could not be found */
    if (curr->status == L7_FAILURE)
    {
      printf ("ERROR!! failure in pre_statsGet 0x%08x\n", curr->cId);
      l7rc = L7_FAILURE;
    }
    /* Increment curr to the next element in the list */
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
*               in the Statistics Manager
*          
* @parameter    L7_uint32               listSize
* @parameter    pCounterValue_list_t    pCounterValueList
* @return       L7_SUCCESS    If all of the specified counters could be 
*                             successfully reset
*               L7_FAILURE    If one or more of the specified counters could not 
*                             reset
*
* @end
*
**********************************************************************
*/
L7_RC_t pre_statsReset (Presenter * pStatsPre, L7_uint32 listSize,
                        pCounterValue_list_t pCounterValueList)
{
  L7_ushort16 i;
  L7_RC_t l7rc = L7_SUCCESS;

  /* Define and initialize curr to pCounterValueList */
  pCounterValue_entry_t curr = pCounterValueList;
  /* Define and initialize l7rc to L7_SUCCESS */

  for (i = 0; i < listSize; i++)
  {
    /* Call the Collector's reset method */
    collector_reset (pStatsPre->pCollector, curr);
    /* If the Collector returns status == L7_FAILURE
       the counter could not be reset */
    if (curr->status == L7_FAILURE)
    {
      l7rc = L7_FAILURE;
    }
    /* Increment curr to the next element in the list */
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
*               LocalCounters
*          
* @parameter    L7_uint32               listSize
*               pCounterValue_list_t    pCounterValueList
* @return       L7_SUCCESS      If all of the specified counter(s) were 
*                               successfully incremented 
* @return       L7_FAILURE      If one or more of the specified counter could not be 
*                               incremented
*
* @end
*
**********************************************************************
*/
L7_RC_t pre_statsIncrement (Presenter * pStatsPre,
                            L7_uint32 listSize, pCounterValue_list_t pCounterValueList)
{
  L7_ushort16 i;
  L7_RC_t l7rc = L7_SUCCESS;
  /* Define and initialize curr to pCounterValueList */

  pCounterValue_entry_t curr = pCounterValueList;
  /* Define and initialize l7rc to L7_SUCCESS */

  for (i = 0; i < listSize; i++)
  {
    /* Call the Collector's increment method */
    c_increment (pStatsPre->pCollector, curr);
    /* If the Collector returns status == L7_FAILURE
       the counter could not be incremented */
    if (curr->status == L7_FAILURE)
    {
      printf ("ERROR!! failure in pre_statsIncrement\n");
      l7rc = L7_FAILURE;
    }
    /* Increment curr to the next element in the list */
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
*               LocalCounters 
*          
* @parameter    L7_uint32               listSize
* @parameter    pCounterValue_list_t    pCounterValueList
*
* @return       L7_SUCCESS      If all the specified counters were successfully
*                               decremented 
* @return       L7_FAILURE      If one or more of the specified counters could not be 
*                               decremented
*
* @end
*
**********************************************************************
*/
L7_RC_t pre_statsDecrement (Presenter * pStatsPre,
                            L7_uint32 listSize, pCounterValue_list_t pCounterValueList)
{
  L7_ushort16 i;
  L7_RC_t l7rc = L7_SUCCESS;
  /* Define and initialize curr to pCounterValueList */
  pCounterValue_entry_t curr = pCounterValueList;
/* Define and initialize l7rc to L7_SUCCESS*/

  for (i = 0; i < listSize; i++)
  {
    /* Call the Collector's decrement method */
    c_decrement (pStatsPre->pCollector, curr);
    /* If the Collector returns status == L7_FAILURE
       the counter could not be decremented */
    if (curr->status == L7_FAILURE)
    {
      printf ("ERROR!! failure in pre_statsDecrement\n");
      l7rc = L7_FAILURE;
    }
    /* Increment curr to the next element in the list */
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
*                               added to the MutantSet(s) 
* @return       L7_FAILURE       If one or more of the specified counters could not be 
*                               added to any one of the mutantSet(s)
*
* @end
*
**********************************************************************
*/
L7_RC_t pre_statsMutlingsAdd (Presenter * pStatsPre,
                              L7_uint32 listSize, pStatsParm_list_t pStatsParmList)
{
  L7_uint32 i;
  L7_RC_t l7rc = L7_SUCCESS;
  /* Define and initialize curr to pCounterValueList */

  pStatsParm_list_t curr = pStatsParmList;
  /* Define and initialize l7rc to L7_SUCCESS */

  for (i = 0; i < listSize; i++)
  {
    /* Call the Collector's decrement method */
    collector_addMutlings (pStatsPre->pCollector, curr);
    /* If the Collector returns status == L7_FAILURE
       the counter could not be decremented */
    if (curr->status == L7_FAILURE)
    {
      printf ("ERROR!! failure in pre_statsMutlingsAdd\n");
      l7rc = L7_FAILURE;
    }
    /* Increment curr to the next element in the list */
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
*                              deleted from the mutantSet(s)
* @return       L7_FAILURE     If one or more of the specified counters could not be 
*                              deleted from the mutantSet(s)
*
* @end
*
**********************************************************************
*/
L7_RC_t pre_statsMutlingsDel (Presenter * pStatsPre,
                              L7_uint32 listSize, pStatsParm_list_t pStatsParmList)
{
  L7_uint32 i;
  L7_RC_t l7rc = L7_SUCCESS;
  /* Define and initialize curr to pCounterValueList */
  pStatsParm_list_t curr = pStatsParmList;
  /* Define and initialize l7rc to L7_SUCCESS */

  for (i = 0; i < listSize; i++)
  {
    /* Call the Collector's decrement method */
    collector_delMutlings (pStatsPre->pCollector, curr);
    /* If the Collector returns status == L7_FAILURE
       the counter could not be decremented */
    if (curr->status == L7_FAILURE)
    {
      printf ("ERROR!! failure in pre_statsMutlingsDel\n");
      l7rc = L7_FAILURE;
    }
    /* Increment curr to the next element in the list */
    curr++;
  }

  return l7rc;
}

/*
**********************************************************************
*:w

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
                         L7_uint32 listSize, pStatsParm_list_t pStatsParmList)
{
  L7_uint32 i;
  L7_RC_t l7rc = L7_SUCCESS;
  /* Define and initialize curr to pStatsParmList */
  pStatsParm_entry_t curr = pStatsParmList;
  /* Define and initialize l7rc to L7_SUCCESS */

  for (i = 0; i < listSize; i++)
  {
    /* Call the Collector's create method */
    c_counterDelete (pStatsPre->pCollector, curr);
    /* If the Collector returns status == L7_FAILURE */
    /* the counter could not be created */
    if (curr->status == L7_FAILURE)
    {
      printf ("ERROR!! failure in pre_statsDelete\n");
      l7rc = L7_FAILURE;
    }
    /* Increment curr to the next element in the list */
    curr++;
  }

  return l7rc;
}
