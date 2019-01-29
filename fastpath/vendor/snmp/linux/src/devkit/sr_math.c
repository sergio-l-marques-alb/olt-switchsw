/*
 *
 * Copyright (C) 1992-2003 by SNMP Research, Incorporated.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by SNMP Research, Incorporated.
 *
 * Restricted Rights Legend:
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions as set forth in subparagraph (c)(1)(ii) of the Rights
 *  in Technical Data and Computer Software clause at DFARS 252.227-7013;
 *  subparagraphs (c)(4) and (d) of the Commercial Computer
 *  Software-Restricted Rights Clause, FAR 52.227-19; and in similar
 *  clauses in the NASA FAR Supplement and other corresponding
 *  governmental regulations.
 *
 */

/*
 *                PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 *
 *
 *  this file contains all the functions related to the Counter64 math. It inpliments
 *  64 bit math on a 32 bit system.
 */

#include "sr_conf.h"

#include <stdio.h>

#include <malloc.h>

#include <stdlib.h>

#include <string.h>

#include "sr_type.h"
#include "sr_proto.h"
#include "sr_snmp.h"
#include "sr_math.h"
#include "diag.h"
SR_FILENAME


#define SR_BOTTOM16	0x0000ffff 	/* 65,535, bottom 16 bits of a 32 bit number are all 1s */
#define SR_TOP16	0xffff0000	/* 4,294,901,760 top 16 bits of a 32 bit number are all 1s */
#define SR_BIT16	0x00008000	/* 32,768  only bit 16 is a 1 in a 32 bit number */
#define SR_BIT17	0x00010000	/* 65,536  only bit 17 is a 1 in a 32 bit number*/
#define SR_BIT32	0x80000000	/* 2,147,483,648 only bit 32 is a 1 in a 32 bit number */

/****************************************************************************************************/
/*             Counter64Multiply()

  Arguments - 2 pointers to Counter64 data type.
  Return Value - A pointer to Counter64 data type

  Function - Multiplies Counter64 c1 by Counter64 c2.
	     Unpredictable results may occur if one number is greater
	     than Oxffff ffff ( 2^32 or greater -  app. 4.3 billion or greater)
	     and the other number is greater than Ox0000 ffff (2^16 or greater -  65,536 or greater).
	     Unpredictable results will occur if both numbers are
 	     greater than Oxffff ffff.
********************************************************************************************************/


