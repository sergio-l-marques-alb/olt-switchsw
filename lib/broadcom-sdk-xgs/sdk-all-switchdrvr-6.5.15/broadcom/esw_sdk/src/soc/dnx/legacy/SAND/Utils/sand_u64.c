/* $Id: sand_u64.c,v 1.9 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/

/* 
 Contains multiple-precision arithmetic code originally written by David Ireland,
 copyright (c) 2001-8 by D.I. Management Services Pty Limited <www.di-mgt.com.au>,
 and is used with permission"
*/ 




#include <shared/bsl.h>
#include <soc/dnx/legacy/drv.h>

#include <soc/dnx/legacy/SAND/Utils/sand_u64.h>
#include <soc/dnx/legacy/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dnx/legacy/SAND/Utils/sand_os_interface.h>

/*****************************************************
 * See details in dnx_sand_u64.h
 *****************************************************/
uint32
  dnx_sand_long_to_u64(
    DNX_SAND_IN  uint32      		u32,
    DNX_SAND_OUT DNX_SAND_U64* 		u64
  )
{
  uint32
    result = TRUE;

  

  if(!u64)
  {
	  result = FALSE;
	  goto exit;
  }

  dnx_sand_u64_clear(u64);

  u64->arr[0] = u32;

exit:
  return result;
}

/*****************************************************
 * See details in dnx_sand_u64.h
 *****************************************************/
uint32
  dnx_sand_u64_copy(
    DNX_SAND_IN  DNX_SAND_U64*      src,
    DNX_SAND_OUT DNX_SAND_U64* 		dst
  )
{
  uint32
	i,
    result = TRUE;

  

  if(!src || !dst)
  {
	  result = FALSE;
	  goto exit;
  }

  for (i = 0; i < DNX_SAND_U64_NOF_UINT32S; i++) {
	  dst->arr[i] = src->arr[i];
  }

exit:
  return result;
}

/*****************************************************
 * See details in dnx_sand_u64.h
 *****************************************************/
void
  dnx_sand_u64_clear(
    DNX_SAND_INOUT DNX_SAND_U64* u64
  )
{
    

  if(NULL == u64)
  {
    goto exit;
  }

  dnx_sand_os_memset(
    u64,
    0x0,
    sizeof(*u64)
  );

exit:
  return;
}

/*****************************************************
 * See details in dnx_sand_u64.h
 *****************************************************/
uint32
  dnx_sand_u64_to_long(
    DNX_SAND_IN  DNX_SAND_U64*      u64,
    DNX_SAND_OUT uint32* ulong
  )
{
  uint32
    overflowed;
  overflowed = FALSE;

  

  if(NULL == u64)
  {
    goto exit;
  }

  if(u64->arr[1] != 0)
  {
    overflowed = TRUE;
    *ulong     = 0xFFFFFFFF;
  }
  else
  {
    *ulong     = u64->arr[0];
  }

exit:
  return overflowed;
}

/*****************************************************
 * See details in dnx_sand_u64.h
 *****************************************************/
void
  dnx_sand_u64_multiply_longs(
    DNX_SAND_IN  uint32 x,
    DNX_SAND_IN  uint32 y,
    DNX_SAND_OUT DNX_SAND_U64*     result
  )
{  /*  Computes result = x * y */
  /*     high    p1                p0     low
        +--------+--------+--------+--------+
        |      x1*y1      |      x0*y0      |
        +--------+--------+--------+--------+
               +-+--------+--------+
               |1| (x0*y1 + x1*y1) |
               +-+--------+--------+
                ^carry from adding (x0*y1+x1*y1) together
                        +-+
                        |1|< carry from adding LOHALF t
                        +-+  to high half of p0
  */
  uint32
    x0,
    y0,
    x1,
    y1;
  uint32
    t,
    u,
    carry;

  

  if(NULL == result)
  {
    goto exit;
  }

  /*  Split each x,y into two halves
    x = x0 + B*x1
    y = y0 + B*y1
    where B = 2^16, half the digit size
    Product is
    xy = x0y0 + B(x0y1 + x1y0) + B^2(x1y1)
  */

  x0 = DNX_SAND_LOHALF(x);
  x1 = DNX_SAND_HIHALF(x);
  y0 = DNX_SAND_LOHALF(y);
  y1 = DNX_SAND_HIHALF(y);

  /* Calc low part - no carry */
  result->arr[0] = x0 * y0;

  /* Calc middle part */
  t = x0 * y1;
  u = x1 * y0;
  t += u;
  if (t < u)
  {
    carry = 1;
  }
  else
  {
    carry = 0;
  }

  /*  This carry will go to high half of result->arr[1]
    + high half of t into low half of result->arr[1] */
  carry = DNX_SAND_TOHIGH(carry) + DNX_SAND_HIHALF(t);

  /* Add low half of t to high half of result->arr[0] */
  t = DNX_SAND_TOHIGH(t);
  result->arr[0] += t;
  if (result->arr[0] < t)
  {
    carry++;
  }

  result->arr[1] = x1 * y1;
  result->arr[1] += carry;

exit:
  return;
}


