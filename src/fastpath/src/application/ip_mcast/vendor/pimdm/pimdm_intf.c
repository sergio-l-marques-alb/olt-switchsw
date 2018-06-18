/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   pimdm_intf.c
*
* @purpose    Interface Related Functionality
*
* @component  PIM-DM
*
* @comments
*
* @create     07/06/2006
*
* @author     Krishna Mohan CS
* @end
*
**********************************************************************/

/*******************************************************************************
**                             Includes                                       **
*******************************************************************************/
#include "l7_common.h"
#include "buff_api.h"
#include "l7apptimer_api.h"
#include "mcast_bitset.h"
#include "l7_pimdm_api.h"
#include "l7_mcast_api.h"
#include "heap_api.h"
#include "pimdm_defs.h"
#include "pimdm_debug.h"
#include "pimdm_ctrl.h"
#include "pimdm_util.h"
#include "pimdm_intf.h"
#include "pimdm_mrt.h"
#include "pimdm_mgmd.h"
#include "pimdm_admin_scope.h"

/*******************************************************************************
**                    Externs & Global declarations                           **
*******************************************************************************/

/*******************************************************************************
**                      Defines & local Typedefs                              **
*******************************************************************************/
/* Enumerations to indicate Neighbor ADD/DELETE/UPDATE operations */
typedef enum
{
  PIMDM_INTF_NBR_ADD = 1,
  PIMDM_INTF_NBR_DELETE,
  PIMDM_INTF_NBR_UPDATE
} PIMDM_INTF_OP_TYPE_t;

/*******************************************************************************
**                        Function Definitions                                **
*******************************************************************************/
/*********************************************************************
*
* @purpose  Compute interface operational parameters from the neighbors
*
* @param    pCB        @b{(input)} Pointer to the PIM-DM Control Block
* @param    pNbr       @b{(input)} Pointer to the neighbor structure
* @param    operation  @b{(input)} Type of neighbor operation (ADD/DELETE/UPDATE)
* @param    pIf        @b{(inout)} Pointer to the interface structure
*
* @returns  None
*
* @comments
*
* @end
*********************************************************************/
static void
pimdmIntfParamsCompute(pimdmCB_t            *pimdmCB,
                       pimdmNeighbor_t      *pNbr,
                       PIMDM_INTF_OP_TYPE_t operation,
                       pimdmInterface_t     *pIf)
{
  pimdmIntfInfo_t *pInfo;
  pimdmNeighbor_t *pTmpNbr;
  L7_BOOL         bNbrFound;
  L7_uint32       i;

  /* Basic sanity checks */
  if((pimdmCB == L7_NULLPTR) || (pIf == L7_NULLPTR) || (pNbr == L7_NULLPTR))
    return;

  /* Compute the interface's operational parameters */
  switch(operation)
  {
    case PIMDM_INTF_NBR_ADD:
      /* New neighbor has just been added */
      if(pNbr->bLanPruneDelayEnabled == L7_TRUE)
      {
        if(pIf->isLanDelayEnabled == L7_TRUE)
        {
          if(pIf->effOverrideIntvl < pNbr->overrideIntvl)
            pIf->overrideIntvl = pNbr->overrideIntvl;
          if(pIf->effPropagationDelay < pNbr->lanPruneDelay)
            pIf->effPropagationDelay = pNbr->lanPruneDelay;
        }
        else if(pIf->numNbrs == 1)
        {
          /* First neighbor on this interface */
          pIf->effOverrideIntvl = pNbr->overrideIntvl;
          pIf->effPropagationDelay = pNbr->lanPruneDelay;
          pIf->isLanDelayEnabled = L7_TRUE;
        }
      }
      else
      {
        pIf->isLanDelayEnabled = L7_FALSE;
        pIf->effOverrideIntvl = L7_PIMDM_DEFAULT_OVERRIDEINTERVAL;
        pIf->effPropagationDelay = L7_PIMDM_DEFAULT_LANPRUNEDELAY;
      }
      if(pNbr->bStateRefreshCapable == L7_TRUE)
      {
        if(pIf->isStateRefreshCapable == L7_TRUE)
        {
          if(pIf->effStateRefreshIntvl > pNbr->stateRefreshIntvl)
            pIf->effStateRefreshIntvl = pNbr->stateRefreshIntvl;
        }
        else if(pIf->numNbrs == 1)
        {
          /* First neighbor on this interface */
          pIf->isStateRefreshCapable = L7_TRUE;
          pIf->effStateRefreshIntvl = pNbr->stateRefreshIntvl;
        }
      }
      else
      {
        pIf->isStateRefreshCapable = L7_FALSE;
        pIf->effStateRefreshIntvl = L7_PIMDM_DEFAULT_STATEREFRESHINTERVAL;
      }
      break;
    case PIMDM_INTF_NBR_DELETE:
      /* If the last neighbor is leaving, set params to defaults */
      if(pIf->numNbrs == 1)
      {
        pIf->isLanDelayEnabled = L7_FALSE;
        pIf->effPropagationDelay = L7_PIMDM_DEFAULT_LANPRUNEDELAY;
        pIf->effOverrideIntvl = L7_PIMDM_DEFAULT_OVERRIDEINTERVAL;
        pIf->isStateRefreshCapable = L7_FALSE;
        pIf->effStateRefreshIntvl = L7_PIMDM_DEFAULT_STATEREFRESHINTERVAL;
        break;
      }
      /* Intentional fall-through */
    case PIMDM_INTF_NBR_UPDATE:
      /* Set the values up for computation */
      bNbrFound = L7_FALSE;
      if ((pInfo = pimdmCB->intfInfo) == L7_NULLPTR)
        return;
      pIf->isLanDelayEnabled = L7_TRUE;
      pIf->effPropagationDelay = 0;
      pIf->effOverrideIntvl = 0;
      pIf->isStateRefreshCapable = L7_TRUE;
      pIf->effStateRefreshIntvl = 0xFFFFFFFF;

      /* Compute by cycling through all the neighbors */
      for(i = 0; i < PIMDM_MAX_NEIGHBORS; i++)
      {
        if(MCAST_BITX_TEST(pIf->nbrBitmap, i) != 0)
        {
          pTmpNbr = &(pInfo->nbrs[i]);
          if(operation == PIMDM_INTF_NBR_DELETE)
          {
            /* Ignore the given neighbor on a DELETE operation */
            if(L7_INET_IS_ADDR_EQUAL(&(pNbr->addr), &(pTmpNbr->addr)) == L7_TRUE)
              continue;
          }
          bNbrFound = L7_TRUE;
          if(pIf->isLanDelayEnabled == L7_TRUE)
          {
            if(pTmpNbr->bLanPruneDelayEnabled == L7_TRUE)
            {
              if(pIf->effPropagationDelay < pTmpNbr->lanPruneDelay)
                pIf->effPropagationDelay = pTmpNbr->lanPruneDelay;
              if(pIf->effOverrideIntvl < pTmpNbr->overrideIntvl)
                pIf->effOverrideIntvl = pTmpNbr->overrideIntvl;
            }
            else
            {
              pIf->isLanDelayEnabled = L7_FALSE;
              pIf->effPropagationDelay = L7_PIMDM_DEFAULT_LANPRUNEDELAY;
              pIf->effOverrideIntvl = L7_PIMDM_DEFAULT_OVERRIDEINTERVAL;
            }
          }
          if(pIf->isStateRefreshCapable == L7_TRUE)
          {
            if(pTmpNbr->bStateRefreshCapable == L7_TRUE)
            {
              if(pIf->effStateRefreshIntvl > pTmpNbr->stateRefreshIntvl)
                pIf->effStateRefreshIntvl = pTmpNbr->stateRefreshIntvl;
            }
            else
            {
              pIf->isStateRefreshCapable = L7_FALSE;
              pIf->effStateRefreshIntvl = L7_PIMDM_DEFAULT_STATEREFRESHINTERVAL;
            }
          }
        }
      }
      if(bNbrFound != L7_TRUE)
      {
        /* If no neighbors, set to defaults */
        pIf->isLanDelayEnabled = L7_FALSE;
        pIf->effPropagationDelay = L7_PIMDM_DEFAULT_LANPRUNEDELAY;
        pIf->effOverrideIntvl = L7_PIMDM_DEFAULT_OVERRIDEINTERVAL;
        pIf->isStateRefreshCapable = L7_FALSE;
        pIf->effStateRefreshIntvl = L7_PIMDM_DEFAULT_STATEREFRESHINTERVAL;
      }
      break;
    default:
      PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Unsupported neighbor operation %d\n", operation);
      break;
  }
}

/*********************************************************************
*
* @purpose  Clear a PIM-DM neighbor information entry
*
* @param    pCB            @b{(input)} Pointer to the PIM-DM Control Block
* @param    rtrIfNum       @b{(input)} Router interface index associated with the neighbor
* @param    nbrIndex       @b{(input)} Internal array index of the neighbor
*
* @returns  L7_TRUE        pim_nbr() bitmap was changed [in the Control Block]
* @returns  L7_FALSE       No change to the pim_nbr() bitmap in the control bolck
*
* @comments This function, in addition to clearing out the neighbor entry for the
*           given neighbor index, also updates the bitmaps in the interface structure
*           as well as in the control block itself as necessary. The return value
*           indicates if the neighbor bitmap in the control block was changed or not.
*           The return value helps is triggering a oif computation as needed.
*
* @end
*********************************************************************/
static L7_BOOL
pimdmIntfNeighborClear(pimdmCB_t *pimdmCB,
                       L7_uint32 rtrIfNum,
                       L7_uint32 nbrIndex)
{
  pimdmInterface_t *pIf;
  pimdmNeighbor_t  *pNbr;
  pimdmIntfInfo_t  *pInfo = L7_NULLPTR;
  L7_uint32        tempGenId;

  /* Basic sanity checks */
  if (pimdmCB == L7_NULLPTR)
    return L7_FALSE;
  if ((rtrIfNum < 1) || (rtrIfNum >= PIMDM_MAX_INTERFACES))
    return L7_FALSE;
  if(nbrIndex >= PIMDM_MAX_NEIGHBORS)
    return L7_FALSE;

  if ((pInfo = pimdmCB->intfInfo) == L7_NULLPTR)
  {
    return L7_FALSE;
  }

  /* Clear the neighbor entry */
  pIf = &(pInfo->intfs[rtrIfNum]);
  pNbr = &(pInfo->nbrs[nbrIndex]);

  /* Release the J/P Working Buffer */
  pimdmJPWorkingBuffReturn (pimdmCB, pNbr);

  pimdmIntfParamsCompute(pimdmCB, pNbr, PIMDM_INTF_NBR_DELETE, pIf);
  tempGenId = pNbr->livenessTimerGenId;
  memset(pNbr, 0, sizeof(pimdmNeighbor_t));
  pNbr->livenessTimerGenId = tempGenId;
  if(MCAST_BITX_TEST(pIf->nbrBitmap, nbrIndex) != 0)
  {
    MCAST_BITX_RESET(pIf->nbrBitmap, nbrIndex);
    pIf->numNbrs -= 1;
    if(pIf->numNbrs == 0)
    {
      BITX_RESET(&pimdmCB->nbrBitmap, rtrIfNum);
      return L7_TRUE;
    }
  }
  return L7_FALSE;
}

/*********************************************************************
*
* @purpose Retrieve the array index for a neighbor
*
* @param    pCB            @b{(input)} Pointer to the PIM-DM Control Block
* @param    pNbrAddr       @b{(input)} Pointer to the Neighbor's IP Address
*
* @returns  nnn            Array index of the entry, if found
* @returns  -nnn           Indication of the probably insertion point, if not found
*
* @comments If the neighbor address is not found in the internal array, then
*           a negative number generated by the algorithm : [-nnn - 1] would
*           be returned to the caller; where nnn is the first available
*           location in the internal array where a neighbor could be inserted.
* @comments The negative number returning algorithm avoids one extra walk on the
*           whole array for an insertion of a new neighbor, wherein the application
*           usually verifies the presence of the neighbor before adding it as new.
*
* @end
*********************************************************************/
static L7_int32
pimdmIntfNeighborIndexGet(pimdmCB_t      *pimdmCB,
                          L7_inet_addr_t *pNbrAddr,
                          L7_uint32 rtrIfNum)
{
  L7_int32        insertionIndex;
  L7_inet_addr_t  zeroInet;
  L7_uint32       i;
  pimdmNeighbor_t *pNbr;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  insertionIndex = PIMDM_MAX_NEIGHBORS;   /* Initialize to a bad insertion point */

  /* Basic sanity checks */
  if(pimdmCB == L7_NULLPTR)
    return (-insertionIndex - 1);
  if(pNbrAddr == L7_NULLPTR)
    return (-insertionIndex - 1);
   if(inetIsAddressZero(pNbrAddr)==L7_TRUE)
     return(-insertionIndex - 1);
  /* Search through the array for the given neighbor, while
   * keeping track of any free slots
   */
  inetAddressZeroSet (pimdmCB->addrFamily, &zeroInet);
  for(i = 0; i < PIMDM_MAX_NEIGHBORS; i++)
  {
    pNbr = &(pimdmCB->intfInfo->nbrs[i]);

    if(L7_INET_IS_ADDR_EQUAL(pNbrAddr, &(pNbr->addr)) == L7_TRUE)
    {
      if((rtrIfNum == pNbr->rtrIfNum) || (rtrIfNum ==0))
      {
        return i;
      }
    }

    if(insertionIndex == PIMDM_MAX_NEIGHBORS)
    {
      if (L7_INET_ADDR_COMPARE (&zeroInet, &(pNbr->addr)) == 0)
      {
        insertionIndex = i;
      }
    }
  }

  /* A matching entry is not found; return a probable insertion point */
  return (-insertionIndex - 1);
}

