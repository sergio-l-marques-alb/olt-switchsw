/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 * *********************************************************************
 *
 * @filename           tuning.h
 *
 * @purpose            tuning parameters file
 *
 * @component
 *
 * @comments
 *                    This file is included by local.h.
 *                    It contains parameters that may be set for tuning of the routing system.
 *                    They do not necessarily have to be changed from their default
 *                    values (if you have enough memory in your system),
 *                    but this may be desirable for finer tuning.

 *
 * @create
 *
 * @author
 *
 * @end
 *
 * ********************************************************************/

#ifndef tuning_h
#define tuning_h

/* AUTOL7-BEGINNING-OF-CONFIGURATION   please do not remove this comment */


/******************** Buffer pool tuning parameters ****************/
/* This global (default) buffer pool is created during global L7 */
/* initialization if B_BUFFER_NUM != 0 - otherwise the pool is not */
/* created and all data buffers are allocated from L7's dynamic  */
/* heap. B_BUFFER_ALGN - data buffer alignment requirement should  */
/* be chosen depending on specific drivers used by application.    */
/* See below some recommendation for this choice.                  */
/* Improperly chosen B_BUFFER_ALGN may cause failure of compilation*/
/* of some specific driver.                                        */
/* In most cases this buffer data pool is used as actually transmit*/
/* buffer pool only but some drivers use this pool for Rx buffers  */
/* allocation as well.                                             */
/*******************************************************************/
/* Some driver/application specific recommendations:               */
/* --------------------------------------------------              */
/* MPC860/360 ATM drivers (ATOM1 based) do not require allocation  */
/*  of global data buffer pool, i.e. dynamic heap could be used    */
/*  for all buffer allocations and therefore B_BUFFER_NUM = 0      */
/*  should be chosen (in pure ATM application).                    */
/*                                                                 */
/* MPC860SAR ATM driver needs B_BUFFER_ALGN = 16 (at least) because*/ 
/* its Rx buffers (which require this alignment) are allocated from*/ 
/* the same global data buffer pool.                               */
/*                                                                 */
/* If global data buffer pool is used for Rx buffers allocation for*/
/* MPC860/360 HDLC drivers (i.e. its handle is passed as           */
/* RxPartition parameter of PHY_Init) B_BUFFER_ALGN + B_OFFSET     */
/* should be aligned to 4.                                         */
/*******************************************************************/

/*
 * Reserve this number of bytes at the start of
 * each new data buffer (for adding protocol headers)
 */
#ifndef B_OFFSET 
#define B_OFFSET             0
#endif

/*
 * Reserve this number of bytes at the tail of each allocated
 * data buffer and do not use it. This reservation is needed by
 * some specific drivers (for example by SAR8260 ATM driver) for
 * putting there chip specific stuff (CPI & UUI values of AAL5 trailer)
 */
#ifndef B_TRAILER
#define B_TRAILER            2
#endif

/*
 * Physical buffer size (in bytes).
 * Even if global data buffer pool is not created 
 * (when B_BUFFER_NUM = 0) this constant is used as
 * default data buffer size when new data buffer is allocated
 * without explicit size provided.
 */
#ifndef B_BUFFER_SIZE
#define B_BUFFER_SIZE       480
#endif

/*
 * Number of data buffers in global data buffer pool -
 * if equals 0, global data buffers pool is not created.
 * This appears to only be used in FASTPATH for ARP messages.
 * With an ARP Request flood into the box, only
 * a single buffer was used from the pool. So 100 seems safe.
 */
#ifndef B_BUFFER_NUM
  #define B_BUFFER_NUM        100
#endif

/*
 * Data buffer alignment requirement.
 * If a number of drivers are used in system simultaneously
 * the strongest alignment requirement should be chosen.
 */
#ifndef B_BUFFER_ALGN
#define B_BUFFER_ALGN       16
#endif


#if (B_BUFFER_SIZE < 48) || (B_BUFFER_SIZE < B_OFFSET)
#error Illegal B_OFFSET or B_BUFFER_SIZE
#endif

/******************** Cell pool tuning parameters ******************/
/* This global (default) cell pool is created during global L7   */
/* initialization if (C_TXNUM and C_RXNUM) != 0 otherwise the pool */
/* is created by proper ATM driver. The pool is allocated only if: */
/*   L7_ATM is defined and C_TXNUM > 0 and C_RXNUM > 0           */
/* C_ALIGN - cell data buffer alignment requirement should         */
/* be chosen depending on specific drivers used by application.    */
/* See below some recommendation for this choice.                  */
/* Improperly chosen C_ALIGN may cause failure of compilation      */
/* of some specific driver.                                        */
/*******************************************************************/
/* Some driver/application specific recommendations:               */
/* -------------------------------------------------               */
/* MPC860/360 ATM drivers (ATOM1 based) allocate data buffer pool  */
/*  in first call to AP_Init therefore both C_TXNUM and C_RXNUM    */
/*  must be chosen = 0 (otherwise driver compilation fails)        */
/*                                                                 */
/* MPC860SAR ATM driver needs C_ALGN = 16 and C_BSIZE = 64         */
/* BUT it does not transmit cells longer than 52 bytes !           */
/*                                                                 */
/*******************************************************************/

