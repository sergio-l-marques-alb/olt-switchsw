/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 * *********************************************************************
 *
 * @filename           local.h
 *
 * @purpose            This file contains local parameters that may be
 *                     tuned for a specific application
 *
 * @component
 *
 * @comments
 *
 * ********************************************************************/

#ifndef local_h
#define local_h



/* List of predefined OS symbols used further for RTOS selection
   0 = no operating system
   2 = VxWorks
   9 = Linux
 */
#define NO_RTOS       0
#define VXWORKS_RTOS  2
#define LINUX_RTOS    9
/*--- List of predefined core CPU symbols used further for L7_CORE_CPU selection ----*/
#define PCSIM     0     /* PC-based simulation    */
#define TRICORE   1     /* TriCore (Infineon Tech)*/
#define PPC_CORE  2     /* PPC Core: e.g. MPC750  */
#define MC302     302   /* MC68302                */
#define MC360     360   /* MC360                  */
#define MPC860    860   /* MPC860                 */
#define MPC8260   8260  /* MPC8260                */
#define IDT4650   4650  /* IDT 4650 MIPS          */
#define VRT_HELIUM 1000 /* Helium          */


/* AUTOL7-BEGINNING-OF-CONFIGURATION   please do not remove this comment */

/*****************************************************************
  Protocol and other compile time selections

  The following defines are used to reduce the size of the
  compiled code by excluding modules that are not required.
  In all cases, set the flag of the flag to 0 to exclude a module
  from compilation and 1 to include it.
  All flags may be set externally via compiler switches.
 *****************************************************************/


/*--------------------Switch library module---------------------*/

#if !defined(L7_SWLIB)
  #define L7_SWLIB        0   /* Switch library module        */
#endif

/*--------------------------------------------------------------*/


/*-------------------  RPS modules--------------------------------------------*/
#if !defined L7_RPS
  #define L7_RPS              0       /* LVL7_MOD (was 1) */
#endif

/*------------------L7 REFORMAT-------------------------------------------------*/
#if !defined L7_REFORMAT
  #define L7_REFORMAT         1
#endif


/*-------------------MPLS  modules-----------------------------------------------*/
#if !defined L7_MPLS
  #define L7_MPLS        0     /* MPLS  */  /* LVL7_MOD_split (was 1) */
#endif

#if L7_MPLS

  #if !defined(EXTERN_DPLANE)
    #define EXTERN_DPLANE    1     /* external (usually in hardware) data plane is used */
  #endif

  #if L7_RPS

    #if !defined MPLSCTRL_EXTERNAL
      #define MPLSCTRL_EXTERNAL     0
    #endif

    #if !defined FIB_EXTERNAL
      #define FIB_EXTERNAL          0
    #endif

    #if !defined SIGN_EXTERNAL
      #define SIGN_EXTERNAL         0
    #endif

    #if !defined L2_EXTERNAL
      #define L2_EXTERNAL           0
    #endif


    #if !defined LSP_RPS_BY_USER_STACK
      #define LSP_RPS_BY_USER_STACK 1
    #endif

  #endif /* L7_RPS */

#endif /* L7_MPLS */

/*--------------------- Network Interface integration modules -------------------*/
#ifndef L7_NI
  #define L7_NI        0
#endif

/* For VxWorks we have 2 implementations for L7_NI: SENS fashion and END fashion */
/* If you want to choose the END fashion implementation define L7_END as 1       */
#if L7_NI
  #ifndef L7_END
    #define L7_END       0
  #endif
#endif



/*-------------------------------------------------------------------------------*/



/*--------------------- MIB support modules -------------------------------------*/
#ifndef L7MIB_MIB2
  #define L7MIB_MIB2      1
#endif

