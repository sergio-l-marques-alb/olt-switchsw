/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   pimdm_api.c
*
* @purpose    Mapping Layer Interfacing routines
*
* @component  PIM-DM
*
* @comments   none
*
* @create     
*
* @author     Krishna Mohan CS
* @end
*
**********************************************************************/

/*******************************************************************************
**                             Includes                                       **
*******************************************************************************/
#include "l7_common.h"
#include "l3_addrdefs.h"
#include "l3_mcast_commdefs.h"
#include "l7_pimdm_api.h"
#include "pimdm_main.h"
#include "pimdm_defs.h"
#include "pimdm_intf.h"
#include "pimdm_mrt.h"
#include "pimdm_debug.h"
#include "pimdm_api.h"

/*******************************************************************************
**                        Function Declarations                               **
*******************************************************************************/
/* None */

/*******************************************************************************
**                        Global Declarations                                 **
*******************************************************************************/
/* None */

/*******************************************************************************
**                        Function Definitions                                **
*******************************************************************************/

/*********************************************************************
*
* @purpose Get the Up Time of the specified PIM-DM Neighbor
*
* @param    cbHandle       @b{(input)}  Handle to the PIM-DM Control Block
* @param    nbrAddr        @b{(input)}  Pointer to the Neighbor's IP Address
* @param    nbrUpTime      @b{(output)} Pointer to the Neighbor's Up Time
*
* @returns  L7_SUCCESS, if success 
* @returns  L7_FAILURE, if failure 
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t
pimdmExtenApiNbrUpTimeGet (MCAST_CB_HNDL_t cbHandle,
                           L7_inet_addr_t *nbrAddr,
                           L7_uint32 *nbrUpTime)
{
  pimdmCB_t *pimdmCB = L7_NULLPTR;
  L7_uint32 upTime = 0;
  L7_uchar8 nbr[PIMDM_MAX_DBG_ADDR_SIZE];

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if ((pimdmCB = (pimdmCB_t*) cbHandle) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, 
                 "Invalid PIM-DM Ctrl Block while accessing Neighbor Up Time");
    return L7_FAILURE;
  }

  if (pimdmIntfNeighborUpTimeGet (pimdmCB, nbrAddr,L7_NULL, &upTime)
                               != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Failed to get the UpTime for nbr %s "
                 "for familyType %d\n", inetAddrPrint(nbrAddr,nbr),
                 pimdmCB->addrFamily);
    return L7_FAILURE;
  }

  *nbrUpTime = upTime;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");  
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the Expire Time of the specified PIM-DM Neighbor
*
* @param    cbHandle       @b{(input)}  Handle to the PIM-DM Control Block
* @param    nbrAddr        @b{(input)}  Pointer to the Neighbor's IP Address
* @param    nbrExpiryTime  @b{(output)} Pointer to the Neighbor's Expiry Time
*
* @returns  L7_SUCCESS, if success 
* @returns  L7_FAILURE, if failure 
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t
pimdmExtenApiNbrExpireTimeGet (MCAST_CB_HNDL_t cbHandle,
                               L7_inet_addr_t *nbrAddr,
                               L7_uint32 *nbrExpiryTime)
{
  pimdmCB_t *pimdmCB = L7_NULLPTR;
  L7_uint32 expiryTime = 0;
  L7_uchar8 nbr[PIMDM_MAX_DBG_ADDR_SIZE];

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if ((pimdmCB = (pimdmCB_t*) cbHandle) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, 
                 "Invalid PIM-DM Ctrl Block while accessing Neighbor Expire Time");
    return L7_FAILURE;
  }

  if (pimdmIntfNeighborExpiryTimeGet (pimdmCB, nbrAddr,L7_NULL, &expiryTime)
                                   != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                 "Failed to get Expiry time for nbr %s for familyType %d",
                 inetAddrPrint(nbrAddr,nbr),pimdmCB->addrFamily);
    return L7_FAILURE;
  }

  *nbrExpiryTime = expiryTime;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the Up Time of the specified PIM-DM Neighbor
*
* @param    cbHandle       @b{(input)}  Handle to the PIM-DM Control Block
* @param    nbrAddr        @b{(input)}  Pointer to the Neighbor's IP Address
* @param    nbrAddr        @b{(input)}  internal interface number
* @param    nbrUpTime      @b{(output)} Pointer to the Neighbor's Up Time
*
* @returns  L7_SUCCESS, if success 
* @returns  L7_FAILURE, if failure 
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t
pimdmExtenApiNbrUpTimeByIfIndexGet (MCAST_CB_HNDL_t cbHandle,
                           L7_inet_addr_t *nbrAddr,L7_uint32 intIfNum,
                           L7_uint32 *nbrUpTime)
{
  pimdmCB_t *pimdmCB = L7_NULLPTR;
  L7_uint32 upTime = 0;
  L7_uchar8 nbr[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uint32 rtrIfNum =0;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if ((pimdmCB = (pimdmCB_t*) cbHandle) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, 
                 "Invalid PIM-DM Ctrl Block while accessing Neighbor Up Time");
    return L7_FAILURE;
  }

  if (mcastIpMapIntIfNumToRtrIntf(pimdmCB->addrFamily, 
                                  intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
        
    return L7_FAILURE;
  }

  if (pimdmIntfNeighborUpTimeGet (pimdmCB, nbrAddr,rtrIfNum, &upTime)
                               != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                 "Failed to get the UpTime for nbr %s for familyType %d",
                 inetAddrPrint(nbrAddr,nbr),pimdmCB->addrFamily);
    return L7_FAILURE;
  }

  *nbrUpTime = upTime;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");  
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the Expire Time of the specified PIM-DM Neighbor
*
* @param    cbHandle       @b{(input)}  Handle to the PIM-DM Control Block
* @param    nbrAddr        @b{(input)}  Pointer to the Neighbor's IP Address
* @param    nbrAddr        @b{(input)}  internal interface number
* @param    nbrExpiryTime  @b{(output)} Pointer to the Neighbor's Expiry Time
*
* @returns  L7_SUCCESS, if success 
* @returns  L7_FAILURE, if failure 
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t
pimdmExtenApiNbrExpireTimeByIfIndexGet (MCAST_CB_HNDL_t cbHandle,
                               L7_inet_addr_t *nbrAddr,
                               L7_uint32 intIfNum,
                               L7_uint32 *nbrExpiryTime)
{
  pimdmCB_t *pimdmCB = L7_NULLPTR;
  L7_uint32 expiryTime = 0;
  L7_uchar8 nbr[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uint32 rtrIfNum =0;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if ((pimdmCB = (pimdmCB_t*) cbHandle) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, 
                 "Invalid PIM-DM Ctrl Block while accessing Neighbor Expire Time");
    return L7_FAILURE;
  }

  if (mcastIpMapIntIfNumToRtrIntf(pimdmCB->addrFamily, 
                                  intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
        
    return L7_FAILURE;
  }

  if (pimdmIntfNeighborExpiryTimeGet (pimdmCB, nbrAddr,rtrIfNum, &expiryTime)
                                   != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Failed to get Expiry time for nbr %s \
                 for familyType %d",inetAddrPrint(nbrAddr,nbr),
                 pimdmCB->addrFamily);
    return L7_FAILURE;
  }

  *nbrExpiryTime = expiryTime;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the Neighbor Count on the specified interface
*
* @param    cbHandle     @b{(input)}  Handle to the PIM-DM Control Block
* @param    rtrIfNum     @b{(input)}  Index of the Router Interface
* @param    nbrCount     @b{(output)} Number of Neighbors
*
* @returns  L7_SUCCESS, if success 
* @returns  L7_FAILURE, if failure 
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t
pimdmExtenApiNbrCountGet (MCAST_CB_HNDL_t cbHandle,
                          L7_uint32 rtrIfNum,
                          L7_uint32 *nbrCount)
{
  pimdmCB_t* pimdmCB = L7_NULLPTR;
  L7_int32 intfNbrCount = -1;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if ((pimdmCB = (pimdmCB_t*) cbHandle) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, 
                 "Invalid PIM-DM Ctrl Block while accessing Neighbor Count");
    return L7_FAILURE;
  }

  if ((intfNbrCount = pimdmIntfNumNeighborsGet (pimdmCB, rtrIfNum)) == -1)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                 "Failed to get nbr count for the rtrIfNum %d for \
                 familyType %d", rtrIfNum,pimdmCB->addrFamily);
    return L7_FAILURE;
  }

  *nbrCount = (L7_uint32) intfNbrCount;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the Rtr Interface Index where the specified neighbor
*          resides
*
* @param    cbHandle     @b{(input)}  Handle to the PIM-DM Control Block
* @param    nbrIpAddr    @b{(input)}  Pointer to the Neighbor IP Address
* @param    rtrIfNum     @b{(output)} Index of the Router Interface
*
* @returns  L7_SUCCESS, if success 
* @returns  L7_FAILURE, if failure 
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t
pimdmExtenApiNbrIntfIndexGet (MCAST_CB_HNDL_t cbHandle,
                              L7_inet_addr_t *nbrIpAddr,
                              L7_uint32 *rtrIfNum)
{
  pimdmCB_t* pimdmCB = L7_NULLPTR;
  L7_uint32 nbrRtrIfNum = 0;
  L7_uchar8 nbr[PIMDM_MAX_DBG_ADDR_SIZE];

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if ((pimdmCB = (pimdmCB_t*) cbHandle) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, 
                 "Invalid PIM-DM Ctrl Block while accessing Neighbor Rtr Intf Index.\n");
    return L7_FAILURE;
  }

  if (pimdmIntfNeighborRtrIfIndexGet (pimdmCB, nbrIpAddr, &nbrRtrIfNum)
                                   != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                "Failed to get the nbrRtrIfNum for the nbr %s for \
                 familyType %d",inetAddrPrint(nbrIpAddr,nbr),
                 pimdmCB->addrFamily);
    return L7_FAILURE;
  }

  *rtrIfNum = nbrRtrIfNum;
  
  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Check if the specified is Valid
*
* @param    cbHandle     @b{(input)}  Handle to the PIM-DM Control Block
* @param    nbrAddr      @b{(input)}  Pointer to the Neighbor IP Address
*
* @returns  L7_SUCCESS, if success 
* @returns  L7_FAILURE, if failure 
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t
pimdmExtenApiNbrEntryGet (MCAST_CB_HNDL_t cbHandle,
                          L7_inet_addr_t *nbrAddr)
{
  pimdmCB_t* pimdmCB = L7_NULLPTR;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");
  if ((pimdmCB = (pimdmCB_t*) cbHandle) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, 
                 "Invalid PIM-DM Ctrl Block while accessing Neighbor Entry");
    return L7_FAILURE;
  }

  if (pimdmIntfNeighborGet (pimdmCB, nbrAddr) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Failed to get the NbrAddr for \
                 familyType %d",pimdmCB->addrFamily);
    return L7_FAILURE;
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the next valid Neighbor
*
* @param    cbHandle     @b{(input)}  Handle to the PIM-DM Control Block
* @param    nbrAddr      @b{(i/o  )}  Pointer to the Neighbor IP Address
*
* @returns  L7_SUCCESS, if success 
* @returns  L7_FAILURE, if failure 
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t
pimdmExtenApiNbrEntryNextGet (MCAST_CB_HNDL_t cbHandle,
                              L7_inet_addr_t *nbrAddr)
{
  pimdmCB_t *pimdmCB = L7_NULLPTR;
 
  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if ((pimdmCB = (pimdmCB_t*) cbHandle) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, 
                 "Invalid PIM-DM Ctrl Block while accessing Neighbor Entry");
    return L7_FAILURE;
  }

  if (pimdmIntfNeighborNextGet (pimdmCB, nbrAddr) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Failed to get next the NbrAddr for \
                 familyType %d",pimdmCB->addrFamily);
    return L7_FAILURE;
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the PIMDM neighbor entry validity on the specified interface
*
* @param    cbHandle     @b{(input)}  Handle to the PIM-DM Control Block
* @param    intIfNum     @b{(input)}  Internal interface number
* @param    nbrAddr      @b{(input)}  Pointer to the Neighbor IP Address
*
* @returns  L7_SUCCESS, if success 
* @returns  L7_FAILURE, if failure 
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t
pimdmExtenApiNbrEntryByIfIndexGet (MCAST_CB_HNDL_t cbHandle,
                                   L7_uint32 intIfNum,
                                   L7_inet_addr_t *nbrAddr)
{
  pimdmCB_t *pimdmCB = L7_NULLPTR;
  L7_uint32 rtrIfNum =0;
  L7_RC_t retCode = L7_FAILURE;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if ((pimdmCB = (pimdmCB_t*) cbHandle) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, 
                 "Invalid PIM-DM Ctrl Block while accessing Neighbor Entry");
    return L7_FAILURE;
  }

  if (mcastIpMapIntIfNumToRtrIntf (pimdmCB->addrFamily, intIfNum, &rtrIfNum)
                                != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  retCode = pimdmIntfNeighborByIfIndexGet (pimdmCB, rtrIfNum, nbrAddr);

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return retCode;
}

/*********************************************************************
*
* @purpose Get the next valid Neighbor by Interface Index
*
* @param    cbHandle     @b{(input)}  Handle to the PIM-DM Control Block
* @param    nbrAddr      @b{(i/o  )}  Pointer to interface number
* @param    nbrAddr      @b{(i/o  )}  Pointer to the Neighbor IP Address
*
* @returns  L7_SUCCESS, if success 
* @returns  L7_FAILURE, if failure 
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t
pimdmExtenApiNbrEntryByIfIndexNextGet (MCAST_CB_HNDL_t cbHandle,
                                       L7_uint32 *outIntIfNum,
                                       L7_inet_addr_t *nbrAddr)
{
  pimdmCB_t *pimdmCB = L7_NULLPTR;
  L7_uint32 rtrIfNum =0;
  L7_RC_t retCode = L7_FAILURE;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if ((pimdmCB = (pimdmCB_t*) cbHandle) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, 
                 "Invalid PIM-DM Ctrl Block while accessing Neighbor Entry");
    return L7_FAILURE;
  }

  if (*outIntIfNum == 0)
  {
    rtrIfNum = L7_NULL;
    retCode = pimdmIntfNeighborByIfIndexNextGet (pimdmCB, nbrAddr,&rtrIfNum);  
  }
  else
  {
    if (mcastIpMapIntIfNumToRtrIntf(pimdmCB->addrFamily, 
                                    *outIntIfNum, &rtrIfNum) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    retCode = pimdmIntfNeighborByIfIndexNextGet (pimdmCB, nbrAddr,&rtrIfNum);  
  }
  if (retCode == L7_SUCCESS)
  {
    if (mcastIpMapRtrIntfToIntIfNum(pimdmCB->addrFamily, 
                                    rtrIfNum,outIntIfNum) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return retCode;
}


/*********************************************************************
*
* @purpose Get the Assert Metric for the (S,G) entry
*
* @param    cbHandle     @b{(input)}   Handle to the PIM-DM Control Block
* @param    grpAddr      @b{(input)}   Address of the Multicast Group
* @param    srcAddr      @b{(input)}   Address of the Multicast Source
* @param    srcMask      @b{(input)}   Address of the Multicast Source Mask
* @param    assertMetric @b{(output)}  Pointer to the Assert Metric
*
* @returns  L7_SUCCESS, if success 
* @returns  L7_FAILURE, if failure 
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t
pimdmExtenApiIpMRouteAssertMetricGet (MCAST_CB_HNDL_t cbHandle,
                                      L7_inet_addr_t *grpAddr,
                                      L7_inet_addr_t *srcAddr,
                                      L7_inet_addr_t *srcMask,
                                      L7_uint32 *assertMetric)
{
  pimdmCB_t* pimdmCB = L7_NULLPTR;
  pimdmMrtEntry_t* mrtEntry = L7_NULLPTR;
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");
  
  if ((pimdmCB = (pimdmCB_t*) cbHandle) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Invalid Ctrl Block");
    return L7_FAILURE;
  }

  if (osapiSemaTake (pimdmCB->mrtSGTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Failed to Acquire PIM-DM MRT Semaphore");
    return L7_FAILURE;
  }

  if ((mrtEntry = pimdmMrtEntryGet (pimdmCB, srcAddr, grpAddr)) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                 "MRT Lookup Failed for Src - %s, Group - %s",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));
    osapiSemaGive (pimdmCB->mrtSGTree.semId);
    return L7_FAILURE;
  }

  /* Get the assert metric value in the  current entry */
  *assertMetric = mrtEntry->rpfInfo.rpfRouteMetric;

  osapiSemaGive (pimdmCB->mrtSGTree.semId);

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the Assert Metric Preference for the (S,G) entry
*
* @param    cbHandle   @b{(input)}   Handle to the PIM-DM Control Block
* @param    grpAddr    @b{(input)}   Address of the Multicast Group
* @param    srcAddr    @b{(input)}   Address of the Multicast Source
* @param    srcMask    @b{(input)}   Address of the Multicast Source Mask
* @param    assertMetricPref @b{(output)}  Pointer to the Assert Metric
*                                          Preference
*
* @returns  L7_SUCCESS, if success 
* @returns  L7_FAILURE, if failure 
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t
pimdmExtenApiIpMRouteAssertMetricPrefGet (MCAST_CB_HNDL_t cbHandle,
                                          L7_inet_addr_t *grpAddr,
                                          L7_inet_addr_t *srcAddr,
                                          L7_inet_addr_t *srcMask,
                                          L7_uint32 *assertMetricPref)
{
  pimdmCB_t* pimdmCB = L7_NULLPTR;
  pimdmMrtEntry_t* mrtEntry = L7_NULLPTR;
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];
     
  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");
  
  if ((pimdmCB = (pimdmCB_t*) cbHandle) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Invalid Ctrl Block");
    return L7_FAILURE;
  }

  if (osapiSemaTake (pimdmCB->mrtSGTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Failed to Acquire PIM-DM MRT Semaphore");
    return L7_FAILURE;
  }

  if ((mrtEntry = pimdmMrtEntryGet (pimdmCB, srcAddr, grpAddr)) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                 "MRT Lookup Failed for Src - %s, Group - %s",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));
    osapiSemaGive (pimdmCB->mrtSGTree.semId);
    return L7_FAILURE;
  }

  *assertMetricPref = mrtEntry->rpfInfo.rpfRouteMetricPref;

  osapiSemaGive (pimdmCB->mrtSGTree.semId);

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the RPT bit status for the (S,G) entry
*
* @param    cbHandle   @b{(input)}   Handle to the PIM-DM Control Block
* @param    grpAddr    @b{(input)}   Address of the Multicast Group
* @param    srcAddr    @b{(input)}   Address of the Multicast Source
* @param    srcMask    @b{(input)}   Address of the Multicast Source Mask
* @param    rptBitStatus @b{(output)}  Pointer to the RPT Bit Status
*
* @returns  L7_SUCCESS, if success 
* @returns  L7_FAILURE, if failure 
*
* @comments This is PIM-SM specific and not supported.
*
* @end
*********************************************************************/
L7_RC_t
pimdmExtenApiIpMRouteAssertRPTBitGet (MCAST_CB_HNDL_t cbHandle,
                                      L7_inet_addr_t *grpAddr,
                                      L7_inet_addr_t *srcAddr,
                                      L7_inet_addr_t *srcMask,
                                      L7_uint32 *rptBitStatus)
{
  pimdmCB_t* pimdmCB = L7_NULLPTR;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");
  
  if ((pimdmCB = (pimdmCB_t*) cbHandle) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Invalid Ctrl Block");
    return L7_FAILURE;
  }
 
  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_NOT_SUPPORTED;
}

