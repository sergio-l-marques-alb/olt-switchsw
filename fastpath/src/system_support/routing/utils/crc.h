/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename         crc.h
 *
 * @purpose          Defines used by the CRC generator
 *
 * @component        Routing Utils Component
 *
 * @comments
 *
 * @create           
 *
 * @author
 *
 * @end                                                                        
 *
 * ********************************************************************/        +
#ifndef crc_h
#define crc_h


/*
 * CRC-32 polynomial is
 *       x32 + x26 + x23 + x22 + x16 + x12 + x11 + x10
 *           + x8  + x7  + x5  + x4  + x2  + x1  + x0
 *
 * NOTE: I.363 specifies transmitting the one's complement
 *       of the CRC value
 *
 *
 * CRC-32 checking (over data+CRC) should yield the following result
 *       x31 + x30 + x26 + x25 + x24 + x18 + x15 + x14 + x12
 *     + x11 + x10 + x8  + x6  + x5  + x4  + x3  + x1  + x0
 *
 * The following polynomial and result assume that the
 * most significant bit of each byte is sent/received first
 * (this is true for ATM - not for HDLC, etc).
 */

#define CRC32_POLYNOMIAL      0x04c11db7L



/*
 * CRC-10
 * Used in SAR-3/4 and OAM cells
 *
 * CRC-10 polynomial is
 *   x10 + x9 + x5 + x4 + x + 1
 *
 * The following polynomial and result assume that the
 * most significant bit of each byte is sent/received first
 * (this is true for ATM - not for HDLC, etc).
 */

#define CRC10_POLYNOMIAL     0x8cc0      /* shifted left by 6 */

/*
 * CRC-8 polynomial is
 *       x8 + x2 + x1 + 1
 *
 * NOTE: ANSI T1.646-1995
 *
 * The following polynomial and result assume that the
 * most significant bit of each byte is sent/received first.
 */

#define CRC8_POLYNOMIAL  0x107  /* x8 + x2 + x + 1 */
#define CRC8_RESULT      0x00

#endif

/* --- end of file crc.h --- */

