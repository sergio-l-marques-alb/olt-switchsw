/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename          os_xxcmn.c
 *
 * @purpose           non-portable system services common implementations
 *                    used by a number of OS specific portings
 *
 * @component         Routing Utils Component
 *
 * @comments
 *
 * @create            01/01/1998
 *
 * @author            Gennady Dagman
 *
 * @end
 *
 * ********************************************************************/
#ifdef FILEID_IN_MEMORY
static char __fileid[] = "$Header: lvl7dev\src\system_support\routing\utils\os_xxcmn.c 1.1.3.1 2002/02/13 23:07:15EST ssaraswatula Development  $";
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*======== Specific includes ===========*/

#include "std.h"
#include "common.h"
#include "xx.ext"
#include "os_xx.ext"
#include "os_xxcmn.h"
#include "timer.ext"

#include "l7_common.h"
#include "osapi.h"
#include "platform_config.h"
#include "log.h"

#ifdef _L7_OS_VXWORKS_
#include "vxWorks.h"
#include "symbol.h"
#include "ioLib.h"
#include "taskLib.h"
#endif /* _L7_OS_VXWORKS_ */

#if L7_CORE_CPU == MC360
word _Saved_SR; /* Saved SR register for XX_Freeze, XX_Thaw support          */
word _Temp_SR;  /* Temporary saved SR register for XX_Freeze, XX_Thaw support */
#elif (L7CORE_CPU == MPC860) || (L7_CORE_CPU == MPC8260) || (L7_CORE_CPU == PPC_CORE)
ulng _Saved_EE; /* Saved EE bit of MSR register for XX_Freeze, XX_Thaw support */
#if L7_CORE_CPU == PPC_CORE
/*     Choose proper values for the following symbols         */
/* if non cacheable partition is to be supported with PPC CPU */
#define NO_CACHE_MEMORY    0
#define NO_CACHE_SIZE      0
#define CACHE_LINE_SIZE    32
#endif
t_Handle NoCacheHeap;
void  *_No_Cache_Memory = (void *)NO_CACHE_MEMORY;
#elif L7_CORE_CPU == IDT4650
ulng _Temp_SR;  /* Temporary saved SR register for XX_Freeze, XX_Thaw support */
ulng _Saved_SR; /* Saved SR register for XX_Freeze, XX_Thaw support           */
/* Use default NO_CACHE_MEMORY constant if not defined externally */
#if !defined(NO_CACHE_MEMORY) && !defined(NO_CACHE_SIZE)
#define NO_CACHE_MEMORY    0x20000000     /* 0x80000000 -> 0xa0000000 */
#define NO_CACHE_SIZE      0
#define CACHE_LINE_SIZE    32
#endif
#endif

#if L7_UART_MC360
#include "uart360.ext"
t_Handle p_Uart;
#endif

/*======== interrupt vectors mapping for MPC860 ========*/
#if L7_CORE_CPU == MPC860
#define OSXX_SIU_VECTOR_BASE   0x00
#define OSXX_SIU_VECTOR_END    (OSXX_SIU_VECTOR_BASE + 0x0F)
#define OSXX_CPM_VECTOR_BASE   (OSXX_SIU_VECTOR_END  + 0x11)
#define OSXX_CPM_VECTOR_END    (OSXX_CPM_VECTOR_BASE + 0x1F)
#endif

#define MAX_PRINT_STRING_SIZE 63 /*print string size limit */


static void _LVL7timerLoop(int argc, L7_uint32 *argv[]);
static void _timerLoop(e_Err (*)(), ulng);
void        _handleLVL7Task(int argc, L7_uint32 *argv[]);
void        _handleTask(e_Err (*)(void *), OS_Thread *);
void        _handlePermTask(e_Err (*)(void *), OS_Thread *);


void *lockSem;

#if L7_BCLOCK && L7_INTERNAL_BCLOCK
extern void  BCLOCK_Tick( void );
#endif


#if OS_CMN_MAX_INT_WRAPPERS

#if OS_CMN_MAX_INT_WRAPPERS > 32
#error   "OS_CMN_MAX_INT_WRAPPERS must be less than 33"
#endif

/*========= Interrupt wrapper data type =============*/
typedef struct tagt_OS_IntWrapperData
{
   OS_CMN_IntWrapper wrapper;   /* Wrapper function */
   FUNC_Interrupt    f_intFun;  /* Application interrupt handler */
   t_HandleAsLong    parm;      /* Interrupt handler parameter */
   word              vector;    /* vector number */
} t_OS_IntWrapperData;

/* Forward declarations of wrapper functions */
static void OS_CMN_IntWrapper0( void );
#if OS_CMN_MAX_INT_WRAPPERS > 1
static void OS_CMN_IntWrapper1( void );
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 2
static void OS_CMN_IntWrapper2( void );
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 3
static void OS_CMN_IntWrapper3( void );
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 4
static void OS_CMN_IntWrapper4( void );
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 5
static void OS_CMN_IntWrapper5( void );
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 6
static void OS_CMN_IntWrapper6( void );
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 7
static void OS_CMN_IntWrapper7( void );
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 8
static void OS_CMN_IntWrapper8( void );
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 9
static void OS_CMN_IntWrapper9( void );
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 10
static void OS_CMN_IntWrapper10( void );
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 11
static void OS_CMN_IntWrapper11( void );
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 12
static void OS_CMN_IntWrapper12( void );
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 13
static void OS_CMN_IntWrapper13( void );
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 14
static void OS_CMN_IntWrapper14( void );
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 15
static void OS_CMN_IntWrapper15( void );
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 16
static void OS_CMN_IntWrapper16( void );
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 17
static void OS_CMN_IntWrapper17( void );
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 18
static void OS_CMN_IntWrapper18( void );
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 19
static void OS_CMN_IntWrapper19( void );
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 20
static void OS_CMN_IntWrapper20( void );
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 21
static void OS_CMN_IntWrapper21( void );
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 22
static void OS_CMN_IntWrapper22( void );
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 23
static void OS_CMN_IntWrapper23( void );
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 24
static void OS_CMN_IntWrapper24( void );
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 25
static void OS_CMN_IntWrapper25( void );
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 26
static void OS_CMN_IntWrapper26( void );
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 27
static void OS_CMN_IntWrapper27( void );
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 28
static void OS_CMN_IntWrapper28( void );
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 29
static void OS_CMN_IntWrapper29( void );
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 30
static void OS_CMN_IntWrapper30( void );
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 31
static void OS_CMN_IntWrapper31( void );
#endif

