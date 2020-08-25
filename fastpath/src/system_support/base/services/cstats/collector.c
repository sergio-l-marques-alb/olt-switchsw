/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename    collector.c
 * @purpose     Implements the collector functionality of stats comp
 * @component   stats
 * @comments    none
 * @create      18/06/2007
 * @author      YSaritha
 * @end
 *
 **********************************************************************/
#include <stdio.h>
#include <string.h>

#include "commdefs.h"           /* lvl7 common definitions   */
#include "datatypes.h"          /* lvl7 data types definition */
#include "osapi.h"              /* lvl7 operating system apis */
#include "statsapi.h"           /* stats public methods */
#include "counter64.h"
#include "collector.h"
#include "presenter.h"
#include "statsconfig.h"
#include "log.h"

static L7_ulong64 cSizeArray[3] = {
  {0x00000000, 0x0000FFFF},
  {0x00000000, 0xFFFFFFFF},
  {0xFFFFFFFF, 0xFFFFFFFF}
};

/*
 **********************************************************************
 *
 * @function  set()
 *
 * @purpose   This function allows the client to reset the value associated
 *            with a counter
 *
 * @param     pCounterValue_entry_t pInfo
 *            L7_doubl64            v
 *
 * @returns   None
 *
 * @end
 *
 **********************************************************************
 */

void ec_set (ExternalCounter * ptr, pCounterValue_entry_t pInfo)
{
  /* If the counter is resettable call the source to reset its value */

  if (ptr->isResettable == L7_TRUE)
  {
    pInfo->cMode = setMode;
    ptr->source (pInfo);
  }
  else
  {
    pInfo->cMode = getMode;
    ptr->source (pInfo);
    ptr->delta = pInfo->cValue; /* store the current value to */
    /* reset the counter from the user's
       perspective */
    pInfo->cValue.high = 0;
    pInfo->cValue.low = 0;
    pInfo->cSize = ptr->size;   /* fill in the size */
    pInfo->status = L7_SUCCESS; /* If we reached till here the operation was successful. */
  }
}

/*
 **********************************************************************
 *
 * @function   increment()
 *
 * @purpose    This function allows the client to increment the value
 *             associated with a Localcounter
 *
 * @param      pCounterValue_entry_t pInfo
 *
 * @returns    None
 *
 * @end
 *
 **********************************************************************
 */

void lc_increment (LocalCounter * ptr, pCounterValue_entry_t pInfo)
{
  ptr->value = counter64_incr (ptr->value);     /* increment value */
  pInfo->cSize = ptr->size;
  /* fill in the size */
  pInfo->status = L7_SUCCESS;   /* If we reached till here the */
  /* operation was successful */
}

/*
 **********************************************************************
 *
 * @function     decrement()
 *
 * @purpose      This function allows the client to decrement the value
 *               associated with a Localcounter
 *
 * @param        pCounterValue_entry_t pInfo
 *
 * @returns      None
 *
 * @end
 *
 **********************************************************************
 */
void lc_decrement (LocalCounter * ptr, pCounterValue_entry_t pInfo)
{
  L7_ulong64 zval;

  zval.low = zval.high = 0;

  if (counter64_notequals (ptr->value, zval))
  {
    ptr->value = counter64_decr (ptr->value);
  }                             /* decrement the counter */
  pInfo->cSize = ptr->size;     /* fill in the size */
  pInfo->status = L7_SUCCESS;   /* If we reached till here the operation was successful */
}

void counter_init (counter_type_t ctype, void *pCtr,
                   pStatsParm_entry_t entry, signedCounter * mutlings)
{
  L7_ushort16 ii, listSize = STATS_MAX_MUTLING;

  MutantCounter *mCtr = L7_NULL;
  LocalCounter *lCtr = L7_NULL;
  ExternalCounter *eCtr = L7_NULL;

  if (entry == L7_NULL)
  {
    printf ("ERROR!! NULL entry in counter_init .\n");
    return;
  }

  switch (ctype)
  {
  case MutantCtr:

    mCtr = (MutantCounter *) pCtr;
    mCtr->cKey = entry->cKey;   /* counter key */
    mCtr->cId = entry->cId;     /* counter id */
    mCtr->value.low = mCtr->value.high = 0;     /* initial value */
    mCtr->size = entry->cSize;  /* set size to s */
    mCtr->isResettable = entry->isResettable;   /* set the value of isResettable to r */
    mCtr->delta.low = mCtr->delta.high = 0;
    mCtr->cType = MutantCtr;
    memset (mCtr->mSet, 0, sizeof (mCtr->mSet));
    if (entry->pMutlingsParmList != L7_NULL)
    {

      if (entry->pMutlingsParmList->listSize < STATS_MAX_MUTLING)
      {
        listSize = entry->pMutlingsParmList->listSize;
      }

      /* Repeatedly insert mutlings in the set verifying if the operation
         was successful at each iteration */
      for (ii = 0; ii < listSize; ii++)
      {
        memcpy ((L7_char8 *) & (mCtr->mSet[ii]), (L7_char8 *) & mutlings[ii],
                sizeof (signedCounter));
      }
    }
    entry->status = L7_SUCCESS;

    break;

  case LocalCtr:

    lCtr = (LocalCounter *) pCtr;
    lCtr->cKey = entry->cKey;   /* counter key */
    lCtr->cId = entry->cId;     /* counter id */
    lCtr->value.low = lCtr->value.high = 0;     /* initial value */
    lCtr->size = entry->cSize;  /* set size to s */
    lCtr->isResettable = entry->isResettable;   /* set the value of isResettable to r */
    lCtr->delta.low = lCtr->delta.high = 0;     /* initialize delta to 0 */

    lCtr->cType = LocalCtr;
    entry->status = L7_SUCCESS;

    break;

  case ExternalCtr:

    eCtr = (ExternalCounter *) pCtr;
    eCtr->cKey = entry->cKey;   /* counter key */
    eCtr->cId = entry->cId;     /* counter id */
    eCtr->value.low = 0;
    eCtr->value.high = 0;       /* initial value */
    eCtr->size = entry->cSize;  /* set size to s */
    eCtr->isResettable = entry->isResettable;   /* set the value of isResettable to r */
    eCtr->delta.low = 0;
    eCtr->delta.high = 0;       /* initialize delta to 0 */

    eCtr->cType = ExternalCtr;
    if (entry->pSource != L7_NULL)
    {
      eCtr->source = entry->pSource;    /* set the source   */
      entry->status = L7_SUCCESS;
    }
    else
    {
      printf ("ERROR!! failure in ec_init function\\n");
      entry->status = L7_FAILURE;
    }
    break;

  default:

    printf (" Unknown counter type for initialisation \n");
    entry->status = L7_FAILURE;
    break;
  }
}

