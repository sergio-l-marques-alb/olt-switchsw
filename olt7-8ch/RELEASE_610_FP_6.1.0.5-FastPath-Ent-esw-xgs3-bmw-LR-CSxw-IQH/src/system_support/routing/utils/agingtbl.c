/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename       agingtbl.c
 *
 * @purpose        Aging table object implementation.
 *
 * @component      Routing Utils Component
 *
 * @comments
 *
 *  Aging table object can be used in order to keep track of an
 *  age of the user's data elements. 
 *  For example, routing protocol OSPF requires to
 *  store the protocol messages (LSA) in the data base no more than 
 *  specified maximal time (1 hour). 
 *  Each LsRefresh time (30 mins) each LSA message has
 *  to be reoriginated; Each 5 mins the checksum of the message has to
 *  be checked. To support all these requirements the aging table object
 *  has to be created for the application.
 *
 *  The Aging Table consists of Aging lists.
 *  All the user elements (in OSPF case these are LSAs) of the given age
 *  are put into the same aging list. 
 *  Value of Index0 represents index of the Aging List containing elements
 *  of the age 0. 
 *  Every AgingInterval procedure AgingTbl_AgingTimerTick is called from
 *  the timer. (AgingTbl_AgingTimerTick procedure runs under the user thread)
 *  It performed the following actions:
 *      - updates (decrements) value of Index0; 
 *      - calls procedure supplied by the user;
 *      - moves all entries of the maximal age to the overaged list
 *          (AgingTable[TblMaxIndex + 1]).
 *
 *   IMPORTANT NOTE
 *   --------------
 *   Since the Aging object is not thread safety, it is recommended
 *   to call API procedures only from the user thread specified in
 *   AgingTbl_Init procedure.
 *   This way all aging related code for one user application 
 *   should run under single thread.
 *
 *
 * @create         02/04/23
 *
 * @author         dima
 *
 * @end
 *
 * ********************************************************************/

#include <string.h>
#include "std.h"
#include "xx.ext"
#include "timer.ext"
#include "agingtbl.ext"


/* data type definitions */

/* Entry of aging table */
typedef struct t_AgingEntry
{
   struct t_AgingEntry *next;
   struct t_AgingEntry *prev;
   void  *p_Data;

} t_AgingEntry;

typedef struct t_AgingObj
{
   t_Handle      userId;      /*  handle of the user object */
	t_Handle      userThread;  /*  user thread               */

   /* Aging object configuration information */
	word          AgingMaxAge;
   word          AgingInterval;

	/* Aging object internal information */
   XX_Status     xx_status;     /* status :    XX_STAT_Valid/XX_STAT_Invalid */
   Bool          agingRunning;  /* stopped/running                           */

	t_AgingEntry **AgingTable; /* Each entry of the table is a list of user   */
	                           /* objects (messages, routes, etc ...) with    */
	                           /* the same age. Overaged elements are located */
	                           /* the last entry (TblMaxIndex + 1 )           */
  word          TblMaxIndex; /* Maximal index of AgingTable                 */
	word          AgingIndex0; /* Index of the AgingTable entry where data    */
	                           /* with age 0 are contained                    */
	t_Handle      AgingTimer;  /* The aging timer                             */
	
	F_AgingTickCB f_AgingTickCB;

	ulng          numOfEntries; /* Number of entries into the aging table */
} t_AgingObj;

/* Internal procedure */
static e_Err AgingTbl_AgingTimerTick(t_Handle Id, t_Handle TimerId, word Flag);
static e_Err deleteEntryFromAgingList(void *p_Data, t_AgingEntry **AgingList);

/***************Public API***********************/
/*--------------------------------------------------------------------
 * ROUTINE:    AgingTbl_Init
 *
 * DESCRIPTION:
 *    Creates an AgingTbl object.
 * ARGUMENTS:
 *    userId     - handle of the user object
 *    userThread - handle of user thread
 *    AgingObjId - Address of the variable to store the new handle.
 * RETURNS
 *    AgingObjId - the handle of the created object is returned here
 *
 * RESULTS:
 *    E_OK       - AgingTbl object has been created successfully
 *    E_FAILED   - cannot created an object
 *    E_NOMEMORY - there is no enough memory
 *--------------------------------------------------------------------*/