/* Array of interrupt wrapper data.
   Number of initializers in the array defines the max number
   of wrappers which can be used simultaneously
*/
static t_OS_IntWrapperData  intWrapperData[]   =  {
   {  OS_CMN_IntWrapper0,  0, 0, 0 }
#if OS_CMN_MAX_INT_WRAPPERS > 1
   ,{  OS_CMN_IntWrapper1,  0, 0, 0 }
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 2
   ,{  OS_CMN_IntWrapper2,  0, 0, 0 }
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 3
   ,{  OS_CMN_IntWrapper3,  0, 0, 0 }
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 4
   ,{  OS_CMN_IntWrapper4,  0, 0, 0 }
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 5
   ,{  OS_CMN_IntWrapper5,  0, 0, 0 }
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 6
   ,{  OS_CMN_IntWrapper6,  0, 0, 0 }
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 7
   ,{  OS_CMN_IntWrapper7,  0, 0, 0 }
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 8
   ,{  OS_CMN_IntWrapper8,  0, 0, 0 }
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 9
   ,{  OS_CMN_IntWrapper9,  0, 0, 0 }
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 10
   ,{  OS_CMN_IntWrapper10, 0, 0, 0 }
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 11
   ,{  OS_CMN_IntWrapper11, 0, 0, 0 }
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 12
   ,{  OS_CMN_IntWrapper12, 0, 0, 0 }
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 13
   ,{  OS_CMN_IntWrapper13, 0, 0, 0 }
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 14
   ,{  OS_CMN_IntWrapper14, 0, 0, 0 }
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 15
   ,{  OS_CMN_IntWrapper15, 0, 0, 0 }
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 16
   ,{  OS_CMN_IntWrapper16, 0, 0, 0 }
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 17
   ,{  OS_CMN_IntWrapper17, 0, 0, 0 }
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 18
   ,{  OS_CMN_IntWrapper18, 0, 0, 0 }
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 19
   ,{  OS_CMN_IntWrapper19, 0, 0, 0 }
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 20
   ,{  OS_CMN_IntWrapper20, 0, 0, 0 }
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 21
   ,{  OS_CMN_IntWrapper21, 0, 0, 0 }
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 22
   ,{  OS_CMN_IntWrapper22, 0, 0, 0 }
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 23
   ,{  OS_CMN_IntWrapper23, 0, 0, 0 }
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 24
   ,{  OS_CMN_IntWrapper24, 0, 0, 0 }
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 25
   ,{  OS_CMN_IntWrapper25, 0, 0, 0 }
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 26
   ,{  OS_CMN_IntWrapper26, 0, 0, 0 }
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 27
   ,{  OS_CMN_IntWrapper27, 0, 0, 0 }
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 28
   ,{  OS_CMN_IntWrapper28, 0, 0, 0 }
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 29
   ,{  OS_CMN_IntWrapper29, 0, 0, 0 }
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 30
   ,{  OS_CMN_IntWrapper30, 0, 0, 0 }
#endif
#if OS_CMN_MAX_INT_WRAPPERS > 31
   ,{  OS_CMN_IntWrapper31, 0, 0, 0 }
#endif
                                             };

#endif  /* #if OS_CMN_MAX_INT_WRAPPERS */

int _Nesting_counter;

static t_OS_PoolInfo *poolListOS;

/*========= Initialization of processor specific stuff ========*/
e_Err OS_CMN_InitAll( void )
{
   e_Err   err = E_OK;

   _Nesting_counter = 0;

#if L7_CORE_CPU == MC360
   _Saved_SR = 0;
#elif (L7_CORE_CPU == MPC860) || (L7_CORE_CPU == MPC8260) || (L7_CORE_CPU == PPC_CORE)
   _Saved_EE = 0x0008000L;

#if NO_CACHE_SIZE
   if(_No_Cache_Memory &&
     ( (err = XX_InitHeap( "NCHE", (void *)_No_Cache_Memory, NO_CACHE_SIZE, &NoCacheHeap )) != E_OK) )
      return err;
#endif

#endif

   /* Initialize the QUICC */
#if L7_CPM_MC360
   Q_QuiccInit( (ulng)QUICC_BASE, 0x60 );
   /* Uart initialization */
#if L7_UART_MC360
   {
      t_UART_Parms Parms;

      /* Initialize uart */
      memset( &Parms, 0, sizeof( Parms ));
      Parms.Scc      = SCC1;
      Parms.Clk      = BRG1 + SCC1;
      Parms.Baud     = 9600;
      Parms.XonXoff  = TRUE;
      Parms.MaxIdles = 10;  /* Close buffer after 10 Idles (~10ms for baud 9600) */

      if (UART_Init( QUICC_BASE, &Parms, &p_Uart ) != E_OK)
         ASSERT( FALSE );
   }
#endif /* #if L7_UART_MC360 */
#endif /* #if L7_CPM_MC360  */

#if L7_CORE_CPU == MPC860
   if( (err = PQ_Init()) != E_OK )
      return err;
#endif

#if L7_CPM_MPC8260
   if( (err = PQII_Init()) != E_OK )
      return err;
#endif

/* Initialize CPM timers */
#if L7_BCLOCK && L7_INTERNAL_BCLOCK
#if L7_CPM_MC360
   if( (err = Q_InitTimers( QUICC_BASE, 0, 0 )) != E_OK )
   {
      ASSERT(FALSE);
      return err;
   }
#elif L7_CPM_MPC860_FAMILY
   if( (err = PQ_InitTimers()) != E_OK )
   {
      ASSERT(FALSE);
      return err;
   }
#elif L7_CPM_MPC8260
   if( (err = PQII_InitTimers( 0 )) != E_OK )
   {
      ASSERT(FALSE);
      return err;
   }
#endif
#endif /* #if L7_BCLOCK && L7_INTERNAL_BCLOCK */

   return err;
}


/*========= Start of processor specific stuff =========*/
void  OS_CMN_Start( void )
{
#if L7_BCLOCK && L7_INTERNAL_BCLOCK
#if L7_CPM_MC360

   if( Q_ConfigureTimer( QUICC_BASE, BC_CPM_TIMER, 1 ) != E_OK )
   {
      ASSERT(FALSE);
      return;
   }

   /* bind an expiration function to the QUICC timer */
   if( Q_TimerBindExpFunc(BC_CPM_TIMER, (F_Q_TimerExpFunc)(&BCLOCK_Tick), NULLP) != E_OK )
   {
      ASSERT(FALSE);
      return;
   }

   if( Q_StartTimer( QUICC_BASE, BC_CPM_TIMER, TRUE, (1000000/BC_TICKS_PER_SECOND) ) != E_OK )
   {
      ASSERT(FALSE);
      return;
   }

#elif L7_CPM_MPC860_FAMILY

   if( PQ_ConfigureTimer( BC_CPM_TIMER, 1) != E_OK )
   {
      ASSERT(FALSE);
      return;
   }

   /* bind an expiration function to the P-QUICC timer */
   if( PQ_TimerBindExpFunc(BC_CPM_TIMER, (F_PQ_TimerExpFunc)(&BCLOCK_Tick), NULLP) != E_OK )
   {
      ASSERT(FALSE);
      return;
   }

   if(PQ_StartTimer( BC_CPM_TIMER, TRUE, (1000000/BC_TICKS_PER_SECOND)) != E_OK )
   {
      ASSERT(FALSE);
      return;
   }
#elif L7_CPM_MPC8260

   if( PQII_ConfigureTimer( 0, PQII_BC_CPM_TIMER, 1) != E_OK )
   {
      ASSERT(FALSE);
      return;
   }

   /* bind an expiration function to the P-QUICCII timer */
   if( PQII_TimerBindExpFunc(0, PQII_BC_CPM_TIMER, (F_PQII_TimerExpFunc)(&BCLOCK_Tick), 0, NULLP) != E_OK )
   {
      ASSERT(FALSE);
      return;
   }
   if(PQII_StartTimer( 0, PQII_BC_CPM_TIMER, TRUE, (1000000/BC_TICKS_PER_SECOND)) != E_OK )
   {
      ASSERT(FALSE);
      return;
   }
#endif
#endif /* #if L7_BCLOCK && L7_INTERNAL_BCLOCK */
}


/*========= Memory global initialization ==========*/
void OS_CMN_MEM_InitAll(void)
{
    poolListOS = NULLP;
}

/*============ Create and initialize a pool of memory ========*/
e_Err OS_CMN_MEM_Init(void *begaddr, ulng memsize, t_Handle *p_poolHandle)
{
   return OS_CMN_MEM_InitAlign(begaddr,memsize,ALIGN,p_poolHandle);
}

