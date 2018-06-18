 /********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename       ll.h
 *
 * @purpose        Linked list management internal structures
 *
 * @component      Routing Utils Component
 *
 * @comments
 *
 * @create         01/09/93
 *
 * @author         Jonathan Masel
 *
 * @end
 *
 * ********************************************************************/
#ifndef _ll_h
#define _ll_h

#include "std.h"

/* flow control object */
typedef struct
{
   t_Handle Owner;                       /* owner for flow control           */
   word     CurrLevel;                   /* current congestion level         */
   word     MaxLevel;                    /* maximum congestion level         */
   ulng     CurrCount;                   /* current resource count           */
   word     (*f_Resources)(void *);      /* calculates resources in object   */
   void     (*f_CongInd)(t_Handle, word);/* congestion indication call-back  */

   ulng     CongLevels[1];              /* congestion levels - fake size (MUST BE LAST) */
                                        /*  the levels are arranged in pairs : */
                                        /*  clear_level_1,set_level_1,         */
                                        /*  clear_level_2,set_level_2,...      */
                                        /*  set_level_N-1 < set_level_N        */
                                        /*  clear_level_N <= set_level_N       */
                                        /*  clear_level_N >= set_level_N-1     */
} t_LL_FlowControl;

typedef struct
{
   Bool     Taken;           /* this element is taken (for prealloc)   */
   Bool     Prealloc;        /* this element is in the prealloc list   */
   word     NextOffset;      /* Offset of "next" field in application object */
   word     Max;             /* maximum size of list (limit)           */
   word     Count;           /* number of elements currently in list   */
   word     MaxIn;           /* max reached number of elements in list */
   word     ReadShiftCount;  /* running counter of READ pointer changes*/
                             /* performed as result of calling LL_Get  */
   ulng     PutFailures;     /* number of failed put operations        */
   void     *First;          /* pointer to first element in list       */
   void     *Last;           /* pointer to last element in list        */
   void     *Read;           /* current reading position               */

   t_LL_FlowControl *p_FC;   /* flow control object's handle           */
} t_LL;

#endif /* _ll_h */

/* --- end of file ll.h --- */

