 /********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 * *********************************************************************
 *
 * @filename          ip_lib.c
 *
 * @purpose           provides IP libraries
 *
 * @component         Routing IP Component
 *
 * @comment
 *      EXTERNAL API calls:
 *       IPLIB_Sum
 *       IPLIB_Checksum
 *
 *              Moved to system_support/routing/utils and trimmed to just the 
 *              functions we use. (wrr, June 08)
 *
 * @create     4/30/1999
 *
 * @author     Alex Ossinski
 *
 * @end
 *
 * ********************************************************************/
#include <stddef.h>

#include "ip_lib.h"


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
ulng IPLIB_Sum(IN byte *data, IN word length, ulng startSum)
{
    for (; length > 1; data += 2, length -= 2)
        startSum += (data[0] << 8) + data[1];

    if (length > 0)
        startSum += (data[0] << 8);
    return startSum;
}

/*********************************************************************
 * @purpose     Calculates checksum of array as a one complement of the
 *              16 bit integers sum
 *
 *
 * @param p_Frame      @b{(input)}  data frame pointer
 *
 * @returns            checksum itself
 *
 * @notes
 *
 * @end
 * ********************************************************************/
word IPLIB_Checksum(IN byte *data, IN word length, IN ulng startSum)
{
    ulng sum;

    sum = IPLIB_Sum(data, length, startSum);
    sum = (sum >> 16) + (sum & 0xffff);
    sum = (sum >> 16) + (sum & 0xffff);
    return (word)~sum;
}

/********************end of the file*********************/
