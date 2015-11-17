/* $Id: arad_chip_regs.h,v 1.7 Broadcom SDK $
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

#ifndef __ARAD_CHIP_REGS_INCLUDED__
/* { */
#define __ARAD_CHIP_REGS_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/SAND_FM/sand_cell.h>
#include <soc/dpp/ARAD/arad_chip_defines.h>
#include <soc/dpp/ARAD/arad_api_general.h>
/* } */

/*************
 * DEFINES   *
 *************/
/* { */

#define  ARAD_NOF_STL_GROUP_CONFIG_REGS          6
#define  ARAD_NOF_STL_GROUP_CONFIG_FLDS          16
#define  ARAD_FAST_REGISTERS_AND_FIELDS_ACCESS

typedef enum
{
  ARAD_REG_NIF_DIRECTION_RX = 0,
  ARAD_REG_NIF_DIRECTION_TX = 1,
  ARAD_REG_NOF_NIF_DIRECTIONS
}ARAD_REG_NIF_DIRECTION;


#define  ARAD_EGQ_CNM_CPID_TO_FC_TYPE_REG_NOF_FLDS 3

#define ARAD_TRANSMIT_DATA_QUEUE_NOF_REGS      7
#define ARAD_REGS_DBUFF_PTR_Q_THRESH_NOF_REGS  8

/*
 * Fabrics registers with bit-per-link, 36 bits
 */
#define  ARAD_NOF_BIT_PER_LINK_REGS              2

#define ARAD_TIMEOUT                            (20*1000)
#define ARAD_MIN_POLLS                          (100)


#define  ARAD_DRAM_NDX_MAX                      (7)
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

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

/*****************************************************
*NAME
* arad_polling
*TYPE:
*  PROC
*DATE:
*  24/10/2011
*FUNCTION:
*  Doing polling till a function gets 1
*INPUT:
*  SOC_SAND_DIRECT:
*    sal_usecs_t    time_out - maximal time for polling
*    int32       min_polls - minimal polls
*    soc_reg_t      reg - the required register
*    int            port - the required port
*    int            index - the required index
*    soc_field_t    field - the required field of the register
*    uint32       expected_value - the expected value
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
uint32
  arad_polling(
    SOC_SAND_IN int   unit,
    SOC_SAND_IN sal_usecs_t time_out,
    SOC_SAND_IN int32    min_polls,
    SOC_SAND_IN soc_reg_t   reg,
    SOC_SAND_IN int32    port,
    SOC_SAND_IN int32    index,
    SOC_SAND_IN soc_field_t field,
    SOC_SAND_IN uint32    expected_value
  );

/*****************************************************
*NAME
* soc_dpp_polling
*TYPE:
*  PROC
*DATE:
*  24/10/2011
*FUNCTION:
*  Doing polling till a function gets 1
*INPUT:
*  SOC_SAND_DIRECT:
*    sal_usecs_t    time_out - maximal time for polling
*    int32       min_polls - minimal polls
*    soc_reg_t      reg - the required register
*    int            port - the required port
*    int            index - the required index
*    soc_field_t    field - the required field of the register
*    uint32       expected_value - the expected value
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
uint32
  soc_dpp_polling(
    int         unit,
    sal_usecs_t time_out,
    int32       min_polls,
    soc_reg_t   reg,
    int32       port,
    int32       index,
    soc_field_t field,
    uint32      expected_value
  );

/*****************************************************
*NAME
* arad_polling
*TYPE:
*  PROC
*DATE:
*  24/10/2011
*FUNCTION:
*  Doing polling till a function gets 1
*INPUT:
*  SOC_SAND_DIRECT:
*    sal_usecs_t    time_out - maximal time for polling
*    int32       min_polls - minimal polls
*    soc_mem_t      mem - the required memory
*    uint32         buff_off - offset within the array mem
*    uint32         array_type - array type of the memory
*    int            index - the required index
*    soc_field_t    field - the required field of the memory
*    uint32       expected_value - the expected value
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
uint32
  arad_mem_polling(
    SOC_SAND_IN int   unit,
    SOC_SAND_IN sal_usecs_t time_out,
    SOC_SAND_IN int32    min_polls,
    SOC_SAND_IN soc_mem_t mem,
    SOC_SAND_IN uint32  buff_off,
    SOC_SAND_IN uint32   array_type,
    SOC_SAND_IN soc_field_t field,
    SOC_SAND_IN uint32    expected_value
  );


/***************************************************** 
* ARAD_FAST_XXX_MECHANISM 
* this mechanism improve performance by saving registers and memory information  in the init. 
*  
*   there are 3 type of mechanisms:
*   reg acces     - save registers info to perform a faster register set/get.
*   field access  - save specific field and register information to perform a faster field set/get.
*   memory access - save specific field and memory information to perform a faster field set/get.
*  
*   to use this mechanism to make sure flag ARAD_FAST_REGISTERS_AND_FIELDS_ACCESS is defined.
*  
*   use the following macros to perform fast call:
*   ARAD_FAST_FIELD_GET
*   ARAD_FAST_FIELD_SET
*  
*   ARAD_FAST_REGISER_GET
*   ARAD_FAST_REGISER_SET
*  
*   ARAD_FAST_MEM_FIELD_GET
*   ARAD_FAST_MEM_FIELD_SET
*  
*   add new field/register to mechanism:
*  
*   add value in enum arad_fast_xxx_type_t that represents the new field/register
*   update arad_fast_xxx_value_init 
*  
*/ 

