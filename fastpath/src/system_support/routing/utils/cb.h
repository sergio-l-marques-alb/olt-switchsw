/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename         cb.h
 *
 * @purpose          Internal definitions for the CB object
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
#ifndef cb_h
#define cb_h


/*
 * the definition of the circular buffer
 */
typedef struct
{
   word    Size;       /* size in bytes of the buffer */
   word    Count;      /* number of bytes in circular buffer */
   ulng    GetCount;   /* total number of bytes got from buffer */
   ulng    PutCount;   /* total number of bytes put to buffer */
   ulng    PutFailures;   /* number of bytes we failed to put in buffer */
   t_Mutex Mutex;      /* for multi-threaded environments */
   byte   *p_Start;    /* base address of buffer */
   byte   *p_End;      /* last address of buffer + 1 */

   byte   *p_Put;      /* put pointer: next address for adding data */
   byte   *p_Get;      /* get pointer: next address for reading data */

} t_CircularBuff;

#endif

/* --- end of file cb.h --- */
