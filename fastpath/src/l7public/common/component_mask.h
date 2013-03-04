
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename component_mask.h
*
* @purpose component mask data structures and defines
*
* @component system
*
* @comments none
*
* @create 07/29/2003
*
* @author wjacobs
*
* @end
*             
**********************************************************************/

/*************************************************************
                    
*************************************************************/


#ifndef COMPONENT_MASK_H
#define COMPONENT_MASK_H

/*-----------------------*/
/* Component Mask Macros */
/*-----------------------*/


/* Number of bytes in mask */
#define COMPONENT_INDICES   ((L7_LAST_COMPONENT_ID - 1) / (sizeof(L7_uchar8) * 8) + 1)

/* Component Mask Storage */
typedef struct {
        L7_uchar8 value[COMPONENT_INDICES];
} COMPONENT_MASK_t;


typedef COMPONENT_MASK_t ComponentMask; 
                                        


/*
 * NONZEROMASK returns true if any bit in word mask of NUM length
 * is turned-on.  The result, TRUE or FALSE is stored in
 * result.
 */
#define COMPONENT_NONZEROMASK(mask, result){                          \
    L7_uint32 _i_;                                                    \
    COMPONENT_MASK_t *_p_;                                            \
                                                                      \
    _p_ = (COMPONENT_MASK_t *)&(mask);                                \
    for(_i_ = 0; _i_ < COMPONENT_INDICES; _i_++)                      \
        if(_p_ -> value[_i_]){                                        \
            result = L7_TRUE;                                         \
            break;                                                    \
        }                                                             \
        else                                                          \
            result = L7_FALSE;                                        \
}


#define COMPONENT_MASKNUMBITSETGET(mask, result){                          \
    L7_uint32 _i_;                                                    \
    COMPONENT_MASK_t *_p_;                                            \
                                                                      \
    _p_ = (COMPONENT_MASK_t *)&(mask);result=0;                       \
    for(_i_ = 0; _i_ < COMPONENT_INDICES; _i_++)                      \
        if(_p_ -> value[_i_]){                                        \
            result++;                                         \
            break;                                                    \
        }                                                             \
}


/* Least significant bit/rightmost bit is lowest interface # */

/* SETMASKBIT turns on bit index # k in mask j. */
#define COMPONENT_SETMASKBIT(j, k)                                      \
            ((j).value[((k-1)/(8*sizeof(L7_uchar8)))]                   \
                         |= 1 << ((k-1) % (8*sizeof(L7_uchar8))))   



/* CLRMASKBIT turns off bit index # k in mask j. */
#define COMPONENT_CLRMASKBIT(j, k)                                      \
           ((j).value[((k-1)/(8*sizeof(L7_uchar8)))]                    \
                        &= ~(1 << ((k-1) % (8*sizeof(L7_uchar8)))))      
            

/* ISMASKBITSET returns 0 if the interface k is not set in mask j */
#define COMPONENT_ISMASKBITSET(j, k)                                    \
        ((j).value[((k-1)/(8*sizeof(L7_uchar8)))]                       \
                         & ( 1 << ((k-1) % (8*sizeof(L7_char8)))) )  





/*--------------------------------------------*/
/*   COMPONENT ACQUISITION MACROS             */
/*
     Used for managing masks of components 
     which have acquired an interface
*/
/*--------------------------------------------*/

/* Number of bytes in mask */
#define COMPONENT_ACQ_INDICES COMPONENT_INDICES} 
#define COMPONENT_ACQUIRED_MASK_t  COMPONENT_MASK_t


typedef COMPONENT_ACQUIRED_MASK_t AcquiredMask;  /*  Mask of components which have 
                                                    "acquired" an interface */

#define COMPONENT_ACQ_NONZEROMASK  COMPONENT_NONZEROMASK 
#define COMPONENT_ACQ_SETMASKBIT  COMPONENT_SETMASKBIT   
#define COMPONENT_ACQ_CLRMASKBIT COMPONENT_CLRMASKBIT
#define COMPONENT_ACQ_ISMASKBITSET  COMPONENT_ISMASKBITSET

  
#endif /* COMPONENT_MASK_H */

