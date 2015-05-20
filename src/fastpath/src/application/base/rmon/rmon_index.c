/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename rmon_index.c
*
* @purpose  this is the core file for RMON index mapping utility.
*           
* @component SNMP-RMON
*
*
* @create  08/18/2001
*
* @author  Andrey Tsigler
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#include <stdio.h>
#include <rmon_index.h>
#include <stdlib.h>
#include "osapi.h"


/* Mapping tables.
*/
static rmonIndexMap_t rmonIndexMap[RMON_MAX_MAP_TAB];

/* Number of mapping tables that have been created.
*/
static L7_uint32 numMapTabs = 0;


/* Semaphore to protect the rmon index tables.
*/
static void * index_sem = 0;

/*********************************************************************
*
* @purpose    Create a mapping table.
*
* @param      size        (input) Maximum size of the mapping table 
*             
* @returntype map_id  Handle for the mapping table.
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_uint32 rmonMapCreate (L7_uint32 max_size)
{
  L7_uint32 handle;
  L7_uint32 i;

  if (index_sem == 0)
  {
    index_sem = osapiSemaBCreate (OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  }

  if (numMapTabs == RMON_MAX_MAP_TAB)
  {
    L7_LOG_ERROR (numMapTabs);
  };

  /* Allocate memory for table storage.
  */
  rmonIndexMap[numMapTabs].array_index = osapiMalloc (L7_RMON_COMPONENT_ID, max_size * sizeof (L7_uint32));
  if (rmonIndexMap[numMapTabs].array_index == 0)
  {
    L7_LOG_ERROR (0);
  }

  rmonIndexMap[numMapTabs].snmp_index = osapiMalloc (L7_RMON_COMPONENT_ID, max_size * sizeof (L7_uint32));
  if (rmonIndexMap[numMapTabs].snmp_index == 0)
  {
    L7_LOG_ERROR (0);
  }

  for (i = 0; i < max_size; i++)
  {
    /* The mapper assumes that RMON arrays are index from zero.
    */
    rmonIndexMap[numMapTabs].array_index[i] = i;

    /* To make debugging easier set unused SNMP indexes to 0xffffffff
    */
    rmonIndexMap[numMapTabs].snmp_index[i] = 0xffffffff;
  }

  rmonIndexMap[numMapTabs].max_elements = max_size;
  rmonIndexMap[numMapTabs].num_elements = 0;

  handle = numMapTabs;

  numMapTabs++;

  return handle;
}


/*********************************************************************
*
* @purpose    Add SNMP index to the table. The function returns 
*             an error if table is full.
*
* @param      handle      Table handle returned by rmonMapCreate
* @param      snmp_index  SNMP Index.
*             
* @returntype L7_SUCCESS - Index has been added to the table.
*             L7_ERROR   - Table is full.
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t rmonMapInsert (L7_uint32 handle, L7_uint32 snmp_index)
{
  L7_uint32 i, new_pos;
  rmonIndexMap_t *map;
  L7_uint32 free_array_index;
  L7_RC_t rc;

  if ((handle < 0) || (handle >= RMON_MAX_MAP_TAB) )
  {
    L7_LOG_ERROR (handle);
  }

  if(snmp_index < 1 || snmp_index >65535)
      return L7_ERROR;

  rc = osapiSemaTake (index_sem, L7_WAIT_FOREVER);
  if (rc != L7_SUCCESS)
  {
    L7_LOG_ERROR (rc);
  }

  map = &rmonIndexMap[handle];
  /* If table is full then return an error
  */
  if (map->num_elements == map->max_elements)
  {
    osapiSemaGive (index_sem);
    return L7_ERROR;
  }

  /* Get next free array index. 
  */
  free_array_index = map->array_index[map->num_elements];

  for (new_pos = 0; new_pos < map->num_elements; new_pos++)
  {
    /* All indexes should be unique or 0xffffffff. If we found a 
    ** duplicate then something is terribly wrong.
    */
    if (map->snmp_index[new_pos] == snmp_index)
    {
      L7_LOG_ERROR (snmp_index);
    }

    if (map->snmp_index[new_pos] > snmp_index)
      break;
  }

  /* Move indexes that are greater than current index to the right.
  */
  for (i = map->num_elements; i > new_pos; i--)
  {
    map->snmp_index[i] = map->snmp_index[i-1];
    map->array_index[i] = map->array_index[i-1];
  }

  /* Insert new index.
  */
  map->snmp_index[new_pos] = snmp_index;

  /* Assign new array index.
  */
  map->array_index[new_pos] = free_array_index;

  map->num_elements++;

  osapiSemaGive (index_sem);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose    Remove SNMP index from the table. The function returns 