e_Err AgingTbl_Init(t_Handle userId, t_Handle userThread, t_Handle *AgingObjId)
{
   t_AgingObj *p_Obj;
	e_Err       e             = E_FAILED;
	
	/* Verify the input parameters */
	if(!userId || !AgingObjId)
		return E_FAILED;

	/* Allocate the aging object and fill config info */
	p_Obj = (t_AgingObj *)XX_Malloc(sizeof(t_AgingObj));
	if(!p_Obj)
		return E_NOMEMORY;
	memset(p_Obj, 0, sizeof(t_AgingObj));

	p_Obj->userId        = userId;
	p_Obj->userThread    = userThread;

	/* Create aging timer */
	e = TIMER_InitSec( 1, (t_Handle)p_Obj, &p_Obj->AgingTimer);
	if(e != E_OK)
	{
		XX_Free(p_Obj);
		return e;
	}

	/* If we get here, everything is OK */
	p_Obj->xx_status = XX_STAT_Valid;

   *AgingObjId = p_Obj;

   return E_OK;

}

/*--------------------------------------------------------------------
 * ROUTINE:    AgingTbl_Destroy
 *
 * DESCRIPTION:
 *    Destroys the AgingTbl object.
 * ARGUMENTS:.
 *    AgingObjId - pointer to the handle of an object being destroyed;
 *                 (is set to NULLP after destoying)
 * RESULTS:
 *    E_OK       - AgingTbl object has been destroyed successfully
 *    E_FAILED   - the object is in incorrect state
 *--------------------------------------------------------------------*/
e_Err AgingTbl_Destroy(t_Handle *AgingObjId)
{
  t_AgingObj *p_Obj;
  t_AgingEntry *entry = NULLP, *nextEntry = NULLP;
  word i = 0;

  if (!AgingObjId)
    return E_FAILED;

  p_Obj = (t_AgingObj *)*AgingObjId;
  if(!p_Obj || p_Obj->xx_status != XX_STAT_Valid)
		return E_FAILED;

	p_Obj->xx_status = XX_STAT_Invalid;

	/* stop the aging timer */
	TIMER_Delete(p_Obj->AgingTimer);

	/* Deallocate the aging table */
   if(p_Obj->AgingTable)
	{
      for(i = 0; i<=p_Obj->TblMaxIndex + 1; i++)
		{
		   entry = p_Obj->AgingTable[i];
		   while(entry)
			{
			   nextEntry = entry->next;
			   XX_Free(entry);
			   entry = nextEntry;
			}
		}
      XX_Free(p_Obj->AgingTable);
      p_Obj->AgingTable = NULLP;
   }

	/* deallocate the object */
	XX_Free(p_Obj); 
  *AgingObjId = 0;

	return E_OK;
}

/*--------------------------------------------------------------------
 * ROUTINE:    AgingTbl_Start
 *
 * DESCRIPTION:
 *    Starts the AgingTbl object: 
 *    (allocates aging table and starts the aging timer
 *     for AgingInterval seconds)    
 * ARGUMENTS:.
 *    AgingObjId    - the handle of the object being started;
 *    AgingMaxAge   - maximal age
 *    AgingInterval - time interval
 *    f_AgingTickCB - user callback procedure called 
 *                    each AgingInterval seconds 
 * RESULTS:
 *    E_OK       - AgingTbl object has been started successfully
 *    E_FAILED   - otherwise
 *--------------------------------------------------------------------*/
