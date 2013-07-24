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
 
extern DOT1S_PORT_COMMON_t			*dot1sPort;
L7_uint32 portIndexDebug = 0;

L7_uint32 dot1sPortIndexFromIntfNumGet(L7_uint32 intIfNum)
{
  L7_uint32 portIndex = L7_NULL;
  L7_uint32 minLag, maxLag;

  if (nimIntIfNumRangeGet(L7_LAG_INTF,&minLag, &maxLag) != L7_SUCCESS)
  {
    return L7_NULL;
  }


  if (intIfNum <= L7_MAX_PORT_COUNT)
  {
    portIndex = intIfNum;

  }
  else if ((intIfNum >= minLag) &&
           (intIfNum <= maxLag))
  {
    portIndex = L7_MAX_PORT_COUNT + (intIfNum - minLag + 1);
  }

  

  return portIndex;
}

L7_uint32 dot1sIntIfNumFromPortIndexGet(L7_uint32 portIndex)
{
  L7_uint32 minLag, maxLag;
  L7_uint32 intIfNum = L7_NULL;

  if (nimIntIfNumRangeGet(L7_LAG_INTF,&minLag, &maxLag) != L7_SUCCESS)
  {
    return L7_NULL;
  }

  if (portIndex >= L7_DOT1S_MAX_INTERFACE_COUNT)
  {
    return L7_NULL;
  }

  if (portIndex <= L7_MAX_PORT_COUNT)
  {
    intIfNum = portIndex;
  }
  else
  {
    intIfNum = (portIndex - L7_MAX_PORT_COUNT) + (minLag - 1);
  }

  return intIfNum;

}
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
  L7_uint32 portIndex = dot1sPortIndexFromIntfNumGet(intIfNum);

  
  if (portIndex != L7_NULL )
  {
    return &dot1sPort[portIndex];
  }

  return L7_NULLPTR;
}

L7_uint32 dot1sPortIndexFindFirst()
{
  L7_uint32 index = 1;

  while ((index < L7_DOT1S_MAX_INTERFACE_COUNT) &&
         (dot1sPort[index].portNum == 0))
  {
    index++;
  }
  
  if (index < L7_DOT1S_MAX_INTERFACE_COUNT)
  {
    return index;
  }

  return L7_NULL;

}

L7_uint32 dot1sPortIndexFindNext(L7_uint32 prevIndex)
{
  
  prevIndex++;
  while ((prevIndex < L7_DOT1S_MAX_INTERFACE_COUNT) &&
         (dot1sPort[prevIndex].portNum == 0))
  {
    prevIndex++;
  }
  
  if (prevIndex < L7_DOT1S_MAX_INTERFACE_COUNT)
  {
    return prevIndex;
  }

  return L7_NULL;
}

/*********************************************************************
* @purpose 	Finds the first dot1s interface record 
*
* @returns  DOT1S_PORT_COMMON_t *	Pointer to the record  
*
* @comments
*
* @end
*********************************************************************/
DOT1S_PORT_COMMON_t *dot1sIntfFindFirst()
{
  L7_uint32 index =  dot1sPortIndexFindFirst();

  if (index != L7_NULL)
  {
    return &dot1sPort[index];
  }

  return L7_NULLPTR;

}

/*********************************************************************
* @purpose 	Finds the first dot1s interface record 
*
* @returns  DOT1S_PORT_COMMON_t *	Pointer to the record  
*
* @comments
*
* @end
*********************************************************************/
DOT1S_PORT_COMMON_t *dot1sIntfFindNext(L7_uint32 prevIntIfNum)
{
  L7_uint32  prevPortIndex;
  L7_uint32 index;
  prevPortIndex = dot1sPortIndexFromIntfNumGet(prevIntIfNum);

  index = dot1sPortIndexFindNext(prevPortIndex);
  if (index != L7_NULL)
  {
    return &dot1sPort[index];
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

L7_uint32 dot1sGetPortTransmitIntervalInSec(L7_uint32 intIfNum)
{
  DOT1S_PORT_COMMON_t *p  = dot1sIntfFind(intIfNum);

  if (p != L7_NULLPTR && p->portNum != L7_NULL)
  {
    return (p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistPortTimes.helloTime/DOT1S_TIMER_UNIT);
  }

  return L7_NULL;
}


void dot1sIntfTest()
{
  portIndexDebug = 1;
  DOT1S_PORT_COMMON_t *p;
  printf("MAx interface count %d \t Max Port count %d \t L7_MAX_NUM_LAG_INTF %d\n",
         L7_MAX_INTERFACE_COUNT,L7_MAX_PORT_COUNT,L7_MAX_NUM_LAG_INTF);

  printf("%s: Enter \n", __FUNCTION__);
  p = dot1sIntfFindFirst();
  while (p != L7_NULLPTR)
  {
     printf("%s :  p->portNum %d \n", __FUNCTION__, p->portNum);
     p = dot1sIntfFindNext(p->portNum);
  }


  printf("%s: Exit \n", __FUNCTION__);
}

void dot1sIntfNumTest()
{
  L7_uint32 i, portIndex;
  printf("%s: Enter \n", __FUNCTION__);

  for (i = 0; i < L7_MAX_INTERFACE_COUNT; i++)
  {
    portIndex = dot1sPortIndexFromIntfNumGet(i);
    printf("%s :  IntIfNum %d portIndex %d\n", __FUNCTION__, i, portIndex);
  }


  printf("%s: Exit \n", __FUNCTION__);
}

void dot1sPortIndexTest()
{
  L7_uint32 i, intIfNum;
  printf("%s: Enter \n", __FUNCTION__);

  for (i = 0; i < L7_MAX_INTERFACE_COUNT; i++)
  {
    intIfNum = dot1sIntIfNumFromPortIndexGet(i);
    printf("%s : portIndex  %d intIfNum %d\n", __FUNCTION__, i, intIfNum);
  }


  printf("%s: Exit \n", __FUNCTION__);
}
