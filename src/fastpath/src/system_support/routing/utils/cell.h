/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename     cell.ext
 *
 * @purpose      External prototypes for cell object
 *
 * @component    Routing Utils Component
 *
 * @comments
 *
 * @create       01/02/95
 *
 * @author
 *
 * @end                                                                        
 *
 * ********************************************************************/
#ifndef cell_h
#define cell_h

#include "std.h"
#include "local.h"


typedef struct
{
   void  *p_Next;       /* MUST BE FIRST                          */
   byte  *p_Buffer;     /* address of data buffer (C_BSIZE bytes) */
   void  *Owner;        /* user field                             */
   word   First;        /* index of first byte in buffer          */
   word   Length;       /* number of bytes in buffer              */
   word   Flags;        /* cell related flags                     */
} t_Cell;


#endif

/* --- end of file cell.h --- */