/* $Id: sand_u64.c,v 1.9 Broadcom SDK $
 * utility function to be use by 'dnx_sand_u64_device_u64_long_high'
 */
static void
  dnx_sand_u64_device_u64_long_high_inner(
    uint32 uu[2],
    uint32 qhat,
    uint32 v1,
    uint32 v0
  )
{
  /*  Compute uu = uu - q(v1v0)
    where uu = u3u2u1u0, u3 = 0
    and u_n, v_n are all half-digits
    even though v1, v2 are passed as full digits.
  */
  uint32 p0, p1, t;

  

  p0 = qhat * v0;
  p1 = qhat * v1;
  t = p0 + DNX_SAND_TOHIGH(DNX_SAND_LOHALF(p1));
  uu[0] -= t;
  if (uu[0] > 0xffffffff - t)
    uu[1]--;  /* Borrow */
  uu[1] -= DNX_SAND_HIHALF(p1);
}


/*****************************************************
*NAME
* dnx_sand_u64_devide_u64_long_high
*TYPE:
*  PROC
*DATE:
*  03/03/2004
*FUNCTION:
*  Devide DNX_SAND_U64 with uint32 and load result into uint32.
*  y -- high bit need to be high. That is (y&0x80000000==0x80000000).
*  result    = x / y
*  remainder = x % y
*INPUT:
*  DNX_SAND_DIRECT:
*    DNX_SAND_IN  DNX_SAND_U64*     x -
*      value to device
*    DNX_SAND_IN  uint32 y -
*      value to device
*    DNX_SAND_OUT uint32 result -
*      Loaded with result
*    DNX_SAND_OUT uint32 remainder -
*      Loaded with remainder
*  DNX_SAND_INDIRECT:
*    None.
*OUTPUT:
*  DNX_SAND_DIRECT:
*    None.
*  DNX_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
static uint32
  dnx_sand_u64_devide_u64_long_high(
    DNX_SAND_IN  DNX_SAND_U64*      x,
    DNX_SAND_IN  uint32  y,
    DNX_SAND_OUT uint32* result,
    DNX_SAND_OUT uint32* remainder
  )
{  /*  Computes quotient result = x / y, remainder = x mod y
    where x is a double digit
    and result, y, remainder are single precision digits.
    Returns high digit of quotient (max value is 1)
    Assumes normalised such that v1 >= b/2
    where b is size of HALF_DIGIT
    i.e. the most significant bit of y should be one

    In terms of half-digits:
    (q2q1q0) = (u4u3u2u1u0) / (v1v0)
    (r1r0) = (u4u3u2u1u0) mod (v1v0)
    for m = 2, n = 2 where u4 = 0
    q2 is either 0 or 1.
    We set result = (q1q0) and return q2 as "overflow'
  */
  uint32
    qhat,
    rhat,
    t,
    v0,
    v1,
    u0,
    u1,
    u2,
    u3,
    B;
  uint32
    uu[2],
    q2;
  uint32
    overflowed;

  

  overflowed = FALSE;
  B  = (0xffff + 1);

  if( (NULL == result) ||
      (NULL == remainder)
    )
  {
    goto exit;
  }

  /* Check for normalisation */
  if (!(y & DNX_SAND_HIBITMASK))
  {
    *result = *remainder = 0;
    overflowed = TRUE;
    goto exit;
  }

  /* Split up into half-digits */
  v0 = DNX_SAND_LOHALF(y);
  v1 = DNX_SAND_HIHALF(y);
  u0 = DNX_SAND_LOHALF(x->arr[0]);
  u1 = DNX_SAND_HIHALF(x->arr[0]);
  u2 = DNX_SAND_LOHALF(x->arr[1]);
  u3 = DNX_SAND_HIHALF(x->arr[1]);

  /* Do three rounds*/

  /*  ROUND 1. Set j = 2 and calculate q2 */
  /*  Estimate qhat = (u4u3)/v1  = 0 or 1
    then set (u4u3u2) -= qhat(v1v0)
    where u4 = 0.
  */
  qhat = u3 / v1;
  if (qhat > 0)
  {
    rhat = u3 - qhat * v1;
    t = DNX_SAND_TOHIGH(rhat) | u2;
    if (qhat * v0 > t)
    {
      qhat--;
    }
  }
  uu[1] = 0;    /* (u4) */
  uu[0] = x->arr[1];  /* (u3u2) */
  if (qhat > 0)
  {
    /* (u4u3u2) -= qhat(v1v0) where u4 = 0 */
    dnx_sand_u64_device_u64_long_high_inner(uu, qhat, v1, v0);
    if (DNX_SAND_HIHALF(uu[1]) != 0)
    {  /* Add back */
      qhat--;
      uu[0] += y;
      uu[1] = 0;
    }
  }
  q2 = qhat;

  /*  ROUND 2. Set j = 1 and calculate q1 */
  /*  Estimate qhat = (u3u2) / v1
    then set (u3u2u1) -= qhat(v1v0)
  */
  t = uu[0];
  qhat = t / v1;
  rhat = t - qhat * v1;
  /* Test on v0 */
  t = DNX_SAND_TOHIGH(rhat) | u1;
  if ((qhat == B) || (qhat * v0 > t))
  {
    qhat--;
    rhat += v1;
    t = DNX_SAND_TOHIGH(rhat) | u1;
    if ((rhat < B) && (qhat * v0 > t))
      qhat--;
  }

  /*  Multiply and subtract
    (u3u2u1)' = (u3u2u1) - qhat(v1v0)
  */
  uu[1] = DNX_SAND_HIHALF(uu[0]);  /* (0u3) */
  uu[0] = DNX_SAND_TOHIGH(DNX_SAND_LOHALF(uu[0])) | u1;  /* (u2u1) */
  dnx_sand_u64_device_u64_long_high_inner(uu, qhat, v1, v0);
  if (DNX_SAND_HIHALF(uu[1]) != 0)
  {  /* Add back */
    qhat--;
    uu[0] += y;
    uu[1] = 0;
  }

  /* q1 = qhat */
  *result = DNX_SAND_TOHIGH(qhat);

  /* ROUND 3. Set j = 0 and calculate q0 */
  /*  Estimate qhat = (u2u1) / v1
    then set (u2u1u0) -= qhat(v1v0)
  */
  t = uu[0];
  qhat = t / v1;
  rhat = t - qhat * v1;
  /* Test on v0 */
  t = DNX_SAND_TOHIGH(rhat) | u0;
  if ((qhat == B) || (qhat * v0 > t))
  {
    qhat--;
    rhat += v1;
    t = DNX_SAND_TOHIGH(rhat) | u0;
    if ((rhat < B) && (qhat * v0 > t))
      qhat--;
  }

  /*  Multiply and subtract
    (u2u1u0)" = (u2u1u0)' - qhat(v1v0)
  */
  uu[1] = DNX_SAND_HIHALF(uu[0]);  /* (0u2) */
  uu[0] = DNX_SAND_TOHIGH(DNX_SAND_LOHALF(uu[0])) | u0;  /* (u1u0) */
  dnx_sand_u64_device_u64_long_high_inner(uu, qhat, v1, v0);
  if (DNX_SAND_HIHALF(uu[1]) != 0)
  {  /* Add back */
    qhat--;
    uu[0] += y;
    uu[1] = 0;
  }

  /* q0 = qhat */
  *result |= DNX_SAND_LOHALF(qhat);

  /* Remainder is in (u1u0) i.e. uu[0] */
  *remainder = uu[0];

  overflowed = q2;