/*==== Calculates the real size of data partition memory ====*/
/*====  which will be allocated by OS_CMN_MEM_InitAlign =====*/
ulng OS_CMN_MEM_SizeofInitAlign(Bool staticPart, ulng memsize, word align)
{
   if(staticPart)
      return memsize+align+sizeof(t_OS_PoolInfo);
   else
      return memsize+align;
}


/*============ Create and initialize a pool of aligned memory ========*/
e_Err OS_CMN_MEM_InitAlign(void *begaddr, ulng memsize, word align, t_Handle *p_poolHandle)
{
   t_OS_PoolInfo *pool;

   if(begaddr) /* Is there user defined partition ? (i.e. are we allocating a static pool ?) */
   {
       pool = (t_OS_PoolInfo *)begaddr;                        /* Put pool info in the beginning of partition */
       memset(begaddr, 0, memsize+align+sizeof(t_OS_PoolInfo));/* Clear all the partition */
       pool->poolStrtAddr = (byte *)begaddr + sizeof(t_OS_PoolInfo);
   }
   else
   {
       pool = (t_OS_PoolInfo *) osapiMalloc(L7_IP_MAP_COMPONENT_ID,
                                            sizeof(t_OS_PoolInfo));
       ASSERT(pool);
       memset(pool, 0, sizeof(t_OS_PoolInfo));

       if(!( pool->poolStrtAddr = (byte *)osapiMalloc(L7_IP_MAP_COMPONENT_ID,
                                                      memsize+align) ))
       {
          osapiFree(L7_IP_MAP_COMPONENT_ID, pool);
          return E_NOMEMORY;
       }

       memset(pool->poolStrtAddr,0,memsize+align);
   }

   pool->poolFreeAdd = pool->poolStrtAddr;

   ALIGN_ADDRESS(pool->poolFreeAdd, align, byte *);

   pool->static_pool = (ulng)begaddr;

   pool->size = pool->sizeFree = memsize;

   if((pool->next = poolListOS) != NULLP)
      poolListOS->prev = pool;
   poolListOS = pool;

   *(p_poolHandle) = (void *)pool;

   return E_OK;
}

/*============ Delete memory pool ===================*/
e_Err OS_CMN_MEM_Delete(t_Handle poolHandle)
{
    t_OS_PoolInfo *pool = (t_OS_PoolInfo *)poolHandle;
    ASSERT ( pool );

    if(pool->prev)
    {
        if((pool->prev->next = pool->next) != NULLP)
            pool->next->prev = pool->prev;
    }
    else
    {
        if((poolListOS = pool->next) != NULLP)
            pool->next->prev = pool->prev;
    }

    if(!pool->static_pool)
    {
        osapiFree(L7_IP_MAP_COMPONENT_ID, pool->poolStrtAddr);
        osapiFree(L7_IP_MAP_COMPONENT_ID, pool);
    }

    return E_OK;
}

/*============ Allocate block of memory ================*/
void *OS_CMN_GetMem(t_Handle poolHandle, ulng size)
{
   byte *Addr = NULLP;
   t_OS_PoolInfo *pool = (t_OS_PoolInfo *)poolHandle;

   OS_XX_Freeze();

   if(pool->sizeFree < size)
   {
      OS_XX_Thaw();
      return NULLP;
   }

   pool->sizeFree-=size;

   Addr = pool->poolFreeAdd;
   pool->poolFreeAdd +=size;

   OS_XX_Thaw();
   return (void *)Addr;
}


/*============ Get free memory counter =================*/
ulng OS_CMN_GetFreeMem(t_Handle poolHandle)
{
    t_OS_PoolInfo *pool = (t_OS_PoolInfo *)poolHandle;
    return (ulng)(pool->sizeFree);
}


byte *OS_CMN_MallocNoCache( ulng Size )
{
   byte *p;

#if ((L7_CORE_CPU == MPC860) || (L7_CORE_CPU == MPC8260) || (L7_CORE_CPU == PPC_CORE)) && NO_CACHE_SIZE
   p = (byte *)XX_MallocExt(NoCacheHeap, Size, TRUE);

#elif NO_CACHE_MEMORY && !NO_CACHE_SIZE
   if ( CACHE_LINE_SIZE && (Size % CACHE_LINE_SIZE) )
      Size = ((Size / CACHE_LINE_SIZE) + 1) * CACHE_LINE_SIZE;
   p = (byte *)XX_MallocAlign( Size, CACHE_LINE_SIZE );
   if (p)
      p = (byte *)((ulng)p | NO_CACHE_MEMORY);

#else
   p = (byte *) XX_Malloc( Size );

#endif

   return p;
}


byte *OS_CMN_MallocNoCacheAlign( ulng Size, word Align )
{
   byte *p;

#if ((L7_CORE_CPU == MPC860) || (L7_CORE_CPU == MPC8260) || (L7_CORE_CPU == PPC_CORE)) && NO_CACHE_SIZE
   p = (byte *)XX_MallocAlignExt(NoCacheHeap, Size, Align);

#elif NO_CACHE_MEMORY && !NO_CACHE_SIZE
   Align = (Align < CACHE_LINE_SIZE) ? CACHE_LINE_SIZE : Align;
   if ( CACHE_LINE_SIZE && (Size % CACHE_LINE_SIZE) )
      Size = ((Size / CACHE_LINE_SIZE) + 1) * CACHE_LINE_SIZE;
   p = (byte *) XX_MallocAlign( Size, Align );
   if (p)
      p = (byte *)((ulng)p | NO_CACHE_MEMORY);

#else
   p = (byte *) XX_MallocAlign( Size, Align );

#endif

   return p;
}


void OS_CMN_FreeNoCache( void *buf )
{
#if NO_CACHE_MEMORY && !NO_CACHE_SIZE
   XX_Free( (void *)((ulng)buf & ~NO_CACHE_MEMORY ) );

#else
   XX_Free( buf );
#endif
}



#if RTOS != NO_RTOS
#if L7_CPM_MPC8260 || L7_CPM_MPC860  || (L7_CORE_CPU == MPC860) || L7_CPM_MC360    || (L7_CORE_CPU == MC360)
void U_Timer( word Timer, void *pQuicc )
{
   UNUSED( Timer );
   UNUSED( pQuicc );
}
#endif
#endif

#if L7_CPM_MC360
void U_SwTimer( word Timers, void *pQuicc ){}
void U_Error( char * ErrorMsg ){}
#endif

#if (L7_CORE_CPU == MPC8260) || (L7_CORE_CPU == MPC860) || (L7_CORE_CPU == PPC_CORE)
/*
 * Set the MSR bits
 */
void SetBits_MSR( ulng msr )
{
   __asm("   mfmsr  4        ");
   __asm("   or     4,4,3    ");
   __asm("   mtmsr  4        ");
   __asm("   isync           ");
   __asm("   sync            ");
}

/*
 * Clear the MSR bits
 */
ulng ClearBits_MSR( ulng msr )
{
   __asm("   mfmsr  4        ");
   __asm("   and    5,4,3    ");
   __asm("   andc   4,4,3    ");
   __asm("   mtmsr  4        ");
   __asm("   addi   3,5,0    ");
   __asm("   isync           ");
   __asm("   sync            ");
}
#endif /* (L7_CORE_CPU == MPC8260) || (L7_CORE_CPU == MPC860) || (L7_CORE_CPU == PPC_CORE) */

#if OS_CMN_MAX_INT_WRAPPERS

/* Interrupt wrapper service */