/*********************************************************************
*
* @purpose Process a Hello Timer expiry
*
* @param   pParam  @b{(input)} Pointer to the hello timer handle
*
* @returns None
*
* @comments
*
* @end
*********************************************************************/
static void
pimdmIntfHelloTimerExpiryHandler (void *pParam)
{
  pimdmInterface_t *pIf = L7_NULLPTR;
  pimdmCB_t *pimdmCB = L7_NULLPTR;
  pimdmIntfNbrInfo_t nbrInfo;
  L7_uint32 rtrIfNum;
  L7_uchar8 errMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];
  L7_uchar8 sucMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if ((pIf = (pimdmInterface_t*) handleListNodeRetrieve ((L7_uint32) pParam))
          == L7_NULLPTR)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
               "Invalid interface info [NULL] in Hello Timer expiry Handle\n");
    return;
  }

  pimdmCB = pIf->pimdmCB;
  rtrIfNum = pIf->rtrIfNum;

  if (pIf->helloTimer == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_PIMDM_MAP_COMPONENT_ID,
            "PIMDM helloTimer (HT) is NULL, But Still Expired");
    return;
  }

  /* Hello Timer has expired.  Update it to NULL.
   */
  pIf->helloTimer = L7_NULLPTR;

  /* Send the Hello Packet */
  memset(&nbrInfo, 0, sizeof(pimdmIntfNbrInfo_t));
  nbrInfo.holdTime = pIf->helloHoldTime;
  nbrInfo.bValidLanPruneDelay = L7_TRUE;
  nbrInfo.lanPruneDelay = pIf->propagationDelay;
  nbrInfo.overrideInterval = pIf->overrideIntvl;
  nbrInfo.bValidGenId = L7_TRUE;
  nbrInfo.genId = pIf->genID;
  nbrInfo.bStateRefreshCapable = L7_TRUE;
  nbrInfo.stateRefreshInterval = pIf->effStateRefreshIntvl;

  if(pimdmHelloMsgSend (pimdmCB, rtrIfNum, &nbrInfo) != L7_SUCCESS)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                "Failed sending Hello Message on rtrIfNum %d", rtrIfNum);
  }

  osapiSnprintf (errMsgBuf,PIMDM_MAX_DBG_MSG_SIZE,
                 "Hello timer refresh failed on rtrIfNum %d", rtrIfNum);
  osapiSnprintf (sucMsgBuf,PIMDM_MAX_DBG_MSG_SIZE,
                 "Hello timer refresh success on rtrIfNum %d", rtrIfNum);
  /* Restart the Hello Timer */
  if (pimdmUtilAppTimerSet (pimdmCB, pimdmIntfHelloTimerExpiryHandler,
                            (void*) pIf->intfTmrHandle, pIf->helloInterval,
                            errMsgBuf, sucMsgBuf, &(pIf->helloTimer),
                            "DM-HT")
                         != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                 "Failed to start the Hello Timer for rtrIfNum %d", rtrIfNum);
    handleListNodeDelete (pimdmCB->handleList, &(pIf->intfTmrHandle));
    return;
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit.");
}

/*********************************************************************
*
* @purpose Process a Neighbor Liveness Timer expiry
*
* @param   pParam   @b{(input)} Pointer to the neighbor liveness timer handle
*
* @returns None
*
* @comments
*
* @end
*********************************************************************/
static void
pimdmIntfNbrTimerExpiryHandler (void *pParam)
{
  pimdmCB_t *pimdmCB = L7_NULLPTR;
  pimdmNeighbor_t *pNbr = L7_NULLPTR;
  L7_int32 nbrIndex,numNbrs=-1;
  L7_uint32 rtrIfNum = 0;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if ((pNbr = (pimdmNeighbor_t*) handleListNodeRetrieve ((L7_uint32) pParam))
          == L7_NULLPTR)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
               "Invalid neighbor info [NULL] in NLT expiry Handle");
    return;
  }

  if ((pimdmCB = pNbr->pimdmCB) == L7_NULLPTR)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                "Invalid PIM-DM Control Block");
    return;
  }

  if (pNbr->livenessTimer == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_PIMDM_MAP_COMPONENT_ID,
            "PIMDM livenessTimer (NLT) is NULL, But Still Expired");
    return;
  }

  rtrIfNum = pNbr->rtrIfNum;
  nbrIndex = pimdmIntfNeighborIndexGet (pimdmCB, &(pNbr->addr),pNbr->rtrIfNum);

  if((nbrIndex < 0) || (nbrIndex >= PIMDM_MAX_NEIGHBORS))
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Invalid Neighbor Index");
    return;
  }

  /* Neighbor Timer has expired.  Update it to NULL.
   */
  pNbr->livenessTimer = L7_NULLPTR;

  /* Notify the MRT of expiry of a Neighbor Liveness Timer */
  if((numNbrs = pimdmIntfNumNeighborsGet(pimdmCB,pNbr->rtrIfNum)) > 0)
  {
    pimdmMrtNLTExpiryProcess(pimdmCB, &(pNbr->addr), pNbr->rtrIfNum, numNbrs);
  }

  /* Clean-up the neighbor entry */
  if (pimdmIntfNeighborClear (pimdmCB, pNbr->rtrIfNum, nbrIndex) == L7_TRUE)
  {
    /* Generate an SNMP Trap for the Neighbor Loss */
    pimdmMapPimTrapNeighborLoss (pimdmCB->addrFamily, rtrIfNum);

    /* Notify the MRT of change to the pim_nbrs() bitmap in the control block */
    pimdmMrtOifListCompute(pimdmCB, L7_NULLPTR, L7_NULLPTR, PIMDM_MRT_CHANGE_NBR_LIST);
  }

  return;
}

/*********************************************************************
*
* @purpose  Check if PIM-DM is enabled on an interface
*
* @param    pCB            @b{(input)} Pointer to the PIM-DM Control Block
* @param    rtrIfNum       @b{(input)} Router Interface Index of the interface
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL
pimdmIntfIsEnabled (pimdmCB_t *pimdmCB,
                    L7_uint32 rtrIfNum)
{
  pimdmIntfInfo_t *pInfo = L7_NULLPTR;
  pimdmInterface_t *pIf = L7_NULLPTR;

  /* PIMDM_TRACE (PIMDM_DEBUG_API, "Entry"); */

  /* Validations */
  if (rtrIfNum >= PIMDM_MAX_INTERFACES)
    return L7_FALSE;
  if (pimdmCB == L7_NULLPTR)
    return L7_FALSE;

  /* Retrieve the interface entry details and check */
  if ((pInfo = pimdmCB->intfInfo) == L7_NULLPTR)
  {
    return L7_FALSE;
  }
  pIf = &(pInfo->intfs[rtrIfNum]);
  if (pIf->rtrIfNum == 0)
  {
    return L7_FALSE;
  }

  /* PIMDM_TRACE (PIMDM_DEBUG_API, "Exit."); */
  return L7_TRUE;
}

