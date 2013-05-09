/*********************************************************************
*
* (C) Copyright Broadcom Corporation 1999-2007
*
**********************************************************************
*
* @filename   fftp_api.h
*
* @purpose    Component code for FASTPATH File Transfer Protocol
*
* @component  fftp
*
* @comments   
*
* @create     9/11/2003
*
* @author     djohnson
* @end
*
**********************************************************************/

#ifndef FFTP_API_H
#define FFTP_API_H

#include "l7_common.h"

#include "fftp_exports.h"

typedef enum
{
  FFTP_SUCCESS = 1,
  FFTP_END_OF_FILE,
  FFTP_FILE_NOT_FOUND,
  FFTP_FAILURE,
} fftpResponseCode_t;

L7_RC_t fftpFileRegister(L7_uchar8 *fileName, L7_uchar8 *filePath);
L7_RC_t fftpFileDeregister(L7_uchar8 *fileName);

L7_RC_t fftpFileRequest(L7_uchar8 *fileName, L7_uint32 unitNum,
                                   L7_uint32 slotNum, L7_uint32 offset,
                                   L7_uint32 numBytes,
                                   L7_uint32 segmentSize, L7_uchar8 *localFileName);


#endif                          
