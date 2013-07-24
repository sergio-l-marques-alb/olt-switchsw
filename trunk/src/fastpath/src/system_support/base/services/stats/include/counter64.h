/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename     64bitCtr.h
*
* @purpose      The purpose of this file is to define the 64 bit counter class
*
* @component    Statistics Manager
*
* @comments     none
*
* @create       10/08/2000
*
* @author       Archana Suthan
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#ifndef INCLUDE_COUNTER64_H
#define INCLUDE_COUNTER64_H

/*
***********************************************************************
*                           64 BIT COUNTER CLASS
***********************************************************************
*/

/*
**********************************************************************
*
* @class        Counter64
*
* @purpose      This class is used to implement 64 bit counters 
*
* @attribute    L7_uint32 high
* @attribute    L7_uint32 low
*  
* @member       operator+ : adds two 64 bit counters
* @member       operator- : subtracts one 64 bit counter from another
* @member       operator++: increments the 64 bit counter by 1
* @member       operator--: decrements the 64 bit counter by 1
*
* @end
*
**********************************************************************
*/

class Counter64
{
private:
	L7_uint32 high;  // This is the higher 32 bits of the counter
    L7_uint32 low;   // This is the lower 32 bits of the counter

public:
	Counter64(L7_uint32 h, L7_uint32 l) : high(h), low(l) {}   // ConstructorI

    Counter64() : high(0), low(0) {}

    Counter64(L7_ulong64 c) {                                     // Conversion
        high = c.high; low  = c.low;                           // Constructor
    }

	virtual ~Counter64() {}   						           // Destructor

    Counter64(const Counter64 &c): high(c.high), low(c.low) {} // Copy 
                                                               // Constructor

    Counter64& operator=(int x){
        high = 0; low = x; return *this;
    }

    Counter64& operator=(const Counter64 &c) {                 // Assignment
        high = c.high; low = c.low; return *this;
    }
    
/*
**********************************************************************
*
* @function     operator ulong64()
*
* @purpose      This function allows the conversion from Counter64 to ulong64
*               to faciliate its use in C code
*          
* @parameter    Counter64          c
*     
* @return       Counter64       Casts the specified value to a ulong64 type
*
* @end
**********************************************************************
*/

	operator L7_ulong64(){
        L7_ulong64 tmp;
        tmp.high = high; tmp.low  = low;
        return tmp;
    }
    
/*
**********************************************************************
*
* @function     operator+
*
* @purpose      This function allows the client to add two 64 bit counters  
*          
* @parameter    Counter64          c
*     
* @return       Counter64 Returns the counter after the addition of the
*               specified 64 bit counter 
*
* @end
*
*
**********************************************************************
*/

	L7_ulong64 operator+(const Counter64 &c) const;
/*
**********************************************************************
*
* @function     operator+=
*
* @purpose      This function allows the client to add two 64 bit counters  
*          
* @parameters   Counter64          c
*     
* @returns      Counter64   Returns the counter after the addition of the
*               specified 64 bit counter 
*
* @end
*
*
**********************************************************************
*/

	Counter64 operator+=(const L7_ulong64 &c);

/*
**********************************************************************
*
* @function     operator-
*
* @purpose      This function allows the client to subtract one 64 bit counter
*               from another  
*          
* @parameter    Counter64          c
*     
* @return       Counter64      Returns the counter after the subtraction of the
*               specified 64 bit counter 
* @end
*
*
**********************************************************************
*/

	L7_ulong64 operator-(const Counter64 &c) const;

/*
**********************************************************************
*
* @function     operator-=
*
* @purpose      This function allows the client to subtract one 64 bit counter
*               from another  
*          
* @parameter    Counter64          c
*     
* @return       Counter64      Returns the counter after the subtraction of the
*               specified 64 bit counter 
* @end
*
*
**********************************************************************
*/

	Counter64 operator-=(const L7_ulong64 &c);

/*
**********************************************************************
*
* @function     operator++
* 
* @purpose      This function allows the client increment a 64 bit counter
*               by 1  
*          
* @parameter    Counter64          c
*     
* @return       Counter64      Returns the counter after the subtraction of the
*               specified 64 bit counter 
* @end
*
*
**********************************************************************
*/

	Counter64 operator++(void);
/*
**********************************************************************
*
* @function     operator--
*
* @purpose      This function allows the client decrement a 64 bit counter
*               by 1  
*          
* @parameter    Counter64          c
*     
* @return       Counter64      Returns the counter after decrementing its value
*
* @end
*
**********************************************************************
*/

	Counter64 operator--(void);

/*
**********************************************************************
*
* @function     operator==
*
* @purpose      This function allows the client to compare two 64 bit counters  
*          
* @parameter    Counter64          c
*     
* @return       Counter64     Returns the true if this counter is equal to the
*               specified counter 
*
* @end
*
**********************************************************************
*/

	bool operator==(const Counter64 &c) const;

/*
**********************************************************************
*
* @function    operator!=
*
* @purpose     This function allows the client to compare two 64 bit counters  
*          
* @parameter   Counter64          c
*     
* @return      Counter64      Returns the true if this counter is equal to the
*              specified counter 
*
* @end
*
**********************************************************************
*/

	bool operator!=(const Counter64 &c) const;

    bool operator!=(int l) const;
/*
**********************************************************************
*
* @function     operator>
*
* @purpose      This function allows the client to compare two 64 bit counters  
*          
* @parameter    Counter64          c
*     
* @return       Counter64      Returns the true if this counter is greater than
*               the specified counter 
*
* @end
*
**********************************************************************
*/

	bool operator>(const Counter64 &c) const;

/*
**********************************************************************
*
* @function     operator>=
*
* @purpose      This function allows the client to compare two 64 bit counters  
*          
* @parameter    Counter64          c
*     
* @return       Counter64      Returns the true if this counter is greater than
*               the specified counter 
*
* @end
*
**********************************************************************
*/

    bool operator>=(const Counter64 &c) const;

/*
**********************************************************************
*
* @function     operator<
*
* @purpose      This function allows the client to compare two 64 bit counters  
*          
* @parameter    Counter64          c
*     
* @return       Counter64      Returns the true if this counter is lesser than
*               the specified counter 
*
* @end
*
**********************************************************************
*/

	bool operator<(const Counter64 &c) const;

/*
**********************************************************************
*
* @function     operator<=
*
* @purpose      This function allows the client to compare two 64 bit counters  
*          
* @parameter    Counter64          c
*     
* @return       Counter64      Returns the true if this counter is lesser than
*               the specified counter 
*
* @end
*
**********************************************************************
*/

    bool operator<=(const Counter64 &c) const;

};	


/*
/------------------------------------------------------------------\
*            END OF COUNTER64 DEFINITION                           *
\------------------------------------------------------------------/
*/

#endif	// INCLUDE_COUNTER64_H

