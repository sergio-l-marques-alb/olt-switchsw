
/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename         OS_XXCMN.H
 *
 * @purpose          Contains some common.definitions for OS porting modules
 *
 * @component        Routing Utils Component
 *
 * @comments
 *
 * @create
 *
 * @author
 *
 * @end
 *
 * ********************************************************************/
#ifndef  os_xxcmn_h
#define  os_xxcmn_h

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "os_xx.ext"

#if L7_CPM_MC360    || (L7_CORE_CPU == MC360)
#include "quicc.ext"
#include "quicc.h"
#endif

#if L7_CORE_CPU == MPC860
#include "pquicc.ext"
#include "pqregs.h"
#if L7_CPM_MPC860_FAMILY
#include "pquicc.h"
#else
#include "pqmmap.h"
#endif
#endif

#if L7_CPM_MPC8260  || (L7_CORE_CPU == MPC8260)
#include "pqii.ext"
#endif

#define  OS_MSG_SIZE           sizeof(void *)

/* It is important that the message queue never overflow. If it does,
 * an application could lose important state information. For example,
 * if a timer callback is lost, that timer may never get rescheduled. */
#define  OS_MAX_MSG_IN_QUEUE   3000

#if   L7_CORE_CPU == MC360
extern word _Saved_SR;
extern word _Temp_SR;
#endif

#if L7_CORE_CPU == IDT4650
extern ulng _Saved_SR;
extern ulng _Temp_SR;
#endif

extern int _Nesting_counter;       /* XX_Freeze, XX_Thaw calls nesting counter */
extern void  *_No_Cache_Memory;

#if (L7_CORE_CPU == MPC8260) || (L7_CORE_CPU == MPC860) || (L7_CORE_CPU == PPC_CORE)

extern ulng _Saved_EE;
/*
 * Set the MSR bit function
 */
void SetBits_MSR( ulng msr );
/*
 * Clear the MSR bit function
 */
ulng ClearBits_MSR( ulng msr );
#endif /* (L7_CORE_CPU == MPC8260) || (L7_CORE_CPU == MPC860) || (L7_CORE_CPU == PPC_CORE) */


/*===========  Memory info keeping structure =============*/
typedef struct tagt_OS_PoolInfo
{
   struct tagt_OS_PoolInfo *next;  /* chaining element                                               */
   struct tagt_OS_PoolInfo *prev;
   byte  *poolStrtAddr;            /* starting address of the memory in the pool                     */
   byte  *poolFreeAdd;             /* starting address of the free memory part in the pool           */
   ulng   size;                    /* size of the pool                                               */
   ulng   sizeFree;                /* size of available (free) memory                                */
   ulng   static_pool;             /* this flag says if the pool is a static, i.e. is located in the */
                                   /* area defined by user or a dynamic (allocated from a heap) one  */
                                   /* for static pool the real start address is stored here          */
} t_OS_PoolInfo;


#define L7_ROUTING_MAX_QUEUES 16

/*========= Thread data type =============*/
typedef struct tagt_OS_Thread
{
   ulng  QueueID[L7_ROUTING_MAX_QUEUES];
   ulng  QueueWeights[L7_ROUTING_MAX_QUEUES];
   ulng  TaskID;
   ulng  numQueues;
   void  *MsgQSema;

} OS_Thread;


/*========= Max number of interrupt wrappers =============*/
/*========= which can be used simultaneously =============*/
/* os_xxcmn.c provides support for up to 32 wrappers      */
/* If more than 32 wrappers is required, additional       */
/* wrapper functions must be added in os_xxcmn.c          */
#define OS_CMN_MAX_INT_WRAPPERS        32

#if OS_CMN_MAX_INT_WRAPPERS
/*========= Interrupt wrapper type =============*/
typedef void (*OS_CMN_IntWrapper)(void);
#endif

/*======== Disable the interrupts common use macro ===========*/

#if L7_CORE_CPU == MC360
#define OS_CMN_Freeze                        \
   {                                         \
      __asm("   move.w   sr, -(SP)");        \
      __asm("   ori      #$0700, sr");       \
      __asm("   move.w   (SP)+, __Temp_SR"); \
      if(_Nesting_counter == 0)              \
         _Saved_SR = _Temp_SR & 0x700;       \
      _Nesting_counter++;                    \
   }

#elif (L7_CORE_CPU == MPC860) || (L7_CORE_CPU == MPC8260) || (L7_CORE_CPU == PPC_CORE)
#define OS_CMN_Freeze                     \
   {                                      \
      ulng _Temp_MSR;                     \
                                          \
      _Temp_MSR = ClearBits_MSR( 0x00008000L );\
      if(_Nesting_counter == 0)           \
         _Saved_EE = _Temp_MSR;           \
      _Nesting_counter++;                 \
   }