/* information used to perform fast register call */
typedef struct
{
  soc_reg_t reg;
  int       reg_addr;
  int       block;
  uint8     at;
} ARAD_FAST_REG_CALL_INFO;

/* information used to perform fast field set/get (registers) */
typedef struct
{
  uint16        bp;
  uint16        len;
  soc_field_t   field;
  soc_reg_t     reg;
} ARAD_FAST_FIELD_CALL_INFO;

/* information used to perform fast field set/get (memory) */
typedef struct
{
  int            bp;
  int            len;
  soc_field_t    field;
  uint32         flags;
  soc_mem_t      mem;
  soc_mem_info_t *meminfo;
} ARAD_FAST_MEMORY_CALL_INFO;


/* enums that represents the register/field that has fast access */
typedef enum arad_fast_regs_type_e
{
    ARAD_FAST_REG_IHP_MACT_CPU_REQUEST_REQUEST,
    ARAD_FAST_REG_IHP_MACT_REPLY_FIFO_REPLY_FIFO_ENTRY_COUNT,
    ARAD_FAST_REG_IHP_MACT_REPLY,
    ARAD_FAST_REG_IHP_MACT_CPU_REQUEST_TRIGGER,
    ARAD_FAST_REG_IHP_ISA_INTERRUPT_REGISTER,
    ARAD_FAST_REG_IHP_ISB_INTERRUPT_REGISTER,
    ARAD_FAST_REG_IHP_ISA_MANAGEMENT_UNIT_FAILURE,
    ARAD_FAST_REG_IHP_ISB_MANAGEMENT_UNIT_FAILURE,
    ARAD_FAST_REG_TYPE_LAST		    /* Last */
        
} arad_fast_regs_type_t;


typedef enum arad_fast_field_type_e
{
    ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_QUALIFIER,
    ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_SELF,
    ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_AGE_PAYLOAD,
    ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_PAYLOAD_IS_DYNAMIC,
    ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_PAYLOAD,
    ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_STAMP,
    ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_COMMAND,
    ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_PART_OF_LAG,
    ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_MFF_IS_KEY,
    ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_MFF_KEY,
    ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_SUCCESS,
    ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_REASON,
    ARAD_FAST_FIELD_IHP_ISA_INTERRUPT_REGISTER_ONE_ISA_MANAGEMENT_COMPLETED,
    ARAD_FAST_FIELD_IHP_ISB_INTERRUPT_REGISTER_ONE_ISB_MANAGEMENT_COMPLETED,
    ARAD_FAST_FIELD_IHP_ISA_MANAGEMENT_UNIT_FAILURE_ISA_MNGMNT_UNIT_FAILURE_VALID,
    ARAD_FAST_FIELD_IHP_ISB_MANAGEMENT_UNIT_FAILURE_ISB_MNGMNT_UNIT_FAILURE_VALID,
    ARAD_FAST_FIELD_IHP_ISA_MANAGEMENT_UNIT_FAILURE_ISA_MNGMNT_UNIT_FAILURE_REASON,
    ARAD_FAST_FIELD_IHP_ISB_MANAGEMENT_UNIT_FAILURE_ISB_MNGMNT_UNIT_FAILURE_REASON,
    ARAD_FAST_FIELD_TYPE_LAST		    /* Last */
        
} arad_fast_field_type_t;

