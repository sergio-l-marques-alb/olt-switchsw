/* $Id: chip_sim_em.c,v 1.9 Broadcom SDK $
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

#ifdef _MSC_VER
/*
 * In Microsoft:
 * cancel the warning of internal int to unsigned
 * The S/GET_FLD_IN_PLACE cause it
 */
#pragma warning(disable:4308)
#endif /* _MSC_VER */


#include <soc/dpp/SAND/Utils/sand_os_interface.h>
/*#include <soc/dpp/SAND/SAND_FM/sand_indirect_access.h>*/
#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/dpp_wb_engine.h>
#include "chip_sim_em.h"
#include "chip_sim_mem.h"
/* #include "chip_sim_log.h"*/
#include "chip_sim.h"


#include <string.h>
#define CHIP_SIM_EM_BYTE_FILLER  0x0000

/* This should be replaced by the standard structure. */
CHIP_SIM_EM_BLOCK
Soc_pb_em_blocks[SOC_PB_EM_TABLE_ID_LAST + 1];

CHIP_SIM_EM_BLOCK
Arad_em_blocks[ARAD_EM_TABLE_ID_LAST + 1];


struct
{

  CHIP_SIM_EM_BLOCK* blocks;

} Em;


/* 
 * these two function used to be supplied as callback to the multicast created in this module. 
 * these callback are now sent as NULL to multicast create. 
 * the module was using these callbacks internally (generally not recommended even before the multi 
 * set DS has changed). to solve the problem, these function were left here and are used internally. 
 */
uint32
chip_sim_em_key_map_set_entry(
                SOC_SAND_IN int            unit,
                SOC_SAND_IN uint32         sec_hanlde,
                SOC_SAND_INOUT  uint8      *buffer,
                SOC_SAND_IN  uint32        offset,
                SOC_SAND_IN  uint32        len,
                SOC_SAND_IN uint8          *data
                )
{
  sal_memcpy(
    buffer + (offset * len),
    data,
    len
    );
  return SOC_SAND_OK;
}
STATIC uint32
chip_sim_em_key_map_get_entry(
                SOC_SAND_IN  int            unit,
                SOC_SAND_IN  uint32         sec_hanlde,
                SOC_SAND_IN  uint8         *buffer,
                SOC_SAND_IN  uint32         offset,
                SOC_SAND_IN  uint32         len,
                SOC_SAND_OUT uint8*          const data
                )
{
  sal_memcpy(
    data,
    buffer + (offset * len),
    len
    );
  return SOC_SAND_OK;
}


uint32
  chip_sim_exact_match_entry_add_unsafe(
          SOC_SAND_IN  int         unit,
          SOC_SAND_IN  uint32      offset, /* ISEM,LEM,ESEM,SVEM */
          SOC_SAND_IN  uint32      *key_ptr,
          SOC_SAND_IN  uint32     key_size,
          SOC_SAND_IN  uint32      *data_ptr,
          SOC_SAND_IN  uint32     data_size,
          SOC_SAND_OUT uint8     *success
            )
{

  uint32
    res = SOC_SAND_OK;
  uint8
    key8[CHIP_SIM_TABLE_KEY_MAX_SIZE],
    data8[CHIP_SIM_TABLE_DATA_MAX_SIZE];
  SOC_SAND_MULTI_SET_INFO
    *multi_set;
  uint32
    key_id, id, ref_count=0;
  uint8
    first_appear,
    is_success;
  uint32
    status = OK ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  *success = TRUE;
  status = chip_sim_em_offset_to_table_id (offset, &id);
  if (status != OK)
  {
    GOTO_FUNC_EXIT_POINT;
  }

  multi_set = &Em.blocks[id].multi_set[unit][0];
  res = soc_sand_U32_to_U8(
    key_ptr,
    key_size,
    key8
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, FUNC_EXIT_POINT);

  res = soc_sand_multi_set_member_add(
    unit,
    multi_set,
    key8,
    &key_id,
    &first_appear,
    &is_success
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, FUNC_EXIT_POINT);


  
  if (!is_success)
  {
    *success = FALSE;
    status = ERROR;
    GOTO_FUNC_EXIT_POINT;
  }

  /* Get the insertion index */
  if (first_appear &&  (key_id == SOC_SAND_HASH_TABLE_NULL))  {
      res = soc_sand_multi_set_member_lookup(
        unit,
        multi_set,
        key8,
        &key_id,
        &ref_count
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 21, FUNC_EXIT_POINT);

  }
  res = soc_sand_U32_to_U8(
    data_ptr,
    data_size,
    data8
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, FUNC_EXIT_POINT);

 
  status = chip_sim_em_key_map_set_entry(
    unit, 
    0, 
    Em.blocks[id].base[unit],
    key_id * Em.blocks[id].data_nof_bytes,
    Em.blocks[id].data_nof_bytes,
    data8
    );
  if (status != OK)
  {
    GOTO_FUNC_EXIT_POINT;
  }

FUNC_EXIT_POINT:
  return status;
}