exit:
  return overflowed;
}


/*****************************************************
*NAME
* dnx_sand_u64_shift_left
*TYPE:
*  PROC
*DATE:
*  03/03/2004
*FUNCTION:
*  Computes a = b << x.
*INPUT:
*  DNX_SAND_DIRECT:
*    DNX_SAND_OUT DNX_SAND_U64*    a -
*      loaded with result
*    DNX_SAND_IN  DNX_SAND_U64*    b -
*      number to shift
*    DNX_SAND_IN  uint32 result -
*      Loaded with result
*  DNX_SAND_INDIRECT:
*    None.
*OUTPUT:
*  DNX_SAND_DIRECT:
*    uint32 - the carry.
*  DNX_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
uint32
  dnx_sand_u64_shift_left(
    DNX_SAND_OUT DNX_SAND_U64*    a,
    DNX_SAND_IN  DNX_SAND_U64*    b,
    DNX_SAND_IN  uint32 x
  )
{
  uint32
    i,
    y;
  uint32
    mask,
    carry,
    nextcarry;

  

  carry = 0;

  if( (NULL == a) ||
      (NULL == b)
    )
  {
    goto exit;
  }


  /* Check input - NB unspecified result */
  if (x >= DNX_SAND_NOF_BITS_IN_UINT32)
    return 0;

  /* Construct mask */
  mask = DNX_SAND_HIBITMASK;
  for (i = 1; i < x; i++)
  {
    mask = (mask >> 1) | mask;
  }
  if (x == 0) {
    *a = *b;
    goto exit;
  }

  y = DNX_SAND_NOF_BITS_IN_UINT32 - x;
  carry = 0;
  for (i = 0; i < DNX_SAND_U64_NOF_UINT32S; i++)
  {
    nextcarry = (b->arr[i] & mask) >> y;
    a->arr[i] = b->arr[i] << x | carry;
    carry = nextcarry;
  }