UInt64 *
Counter64Multiply(UInt64 *c1, UInt64 *c2)
{


  UInt64 *answer;
  SR_UINT32 value, c1_bits, c2_bits, old_value;

  answer = (UInt64 *)malloc(sizeof(struct _UInt64));
  if(answer == NULL)  {
    DPRINTF((APERROR, "Counter64Multiple: malloc failed\n"));
    return(NULL);
  }
  answer->big_end = 0;
  answer->little_end = 0;


 if(c1 == NULL || c2 == NULL)   {
   DPRINTF((APERROR, "Counter64Multiple Error: NULL pointer passed.\n"));
   return(answer);
  }

 if((c1->big_end == 0 && c1->little_end == 0) || (c2->big_end == 0 && c2->little_end == 0))  {
   return(answer);							/* just return 0 value */
  }


/********** get bottom 16 bits of little end of c1 ****************/
/******************************************************************/
 c1_bits = c1->little_end & SR_BOTTOM16;
  if(c1_bits != 0)   {
    c1_bits = c1_bits << 16;

        c2_bits = c2->little_end & SR_BOTTOM16;	/* bottom 16 bits of c2 little end  */
	if(c2_bits != 0 )  {
          c2_bits = c2_bits << 16;
          value = DoMultiply(c1_bits,c2_bits);
          answer->little_end += value;
 	}				/* end of if c2 != 0  */


        c2_bits = c2->little_end & SR_TOP16;	/* top 16 bits of c2 little end  */
	if(c2_bits != 0 )  {
          value = DoMultiply(c1_bits,c2_bits);
          old_value = answer->little_end;
          answer->little_end += (value << 16);
          answer->big_end += (value >> 16);
	  if(answer->little_end < old_value || answer->little_end < (value << 16))  {			/* had a carry */
 	     answer->big_end += 1;
           }

 	}				/* end of if c2 != 0  */


      c2_bits = c2->big_end & SR_BOTTOM16;		/* bottom 16 bits of c2 big_end */
	if(c2_bits != 0 )  {
          c2_bits = c2_bits << 16;
          value = DoMultiply(c1_bits,c2_bits);
          answer->big_end += value;
 	}				/* end of if c2 != 0  */



        c2_bits = c2->big_end & SR_TOP16;		/* top 16 bits of c2 big_end  */
	if(c2_bits != 0 )  {
          value = DoMultiply(c1_bits,c2_bits);
          answer->big_end += (value << 16);
 	}				/* end of if c2 != 0  */

    }				/* end of if c1 != 0   */


/********** get top 16 bits of little end of c1 ****************/
/******************************************************************/
 c1_bits = c1->little_end & SR_TOP16;
  if(c1_bits != 0)   {

        c2_bits = c2->little_end & SR_BOTTOM16;	/* bottom 16 bits of c2 little_end  */
	if(c2_bits != 0 )  {
          c2_bits = c2_bits << 16;
          value = DoMultiply(c1_bits,c2_bits);
          old_value = answer->little_end;
          answer->little_end += (value << 16);
          answer->big_end += (value >> 16);
	  if(answer->little_end < old_value || answer->little_end < (value << 16))  {			/* had a carry */
	     answer->big_end += 1;
           }
 	}				/* end of if c2 != 0  */


        c2_bits = c2->little_end & SR_TOP16;	/* top 16 bits of c2 little_end  */
	if(c2_bits != 0 )  {
          value = DoMultiply(c1_bits,c2_bits);
          answer->big_end += value;
 	}				/* end of if c2 != 0  */


      c2_bits = c2->big_end & SR_BOTTOM16;		/* bottom 16 bits of c2 big_end  */
	if(c2_bits != 0 )  {
          c2_bits = c2_bits << 16;
          value = DoMultiply(c1_bits,c2_bits);
          answer->big_end += (value << 16);
 	}				/* end of if c2 != 0  */

    }				/* end of if c1 != 0   */

/********** get bottom 16 bits of big end of c1 ****************/
/******************************************************************/
 c1_bits = c1->big_end & SR_BOTTOM16;
  if(c1_bits != 0)   {
    c1_bits = c1_bits << 16;

        c2_bits = c2->little_end & SR_BOTTOM16;	/* bottom 16 bits of c2 little_end  */
	if(c2_bits != 0 )  {
          c2_bits = c2_bits << 16;
          value = DoMultiply(c1_bits,c2_bits);
          answer->big_end += value;
 	}				/* end of if c2 != 0  */


        c2_bits = c2->little_end & SR_TOP16;	/* top 16 bits of c2 little_end  */
	if(c2_bits != 0 )  {
          value = DoMultiply(c1_bits,c2_bits);
          answer->big_end += (value << 16);
 	}				/* end of if c2 != 0  */

    }				/* end of if c1 != 0   */

/********** get top 16 bits of big end of c1 ****************/
/*****************************************************************/
 c1_bits = c1->big_end & SR_TOP16;
  if(c1_bits != 0)   {

        c2_bits = c2->little_end & SR_BOTTOM16;		/* bottom 16 bits of c2 little_end  */
	if(c2_bits != 0 )  {
          c2_bits = c2_bits << 16;
          value = DoMultiply(c1_bits,c2_bits);
          answer->big_end += (value << 16);
 	}				/* end of if c2 != 0  */

    }				/* end of if c1 != 0   */

   

 return(answer);

}						/****** END OF FUNCTION  Counter64Multiply   *******/



/*************************************************************************************/
/*		DoMultiply()

  Arguments - 2 unsigned32 ints.
  Return value - unsigned32 int.

  Function - Takes 2 unsigned ints, both of which are non zero,
	     and preforms the binary multiplication on them.
             answer is returned as an unsigned32 int.
**************************************************************************************/

SR_UINT32 
DoMultiply(SR_UINT32 c1_bits, SR_UINT32 c2_bits)
 {

  int x;
  SR_UINT32 value = 0, check;

    c2_bits = c2_bits >> 1;

	  for(x = 0; x < 16; x++)  {
   	    check = SR_BIT17 << x;		/* isolate bit we want to check */

    	    if((c1_bits & check) == check)  {	/* if c1_bits has a 1 in that spot */
              value = value >> 1;		/* shift total over to right */
      	      value += c2_bits;			/* add in value of c2_bits  */
  	     }				        /* end of if need to add */

            else    {
	      value = value >> 1;       /* no need to add in c2, just shift total */
  	    }				/* end of else */	
        }                              /* end of for loop */

   return(value);

 }				/******* END OF FUCNTION  DoMultiply  *******/