e_Err AgingTbl_Start(t_Handle *AgingObjId, word AgingMaxAge, word AgingInterval,
                     F_AgingTickCB f_AgingTickCB)
{
	t_AgingObj   *p_Obj = (t_AgingObj *)AgingObjId;

	ASSERT(p_Obj);
	ASSERT(p_Obj->xx_status == XX_STAT_Valid);

	/* Verification of the input parameters */
   if(!p_Obj || p_Obj->xx_status != XX_STAT_Valid || !p_Obj->AgingTimer)
		return E_FAILED;
		
	if(!AgingMaxAge || !AgingInterval)
		return E_BADPARM;

	/* Check accordance between maxAge and aging interval. Value of the */
	/* aging timer interval has to be a divisor of maxAge               */
	if(AgingMaxAge % AgingInterval)
		return E_BADPARM;

	if(p_Obj->agingRunning)
		return E_OK;

   /* Fill config info into the object*/
	p_Obj->AgingMaxAge    = AgingMaxAge;
	p_Obj->AgingInterval  = AgingInterval;
	p_Obj->f_AgingTickCB  = f_AgingTickCB;

	/* Create aging table  */
	p_Obj->TblMaxIndex    = AgingMaxAge/AgingInterval;
	p_Obj->AgingTable = XX_Malloc((p_Obj->TblMaxIndex + 2) * sizeof(ulng));
	if(!p_Obj->AgingTable)
	{
		return E_NOMEMORY;
	}
	memset(p_Obj->AgingTable, 0, (p_Obj->TblMaxIndex + 2) * sizeof(ulng));

	/* Start the aging timer  */ 
	if(!TIMER_Active(p_Obj->AgingTimer))
		if(TIMER_StartSec(p_Obj->AgingTimer, p_Obj->AgingInterval, TRUE, 
                AgingTbl_AgingTimerTick, p_Obj->userThread) != E_OK)
		{
			p_Obj->agingRunning = FALSE;
			return E_FAILED;
		}

   /* If we get here everything is OK */
   p_Obj->agingRunning = TRUE;
   p_Obj->numOfEntries = 0;

   return E_OK;

}

/*--------------------------------------------------------------------
 * ROUTINE:    AgingTbl_StopAndCleanup
 *
 * DESCRIPTION:
 *    Stops the aging timer and deallocates all the aging lists; 
 *    does not delete the timer and does no destroy the object.     
 * ARGUMENTS:.
 *    AgingObjId - the handle of aging object;
 * RESULTS:
 *    E_OK       - AgingTbl object has been stopped successfully
 *    E_FAILED   - otherwise
 *--------------------------------------------------------------------*/
e_Err AgingTbl_StopAndCleanup(t_Handle *AgingObjId)
{
	t_AgingObj   *p_Obj = (t_AgingObj *)AgingObjId;
   t_AgingEntry *entry = NULLP, *nextEntry = NULLP;
	word         i      = 0;

   ASSERT(p_Obj);
	ASSERT(p_Obj->xx_status == XX_STAT_Valid);

	if(!p_Obj || p_Obj->xx_status != XX_STAT_Valid)
		return E_FAILED;

	if(TIMER_Active(p_Obj->AgingTimer))
		TIMER_Stop(p_Obj->AgingTimer);

	/* Deallocate the aging table lists and the aging table */
  if(p_Obj->AgingTable)
  {
    for(i = 0; i<=p_Obj->TblMaxIndex; i++)
    {
      entry = p_Obj->AgingTable[i];
      while(entry)
      {
        nextEntry = entry->next;
        XX_Free(entry);
        entry = nextEntry;
      }
      p_Obj->AgingTable[i] = NULLP;
    }

     XX_Free(p_Obj->AgingTable);
     p_Obj->AgingTable = NULLP;
  }

  p_Obj->agingRunning = FALSE;
  p_Obj->numOfEntries = 0;

  return E_OK;
}

/*--------------------------------------------------------------------
 * ROUTINE:    AgingTbl_AgeToIndex
 *
 * DESCRIPTION:
 *    Converts the age to aging table index.
 * ARGUMENTS:.
 *    AgingObjId - the handle of aging object;
 *    Age        - the converted age
 *    Index      - pointer to index in aging table
 * RESULTS:
 *    E_OK       - the age has been converted successfully
 *    E_FAILED   - The specified age cannot be converted to index
 *--------------------------------------------------------------------*/