/* OS_CMN_AllocWrapper
   Allocate interrupt wrapper.

   Function scans a wrapper array and looks for an element allocated to the same vector.
   If wrapper with the same vector is found - it is updated.
   Otherwise, an empty element is taken.

   Parameters:
      f_intFun          application interrupt handler
      vector            interrupt vector
      parm              passed transparently to application interrupt handler

   Functions return a pointer to internal wrapper function.
   The caller is responsible to connect this function to a h/w interrupt.
   When a h/w interrupt is fired, the rapper function calls an application
   interrupt handler passing <parm> as a parameter.

   OS_CMN_AllocWrapper returns NULLP if the wrapper array is exhausted.
*/
OS_CMN_IntWrapper OS_CMN_AllocWrapper( FUNC_Interrupt f_intFun, word vector, t_HandleAsLong parm )
{
   int   empty_idx = OS_CMN_MAX_INT_WRAPPERS;
   int   idx;
   int   i;

   for( i=0; i< OS_CMN_MAX_INT_WRAPPERS; i++ )
   {
      if ( intWrapperData[i].vector == vector )
         break;
      if ( !intWrapperData[i].vector )
         empty_idx = i;
   }
   if ( i == OS_CMN_MAX_INT_WRAPPERS )
   {
      if ( empty_idx == OS_CMN_MAX_INT_WRAPPERS )
         return NULLP;
      idx = empty_idx;
   }
   else
      idx = i;

   intWrapperData[idx].vector    = vector;
   intWrapperData[idx].f_intFun  = f_intFun;
   intWrapperData[idx].parm      = parm;

   return intWrapperData[idx].wrapper;
}


/* OS_CMN_FreeWrapper
   Release interrupt wrapper assocoated with the given vector.

   Returns: E_OK
            E_FAILED    vector is not in use
*/
e_Err OS_CMN_FreeWrapper( word vector )
{
   int i;

   for( i=0; i< OS_CMN_MAX_INT_WRAPPERS; i++ )
   {
      if ( intWrapperData[i].vector == vector )
      {
         intWrapperData[i].vector    = 0;
         intWrapperData[i].f_intFun  = NULLP;
         intWrapperData[i].parm      = 0;
         return E_OK;
      }
   }

   return E_FAILED;
}

/* Interrupt wrapper functions */
static void OS_CMN_IntWrapper0( void )
{
   intWrapperData[0].f_intFun( intWrapperData[0].parm );
}

#if OS_CMN_MAX_INT_WRAPPERS > 1
static void OS_CMN_IntWrapper1( void )
{
   intWrapperData[1].f_intFun( intWrapperData[1].parm );
}
#endif

#if OS_CMN_MAX_INT_WRAPPERS > 2
static void OS_CMN_IntWrapper2( void )
{
   intWrapperData[2].f_intFun( intWrapperData[2].parm );
}
#endif

#if OS_CMN_MAX_INT_WRAPPERS > 3
static void OS_CMN_IntWrapper3( void )
{
   intWrapperData[3].f_intFun( intWrapperData[3].parm );
}
#endif

#if OS_CMN_MAX_INT_WRAPPERS > 4
static void OS_CMN_IntWrapper4( void )
{
   intWrapperData[4].f_intFun( intWrapperData[4].parm );
}
#endif

#if OS_CMN_MAX_INT_WRAPPERS > 5
static void OS_CMN_IntWrapper5( void )
{
   intWrapperData[5].f_intFun( intWrapperData[5].parm );
}
#endif

#if OS_CMN_MAX_INT_WRAPPERS > 6
static void OS_CMN_IntWrapper6( void )
{
   intWrapperData[6].f_intFun( intWrapperData[6].parm );
}
#endif

#if OS_CMN_MAX_INT_WRAPPERS > 7
static void OS_CMN_IntWrapper7( void )
{
   intWrapperData[7].f_intFun( intWrapperData[7].parm );
}
#endif

#if OS_CMN_MAX_INT_WRAPPERS > 8
static void OS_CMN_IntWrapper8( void )
{
   intWrapperData[8].f_intFun( intWrapperData[8].parm );
}
#endif

#if OS_CMN_MAX_INT_WRAPPERS > 9
static void OS_CMN_IntWrapper9( void )
{
   intWrapperData[9].f_intFun( intWrapperData[9].parm );
}
#endif

#if OS_CMN_MAX_INT_WRAPPERS > 10
static void OS_CMN_IntWrapper10( void )
{
   intWrapperData[10].f_intFun( intWrapperData[10].parm );
}
#endif

#if OS_CMN_MAX_INT_WRAPPERS > 11
static void OS_CMN_IntWrapper11( void )
{
   intWrapperData[11].f_intFun( intWrapperData[11].parm );
}
#endif

#if OS_CMN_MAX_INT_WRAPPERS > 12
static void OS_CMN_IntWrapper12( void )
{
   intWrapperData[12].f_intFun( intWrapperData[12].parm );
}
#endif

#if OS_CMN_MAX_INT_WRAPPERS > 13
static void OS_CMN_IntWrapper13( void )
{
   intWrapperData[13].f_intFun( intWrapperData[13].parm );
}
#endif

#if OS_CMN_MAX_INT_WRAPPERS > 14
static void OS_CMN_IntWrapper14( void )
{
   intWrapperData[14].f_intFun( intWrapperData[14].parm );
}
#endif

#if OS_CMN_MAX_INT_WRAPPERS > 15
static void OS_CMN_IntWrapper15( void )
{
   intWrapperData[15].f_intFun( intWrapperData[15].parm );
}
#endif

#if OS_CMN_MAX_INT_WRAPPERS > 16
static void OS_CMN_IntWrapper16( void )
{
   intWrapperData[16].f_intFun( intWrapperData[16].parm );
}
#endif

#if OS_CMN_MAX_INT_WRAPPERS > 17
static void OS_CMN_IntWrapper17( void )
{
   intWrapperData[17].f_intFun( intWrapperData[17].parm );
}
#endif

#if OS_CMN_MAX_INT_WRAPPERS > 18
static void OS_CMN_IntWrapper18( void )
{
   intWrapperData[18].f_intFun( intWrapperData[18].parm );
}
#endif

#if OS_CMN_MAX_INT_WRAPPERS > 19
static void OS_CMN_IntWrapper19( void )
{
   intWrapperData[19].f_intFun( intWrapperData[19].parm );
}
#endif

#if OS_CMN_MAX_INT_WRAPPERS > 20
static void OS_CMN_IntWrapper20( void )
{
   intWrapperData[20].f_intFun( intWrapperData[20].parm );
}
#endif

#if OS_CMN_MAX_INT_WRAPPERS > 21
static void OS_CMN_IntWrapper21( void )
{
   intWrapperData[21].f_intFun( intWrapperData[21].parm );
}
#endif

#if OS_CMN_MAX_INT_WRAPPERS > 22
static void OS_CMN_IntWrapper22( void )
{
   intWrapperData[22].f_intFun( intWrapperData[22].parm );
}
#endif

#if OS_CMN_MAX_INT_WRAPPERS > 23
static void OS_CMN_IntWrapper23( void )
{
   intWrapperData[23].f_intFun( intWrapperData[23].parm );
}
#endif

#if OS_CMN_MAX_INT_WRAPPERS > 24
static void OS_CMN_IntWrapper24( void )
{
   intWrapperData[24].f_intFun( intWrapperData[24].parm );
}
#endif

#if OS_CMN_MAX_INT_WRAPPERS > 25
static void OS_CMN_IntWrapper25( void )
{
   intWrapperData[25].f_intFun( intWrapperData[25].parm );
}
#endif

#if OS_CMN_MAX_INT_WRAPPERS > 26
static void OS_CMN_IntWrapper26( void )
{
   intWrapperData[26].f_intFun( intWrapperData[26].parm );
}
#endif

