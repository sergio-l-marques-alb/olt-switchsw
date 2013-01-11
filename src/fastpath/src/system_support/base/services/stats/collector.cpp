/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename collector.cpp
*
* @purpose The purpose of this file is to implement the functionality
*          of the Collector Class.
*
* @component Statistics Manager
*
* @comments none
*
* @create 10/08/2000
*
* @author Archana Suthan
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/



#include <string.h>
#include <statsinclude.h>


static L7_ulong64 cSizeArray[3] =
{{0x00000000, 0x0000FFFF}, {0x00000000, 0xFFFFFFFF}, {0xFFFFFFFF, 0xFFFFFFFF}};

/*
***********************************************************************
*                           COUNTERS
***********************************************************************
*/


/*
**********************************************************************
*
* @function  Counter()
*
* @purpose   This is the constructor for the Counter Class
*
* @param     L7_ulong64 v
* @param     L7_ulong64 s
*
* @returns   None
*
* @end
**********************************************************************
*/

Counter :: Counter(pStatsParm_entry_t c)
{
  cKey = c->cKey;       // counter key
  cId = c->cId;      // counter id
  value = 0;                      // initial value
  size  = c->cSize;             // set size to s
  isResettable = c->isResettable; // set the value of isResettable to r
  delta = 0;                  // initialize delta to 0
  nextPtr = NULL;
  return;
}

Counter :: ~Counter()
{
}

/*
**********************************************************************
*
* @function  getId()
*
* @purpose   This function returns the counter Id
*
* @params    None
*
* @returns   counter id
*
* @notes     Implemented in src\application\stats\private\collector.cpp
*
* @end
*
**********************************************************************
*/
counter_id_t Counter::getId()
{
  return cId;
}

/*
**********************************************************************
*
* @function  getKey()
*
* @purpose   This function returns the counter key
*
* @params    None
*
* @returns   None
*
* @notes     Implemented in src\application\stats\private\collector.cpp
*
* @end
*
**********************************************************************
*/
L7_uint32 Counter::getKey()
{
  return cKey;
}

/*
**********************************************************************
*
* @function  clear()
*
* @purpose   This function clears the counter i.e. sets its initial value
*            to zero when created.
*
* @param     None
*
* @returns   None
*
* @notes     Implemented in src\application\stats\private\collector.cpp
*
* @end
*
**********************************************************************
*/