void counter_get (Counter * ptr, pCounterValue_entry_t pInfo)
{
  L7_ulong64 maxVal, result;

  maxVal = cSizeArray[ptr->size];

  if (ptr->isResettable == L7_FALSE && pInfo->raw == L7_FALSE)
  {
    if (counter64_greater_equal (ptr->value, ptr->delta))
    {
      pInfo->cValue = counter64_sub (ptr->value, ptr->delta);
    }
    /* fill in the relative value */
    else
    {
      result = counter64_add ((counter64_sub (maxVal, ptr->delta)), ptr->value);

      /* increment result by 1 to account for the counter wrapping */
      result = counter64_incr (result);
      pInfo->cValue = result;
    }
  }
  /* FIXME */
  else
  {
    pInfo->cValue = ptr->value;
  }
  pInfo->cValue.high &= cSizeArray[ptr->size].high;
  pInfo->cValue.low &= cSizeArray[ptr->size].low;
  pInfo->cSize = ptr->size;     /* fill in the size */
  pInfo->status = L7_SUCCESS;   /* If we reached till here the */
  return;
}

/*
 * *********************************************************************
 *
 * @function     getMutlings()
 *
 * @purpose      This function allows the calling function to get the
 *               sum of the values of the mutlings of a mutant counter
 *
 * @param        pCounterValue_entry_t    c
 * @param        pCounterValue_entry_t    p
 *
 * @returns      None
 *
 * @end
 *
 **********************************************************************
 */

L7_ulong64 mc_getMutlings (MutantCounter * ptr, pCounterValue_entry_t c, pCounterValue_entry_t p)
{
  L7_ulong64 addval, subval, val;
  L7_ushort16 i;
  ExternalCounter *x = L7_NULL;
  MutantCounter   *y = L7_NULL;
  LocalCounter    *z = L7_NULL;


  addval.high = addval.low = subval.high = subval.low = val.high = val.low = 0;

  /* Iterate through the set of mutlings to get the
     individual values of the mutlings and add it
     up with the corresponding sign to arrive at
     the value of the mutantCounter */
  for (i = 0; (i < STATS_MAX_MUTLING) && (c->status != L7_FAILURE); i++)
  {
    /* Check if this is a valid counter pointer */
    if (ptr->mSet[i].pCtr == L7_NULLPTR)
    {
      continue;
    }

    switch ( ptr->mSet[i].ctype_signed )
    {
      case MutantCtr:
        y = (MutantCounter *) (ptr->mSet[i].pCtr);
        if ( y == L7_NULL)
        {
          printf ("  mc_getMutlings :  Mutant get   Failed\n");
          val.high = val.low = 0;
          return val;
        }
        c->status = L7_FAILURE;
        c->cId = y->cId;
        c->cKey = y->cKey;
        c->raw = p->raw;
        c->cMode = getMode;
        get_counter_value ( y  , c, MutantCtr );
        break;

      case ExternalCtr:
        x = (ExternalCounter *) (ptr->mSet[i].pCtr);
        if (x == L7_NULL)
        {
          printf ("Failed\n");
          val.high = val.low = 0;
          return val;
        }
        c->status = L7_FAILURE;
        c->cId = x->cId;
        c->cKey = x->cKey;
        c->raw = p->raw;
        c->cMode = getMode;
        get_counter_value (x, c, ExternalCtr);
        break;

      case LocalCtr :
        z  = (LocalCounter *) (ptr->mSet[i].pCtr);
        if ( z == L7_NULL)
        {
          printf ("Failed\n");
          val.high = val.low = 0;
          return val;
        }
        c->status = L7_FAILURE;
        c->cId = z->cId;
        c->cKey = z->cKey;
        c->raw = p->raw;
        c->cMode = getMode;
        printf("  Untested check this  \n");
        get_counter_value ( z , c, LocalCtr);
        break;

      default:
        printf("  Untested check this  \n");
        break;

    }

    if (ptr->mSet[i].sign == cPLUS)
    {
      addval = counter64_add (addval, (c->cValue));
    }
    else if (ptr->mSet[i].sign == cMINUS)
    {                           /* cMINUS */
      subval = counter64_add (subval, c->cValue);
    }

    /* If we could not get the value of even one counter
       the operation was a failure so set pInfo->status
       to L7_FAILURE */
    if (c->status == L7_FAILURE)
    {
      p->status = L7_FAILURE;
    }
  }

  if (counter64_greater (addval, subval))
  {
    val = counter64_sub (addval, subval);
  }

  return val;
}