e_Err AgingTbl_AgeToIndex(t_Handle *AgingObjId, word Age, word *Index)
{
	t_AgingObj *p_Obj = (t_AgingObj *)AgingObjId;
	word index;

	if(!p_Obj || !Index)
		return E_FAILED;

	if(Age > p_Obj->AgingMaxAge)
	{
		index = p_Obj->TblMaxIndex + 1;
	}
   else
	{
      index = p_Obj->AgingIndex0 + (Age/p_Obj->AgingInterval);

	   if(index > p_Obj->TblMaxIndex)
		   index = index - p_Obj->TblMaxIndex - 1;
	}

	*Index = index;
	return E_OK;
}

/*--------------------------------------------------------------------
 * ROUTINE:   AgingTbl_AddEntry 
 *     
 * DESCRIPTION:
 *    Adds a new entry with the specified age to the aging object     
 * ARGUMENTS:.
 *    AgingObjId - the handle of aging object;
 *    p_Data     -  data of the entry to be added
 *    Age        -  Age of the entry to be added
 * RETURNS
 *    AgeIndex   - Index of the aging list where the new entry 
 *                 has been added
 * RESULTS:
 *    E_OK       - New entry has been added successfully
 *    E_FAILED   - otherwise
 *--------------------------------------------------------------------*/
e_Err AgingTbl_AddEntry(t_Handle *AgingObjId, void *p_Data, word Age, 
								word *AgeIndex)
{
	t_AgingObj   *p_Obj = (t_AgingObj *)AgingObjId;
	word          agingIndex;
	t_AgingEntry *newEntry;

	if(!p_Obj || (p_Obj->xx_status != XX_STAT_Valid) ||
		!p_Obj->agingRunning)
		return E_FAILED;

	if(!p_Data)
		return E_FAILED;

	/* Find the list corresponding to the age */
	if(AgingTbl_AgeToIndex(AgingObjId, Age, &agingIndex) != E_OK)
	   return NULLP;

	newEntry = XX_Malloc(sizeof(t_AgingEntry));

	if(!newEntry)
		return E_NOMEMORY;

	newEntry->p_Data = p_Data;
	XX_AddToDLList(newEntry, p_Obj->AgingTable[agingIndex]);

	if(AgeIndex)
		*AgeIndex = agingIndex;

	p_Obj->numOfEntries ++;
	return E_OK;
}


/*--------------------------------------------------------------------
 * ROUTINE:  AgingTbl_DeleteEntry 
 *
 * DESCRIPTION:
 *    Deletes entry from specified list of the aging object 
 * ARGUMENTS:.
 *    AgingObjId -  handle of the aging object;
 *    p_Data     -  data of the entry to be deleted
 *    AgeIndex   -  Index of the entry to be deleted in the Aging Table
 * RESULTS:
 *    E_OK       - entry has been deleted successfully
 *    E_FAILED   - otherwise
 *--------------------------------------------------------------------*/
e_Err AgingTbl_DeleteEntry(t_Handle *AgingObjId, void *p_Data, word AgeIndex)
{
	t_AgingObj    *p_Obj = (t_AgingObj *)AgingObjId;
	e_Err          e = E_FAILED;

	if(!p_Obj || (p_Obj->xx_status != XX_STAT_Valid) || !p_Data)
		return E_FAILED;

	if(AgeIndex > p_Obj->TblMaxIndex + 1)
		return E_FAILED;
		
	e = deleteEntryFromAgingList(p_Data,&p_Obj->AgingTable[AgeIndex]);

	/* If the deleted element has not been found in the aging list */
	/* maybe it was moved to the overaged list                     */
	if(e != E_OK)
		e = deleteEntryFromAgingList(p_Data, 
		           &p_Obj->AgingTable[p_Obj->TblMaxIndex + 1]);

	if(e == E_OK)
		p_Obj->numOfEntries --;
	

	/*ASSERT(e == E_OK);*/

	return E_OK;
}


/*--------------------------------------------------------------------
 * ROUTINE: AgingTbl_Running  
 *
 * DESCRIPTION:
 *    Returns TRUE if the aging object is running.   
 * ARGUMENTS:.
 *    AgingObjId - the handle of aging object;
 * RESULTS:
 *    TRUE    - AgingTbl object is running,
 *    FALSE   - AgingTBK object is stopped
 *--------------------------------------------------------------------*/
