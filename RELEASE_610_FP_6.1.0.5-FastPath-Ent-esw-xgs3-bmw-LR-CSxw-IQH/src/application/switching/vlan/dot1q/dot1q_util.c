/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename dot1q_util.c
*
* @purpose 802.1Q Utilities File
*
* @component dot1Q
*
* @comments none
*
* @create 09/15/2000
*
* @author w. jacobs
*
* @end
*             
**********************************************************************/

#include <string.h>
#include "flex.h"
#include "dot1q_common.h"
#include "l7_product.h"
#include "nimapi.h"
#include "log.h"
#include "dot1q_intf.h"
#include "dot1q_cnfgr.h"
   

extern  dot1qIntfData_t     *dot1qQports;
/*********************************************************************
* @purpose  Convert dot1q bitmasks to SNMP octet-strings 
*
* @param    in          mask to be converted  
*           out         pointer to storage to hold the converted mask
*
* @returns  L7_SUCCESS
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t dot1qReverseMask(NIM_INTF_MASK_t in, NIM_INTF_MASK_t* out)
{
  L7_uint32 j,k;
  NIM_INTF_MASK_t temp;

  memset(&temp, 0, sizeof(NIM_INTF_MASK_t));

  for (k=0;k<NIM_INTF_INDICES;k++)  /* bytes 0 to max-1 */
  {
    for (j=1;j<=8;j++)  /* interfaces 1 to 8 within a byte */
    {
      if (NIM_INTF_ISMASKBITSET(in, 8*k + j)) /* kth byte, jth interface */
        NIM_INTF_SETMASKBIT( temp,8*k + (8-j+1) ); /* reverse msb-lsb */
      else
        NIM_INTF_CLRMASKBIT(temp, 8*k + (8-j+1) );
    }
  }

  memcpy(out, &temp, NIM_INTF_INDICES);
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Determine if interface is a state for config to be applied
*
* @param    intIfNum    @b{(input)}  Internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments Returns L7_TRUE if the interface state is either 
*           L7_ATTACHED or L7_ATTACHING.  This is a pre-condition
*           being able to apply config data to the device (i.e.
*           make DTL calls).
*
* @end
*********************************************************************/
L7_BOOL dot1qIntfIsSettable(L7_uint32 intIfNum)
{
  L7_NIM_QUERY_DATA_t queryData;
  
  /* do some preliminary checking so NIM doesn't log a msg for improper usage */
  if (dot1qCnfgrStateCheck() != L7_TRUE)
    return L7_FALSE;

  /* Check general intIfNum boundary conditions */
  if (intIfNum <= 0 || intIfNum >= platIntfMaxCountGet())
    return L7_FALSE;

  memset(&queryData, 0, sizeof(queryData));
  queryData.intIfNum = intIfNum;
  queryData.request = L7_NIM_QRY_RQST_STATE;

  if (nimIntfQuery(&queryData) == L7_SUCCESS)
  {
    if ((queryData.data.state == L7_INTF_ATTACHED) ||
        (queryData.data.state == L7_INTF_ATTACHING))
    {
      return L7_TRUE;
    }
  }

  return L7_FALSE;
}