*             an error if index is not found.
*
* @param      handle      Table handle returned by rmonMapCreate
* @param      snmp_index  SNMP Index.
*             
* @returntype L7_SUCCESS - Index has been removed from the table.
*             L7_ERROR   - Index is not found in the table.
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t rmonMapRemove (L7_uint32 handle, L7_uint32 snmp_index)
{
  L7_uint32 i, new_pos;
  rmonIndexMap_t *map;
  L7_uint32 free_array_index;
  L7_RC_t rc;

  if ((handle < 0) || (handle >= RMON_MAX_MAP_TAB) )
  {
    L7_LOG_ERROR (handle);
  }

  rc = osapiSemaTake (index_sem, L7_WAIT_FOREVER);
  if (rc != L7_SUCCESS)
  {
    L7_LOG_ERROR (rc);
  }

  map = &rmonIndexMap[handle];
  /* If table is full then return an error
  */
  if (map->num_elements == 0)
  {
    osapiSemaGive (index_sem);
    return L7_ERROR;
  }

  /* Find specified index.
  */
  for (new_pos = 0; new_pos < map->num_elements; new_pos++)
  {
    if (map->snmp_index[new_pos] == snmp_index)
      break;
  };

  /* If specified SNMP index is not found then return an error.
  */
  if (new_pos == map->num_elements)
  {
    osapiSemaGive (index_sem);
    return L7_ERROR;
  }

  /* Save current array index.
  */
  free_array_index = map->array_index[new_pos];

  /* Move indexes that are greater than current index to the left.
  */
  for (i = new_pos; i < (map->num_elements - 1);  i++)
  {
    map->snmp_index[i] = map->snmp_index[i+1];
    map->array_index[i] = map->array_index[i+1];
  }

  /* Store free array index at the end of the list.
  ** Set unused SNMP index to 0xffffffff
  */
  map->snmp_index [map->num_elements-1] = 0xffffffff;
  map->array_index [map->num_elements-1] = free_array_index;

  map->num_elements--;

  osapiSemaGive (index_sem);
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose    Return array index that corresponds to the SNMP index.
*
* @param      handle      Table handle returned by rmonMapCreate
* @param      snmp_index  SNMP Index.
* @param      array_index  (output) Matching array index.
*             
* @returntype L7_SUCCESS - Found matching array index.
*             L7_ERROR   - Specified SNMP index is not in the table.
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t rmonMapMatch (L7_uint32 handle, L7_uint32 snmp_index, 
                      L7_uint32 * array_index)
{
  L7_int32 pos;
  rmonIndexMap_t *map;

  if ((handle < 0) || (handle >= RMON_MAX_MAP_TAB) )
  {
    L7_LOG_ERROR (handle);
  }


  map = &rmonIndexMap[handle];
  
  if (map->num_elements == 0)
  {
    return L7_ERROR; /* List is empty */
  }


  /**** doing Binary Search *****/
  if(rmonBinarySearch(handle,snmp_index,&pos) != L7_FAILURE)
  {
      *array_index = map->array_index[pos];
      return L7_SUCCESS;
  }

  return L7_ERROR;
}


/*********************************************************************
*
* @purpose    Get lowest SNMP index in the table.
*
* @param      handle      Table handle returned by rmonMapCreate
* @param      (output) snmp_index  SNMP Index.
*             
* @returntype L7_SUCCESS - Index found.
*             L7_ERROR   - Table is empty.
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t rmonMapLowestGet (L7_uint32 handle, L7_uint32 * snmp_index)
{
  rmonIndexMap_t *map;

  if ((handle < 0) || (handle >= RMON_MAX_MAP_TAB) )
  {
    L7_LOG_ERROR (handle);
  }

  map = &rmonIndexMap[handle];

  if (map->num_elements == 0)
  {
    return L7_ERROR; /* List is empty */
  }


  *snmp_index = map->snmp_index[0];

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose    Get highest SNMP index in the table.
*
* @param      handle      Table handle returned by rmonMapCreate
* @param      (output) snmp_index  SNMP Index.
*             
* @returntype L7_SUCCESS - Index found.
*             L7_ERROR   - Table is empty.
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t rmonMapHighestGet (L7_uint32 handle, L7_uint32 * snmp_index)
{
  rmonIndexMap_t *map;

  if ((handle < 0) || (handle >= RMON_MAX_MAP_TAB) )
  {
    L7_LOG_ERROR (handle);
  }

  map = &rmonIndexMap[handle];

  if (map->num_elements == 0)
  {
    return L7_ERROR; /* List is empty */
  }


  *snmp_index = map->snmp_index[map->num_elements - 1];

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose    Get next SNMP index in the table.
*
* @param      handle      Table handle returned by rmonMapCreate
* @param      snmp_index  SNMP Index.
* @param      next_snmp_index  (output) next SNMP Index.
*             
* @returntype L7_SUCCESS - Index found.
*             L7_ERROR   - Next index is higher than largest index 
*                          in the table.
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t rmonMapNextGet (L7_uint32 handle, L7_uint32 snmp_index,
                        L7_uint32 *next_snmp_index)
{
  L7_int32 pos;
  rmonIndexMap_t *map;

  if ((handle < 0) || (handle >= RMON_MAX_MAP_TAB) )
  {
    L7_LOG_ERROR (handle);
  }

  map = &rmonIndexMap[handle];

  if (map->num_elements == 0)
  {
    return L7_ERROR; /* List is empty */
  }

   /**** doing Binary Search *****/
  rmonBinarySearch(handle,snmp_index,&pos);

  if(pos == -1 || pos < (map->num_elements-1))
  {
     *next_snmp_index = map->snmp_index[pos+1];
     return L7_SUCCESS;
  }else
     return L7_ERROR;
 
}

/*********************************************************************
*
* @purpose    Get first available SNMP index in the table.
*
* @param      handle      Table handle returned by rmonMapCreate
* @param      (output) first_available_snmp_index  SNMP Index.
*             
* @returntype L7_SUCCESS - Index found.
*             L7_ERROR   - Table is empty.
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t rmonMapFirstAvailableIndexGet (L7_uint32 handle, L7_uint32 * first_available_snmp_index)
{
    L7_uint32 i = 0;
    L7_BOOL boolBreak = L7_FALSE;
    rmonIndexMap_t *map;
    L7_RC_t rc;

    if ((handle < 0) || (handle >= RMON_MAX_MAP_TAB) )
    {
        L7_LOG_ERROR (handle);
    }

    map = &rmonIndexMap[handle];

    rc = osapiSemaTake (index_sem, L7_WAIT_FOREVER);
    if (rc != L7_SUCCESS)
    {
      L7_LOG_ERROR (rc);
    }

    /* If table is full then return an error
    */
    if (map->num_elements == map->max_elements)
    {
        osapiSemaGive (index_sem);
        return L7_ERROR;
    }

    /* If the list is empty or if the first found snmp index is more than one, 
     * then 1 can be used as first available snmp index.
     */ 
    if ((map->num_elements == 0) ||
        (map->snmp_index[i] > 1))
    {
        *first_available_snmp_index = 1;
        osapiSemaGive (index_sem);
        return L7_SUCCESS;
    }
      
  
    for (i = 0; i < map->num_elements; i++ )
    {
        if ((map->snmp_index[i+1] - map->snmp_index[i]) > 1)
        {
            boolBreak = L7_TRUE;
            break;
        }
     
    }


    if(boolBreak == L7_TRUE)
    {
        *first_available_snmp_index = map->snmp_index[i]+1;
        osapiSemaGive (index_sem);
        return L7_SUCCESS;

    }
       

    if(boolBreak == L7_FALSE && i > 0)
    {
        *first_available_snmp_index = map->snmp_index[i-1]+1;
        osapiSemaGive (index_sem);
        return L7_SUCCESS;

    }
      
    osapiSemaGive (index_sem);
    return L7_ERROR; /* no index is available in the range of 1 to 65535 */
}