Bool AgingTbl_Running(t_Handle AgingObjId)
{
	t_AgingObj  *p_Obj = (t_AgingObj *)AgingObjId;

	return(p_Obj && (p_Obj->xx_status == XX_STAT_Valid) &&
		    p_Obj->agingRunning);

}

/*--------------------------------------------------------------------
 * ROUTINE:  AgingTbl_FindNext
 *
 * DESCRIPTION:
 *    Finds the next entry after the current one    
 * ARGUMENTS:.
 *    AgingObjId - the handle of aging object;
 *    AgeIndex   - Index of age list of the current entry
 *    currValue  - current entry
 * RETURNS:
 *    value      - the next value having the same age as the 
 *                 current one   
 * RESULTS:
 *    E_OK       - next entry with the same age as the current
 *                 entry has been found successfully
 *    E_FAILED   - otherwise
 *--------------------------------------------------------------------*/
e_Err AgingTbl_FindNext(t_Handle *AgingObjId, word AgeIndex, 
							  void *currValue, void **value)
{
	t_AgingObj    *p_Obj = (t_AgingObj *)AgingObjId;
	t_AgingEntry  *entry = NULLP, *AgingList = NULLP;

	if(!p_Obj || (p_Obj->xx_status != XX_STAT_Valid))
		return E_FAILED;

	/* Find the list corresponding to the age */
	if(AgeIndex > p_Obj->TblMaxIndex + 1)
      return E_FAILED;

	AgingList = p_Obj->AgingTable[AgeIndex];
  entry = AgingList;
 
	if(currValue)
  {
    /* Find the current value */
    while(entry && (entry->p_Data != currValue))
      entry = entry->next;

    /* get the next entry */
    if(entry)
      entry = entry->next;
  }

  if(!entry)
    return E_NOT_FOUND;

	/* Store the found next entry data, or the first entry at 
   * this inded if currValue is not specified.
   */
	if(value)
		*value = entry->p_Data;

	return E_OK;
}

/*--------------------------------------------------------------------
 * ROUTINE:
 *    AgingTbl_Browse
 * DESCRIPTION:     
 *    Aging object browse procedure.
 *    Goes over all the entries of the aging table and calls 
 *    the user browse function for each one; 
 *    if the user browse function returned FALSE
 *    deletes this entry.        
 * ARGUMENTS:.
 *    AgingObjId - the handle of aging object;
 *    f_Browse   - the user procedure called for each entry
 *    param      - parameter passed to the user procedure f_Browse  
 * RESULTS:
 *    E_OK       - browsing has been finished OK
 *    E_FAILED   - otherwise
 *--------------------------------------------------------------------*/
e_Err AgingTbl_Browse(t_Handle *AgingObjId, AGING_BROWSEFUNC f_Browse, 
							ulng param)

{
	t_AgingObj    *p_Obj = (t_AgingObj *)AgingObjId;
	word           i;
	t_AgingEntry  *entry = NULLP, *nextEntry = NULLP;

	if(!p_Obj || (p_Obj->xx_status != XX_STAT_Valid) || !f_Browse)
		return E_FAILED;

	for(i = 0; i <= p_Obj->TblMaxIndex + 1; i++)
	{
		entry = p_Obj->AgingTable[i];
		while(entry)
		{
			nextEntry = entry->next;
			if(!f_Browse(p_Obj->userId, entry->p_Data, param))
			{
			   XX_DelFromDLList(entry, p_Obj->AgingTable[i]);
				XX_Free(entry);
			}
			entry = nextEntry;
		}
		p_Obj->AgingTable[i] = NULLP;
	}

	return E_OK;
}

