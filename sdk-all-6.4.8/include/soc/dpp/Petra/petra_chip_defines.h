/* $Id$
 * $Copyright: Copyright 2015 Broadcom Corporation.
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


#ifndef __SOC_PETRA_CHIP_DEFINES_INCLUDED__
/* { */
#define __SOC_PETRA_CHIP_DEFINES_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/TMC/tmc_api_reg_access.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PETRA_EXPECTED_CHIP_TYPE     0xFA080
#define SOC_PETRA_EXPECTED_CHIP_VER_01   0x01
#define SOC_PETRA_EXPECTED_CHIP_VER      0x01

/* $Id$
 *	Soc_petra-B
 */
#define SOC_PB_EXPECTED_CHIP_TYPE     0xFA100

#define SOC_PA_TOTAL_SIZE_OF_REGS        0x15bf0

#ifdef LINK_PB_LIBRARIES
#define SOC_PB_TOTAL_SIZE_OF_REGS        0x20000
#else
#define SOC_PB_TOTAL_SIZE_OF_REGS        SOC_PA_TOTAL_SIZE_OF_REGS
#endif


/*
 * Number of local ports in a device
 */
#define SOC_PETRA_NOF_LOCAL_PORTS 80

/* Device Version managment { */


/************************************************************************/
/* Soc_petra-A revisions                                                    */
/************************************************************************/


#define SOC_PETRA_REV_A1_OR_BELOW  \
  (soc_petra_revision_get(unit) <= SOC_PETRA_REV_A1)
#define SOC_PETRA_REV_A3_OR_BELOW  \
  (soc_petra_revision_get(unit) <= SOC_PETRA_REV_A3)

#define SOC_PETRA_REV_ABOVE_A1  \
  (!(SOC_PETRA_REV_A1_OR_BELOW))

/************************************************************************/
/* Soc_petra-B revisions                                                    */
/************************************************************************/

#define SOC_PB_REV_A0_OR_BELOW  \
  (soc_petra_revision_get(unit) == SOC_PB_REV_A0)
#define SOC_PB_REV_A1_OR_BELOW  \
  (soc_petra_revision_get(unit) <= SOC_PB_REV_A1)

#define SOC_PB_REV_ABOVE_A0  \
  (!(SOC_PB_REV_A0_OR_BELOW))
#define SOC_PB_REV_ABOVE_A1  \
  (!(SOC_PB_REV_A1_OR_BELOW))

typedef enum
{
  /*
   *  Soc_petra revision: A-0
   */
  SOC_PETRA_REV_A0=0,
  /*
   *  Soc_petra revision: A-1
   */
  SOC_PETRA_REV_A1=1,
  /*
   *  Soc_petra revision: A-2
   */
  SOC_PETRA_REV_A2=2,
  /*
   *  Soc_petra revision: A-3
   */
  SOC_PETRA_REV_A3=3,
  /*
   *  Soc_petra revision: A-4
   */
  SOC_PETRA_REV_A4=4,
  /*
   *  Soc_petra-B revision: A-0
   */
  SOC_PB_REV_A0=5,
  /*
   *  Soc_petra-B revision: A-1
   */
  SOC_PB_REV_A1=6,
  /*
   *  Soc_petra-B revision: B-0
   */
  SOC_PB_REV_B0=7,
  /*
   *  Total number of Soc_petra device revisions.
   */
  SOC_PETRA_NOF_REVS
}SOC_PETRA_REV;


/* Device Version managment } */



/* } */

/*************
 *  MACROS   *
 *************/
/* { */


/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

typedef SOC_TMC_REG_ADDR                                       SOC_PETRA_REG_ADDR;
typedef SOC_TMC_REG_FIELD                                      SOC_PETRA_REG_FIELD;
typedef SOC_TMC_REG_INFO                                       SOC_PETRA_REG_INFO;

typedef struct
{
  uint32       base;       /* table start address */
  uint32       size;       /* table size (number of lines) */
  uint16       width_bits; /* the width of each line in bits */
} __ATTRIBUTE_PACKED__ SOC_PETRA_TBL_ADDR;