exit:
  return carry;
}

/*****************************************************
 * See details in dnx_sand_u64.h
 *****************************************************/
uint32
  dnx_sand_u64_devide_u64_long(
    DNX_SAND_IN  DNX_SAND_U64*     x,
    DNX_SAND_IN  uint32 y,
    DNX_SAND_OUT DNX_SAND_U64*     result
  )
{
  /*  Calculates quotient q = u div v
    Returns remainder r = u mod v
    where q, u are multiprecision integers of ndigits each
    and d, v are single precision digits.

    Makes no assumptions about normalisation.
  */
  int
    j;
  DNX_SAND_U64
    t;
  uint32
    r;
  uint32
    shift;
  uint32
    bitmask,
    overflow,
    *uu,
    ndigits,
    local_y;

  

  local_y = y;
  ndigits = DNX_SAND_U64_NOF_UINT32S;

  if (local_y == 0)
  {
    return 0; /* Divide by zero error */
  }


  /*  Normalise first */
  /*  Requires high bit of V
    to be set, so find most signif. bit then shift left,
    i.e. d = 2^shift, u' = u * d, v' = v * d.
  */
  bitmask = DNX_SAND_HIBITMASK;
  for (shift = 0; shift < DNX_SAND_NOF_BITS_IN_UINT32; shift++)
  {
    if (local_y & bitmask)
    {
      break;
    }
    bitmask >>= 1;
  }

  /* shift can't be 32.
   * if local_y is 0 -> we exit before the for loop. 
   * o/w local_y has 1 set bit, and the loop will be broken before 
   */
  /* coverity[large_shift:FALSE] */
  local_y <<= shift;
  overflow = dnx_sand_u64_shift_left(result, x, shift);
  uu = result->arr;


  /* Step S1 - modified for extra digit. */
  r = overflow;  /* New digit Un */
  j = ndigits;

  for( j=ndigits-1; j >= 0; j--)
  {
    /* Step S2. */
    t.arr[1] = r;
    t.arr[0] = uu[j];
    overflow = dnx_sand_u64_devide_u64_long_high( &t, local_y, &result->arr[j], &r);
  }

  /* Unnormalise */
  r >>= shift;


  return r;
}


