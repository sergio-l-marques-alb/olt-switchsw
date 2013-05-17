/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename            buffer.h
 *
 * @purpose             Buffer management internal structures
 *
 * @component           Routing Utils Component
 *
 * @comments
 *
 * @create              01/09/1993
 *
 *
 * @author              Jonathan Masel
 *                      Igor Bryskin
 *
 * @end
 *
 * ********************************************************************/
#ifndef _BUFFER_H
#define _BUFFER_H

#include "std.h"
#include "local.h"
#include "xx.ext"

typedef struct tagt_Buffer
{
   struct tagt_Buffer *next;    /* next buffer in the frame's chain      */
   struct tagt_Buffer *prev;    /* previous buffer in the frame's chain  */
   t_Handle  datPool;           /* memory pool handle for data           */
   t_Handle  bufPool;           /* memory pool buffer was allocated from */
#ifdef ERRCHK
   XX_Status status;            /* status : valid/invalid                */
#endif
   byte       useCount;         /* use counter                           */
   Bool       fFull;            /* if TRUE, buffer is full               */
   byte      *data;             /* pointer to the buffer's data          */
   word       head;             /* start index                           */
   word       tail;             /* end index                             */
   word       size;             /* total buffer size                     */
} t_Buffer;

#define MAX_REWRAP_INPLACE_SEG  16

#endif  /*_BUFFER_H */