/*********************************************************************
*
* @purpose    Implements Binary Search algorithm for searching SNMP index in the table.
*
* @param      handle      Table handle returned by rmonMapCreate
* @param      key         variable to be searched
* @param      pos(output parameter)     position of the key in the array
*             
* @returntype L7_SUCCESS - Index found.
*             L7_ERROR   - Table is empty.
*             L7_FAILURE - Index not found.
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t rmonBinarySearch (L7_uint32 handle, L7_uint32 key, L7_int32 *pos)
{
    L7_uint32 bottom, top, mid;
    
    rmonIndexMap_t *map;

    if ((handle < 0) || (handle >= RMON_MAX_MAP_TAB) )
    {
        L7_LOG_ERROR (handle);
    }

    map = &rmonIndexMap[handle];

    if (map->num_elements == 0)
    {
        return L7_ERROR; /* List is empty */
    }
    
    bottom = 0;
    top = (map->num_elements)-1;

    while(bottom<=top)
    {
        mid = (top+bottom)/2;

        if((mid == 0) && (map->snmp_index[mid] > key))
        {
            *pos = -1;
            return L7_FAILURE;
        }
            

        if(map->snmp_index[mid] == key)
        {
            *pos = mid;
            return L7_SUCCESS;

        }
        else if(map->snmp_index[mid] < key)
            bottom = mid+1;
        else
            top = mid-1;
     
    }/* end of while loop */
    *pos = top;
    return L7_FAILURE;
}/* end of binary search function */




