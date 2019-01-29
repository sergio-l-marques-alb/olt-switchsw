/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2010
*
**********************************************************************
*
* Name: util_stk.c
*
* Purpose: stk file utilites
*
*
* Created by: Vamshi krishna Madap 08/05/2010
*
* Component:
*
*********************************************************************/
#include<string.h>
#include<stdio.h>
#include "stk.h"
#include "strlib_base_cli.h"
#include "osapi.h"


/*********************************************************************
* @purpose  Get the dim error string associated with the dim code
*
* @param    code      - stk return code
* @param    errBuf    - place to copy the error string
*
* @returns  L7_SUCCESS   - if the err string is copied successfully
* @returns  L7_FAILURE   - if the errBuf is NULL or code is invalid
*
* @end
********************************************************************/

L7_RC_t cliUtilGetStkErrorString(STK_RC_t code, L7_char8* errBuf, L7_uint32 len)
{
  if (!errBuf)
  {
    return L7_FAILURE;
  }

  if (code < STK_SUCCESS || code > STK_LAST_ENTRY)
  {
    return L7_FAILURE;
  }
  
  switch(code)
  {
    case STK_SUCCESS:
      osapiStrncpy(errBuf,pStrInfo_base_stkSuccess,len);
      break;
      
    case STK_FAILURE:
      osapiStrncpy(errBuf,pStrErr_base_stkFailure,len);
      break;

    case STK_IMAGE_DOESNOT_EXIST:
      osapiStrncpy(errBuf,pStrErr_base_stkImageNotExists,len);
      break;

    case STK_INVALID_IMAGE:
      osapiStrncpy(errBuf,pStrErr_base_stkInvalidImage,len);
      break;

    case STK_FILE_SIZE_FAILURE:
      osapiStrncpy(errBuf,pStrErr_base_stkInvalidSize,len);
      break;

    case STK_FILE_SIZE_MISMATCH:
      osapiStrncpy(errBuf,pStrErr_base_stkSizeMismatch,len);
      break;

    case STK_TOO_MANY_IMAGES_IN_STK:
      osapiStrncpy(errBuf,pStrErr_base_stkTooManyImages,len);
      break;

    case STK_STK_EMPTY:
      osapiStrncpy(errBuf,pStrErr_base_stkEmpty,len);
      break;

    case STK_PLATFORM_MISMATCH: 
      osapiStrncpy(errBuf,pStrErr_base_stkPlatformMismatch,len);
      break;

    case STK_INVALID_IMAGE_FORMAT:
      osapiStrncpy(errBuf,pStrErr_base_stkInvalidFormat,len);
      break;

    case STK_NOT_ACTIVATED:
      osapiStrncpy(errBuf,pStrErr_base_stkNotActivated,len);
      break;

    case STK_ACTIVATED:
      osapiStrncpy(errBuf,pStrErr_base_stkActivated,len);
      break;

    case STK_TABLE_IS_FULL:
      osapiStrncpy(errBuf,pStrErr_base_stkTableFull,len);
      break;

    case STK_VALID_IMAGE:
      osapiStrncpy(errBuf,pStrErr_base_stkValid,len);
      break;
      
    default:
      osapiStrncpy(errBuf,pStrErr_base_stkFailure,len);
       
  }
  return L7_SUCCESS;
}