void Counter :: clear()
{
  delta = 0;
  value = 0;
}
/*
**********************************************************************
*
* @function     get()
*
* @purpose      This function allows the client to get the value associated
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

void Counter :: get(pCounterValue_entry_t pInfo)
{
  Counter64 maxVal(cSizeArray[size].high, cSizeArray[size].low);
  // if not resettable then the value to be returned is the relative
  // value i.e. value - delta
  if(isResettable == L7_FALSE && pInfo->raw == L7_FALSE)
  {
    if (value >= delta)
      pInfo->cValue = value - delta;    // fill in the relative value
    else
    {
      Counter64 result(Counter64(maxVal - delta) + value);

      // increment result by 1 to account for the counter wrapping
      ++result;
      pInfo->cValue = (L7_ulong64)(result);
    }
  }
  else
    pInfo->cValue   = value;     // if resettable return value
  pInfo->cValue.high &= cSizeArray[size].high;
  pInfo->cValue.low  &= cSizeArray[size].low;
  pInfo->cSize   = size;           // fill in the size
  pInfo->status  = L7_SUCCESS;       // If we reached till here the
  // operation was successful.
  return;
}

/*
**********************************************************************
*
* @function set()
*
* @purpose  This function allows the client to reset the value associated
*           with a counter
*
* @params   pCounterValue_entry_t pInfo
*
* @returns
*           None
*
* @notes    Implemented in src\application\stats\private\collector.cpp
*
* @end
*
**********************************************************************
*/
void Counter :: set(pCounterValue_entry_t pInfo)
{
  if(isResettable == L7_FALSE)   // if the counter cannot be reset
    delta = value;        // copy its current value to delta
  else
    value = pInfo->cValue;  // reset the counter
  pInfo->cSize  = size;       // fill in the size
  pInfo->status = L7_SUCCESS; // If we reached till here the
  // operation was successful.
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
* @param        pCounterValue_entry_t pInfo
*
* @returns      None
*
* @end
*
**********************************************************************
*/

void Counter :: increment(pCounterValue_entry_t pInfo)
{
  // This function is not supported by External & Mutant
  // counters
  pInfo->status = L7_FAILURE;
  return;
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
void Counter :: decrement(pCounterValue_entry_t pInfo)
{
  // This function is not supported by External & Mutant
  // counters
  pInfo->status = L7_FAILURE;
  return;
}

/*
**********************************************************************
*
* @function     ExternalCounter()
*
* @purpose      This is the constructor for the ExternalCounter Class
*
* @param        pStatsParm_entry_t e
*
* @returns      None
*
* @end
*
**********************************************************************
*/

ExternalCounter :: ExternalCounter(pStatsParm_entry_t e)
:Counter(e)
{
  cType = ExternalCtr;
  if(e->pSource != NULL)
  {
    source = e->pSource;    // set the source
    e->status = L7_SUCCESS;
  }
  else
  {
    source = NULL;
    e->status = L7_FAILURE;
  }
  return;
}

ExternalCounter :: ~ExternalCounter()
{
}

/*
**********************************************************************
*
* @function  getType()
*
* @purpose   This function returns the counter type
*
* @param     None
*
* @returns   None
*
* @notes     Implemented in src\application\stats\private\collector.cpp
*
* @end
*
**********************************************************************
*/
counter_type_t ExternalCounter::getType() const
{
  return cType;
}

/*
**********************************************************************
*
* @function  get()
*
* @purpose   This function allows the client to get the value associated
*            with a counter
*
* @param     pCounterValue_entry_t pInfo
*
* @returns   None
*
* @end
*
**********************************************************************
*/

void ExternalCounter :: get(pCounterValue_entry_t pInfo)
{
  Counter64 maxVal(cSizeArray[size].high, cSizeArray[size].low);

  pInfo->cMode = getMode;
  pInfo->cSize = size;
  source(pInfo);
  value = pInfo->cValue;
  if(isResettable == L7_FALSE && pInfo->raw == L7_FALSE)
  {
    if (value >= delta)
      pInfo->cValue = (L7_ulong64)(value - delta);    // fill in the relative value
    else
    {
      Counter64 result(Counter64(maxVal - delta) + value);

      // increment result by 1 to account for the counter wrapping
      ++result;
      pInfo->cValue = (L7_ulong64)(result);
    }
  }
  pInfo->cValue.high &= cSizeArray[size].high;
  pInfo->cValue.low  &= cSizeArray[size].low;
  return;
}

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

void ExternalCounter :: set(pCounterValue_entry_t pInfo)
{
  // If the counter is resettable call the source to reset its value
  if(isResettable == L7_TRUE)
  {
    pInfo->cMode = setMode;
    source(pInfo);
  }
  else
  {
    pInfo->cMode = getMode;
    source(pInfo);
    delta        = pInfo->cValue;       // store the current value to
    // reset the counter from the user's
    // perspective
    pInfo->cValue.high = 0;
    pInfo->cValue.low  = 0;
    pInfo->cSize  = size;       // fill in the size
    pInfo->status = L7_SUCCESS; // If we reached till here the
    // operation was successful.
  }
  return;
}


/*
**********************************************************************
*
* @function   LocalCounter()
*
* @purpose    This is the constructor for the LocalCounter Class
*
* @param      pStatsParm_entry_t l
*
* @returns    None
*
* @end
*
**********************************************************************
*/

LocalCounter :: LocalCounter(pStatsParm_entry_t l)
:Counter(l)
{
  cType = LocalCtr;
  l->status = L7_SUCCESS;
}

LocalCounter :: ~LocalCounter()
{
}

/*
**********************************************************************
*
* @function   getType()
*
* @purpose    This function returns the counter type
*
* @param      None
*
* @returns    None
*
* @notes      Implemented in src\application\stats\private\collector.cpp
*
* @end
*
**********************************************************************
*/
counter_type_t LocalCounter::getType() const
{
  return cType;
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

void LocalCounter :: increment(pCounterValue_entry_t pInfo)
{
  ++value;                   // increment value
  pInfo->cSize  = size;          // fill in the size
  pInfo->status = L7_SUCCESS;        // If we reached till here the
  // operation was successful
  return;
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
void LocalCounter :: decrement(pCounterValue_entry_t pInfo)
{

  if(value != 0)
    --value;                     // decrement the counter
  pInfo->cSize  = size;    // fill in the size
  pInfo->status = L7_SUCCESS;  // If we reached till here the
  // the operation was successful
  return;
}


/*
**********************************************************************
*
* @function     MutantCounter()
*
* @purpose      This is the constructor for the MutantCounter Class
*
* @param        pStatsParm_entry_t m
*
* @returns      None
*
* @notes  The MutantCounter by default is resettable.
*
* @end
*
**********************************************************************
*/

MutantCounter :: MutantCounter(pStatsParm_entry_t m,
                               signedCounter *mutlings)
:Counter(m)
{
  L7_ushort16 listSize = STATS_MAX_MUTLING;

  cType = MutantCtr;
  memset((L7_char8 *)mSet, 0, STATS_MAX_MUTLING*sizeof(signedCounter));

  if(m == L7_NULL)
    return;

  if(m->pMutlingsParmList != L7_NULL)
  {
    if(m->pMutlingsParmList->listSize < STATS_MAX_MUTLING)
      listSize = m->pMutlingsParmList->listSize;

    // Repeatedly insert mutlings in the set verifying if the operation
    // was successful at each iteration
    for(L7_ushort16 i=0; i < listSize; i++)
    {
      memcpy((L7_char8 *)&mSet[i], (L7_char8 *)&mutlings[i], sizeof(signedCounter));
    }
  }

  m->status = L7_SUCCESS;

  return;
}

MutantCounter :: ~MutantCounter()
{
}

/*
**********************************************************************
*
* @function     getType()
*
* @purpose      This function returns the counter type
*
* @param        None
*
* @returns      None
*
* @notes        Implemented in src\application\stats\private\collector.cpp
*
* @end
*
**********************************************************************
*/
counter_type_t MutantCounter::getType() const
{
  return cType;
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

L7_ulong64 MutantCounter :: getMutlings(pCounterValue_entry_t c,
                                        pCounterValue_entry_t p)
{
  Counter64 addval(0,0), subval(0,0), val(0,0);
  Counter *x;

  // Iterate through the set of mutlings to get the
  // individual values of the mutlings and add it
  // up with the corresponding sign to arrive at
  // the value of the mutantCounter
  for(L7_ushort16 i = 0; (i < STATS_MAX_MUTLING) && (c->status != L7_FAILURE); i++)
  {
    // Check if this is a valid counter pointer
    if(mSet[i].pCtr == L7_NULLPTR)
      continue;

    x = mSet[i].pCtr;
    c->status = L7_FAILURE;
    c->cId    = x->getId();
    c->cKey   = x->getKey();
    c->raw    = p->raw;
    c->cMode  = getMode;

    x->get(c);

    if(mSet[i].sign == cPLUS)
      addval += (c->cValue);
    else if (mSet[i].sign == cMINUS) /* cMINUS */
      subval += c->cValue;

    // If we could not get the value of even one counter
    // the operation was a failure so set pInfo->status
    // to L7_FAILURE
    if(c->status == L7_FAILURE)
      p->status = L7_FAILURE;
  }

  if(addval > subval)
    val = addval - subval;

  return L7_ulong64(val);
}

/*
**********************************************************************
*
* @function     get()
*
* @purpose      This function allows the client to get the value associated
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

void MutantCounter :: get(pCounterValue_entry_t pInfo)
{
  counterValue_entry_t CtrVal;
  pCounterValue_entry_t pCtrVal = &CtrVal;

  // This will be reset to failure if the function
  // getMutlings fails
  pCtrVal->status = L7_SUCCESS;

  // if not resettable then the value to be returned is the relative
  // value i.e. value - delta
  value = getMutlings(pCtrVal, pInfo);
  pInfo->cValue = (L7_ulong64)value;
  pInfo->cValue.high &= cSizeArray[size].high;
  pInfo->cValue.low  &= cSizeArray[size].low;
  pInfo->cSize  = size;    // fill in the size
  if(pCtrVal->status == L7_SUCCESS)
    pInfo->status = L7_SUCCESS;

  return;
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
void MutantCounter :: resetMutlings(pCounterValue_entry_t c,
                                    pCounterValue_entry_t p)
{
  // Iterate through the set of mutlings to set the
  // individual values of the mutlings
  for(L7_ushort16 i=0; (i < STATS_MAX_MUTLING) && (c->status != L7_FAILURE); i++)
  {
    // Check if this is a valid counter pointer
    if(mSet[i].pCtr == L7_NULLPTR)
      continue;

    c->cId    = mSet[i].pCtr->getId();
    c->cKey   = mSet[i].pCtr->getKey();
    c->status = L7_FAILURE;
    c->cValue = p->cValue;
    c->raw    = p->raw;
    c->cMode  = setMode;

    mSet[i].pCtr->set(c);

    // If we could not reset the value of even one counter
    // the operation was a failure so set pInfo->status
    // to L7_FAILURE
    if(c->status == L7_FAILURE)
      p->status = L7_FAILURE;
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
void MutantCounter :: set(pCounterValue_entry_t pInfo)
{
  counterValue_entry_t CtrStatus;
  pCounterValue_entry_t pCtrStatus = &CtrStatus;

  // Set status to success, if the operation
  // fails status will be set to failure by the
  // function
  pCtrStatus->status = L7_SUCCESS;
  if(isResettable == L7_FALSE)
    delta = getMutlings(pCtrStatus, pInfo);
  else
    resetMutlings(pCtrStatus, pInfo);

  if(pCtrStatus->status == L7_SUCCESS)
    pInfo->status = L7_SUCCESS;

  value   = pInfo->cValue;
  pInfo->cSize   = size;  // fill in the size

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

void MutantCounter :: addMutlings(pStatsParm_entry_t pM,
                                  signedCounter *mutlings)
{
  L7_ushort16 listSize = STATS_MAX_MUTLING;

  if(pM->pMutlingsParmList->listSize < STATS_MAX_MUTLING)
    listSize = pM->pMutlingsParmList->listSize;

  // Repeatedly insert mutlings in the set verifying if the operation
  // was successful at each iteration
  if(mutlings != L7_NULL)
  {
    for(L7_ushort16 i=0; i < listSize; i++)
    {
      for(L7_ushort16 j=0; j < STATS_MAX_MUTLING; j++)
      {
        // Find a vacancy to add the new mutling
        if(mSet[j].pCtr == L7_NULLPTR)
        {
          memcpy((L7_char8 *)&mSet[j], (L7_char8 *)&mutlings[i], sizeof(signedCounter));
          break;
        }
      }
    }

    // Set status to success after inserting mutlings into set
    pM->status = L7_SUCCESS;
  }
  else
    pM->status = L7_FAILURE;
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

void MutantCounter :: delMutlings(pStatsParm_entry_t pM,
                                  signedCounter *mutlings)
{
  // Iterate through the set of mutlings to set the
  // individual values of the mutlings
  if(pM->pMutlingsParmList != L7_NULL)
  {
    for(L7_short16 i=0; i < pM->pMutlingsParmList->listSize; i++)
    {
      // If the counter to delete is a null ptr go to the next entry
      if(mutlings[i].pCtr == L7_NULLPTR)
        continue;

      for(L7_ushort16 j=0; j < STATS_MAX_MUTLING; j++)
      {
        // Sanity check this is not a vacant entry in the set
        if(mSet[j].pCtr == L7_NULLPTR)
          continue;

        // If the key and id match delete this counter from the set
        if((mSet[j].pCtr->getKey() == mutlings[i].pCtr->getKey()) &&
           (mSet[j].pCtr->getId() == mutlings[i].pCtr->getId()))
        {
          mSet[j].sign = cINVALID;
          mSet[j].pCtr = L7_NULL;
          break;
        }
      }
    }

    pM->status = L7_SUCCESS;
  }
  else
    pM->status = L7_FAILURE;
  return;
}



/*
***********************************************************************
*                           COLLECTOR
***********************************************************************
*/

Collector :: ~Collector()
{
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

void Collector :: get (pCounterValue_entry_t pCtrValInfo)
{
  // Lookup the flyweight if the Ctr already exists
  pCtr = flyweight[keyPair(pCtrValInfo->cKey,
                           pCtrValInfo->cId)];

  // If the Ctr exists call its method get to determine its value
  if(pCtr != NULL)
  {
    // set status to failure before calling the get method
    // this will be reset to success if the counter get
    // was successful
    pCtrValInfo->status = L7_FAILURE;
    pCtr->get(pCtrValInfo);
  }
  // If the Ctr does not exist set status as L7_FAILURE
  else
    pCtrValInfo->status = L7_FAILURE;
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
void Collector :: reset(pCounterValue_entry_t pCtrStatusInfo)
{
  // Lookup the flyweight if the Ctr already exists
  pCtr = flyweight[keyPair(pCtrStatusInfo->cKey,
                           pCtrStatusInfo->cId)];

  // If the Ctr exists call its method set to reset its value
  if(pCtr != NULL)
  {
    // set status to failure before calling the set method
    // this will be reset to success if the counter set
    // was successful
    pCtrStatusInfo->status = L7_FAILURE;
    pCtrStatusInfo->cValue.high = 0;
    pCtrStatusInfo->cValue.low  = 0;
    pCtr->set(pCtrStatusInfo);
  }
  // If the Ctr does not exist set status as L7_FAILURE
  else
    pCtrStatusInfo->status = L7_FAILURE;
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

void Collector :: increment(pCounterValue_entry_t pCtrIncrInfo)
{
  // Lookup the flyweight if the Ctr already exists
  pCtr = flyweight[keyPair(pCtrIncrInfo->cKey,
                           pCtrIncrInfo->cId)];

  // If the Ctr exists call its method increment
  if(pCtr != NULL)
  {
    // set status to failure before calling the increment method
    // this will be reset to success if the counter increment
    // was successful
    pCtrIncrInfo->status = L7_FAILURE;
    pCtr->increment(pCtrIncrInfo);
  }
  // If the Ctr does not exist set status as L7_FAILURE
  else
    pCtrIncrInfo->status = L7_FAILURE;
  return;
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

void Collector :: decrement(pCounterValue_entry_t  pCtrDecrInfo)
{
  // Lookup the flyweight if the Ctr already exists
  pCtr = flyweight[keyPair(pCtrDecrInfo->cKey,
                           pCtrDecrInfo->cId)];

  // If the Ctr exists call its method decrement
  if(pCtr != NULL)
  {
    // set status to failure before calling the decrement method
    // this will be reset to success if the counter decrement
    // was successful
    pCtrDecrInfo->status = L7_FAILURE;
    pCtr->decrement(pCtrDecrInfo);
  }
  // If the Ctr does not exist set status as L7_FAILURE
  else
    pCtrDecrInfo->status = L7_FAILURE;
  return;
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

void Collector :: createMutlingList (pStatsParm_entry_t c, signedCounter *m)
{
  Counter *pTempCtr;
  pMutling_id_t curr;
  curr = c->pMutlingsParmList->pMutlingParms;

  for(L7_short16 i = 0; (c->status != L7_FAILURE) &&
     (i < c->pMutlingsParmList->listSize); i++)
  {
    // lookup the keypair in the flyweight to get the corresponding
    // counter pointer
    pTempCtr = flyweight[keyPair(curr->cKey, curr->cId)];

    if(pTempCtr != NULL)
    { // we have found the mutling add it to the mutling array
      m[i].sign = curr->cSign;
      m[i].pCtr = pTempCtr;
    }
    else c->status = L7_FAILURE; // if the mutling could
    // not be recovered
    curr++;
  }
  return;
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
void Collector :: create(pStatsParm_entry_t pCtrCreateInfo)
{
  signedCounter mutlings[STATS_MAX_MUTLING];

  // Lookup the flyweight if the Ctr already exists
  pCtr = flyweight[keyPair(pCtrCreateInfo->cKey,
                           pCtrCreateInfo->cId)];

  // If counter exists return immediately else create one
  if(pCtr == NULL)
  {
    // Before creating any counter set status to L7_FAILURE
    // If the create operation is successful status will be
    // reset to L7_SUCCESS by the counter
    pCtrCreateInfo->status = L7_FAILURE;
    switch(pCtrCreateInfo->cType)
    {
    case LocalCtr:
      // Create a LocalCounter
      if (pLocalCtrFreeList == NULL)
      {
        pCtr = new LocalCounter(pCtrCreateInfo);
      }
      else
      {
        pCtr = pLocalCtrFreeList;
        pLocalCtrFreeList = pCtr->nextPtr;
        *((LocalCounter*)pCtr) = LocalCounter(pCtrCreateInfo);
      }
      break;
    case ExternalCtr:
      // Create an ExternalCounter
      if (pExternalCtrFreeList == NULL)
      {
        pCtr = new ExternalCounter(pCtrCreateInfo);
      }
      else
      {
        pCtr = pExternalCtrFreeList;
        pExternalCtrFreeList = pCtr->nextPtr;
        *((ExternalCounter*)pCtr) = ExternalCounter(pCtrCreateInfo);
      }
      break;
    case MutantCtr:
      // First check if the given list size is less than or equal
      // to STATS_MAX_MUTLING
      if((pCtrCreateInfo->pMutlingsParmList != L7_NULL) &&
         (STATS_MAX_MUTLING <= pCtrCreateInfo->pMutlingsParmList->listSize))
        return;

      // Initialize status to success and lookup the flyweight
      // for the component counters. If at least one counter
      // cannot be recovered return immediately without attempting to
      // create a mutant counter
      pCtrCreateInfo->status = L7_SUCCESS;

      // Function call to create a list consisting of signed counters
      // which represent the component counters of the mutant counter
      if(pCtrCreateInfo->pMutlingsParmList != L7_NULL)
        createMutlingList(pCtrCreateInfo, mutlings);

      // Create a MutantCounter
      if(pCtrCreateInfo->status == L7_SUCCESS)
      {
        if (pMutantCtrFreeList == NULL)
        {
          // Create a MutantCounter
          pCtr = new MutantCounter(pCtrCreateInfo, mutlings);
        }
        else
        {
          pCtr = pMutantCtrFreeList;
          pMutantCtrFreeList = pCtr->nextPtr;
          *((MutantCounter*)pCtr) = MutantCounter(pCtrCreateInfo,mutlings);
        }
      }
      break;
    default:
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_STATSMGR_COMPONENT_ID,
              "Unknown Counter type requested.\n");
      // If we got here is because invalid counter type and pCtr == NULL,
      // the pCtrCreateInfo->status will be set to L7_FAILURE below.
      pCtrCreateInfo->status = L7_FAILURE;
      break;
    }

    if(pCtr != NULL  && pCtrCreateInfo->status == L7_SUCCESS)
    {
      // Insert the newly created ctr into the flyweight
      flyweight[keyPair(pCtrCreateInfo->cKey,
                        pCtrCreateInfo->cId)] = pCtr;

      pCtr->nextPtr = NULL;
    }
  }   // endif: pCtr == NULL

  // If the counter has been created or already exists in the flyweight
  // reset its value to zero
  if(pCtr != NULL)
  {
    pCtrCreateInfo->status = L7_SUCCESS;
    pCtr->clear();
  }
  else
    pCtrCreateInfo->status = L7_FAILURE;

  return;
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
void Collector :: addMutlings(pStatsParm_entry_t pStatsParmEntry)
{
  signedCounter mutlings[STATS_MAX_MUTLING];

  // Lookup the flyweight to find the mutantCounter in which to
  // add the specified mutlings
  pCtr = flyweight[keyPair(pStatsParmEntry->cKey,
                           pStatsParmEntry->cId)];

  // If the Ctr exists call its method to add mutlings. If the
  // specified counter is not a mutantCounter this will cause
  // the status field to be set to L7_FAILURE
  if(pCtr != NULL)
  {
    // Initialize status to success and lookup the flyweight
    // for the component counters. If at least one counter
    // cannot be recovered return immediately without attempting to
    // add to the specified mutant counter
    pStatsParmEntry->status = L7_SUCCESS;

    if(pStatsParmEntry->pMutlingsParmList != L7_NULL)
    {
      createMutlingList(pStatsParmEntry, mutlings);

      if(pStatsParmEntry->status == L7_SUCCESS)
      {
        // Will be set to L7_SUCCESS if the mutlings could
        // be successfully added to the mutant Set
        pStatsParmEntry->status = L7_FAILURE;
        if(pCtr->getType() == MutantCtr)
          ((MutantCounter *)(pCtr))->addMutlings(pStatsParmEntry, mutlings);
      }
    }
    else
    {
  // If the Ctr does not exist set status as L7_FAILURE
  pStatsParmEntry->status = L7_FAILURE;
    }
  }

  return;

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
void Collector :: delMutlings(pStatsParm_entry_t pStatsParmEntry)
{
  signedCounter mutlings[STATS_MAX_MUTLING];

  // Lookup the flyweight to find the mutantCounter in which to
  // delete the specified mutlings
  pCtr = flyweight[keyPair(pStatsParmEntry->cKey,
                           pStatsParmEntry->cId)];

  // If the Ctr exists call its method to add mutlings. If the
  // specified counter is not a mutantCounter this will cause
  // the status field to be set to L7_FAILURE
  if(pCtr != NULL)
  {
    // Initialize status to success and lookup the flyweight
    // for the component counters. If at least one counter
    // cannot be recovered return immediately without attempting to
    // add to the specified mutant counter
    pStatsParmEntry->status = L7_SUCCESS;

    if(pStatsParmEntry->pMutlingsParmList != L7_NULL)
      createMutlingList(pStatsParmEntry, mutlings);

    if(pStatsParmEntry->status == L7_SUCCESS)
    {
      // Will be set to L7_SUCCESS if the mutlings could
      // be successfully deleted from the mutant Set
      pStatsParmEntry->status = L7_FAILURE;
      if(pCtr->getType() == MutantCtr)
        ((MutantCounter *)(pCtr))->delMutlings(pStatsParmEntry, mutlings);
    }

  }
  // If the Ctr does not exist set status as L7_FAILURE
  else
    pStatsParmEntry->status = L7_FAILURE;
  return;

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
void Collector::counterDelete(pStatsParm_entry_t pCtrDeleteInfo)
{
  counter_type_t type;

  /* Get the ptr to the counter */
  // Lookup the flyweight if the Ctr already exists
  pCtr = flyweight[keyPair(pCtrDeleteInfo->cKey,
                           pCtrDeleteInfo->cId)];

  // If counter exists return immediately else create one
  if(pCtr != NULL)
  {
    /* get the type of counter */
    type=pCtr->getType();

    /* Link to the head of the free list for the appropriate counter type */
    switch(type)
    {
      case LocalCtr:
        pCtr->nextPtr = pLocalCtrFreeList;
        pLocalCtrFreeList = pCtr;
        break;

      case ExternalCtr:
        pCtr->nextPtr = pExternalCtrFreeList;
        pExternalCtrFreeList = pCtr;
        break;

      case MutantCtr:
        pCtr->nextPtr = pMutantCtrFreeList;
        pMutantCtrFreeList= pCtr;
        break;

      default:
        break;
    }
  }

  /* Get the ptr to the counter */
  // set the key pair to invalid
  flyweight[keyPair(pCtrDeleteInfo->cKey,
                    pCtrDeleteInfo->cId)] = NULL;

  pCtrDeleteInfo->status = L7_SUCCESS;

}


