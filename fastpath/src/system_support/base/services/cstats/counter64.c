/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename    counter64.c
 * @purpose     API's for 64bit  operations
 * @component   stats
 * @comments    none
 * @create      18/06/2007
 * @author      YSaritha
 * @end
 *
 **********************************************************************/

#include "commdefs.h"           /* lvl7 common definitions   */
#include "datatypes.h"          /* lvl7 data types definition */
#include "osapi.h"              /* lvl7 operating system apis */
#include "statsapi.h"           /* stats public methods */
#include "counter64.h"
#include "collector.h"
#include "presenter.h"
#include "statsconfig.h"
#include "log.h"

L7_ulong64 counter64_assign (L7_uint32 op1, L7_uint32 op2)
{
  L7_ulong64 result;

  result.low = op2;
  result.high = op1;
  return result;
}

/*
 **********************************************************************
 *
 * @function     operator+
 *
 * @purpose      This function allows the client to add two 64 bit counters
 *
 * @param        Counter64          c
 *
 * @returns      Counter64      Returns the counter after the addition of the
 *                              specified 64 bit counter
 *
 * @end
 *
 **********************************************************************
 */

L7_ulong64 counter64_add (L7_ulong64 op1, L7_ulong64 op2)
{
  L7_uint32 tmp = 0;
  L7_ulong64 result;
  result.low = result.high = 0;

  if (op1.low > (op1.low + op2.low))
  {
    tmp = 1;
  }
  result = counter64_assign ((op1.high + tmp) + op2.high, op1.low + op2.low);

  return result;
}

/*
 **********************************************************************
 *
 * @function     operator+=
 *
 * @purpose      This function allows the client to add two 64 bit counters
 *
 * @param        Counter64          c
 *
 * @returns      Counter64      Returns the counter after the addition of the
 *                              specified 64 bit counter
 *
 * @end
 *
 **********************************************************************
 */

/********************************************************************
 *
 * @function         operator-
 *
 * @purpose          This function allows the client to subtract one 64 bit counter
 *                   from another
 *
 * @param            Counter64          c
 *
 * @returns          Counter64      Returns the counter after the subtraction of the
 *                                  specified 64 bit counter
 *
 * @end

 *
 **********************************************************************
 */

L7_ulong64 counter64_sub (L7_ulong64 op1, L7_ulong64 op2)
{
  L7_uint32 tmp = 0;
  L7_ulong64 result;
  result.low = result.high = 0;

  if (op1.low < op2.low)
  {
    tmp = 1;
  }
  result = counter64_assign ((op1.high - tmp) - op2.high, op1.low - op2.low);
  return result;
}

/*
 **********************************************************************
 *
 * @function         operator-=
 *
 * @purpose          This function allows the client to subtract one 64 bit counter
 *                   from another
 *
 * @param            Counter64          c
 *
 * @returns          Counter64      Returns the counter after the subtraction of the
 *                                  specified 64 bit counter
 *
 * @end
 *
 **********************************************************************
 */

/*
 **********************************************************************
 *
 * @function         operator++
 *
 * @purpose          This function allows the client increment a 64 bit counter
 *                   by 1
 *
 * @param            Counter64          c
 *
 * @returns          Counter64     Returns the counter after incrementing its value
 *
 * @end
 **********************************************************************
 */

L7_ulong64 counter64_incr (L7_ulong64 op)
{
  if ((op.low + 1) < op.low)
  {
    op.high++;
  }
  op.low++;
  return op;
}

/*
 **********************************************************************
 *
 * @function         operator++
 *
 * @purpose          This function allows the client decrement a 64 bit counter
 *                   by 1
 *
 * @param            Counter64          c
 *
 * @returns          Counter64      Returns the counter after decrementing its value
 *
 * @end
 **********************************************************************
 */

L7_ulong64 counter64_decr (L7_ulong64 op)
{
  if ((op.low - 1) > op.low)
  {
    op.high--;
  }
  op.low--;
  return op;
}

/*
 **********************************************************************
 *
 * @function         operator==
 *
 * @purpose          This function allows the client to compare two 64 bit counters
 *
 * @param            Counter64          c
 *
 * @returns          Counter64      Returns the true if this counter is equal to the
 *                                  specified counter
 *
 * @end
 *
 **********************************************************************
 */

L7_ushort16 counter64_equals (L7_ulong64 op1, L7_ulong64 op2)
{
  if (op1.high == op2.high)
  {
    return (op1.low == op2.low) ? 1 : 0;
  }
  else
  {
    return 0;
  }
}

/*
 **********************************************************************
 *
 * @function         operator!=
 *
 * @purpose          This function allows the client to compare two 64 bit counters
 *
 * @param            Counter64          c
 *
 * @returns          Counter64      Returns the true if this counter is equal to the
 *                                  specified counter
 *
 * @end
 *
 **********************************************************************
 */

L7_ushort16 counter64_notequals (L7_ulong64 op1, L7_ulong64 op2)
{
  return op1.high != op2.high || op1.low != op2.low;
}

L7_ushort16 counter64_notintequals (L7_ulong64 op1, L7_uint32 op2)
{
  return op1.low != op2;
}

/*
 **********************************************************************
 *
 * @function     operator>
 *
 * @purpose      This function allows the client to compare two 64 bit counters
 *
 * @param        Counter64          c
 *
 * @returns      Counter64      Returns the true if this counter is greater than
 *                              the specified counter
 *
 * @end
 *
 **********************************************************************
 */

L7_ushort16 counter64_greater (L7_ulong64 op1, L7_ulong64 op2)
{
  if (op1.high == op2.high)
  {
    return (op1.low > op2.low) ? 1 : 0;
  }
  else
  {
    return (op1.high > op2.high) ? 1 : 0;
  }
}

/*
 **********************************************************************
 *
 * @function         operator>=
 *
 * @purpose          This function allows the client to compare two 64 bit counters
 *
 * @param            Counter64          c
 *
 * @returns          Counter64     Returns the true if this counter is greater than
 *                                 or equal to the specified counter
 *
 * @end
 *
 **********************************************************************
 */

L7_ushort16 counter64_greater_equal (L7_ulong64 op1, L7_ulong64 op2)
{

  if (op1.high == op2.high)
  {
    return (op1.low >= op2.low) ? 1 : 0;
  }
  else
  {
    return (op1.high >= op2.high) ? 1 : 0;
  }
}

/*
 **********************************************************************
 *
 * @function         operator<
 *
 * @purpose          This function allows the client to compare two 64 bit counters
 *
 * @param            Counter64          c
 *
 * @returns          Counter64     Returns the true if this counter is lesser than
 *                                 the specified counter
 *
 * @end
 *
 **********************************************************************
 */

L7_ushort16 counter64_less (L7_ulong64 op1, L7_ulong64 op2)
{
  if (op1.high == op2.high)
  {
    return (op1.low < op2.low) ? 1 : 0;
  }
  else
  {
    return (op1.high < op2.high) ? 1 : 0;
  }
}

/*
 **********************************************************************
 *
 * @function         operator<=
 *
 * @purpose          This function allows the client to compare two 64 bit counters
 *
 * @param            Counter64          c
 *
 * @returns          Counter64      Returns the true if this counter is lesser than
 *                                  or equal to the specified counter
 *
 * @end
 *
 **********************************************************************
 */

L7_ushort16 counter64_less_equal (L7_ulong64 op1, L7_ulong64 op2)
{
  if (op1.high == op2.high)
  {
    return (op1.low <= op2.low) ? 1 : 0;
  }
  else
  {
    return (op1.high <= op2.high) ? 1 : 0;
  }

}
