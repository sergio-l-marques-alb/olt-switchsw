
#ifndef DATATYPE_H
#define DATATYPE_H

/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include "global_macro.h"



/******************************************************************************
*CONSTANT AND MACRO DEFINE
******************************************************************************/
#ifdef SV_TEST
typedef char CHAR;
typedef unsigned char UINT_8;
typedef unsigned short int UINT_16;
typedef unsigned int UINT_32;
typedef unsigned long int UINT_64;
#else
typedef char CHAR;
typedef unsigned char U_CHAR;
typedef signed char S_CHAR;
typedef unsigned char UINT_8;
typedef signed char SINT_8;
typedef unsigned short int UINT_16;
typedef signed short int SINT_16;
typedef unsigned int UINT_32;
typedef signed long int SINT_32;
typedef unsigned long long int UINT_64;
typedef signed long long int SINT_64;
#endif

typedef UINT_32 RET_STATUS;



/******************************************************************************
*VARIABLE DEFINE
******************************************************************************/



/******************************************************************************
*LOCAL FUNCTION PROTOTYPE DECLARATION
******************************************************************************/



/******************************************************************************
*GLOBAL FUNCTION DEFINITION
******************************************************************************/



#endif
