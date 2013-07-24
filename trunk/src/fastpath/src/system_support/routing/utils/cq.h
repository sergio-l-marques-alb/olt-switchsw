/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename          cq.h
 *
 * @purpose           Internal definitions for the circular-queue object
 *
 * @component         Routing Utils Component
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
#ifndef cq_h
#define cq_h

/* dynamically allocated receive frame queue */
typedef struct
{
   int      Max;             /* size of queue */
   int      PutFailures;     /* number of failed put operations */
   int      MaxIn;           /* maximum items in queue */
   volatile int First;       /* index of first in queue */
   volatile int Last;        /* index after last in queue */
   volatile int Read;        /* current reading position */
   void    *Table[1];        /* fake size */
} t_CQ;

#endif

/* --- end of file cq.h --- */