uint32
chip_sim_exact_match_entry_get_unsafe(
        SOC_SAND_IN  int         unit,
        SOC_SAND_IN  uint32      offset,
        SOC_SAND_IN  uint32      *key_ptr,
        SOC_SAND_IN  uint32     key_size,
        SOC_SAND_OUT  uint32     *data_ptr,
        SOC_SAND_IN  uint32     data_size,
        SOC_SAND_OUT  uint8    *found
                                  )
{
  uint32
    res = SOC_SAND_OK;
  uint8
    key8[CHIP_SIM_TABLE_KEY_MAX_SIZE], 
    data8[CHIP_SIM_TABLE_DATA_MAX_SIZE];
  SOC_SAND_MULTI_SET_INFO
    *multi_set;
  uint32
   id, indx, ref_count;
  uint32
    status = OK ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  *found = FALSE;
  sal_memset(data8, 0x0, CHIP_SIM_ISEM_KEY_SIZE * sizeof(uint8));
  sal_memset(key8, 0x0, CHIP_SIM_ISEM_KEY_SIZE * sizeof(uint8));

  status = chip_sim_em_offset_to_table_id (offset, &id);
  if (status != OK)
  {
    GOTO_FUNC_EXIT_POINT;
  }

  multi_set = &Em.blocks[id].multi_set[unit][0];
  res = soc_sand_U32_to_U8(
    key_ptr,
    key_size,
    key8
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, FUNC_EXIT_POINT);

  res = soc_sand_multi_set_member_lookup(
    unit,
    multi_set,
    key8,
    &indx,
    &ref_count
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, FUNC_EXIT_POINT);

  if (indx == SOC_SAND_MULTI_SET_NULL)
  {
    *found = FALSE;
    GOTO_FUNC_EXIT_POINT;
  }
  *found = TRUE;
  status = chip_sim_em_key_map_get_entry(
              unit, 
              0, 
              Em.blocks[id].base[unit],
              indx * Em.blocks[id].data_nof_bytes ,
              Em.blocks[id].data_nof_bytes,
              data8
              );
  if (status != OK)
  {
    GOTO_FUNC_EXIT_POINT;
  }
  res = soc_sand_U8_to_U32(
                data8,
                Em.blocks[id].data_nof_bytes,
                data_ptr);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, FUNC_EXIT_POINT);

 
FUNC_EXIT_POINT:
  return status;
}