/*
 **********************************************************************
 *
 * @function     resetMutlings()
 *
 * @purpose      This function allows the calling function to reset the
 *               mutlings of a mutant counter
 *
 * @param        pCounterValue_entry_t   c
 * @param        pCounterValue_entry_t   p
 * @param        L7_ulong64              v
 *
 * @returns      None
 *
 * @end
 *
 **********************************************************************
 */
void mc_resetMutlings (MutantCounter * ptr, pCounterValue_entry_t c, pCounterValue_entry_t p)
{
  /* Iterate through the set of mutlings to set the
     individual values of the mutlings  */
  L7_ushort16 i;

  for (i = 0; (i < STATS_MAX_MUTLING) && (c->status != L7_FAILURE); i++)
  {
    /* Check if this is a valid counter pointer */
    if (ptr->mSet[i].pCtr == L7_NULLPTR)
    {
      continue;
    }

    c->cId = ptr->mSet[i].pCtr->cId;
    c->cKey = ptr->mSet[i].pCtr->cKey;
    c->status = L7_FAILURE;
    c->cValue = p->cValue;
    c->raw = p->raw;
    c->cMode = setMode;

    /*   XXX  TODO   */
    /* This part of the code is not tested,
       And I think this should be ec_set( ptr->mSet[i].pCtr,c )
     */
#if 0
    mc_set (ptr->mSet[i].pCtr, c);
#endif
    ec_set ((ExternalCounter *) ptr->mSet[i].pCtr, c);

    /* If we could not reset the value of even one counter
       the operation was a failure so set pInfo->status
       to L7_FAILURE */
    if (c->status == L7_FAILURE)
    {
      p->status = L7_FAILURE;
    }
  }

  return;
}

/*
 **********************************************************************
 *
 * @function     set()
 *
 * @purpose      This function allows the client to reset the value associated
 *               with a counter
 *
 * @param        pCounterValue_entry_t pInfo
 *
 * @returns      None
 *
 * @end
 *
 **********************************************************************
 */
void mc_set (MutantCounter * ptr, pCounterValue_entry_t pInfo)
{
  counterValue_entry_t CtrStatus;
  pCounterValue_entry_t pCtrStatus = &CtrStatus;

  /* Set status to success, if the operation
     fails status will be set to failure by the
     function */

  pCtrStatus->status = L7_SUCCESS;
  if (ptr->isResettable == L7_FALSE)
  {
    ptr->delta = mc_getMutlings (ptr, pCtrStatus, pInfo);
  }
  else
  {
    mc_resetMutlings (ptr, pCtrStatus, pInfo);
  }

  if (pCtrStatus->status == L7_SUCCESS)
  {
    pInfo->status = L7_SUCCESS;
  }
  else
  {
    printf ("ERROR!! failure in function mc_set.\n");
  }
  ptr->value = pInfo->cValue;
  pInfo->cSize = ptr->size;     /* fill in the size */

  return;
}

/*
 **********************************************************************
 *
 * @function     addMutlings()
 *
 * @purpose      This function allows the client to dynamically add
 *               mutlings to a mutantCounter
 *
 * @param        pStatsParm_entry_t pM
 *
 * @returns      None
 *
 * @end
 *
 **********************************************************************
 */

void mc_addMutlings (MutantCounter * ptr, pStatsParm_entry_t pM, signedCounter * mutlings)
{
  L7_ushort16 i, j, listSize = STATS_MAX_MUTLING;

  if (pM->pMutlingsParmList->listSize < STATS_MAX_MUTLING)
  {
    listSize = pM->pMutlingsParmList->listSize;
  }

  /* Repeatedly insert mutlings in the set verifying if the operation
     was successful at each iteration */
  if (mutlings != L7_NULL)
  {
    for (i = 0; i < listSize; i++)
    {
      for (j = 0; j < STATS_MAX_MUTLING; j++)
      {
        /* Find a vacancy to add the new mutling */
        if (ptr->mSet[j].pCtr == L7_NULLPTR)
        {
          memcpy ((L7_char8 *) & (ptr->mSet[j]), (L7_char8 *) & mutlings[i],
                  sizeof (signedCounter));
          break;
        }
      }
    }

    /* Set status to success after inserting mutlings into set */
    pM->status = L7_SUCCESS;
  }
  else
  {
    printf ("ERROR!! failure in function mc_addmutlings\n");
    pM->status = L7_FAILURE;
  }

  return;
}

/*
 **********************************************************************
 *
 * @function     delMutlings()
 *
 * @purpose      This function allows the client to dynamically delete
 *               mutlings from a mutantCounter
 *
 * @param        pStatsParm_entry_t pM
 *
 * @returns      None
 *
 * @end
 *
 **********************************************************************
 */

