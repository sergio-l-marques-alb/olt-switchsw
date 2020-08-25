/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2009
*
**********************************************************************
*
* @filename llpf_api.c
*
* @purpose    Link Local Protocol Filtering API's
*
* @component  LLPF
*
* @comments   This file contains all the defines, function prototypes
*             and include files required by llpf.c
*
* @create     10/08/2009
*
* @author     Vijayanand K(kvijayan)
* @end
*
**********************************************************************/

/* Common header file includes */
#include <string.h>

#include "l7_common.h"
#include "llpf_exports.h"

/* Component related header file includes */
#include "llpf_util.h"

extern osapiRWLock_t llpfCfgRWLock;

/*********************************************************************
* @purpose  Set LLPF Protocol Filter Blocking Mode.
*
* @param    intfNum       @{{input}}The Current Interface Number
* @param    protocolType  @{{input}}  Type of Protocol
* @param    mode          @{{input}}  Enable/Disable LLPF
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @notes
*
* @end
*********************************************************************/
L7_RC_t llpfIntfBlockModeSet(L7_uint32 intIfNum,
                 L7_LLPF_BLOCK_TYPE_t protocolType, L7_BOOL mode)
{
  L7_BOOL getMode = L7_FALSE;

  L7_RC_t rc = L7_FAILURE;

  /* First check whether the provided interface is valid 
   * llpf Interface or not 
   */
  if(!llpfIntfIsValid(intIfNum))
  {
     L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
     nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

     L7_LOGF(L7_LOG_SEVERITY_DEBUG,L7_LLPF_COMPONENT_ID,
               "\r\nInvalid Interface is specified:%s \r\n",ifName);
    return L7_NOT_SUPPORTED;
  }
 
  /* Get the RW Lock Semaphore */ 
  if(osapiWriteLockTake(llpfCfgRWLock,L7_WAIT_FOREVER) != L7_SUCCESS)
  {
     L7_LOGF(L7_LOG_SEVERITY_INFO,L7_LLPF_COMPONENT_ID,
               "\r\nFailed to take the write lock semaphore \r\n");
    return L7_FAILURE;
  }
  /* Get the current Mode and set the mode only if it is
   * different from current One 
   */
  if(llpfIntfFilterModeGet(intIfNum,protocolType,&getMode) == L7_SUCCESS)
  {
    if(getMode != mode)
    {
      if(mode)
      {
        rc = llpfIntfFilterModeSet(intIfNum,protocolType);
      }
      else
      {
       rc  = llpfIntfFilterModeClear(intIfNum,protocolType);
      } 
    
      if(rc != L7_SUCCESS)
      {
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

        osapiWriteLockGive(llpfCfgRWLock);
        L7_LOGF(L7_LOG_SEVERITY_INFO,L7_LLPF_COMPONENT_ID,
               "\r\nFailed to Set or clear LLPF block Mode for Interface:%s with ProtoID:%d",
                ifName,protocolType);
 
        return L7_FAILURE;
      }
    
      /* Apply the interface protocol mode change to hardware */ 
      if(llpfIntfBlockModeApply(intIfNum,protocolType,mode) != L7_SUCCESS)
      {
        osapiWriteLockGive(llpfCfgRWLock);
        /* Clear the bit mask flags before returning */
        if(mode)
        {
          llpfIntfFilterModeClear(intIfNum,protocolType);
        }
        else
        {
          llpfIntfFilterModeSet(intIfNum,protocolType);
        } 
        L7_LOGF(L7_LOG_SEVERITY_INFO,L7_LLPF_COMPONENT_ID,
               "\r\nFailed to Apply LLPF block Mode on hardware for Interface:%d with ProtoID:%d",
                intIfNum,protocolType);
         return L7_FAILURE;
      }
      /* Set the Config Data changed flag */
      llpfCfgDataChange();
    }
  }
  else
  {
    osapiWriteLockGive(llpfCfgRWLock);
    L7_LOGF(L7_LOG_SEVERITY_INFO,L7_LLPF_COMPONENT_ID,
               "\r\nFailed to give the Write lock semaphore \r\n");
    return L7_FAILURE;
  } 

  osapiWriteLockGive(llpfCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get LLPF Protocol Filter Blocking Mode.
*
* @param    intfNum       @{{input}}The Current Interface Number
* @param    protocolType  @{{input}}  Type of Protocol
* @param    *mode         @{{output}} Enable/Disable LLPF
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @notes
*
*
* @end
*********************************************************************/
L7_RC_t llpfIntfBlockModeGet(L7_uint32 intIfNum,
                 L7_LLPF_BLOCK_TYPE_t protocolType, L7_uint32 *mode)
{
  L7_BOOL modeValue; 
  /* First check whether the provided interface is valid 
   * llpf Interface or not 
   */
  if(!llpfIntfIsValid(intIfNum))
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_DEBUG,L7_LLPF_COMPONENT_ID,
               "\r\nInvalid Interface is specified:%s \r\n", ifName);
    return L7_NOT_SUPPORTED;
  }
 
  /* Get the RW Lock Semaphore */ 
  if(osapiReadLockTake(llpfCfgRWLock,L7_WAIT_FOREVER) != L7_SUCCESS)
  {
     L7_LOGF(L7_LOG_SEVERITY_INFO,L7_LLPF_COMPONENT_ID,
               "\r\nFailed to take the Read lock semaphore \r\n");
    return L7_FAILURE;
  }
 
  /* Get the Block Mode for a given Protocol  */ 
  if(llpfIntfFilterModeGet(intIfNum,protocolType,&modeValue) != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    osapiReadLockGive(llpfCfgRWLock);
    L7_LOGF(L7_LOG_SEVERITY_INFO,L7_LLPF_COMPONENT_ID,
               "\r\nFailed to Get LLPF block Mode for Interface:%s with ProtoID:%d \r\n",
                ifName, protocolType);
    return L7_FAILURE;
  }   

  if( osapiReadLockGive(llpfCfgRWLock) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO,L7_LLPF_COMPONENT_ID,
               "\r\nFailed to give the Read lock semaphore \r\n");
    return L7_FAILURE;
  }
  if(modeValue)
  {
    *mode = L7_ENABLE; 
  }
  else
  {
   *mode = L7_DISABLE; 
  }
  return L7_SUCCESS;
}

