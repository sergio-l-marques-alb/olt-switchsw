/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  osapi_stdlib.c
*
* @purpose   Supply missing standard C library functions
*
* @component osapi
*
* @comments
*
* @create    01/26/2006
*
* @author    gpaussa
*
* @end
*
*********************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include "l7_common.h"
#include "osapi.h"


/*==============================================================================
 *
 * LICENSE NOTICE:  This file MAY contain code that is derived from suitable 
 *                  (i.e. non-GPL) open source, for which the appropriate
 *                  copyright and license statements are included herein.
 *
 *==============================================================================
 *
 * <Include copy of any relevant license text here>
 *
 *==============================================================================
 */



/**************************************************************************
*
* @purpose  Concatenate first n characters of one string to another
*
* @param    *dest       @b{(input)}  destination location
* @param    *src        @b{(input)}  source string ptr
* @param    n           @b{(input)}  maximum number of characters to append
*
* @returns  Pointer to destination location (always)
*
* @comments The dest and src strings must not overlap.  The dest location 
*           must have enough space for n additional characters.
*
* @comments Only the first n characters of src are appended to dest.  The 
*           resulting dest string is always null terminated.  
*
* @comments To maintain consistency with the POSIX functions, the dest 
*           pointer is always returned, even if no characters were actually
*           appended.
*
* @end
*
*************************************************************************/
L7_char8 *osapiStrncat(L7_char8 *dest, const L7_char8 *src, L7_uint32 n)
{
  return strncat(dest, src, n);
}

/**************************************************************************
*
* @purpose  Compare at most n characters of two strings
*
* @param    *s1         @b{(input)}  string being compared
* @param    *s2         @b{(input)}  string s1 is compared against
* @param    n           @b{(input)}  maximum number of characters to compare
*
* @returns  <0          s1 less than s2
* @returns  0           s1 matches s2
* @returns  >0          s1 greater than s2
*
* @comments A lexicographical comparison is performed.  At most n
*           characters of s1 are compared to s2.
*
* @end
*
*************************************************************************/
L7_int32 osapiStrncmp(const L7_char8 *s1, const L7_char8 *s2, L7_uint32 n)
{
  return strncmp(s1, s2, n);
}

/**************************************************************************
*
* @purpose  Copy a string to a buffer with a bounded length
*
* @param    *dest       @b{(input)}  destination location
* @param    *src        @b{(input)}  source string ptr
* @param    n           @b{(input)}  maximum number of characters to copy
*
* @returns  Pointer to destination location (always)
*
* @comments The dest and src strings must not overlap.  The dest location 
*           must have enough space for n additional characters.
*
* @comments No more than n characters are copied from src to dest.  If there
*           is no '\0' character within the first n characters of src, the
*           dest string will be UNTERMINATED.  See osapiStrncpySafe()
*           for an alternative implementation.
*
* @comments If the src string length is less than n, the remainder of
*           dest is padded with nulls (total of n characters).
*
* @comments To maintain consistency with the POSIX functions, the dest 
*           pointer is always returned, even if no characters were actually
*           copied.  Invalid input parameters (such as null pointers)  
*           are not checked here.  See osapiStrncpySafe() for an 
*           alternative implementation offering more protection.
*
* @end
*
*************************************************************************/
L7_char8 *osapiStrncpy(L7_char8 *dest, const L7_char8 *src, L7_uint32 n)
{
  return strncpy(dest, src, n);
}