void mc_delMutlings (MutantCounter * ptr, pStatsParm_entry_t pM, signedCounter * mutlings)
{
  /* Iterate through the set of mutlings to set the
     individual values of the mutlings  */
  L7_short16 i, j;

  if (pM->pMutlingsParmList != L7_NULL)
  {
    for (i = 0; i < pM->pMutlingsParmList->listSize; i++)
    {
      /* If the counter to delete is a null ptr go to the next entry */
      if (mutlings[i].pCtr == L7_NULLPTR)
      {
        continue;
      }

      for (j = 0; j < STATS_MAX_MUTLING; j++)
      {
        /* Sanity check this is not a vacant entry in the set */
        if (ptr->mSet[j].pCtr == L7_NULLPTR)
        {
          continue;
        }

        /* If the key and id match delete this counter from the set */
        if ((ptr->mSet[j].pCtr->cKey == mutlings[i].pCtr->cKey) &&
            (ptr->mSet[j].pCtr->cId == mutlings[i].pCtr->cId))
        {
          ptr->mSet[j].sign = cINVALID;
          ptr->mSet[j].pCtr = L7_NULL;
          break;
        }
      }
    }

    pM->status = L7_SUCCESS;
  }
  else
  {
    printf ("ERROR!! failure in function mc_delMutlings.\n");
    pM->status = L7_FAILURE;
  }

  return;
}

void collector_init (Collector * ptr)
{
  ptr->pLCtr = L7_NULL;
  ptr->pECtr = L7_NULL;
  ptr->pMCtr = L7_NULL;
  ptr->pMutantCtrFreeList = L7_NULL;
  ptr->pExternalCtrFreeList = L7_NULL;
  ptr->pLocalCtrFreeList = L7_NULL;

  keyPair_init (ptr->flyweight);
}

/*
 **********************************************************************
 *
 * @function     get()
 *
 * @purpose      This function allows the client to get the value associated
 *               with a counter
 *
 * @param        pCounterValue_entry_t pCtrValInfo
 *
 * @returns      None
 *
 * @notes        Completes the provided counterValue_entry_t with the required
 *               information about the counter
 *
 * @end
 *
 **********************************************************************
 */

void collector_get (Collector * ptr, pCounterValue_entry_t pCtrValInfo)
{
  /* Lookup the flyweight if the Ctr already exists */
  L7_RC_t rc;
  keyPair kp;

  kp.key = pCtrValInfo->cKey;
  kp.id = pCtrValInfo->cId;
  rc = keyPair_get (&kp, ptr->flyweight);

  /* If the Ctr exists call its method get to determine its value */
  if (rc != L7_FAILURE)
  {
    /* set status to failure before calling the get method
       this will be reset to success if the counter get
       was successful */
    pCtrValInfo->status = L7_FAILURE;

    get_counter_value (kp.ptr, pCtrValInfo, kp.ctype);
  }
  /* If the Ctr does not exist set status as L7_FAILURE */
  else
  {
    printf ("ERROR!! failure in function c_get.\n");
    pCtrValInfo->status = L7_FAILURE;
  }

  return;
}

/*
 **********************************************************************
 *
 * @function     reset()
 *
 * @purpose      This function allows the client to reset the value associated
 *               with a counter
 *
 * @param        counterStatus_entry_t  pCtrStatus
 *
 * @returns      None
 *
 * @end
 *
 **********************************************************************
 */
void collector_reset (Collector * ptr, pCounterValue_entry_t pCtrStatusInfo)
{
  /* Lookup the flyweight if the Ctr already exists */
  L7_RC_t rc;
  keyPair kp;

  kp.key = pCtrStatusInfo->cKey;
  kp.id = pCtrStatusInfo->cId;

  rc = keyPair_get (&kp, ptr->flyweight);

  /* If the Ctr exists call its method set to reset its value */
  if (rc != L7_FAILURE)
  {
    /* set status to failure before calling the set method
       this will be reset to success if the counter set
       was successful */
    pCtrStatusInfo->status = L7_FAILURE;
    pCtrStatusInfo->cValue.high = 0;
    pCtrStatusInfo->cValue.low = 0;
    switch (kp.ctype)
    {
    case LocalCtr:

      lc_set ((LocalCounter *) (kp.ptr), pCtrStatusInfo);
      break;
    case ExternalCtr:
      ec_set ((ExternalCounter *) (kp.ptr), pCtrStatusInfo);
      break;
    case MutantCtr:
      mc_set ((MutantCounter *) (kp.ptr), pCtrStatusInfo);
      break;
    }
  }
  /* If the Ctr does not exist set status as L7_FAILURE */
  else
  {
    pCtrStatusInfo->status = L7_FAILURE;
  }

  return;
}

/*
 **********************************************************************
 *
 * @function     increment()
 *
 * @purpose      This function allows the client to increment the value
 *               associated with a Localcounter
 *
 * @param        pCounterValue_entry_t  pCtrIncrInfo
 *
 * @returns      None
 *
 * @end
 *
 **********************************************************************
 */