typedef struct
{
  uint16      msb;
  uint16      lsb;
} __ATTRIBUTE_PACKED__ SOC_PETRA_TBL_FIELD;

typedef struct
{
  uint32
    ticks_per_sec;

} __ATTRIBUTE_PACKED__ SOC_PETRA_CHIP_DEFINITIONS;

/* } */

/*************
 * GLOBALS   *
 *************/
/* { */

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

/*
 *  Read soc_petra revision field.
 *  Notes:
 *  1. The function does not take device semaphore.
 *  2. The function does not check the register access success/fail indication.
 */
uint32
  soc_petra_revision_fld_get(
    SOC_SAND_IN  int unit
  );

uint32
  soc_petra_chip_defines_init(void);

/*
 *   Clock Parameters. {
 */

/*****************************************************
*NAME
*  soc_petra_chip_time_to_ticks
*TYPE:
*  PROC
*DATE:
*  23/12/2007
*FUNCTION:
*  This procedure is used to convert from time values to machine
*  clocks value.
*INPUT:
*  SOC_SAND_DIRECT:
*     SOC_SAND_IN  uint32        time_value -
*       the value of the time to convert to ticks.
*     SOC_SAND_IN  uint8       is_nano -
*        whether the time is given in nanoseconds or in milliseconds
*     SOC_SAND_IN  uint32       result_granularity -
*        the 'resolution' of the result.
*     SOC_SAND_IN  uint8       is_round_up -
         If TRUE, the result is rounded up, otherwise - down
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*  SOC_SAND_INDIRECT:
*     SOC_SAND_OUT uint32        *result -
*         the result, the number of machine ticks for the given time.
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_chip_time_to_ticks(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  uint32        time_value,
    SOC_SAND_IN  uint8       is_nano,
    SOC_SAND_IN  uint32       result_granularity,
    SOC_SAND_IN  uint8       is_round_up,
    SOC_SAND_OUT uint32        *result
  );

/*****************************************************
*NAME
*  soc_petra_ticks_to_time
*TYPE:
*  PROC
*DATE:
*  23/12/2007
*FUNCTION:
*  TThis procedure is used to convert from machine to
*  time values.
*INPUT:
*  SOC_SAND_DIRECT:
*     SOC_SAND_IN  uint32        time_value -
*       the value of the time to convert to ticks.
*     SOC_SAND_IN  uint8       is_nano -
*        whether the time is given in nanoseconds or in milliseconds
*     SOC_SAND_IN  uint32       result_granularity -
*        the 'resolution' of the result.
*     SOC_SAND_IN  uint8       is_round_up -
         If TRUE, the result is rounded up, otherwise - down
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*  SOC_SAND_INDIRECT:
*     SOC_SAND_OUT uint32        *result -
*         the result, the number of machine ticks for the given time.
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_ticks_to_time(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  uint32        ticks_value,
    SOC_SAND_IN  uint8       is_nano,
    SOC_SAND_IN  uint32       result_granularity,
    SOC_SAND_OUT uint32        *result
  );

uint32
  soc_petra_chip_ticks_per_sec_get(
    SOC_SAND_IN int unit
  );

void
  soc_petra_chip_kilo_ticks_per_sec_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32  clck_freq_khz
  );

uint32
  soc_petra_chip_kilo_ticks_per_sec_get(
    SOC_SAND_IN int unit
  );

uint32
  soc_petra_chip_mega_ticks_per_sec_get(
    SOC_SAND_IN int unit
  );

/*
 *  END Clock Parameters. }
 */

void
  soc_petra_PETRA_REG_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_REG_INFO *info
  );

/*
 *  Get Soc_petra device revision
 */
SOC_PETRA_REV
  soc_petra_revision_get(
    SOC_SAND_IN  int  unit
  );

const char*
  soc_petra_PETRA_REV_to_string(
    SOC_SAND_IN SOC_PETRA_REV enum_val
  );

void
  soc_petra_PETRA_REG_INFO_print(
    SOC_SAND_IN SOC_PETRA_REG_INFO *info
  );

/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>


/* } __SOC_PETRA_CHIP_DEFINES_INCLUDED__*/
#endif
