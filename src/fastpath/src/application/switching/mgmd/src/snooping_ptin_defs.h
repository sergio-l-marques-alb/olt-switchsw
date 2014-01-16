/*
 * snooping_ptin_defs.h
 *
 *  Created on: 23 de Jul de 2012
 *      Author: Daniel Figueira
 */

#ifndef SNOOPING_PTIN_DEFS_H_
#define SNOOPING_PTIN_DEFS_H_

#define PTIN_MGMD_CLEAR_ARRAY(array)         memset((array),0x00,sizeof(array))

#define PTIN_MGMD_IS_MASKBITSET(array,idx) ((array[(idx)/(sizeof(uint8)*8)] >> ((idx)%(sizeof(uint8)*8))) & 1)
#define PTIN_MGMD_SET_MASKBIT(array,idx)   { array[(idx)/(sizeof(uint8)*8)] |=   (uint8) 1 << ((idx)%(sizeof(uint8)*8)) ; }
#define PTIN_MGMD_UNSET_MASKBIT(array,idx) { array[(idx)/(sizeof(uint8)*8)] &= ~((uint8) 1 << ((idx)%(sizeof(uint8)*8))); }

#define PTIN_MGMD_CLIENT_NONZEROMASK(array, result)                \
{                                                                  \
    uint32 _i_;                                                    \
    result=-1;                                                     \
    for(_i_ = 0; _i_ < sizeof(array)/sizeof(uint8); ++_i_)         \
        if(array[_i_] != 0)                                        \
        {                                                          \
            result = _i_;                                          \
            break;                                                 \
        }                                                          \
}

#endif /* SNOOPING_PTIN_DEFS_H_ */