#if OS_CMN_MAX_INT_WRAPPERS > 27
static void OS_CMN_IntWrapper27( void )
{
   intWrapperData[27].f_intFun( intWrapperData[27].parm );
}
#endif

#if OS_CMN_MAX_INT_WRAPPERS > 28
static void OS_CMN_IntWrapper28( void )
{
   intWrapperData[28].f_intFun( intWrapperData[28].parm );
}
#endif

#if OS_CMN_MAX_INT_WRAPPERS > 29
static void OS_CMN_IntWrapper29( void )
{
   intWrapperData[29].f_intFun( intWrapperData[29].parm );
}
#endif

#if OS_CMN_MAX_INT_WRAPPERS > 30
static void OS_CMN_IntWrapper30( void )
{
   intWrapperData[30].f_intFun( intWrapperData[30].parm );
}
#endif

#if OS_CMN_MAX_INT_WRAPPERS > 31
static void OS_CMN_IntWrapper31( void )
{
   intWrapperData[31].f_intFun( intWrapperData[31].parm );
}
#endif

#endif   /* #if OS_CMN_MAX_INT_WRAPPERS */


/* 5/11/2005 - The code from here to the end of the file was in
 * os/linux/routing/os_xxlinux.c and os/vxworks/routing/os_xxvxw.c.
 * OS calls have been removed and replaced with osapi calls, leaving
 * OS independent code inserted here.
 */

/*========= Initialization of OS/processor related stuff ============*/
e_Err OS_XX_InitAll( void )
{
   if ((lockSem = osapiSemaMCreate(OSAPI_SEM_Q_FIFO)) == NULL)
   {
      L7_LOG_ERROR((L7_uint32)lockSem);
   }

#if ((L7_CORE_CPU == MPC860) || (L7_CORE_CPU == MPC8260)) && NO_CACHE_SIZE && !NO_CACHE_MEMORY
   extern void * cacheDmaMalloc(size_t bytes);


   if( !(_No_Cache_Memory = (void *) cacheDmaMalloc (NO_CACHE_SIZE)) )
   {
      L7_LOG_ERROR(0);
      return E_NOMEMORY;
   }
#endif

   return OS_CMN_InitAll();
}

/*========= Start things running - just suspend current (root) task ============*/
void OS_XX_Start( void )
{
   OS_CMN_Start();
}


/*============Redirect print and Terminate functions */
static FUNC_Print f_Print;
static FUNC_Print f_Terminate;

FUNC_Print OS_XX_RedirectPrint(FUNC_Print func_print)
{
    FUNC_Print prev = f_Print;
    f_Print = func_print;
    return prev;
}
FUNC_Print OS_XX_RedirectTerminate(FUNC_Print func_terminate)
{
    FUNC_Print prev = f_Print;
    f_Terminate = func_terminate;
    return prev;
}

/*========= Print a message ==============*/
static e_Err os_xx_UnpackPrint(void *p_Info)
{
   t_XXCallInfo *callInfo  = (t_XXCallInfo *) p_Info;
   byte         *string    = (byte *)PACKET_GET(callInfo,0);

   printf("%s\n", string);
   XX_Free((void *)string);

   return E_OK;
}

/*========= print a message ==============*/
void OS_XX_Print ( const char *string )
{
   int    length, len=0, index;
   extern ulng Print(char *format, ...);
#if L7_UART_MC360
   extern t_Handle p_Uart;
   byte   tmp[100];
#endif
   e_Err  e;

   /* If XX_Print was redirected just call f_Print */
   if(f_Print != NULLP)
   {
     f_Print((char *)string);
     return;
   }
   /* check and modify string length */
   for(length = strlen(string), index = 0; length > 0; length -= len, index +=len)
   {
       char tmpString[MAX_PRINT_STRING_SIZE + 1];
       len = length > MAX_PRINT_STRING_SIZE? MAX_PRINT_STRING_SIZE: length;
       memcpy(tmpString, &string[index], len);
       tmpString[len] = 0;

#if L7_UART_MC360
       sprintf( tmp, "%s\r\n", tmpString );
       UART_Transmit( p_Uart, tmp, strlen( tmp ) );
#else
       if(L7_DEFAULT_THREAD)
       {
           byte *tmp;

           if( (tmp = (byte *)XX_MallocChunk(len + 1)) )
           {
               t_XXCallInfo  *p_callInfo;

               memcpy(tmp,tmpString,len + 1);
               PACKET_INIT(p_callInfo, os_xx_UnpackPrint, NULLP, NULLP, 1, (ulng)tmp);
               e = XX_Call(L7_DEFAULT_THREAD, p_callInfo);
               if (e != E_OK)
               {
                   L7_LOGF(L7_LOG_SEVERITY_INFO, L7_IP_MAP_COMPONENT_ID, "OS_XX_Print: unable to send message for printing");
               }
           }
       }
       else
           printf("%s\n", tmpString);  /* This implementation works only from task thread */
           /* logMsg("%s\n",(int)tmpString,0,0,0,0,0); */
#endif
   }
}

/*========= Memory global initialization ==========*/
void OS_MEM_InitAll(void)
{
   OS_CMN_MEM_InitAll();
}

/*============ Create and initialize a pool of memory ===============*/
e_Err OS_MEM_Init(void *begaddr, ulng memsize, t_Handle *p_poolHandle)
{
   return OS_CMN_MEM_Init(begaddr, memsize, p_poolHandle);
}

/*============ Create and initialize a pool of memory ===============*/
e_Err OS_MEM_InitAlign(void *begaddr, ulng memsize, word align, t_Handle *p_poolHandle)
{
   return OS_CMN_MEM_InitAlign(begaddr, memsize, align, p_poolHandle);
}

/*====== Calculates size of data partition allocated by OS_MEM_InitAlign =======*/
ulng  OS_MEM_SizeofInitAlign(Bool staticPart, ulng memsize, word align)
{
   return OS_CMN_MEM_SizeofInitAlign(staticPart, memsize, align);
}

/*============ Delete memory pool ===================*/
e_Err OS_MEM_Delete(t_Handle poolHandle)
{
   return OS_CMN_MEM_Delete(poolHandle);
}

/*============ Allocate block of memory ================*/
void *OS_GetMem(t_Handle poolHandle, ulng size)
{
   return OS_CMN_GetMem(poolHandle, size);
}

/*============ Get free memory counter =================*/
ulng OS_GetFreeMem(t_Handle poolHandle)
{
   return OS_CMN_GetFreeMem(poolHandle);
}


/*======== Disable the interrupts ===========*/
void OS_XX_Freeze(void)
{
#if defined(OS_CMN_Freeze)
   OS_CMN_Freeze
#else
   if (osapiSemaTake(lockSem, L7_WAIT_FOREVER) != L7_SUCCESS)
   {
      L7_LOG_ERROR(L7_FAILURE);
   }
#endif
}

/*======= Enable the interrupts ==========*/
void OS_XX_Thaw(void)
{
#if defined(OS_CMN_Freeze)
   OS_CMN_Thaw
#else
   if (osapiSemaGive(lockSem) != L7_SUCCESS)
   {
      L7_LOG_ERROR(L7_FAILURE);
   }
#endif
}


/*======= Mutex creation/deletion routines ===========*/
e_Err OS_XX_CreateMutex(t_Handle *p)
{
   return E_OK;
}


void OS_XX_DeleteMutex(t_Handle *p)
{
}


/*======= Lock/Unlock routines ===========*/
void OS_XX_Lock ( t_Handle p )
{
}

void OS_XX_Unlock ( t_Handle p )
{
}