/*********************************************************************
*
* @purpose Get the Upstream Assert Holdtime for the (S,G) entry
*
* @param    cbHandle   @b{(input)}   Handle to the PIM-DM Control Block
* @param    grpAddr    @b{(input)}   Address of the Multicast Group
* @param    srcAddr    @b{(input)}   Address of the Multicast Source
* @param    srcMask    @b{(input)}   Address of the Multicast Source Mask
* @param    upstrmAssertTime @b{(output)}  Pointer to the Assert Time
*
* @returns  L7_SUCCESS, if success 
* @returns  L7_FAILURE, if failure 
*
* @comments The Upstream Assert Timer is not supported.
*
* @end
*********************************************************************/
L7_RC_t
pimdmExtenApiIpMRouteUpstreamAssertTimerGet (MCAST_CB_HNDL_t cbHandle,
                                             L7_inet_addr_t *grpAddr,
                                             L7_inet_addr_t *srcAddr,
                                             L7_inet_addr_t *srcMask,
                                             L7_uint32 *upstrmAssertTime)
{
  pimdmCB_t* pimdmCB = L7_NULLPTR;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");
  
  if ((pimdmCB = (pimdmCB_t*) cbHandle) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Invalid Ctrl Block");
    return L7_FAILURE;
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_NOT_SUPPORTED;
}

/*********************************************************************
*
* @purpose Check if the specified (S,G) entry is valid
*
* @param    cbHandle   @b{(input)}   Handle to the PIM-DM Control Block
* @param    grpAddr    @b{(input)}   Address of the Multicast Group
* @param    srcAddr    @b{(input)}   Address of the Multicast Source
* @param    srcMask    @b{(input)}   Address of the Multicast Source Mask
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmExtenApiIpMRouteEntryGet (MCAST_CB_HNDL_t cbHandle,
                               L7_inet_addr_t *grpAddr,
                               L7_inet_addr_t *srcAddr,
                               L7_inet_addr_t *srcMask)
{
  pimdmCB_t* pimdmCB = L7_NULLPTR;
  pimdmMrtEntry_t* mrtEntry = L7_NULLPTR;
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");
  
  if ((pimdmCB = (pimdmCB_t*) cbHandle) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Invalid Ctrl Block");
    return L7_FAILURE;
  }

  if (osapiSemaTake (pimdmCB->mrtSGTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Failed to Acquire PIM-DM MRT Semaphore");
    return L7_FAILURE;
  }

  if ((mrtEntry = pimdmMrtEntryGet (pimdmCB, srcAddr, grpAddr)) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                 "MRT Lookup Failed for Src - %s, Group - %s",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));
    osapiSemaGive (pimdmCB->mrtSGTree.semId);
    return L7_FAILURE;
  }

  osapiSemaGive (pimdmCB->mrtSGTree.semId);

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the Next MRT (S,G) entry
*
* @param    cbHandle   @b{(input)}   Handle to the PIM-DM Control Block
* @param    grpAddr    @b{(input/output  )}   Address of the Multicast Group
* @param    srcAddr    @b{(input/output  )}   Address of the Multicast Source
* @param    srcMask    @b{(input/output  )}   Address of the Multicast Source Mask
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmExtenApiIpMRouteEntryNextGet (MCAST_CB_HNDL_t cbHandle,
                                   L7_inet_addr_t *grpAddr,
                                   L7_inet_addr_t *srcAddr,
                                   L7_inet_addr_t *srcMask)
{
  pimdmCB_t* pimdmCB = L7_NULLPTR;
  pimdmMrtEntry_t* mrtEntry = L7_NULLPTR;
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];
  
  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");
  
  if ((pimdmCB = (pimdmCB_t*) cbHandle) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Invalid Ctrl Block");
    return L7_FAILURE;
  }

  if (osapiSemaTake (pimdmCB->mrtSGTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Failed to Acquire PIM-DM MRT Semaphore");
    return L7_FAILURE;
  }

  if ((mrtEntry = pimdmMrtEntryNextGet (pimdmCB, srcAddr, grpAddr))
      == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                 "MRT Lookup Failed for Src - %s, Group - %s",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));
    osapiSemaGive (pimdmCB->mrtSGTree.semId);
    return L7_FAILURE;
  }

  if (inetCopy (grpAddr, &(mrtEntry->grpAddr)) != L7_SUCCESS)
  {
    osapiSemaGive (pimdmCB->mrtSGTree.semId);
    return L7_FAILURE;
  }
  if (inetCopy (srcAddr, &(mrtEntry->srcAddr)) != L7_SUCCESS)
  {
    osapiSemaGive (pimdmCB->mrtSGTree.semId);
    return L7_FAILURE;
  }
  if (inetMaskLenToMask (pimdmCB->addrFamily, mrtEntry->rpfInfo.rpfMaskLen,
                         srcMask)
                      != L7_SUCCESS)
  {
    osapiSemaGive (pimdmCB->mrtSGTree.semId);
    return L7_FAILURE;
  }

  osapiSemaGive (pimdmCB->mrtSGTree.semId);

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the Nexthop Table entry
*
* @param    cbHandle        @b{(input)}   Handle to the PIM-DM Control Block
* @param    nextHopGrpAddr  @b{(input)}   Address of the Multicast Group
* @param    nextHopSrcAddr  @b{(input)}   Address of the Multicast Source
* @param    nextHopSrcMask  @b{(input)}   Address of the Multicast Source Mask
* @param    nextHopRtrIfNum @b{(input)}   Index of the Upstream Interface
* @param    nextHopAddress  @b{(input)}   Address of the Nexthop (RPF) 
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmExtenApiIpMRouteNextHopEntryGet (MCAST_CB_HNDL_t cbHandle,
                                      L7_inet_addr_t *nextHopGrpAddr,
                                      L7_inet_addr_t *nextHopSrcAddr,
                                      L7_inet_addr_t *nextHopSrcMask,
                                      L7_uint32 nextHopRtrIfNum,
                                      L7_inet_addr_t *nextHopAddress)
{
  pimdmCB_t* pimdmCB = L7_NULLPTR;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");
  
  if ((pimdmCB = (pimdmCB_t*) cbHandle) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Invalid Ctrl Block");
    return L7_FAILURE;
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_NOT_SUPPORTED;
}

/*********************************************************************
*
* @purpose Get the next entry of Nexthop Table
*
* @param    cbHandle        @b{(input)}   Handle to the PIM-DM Control Block
* @param    nextHopGrpAddr  @b{(input)}   Address of the Multicast Group
* @param    nextHopSrcAddr  @b{(input)}   Address of the Multicast Source
* @param    nextHopSrcMask  @b{(input)}   Address of the Multicast Source Mask
* @param    nextHopRtrIfNum @b{(input)}   Indext of the Upstream Interface
* @param    nextHopAddress  @b{(input/output  )}   Address of the Nexthop (RPF) 
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmExtenApiIpMRouteNextHopEntryNextGet (MCAST_CB_HNDL_t cbHandle,
                                          L7_inet_addr_t *nextHopGrpAddr,
                                          L7_inet_addr_t *nextHopSrcAddr,
                                          L7_inet_addr_t *nextHopSrcMask,
                                          L7_uint32 *nextHopRtrIfNum,
                                          L7_inet_addr_t *nextHopAddress)
{
  pimdmCB_t* pimdmCB = L7_NULLPTR;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");
  
  if ((pimdmCB = (pimdmCB_t*) cbHandle) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Invalid Ctrl Block");
    return L7_FAILURE;
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_NOT_SUPPORTED;
}

/*********************************************************************
*
* @purpose Get the Prune Reason for the entry of the Nexthop Table
*
* @param    cbHandle        @b{(input)}   Handle to the PIM-DM Control Block
* @param    nextHopGrpAddr  @b{(input)}   Address of the Multicast Group
* @param    nextHopSrcAddr  @b{(input)}   Address of the Multicast Source
* @param    nextHopSrcMask  @b{(input)}   Address of the Multicast Source Mask
* @param    nextHopRtrIfNum @b{(input)}   Indext of the Upstream Interface
* @param    nextHopAddress  @b{(input)}   Address of the Nexthop (RPF) 
* @param    pruneReason     @b{(output)}  Pointer to the Reason for Prune
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmExtenApiIpMRouteNextHopPruneReasonGet (MCAST_CB_HNDL_t cbHandle,
                                            L7_inet_addr_t *nextHopGrpAddr,
                                            L7_inet_addr_t *nextHopSrcAddr,
                                            L7_inet_addr_t *nextHopSrcMask,
                                            L7_uint32 nextHopRtrIfNum,
                                            L7_inet_addr_t *nextHopAddress,
                                            L7_uint32 *pruneReason)
{
  pimdmCB_t* pimdmCB = L7_NULLPTR;
  pimdmMrtEntry_t* mrtEntry = L7_NULLPTR;
  PIMDM_MRT_ENTRY_PRUNE_REASON_t mrtEntryPruneReason = 0;
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if ((pimdmCB = (pimdmCB_t*) cbHandle) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Invalid Ctrl Block");
    return L7_FAILURE;
  }

  /* Get the corresponding MRT Entry */
  if (osapiSemaTake (pimdmCB->mrtSGTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Failed to Acquire PIM-DM MRT Semaphore");
    return L7_FAILURE;
  }

  if ((mrtEntry = pimdmMrtEntryNextGet (pimdmCB, nextHopSrcAddr, nextHopGrpAddr))
      == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                 "MRT Lookup Failed for Src - %s, Group - %s",
                  inetAddrPrint(nextHopSrcAddr,src), 
                  inetAddrPrint(nextHopGrpAddr,grp));
    osapiSemaGive (pimdmCB->mrtSGTree.semId);
    return L7_FAILURE;
  }

  /* Compare the Next Hop RPF address */
  if (L7_INET_ADDR_COMPARE (&(mrtEntry->upstrmNbrInfo.assertWnrAddr),
                            nextHopAddress) != 0)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                 "RPF Next Hop Match Failed for Src - %s, Group - %s",
                 inetAddrPrint(nextHopSrcAddr,src), 
                 inetAddrPrint(nextHopGrpAddr,grp));
    osapiSemaGive (pimdmCB->mrtSGTree.semId);
    return L7_FAILURE;
  }

  /* Find the reason for the Prune and return */
  if (BITX_TEST (&mrtEntry->lostAssertMask, nextHopRtrIfNum) != 0)
  {
    mrtEntryPruneReason = PIMDM_MRT_ENTRY_PRUNE_REASON_ASSERT;
  }
  else if (BITX_TEST (&mrtEntry->downstrmPruneMask, nextHopRtrIfNum) != 0)
  {
    mrtEntryPruneReason = PIMDM_MRT_ENTRY_PRUNE_REASON_PRUNE;
  }
  else
  {
    mrtEntryPruneReason = PIMDM_MRT_ENTRY_PRUNE_REASON_OTHER;
  }

  osapiSemaGive (pimdmCB->mrtSGTree.semId);

  *pruneReason = mrtEntryPruneReason;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the Multicast Route Entry Flags