/*--------------------------------------------------------------------
 * ROUTINE:   
 *     AgingTbl_BrowseAge
 * DESCRIPTION:
 *     One more browse procedure.
 *     Goes over all the entries having the specified age
 *     and calls the user browse function for each one.
 *     If the user browse function returned FALSE. 
 *     deletes this entry.
 *     If the specified age is more than the Maximal age of the
 *     aging object, the browse procedure goes over overaged
 *     list and calls the user browse function for each overaged entry.
 * ARGUMENTS:.
 *    AgingObjId - the handle of aging object;
 * RESULTS:
 *    E_OK       - browsing has been finished OK
 *    E_FAILED   - otherwise
 *--------------------------------------------------------------------*/
e_Err AgingTbl_BrowseAge(t_Handle *AgingObjId, AGING_BROWSEFUNC f_Browse, 
							    word Age, ulng param)

{
	t_AgingObj    *p_Obj = (t_AgingObj *)AgingObjId;
	word           agingIndex;
	t_AgingEntry  *entry = NULLP, *nextEntry = NULLP, *AgingList = NULLP;

	if(!p_Obj || (p_Obj->xx_status != XX_STAT_Valid) || !f_Browse)
		return E_FAILED;

	/* Find the list corresponding to the age */
	if(AgingTbl_AgeToIndex(AgingObjId, Age, &agingIndex) != E_OK)
      return E_FAILED;

	AgingList = p_Obj->AgingTable[agingIndex];
	
	entry = AgingList;
	while(entry)
	{
		nextEntry = entry->next;
		if(!f_Browse(p_Obj->userId, entry->p_Data, param))
		{
			XX_DelFromDLList(entry,p_Obj->AgingTable[agingIndex]);
			XX_Free(entry);
		}
		entry = nextEntry;
	}	

	return E_OK;
}

/*--------------------------------------------------------------------
 * ROUTINE:    
 *		AgingTbl_AgingTimerTick
 * DESCRIPTION:
 *    This is the main procedure of the Aging object.
 *    It is called every AgingInterval and perform the following actions:
 *      - updates index of list containg entries with the age 0,
 *      - calles the user callback procedure,
 *      - moves all max aged entries to overages list
 *         (i.e. list AgingTable[max index + 1]).
 *        
 * ARGUMENTS:.
 *    AgingObjId - the handle of aging object;
 * RESULTS:
 *    E_OK       - AgingTbl object has been stopped successfully
 *    E_FAILED   - otherwise
 *--------------------------------------------------------------------*/
static e_Err AgingTbl_AgingTimerTick(t_Handle Id, t_Handle TimerId, word Flag)

{
   t_AgingObj     *p_Obj = (t_AgingObj *)Id;
	word           maxAgeIndex;
   t_AgingEntry  *entry, *nextEntry;
	e_Err          err; 

   p_Obj->AgingIndex0 = (p_Obj->AgingIndex0 == 0) ? 
		                p_Obj->TblMaxIndex : p_Obj->AgingIndex0 - 1;

	/* Call the user aging procedure */
	if(p_Obj->f_AgingTickCB)
		err = p_Obj->f_AgingTickCB(p_Obj->userId, (t_Handle)p_Obj);

	/* Move all maxAge entries from the aging table to the overage list*/
   maxAgeIndex = p_Obj->AgingIndex0 ? 
      p_Obj->AgingIndex0 - 1 : p_Obj->TblMaxIndex;
	
   entry = p_Obj->AgingTable[maxAgeIndex];
	
	while(entry)
	{
		nextEntry = entry->next;
		XX_DelFromDLList(entry, p_Obj->AgingTable[maxAgeIndex]);
		XX_AddToDLList  (entry, p_Obj->AgingTable[p_Obj->TblMaxIndex + 1]);
		entry = nextEntry;
	}

	return E_OK;
}



static e_Err deleteEntryFromAgingList(void *p_Data, t_AgingEntry **AgingList)
{
   t_AgingEntry  *entry;

	/* Find the appropriate entry and remove it from the list */
	for(entry = *AgingList; entry; entry = entry->next)
	{
		if(entry->p_Data == p_Data)
		{
			XX_DelFromDLList(entry, *AgingList);
			XX_Free(entry);

			return E_OK;
		}
	}

	return E_NOT_FOUND;
}

/* --- end of file SPARO.C --- */