#if L7MIB_MIB2

  #ifndef L7MIB_FRDCE
    #define L7MIB_FRDCE     0
  #endif

  #ifndef L7MIB_FRDTE
    #define L7MIB_FRDTE     0
  #endif

  #ifndef L7MIB_ATOM
    #define L7MIB_ATOM      0
  #endif

  #if L7MIB_ATOM
    #ifndef L7MIB_RFC2515
      #define L7MIB_RFC2515   0
    #endif
  #endif

  #ifndef L7MIB_ILMI
    #define L7MIB_ILMI      0
  #endif

  #ifndef L7MIB_CES
    #define L7MIB_CES       0
  #endif

  #ifndef L7MIB_MPLS
    #define L7MIB_MPLS      0   /* LSR and LDP MIBs */                  /* LVL7_MOD (was 1) */
  #endif

  #ifndef L7MIB_MPLS_LSR
    #define L7MIB_MPLS_LSR  0   /* LSR  MIBs */                         /* LVL7_MOD (was 1) */
  #endif

  #ifndef L7MIB_RSVP
    #define L7MIB_RSVP      0    /* LVL7_MOD (was 1) */
  #endif

  #ifndef L7MIB_MPLS_FTN
    #define L7MIB_MPLS_FTN  0   /* MPLS FTN (Packet Classifier) MIB */  /* LVL7_MOD (was 1) */
  #endif

  #ifndef L7MIB_MPLS_TE
    #define L7MIB_MPLS_TE   0  /* MPLS traffic Engineering MIB */       /* LVL7_MOD (was 1) */
  #endif

  #ifndef L7_MIB_OSPF
    #define L7_MIB_OSPF      1   /* OSPF MIB */
  #endif

  #ifndef L7_MIB_OSPFV3
    #define L7_MIB_OSPFV3    1   /* OSPFV3 MIB */
  #endif

#endif
/*-------------------------------------------------------------------------------*/


/*--------------------- Tunneling protocols modules -----------------------------*/
#if !defined(L7_L2TP)
  #define L7_L2TP        0          /* L2TP tunneling protocol  */
#endif


#if !defined(L7_PPTP)
  #define L7_PPTP        0          /* PPTP tunneling protocol */
#endif


/*-------------------------------------------------------------------------------*/


/*--------------------- Routing protocols modules -----------------------------*/

/* Broadcom has not tested MOSPF or OSPF TE. The code is an unexercised vestige from 
 * a former vendor. */
#if !defined(L7_MOSPF)
    #define L7_MOSPF        0          /* Multicast Extension to the OSPF routing protocol  */
#endif

#if !defined(L7_OSPF_TE)
    #define L7_OSPF_TE      0          /* Traffic Engineering support under OSPF */
#endif


#if !defined(ATIC_BGP)
  #define ATIC_BGP          1          /* BGP routing protocol */     /* LVL7_MOD_split (was 1) */
  #define ATIC_BGP_FLAP     1
  #define ATICMIB_BGP       1          /* by Anand SSV */
#endif

#if !defined(L7_IGMP)
  #define L7_IGMP         0          /* IGMP routing protocal */    /* LVL7_MOD_split (was 1) */
#endif

#if L7_MPLS || ATIC_BGP
  #ifndef ATIC_POLICY
    #define ATIC_POLICY       1          /* POLICY module support */  /* LVL7_MOD_split (was 1) */
  #endif
#endif


/*-------------------------------------------------------------------------------*/

/*--------------------- Atic monitor interface support --------------------------*/
#if !defined(L7_MONITOR)
  #define L7_MONITOR      0          /* Monitor interface support  */
#endif



/*--------------------------- OS environment definitions ------------------------*/

/*------- Select your operating system using one of predefined OS symbols -------*/
#if !defined(RTOS)

    #ifdef _L7_OS_VXWORKS_
        #define RTOS VXWORKS_RTOS
    #endif

    #ifdef _L7_OS_LINUX_
        #define RTOS  LINUX_RTOS
    #endif

    #ifdef _L7_OS_ECOS_
        #define RTOS  LINUX_RTOS
    #endif

#endif


#if !defined(MULTI_PROCESSOR)
  #define MULTI_PROCESSOR  0      /* 1 - multi-processor environment */
#endif

#if !(MULTI_PROCESSOR && L7_MPLS) && MPLSCTRL_EXTERNAL
  #error "MULTI_PROCESSOR" and "L7_MPLS" must be set if "MPLSCTRL_EXTERNAL" is set
#endif

#if !(MULTI_PROCESSOR && L7_MPLS) && FIB_EXTERNAL
  #error "MULTI_PROCESSOR" and "L7_MPLS" must be set if "FIB_EXTERNAL" is set