uint32
soc_sand_exact_match_entry_get_by_index_unsafe(
  SOC_SAND_IN  int         unit,
  SOC_SAND_IN  uint32      tbl_offset,
  SOC_SAND_IN  uint32      entry_offset,
  SOC_SAND_OUT  uint32      *key_ptr,
  SOC_SAND_IN  uint32     key_size,
  SOC_SAND_OUT  uint32     *data_ptr,
  SOC_SAND_IN  uint32     data_size,
  SOC_SAND_OUT  uint8    *found
  )
{
  uint32
    res = SOC_SAND_OK;
  uint8
    key8[CHIP_SIM_TABLE_KEY_MAX_SIZE], 
    data8[CHIP_SIM_TABLE_DATA_MAX_SIZE];
  SOC_SAND_MULTI_SET_INFO
    *multi_set;
  uint32
   id, ref_count;
 
  uint32
    status = OK ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  *found = FALSE;

  status = chip_sim_em_offset_to_table_id (tbl_offset, &id);
  if (status != OK)
  {
    GOTO_FUNC_EXIT_POINT;
  }

  multi_set = &Em.blocks[id].multi_set[unit][0];

  res = soc_sand_multi_set_get_by_index(
    unit,
    multi_set,
    entry_offset,
    key8,
    &ref_count
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, FUNC_EXIT_POINT);

  if (ref_count == 0)
  {
    *found = FALSE;
    GOTO_FUNC_EXIT_POINT;
  }
  *found = TRUE;
  status = chip_sim_em_key_map_get_entry(
              unit, 
              0, 
              Em.blocks[id].base[unit],
              entry_offset * Em.blocks[id].data_nof_bytes ,
              Em.blocks[id].data_nof_bytes,
              data8
              );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, FUNC_EXIT_POINT);

  res = soc_sand_U8_to_U32(
                data8,
                Em.blocks[id].data_nof_bytes,
                data_ptr);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, FUNC_EXIT_POINT);

    res = soc_sand_U8_to_U32(
                key8,
                key_size,
                key_ptr);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, FUNC_EXIT_POINT);

 
FUNC_EXIT_POINT:
  return status;
}


uint32
chip_sim_exact_match_entry_remove_unsafe(
            SOC_SAND_IN  int         unit,
            SOC_SAND_IN  uint32      offset,
            SOC_SAND_IN  uint32      *key_ptr,
            SOC_SAND_IN  uint32     key_size
                                        )
{
  uint32
    id, data_indx;
  uint8
    is_last;
  uint32
    res = SOC_SAND_OK;
  uint8
    key8[CHIP_SIM_TABLE_KEY_MAX_SIZE];
  SOC_SAND_MULTI_SET_INFO
    *multi_set;

  uint32
    status = OK ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  status = chip_sim_em_offset_to_table_id (offset, &id);
  if (status != OK)
  {
    GOTO_FUNC_EXIT_POINT;
  }

  res = soc_sand_U32_to_U8(
    key_ptr,
    key_size,
    key8
    );

  SOC_SAND_CHECK_FUNC_RESULT(res, 10, FUNC_EXIT_POINT);
  multi_set = &Em.blocks[id].multi_set[unit][0];
  res = soc_sand_multi_set_member_remove(
                          unit,
                          multi_set,
                          key8,
                          &data_indx,
                          &is_last);

  SOC_SAND_CHECK_FUNC_RESULT(res, 10, FUNC_EXIT_POINT);
 
FUNC_EXIT_POINT:
  return status;
}