/*********************************************************************
*
* @purpose  Retrieve the internal interface number for a router interface
*
* @param    pCB            @b{(input)} Pointer to the PIM-DM Control Block
* @param    rtrIfNum       @b{(input)} Router Interface Index of the interface
* @param    pIntIfNum      @b{(output)} Pointer to storage for Internal If Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This function is provided to aid in delayed retrieval of Internal
*           Interface numbers for Router Interface Numbers. The standard IPMAP
*           routines would have purged the internal interface number to router
*           interface number mappings once the router interface is deleted.
* @comments In the PIM-DM implementation of this mapping, the rtrIfNum<->intIfNum
*           is maintained forever until overwritten.
*
* @end
*********************************************************************/
L7_RC_t
pimdmIntfIntIfNumGet (pimdmCB_t *pimdmCB,
                      L7_uint32 rtrIfNum,
                      L7_uint32 *pIntIfNum)
{
  pimdmIntfInfo_t *pInfo;
  L7_int32        intIfNum;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Validations */
  if (rtrIfNum >= PIMDM_MAX_INTERFACES)
    return L7_FAILURE;
  if (pimdmCB == L7_NULLPTR)
    return L7_FAILURE;

  /* Retrieve the mapped internal interface number */
  if ((pInfo = pimdmCB->intfInfo) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  intIfNum = pInfo->intIfNum[rtrIfNum];
  if (intIfNum == -1)
  {
    return L7_FAILURE;
  }

  *pIntIfNum = (L7_uint32)intIfNum;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Retrieve the IP address for a router interface
*
* @param    pCB            @b{(input)} Pointer to the PIM-DM Control Block
* @param    rtrIfNum       @b{(input)} Router Interface Index of the interface
* @param    pIpAddress     @b{(output)} Pointer to storage for IP Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmIntfIpAddressGet (pimdmCB_t      *pimdmCB,
                       L7_uint32      rtrIfNum,
                       L7_inet_addr_t *pIpAddress)
{
  pimdmIntfInfo_t *pInfo = L7_NULLPTR;
  pimdmInterface_t *pIf = L7_NULLPTR;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Validations */
  if (rtrIfNum >= PIMDM_MAX_INTERFACES)
    return L7_FAILURE;
  if (pimdmCB == L7_NULLPTR)
    return L7_FAILURE;
  if ((pInfo = pimdmCB->intfInfo) == L7_NULLPTR)
    return L7_FAILURE;
  if (pIpAddress == L7_NULLPTR)
    return L7_FAILURE;

  /* Lock the module */
  if(osapiSemaTake(pInfo->semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  /* Retrieve the mapped internal interface number */
  pIf = &(pInfo->intfs[rtrIfNum]);
  if (pIf->rtrIfNum == 0)
  {
    osapiSemaGive(pInfo->semId);
    return L7_FAILURE;
  }

  inetCopy(pIpAddress, &(pIf->ipAddress));
  osapiSemaGive(pInfo->semId);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Retrieve the Network Mask for a router interface
*
* @param    pCB            @b{(input)} Pointer to the PIM-DM Control Block
* @param    rtrIfNum       @b{(input)} Router Interface Index of the interface
* @param    pNetMask       @b{(output)} Pointer to storage for Network Mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmIntfNetMaskGet (pimdmCB_t      *pimdmCB,
                     L7_uint32      rtrIfNum,
                     L7_inet_addr_t *pNetMask)
{
  pimdmIntfInfo_t *pInfo = L7_NULLPTR;
  pimdmInterface_t *pIf = L7_NULLPTR;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Validations */
  if (rtrIfNum >= PIMDM_MAX_INTERFACES)
    return L7_FAILURE;
  if (pimdmCB == L7_NULLPTR)
    return L7_FAILURE;
  if ((pInfo = pimdmCB->intfInfo) == L7_NULLPTR)
    return L7_FAILURE;
  if (pNetMask == L7_NULLPTR)
    return L7_FAILURE;

  /* Lock the module */
  if(osapiSemaTake(pInfo->semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  /* Retrieve the mapped internal interface number */
  pIf = &(pInfo->intfs[rtrIfNum]);
  if (pIf->rtrIfNum == 0)
  {
    osapiSemaGive(pInfo->semId);
    return L7_FAILURE;
  }

  inetCopy(pNetMask, &(pIf->netMask));
  osapiSemaGive(pInfo->semId);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Handler for Mapping Layer Interface admin Mode.
*
* @param    pimdmCbHandle    @b{ (input) } PIM-DM Ctrl Block Handle
* @param    mapLayerInfo     @b{ (input) } Mapping Layer Event Info
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments All the SET commands will be posted to the Vendor by the
*           PIM-DM Mapping Layer.
*
* @end
*********************************************************************/
L7_RC_t
pimdmIntfAdminModeSet (MCAST_CB_HNDL_t pimdmCbHandle,
                       pimdmMapEvent_t *mapLayerInfo)
{
  pimdmMapIntfMode_t *intfMode = L7_NULLPTR;
  L7_RC_t             retVal = L7_FAILURE;
  pimdmCB_t          *pimdmCB = (pimdmCB_t *) pimdmCbHandle;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if ((pimdmCB == L7_NULLPTR) || (mapLayerInfo == L7_NULLPTR))
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE, "Inputs Validation Failed");
    return L7_FAILURE;
  }
  if (pimdmCB->operMode == L7_FALSE)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE, "PIM-DM is Not Operational");
    return L7_FAILURE;
  }

  intfMode = &(mapLayerInfo->msg.intfMode);

  PIMDM_TRACE (PIMDM_DEBUG_EVENTS, "MCAST_EVENT_PIMDMMAP_INTF_ADMINMODE_SET"
                " Event for Interface %d with Mode %d", intfMode->rtrIfNum,
                intfMode->mode);

  if (intfMode->mode == L7_ENABLE)
  {
    /* Create the PIM-DM Interface */
    retVal = pimdmIntfUp (pimdmCB, intfMode->rtrIfNum, intfMode->intIfNum,
                     intfMode->helloInterval, intfMode->triggeredHelloDelay,
                     intfMode->helloHoldTime, intfMode->propagationDelay,
                     intfMode->overrideIntvl, &(intfMode->intfAddr),
                     &(intfMode->intfMask));
    if (retVal != L7_SUCCESS)
    {
      PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "PIM-DM Interface Activation Failed "
                   "for rtrIfNum - %d", intfMode->rtrIfNum);
      pimdmIntfPurge (pimdmCB, intfMode->rtrIfNum);
    }
  }
  else if (intfMode->mode == L7_DISABLE)
  {
    retVal = pimdmIntfDown (pimdmCB, intfMode->rtrIfNum);
    if (retVal != L7_SUCCESS)
    {
      PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "PIM-DM Interface De-Activation Failed "
                   "for rtrIfNum - %d", intfMode->rtrIfNum);
    }
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return retVal;
}

/*********************************************************************
*
* @purpose  Modify and update the Hello interval for an interface
*
* @param    pimdmCbHandle  @b{(input)} PIM-DM Control Block Handle
* @param    mapLayerInfo   @b{(input)} Mapping Layer Info.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmIntfHelloIntervalSet (MCAST_CB_HNDL_t pimdmCbHandle,
                           pimdmMapEvent_t *mapLayerInfo)
{
  pimdmCB_t        *pimdmCB = (pimdmCB_t *)pimdmCbHandle;
  pimdmIntfInfo_t *pInfo = L7_NULLPTR;
  pimdmInterface_t *pIf = L7_NULLPTR;
  L7_uint32        rtrIfNum = mapLayerInfo->msg.helloIntrvl.rtrIfNum;
  L7_uint32        helloInterval = mapLayerInfo->msg.helloIntrvl.intrvl;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Validations */
  if (rtrIfNum >= PIMDM_MAX_INTERFACES)
    return L7_FAILURE;
  if (pimdmCB == L7_NULLPTR)
    return L7_FAILURE;
  if ((pInfo = pimdmCB->intfInfo) == L7_NULLPTR)
    return L7_FAILURE;
  if(helloInterval == 0)
    return L7_FAILURE;

  /* Lock the module */
  if(osapiSemaTake(pInfo->semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  /* Retrieve the mapped internal interface number */
  pIf = &(pInfo->intfs[rtrIfNum]);
  if (pIf->rtrIfNum == 0)
  {
    osapiSemaGive(pInfo->semId);
    return L7_FAILURE;
  }

  pIf->helloInterval = helloInterval;
  pIf->helloHoldTime = (35 * helloInterval) / 10;

  osapiSemaGive(pInfo->semId);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Retrieve the Hello interval for an interface
*
* @param    pCB            @b{(input)} Pointer to the PIM-DM Control Block
* @param    rtrIfNum       @b{(input)} Router Interface Index of the interface
* @param    pHelloInterval @b{(output)} Pointer to the retrieved Hello Interval in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmIntfHelloIntervalGet (pimdmCB_t *pimdmCB,
                           L7_uint32 rtrIfNum,
                           L7_uint32 *pHelloInterval)
{
  pimdmIntfInfo_t *pInfo = L7_NULLPTR;
  pimdmInterface_t *pIf = L7_NULLPTR;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Validations */
  if (rtrIfNum >= PIMDM_MAX_INTERFACES)
    return L7_FAILURE;
  if (pimdmCB == L7_NULLPTR)
    return L7_FAILURE;
  if ((pInfo = pimdmCB->intfInfo) == L7_NULLPTR)
    return L7_FAILURE;

  /* Lock the module */
  if(osapiSemaTake(pInfo->semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  /* Retrieve the mapped internal interface number */
  pIf = &(pInfo->intfs[rtrIfNum]);
  if (pIf->rtrIfNum == 0)
  {
    osapiSemaGive(pInfo->semId);
    return L7_FAILURE;
  }

  *pHelloInterval = pIf->helloInterval;

  osapiSemaGive(pInfo->semId);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Check if an interface is capable of processing State-Refresh messages
*
* @param    pCB            @b{(input)} Pointer to the PIM-DM Control Block
* @param    rtrIfNum       @b{(input)} Router Interface Index of the interface
*
* @returns  L7_TRUE        State-Refresh capable
* @returns  L7_FALSE       Not State-Refresh capable
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL
pimdmIntfIsStateRefreshCapable (pimdmCB_t *pimdmCB,
                                L7_uint32 rtrIfNum)
{
  pimdmIntfInfo_t  *pInfo = L7_NULLPTR;
  pimdmInterface_t *pIf = L7_NULLPTR;
  L7_BOOL          retVal;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Validations */
  if (rtrIfNum >= PIMDM_MAX_INTERFACES)
    return L7_FALSE;
  if (pimdmCB == L7_NULLPTR)
    return L7_FALSE;
  if ((pInfo = pimdmCB->intfInfo) == L7_NULLPTR)
    return L7_FALSE;

  /* Lock the module */
  if(osapiSemaTake(pInfo->semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FALSE;

  /* Retrieve the mapped internal interface number */
  pIf = &(pInfo->intfs[rtrIfNum]);
  if (pIf->rtrIfNum == 0)
  {
    osapiSemaGive(pInfo->semId);
    return L7_FALSE;
  }

  retVal = pIf->isStateRefreshCapable;

  osapiSemaGive(pInfo->semId);
  return retVal;
}

/*********************************************************************
*
* @purpose  Get LAN Propagation Delay on an interface
*
* @param    pCB            @b{(input)} Pointer to the PIM-DM Control Block
* @param    rtrIfNum       @b{(input)} Router Interface Index of the interface
*
* @returns  -1             Error retrieving the value
* @returns  nnnnn          LAN propagation delay in milliseconds
*
* @comments
*
* @end
*********************************************************************/
L7_int32
pimdmIntfLanPropagationDelayGet (pimdmCB_t *pimdmCB,
                                 L7_uint32 rtrIfNum)
{
  pimdmIntfInfo_t  *pInfo = L7_NULLPTR;
  pimdmInterface_t *pIf = L7_NULLPTR;
  L7_int32         retVal;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Validations */
  if (rtrIfNum >= PIMDM_MAX_INTERFACES)
    return -1;
  if (pimdmCB == L7_NULLPTR)
    return -1;
  if ((pInfo = pimdmCB->intfInfo) == L7_NULLPTR)
    return -1;

  /* Lock the module */
  if(osapiSemaTake(pInfo->semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    return -1;

  /* Retrieve the interface entry details and return the value */
  pIf = &(pInfo->intfs[rtrIfNum]);
  if (pIf->rtrIfNum == 0)
  {
    osapiSemaGive(pInfo->semId);
    return -1;
  }

  retVal = pIf->effPropagationDelay;

  osapiSemaGive(pInfo->semId);
  return retVal;
}

/*********************************************************************
*
* @purpose  Get Override Interval for an interface
*
* @param    pCB            @b{(input)} Pointer to the PIM-DM Control Block
* @param    rtrIfNum       @b{(input)} Router Interface Index of the interface
*
* @returns  -1             Error retrieving the value
* @returns  nnnnn          Override interval in milliseconds
*
* @comments
*
* @end
*********************************************************************/
L7_int32
pimdmIntfOverrideIntervalGet (pimdmCB_t *pimdmCB,
                              L7_uint32 rtrIfNum)
{
  pimdmIntfInfo_t  *pInfo = L7_NULLPTR;
  pimdmInterface_t *pIf = L7_NULLPTR;
  L7_int32         retVal;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Validations */
  if (rtrIfNum >= PIMDM_MAX_INTERFACES)
    return -1;
  if (pimdmCB == L7_NULLPTR)
    return -1;
  if ((pInfo = pimdmCB->intfInfo) == L7_NULLPTR)
    return -1;

  /* Lock the module */
  if(osapiSemaTake(pInfo->semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    return -1;

  /* Retrieve the interface entry details, and return the value */
  pIf = &(pInfo->intfs[rtrIfNum]);
  if (pIf->rtrIfNum == 0)
  {
    osapiSemaGive(pInfo->semId);
    return -1;
  }

  retVal = pIf->effOverrideIntvl;

  osapiSemaGive(pInfo->semId);
  return retVal;
}

/*********************************************************************
*
* @purpose  Check if a PIM-DM neighbor is present/valid on an interface
*
* @param    pCB            @b{(input)} Pointer to the PIM-DM Control Block
* @param    rtrIfNum       @b{(input)} Router Interface Index of the interface
* @param    pNbrAddr       @b{(input)} Pointer to the PIM-DM neighbor's IP address
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL
pimdmIntfIsNeighborValid (pimdmCB_t      *pimdmCB,
                          L7_uint32      rtrIfNum,
                          L7_inet_addr_t *pNbrAddr)
{
  pimdmIntfInfo_t  *pInfo = L7_NULLPTR;
  pimdmInterface_t *pIf = L7_NULLPTR;
  L7_uint32        i;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Validations */
  if (rtrIfNum >= PIMDM_MAX_INTERFACES)
    return L7_FALSE;
  if (pimdmCB == L7_NULLPTR)
    return L7_FALSE;
  if ((pInfo = pimdmCB->intfInfo) == L7_NULLPTR)
    return L7_FALSE;
  if (pNbrAddr == L7_NULLPTR)
    return L7_FALSE;

  /* Lock the module */
  if(osapiSemaTake(pInfo->semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FALSE;

  /* Retrieve the interface entry details */
  pIf = &(pInfo->intfs[rtrIfNum]);
  if (pIf->rtrIfNum == 0)
  {
    osapiSemaGive(pInfo->semId);
    return L7_FALSE;   /* Interface not valid for PIM-DM */
  }

  /* Check all the neighbors on the interface if any match the given
   * IP Address
   */
  for (i = 0; i < PIMDM_MAX_NEIGHBORS; i++)
  {
    if (MCAST_BITX_TEST(pIf->nbrBitmap, i) != 0)
    {
      pimdmNeighbor_t *pNbr =  L7_NULLPTR;

      pNbr = &(pInfo->nbrs[i]);
      if (L7_INET_IS_ADDR_EQUAL(pNbrAddr, &(pNbr->addr)) == L7_TRUE)
      {
        osapiSemaGive(pInfo->semId);
        return L7_TRUE;
      }
    }
  }

  osapiSemaGive(pInfo->semId);
  return L7_FALSE;
}

/*********************************************************************
*
* @purpose  Get number of PIM-DM neighbors on an interface
*
* @param    pCB            @b{(input)} Pointer to the PIM-DM Control Block
* @param    rtrIfNum       @b{(input)} Router Interface Index of the interface
*
* @returns  -1             Error retrieving the value
* @returns  nnnnn          Number of PIM-DM neighbors
*
* @comments
*
* @end
*********************************************************************/
L7_int32
pimdmIntfNumNeighborsGet (pimdmCB_t *pimdmCB,
                          L7_uint32 rtrIfNum)
{
  pimdmIntfInfo_t  *pInfo = L7_NULLPTR;
  pimdmInterface_t *pIf = L7_NULLPTR;
  L7_int32         numNbrs;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Validations */
  if (rtrIfNum >= PIMDM_MAX_INTERFACES)
    return -1;
  if (pimdmCB == L7_NULLPTR)
    return -1;
  if ((pInfo = pimdmCB->intfInfo) == L7_NULLPTR)
    return -1;

  /* Lock the module */
  if(osapiSemaTake(pInfo->semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  /* Retrieve the interface entry details, and return the value */
  pIf = &(pInfo->intfs[rtrIfNum]);
  if (pIf->rtrIfNum == 0)
  {
    osapiSemaGive(pInfo->semId);
    return -1;
  }

  numNbrs = pIf->numNbrs;

  osapiSemaGive(pInfo->semId);
  return numNbrs;
}

/*********************************************************************
*
* @purpose  Retrieve the Router Interface Index for a PIM-DM neighbor
*
* @param    pCB          @b{(input)} Pointer to the PIM-DM Control Block
* @param    pNbrAddr     @b{(input)} Pointer to the neighbor address
* @param    pRtrIfNum    @b{(output)} Pointer to the router interface index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmIntfNeighborRtrIfIndexGet (pimdmCB_t      *pimdmCB,
                                L7_inet_addr_t *pNbrAddr,
                                L7_uint32      *pRtrIfNum)

{
  pimdmIntfInfo_t  *pInfo = L7_NULLPTR;
  L7_int32 nbrIndex;

  /* Validations */
  if(pimdmCB == L7_NULLPTR)
    return L7_FAILURE;
  if ((pInfo = pimdmCB->intfInfo) == L7_NULLPTR)
    return L7_FAILURE;
  if(pNbrAddr == L7_NULLPTR)
    return L7_FAILURE;
  if(pRtrIfNum == L7_NULLPTR)
    return L7_FAILURE;

  /* Lock the module */
  if(osapiSemaTake(pInfo->semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  /* Check if neighbor present in our storage */
  nbrIndex = pimdmIntfNeighborIndexGet(pimdmCB, pNbrAddr,L7_NULL);
  if((nbrIndex < 0) || (nbrIndex >= PIMDM_MAX_NEIGHBORS))
  {
    osapiSemaGive(pInfo->semId);
    return L7_FAILURE;
  }

  *pRtrIfNum = pInfo->nbrs[nbrIndex].rtrIfNum;

  osapiSemaGive(pInfo->semId);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Retrieve the UP Time for a PIM-DM neighbor
*
* @param    pCB            @b{(input)} Pointer to the PIM-DM Control Block
* @param    pNbrAddr       @b{(input)} Pointer to the neighbor address
* @param    rtrIfNum       @b{(input)} router interface number
* @param    pUpTime        @b{(output)} Pointer to the UP time storage
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmIntfNeighborUpTimeGet (pimdmCB_t      *pimdmCB,
                            L7_inet_addr_t *pNbrAddr,
                            L7_uint32      rtrIfNum,
                            L7_uint32      *pUpTime)
{
  pimdmIntfInfo_t *pInfo = L7_NULLPTR;
  L7_int32  nbrIndex;
  L7_uint32 timeNow;

  /* Validations */
  if(pimdmCB == L7_NULLPTR)
    return L7_FAILURE;
  if ((pInfo = pimdmCB->intfInfo) == L7_NULLPTR)
    return L7_FAILURE;
  if(pNbrAddr == L7_NULLPTR)
    return L7_FAILURE;
  if(pUpTime == L7_NULLPTR)
    return L7_FAILURE;

  timeNow = osapiUpTimeRaw();

  /* Lock the module */
  if(osapiSemaTake(pInfo->semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  /* Check if neighbor present in our storage */
  nbrIndex = pimdmIntfNeighborIndexGet(pimdmCB, pNbrAddr,rtrIfNum);
  if((nbrIndex < 0) || (nbrIndex >= PIMDM_MAX_NEIGHBORS))
  {
    osapiSemaGive(pInfo->semId);
    return L7_FAILURE;
  }

  *pUpTime = timeNow - (pInfo->nbrs[nbrIndex].createTime);

  osapiSemaGive(pInfo->semId);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Retrieve the Expiry Time for a PIM-DM neighbor
*
* @param    pCB            @b{(input)} Pointer to the PIM-DM Control Block
* @param    pNbrAddr       @b{(input)} Pointer to the neighbor address
* @param    pExpiryTime    @b{(output)} Pointer to the expiry time storage
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The Expiry Time is returned as seconds since the system was
*           started/powered-up.
*
* @end
*********************************************************************/
L7_RC_t
pimdmIntfNeighborExpiryTimeGet (pimdmCB_t      *pimdmCB,
                                L7_inet_addr_t *pNbrAddr,
                                L7_uint32       rtrIfNum,
                                L7_uint32      *pExpiryTime)
{
  pimdmNeighbor_t *pNbr = L7_NULLPTR;
  pimdmIntfInfo_t *pInfo = L7_NULLPTR;
  L7_int32        nbrIndex;

  /* Validations */
  if(pimdmCB == L7_NULLPTR)
    return L7_FAILURE;
  if ((pInfo = pimdmCB->intfInfo) == L7_NULLPTR)
    return L7_FAILURE;
  if(pNbrAddr == L7_NULLPTR)
    return L7_FAILURE;
  if(pExpiryTime == L7_NULLPTR)
    return L7_FAILURE;

  /* Lock the module */
  if(osapiSemaTake(pInfo->semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  /* Check if neighbor present in our storage */
  nbrIndex = pimdmIntfNeighborIndexGet(pimdmCB, pNbrAddr,rtrIfNum);
  if((nbrIndex < 0) || (nbrIndex >= PIMDM_MAX_NEIGHBORS))
  {
    osapiSemaGive(pInfo->semId);
    return L7_FAILURE;
  }
  pNbr = &(pInfo->nbrs[nbrIndex]);
  if((pNbr->holdTime == PIMDM_INFINITE_HOLDTIME) ||
     (pNbr->livenessTimer == L7_NULLPTR))
  {
    /* Never to expire */
    *pExpiryTime = pNbr->holdTime;
  }
  else
  {
    if(appTimerTimeLeftGet(pimdmCB->appTimer, pNbr->livenessTimer, pExpiryTime) != L7_SUCCESS)
    {
      /* Timer must have expired but not processed yet */
      *pExpiryTime = 0;
    }
  }

  osapiSemaGive(pInfo->semId);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Check for the presence of a PIM-DM neighbor
*
* @param    pCB            @b{(input)} Pointer to the PIM-DM Control Block
* @param    pNbrAddr       @b{(input)} Pointer to the neighbor address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmIntfNeighborGet (pimdmCB_t      *pimdmCB,
                      L7_inet_addr_t *pNbrAddr)
{
  pimdmIntfInfo_t *pInfo = L7_NULLPTR;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Validations */
  if(pimdmCB == L7_NULLPTR)
    return L7_FAILURE;
  if ((pInfo = pimdmCB->intfInfo) == L7_NULLPTR)
    return L7_FAILURE;
  if (pNbrAddr == L7_NULLPTR)
    return L7_FAILURE;

  /* Lock the module */
  if(osapiSemaTake(pInfo->semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  /* Check if neighbor present in our storage */
  if(pimdmIntfNeighborIndexGet(pimdmCB, pNbrAddr,L7_NULL) < 0)
  {
    osapiSemaGive(pInfo->semId);
    return L7_FAILURE;
  }

  osapiSemaGive(pInfo->semId);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Check for the presence of a PIM-DM neighbor on the specified interface
*
* @param    pimdmCB        @b{(input)} Pointer to the PIM-DM Control Block
* @param    rtrIfNum       @b{(input)} Router Interface Number
* @param    pNbrAddr       @b{(input)} Pointer to the neighbor address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments If an exact entry with the given neighbor address is found,
*           then the same neighbor address is returned. If not, the next
*           numerically higher neighbor address found in the system is returned.
*
* @end
*********************************************************************/
L7_RC_t
pimdmIntfNeighborByIfIndexGet (pimdmCB_t *pimdmCB,
                               L7_uint32 rtrIfNum,
                               L7_inet_addr_t *pNbrAddr)
{
  pimdmIntfInfo_t *pInfo = L7_NULLPTR;
  pimdmNeighbor_t *pNbr;
  L7_uint32       i;
  L7_inet_addr_t  zeroInet;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Validations */
  if(pimdmCB == L7_NULLPTR)
    return L7_FAILURE;
  if ((pInfo = pimdmCB->intfInfo) == L7_NULLPTR)
    return L7_FAILURE;
  if (pNbrAddr == L7_NULLPTR)
    return L7_FAILURE;

  inetAddressZeroSet (pimdmCB->addrFamily, &zeroInet);

  for(i = 0; i < PIMDM_MAX_NEIGHBORS; i++)
  {
    pNbr = &(pInfo->nbrs[i]);
    if (L7_INET_ADDR_COMPARE (&zeroInet, &(pNbr->addr)) == 0)
    {
      continue;
    }
    if (L7_INET_ADDR_COMPARE(pNbrAddr, &(pNbr->addr)) == 0)
    {
      if(rtrIfNum == pNbr->rtrIfNum)
      {
        return L7_SUCCESS;
      }
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Retrieve the GetNext PIM-DM neighbor address
*
* @param    pCB            @b{(input)} Pointer to the PIM-DM Control Block
* @param    pNbrAddr       @b{(inout)} Pointer to the neighbor address
* @param    outRtrIfNum    @b{(inout)} Pointer to rtrIfNum
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments If an exact entry with the given neighbor address is found,
*           then the same neighbor address is returned. If not, the next
*           numerically higher neighbor address found in the system is returned.
*
* @end
*********************************************************************/
L7_RC_t
pimdmIntfNeighborByIfIndexNextGet (pimdmCB_t      *pimdmCB,
                          L7_inet_addr_t *pNbrAddr,
                          L7_uint32 *outRtrIfNum)
{
  pimdmIntfInfo_t *pInfo = L7_NULLPTR;
  pimdmNeighbor_t *pNbr;
  L7_inet_addr_t  *pBestMatch;
  L7_int32        compare;
  L7_uint32       i;
  L7_uint32       nbrIndex = 0;
  L7_inet_addr_t  zeroInet;
  L7_inet_addr_t  nbrAddr;
  L7_uint32 rtrIfNumStart =0,rtrIfNum =0;
  L7_BOOL found = L7_FALSE;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Validations */
  if(pimdmCB == L7_NULLPTR)
    return L7_FAILURE;
  if ((pInfo = pimdmCB->intfInfo) == L7_NULLPTR)
    return L7_FAILURE;
  if (pNbrAddr == L7_NULLPTR)
    return L7_FAILURE;

  rtrIfNum = *outRtrIfNum;

  if ( rtrIfNum == 0 && inetIsAddressZero(pNbrAddr) == L7_TRUE)
  {
      rtrIfNumStart = 0;
  }
  else
  {
    rtrIfNumStart = rtrIfNum;
  }


  while(1)
  {
    for (i = rtrIfNumStart; i < PIMDM_MAX_INTERFACES; i++)
    {
      if(pimdmIntfIsEnabled(pimdmCB,i) == L7_TRUE)
      {
        rtrIfNum = i;
        found = L7_TRUE;
        break;
      }
    }
    if ( i >= PIMDM_MAX_INTERFACES )
    {
       break;
    }

    /* Walk through all the neighbor entries, keeping track of the next
     * possible address to return
     */
    pBestMatch = L7_NULLPTR;
    if(found == L7_TRUE)
    {
      /* Lock the module */
      if(osapiSemaTake(pInfo->semId, L7_WAIT_FOREVER) != L7_SUCCESS)
        return L7_FAILURE;

      inetAddressZeroSet (pimdmCB->addrFamily, &zeroInet);
      if (*outRtrIfNum == rtrIfNum)
      {
        inetCopy (&nbrAddr, pNbrAddr);
      }
      else
      {
        inetAddressZeroSet (pimdmCB->addrFamily, &nbrAddr);
      }

      for(nbrIndex = 0; nbrIndex < PIMDM_MAX_NEIGHBORS; nbrIndex++)
      {
        pNbr = &(pInfo->nbrs[nbrIndex]);

        if(rtrIfNum == pNbr->rtrIfNum)
        {
          if (L7_INET_ADDR_COMPARE (&zeroInet, &(pNbr->addr)) == 0)
          {
            continue;
          }
          compare = L7_INET_ADDR_COMPARE(&nbrAddr, &(pNbr->addr));

          if(compare < 0)
          {
            if(pBestMatch != L7_NULLPTR)
            {
              if(L7_INET_ADDR_COMPARE(pBestMatch, &(pNbr->addr)) > 0)
              {
                pBestMatch = &(pNbr->addr);
              }
            }
            else
            {
              pBestMatch = &(pNbr->addr);
            }
          }
#if 0
          else if(compare > 0)
          {
            pBestMatch = &(pNbr->addr);
          }
          else
          {
            /* Do Nothing */
          }
#endif
        }
      }
      /* Return the next neighbor address if found */
      if(pBestMatch != L7_NULLPTR)
      {
        inetCopy(pNbrAddr, pBestMatch);
        osapiSemaGive(pInfo->semId);
        *outRtrIfNum = rtrIfNum;
        return L7_SUCCESS;
      }
      osapiSemaGive(pInfo->semId);
    }
    rtrIfNumStart++;
  }

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Retrieve the GetNext PIM-DM neighbor address
*
* @param    pCB            @b{(input)} Pointer to the PIM-DM Control Block
* @param    pNbrAddr       @b{(inout)} Pointer to the neighbor address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments If an exact entry with the given neighbor address is found,
*           then the same neighbor address is returned. If not, the next
*           numerically higher neighbor address found in the system is returned.
*
* @end
*********************************************************************/
L7_RC_t
pimdmIntfNeighborNextGet (pimdmCB_t      *pimdmCB,
                          L7_inet_addr_t *pNbrAddr)
{
  pimdmIntfInfo_t *pInfo = L7_NULLPTR;
  pimdmNeighbor_t *pNbr;
  L7_inet_addr_t  *pBestMatch;
  L7_int32        compare;
  L7_uint32       i;
  L7_inet_addr_t  zeroInet;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Validations */
  if(pimdmCB == L7_NULLPTR)
    return L7_FAILURE;
  if ((pInfo = pimdmCB->intfInfo) == L7_NULLPTR)
    return L7_FAILURE;
  if (pNbrAddr == L7_NULLPTR)
    return L7_FAILURE;

  /* Walk through all the neighbor entries, keeping track of the next
     * possible address to return
     */

  /* Lock the module */
  if(osapiSemaTake(pInfo->semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  pBestMatch= L7_NULLPTR;

  inetAddressZeroSet (pimdmCB->addrFamily, &zeroInet);
  for(i = 0; i < PIMDM_MAX_NEIGHBORS; i++)
  {
    pNbr = &(pInfo->nbrs[i]);

    if (L7_INET_ADDR_COMPARE (&zeroInet, &(pNbr->addr)) == 0)
    {
      continue;
    }

    compare = L7_INET_ADDR_COMPARE(pNbrAddr, &(pNbr->addr));

    if(compare == 0)
    {
      /* Exact Match found */
      osapiSemaGive(pInfo->semId);
      return L7_SUCCESS;
    }
    else if(compare < 0)
    {
      if(pBestMatch != L7_NULLPTR)
      {
        if(L7_INET_ADDR_COMPARE(pBestMatch, &(pNbr->addr)) > 0)
        {
          pBestMatch = &(pNbr->addr);
        }
      }
      else
      {
        pBestMatch = &(pNbr->addr);
      }
    }
  }
  /* Return the next neighbor address if found */
  if(pBestMatch != L7_NULLPTR)
  {
    inetCopy(pNbrAddr, pBestMatch);
    osapiSemaGive(pInfo->semId);
    return L7_SUCCESS;
  }

  osapiSemaGive(pInfo->semId);

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Process the options received in a Hello message
*
* @param   pCB          @b{(input)} Pointer to the PIM-DM Control Block
* @param   rtrIfNum     @b{(input)} Router Interface Index of the ingress interface
* @param   pNbrInfo     @b{(input)} Pointer to the neighbor information from the hello message
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmIntfHelloMsgProcess (pimdmCB_t          *pimdmCB,
                          L7_uint32          rtrIfNum,
                          pimdmIntfNbrInfo_t *pNbrInfo)
{
  pimdmIntfInfo_t *pInfo = L7_NULLPTR;
  pimdmInterface_t *pIf;
  pimdmNeighbor_t  *pNbr;
  L7_int32         nbrIndex;
  L7_uint32        timerGenId, tmpGenId;
  L7_BOOL          bChange = L7_FALSE;
  L7_uchar8 errMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];
  L7_uchar8 sucMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Basic sanity checks */
  if(pimdmCB == L7_NULLPTR)
    return L7_FAILURE;
  if ((pInfo = pimdmCB->intfInfo) == L7_NULLPTR)
    return L7_FAILURE;
  if(pNbrInfo == L7_NULLPTR)
    return L7_FAILURE;
  if(pimdmIntfIsEnabled(pimdmCB, rtrIfNum) != L7_TRUE)
    return L7_FAILURE;

  pIf = &(pInfo->intfs[rtrIfNum]);

  /* Retrieve the neighbor entry */
  nbrIndex = pimdmIntfNeighborIndexGet(pimdmCB, pNbrInfo->pNbrAddr,rtrIfNum);
  if(nbrIndex < 0)
  {
    /* New Neighbor */
    if(pNbrInfo->holdTime == 0)
      return L7_SUCCESS;
    nbrIndex = (-nbrIndex) - 1;
    if(nbrIndex >= PIMDM_MAX_NEIGHBORS)
      return L7_FAILURE;   /* No room for this new neighbor */

    /* Fill-up the neighbor entry */
    pNbr = &(pInfo->nbrs[nbrIndex]);
    timerGenId = pNbr->livenessTimerGenId;
    memset(pNbr, 0, sizeof(pimdmNeighbor_t));
    if(timerGenId == 0)
      timerGenId = pimdmUtilRandomGet(1, 0xFFFFFFFF);
    else
      timerGenId++;
    if(timerGenId == 0)
      timerGenId = 1;
    if(inetCopy(&(pNbr->addr), pNbrInfo->pNbrAddr) != L7_SUCCESS)
      return L7_FAILURE;
    pNbr->holdTime = pNbrInfo->holdTime;
    pNbr->bLanPruneDelayEnabled = pNbrInfo->bValidLanPruneDelay;
    if(pNbrInfo->bValidLanPruneDelay == L7_TRUE)
    {
      pNbr->lanPruneDelay = pNbrInfo->lanPruneDelay;
      pNbr->overrideIntvl = pNbrInfo->overrideInterval;
    }
    pNbr->bStateRefreshCapable = pNbrInfo->bStateRefreshCapable;
    if(pNbrInfo->bStateRefreshCapable == L7_TRUE)
      pNbr->stateRefreshIntvl = pNbrInfo->stateRefreshInterval;
    pNbr->genID = (pNbrInfo->bValidGenId == L7_TRUE) ? pNbrInfo->genId : 0;
    pNbr->livenessTimerGenId = timerGenId;
    pNbr->createTime = osapiUpTimeRaw();
    pNbr->rtrIfNum = rtrIfNum;
    pNbr->pimdmCB = pimdmCB;

    /* Update the relevant bitmaps */
    MCAST_BITX_SET(pIf->nbrBitmap, nbrIndex);
    pIf->numNbrs += 1;
    BITX_SET(&pimdmCB->nbrBitmap, rtrIfNum);

    /* Check & update values on the interface */
    pimdmIntfParamsCompute(pimdmCB, pNbr, PIMDM_INTF_NBR_ADD, pIf);

    /* Start liveness timer as needed */
    if(pNbrInfo->holdTime != PIMDM_INFINITE_HOLDTIME)
    {
      if ((pNbr->nbrTmrHandle = handleListNodeStore (pimdmCB->handleList,
                                                     (void*) pNbr))
                             == 0)
      {
        PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Handle List Add for NLT Failed");
        pimdmIntfNeighborClear(pimdmCB, rtrIfNum, nbrIndex);
        return L7_FAILURE;
      }

      /* Start the NLT */
      if (pimdmUtilAppTimerSet (pimdmCB, pimdmIntfNbrTimerExpiryHandler,
                                (void*) pNbr->nbrTmrHandle,
                                pNbrInfo->holdTime, L7_NULLPTR, L7_NULLPTR,
                                &(pNbr->livenessTimer),
                                "DM-NLT")
                             != L7_SUCCESS)
      {
        PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                     "Neighbor Liveness Timer could not be started");
        pimdmIntfNeighborClear(pimdmCB, rtrIfNum, nbrIndex);
        return L7_FAILURE;
      }
    }

    /* Speed-up the Hello Hello timer, if required */
    if(pIf->helloTimer != L7_NULLPTR)
    {
      L7_uint32 nextDelay, timeLeft;

      nextDelay = pimdmUtilRandomGet(0, pIf->triggeredHelloDelay);
      if (appTimerTimeLeftGet (pimdmCB->appTimer, pIf->helloTimer, &timeLeft)
                            == L7_SUCCESS)
      {
        if(timeLeft > nextDelay)
        {

          osapiSnprintf (errMsgBuf,PIMDM_MAX_DBG_MSG_SIZE,
                 "Hello timer Fastening failed on rtrIfNum %d", rtrIfNum);
          osapiSnprintf (sucMsgBuf,PIMDM_MAX_DBG_MSG_SIZE,
                 "Hello timer Fastening success on rtrIfNum %d", rtrIfNum);

          /* Update the Hello Timer */
          if (pimdmUtilAppTimerSet (pimdmCB, pimdmIntfHelloTimerExpiryHandler,
                                    (void*) pIf->intfTmrHandle,
                                    nextDelay, errMsgBuf, sucMsgBuf,
                                    &(pIf->helloTimer),
                                    "DM-HT2")
                                 != L7_SUCCESS)
          {
            PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Hello Timer Updation Failed");
          }
        }
      }
    }

    /* Register any change to pim_nbrs() */
    if(pIf->numNbrs == 1)
    {
      bChange = L7_TRUE;
    }
    else if(pIf->numNbrs > 1)
    {
      if(pimdmNbrAddMrtOifListCompute(pimdmCB,rtrIfNum) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
    }
  } /* if (nbrIndex < 0) */
  else
  {
    /* Existing Neighbor */
    pNbr = &(pInfo->nbrs[nbrIndex]);
    if(pNbr->rtrIfNum != rtrIfNum)
      return L7_FAILURE;

    if(pNbrInfo->holdTime == 0)
    {
      /* Remove the neighbor */
      /* First : Stop the liveness timer */
      if(pNbr->livenessTimer != L7_NULLPTR)
      {
        pimdmUtilAppTimerCancel (pimdmCB, &(pNbr->livenessTimer));
        handleListNodeDelete (pimdmCB->handleList, &(pNbr->nbrTmrHandle));
      }

      /* Second : Update the interface parameters */
      /*pimdmIntfParamsCompute(pCB, pNbr, PIMDM_INTF_NBR_DELETE, pIf);*/

      pimdmMrtNLTExpiryProcess(pimdmCB,&pNbr->addr,rtrIfNum,pIf->numNbrs);

      /* Third : Clean-up the neighbor structure */
      bChange = pimdmIntfNeighborClear(pimdmCB, rtrIfNum, nbrIndex);

      /* Generate an SNMP Trap for the Neighbor Loss */
      pimdmMapPimTrapNeighborLoss (pimdmCB->addrFamily, rtrIfNum);
    }
    else
    {
      /* Update the neighbor entry */
      pNbr->holdTime = pNbrInfo->holdTime;
      pNbr->bLanPruneDelayEnabled = pNbrInfo->bValidLanPruneDelay;
      if(pNbrInfo->bValidLanPruneDelay == L7_TRUE)
      {
        pNbr->lanPruneDelay = pNbrInfo->lanPruneDelay;
        pNbr->overrideIntvl = pNbrInfo->overrideInterval;
      }
      pNbr->bStateRefreshCapable = pNbrInfo->bStateRefreshCapable;
      if(pNbrInfo->bStateRefreshCapable == L7_TRUE)
        pNbr->stateRefreshIntvl = pNbrInfo->stateRefreshInterval;
      tmpGenId = pNbr->genID;
      pNbr->genID = (pNbrInfo->bValidGenId == L7_TRUE) ? pNbrInfo->genId : 0;
      if(tmpGenId != pNbr->genID)
      {
        pimdmIntfNbrInfo_t nbrInfo;

        /* Treat GENID changes as if the neighbor left and re-joined */
        pNbr->createTime = osapiUpTimeRaw();

        /* Neighbor's GENID has changed. Send the Hello Message */
         memset(&nbrInfo, 0, sizeof(pimdmIntfNbrInfo_t));
         nbrInfo.holdTime = pIf->helloHoldTime;
         nbrInfo.bValidLanPruneDelay = L7_TRUE;
         nbrInfo.lanPruneDelay = pIf->propagationDelay;
         nbrInfo.overrideInterval = pIf->overrideIntvl;
         nbrInfo.bValidGenId = L7_TRUE;
         nbrInfo.genId = pIf->genID;
         nbrInfo.bStateRefreshCapable = L7_TRUE;
         nbrInfo.stateRefreshInterval = pIf->effStateRefreshIntvl;

         if(pimdmHelloMsgSend (pimdmCB, rtrIfNum, &nbrInfo) != L7_SUCCESS)
         {
           PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                       "Failed sending Hello Message on rtrIfNum %d", rtrIfNum);
         }

         osapiSnprintf (errMsgBuf,PIMDM_MAX_DBG_MSG_SIZE,
                        "Hello timer refresh failed on rtrIfNum %d", rtrIfNum);
         osapiSnprintf (sucMsgBuf,PIMDM_MAX_DBG_MSG_SIZE,
                        "Hello timer refresh success on rtrIfNum %d", rtrIfNum);
         /* Restart the Hello Timer */
         if (pimdmUtilAppTimerSet (pimdmCB, pimdmIntfHelloTimerExpiryHandler,
                                   (void*) pIf->intfTmrHandle, pIf->helloInterval,
                                   errMsgBuf, sucMsgBuf, &(pIf->helloTimer),
                                   "DM-HT")
                                != L7_SUCCESS)
         {
           PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                        "Failed to start the Hello Timer for rtrIfNum %d", rtrIfNum);
           handleListNodeDelete (pimdmCB->handleList, &(pIf->intfTmrHandle));
         }

        /* Handle the Router Failure scenario by invoking the MRT to send
         * Prunes Upstream, if necessary - Section 4.3.4.
         */
        pimdmMrtNbrGenIDChangeProcess (pimdmCB, rtrIfNum, &pNbr->addr);
      }

      /* Update the interface's operational parameters */
      pimdmIntfParamsCompute(pimdmCB, pNbr, PIMDM_INTF_NBR_UPDATE, pIf);

      /* Start, Stop or update the liveness timer as necessary */
      if(pNbrInfo->holdTime == PIMDM_INFINITE_HOLDTIME)
      {
        pimdmUtilAppTimerCancel (pimdmCB, &(pNbr->livenessTimer));
        handleListNodeDelete (pimdmCB->handleList, &(pNbr->nbrTmrHandle));
      }
      else
      {
        L7_BOOL bNewTimerNeeded = L7_FALSE;

        if(pNbr->livenessTimer != L7_NULLPTR)
        {
          if (pimdmUtilAppTimerSet (pimdmCB, pimdmIntfNbrTimerExpiryHandler,
                                    (void*) pNbr->nbrTmrHandle,
                                    pNbrInfo->holdTime, L7_NULLPTR, L7_NULLPTR,
                                    &(pNbr->livenessTimer),
                                    "DM-NLT2")
                                 != L7_SUCCESS)
          {
            bNewTimerNeeded = L7_TRUE;
          }
        }
        else
        {
          bNewTimerNeeded = L7_TRUE;
        }
        if(bNewTimerNeeded == L7_TRUE)
        {
          pNbr->livenessTimerGenId += 1;
          if(pNbr->livenessTimerGenId == 0)
            pNbr->livenessTimerGenId = 1;
          if ((pNbr->nbrTmrHandle = handleListNodeStore (pimdmCB->handleList,
                                                         (void*) pNbr))
                                 == 0)
          {
            PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                         "Failed to Store node in the Timer Handle List");
            pNbr->livenessTimer = L7_NULLPTR;
          }
          else
          {
            if (pimdmUtilAppTimerSet (pimdmCB, pimdmIntfNbrTimerExpiryHandler,
                                      (void*) pNbr->nbrTmrHandle,
                                      pNbrInfo->holdTime, L7_NULLPTR, L7_NULLPTR,
                                      &(pNbr->livenessTimer),
                                      "DM-NLT3")
                                   != L7_SUCCESS)
            {
              /* Could not start a liveness timer. Purge the neighbor */
              handleListNodeDelete (pimdmCB->handleList, &(pNbr->nbrTmrHandle));
              /*pimdmIntfParamsCompute(pCB, pNbr, PIMDM_INTF_NBR_DELETE, pIf);*/
              bChange = pimdmIntfNeighborClear(pimdmCB, rtrIfNum, nbrIndex);
            }
          }
        }
      }
    }
  } /* if (nbrIndex < 0) */

  /* If changes to pim_nbrs() macro, re-compute the OIF list */
  if(bChange == L7_TRUE)
  {
    pimdmMrtOifListCompute(pimdmCB, L7_NULLPTR, L7_NULLPTR, PIMDM_MRT_CHANGE_NBR_LIST);
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Update MFC regarding the Interface Status Changes
*
* @param    pimdmCB     @b{ (input) } Pointer to the PIM-DM Ctrl Block
* @param    rtrIfNum    @b{ (input) } Router Interface Number
* @param    intfMode    @b{ (input) } Interface Mode
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments    This API deletes all the PIM-DM Operational data.
*
* @end
*********************************************************************/
static L7_RC_t
pimdmIntfStatusChangeMFCUpdate (pimdmCB_t *pimdmCB,
                                L7_uint32 rtrIfNum,
                                L7_uint32 intfMode)
{
  mfcInterfaceChng_t mfcIfChngEvent;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry ");

  memset (&mfcIfChngEvent, 0, sizeof (mfcInterfaceChng_t));
  mfcIfChngEvent.family = pimdmCB->addrFamily;
  mfcIfChngEvent.rtrIfNum = rtrIfNum;
  mfcIfChngEvent.mode = intfMode;

  if (mfcMessagePost (MFC_INTERFACE_STATUS_CHNG, &mfcIfChngEvent,
                      sizeof (mfcIfChngEvent))
                   != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "MFC_INTERFACE_MODE_CHG_EVENT Send Failed");
    return L7_FAILURE;
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit ");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Process an Interface UP event
*
* @param    pCB            @b{(input)} Pointer to the PIM-DM Control Block
* @param    rtrIfNum       @b{(input)} Router interface number
* @param    intIfNum       @b{(input)} Internal interface number
* @param    helloInterval  @b{(input)} Hello message interval in seconds
* @param    triggeredHelloDelay @b{(input)} Max triggered Hello message delay in seconds
* @param    helloHoldTime  @b{(input)} Hold Time for the outgoing hello messages in seconds
* @param    propogationDelay    @b{(input)} LAN Prune propogation delay in milliseconds
* @param    overrideInterval    @b{(input)} Prune Override Internal in milliseconds
* @param    pIpAddr        @b{(input)} Pointer to the interface's primary IP address
* @param    pNetMask       @b{(input)} Pointer to the interface's Network Mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmIntfUp (pimdmCB_t      *pimdmCB,
             L7_uint32      rtrIfNum,
             L7_uint32      intIfNum,
             L7_uint32      helloInterval,
             L7_uint32      triggeredHelloDelay,
             L7_uint32      helloHoldTime,
             L7_uint32      propagationDelay,
             L7_uint32      overrideIntvl,
             L7_inet_addr_t *pIpAddr,
             L7_inet_addr_t *pNetMask)
{
  pimdmInterface_t *pIf;
  pimdmIntfInfo_t  *pInfo;
  L7_uint32        tmpGenId, initialDelay;
  L7_uchar8 errMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];
  L7_uchar8 sucMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Basic sanity checks */
  if(pimdmCB == L7_NULLPTR)
    return L7_FAILURE;
  if(pIpAddr == L7_NULLPTR)
    return L7_FAILURE;
  if(pNetMask == L7_NULLPTR)
    return L7_FAILURE;
  if(rtrIfNum >= PIMDM_MAX_INTERFACES)
    return L7_FAILURE;
  if(helloInterval == 0)
    return L7_FAILURE;

  /* Retrieve the Interface entry */
  if ((pInfo = pimdmCB->intfInfo) == L7_NULLPTR)
    return L7_FAILURE;

  pIf = &(pInfo->intfs[rtrIfNum]);
  if (pIf->rtrIfNum == rtrIfNum)
    return L7_SUCCESS;  /* Already UP; nothing to do */
  if (pIf->rtrIfNum != 0)
    return L7_FAILURE;  /* Inconsistancy in the interface list */

  memset (pIf->intfStats, 0, sizeof(pIf->intfStats));

  /* Save the internal interface number */
  pInfo->intIfNum[rtrIfNum] = intIfNum;

  /* Setup the interface entry */
  tmpGenId = pIf->genID;
  memset(pIf, 0, sizeof(pimdmInterface_t));
  if(tmpGenId == 0)
    tmpGenId = pimdmUtilRandomGet(0, 0xFFFFFFFF);
  else
    tmpGenId += 1;
  if(tmpGenId == 0)
    tmpGenId = 1;
  pIf->genID = tmpGenId;
  pIf->helloInterval = helloInterval;
  pIf->triggeredHelloDelay = triggeredHelloDelay;
  pIf->helloHoldTime = helloHoldTime;
  pIf->propagationDelay = propagationDelay;
  pIf->overrideIntvl = overrideIntvl;
  pIf->isLanDelayEnabled = L7_FALSE;
  pIf->effPropagationDelay = L7_PIMDM_DEFAULT_LANPRUNEDELAY;
  pIf->effOverrideIntvl = L7_PIMDM_DEFAULT_OVERRIDEINTERVAL;
  pIf->isStateRefreshCapable = L7_FALSE;
  pIf->effStateRefreshIntvl = L7_PIMDM_DEFAULT_STATEREFRESHINTERVAL;
  pIf->rtrIfNum = rtrIfNum;
  inetCopy(&(pIf->ipAddress), pIpAddr);
  inetCopy(&(pIf->netMask), pNetMask);
  pIf->pimdmCB = pimdmCB;

  /* Start the Hello timer */
  initialDelay = pimdmUtilRandomGet(0, triggeredHelloDelay);
  if ((pIf->intfTmrHandle = handleListNodeStore (pimdmCB->handleList,
                                                 (void*) pIf))
                                              == 0)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                 "Failed to Store node in the Timer Handle List");
    return L7_FAILURE;
  }

  osapiSnprintf (errMsgBuf,PIMDM_MAX_DBG_MSG_SIZE,
         "Initial Hello timer set failed on rtrIfNum %d", rtrIfNum);
  osapiSnprintf (sucMsgBuf,PIMDM_MAX_DBG_MSG_SIZE,
         "Initial Hello timer set success on rtrIfNum %d", rtrIfNum);

  if (pimdmUtilAppTimerSet (pimdmCB, pimdmIntfHelloTimerExpiryHandler,
                            (void*) pIf->intfTmrHandle, initialDelay, errMsgBuf,
                            sucMsgBuf, &(pIf->helloTimer),
                            "DM-HT4")
                         != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                "Hello Timer start failed for rtrIfNum %d", rtrIfNum);
    return L7_FAILURE;
  }

  /* Enable PIM-DM Control packet reception on this interface for
   * IPv6 family.
   */
  if (pimdmCB->addrFamily == L7_AF_INET6)
  {
    if (pimdmIntfStatusChangeMFCUpdate (pimdmCB, rtrIfNum, L7_ENABLE) != L7_SUCCESS)
    {
      PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Intf [%d] Mode Change Enable Event Post "
                   "to MFC Failed", rtrIfNum);
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_PIMDM_MAP_COMPONENT_ID,
              "PIMDM router Intf [%d] Mode Change Enable Event Post to MFC Failed", rtrIfNum);
      pimdmMrtIntfDownUpdate (pimdmCB, rtrIfNum);
      return L7_FAILURE;
    }

    if (pimV6SockChangeMembership (rtrIfNum, intIfNum, L7_ENABLE, pimdmCB->sockFD)
                                != L7_SUCCESS)
    {
      PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "v6 Socket Memb'ship Enable Failed"
                   "For rtrIfNum %d", rtrIfNum);
      L7_LOGF(L7_LOG_SEVERITY_WARNING,L7_FLEX_PIMDM_MAP_COMPONENT_ID, "PIMDMv6 Socket Memb'ship Enable Failed "
                         "for rtrIfNum %d. Socket options Set to enable the reception of PIMv6 packets Failed."
                         " As a result of this, the PIMv6 packets will not be received by the application. ",
                         rtrIfNum);
      pimdmIntfStatusChangeMFCUpdate (pimdmCB, rtrIfNum, L7_DISABLE);
      pimdmMrtIntfDownUpdate (pimdmCB, rtrIfNum);
      return L7_FAILURE;
    }
  }

  /* Check with MGMD and get the membership details, if any, for this interface.
   */
  if (pimdmMgmdIntfGroupInfoGet (pimdmCB, rtrIfNum) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Failure in getting the MGMD Info for "
                 "rtrIfNum - %d", rtrIfNum);
    pimdmIntfStatusChangeMFCUpdate (pimdmCB, rtrIfNum, L7_DISABLE);
    pimV6SockChangeMembership (rtrIfNum, intIfNum, L7_DISABLE, pimdmCB->sockFD);
    pimdmMrtIntfDownUpdate (pimdmCB, rtrIfNum);
    return L7_FAILURE;
  }

  if (pimdmCB->addrFamily == L7_AF_INET)
  {
    if (mcastMapMRPAdminScopeInfoGet (pimdmCB->addrFamily, rtrIfNum) != L7_SUCCESS)
    {
      PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Failure in getting the AdminScope Info "
                   "for rtrIfNum - %d", rtrIfNum);
      pimdmIntfStatusChangeMFCUpdate (pimdmCB, rtrIfNum, L7_DISABLE);
      pimV6SockChangeMembership (rtrIfNum, intIfNum, L7_DISABLE, pimdmCB->sockFD);
      pimdmMrtIntfDownUpdate (pimdmCB, rtrIfNum);
      return L7_FAILURE;
    }
  }

  /* Notify PIM-DM MRT Database of the interface coming up */
  if (pimdmMrtIntfUpUpdate (pimdmCB, rtrIfNum) != L7_SUCCESS)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                "MRT Database updation failure on interface %d Up", rtrIfNum);
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Process an Interface DOWN event
*
* @param    pCB            @b{(input)} Pointer to the PIM-DM Control Block
* @param    rtrIfNum       @b{(input)} Router interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmIntfDown (pimdmCB_t *pimdmCB,
               L7_uint32 rtrIfNum)
{
  pimdmInterface_t   *pIf;
  L7_uint32          tmpGenId, i;
  L7_BOOL            bChange;
  pimdmIntfNbrInfo_t nbrInfo;
  L7_uint32 intIfNum = 0;
  pimdmIntfInfo_t  *pInfo = L7_NULLPTR;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Basic sanity checks */
  if(pimdmCB == L7_NULLPTR)
    return L7_FAILURE;
  if(rtrIfNum >= PIMDM_MAX_INTERFACES)
    return L7_FAILURE;

  bChange = L7_FALSE;

  /* Retrieve the Interface entry */
  if ((pInfo = pimdmCB->intfInfo) == L7_NULLPTR)
    return L7_FAILURE;

  /* Retrieve the Interface entry */
  pIf = &(pInfo->intfs[rtrIfNum]);
  if (pIf->rtrIfNum == 0)
    return L7_SUCCESS;  /* Already DOWN; nothing to do */
  if (pIf->rtrIfNum != rtrIfNum)
    return L7_FAILURE;  /* Inconsistancy in the interface list */

  intIfNum = pInfo->intIfNum[rtrIfNum];

  /* Stop the Hello timer */
  PIMDM_TRACE (PIMDM_DEBUG_API, " Deleting Hello timer of TimerNode : %p",
               pIf->helloTimer);
  PIMDM_TRACE (PIMDM_DEBUG_API, " Deleting Hello timer of Timerhandle : %p",
               pIf->intfTmrHandle);
  pimdmUtilAppTimerCancel (pimdmCB, &(pIf->helloTimer));
  handleListNodeDelete (pimdmCB->handleList, &(pIf->intfTmrHandle));

  /* Notify PIM-DM MGMD Database of the interface going down */
  if (pimdmMgmdIntfDownUpdate (pimdmCB, rtrIfNum) != L7_SUCCESS)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                "MGMD Database updation failure on interface %d Down", rtrIfNum);
  }

  /* Notify PIM-DM AdminScope Database of the interface going down */
  if (pimdmAdminScopeIntfDownUpdate(pimdmCB, rtrIfNum) != L7_SUCCESS)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                "Admin-scope Database updation failure on interface %d Down", rtrIfNum);
  }

  /* Notify PIM-DM MRT Database of the interface going down */
  if (pimdmMrtIntfDownUpdate (pimdmCB, rtrIfNum) != L7_SUCCESS)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                "MRT Database updation failure on interface %d Down", rtrIfNum);
  }

  /* Clean-up all the Neighbors associated with this interface */
  if(pIf->numNbrs != 0)
  {
    L7_inet_addr_t zeroInet;

    inetAddressZeroSet (pimdmCB->addrFamily, &zeroInet);
    for(i = 0; i < PIMDM_MAX_NEIGHBORS; i++)
    {
      if(MCAST_BITX_TEST(pIf->nbrBitmap, i) != 0)
      {
        pimdmNeighbor_t *pNbr;

        pNbr = &(pInfo->nbrs[i]);
        if (L7_INET_ADDR_COMPARE (&zeroInet, &(pNbr->addr)) != 0)
        {
          /* Clean-up the neighbor structure */
          pimdmUtilAppTimerCancel (pimdmCB, &(pNbr->livenessTimer));
          handleListNodeDelete (pimdmCB->handleList, &(pNbr->nbrTmrHandle));
          pimdmJPWorkingBuffReturn (pimdmCB, pNbr);
          tmpGenId = pNbr->genID;
          memset(pNbr, 0, sizeof(pimdmNeighbor_t));
          pNbr->genID = tmpGenId;
        }
      }
    }

    /* Clear-out the interface from the neighbor listing */
    BITX_RESET(&pimdmCB->nbrBitmap, rtrIfNum);
    PIMDM_TRACE (PIMDM_DEBUG_EVENTS, "Nbr Bitmap is Reset for rtrIfNum - %d",
                 rtrIfNum);
    bChange = L7_TRUE;   /* pim_nbrs() is changing */
  }

  /* Send out a Hello message with Zero holdtime */
  memset(&nbrInfo, 0, sizeof(pimdmIntfNbrInfo_t));
  nbrInfo.holdTime = 0;
  nbrInfo.bValidLanPruneDelay = L7_TRUE;
  nbrInfo.lanPruneDelay = pIf->propagationDelay;
  nbrInfo.overrideInterval = pIf->overrideIntvl;
  nbrInfo.bStateRefreshCapable = L7_TRUE;
  nbrInfo.stateRefreshInterval = pIf->effStateRefreshIntvl;
  nbrInfo.bValidGenId = L7_TRUE;
  nbrInfo.genId = pIf->genID;
  pimdmHelloMsgSend(pimdmCB, pIf->rtrIfNum, &nbrInfo);

  /* Notify Others if the pim_nbrs() has changed */
  if(bChange == L7_TRUE)
  {
    pimdmMrtOifListCompute(pimdmCB, L7_NULLPTR, L7_NULLPTR, PIMDM_MRT_CHANGE_NBR_LIST);
  }

  /* Disable PIM-DM Control packet reception on this interface for
   * IPv6 family.
   */
  if (pimdmCB->addrFamily == L7_AF_INET6)
  {
    if (pimdmIntfStatusChangeMFCUpdate (pimdmCB, rtrIfNum, L7_DISABLE) != L7_SUCCESS)
    {
      PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Intf [%d] Mode Change Disable Event Post "
                   "to MFC Failed", rtrIfNum);
    }

    if (pimV6SockChangeMembership (rtrIfNum, intIfNum, L7_DISABLE, pimdmCB->sockFD)
                                != L7_SUCCESS)
    {
      PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "v6 Socket Memb'ship Disable Failed"
                   "For Interface %d", rtrIfNum);
    }
  }

  /* Clear the Stats for this Interface.
   */
  pimdmIntfStatsClear (pimdmCB, rtrIfNum, 0, 0);

  /* Clean-up the interface entry itself */
  tmpGenId = pIf->genID;
  memset(pIf, 0, sizeof(pimdmInterface_t));
  pIf->genID = tmpGenId;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Pruge the PM-DM Interface
*
* @param    pimdmCB        @b{(input)} Pointer to the PIM-DM Control Block
* @param    rtrIfNum       @b{(input)} Router interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmIntfPurge (pimdmCB_t *pimdmCB,
                L7_uint32 rtrIfNum)
{
  L7_uint32 tmpGenId;
  pimdmInterface_t *pIf = L7_NULLPTR;
  pimdmIntfInfo_t *pInfo = L7_NULLPTR;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Basic sanity checks */
  if(pimdmCB == L7_NULLPTR)
    return L7_FAILURE;
  if(rtrIfNum >= PIMDM_MAX_INTERFACES)
    return L7_FAILURE;

  /* Retrieve the Interface info */
  if ((pInfo = pimdmCB->intfInfo) == L7_NULLPTR)
    return L7_FAILURE;

  /* Retrieve the Interface entry */
  pIf = &(pInfo->intfs[rtrIfNum]);
  if (pIf->rtrIfNum == 0)
    return L7_SUCCESS;  /* Already DOWN; nothing to do */
  if (pIf->rtrIfNum != rtrIfNum)
    return L7_FAILURE;  /* Inconsistancy in the interface list */

  pInfo->intIfNum[rtrIfNum] = 0;

  /* Stop the Hello timer */
  pimdmUtilAppTimerCancel (pimdmCB, &(pIf->helloTimer));
  handleListNodeDelete (pimdmCB->handleList, &(pIf->intfTmrHandle));

  /* Clean-up the interface entry itself */
  tmpGenId = pIf->genID;
  memset(pIf, 0, sizeof(pimdmInterface_t));
  pIf->genID = tmpGenId;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Initialize the module
*
* @param    pCB            @b{(input)} Pointer to the PIM-DM Control Block
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmIntfInit(pimdmCB_t *pimdmCB)
{
  pimdmIntfInfo_t *pInfo;
  L7_uint32       i;

  /* Basic sanity checks */
  if(pimdmCB == L7_NULLPTR)
    return L7_FAILURE;
  if(pimdmCB->intfInfo != L7_NULLPTR)
    return L7_FAILURE;   /* Already initialized */

  if ((pInfo = PIMDM_ALLOC (pimdmCB->addrFamily, sizeof (pimdmIntfInfo_t)))
                         == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "PIMDM IntfInfo Heap Allocation Failed");
    return L7_FAILURE;
  }

  memset(pInfo, 0, sizeof(pimdmIntfInfo_t));
  pInfo->semId = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if(pInfo->semId == L7_NULLPTR)
  {
    PIMDM_FREE (pimdmCB->addrFamily, (void*) pInfo);
    return L7_FAILURE;
  }
  for(i = 1; i < PIMDM_MAX_INTERFACES; i++)
    pInfo->intIfNum[i] = -1;

  pimdmCB->intfInfo = pInfo;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  De-Initialize the module
*
* @param    pCB            @b{(input)} Pointer to the PIM-DM Control Block
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmIntfDeInit(pimdmCB_t *pimdmCB)
{
  pimdmIntfInfo_t *pInfo;
  L7_uint32       i;

  /* Basic sanity checks */
  if(pimdmCB == L7_NULLPTR)
    return L7_FAILURE;
  if((pInfo = pimdmCB->intfInfo) == L7_NULLPTR)
    return L7_FAILURE;   /* Already de-initialized */

  /* Close-up all interfaces */
  for(i = 1; i < PIMDM_MAX_INTERFACES; i++)
  {
    if(pInfo->intfs[i].rtrIfNum != 0)
      pimdmIntfDown(pimdmCB, i);
  }

  /* Free-up all the allocated space */
  osapiSemaDelete(pInfo->semId);
  memset(pInfo, 0, sizeof(pimdmIntfInfo_t));

  PIMDM_FREE (pimdmCB->addrFamily, (void*) pInfo);

  pimdmCB->intfInfo = L7_NULLPTR;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the effective State Refresh Interval for the specified
*           interface
*
* @param    pCB               @b{(input)} Pointer to the PIM-DM Control Block
* @param    rtrIfNum          @b{(input)} Router interface number
* @param    effStRfrThreshold @b{(input)} Effective State Refresh Threshold
*                                         value for the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmIntfStateRefreshIntervalGet (pimdmCB_t *pimdmCB,
                                  L7_uint32 rtrIfNum,
                                  L7_uint32 *effStRfrThreshold)
{
  pimdmIntfInfo_t *pInfo = L7_NULLPTR;
  pimdmInterface_t *pIf = L7_NULLPTR;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Validations */
  if (rtrIfNum >= PIMDM_MAX_INTERFACES)
    return L7_FAILURE;
  if (pimdmCB == L7_NULLPTR)
    return L7_FAILURE;
  if ((pInfo = pimdmCB->intfInfo) == L7_NULLPTR)
    return L7_FAILURE;

  *effStRfrThreshold = 0;

  /* Lock the module */
  if(osapiSemaTake(pInfo->semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  /* Retrieve the mapped internal interface number */
  pIf = &(pInfo->intfs[rtrIfNum]);
  if (pIf->rtrIfNum == 0)
  {
    osapiSemaGive(pInfo->semId);
    return L7_FAILURE;
  }

  *effStRfrThreshold = pIf->effStateRefreshIntvl;

  osapiSemaGive(pInfo->semId);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Set the  State Refresh TTL Threshold for the specified
*           interface
*
* @param    pCB               @b{(input)} Pointer to the PIM-DM Control Block
* @param    rtrIfNum          @b{(input)} Router interface number
* @param    ttlThreshold      @b{(input)} State Refresh TTL threshold
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmIntfStateRefreshTTLThresholdSet (pimdmCB_t *pimdmCB,
                                  L7_uint32 rtrIfNum,
                                  L7_uint32 ttlThreshold)
{
  pimdmIntfInfo_t *pInfo = L7_NULLPTR;
  pimdmInterface_t *pIf = L7_NULLPTR;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Validations */
  if (rtrIfNum >= PIMDM_MAX_INTERFACES)
    return L7_FAILURE;
  if (pimdmCB == L7_NULLPTR)
    return L7_FAILURE;
  if ((pInfo = pimdmCB->intfInfo) == L7_NULLPTR)
    return L7_FAILURE;

  /* Lock the module */
  if(osapiSemaTake(pInfo->semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  /* Retrieve the mapped internal interface number */
  pIf = &(pInfo->intfs[rtrIfNum]);
  if (pIf->rtrIfNum == 0)
  {
    osapiSemaGive(pInfo->semId);
    return L7_FAILURE;
  }

  pIf->ttlThreshold = ttlThreshold;

  osapiSemaGive(pInfo->semId);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the State Refresh TTL Threshold for the specified
*           interface
*
* @param    pCB               @b{(input)} Pointer to the PIM-DM Control Block
* @param    rtrIfNum          @b{(input)} Router interface number
* @param    ttlThreshold      @b{(input)} State Refresh TTL Threshold
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmIntfStateRefreshTTLThresholdGet (pimdmCB_t *pimdmCB,
                                  L7_uint32 rtrIfNum,
                                  L7_uint32 *ttlThreshold)
{
  pimdmIntfInfo_t *pInfo = L7_NULLPTR;
  pimdmInterface_t *pIf = L7_NULLPTR;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Validations */
  if (rtrIfNum >= PIMDM_MAX_INTERFACES)
    return L7_FAILURE;
  if (pimdmCB == L7_NULLPTR)
    return L7_FAILURE;
  if ((pInfo = pimdmCB->intfInfo) == L7_NULLPTR)
    return L7_FAILURE;

  *ttlThreshold = 0;

  /* Lock the module */
  if(osapiSemaTake(pInfo->semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  /* Retrieve the mapped internal interface number */
  pIf = &(pInfo->intfs[rtrIfNum]);
  if (pIf->rtrIfNum == 0)
  {
    osapiSemaGive(pInfo->semId);
    return L7_FAILURE;
  }

  *ttlThreshold = pIf->ttlThreshold;

  osapiSemaGive(pInfo->semId);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Print basic information of all PIM-DM interfaces
*
* @param    pCB            @b{(input)} Pointer to the PIM-DM Control Block
*
* @returns  None
*
* @comments This function is to be used ONLY FOR DEBUG PURPOSES.
*
* @end
*********************************************************************/
void
pimdmIntfAllShow(pimdmCB_t *pimdmCB)
{
  pimdmIntfInfo_t  *pInfo;
  pimdmInterface_t *pIf;
  L7_uint32        i;

  /* Basic sanity checks */
  if(pimdmCB == L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%s : Invalid Control Block pointer : NULL\n", __FUNCTION__);
    return;
  }
  if ((pInfo = pimdmCB->intfInfo) == L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%s : Invalid pInfo pointer : NULL\n", __FUNCTION__);
    return;
  }
  if(pInfo->semId == L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%s : Invalid semaphore ID : NULL\n", __FUNCTION__);
    return;
  }

  /* Print basic information on all the interfaces */
  if(osapiSemaTake(pInfo->semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%s : Failed to acquire semaphore\n", __FUNCTION__);
    return;
  }
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "PIM-DM Interface Basic Information :\n");
  for(i = 1; i < PIMDM_MAX_INTERFACES; i++)
  {
    pIf = &(pInfo->intfs[i]);
    if(pIf->rtrIfNum == 0)
    {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                    "  RtrIntf[%-3d] : PIM-DM not enabled\n", i);
    }
    else
    {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                    "  RtrIntf[%-3d] : HelloIntrvl[%-4d],GenID[%8x],NumNbrs[%-3d],LPDEnabled[%s],StRfrshCapable[%s]\n",
                    pIf->rtrIfNum, pIf->helloInterval, pIf->genID,
                    pIf->numNbrs, (pIf->isLanDelayEnabled == L7_TRUE) ? "YES" : "NO ",
                    (pIf->isStateRefreshCapable == L7_TRUE) ? "YES" : "NO ");
    }
  }
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Done.\n");

  osapiSemaGive(pInfo->semId);
}

/*********************************************************************
*
* @purpose  Print interface information for a PIM-DM interface
*
* @param    pCB            @b{(input)} Pointer to the PIM-DM Control Block
* @param    rtrIfNum       @b{(input)} Router Interface Index
*
* @returns  None
*
* @comments This function is to be used ONLY FOR DEBUG PURPOSES.
*
* @end
*********************************************************************/
void
pimdmIntfShow(pimdmCB_t *pimdmCB, L7_uint32 rtrIfNum)
{
  pimdmIntfInfo_t  *pInfo;
  pimdmInterface_t *pIf;
  L7_uint32        i;
  L7_uchar8 addr[PIMDM_MAX_DBG_ADDR_SIZE];
  /* Basic sanity checks */
  if(pimdmCB == L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%s : Invalid Control Block pointer : NULL\n", __FUNCTION__);
    return;
  }
  if ((pInfo = pimdmCB->intfInfo) == L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%s : Invalid pInfo pointer : NULL\n", __FUNCTION__);
    return;
  }
  if(pInfo->semId == L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%s : Invalid semaphore ID : NULL\n", __FUNCTION__);
    return;
  }
  if((rtrIfNum < 1) || (rtrIfNum >= PIMDM_MAX_INTERFACES))
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%s : Invalid rtrIfNum : %d\n", __FUNCTION__, rtrIfNum);
    return;
  }

  /* Print all the information for the interface */
  if(osapiSemaTake(pInfo->semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%s : Failed to acquire semaphore\n", __FUNCTION__);
    return;
  }
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "PIM-DM Interface Information for Router Interface : %d\n", rtrIfNum);
  pIf = &(pInfo->intfs[rtrIfNum]);
  if(pIf->rtrIfNum == 0)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "  PIM-DM not enabled\n");
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "  Router Interface Num : %d\n", pIf->rtrIfNum);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "  Interface Address : %s\n", inetAddrPrint(&pIf->ipAddress,addr));
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "  Configuration Items :\n");
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "    Hello Interval (seconds) : %d\n", pIf->helloInterval);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "    Triggered Hello Delay (seconds) : %d\n", pIf->triggeredHelloDelay);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "    Hello Hold Time (seconds) : %d\n", pIf->helloHoldTime);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "    LAN Prune Propagation Delay (milliseconds) : %d\n", pIf->propagationDelay);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "    Override Interval (milliseconds) : %d\n", pIf->overrideIntvl);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "    State Refresh TTL Threshold : %d\n", pIf->ttlThreshold);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "  Operational Items :\n");
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "    Generation ID : %d\n", pIf->genID);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "    LAN Prune Delay Enabled : %s\n",
                  (pIf->isLanDelayEnabled == L7_TRUE) ? "YES" : "NO");
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "    Effective LAN Prune Propagation Delay (milliseconds) : %d\n",
                  pIf->effPropagationDelay);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "    Effective Override Interval (milliseconds) : %d\n",
                  pIf->effOverrideIntvl);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "    State Refresh Capable : %s\n",
                  (pIf->isStateRefreshCapable == L7_TRUE) ? "YES" : "NO");
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "    Effective State Refresh Interval (seconds) : %d\n",
                  pIf->effStateRefreshIntvl);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "  Hello Timer Items :\n");
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "    Hello Timer Handle : %p\n",
                  pIf->intfTmrHandle);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "    Hello Timer node : %p\n",
                  pIf->helloTimer);
    if(pIf->helloTimer == L7_NULLPTR)
    {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                    "    Timer not running\n");
    }
    else
    {
      L7_uint32        timeLeft;

      if(appTimerTimeLeftGet(pimdmCB->appTimer, pIf->helloTimer, &timeLeft) != L7_SUCCESS)
      {
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                      "    Timer has expired\n");
      }
      else
      {
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                      "    Time Left for Hello Timer Expiry (seconds) : %d\n", timeLeft);
      }
    }
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "  Neighbor Items :\n");
    if(pIf->numNbrs == 0)
    {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                    "    No Neighbors Established.\n");
    }
    else
    {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                    "    Number of Neighbors : %d\n", pIf->numNbrs);
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                    "    Neighbor Array Indices :");
      for(i = 0; i < PIMDM_MAX_NEIGHBORS; i++)
      {
        if(MCAST_BITX_TEST(pIf->nbrBitmap, i) != 0)
        {
          SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                        " %d", i);
        }
      }
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "\n");
    }
  }
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Done.\n");

  osapiSemaGive(pInfo->semId);
}