#endif

#if !(MULTI_PROCESSOR && L7_MPLS) && SIGN_EXTERNAL
  #error "MULTI_PROCESSOR" and "L7_MPLS" must be set if "SIGN_EXTERNAL" is set
#endif

#if !(MULTI_PROCESSOR && L7_MPLS) && L2_EXTERNAL
  #error "MULTI_PROCESSOR" and "L7_MPLS" must be set if "L2_EXTERNAL" is set
#endif

/*
 * number of hardware timer ticks (interrupts)
 * per second
 * Default is 10 (i.e. 100 milli each tick)
 */
#ifdef _L7_OS_VXWORKS_
    #ifndef TICKS_PER_SECOND
        extern int sysClkRateGet (void);
        #define TICKS_PER_SECOND sysClkRateGet()
    #endif
#endif

#ifdef _L7_OS_LINUX_
    #ifndef TICKS_PER_SECOND
        #define TICKS_PER_SECOND    100       /* LVL7_MOD (was 10) */
    #endif
#endif

#ifdef _L7_OS_ECOS_
    #ifndef TICKS_PER_SECOND
        #define TICKS_PER_SECOND    100       /* LVL7_MOD (was 10) */
    #endif
#endif

/*
 * creates a default timer task that is activated by the OS.
 * The following define determines the period (in terms of the number
 * of OS ticks) with which the default timer is activated.
 * For example, if the OS tick timer has a period of 10 milliseconds and
 * the timer needs a period of 100 mill (TICKS_PER_SECOND=10), then set
 * OS_TICKS to 10.
 */
#ifndef OS_TICKS
  #define OS_TICKS  10          /* LVL7_MOD (was 1) */
#endif


/*
 * Few protocols (ILMI, BGP, IPOA, MPLS/LDP, OSPF, IGMP) and RTSIM use
 * DEFAULT_TIMER_THREAD for some timer expiration processing.
 * Following symbol allows to choose priority for this thread or
 * cancel its creation (if = 0) at all. If you don't use above
 * protocols you can set it 0. Typically, if used, this priority
 * should be minimal priority from all standard threads, but higher than
 * priority of any permanent thread and any foreign polling permanent
 * task in system.
 */
#ifndef DEFAULT_TIMER_THREAD_PRIORITY
  #define DEFAULT_TIMER_THREAD_PRIORITY  254
#endif


/*
 * BCLOCK - Basic Clock object has an interface similar to the TIMER object.
 * BCLOCK can be driven by a hardware timer. It provides precise timing for
 * short intervals. This object is optional.
 */
#ifndef L7_BCLOCK
  #define L7_BCLOCK 0
#endif

#if L7_BCLOCK
/*
 * Number of BCLOCK ticks in one second. Should be set according with the
 * period of the timer driving the Basic Clock object.
 * For example, BC_TICKS_PER_SECOND is set to 200, if the driving timer
 * period is 5 milliseconds (1000 milliseconds / 200).
 */
  #ifndef BC_TICKS_PER_SECOND
    #define BC_TICKS_PER_SECOND   200
  #endif

/*
 * Choose, whether the Basic Clock Timer is internal or external
 */
  #ifndef L7_INTERNAL_BCLOCK
    #define L7_INTERNAL_BCLOCK  0
  #endif

/*
 * The Basic Clock is hardware dependent. It may occupy less then the
 * sizeof(ulng) bytes. The mask is used to handle the wrap around.
 * For example, if the BCLOCK timer has 16 bits the mask is 0xffff.
 */
  #ifndef BCLOCK_MASK
    #define BCLOCK_MASK 0xffffffff
  #endif

#endif /* L7_BCLOCK */

/*-------------------------------------------------------------------------------*/
/* The following constant should match pointer size on
 * the given h/w / s/w platform.
 */
#ifndef POINTER_SIZE
  #define POINTER_SIZE       4
#endif

#ifndef L7_CORE_CPU
  #define L7_CORE_CPU   PCSIM
#endif

/*
 * set the endian mode of the target processor
 * choose BIG_ENDIAN 0 for little endian mode
 */
