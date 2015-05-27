/*
 * snooping_ptin_defs.h
 *
 *  Created on: 23 de Jul de 2012
 *      Author: Daniel Figueira
 */

#ifndef SNOOPING_PTIN_DEFS_H_
#define SNOOPING_PTIN_DEFS_H_

#if SNOOP_PTIN_IGMPv3_PROXY
#define PTIN_PROXY_IS_MASKBITSET(array,idx)   ((array[(idx)/(sizeof(L7_uint8)*8)] >> ((idx)%(sizeof(L7_uint8)*8))) & 1)
#define PTIN_PROXY_SET_MASKBIT(array,idx)     { array[(idx)/(sizeof(L7_uint8)*8)] |=   ((L7_uint8) 1 << ((idx)%(sizeof(L7_uint8)*8)) ; }
#define PTIN_PROXY_UNSET_MASKBIT(array,idx)   { array[(idx)/(sizeof(L7_uint8)*8)] &= ~(((L7_uint8) 1 << ((idx)%(sizeof(L7_uint8)*8))); }
#endif

#endif /* SNOOPING_PTIN_DEFS_H_ */
