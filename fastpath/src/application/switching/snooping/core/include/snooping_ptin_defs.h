/*
 * snooping_ptin_defs.h
 *
 *  Created on: 23 de Jul de 2012
 *      Author: Daniel Figueira
 */

#ifndef SNOOPING_PTIN_DEFS_H_
#define SNOOPING_PTIN_DEFS_H_

#define PTIN_IS_MASKBITSET(array,idx) ((array[(idx)/(sizeof(L7_uint32)*8)] >> ((idx)%(sizeof(L7_uint32)*8))) & 1)
#define PTIN_SET_MASKBIT(array,idx)   { array[(idx)/(sizeof(L7_uint32)*8)] |=   (L7_uint32) 1 << ((idx)%(sizeof(L7_uint32)*8)) ; }
#define PTIN_UNSET_MASKBIT(array,idx) { array[(idx)/(sizeof(L7_uint32)*8)] &= ~((L7_uint32) 1 << ((idx)%(sizeof(L7_uint32)*8))); }

#endif /* SNOOPING_PTIN_DEFS_H_ */