void c_increment (Collector * ptr, pCounterValue_entry_t pCtrIncrInfo)
{
  /* Lookup the flyweight if the Ctr already exists */
  L7_RC_t rc;
  keyPair kp;

  kp.key = pCtrIncrInfo->cKey;
  kp.id = pCtrIncrInfo->cId;

  rc = keyPair_get (&kp, ptr->flyweight);

  /* If the Ctr exists call its method increment */
  if (rc != L7_FAILURE)
  {
    /* set status to failure before calling the increment method
       this will be reset to success if the counter increment
       was successful */
    pCtrIncrInfo->status = L7_FAILURE;
    switch (kp.ctype)
    {
    case LocalCtr:

      lc_increment ((LocalCounter *) kp.ptr, pCtrIncrInfo);
      break;
    case ExternalCtr:
      return;
      /*ec_increment((ExternalCounter*)kp.ptr, pCtrIncrInfo); */

    case MutantCtr:
      return;

    }

  }
  else
  {
    pCtrIncrInfo->status = L7_FAILURE;
    printf ("ERROR!! failure in c_increment.\n");
  }
}

/*
 **********************************************************************
 *
 * @function     decrement()
 *
 * @purpose      This function allows the client to decrement the value
 *               associated with a Localcounter
 *
 * @param        pCounterValue_entry_t  pCtrDecrInfo
 *
 * @returns      None
 *
 * @end
 *
 **********************************************************************
 */

void c_decrement (Collector * ptr, pCounterValue_entry_t pCtrDecrInfo)
{
  /* Lookup the flyweight if the Ctr already exists */
  L7_RC_t rc;
  keyPair kp;

  kp.key = pCtrDecrInfo->cKey;
  kp.id = pCtrDecrInfo->cId;

  rc = keyPair_get (&kp, ptr->flyweight);

  /* If the Ctr exists call its method decrement */
  if (rc != L7_FAILURE)
  {
    /* set status to failure before calling the decrement method
       this will be reset to success if the counter decrement
       was successful */
    pCtrDecrInfo->status = L7_FAILURE;
    switch (kp.ctype)
    {
    case LocalCtr:

      lc_decrement ((LocalCounter *) (kp.ptr), pCtrDecrInfo);
      break;
    case ExternalCtr:
      /*ec_decrement((ExternalCounter*)kp.ptr, pCtrDecrInfo); */
      return;

    case MutantCtr:
      return;

    }

  }
  /* If the Ctr does not exist set status as L7_FAILURE */
  else
  {
    printf ("ERROR!! failure in c_decrement.\n");
    pCtrDecrInfo->status = L7_FAILURE;
  }
}

/*
 **********************************************************************
 *
 * @function     createMutlingList()
 *
 * @purpose      This function allows the calling function to create an
 *               array of mutlings
 *
 * @param        pStatsParm_entry_t   c
 * @param        signedCounter        *m
 *
 * @returns      None
 *
 * @end
 *
 **********************************************************************
 */

void collector_createMutlingList (Collector * ptr, pStatsParm_entry_t c, signedCounter * m)
{
  L7_ushort16 i;
  keyPair kp;
  pMutling_id_t curr;
  void *pTempCtr = L7_NULL;
  L7_RC_t rc;

  curr = c->pMutlingsParmList->pMutlingParms;

  for (i = 0; (c->status != L7_FAILURE) && (i < c->pMutlingsParmList->listSize); i++)
  {
    /* lookup the keypair in the flyweight to get the corresponding
       counter pointer */

    kp.key = curr->cKey;
    kp.id = curr->cId;
    rc = keyPair_get (&kp, ptr->flyweight);
    if (rc != L7_FAILURE)
    {
      pTempCtr = (kp.ptr);
    }
    else
    {
      printf ("createMutlingList: keyPair_get 0x%08x failed\n", kp.id);
      return;
    }

    if (pTempCtr != L7_NULL)
    {                           /* we have found the mutling add it to the mutling array */
      m[i].sign = curr->cSign;
      m[i].pCtr = pTempCtr;
      m[i].ctype_signed = kp.ctype;
    }
    else
    {
      printf ("ERROR!! failure in collector_createmutlingslist\n");
      c->status = L7_FAILURE;   /* if the mutling could */
    }
    /* not be recovered */
    curr++;
  }
}

void lc_set (LocalCounter * ptr, pCounterValue_entry_t pInfo)
{
  if (ptr->isResettable == L7_FALSE)
  {
    ptr->delta = ptr->value;
  }
  else
  {
    ptr->value = pInfo->cValue;
  }

  pInfo->cSize = ptr->size;
  pInfo->status = L7_SUCCESS;
}

