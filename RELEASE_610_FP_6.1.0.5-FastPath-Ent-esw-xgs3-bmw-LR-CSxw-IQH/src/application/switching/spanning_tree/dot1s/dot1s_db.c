/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  dot1s_db.c
*
* @purpose   Multiple Spanning tree database file
*
* @component dot1s
*
* @comments 
*
* @create    10/14/2002
*
* @author    skalyanam
*
* @end
*             
**********************************************************************/
#include "dot1s_include.h"
 
/*********************************************************************
* @purpose 	Finds the interface record 
*
* @param    intIfNum	@b{(input)} Internal Interface Number   
*
* @returns  DOT1S_PORT_COMMON_t *	Pointer to the record  
* @returns  L7_NULLPTR 				Interface not Found 
*
* @comments
*
* @end
*********************************************************************/
DOT1S_PORT_COMMON_t *dot1sIntfFind(L7_uint32 intIfNum)
{
  if (intIfNum <= ((L7_uint32)L7_MAX_INTERFACE_COUNT))
  {
    return &dot1sPort[intIfNum];
  }

  return L7_NULLPTR;
}
/*********************************************************************
* @purpose 	Finds the index associated with a particular instance number
*
* @param    instNumber	@b{(input)}  Instance Number  
* @param    *index		@b{(output)} pointer to the index 
*
* @returns  L7_SUCCESS 	instance number is found and inUse is L7_TRUE  
* @returns  L7_FAILURE 	instance number is not found also 
*						*index is returned as L7_NULL 
*
* @comments The inputed insatance number is the number that the user 
*       	assigns when creating the instance. The returned index is 
*			the index into the instance index mapping array
*
* @end
*********************************************************************/
L7_RC_t	dot1sInstIndexFind(L7_uint32 instNumber, L7_uint32	*index)
{
  L7_uint32 i;

  for (i = 0; i <= L7_MAX_MULTIPLE_STP_INSTANCES; i++)
  {
    if ((dot1sInstanceMap[i].instNumber == instNumber) &&
        (dot1sInstanceMap[i].inUse == L7_TRUE))
    {
      *index = i;
      return L7_SUCCESS;
    }
  }

  *index = L7_NULL;

  return L7_FAILURE;
}
/*********************************************************************
* @purpose 	Finds the user assigned instance number given the instance index
*
* @param    index		@b{(input)}   Index into the instance mapping array
* @param    *instNumber	@b{(output)}  value of the instance Number
*
* @returns  L7_SUCCESS	inUse is L7_TRUE 
* @returns  L7_FAILURE	inUse is L7_FALSE or 
*						index is greater than L7_MAX_MULTIPLE_INSTANCES
*
* @comments *instNumber is set to L7_NULL if L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t	dot1sInstNumFind(L7_uint32 index, L7_uint32 *instNumber)
{
  if ((index <= (L7_MAX_MULTIPLE_STP_INSTANCES)) && 
	  (index >= 0) && 
	  dot1sInstanceMap[index].inUse == L7_TRUE)
  {
    *instNumber = dot1sInstanceMap[index].instNumber; 

    return L7_SUCCESS;
  }

  *instNumber = L7_NULL;

  return L7_FAILURE;
}
/*********************************************************************
* @purpose 	Finds the bridge parameter structure
*
* @param      
*
* @returns  Valid structure pointer on a successful find 
* @returns  L7_NULLPTR on any other failure 
*
* @comments
*
* @end
*********************************************************************/
DOT1S_BRIDGE_t *dot1sBridgeParmsFind()
{
  return dot1sBridge;
}
/*********************************************************************
* @purpose 	Finds the Instance structure
*
* @param      
*
* @returns  Valid structure pointer on a successful find 
* @returns  L7_NULLPTR on any other failure 
*
* @comments
*
* @end
*********************************************************************/
DOT1S_INSTANCE_INFO_t *dot1sInstFind()
{
  return dot1sInstance;
}
