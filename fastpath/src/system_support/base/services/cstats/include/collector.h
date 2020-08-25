/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    collector.h
* @purpose     stats mgr API's and data structures
* @component   stats
* @comments    
* @create      18/06/2007
* @author      YSaritha 
* @end
*             
**********************************************************************/

#ifndef INCLUDE_COLLECTOR_H
#define INCLUDE_COLLECTOR_H

#include "keypair.h"
/*
***********************************************************************
*                           CONSTANTS
***********************************************************************
*/

#define MAX_MUTLING 16

typedef struct Counter_s
{
  L7_uint32 cKey;               /* counter key                */
  counter_id_t cId;             /* counter id */
  L7_ulong64 value;
/* FIXME  */
#if 0
  Counter64 value;              /* This represents the current value of the  */
#endif
  /* counter */
  counter_size_t size;          /* This represents the size of the counter */
  L7_BOOL isResettable;         /* This checks if the counter can be reset */
#if 0
  Counter64 delta;              /* The difference in counter value when */

  /* FIXME */
#endif
  L7_ulong64 delta;             /* The difference in counter value when */
  struct Counter_s *nextPtr;
} Counter;

typedef struct _ExternalCounter_
{
  L7_uint32 cKey;               /* counter key */
  counter_id_t cId;             /* counter id */
  L7_ulong64 value;             /* This represents the current value of the counter */

  counter_size_t size;          /* This represents the size of the counter */
  L7_BOOL isResettable;         /* This checks if the counter can be reset */
  L7_ulong64 delta;             /* The difference in counter value when
                                   the user asks for a reset */

  struct _ExternalCounter_ *next;               /*---needs to b verified. */


  counter_type_t cType;
  L7_VOIDFUNCPTR_t source;      /* This represents a pointer to the source of the 
                                   counter */
} ExternalCounter;


typedef struct _LocalCounter_
{
  /* Counter attributes. */
  L7_uint32 cKey;               /* counter key */
  counter_id_t cId;             /* counter id */
  L7_ulong64 value;             /* This represents the current value of the
                                   counter */
  counter_size_t size;          /* This represents the size of the counter */
  L7_BOOL isResettable;         /* This checks if the counter can be reset */
  L7_ulong64 delta;             /* The difference in counter value when
                                   the user asks for a reset */

  struct _LocalCounter_ *next;
  counter_type_t cType;
} LocalCounter;

typedef struct signedCounter_s
{
  counter_sign_t sign;
  Counter *pCtr;
  counter_type_t  ctype_signed;
} signedCounter;




typedef struct _MutantCounter_
{
  /*  Counter attrinutes.  */
  L7_uint32 cKey;               /*  counter key  */
  counter_id_t cId;             /* counter id    */
  L7_ulong64 value;             /* This represents the current value of the
                                   counter */
  counter_size_t size;          /*  This represents the size of the counter */
  L7_BOOL isResettable;         /* This checks if the counter can be reset */
  L7_ulong64 delta;             /* The difference in counter value when
                                   the user asks for a reset */
  struct _MutantCounter_ *next;

/* MutantCounter attributes. */
  counter_type_t cType;
  signedCounter mSet[STATS_MAX_MUTLING];
} MutantCounter;


typedef struct
{

  ExternalCounter *pECtr;
  LocalCounter *pLCtr;
  MutantCounter *pMCtr;         /* This is a pointer to the current
                                   counter */
  /* This is a pointer to the Collector's counterflyweight */
  keyPair *flyweight[COUNTER_FLY_WEIGHT_MAX];

  /* 
   * Free lists for each of the counter types 
   * Instead of actually freeing the memory when a counter must be deleted
   * the pointer will be added to the appropriate free list
   */
  MutantCounter *pMutantCtrFreeList;
  ExternalCounter *pExternalCtrFreeList;
  LocalCounter *pLocalCtrFreeList;
} Collector;


counter_type_t ec_getType (ExternalCounter * ptr);

void ec_get (ExternalCounter * ptr, pCounterValue_entry_t pInfo);

void ec_set (ExternalCounter * ptr, pCounterValue_entry_t pInfo);
void lc_increment (LocalCounter * ptr, pCounterValue_entry_t pInfo);
void lc_decrement (LocalCounter * ptr, pCounterValue_entry_t pInfo);

void lc_set (LocalCounter *, pCounterValue_entry_t pInfo);

void lc_get (LocalCounter *, pCounterValue_entry_t pInfo);

L7_ulong64 mc_getMutlings (MutantCounter * ptr, pCounterValue_entry_t c, pCounterValue_entry_t p);

void mc_resetMutlings (MutantCounter * ptr, pCounterValue_entry_t c, pCounterValue_entry_t p);
void mc_get (MutantCounter * ptr, pCounterValue_entry_t pInfo);
void mc_set (MutantCounter * ptr, pCounterValue_entry_t pInfo);
void mc_addMutlings (MutantCounter * ptr, pStatsParm_entry_t pM, signedCounter * mutlings);
void mc_delMutlings (MutantCounter * ptr, pStatsParm_entry_t pM, signedCounter * mutlings);
void collector_init (Collector *);
void collector_createMutlingList (Collector * ptr, pStatsParm_entry_t c, signedCounter * m);
void collector_get (Collector * ptr, pCounterValue_entry_t pCtrValInfo);
void collector_reset (Collector * ptr, pCounterValue_entry_t pCtrStatusInfo);
void c_increment (Collector * ptr, pCounterValue_entry_t pCtrIncrInfo);
void c_decrement (Collector * ptr, pCounterValue_entry_t pCtrDecrInfo);
void collector_create (Collector * ptr, pStatsParm_entry_t pCtrCreateInfo);
void collector_addMutlings (Collector * ptr, pStatsParm_entry_t pStatsParmEntry);
void collector_delMutlings (Collector * ptr, pStatsParm_entry_t pStatsParmEntry);
void c_counterDelete (Collector * ptr, pStatsParm_entry_t pCtrCreateInfo);
void get_counter_value (void *pCtr, pCounterValue_entry_t pInfo, counter_type_t ctype);

#endif /* INCLUDE_COLLECTOR_H */