/*========= Post a message to a thread ============*/
extern t_Handle TIMER_Thread;
extern t_Handle TIMER_HighThread;

e_Err OS_XX_Call(t_Handle threadID, void *p_info, char *funcName, ulng lineNum)
{
   OS_Thread *p_thread = (OS_Thread *) threadID;
   t_XXCallInfo *xx  = (t_XXCallInfo *)p_info;
    L7_RC_t         rc;
#ifdef DCB_XX_DEBUG
    int             i;
#endif /* DCB_XX_DEBUG */

    if (xx == NULL)
    {
#ifdef DCB_XX_DEBUG
        printf("OS_XX_Call: could not send NULL message to thread %p\n", threadID);
#endif /* DCB_XX_DEBUG */
        return E_FAILED;
    }

  if (p_thread == (OS_Thread *)0xffffff)
  {
    ASSERT(TIMER_Thread != NULLP);
    p_thread = (OS_Thread *) TIMER_Thread;
    }

#ifdef DCB_XX_DEBUG
    printf("OS_XX_Call: ThreadID: %p, QueueID: %d, nmbParams: %ld\n",
           threadID, xx->QueueID, xx->nmbParams);
    for (i = 0; i < xx->nmbParams; i++)
    {
        printf("OXC Param[%d]: 0x%08x\n", (ulng)i, (ulng)PACKET_GET(xx, i));
    }
#endif /* DCB_XX_DEBUG */

    /* copy across source information */
    osapiStrncpy(xx->funcName, funcName, MAX_FUNCNAME);
    xx->lineNum = lineNum;

    rc = osapiMessageSend((void *) p_thread->QueueID[xx->QueueID],
                         (void *) &(xx->start), (L7_uint32)OS_MSG_SIZE,
                         L7_NO_WAIT, L7_MSG_PRIORITY_NORM);

    if (rc == L7_SUCCESS)
    {
        osapiSemaGive(p_thread->MsgQSema);
    }
    else
    {
#ifdef DCB_XX_DEBUG
        printf("OS_XX_Call: could not send message to thread %p\n", threadID);
#endif /* DCB_XX_DEBUG */
        XX_Free(p_info);
        return E_FAILED;
    }

    return E_OK;
}

char *fptr2name(void *func_ptr, char *func_name, ulng name_size)
{
  L7_uint32 offset;

  if (osapiFunctionLookup((L7_uint32) func_ptr, func_name, name_size,
               &offset) != L7_SUCCESS)
  {
    snprintf(func_name, name_size, "Unk");
  }
  return func_name;
}

void XX_DisplayQueue(t_Handle threadID, ulng queueId, ulng resolveNames, long maxDepth)
{
  void * msgQueue = 0;
  L7_int32 currLen = 0, currOffset = 0;
  t_XXCallInfo *p_info = 0;
  char currFn[MAX_FUNCNAME];
  char unpackFnName[MAX_FUNCNAME];
  OS_Thread *p_thread = (OS_Thread *) threadID;
  L7_RC_t rc;

  if(!threadID)
  {
    printf("\r\nInvalid threadId, please specify a threadId to avoid crashing the box!");
    return;
  }

  if(queueId && (p_thread->numQueues < queueId))
  {
    printf("\r\nInvalid queueId, thread has %u queue(s)", (L7_uint32)p_thread->numQueues);
    return;
  }

  /* default to 20, -1 => show all */
  if(maxDepth == 0)
    maxDepth = 20;

  msgQueue = (void *)p_thread->QueueID[queueId];

  rc = osapiMsgQueueGetNumMsgs(msgQueue, &currLen);
  if(rc != L7_SUCCESS)
  {
     L7_LOG_ERROR(rc);
  }
  printf("\r\nnumber of messages %d", currLen);

  while((maxDepth < 0) || (currOffset < maxDepth))
  {
    /* peek message */
    p_info = 0;
    if(osapiMessagePeek(msgQueue, (void *) &p_info, OS_MSG_SIZE, currOffset) != L7_SUCCESS)
    {
      printf("\r\nosapiMessagePeek failed!");
      break;
    }

    /* print message info */
    if(p_info)
    {
      memcpy(currFn, p_info->funcName, MAX_FUNCNAME);
      currFn[MAX_FUNCNAME-1] = 0;
      if(resolveNames)
        printf("\r\nmsg %d queued by %s:%d calls %s(0x%x)",
          currOffset, currFn, (int)p_info->lineNum,
          fptr2name(p_info->unpackFunc, unpackFnName, MAX_FUNCNAME),
          (int)p_info->unpackFunc);
      else
        printf("\r\nmsg %d queued by %s:%d calls 0x%x", currOffset, currFn, (int)p_info->lineNum, (unsigned int)p_info->unpackFunc);
    }

    currOffset++;
  }
}


/*========= Create timer task ===============*/
e_Err OS_XX_CreateTmrTsk ( ulng period, e_Err (*f_Tick) (void), t_Handle *p_TimerHndle )
{
   static int timerNum = 0;

   ulng TimerTaskId;
   char TmrTskName[22];
   L7_uint32 argc = 2;
   L7_uint32 **Argv = L7_NULL ;   /* Argv[] is an array of pointers */

    if (f_Tick == L7_NULL)
    {
        argc = 1;
    }

    /* the argv[] memory will be freed by spawned task */
    Argv = osapiMalloc(L7_IP_MAP_COMPONENT_ID, sizeof(L7_uint32 *) * argc);
    ASSERT(Argv);

    if (argc == 2)
    {
       Argv[0] = (L7_uint32 *)f_Tick;
       Argv[1] = (L7_uint32 *)period;
   }
   else
   {
       Argv[0] = (L7_uint32 *)period;
   }

   sprintf ( TmrTskName, "tL7Timer%d", timerNum++ );
   /* packing the address for passing to taskSpawn */

    TimerTaskId = osapiTaskCreate(TmrTskName, _LVL7timerLoop, argc, Argv,
                                  20000, 100, L7_DEFAULT_TASK_SLICE);

    if (TimerTaskId == L7_ERROR)
    {
        return E_FAILED;
    }

   *p_TimerHndle = (t_Handle)TimerTaskId;

   return E_OK;
}


/*============== Kill a timer task routine ============*/
e_Err OS_XX_KillTmrTsk (t_Handle timerHandler )
{
   osapiTaskDelete( (L7_uint32) timerHandler);
   return E_OK;
}