/*****************************************************
*NAME
*  chip_sim_em_init_chip_specifics
*Programmer name  : Hana Gutman
*TYPE: PROC
*DATE: 15-Dec-09 19:02:39
*FUNCTION:
*  Clear from the code.
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    None.
*  EM:
*    (1) CHIP_SIM_INDIRECT_BLOCK* indirect_blocks
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
void
chip_sim_em_init_chip_specifics(
                    SOC_SAND_DEVICE_TYPE device_type,
                    CHIP_SIM_EM_BLOCK* em_blocks
                    )
{
  int
    i;
  CHIP_SIM_EM_BLOCK*
    block_p;

  Em.blocks  =  em_blocks;


  if(device_type == SOC_SAND_DEV_ARAD) {
    sal_memset(Em.blocks, 0x0, sizeof(CHIP_SIM_EM_BLOCK) * ARAD_EM_TABLE_ID_LAST+1);
    Em.blocks[ARAD_EM_TABLE_ID_LAST].read_result_address = INVALID_ADDRESS;
  }

  for (block_p=Em.blocks; block_p->read_result_address!=INVALID_ADDRESS; block_p++)
  {
    for (i=0; i<CHIP_SIM_NOF_CHIPS; i++)
    {
      block_p->base[i] = NULL ;
    }
  }

  /* for arad do the init */
  if(device_type == SOC_SAND_DEV_ARAD) {
   
    Em.blocks[ARAD_EM_TABLE_LEM].offset = ARAD_CHIP_SIM_LEM_BASE;
    Em.blocks[ARAD_EM_TABLE_LEM].key_size = ARAD_CHIP_SIM_LEM_KEY;
    Em.blocks[ARAD_EM_TABLE_LEM].data_nof_bytes = ARAD_CHIP_SIM_LEM_PAYLOAD;
    Em.blocks[ARAD_EM_TABLE_LEM].start_address = 0;
    Em.blocks[ARAD_EM_TABLE_LEM].end_address = ARAD_CHIP_SIM_LEM_TABLE_SIZE;
    Em.blocks[ARAD_EM_TABLE_LEM].read_result_address = 0;
    Em.blocks[ARAD_EM_TABLE_ISEM_A].offset = ARAD_CHIP_SIM_ISEM_A_BASE;
    Em.blocks[ARAD_EM_TABLE_ISEM_A].key_size = ARAD_CHIP_SIM_ISEM_A_KEY;
    Em.blocks[ARAD_EM_TABLE_ISEM_A].data_nof_bytes = ARAD_CHIP_SIM_ISEM_A_PAYLOAD;
    Em.blocks[ARAD_EM_TABLE_ISEM_A].start_address = 0;
    Em.blocks[ARAD_EM_TABLE_ISEM_A].end_address = ARAD_CHIP_SIM_ISEM_A_TABLE_SIZE;
    Em.blocks[ARAD_EM_TABLE_ISEM_A].read_result_address = 0;
    Em.blocks[ARAD_EM_TABLE_ISEM_B].offset = ARAD_CHIP_SIM_ISEM_B_BASE;
    Em.blocks[ARAD_EM_TABLE_ISEM_B].key_size = ARAD_CHIP_SIM_ISEM_B_KEY;
    Em.blocks[ARAD_EM_TABLE_ISEM_B].data_nof_bytes = ARAD_CHIP_SIM_ISEM_B_PAYLOAD;
    Em.blocks[ARAD_EM_TABLE_ISEM_B].start_address = 0;
    Em.blocks[ARAD_EM_TABLE_ISEM_B].end_address = ARAD_CHIP_SIM_ISEM_B_TABLE_SIZE;
    Em.blocks[ARAD_EM_TABLE_ISEM_B].read_result_address = 0;
    Em.blocks[ARAD_EM_TABLE_ESEM].offset = ARAD_CHIP_SIM_ESEM_BASE;
    Em.blocks[ARAD_EM_TABLE_ESEM].key_size = ARAD_CHIP_SIM_ESEM_KEY;
    Em.blocks[ARAD_EM_TABLE_ESEM].data_nof_bytes = ARAD_CHIP_SIM_ESEM_PAYLOAD;
    Em.blocks[ARAD_EM_TABLE_ESEM].start_address = 0;
    Em.blocks[ARAD_EM_TABLE_ESEM].end_address = ARAD_CHIP_SIM_ESEM_TABLE_SIZE;
    Em.blocks[ARAD_EM_TABLE_ESEM].read_result_address = 0;
    Em.blocks[ARAD_EM_TABLE_RMAPEM].offset = ARAD_CHIP_SIM_RMAPEM_BASE;
    Em.blocks[ARAD_EM_TABLE_RMAPEM].key_size = ARAD_CHIP_SIM_RMAPEM_KEY;
    Em.blocks[ARAD_EM_TABLE_RMAPEM].data_nof_bytes = ARAD_CHIP_SIM_RMAPEM_PAYLOAD;
    Em.blocks[ARAD_EM_TABLE_RMAPEM].start_address = 0;
    Em.blocks[ARAD_EM_TABLE_RMAPEM].end_address = ARAD_CHIP_SIM_RMAPEM_TABLE_SIZE;
    Em.blocks[ARAD_EM_TABLE_RMAPEM].read_result_address = 0;
    Em.blocks[ARAD_EM_TABLE_OAMEM_A].offset = ARAD_CHIP_SIM_OEMA_BASE;
    Em.blocks[ARAD_EM_TABLE_OAMEM_A].key_size = ARAD_CHIP_SIM_OEMA_KEY;
    Em.blocks[ARAD_EM_TABLE_OAMEM_A].data_nof_bytes = ARAD_CHIP_SIM_OEMA_PAYLOAD;
    Em.blocks[ARAD_EM_TABLE_OAMEM_A].start_address = 0;
    Em.blocks[ARAD_EM_TABLE_OAMEM_A].end_address = ARAD_CHIP_SIM_OEMA_TABLE_SIZE;
    Em.blocks[ARAD_EM_TABLE_OAMEM_A].read_result_address = 0;
    Em.blocks[ARAD_EM_TABLE_OAMEM_B].offset = ARAD_CHIP_SIM_OEMB_BASE;
    Em.blocks[ARAD_EM_TABLE_OAMEM_B].key_size = ARAD_CHIP_SIM_OEMB_KEY;
    Em.blocks[ARAD_EM_TABLE_OAMEM_B].data_nof_bytes = ARAD_CHIP_SIM_OEMB_PAYLOAD;
    Em.blocks[ARAD_EM_TABLE_OAMEM_B].start_address = 0;
    Em.blocks[ARAD_EM_TABLE_OAMEM_B].end_address = ARAD_CHIP_SIM_OEMB_TABLE_SIZE;
    Em.blocks[ARAD_EM_TABLE_OAMEM_B].read_result_address = 0;
    Em.blocks[ARAD_EM_TABLE_GLEM].offset = ARAD_CHIP_SIM_GLEM_BASE;
    Em.blocks[ARAD_EM_TABLE_GLEM].key_size = ARAD_CHIP_SIM_GLEM_KEY;
    Em.blocks[ARAD_EM_TABLE_GLEM].data_nof_bytes = ARAD_CHIP_SIM_GLEM_PAYLOAD;
    Em.blocks[ARAD_EM_TABLE_GLEM].start_address = 0;
    Em.blocks[ARAD_EM_TABLE_GLEM].end_address = ARAD_CHIP_SIM_GLEM_TABLE_SIZE;
    Em.blocks[ARAD_EM_TABLE_GLEM].read_result_address = 0;
  }

}
/*****************************************************
*NAME
*  chip_sim_em_malloc
*Programmer name  : Hana Gutman
*TYPE: PROC
*DATE: 24-Sep-02 19:02:44
*FUNCTION:
*  Clear from the code.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) void

*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
uint32
chip_sim_em_malloc(
             CHIP_SIM_EM_BLOCK* em_blocks, 
             int                block_cnt
             )
{
  uint32
    status = OK;
  UINT32
    nof_addresses =0;
  int
    i, blk_ndx;
  CHIP_SIM_EM_BLOCK*
    block_p;
  uint32
    res;
  CHIP_SIM_EM_TABLE_TYPE
    table_type = SOC_PB_EM_TABLE_ISEM;
  uint8
    is_first_loop = TRUE;
  SOC_SAND_MULTI_SET_PTR
    *multi_set_info;
  SOC_SAND_MULTI_SET_INIT_INFO
    multi_set_init_info;


  for (blk_ndx = 0; blk_ndx < block_cnt; blk_ndx++) {
    block_p = &(em_blocks[blk_ndx]);

    if (is_first_loop == TRUE) 
    {
      is_first_loop = FALSE;
      table_type = SOC_PB_EM_TABLE_ISEM;
    } 
    else 
    {
      table_type++;
    }

    nof_addresses = block_p->end_address - block_p->start_address + 1;
    for (i=0; i<CHIP_SIM_NOF_CHIPS; i++)
    {
      block_p->base[i] =
        (uint8*)sal_alloc(nof_addresses*block_p->data_nof_bytes, "EM buffer");
      if (NULL == block_p->base[i])
      {
        /* chip_sim_log_run("failed to alloc in chip_sim_em_malloc()\r\n");*/
        status = ERROR ;
        chip_sim_mem_free();
        GOTO_FUNC_EXIT_POINT;
      }
      multi_set_info = &(block_p->multi_set[i]);
      soc_sand_os_memset(&multi_set_init_info, 0x0, sizeof(SOC_SAND_MULTI_SET_INIT_INFO));
      multi_set_init_info.get_entry_fun = NULL;
      multi_set_init_info.set_entry_fun = NULL;
      multi_set_init_info.max_duplications = 1;/*maximal packets to store from each content*/
      multi_set_init_info.sec_handle = 0;
      multi_set_init_info.prime_handle = i;
      multi_set_init_info.member_size = block_p->key_size; /* Soc_pb_em_blocks[table_type] */
      multi_set_init_info.nof_members = block_p->end_address - block_p->start_address + 1; /*maximal packets to store*/

      multi_set_init_info.wb_var_index = SOC_DPP_WB_ENGINE_VAR_NONE;
      res = soc_sand_multi_set_create(
            BSL_UNIT_UNKNOWN,
            multi_set_info,
            multi_set_init_info
        );
      {
        /*
        * Initialize the the indirect allocated memory with
        * CHIP_SIM_INDIRECT_LONG_FILLER+.... to be marked.
        * Debuging porpuses.
        */
#if 0
        UINT32
          byte_i;
#endif
        uint8
          filler_i;
        sal_memset(
          block_p->base[i],
          0x0,
          nof_addresses*block_p->data_nof_bytes
          );
        filler_i = CHIP_SIM_EM_BYTE_FILLER;
#if 0
        for (byte_i=0; byte_i<nof_addresses*block_p->data_nof_bytes; byte_i++)
        {
          (block_p->base[i])[byte_i] = filler_i;
          if (CHIP_SIM_EM_BYTE_FILLER != 0x0)
          {
            /* if filler not zero incremant it, so every place as different value*/
            filler_i++;
            if (filler_i==0x0)
            {
              filler_i = CHIP_SIM_EM_BYTE_FILLER;
            }
          }
        }
#endif
      }
    }
  }


