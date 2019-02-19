/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename     da.c
 *
 * @purpose      Implementation of Dynamic Array (DA) object
 *
 * @component    Routing Utils Component
 *
 * @comments
 *   ExternalRoutines:
 *
 * DA_Construct
 * DA_Destruct
 * DA_Insert
 * DA_Get
 * DA_Delete
 * DA_Browse
 * DA_GetEntriesNmb
 *
 *
 * @create         08/09/1998
 *
 * @author
 *
 * @end
 *
 *********************************************************************/
#ifdef FILEID_IN_MEMORY
static char __fileid[] = "$Header: /home/cvs/PR1003/PR1003/FASTPATH/RELEASE_610_FP_6.1.0.5-FastPath-Ent-esw-xgs3-bmw-LR-CSxw-IQH/src/system_support/routing/utils/da.c,v 1.1 2011/04/18 17:10:53 mruas Exp $";
#endif


#include <string.h>
#include "std.h"
#include "xx.ext"
#include "da.ext"

/* DA control block */
typedef struct t_DAH
{
#ifdef ERRCHK
   XX_Status  status;        /* Current status XX_STAT_Valid                   */
#endif
   word       clustSize;     /* Size of DA cluster - memory in void *
                                to be added to DA when it runs out of
                                allocated memory   */
   word       entryNmb;      /* Number of entries in DA                        */
   word       daSize;        /* Currently allocated memory for DA in void *    */
   void       **array;       /* Pointer to DA contents                         */
   t_Handle   mutex;         /* DA object mutex                                */
}t_DAH;



/*********************************************************************
 * @purpose            Initialize DA object
 *
 *
 * @param clustSize    @b{(input)}   cluster size
 * @param p_da         @b{(output)}  pointer to return handle of successfully
 *                                   created DA object
 *
 * @returns            E_OK        success
 * @returns            E_NOMEMORY  out of memory
 * @returns            E_BADPARM   invalid parameter(s)
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err DA_Construct( IN word clustSize,
                    OUT t_Handle *p_da)
{
   t_DAH *p_H;
   ASSERT(clustSize);
   
   /* Allocate DA control block */
   if((p_H = XX_Malloc(sizeof(t_DAH))) == NULLP)
      return E_NOMEMORY;

   memset (p_H, 0, sizeof(t_DAH));
   if((p_H->array = XX_Malloc(p_H->daSize = p_H->clustSize = clustSize)) == NULLP)
   {
	   XX_Free(p_H);
	   return E_NOMEMORY;
   }
   
   XX_CreateMutex(&p_H->mutex);
#ifdef ERRCHK
   p_H->status = XX_STAT_Valid;
#endif
   *p_da = p_H;
   return E_OK;
}


/*********************************************************************
 * @purpose         Destroy DA object
 *
 *
 * @param p_da      @b{(input)}  pointer to DA object handle to be
 *                               destroyd
 *
 * @return          E_OK        success
 * @returns         E_BADPARM   invalid handle
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err DA_Destruct ( IN OUT t_Handle *p_da)
{
    t_DAH *p_H;
	ASSERT(p_da);
    p_H = (t_DAH *)*p_da;
    ASSERT(p_H);
    ASSERT(p_H->status == XX_STAT_Valid);

    XX_DeleteMutex(&p_H->mutex);

    XX_Free(p_H->array);
#ifdef ERRCHK
    p_H->status = XX_STAT_Invalid;
#endif
    XX_Free(p_H);
    *p_da = NULLP;
    return E_OK;
}


/*********************************************************************
 * @purpose        Insert entry into DA object
 *
 *
 * @param  da      @b{(input)} handle of DA object
 * @param  entry   @b{(input)} entry to be inserted
 * @param  index   @b{(input)} index in DA where entry is to be inserted
 *
 * @returns        E_OK       success
 * @returns        E_NOMEMORY out of memory
 * @returns        E_BADPARM  invalid parameters
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err DA_Insert( IN t_Handle da, IN void *entry, IN word index)
{
    t_DAH   *p_H = (t_DAH *)da;

    ASSERT(p_H);
    ASSERT(p_H->status == XX_STAT_Valid);

	if(index > p_H->entryNmb)
		index = p_H->entryNmb;

    XX_Lock(p_H->mutex);

	if(index < p_H->daSize-1)
	{
		/* We are still within current cluster */
		if(p_H->entryNmb - index > 0)
			memmove(&p_H->array[index+1], &p_H->array[index], 
			         (p_H->entryNmb - index)*sizeof(void *));
	}
	else
	{
		/* Reallocate memory for the array increasing it by one cluster */
		void **temp = p_H->array;
        if((p_H->array = XX_Malloc(p_H->daSize+p_H->clustSize))==NULLP)
		{
			p_H->array = temp;
		    XX_Unlock(p_H->mutex);
			return E_NOMEMORY;
		}
		if(index)
		   memcpy(&p_H->array[0], &temp[0], (index-1)*sizeof(void *));
		if(p_H->entryNmb - index > 0)
			memcpy(&p_H->array[index+1], &temp[index],
			         (p_H->entryNmb - index)*sizeof(void *));
		XX_Free(temp);
		p_H->daSize+=p_H->clustSize;
	}

	p_H->array[index] = entry;
	p_H->entryNmb++;
    XX_Unlock(p_H->mutex);
	return E_OK;
}

