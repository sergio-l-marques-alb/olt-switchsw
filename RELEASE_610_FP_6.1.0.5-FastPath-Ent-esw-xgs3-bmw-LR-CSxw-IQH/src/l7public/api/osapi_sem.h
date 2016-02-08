/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    osapi_sem.h
*
* @purpose     I/O semaphore implementation
*
* @component   osapi
*
* @create      03/24/05
*
* @author      
*
* @end
*
*********************************************************************/

#ifndef OSAPISEM_HEADER
#define OSAPISEM_HEADER



/**************************************************************************
*
* @purpose  Give the IO Semaphore
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if semaphore is invalid
*
* @comments
*
* @end
*
*************************************************************************/

L7_RC_t osapiIOSemaGive (void);

/**************************************************************************
*
* @purpose  Take the IO Semaphore
*
* @param    timeout   @b{(input)}  time to wait in ticks, forever (-1), or no wait (0)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if timeout or if semaphore does not exist
*
* @comments
*
* @end
*
*************************************************************************/

L7_RC_t osapiIOSemaTake (L7_int32 timeout);

#endif

