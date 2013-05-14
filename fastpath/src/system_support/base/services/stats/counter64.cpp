
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename Counter64.cpp
*
* @purpose The purpose of this file is to implement the 64 bit counter class
*
* @component Statistics Manager
*
* @comments none
*
* @create 10/08/2000
*
* @author Archana Suthan
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/


#include <statsinclude.h>

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

L7_ulong64 Counter64::operator+(const Counter64 &c) const
{
    L7_uint32 tmp=0;
    if (low > (low+c.low))
        tmp=1;
    return Counter64((high+tmp)+c.high, low+c.low); 
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

Counter64 Counter64::operator+=(const L7_ulong64 &c)
{
    if (low> low+c.low)
        high++;
    low += c.low;
    high += c.high;
    return *this;
}

/*
**********************************************************************
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

L7_ulong64 Counter64::operator-(const Counter64 &c) const
{
    L7_uint32 tmp=0;
    if (low < c.low)
        tmp=1;
    return Counter64((high-tmp)-c.high, low-c.low);
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

Counter64 Counter64::operator-=(const L7_ulong64 &c)
{
    if (low < c.low)
        high--;
    low -= c.low;
    high -= c.high;
    return *this;
}

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

Counter64 Counter64::operator++(void)
{
    if ((low+1) < low)
        high++;
    low++;
    return *this;
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

Counter64 Counter64::operator--(void)
{
    if ((low-1) > low)
        high--;
    low--;
    return *this;
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

bool Counter64::operator==(const Counter64 &c) const
{
    if (high == c.high)
        return (low == c.low)? true:false;
    else 
        return false;
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

bool Counter64::operator!=(const Counter64 &c) const
{
    return (high != c.high || low != c.low);
}

bool Counter64::operator!=(int l) const
{
    return (low != (L7_uint32)l);
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

bool Counter64::operator>(const Counter64 &c) const
{
    if (high == c.high)
        return (low > c.low)? true:false;
    else              
        return (high > c.high)? true:false;
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

bool Counter64::operator>=(const Counter64 &c) const
{
    if (high == c.high)
        return (low >= c.low)? true:false;
    else              
        return (high >= c.high)? true:false;
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

bool Counter64::operator<(const Counter64 &c) const
{
    if (high == c.high)
        return (low < c.low)? true:false;
    else
        return (high < c.high)? true:false;
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

bool Counter64::operator<=(const Counter64 &c) const
{
    if (high == c.high) 
        return (low <= c.low)? true:false;
    else 
        return (high <= c.high)? true:false;
    
}