/*********************************************************************
*
* @purpose  Print basic information of all PIM-DM neighbors
*
* @param    pCB            @b{(input)} Pointer to the PIM-DM Control Block
*
* @returns  None
*
* @comments This function is to be used ONLY FOR DEBUG PURPOSES.
*
* @end
*********************************************************************/
void
pimdmIntfNbrAllShow(pimdmCB_t *pimdmCB)
{
  pimdmIntfInfo_t  *pInfo;
  pimdmNeighbor_t  *pNbr;
  L7_uint32        i;
  L7_uint32        numNbrs;
  L7_inet_addr_t   zeroInet;
  L7_uchar8        printBuf[IPV6_DISP_ADDR_LEN];

  /* Basic sanity checks */
  if(pimdmCB == L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%s : Invalid Control Block pointer : NULL\n", __FUNCTION__);
    return;
  }
  if ((pInfo = pimdmCB->intfInfo) == L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%s : Invalid pInfo pointer : NULL\n", __FUNCTION__);
    return;
  }
  if(pInfo->semId == L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%s : Invalid semaphore ID : NULL\n", __FUNCTION__);
    return;
  }

  /* Print basic information on all the interfaces */
  if(osapiSemaTake(pInfo->semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%s : Failed to acquire semaphore\n", __FUNCTION__);
    return;
  }
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "PIM-DM Neighbor Basic Information :\n");
  inetAddressZeroSet (pimdmCB->addrFamily, &zeroInet);
  numNbrs = 0;
  for(i = 0; i < PIMDM_MAX_NEIGHBORS; i++)
  {
    pNbr = &(pInfo->nbrs[i]);
    if (L7_INET_ADDR_COMPARE (&zeroInet, &(pNbr->addr)) != 0)
    {
      numNbrs++;
      inetAddrHtop(&(pNbr->addr), printBuf);
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                    "  %d. NbrIndex[%-3d] : Address[%s],GenID[%8x],HoldTime[%-5d],LPDEnabled[%s],SfRfrshCapable[%s],RtrIfNum[%-3d]\n",
                    numNbrs, i, printBuf, pNbr->genID, pNbr->holdTime,
                    (pNbr->bLanPruneDelayEnabled == L7_TRUE) ? "YES" : "NO ",
                    (pNbr->bStateRefreshCapable == L7_TRUE) ? "YES" : "NO ",
                    pNbr->rtrIfNum);
    }
  }
  if(numNbrs == 0)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "  No neighbors found.\n");
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "  Total number of neighbors found : %d.\n", numNbrs);
  }
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Done.\n");

  osapiSemaGive(pInfo->semId);
}

