
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename portevent_mask.h
*
* @purpose portevent mask data structures and defines
*
* @portevent system
*
* @comments none
*
* @create 11/11/2008
*
* @author bradyr
*
* @end
*             
**********************************************************************/

/*************************************************************
                    
*************************************************************/


#ifndef PORTEVENT_MASK_H
#define PORTEVENT_MASK_H

/*------------------------*/
/* Port Event Mask Macros */
/*------------------------*/


/* Number of bytes in mask */
#define PORTEVENT_INDICES   ((L7_LAST_PORT_EVENT - 1) / (sizeof(L7_uchar8) * 8) + 1)

/* Port Event Mask Storage */
typedef struct {
        L7_uchar8 value[PORTEVENT_INDICES];
} PORTEVENT_MASK_t;


typedef PORTEVENT_MASK_t PortEventMask; 
                                        


/*
 * NONZEROMASK returns true if any bit in word mask of NUM length
 * is turned-on.  The result, TRUE or FALSE is stored in
 * result.
 */
#define PORTEVENT_NONZEROMASK(mask, result){                          \
    L7_uint32 _i_;                                                    \
    PORTEVENT_MASK_t *_p_;                                            \
                                                                      \
    _p_ = (PORTEVENT_MASK_t *)&(mask);                                \
    for(_i_ = 0; _i_ < PORTEVENT_INDICES; _i_++)                      \
        if(_p_ -> value[_i_]){                                        \
            result = L7_TRUE;                                         \
            break;                                                    \
        }                                                             \
        else                                                          \
            result = L7_FALSE;                                        \
}


#define PORTEVENT_MASKNUMBITSETGET(mask, result){                          \
    L7_uint32 _i_;                                                    \
    PORTEVENT_MASK_t *_p_;                                            \
                                                                      \
    _p_ = (PORTEVENT_MASK_t *)&(mask);result=0;                       \
    for(_i_ = 0; _i_ < PORTEVENT_INDICES; _i_++)                      \
        if(_p_ -> value[_i_]){                                        \
            result++;                                         \
            break;                                                    \
        }                                                             \
}


/* Least significant bit/rightmost bit is lowest port event # */

/* SETMASKBIT turns on bit index # k in mask j. */
#define PORTEVENT_SETMASKBIT(j, k)                                      \
            ((j).value[((k)/(8*sizeof(L7_uchar8)))]                   \
                         |= 1 << ((k) % (8*sizeof(L7_uchar8))))   



/* CLRMASKBIT turns off bit index # k in mask j. */
#define PORTEVENT_CLRMASKBIT(j, k)                                      \
           ((j).value[((k)/(8*sizeof(L7_uchar8)))]                    \
                        &= ~(1 << ((k) % (8*sizeof(L7_uchar8)))))      
            

/* ISMASKBITSET returns 0 if the port event k is not set in mask j */
#define PORTEVENT_ISMASKBITSET(j, k)                                    \
        ((j).value[((k)/(8*sizeof(L7_uchar8)))]                       \
                         & ( 1 << ((k) % (8*sizeof(L7_char8)))) )  

#endif /* PORTEVENT_MASK_H */