/*********************************************************************
 * @purpose         Get entry from DA object
 *
 *
 * @param  da       @b{(input)}  DA object handle
 * @param  p_Entry  @b{(output)} pointer to return entry
 * @param  index    @b{(input)}  index in DA where entry is stored
 *
 * @returns         E_OK        success
 * @returns         E_BADPARM   invalid parameters
 * @returns         E_FAILED    entry wasn't found
 *
 * @notes
 *
 * @end
 *
 * ********************************************************************/
e_Err DA_Get( IN t_Handle da, OUT void **p_Entry, IN word index)
{
    t_DAH   *p_H = (t_DAH *)da;

    ASSERT(p_H);
    ASSERT(p_H->status == XX_STAT_Valid);
	ASSERT(p_Entry);

	if(index>=p_H->entryNmb)
		return E_FAILED;

	*p_Entry = p_H->array[index];
	return E_OK;
}


/*********************************************************************
 * @purpose         Delete entry from DA object
 *
 *
 * @param  da       @b{(input)}  DA object handle
 * @param  p_Entry  @b{(output)} pointer to return entry
 * @param  index    @b{(input)}  index in DA where entry is stored
 *
 *
 * @returns        E_OK        success
 * @returns        E_BADPARM   invalid parameters
 * @returns        E_FAILED    entry wasn't found
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err DA_Delete( IN t_Handle da, IN word index)
{
    t_DAH   *p_H = (t_DAH *)da;

    ASSERT(p_H);
    ASSERT(p_H->status == XX_STAT_Valid);

	if(index>=p_H->entryNmb)
		return E_FAILED;

   XX_Lock(p_H->mutex);
	if(p_H->entryNmb-1>=0)
	{
	   if(index != p_H->entryNmb -1)
		   memmove(&p_H->array[index], &p_H->array[index+1],
                        (p_H->entryNmb-(index+1))*sizeof(void *));
       p_H->entryNmb--;
	}
    XX_Unlock(p_H->mutex);
    return E_OK;
}

/*********************************************************************
 * @purpose           Browse entire DA, calling  browse funcion for every
 *                    entry .
 *
 *
 * @param  da         @b{(input)}    DA object handle
 * @param  f_browse   @b{(input)}    pointer to user defined browse function
 * @param  parameter  @b{(input)}    parameter to be passed to browse function
 *
 * @returns           E_OK        success
 * @returns           E_BADPARM  wrong parameters
 *
 * @notes             If browse funcion for some entry returns FALSE,
 *                    delete the entry
 *
 * @end
 * ********************************************************************/
e_Err DA_Browse( IN t_Handle da,  IN DA_BROWSEFUNC f_browse, IN ulng param)
{
	word i;
    t_DAH   *p_H = (t_DAH *)da;
    ASSERT(p_H);
    ASSERT(p_H->status == XX_STAT_Valid);
    ASSERT(f_browse);

	for(i=0; i<p_H->entryNmb;)
	{    
       if(f_browse(p_H->array[i], param) == FALSE)
       {
            /* DA was advised to delete entry */
			XX_Lock(p_H->mutex);
			if(p_H->entryNmb-1>0)
			{
			   if(i != p_H->entryNmb -1)
				   memmove(&p_H->array[i], &p_H->array[i+1],
								(p_H->entryNmb-(i+1))*sizeof(void *));
			   p_H->entryNmb--;
			}
			XX_Unlock(p_H->mutex);
			continue;
	   }
	   else
		   i++;
	}
    return E_OK;
}


/*********************************************************************
 * @purpose              Returns number of entries in  DA object
 *
 *
 * @param da             @b{(input)}  DA object handle
 * @param p_entriesNmb   @b{(output)}  pointer to return DA entries number
 *
 * @returns              E_OK        success
 * @returns              E_BADPARM   wrong parameters
 *
 * @notes
 *
 * @end
 * ********************************************************************/

e_Err DA_GetEntriesNmb(t_Handle da, word *p_entriesNmb)
{
    t_DAH   *p_H = (t_DAH *)da;

    ASSERT(p_H);
    ASSERT(p_H->status == XX_STAT_Valid);

    *p_entriesNmb = p_H->entryNmb;
    return E_OK;
}