/*****************************************************
 * See details in dnx_sand_u64.h
 *****************************************************/
uint32
  dnx_sand_u64_add_long(
    DNX_SAND_INOUT  DNX_SAND_U64*     x,
    DNX_SAND_IN     uint32 y
  )
{
  DNX_SAND_U64
    y_u64;
  uint32
    overflowed;

  

  dnx_sand_u64_clear(&y_u64);
  y_u64.arr[0] = y;

  overflowed = dnx_sand_u64_add_u64(x, &y_u64);

  return overflowed;
}

/*****************************************************
 * See details in dnx_sand_u64.h
 *****************************************************/
uint32
  dnx_sand_u64_add_u64(
    DNX_SAND_INOUT  DNX_SAND_U64*     x,
    DNX_SAND_IN     DNX_SAND_U64*     y
  )
{
  uint32
    old_val_low,
    old_val_high,
    overflowed;
  uint32
    overflow_low,
    overflow_high;

  

  overflow_low  = FALSE;
  overflow_high = FALSE;
  old_val_low   = 0;
  old_val_high  = 0;
  overflowed    = FALSE;

  if( (NULL == x)       ||
      (NULL == y)
    )
  {
    goto exit;
  }

  /*
   * Add low bits
   */
  old_val_low = x->arr[0];
  x->arr[0] += y->arr[0];
  if(x->arr[0] < old_val_low)
  {
    overflow_low = TRUE;
  }

  /*
   * Add high bits
   */
  old_val_high = x->arr[1];
  x->arr[1] += y->arr[1];
  if(overflow_low)
  {
    x->arr[1]++;
  }

  if(x->arr[1] < old_val_high)
  {
    overflow_high = TRUE;
  }

  /*
   * If any has overflow
   */
  if( TRUE == overflow_high )
  {
    overflowed = TRUE;
  }

exit:
  return overflowed;
}


/*****************************************************
 * See details in dnx_sand_u64.h
 *****************************************************/
uint32
  dnx_sand_u64_subtract_u64(
    DNX_SAND_INOUT  DNX_SAND_U64*     x,
    DNX_SAND_IN     DNX_SAND_U64*     y
  )
{
  uint32
    old_val_low,
    old_val_high;
  uint32
    overflow_low,
    overflow_high,
    pass;

  

  overflow_low  = FALSE;
  overflow_high = FALSE;
  old_val_low   = 0;
  old_val_high  = 0;
  pass        = TRUE;

  if( (NULL == x)       ||
      (NULL == y)
    )
  {
    goto exit;
  }

  /*
   * subtract low bits
   */
  old_val_low = x->arr[0];
  x->arr[0] -= y->arr[0];
  if(x->arr[0] > old_val_low)
  {
    overflow_low = TRUE;
  }

  /*
   * Add high bits
   */
  old_val_high = x->arr[1];
  x->arr[1] -= y->arr[1];
  if(overflow_low)
  {
    x->arr[1]--;
  }

  if(x->arr[1] > old_val_high)
  {
    overflow_high = TRUE;
  }

  /*
   * If any has overflow
   */
  if( TRUE == overflow_high )
  {
    pass = FALSE;
    x->arr[0] = x->arr[1] = 0;
  }

exit:
  return pass;
}

/*****************************************************
 * See details in dnx_sand_u64.h
 *****************************************************/
uint32
  dnx_sand_u64_is_bigger(
    DNX_SAND_IN     DNX_SAND_U64*     x,
    DNX_SAND_IN     DNX_SAND_U64*     y
  )
{
  uint32
    bigger;

  

  bigger = FALSE;

  if( (NULL == x)       ||
      (NULL == y)
    )
  {
    goto exit;
  }

  if( x->arr[1] >  y->arr[1] )
  {
    bigger = TRUE;
  }
  else if( x->arr[1] ==  y->arr[1] )
  {
    if( x->arr[0] >  y->arr[0] )
    {
      bigger = TRUE;
    }
  }

exit:
  return bigger;
}

