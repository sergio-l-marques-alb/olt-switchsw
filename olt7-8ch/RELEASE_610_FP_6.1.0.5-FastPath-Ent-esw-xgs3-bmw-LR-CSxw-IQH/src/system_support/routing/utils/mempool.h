/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename    mempool.h
 *
 * @purpose     Memory pool management internal structures
 *
 * @component   Routing Utils Component
 *
 * @comments
 *
 * @create      01/09/1993
 *
 * @author      Jonathan Masel
 *              Igor Bryskin
 *
 * @end
 *
 * ********************************************************************/
#ifndef _MEMPOOL_H
#define _MEMPOOL_H

#include "local.h"
#include "xx.ext"

/* Memory pool info */
typedef struct tagt_MemPoolInfo
{
   struct tagt_MemPoolInfo *next;  /* chaining element                    */
   struct tagt_MemPoolInfo *prev;
   t_Name  Name;                   /* pool's name                         */
#ifdef ERRCHK
   XX_Status status;               /* status : valid/invalid              */
#endif
   t_Handle  osPoolHandle;         /* OS assigned pool handle             */
   void     *startAddress;         /* start address of the pool           */
   word      bufferNmb;            /* total number of buffers in the pool */
   word      buffersFree;          /* current number of free buffers      */
   word      minBuffersFree;       /* low water mark */
   word      bufferSize;           /* size of the every pool's buffer     */
   word      trailerSize;          /* size of reserved trailer in buffer  */
   word      dataOffset;           /* offset to start of data buffer      */
   word      align;                /* alignment of data buffers in pool   */
   void     *bufferList;           /* buffer list                         */
   Bool      markDel;              /* pool is marked to be deleted */

   Bool      poolState;            /* IsEmpty / IsNotEmpty                */
   word      thresholdDown;        /* pool becames to be empty event - down threshold */
   word      thresholdUp;          /* pool returns back to be active - up threshold */
   t_Handle  userId;               /* empty event notificator user id */
   void (*f_UsrCallback)(t_Handle UserId, Bool ThreshCrossedDown); /* empty event callback */

} t_MemPoolInfo;

#endif  /*_MEMPOOL_H */
