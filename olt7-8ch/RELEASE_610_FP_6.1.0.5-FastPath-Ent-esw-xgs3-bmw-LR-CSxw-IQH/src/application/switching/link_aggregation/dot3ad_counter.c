/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    dot3ad_counter.c
* @purpose     dot3ad counter maintenance
* @component   lag
* @comments    none
* @create      11/05/2001
* @author      skalyanam
* @end
*             
**********************************************************************/

/*************************************************************
                    
*************************************************************/




#include "dot3ad_include.h"
#include "platform_counters.h"
#include "statsapi.h"
#include "usmdb_common.h"
#include "usmdb_sim_api.h"



/* LAG counter ID lists
 *   - mutants consisting of all member intf counters
 *   - LAG-sourced external counters
 */
#define DOT3AD_MUTANT_LIST_ENTRY_SIZE     sizeof(dot3adMutantCtr_t)
static dot3adMutantCtr_t dot3adMutantCtrIdList[] =
{
  {L7_PLATFORM_CTR_RX_TOTAL_BYTES,    L7_PLATFORM_CTR_LAG_RX_TOTAL_BYTES},         
  {L7_PLATFORM_CTR_RX_64,             L7_PLATFORM_CTR_LAG_RX_64},                  
  {L7_PLATFORM_CTR_RX_65_127,         L7_PLATFORM_CTR_LAG_RX_65_127},              
  {L7_PLATFORM_CTR_RX_128_255,        L7_PLATFORM_CTR_LAG_RX_128_255},             
  {L7_PLATFORM_CTR_RX_256_511,        L7_PLATFORM_CTR_LAG_RX_256_511},             
  {L7_PLATFORM_CTR_RX_512_1023,       L7_PLATFORM_CTR_LAG_RX_512_1023},            
  {L7_PLATFORM_CTR_RX_1024_1518,      L7_PLATFORM_CTR_LAG_RX_1024_1518},           
  {L7_PLATFORM_CTR_RX_1519_1530,      L7_PLATFORM_CTR_LAG_RX_1519_1530},           
  {L7_PLATFORM_CTR_RX_GOOD_OVERSIZE,  L7_PLATFORM_CTR_LAG_RX_GOOD_OVERSIZE},       
  {L7_PLATFORM_CTR_RX_ERROR_OVERSIZE, L7_PLATFORM_CTR_LAG_RX_ERROR_OVERSIZE},      
  {L7_PLATFORM_CTR_RX_GOOD_UNDERSIZE, L7_PLATFORM_CTR_LAG_RX_GOOD_UNDERSIZE},      
  {L7_PLATFORM_CTR_RX_ERROR_UNDERSIZE, L7_PLATFORM_CTR_LAG_RX_ERROR_UNDERSIZE},     
  {L7_PLATFORM_CTR_RX_UCAST_FRAMES,   L7_PLATFORM_CTR_LAG_RX_UCAST_FRAMES},        
  {L7_PLATFORM_CTR_RX_MCAST_FRAMES,   L7_PLATFORM_CTR_LAG_RX_MCAST_FRAMES},        
  {L7_PLATFORM_CTR_RX_BCAST_FRAMES,   L7_PLATFORM_CTR_LAG_RX_BCAST_FRAMES},        
  {L7_PLATFORM_CTR_RX_ALIGN_ERRORS,   L7_PLATFORM_CTR_LAG_RX_ALIGN_ERRORS},        
  {L7_PLATFORM_CTR_RX_FCS_ERRORS,     L7_PLATFORM_CTR_LAG_RX_FCS_ERRORS},          
  {L7_PLATFORM_CTR_RX_OVERRUNS,       L7_PLATFORM_CTR_LAG_RX_OVERRUNS},            
  {L7_PLATFORM_CTR_RX_FRAME_TOO_LONG, L7_PLATFORM_CTR_LAG_RX_FRAME_TOO_LONG},            

  {L7_PLATFORM_CTR_TX_TOTAL_BYTES,    L7_PLATFORM_CTR_LAG_TX_TOTAL_BYTES},         
  {L7_PLATFORM_CTR_TX_64,             L7_PLATFORM_CTR_LAG_TX_64},                  
  {L7_PLATFORM_CTR_TX_65_127,         L7_PLATFORM_CTR_LAG_TX_65_127},              
  {L7_PLATFORM_CTR_TX_128_255,        L7_PLATFORM_CTR_LAG_TX_128_255},             
  {L7_PLATFORM_CTR_TX_256_511,        L7_PLATFORM_CTR_LAG_TX_256_511},             
  {L7_PLATFORM_CTR_TX_512_1023,       L7_PLATFORM_CTR_LAG_TX_512_1023},            
  {L7_PLATFORM_CTR_TX_1024_1518,      L7_PLATFORM_CTR_LAG_TX_1024_1518},           
  {L7_PLATFORM_CTR_TX_1519_1530,      L7_PLATFORM_CTR_LAG_TX_1519_1530},           
  {L7_PLATFORM_CTR_TX_UCAST_FRAMES,   L7_PLATFORM_CTR_LAG_TX_UCAST_FRAMES},        
  {L7_PLATFORM_CTR_TX_MCAST_FRAMES,   L7_PLATFORM_CTR_LAG_TX_MCAST_FRAMES},        
  {L7_PLATFORM_CTR_TX_BCAST_FRAMES,   L7_PLATFORM_CTR_LAG_TX_BCAST_FRAMES},        
  {L7_PLATFORM_CTR_TX_FCS_ERRORS,     L7_PLATFORM_CTR_LAG_TX_FCS_ERRORS},          
  {L7_PLATFORM_CTR_TX_OVERSIZED,      L7_PLATFORM_CTR_LAG_TX_OVERSIZED},           
  {L7_PLATFORM_CTR_TX_UNDERRUN_ERRORS, 
    L7_PLATFORM_CTR_LAG_TX_UNDERRUN_ERRORS},     
  {L7_PLATFORM_CTR_TX_ONE_COLLISION,  L7_PLATFORM_CTR_LAG_TX_ONE_COLLISION},       
  {L7_PLATFORM_CTR_TX_MULTIPLE_COLLISION, 
    L7_PLATFORM_CTR_LAG_TX_MULTIPLE_COLLISION},  
  {L7_PLATFORM_CTR_TX_EXCESSIVE_COLLISION, 
    L7_PLATFORM_CTR_LAG_TX_EXCESSIVE_COLLISION},
  {L7_PLATFORM_CTR_TX_LATE_COLLISION,  
    L7_PLATFORM_CTR_LAG_TX_LATE_COLLISION},

  {L7_PLATFORM_CTR_TX_RX_64,                L7_PLATFORM_CTR_LAG_TX_RX_64},                  
  {L7_PLATFORM_CTR_TX_RX_65_127,            L7_PLATFORM_CTR_LAG_TX_RX_65_127},              
  {L7_PLATFORM_CTR_TX_RX_128_255,           L7_PLATFORM_CTR_LAG_TX_RX_128_255},             
  {L7_PLATFORM_CTR_TX_RX_256_511,           L7_PLATFORM_CTR_LAG_TX_RX_256_511},             
  {L7_PLATFORM_CTR_TX_RX_512_1023,          L7_PLATFORM_CTR_LAG_TX_RX_512_1023},            
  {L7_PLATFORM_CTR_TX_RX_1024_1518,         L7_PLATFORM_CTR_LAG_TX_RX_1024_1518},           
  {L7_PLATFORM_CTR_TX_RX_1519_1522,         L7_PLATFORM_CTR_LAG_TX_RX_1519_1522},           
  {L7_PLATFORM_CTR_TX_RX_1523_2047,         L7_PLATFORM_CTR_LAG_TX_RX_1523_2047},           
  {L7_PLATFORM_CTR_TX_RX_2048_4095,         L7_PLATFORM_CTR_LAG_TX_RX_2048_4095},           
  {L7_PLATFORM_CTR_TX_RX_4096_9216,         L7_PLATFORM_CTR_LAG_TX_RX_4096_9216},
  {L7_PLATFORM_CTR_ETHER_STATS_DROP_EVENTS, L7_PLATFORM_CTR_LAG_ETHER_STATS_DROP_EVENTS},           
  {L7_PLATFORM_CTR_SNMPIFOUTDISCARD_FRAMES, L7_PLATFORM_CTR_LAG_SNMPIFOUTDISCARD_FRAMES},           
  {L7_PLATFORM_CTR_SNMPIFINDISCARD_FRAMES,  L7_PLATFORM_CTR_LAG_SNMPIFINDISCARD_FRAMES}          

};