/*******************************************************************************************/
/*             Counter64Add()

  Arguments - 2 pointers to Counter64 data type.
  Return Value - A pointer to Counter64 data type

  Function - Adds 2 counter64 data types by breaking both 32 bit segments
 	      into 16 bits segments and adding the corresponding 16 bit segments. 
 	     Unpredictable results occur when the values of the "big_end"'s
	      sum to more then can be stored in a 32 bit unsigned int, as there
	      is no place to store the carry in the Counter64 structure.( sum will be
	      greater than 0xffff ffff).
*******************************************************************************************/


UInt64 *
Counter64Add(UInt64 *c1, UInt64 *c2)
{

  UInt64 *answer;
  int carry_bl = 0, carry_tl = 0, carry_bb = 0;
  SR_UINT32 c1_top,c1_bottom;
  SR_UINT32 c2_top,c2_bottom,bottom_ans,top_ans;


 answer = (UInt64 *)malloc(sizeof(struct _UInt64));
  if(answer == NULL)  {
    DPRINTF((APERROR, "Counter64Add: malloc failed\n"));   
    return(NULL);
  }

 answer->big_end = 0;
 answer->little_end = 0;


  if(c1 == NULL || c2 == NULL)   {
      DPRINTF((APERROR, "Counter64Add Error: NULL pointer passed.\n"));
      return(answer);
   }


/*********************** LITTLE END  **************************************/
  c1_top = c1->little_end & SR_TOP16;		/* get bottom 32 bits from arg1  */
  c1_bottom = c1->little_end & SR_BOTTOM16;
  c1_top = c1_top >> 16;

  c2_top = c2->little_end & SR_TOP16;		/* get bottom 32 bits from arg 2 */
  c2_bottom = c2->little_end & SR_BOTTOM16;
  c2_top = c2_top >> 16;

  bottom_ans = c1_bottom + c2_bottom;			/* add bottom 16 bits from each */
   if((SR_BIT17 & bottom_ans) == SR_BIT17)  {		/* if bottom had a carry */
     carry_bl = 1;					/* set carry to use later */
     bottom_ans = bottom_ans & SR_BOTTOM16;		/* clear off carry bit if any */ 
   }
  answer->little_end = bottom_ans;

  top_ans = c1_top + c2_top + carry_bl;			/* add top 16 bits from each  */
   if((SR_BIT17 & top_ans) == SR_BIT17)  {		/* if top had a carry */
     carry_tl = 1;					/* set carry to use later */
     top_ans = top_ans & SR_BOTTOM16;			/* clear off carry bit if any */ 
   }

  top_ans = top_ans << 16;				/* shift top answer back to top 16 bits */
  answer->little_end += top_ans;

/*************************** BIG END  *********************/
  c1_top = c1->big_end & SR_TOP16;				/* get top 32 bits from arg 1  */
  c1_bottom = c1->big_end & SR_BOTTOM16;
  c1_top = c1_top >> 16;

  c2_top = c2->big_end & SR_TOP16;				/* get top 32 bits from arg 2  */
  c2_bottom = c2->big_end & SR_BOTTOM16;
  c2_top = c2_top >> 16;

  bottom_ans = c1_bottom + c2_bottom + carry_tl;	/* add bottom 16 bits from each */
   if((SR_BIT17 & bottom_ans) == SR_BIT17)  {		/* if bottom had a carry */
     carry_bb = 1;					/* set carry to use later */
     bottom_ans = bottom_ans & SR_BOTTOM16;		/* clear off carry bit if any */ 
   }
  answer->big_end = bottom_ans;

  top_ans = c1_top + c2_top + carry_bb;			/* add top 16 bits from each */
  top_ans = top_ans << 16;				/* shift top answer back to top 16 bits */
							/* any carry here is lost  */
  answer->big_end += top_ans;

 return(answer);

}						/************ END OF FUNCTION Counter64Add   *************/



/***************************************************************************/
/*             Counter64Subtract()

  Arguments - 2 pointers to Counter64 data type.
  Return Value - A pointer to Counter64 data type

  Function - Subtracts Counter64 c2 from Counter64 c1.
             If the value of c2 is less than c1, the value of c2 is subtracted 
              from c1 by doing a 2's compliment on c2 and then calling 
              Counter64Add, passing c1 and c2 as arguments.
	     If c2 is greater than c1, unpredictable results occur.
***************************************************************************/