/*****************************************************
 * See details in dnx_sand_u64.h
 *****************************************************/
uint32
  dnx_sand_u64_is_zero(
    DNX_SAND_IN     DNX_SAND_U64*     x
  )
{
  uint32
    is_zero;

  

  is_zero = FALSE;

  if( (NULL == x)  )
  {
    goto exit;
  }

  if( ( x->arr[0] == 0 ) &&
      ( x->arr[1] == 0 )
    )
  {
    is_zero = TRUE;
  }

exit:
  return is_zero;
}

/*****************************************************
 * See details in dnx_sand_u64.h
 *****************************************************/
uint32
  dnx_sand_u64_are_equal(
    DNX_SAND_IN     DNX_SAND_U64*     x,
    DNX_SAND_IN     DNX_SAND_U64*     y
  )
{
  uint32
    are_equal;

  

  are_equal = FALSE;

  if( (NULL == x) ||  (NULL == y))
  {
    goto exit;
  }

  if( ( x->arr[0] ==  y->arr[0] ) &&
      ( x->arr[1] ==  y->arr[1] )
    )
  {
    are_equal = TRUE;
  }

exit:
  return are_equal;
}
/*****************************************************
 * See details in dnx_sand_u64.h
 *****************************************************/
uint32
  dnx_sand_u64_log2_round_up(
    DNX_SAND_IN DNX_SAND_U64 *x
  )
{
  uint32
    msb_bit;
  DNX_SAND_U64
    calc;
  dnx_sand_u64_clear(&calc);

  if (x->arr[1]==0)
  {
    msb_bit = dnx_sand_msb_bit_on(x->arr[0]);
    calc.arr[0] = 1u<<msb_bit;
    calc.arr[1] = 0;
  }
  else
  {
    msb_bit = dnx_sand_msb_bit_on(x->arr[1]);
    calc.arr[0] = 0;
    calc.arr[1] = 1u<<msb_bit;
    msb_bit += sizeof(x->arr[0])*8; /* +=32 */
  }

  if ( dnx_sand_u64_is_bigger(x, &calc) )
  {
    /*
     * need to make round up.
     */
    msb_bit++;
  }

  return msb_bit;
}


/*****************************************************
 * See details in dnx_sand_u64.h
 *****************************************************/
void
  dnx_sand_u64_print(
    DNX_SAND_IN DNX_SAND_U64*    u64,
    DNX_SAND_IN uint32 print_type,
    DNX_SAND_IN uint32 short_format
  )
{

    

  if(NULL == u64)
  {
    LOG_CLI((BSL_META("dnx_sand_u64_print received NULL ptr\n\r")));
    goto exit;
  }

  if(print_type == 0)
  {
    /*
     * Decimal Format
     */
    if(short_format)
    {
      if(0 != u64->arr[1])
      {
        LOG_CLI((BSL_META("%u * 2^32 + "),
                 u64->arr[1]
                 ));
      }
      LOG_CLI((BSL_META("%u"),
               u64->arr[0]
               ));
    }
    else
    {
      LOG_CLI((BSL_META("arr[1]: %u, arr[0]: %u\n\r"),
               u64->arr[1],
               u64->arr[0]
               ));
    }
  }
  else
  {
    /*
     * Hexadecimal Format
     */
    if(short_format)
    {
      if(0 != u64->arr[1])
      {
        LOG_CLI((BSL_META("0x%X "),
                 u64->arr[1]
                 ));
      }
      LOG_CLI((BSL_META("0x%X"),
               u64->arr[0]
               ));
    }
    else
    {
      LOG_CLI((BSL_META("arr[1]: 0x%8X, arr[0]: 0x%8X\n\r"),
               u64->arr[1],
               u64->arr[0]
               ));
    }
  }

exit:
  return;
}