*
* @param    cbHandle   @b{(input)}   Handle to the PIM-DM Control Block
* @param    grpAddr    @b{(input)}   Address of the Multicast Group
* @param    srcAddr    @b{(input)}   Address of the Multicast Source
* @param    srcMask    @b{(input)}   Address of the Multicast Source Mask
* @param    entryFlags @b{(output)}  Pointer to the Entry Flags
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments This is PIM-SM specific and not supported.
*
* @end
*********************************************************************/
L7_RC_t
pimdmExtenApiIpMRouteFlagsGet (MCAST_CB_HNDL_t cbHandle,
                               L7_inet_addr_t *grpAddr,
                               L7_inet_addr_t *srcAddr,
                               L7_inet_addr_t *srcMask,
                               L7_uint32 *entryFlags)
{
  pimdmCB_t* pimdmCB = L7_NULLPTR;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");
  if ((pimdmCB = (pimdmCB_t*) cbHandle) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Invalid Ctrl Block");
    return L7_FAILURE;
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_NOT_SUPPORTED;
}

/*********************************************************************
*
* @purpose Get the Multicast Route Table Entry's OIF List
*
* @param    cbHandle   @b{(input)}   Handle to the PIM-DM Control Block
* @param    grpAddr    @b{(input)}   Address of the Multicast Group
* @param    srcAddr    @b{(input)}   Address of the Multicast Source
* @param    srcMask    @b{(input)}   Address of the Multicast Source Mask
* @param    rtrIfNum   @b{(output)}  Pointer to the Router Interface Index
* @param    entryState @b{(output)}  Pointer to the Entry State
* @param    entryMode  @b{(output)}  Pointer to the Entry Mode
* @param    entryUPTime     @b{(output)}  Pointer to the Entry UP Time
* @param    entryExpireTime @b{(output)}  Pointer to the Entry Expiry Time
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmExtenApiIpMRouteOIFNextGet (MCAST_CB_HNDL_t cbHandle,
                                 L7_inet_addr_t *grpAddr,
                                 L7_inet_addr_t *srcAddr,
                                 L7_inet_addr_t *srcMask,
                                 L7_uint32 *rtrIfNum,
                                 L7_uint32 *entryState,
                                 L7_uint32 *entryMode,
                                 L7_uint32 *entryUPTime,
                                 L7_uint32 *entryExpireTime)
 {
  pimdmCB_t* pimdmCB = L7_NULLPTR;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if ((pimdmCB = (pimdmCB_t*) cbHandle) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Invalid Ctrl Block");
    return L7_FAILURE;
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_NOT_SUPPORTED;
}

/*********************************************************************
*
* @purpose Get the MRT (S,G) Entry's Next Outgoing Interface Index
*
* @param    cbHandle    @b{(input)}   Handle to the PIM-DM Control Block
* @param    grpAddr     @b{(input)}   Address of the Multicast Group
* @param    srcAddr     @b{(input)}   Address of the Multicast Source
* @param    srcMask     @b{(input)}   Address of the Multicast Source Mask
* @param    outRtrIfNum @b{(output)}  Pointer to the Outgoing Interface
*                                    Index
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmExtenApiIpRouteOutIntfEntryNextGet (MCAST_CB_HNDL_t cbHandle,
                                         L7_inet_addr_t *grpAddr,
                                         L7_inet_addr_t *srcAddr,
                                         L7_inet_addr_t *srcMask,
                                         L7_uint32 *outRtrIfNum)
{
  pimdmCB_t* pimdmCB = L7_NULLPTR;
  pimdmMrtEntry_t *mrtEntry = L7_NULLPTR;
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uint32 rtrIfNum = 0;
  L7_RC_t retVal = L7_FAILURE;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");
  
  if ((pimdmCB = (pimdmCB_t*) cbHandle) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Invalid Ctrl Block");
    return L7_FAILURE;
  }

  if (osapiSemaTake (pimdmCB->mrtSGTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Failed to Acquire PIM-DM MRT Semaphore");
    return L7_FAILURE;
  }

  if ((mrtEntry = pimdmMrtEntryGet (pimdmCB, srcAddr, grpAddr))
      == L7_NULLPTR)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                "MRT Lookup Failed for Src - %s, Group - %s",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp)); 
    osapiSemaGive (pimdmCB->mrtSGTree.semId);
    return L7_FAILURE;
  }

  if (*outRtrIfNum == 0)
  {
    for (rtrIfNum = 1; rtrIfNum < PIMDM_MAX_INTERFACES; rtrIfNum++)
    {
      if (BITX_TEST (&mrtEntry->oifList, rtrIfNum) != 0)
      {
        *outRtrIfNum = rtrIfNum;
        retVal = L7_SUCCESS;
        break;
      }
    }
  }
  else
  {
    for (rtrIfNum = *outRtrIfNum+1; rtrIfNum < PIMDM_MAX_INTERFACES; rtrIfNum++)
    {
      if (BITX_TEST (&mrtEntry->oifList, rtrIfNum) != 0)
      {
        *outRtrIfNum = rtrIfNum;
        retVal = L7_SUCCESS;
        break;
      }
    }
  }

  osapiSemaGive (pimdmCB->mrtSGTree.semId);

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return retVal;
}


/*********************************************************************
*
* @purpose Get the Upstream Interface Index for the specified MRT
*          (S,G) Entry
*
* @param    cbHandle   @b{(input)}   Handle to the PIM-DM Control Block
* @param    grpAddr    @b{(input)}   Address of the Multicast Group
* @param    srcAddr    @b{(input)}   Address of the Multicast Source
* @param    srcMask    @b{(input)}   Address of the Multicast Source Mask
* @param    rtrIfNum   @b{(output)}  Pointer to the Router Interface Index
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmExtenApiRouteIfIndexGet (MCAST_CB_HNDL_t cbHandle,
                              L7_inet_addr_t *grpAddr,
                              L7_inet_addr_t *srcAddr,
                              L7_inet_addr_t *srcMask,
                              L7_uint32 *rtrIfNum)
{
  pimdmCB_t *pimdmCB = L7_NULLPTR;
  pimdmMrtEntry_t *mrtEntry = L7_NULLPTR;
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if ((pimdmCB = (pimdmCB_t*) cbHandle) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Invalid Ctrl Block");
    return L7_FAILURE;
  }

  if (osapiSemaTake (pimdmCB->mrtSGTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Failed to Acquire PIM-DM MRT Semaphore");
    return L7_FAILURE;
  }

  if ((mrtEntry = pimdmMrtEntryGet (pimdmCB, srcAddr, grpAddr))
      == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                "MRT Lookup Failed for Src - %s, Group - %s",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp)); 
    osapiSemaGive (pimdmCB->mrtSGTree.semId);
    return L7_FAILURE;
  }

  *rtrIfNum = mrtEntry->upstrmRtrIfNum;

  osapiSemaGive (pimdmCB->mrtSGTree.semId);

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the UP Time for the specified MRT (S,G) Entry
*
* @param    cbHandle   @b{(input)}   Handle to the PIM-DM Control Block
* @param    grpAddr    @b{(input)}   Address of the Multicast Group
* @param    srcAddr    @b{(input)}   Address of the Multicast Source
* @param    srcMask    @b{(input)}   Address of the Multicast Source Mask
* @param    entryUPTime @b{(output)}  Pointer to the Entry UP Time
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmExtenApiRouteUptimeGet (MCAST_CB_HNDL_t cbHandle,
                             L7_inet_addr_t *grpAddr,
                             L7_inet_addr_t *srcAddr,
                             L7_inet_addr_t *srcMask,
                             L7_uint32 *entryUPTime)
{
  pimdmCB_t* pimdmCB = L7_NULLPTR;
  pimdmMrtEntry_t *mrtEntry = L7_NULLPTR;
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if ((pimdmCB = (pimdmCB_t*) cbHandle) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Invalid Ctrl Block");
    return L7_FAILURE;
  }

  if (osapiSemaTake (pimdmCB->mrtSGTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Failed to Acquire PIM-DM MRT Semaphore");
    return L7_FAILURE;
  }

  if ((mrtEntry = pimdmMrtEntryGet (pimdmCB, srcAddr, grpAddr))
      == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                "MRT Lookup Failed for Src - %s, Group - %s",
                inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp)); 
    osapiSemaGive (pimdmCB->mrtSGTree.semId);
    return L7_FAILURE;
  }

  *entryUPTime = osapiUpTimeRaw() - mrtEntry->entryUpTime;

  osapiSemaGive (pimdmCB->mrtSGTree.semId);

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the Expiry Time for the specified MRT (S,G) Entry
*
* @param    cbHandle   @b{(input)}   Handle to the PIM-DM Control Block
* @param    grpAddr    @b{(input)}   Address of the Multicast Group
* @param    srcAddr    @b{(input)}   Address of the Multicast Source
* @param    srcMask    @b{(input)}   Address of the Multicast Source Mask
* @param    entryExpireTime @b{(output)}  Pointer to the Entry Expiry Time
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmExtenApiRouteExpiryTimeGet (MCAST_CB_HNDL_t cbHandle,
                                L7_inet_addr_t *grpAddr,
                                L7_inet_addr_t *srcAddr,
                                L7_inet_addr_t *srcMask,
                                L7_uint32 *entryExpireTime)
{
  pimdmCB_t* pimdmCB = L7_NULLPTR;
  pimdmMrtEntry_t *mrtEntry = L7_NULLPTR;
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uint32 mrtEntryExpiryTime = 0;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if ((pimdmCB = (pimdmCB_t*) cbHandle) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Invalid Ctrl Block");
    return L7_FAILURE;
  }

  if (osapiSemaTake (pimdmCB->mrtSGTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Failed to Acquire PIM-DM MRT Semaphore");
    return L7_FAILURE;
  }

  if ((mrtEntry = pimdmMrtEntryGet (pimdmCB, srcAddr, grpAddr))
      == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                "MRT Lookup Failed for Src - %s, Group - %s",
                inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp)); 
    osapiSemaGive (pimdmCB->mrtSGTree.semId);
    return L7_FAILURE;
  }

  if (appTimerTimeLeftGet (pimdmCB->appTimer, mrtEntry->mrtEntryExpiryTimer,
                           &mrtEntryExpiryTime)
                        != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                "MRT App Timer Time Left Get Failed for Src - %s, Group - %s",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp)); 
    osapiSemaGive (pimdmCB->mrtSGTree.semId);
    return L7_FAILURE;
  }

  osapiSemaGive (pimdmCB->mrtSGTree.semId);

  *entryExpireTime = mrtEntryExpiryTime;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the RPF Address for the specified MRT (S,G) Entry
*
* @param    cbHandle   @b{(input)}   Handle to the PIM-DM Control Block
* @param    grpAddr    @b{(input)}   Address of the Multicast Group
* @param    srcAddr    @b{(input)}   Address of the Multicast Source
* @param    srcMask    @b{(input)}   Address of the Multicast Source Mask
* @param    rpfAddr    @b{(output)}  Pointer to the entry RPF Address
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmExtenApiIpMRouteRtAddrGet (MCAST_CB_HNDL_t cbHandle,
                                L7_inet_addr_t *grpAddr,
                                L7_inet_addr_t *srcAddr,
                                L7_inet_addr_t *srcMask,
                                L7_inet_addr_t *rpfAddr)
{
  pimdmCB_t *pimdmCB = L7_NULLPTR;
  pimdmMrtEntry_t *mrtEntry = L7_NULLPTR;
  L7_RC_t retVal = L7_FAILURE;
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if ((pimdmCB = (pimdmCB_t*) cbHandle) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Invalid Ctrl Block");
    return L7_FAILURE;
  }

  if (osapiSemaTake (pimdmCB->mrtSGTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Failed to Acquire PIM-DM MRT Semaphore");
    return L7_FAILURE;
  }

  if ((mrtEntry = pimdmMrtEntryGet (pimdmCB, srcAddr, grpAddr))
      == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                "MRT Lookup Failed for Src - %s, Group - %s",
                inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));
    osapiSemaGive (pimdmCB->mrtSGTree.semId);
    return L7_FAILURE;
  }

  retVal = inetCopy (rpfAddr, &(mrtEntry->rpfInfo.rpfNextHop));

  osapiSemaGive (pimdmCB->mrtSGTree.semId);

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return retVal;
}

/*********************************************************************
*
* @purpose Get the RPF Address Mask for the specified MRT (S,G) Entry
*
* @param    cbHandle   @b{(input)}   Handle to the PIM-DM Control Block
* @param    grpAddr    @b{(input)}   Address of the Multicast Group
* @param    srcAddr    @b{(input)}   Address of the Multicast Source
* @param    srcMask    @b{(input)}   Address of the Multicast Source Mask
* @param    rpfMask    @b{(output)}  Pointer to the entry RPF Address
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmExtenApiIpMRouteRtMaskGet (MCAST_CB_HNDL_t cbHandle,
                                L7_inet_addr_t *grpAddr,
                                L7_inet_addr_t *srcAddr,
                                L7_inet_addr_t *srcMask,
                                L7_inet_addr_t *rpfMask)
{
  pimdmCB_t *pimdmCB = L7_NULLPTR;
  pimdmMrtEntry_t *mrtEntry = L7_NULLPTR;
  L7_RC_t retVal = L7_FAILURE;
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");
  
  if ((pimdmCB = (pimdmCB_t*) cbHandle) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Invalid Ctrl Block");
    return L7_FAILURE;
  }

  if (osapiSemaTake (pimdmCB->mrtSGTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Failed to Acquire PIM-DM MRT Semaphore");
    return L7_FAILURE;
  }

  if ((mrtEntry = pimdmMrtEntryGet (pimdmCB, srcAddr, grpAddr))
      == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                 "MRT Lookup Failed for Src - %s, Group - %s",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));
    osapiSemaGive (pimdmCB->mrtSGTree.semId);
    return L7_FAILURE;
  }

  retVal = inetMaskLenToMask (pimdmCB->addrFamily, mrtEntry->rpfInfo.rpfMaskLen,
                              rpfMask);
  osapiSemaGive (pimdmCB->mrtSGTree.semId);

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return retVal;
}

/*********************************************************************
*
* @purpose Get the RPF Route Type for the specified MRT (S,G) Entry
*
* @param    cbHandle   @b{(input)}   Handle to the PIM-DM Control Block
* @param    grpAddr    @b{(input)}   Address of the Multicast Group
* @param    srcAddr    @b{(input)}   Address of the Multicast Source
* @param    srcMask    @b{(input)}   Address of the Multicast Source Mask
* @param    routeType  @b{(output)}  Pointer to the entry's Route Type
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmExtenApiIpMRouteRtTypeGet (MCAST_CB_HNDL_t cbHandle,
                                L7_inet_addr_t *grpAddr,
                                L7_inet_addr_t *srcAddr,
                                L7_inet_addr_t *srcMask,
                                L7_uint32 *routeType)
{
  pimdmCB_t* pimdmCB = L7_NULLPTR;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if ((pimdmCB = (pimdmCB_t*) cbHandle) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Invalid Ctrl Block");
    return L7_FAILURE;
  }

  *routeType = L7_UNICAST_ROUTE; 

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the Route Protocol for the specified MRT (S,G) Entry
*
* @param    cbHandle   @b{(input)}   Handle to the PIM-DM Control Block
* @param    grpAddr    @b{(input)}   Address of the Multicast Group
* @param    srcAddr    @b{(input)}   Address of the Multicast Source
* @param    srcMask    @b{(input)}   Address of the Multicast Source Mask
* @param    routeProto @b{(output)}  Pointer to the entry's Route Protocol
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmExtenApiIpMRouteRtProtoGet (MCAST_CB_HNDL_t cbHandle,
                                 L7_inet_addr_t *grpAddr,
                                 L7_inet_addr_t *srcAddr,
                                 L7_inet_addr_t *srcMask,
                                 L7_uint32 *routeProto)
{
  pimdmCB_t* pimdmCB = L7_NULLPTR;
  pimdmMrtEntry_t *mrtEntry = L7_NULLPTR;
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if ((pimdmCB = (pimdmCB_t*) cbHandle) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Invalid Ctrl Block");
    return L7_FAILURE;
  }

  if (osapiSemaTake (pimdmCB->mrtSGTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Failed to Acquire PIM-DM MRT Semaphore");
    return L7_FAILURE;
  }

  if ((mrtEntry = pimdmMrtEntryGet (pimdmCB, srcAddr, grpAddr))
      == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                "MRT Lookup Failed for Src - %s, Group - %s",
                inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));
    osapiSemaGive (pimdmCB->mrtSGTree.semId);
    return L7_FAILURE;
  }

  *routeProto = mrtEntry->rpfInfo.rpfRouteProto;

  osapiSemaGive (pimdmCB->mrtSGTree.semId);

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the RPF Address for the specified MRT (S,G) Entry
*
* @param    cbHandle   @b{(input)}   Handle to the PIM-DM Control Block
* @param    grpAddr    @b{(input)}   Address of the Multicast Group
* @param    srcAddr    @b{(input)}   Address of the Multicast Source
* @param    srcMask    @b{(input)}   Address of the Multicast Source Mask
* @param    rpfAddr    @b{(output)}  Pointer to the entry RPF Address
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmExtenApiIpMRouteRpfAddrGet (MCAST_CB_HNDL_t cbHandle,
                                 L7_inet_addr_t *grpAddr,
                                 L7_inet_addr_t *srcAddr,
                                 L7_inet_addr_t *srcMask,
                                 L7_inet_addr_t *rpfAddr)
{
  pimdmCB_t *pimdmCB = L7_NULLPTR;
  pimdmMrtEntry_t *mrtEntry = L7_NULLPTR;
  L7_RC_t retVal = L7_FAILURE;
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if ((pimdmCB = (pimdmCB_t*) cbHandle) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Invalid Ctrl Block");
    return L7_FAILURE;
  }

  if (osapiSemaTake (pimdmCB->mrtSGTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Failed to Acquire PIM-DM MRT Semaphore");
    return L7_FAILURE;
  }

  if ((mrtEntry = pimdmMrtEntryGet (pimdmCB, srcAddr, grpAddr))
      == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                "MRT Lookup Failed for Src - %s, Group - %s",
                inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));
    osapiSemaGive (pimdmCB->mrtSGTree.semId);
    return L7_FAILURE;
  }

  retVal = inetCopy (rpfAddr, &(mrtEntry->rpfInfo.rpfNextHop));

  osapiSemaGive (pimdmCB->mrtSGTree.semId);

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit"); 
  return retVal;
}

/*********************************************************************
*
* @purpose Get the MRT Table Entry Count
*
* @param    cbHandle   @b{(input)}   Handle to the PIM-DM Control Block
* @param    entryCount @b{(output)}  Pointer to the entry count
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmExtenApiIpMRouteEntryCountGet (MCAST_CB_HNDL_t cbHandle,
                                    L7_uint32 *entryCount)
{
  pimdmCB_t* pimdmCB = L7_NULLPTR;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if ((pimdmCB = (pimdmCB_t*) cbHandle) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Invalid Ctrl Block");
    return L7_FAILURE;
  }

  if (osapiSemaTake (pimdmCB->mrtSGTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Failed to Acquire PIM-DM MRT Semaphore");
    return L7_FAILURE;
  }

  *entryCount = pimdmCB->mrtSGTree.count;

  osapiSemaGive (pimdmCB->mrtSGTree.semId);

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the MRT Table Highest Entry Count
*
* @param    cbHandle   @b{(input)}   Handle to the PIM-DM Control Block
* @param    highEntryCount @b{(output)}  Pointer to the Highest Entry Count
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmExtenApiIpMRouteHighestEntryCountGet (MCAST_CB_HNDL_t cbHandle,
                                           L7_uint32 *highEntryCount)
{
  pimdmCB_t* pimdmCB = L7_NULLPTR;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if ((pimdmCB = (pimdmCB_t*) cbHandle) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Invalid Ctrl Block");
    return L7_FAILURE;
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_NOT_SUPPORTED;
}

/*********************************************************************
*
* @purpose  Delete the specified MRT (S,G) Entry
*
* @param    cbHandle   @b{(input)}   Handle to the PIM-DM Control Block
* @param    grpAddr    @b{(input)}   Address of the Multicast Group
* @param    srcAddr    @b{(input)}   Address of the Multicast Source
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmExtenApiRouteDelete (MCAST_CB_HNDL_t cbHandle,
                          L7_inet_addr_t *grpAddr,
                          L7_inet_addr_t *srcAddr)
{
  pimdmCB_t* pimdmCB = L7_NULLPTR;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");
  
  if ((pimdmCB = (pimdmCB_t*) cbHandle) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Invalid Ctrl Block");
    return L7_FAILURE;
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_NOT_SUPPORTED;
}

/*********************************************************************
*
* @purpose  Delete all the entries of the MRT (S,G) Table
*
* @param    cbHandle   @b{(input)}   Handle to the PIM-DM Control Block
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmExtenApiRouteDeleteAll (MCAST_CB_HNDL_t cbHandle)
{
  pimdmCB_t* pimdmCB = L7_NULLPTR;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");
  
  if ((pimdmCB = (pimdmCB_t*) cbHandle) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Invalid Ctrl Block");
    return L7_FAILURE;
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_NOT_SUPPORTED;
}