UInt64 *
Counter64Subtract(UInt64 *c1, UInt64 *c2)
{

 UInt64 *answer, *c3;

     

  c3 = (UInt64 *)malloc(sizeof(struct _UInt64));
  if(c3 == NULL)  {
     DPRINTF((APERROR, "Counter64Subtract: malloc failed\n"));
    return(NULL);
  }

  c3->big_end = 0;
  c3->little_end = 0;

  if(c1 == NULL || c2 == NULL)   {
      DPRINTF((APERROR, "Counter64Subtract Error: NULL pointer passed.\n"));
      return(c3);
   }
  

  if(c2->big_end == 0 && c2->little_end == 0)   {	/* if tring to subtract 0 */
    c3->big_end = c1->big_end;
    c3->little_end = c1->little_end;
    return(c3);						/* just return value of c1 */
  }
 
  c3->big_end = c2->big_end;				/* get values of c2 to flip  */
  c3->little_end = c2->little_end;

  c3->big_end = ~(c3->big_end);				/* preform 2's compliment */
  c3->little_end = ~(c3->little_end);
  c3->little_end ++;
   if(c3->little_end == 0)    {
     c3->big_end ++;
    }

 
  answer = Counter64Add(c1,c3);				/* call add function with new values */
  FreeCounter64(c3);

 return(answer);


}			/***** END OF FUNCTION  Counter64Subtract ******/


/******************************************************************************
		SetCounter64FromC64();

  Arguments - 2 pointers to a Counter64 date type.
  Return Value - INT. Returns -1 on failure, 0 otherwise.

  Function - Sets the 1st Counter64 to the value of the 2nd Counter64.
	     No mallocing is done. Function sets both the "big_end" and
	     the "little_end" of the 1st Counter64 to the value stored 
	     in the 2nd Counter64.

*****************************************************************************/
int 
SetCounter64FromC64(UInt64 *c1, UInt64 *c2)
 {

   if(c1 == NULL || c2 == NULL)   {	/* if either Counter64 points to NULL  */
      return (-1);			/* return failure */
    }

   else   {				/* ok to set value  */
     c1->big_end = c2->big_end;
     c1->little_end = c2->little_end;
     return 0;
   }

}					/**** END OF FUNCTION SetCounter64FromC64    **********/

/******************************************************************************
		SetCounter64FromInt();

  Arguments - A pointer to a Counter64 date type, a unsigned long int.
  Return Value - int. Returns -1 on failure, 0 otherwise.

  Function - Sets the passed Counter64 to the value of the unsigned long int
	     that is passed as the 2nd argument. No mallocing is done. the value
	     that is passed is put in the "little_end" of the counter64.

*****************************************************************************/

int 
SetCounter64FromInt(UInt64 *c1, SR_UINT32 value)
 {

   if(c1 == NULL)   {			/* if Counter64 not already malloced */
      return (-1);			/* return failure */
    }

   else   {				/* ok to set value  */
     c1->big_end = 0;
     c1->little_end = value;
     return 0;
   }

 }					/**** END OF FUNCTION SetCounter64FromInt    **********/

/******************************************************************************
		IncrementCounter64();

  Arguments - A pointer to a Counter64 date type.
  Return Value - int. Returns -1 on failure, 0 otherwise.

  Function - Increases the value of the passed Counter64 data type by 1.
	     checks for any wrap from the little_end, and if found, increase
	     the big_end by 1.

*****************************************************************************/

int 
IncrementCounter64(UInt64 *c1)
 {

   if(c1 == NULL)    {                  /* if bad pointer sent  */
     return (-1);
   }
  
   c1->little_end++;

   if(c1->little_end == 0)   {                  /* if wrap occured */
     c1->big_end ++;                            /* increment big end */
   }
     
  return(0);

 }                              /*** END OF FUNCTION IncrementCounter64()   ****/


/******************************************************************************
		IncreaseCounter64();

  Arguments - A pointer to a Counter64 date type, an unsigned long int.
  Return Value - int. Returns -1 on failure, 0 otherwise.

  Function - Increases the value of the passed Counter64 by the value
	     of the passed int. 
	     checks for any wrap from the little_end, and if found, increase  
             the big_end by 1.

*****************************************************************************/

int 
IncreaseCounter64(UInt64 *c1, SR_UINT32 value)
 {
    
   if(c1 == NULL)    {                                  /* if bad pointer sent  */
     return (-1);
   }
   

   c1->little_end += value;
     
   if(c1->little_end <  value )   {                     /* if wrap occured */
     c1->big_end ++;                                    /* increment big end */
   }

  return(0);

 

 }                      /**** END OF FUNCTION  IncreaseCounter64()   ****/