/*=========== Create a new thread routine =============*/
e_Err OS_XX_CreateThread(byte priority, e_Err (*f_Thread) (void *),
                         Bool Permanent, t_Handle *p_threadId,
                         byte numQueues,               /* Num queues serviced by this thread */
                         unsigned int *queueLengths,   /* Array of queue lengths */
                         unsigned int *queueWeights,   /* Num msgs to reach from each queue */
                         char *ThreadName)
{
   L7_uint32   argc = 3;
   OS_Thread *p_thread;
    int         i;
    Bool        *tmpPermanent;
    char queueName[255];
    unsigned int qLen;

   L7_uint32 **Argv = L7_NULL ;   /* Argv[] is an array of pointers */


    if (Permanent == TRUE && numQueues > 1)
    {
        ASSERT(FALSE);
        return E_FAILED;
    }

    tmpPermanent = (Bool *) XX_Malloc(sizeof(Bool));
    if (tmpPermanent == NULL)
    {
        return E_FAILED;
    }
    *tmpPermanent = Permanent;

    if (numQueues > L7_ROUTING_MAX_QUEUES)
    {
        return E_FAILED;
    }

   p_thread = (OS_Thread *) XX_Malloc(sizeof(OS_Thread));
   ASSERT(p_thread);

   memset(p_thread, 0, sizeof(OS_Thread));

    if (f_Thread == L7_NULL)
    {
        argc = 2;
    }

    /* the argv[] memory will be freed by spawned task */
    Argv = osapiMalloc(L7_IP_MAP_COMPONENT_ID, sizeof(L7_uint32 *) * argc);
    ASSERT(Argv);

    p_thread->MsgQSema = osapiSemaCCreate (OSAPI_SEM_Q_FIFO, 0);

    ASSERT(p_thread->MsgQSema);

    for (i = 0; i < numQueues; i++)
    {
      osapiSnprintf(queueName, sizeof(queueName), "%s_q%d", ThreadName, i);
      if (queueLengths)
        qLen = queueLengths[i];
      else
        qLen = OS_MAX_MSG_IN_QUEUE;
      p_thread->QueueID[i] = (ulng) osapiMsgQueueCreate(queueName, qLen, OS_MSG_SIZE);
      if (queueWeights)
      {
        p_thread->QueueWeights[i] = queueWeights[i];
      }
      else
        p_thread->QueueWeights[i] = 1;
      ASSERT(p_thread->QueueID[i]);
    }

    if (argc == 3)
    {
          Argv[0] = (L7_uint32 *)f_Thread;
          Argv[1] = (L7_uint32 *)p_thread;
        Argv[2] = (L7_uint32 *)tmpPermanent;
      }
      else
      {
          Argv[0] = (L7_uint32 *)p_thread;
        Argv[1] = (L7_uint32 *)tmpPermanent;
    }

#ifdef DCB_XX_DEBUG
    printf("OS_XX_CreateThread: creating %s, argc %d, priority %d, numQueues %d\n",
           ThreadName, argc, priority, numQueues);
#endif /* DCB_XX_DEBUG */

    p_thread->TaskID = osapiTaskCreate(ThreadName, _handleLVL7Task, argc, Argv,
                                       40000, priority, L7_DEFAULT_TASK_SLICE);

    if (p_thread->TaskID == L7_ERROR)
    {
        ASSERT(FALSE);
        for (i = 0; i < numQueues; i++)
        {
            if (p_thread->QueueID[i])
            {
                osapiMsgQueueDelete((void *)p_thread->QueueID[i]);
            }
        }
        osapiSemaDelete(p_thread->MsgQSema);
        XX_Free(p_thread);
        return E_FAILED;
    }

    p_thread->numQueues = numQueues;

   *p_threadId = (t_Handle)p_thread;

   return E_OK;
}

/*============== Kill a thread ============*/
e_Err OS_XX_DestroyThread(void *p_Info)
{
    e_Err ierror = E_OK;
    ulng taskid;
    int         i;
    t_XXCallInfo *xxCallInfo = (t_XXCallInfo *)p_Info;
    OS_Thread *p_thread;

    if (!xxCallInfo)
    {
        return E_FAILED;
    }

    p_thread = (OS_Thread *)PACKET_GET(xxCallInfo, 0);
    taskid = p_thread->TaskID;

    for (i = 0; i < p_thread->numQueues; i++)
    {
        if (osapiMsgQueueDelete((void *)p_thread->QueueID[i]) != L7_SUCCESS)
        {
           ierror = E_FAILED;
        }
    }

    XX_Free(xxCallInfo);

    osapiSemaDelete(p_thread->MsgQSema);

    XX_Free(p_thread);

    /* this function may be called on the thread being destroyed.
     * So we have to do the task delete last. */
    osapiTaskDelete(taskid);

    return(ierror);
}

/*============== Kill a thread in the thread's context itself ============*/
e_Err OS_XX_KillThread(t_Handle threadId, Bool taskContext)
{
   t_XXCallInfo    *xxci = NULLP;
   e_Err           e;

   PACKET_INIT(xxci, OS_XX_DestroyThread, 0, 0, 1, threadId);

   if(taskContext == TRUE)
   {
      e = XX_Call(threadId, xxci);   /* Kill the thread in self context */
      if (e != E_OK)
      {
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID, "OS_XX_KillThread: unable to kill thread");
      }
   }
   else
   {
      OS_XX_DestroyThread(xxci); /* Kill the thread right away */
   }

   return E_OK;
}

/*=========== Get current board id ===============*/
word OS_XX_GetCurBoardId(void)
{
    return 0;
}

/*=========== Get current thread id ==============*/
Bool OS_XX_IsThreadCurrent(t_Handle threadId)
{
   OS_Thread *p_thread = (OS_Thread *)threadId;

   return ( p_thread->TaskID == osapiTaskIdSelf() );
}

/*=========== Get current processor id ===========*/
ulng OS_XX_GetCurProcId(void)
{
    return 0;
}

/*=========== get current date ===================*/
void OS_XX_GetDateTime ( t_DateTime *dateTime )
{

}

/* Allocate memory block from non cacheable area */
byte *OS_XX_MallocNoCache( ulng Size )
{
   return OS_CMN_MallocNoCache( Size );
}

/* Allocate memory block from non cacheable area with alignment */
byte *OS_XX_MallocNoCacheAlign( ulng Size, word Align )
{
   return OS_CMN_MallocNoCacheAlign( Size, Align);
}

/* Release memory block allocated from non cacheable area */
void OS_XX_FreeNoCache( void *buf )
{
   OS_CMN_FreeNoCache( buf );
}



/*=====================================================*/
/*=============== Local routines ======================*/
/*=====================================================*/

/*========== Timer task handle ========================*/
static void _LVL7timerLoop(int argc, L7_uint32 *argv[])
{
  L7_uint32 *local_argv[2];
  L7_uint32 i;

  if (argv != L7_NULL)
  {
    /* copy contents of argv[] and free its memory block before starting task */
    memset(local_argv, 0, sizeof(local_argv));

        for (i = 0; i < argc; i++)
        {
            local_argv[i] = argv[i];
        }

        osapiFree(L7_IP_MAP_COMPONENT_ID, argv);

        if ((argc == 2) && (local_argv[0] != L7_NULL))
        {
            _timerLoop((e_Err (*)()) local_argv[0], (ulng) local_argv[1]);
        }
        if ((argc == 1) && (local_argv[0] != L7_NULL))
        {
            _timerLoop(0, (ulng) local_argv[0]);
        }
    }
}

static void _timerLoop(e_Err (*f_Tick)(), ulng period)
{
  static L7_uint32  timerHandle;
  L7_uint32  msecWait;
  L7_RC_t rc;

    /*
    convert incoming period value (in ticks) to milliseconds for osapi

    NOTE: this is actually (period/TICKS_PER_SECOND)*1000, but is
    rearranged for integer division.
    */

   msecWait = (period * 1000) / TICKS_PER_SECOND;
   if (msecWait == 0)
    {
     msecWait = 1;
    }

   rc = osapiPeriodicUserTimerRegister(msecWait, &timerHandle);
   ASSERT(rc == L7_SUCCESS);

    osapiTaskDelay(2);

   while (TRUE)
   {
     osapiPeriodicUserTimerWait(timerHandle);

     /* We call the function pointer directly.This is a temporary Fix */

        if (f_Tick)
        {
        (*f_Tick) ();
   }
}
}