void get_counter_value (void *pCtr, pCounterValue_entry_t pInfo, counter_type_t ctype)
{
  L7_ulong64 maxVal, result;
  LocalCounter *lCtr = L7_NULL;
  ExternalCounter *eCtr = L7_NULL;
  MutantCounter *mCtr = L7_NULL;

  switch (ctype)
  {
  case LocalCtr:

    lCtr = (LocalCounter *) pCtr;
    maxVal.high = cSizeArray[lCtr->size].high;
    maxVal.low = cSizeArray[lCtr->size].low;
    if (lCtr->isResettable == L7_FALSE && pInfo->raw == L7_FALSE)
    {
      if (counter64_greater_equal (lCtr->value, lCtr->delta))
      {
        pInfo->cValue = counter64_sub (lCtr->value, lCtr->delta);
      }
      else
      {
        result = counter64_add (counter64_sub (maxVal, lCtr->delta), lCtr->value);
        result = counter64_incr (result);
        pInfo->cValue = (result);
      }
    }
    else
    {
      pInfo->cValue = lCtr->value;
    }
    pInfo->cValue.high &= cSizeArray[lCtr->size].high;
    pInfo->cValue.low &= cSizeArray[lCtr->size].low;
    pInfo->cSize = lCtr->size;
    pInfo->status = L7_SUCCESS;
    break;

  case ExternalCtr:

    eCtr = (ExternalCounter *) pCtr;
    maxVal = cSizeArray[eCtr->size];
    pInfo->cMode = getMode;
    pInfo->cSize = eCtr->size;
    if(eCtr->source == L7_NULLPTR)
    {
      printf("get_counter_value: External function missing for 0x%08x\n", pInfo->cId);
      eCtr->value = pInfo->cValue;
    }
    else
    {
      eCtr->source (pInfo);
      eCtr->value = pInfo->cValue;
    }
    if (eCtr->isResettable == L7_FALSE && pInfo->raw == L7_FALSE)
    {
      if (counter64_greater_equal (eCtr->value, eCtr->delta))
      {
        pInfo->cValue = counter64_sub (eCtr->value, eCtr->delta);
      }
      /* fill in the relative value */
      else
      {
        result = counter64_add ((counter64_sub (maxVal, eCtr->delta)), eCtr->value);

        /* increment result by 1 to account for the counter wrapping */
        result = counter64_incr (result);
        pInfo->cValue = result;
      }
    }
    pInfo->cValue.high &= cSizeArray[eCtr->size].high;
    pInfo->cValue.low &= cSizeArray[eCtr->size].low;
    pInfo->status = L7_SUCCESS;
    break;

  case MutantCtr:
    {
      counterValue_entry_t CtrVal;
      pCounterValue_entry_t pCtrVal = &CtrVal;

      /* This will be reset to failure if the function
         getMutlings fails */
      pCtrVal->status = L7_SUCCESS;
      mCtr = (MutantCounter *) pCtr;

      /* if not resettable then the value to be returned is the relative
         value i.e. value - delta */
      mCtr->value = mc_getMutlings (mCtr, pCtrVal, pInfo);
      pInfo->cValue = mCtr->value;
      pInfo->cValue.high &= cSizeArray[mCtr->size].high;
      pInfo->cValue.low &= cSizeArray[mCtr->size].low;
      pInfo->cSize = mCtr->size;        /* fill in the size */
      if (pCtrVal->status == L7_SUCCESS)
      {
        pInfo->status = L7_SUCCESS;
      }
      else
      {
        printf ("ERROR!! failure in mc_get .\n");
      }
      break;
    }
  default:
    printf (" Error : Unknown Counter type in get_counter_value \n");
    break;
  }
}

/*
 **********************************************************************
 *
 * @function     create()
 *
 * @purpose      This function allows the client to create a new counter by
 *               specifying the appropriate parameters
 *
 * @param        pStatsParm_entry_t pCtrCreateInfo
 *
 * @returns      None
 *
 * @end
 *
 **********************************************************************
 */