/****************************************************************************************************/
/*             Counter64Divide()

  Arguments - 2 pointers to Counter64 data type.
  Return Value - A pointer to Counter64 data type

  Function - Divides Counter64 c1 by Counter64 c2

	     If c2 = 0, returns NULL and prints error message.
	     If c2 > c1 or c1 = 0, returns value 0.
	     Else returns integer division   c1/c2.
********************************************************************************************************/

UInt64 *
Counter64Divide(UInt64 *c1, UInt64 *c2)
{

  UInt64 *answer;
  SR_UINT32  value, c1_bits, c2_bits, remainder = 0;
  SR_INT32 carry, hold, c1_top, c2_top;

  answer = (UInt64 *)malloc(sizeof(struct _UInt64));
   if(answer == NULL)    {
       DPRINTF((APERROR, "Counter64Divide: malloc failed\n"));
       return(NULL);
    }

  answer->big_end = 0;
  answer->little_end = 0;

   if(c1 == NULL || c2 == NULL)   {
     DPRINTF((APERROR, "Counter64Divide Error: NULL pointer passed.\n"));
     return(answer);
    }


   /************************************************/
   /*  error handling for bad division values      */ 
   /************************************************/
    if(c2->big_end == 0 && c2->little_end == 0)  {		/* if c2 = 0, print error and return null */
      DPRINTF((APERROR, "ERROR: Division by zero \n"));
      DPRINTF((APERROR,"Returning NULL \n\n"));
      return(NULL);
    }

   if(c1->big_end == 0 && c1->little_end == 0)  {		/* if c1 = 0, return 0   */
         return(answer);
     }

    if(c2->big_end > c1->big_end)  {				/* if c2 > c1, return 0  */
      return(answer);      
     }

    if((c2->big_end == 0 && c1->big_end == 0) && (c2->little_end > c1->little_end))   {		/* if c2 > c1, return 0  */
      return(answer);
    }

    if((c1->big_end == c2->big_end) && (c2->little_end > c1->little_end))   {		/* if c2 > c1, return 0  */
        return(answer);
    }



   if(c1->big_end == 0 && c2->big_end == 0)  {			/* if only using values from little ends  */
       value = DoDivide(c1->little_end,c2->little_end);
       answer->little_end = value;
       return(answer);
    }



     else if(c2->big_end == 0)	{				/* only c1 has values in the big end */
  	c1_bits = c1->big_end;
	c2_bits = c2->little_end;
	value = DoDivide(c1_bits,c2_bits);	/* divide top 32 bits of c1 by bottom 32 bits of c2 */
	remainder = (c1_bits % c2_bits);	/* calculate remainder */
	answer->big_end += value;


	c1_bits = (remainder << 16) + ((c1->little_end & SR_TOP16) >> 16);	/* reaminder of top plus top 16 bits from */
	value = DoDivide(c1_bits,c2_bits);					/* bottom 32 bits of c1  */
	remainder = (c1_bits % c2_bits);

	answer->big_end += (value >> 16);		/* put top part of answer in big end */
	answer->little_end += (value << 16);		/* put bottom part of answer at top of little end */


	if(remainder > SR_BOTTOM16)   {			/* if still have something in top of c1 */

	carry = remainder >> 16;
	hold = 0;

	 while(carry > 0)  {				/* divide out top bits one at a time */
   	   c1_bits = SR_TOP16 + SR_BOTTOM16;		/* set = ffff ffff  */
   	   carry -= 1;					/* subtract 1 from carry */
   	   hold += 1;					/* stick the extra in hold    c1_bits + hold = 1 from carry */
	   value = DoDivide(c1_bits,c2_bits);		/* divide out what we can */
	   remainder = (c1_bits % c2_bits);

	   hold += remainder;
            if(hold < remainder)			/* had a carry on the remainders */   
             carry ++;					/* add on back to carry  */

	   answer->little_end += value;
	 }					/* end of while */

    	 c1_bits = hold + (c1->little_end & SR_BOTTOM16);
       }					/* end of if remainder > bottom 16  */

       else  {
           c1_bits = (remainder << 16) + (c1->little_end & SR_BOTTOM16);
       }


	value = DoDivide(c1_bits,c2_bits);		/* everything now in bottom 32 bits, just do divide */
	remainder = (c1_bits % c2_bits);
	answer->little_end += value;

      return(answer);
    }					/* end of else only c1 has big  */



  else   {				/* both have values in the big end */

	c1_bits = c1->little_end;
  	c2_bits = c2->little_end;
	c1_top = c1->big_end;
	c2_top = c2->big_end;


                                      /* shift both right into little_end till c1 fully into little_end */
      	while(c1_top > 0)	{
	  if((c1_top & 1) == 1)   			/* if right most bit of top is a 1     */
	     c1_bits = (c1_bits >> 1) + SR_BIT32;	/* right shift little end, shift in the 1 */
	  else
 		c1_bits = c1_bits >> 1;			/* just right shift little end */

	  if((c2_top & 1) == 1)   			/* same as for c1 */
	     c2_bits = (c2_bits >> 1) + SR_BIT32;
	  else
 		c2_bits = c2_bits >> 1;

  	  c1_top = c1_top >> 1;				/* right shift tops  */
	  c2_top = c2_top >> 1;
        }

      value = DoDivide(c1_bits,c2_bits);		/* both just in little end, can call divide function */
      answer->little_end += value;

      return(answer);
    }				/* end of else both have big_end */



}				/********  END OF FUNCTION  Counter64Divide  *************/