#elif (L7_CORE_CPU == IDT4650) && (defined(DIAB) || defined(__DIAB))
   /* IDT MIPS CPU. Implementation for DiabData comp */
#define OS_CMN_Freeze                           \
   {                                            \
   __asm("  mfc0  $8,$12");                     \
   __asm("  li    $9,0xffff00ff");              \
   __asm("  and   $9,$8,$9");                   \
   __asm("  mtc0  $9,$12");                     \
   __asm("  nop");                              \
   __asm("  nop");                              \
   __asm("  nop");                              \
   __asm("  sw    $8,%sdaoff(_Temp_SR)($28)");  \
   if(_Nesting_counter == 0)                    \
      _Saved_SR = _Temp_SR;                     \
   _Nesting_counter++;                          \
   }

#elif L7_CORE_CPU == TRICORE
#define OS_CMN_Freeze                           \
   {                                            \
      if ( _Nesting_counter == 0 )              \
      {                                         \
         _disable();                            \
      }                                         \
      _Nesting_counter++;                       \
   }
#endif /* L7_CORE_CPU */


/*======== Enable the interrupts common use macro ===========*/

#if L7_CORE_CPU == MC360
#define OS_CMN_Thaw                       \
   {                                      \
      if(_Nesting_counter <= 1)           \
      {                                   \
         _Nesting_counter = 0;            \
         _Temp_SR = 0x2000 | _Saved_SR;   \
         __asm("  move.w __Temp_SR,sr");  \
      }                                   \
      else                                \
         _Nesting_counter--;              \
   }

#elif (L7_CORE_CPU == MPC860) || (L7_CORE_CPU == MPC8260) || (L7_CORE_CPU == PPC_CORE)
#define OS_CMN_Thaw                       \
   {                                      \
      if(_Nesting_counter <= 1)           \
      {                                   \
         _Nesting_counter = 0;            \
         if(_Saved_EE == 0x00008000L)     \
            SetBits_MSR( 0x00008000L );   \
      }                                   \
      else                                \
         _Nesting_counter--;              \
   }

#elif (L7_CORE_CPU == IDT4650) && (defined(DIAB) || defined(__DIAB))
/* IDT MIPS CPU. Implementation for DiabData comp */
#define OS_CMN_Thaw                       \
   {                                      \
      if(_Nesting_counter <= 1)           \
      {                                   \
         _Nesting_counter = 0;            \
         __asm("  lw    $8,%sdaoff(_Saved_SR)($28)");  \
         __asm("  mtc0 $8,$12");          \
         __asm("  nop");                  \
         __asm("  nop");                  \
         __asm("  nop");                  \
      }                                   \
      else                                \
         _Nesting_counter--;              \
   }

#elif L7_CORE_CPU == TRICORE
#define OS_CMN_Thaw                     \
   {                                    \
      if ( _Nesting_counter <= 1 )      \
      {                                 \
         _enable();                     \
         _Nesting_counter = 0;          \
      }                                 \
      else                              \
         _Nesting_counter--;            \
   }
#endif /* L7_CORE_CPU */

/* Some common function prototypes */

e_Err OS_CMN_InitAll( void );
void  OS_CMN_Start( void );
void  OS_CMN_Terminate (const char *reason);
void  OS_CMN_MEM_InitAll(void);
e_Err OS_CMN_MEM_Init(void *begaddr, ulng memsize, t_Handle *p_poolHandle);
e_Err OS_CMN_MEM_InitAlign(void *begaddr, ulng memsize, word align, t_Handle *p_poolHandle);
ulng  OS_CMN_MEM_SizeofInitAlign(Bool staticPart, ulng memsize, word align);
void *OS_CMN_GetMem(t_Handle poolHandle, ulng size);
ulng  OS_CMN_GetFreeMem(t_Handle poolHandle);
e_Err OS_CMN_MEM_Delete(t_Handle poolHandle);
byte *OS_CMN_MallocNoCache( ulng Size );
byte *OS_CMN_MallocNoCacheAlign( ulng Size, word Align );
void  OS_CMN_FreeNoCache( void *buf );
#if OS_CMN_MAX_INT_WRAPPERS
OS_CMN_IntWrapper OS_CMN_AllocWrapper( FUNC_Interrupt f_intFun, word vector, t_HandleAsLong parm );
e_Err OS_CMN_FreeWrapper( word vector );
#endif

#ifdef __cplusplus
}
#endif

#endif  /* os_xxcmn_h */
