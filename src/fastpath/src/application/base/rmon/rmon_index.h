#ifndef RMON_INDEX_H
#define RMON_INDEX_H
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename rmon_index.h
*
* @purpose  this is the header file for RMON utility to map SNMP index
*           to an array index.
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




#include <l7_common.h>
#include <log.h>

/* Maximum number of mapping tables supported by this library.
*/
#define RMON_MAX_MAP_TAB    10

/* SNMP/array mapping structure
*/
typedef struct rmonIndexMap_s
{
  L7_uint32 max_elements;   /* Maximum number of entries in this list. */
  L7_uint32 num_elements;   /* Current number of entries in this list. */
  L7_uint32 *array_index;   /* List of array indexes. */
  L7_uint32 *snmp_index;    /* List of SNMP indexes */

} rmonIndexMap_t;


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
L7_uint32 rmonMapCreate (L7_uint32 max_size);

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
L7_RC_t rmonMapInsert (L7_uint32 handle, L7_uint32 snmp_index);

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
L7_RC_t rmonMapRemove (L7_uint32 handle, L7_uint32 snmp_index);


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
                      L7_uint32 * array_index);


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
L7_RC_t rmonMapLowestGet (L7_uint32 handle, L7_uint32 * snmp_index);

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
L7_RC_t rmonMapHighestGet (L7_uint32 handle, L7_uint32 * snmp_index);

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
                        L7_uint32 *next_snmp_index);

/*********************************************************************
*
* @purpose    Get first available SNMP index in the table.
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
L7_RC_t rmonMapFirstAvailableIndexGet (L7_uint32 handle, L7_uint32 * first_available_snmp_index);

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
L7_RC_t rmonBinarySearch (L7_uint32 handle, L7_uint32 key, L7_int32 *pos);
#endif
