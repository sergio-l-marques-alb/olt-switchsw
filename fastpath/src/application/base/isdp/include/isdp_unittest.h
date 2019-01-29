/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2007
*
**********************************************************************
* @filename  isdp_unittest.h
*
* @purpose   isdp unit test utilities
*
* @component isdp
*
* @comments
*
* @create    06/02/2008
*
* @author    dgaryachy
*
* @end
*
**********************************************************************/
#ifndef ISDP_UNITTEST_H
#define ISDP_UNITTEST_H

#include "isdp_include.h"

L7_RC_t isdpUnitTestPacketCreate(L7_uint32 intIfNum, L7_char8 *deviceId,
                                 L7_char8 holdTime, L7_short16 etype,
                                 L7_char8 version, L7_uint32 addToLlcLength,
                                 L7_netBufHandle *pBufHandle);
L7_RC_t isdpUnitTestAddNeighbors(L7_uint32 number, L7_char8 holdtime);
void isdpUnitTestDatabasePositiveTests(void);
void isdpUnitTestDatabaseBoundaryTests(void);
void isdpUnitTestDatabaseNegativeTests(void);
void isdpUnitTestApi(void);
L7_RC_t isdpUnitTestSend(L7_uint32 intIfNum, L7_uint32 number, L7_short16 etype,
                         L7_char8 version, L7_char8 holdtime, L7_uint32 addToLlcLength
                         );

#endif /* ISDP_UNITTEST_H*/
