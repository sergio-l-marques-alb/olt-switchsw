/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename          funnel.h
 *
 * @purpose           Definitions for funnels
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
#ifndef funnel_h
#define funnel_h



/*
 * each frame stored within the funnel is referenced
 * by a t_FunnelEntry data structure
 */
typedef struct
{
   t_Handle Owner;          /* the owner of the frame */
   void    *p_Frame;        /* pointer to a frame in the buffer */
   ulng     Length;         /* length of frame (in bytes) */
   word     CongBits;       /* congestion status bits as in frame's dlci */
} t_FunnelEntry;


/*
 * information rates are monitored by the
 * following data structure
 */
typedef struct
{
   word     Now;            /* current position in sampling window */

/* current info rate values */
   ulng     Curr;           /* current info rate */
   ulng     Max;            /* maximum allowed info rate */

/* info rate sampling window */
   ulng     Window[MAX_SAMPLING_WINDOW];
} t_InfoRate;


typedef e_Err (*F_Handler)(t_Handle Id, void *p_Frame, word CongBits);

/*
 * funnel data structure
 */
typedef struct Funnel
{
   Bool     Prealloc;       /* taken from preallocated pool        */
   Bool     Taken;          /* funnel is currently in use          */
   Bool     Blocked;        /* funnel is blocked to CongestedGauge */
   Bool     Congested;      /* funnel is congested                 */
   Bool     ExtCongestion;  /* external congestion(port congested) */
   word     Links;          /* number of connections               */
                            /*   to which funnel is linked         */
   t_Handle QueueId;        /* for communicating with timer        */
   t_Mutex  Mutex;          /* this funnel's mutex                 */

   /* pointer to pointer to handler routine of owner */
   F_Handler *p_f_Handler;

   /* flow control */
   ulng     BuffsToBusy;    /* busy when this many buffers in funnel  */
   ulng     BuffsToClear;   /* clear when this many buffers in funnel */
   /* owner's busy indication/clearing routine */
   e_Err (* f_Busy)(t_Handle Id, word Cong);

   /* gauge control */
   ulng     InitialGauge;   /* initial info rate */
   ulng     CurrentGauge;   /* currently allowed info rate */
   ulng     CongestedGauge; /* gauge to use during congestion */
   ulng     Step;           /* used in changing gauge size */
   ulng     Bandwidth;      /* bandwidth consumption (Committed+Excess) */

   /* information rate monitoring */
   word     MeasurementInterval; /* measurement interval time counter */
   t_Handle Timer;          /* handle to timer used by this funnel */
   t_InfoRate  Committed;   /* committed info data rate measurements */
   t_InfoRate  Excess;      /* excess info data rate measurements */

   /* frame buffering within the funnel */
   word     Frames;         /* current number of frames in funnel */
   word     Buffs;          /* current number of buffers in funnel */
   word     Head;           /* index of first frame in funnel */
   word     Tail;           /* index of last frame in funnel */
   ulng     Depth;          /* current number of bytes in funnel */
   ulng     MaxDepth;       /* maximum number of bytes allowed in funnel */

   /* internal buffer for frames */
   t_FunnelEntry Entry[FUNNEL_DEPTH+1];  /* for storing frames in funnel */

} t_Funnel;


#endif

/* --- end of file funnel.h --- */
