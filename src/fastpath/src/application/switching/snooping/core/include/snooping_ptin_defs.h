/*
 * snooping_ptin_defs.h
 *
 *  Created on: 23 de Jul de 2012
 *      Author: Daniel Figueira
 */

#ifndef SNOOPING_PTIN_DEFS_H_
#define SNOOPING_PTIN_DEFS_H_

#define PTIN_CLEAR_ARRAY(array)         memset((array),0x00,sizeof(array))
#define PTIN_IS_MASKBITSET(array,idx) ((array[(idx)/(sizeof(L7_uint32)*8)] >> ((idx)%(sizeof(L7_uint32)*8))) & 1)
#define PTIN_SET_MASKBIT(array,idx)   { array[(idx)/(sizeof(L7_uint32)*8)] |=   (L7_uint32) 1 << ((idx)%(sizeof(L7_uint32)*8)) ; }
#define PTIN_UNSET_MASKBIT(array,idx) { array[(idx)/(sizeof(L7_uint32)*8)] &= ~((L7_uint32) 1 << ((idx)%(sizeof(L7_uint32)*8))); }

#define PTIN_PROXY_NONZEROMASK(array, result)                              \
{                                                                    \
    L7_int64 _i_;                                                \
    result=-1;                                                       \
    for(_i_ = 0; _i_ < sizeof(array)/sizeof(L7_uint32); _i_++)       \
        if(array[_i_] != 0)                                          \
        {                                                            \
            result = _i_;                                            \
            break;                                                   \
        }                                                            \  
}

#if SNOOP_PTIN_IGMPv3_PROXY
#define PTIN_PROXY_IS_MASKBITSET(array,idx)   ((array[(idx)/(sizeof(L7_uint8)*8)] >> ((idx)%(sizeof(L7_uint8)*8))) & 1)
#define PTIN_PROXY_SET_MASKBIT(array,idx)     { array[(idx)/(sizeof(L7_uint8)*8)] |=   ((L7_uint8) 1 << ((idx)%(sizeof(L7_uint8)*8)) ; }
#define PTIN_PROXY_UNSET_MASKBIT(array,idx)   { array[(idx)/(sizeof(L7_uint8)*8)] &= ~(((L7_uint8) 1 << ((idx)%(sizeof(L7_uint8)*8))); }
#endif

#endif /* SNOOPING_PTIN_DEFS_H_ */