void collector_create (Collector * ptr, pStatsParm_entry_t pCtrCreateInfo)
{
  signedCounter mutlings[STATS_MAX_MUTLING];
  L7_ushort16 flag = 0;

  /* Lookup the flyweight if the Ctr already exists */
  L7_RC_t rc;
  keyPair kp;

  kp.key = pCtrCreateInfo->cKey;
  kp.id = pCtrCreateInfo->cId;
  rc = keyPair_get (&kp, ptr->flyweight);

  /* If counter exists return immediately else create one */
  if (rc == L7_FAILURE)
  {
    /* Before creating any counter set status to L7_FAILURE
       If the create operation is successful status will be
       reset to L7_SUCCESS by the counter */
    pCtrCreateInfo->status = L7_FAILURE;
    switch (pCtrCreateInfo->cType)
    {
    case LocalCtr:

      /* Create a LocalCounter   */
      if (ptr->pLocalCtrFreeList == L7_NULL)
      {
        ptr->pLCtr = (LocalCounter *) osapiMalloc (L7_STATSMGR_COMPONENT_ID, sizeof (LocalCounter));
        memset(  ptr->pLCtr , 0, sizeof(LocalCounter));
      }
      else
      {
        ptr->pLCtr = ptr->pLocalCtrFreeList;
        ptr->pLocalCtrFreeList = ptr->pLCtr->next;
      }

      counter_init (LocalCtr, (void *) (ptr->pLCtr), pCtrCreateInfo, L7_NULL);
#if 0
      lc_init (ptr->pLCtr, pCtrCreateInfo);  /*---- to be modified--*/
#endif
      kp.ptr = ptr->pLCtr;
      kp.ctype = LocalCtr;
      flag = 1;
      break;
    case ExternalCtr:

      /* Create an ExternalCounter */
      if (ptr->pExternalCtrFreeList == L7_NULL)
      {
/* FIXME */
#if 1
        ptr->pECtr = (ExternalCounter *) osapiMalloc (L7_STATSMGR_COMPONENT_ID,
                                                      sizeof (ExternalCounter));
        memset(  ptr->pECtr , 0, sizeof(ExternalCounter));
#endif
#if 0
        ptr->pECtr = (MutantCounter *) malloc (sizeof (MutantCounter));

#endif
/*
       printf(" Malloc an external counter : %x  \n", ptr->pECtr);
 */
#if 0
        ec_init (ptr->pECtr, pCtrCreateInfo);
#endif
      }
      else
      {
        ptr->pECtr = ptr->pExternalCtrFreeList;
        ptr->pExternalCtrFreeList = ptr->pECtr->next;
        /*((ExternalCounter*)pCtr) = ExternalCounter(pCtrCreateInfo); */
        /*    printf(" Reusing  an external counter \n"); */
#if 0
        ec_init (ptr->pECtr, pCtrCreateInfo);
#endif
      }
      counter_init (ExternalCtr, (void *) (ptr->pECtr), pCtrCreateInfo, L7_NULL);
      kp.ptr = ptr->pECtr;
      kp.ctype = ExternalCtr;
      flag = 2;
      break;
    case MutantCtr:

      /* First check if the given list size is less than or equal
         to STATS_MAX_MUTLING */
      if ((pCtrCreateInfo->pMutlingsParmList != L7_NULL) &&
          (STATS_MAX_MUTLING <= pCtrCreateInfo->pMutlingsParmList->listSize))
      {

        return;
      }

      /* Initialize status to success and lookup the flyweight
         for the component counters. If at least one counter
         cannot be recovered return immediately without attempting to
         create a mutant counter */
      pCtrCreateInfo->status = L7_SUCCESS;

      /* Function call to create a list consisting of signed counters
         which represent the component counters of the mutant counter */
      if (pCtrCreateInfo->pMutlingsParmList != L7_NULL)
      {
        collector_createMutlingList (ptr, pCtrCreateInfo, mutlings);
      }

      /* Create a MutantCounter */
      if (pCtrCreateInfo->status == L7_SUCCESS)
      {
        if (ptr->pMutantCtrFreeList == L7_NULL)
        {
          /* Create a MutantCounter */
          ptr->pMCtr = (MutantCounter *) osapiMalloc (L7_STATSMGR_COMPONENT_ID,
                                                      sizeof (MutantCounter));
          if (ptr->pMCtr == L7_NULL)
          {
            return;
          }
          memset(  ptr->pMCtr , 0, sizeof(MutantCounter));
#if 0
          mc_init (ptr->pMCtr, pCtrCreateInfo, mutlings);
#endif

        }
        else
        {
          ptr->pMCtr = ptr->pMutantCtrFreeList;
          ptr->pMutantCtrFreeList = ptr->pMCtr->next;
#if 0
          mc_init (ptr->pMCtr, pCtrCreateInfo, mutlings);
#endif
        }

        counter_init (MutantCtr, (void *) (ptr->pMCtr), pCtrCreateInfo, mutlings);
      }
      kp.ptr = ptr->pMCtr;
      kp.ctype = MutantCtr;
      flag = 3;
      break;
    default:
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_STATSMGR_COMPONENT_ID,
              "Unknown Counter type requested.\n");
      /* If we got here is because invalid counter type and pCtr == NULL,
         the pCtrCreateInfo->status will be set to L7_FAILURE below. */
      pCtrCreateInfo->status = L7_FAILURE;
      break;
    }
    if (pCtrCreateInfo->status == L7_SUCCESS)
    {
      /* Insert the newly created ctr into the flyweight */
      if(L7_SUCCESS != keyPair_set (&kp, ptr->flyweight))
      {
        printf("collector_create: keyPair_set 0x%08x failed\n", kp.id);
      }
      if (flag == 1)
      {
        ptr->pLCtr->next = L7_NULL;
        /* clearing the counter */
        ptr->pLCtr->value.high = 0;
        ptr->pLCtr->value.low = 0;
        ptr->pLCtr->delta.high = 0;
        ptr->pLCtr->delta.low = 0;

      }
      else if (flag == 2)
      {
        ptr->pECtr->next = L7_NULL;
        /* clearing the counter */
        ptr->pECtr->value.high = 0;
        ptr->pECtr->value.low = 0;
        ptr->pECtr->delta.high = 0;
        ptr->pECtr->delta.low = 0;

      }
      else if (flag == 3)
      {
        ptr->pMCtr->next = L7_NULL;
        ptr->pMCtr->value.high = 0;
        ptr->pMCtr->value.low = 0;
        ptr->pMCtr->delta.high = 0;
        ptr->pMCtr->delta.low = 0;
        /* clearing the counter */

      }
    }
  }                             /* endif: pCtr == NULL */

  /* If the counter has been created or already exists in the flyweight
     reset its value to zero */
  if (rc != L7_FAILURE)
  {
    pCtrCreateInfo->status = L7_SUCCESS;
    /*pCtr->clear(); */
  }
#if 0
  else
  {
    printf ("error!! failure in collector_create function.\n");
    pCtrCreateInfo->status = L7_FAILURE;
  }
#endif
}

/*
 **********************************************************************
 *
 * @function     addMutlings()
 *
 * @purpose      This function allows the client to dynamically add mutlings
 *               to a mutantCounter
 *
 * @param        pStatsParm_entry_t pStatsParmEntry
 *
 * @returns      None
 *
 * @end
 *
 **********************************************************************
 */