/*
 * Cell's data buffer size (in bytes).
 * Even if global cell data buffer pool is not created 
 * (when C_TXNUM = 0 and C_RXNUM = 0) this constant is used as
 * cell data buffer size when cell pool is allocated by specific
 * ATM driver (during first call to AP_Init).
 */
#ifndef C_BSIZE
#define C_BSIZE      72
#endif

/*
 * C_TXNUM is the number of cell objects (i.e. cell data buffers connected
 * to cell object structure in the system).
 * C_RXNUM is number of cell data buffers used exclusively by
 * driver for Rx cell data buffer allocation and not connected to cell
 * objects. Total number of allocated cell data buffers = C_TXNUM + C_RXNUM
 */
#ifndef C_TXNUM
#define C_TXNUM      300
#endif

#ifndef C_RXNUM
#define C_RXNUM      300
#endif


/*
 * Cell data buffer alignment requirement.
 */
#ifndef C_ALIGN
#define C_ALIGN       32
#endif

#if C_ALIGN == 0
#error "C_ALIGN can not be 0"
#endif

/*
 * Number of bytes reserved in the beginning of cell buffer
 * Some devices require some room to be reserved in the beginning of cell buffer.
 * If C_RESERVED is set - cell buffers are allocated of size C_RESERVED+C_BSIZE
 * and C_RESERVED bytes in the beginning of cell buffer are reserved.
 * C_RESERVED must be proportional to C_ALIGN
 */
#ifndef C_RESERVED
#define C_RESERVED    0
#endif
#if (C_ALIGN > 0) && ((C_RESERVED % C_ALIGN) != 0)
#error "C_RESERVED must be proprtional to C_ALIGN"
#endif

/*
 * The HDLC-based protocols (frame relay, DXI) support
 * several levels of priority at the driver level.
 * These are the relative priorities at which frames are
 * presented to the HDLC controller for transmission.
 * They affect the order in which the frames are stored in
 * the driver's internal queue (i.e. before they are pased
 * to the chip). Priority 0 is the highest priority and
 * ascending numbers correspond to descending priority values.
 * A connection has a default priority and each frame may
 * in addition specify a different priority level
 */
#ifndef MAX_HDLC_PRI
#define MAX_HDLC_PRI     3    /* support four levels of priority (0-3) */
#endif


/******************************************
  Router (used to route connections
  of all types - frame relay, ATM, ...)
 ******************************************/

/*
 * A router object is required in switches where data
 * is received on one connection and transmitted
 * on another (using the same protocol). A separate router
 * object is used for each swicthed connection.
 * The following parameter is the number of pre-allocated
 * router objects (i.e. allocated at compile time).
 *
 * If you are not implementing a switch (FR to FR, or
 * ATM to ATM), or you wish to allocate all memory for the router
 * dynamically, you may set the following parameter to 0.
 */
#ifndef MAX_PREALLOC_RT
#define MAX_PREALLOC_RT      10
#endif


/*
 * a multi-cast group is a connection whose received data
 * is forwarded to a number of destination connections, using
 * a limited broadcast facility.
 * The following parameter defines the total number of
 * multi-cast groups supported in the system.
 * It may be set to 0 if you are not implementing a switch,
 * or do not support multi-cast functions.
 */
#ifndef MAX_MULTI
#define MAX_MULTI            8
#endif



/******************************************
  Funnels
 ******************************************/

/* number of preallocated funnel structures */
#ifndef MAX_PREALLOC_FUNNEL
#define MAX_PREALLOC_FUNNEL       8
#endif

/*
 * maximum funnel depth
 * Each funnel created may store up to this number
 * of frames inside while controlling data rates
 */
#ifndef FUNNEL_DEPTH
#define FUNNEL_DEPTH              16
#endif

/*
 * info rate sampling window width
 * All information rates are measured in a sliding window.
 * The package collects a number of measurements and their
 * sum is the current info rate. Each time interval, the
 * window slides. This parameter defines the number of
 * periods in the window: the larger the number, the more
 * accurate the info rate measurements are, but the more
 * memory is required to implement them
 */
#ifndef MAX_SAMPLING_WINDOW
#define MAX_SAMPLING_WINDOW       4
#endif


/******************************************
  Linked lists
 ******************************************/

/* number of preallocated linked lists data structures */
#ifndef MAX_PREALLOC_LL
#define MAX_PREALLOC_LL           100
#endif


/******************************************
  Timing trace
 ******************************************/
/* Number of records in default timetrc buffer
 * See xxtrace.ext for details
 * XX_TRACE_GET_TIMESTAMP in xxtrace.ext can also
 * require tuning
 */
#ifndef TIMETRC_BNUM
#define TIMETRC_BNUM              50
#endif


/******************************************
  Frames and buffers free lists.
  Define the following symbols to 1 if you
  want to have free lists for frame and/or
  buffer objects. In this case once allocated
  frames/buffers are returned to these lists
  and next allocations go a little faster.
  This optimization is not supported if 
  frames are emulated vua ATMOS CYAN buffers
 ******************************************/
#ifndef FRMOBJ_FREELIST
#define FRMOBJ_FREELIST          0
#endif

#ifndef BUFOBJ_FREELIST
#define BUFOBJ_FREELIST          0
#endif



/* AUTOL7-END-OF-CONFIGURATION   please do not remove this comment */

#endif

/* --- end of file tuning.loc --- */