#ifdef _L7_OS_VXWORKS_
    #if !defined(BIG_ENDIAN)
        #define BIG_ENDIAN   1                /* LVL7_MOD (was 0) */
    #endif
#endif

#ifdef _L7_OS_LINUX_
    #include <endian.h>
    #if __BYTE_ORDER == BIG_ENDIAN
        #if defined(BIG_ENDIAN)
            #undef BIG_ENDIAN
        #endif
        #define BIG_ENDIAN 1
    #else
        #if defined(BIG_ENDIAN)
            #undef BIG_ENDIAN
        #endif
        #define BIG_ENDIAN 0
    #endif
#endif

#ifndef ALIGN
  #define ALIGN        4    /* default alignment in memory allocations */
#endif


/*
 * periferal controllers
 * A few controllers might be included simultaneously
 */

#ifndef L7_CPM_MC360
  #define L7_CPM_MC360        0
#endif

#if L7_CPM_MC360
  #ifndef L7_UART_MC360
    #define L7_UART_MC360       0
  #endif
#endif

#ifndef L7_CPM_MPC860
  #define L7_CPM_MPC860       0
#endif

#ifndef L7_CPM_MPC850
  #define L7_CPM_MPC850       0
#endif

#ifndef L7_CPM_MPC860SAR
  #define L7_CPM_MPC860SAR    0
#endif

#ifndef L7_CPM_MPC850SAR
  #define L7_CPM_MPC850SAR    0
#endif

#ifndef L7_CPM_MPC860T
  #define L7_CPM_MPC860T      0
#endif

#ifndef L7_CPM_MPC860P
  #define L7_CPM_MPC860P      0
#endif

#ifndef L7_CPM_MPC8260
  #define L7_CPM_MPC8260      0
#endif

#ifndef L7_CPM_MC92500
  #define L7_CPM_MC92500      0
#endif

#ifndef L7_CPM_IDT77201
  #define L7_CPM_IDT77201     0
#endif

#ifndef L7_CPM_IDT77211
  #define L7_CPM_IDT77211     0
#endif

#ifndef L7_CPM_IDT77252
  #define L7_CPM_IDT77252     0
#endif

#ifndef L7_CPM_IDT77222
  #define L7_CPM_IDT77222     0
#endif

/* ATMOS/Cyan-based device */
#ifndef L7_CPM_ATMOS
  #define L7_CPM_ATMOS        0
#endif

#ifndef L7_CPM_VIRTUAL
  #define L7_CPM_VIRTUAL      0
#endif



/*----------------- Debugging options -------------------------------------------
 *
 * L7_DEBUG - set the debug level for modules as follows:
 *    0 - production code - no tracers
 *    1 - access cell/frame/buffer data structures via routines (not macros)
 *        generate calls to error tracer
 *    2 - generate calls to event tracer
 *    3 - generate calls to trace routines for all primitives
 *
 * LACKMEMCHK - define following symbol as 1,2 or 3 if you want run-time lack of memory checks
 *    0 - no run time checks of lack of memory
 *    1 - crash on lack of memory
 *    2 - only print out a message on lack of memory
 *    3 - only issue EV_NO_MEMORY event on lack of memory
 *
 * DO_FILEID_IN_MEMORY - define this to include file name and version
 *    information in each ".c" file in memory.
 *    If defined, a static character array named
 *    __fileid is created in each source file of
 *    the format "Name Revision Date Author".
 *    If undefined, no variable will be compiled in
 *    memory, but the revision information can still be
 *    read from the source file.
 */


#ifndef DO_ERRCHK
  #define DO_ERRCHK  1          /* run-time error checks */
#endif

#ifndef DO_FRMCHK
  #define DO_FRMCHK  0          /* frame leak checking */
#endif

#ifndef DO_MEMCHK
  #define DO_MEMCHK  0          /* memory leak checking */
#endif

#ifndef DO_MATCHK
  #define DO_MATCHK  1          /* match tables leak checking */
#endif

#ifndef DO_FILEID_IN_MEMORY
  #define DO_FILEID_IN_MEMORY  0
#endif

/*-------------------------------------------------------------------------------*/