/*********************************************************************
*
* @purpose  Print neighbor information for a PIM-DM neighbor
*
* @param    pCB            @b{(input)} Pointer to the PIM-DM Control Block
* @param    nbrIndex       @b{(input)} Neighbor Array Index
*
* @returns  None
*
* @comments This function is to be used ONLY FOR DEBUG PURPOSES.
*
* @end
*********************************************************************/
void
pimdmIntfNbrShow(pimdmCB_t *pimdmCB, L7_uint32 nbrIndex)
{
  pimdmIntfInfo_t  *pInfo;
  pimdmNeighbor_t  *pNbr;
  L7_inet_addr_t   zeroInet;
  L7_uchar8        printBuf[IPV6_DISP_ADDR_LEN];

  /* Basic sanity checks */
  if(pimdmCB == L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%s : Invalid Control Block pointer : NULL\n", __FUNCTION__);
    return;
  }
  if ((pInfo = pimdmCB->intfInfo) == L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%s : Invalid pInfo pointer : NULL\n", __FUNCTION__);
    return;
  }
  if(pInfo->semId == L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%s : Invalid semaphore ID : NULL\n", __FUNCTION__);
    return;
  }
  if(nbrIndex >= PIMDM_MAX_NEIGHBORS)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%s : Invalid nbrIndex : %d\n", __FUNCTION__, nbrIndex);
    return;
  }

  /* Print all the information for the neighbor */
  if(osapiSemaTake(pInfo->semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%s : Failed to acquire semaphore\n", __FUNCTION__);
    return;
  }
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "PIM-DM Neighbor Information for Neighbor Index : %d\n", nbrIndex);
  pNbr = &(pInfo->nbrs[nbrIndex]);
  inetAddressZeroSet (pimdmCB->addrFamily, &zeroInet);
  if (L7_INET_ADDR_COMPARE (&zeroInet, &(pNbr->addr)) == 0)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "  No valid Neighbor present at this index.\n");
  }
  else
  {
    inetAddrHtop(&(pNbr->addr), printBuf);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "  Neighbor Address : %s\n", printBuf);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "  Neighbor Generation ID : %x\n", pNbr->genID);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "  Router Interface Index : %d\n", pNbr->rtrIfNum);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "  Hold Time (seconds) : %d\n", pNbr->holdTime);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "  LAN Prune Delay Enabled : %s\n",
                  (pNbr->bLanPruneDelayEnabled == L7_TRUE) ? "YES" : "NO");
    if(pNbr->bLanPruneDelayEnabled == L7_TRUE)
    {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                    "    LAN Prune Propagation Delay (milliseconds) : %d\n",
                    pNbr->lanPruneDelay);
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                    "    Override Interval (milliseconds) : %d\n",
                    pNbr->overrideIntvl);
    }
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "  State Refresh Capable : %s\n",
                  (pNbr->bStateRefreshCapable == L7_TRUE) ? "YES" : "NO");
    if(pNbr->bStateRefreshCapable == L7_TRUE)
    {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                    "    State Refresh Interval (seconds) : %d\n", pNbr->stateRefreshIntvl);
    }
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "  Liveness Timer Items :\n");
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "    Liveness Timer Beneration ID : 0x%x\n", pNbr->livenessTimerGenId);
    if(pNbr->livenessTimer == L7_NULLPTR)
    {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                    "    Timer not running\n");
    }
    else
    {
      L7_uint32        timeLeft;

      if(appTimerTimeLeftGet(pimdmCB->appTimer, pNbr->livenessTimer, &timeLeft) != L7_SUCCESS)
      {
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                      "    Timer has expired\n");
      }
      else
      {
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                      "    Time Left for Liveness Timer Expiry (seconds) : %d\n", timeLeft);
      }
    }
  }
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Done.\n");

  osapiSemaGive(pInfo->semId);
}