/**************************************************************************
*
* @purpose  Copy a string to a buffer with a bounded length (with safeguards)
*
* @param    *dest       @b{(input)}  destination location
* @param    *src        @b{(input)}  source string ptr
* @param    n           @b{(input)}  maximum number of characters to copy
*
* @returns  Pointer to destination location (always)
*
* @comments The dest and src strings must not overlap.  The dest location 
*           must have enough space for n additional characters.
*
* @comments No more than n characters are copied from src to dest.  If there
*           is no '\0' character within the first n > 0 characters of src, the
*           n-th byte of dest string will be written with the '\0'
*           string termination character (e.g., if n=10, *(dest+9)='\0').
*
* @comments If the src string length is less than n, the remainder of
*           dest is padded with nulls.
*
* @comments To maintain consistency with the POSIX functions, the dest 
*           pointer is returned, even if no characters were actually copied
*           (e.g., src string has zero length, src ptr is null, n is 0,
*           etc.)
*
* @end
*
*************************************************************************/
L7_char8 *osapiStrncpySafe(L7_char8 *dest, const L7_char8 *src, L7_uint32 n)
{
  L7_char8      *retptr = dest;

  /* don't copy anything if there is no dest location */
  if (dest == L7_NULLPTR)
    return retptr;

  if (src == L7_NULLPTR)
  {
    if (n > 0)
    {
      /* null src ptr, but n > 0:  pad dest to ensure termination */
      memset(dest, L7_EOS, n);
    }
    return retptr;
  }

  retptr = strncpy(dest, src, n);

  /* the standard library doesn't ensure string termination, but this 'safe'
   * function does
   */
  if (n > 0)
    *(dest+n-1) = L7_EOS;

  return retptr;
}

/**************************************************************************
*
* @purpose  Print a formatted string to a buffer with a bounded length
*
* @param    *str        @b{(input)}  destination location
* @param    n           @b{(input)}  maximum bytes to copy (including '\0')
* @param    *fmt        @b{(input)}  format string (like printf)
* @param    ...         @b{(input)}  variable arguments
*
* @returns  Number of characters printed (not including the trailing '\0'),
*           or that would have been printed if the str location had been
*           large enough (per ANSI C99).
*
* @returns  Negative value indicates an error ('errno' contains reason code).
*
* @comments
*
* @end
*
*************************************************************************/
L7_int32 osapiSnprintf(L7_char8 *str, L7_uint32 n, L7_char8 const *fmt, ...)
{
  L7_int32      rc;
  va_list       ap;

  memset(&ap, 0, sizeof(ap));
  va_start(ap, fmt);
  rc = osapiVsnprintf(str, n, fmt, ap);
  va_end(ap);

  return rc;
}

/**************************************************************************
*
* @purpose  Append a formatted string to a buffer with a bounded length
*
* @param    *str        @b{(input)}  destination location
* @param    n           @b{(input)}  maximum bytes to copy (including '\0')
* @param    *fmt        @b{(input)}  format string (like printf)
* @param    ...         @b{(input)}  variable arguments
*
* @returns  Number of characters printed (not including the trailing '\0'),
*           or that would have been printed if the str location had been
*           large enough (per ANSI C99).
*
* @returns  Negative value indicates an error ('errno' contains reason code).
*
* @comments
*
* @end
*
*************************************************************************/
L7_int32 osapiSnprintfcat(L7_char8 *str, L7_uint32 n, L7_char8 const *fmt, ...)
{
  L7_int32      len;
  L7_int32      rc;
  va_list       ap;

  len = strlen (str);
  memset(&ap, 0, sizeof(ap));
  va_start(ap, fmt);
  rc = osapiVsnprintf(str + len, n - len, fmt, ap);
  va_end(ap);

  return rc;
}