void collector_addMutlings (Collector * ptr, pStatsParm_entry_t pStatsParmEntry)
{
  L7_RC_t rc;
  signedCounter mutlings[STATS_MAX_MUTLING];
  keyPair kp;

  kp.key = pStatsParmEntry->cKey;
  kp.id = pStatsParmEntry->cId;
  rc = keyPair_get (&kp, ptr->flyweight);

  /* Lookup the flyweight to find the mutantCounter in which to
     add the specified mutlings */

  /* If the Ctr exists call its method to add mutlings. If the
     specified counter is not a mutantCounter this will cause
     the status field to be set to L7_FAILURE */
  if (rc != L7_FAILURE)
  {
    ptr->pMCtr = (MutantCounter *) (kp.ptr);

    /* Initialize status to success and lookup the flyweight
       for the component counters. If at least one counter
       cannot be recovered return immediately without attempting to
       add to the specified mutant counter */

    pStatsParmEntry->status = L7_SUCCESS;

    if (pStatsParmEntry->pMutlingsParmList != L7_NULL)
    {
      collector_createMutlingList (ptr, pStatsParmEntry, mutlings);
    }

    if (pStatsParmEntry->status == L7_SUCCESS)
    {
      /* Will be set to L7_SUCCESS if the mutlings could
         be successfully added to the mutant Set */
      pStatsParmEntry->status = L7_FAILURE;
      if (ptr->pMCtr->cType == MutantCtr)
      {
        mc_addMutlings (ptr->pMCtr, pStatsParmEntry, mutlings);
      }
    }
  }
  /* If the Ctr does not exist set status as L7_FAILURE */
  else
  {
    printf ("ERROR!! failure in collector_addmutlings\n");
    pStatsParmEntry->status = L7_FAILURE;
  }
}

/*
 **********************************************************************
 *
 * @function     delMutlings()
 *
 * @purpose      This function allows the client to dynamically delete mutlings
 *               from a mutantCounter
 *
 * @param        pStatsParm_entry_t pStatsParmEntry
 *
 * @returns      None
 *
 * @end
 *
 **********************************************************************
 */
void collector_delMutlings (Collector * ptr, pStatsParm_entry_t pStatsParmEntry)
{
  signedCounter mutlings[STATS_MAX_MUTLING];
  keyPair kp;
  L7_RC_t rc;

  kp.key = pStatsParmEntry->cKey;
  kp.id = pStatsParmEntry->cId;
  rc = keyPair_get (&kp, ptr->flyweight);

  /* Lookup the flyweight to find the mutantCounter in which to
     delete the specified mutlings
   */

  /* If the Ctr exists call its method to add mutlings. If the
     specified counter is not a mutantCounter this will cause
     the status field to be set to L7_FAILURE
   */
  if (rc != L7_FAILURE)
  {
    /* Initialize status to success and lookup the flyweight
       for the component counters. If at least one counter
       cannot be recovered return immediately without attempting to
       add to the specified mutant counter */
    pStatsParmEntry->status = L7_SUCCESS;

    if (pStatsParmEntry->pMutlingsParmList != L7_NULL)
    {
      collector_createMutlingList (ptr, pStatsParmEntry, mutlings);
    }

    if (pStatsParmEntry->status == L7_SUCCESS)
    {
      /* Will be set to L7_SUCCESS if the mutlings could
         be successfully deleted from the mutant Set */
      pStatsParmEntry->status = L7_FAILURE;
      if (ptr->pMCtr->cType == MutantCtr)
      {
        mc_delMutlings (ptr->pMCtr, pStatsParmEntry, mutlings);
      }
    }

  }
  /* If the Ctr does not exist set status as L7_FAILURE */
  else
  {
    printf ("ERROR!! failure in collector_delMutlings\n");
    pStatsParmEntry->status = L7_FAILURE;
  }
}

/*
 **********************************************************************
 *
 * @function     counterDelete()
 *
 * @purpose      This function allows the client to delete a new counter by
 *               specifying the appropriate parameters
 *
 * @parameter    pStatsParm_entry_t pCtrCreateInfo
 *
 * @return       None
 *
 * @note         Implemented in src\application\stats\private\collector.cpp
 *
 * @end
 *
 **********************************************************************
 */
void c_counterDelete (Collector * ptr, pStatsParm_entry_t pCtrDeleteInfo)
{
  counter_type_t type;
  keyPair kp;
  L7_RC_t rc;

  kp.key = pCtrDeleteInfo->cKey;
  kp.id = pCtrDeleteInfo->cId;

  /* Get the ptr to the counter */
  /* Lookup the flyweight if the Ctr already exists */
  rc = keyPair_get (&kp, ptr->flyweight);

  if (rc != L7_FAILURE)
  {
    /* get the type of counter */
    type = kp.ctype;
    /* Link to the head of the free list for the appropriate counter type */
    switch (type)
    {
    case LocalCtr:
      ptr->pLCtr = (LocalCounter *) kp.ptr;
      ptr->pLCtr->next = ptr->pLocalCtrFreeList;
      ptr->pLocalCtrFreeList = ptr->pLCtr;
      break;

    case ExternalCtr:
      ptr->pECtr = (ExternalCounter *) kp.ptr;
      ptr->pECtr->next = ptr->pExternalCtrFreeList;
      ptr->pExternalCtrFreeList = ptr->pECtr;
      break;

    case MutantCtr:
      ptr->pMCtr = (MutantCounter *) kp.ptr;
      ptr->pMCtr->next = ptr->pMutantCtrFreeList;
      ptr->pMutantCtrFreeList = ptr->pMCtr;
      break;

    default:
      break;
    }
  }

  /* Get the ptr to the counter */
  /* set the key pair to invalid */
  kp.ptr = L7_NULL;
  kp.ctype = 0;
  /*FIXME FIXME FIXME Memory freeing of the keypair?? */
  keyPair_set (&kp, ptr->flyweight);
  pCtrDeleteInfo->status = L7_SUCCESS;
}