/******************************************************************************
* @purpose  Return the interface block for the given interface index
*
* @param    pimdmCb       @b{(input)}  control block
* @param    rtrIfNum      @b{(input)} router interface number
* @param    ppIntfEntry   @b{(output)}  pointer to interface entry
*
* @returns  void
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t
pimdmIntfEntryGet (pimdmCB_t *pimdmCB,
                   L7_uint32 rtrIfNum,
                   pimdmInterface_t **ppIntfEntry)
{
  pimdmIntfInfo_t  *pInfo = L7_NULLPTR;
  pimdmInterface_t *pIntfEntry = L7_NULLPTR;

  if ((pimdmCB == L7_NULLPTR) || (rtrIfNum >= PIMDM_MAX_INTERFACES) ||
      (rtrIfNum < 1))
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Invalid rtrIfNum - %d", rtrIfNum);
    return L7_FAILURE;
  }

  if ((pInfo = pimdmCB->intfInfo) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (osapiSemaTake (pInfo->semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if ((pIntfEntry = &(pInfo->intfs[rtrIfNum])) == L7_NULLPTR)
  {
    osapiSemaGive (pInfo->semId);
    return L7_FAILURE;
  }
  if ((pIntfEntry->rtrIfNum == 0) ||
      (pIntfEntry->rtrIfNum != rtrIfNum))
  {
    osapiSemaGive (pInfo->semId);
    return L7_FAILURE;
  }

  if (ppIntfEntry != L7_NULLPTR)
  {
    *ppIntfEntry = pIntfEntry;
  }

  osapiSemaGive (pInfo->semId);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Finds and returns a neighbor from the Neighbor List
*
* @param    pimdmCb     @b{(input)}  control block
* @param    pIntfEntry  @b{(input)} interface entry
*           pSrcAddr    @b{(input)} address of neighbor to find
*           pNbrEntry   @b{(output)} addr of pointer to
                           neighbor entry struct
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimdmNeighborFind(pimdmCB_t * pimdmCB,
                                 pimdmInterface_t *pIntfEntry,
                                 L7_inet_addr_t *pSrcAddr,
                                 pimdmNeighbor_t ** ppNbrEntry)
{
  pimdmIntfInfo_t  *pInfo = L7_NULLPTR;
  pimdmNeighbor_t  *pNbrEntry = L7_NULLPTR;
  L7_uint32        i;
  L7_BOOL found = L7_FALSE;

  PIMDM_TRACE( PIMDM_DEBUG_API, "Enter ");

  if(pimdmCB == L7_NULLPTR || pIntfEntry == L7_NULLPTR || pSrcAddr == L7_NULLPTR)
  {
    PIMDM_TRACE( PIMDM_DEBUG_FAILURE,
                "Invalid input parameters");
    return L7_FAILURE;
  }
  if ((pInfo = pimdmCB->intfInfo) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  /* Lock the module */
  if (osapiSemaTake (pInfo->semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMDM_TRACE( PIMDM_DEBUG_FAILURE,
                "osapiSemaTake() failed");
    return L7_FAILURE;
  }

  /* Check all the neighbors on the interface if any match the given
   * IP Address
   */
  for (i = 0; i < PIMDM_MAX_NEIGHBORS; i++)
  {
    if (MCAST_BITX_TEST(pIntfEntry->nbrBitmap, i) != 0)
    {
      pNbrEntry  = &(pInfo->nbrs[i]);
      if (L7_INET_IS_ADDR_EQUAL(pSrcAddr, &(pNbrEntry->addr)) == L7_TRUE)
      {
        found = L7_TRUE;
        break;
      }
    }
  }
  osapiSemaGive (pInfo->semId);

  if(found == L7_FALSE)
  {
    PIMDM_TRACE( PIMDM_DEBUG_API, "NBR Find ret NULL ");
    return L7_FAILURE;
  }
  if(ppNbrEntry != L7_NULLPTR)
  {
    *ppNbrEntry = pNbrEntry;
  }
  PIMDM_TRACE( PIMDM_DEBUG_API, "EXIT ");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To Clear the PIM-DM Statistics
*
* @param    pimdmCB      @b{ (input) } Pointer to the PIM-DM Control Block
*           rtrIfNum     @b{ (input) } Index of the Router Interface for
*                                      Stats has to be cleared
*                                      Specifying 0 would clear stats specified
*                                      by the statsType for the specified
*                                      message on all Interfaces
*           msgType      @b{ (input) } Type of the PIM-DM Control Pkt
*                                      Specifying 0 would clear stats specified
*                                      by the statsType for all messages.
*           statsType    @b{ (input) } Type of the Statistic
*                                      Specifying 0 would clear all types of
*                                      Statistics for the specified message.
*
* @returns  void, if success
* @returns  void, if failure
*
* @comments    This API is provided only for debugging purpose and should
*           not be used anywhere in the code flow.

            IGNORE PARAMS : msgType, statsType
            CALLER should always pass them as zeros.
*
* @end
*********************************************************************/
void
pimdmIntfStatsClear (pimdmCB_t *pimdmCB,
                     L7_uint32 rtrIfNum,
                     PIMDM_CTRL_PKT_TYPE_t msgType,
                     PIMDM_STATS_TYPE_t statsType)
{
  pimdmInterface_t *pimdmIntf = L7_NULLPTR;
  L7_uint32 tempRtrIfNum = 0;

  if(rtrIfNum >= PIMDM_MAX_INTERFACES)
  {
    PIMDM_DEBUG_PRINTF ("Invalid rtrIfNum-%d.\n", rtrIfNum);
    return;
  }

  if (rtrIfNum == 0) /* Clear for All Interfaces */
  {
    for (tempRtrIfNum = 1; tempRtrIfNum < PIMDM_MAX_INTERFACES; tempRtrIfNum++)
    {
      if (pimdmIntfEntryGet (pimdmCB, tempRtrIfNum, &pimdmIntf) != L7_SUCCESS)
      {
        continue;
      }
      if (pimdmIntf == L7_NULLPTR)
      {
        continue;
      }

      memset (pimdmIntf->intfStats, 0 , sizeof (pimdmIntf->intfStats));
    }
  }
  else /* Clear for the specified Interface */
  {
    if (pimdmIntfEntryGet (pimdmCB, rtrIfNum, &pimdmIntf) != L7_SUCCESS)
    {
      PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Intf Entry Get Failed for rtrIfNum-%d.\n", rtrIfNum);
      return;
    }
    if (pimdmIntf == L7_NULLPTR)
    {
      PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Intf Entry is NULL for rtrIfNum-%d.\n", rtrIfNum);
      return;
    }

    memset (pimdmIntf->intfStats, 0 , sizeof (pimdmIntf->intfStats));
  }

  return;
}

/*********************************************************************
*
* @purpose  To Update the PIM-DM Interface Statistics
*
* @param    pimdmCB      @b{ (input) } Pointer to the PIM-DM Ctrl Block
*           rtrIfNum     @b{ (input) } Index of the Router Interface for
*                                      Stats has to be updated
*           msgType      @b{ (input) } Type of the PIM-DM Control Pkt
*           statsType    @b{ (input) } Type of the Statistic
*
* @returns  void, if success
* @returns  void, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmIntfStatsUpdate (pimdmCB_t *pimdmCB,
                      L7_uint32 rtrIfNum,
                      PIMDM_CTRL_PKT_TYPE_t msgType,
                      PIMDM_STATS_TYPE_t statsType)
{
  pimdmInterface_t *pimdmIntf = L7_NULLPTR;

  /* Basic validations.
   */
  if ((msgType >= PIMDM_CTRL_PKT_MAX) || (statsType >= PIMDM_STATS_TYPE_MAX) ||
      (rtrIfNum < 1) || (rtrIfNum >= PIMDM_MAX_INTERFACES))
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                 "Invalid msg Type or stats Type or rtrIfNum");
    return L7_FAILURE;
  }

  /* Get the Interface Entry.
   */
  if (pimdmIntfEntryGet (pimdmCB, rtrIfNum, &pimdmIntf) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Intf Entry Get Failed for rtrIfNum-%d.\n", rtrIfNum);
    return L7_FAILURE;
  }
  if (pimdmIntf == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Intf Entry is NULL for rtrIfNum-%d.\n", rtrIfNum);
    return L7_FAILURE;
  }

  /* Update the Statistic.
   */
  (pimdmIntf->intfStats[msgType][statsType])++;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To display the PIM-DM Statistics in detail