/**************************************************************************
*
* @purpose  Print a formatted string to a buffer with a bounded length
*           using a variable arg list
*
* @param    *str        @b{(input)}  destination location
* @param    n           @b{(input)}  maximum bytes to copy (including '\0')
* @param    *fmt        @b{(input)}  format string (like printf)
* @param    ap          @b{(input)}  variable argument list
*
* @returns  Number of characters printed (not including the trailing '\0'),
*           or that would have been printed if the str location had been
*           large enough (per ANSI C99).
*
* @returns  Negative value indicates an error ('errno' contains reason code).
*
* @comments The standard library typically returns -1 for all errors.  ANSI C99
*           permits any negative value to indicate an error, so negative
*           values other than -1 denote an error detected by OSAPI-specific 
*           checking.
*
* @end
*
*************************************************************************/
L7_int32 osapiVsnprintf(L7_char8 *str, L7_uint32 n, L7_char8 const *fmt, va_list ap)
{
  /* check inputs */

  /* NOTE: let vsnprintf handle special case of null str ptr with n==0 */
  if ((str == L7_NULLPTR) && (n != 0))
  {
    errno = EINVAL;
    return -3;
  }

  /* NOTE: vsnprintf handles null fmt ptr
   *
   * NOTE: ANSI C99 defines size_t as an unsigned int, so vsnprintf treats 
   *       negative values of n as a (possibly large) positive number.
   */

  return vsnprintf(str, n, fmt, ap);
}

/*********************************************************************
* @purpose  Duplicate a string
*
* @param    sourceString Pointer to string to duplicate
*
* @returns  Pointer to duplicated sourceString or L7_NULLPTR if an error occured
*
* @comments none.
*
* @end
*
*********************************************************************/
L7_char8 *osapiStrDup(L7_char8 *sourceString)
{
  L7_char8 *duplicateString = (L7_char8 *)L7_NULLPTR;
  if (sourceString == (L7_char8 *)L7_NULLPTR)
  {
    return (L7_char8 *)L7_NULLPTR;
  }

  duplicateString = (L7_char8 *)osapiMalloc(L7_OSAPI_COMPONENT_ID, (L7_uint32)(strlen(sourceString)+1));
  if (duplicateString != (L7_char8 *)L7_NULLPTR)
  {
    (void)strcpy(duplicateString, sourceString);
  }
  return duplicateString;
}

