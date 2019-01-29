/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2009
*
**********************************************************************
*
* @filename   k_mib_fastpath_llpf_api.c
*
* @purpose    Wrapper functions for LLPF  MIB
*
* @component  SNMP
*
* @comments
*
* @create     10/28/2009
*
* @author     Vijayanand K(kvijayan)
* @end
*
**********************************************************************/
#include "k_private_base.h"

#include "commdefs.h"

#include "k_mib_fastpath_llpf_api.h"
#include "usmdb_llpf_api.h"
#include "llpf_exports.h"
#include "usmdb_util_api.h"

/*********************************************************************
*
* @purpose  
*
* @param    
* @param    
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
* @notes
*
* @end
*
*********************************************************************/

L7_RC_t
snmpLlpfIntfProtocolModeGet( L7_uint32 intIfNum,
                            L7_LLPF_BLOCK_TYPE_t protocolType, L7_uint32 *blockMode)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 tempBlockMode = 0;

  rc = usmDbLlpfIntfBlockModeGet(intIfNum,protocolType, &tempBlockMode);

  if (rc == L7_SUCCESS)
  {
    switch (tempBlockMode)
    {

    case L7_TRUE:
      *blockMode = D_agentSwitchLlpfPortBlockMode_enable;
      break;

    case L7_FALSE:
      *blockMode = D_agentSwitchLlpfPortBlockMode_disable;
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}
/*********************************************************************
*
* @purpose  Set the owner string for the Receiver
*
* @param    UnitIndex @b((input)) the unit for this operation
* @param    index @b((input))     Index value
* @param    buffer @b((input))   owner string of the index
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
* @notes
*
* @end
*
*********************************************************************/

L7_RC_t
snmpLlpfIntfProtocolModeSet( L7_uint32 intIfNum,
                            L7_LLPF_BLOCK_TYPE_t protocolType, L7_uint32 blockMode)
{

  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 blockModeSet = 0;

  switch (blockMode)
  {
    case D_agentSwitchLlpfPortBlockMode_enable:
        blockModeSet= L7_TRUE;
      break;

    case D_agentSwitchLlpfPortBlockMode_disable:
        blockModeSet= L7_FALSE;
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
  }

  if(rc == L7_SUCCESS)
    rc = usmDbLlpfIntfBlockModeSet (intIfNum,protocolType, blockModeSet);
  return rc;
}

/*********************************************************************
*
* @purpose Used to get the values of protocol index and interface number. 
*
* @param    extIfNum 
* @param    protoIndex
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t
snmpLlpfIntfGet(L7_uint32 extIfNum,L7_uint32 protoIndex)
{
  L7_uint32 intIfNum;
  if (usmDbIntIfNumFromExtIfNum(extIfNum, &intIfNum) == L7_SUCCESS)
  {
     if ((usmDbLlpfIntfIsValid(intIfNum) != L7_TRUE) ||
        (protoIndex > L7_LLPF_BLOCK_TYPE_ALL))
        return L7_FAILURE;
     else
        return L7_SUCCESS;
  }

  return L7_FAILURE;
}
/*********************************************************************
*
* @purpose  
*
* @param  extIfNum
* @param  *nextExtIfNum
* @param  protoIndex
* @param  *nxtProtoIndex
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
* @notes
*
* @end
*
*********************************************************************/

L7_RC_t 
snmpLlpfIntfNextGet(L7_uint32 extIfNum, L7_uint32 *nextExtIfNum,
                    L7_uint32 protoIndex,L7_uint32 *nxtProtoIndex)
{
  L7_uint32 intIfNum;
  
  if((nextExtIfNum == NULL) ||(nxtProtoIndex == NULL))
  {
    return L7_FAILURE;
  }
  
  if( usmDbGetNextVisibleExtIfNumber(extIfNum,&extIfNum)  == L7_SUCCESS)
  {
    /* check to see if this is a visible external interface number */
    if (usmDbIntIfNumFromExtIfNum(extIfNum, &intIfNum) == L7_SUCCESS)
    {
      do
      {
        if (usmDbLlpfIntfIsValid(intIfNum) == L7_TRUE)
        {
          if (protoIndex < L7_LLPF_BLOCK_TYPE_ALL) 
          {
             /* increment the protocol Index */
             *nxtProtoIndex = (protoIndex +1);
          }
          else
          {
             *nxtProtoIndex = 0;
          }
          /* Convert internal to external interface number */ 
          if( usmDbExtIfNumFromIntIfNum(intIfNum,nextExtIfNum) != L7_SUCCESS)
          {
            return L7_FAILURE;
          }
          else
          {
            return L7_SUCCESS;
          }
        }
      }while ((L7_SUCCESS == usmDbValidIntIfNumNext(intIfNum, &intIfNum)));
    }
    else
    {
       /* Set the nxt ext intf Num and get set the proto Index for */
       return L7_FAILURE;
    }
  } 
 return L7_SUCCESS;
}

