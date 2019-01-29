/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    counter64.h
* @purpose     API's for 64 bit operations
* @component   stats
* @comments    
* @create      18/06/2007
* @author      YSaritha 
* @end
*             
**********************************************************************/

#ifndef INCLUDE_COUNTER64_H
#define INCLUDE_COUNTER64_H

#include "datatypes.h"

L7_ulong64 counter64_assign (L7_uint32, L7_uint32);
L7_ulong64 counter64_add (L7_ulong64, L7_ulong64);
L7_ulong64 counter64_sub (L7_ulong64, L7_ulong64);
L7_ulong64 counter64_incr (L7_ulong64);
L7_ulong64 counter64_decr (L7_ulong64);
L7_ushort16 counter64_equals (L7_ulong64, L7_ulong64);
L7_ushort16 counter64_notequals (L7_ulong64, L7_ulong64);
L7_ushort16 counter64_notintequals (L7_ulong64, L7_uint32);
L7_ushort16 counter64_greater (L7_ulong64, L7_ulong64);
L7_ushort16 counter64_greater_equal (L7_ulong64, L7_ulong64);
L7_ushort16 counter64_less (L7_ulong64, L7_ulong64);
L7_ushort16 counter64_less_equal (L7_ulong64, L7_ulong64);
#endif