#define DOT3AD_EXTERNAL_LIST_ENTRY_SIZE   sizeof(L7_uint32)
static L7_uint32 dot3adExternalCtrIdList[] = 
{
  L7_PLATFORM_CTR_LAG_NUM_MEMBERS,
  L7_PLATFORM_CTR_LAG_NUM_ACTIVE_MEMBERS,
};


/*********************************************************************
* @purpose  Get one of the LAG external counters for this interface.
*
* @param    *pCtr       Pointer to counter specifier
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This function is only intended to be called directly by the 
*           Stats Manager component, since this function pointer was
*           provided by the dot3ad component when the external counters
*           were created.  It is not a public API function.
*      
* @end
*********************************************************************/
L7_RC_t dot3adCounterExternalGet(counterValue_entry_t *pCtr)
{
  dot3ad_agg_t      *agg;
  L7_RC_t        rc;

  /* Set up pointer to LAG table entry for this interface. */
  agg = dot3adAggIntfFind(pCtr->cKey);
  if (agg == L7_NULLPTR)
  {
    pCtr->status = L7_FAILURE;
    return(L7_FAILURE);
  }

  rc = L7_SUCCESS;

  /* Treat all LAG-sourced counters as 32-bits for simplicity */
  pCtr->cSize = C32_BITS;

  switch (pCtr->cId)
  {
  case L7_PLATFORM_CTR_LAG_NUM_MEMBERS:
    pCtr->cValue.high = 0;
    pCtr->cValue.low = agg->currNumWaitSelectedMembers;
    break;

  case L7_PLATFORM_CTR_LAG_NUM_ACTIVE_MEMBERS:
    pCtr->cValue.high = 0;
    pCtr->cValue.low = agg->currNumMembers;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  pCtr->status = rc;
  return(rc);
}


/*********************************************************************
* @purpose  Creates the set of counters for the specified LAG interface.
*           This covers both mutant and non-mutant varieties.
*
* @param    intIfNum    internal interface number of the LAG
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The mutant counters created here are initially empty, that is,
*           they have no associated mutlings.  Each time the LAG
*           membership is updated, the counters associated with the 
*           member interfaces are added/deleted as mutlings through
*           the Stats API.
*       
* @end
*********************************************************************/
L7_RC_t dot3adCounterCreate(L7_uint32 intIfNum)
{
  L7_uint32            numCtrs;
  statsParm_entry_t    s;
  L7_uint32            i;
  dot3adMutantCtr_t       *pMCtr;
  L7_uint32            *pXCtr;
  L7_VOIDFUNCPTR_t	   pFvoid = L7_NULLPTR;

  /* Create the set of LAG mutant counters.
   *
   * NOTE:  These mutant counters are initially created with no mutlings.
   */
  numCtrs = (L7_uint32)(sizeof(dot3adMutantCtrIdList) / DOT3AD_MUTANT_LIST_ENTRY_SIZE);
  s.cType = MutantCtr;
  s.cKey = intIfNum;
  s.cSize = C64_BITS; 
  s.isResettable = L7_TRUE;
  s.pSource = pFvoid;
  s.pMutlingsParmList = L7_NULL;

  for (i = 0, pMCtr = dot3adMutantCtrIdList; i < numCtrs; i++, pMCtr++)
  {
    s.cId = pMCtr->mutant;

    if ((statsCreate(1, &s) != L7_SUCCESS) || (s.status != L7_SUCCESS))
    {
      return(L7_FAILURE);
    }
  }

  /* Create the set of LAG external counters */
  numCtrs = (L7_uint32)(sizeof(dot3adExternalCtrIdList) / DOT3AD_EXTERNAL_LIST_ENTRY_SIZE);
  s.cType = ExternalCtr;
  s.cKey = intIfNum;
  s.cSize = C32_BITS;
  s.isResettable = L7_FALSE;
  s.pSource = (L7_VOIDFUNCPTR_t)dot3adCounterExternalGet;
  s.pMutlingsParmList = L7_NULL;

  for (i = 0, pXCtr = dot3adExternalCtrIdList; i < numCtrs; i++, pXCtr++)
  {
    s.cId = *pXCtr;

    if ((statsCreate(1, &s) != L7_SUCCESS) || (s.status != L7_SUCCESS))
    {
      return(L7_FAILURE);
    }
  }

  /* Invoke USMDB to create its set of LAG user counters */
  if (usmDbUserStatsCreate(usmDbThisUnitGet(), L7_USMDB_USER_STAT_LAG, intIfNum) != L7_SUCCESS)
    return(L7_FAILURE);

  return(L7_SUCCESS);
}


/*********************************************************************
* @purpose  adds mutlings of the added LAG members
*
* @param    lagIntf    internal interface number of the LAG interface
* @param    memIntf    internal interface number of the member to be added
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t dot3adCounterMutlingAdd(L7_uint32 lagIntf, L7_uint32 memIntf)
{
  L7_uint32            i;
  L7_uint32            numCtrs;
  statsParm_entry_t    s;
  mutling_parm_list_t  mpl;
  mutling_id_t         mid;
  dot3adMutantCtr_t    *pMCtr;
  L7_VOIDFUNCPTR_t	   pFvoid = L7_NULLPTR;

  /* Most of the setup info is the same for all mutlings */
  numCtrs = (L7_uint32)(sizeof(dot3adMutantCtrIdList) / DOT3AD_MUTANT_LIST_ENTRY_SIZE);
  s.cType = MutantCtr;
  s.cKey = lagIntf;
  s.cSize = C64_BITS;
  s.isResettable = L7_TRUE;
  s.pSource = pFvoid;
  s.pMutlingsParmList = &mpl;

  /* Update each LAG mutant counter individually */
  for (i = 0, pMCtr = dot3adMutantCtrIdList; i < numCtrs; i++, pMCtr++)
  {
    if (pMCtr->mutant != 0 && 
		pMCtr->mutling != 0)
	{
	  s.cId = pMCtr->mutant;     /* LAG mutant counter ID */


      mid.cId = pMCtr->mutling;  /* member mutling counter ID */
      mid.cSign = cPLUS;
      mid.cKey = memIntf;        /* mutling key is member internal interface number */
  
  
      mpl.listSize = 1;
      mpl.pMutlingParms = &mid;
  
      /* Add mutling set to this individual LAG mutant counter */
      if ((statsMutlingsAdd(1, &s) != L7_SUCCESS)
          || (s.status != L7_SUCCESS))
      {
        return(L7_FAILURE);
      }
	}

  }

  return(L7_SUCCESS);
}
/*********************************************************************
* @purpose  Removes multings of deleted LAG members from the set of 
*           LAG mutant counters 
*
* @param    lagIntf    internal interface number of the LAG interface
* @param    memIntf    internal interface number of the member to be deleted
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t dot3adCounterMutlingDelete(L7_uint32 lagIntf, L7_uint32 memIntf)
{
  L7_uint32            i;
  L7_uint32            numCtrs;
  statsParm_entry_t    s;
  mutling_parm_list_t  mpl;
  mutling_id_t         mid;
  dot3adMutantCtr_t    *pMCtr;
  L7_VOIDFUNCPTR_t	   pFvoid = L7_NULLPTR;

  /* Most of the setup info is the same for all mutlings, both add and del */
  numCtrs = (L7_uint32)(sizeof(dot3adMutantCtrIdList) / DOT3AD_MUTANT_LIST_ENTRY_SIZE);
  s.cType = MutantCtr;
  s.cKey = lagIntf;
  s.cSize = C64_BITS;
  s.isResettable = L7_TRUE;
  s.pSource = pFvoid;
  s.pMutlingsParmList = &mpl;

  /* Update each LAG mutant counter individually */
  for (i = 0, pMCtr = dot3adMutantCtrIdList; i < numCtrs; i++, pMCtr++)
  {
    if (pMCtr->mutant != 0 && 
		pMCtr->mutling != 0)
	{
      s.cId = pMCtr->mutant;                    /* LAG mutant counter ID */

      mid.cId = pMCtr->mutling; /* member mutling counter ID */
      mid.cSign = cPLUS;
      mid.cKey = memIntf;   /* mutling key is member internal interface number */



      mpl.listSize = 1;
      mpl.pMutlingParms = &mid;

      /* Delete mutling set from this individual LAG mutant counter */
      if ((statsMutlingsDel(1, &s) != L7_SUCCESS)
          || (s.status != L7_SUCCESS))
      {
        return(L7_FAILURE);
      }
    }
  }

  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Resets all mutant counters for the specified LAG interface.
