/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename     routing_init.c
 *
 * @purpose    Global interface routines.
 *
 * @component  Routing Utils Component
 *
 * @comments   External Routines:
 *                     routingInit    initialize everything
 *
 * @create
 *
 * @author
 * @end
 *
 * ********************************************************************/
#ifdef FILEID_IN_MEMORY
static char __fileid[] = "$Header: lvl7dev\src\system_support\routing\utils\routing_init.c 1.1.3.1 2002/02/13 23:05:09EST ssaraswatula Development  $";
#endif


/* --- standard include files --- */
#include "std.h"
#include "atmtypes.h"


/* --- external object interfaces --- */

#include "xx.ext"
#include "mem.ext"
#include "crc.ext"
#include "ll.ext"
#include "buffer.ext"
#include "frame.ext"
#include "cell.ext"
#include "routing_init.ext"
#include "osapi.h"
#include "log.h"
#include "platform_config.h"

#if DO_TIMETRC
#include "xxtrace.ext"
#endif

#if L7MIB_MIB2
#include "sysmib.h"
#endif

#if L7_MPLS && MPLS_RPS
#include "cm_lay3.ext"
#include "rtm/l7_rtm.ext"
#include "lay2if.ext"
#include "lsp_rps.ext"
#include "mplsctrl/mpls_ctr.ext"
#include "mplsctrl/ctrl_rps.ext"
#include "fib/mpls_fib.ext"
#endif


/*********************************************************************
 * @purpose                 Initialize all modules in the system.
 *
 *
 * @param  priority         @b{(input)}  Priority of the default
 *                                       thread if = 0 - default thread
 *                                       is not created
 * @param  HeapStartAddr    @b{(input)}  Start address for the dynamic
 *                                       heap may be = 0 - i.e. starts anywhere
 * @param  HeapSize         @b{(input)}  Size of the dynamic heap to be
 *                                       allocated
 * @param  f_Init           @b{(input)}  User application initialization
 *                                       function
 *
 * @returns                 error code if failed
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err routingInit(byte priority, void *HeapStartAddr, ulng HeapSize, void (* f_Init)(void))
{
   e_Err  ierror;

   union
   {
      word  Val;
      byte  Bytes[2];
   } x;

   /* Work out if we're big or little endian */
   x.Bytes[0] = 0x12;
   x.Bytes[1] = 0x34;
   if( x.Val == 0x1234 )
   {
      ASSERT (BIG_ENDIAN);
#if !BIG_ENDIAN
      return E_FAILED;
#endif
   }
   else
   {
      ASSERT (!BIG_ENDIAN);
#if BIG_ENDIAN
      return E_FAILED;
#endif
   }

   /* Verify pointer size */
   if (POINTER_SIZE != sizeof( void *))
   {
      ASSERT( !"POINTER_SIZE in local.h is incorrect" );
      return E_FAILED;
   }
   /* Verify t_HandleAsLong (pointer as long) size */
   if (sizeof( t_HandleAsLong ) != sizeof( void *) )
   {
      ASSERT( !"t_HandleAsLong type size in std.h is incorrect" );
      return E_FAILED;
   }

   /* Initialize OS and processor related stuff */
   if( (ierror = OS_XX_InitAll()) != E_OK)
   {
      ASSERT(FALSE);
      return ierror;
   }


   /* Initialize external environment */
   if( (ierror = XX_InitAll( HeapStartAddr, HeapSize)) != E_OK)
   {
      ASSERT(FALSE);
      return ierror;
   }


   /* Initialize objects and create default data buffer pools */

#if B_BUFFER_NUM
   /* Create global default data buffer pool if needed */
   {
      byte     *part_pointer=NULLP;
      t_Handle  buff_pool;


      /* Allocate memory for buffers from  non-cacheable heap           */
      /* Case of ATMOS RTOS is special. In ATMOS environment            */
      /* cyan pool service is utilized and buffers are allocated from   */
      /* ATMOS memory.                                                  */
#if RTOS != ATMOS_RTOS
      /* Try to allocate non cacheable partition for buffers */
      part_pointer = XX_MallocNoCache(MEM_SizeofInitAlign(TRUE, B_BUFFER_NUM,
                             B_BUFFER_SIZE+B_TRAILER, B_BUFFER_ALGN));


      if(!part_pointer)
      {
         ASSERT(FALSE);
         return E_NOMEMORY;
      }
#endif   /* RTOS != ATMOS_RTOS */

      /* Initialize buffer memory pool properly aligned */
      ierror = MEM_InitAlign("BUFF", &buff_pool, part_pointer,
                             B_BUFFER_NUM, B_BUFFER_SIZE+B_TRAILER, B_BUFFER_ALGN );

      if(ierror != E_OK)
      {
         ASSERT(FALSE);
         return ierror;
      }

      if(B_TRAILER)
         MEM_AdjustPoolTrailer(buff_pool, B_TRAILER);

      /* Assign this pool to be used in all external buffer data allocations */
      ierror = F_AssignDefPools ( NULLP, NULLP, buff_pool);

      if(ierror != E_OK)
      {
         ASSERT(FALSE);
         return ierror;
      }
   }
#else   /* #if B_BUFFER_NUM */
#if RTOS == ATMOS_RTOS
   {
      /* In ATMOS environment connect to ATMOS default buffer pool */
      t_Handle buff_pool;

      buff_pool = (t_Handle)cyan_poolFind("Generic");
      if (buff_pool != NULLP)
         F_AssignDefPools ( NULLP, NULLP, buff_pool);
   }
#endif  /* RTOS == ATMOS_RTOS */
#endif  /* #if B_BUFFER_NUM */

#if BUFOBJ_FREELIST
   B_FreeListInit();
#endif

#if FRMOBJ_FREELIST
   F_FreeListInit();
#endif


#if DO_TIMETRC
   /* Initialize timing trace buffer */
   XXTrc_Create( "L7", TIMETRC_BNUM );
#endif

#ifndef L7_DO_NOT_INIT
#if L7MIB_MIB2
   L7MibInit();
#endif

#endif /* #ifndef L7_DO_NOT_INIT */

   /* Create default thread if requested so */
   if(priority)
   {
      t_Handle thread;
      if ( (ierror = XX_CreateThread(priority, NULLP, FALSE, &thread, "Routing Default")) != E_OK)
      {
         ASSERT(FALSE);
         return ierror;
      }
      else
          XX_SetL7_tid(thread);
   }

   if(f_Init)
   {
      f_Init();
      OS_XX_Start();
   }

   return E_OK;
}

L7_RC_t routingStart(void)
{
  e_Err           erc;
  L7_uchar8       *heapStart;
  L7_uint32       heapSize;

  /* Global Routing system environment initialization:
     *   - requests the memory from OS for all dynamic allocations
     *   - all needed base resources are set up
     */
  vendorHeapIpInfoGet(&heapStart, &heapSize);   /* HW-specific outcall */
  if ((erc = routingInit(0, (void *)heapStart, (ulng)heapSize,
                         (void (*)(void))NULLP)) != E_OK)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_IP_MAP_COMPONENT_ID,
            "%s: routingInit failed, rc=%d\n", __FUNCTION__, erc);
    L7_LOG_ERROR(0);
    return erc;
  }
  OS_XX_Start();

  return L7_SUCCESS;
}

/* --- end of file routing_init.c --- */