/*======== Task handles ======================*/
void _handleLVL7Task(int argc, L7_uint32 *argv[])
{
    L7_uint32   *local_argv[3];
    L7_uint32   i;
    Bool        tmpPerm;

    if (argv != L7_NULL)
    {
        /* copy contents of argv[] and free its memory block before starting task */

        memset(local_argv, 0, sizeof(local_argv));

        for (i = 0; i < argc; i++)
        {
            local_argv[i] = argv[i];
        }

        osapiFree(L7_IP_MAP_COMPONENT_ID, argv);

        if ((argc == 3) && (local_argv[0] != L7_NULL))
        {
#ifdef DCB_XX_DEBUG
            printf("_handleLvl7Task: argv0: %p, argv1: %p, argv2: %p\n",
                   local_argv[0], local_argv[1], local_argv[2]);
#endif /* DCB_XX_DEBUG */

            tmpPerm = *((Bool *) local_argv[2]);
            XX_Free(local_argv[2]);
            if (tmpPerm == TRUE)
            {
                _handlePermTask((e_Err (*)(void *)) local_argv[0],
                            (OS_Thread *) local_argv[1]);
            }
            else
            {
                _handleTask((e_Err (*)(void *)) local_argv[0],
                            (OS_Thread *) local_argv[1]);
            }
        }
        if ((argc == 2) && (local_argv[0] != L7_NULL))
        {
#ifdef DCB_XX_DEBUG
            printf("_handleLvl7Task: argv0: %p, argv1: %p\n",
                   local_argv[0], local_argv[1]);
#endif /* DCB_XX_DEBUG */

            tmpPerm = *((Bool *) local_argv[1]);
            XX_Free(local_argv[1]);
            if (tmpPerm == TRUE)
            {
                _handlePermTask(0, (OS_Thread *) local_argv[0]);
            }
            else
            {
                _handleTask(0, (OS_Thread *) local_argv[0]);
            }
        }
    }
}

static ulng debugQueues = 0;
void DebugQueues(ulng val)
{
  debugQueues = val;
}

/* Thread handler */
void _handleTask(e_Err (*f_Thread)(void *), OS_Thread *p_thread)
{
    t_XXCallInfo *p_info;
    L7_RC_t status;
    int     queuesRead = 0, currentQ = 0;
    int     msgsReadFromCurrentQ = 0;
    L7_uint32 execStart = 0;
    int lastQ;

#ifdef DCB_XX_DEBUG
    printf("_handleTask: f_Thread: %p, p_thread: %p\n",
           f_Thread, p_thread);
#endif /* DCB_XX_DEBUG */

    for (;;)
    {
        p_info = NULLP;

        if (osapiSemaTake(p_thread->MsgQSema, L7_WAIT_FOREVER) != L7_SUCCESS)
        {
            L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
                   "Failure taking task message queue semaphore.");
            continue;
        }

#ifdef DCB_XX_DEBUG
        printf("_handleTask: f_Thread: %p, p_thread %p, got MsgQSema\n",
               f_Thread, p_thread);
#endif /* DCB_XX_DEBUG */

        /* Get a message from the current queue. Continue to service
         * the current queue until the number of messages processed from
         * the queue equals the queue weight. Only try each queue once. */

        status = L7_ERROR;
        queuesRead = 0;

        while ((status != L7_SUCCESS) && (queuesRead < p_thread->numQueues))
        {
            status = osapiMessageReceive((void *) p_thread->QueueID[currentQ],
                                         (void *) &p_info, OS_MSG_SIZE,
                                         L7_NO_WAIT);

#ifdef DCB_XX_DEBUG
            printf("_handleTask: status = %d for q: %d\n", status, currentQ);
#endif /* DCB_XX_DEBUG */
            if (status != L7_SUCCESS)
            {
              /* No messages in current Q. Go to next Q. */
              currentQ++;
              currentQ %= p_thread->numQueues;
              queuesRead++;
              msgsReadFromCurrentQ = 0;
            }
        }

        if (status == L7_SUCCESS)
        {
          msgsReadFromCurrentQ++;
          lastQ = currentQ;
          if (msgsReadFromCurrentQ >= p_thread->QueueWeights[currentQ])
          {
            if (debugQueues > 0)
            {
              printf("\n%llu: Task %#lx read %d msgs from q%d",
                     osapiTimeMillisecondsGet64(),
                     p_thread->TaskID, msgsReadFromCurrentQ, currentQ);
            }
            currentQ++;
            currentQ %= p_thread->numQueues;
            queuesRead++;
            msgsReadFromCurrentQ = 0;
          }
#ifdef DCB_XX_DEBUG
            printf("_handleTask: received p_info: %p\n", p_info);
#endif /* DCB_XX_DEBUG */

            if (debugQueues > 1)
            {
              char currFn[MAX_FUNCNAME];
              char unpackFnName[MAX_FUNCNAME];
              memcpy(currFn, p_info->funcName, MAX_FUNCNAME);
              currFn[MAX_FUNCNAME-1] = 0;
              printf("\r\nmsg queued by %s:%d calls %s(0x%x)",
                     currFn, (int)p_info->lineNum,
                     fptr2name(p_info->unpackFunc, unpackFnName, MAX_FUNCNAME),
                     (int)p_info->unpackFunc);
            }

            if (debugQueues > 0)
            {
              execStart = osapiTimeMillisecondsGet();
            }
            XX_CALL_THREAD(p_info, f_Thread);

            if ((debugQueues > 0) && ((osapiTimeMillisecondsGetOffset(execStart)) > 1000))
            {
              char currFn[MAX_FUNCNAME];
              char unpackFnName[MAX_FUNCNAME];
              memcpy(currFn, p_info->funcName, MAX_FUNCNAME);
              currFn[MAX_FUNCNAME-1] = 0;
              printf("\r\n%llu: msg queued on queue %d by %s:%d calls %s(0x%x) with exec time %u",
                     osapiTimeMillisecondsGet64(),
                     lastQ, currFn, (int)p_info->lineNum,
                     fptr2name(p_info->unpackFunc, unpackFnName, MAX_FUNCNAME),
                     (int)p_info->unpackFunc, osapiTimeMillisecondsGetOffset(execStart));
            }

            /* free the XX_Call parameter block */
            if (p_info != NULLP)
            {
                XX_Free(p_info);
            }
        }
#ifdef DCB_XX_DEBUG
        else
        {
            printf("_handleTask: f_Thread: %p, p_thread %p,  could not receive msg on any queue!\n",
                   f_Thread, p_thread);
        }
#endif /* DCB_XX_DEBUG */
    }
}

/* Permanent thread handler */
void _handlePermTask(e_Err (*f_Thread)(void *), OS_Thread *p_thread)
{
    void    *p_info;
    L7_RC_t status;

#ifdef DCB_XX_DEBUG
    printf("_handlePermTask: f_Thread: %p, p_thread: %p\n",
           f_Thread, p_thread);
#endif /* DCB_XX_DEBUG */

    for (;;)
    {
        p_info = NULLP;

        /*
        Permanent threads can have only one queue, and they will
        not wait on a semaphore as there is no task that will ever
        give the semaphore
        */

        /*
        Get a message from queue 0
        */

        status = osapiMessageReceive((void *) p_thread->QueueID[0],
                                     (void *) &p_info, OS_MSG_SIZE,
                                     L7_NO_WAIT);
        if (status == L7_SUCCESS)
        {
            /* There is a meesage queued to be processed */
            XX_CALL_THREAD(p_info, 0);
        }
        else
        {
            /*
            Permanent tasks will not be waiting on a semaphore, and
            they only read from one queue, so don't let a permanent
            task run away with the processor
            */

            osapiTaskDelay(1);

            /* Call the default thread handler function, if there
             * is no message queued */
            if (f_Thread)
               (void) (*f_Thread) (p_info);
        }

        /* free the XX_Call parameter block */
        if (p_info != NULLP)
        {
            XX_Free(p_info);
        }
   }
}