*
* @param    intIfNum      internal interface number of the LAG
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This is not a public API function, but is intended for use
*           by the LAG component when it needs to clear out the set
*           of mutant counters for the specified LAG.  This might be
*           useful when a LAG is (re)created so that it doesn't carry
*           over obsolete information.
*       
* @notes    The LAG *external* counters are all non-resettable.
*
* @end
*********************************************************************/
L7_RC_t dot3adCounterReset(L7_uint32 intIfNum)
{
  L7_uint32            numCtrs;
  counterValue_entry_t c;
  L7_uint32            i;
  dot3adMutantCtr_t       *pCtr;

  numCtrs = (L7_uint32)(sizeof(dot3adMutantCtrIdList) / DOT3AD_MUTANT_LIST_ENTRY_SIZE);
  pCtr = dot3adMutantCtrIdList;

  c.status = L7_FAILURE;
  c.cMode = setMode;
  c.cKey = intIfNum;
  c.raw = L7_FALSE;

  for (i = 0, pCtr = dot3adMutantCtrIdList; i < numCtrs; i++, pCtr++)
  {
    c.cId = pCtr->mutant;

    if ((statsReset(1, &c) != L7_SUCCESS) || (c.status != L7_SUCCESS))
    {
      return(L7_FAILURE);
    }
  }

  return(L7_SUCCESS);
}

