/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename     iptypes.h
 *
 * @purpose      IP definitions file
 *
 * @component    Routing Utils Component
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
#include <std.h>

#ifndef L7_iptypes_definitions
#define L7_iptypes_definitions

#include "xx.ext"
#include "timer.ext"
#include "frame.ext"

#define BGP_TCP_PORT  179
#define L2TP_UDP_PORT 1701

/* ***********************************************************

     Network Field order to/from converting macro definitions

   *********************************************************** */

#define A_SET_4B(var, p_field)                       \
     (*(p_field)      = (byte) (((var) >> 24) & 0xffL),  \
     *((p_field) + 1) = (byte) (((var) >> 16) & 0xffL),  \
     *((p_field) + 2) = (byte) (((var) >>  8) & 0xffL),  \
     *((p_field) + 3) = (byte)  ((var)        & 0xffL))

#define A_SET_3B(var, p_field)                       \
     (*(p_field)      = (byte) (((var) >> 16) & 0xffL),  \
     *((p_field) + 1) = (byte) (((var) >>  8) & 0xffL),  \
     *((p_field) + 2) = (byte)  ((var)        & 0xffL))

#define A_SET_2B(var, p_field)                       \
     ((*(p_field)      = (byte) (((var) >>  8) & 0xffL)),\
     (*((p_field) + 1) = (byte)  ((var)        & 0xffL)))

#define A_GET_2B(p_field) ((((int) *(p_field)) << 8) | *((p_field) + 1))

#define A_GET_3B(p_field)   \
      ((((ulng) *(p_field))       << 16) |  \
       (((ulng) *((p_field) + 1)) <<  8) |  \
        ((ulng) *((p_field) + 2)))

#define A_GET_4B(p_field)   \
      ((((ulng) *(p_field))       << 24) |  \
       (((ulng) *((p_field) + 1)) << 16) |  \
       (((ulng) *((p_field) + 2)) <<  8) |  \
        ((ulng) *((p_field) + 3)))

#define A_GET_2W(p_field) (((ulng)((word) *(p_field)) << 16) | (ulng)*(((word*)p_field) + 1))

#define A_SET_2W(var, p_field)                       \
     ((*(p_field)      = (word) (((var) >>  16) & 0xffffL)),\
     (*((p_field) + 1) = (word)  ((var)         & 0xffffL)))

/* Timer jittering random fractional variance 10% and 25% */
#define TimerFractVar10(val) ((word)((val < 5) ?  val : (((long) val)*9)/10 + ((((long) val)/5)*rand())/RAND_MAX))
#define TimerFractVar25(val) ((word)((val < 2) ?  val : (((long) val)*3)/4  + ((((long) val)/2)*rand())/RAND_MAX))

/* Get time interval from the time */
#define GET_INTERVAL(time)                                         \
   (TIMER_SysTime() >= (time) ?                                    \
    TICKS_TO_SECONDS(TIMER_SysTime() - (time)) :                   \
    TICKS_TO_SECONDS(TIMER_SysTime() + ((0xffffffffL - (time)) + 1)))

/* Get pointer to the packet's header (if packet is a BIG-TAIL) */
#define GET_PCK_HDR(p_F) ((byte *) B_GetData(F_GetFirstBuf(p_F)))

/* SNMP V2 TC standard: MIB ROW commands */
typedef enum e_A_RowStatus
{
   /* SNMP V2 TC standard */
   ROW_ACTIVE = 1,      /* - in call:   activate the object                        */
                        /*   in return: the object is active                       */
   ROW_NOT_IN_SERVICE,  /* - in call:   deactivate the object                      */
                        /* - in return: the object is not active                   */
   ROW_NOT_READY,       /* - in return only: the object not ready                  */
   ROW_CREATE_AND_GO,   /* - in call only: init and run the object                 */
   ROW_CREATE_AND_WAIT, /* - in call only: init the object                         */
   ROW_DESTROY,         /* - in call only: delete the object                       */
   /*  Specific */  /*                                                         */
   ROW_READ,            /* - in call only: read the current/first object's element */
   ROW_READ_NEXT,       /* - in call only: read the next object's element          */
   ROW_CHANGE,          /* - in call only: modify the object's element             */
   ROW_MODIFY           /* - internal use only: the object's element modified internally */
}  e_A_RowStatus;

/* Macro for IP addresses comparision */
#define IPADR_CMP(a1,a2) memcmp((a1),(a2),sizeof(t_IPAddr))
/* Macro for IP addresses copy */
#define IPADR_COPY(a1,a2) memcpy((a1),(a2),sizeof(t_IPAddr))
/* Macro for IP addresses reset */
#define IPADR_RESET(a) memset((a),0,sizeof(t_IPAddr))
/* Macro for IP addresses null check */
#define IPADR_EMPTY(a) (A_GET_4B(a) ? FALSE : TRUE)

/* alternative definition of an IP address */
typedef ulng SP_IPADR;

#define GET_PREFIX_LEN(prl) (prl ? ((prl)/8 + (((prl) % 8) ? 1 : 0)) : 0)

#define GET_PREFIX_BITS(prl) ((0xFF00 >> ((prl)%8))&0xFF)

#define PREFIX_COMP(spr,dpr,prl) \
         (memcmp((spr), (dpr), (prl)/8) && (((prl)%8 == 0) ||       \
         ((*((byte*)(spr)+((prl)/8)+1) & GET_PREFIX_BITS(prl)) ==    \
         (*((byte*)(dpr)+((prl)/8)+1) & GET_PREFIX_BITS(prl)))))

#define SET_PREFIX_MASK(prl,prm) \
{\
   prm = 0;      \
   if(prl < 8)   \
      prm = GET_PREFIX_BITS(prl) << 24;\
   else if(prl < 16)   \
      prm = 0xFF000000L | GET_PREFIX_BITS(prl) << 16;\
   else if(prl < 24)   \
      prm = 0xFFFF0000L | GET_PREFIX_BITS(prl) << 8;\
   else if(prl < 32)   \
      prm = 0xFFFFFF00L | GET_PREFIX_BITS(prl);\
   else      \
      prm = 0xFFFFFFFFL;\
}
#define GET_PREFIX_FROM_MASK(prm, prl)\
{\
   ulng mask = prm;\
   \
   prl = 0;\
   while(mask & 0x80000000L)\
   {\
      prl += 1;\
      mask <<= 1;\
   }\
}

#endif /* L7_iptypes_definitions */

/*******************end of the file*********************/
