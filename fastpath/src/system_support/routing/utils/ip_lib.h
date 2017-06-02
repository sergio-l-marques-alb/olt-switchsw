/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 * *********************************************************************
 *
 * @filename           ip_lib.h
 *
 * @purpose            IP library header file
 *
 * @component          Routing IP Component
 *
 * @comments           moved to system_support/routing/utils and stripped down
 *                     to just what we use. (WRR June 08)
 *
 * @create
 *
 * @author
 *
 * @end
 *
 * ********************************************************************/
#include <std.h>

#ifndef l7_sim_iplib_h
#define l7_sim_iplib_h



/*********************************************************************
 * @purpose         Calculates the sum of array as a 16 bit integers
 *
 *
 * @param data      @b{(input)}  data array pointer
 * @param length    @b{(input)}  data array length
 * @param startSum  @b{(input)}  initial sum
 *
 * @returns         sum itself
 *
 * @notes
 *
 * @end
 * ********************************************************************/
ulng IPLIB_Sum(IN byte *data, IN word length, ulng startSum);


/*********************************************************************
 * @purpose         Calculates checksum of array as a one complement of the
 *                  16 bit integers sum
 *
 *
 * @param data      @b{(input)}   data array pointer
 * @param length    @b{(input)} data array length
 * @param startSum  @b{(input)} initial sum
 *
 * @returns         checksum itself
 *
 * @notes
 *
 * @end
 * ********************************************************************/
word IPLIB_Checksum(IN byte *data, IN word length, IN ulng startSum);

#endif
/*******************end of the file*********************/
