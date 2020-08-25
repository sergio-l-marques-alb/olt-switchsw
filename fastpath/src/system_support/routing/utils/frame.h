/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename     frame.h
 *
 * @purpose      Frame management internal structures
 *
 * @component    Routing Utils Component
 *
 * @comments
 *
 * @create       01/09/93
 *
 * @author       Jonathan Masel
 *               Igor Bryskin
 *
 * @end
 *
 * ********************************************************************/
#ifndef _FRAME_H
#define _FRAME_H

#include "std.h"
#include "xx.ext"
#include "buffer.h"
#include "local.h"

#define FRAME_USER_FIELDS_NMB    8

typedef struct tagt_Frame       /* frame structure                             */
{
   struct tagt_Frame *nextFrame;/* next frame in the list of frames            */
   struct tagt_Frame *prevFrame;/* prev frame in the list of frames            */
   t_Buffer *firstBuf;          /* first dynamic buffer in the list of buffers */
   t_Handle  framesPool;        /* memory pool handle frame was allocated from */
   t_Handle  bufPool;           /* memory pool handle for buffer structures    */
   t_Handle  datPool;           /* memory pool handle for data                 */
   word      frameLen;          /* total frame length in bytes                 */
   byte      useCount;          /* current number of frame users               */
   void (*f_ReleaseInd)(void *);/* callback on release notification            */
   void     *relParam;          /* user defined parameter for f_ReleaseInd     */
                                /* = this frame's handle by default            */
#if FRAME_USER_FIELDS_NMB
   t_HandleAsLong frameAttr[FRAME_USER_FIELDS_NMB];
#endif
#ifdef ERRCHK
   XX_Status status;            /* status : valid/invalid                      */
#endif
#ifdef FRMCHK
   word               allocLine;
   word               freeLine;
   const char        *allocFile;
   const char        *freeFile;
   struct tagt_Frame *next;
   struct tagt_Frame *prev;
   word               ownerLine;
   const char        *ownerFile;
#endif
} t_Frame;

#endif  /*_FRAME_H */