*
* @param    pimdmCB    @b{ (input) } Pointer to the PIM-DM Control Block
* @param    rtrIfNum   @b{ (input) } Index of the Router Interface
*
* @returns  void, if success
* @returns  void, if failure
*
* @comments    This API is provided only for debugging purpose and should
*           not be used anywhere in the code flow.
*
* @end
*********************************************************************/
void
pimdmIntfStatsShow (pimdmCB_t *pimdmCB,
                    L7_uint32 rtrIfNum)
{
  pimdmInterface_t *pimdmIntf = L7_NULLPTR;
  L7_uchar8 pktStr[] = "PktType";
  L7_uchar8 rcvStr[] = "Received";
  L7_uchar8 sntStr[] = "Sent";
  L7_uchar8 drpStr[] = "Dropped";
  PIMDM_CTRL_PKT_TYPE_t pktType;
  extern L7_char8 *gPimdmPktType[];
  L7_uchar8 addr[PIMDM_MAX_DBG_ADDR_SIZE];

  /* Basic Validations */
  if (pimdmCB == L7_NULLPTR)
  {
    PIMDM_DEBUG_PRINTF ("Invalid Ctrl Block");
    return;
  }

  /* Get the Interface Entry.
   */
  if (pimdmIntfEntryGet (pimdmCB, rtrIfNum, &pimdmIntf) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Intf Entry Get Failed for rtrIfNum-%d.\n", rtrIfNum);
    return;
  }
  if (pimdmIntf == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Intf Entry is NULL for rtrIfNum-%d.\n", rtrIfNum);
    return;
  }
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "  Interface Address : %s\n", inetAddrPrint(&pimdmIntf->ipAddress,addr));

  /* Display the Interface Statistics */
  PIMDM_DEBUG_PRINTF ("==========================================\n");
  PIMDM_DEBUG_PRINTF ("%14s ", pktStr);
  PIMDM_DEBUG_PRINTF ("%8s ", rcvStr);
  PIMDM_DEBUG_PRINTF ("%8s ", sntStr);
  PIMDM_DEBUG_PRINTF ("%8s ", drpStr);
  PIMDM_DEBUG_PRINTF ("\n==========================================\n");

  for (pktType = 0; pktType < PIMDM_CTRL_PKT_MAX; pktType++)
  {
    PIMDM_STATS_TYPE_t statsType;

    PIMDM_DEBUG_PRINTF ("%14s ", gPimdmPktType[pktType]);
    for (statsType = 0; statsType < PIMDM_STATS_TYPE_MAX; statsType++)
      PIMDM_DEBUG_PRINTF ("%8d", pimdmIntf->intfStats[pktType][statsType]);
    PIMDM_DEBUG_PRINTF ("\n------------------------------------------\n");
  }

  return;
}