/********************************************************
** Test Code.
*********************************************************/
void rmonMapTest1 (void)
{
  L7_uint32 alarmTab, eventTab;
  L7_RC_t rc;
  L7_uint32 snmp_index, array_index, pos;
  L7_uint32 next_snmp_index;

  alarmTab = rmonMapCreate(10);
  eventTab = rmonMapCreate(20);

  printf("alarmTab= %d, eventTab = %d\n", 
         alarmTab, eventTab);

  snmp_index = 200;
  rc = rmonMapInsert (alarmTab, snmp_index);
  printf("rmonMapInsert, snmp_index = %d, rc = %d\n", 
         snmp_index, rc);

  snmp_index = 100;
  rc = rmonMapInsert (alarmTab, snmp_index);
  printf("rmonMapInsert, snmp_index = %d, rc = %d\n", 
         snmp_index, rc);


  snmp_index = 300;
  rc = rmonMapInsert (alarmTab, snmp_index);
  printf("rmonMapInsert, snmp_index = %d, rc = %d\n", 
         snmp_index, rc);

  snmp_index = 400;
  rc = rmonMapInsert (alarmTab, snmp_index);
  printf("rmonMapInsert, snmp_index = %d, rc = %d\n", 
         snmp_index, rc);

  snmp_index = 500;
  rc = rmonMapInsert (alarmTab, snmp_index);
  printf("rmonMapInsert, snmp_index = %d, rc = %d\n", 
         snmp_index, rc);

  snmp_index = 600;
  rc = rmonMapInsert (alarmTab, snmp_index);
  printf("rmonMapInsert, snmp_index = %d, rc = %d\n", 
         snmp_index, rc);

  snmp_index = 1;
  rc = rmonMapInsert (alarmTab, snmp_index);
  printf("rmonMapInsert, snmp_index = %d, rc = %d\n", 
         snmp_index, rc);


  snmp_index = 800;
  rc = rmonMapInsert (alarmTab, snmp_index);
  printf("rmonMapInsert, snmp_index = %d, rc = %d\n", 
         snmp_index, rc);

  rc = rmonBinarySearch(alarmTab, 1, &pos);
  printf("rmonBinarySearch, 100, pos = %d, rc = %d\n",
         pos,rc);

  rc = rmonBinarySearch(alarmTab, 100, &pos);
  printf("rmonBinarySearch, 100, pos = %d, rc = %d\n",
         pos,rc);
   rc = rmonBinarySearch(alarmTab, 200, &pos);
  printf("rmonBinarySearch, 200, pos = %d, rc = %d\n",
         pos,rc);
   rc = rmonBinarySearch(alarmTab, 300, &pos);
  printf("rmonBinarySearch, 300, pos = %d, rc = %d\n",
         pos,rc);
   rc = rmonBinarySearch(alarmTab, 400, &pos);
  printf("rmonBinarySearch, 400, pos = %d, rc = %d\n",
         pos,rc);
   rc = rmonBinarySearch(alarmTab, 500, &pos);
  printf("rmonBinarySearch, 500, pos = %d, rc = %d\n",
         pos,rc);
   rc = rmonBinarySearch(alarmTab, 600, &pos);
  printf("rmonBinarySearch, 600, pos = %d, rc = %d\n",
         pos,rc);
  rc = rmonBinarySearch(alarmTab, 800, &pos);
  printf("rmonBinarySearch, 800, pos = %d, rc = %d\n",
        pos,rc);
    

  snmp_index = 1;
  rc = rmonMapRemove (alarmTab, snmp_index);
  printf("rmonMapRemove, snmp_index = %d, rc = %d\n", 
         snmp_index, rc);

  rc = rmonBinarySearch(alarmTab, 1, &pos);
  printf("rmonBinarySearch, 100, pos = %d, rc = %d\n",
         pos,rc);

  snmp_index = 100;
  rc = rmonMapRemove (alarmTab, snmp_index);
  printf("rmonMapRemove, snmp_index = %d, rc = %d\n", 
         snmp_index, rc);


  rc = rmonBinarySearch(alarmTab, 100, &pos);
  printf("rmonBinarySearch, 100, pos = %d, rc = %d\n",
         pos,rc);

  snmp_index = 500;
  rc = rmonMapInsert (alarmTab, snmp_index);
  printf("rmonMapInsert, snmp_index = %d, rc = %d\n", 
         snmp_index, rc);


  /* Find array index for specified SNMP index.
  */
  snmp_index = 500;
  rc = rmonMapMatch (alarmTab, snmp_index, &array_index);
  printf("rmonMapInsert, snmp_index = %d,  array_index = %d, rc = %d\n", 
         snmp_index, array_index, rc);

  snmp_index = 200;
  rc = rmonMapMatch (alarmTab, snmp_index, &array_index);
  printf("rmonMapInsert, snmp_index = %d,  array_index = %d, rc = %d\n", 
         snmp_index, array_index, rc);


  snmp_index = 300;
  rc = rmonMapMatch (alarmTab, snmp_index, &array_index);
  printf("rmonMapInsert, snmp_index = %d,  array_index = %d, rc = %d\n", 
         snmp_index, array_index, rc);


  snmp_index = 600;
  rc = rmonMapMatch (alarmTab, snmp_index, &array_index);
  printf("rmonMapInsert, snmp_index = %d,  array_index = %d, rc = %d\n", 
         snmp_index, array_index, rc);


  /* Get lowest Index
  */
  rc = rmonMapLowestGet (alarmTab, &snmp_index);
  printf("rmonMapLowestGet, snmp_index = %d, rc = %d\n", 
         snmp_index, rc);


  /* Get Highest Index
 */
  rc = rmonMapHighestGet (alarmTab, &snmp_index);
  printf("rmonMapLowestGet, snmp_index = %d, rc = %d\n", 
         snmp_index, rc);


  /* Get Next test.
  */
  snmp_index = 1;
  rc = rmonMapNextGet (alarmTab, snmp_index, &next_snmp_index);
  printf("rmonNextGet, snmp_index = %d, next_snmp_index = %d, rc = %d\n",
         snmp_index, next_snmp_index, rc);

  snmp_index = next_snmp_index;
  rc = rmonMapNextGet (alarmTab, snmp_index, &next_snmp_index);
  printf("rmonNextGet, snmp_index = %d, next_snmp_index = %d, rc = %d\n",
         snmp_index, next_snmp_index, rc);


  snmp_index = next_snmp_index;
  rc = rmonMapNextGet (alarmTab, snmp_index, &next_snmp_index);
  printf("rmonNextGet, snmp_index = %d, next_snmp_index = %d, rc = %d\n",
         snmp_index, next_snmp_index, rc);

  snmp_index = next_snmp_index;
  rc = rmonMapNextGet (alarmTab, snmp_index, &next_snmp_index);
  printf("rmonNextGet, snmp_index = %d, next_snmp_index = %d, rc = %d\n",
         snmp_index, next_snmp_index, rc);


}

/* Need to test boundary conditions for all functions.......
*/