/*************************************************************************************/
/*		DoDivide()

  Arguments - 2 unsigned32 ints.
  Return value - unsigned32 int.

  Function - Takes 2 unsigned ints, both of which are non zero,
	     and preforms the binary division on them.
             answer is returned as an unsigned32 int.
**************************************************************************************/

SR_UINT32 
DoDivide(SR_UINT32 c1_bits, SR_UINT32 c2_bits)
 {
  
  int x, add_bit = 0;
  SR_UINT32 value = 0, buffer = 0;

	  for(x = 0; x < 32; x++)  {
    	    if((c1_bits & SR_BIT32) == SR_BIT32)  {		/* if leftmost bit is a 1  */
               add_bit = 1;
             }

            c1_bits = c1_bits << 1;				/* shift out top bit of c1_bits   */
            buffer = buffer << 1;				/* left shift holding buffer  */
            buffer += add_bit;					/* add in bit shifted out of c1_bits, if any   */
            add_bit = 0;					/* reset add_bit      */
	    value = value << 1;

            if(buffer >= c2_bits)   {
              buffer -= c2_bits;
              value += 1;
             }       

  	  }				/* end of for loop */

   return(value);

 }				/******* END OF FUCNTION  DoDivide  *******/



/*************************************************************************************/
/*		Counter64Mod()

  Arguments - 2 pointers to Counter64 Data Types
  Return value - a pointer to a Counter64 Data Type.

  Function - Returns the remainder of a Counter64Divide().

	     If c2 = 0, returns NULL and prints error message.
	     If c2 > c1 or c1 = 0, returns value 0.
	     Else returns remainder of integer division   c1/c2.

**************************************************************************************/
UInt64 *
Counter64Mod(UInt64 *c1, UInt64 *c2)
{
  
 UInt64 *answer, *temp, *cptr;
    
  answer = (UInt64 *)malloc(sizeof(struct _UInt64));
  if(answer == NULL)  {
  DPRINTF((APERROR, "Counter64Mod: malloc failed\n"));
    return(NULL);
  }
     
 answer->big_end = 0;
 answer->little_end = 0;
     

  /*****  handle errors and special cases  *******/
 if(c2 ==  NULL || c1 == NULL)  {
    DPRINTF((APERROR, "Counter64Mod Error: NULL pointer passed.\n"));
    return(answer);
 }
 
 if(c1->big_end == 0 && c1->little_end == 0)  {
   return(answer);
 }

 if(c2->big_end  == 0 && c2->little_end == 0)   {
   FreeCounter64(answer);
   return(NULL);
 }
 
 
   temp = Counter64Divide(c1,c2);

   if(temp==NULL){
        FreeCounter64(answer);
        return(NULL);
   }
     
   if(temp->big_end == 0 && temp->little_end == 1)   {   /* if only going to subtract c2's value once */
       temp->big_end  = c2->big_end;			 /* set to c2's value  */
       temp->little_end = c2->little_end;
    }
   else  {
     cptr = temp;					/* else, calculate how much to remove  */
     temp = Counter64Multiply(temp,c2);
     FreeCounter64(cptr);
   }
  
   FreeCounter64(answer);
   answer = Counter64Subtract(c1,temp);			/* calculate remainder */
   FreeCounter64(temp);
  
  return(answer);
}               /* end of function  */