FUNC_EXIT_POINT:
  return status;
}

/*****************************************************
*NAME
*  chip_sim_indirect_free
*Programmer name  :Hana Gutman
*TYPE: PROC
*DATE: 15-Dec-09 19:02:50
*FUNCTION:
*  Clear from the code.
*INPUT:
*  SOC_SAND_DIRECT:
*    None.

*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
void
chip_sim_em_free(
             void
             )
{
  int
    i;
  CHIP_SIM_EM_BLOCK*
    block_p;

  if (NULL == Em.blocks)
  {
    GOTO_FUNC_EXIT_POINT;
  }

  for (block_p=Em.blocks; block_p->read_result_address!=INVALID_ADDRESS; block_p++)
  {
    for (i=0; i<CHIP_SIM_NOF_CHIPS; i++)
    {
      if (NULL != block_p->base[i])
      {
        CHIP_SIM_FREE(block_p->base[i]) ;
      }
      block_p->base[i] = NULL;
    }
  }
  Em.blocks = NULL;

FUNC_EXIT_POINT:
  return;
}


/*****************************************************
*NAME
*  chip_sim_indirect_free
*Programmer name  :Hana Gutman
*TYPE: PROC
*DATE: 15-Dec-09 19:02:50
*FUNCTION:
*  Clear from the code.
*INPUT:
*  SOC_SAND_DIRECT:
*    None.

*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
uint32
chip_sim_em_init(SOC_SAND_DEVICE_TYPE device_type)
{
  CHIP_SIM_EM_BLOCK* indirect_blocks;
  int                block_cnt;


  SOC_SAND_INTERRUPT_INIT_DEFS;
   SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_INTERRUPTS_STOP; 

  indirect_blocks = NULL;
  block_cnt = 0;

  switch(device_type)
  {
  case SOC_SAND_DEV_PB:
    indirect_blocks  =  Soc_pb_em_blocks;
    block_cnt = SOC_PB_EM_TABLE_ID_LAST;
    break;
  case SOC_SAND_DEV_ARAD:
  case SOC_SAND_DEV_JERICHO:
    indirect_blocks  =  Arad_em_blocks;
    block_cnt = ARAD_EM_TABLE_ID_LAST;
    break;
  default:
      SOC_SAND_SET_ERROR_CODE(SOC_PPD_FUNC_UNSUPPORTED_ERR, 10, exit);     
      break;
  }

  chip_sim_em_init_chip_specifics(device_type, indirect_blocks);
  chip_sim_em_malloc(indirect_blocks, block_cnt);

exit:
  SOC_SAND_INTERRUPTS_START_IF_STOPPED;
  SOC_SAND_EXIT_AND_SEND_ERROR("error in chip_sim_em_init()",0,0);
  

}


uint32 
chip_sim_em_offset_to_table_id (SOC_SAND_IN   uint32      offset,
                                SOC_SAND_OUT  uint32      *id)
{
  CHIP_SIM_EM_BLOCK*
    block_p;
  uint32
    status = OK ;

  uint32 i = 0;

  for (block_p=Em.blocks; block_p->read_result_address!=INVALID_ADDRESS; block_p++)
  {
    if ( block_p->offset == offset)
    {
      *id = i;
      GOTO_FUNC_EXIT_POINT;
    }
    i++;
  }
  *id = -1;
  status = ERROR;
 
FUNC_EXIT_POINT:
  return status;
}

uint32
chip_sim_em_get_block(
  SOC_SAND_IN       int         unit,
  SOC_SAND_IN       uint32      tbl_offset,
  SOC_SAND_IN       uint32     key_size,
  SOC_SAND_IN       uint32     data_size,
  SOC_SAND_IN       void*      *filter_data,
  SOC_SAND_INOUT    uint32     *key_array,
  SOC_SAND_INOUT    uint32     *data_array,
  SOC_SAND_OUT      uint32     *num_entries,
  SOC_SAND_INOUT    SOC_SAND_TABLE_BLOCK_RANGE  *block_range
  )
{
  uint32
    res = SOC_SAND_OK;
  uint8
    key8[CHIP_SIM_TABLE_KEY_MAX_SIZE];
  uint8  
    data8[CHIP_SIM_TABLE_DATA_MAX_SIZE];
  SOC_SAND_MULTI_SET_INFO
    *multi_set;
  uint32    id, cnt;
  SOC_SAND_MULTI_SET_ITER   iter;
  uint32                     hash_data_ndx, ref_count;
  uint32
    status = OK ;
  SOC_SAND_IN ARAD_PP_IHP_MACT_FLUSH_DB_TBL_DATA                      *tbl_data;
  uint32    eq, is_diff;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  tbl_data = (ARAD_PP_IHP_MACT_FLUSH_DB_TBL_DATA *)filter_data;
  cnt = 0;
  status = chip_sim_em_offset_to_table_id (tbl_offset, &id);
  if (status != OK)
  {
    GOTO_FUNC_EXIT_POINT;
  }

  multi_set = &Em.blocks[id].multi_set[unit][0];

  SOC_SAND_HASH_TABLE_ITER_SET_BEGIN(&iter);

  res = soc_sand_multi_set_get_next(unit, multi_set, &iter, key8, &hash_data_ndx, &ref_count);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, FUNC_EXIT_POINT);


  while (!SOC_SAND_HASH_TABLE_ITER_IS_END(&iter)) {

      res = chip_sim_em_key_map_get_entry(
                  unit, 
                  0, 
                  Em.blocks[id].base[unit],
                  hash_data_ndx * Em.blocks[id].data_nof_bytes ,
                  Em.blocks[id].data_nof_bytes,
                  data8
                  );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, FUNC_EXIT_POINT);


     
      res = soc_sand_U8_to_U32(
                    data8,
                    Em.blocks[id].data_nof_bytes,
                    &data_array[cnt * ARAD_PP_LEM_ACCESS_PAYLOAD_IN_UINT32S]);
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, FUNC_EXIT_POINT);
     
      eq = TRUE;
      is_diff = 0;

      is_diff =( data_array[cnt * ARAD_PP_LEM_ACCESS_PAYLOAD_IN_UINT32S] ^ COMPILER_64_LO(tbl_data->compare_payload_data));
      is_diff = is_diff & COMPILER_64_LO(tbl_data->compare_payload_mask);
      if (is_diff != 0) {
          eq = FALSE;
      } else {
          is_diff =( data_array[cnt * ARAD_PP_LEM_ACCESS_PAYLOAD_IN_UINT32S+1] ^ COMPILER_64_HI(tbl_data->compare_payload_data));
          is_diff = is_diff & COMPILER_64_HI(tbl_data->compare_payload_mask);
          if (is_diff != 0) {
              eq = FALSE;
          }
      }
      if (!eq) {
          goto next_loop;
      }
      res = soc_sand_U8_to_U32(
                    key8,
                    key_size,
                    &key_array[cnt * SOC_DPP_DEFS_MAX_LEM_WIDTH_IN_UINT32S]);
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, FUNC_EXIT_POINT);
          


      cnt++;
next_loop:
      res = soc_sand_multi_set_get_next(unit, multi_set, &iter, key8, &hash_data_ndx, &ref_count);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, FUNC_EXIT_POINT);

  }
  *num_entries = cnt;

FUNC_EXIT_POINT:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PP_LEM_ACCESS_PAYLOAD_verify()",0,0);
  
}

uint32
  chip_sim_em_match_rule_mac(
    SOC_SAND_IN  int                                         unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_TRAVERSE_MATCH_RULE      *rule,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_ENTRY_KEY                *mac_key,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_ENTRY_VALUE              *mac_entry_value,
    SOC_SAND_OUT uint32                                      *match) 
{
    uint8   eq;
    int     i;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *match = FALSE;

    if (mac_key->key_type != ARAD_PP_FRWRD_MACT_KEY_TYPE_MAC_ADDR) {
        GOTO_FUNC_EXIT_POINT;
    }
    if (rule->key_type != SOC_PPD_FRWRD_MACT_KEY_TYPE_MAC_ADDR) {
        GOTO_FUNC_EXIT_POINT;
    }

    /* Match mac and mask*/
    for (i = 0; i < SOC_SAND_PP_MAC_ADDRESS_NOF_U8; i++) {
         eq = ((mac_key->key_val.mac.mac.address[i] ^ rule->key_rule.mac.mac.address[i]) &  rule->key_rule.mac.mac_mask.address[i]);
         if (eq != 0 ) {
             GOTO_FUNC_EXIT_POINT;
         }
    }
    eq = (rule->key_rule.mac.fid ^  mac_key->key_val.mac.fid) & rule->key_rule.mac.fid_mask;
    if (eq != 0 ) {
        GOTO_FUNC_EXIT_POINT;
    }

    
    if (rule->value_rule.compare_mask & SOC_PPD_FRWRD_MACT_TRAVERSE_MATCH_SELECT_IS_DYNAMIC) {
        if (rule->value_rule.val.aging_info.is_dynamic != mac_entry_value->aging_info.is_dynamic) {
            GOTO_FUNC_EXIT_POINT;
        }
    }
    if ((rule->value_rule.compare_mask & SOC_PPD_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DEST_VAL) 
        && (rule->value_rule.compare_mask & SOC_PPD_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DEST_TYPE))
    {
        if (rule->value_rule.val.frwrd_info.forward_decision.dest_id != mac_entry_value->frwrd_info.forward_decision.dest_id) {
            GOTO_FUNC_EXIT_POINT;
        }
        if (rule->value_rule.val.frwrd_info.forward_decision.type != mac_entry_value->frwrd_info.forward_decision.type) {
            GOTO_FUNC_EXIT_POINT;
        }

    }
    *match = TRUE;


FUNC_EXIT_POINT:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in chip_sim_em_match_rule_mac()",0,0);
  
}

