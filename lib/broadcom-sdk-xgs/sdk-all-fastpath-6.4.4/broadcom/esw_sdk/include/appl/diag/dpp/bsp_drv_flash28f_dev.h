/* $Id: bsp_drv_flash28f_dev.h,v 1.2 Broadcom SDK $
 * $Copyright: Copyright 2012 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
*/

#ifndef __BSP_DRV_FLASH28F_DEV_INCLUDED
#define __BSP_DRV_FLASH28F_DEV_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#if !DUNE_BCM
#ifndef SAND_LOW_LEVEL_SIMULATION
#include "vxWorks.h"
#endif
#endif

/* defines */

#define FLASH_28F_WIDTH 4

#ifndef FLASH_28F128
#define FLASH_28F128      0x18    /* device code 28F128 */
#endif

#ifndef FLASH_28F640
#define FLASH_28F640      0x17    /* device code 28F640 */
#endif

#ifndef FLASH_28F320
#define FLASH_28F320      0x16    /* device code 28F320 */
#endif

#if     (FLASH_28F_WIDTH == 1)
#define FLASH_28F_DEF       unsigned char
#define FLASH_28F_CAST      (unsigned char *)
#endif  /* FLASH_28F_WIDTH */

#if     (FLASH_28F_WIDTH == 2)
#define FLASH_28F_DEF       unsigned short
#define FLASH_28F_CAST      (unsigned short *)
#endif  /* FLASH_28F_WIDTH */

#if     (FLASH_28F_WIDTH == 4)
#define FLASH_28F_DEF       unsigned long
#define FLASH_28F_CAST      (unsigned long *)
#endif  /* FLASH_28F_WIDTH */


#ifndef SAND_LOW_LEVEL_SIMULATION

/* function declarations */

typedef struct
{
  /*
   * This hold s the structure version.
   * Version 1 - BSP_FLASH28F_DEF_INF_V1:
   *   Holds: (1) unsigned long f_adrs
   *          (2) unsigned char f_adrs_jump
   *          (3) unsigned char f_type
   *          (4) unsigned long f_sector_size
   * Version 2 - BSP_FLASH28F_DEF_INF_V2:
   *   TBD in the future.
   * NOTE:
   * (a) In order to support older application version in newer boot, every
   *     structure version can not delete parameters, only to add.
   * (b) Newer boot version may use only the structure section indicated by the version number.
   */
  unsigned char struct_version;

  /* ****            **** */
  /* Structure parameters */
  /* Version 1 starts  {  */
  /* ****            **** */
  unsigned long f_adrs;        /*Base address of FLASH*/
  unsigned char f_adrs_jump;   /*E.g. FLASH is 16 bits.*/
                               /* + If we access it by N, N+2, N+4, than put here ONE.*/
                               /* + If we access it by N, N+4, N+8, than put here TWO.*/
  unsigned int  flash_width;   /* word size */
  unsigned char f_type;        /*FLASH type. Supported: FLASH_28F640, FLASH_28F320*/
  unsigned long f_sector_size; /*Sector Byte size*/
  /* ****            **** */
  /* Version 1 ends    }  */
  /* ****            **** */
} BSP_FLASH28F_DEF_INF;

typedef enum
{
  BSP_FLASH28F_DEF_INF_UN_DEF = 0,
  BSP_FLASH28F_DEF_INF_V1 = 1,

  /*Last One*/
  BSP_FLASH28F_DEF_INF_NOF_VERSIONS
} BSP_FLASH28F_DEF_INF_VERSION;

IMPORT  UINT8 bspDrvFlash28fTypeGet_28(void) ;
IMPORT  UINT8 bspDrvFlash28fTypeGet_29(void) ;

IMPORT  STATUS
  bspDrvFlash28fGet(
    const BSP_FLASH28F_DEF_INF *flash_info,  /*Flash information*/
          char          *string,      /* where to copy flash memory      */
          int           strLen,       /* maximum number of bytes to copy */
          int           offset        /* byte offset into flash memory   */
  ) ;

IMPORT  STATUS  bspDrvFlash28fSet (
    char *string, int strLen, int offset,
    int (*poll_indication)()    /* Function to call when polling the Flash.*/
                                /* If NULL then call no function.          */
) ;
IMPORT  unsigned long sysGetFlashBase(void) ;
STATUS
  bspDrvFlash28fKick(
    const BSP_FLASH28F_DEF_INF *flash_info,  /*Flash information*/
          int do_poll_indication,     /* Flag. Non-Zero: print progress indication.*/
          int sector                  /* Number of sector to kick (starting from */
                                      /* zero. If negative then erase the whole  */
                                      /* memory space. If number of sector is    */
                                      /* larger than the maximal for that flash  */
                                      /* type, last sector is erased.            */
  ) ;
STATUS
  bspDrvFlash28fErase(
    const BSP_FLASH28F_DEF_INF *flash_info,  /*Flash information*/
          int do_poll_indication,     /* Flag. Non-Zero print progress indication*/
                                      /* in percentage.                          */
          int sector                  /* Number of sector to erase (starting from*/
                                      /* zero. If negative then erase the whole  */
                                      /* memory space. If number of sector is    */
                                      /* larger than the maximal for that flash  */
                                      /* type, last sector is erased.            */
  ) ;

IMPORT  STATUS
  bspDrvFlash28fWrite(
    const BSP_FLASH28F_DEF_INF *flash_info,  /*Flash information*/
          FLASH_28F_DEF *pFB,       /* string to be copied; use <value> if NULL */
          int       size,       /* size to program in bytes */
          int       offset,     /* byte offset into flash memory */
          FLASH_28F_DEF value,       /* value to program */
          int (*poll_indication)(unsigned long current_work_load) /* Function to call when polling the Flash.*/
                                      /* If NULL then call no function.          */
    ) ;
#endif /*SAND_LOW_LEVEL_SIMULATION*/

#ifdef __cplusplus
}
#endif

#endif /* __BSP_DRV_FLASH28F_DEV_INCLUDED */