/*********************************************************************
* @purpose  Compare the first n characters of the two string. 
*
* @param    char *str1, char *str2, int n
*
* @returns  L7_SUCCESS  If two string matched each other.
* @returns  L7_FAILURE  otherwise
*
* @comments none.
*
* @end
*
*********************************************************************/
L7_RC_t osapiStrCaseCmp(char *str1, char * str2, int n)
{
  int i;

  for ( i = 0; i < n; i++ )
  {
    /* 
     * Verify that none of the string reach to end, \0 and \n shwos i
     *        that string reach to its end
     */
    if ((str1[i] != '\n' && str1[i] != '\0' ) &&
        (str2[i] != '\n' && str2[i] != '\0' ))
    {
      if( tolower(str1[i]) != (tolower(str2[i])))
      {
        return L7_FAILURE;
      }
    }
    else
    {
      return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose compare 2 buffers.
*
* @param L7_char8 *buf1
* @param L7_char8 *buf2
*
* @returns  L7_TRUE if buffers are the same.  L7_FALSE otherwise.
*
* @notes If the buffers can not be compared, L7_FALSE is returned.
* @notes The buffers must be null terminated.
*
* @end
*
*********************************************************************/
L7_BOOL osapiNoCaseCompare(L7_char8 *buf1, L7_char8 *buf2 )
{
  while (*buf1 != '\0' && tolower(*buf1) == tolower(*buf2))
  {
    buf1++;
    buf2++;
  }

  if (*buf1 != '\0' || *buf2 != '\0')
  {
    return L7_FALSE;
  }

  return L7_TRUE;
}

/*********************************************************************
*
* @purpose convert all letters inside a buffer (char8) to lower case
*
* @param L7_char8 *buf
*
* @returns  void
*
* @notes   This f(x) returns the same letter in the same buffer but all
*          lower case, checking the buffer for empty string
* @end
*
*********************************************************************/
void osapiConvertToLowerCase(L7_char8 *buf)
{
  L7_char8 c_tmp;
  L7_uint32 i;
  L7_uint32 len;

  len = strlen(buf);
  for (i = 0; i < len; i++)
  {
    if ( buf[i] != '\n' || buf[i] != '\0' )
    {
      c_tmp = (L7_char8)  tolower(buf[i]);
      buf[i] = c_tmp;
    }
  }
  return;
}

/**************************************************************************
*
* @purpose  convert string to long unsigned integer 
*
* @param    *s1         @b{(input)}  Points to a character string for strtoul() to convert. 
* @param    *s2         @b{(input)}  Is a result parameter that, if not NULL, is 
*                       loaded with the address of the first character that strtoul() 
*                       does not attempt to convert
* @param    n           @b{(input)}  Is the base of the string, a value between 0 and 36. 
*
* @returns  value       converted value
* @returns  0           Error
*
* @end
*
*************************************************************************/
L7_int32 osapiStrtoul(const L7_char8 *s1, L7_uint32 base, L7_uint32 *seqNum)
{
   L7_char8 *s2;

   errno = 0;
   *seqNum = strtoul(s1, &s2, base);
   
   if(s1 == s2)
   {
      return L7_FAILURE;
   }
   else if(errno != L7_NULL)
   {  
      return L7_FAILURE;
   }

   return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose  Returns the number of characters in the string pointed to by s, 
*                 not including the terminating '\0' character, but at most maxlen.
*
* @param    *s         @b{(input)}  Points to a character string.
* @param    maxlen  @b{(input)}  Maximum length of the string.
*
* @returns  length of s (max = maxlen)       
*
* @end
*
*************************************************************************/
L7_uint32 osapiStrnlen(const L7_char8 *s, L7_uint32 maxlen)
{
  L7_uint32 len;

  for(len=0; len<maxlen; len++)
  {
    if (s[len] == L7_EOS)
    {
      break;
    }
  }
  
  return len;
}

/**************************************************************************
*
* @purpose  To convert a formatted time string to tm struct
*
* @param  buf    Is the buffer containing the time specification.
* @param  format Is the format specification for the conversion.
* @param  tm     Points to the resulting converted time.
*
*
* @returns  Upon successful conversion returns a pointer to the character
            following the last character parsed
*           Fail case upon error a null pointer is returned.
*
* @comments Check strptime()
*
* @end
*
*************************************************************************/
L7_uchar8* osapiStrptime(const L7_uchar8* s, const L7_uchar8* format, struct tm* tm)
{
  return strptime(s, format, tm);
}

/**************************************************************************
*
* @purpose To compare two strings and check if the source string is present in
*          the destination string
*
* @param    *dest       @b{(input)}  destination location
* @param    *src        @b{(input)}  source string ptr
*
* @returns  If src is an empty string, dest is returned;
*           If src occurs nowhere in dest, NULL is returned;
*           If src occurs in dest, a pointer to the first character of the 
*                                  first occurrence of src is returned.
*
* @comments The dest and src strings must not overlap.  The dest location 
*           must have enough space for n additional characters.
*
* @comments No more than n characters are copied from src to dest.  If there
*           is no '\0' character within the first n characters of src, the
*           dest string will be UNTERMINATED.  See osapiStrncpySafe()
*           for an alternative implementation.
*
* @comments If the src string length is less than n, the remainder of
*           dest is padded with nulls (total of n characters).
*
* @comments To maintain consistency with the POSIX functions, the dest 
*           pointer is always returned, even if no characters were actually
*           copied.  Invalid input parameters (such as null pointers)  
*           are not checked here.  See osapiStrncpySafe() for an 
*           alternative implementation offering more protection.
*
* @end
*
*************************************************************************/
L7_char8 *osapiStrStr(L7_char8 *dest, const L7_char8 *src)
{
  return strstr(dest, src);
}

/**************************************************************************
*
* @purpose  The function shall test whether the value is a lowercase letter
*
* @param    c       @b{(input)}  A character
*
* @returns  non-zero value if c is a lowercase letter; 
*           otherwise, it returns 0.       
*
* @end
*
*************************************************************************/
L7_int32 osapiIsLower(L7_int32 c)
{
  return islower(c);
}

/**************************************************************************
*
* @purpose  The function shall test whether the value is a uppercase letter
*
* @param    c       @b{(input)}  A character
*
* @returns  non-zero value if c is a uppercase letter; 
*           otherwise, it returns 0.       
*
* @end
*
*************************************************************************/
L7_int32 osapiIsUpper(L7_int32 c)
{
  return isupper(c);
}

/**************************************************************************
*
* @purpose  The function shall test whether the value is a decimal number
*
* @param    c       @b{(input)}  A number
*
* @returns  non-zero value if c is a decimal number; 
*           otherwise, returns 0.       
*
* @end
*
*************************************************************************/
L7_int32 osapiIsDigit(L7_int32 c)
{
  return isdigit(c);
}

/**************************************************************************
*
* @purpose  The function shall test whether the value is a printable character
*
* @param    c       @b{(input)}  A Character
*
* @returns  non-zero value if c is a printable character; 
*           otherwise, returns 0.       
*
* @end
*
*************************************************************************/
L7_int32 osapiIsPrint(L7_int32 c)
{
  return isprint(c);
}

/**************************************************************************
*
* @purpose  The function shall test whether the value is a special character
*
* @param    c       @b{(input)}  A character
*
* @returns  non-zero value if c is a special character; 
*           otherwise, returns 0.       
*
* @end
*
*************************************************************************/
L7_int32 osapiIsSpecial(L7_int32 c)
{
  if((c >= 33 && c <= 47) ||   /* ! to / */
     (c >= 58 && c <= 64) ||   /* : to @ */
     (c >= 91 && c <= 96) ||   /* [ to ` */ 
     (c >= 123 && c <= 126))   /* { to ~ */  
  {
    return L7_TRUE; 
  } 
  return L7_FALSE;
}

/*****************************************************************
                         SPECIAL CHARACTERS                       
 *****************************************************************
 Char	Octal	Dec	Hex	Description                       
 =================================================================
 !	41	33	21	Exclamation mark
 "	42	34	22	Quotation mark (&quot; in HTML)
 #	43	35	23	Cross hatch (number sign)
 $	44	36	24	Dollar sign
 %	45	37	25	Percent sign
 &	46	38	26	Ampersand
 `	47	39	27	Closing single quote (apostrophe)
 ( 	50	40	28	Opening parentheses
 )	51	41	29	Closing parentheses
 *	52	42	2a	Asterisk (star, multiply)
 +	53	43	2b	Plus
 ,	54	44	2c	Comma
 -	55	45	2d	Hyphen, dash, minus
 .	56	46	2e	Period
 /	57	47	2f	Slant (forward slash, divide) 
 
 :	72	58	3a	Colon
 ;	73	59	3b	Semicolon
 <	74	60	3c	Less than sign (&lt; in HTML)
 =	75	61	3d	Equals sign
 >	76	62	3e	Greater than sign (&gt; in HTML)
 ?	77	63	3f	Question mark
 @	100	64	40	At-sign 

 [ 	133	91	5b	Opening square bracket
 \	134	92	5c	Reverse slant (Backslash)
 ]	135	93	5d	Closing square bracket
 ^	136	94	5e	Caret (Circumflex)
 _	137	95	5f	Underscore
 `	140	96	60	Opening single quote 

 {	173	123	7b	Opening curly brace
 |	174	124	7c	Vertical line
 }	175	125	7d	Cloing curly brace
 ~	176	126	7e	Tilde (approximate) 
 *****************************************************************/
/**************************************************************************
*
* @purpose  The function shall test whether the value is a alphanumeric 
*           character
*
* @param    c       @b{(input)}  A Character
*
* @returns  non-zero value if c is a alphanumeric character; 
*           otherwise, returns 0.       
*
* @end
*
*************************************************************************/
L7_int32 osapiIsAlnum(L7_int32 c)
{
  return isalnum(c);
}