/*********************************************************************
*
* @purpose  To display the PIM-DM Statistics in detail
*
* @param    pimdmCB    @b{ (input) } Pointer to the PIM-DM Control Block
* @param    rtrIfNum   @b{ (input) } Index of the Router Interface
*
* @returns  void, if success
* @returns  void, if failure
*
* @comments    This API is provided only for debugging purpose and should
*           not be used anywhere in the code flow.
*
* @end
*********************************************************************/
void
pimdmIntfStatsDisplay (pimdmCB_t *pimdmCB,
                       L7_uint32 rtrIfNum)
{
  pimdmInterface_t *intfEntry = L7_NULLPTR;
  L7_uchar8 emptyStr[] = "     ";
  L7_uint32 tempInt = 0;

  if (pimdmIntfIsEnabled (pimdmCB, rtrIfNum) != L7_TRUE)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "PIM-DM is not Enabled on this Interface - %d.\n", rtrIfNum);
    return;
  }
  if (pimdmIntfEntryGet (pimdmCB, rtrIfNum, &intfEntry ) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Interface-%d Entry is NULL", rtrIfNum);
    return;
  }

  PIMDM_DEBUG_PRINTF ("%4d ", rtrIfNum);
  PIMDM_DEBUG_PRINTF ("Rx   ");
  PIMDM_DEBUG_PRINTF ("%8d ", intfEntry->intfStats[PIMDM_CTRL_PKT_HELLO][PIMDM_STATS_RECEIVED]);
  PIMDM_DEBUG_PRINTF ("%8d ", intfEntry->intfStats[PIMDM_CTRL_PKT_JOIN][PIMDM_STATS_RECEIVED]);
  PIMDM_DEBUG_PRINTF ("%8d ", intfEntry->intfStats[PIMDM_CTRL_PKT_ASSERT][PIMDM_STATS_RECEIVED]);
  PIMDM_DEBUG_PRINTF ("%8d ", intfEntry->intfStats[PIMDM_CTRL_PKT_GRAFT][PIMDM_STATS_RECEIVED]);
  PIMDM_DEBUG_PRINTF ("%8d ", intfEntry->intfStats[PIMDM_CTRL_PKT_GRAFT_ACK][PIMDM_STATS_RECEIVED]);
  PIMDM_DEBUG_PRINTF ("%8d ", intfEntry->intfStats[PIMDM_CTRL_PKT_STATE_REFRESH][PIMDM_STATS_RECEIVED]);
  PIMDM_DEBUG_PRINTF ("%7d ", intfEntry->intfStats[PIMDM_CTRL_PKT_DATA_NO_CACHE][PIMDM_STATS_RECEIVED]);
  PIMDM_DEBUG_PRINTF ("%7d ", intfEntry->intfStats[PIMDM_CTRL_PKT_DATA_WRONG_IF][PIMDM_STATS_RECEIVED]);
  PIMDM_DEBUG_PRINTF ("\n");

  PIMDM_DEBUG_PRINTF ("%s", emptyStr);
  PIMDM_DEBUG_PRINTF ("Tx   ");
  PIMDM_DEBUG_PRINTF ("%8d ", intfEntry->intfStats[PIMDM_CTRL_PKT_HELLO][PIMDM_STATS_SENT]);
  PIMDM_DEBUG_PRINTF ("%8d ", intfEntry->intfStats[PIMDM_CTRL_PKT_JOIN][PIMDM_STATS_SENT]);
  PIMDM_DEBUG_PRINTF ("%8d ", intfEntry->intfStats[PIMDM_CTRL_PKT_ASSERT][PIMDM_STATS_SENT]);
  PIMDM_DEBUG_PRINTF ("%8d ", intfEntry->intfStats[PIMDM_CTRL_PKT_GRAFT][PIMDM_STATS_SENT]);
  PIMDM_DEBUG_PRINTF ("%8d ", intfEntry->intfStats[PIMDM_CTRL_PKT_GRAFT_ACK][PIMDM_STATS_SENT]);
  PIMDM_DEBUG_PRINTF ("%8d ", intfEntry->intfStats[PIMDM_CTRL_PKT_STATE_REFRESH][PIMDM_STATS_SENT]);
  PIMDM_DEBUG_PRINTF ("%7d ", intfEntry->intfStats[PIMDM_CTRL_PKT_DATA_NO_CACHE][PIMDM_STATS_SENT]);
  PIMDM_DEBUG_PRINTF ("%7d ", intfEntry->intfStats[PIMDM_CTRL_PKT_DATA_WRONG_IF][PIMDM_STATS_SENT]);
  PIMDM_DEBUG_PRINTF ("\n");

  PIMDM_DEBUG_PRINTF ("%s", emptyStr);
  PIMDM_DEBUG_PRINTF ("Drop ");
  PIMDM_DEBUG_PRINTF ("%8d ", intfEntry->intfStats[PIMDM_CTRL_PKT_HELLO][PIMDM_STATS_DROPPED]);
  PIMDM_DEBUG_PRINTF ("%8d ", intfEntry->intfStats[PIMDM_CTRL_PKT_JOIN][PIMDM_STATS_DROPPED]);
  PIMDM_DEBUG_PRINTF ("%8d ", intfEntry->intfStats[PIMDM_CTRL_PKT_ASSERT][PIMDM_STATS_DROPPED]);
  PIMDM_DEBUG_PRINTF ("%8d ", intfEntry->intfStats[PIMDM_CTRL_PKT_GRAFT][PIMDM_STATS_DROPPED]);
  PIMDM_DEBUG_PRINTF ("%8d ", intfEntry->intfStats[PIMDM_CTRL_PKT_GRAFT_ACK][PIMDM_STATS_DROPPED]);
  PIMDM_DEBUG_PRINTF ("%8d ", intfEntry->intfStats[PIMDM_CTRL_PKT_STATE_REFRESH][PIMDM_STATS_DROPPED]);
  PIMDM_DEBUG_PRINTF ("%7d ", intfEntry->intfStats[PIMDM_CTRL_PKT_DATA_NO_CACHE][PIMDM_STATS_DROPPED]);
  PIMDM_DEBUG_PRINTF ("%7d ", intfEntry->intfStats[PIMDM_CTRL_PKT_DATA_WRONG_IF][PIMDM_STATS_DROPPED]);
  PIMDM_DEBUG_PRINTF ("\n");

  PIMDM_DEBUG_PRINTF ("\n");
  PIMDM_DEBUG_PRINTF ("%s", emptyStr);
  PIMDM_DEBUG_PRINTF ("Invalid Packets Received - %d", tempInt);
  PIMDM_DEBUG_PRINTF ("\n");

  PIMDM_DEBUG_PRINTF ("--------------------------------------------------------------------------------\n");

  return;
}