/* AUTOL7-END-OF-CONFIGURATION   please do not remove this comment */


/***** DO NOT TOUCH BELOW - internal definitions and checks - DO NOT TOUCH BELOW ******/

#define L7_NETSIM 0

#if L7_NETSIM

  #define L7_DECODE 0
  #define L7_ERRINJ 0

  #if !L7_ABR
    #error "L7_ABR should  be set to 1 if Netsim is included"
  #endif

  #if !L7_ILMI
    #error "L7_ILMI should  be set to 1 if Netsim is included"
  #endif

  #if !L7_LANE_LNNI
    #error "L7_LANE_LNNI should  be set to 1 if Netsim is included"
  #endif

  #if !L7_LANE_CLIENT
    #error "L7_LANE_CLIENT should  be set to 1 if Netsim is included"
  #endif

  #if !IP_CTRLEVNT_API
    #error "IP_CTRLEVNT_API should  be set to 1 if Netsim is included"
  #endif

  #if !IP_SVCEVNT_API
    #error "IP_SVCEVNT_API should  be set to 1 if Netsim is included"
  #endif

  #if !IP_ARP_EVENT_API
    #error "IP_ARP_EVENT_API should  be set to 1 if Netsim is included"
  #endif

  #if !IP_FAST_PATH_API
    #error "IP_FAST_PATH_API should  be set to 1 if Netsim is included"
  #endif

  #if !L7_L2TP
    #error "L7_L2TP should  be set to 1 if Netsim is included"
  #endif

  #if !L7_PPTP
    #error "L7_PPTP should  be set to 1 if Netsim is included"
  #endif

  #if RTOS != WINDOWS_RTOS
    #error "RTOS should  define WINDOWS_RTOS "
  #endif

  #if (TICKS_PER_SECOND != 10)
    #error "TICKS_PER_SECOND should define  10"
  #endif

  #if BIG_ENDIAN
    #error "BIG_ENDIAN should  Not be set to 1 if Netsim is included"
  #endif

#endif

#if ALIGN == 0
  #error ALIGN could not be 0
#endif

#if (L7_CPM_MPC860 || L7_CPM_MPC850 || L7_CPM_MPC850SAR || L7_CPM_MPC860SAR) && (L7_CORE_CPU != 860)
  #error No 860 slave mode exists - "L7_CORE_CPU" must be set to 860
#endif

#if (L7_CPM_MPC860 + L7_CPM_MPC850 + L7_CPM_MPC850SAR + L7_CPM_MPC860SAR) > 1
  #error Only one from "L7_CPM_MPC860", "L7_CPM_MPC850", "L7_CPM_MPC850SAR", "L7_CPM_MPC860SAR" could be set
#endif

#if (RTOS == NO_RTOS)
  #error In "NO_RTOS" environment only internal task manager may be used
#endif

#define L7_LEC  0

#define L7_LNNI 0

#define L7_MPC 0

#define L7_MPS 0
#define L7_NHS 0

#if !defined(L7_SGNL)
  #define L7_SGNL
#endif

#if DO_ERRCHK
  #define ERRCHK
#endif

#if DO_FRMCHK
  #define FRMCHK
#endif

#if DO_MEMCHK
  #define MEMCHK
#endif

#if DO_MATCHK
  #define MATCHK
#endif

#if DO_FILEID_IN_MEMORY
  #define FILEID_IN_MEMORY
#endif


/********************************************/
/* include low-level local files */
#include "tuning.h"

/*****************************************************************/
/* Include low-level headers based on the selected configuration */
/*****************************************************************/
#if (L7_CORE_CPU == MPC860)
  #include "drv860.h"
#endif

#if L7_CPM_MPC8260  || (L7_CORE_CPU == MPC8260)
  #include "drv8260.h"
#endif

#if L7_CPM_MC360  || (L7_CORE_CPU == MC360)
  #include "drv360.h"
#endif

#if (L7_CORE_CPU == MC302)
  #include "drv302.h"
#endif

#if L7_CPM_IDT77252 || L7_CPM_IDT77222 || L7_CPM_IDT77211 || L7_CPM_IDT77201
  #include "dridtsar.h"
#endif

#endif

/* --- end of file local.h --- */