typedef enum arad_fast_mem_field_type_e
{    
    ARAD_FAST_MEM_TYPE_IHP_PP_PORT_INFO_PP_PORT_OUTER_HEADER_START,
    ARAD_FAST_MEM_TYPE_LAST		    /* Last */
        
} arad_fast_mem_field_type_t;




extern ARAD_FAST_REG_CALL_INFO              g_fast_reg_info_table[BCM_MAX_NUM_UNITS][ARAD_FAST_REG_TYPE_LAST];
extern ARAD_FAST_FIELD_CALL_INFO            g_fast_field_info_table[BCM_MAX_NUM_UNITS][ARAD_FAST_FIELD_TYPE_LAST];
extern ARAD_FAST_MEMORY_CALL_INFO           g_fast_mem_info_table[BCM_MAX_NUM_UNITS][ARAD_FAST_MEM_TYPE_LAST];



int arad_fast_reg_get(int unit, soc_reg_t reg, int acc_type, int addr, int block, soc_reg_above_64_val_t data);
int arad_fast_reg_set(int unit, soc_reg_t reg, int acc_type, int addr, int block, soc_reg_above_64_val_t data);

void    arad_fast_mem_field_set(int bp, int len,uint32 flags, soc_mem_info_t *meminfo, uint32 *entbuf, uint32 *fldbuf);
uint32* arad_fast_mem_field_get(int bp, int len, uint32 flags, soc_mem_info_t *meminfo, const uint32 *entbuf, uint32 *fldbuf);

void arad_fast_regs_and_fields_access_init(int unit);


/* API MACROS for the ARAD_FAST_XXX mechanism */
#define ARAD_FAST_FIELD_GET(field_id, reg_val, fld_value)       SOC_REG_ABOVE_64_CLEAR(fld_value);\
                                                                SHR_BITCOPY_RANGE(fld_value, 0, reg_val, g_fast_field_info_table[unit][field_id].bp,g_fast_field_info_table[unit][field_id].len);

#define ARAD_FAST_FIELD_SET(field_id, reg_val, fld_value)       SHR_BITCOPY_RANGE(reg_val, g_fast_field_info_table[unit][field_id].bp, fld_value, 0, g_fast_field_info_table[unit][field_id].len);


#define ARAD_FAST_REGISER_SET(reg_id, reg_val)                  arad_fast_reg_set(unit, g_fast_reg_info_table[unit][reg_id].reg, g_fast_reg_info_table[unit][reg_id].at,\
                                                                g_fast_reg_info_table[unit][reg_id].reg_addr, g_fast_reg_info_table[unit][reg_id].block, reg_val);

#define ARAD_FAST_REGISER_GET(reg_id, reg_val)                  arad_fast_reg_get(unit, g_fast_reg_info_table[unit][reg_id].reg, g_fast_reg_info_table[unit][reg_id].at,\
                                                                g_fast_reg_info_table[unit][reg_id].reg_addr, g_fast_reg_info_table[unit][reg_id].block, reg_val);



#define ARAD_FAST_MEM_FIELD_SET(mem_fld_id, mem_val, fld_val)   arad_fast_mem_field_set(g_fast_mem_info_table[unit][mem_fld_id].bp, g_fast_mem_info_table[unit][mem_fld_id].len,\
                                                                g_fast_mem_info_table[unit][mem_fld_id].flags, g_fast_mem_info_table[unit][mem_fld_id].meminfo, mem_val, &fld_val);

#define ARAD_FAST_MEM_FIELD_GET(mem_fld_id, mem_val, fld_val)   arad_fast_mem_field_get(g_fast_mem_info_table[unit][mem_fld_id].bp, g_fast_mem_info_table[unit][mem_fld_id].len,\
                                                                g_fast_mem_info_table[unit][mem_fld_id].flags, g_fast_mem_info_table[unit][mem_fld_id].meminfo, mem_val, fld_val);

#include <soc/dpp/SAND/Utils/sand_footer.h>
#endif

