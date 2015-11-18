/* $Id: pb_pp_sw_db.c,v 1.18 Broadcom SDK $
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

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_sw_db.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_llp_trap.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_llp_mirror.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_eg_mirror.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_llp_cos.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_trap_mgmt.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_lif.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_fp.h>
#include <soc/dpp/Petra/PB_TM/pb_api_general.h>

#include <soc/dpp/Petra/petra_general.h>

#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>

#include<soc/dpp/SAND/Management/sand_low_level.h>

#include <soc/dpp/dpp_wb_engine.h>

#if DUNE_BCM
  #include <soc/dpp/drv.h>
  #include <soc/dpp/Petra/petra_api_ssr.h>
#endif
/* } */

/*************
 * DEFINES   *
 *************/
/* { */
#define SOC_PB_PP_SW_DB_FREE_LIST_MAX_SIZE (256)
/* } */

/*************
 *  MACROS   *
 *************/
/* { */

#define SOC_PB_PP_SW_DB_ACTIVE_INS(cached, name)   \
  ((!cached)?(name):(name##_cache))

/* PAT node payload encoding - copied from ipv4_lpm_mngr:
* bits 31:30 -  pending status of entry (synced, pending add, pending remove)
* bits 29:28 - entry hw target (LPM / LEM)
* bits 27:0 - fec id
* the following macros build node payload value / extract the above fields from PAT node
* payload
*/

#define SOC_PB_PP_IPV4_LPM_MNGR_PAT_DATA_PENDING_OP_TYPE_MASK 0x3
#define SOC_PB_PP_IPV4_LPM_MNGR_PAT_DATA_PENDING_OP_TYPE_SHIFT 30

#define SOC_PB_PP_IPV4_LPM_MNGR_PAT_DATA_HW_TARGET_TYPE_MASK 0x3
#define SOC_PB_PP_IPV4_LPM_MNGR_PAT_DATA_HW_TARGET_TYPE_SHIFT (SOC_PB_PP_IPV4_LPM_MNGR_PAT_DATA_PENDING_OP_TYPE_SHIFT - 2)

#define SOC_PB_PP_IPV4_LPM_MNGR_PAT_DATA_FEC_MASK 0xFFFFFFF
#define SOC_PB_PP_IPV4_LPM_MNGR_PAT_DATA_FEC_SHIFT 0



#define SOC_PB_PP_IPV4_LPM_MGR_PAT_DATA_TO_FEC_ID(payload) \
  (((payload) >> SOC_PB_PP_IPV4_LPM_MNGR_PAT_DATA_FEC_SHIFT) & SOC_PB_PP_IPV4_LPM_MNGR_PAT_DATA_FEC_MASK)

#define SOC_PB_PP_IPV4_LPM_MGR_PAT_DATA_TO_PENDING_OP_TYPE(payload) \
  (((payload) >> SOC_PB_PP_IPV4_LPM_MNGR_PAT_DATA_PENDING_OP_TYPE_SHIFT) & SOC_PB_PP_IPV4_LPM_MNGR_PAT_DATA_PENDING_OP_TYPE_MASK)

#define SOC_PB_PP_IPV4_LPM_MGR_PAT_DATA_TO_HW_TARGET_TYPE(payload) \
  (((payload) >> SOC_PB_PP_IPV4_LPM_MNGR_PAT_DATA_HW_TARGET_TYPE_SHIFT) & SOC_PB_PP_IPV4_LPM_MNGR_PAT_DATA_HW_TARGET_TYPE_MASK)

#if DUNE_BCM
#ifdef BCM_WARM_BOOT_SUPPORT
#define BCM_DIRTY_MARK \
  {uint8 is_init_done;\
  soc_petra_ssr_is_device_init_done(\
  unit,\
  &is_init_done\
  );\
  if (is_init_done == TRUE){\
  SOC_CONTROL_LOCK((unit));\
  SOC_CONTROL((unit))->scache_dirty = 1;\
  SOC_CONTROL_UNLOCK((unit));}}
#else 
#define BCM_DIRTY_MARK
#endif /*BCM_WARM_BOOT_SUPPORT*/

#else
#define BCM_DIRTY_MARK
#endif /*DUNE_BCM*/

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */
/* } */

/*************
* GLOBALS    *
*************/
/* { */

static SOC_PROCEDURE_DESC_ELEMENT
    Soc_pb_pp_procedure_desc_element_sw_db[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_SW_DB_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_SW_DB_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */

  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_SW_DB_INIT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_SW_DB_DEVICE_INIT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_SW_DB_DEVICE_CLOSE),

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

static SOC_ERROR_DESC_ELEMENT
    Soc_pb_pp_error_desc_element_sw_db[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  SOC_ERR_DESC_ELEMENT_DEF_LAST
};

static SOC_PB_PP_SW_DB
  Soc_pb_pp_sw_db;

/* } */

/*************
* FUNCTIONS  *
*************/
/* { */
/* } */

uint32
  soc_pb_pp_sw_db_lif_table_initialize(
    SOC_SAND_IN  int unit
  );

uint32
  soc_pb_pp_sw_db_lif_table_terminate(
    SOC_SAND_IN  int unit
  );

uint32
  soc_pb_pp_sw_db_l2_lif_initialize(
    SOC_SAND_IN  int unit
  );

uint32
  soc_pb_pp_sw_db_l2_lif_terminate(
    SOC_SAND_IN  int unit
  );


STATIC uint32
  soc_pb_pp_sw_db_ipv4_arr_mem_allocator_entry_set(
    SOC_SAND_IN   int                            unit,
    SOC_SAND_IN   uint32                            mem_id,
    SOC_SAND_IN  uint32                             entry_ndx,
    SOC_SAND_IN  SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY         *entry
  )
{
  uint32
    mem_inst;
  uint8
    is_cached;
  SOC_PB_PP_SW_DB_IPV4_INFO
    *ipv4_info;

  ipv4_info = Soc_pb_pp_sw_db.device[unit]->ipv4_info;

  is_cached = SOC_SAND_ARR_MEM_ALLOCATOR_IS_CACHED_INST(mem_id);
  mem_inst = SOC_SAND_ARR_MEM_ALLOCATOR_BANK_INST(mem_id);
  
  if (!is_cached)
  {
    sal_memcpy(&(ipv4_info->lpm_mngr.init_info.mem_allocators[mem_inst].arr_mem_allocator_data.array[entry_ndx]),entry,sizeof(*entry));
  }
  else
  {
    sal_memcpy(&(ipv4_info->lpm_mngr.init_info.mem_allocators[mem_inst].arr_mem_allocator_data.array_cache[entry_ndx]),entry,sizeof(*entry));
  }

  BCM_DIRTY_MARK;

  return SOC_SAND_OK;
}

STATIC uint32
  soc_pb_pp_sw_db_ipv4_arr_mem_allocator_entry_get(
    SOC_SAND_IN   int                            unit,
    SOC_SAND_IN   uint32                            mem_id,
    SOC_SAND_IN  uint32                             entry_ndx,
    SOC_SAND_OUT  SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY        *entry
  )
{
  uint32
    mem_inst;
  uint8
    is_cached;
  SOC_PB_PP_SW_DB_IPV4_INFO
    *ipv4_info;

  ipv4_info = Soc_pb_pp_sw_db.device[unit]->ipv4_info;

  is_cached = SOC_SAND_ARR_MEM_ALLOCATOR_IS_CACHED_INST(mem_id);
  mem_inst = SOC_SAND_ARR_MEM_ALLOCATOR_BANK_INST(mem_id);
  
  if (!is_cached)
  {
    sal_memcpy(entry,&(ipv4_info->lpm_mngr.init_info.mem_allocators[mem_inst].arr_mem_allocator_data.array[entry_ndx]), sizeof(*entry));
  }
  else
  {
    sal_memcpy(entry,&(ipv4_info->lpm_mngr.init_info.mem_allocators[mem_inst].arr_mem_allocator_data.array_cache[entry_ndx]), sizeof(*entry));
  }

  return SOC_SAND_OK;
}

STATIC uint32
  soc_pb_pp_sw_db_ipv4_arr_mem_allocator_free_entry_set(
    SOC_SAND_IN   int                          unit,
    SOC_SAND_IN   uint32                          mem_id,
    SOC_SAND_IN  SOC_SAND_ARR_MEM_ALLOCATOR_PTR         free_list
  )
{
  uint32
    mem_inst;
  uint8
    is_cached;
  SOC_PB_PP_SW_DB_IPV4_INFO
    *ipv4_info;

  ipv4_info = Soc_pb_pp_sw_db.device[unit]->ipv4_info;

  is_cached = SOC_SAND_ARR_MEM_ALLOCATOR_IS_CACHED_INST(mem_id);
  mem_inst = SOC_SAND_ARR_MEM_ALLOCATOR_BANK_INST(mem_id);
  
  if (!is_cached)
  {
    ipv4_info->lpm_mngr.init_info.mem_allocators[mem_inst].arr_mem_allocator_data.free_list = free_list;
  }
  else
  {
    ipv4_info->lpm_mngr.init_info.mem_allocators[mem_inst].arr_mem_allocator_data.free_list_cache = free_list;
  }

  BCM_DIRTY_MARK;

  return SOC_SAND_OK;
}

STATIC uint32
  soc_pb_pp_sw_db_ipv4_arr_mem_allocator_free_entry_get(
    SOC_SAND_IN   int                          unit,
    SOC_SAND_IN   uint32                          mem_id,
    SOC_SAND_OUT  SOC_SAND_ARR_MEM_ALLOCATOR_PTR        *free_list
  )
{
  uint32
    mem_inst;
  uint8
    is_cached;
  SOC_PB_PP_SW_DB_IPV4_INFO
    *ipv4_info;

  ipv4_info = Soc_pb_pp_sw_db.device[unit]->ipv4_info;

  is_cached = SOC_SAND_ARR_MEM_ALLOCATOR_IS_CACHED_INST(mem_id);
  mem_inst = SOC_SAND_ARR_MEM_ALLOCATOR_BANK_INST(mem_id);

  if (!is_cached)
  {
    *free_list = ipv4_info->lpm_mngr.init_info.mem_allocators[mem_inst].arr_mem_allocator_data.free_list;
  }
  else
  {
    *free_list = ipv4_info->lpm_mngr.init_info.mem_allocators[mem_inst].arr_mem_allocator_data.free_list_cache;
  }

  return SOC_SAND_OK;

}

uint32
  soc_pb_pp_ipv4_lpm_test_mem_to_mem_allocator_get(
      SOC_SAND_IN  int                      unit,
      SOC_SAND_IN  uint32                      mem_id,
      SOC_SAND_IN  uint32                      memory_ndx,
      SOC_SAND_OUT SOC_SAND_ARR_MEM_ALLOCATOR_INFO   **mem_allocator
  )
{
  SOC_PB_PP_IPV4_LPM_MNGR_INIT_INFO
    *lpm_init_info;
  SOC_SAND_RET
    soc_sand_ret = SOC_SAND_OK;

  if (Soc_pb_pp_sw_db.device[unit]->ipv4_info == NULL)
  {
    return SOC_SAND_ERR;
  }
  lpm_init_info = &Soc_pb_pp_sw_db.device[unit]->ipv4_info->lpm_mngr.init_info;
  
  *mem_allocator = &lpm_init_info->mem_allocators[memory_ndx];
  
  return soc_sand_ret;
}


/************************************************************************/
/* function mapping from VRF to the corresponding PAT Tree              */
/************************************************************************/
uint32
  soc_pb_pp_ipv4_lpm_vrf_to_lpm_db(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      sec_handle,
    SOC_SAND_IN  uint32                      vrf_ndx,
    SOC_SAND_OUT SOC_SAND_PAT_TREE_INFO            **pat_tree
  )
{

  SOC_PB_PP_IPV4_LPM_MNGR_INIT_INFO
    *lpm_init_info;
  SOC_SAND_RET
    soc_sand_ret = SOC_SAND_OK;

  if (Soc_pb_pp_sw_db.device[unit]->ipv4_info == NULL)
  {
    return SOC_SAND_ERR;
  }

  lpm_init_info = &Soc_pb_pp_sw_db.device[unit]->ipv4_info->lpm_mngr.init_info;

  *pat_tree = &lpm_init_info->lpms[vrf_ndx];
  
  return soc_sand_ret;
}

/* $Id: pb_pp_sw_db.c,v 1.18 Broadcom SDK $
 * Pat tree
 */


uint32
  pp_pb_sw_db_pat_tree_root_set(
    SOC_SAND_IN int                    unit,
    SOC_SAND_IN uint32                    tree_ndx,
    SOC_SAND_IN SOC_SAND_PAT_TREE_NODE_PLACE    root_place
  )
{
  SOC_SAND_RET
    soc_sand_ret = SOC_SAND_OK;
  SOC_PB_PP_IPV4_LPM_MNGR_INIT_INFO
    *lpm_init_info = &Soc_pb_pp_sw_db.device[unit]->ipv4_info->lpm_mngr.init_info;

  lpm_init_info->lpms[tree_ndx].pat_tree_data.root = root_place;

  BCM_DIRTY_MARK;

  return soc_sand_ret;
}


STATIC uint32
  soc_pb_pp_sw_db_ipv4_arr_mem_allocator_write(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  uint32                             bank_id,
    SOC_SAND_IN  uint32                             offset,
    SOC_SAND_IN  SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY         *data
  )
{
  SOC_PB_PP_IHB_LPM_TBL_DATA
    tbl_data;
  SOC_PB_PP_SW_DB_IPV4_INFO
    *ipv4_info;
  SOC_PB_PP_IPV4_LPM_MNGR_INFO
    *lpm_mngr;
  SOC_PB_PP_IPV4_LPM_MNGR_INIT_INFO
    *lpm_init_info;
  SOC_PB_PP_IPV4_LPM_MNGR_INST
    inst;
  int32
    ptr;
  uint8
    is_cached;
  uint32
    inst_id;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ipv4_info = Soc_pb_pp_sw_db.device[unit]->ipv4_info;
  lpm_mngr = &(ipv4_info->lpm_mngr);
  lpm_init_info = &(lpm_mngr->init_info);

  is_cached = SOC_SAND_ARR_MEM_ALLOCATOR_IS_CACHED_INST(bank_id);
  inst_id = SOC_SAND_ARR_MEM_ALLOCATOR_BANK_INST(bank_id);

  if (!is_cached)
  {
    soc_pb_pp_ipv4_lpm_entry_decode(
      lpm_mngr,
      inst_id,
      data,
      1,
      &tbl_data.lpm
    );

    res = soc_pb_pp_ihb_lpm_tbl_set_unsafe(
            unit,
            inst_id+1,
            offset,
            &tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  }

 /*
  * write to software shadow.
  */
  if (is_cached)
  {
    lpm_init_info->mem_allocators[inst_id].arr_mem_allocator_data.mem_shadow_cache[offset] = *data;
  }
  else
  {
    lpm_init_info->mem_allocators[inst_id].arr_mem_allocator_data.mem_shadow[offset] = *data;
  }
 /*
  * if there is defragment, store backward pointer
  */

  if (lpm_init_info->flags & SOC_PB_PP_LPV4_LPM_SUPPORT_DEFRAG){
    /* for all banks except first bank, store backward pointer */
    if (inst_id < lpm_init_info->nof_banks - 1)
    {
      inst.arr[0] = *data;
      ptr = soc_pb_pp_ipv4_lpm_mngr_row_to_base_addr(lpm_mngr,&inst,inst_id);
      if (ptr != -1)
      {
        /* ptr at bank-id + 1 is pointed by offset at bank_id*/
        res = soc_sand_group_mem_ll_member_add(
                lpm_mngr->init_info.rev_ptrs[inst_id+1],
                ptr,
                offset
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
      }
    }
  }
  


  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_ipv4_arr_mem_allocator_write()",0,0);
}


STATIC uint32
soc_pb_pp_sw_db_ipv4_arr_mem_allocator_read(
  SOC_SAND_IN  int                             unit,
  SOC_SAND_IN  uint32                             bank_id,
  SOC_SAND_IN  uint32                             offset,
  SOC_SAND_OUT  SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY         *data
  )
{
  SOC_PB_PP_SW_DB_IPV4_INFO
    *ipv4_info;
  SOC_PB_PP_IPV4_LPM_MNGR_INFO
    *lpm_mngr;
  /*SOC_PB_PP_IHB_LPM_TBL_DATA
    tbl_data,tbl_data_read;*/
  uint32
    inst_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  /*
  * read from software shadow.
  */
  ipv4_info = Soc_pb_pp_sw_db.device[unit]->ipv4_info;
  lpm_mngr = &(ipv4_info->lpm_mngr);

  inst_id = SOC_SAND_ARR_MEM_ALLOCATOR_BANK_INST(bank_id);

  if (SOC_SAND_ARR_MEM_ALLOCATOR_IS_CACHED_INST(bank_id))
  {
    data[0] = lpm_mngr->init_info.mem_allocators[inst_id].arr_mem_allocator_data.mem_shadow_cache[offset];
    
  }
  else
  {
    data[0] = lpm_mngr->init_info.mem_allocators[inst_id].arr_mem_allocator_data.mem_shadow[offset];
  }
  
  data[1] = 0;

  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_ipv4_arr_mem_allocator_read()",0,0);

}

uint32
  soc_pb_sw_db_align_entry_ptr(
    SOC_SAND_IN   uint32            entry_place, /*offset */
    SOC_SAND_IN   uint32            key, /* device + bank-id */
    SOC_SAND_IN   uint32            new_ptr
 )
{
  uint32
    bank_id,
    unit;
  SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY
    data[3];
  SOC_PB_PP_SW_DB_IPV4_INFO
    *ipv4_info;
  SOC_PB_PP_IPV4_LPM_MNGR_INFO
    *lpm_mngr;
  SOC_PB_PP_IPV4_LPM_MNGR_INST
    inst;
  uint32
    mem_inst;
  uint32
	  ret_val;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /* get bank and device-id */
  bank_id = key & (SOC_SAND_BIT(31)|0x7);
  mem_inst = SOC_SAND_ARR_MEM_ALLOCATOR_BANK_INST(bank_id);
  unit = (key & SOC_SAND_RBIT(31)) >> 3;
  if (mem_inst == 0 )
  {
    return SOC_SAND_ERR;/* not expected to be bank zero*/
  }
  bank_id -= 1;
  mem_inst -= 1;

  ipv4_info = Soc_pb_pp_sw_db.device[unit]->ipv4_info;
  lpm_mngr = &(ipv4_info->lpm_mngr);

  /* read old data */
  res = soc_pb_pp_sw_db_ipv4_arr_mem_allocator_read(unit,bank_id,entry_place,data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  inst.arr[0] = data[0];
  /* update pointer in data*/
  ret_val = soc_pb_pp_ipv4_lpm_mngr_update_base_addr(lpm_mngr,&inst,mem_inst,new_ptr);

  /* write updated data */
  if (ret_val != -1)
  {
    data[0] = inst.arr[0];
    res = soc_pb_pp_sw_db_ipv4_arr_mem_allocator_write(unit,bank_id,entry_place,data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_sw_db_align_entry_ptr()",0,0);
}

/*
 * when moving entry in mem-allocator do this alignment
 */
uint32
  soc_pb_pp_sw_db_ipv4_lpm_mem_align(
    SOC_SAND_IN   int                      unit,
    SOC_SAND_IN   uint32                      bank_id,
    SOC_SAND_IN  SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY   *data,
    SOC_SAND_IN   uint32                      old_place,
    SOC_SAND_IN   uint32                      new_place
 )
{
  uint32
    key;
  uint32
    mem_inst;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  mem_inst = SOC_SAND_ARR_MEM_ALLOCATOR_BANK_INST(bank_id);
  /* if this is bank zero, then no one is poiting to it */
  if (mem_inst == 0)
  {
    goto exit;
  }

  key = bank_id;
  key |= (unit<<3);

  res = soc_sand_group_mem_ll_func_run(
            Soc_pb_pp_sw_db.device[unit]->ipv4_info->lpm_mngr.init_info.rev_ptrs[mem_inst],
            old_place,   /* all members point to this place */
            soc_pb_sw_db_align_entry_ptr,/* updated pointers to point to new place*/
            key,        /* this bank and device-id */
            new_place
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_ipv4_lpm_mem_align()",0,0);
}


uint32
  soc_pb_pp_sw_db_pat_tree_root_set(
    SOC_SAND_IN int                    unit,
    SOC_SAND_IN uint32                    tree_ndx,
    SOC_SAND_IN SOC_SAND_PAT_TREE_NODE_PLACE    root_place
  )
{
  uint32
    tree_id;
  uint8
    cached;

  if (Soc_pb_pp_sw_db.device[unit] == NULL)
  {
    return SOC_SAND_ERR;
  }

  tree_id = SOC_SAND_PAT_TREE_TREE_INST(tree_ndx);
  cached = SOC_SAND_PAT_TREE_IS_CACHED_INST(tree_ndx);

  if (!cached)
  {
    Soc_pb_pp_sw_db.device[unit]->ipv4_info->lpm_mngr.init_info.lpms[tree_id].pat_tree_data.root = root_place;
  }
  else
  {
    Soc_pb_pp_sw_db.device[unit]->ipv4_info->lpm_mngr.init_info.lpms[tree_id].pat_tree_data.root_cache = root_place;
  }

  return SOC_SAND_OK;

  BCM_DIRTY_MARK;
}

uint32
  soc_pb_pp_sw_db_pat_tree_root_get(
    SOC_SAND_IN int                    unit,
    SOC_SAND_IN uint32                    tree_ndx,
    SOC_SAND_OUT SOC_SAND_PAT_TREE_NODE_PLACE    *root_place
  )
{
  uint32
    tree_id;
  uint8
    cached;

  if (Soc_pb_pp_sw_db.device[unit] == NULL)
  {
    return SOC_SAND_ERR;
  }

  tree_id = SOC_SAND_PAT_TREE_TREE_INST(tree_ndx);
  cached = SOC_SAND_PAT_TREE_IS_CACHED_INST(tree_ndx);

  if (!cached)
  {
    *root_place = Soc_pb_pp_sw_db.device[unit]->ipv4_info->lpm_mngr.init_info.lpms[tree_id].pat_tree_data.root;
  }
  else
  {
    *root_place = Soc_pb_pp_sw_db.device[unit]->ipv4_info->lpm_mngr.init_info.lpms[tree_id].pat_tree_data.root_cache;
  }

  return SOC_SAND_OK;
}

uint32
  soc_pb_pp_sw_db_pat_tree_node_set(
    SOC_SAND_IN int                          unit,
    SOC_SAND_IN uint32                          tree_ndx,
    SOC_SAND_IN uint32                          node_place,
    SOC_SAND_IN SOC_SAND_PAT_TREE_NODE                *node_info
  )
{
  uint32
    tree_id;
  uint8
    cached;

  tree_id = SOC_SAND_PAT_TREE_TREE_INST(tree_ndx);
  cached = SOC_SAND_PAT_TREE_IS_CACHED_INST(tree_ndx);

  if (!cached)
  {
    sal_memcpy(
      &Soc_pb_pp_sw_db.device[unit]->ipv4_info->lpm_mngr.init_info.lpms[tree_id].pat_tree_data.tree_memory[node_place],
      node_info,
      sizeof(*node_info)
      );
  }
  else
  {
    sal_memcpy(
      &Soc_pb_pp_sw_db.device[unit]->ipv4_info->lpm_mngr.init_info.lpms[tree_id].pat_tree_data.tree_memory_cache[node_place],
      node_info,
      sizeof(*node_info)
      );
  }

  BCM_DIRTY_MARK;

  return SOC_SAND_OK;

}

uint32
  soc_pb_pp_sw_db_pat_tree_node_get(
    SOC_SAND_IN int                          unit,
    SOC_SAND_IN uint32                           tree_ndx,
    SOC_SAND_IN uint32                            node_place,
    SOC_SAND_OUT SOC_SAND_PAT_TREE_NODE                  *node_info
  )
{
  uint32
    tree_id;
  uint8
    cached;

  tree_id = SOC_SAND_PAT_TREE_TREE_INST(tree_ndx);
  cached = SOC_SAND_PAT_TREE_IS_CACHED_INST(tree_ndx);

  if (!cached)
  {
    sal_memcpy(
      node_info,
      &Soc_pb_pp_sw_db.device[unit]->ipv4_info->lpm_mngr.init_info.lpms[tree_id].pat_tree_data.tree_memory[node_place],
      sizeof(*node_info)
    );
  }
  else
  {
    sal_memcpy(
      node_info,
      &Soc_pb_pp_sw_db.device[unit]->ipv4_info->lpm_mngr.init_info.lpms[tree_id].pat_tree_data.tree_memory_cache[node_place],
      sizeof(*node_info)
    );
  }

  return SOC_SAND_OK;
}

uint32
  soc_pb_pp_sw_db_pat_tree_node_ref_get(
    SOC_SAND_IN int                          unit,
    SOC_SAND_IN uint32                           tree_ndx,
    SOC_SAND_IN uint32                            node_place,
    SOC_SAND_OUT SOC_SAND_PAT_TREE_NODE                  **node_info
  )
{
  uint32
    tree_id;
  uint8
    cached;

  tree_id = SOC_SAND_PAT_TREE_TREE_INST(tree_ndx);
  cached = SOC_SAND_PAT_TREE_IS_CACHED_INST(tree_ndx);

  if (!cached)
  {
    *node_info = &Soc_pb_pp_sw_db.device[unit]->ipv4_info->lpm_mngr.init_info.lpms[tree_id].pat_tree_data.tree_memory[node_place];
  }
  else
  {
    *node_info = &Soc_pb_pp_sw_db.device[unit]->ipv4_info->lpm_mngr.init_info.lpms[tree_id].pat_tree_data.tree_memory_cache[node_place];
  }

  return SOC_SAND_OK;
}

uint32
  soc_pb_pp_sw_db_ipv4_default_fec_set(
    SOC_SAND_IN int                    unit,
    SOC_SAND_IN uint32                    default_fec
  )
{

  SOC_PB_PP_SW_DB_IPV4_FIELD_SET(
    unit,
    ipv4_info->default_fec,
    &default_fec
  );
  BCM_DIRTY_MARK;
}

uint32
  soc_pb_pp_sw_db_ipv4_default_fec_get(
    SOC_SAND_IN int                    unit,
    SOC_SAND_OUT uint32                    *default_fec
  )
{
  SOC_PB_PP_SW_DB_IPV4_FIELD_GET(
    unit,
    ipv4_info->default_fec,
    default_fec
  );
}

uint32
  soc_pb_pp_sw_db_ipv4_nof_lpm_entries_in_lpm_set(
    SOC_SAND_IN int                    unit,
    SOC_SAND_IN uint32                    nof_lpm_entries_in_lpm
  )
{
  SOC_PB_PP_SW_DB_IPV4_FIELD_SET(
    unit,
    ipv4_info->nof_lpm_entries_in_lpm,
    &nof_lpm_entries_in_lpm
  );
  BCM_DIRTY_MARK;
}

uint32
  soc_pb_pp_sw_db_ipv4_nof_lpm_entries_in_lpm_get(
    SOC_SAND_IN int                    unit,
    SOC_SAND_OUT uint32                    *nof_lpm_entries_in_lpm
  )
{
  SOC_PB_PP_SW_DB_IPV4_FIELD_GET(
    unit,
    ipv4_info->nof_lpm_entries_in_lpm,
    nof_lpm_entries_in_lpm
  );
}


  
uint32
  soc_pb_pp_sw_db_free_list_add(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint32            bank_id,
    SOC_SAND_IN  uint32             address
  )
{
  SOC_PB_PP_SW_DB_IPV4_INFO
    *ipv4_info;
  uint32
    cur_size;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ipv4_info = Soc_pb_pp_sw_db.device[unit]->ipv4_info;
  cur_size = (ipv4_info->free_list_size[bank_id]);
  if (cur_size >= SOC_PB_PP_SW_DB_FREE_LIST_MAX_SIZE)
  {
    return SOC_SAND_ERR;
  }
  ipv4_info->free_list[bank_id][cur_size] = (uint16)address;
  ++(ipv4_info->free_list_size[bank_id]);

  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_free_list_add()",0,0);
}

uint32
  soc_pb_pp_sw_db_free_list_commit(
    SOC_SAND_IN  int            unit
  )
{
  uint32
    indx,
    bank_indx;
  SOC_PB_PP_SW_DB_IPV4_INFO
    *ipv4_info;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ipv4_info = Soc_pb_pp_sw_db.device[unit]->ipv4_info;

  for (bank_indx = 0; bank_indx < ipv4_info->lpm_mngr.init_info.nof_mems; ++bank_indx)
  {
    for (indx = 0; indx < ipv4_info->free_list_size[bank_indx]; ++indx)
    {
      res = soc_sand_arr_mem_allocator_free(
              &(ipv4_info->lpm_mngr.init_info.mem_allocators[bank_indx]),
              ipv4_info->free_list[bank_indx][indx]
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }
    ipv4_info->free_list_size[bank_indx] = 0;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_free_list_commit()",0,0);
}

uint32
  soc_pb_pp_sw_db_ipv4_nof_vrfs_set(
    SOC_SAND_IN int                    unit,
    SOC_SAND_IN uint32                   nof_vrfs
  )
{
  SOC_PB_PP_SW_DB_FIELD_SET(
    unit,
    oper_mode->ipv4_info.nof_vrfs,
    &nof_vrfs
  );
  BCM_DIRTY_MARK;
}


uint32
  soc_pb_pp_sw_db_ipv4_nof_vrfs_get(
    SOC_SAND_IN int                    unit,
    SOC_SAND_OUT uint32                    *nof_vrfs
  )
{
  SOC_PB_PP_SW_DB_FIELD_GET(
    unit,
    oper_mode->ipv4_info.nof_vrfs,
    nof_vrfs
  );
}

uint8
  soc_pb_pp_sw_db_ipv4_is_vrf_exist(
    SOC_SAND_IN int                    unit,
    SOC_SAND_IN uint32                   vrf_ndx
  )
{
  uint32 nof_vrfs;

  soc_pb_pp_sw_db_ipv4_nof_vrfs_get(
    unit,
    &nof_vrfs
  );
  return (vrf_ndx < nof_vrfs) ? TRUE : FALSE;
}

uint32
  soc_pb_pp_sw_db_init()
{
  uint32 unit;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_SW_DB_INIT);

  if (Soc_pb_pp_sw_db.init)
  {
    goto exit;
  }

  for (unit = 0; unit < SOC_SAND_MAX_DEVICE; ++unit)
  {
    Soc_pb_pp_sw_db.device[unit] = NULL;
  }

  Soc_pb_pp_sw_db.init = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in init()",0,0);
}



uint32
  soc_pb_pp_sw_db_device_init(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_MGMT_OPERATION_MODE    *oper_mode
  )
{
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_SW_DB_DEVICE_INIT);

  SOC_PETRA_ALLOC( Soc_pb_pp_sw_db.device[unit], SOC_PB_PP_SW_DB_DEVICE, 1 );

  res = soc_pb_pp_sw_db_oper_mode_initialize(unit, oper_mode);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  res = soc_pb_pp_sw_db_llp_filter_initialize(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_pb_pp_sw_db_llp_trap_initialize(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_pb_pp_sw_db_llp_mirror_initialize(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_sw_db_llp_vid_assign_initialize(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);
  
  res = soc_pb_pp_sw_db_eg_mirror_initialize(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_pb_pp_sw_db_llp_cos_initialize(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);


  res = soc_pb_pp_sw_db_eth_policer_initialize(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_pb_pp_sw_db_fwd_fec_initialize(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_pb_pp_sw_db_fwd_diag_initialize(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  {
    /* ipv4 */
    uint32
      nof_vrf_bits = soc_sand_log2_round_up(oper_mode->ipv4_info.nof_vrfs), /* number of bits to specify VRF */
      nof_banks = 6, /* number of levels for the lookup */
      /* nof_bits_per_bank[6] = {12,4,5,6,5,4}, number of bits that each bank will use from the VRF/IP address for the next access*/
      nof_mems = 6,  /* number of memories for writing the data */
      nof_rows_per_mem[6] =  {4*1024, 8*1024, 12*1024, 12*1024, 12*1024, 12*1024}; /* number of rows that each memory has */
    uint32
      vrf_bits,
      bits_per_depth,
      nof_bits,
      dpth_indx,
      nof_vrfs;

    uint32
      nof_bits_per_bank[6] = {12,6,5,6,5,6}, /* number of bits that each bank will use from the VRF/IP address for the next access*/
      bank_to_mem[6] = {0,1,2,3,4,5}; /* mapping from bank number, which is sequential in depth, to memory, which may or may not be sequential */
    SOC_PB_PP_IPV4_LPM_PXX_MODEL
      pxx_model = SOC_PB_PP_IPV4_LPM_PXX_MODEL_P6N; /* Which model of PXX to use (p6x, p4x, p6n, etc) */

    /* init number of VRFs for LPM lookup*/
    nof_vrfs = oper_mode->ipv4_info.nof_vrfs;
/*
    for (nof_vrfs = 1; nof_vrfs <= 256 ; nof_vrfs*=2 )
    {*/
    vrf_bits = soc_sand_log2_round_up(nof_vrfs);
    nof_bits = 32 - (12 - vrf_bits);
    /*soc_sand_os_printf("\n\r %-6lu|", nof_vrfs);*/

    for (dpth_indx = 1 ; dpth_indx < 5; ++dpth_indx)
    {
      bits_per_depth = SOC_SAND_DIV_ROUND_DOWN(nof_bits, 5 - dpth_indx + 1);
      nof_bits_per_bank[dpth_indx] = bits_per_depth;
      nof_bits -= bits_per_depth;
    }
    nof_bits_per_bank[5] = nof_bits;
    nof_bits_per_bank[0] = 12;
    if (nof_bits_per_bank[1] > 4 && nof_bits_per_bank[5]<=4)
    {
      nof_bits_per_bank[1]-=2;
      nof_bits_per_bank[5]+=2;
    }
    else if (nof_bits_per_bank[1] > 3 && nof_bits_per_bank[5] <=5)
      {
        nof_bits_per_bank[1]-=1;
        nof_bits_per_bank[5]+=1;
      }

    /* allocate this sw DB only if there is routing table to support */
    if (nof_vrfs > 0)
    {
      res = soc_pb_pp_sw_db_ipv4_initialize(
              unit,
              oper_mode->ipv4_info.nof_vrfs,
              oper_mode->ipv4_info.max_routes_in_vrf,
              nof_vrf_bits,
              nof_banks,
              nof_bits_per_bank,
              bank_to_mem,
              nof_mems,
              nof_rows_per_mem,
              pxx_model,
              oper_mode->ipv4_info.flags,
              0 /* default_sys_fec */
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }
    else
    {
      Soc_pb_pp_sw_db.device[unit]->ipv4_info = NULL;
    }
  }


  res = soc_pb_pp_sw_db_ilm_initialize(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  
  res = soc_pb_pp_sw_db_fwd_mact_initialize(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_pb_pp_sw_db_lif_cos_initialize(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);

  res = soc_pb_pp_sw_db_lif_table_initialize(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  res = soc_pb_pp_sw_db_l2_lif_initialize(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  res = soc_pb_pp_sw_db_fp_initialize(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  res = soc_pb_pp_sw_db_isem_initialize(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

  res = soc_pb_pp_sw_db_lag_initialize(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_device_init()",0,0);
}

uint32
  soc_pb_pp_sw_db_device_close(
    SOC_SAND_IN int unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_SW_DB_DEVICE_CLOSE);
  
  /* if device SW DB not initialized then nothing to do */
  if (Soc_pb_pp_sw_db.device[unit] == NULL)
  {
    SOC_PB_DO_NOTHING_AND_EXIT;
  }

  res = soc_pb_pp_sw_db_oper_mode_terminate(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  res = soc_pb_pp_sw_db_llp_filter_terminate(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  res = soc_pb_pp_sw_db_llp_trap_terminate(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_pb_pp_sw_db_llp_mirror_terminate(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_sw_db_llp_vid_assign_terminate(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_pb_pp_sw_db_eg_mirror_terminate(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_pb_pp_sw_db_llp_cos_terminate(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_pb_pp_sw_db_eth_policer_terminate(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_pb_pp_sw_db_fwd_fec_terminate(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_pb_pp_sw_db_fwd_diag_terminate(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);
  
  res = soc_pb_pp_sw_db_ipv4_terminate(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_pb_pp_sw_db_ilm_terminate(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_pb_pp_sw_db_fwd_mact_terminate(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  res = soc_pb_pp_sw_db_lif_cos_terminate(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 65, exit);

  res = soc_pb_pp_sw_db_lif_table_terminate(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  res = soc_pb_pp_sw_db_l2_lif_terminate(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  res = soc_pb_pp_sw_db_fp_terminate(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  res = soc_pb_pp_sw_db_isem_terminate(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

  res = soc_pb_pp_sw_db_lag_terminate(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

  SOC_PETRA_FREE_ANY_SIZE(Soc_pb_pp_sw_db.device[unit]);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_device_close()",0,0);
}

/*
 * SW DB multiset
 */
SOC_SAND_MULTI_SET_INFO*
  soc_pb_pp_sw_db_multiset_by_type_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  SOC_PB_PP_SW_DB_MULTI_SET     multiset
  )
{
  switch(multiset)
  {
  case SOC_PB_PP_SW_DB_MULTI_SET_L3_PROTOCOL:
    return &(Soc_pb_pp_sw_db.device[unit]->llp_trap->l3_protocols_multi_set);
    break;
  case SOC_PB_PP_SW_DB_MULTI_SET_LLP_MIRROR_PROFILE:
    return &(Soc_pb_pp_sw_db.device[unit]->llp_mirror->mirror_profile_multi_set);
    break;
  case SOC_PB_PP_SW_DB_MULTI_SET_EG_MIRROR_PROFILE:
    return &(Soc_pb_pp_sw_db.device[unit]->eg_mirror->mirror_profile_multi_set);
    break;
  case SOC_PB_PP_SW_DB_MULTI_SET_ETHER_TYPE:
    return &(Soc_pb_pp_sw_db.device[unit]->llp_cos->ether_type_multi_set);
    break;
  case SOC_PB_PP_SW_DB_MULTI_SET_ETH_POLICER_ETH_METER_PROFILE:
    return &(Soc_pb_pp_sw_db.device[unit]->eth_policer_mtr_profile->eth_meter_profile_multi_set);
    break;
  case SOC_PB_PP_SW_DB_MULTI_SET_ETH_POLICER_GLOBAL_METER_PROFILE:
    return &(Soc_pb_pp_sw_db.device[unit]->eth_policer_mtr_profile->global_meter_profile_multi_set);
    break;
  case SOC_PB_PP_SW_DB_MULTI_SET_L2_LIF_VLAN_COMPRESSION_RANGE:
    return &(Soc_pb_pp_sw_db.device[unit]->l2_lif->vlan_compression_range_multi_set);
  default:
    return 0;
  }
}


/*
 * SW DB Multiset }
 */

uint32
  soc_pb_pp_sw_db_oper_mode_initialize(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  SOC_PB_PP_MGMT_OPERATION_MODE    *oper_mode
  )
{
  uint32
    res;
  SOC_PB_PP_MGMT_OPERATION_MODE
    *oper_mode_ptr;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_SAND_OK;

  SOC_PETRA_ALLOC_ANY_SIZE(Soc_pb_pp_sw_db.device[unit]->oper_mode,
               SOC_PB_PP_MGMT_OPERATION_MODE, 1 );

  oper_mode_ptr = Soc_pb_pp_sw_db.device[unit]->oper_mode;

  SOC_PETRA_COPY(oper_mode_ptr, oper_mode, SOC_PB_PP_MGMT_OPERATION_MODE, 1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_oper_mode_initialize()",0,0);
}


uint32
  soc_pb_pp_sw_db_oper_mode_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_OUT  SOC_PB_PP_MGMT_OPERATION_MODE    *oper_mode_p
  )
{
  SOC_SAND_RET
    res = SOC_SAND_OK;
  if (Soc_pb_pp_sw_db.device[unit] == NULL)
  {
    return SOC_SAND_ERR;
  }
  res = SOC_SAND_OK; sal_memcpy(
          oper_mode_p,
          Soc_pb_pp_sw_db.device[unit]->oper_mode,
          sizeof(*oper_mode_p)
        );
 return res;


}



uint32
  soc_pb_pp_sw_db_oper_mode_terminate(
    SOC_SAND_IN  int unit
  )
{
  SOC_PETRA_FREE_ANY_SIZE(Soc_pb_pp_sw_db.device[unit]->oper_mode);

  return SOC_SAND_OK;
}

uint32
  soc_pb_pp_sw_db_llp_filter_initialize(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_SAND_OK;

  SOC_PETRA_ALLOC_ANY_SIZE(
    Soc_pb_pp_sw_db.device[unit]->llp_filter,
    SOC_PB_PP_SW_DB_LLP_FILTER,
    1);

  /* Init each member of llp filter specifically */

  /* llp_filter>ref_count { */
  
  sal_memset(
    &(Soc_pb_pp_sw_db.device[unit]->llp_filter->ref_count),
    0x0,
    sizeof(Soc_pb_pp_sw_db.device[unit]->llp_filter->ref_count)
    );

  /* llp_filter>ref_count } */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_llp_filter_initialize()",0,0);
}

uint32
  soc_pb_pp_sw_db_llp_filter_terminate(
    SOC_SAND_IN  int unit
  )
{
  SOC_PETRA_FREE_ANY_SIZE(Soc_pb_pp_sw_db.device[unit]->llp_filter);

  return SOC_SAND_OK;
}

uint32
  soc_pb_pp_sw_db_llp_filter_desig_vlan_table_ref_count_set(
    SOC_SAND_IN  int                                        unit,
    SOC_SAND_IN  SOC_PB_PP_LLP_FILTER_DESIGNATED_VLAN_TABLE_REF_COUNT *ref_count
  )
{
  SOC_PB_PP_SW_DB_FIELD_SET(unit, llp_filter->ref_count,ref_count);
  BCM_DIRTY_MARK;
}

uint32
  soc_pb_pp_sw_db_llp_filter_desig_vlan_table_ref_count_get(
    SOC_SAND_IN  int                                        unit,
    SOC_SAND_OUT SOC_PB_PP_LLP_FILTER_DESIGNATED_VLAN_TABLE_REF_COUNT *ref_count
  )
{
  SOC_PB_PP_SW_DB_FIELD_GET(unit, llp_filter->ref_count,ref_count);
}


/* for member group */
STATIC uint32
  soc_pb_pp_sw_db_lpm_ll_member_entry_set(
    SOC_SAND_IN   int                            unit,
    SOC_SAND_IN   uint32                            bank_id,
    SOC_SAND_IN  uint32                             member_ndx,
    SOC_SAND_IN  SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY      *member_entry
  )
{
  uint32
    mem_inst;
  uint8
    is_cached;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  is_cached = SOC_SAND_ARR_MEM_ALLOCATOR_IS_CACHED_INST(bank_id);
  mem_inst = SOC_SAND_ARR_MEM_ALLOCATOR_BANK_INST(bank_id);
  
  SOC_PETRA_COPY(
             &(SOC_PB_PP_SW_DB_ACTIVE_INS(is_cached,Soc_pb_pp_sw_db.device[unit]->ipv4_info->lpm_mngr.init_info.rev_ptrs[mem_inst]->group_members_data.members)[member_ndx]),
             member_entry,
             SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY,
             1
            );
  BCM_DIRTY_MARK;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_lpm_ll_member_entry_set()",0,0);
}

STATIC uint32
  soc_pb_pp_sw_db_lpm_ll_member_entry_get(
    SOC_SAND_IN   int                            unit,
    SOC_SAND_IN   uint32                            bank_id,
    SOC_SAND_IN  uint32                             member_ndx,
    SOC_SAND_OUT  SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY      *member_entry
  )
{
  uint32
    mem_inst;
  uint8
    is_cached;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  is_cached = SOC_SAND_ARR_MEM_ALLOCATOR_IS_CACHED_INST(bank_id);
  mem_inst = SOC_SAND_ARR_MEM_ALLOCATOR_BANK_INST(bank_id);
  
  SOC_PETRA_COPY(
             member_entry,
             &(SOC_PB_PP_SW_DB_ACTIVE_INS(is_cached,Soc_pb_pp_sw_db.device[unit]->ipv4_info->lpm_mngr.init_info.rev_ptrs[mem_inst]->group_members_data.members)[member_ndx]),
             SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY,
             1
            );
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_lpm_ll_member_entry_get()",0,0);
}


STATIC uint32
  soc_pb_pp_sw_db_lpm_ll_group_entry_set(
    SOC_SAND_IN   int                          unit,
    SOC_SAND_IN   uint32                          bank_id,
    SOC_SAND_IN  uint32                           group_ndx,
    SOC_SAND_IN  SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY      *group_entry
  )
{
  uint32
    mem_inst;
  uint8
    is_cached;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  is_cached = SOC_SAND_ARR_MEM_ALLOCATOR_IS_CACHED_INST(bank_id);
  mem_inst = SOC_SAND_ARR_MEM_ALLOCATOR_BANK_INST(bank_id);
  
  SOC_PETRA_COPY(
             &(SOC_PB_PP_SW_DB_ACTIVE_INS(is_cached,Soc_pb_pp_sw_db.device[unit]->ipv4_info->lpm_mngr.init_info.rev_ptrs[mem_inst]->group_members_data.groups)[group_ndx]),
             group_entry,
             SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY,
             1
            );

  BCM_DIRTY_MARK;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_lpm_ll_group_entry_set()",0,0);
}

STATIC uint32
  soc_pb_pp_sw_db_lpm_ll_group_entry_get(
    SOC_SAND_IN   int                          unit,
    SOC_SAND_IN   uint32                          bank_id,
    SOC_SAND_IN  uint32                             group_ndx,
    SOC_SAND_OUT  SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY      *group_entry
  )
{

  uint32
    mem_inst;
  uint8
    is_cached;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  is_cached = SOC_SAND_ARR_MEM_ALLOCATOR_IS_CACHED_INST(bank_id);
  mem_inst = SOC_SAND_ARR_MEM_ALLOCATOR_BANK_INST(bank_id);
  
  SOC_PETRA_COPY(
             group_entry,
             &(SOC_PB_PP_SW_DB_ACTIVE_INS(is_cached,Soc_pb_pp_sw_db.device[unit]->ipv4_info->lpm_mngr.init_info.rev_ptrs[mem_inst]->group_members_data.groups)[group_ndx]),
             SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY,
             1
            );
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_lpm_ll_group_entry_get()",0,0);
}


uint32
  soc_pb_pp_sw_db_ipv4_initialize(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32  nof_vrfs,
    SOC_SAND_IN uint32  max_nof_routes[SOC_DPP_NOF_VRFS_PETRAB],
    SOC_SAND_IN uint32  nof_vrf_bits,
    SOC_SAND_IN uint32  nof_banks,
    SOC_SAND_IN uint32  *nof_bits_per_bank,
    SOC_SAND_IN uint32  *bank_to_mem,
    SOC_SAND_IN uint32  nof_mems,
    SOC_SAND_IN uint32  *nof_rows_per_mem, /* must be uint32 */
    SOC_SAND_IN SOC_PB_PP_PXX_MODEL pxx_model,
    SOC_SAND_IN uint32  flags,
    SOC_SAND_IN SOC_SAND_PP_SYSTEM_FEC_ID  default_sys_fec
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    vrf_indx;
  uint32
    indx;
  SOC_PB_PP_SW_DB_IPV4_INFO
    *ipv4_info;
  SOC_PB_PP_IPV4_LPM_MNGR_INIT_INFO
    *lpm_init_info;
  uint8
    support_defrag;
  SOC_SAND_GROUP_MEM_LL_INFO
    *members_info;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PETRA_ALLOC_ANY_SIZE(
    Soc_pb_pp_sw_db.device[unit]->ipv4_info,
    SOC_PB_PP_SW_DB_IPV4_INFO,
    1
  );

  ipv4_info = Soc_pb_pp_sw_db.device[unit]->ipv4_info;

  sal_memset(
    ipv4_info,
    0x0,
    sizeof(*ipv4_info)
  );

  ipv4_info->nof_vrfs = nof_vrfs;
  SOC_SAND_MALLOC(ipv4_info->vrf_modified_bitmask, (sizeof(*ipv4_info->vrf_modified_bitmask)) * SOC_PB_PP_SW_IPV4_VRF_BITMAP_SIZE, "vrf_modified_bitmask");
  sal_memset(ipv4_info->vrf_modified_bitmask, 0, (sizeof(*ipv4_info->vrf_modified_bitmask)) * SOC_PB_PP_SW_IPV4_VRF_BITMAP_SIZE);

  ipv4_info->default_fec = default_sys_fec;
  ipv4_info->uc_host_table_resources = SOC_PB_PP_FRWRD_IPV4_HOST_TABLE_RESOURCE_LEM_THEN_LPM;
  ipv4_info->mc_host_table_resources = SOC_PB_PP_FRWRD_IPV4_MC_HOST_TABLE_RESOURCE_TCAM_ONLY;
  
  /* for each vrf, there is a default route */
  ipv4_info->nof_lpm_entries_in_lpm = 0;

  soc_pb_pp_PB_PP_IPV4_LPM_MNGR_INFO_clear(&ipv4_info->lpm_mngr);

  SOC_PETRA_ALLOC(ipv4_info->free_list_size,uint32,nof_mems);
  SOC_PETRA_ALLOC(ipv4_info->free_list,uint16*,nof_mems);
  

  lpm_init_info = &(ipv4_info->lpm_mngr.init_info);

  lpm_init_info->prime_handle = unit;
  lpm_init_info->sec_handle = 0;
  lpm_init_info->nof_vrf_bits = nof_vrf_bits;
  lpm_init_info->nof_banks = nof_banks;

  SOC_SAND_MALLOC(lpm_init_info->nof_bits_per_bank, sizeof(*lpm_init_info->nof_bits_per_bank) * nof_banks,"nof_bits_per_bank");
  sal_memcpy(lpm_init_info->nof_bits_per_bank, nof_bits_per_bank, sizeof(*lpm_init_info->nof_bits_per_bank) * nof_banks);
  
  SOC_SAND_MALLOC(lpm_init_info->bank_to_mem, sizeof(*lpm_init_info->bank_to_mem) * nof_banks, "bank_to_mem");
  sal_memcpy(lpm_init_info->bank_to_mem, bank_to_mem, sizeof(*lpm_init_info->bank_to_mem) * nof_banks);

  lpm_init_info->nof_mems = nof_mems;
  
  SOC_SAND_MALLOC(lpm_init_info->nof_rows_per_mem, sizeof(*lpm_init_info->nof_rows_per_mem) * nof_mems, "nof_rows_per_mem");
  sal_memcpy(lpm_init_info->nof_rows_per_mem, nof_rows_per_mem, sizeof(*lpm_init_info->nof_rows_per_mem) * nof_mems);
  
  lpm_init_info->mem_alloc_get_fun = soc_pb_pp_ipv4_lpm_test_mem_to_mem_allocator_get;
  lpm_init_info->pat_tree_get_fun = soc_pb_pp_ipv4_lpm_vrf_to_lpm_db;
  lpm_init_info->pxx_model = pxx_model;
  lpm_init_info->nof_lpms = nof_vrfs;

  lpm_init_info->nof_entries_for_hw_lpm_set_fun = soc_pb_pp_sw_db_ipv4_nof_lpm_entries_in_lpm_set;
  lpm_init_info->nof_entries_for_hw_lpm_get_fun = soc_pb_pp_sw_db_ipv4_nof_lpm_entries_in_lpm_get;
  lpm_init_info->max_nof_entries_for_hw_lpm = 10 * 1024;
  support_defrag = (uint8) flags & SOC_PB_PP_MGMT_IPV4_OP_MODE_SUPPORT_DEFRAG;
 /*
  * init the LPM- PAT trees
  */
  SOC_SAND_MALLOC(lpm_init_info->lpms, sizeof(SOC_SAND_PAT_TREE_INFO) * lpm_init_info->nof_lpms,"lpm_init_info->lpms");
  for (indx = 0; indx < lpm_init_info->nof_lpms; ++indx)
  {
      soc_sand_SAND_PAT_TREE_INFO_clear(&(lpm_init_info->lpms[indx]));
  }

  if (flags & SOC_PB_PP_MGMT_IPV4_OP_MODE_SUPPORT_CACHE)
  {
    lpm_init_info->flags |= SOC_PB_PP_LPV4_LPM_SUPPORT_CACHE;
  }
  if (flags & SOC_PB_PP_MGMT_IPV4_OP_MODE_SUPPORT_DEFRAG)
  {
    lpm_init_info->flags |= SOC_PB_PP_LPV4_LPM_SUPPORT_DEFRAG;
  }
  for (indx = 0; indx < lpm_init_info->nof_lpms; ++indx)
  {
    lpm_init_info->lpms[indx].tree_size = max_nof_routes[indx]*2; /*pat_size*/
    lpm_init_info->lpms[indx].node_set_fun = soc_pb_pp_sw_db_pat_tree_node_set;
    lpm_init_info->lpms[indx].node_get_fun = soc_pb_pp_sw_db_pat_tree_node_get;
    lpm_init_info->lpms[indx].node_ref_get_fun = NULL;/*soc_pb_pp_sw_db_pat_tree_node_ref_get;*/
    lpm_init_info->lpms[indx].root_set_fun = soc_pb_pp_sw_db_pat_tree_root_set;
    lpm_init_info->lpms[indx].root_get_fun = soc_pb_pp_sw_db_pat_tree_root_get;
    lpm_init_info->lpms[indx].prime_handle = unit;
    lpm_init_info->lpms[indx].sec_handle = indx;
    lpm_init_info->lpms[indx].support_cache = (uint8) flags & SOC_PB_PP_MGMT_IPV4_OP_MODE_SUPPORT_CACHE;
  }
  /*
  * init the Mem allocator
  */
  SOC_SAND_MALLOC(lpm_init_info->mem_allocators, sizeof(SOC_SAND_ARR_MEM_ALLOCATOR_INFO) * lpm_init_info->nof_mems,"mem_allocators");

  for (indx = 0; indx < lpm_init_info->nof_mems; ++indx)
  {
    soc_sand_SAND_ARR_MEM_ALLOCATOR_INFO_clear(&(lpm_init_info->mem_allocators[indx]));
    lpm_init_info->mem_allocators[indx].entry_get_fun = soc_pb_pp_sw_db_ipv4_arr_mem_allocator_entry_get;
    lpm_init_info->mem_allocators[indx].entry_set_fun = soc_pb_pp_sw_db_ipv4_arr_mem_allocator_entry_set;
    lpm_init_info->mem_allocators[indx].free_set_fun = soc_pb_pp_sw_db_ipv4_arr_mem_allocator_free_entry_set;
    lpm_init_info->mem_allocators[indx].free_get_fun = soc_pb_pp_sw_db_ipv4_arr_mem_allocator_free_entry_get;
    lpm_init_info->mem_allocators[indx].write_fun = soc_pb_pp_sw_db_ipv4_arr_mem_allocator_write;
    lpm_init_info->mem_allocators[indx].read_fun = soc_pb_pp_sw_db_ipv4_arr_mem_allocator_read;
    lpm_init_info->mem_allocators[indx].entry_move_fun = soc_pb_pp_sw_db_ipv4_lpm_mem_align;
    lpm_init_info->mem_allocators[indx].instance_prim_handle = unit;
    lpm_init_info->mem_allocators[indx].instance_sec_handle = indx;
    lpm_init_info->mem_allocators[indx].nof_entries = lpm_init_info->nof_rows_per_mem[indx];
    lpm_init_info->mem_allocators[indx].support_caching = (uint8) flags & SOC_PB_PP_MGMT_IPV4_OP_MODE_SUPPORT_CACHE;
    lpm_init_info->mem_allocators[indx].support_defragment = (indx==0)?FALSE:support_defrag;
    lpm_init_info->mem_allocators[indx].max_block_size = (indx==0)?1:((1 << nof_bits_per_bank[indx])*2);
    SOC_PETRA_ALLOC_ANY_SIZE(ipv4_info->free_list[indx],uint16,SOC_PB_PP_SW_DB_FREE_LIST_MAX_SIZE);
  }
  /* allocate information for defragment if needed */
  if (lpm_init_info->flags & SOC_PB_PP_LPV4_LPM_SUPPORT_DEFRAG)
  {
    SOC_PETRA_ALLOC_ANY_SIZE(lpm_init_info->rev_ptrs,SOC_SAND_GROUP_MEM_LL_INFO*,lpm_init_info->nof_mems);
    /* allocate member groups */
    /* for first there is no pointers on it*/
    for (indx = 1; (uint32)indx < lpm_init_info->nof_mems; ++indx)
    {
      SOC_PETRA_ALLOC_ANY_SIZE(lpm_init_info->rev_ptrs[indx],SOC_SAND_GROUP_MEM_LL_INFO,1);
      members_info = lpm_init_info->rev_ptrs[indx];
      soc_sand_SAND_GROUP_MEM_LL_INFO_clear(members_info);
      members_info->auto_remove = TRUE;
      members_info->group_set_fun = soc_pb_pp_sw_db_lpm_ll_group_entry_set;
      members_info->group_get_fun = soc_pb_pp_sw_db_lpm_ll_group_entry_get;
      members_info->member_set_fun = soc_pb_pp_sw_db_lpm_ll_member_entry_set;
      members_info->member_get_fun = soc_pb_pp_sw_db_lpm_ll_member_entry_get;
      members_info->nof_elements = lpm_init_info->nof_rows_per_mem[indx-1];/* pointers to me */

      members_info->nof_groups = lpm_init_info->nof_rows_per_mem[indx];; /*size of fec table*/
      members_info->instance_prim_handle = unit;
      members_info->instance_sec_handle = indx; /*the use of the group*/
      members_info->support_caching = (uint8)lpm_init_info->flags & SOC_PB_PP_LPV4_LPM_SUPPORT_CACHE;

    }
  }
 /*
  * init the memory shadow
  */
  res = soc_pb_pp_ipv4_lpm_mngr_create(&ipv4_info->lpm_mngr);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  for(vrf_indx = 0; vrf_indx < nof_vrfs; ++vrf_indx)
  {
    soc_pb_pp_ipv4_lpm_mngr_vrf_init(
      &ipv4_info->lpm_mngr,
      vrf_indx,
      default_sys_fec
    );
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_ipv4_initialize()",0,0);
}


uint32
  soc_pb_pp_sw_db_ipv4_terminate(
    SOC_SAND_IN  int unit
  )
{
  uint32
    indx;
  SOC_PB_PP_SW_DB_IPV4_INFO
    *ipv4_info;
  SOC_PB_PP_IPV4_LPM_MNGR_INIT_INFO
    *lpm_init_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ipv4_info = Soc_pb_pp_sw_db.device[unit]->ipv4_info;
  if (ipv4_info == NULL)
  {
    return SOC_SAND_OK;
  }

  lpm_init_info = &(ipv4_info->lpm_mngr.init_info);

  for (indx = 0; indx < lpm_init_info->nof_mems; ++indx)
  {
    SOC_PETRA_FREE_ANY_SIZE(ipv4_info->free_list[indx]);
  }
  SOC_PETRA_FREE_ANY_SIZE(ipv4_info->free_list_size);
  SOC_PETRA_FREE_ANY_SIZE(ipv4_info->free_list);

  soc_pb_pp_ipv4_lpm_mngr_destroy(&ipv4_info->lpm_mngr);
  sal_free(ipv4_info->lpm_mngr.init_info.nof_bits_per_bank);
  sal_free(ipv4_info->lpm_mngr.init_info.bank_to_mem);
  sal_free(ipv4_info->lpm_mngr.init_info.nof_rows_per_mem);
  sal_free(ipv4_info->lpm_mngr.init_info.lpms);
  sal_free(ipv4_info->lpm_mngr.init_info.mem_allocators);
  sal_free(ipv4_info->vrf_modified_bitmask);

  SOC_PETRA_FREE_ANY_SIZE(Soc_pb_pp_sw_db.device[unit]->ipv4_info);

  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_ipv4_terminate()",0,0);
}



uint32
  soc_pb_pp_sw_db_ipv4_uc_table_resources_set(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_IN SOC_PB_PP_FRWRD_IPV4_HOST_TABLE_RESOURCE		       uc_host_table_resources
)
{
  SOC_PB_PP_SW_DB_IPV4_FIELD_SET(unit, ipv4_info->uc_host_table_resources, &uc_host_table_resources);
  BCM_DIRTY_MARK;
}

uint32
  soc_pb_pp_sw_db_ipv4_uc_table_resources_get(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IPV4_HOST_TABLE_RESOURCE		       *uc_host_table_resources
  )
{
  SOC_PB_PP_SW_DB_IPV4_FIELD_GET(unit, ipv4_info->uc_host_table_resources, uc_host_table_resources);
}


uint32
  soc_pb_pp_sw_db_ipv4_mc_table_resources_set(
  SOC_SAND_IN int                                      unit,
  SOC_SAND_IN SOC_PB_PP_FRWRD_IPV4_MC_HOST_TABLE_RESOURCE		       mc_host_table_resources
 )
{
  SOC_PB_PP_SW_DB_IPV4_FIELD_SET(unit, ipv4_info->mc_host_table_resources, &mc_host_table_resources);
  BCM_DIRTY_MARK;
}


uint32
  soc_pb_pp_sw_db_ipv4_mc_table_resources_get(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IPV4_MC_HOST_TABLE_RESOURCE		     *mc_host_table_resources
  )
{
  SOC_PB_PP_SW_DB_IPV4_FIELD_GET(unit, ipv4_info->mc_host_table_resources, mc_host_table_resources);
}

uint32
  soc_pb_pp_sw_db_ipv4_cache_mode_for_ip_type_get(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_IN uint32                                     type,
    SOC_SAND_OUT uint8	                          	     *pending_op
)
{
  uint32
    res = SOC_SAND_OK,
    bitmask = 0;

  res = soc_pb_pp_sw_db_ipv4_cache_mode_get(
          unit,
         &bitmask
        );

  *pending_op = (type & bitmask) ? TRUE : FALSE;

  return res;
}

uint32
  soc_pb_pp_sw_db_ipv4_cache_mode_get(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_OUT uint32                                     *cache_mode_bitmask
)
{
  SOC_PB_PP_SW_DB_IPV4_FIELD_GET(
    unit,
    ipv4_info->cache_mode,
    cache_mode_bitmask
  );
}

uint32
  soc_pb_pp_sw_db_ipv4_cache_mode_set(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_IN uint32                                     cache_mode_bitmask
)
{
  SOC_PB_PP_SW_DB_IPV4_FIELD_SET(
    unit,
    ipv4_info->cache_mode,
    &cache_mode_bitmask
  );
  BCM_DIRTY_MARK;
}



uint32
  soc_pb_pp_sw_db_ipv4_cache_modified_set(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_IN uint8	                          	     modified
)
{
  SOC_PB_PP_SW_DB_IPV4_FIELD_SET(
    unit,
    ipv4_info->cache_modified,
    &modified
  );
  BCM_DIRTY_MARK;
}

uint32
  soc_pb_pp_sw_db_ipv4_cache_modified_get(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_OUT uint8	                          	     *modified
)
{
  SOC_PB_PP_SW_DB_IPV4_FIELD_GET(
    unit,
    ipv4_info->cache_modified,
    modified
  );
}

uint32
  soc_pb_pp_sw_db_ipv4_cache_vrf_modified_get(
    SOC_SAND_IN int                                  unit,
    SOC_SAND_IN  uint32                                  vrf_ndx,
    SOC_SAND_OUT uint32                                  *vrf_modified
)
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (Soc_pb_pp_sw_db.device[unit] == NULL || Soc_pb_pp_sw_db.device[unit]->ipv4_info == NULL)
  {
    return SOC_SAND_ERR;
  }
  *vrf_modified = 0;

  res = soc_sand_bitstream_set_any_field(
          Soc_pb_pp_sw_db.device[unit]->ipv4_info->vrf_modified_bitmask,
          vrf_ndx,
          1,
          (vrf_modified)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
exit:
SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_ipv4_cache_vrf_modified_set()",vrf_ndx,0);
}

uint32
  soc_pb_pp_sw_db_ipv4_cache_vrf_modified_set(
    SOC_SAND_IN int                                  unit,
    SOC_SAND_IN  uint32                                  vrf_ndx,
    SOC_SAND_IN uint32                                   vrf_modified
)
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (Soc_pb_pp_sw_db.device[unit] == NULL || Soc_pb_pp_sw_db.device[unit]->ipv4_info == NULL)
  {
    return SOC_SAND_ERR;
  }
  res = soc_sand_bitstream_set_any_field(
          &(vrf_modified),
          vrf_ndx,
          1,
          Soc_pb_pp_sw_db.device[unit]->ipv4_info->vrf_modified_bitmask
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  BCM_DIRTY_MARK;

exit:
SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_ipv4_cache_vrf_modified_set()",vrf_ndx,0);
}

uint32
soc_pb_pp_sw_db_ipv4_cache_modified_bitmap_set(
  SOC_SAND_IN int                                    unit,
  SOC_SAND_IN uint32	                          	     *modified_bitmap
  )
{
  SOC_SAND_RET
    soc_sand_ret = SOC_SAND_OK;
  if (Soc_pb_pp_sw_db.device[unit] == NULL || Soc_pb_pp_sw_db.device[unit]->ipv4_info == NULL)
  {
    return SOC_SAND_ERR;
  }

  sal_memcpy(
    Soc_pb_pp_sw_db.device[unit]->ipv4_info->vrf_modified_bitmask,
    modified_bitmap,
    SOC_PB_PP_SW_IPV4_VRF_BITMAP_SIZE * sizeof(uint32)
    );
  BCM_DIRTY_MARK;

  return soc_sand_ret;
}

uint32
soc_pb_pp_sw_db_ipv4_cache_modified_bitmap_get(
  SOC_SAND_IN int                                    unit,
  SOC_SAND_OUT uint32	                          	     *modified_bitmap
)
{
  SOC_SAND_RET
    soc_sand_ret = SOC_SAND_OK;
  if (Soc_pb_pp_sw_db.device[unit] == NULL || Soc_pb_pp_sw_db.device[unit]->ipv4_info == NULL)
  {
    return SOC_SAND_ERR;
  }

  sal_memcpy(
    modified_bitmap,
    Soc_pb_pp_sw_db.device[unit]->ipv4_info->vrf_modified_bitmask,
    SOC_PB_PP_SW_IPV4_VRF_BITMAP_SIZE * sizeof(uint32)
  );

    return soc_sand_ret;
}


uint32
  soc_pb_pp_sw_db_ipv4_lpm_mngr_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN SOC_PB_PP_IPV4_LPM_MNGR_INFO  *lpm_mngr
  )
{
    SOC_PB_PP_SW_DB_FIELD_SET(
      unit,
      ipv4_info->lpm_mngr,
      lpm_mngr
    );
    BCM_DIRTY_MARK;
}

uint32
  soc_pb_pp_sw_db_ipv4_lpm_mngr_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT  SOC_PB_PP_IPV4_LPM_MNGR_INFO  *lpm_mngr
  )
{
  SOC_SAND_RET
    soc_sand_ret = SOC_SAND_OK;

  if (Soc_pb_pp_sw_db.device[unit] == NULL || Soc_pb_pp_sw_db.device[unit]->ipv4_info == NULL){
    return SOC_SAND_ERR;
  }

  if (Soc_pb_pp_sw_db.device[unit]->oper_mode->ipv4_info.nof_vrfs == 0)
  {
    return SOC_SAND_VALUE_ABOVE_MAX_ERR;/* no routing enabled */
  }

  soc_sand_ret = SOC_SAND_OK; sal_memcpy(
      lpm_mngr,
      &(Soc_pb_pp_sw_db.device[unit]->ipv4_info->lpm_mngr),
      sizeof(*lpm_mngr)
    );
  return soc_sand_ret;
}

#ifdef SOC_SAND_DEBUG
uint32
  soc_pb_pp_sw_db_ipv4_lem_add_fail_set(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN uint8    *ipv4_lem_add_fail
  )
{
  SOC_PB_PP_SW_DB_IPV4_FIELD_SET(
    unit,
    ipv4_info->lem_add_fail,
    ipv4_lem_add_fail
    );
  BCM_DIRTY_MARK;
}

uint32
  soc_pb_pp_sw_db_ipv4_lem_add_fail_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_OUT  uint8  *ipv4_lem_add_fail
  )
{
  SOC_PB_PP_SW_DB_IPV4_FIELD_GET(
    unit,
    ipv4_info->lem_add_fail,
    ipv4_lem_add_fail
    );
}
#endif

/* } */

/* ILM { */

uint32
  soc_pb_pp_sw_db_ilm_initialize(
    SOC_SAND_IN int unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PETRA_ALLOC_ANY_SIZE(
    Soc_pb_pp_sw_db.device[unit]->ilm_info,
    SOC_PB_PP_SW_DB_ILM_INFO,
    1
  );

  sal_memset(
    Soc_pb_pp_sw_db.device[unit]->ilm_info,
    0x0,
    sizeof(*Soc_pb_pp_sw_db.device[unit]->ilm_info)
  );

  Soc_pb_pp_sw_db.device[unit]->ilm_info->mask_inrif = FALSE;
  Soc_pb_pp_sw_db.device[unit]->ilm_info->mask_inrif = FALSE;
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_ilm_initialize()",0,0);
}

uint32
  soc_pb_pp_sw_db_ilm_terminate(
    SOC_SAND_IN  int unit
  )
{
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PETRA_FREE_ANY_SIZE(Soc_pb_pp_sw_db.device[unit]->ilm_info);


  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_device_mtr_info_initialize()",0,0);
}

uint32
  soc_pb_pp_sw_db_ilm_key_mask_port_set(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN uint8    mask_port
  )
{
  SOC_PB_PP_SW_DB_FIELD_SET(
    unit,
    ilm_info->mask_port,
    &mask_port
    );
  BCM_DIRTY_MARK;
}

uint32
  soc_pb_pp_sw_db_ilm_key_mask_port_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_OUT  uint8  *mask_port
  )
{
  SOC_PB_PP_SW_DB_FIELD_GET(
    unit,
    ilm_info->mask_port,
    mask_port
    );
}
uint32
  soc_pb_pp_sw_db_ilm_key_mask_inrif_set(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN uint8    mask_inrif
  )
{
  SOC_PB_PP_SW_DB_FIELD_SET(
    unit,
    ilm_info->mask_inrif,
    &mask_inrif
    );
  BCM_DIRTY_MARK;
}

uint32
  soc_pb_pp_sw_db_ilm_key_mask_inrif_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_OUT  uint8  *mask_inrif
  )
{
  SOC_PB_PP_SW_DB_FIELD_GET(
    unit,
    ilm_info->mask_inrif,
    mask_inrif
    );
}

/* } */
/* LLP TRAP { */

uint32
  soc_pb_pp_sw_db_llp_trap_initialize(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res;
  SOC_SAND_MULTI_SET_INFO
    *multi_set_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_SAND_OK;

  SOC_PETRA_ALLOC_ANY_SIZE(
    Soc_pb_pp_sw_db.device[unit]->llp_trap,
    SOC_PB_PP_SW_DB_LLP_TRAP,
    1);

  /* Init each member of llp trap specifically */

  /* llp_trap->l3_protocols_multi_set { */
  
  multi_set_info = &(Soc_pb_pp_sw_db.device[unit]->llp_trap->l3_protocols_multi_set);
  soc_sand_SAND_MULTI_SET_INFO_clear(multi_set_info);
  multi_set_info->init_info.get_entry_fun = soc_pb_sw_db_buffer_get_entry;
  multi_set_info->init_info.set_entry_fun = soc_pb_sw_db_buffer_set_entry;
  multi_set_info->init_info.max_duplications = SOC_SAND_U32_MAX - 1;
  multi_set_info->init_info.member_size = sizeof(uint32);
  multi_set_info->init_info.nof_members = SOC_PB_PP_LLP_TRAP_NOF_UD_L3_PROTOCOLS;
  multi_set_info->init_info.sec_handle = 0;
  multi_set_info->init_info.prime_handle = unit;
  res = soc_sand_multi_set_create(
    multi_set_info
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* llp_trap->l3_protocols_multi_set } */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_device_mtr_info_initialize()",0,0);
}

uint32
  soc_pb_pp_sw_db_llp_trap_terminate(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_SAND_OK;

  res = soc_sand_multi_set_destroy(
    &(Soc_pb_pp_sw_db.device[unit]->llp_trap->l3_protocols_multi_set)
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_PETRA_FREE_ANY_SIZE(Soc_pb_pp_sw_db.device[unit]->llp_trap);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_llp_trap_terminate()",0,0);
}

/* LLP TRAP } */

/* LLP MIRROR { */

uint32
  soc_pb_pp_sw_db_llp_mirror_initialize(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res,
    internal_vid_ndx;
  SOC_SAND_MULTI_SET_INFO
    *multi_set_info;
  SOC_PB_PP_PORT
    pp_port_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_SAND_OK;

  SOC_PETRA_ALLOC_ANY_SIZE(
    Soc_pb_pp_sw_db.device[unit]->llp_mirror,
    SOC_PB_PP_SW_DB_LLP_MIRROR,
    1);

  /* Init each member of llp mirror specifically */
  for (pp_port_ndx = 0; pp_port_ndx < SOC_PB_PORT_NOF_PP_PORTS; ++pp_port_ndx)
  {
    for (internal_vid_ndx = 0; internal_vid_ndx < SOC_PB_PP_LLP_MIRROR_NOF_VID_MIRROR_INDICES; ++internal_vid_ndx)
    {
      Soc_pb_pp_sw_db.device[unit]->llp_mirror->llp_mirror_port_vlan[pp_port_ndx][internal_vid_ndx] = FALSE;
    }
  }


  /* llp_mirror->mirror_profile_multi_set { */

  multi_set_info = &(Soc_pb_pp_sw_db.device[unit]->llp_mirror->mirror_profile_multi_set);
  soc_sand_SAND_MULTI_SET_INFO_clear(multi_set_info);
  multi_set_info->init_info.get_entry_fun = soc_pb_sw_db_buffer_get_entry;
  multi_set_info->init_info.set_entry_fun = soc_pb_sw_db_buffer_set_entry;
  multi_set_info->init_info.max_duplications = SOC_SAND_U32_MAX - 1;
  multi_set_info->init_info.member_size = sizeof(uint32);
  multi_set_info->init_info.nof_members = SOC_PB_PP_LLP_MIRROR_NOF_VID_MIRROR_INDICES;
  multi_set_info->init_info.sec_handle = 0;
  multi_set_info->init_info.prime_handle = unit;
  res = soc_sand_multi_set_create(
    multi_set_info
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* llp_mirror->mirror_profile_multi_set } */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_llp_mirror_initialize()",0,0);
}

uint32
  soc_pb_pp_sw_db_llp_mirror_terminate(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_SAND_OK;

  res = soc_sand_multi_set_destroy(
    &(Soc_pb_pp_sw_db.device[unit]->llp_mirror->mirror_profile_multi_set)
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_PETRA_FREE_ANY_SIZE(Soc_pb_pp_sw_db.device[unit]->llp_mirror);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_llp_mirror_terminate()",0,0);
}

/* LLP MIRROR } */


/* LLP VID_ASSIGN { */

uint32
  soc_pb_pp_sw_db_llp_vid_assign_initialize(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_SAND_OK;

  SOC_PETRA_ALLOC_ANY_SIZE(
    Soc_pb_pp_sw_db.device[unit]->llp_vid_assign,
    SOC_PB_PP_SW_DB_LLP_VID_ASSIGN,
    1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_llp_vid_assign_initialize()",0,0);
}

uint32
  soc_pb_pp_sw_db_llp_vid_assign_terminate(
    SOC_SAND_IN  int unit
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PETRA_FREE_ANY_SIZE(Soc_pb_pp_sw_db.device[unit]->llp_vid_assign);

  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_llp_vid_assign_terminate()",0,0);
}



uint32
  soc_pb_pp_sw_db_llp_vid_assign_port_sa_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_PB_PP_PORT            local_port_ndx,
    SOC_SAND_IN  uint8             enable
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  soc_sand_bitstream_set(Soc_pb_pp_sw_db.device[unit]->llp_vid_assign->vid_sa_based_enable,local_port_ndx,SOC_SAND_BOOL2NUM(enable));
  BCM_DIRTY_MARK;

  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_llp_vid_assign_port_sa_set()",0,0);
}

uint32
  soc_pb_pp_sw_db_llp_vid_assign_port_sa_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_PB_PP_PORT            local_port_ndx,
    SOC_SAND_OUT  uint8             *enable
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  *enable = SOC_SAND_NUM2BOOL(
              soc_sand_bitstream_test_bit(Soc_pb_pp_sw_db.device[unit]->llp_vid_assign->vid_sa_based_enable,local_port_ndx)
            );
  
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_llp_vid_assign_port_sa_get()",0,0);
}

/* LLP VID_ASSIGN } */
/* EG MIRROR { */

uint32
  soc_pb_pp_sw_db_eg_mirror_initialize(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res;
  SOC_SAND_MULTI_SET_INFO
    *multi_set_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_SAND_OK;

  SOC_PETRA_ALLOC_ANY_SIZE(
    Soc_pb_pp_sw_db.device[unit]->eg_mirror,
    SOC_PB_PP_SW_DB_EG_MIRROR,
    1);

  /* Init each member of eg mirror specifically */
  multi_set_info = &(Soc_pb_pp_sw_db.device[unit]->eg_mirror->mirror_profile_multi_set);
  soc_sand_SAND_MULTI_SET_INFO_clear(multi_set_info);
  multi_set_info->init_info.get_entry_fun = soc_pb_sw_db_buffer_get_entry;
  multi_set_info->init_info.set_entry_fun = soc_pb_sw_db_buffer_set_entry;
  multi_set_info->init_info.max_duplications = SOC_SAND_U32_MAX - 1;
  multi_set_info->init_info.member_size = sizeof(uint32);
  multi_set_info->init_info.nof_members = SOC_PB_PP_EG_MIRROR_NOF_VID_MIRROR_INDICES;
  multi_set_info->init_info.sec_handle = 0;
  multi_set_info->init_info.prime_handle = unit;
  res = soc_sand_multi_set_create(
    multi_set_info
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_eg_mirror_initialize()",0,0);
}

uint32
  soc_pb_pp_sw_db_eg_mirror_terminate(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_SAND_OK;

  res = soc_sand_multi_set_destroy(
    &(Soc_pb_pp_sw_db.device[unit]->eg_mirror->mirror_profile_multi_set)
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_PETRA_FREE_ANY_SIZE(Soc_pb_pp_sw_db.device[unit]->eg_mirror);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_eg_mirror_terminate()",0,0);
}

uint8
  soc_pb_pp_ipv4_lpm_mngr_pat_tree_node_data_is_identical_fun(
  SOC_SAND_IN SOC_SAND_PAT_TREE_NODE       *node_info_0,
  SOC_SAND_IN SOC_SAND_PAT_TREE_NODE       *node_info_1
  )
{
  if( (SOC_PB_PP_IPV4_LPM_MGR_PAT_DATA_TO_HW_TARGET_TYPE(node_info_0->data) != SOC_PPC_FRWRD_IP_ROUTE_LOCATION_LPM) ||
      (SOC_PB_PP_IPV4_LPM_MGR_PAT_DATA_TO_HW_TARGET_TYPE(node_info_1->data) != SOC_PPC_FRWRD_IP_ROUTE_LOCATION_LPM))
  {
    return TRUE;
  }

  if( (SOC_PB_PP_IPV4_LPM_MGR_PAT_DATA_TO_PENDING_OP_TYPE(node_info_0->data) == SOC_PPC_FRWRD_IP_ROUTE_STATUS_PEND_REMOVE) ||
    (SOC_PB_PP_IPV4_LPM_MGR_PAT_DATA_TO_PENDING_OP_TYPE(node_info_1->data) == SOC_PPC_FRWRD_IP_ROUTE_STATUS_PEND_REMOVE))
  {
    /* It is really dead code because of SOC_PB_PP_IPV4_LPM_MGR_PAT_DATA_TO_PENDING_OP_TYPE(x) is (x>>30) & 0x3
     * and hence, it can't be 4 (SOC_PPC_FRWRD_IP_ROUTE_STATUS_PEND_REMOVE).
     * However, ignore temporary */
    /* coverity[dead_error_line] */
    return TRUE;
  }

  if( SOC_PB_PP_IPV4_LPM_MGR_PAT_DATA_TO_FEC_ID(node_info_0->data) == SOC_PB_PP_IPV4_LPM_MGR_PAT_DATA_TO_FEC_ID(node_info_1->data))
  {
    return (uint8)(node_info_1->data == node_info_0->data && node_info_0->key == node_info_1->key && node_info_0->prefix == node_info_1->prefix);
  }

  return FALSE;
}

uint8
  soc_pb_pp_ipv4_lpm_mngr_pat_node_is_skip_in_lpm_identical_data_query_fun(
    SOC_SAND_IN SOC_SAND_PAT_TREE_NODE       *node_info
  )
{
  /* SOC_PB_PP_IPV4_LPM_MGR_PAT_DATA_TO_HW_TARGET_TYPE(x) is x >> 30, and hence it can't be 4 (SOC_PPC_FRWRD_IP_ROUTE_STATUS_PEND_REMOVE).
   * However, temporary - ignore */
  /* coverity [result_independent_of_operands] */
  if( node_info->is_prefix &&
      ((SOC_PB_PP_IPV4_LPM_MGR_PAT_DATA_TO_HW_TARGET_TYPE(node_info->data) != SOC_PPC_FRWRD_IP_ROUTE_LOCATION_LPM) ||
       (SOC_PB_PP_IPV4_LPM_MGR_PAT_DATA_TO_PENDING_OP_TYPE(node_info->data) == SOC_PPC_FRWRD_IP_ROUTE_STATUS_PEND_REMOVE)))
  {
    return TRUE;
  }

  return FALSE;
}

/* EG MIRROR } */

/* LLP COS { */

uint32
  soc_pb_pp_sw_db_llp_cos_initialize(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res;
  SOC_SAND_MULTI_SET_INFO
    *multi_set_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_SAND_OK;

  SOC_PETRA_ALLOC_ANY_SIZE(
    Soc_pb_pp_sw_db.device[unit]->llp_cos,
    SOC_PB_PP_SW_DB_LLP_COS,
    1);

  /* Init each member of llp cos specifically */

  /* llp_cos->cos_profile_multi_set { */

  multi_set_info = &(Soc_pb_pp_sw_db.device[unit]->llp_cos->ether_type_multi_set);
  soc_sand_SAND_MULTI_SET_INFO_clear(multi_set_info);
  multi_set_info->init_info.get_entry_fun = soc_pb_sw_db_buffer_get_entry;
  multi_set_info->init_info.set_entry_fun = soc_pb_sw_db_buffer_set_entry;
  multi_set_info->init_info.max_duplications = SOC_SAND_U32_MAX - 1;
  multi_set_info->init_info.member_size = sizeof(uint32);
  multi_set_info->init_info.nof_members = SOC_PB_PP_LLP_TRAP_NOF_UD_ETHER_TYPE;
  multi_set_info->init_info.sec_handle = 0;
  multi_set_info->init_info.prime_handle = unit;
  res = soc_sand_multi_set_create(
    multi_set_info
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* llp_cos->cos_profile_multi_set } */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_llp_cos_initialize()",0,0);
}

uint32
  soc_pb_pp_sw_db_llp_cos_terminate(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_SAND_OK;

  res = soc_sand_multi_set_destroy(
    &(Soc_pb_pp_sw_db.device[unit]->llp_cos->ether_type_multi_set)
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_PETRA_FREE_ANY_SIZE(Soc_pb_pp_sw_db.device[unit]->llp_cos);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_llp_cos_terminate()",0,0);
}

/* LLP COS } */

/* ETH_POLICER { */

uint32
  soc_pb_pp_sw_db_eth_policer_initialize(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res;
  SOC_SAND_MULTI_SET_INFO
    *multi_set_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_SAND_OK;

  SOC_PETRA_ALLOC_ANY_SIZE(
    Soc_pb_pp_sw_db.device[unit]->eth_policer_mtr_profile,
    SOC_PB_PPSW_DB_ETH_POLICER_MTR_PROFILE,
    1);

  multi_set_info = &(Soc_pb_pp_sw_db.device[unit]->eth_policer_mtr_profile->eth_meter_profile_multi_set);
  soc_sand_SAND_MULTI_SET_INFO_clear(multi_set_info);
  multi_set_info->init_info.get_entry_fun = soc_pb_sw_db_buffer_get_entry;
  multi_set_info->init_info.set_entry_fun = soc_pb_sw_db_buffer_set_entry;
  multi_set_info->init_info.max_duplications = SOC_SAND_U32_MAX - 1;
  multi_set_info->init_info.member_size = sizeof(uint32);
  multi_set_info->init_info.nof_members = SOC_PB_PP_SW_DB_MULTI_SET_ETH_POLICER_PROFILE_METER_PROFILE_NOF_MEMBER;
  multi_set_info->init_info.sec_handle = 0;
  multi_set_info->init_info.prime_handle = unit;
  res = soc_sand_multi_set_create(
    multi_set_info
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  multi_set_info = &(Soc_pb_pp_sw_db.device[unit]->eth_policer_mtr_profile->global_meter_profile_multi_set);
  soc_sand_SAND_MULTI_SET_INFO_clear(multi_set_info);
  multi_set_info->init_info.get_entry_fun = soc_pb_sw_db_buffer_get_entry;
  multi_set_info->init_info.set_entry_fun = soc_pb_sw_db_buffer_set_entry;
  multi_set_info->init_info.max_duplications = SOC_SAND_U32_MAX - 1;
  multi_set_info->init_info.member_size = sizeof(uint32);
  multi_set_info->init_info.nof_members = SOC_PB_PP_SW_DB_MULTI_SET_ETH_POLICER_GLOBAL_METER_PROFILE_NOF_MEMBER;
  multi_set_info->init_info.sec_handle = 0;
  multi_set_info->init_info.prime_handle = unit;
  res = soc_sand_multi_set_create(
    multi_set_info
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_eth_policer_initialize()",0,0);
}

uint32
  soc_pb_pp_sw_db_eth_policer_terminate(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_SAND_OK;

  res = soc_sand_multi_set_destroy(
    &(Soc_pb_pp_sw_db.device[unit]->eth_policer_mtr_profile->eth_meter_profile_multi_set)
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_sand_multi_set_destroy(
    &(Soc_pb_pp_sw_db.device[unit]->eth_policer_mtr_profile->global_meter_profile_multi_set)
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  
  SOC_PETRA_FREE_ANY_SIZE(Soc_pb_pp_sw_db.device[unit]->eth_policer_mtr_profile);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_eth_policer_terminate()",0,0);
}

void
  soc_pb_pp_sw_db_eth_policer_config_status_bit_set(
    SOC_SAND_IN  int                                        unit,
    SOC_SAND_IN  uint32                                         bit_offset,
    SOC_SAND_IN  uint8                                        status
  )
{

  soc_sand_bitstream_set(Soc_pb_pp_sw_db.device[unit]->eth_policer_mtr_profile->config_meter_status,bit_offset,SOC_SAND_BOOL2NUM(status));
  BCM_DIRTY_MARK;
}

void
  soc_pb_pp_sw_db_eth_policer_config_status_bit_get(
    SOC_SAND_IN  int                                        unit,
    SOC_SAND_IN  uint32                                         bit_offset,
    SOC_SAND_OUT  uint8                                       *status
  )
{
 
  *status = SOC_SAND_NUM2BOOL(soc_sand_bitstream_test_bit(Soc_pb_pp_sw_db.device[unit]->eth_policer_mtr_profile->config_meter_status,bit_offset));
}



/* ETH_POLICER } */

/*
 *	Forward MAC Table
 */

uint32
  soc_pb_pp_sw_db_fwd_mact_initialize(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_SAND_OK;

  SOC_PETRA_ALLOC_ANY_SIZE(
    Soc_pb_pp_sw_db.device[unit]->fwd_mact,
    SOC_PB_PP_SW_DB_FWD_MACT,
    1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_fwd_mact_initialize()",0,0);
}

uint32
  soc_pb_pp_sw_db_fwd_mact_terminate(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res;

  res = SOC_SAND_OK;

  SOC_PETRA_FREE_ANY_SIZE(Soc_pb_pp_sw_db.device[unit]->fwd_mact);

  return res;
}

/*
 *	LIF CoS {
 */

uint32
  soc_pb_pp_sw_db_lif_cos_initialize(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_SAND_OK;

  SOC_PETRA_ALLOC_ANY_SIZE(
    Soc_pb_pp_sw_db.device[unit]->lif_cos,
    SOC_PB_PP_SW_DB_LIF_COS,
    1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_lif_cos_initialize()",0,0);
}

uint32
  soc_pb_pp_sw_db_l2_lif_initialize(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res;
  SOC_SAND_MULTI_SET_INFO
    *multi_set_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_SAND_OK;

  SOC_PETRA_ALLOC_ANY_SIZE(
    Soc_pb_pp_sw_db.device[unit]->l2_lif,
    SOC_PB_PP_SW_DB_L2_LIF,
    1);

  multi_set_info = &(Soc_pb_pp_sw_db.device[unit]->l2_lif->vlan_compression_range_multi_set);
  soc_sand_SAND_MULTI_SET_INFO_clear(multi_set_info);
  multi_set_info->init_info.get_entry_fun = soc_pb_sw_db_buffer_get_entry;
  multi_set_info->init_info.set_entry_fun = soc_pb_sw_db_buffer_set_entry;
  multi_set_info->init_info.max_duplications = SOC_SAND_U32_MAX - 1;
  multi_set_info->init_info.member_size = sizeof(uint32);
  multi_set_info->init_info.nof_members = SOC_PB_PP_SW_DB_MULTI_SET_VLAN_COMPRESSION_RANGE_NOF_MEMBER;
  multi_set_info->init_info.sec_handle = 0;
  multi_set_info->init_info.prime_handle = unit;
  res = soc_sand_multi_set_create(
    multi_set_info
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_l2_lif()",0,0);
}

uint32
  soc_pb_pp_sw_db_l2_lif_terminate(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_SAND_OK;

  res = soc_sand_multi_set_destroy(
    &(Soc_pb_pp_sw_db.device[unit]->l2_lif->vlan_compression_range_multi_set)
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_PETRA_FREE_ANY_SIZE(Soc_pb_pp_sw_db.device[unit]->l2_lif);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_l2_lif_terminate()",0,0);
}



uint32
  soc_pb_pp_sw_db_lif_cos_terminate(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res;

  res = SOC_SAND_OK;

  SOC_PETRA_FREE_ANY_SIZE(Soc_pb_pp_sw_db.device[unit]->lif_cos);

  return res;
}

uint32
  soc_pb_pp_sw_db_lif_table_initialize(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_SAND_OK;

  SOC_PETRA_ALLOC_ANY_SIZE(
    Soc_pb_pp_sw_db.device[unit]->lif_table,
    SOC_PB_PP_SW_DB_LIF_TABLE,
    1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_lif_table_initialize()",0,0);
}

uint32
  soc_pb_pp_sw_db_lif_table_terminate(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res;

  res = SOC_SAND_OK;

  SOC_PETRA_FREE_ANY_SIZE(Soc_pb_pp_sw_db.device[unit]->lif_table);

  return res;
}

uint32
  soc_pb_pp_sw_db_lif_table_entry_use_set(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID         lif_id,
    SOC_SAND_IN  SOC_PB_PP_LIF_ENTRY_TYPE   entry_type
  )
{
  uint32
    type_val;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (Soc_pb_pp_sw_db.device[unit] == NULL)
  {
    return SOC_SAND_ERR;
  }
  if (lif_id > SOC_PB_PP_LIF_ID_MAX || entry_type >= SOC_SAND_BIT(SOC_PB_PP_NOF_LIF_ENTRY_TYPES))
  {
    return SOC_SAND_ERR;
  }

  if(entry_type == SOC_PB_PP_LIF_ENTRY_TYPE_EMPTY)
  {
    type_val = 0;

  }
  else
  {
    type_val = soc_sand_log2_round_down(entry_type);
  }
  
  res = soc_sand_bitstream_set_any_field(
          &type_val,
          lif_id * SOC_PB_PP_SW_DB_TYPE_BITS,
          SOC_PB_PP_SW_DB_TYPE_BITS,
          Soc_pb_pp_sw_db.device[unit]->lif_table->lif_use
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  BCM_DIRTY_MARK;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_lif_table_entry_use_set()",0,0);
}


uint32
  soc_pb_pp_sw_db_lif_table_entry_use_get(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID         lif_id,
    SOC_SAND_OUT  SOC_PB_PP_LIF_ENTRY_TYPE   *entry_type
  )
{
  uint32
    type_val=0;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (Soc_pb_pp_sw_db.device[unit] == NULL)
  {
    return SOC_SAND_ERR;
  }
  if (lif_id > SOC_PB_PP_LIF_ID_MAX)
  {
    return SOC_SAND_ERR;
  }

  res = soc_sand_bitstream_get_any_field(
          Soc_pb_pp_sw_db.device[unit]->lif_table->lif_use,
          lif_id * SOC_PB_PP_SW_DB_TYPE_BITS,
          SOC_PB_PP_SW_DB_TYPE_BITS,
          &type_val
          
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if(type_val == 0)
  {
    *entry_type = SOC_PB_PP_LIF_ENTRY_TYPE_EMPTY;

  }
  else
  {
    *entry_type =  (SOC_PB_PP_LIF_ENTRY_TYPE)(1 << type_val);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_lif_table_entry_use_get()",0,0);
}


uint32
  soc_pb_pp_sw_db_lif_cos_map_from_tc_dp_set(
    SOC_SAND_IN  int                                        unit,
    SOC_SAND_IN  uint8                                        map_from_tc_dp
  )
{
  SOC_PB_PP_SW_DB_FIELD_SET(unit, lif_cos->map_from_tc_dp,&map_from_tc_dp);
  BCM_DIRTY_MARK;
}

uint32
  soc_pb_pp_sw_db_lif_cos_map_from_tc_dp_get(
    SOC_SAND_IN  int                                        unit,
    SOC_SAND_OUT uint8                                        *map_from_tc_dp
  )
{
  SOC_PB_PP_SW_DB_FIELD_GET(unit, lif_cos->map_from_tc_dp,map_from_tc_dp);
}

/* } LIF COS */


/*
 *	ISEM
 */
uint32
  soc_pb_pp_sw_db_isem_initialize(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_SAND_OK;

  SOC_PETRA_ALLOC_ANY_SIZE(
    Soc_pb_pp_sw_db.device[unit]->isem,
    SOC_PB_PP_SW_DB_ISEM,
    1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_isem_initialize()",0,0);
}

uint32
  soc_pb_pp_sw_db_isem_terminate(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res;

  res = SOC_SAND_OK;

  SOC_PETRA_FREE_ANY_SIZE(Soc_pb_pp_sw_db.device[unit]->isem);

  return res;
}

/*
 *	LAG
 */
uint32
  soc_pb_pp_sw_db_lag_initialize(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_SAND_OK;

  SOC_PETRA_ALLOC_ANY_SIZE(
    Soc_pb_pp_sw_db.device[unit]->lag,
    SOC_PB_PP_SW_DB_LAG,
    1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_lag_initialize()",0,0);
}

uint32
  soc_pb_pp_sw_db_lag_terminate(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res;

  res = SOC_SAND_OK;

  SOC_PETRA_FREE_ANY_SIZE(Soc_pb_pp_sw_db.device[unit]->lag);

  return res;
}

void
  soc_pb_pp_sw_db_learning_mode_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN SOC_PB_PP_FRWRD_MACT_LEARNING_MODE  learning_mode
  )
{
  Soc_pb_pp_sw_db.device[unit]->fwd_mact->learning_mode = learning_mode;
  BCM_DIRTY_MARK;
}

SOC_PB_PP_FRWRD_MACT_LEARNING_MODE
  soc_pb_pp_sw_db_learning_mode_get(
    SOC_SAND_IN int unit
  )
{
  return Soc_pb_pp_sw_db.device[unit]->fwd_mact->learning_mode;
}

void
  soc_pb_pp_sw_db_is_petra_a_compatible_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8  is_petra_a_compatible
  )
{
  Soc_pb_pp_sw_db.device[unit]->fwd_mact->is_petra_a_compatible = is_petra_a_compatible;
  BCM_DIRTY_MARK;
}

uint8
  soc_pb_pp_sw_db_is_petra_a_compatible_get(
    SOC_SAND_IN int unit
  )
{
  return Soc_pb_pp_sw_db.device[unit]->fwd_mact->is_petra_a_compatible;
}

/*
 * FEC
 */
uint32
  soc_pb_pp_sw_db_fwd_fec_initialize(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_SAND_OK;

  SOC_PETRA_ALLOC_ANY_SIZE(
    Soc_pb_pp_sw_db.device[unit]->fec,
    SOC_PB_PP_SW_DB_FEC,
    1);

  res = SOC_SAND_OK; sal_memset(Soc_pb_pp_sw_db.device[unit]->fec->fec_entry_type,SOC_PB_PP_NOF_FEC_TYPES, sizeof(uint8) * SOC_DPP_NOF_FECS_PETRAB);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_fwd_fec_initialize()",0,0);
}

uint32
  soc_pb_pp_sw_db_fwd_fec_terminate(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res;

  res = SOC_SAND_OK;

  SOC_PETRA_FREE_ANY_SIZE(Soc_pb_pp_sw_db.device[unit]->fec);

  return res;
}

uint32
  soc_pb_pp_sw_db_fec_glbl_info_set(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN SOC_PB_PP_FRWRD_FEC_GLBL_INFO    *glbl_info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PETRA_COPY(&Soc_pb_pp_sw_db.device[unit]->fec->glbl_info,glbl_info,SOC_PB_PP_FRWRD_FEC_GLBL_INFO,1)
  BCM_DIRTY_MARK;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_fec_sizes_set()",0,0);
}

uint32
  soc_pb_pp_sw_db_fec_glbl_info_get(
    SOC_SAND_IN int                     unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_GLBL_INFO    *glbl_info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PETRA_COPY(glbl_info,&Soc_pb_pp_sw_db.device[unit]->fec->glbl_info,SOC_PB_PP_FRWRD_FEC_GLBL_INFO,1)
 exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_fec_sizes_set()",0,0);
}


uint32
  soc_pb_pp_sw_db_fec_entry_type_set(
    SOC_SAND_IN int         unit,
    SOC_SAND_IN uint32          fec_ndx,
    SOC_SAND_IN SOC_PB_PP_FEC_TYPE    entry_type
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  Soc_pb_pp_sw_db.device[unit]->fec->fec_entry_type[fec_ndx] = (uint8)entry_type;
  BCM_DIRTY_MARK;

  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_fec_entry_type_set()",0,0);
}

uint32
  soc_pb_pp_sw_db_fec_entry_type_get(
    SOC_SAND_IN int         unit,
    SOC_SAND_IN uint32          fec_ndx,
    SOC_SAND_OUT SOC_PB_PP_FEC_TYPE   *entry_type
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  *entry_type = (SOC_PB_PP_FEC_TYPE)Soc_pb_pp_sw_db.device[unit]->fec->fec_entry_type[fec_ndx];

  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_fec_entry_type_get()",0,0);
}
/*
 *	Diag
 */

uint32
  soc_pb_pp_sw_db_fwd_diag_initialize(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_SAND_OK;

  SOC_PETRA_ALLOC_ANY_SIZE(
    Soc_pb_pp_sw_db.device[unit]->diag,
    SOC_PB_PP_SW_DB_DIAG,
    1);

  SOC_PB_PP_DIAG_MODE_INFO_clear(&(Soc_pb_pp_sw_db.device[unit]->diag->mode_info));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_fwd_diag_initialize()",0,0);
}


uint32
  soc_pb_pp_sw_db_fwd_diag_terminate(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res;

  res = SOC_SAND_OK;

  SOC_PETRA_FREE_ANY_SIZE(Soc_pb_pp_sw_db.device[unit]->diag);

  return res;
}

/*
 *	FP
 */
uint32
  soc_pb_pp_sw_db_fp_initialize(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res;
  uint32
    entry_id_ndx,
    pfg_ndx,
    tag_action_ndx,
    db_ndx,
    fem_ndx,
    cycle_ndx,
    key_ndx,
    instr_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_SAND_OK;

  SOC_PETRA_ALLOC_ANY_SIZE(
    Soc_pb_pp_sw_db.device[unit]->fp,
    SOC_PB_PP_SW_DB_FP,
    1
  );

  for (db_ndx = 0; db_ndx < SOC_PB_PP_FP_NOF_DBS; ++db_ndx)
  {
    SOC_PB_PP_FP_DATABASE_INFO_clear(&(Soc_pb_pp_sw_db.device[unit]->fp->db_info[db_ndx]));
    Soc_pb_pp_sw_db.device[unit]->fp->predefined_acl_key[db_ndx] = SOC_PB_PP_NOF_FP_PREDEFINED_ACL_KEYS;
    Soc_pb_pp_sw_db.device[unit]->fp->db_has_key_format[db_ndx] = FALSE;
  }

  for (key_ndx = 0; key_ndx <= SOC_PB_PMF_LOW_LEVEL_PMF_KEY_MAX; ++key_ndx)
  {
    for (instr_ndx = 0; instr_ndx < SOC_PB_PMF_LOW_LEVEL_CE_NDX_MAX; ++instr_ndx)
    {
      Soc_pb_pp_sw_db.device[unit]->fp->ce_instr[key_ndx][instr_ndx] = SOC_PB_PP_NOF_FP_QUAL_TYPES;
    }
  }

  for (cycle_ndx = 0; cycle_ndx < SOC_PB_PMF_NOF_CYCLES; ++cycle_ndx)
  {
    for (fem_ndx = 0; fem_ndx <= SOC_PB_PMF_LOW_LEVEL_FEM_ID_MAX; ++fem_ndx)
    {
      SOC_PB_PP_FP_FEM_ENTRY_clear(&(Soc_pb_pp_sw_db.device[unit]->fp->fem_entry[cycle_ndx][fem_ndx]));
    }
  }

  for (db_ndx = 0; db_ndx < SOC_PB_PP_FP_NOF_DBS; ++db_ndx)
  {
    for (pfg_ndx = 0; pfg_ndx < SOC_PB_PP_FP_NOF_PFGS; ++pfg_ndx)
    {
      SOC_PB_PP_FP_KEY_LOCATION_clear(&(Soc_pb_pp_sw_db.device[unit]->fp->db_key_location[db_ndx][pfg_ndx]));
    }
  }

  for (pfg_ndx = 0; pfg_ndx < SOC_PB_PP_FP_NOF_PFGS; ++pfg_ndx)
  {
    for (tag_action_ndx = 0; tag_action_ndx < SOC_PB_PP_FP_NOF_TAG_ACTION_TYPES; ++tag_action_ndx)
    {
      Soc_pb_pp_sw_db.device[unit]->fp->is_tag_used[pfg_ndx][tag_action_ndx]= SOC_PB_PP_FP_NOF_DBS;
    }
    Soc_pb_pp_sw_db.device[unit]->fp->loc_key_changed[pfg_ndx]= SOC_PB_PP_NOF_FP_KEY_CHANGE_LOCATIONS;
  }

  Soc_pb_pp_sw_db.device[unit]->fp->key_change_size = SOC_PB_PP_FP_KEY_CHANGE_SIZE_DEFAULT;


  for (entry_id_ndx = 0; entry_id_ndx < SOC_PB_PP_FP_NOF_ENTRY_IDS; ++entry_id_ndx)
  {
    SOC_PB_PP_FP_ENTRY_INFO_clear(&(Soc_pb_pp_sw_db.device[unit]->fp->dt_entry[entry_id_ndx]));
  }



exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_fp_initialize()",0,0);
}

uint32
  soc_pb_pp_sw_db_fp_terminate(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res;

  res = SOC_SAND_OK;

  SOC_PETRA_FREE_ANY_SIZE(Soc_pb_pp_sw_db.device[unit]->fp);

  return res;
}

uint32
  soc_pb_pp_sw_db_diag_trap_save_status_set(
    SOC_SAND_IN int              unit,
    SOC_SAND_IN uint8              saved
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  Soc_pb_pp_sw_db.device[unit]->diag->already_saved = saved;
  BCM_DIRTY_MARK;
  
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_diag_trap_save_status_set()",0,0);
}

uint32
  soc_pb_pp_sw_db_diag_trap_save_status_get(
    SOC_SAND_IN int              unit,
    SOC_SAND_OUT uint8              *saved
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  *saved = Soc_pb_pp_sw_db.device[unit]->diag->already_saved;
  
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_diag_trap_save_status_get()",0,0);
}

uint32
  soc_pb_pp_sw_db_diag_trap_dest_set(
    SOC_SAND_IN int                              unit,
    SOC_SAND_IN uint32                               trap_code_ndx,
    SOC_SAND_IN SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_DATA  	 *trap_info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_ERR_IF_ABOVE_MAX(trap_code_ndx, (SOC_PB_PP_NOF_TRAP_CODES-1), SOC_PB_PP_TRAP_MGMT_TRAP_CODE_NDX_OUT_OF_RANGE_ERR, 10, exit);

  Soc_pb_pp_sw_db.device[unit]->diag->trap_dest[4*trap_code_ndx] = trap_info->fwd_act_destination;
  Soc_pb_pp_sw_db.device[unit]->diag->trap_dest[4*trap_code_ndx+1] = trap_info->fwd_act_destination_add_vsi;
  Soc_pb_pp_sw_db.device[unit]->diag->trap_dest[4*trap_code_ndx+2] = trap_info->fwd_act_destination_add_vsi_shift;
  Soc_pb_pp_sw_db.device[unit]->diag->trap_dest[4*trap_code_ndx+3] = trap_info->fwd_act_destination_valid;
  BCM_DIRTY_MARK;
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_diag_trap_dest_set()",0,0);
}

uint32
  soc_pb_pp_sw_db_diag_trap_dest_get(
    SOC_SAND_IN int                              unit,
    SOC_SAND_IN uint32                               trap_code_ndx,
    SOC_SAND_OUT SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_DATA  	 *trap_info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_ERR_IF_ABOVE_MAX(trap_code_ndx, (SOC_PB_PP_NOF_TRAP_CODES-1), SOC_PB_PP_TRAP_MGMT_TRAP_CODE_NDX_OUT_OF_RANGE_ERR, 10, exit);

  trap_info->fwd_act_destination = Soc_pb_pp_sw_db.device[unit]->diag->trap_dest[4*trap_code_ndx];
  trap_info->fwd_act_destination_add_vsi = Soc_pb_pp_sw_db.device[unit]->diag->trap_dest[4*trap_code_ndx+1];
  trap_info->fwd_act_destination_add_vsi_shift = Soc_pb_pp_sw_db.device[unit]->diag->trap_dest[4*trap_code_ndx+2];
  trap_info->fwd_act_destination_valid = Soc_pb_pp_sw_db.device[unit]->diag->trap_dest[4*trap_code_ndx+2];
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_diag_trap_dest_get()",0,0);
}

uint32
  soc_pb_pp_sw_db_diag_mode_set(
    SOC_SAND_IN int                unit,
    SOC_SAND_IN SOC_PB_PP_DIAG_MODE_INFO  	 *mode_info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PETRA_COPY(&(Soc_pb_pp_sw_db.device[unit]->diag->mode_info),mode_info,SOC_PB_PP_DIAG_MODE_INFO,1);
  BCM_DIRTY_MARK;
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_diag_trap_dest_get()",0,0);
}

uint32
  soc_pb_pp_sw_db_diag_mode_get(
    SOC_SAND_IN int                unit,
    SOC_SAND_OUT SOC_PB_PP_DIAG_MODE_INFO  	 *mode_info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PETRA_COPY(mode_info,&(Soc_pb_pp_sw_db.device[unit]->diag->mode_info),SOC_PB_PP_DIAG_MODE_INFO,1);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_diag_trap_dest_get()",0,0);
}

/*
 *	Field Processor
 */
uint32
  soc_pb_pp_sw_db_fp_db_info_set(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint32              db_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_DATABASE_INFO *db_info
   )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_PB_PP_COPY(&(Soc_pb_pp_sw_db.device[unit]->fp->db_info[db_ndx]), db_info, SOC_PB_PP_FP_DATABASE_INFO, 1);
  BCM_DIRTY_MARK;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_fp_db_info_set()", 0, 0);
}

uint32
  soc_pb_pp_sw_db_fp_db_info_get(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               db_ndx,
    SOC_SAND_OUT SOC_PB_PP_FP_DATABASE_INFO *db_info
 )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PB_PP_COPY(db_info, &(Soc_pb_pp_sw_db.device[unit]->fp->db_info[db_ndx]), SOC_PB_PP_FP_DATABASE_INFO, 1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_fp_db_info_get()",0,0);
}

uint32
  soc_pb_pp_sw_db_fp_dt_entry_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             entry_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_ENTRY_INFO   *entry
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_PB_PP_COPY(&(Soc_pb_pp_sw_db.device[unit]->fp->dt_entry[entry_id_ndx]), entry, SOC_PB_PP_FP_ENTRY_INFO, 1);
  BCM_DIRTY_MARK;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_fp_dt_entry_set()", 0, 0);
}

uint32
  soc_pb_pp_sw_db_fp_dt_entry_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             entry_id_ndx,
    SOC_SAND_OUT SOC_PB_PP_FP_ENTRY_INFO  *entry
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PB_PP_COPY(entry, &(Soc_pb_pp_sw_db.device[unit]->fp->dt_entry[entry_id_ndx]), SOC_PB_PP_FP_ENTRY_INFO, 1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_fp_dt_entry_get()",0,0);
}


uint32
  soc_pb_pp_sw_db_fp_db_entries_set(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint32              db_ndx,
    SOC_SAND_IN  SOC_PB_PP_SW_DB_FP_ENTRY  *db_entries
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_PB_PP_COPY(&(Soc_pb_pp_sw_db.device[unit]->fp->db_entries[db_ndx]), db_entries, SOC_PB_PP_SW_DB_FP_ENTRY, 1);
  BCM_DIRTY_MARK;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_fp_db_entries_set()", 0, 0);
}

uint32
  soc_pb_pp_sw_db_fp_db_entries_get(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               db_ndx,
    SOC_SAND_OUT SOC_PB_PP_SW_DB_FP_ENTRY   *db_entries
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PB_PP_COPY(db_entries, &(Soc_pb_pp_sw_db.device[unit]->fp->db_entries[db_ndx]), SOC_PB_PP_SW_DB_FP_ENTRY, 1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_fp_db_entries_get()",0,0);
}

void
  soc_pb_pp_sw_db_fp_db_predefined_acl_key_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   db_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_PREDEFINED_ACL_KEY predefined_acl_key
  )
{
  Soc_pb_pp_sw_db.device[unit]->fp->predefined_acl_key[db_ndx] = predefined_acl_key;
  BCM_DIRTY_MARK;
}

void
  soc_pb_pp_sw_db_fp_db_predefined_acl_key_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      db_ndx,
    SOC_SAND_OUT SOC_PB_PP_FP_PREDEFINED_ACL_KEY   *predefined_acl_key
 )
{
  *predefined_acl_key = Soc_pb_pp_sw_db.device[unit]->fp->predefined_acl_key[db_ndx];
}

uint32
  soc_pb_pp_sw_db_fp_db_udf_set(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     udf_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_KEY_QUAL_INFO        *ce_instr
   )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  Soc_pb_pp_sw_db.device[unit]->fp->udf[udf_ndx] = *ce_instr;
  BCM_DIRTY_MARK;
  
  SOC_PB_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_fp_db_udf_set()", 0, 0);
}


uint32
  soc_pb_pp_sw_db_fp_db_udf_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      udf_ndx,
    SOC_SAND_OUT SOC_PB_PP_FP_KEY_QUAL_INFO         *ce_instr
    )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  *ce_instr = Soc_pb_pp_sw_db.device[unit]->fp->udf[udf_ndx];

  SOC_PB_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_fp_db_udf_get()",0,0);
}

void
  soc_pb_pp_sw_db_fp_ce_instr_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             key_ndx,
    SOC_SAND_IN  uint32             instr_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_QUAL_TYPE    ce_instr
  )
{
  Soc_pb_pp_sw_db.device[unit]->fp->ce_instr[key_ndx][instr_ndx] = ce_instr;
  BCM_DIRTY_MARK;
}

SOC_PB_PP_FP_QUAL_TYPE
  soc_pb_pp_sw_db_fp_ce_instr_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             key_ndx,
    SOC_SAND_IN  uint32             instr_ndx
    )
{
  return Soc_pb_pp_sw_db.device[unit]->fp->ce_instr[key_ndx][instr_ndx];
}

uint32
  soc_pb_pp_sw_db_fp_fem_entry_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             cycle_ndx,
    SOC_SAND_IN  uint32             fem_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_FEM_ENTRY  *fem_entry
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_PB_PP_COPY(&(Soc_pb_pp_sw_db.device[unit]->fp->fem_entry[cycle_ndx][fem_ndx]), fem_entry, SOC_PB_PP_FP_FEM_ENTRY, 1);
  BCM_DIRTY_MARK;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_fp_fem_entry_set()", 0, 0);
}

uint32
  soc_pb_pp_sw_db_fp_fem_entry_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             cycle_ndx,
    SOC_SAND_IN  uint32             fem_ndx,
    SOC_SAND_OUT SOC_PB_PP_FP_FEM_ENTRY   *fem_entry
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PB_PP_COPY(fem_entry, &(Soc_pb_pp_sw_db.device[unit]->fp->fem_entry[cycle_ndx][fem_ndx]), SOC_PB_PP_FP_FEM_ENTRY, 1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_fp_fem_entry_get()",0,0);
}

void
  soc_pb_pp_sw_db_fp_db_id_cycle_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 db_id_ndx,
    SOC_SAND_IN uint8 db_id_cycle
    )
{
  Soc_pb_pp_sw_db.device[unit]->fp->db_id_cycle[db_id_ndx] = db_id_cycle;
  BCM_DIRTY_MARK;
}

uint8
  soc_pb_pp_sw_db_fp_db_id_cycle_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 db_id_ndx
  )
{
  return Soc_pb_pp_sw_db.device[unit]->fp->db_id_cycle[db_id_ndx];
}

uint32
  soc_pb_pp_sw_db_fp_db_key_location_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             db_id_ndx,
    SOC_SAND_IN  uint32             pfg_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_KEY_LOCATION *key_loc
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_PB_PP_COPY(&(Soc_pb_pp_sw_db.device[unit]->fp->db_key_location[db_id_ndx][pfg_ndx]), key_loc, SOC_PB_PP_FP_KEY_LOCATION, 1);
  BCM_DIRTY_MARK;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_fp_db_key_location_set()", 0, 0);
}

uint32
  soc_pb_pp_sw_db_fp_db_key_location_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             db_id_ndx,
    SOC_SAND_IN  uint32             pfg_ndx,
    SOC_SAND_OUT SOC_PB_PP_FP_KEY_LOCATION *key_loc
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PB_PP_COPY(key_loc, &(Soc_pb_pp_sw_db.device[unit]->fp->db_key_location[db_id_ndx][pfg_ndx]), SOC_PB_PP_FP_KEY_LOCATION, 1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_fp_db_key_location_get()",0,0);
}
void
  soc_pb_pp_sw_db_fp_is_tag_used_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 pfg_ndx,
    SOC_SAND_IN uint32 tag_action_ndx,
    SOC_SAND_IN uint32 is_tag_used_db_id
  )
{
  Soc_pb_pp_sw_db.device[unit]->fp->is_tag_used[pfg_ndx][tag_action_ndx] = is_tag_used_db_id;
  BCM_DIRTY_MARK;
}

uint32
  soc_pb_pp_sw_db_fp_is_tag_used_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 pfg_ndx,
    SOC_SAND_IN uint32 tag_action_ndx
  )
{
  return Soc_pb_pp_sw_db.device[unit]->fp->is_tag_used[pfg_ndx][tag_action_ndx];
}

void
  soc_pb_pp_sw_db_fp_inner_eth_nof_tags_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 pfg_ndx,
    SOC_SAND_IN uint32  inner_eth_nof_tags
    )
{
  Soc_pb_pp_sw_db.device[unit]->fp->inner_eth_nof_tags[pfg_ndx] = inner_eth_nof_tags;
  BCM_DIRTY_MARK;
}

uint32
  soc_pb_pp_sw_db_fp_inner_eth_nof_tags_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 pfg_ndx
  )
{
  return Soc_pb_pp_sw_db.device[unit]->fp->inner_eth_nof_tags[pfg_ndx];
}

void
  soc_pb_pp_sw_db_fp_loc_key_changed_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 pfg_ndx,
    SOC_SAND_IN SOC_PB_PP_FP_KEY_CHANGE_LOCATION  loc_key_changed
  )
{
  Soc_pb_pp_sw_db.device[unit]->fp->loc_key_changed[pfg_ndx] = loc_key_changed;
  BCM_DIRTY_MARK;
}

SOC_PB_PP_FP_KEY_CHANGE_LOCATION
  soc_pb_pp_sw_db_fp_loc_key_changed_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 pfg_ndx
  )
{
  return Soc_pb_pp_sw_db.device[unit]->fp->loc_key_changed[pfg_ndx];
}

void
  soc_pb_pp_sw_db_fp_key_change_size_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8   key_change_size
  )
{
  Soc_pb_pp_sw_db.device[unit]->fp->key_change_size = key_change_size;
  BCM_DIRTY_MARK;
}

uint8
  soc_pb_pp_sw_db_fp_key_change_size_get(
    SOC_SAND_IN int unit
  )
{
  return Soc_pb_pp_sw_db.device[unit]->fp->key_change_size;
}

void
  soc_pb_pp_sw_db_fp_db_has_key_format_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 db_id_ndx,
    SOC_SAND_IN uint8 has_key_format
  )
{
  Soc_pb_pp_sw_db.device[unit]->fp->db_has_key_format[db_id_ndx] = has_key_format;
  BCM_DIRTY_MARK;
}

uint8
  soc_pb_pp_sw_db_fp_db_has_key_format_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 db_id_ndx
  )
{
  return Soc_pb_pp_sw_db.device[unit]->fp->db_has_key_format[db_id_ndx];
}

void
  soc_pb_pp_sw_db_fp_key_desc_set(
    SOC_SAND_IN int         unit,
    SOC_SAND_IN uint32         db_id_ndx,
    SOC_SAND_IN SOC_PB_PP_FP_KEY_DESC *key_desc
  )
{
  Soc_pb_pp_sw_db.device[unit]->fp->key_descs[db_id_ndx] = *key_desc;
  BCM_DIRTY_MARK;
}

void
  soc_pb_pp_sw_db_fp_key_desc_get(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  uint32         db_id_ndx,
    SOC_SAND_OUT SOC_PB_PP_FP_KEY_DESC *key_desc
  )
{
  *key_desc = Soc_pb_pp_sw_db.device[unit]->fp->key_descs[db_id_ndx];
}

void
  soc_pb_pp_sw_db_fp_tcam_db_key_size_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 db_id_ndx,
    SOC_SAND_IN uint32 key_size
  )
{
  Soc_pb_pp_sw_db.device[unit]->fp->tcam_db_key_size[db_id_ndx] = key_size;
  BCM_DIRTY_MARK;
}

uint32
  soc_pb_pp_sw_db_fp_tcam_db_key_size_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 db_id_ndx
  )
{
  return Soc_pb_pp_sw_db.device[unit]->fp->tcam_db_key_size[db_id_ndx];
}

/*
 *	Operation mode
 */

void
  soc_pb_pp_sw_db_authentication_enable_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 authentication_enable
  )
{
  Soc_pb_pp_sw_db.device[unit]->oper_mode->authentication_enable = authentication_enable;
  BCM_DIRTY_MARK;
}

uint8
  soc_pb_pp_sw_db_authentication_enable_get(
    SOC_SAND_IN int unit
  )
{
  return Soc_pb_pp_sw_db.device[unit]->oper_mode->authentication_enable;
}

void
  soc_pb_pp_sw_db_pvlan_enable_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 pvlan_enable
  )
{
  Soc_pb_pp_sw_db.device[unit]->oper_mode->ipv4_info.pvlan_enable = pvlan_enable;
  BCM_DIRTY_MARK;
}

uint8
  soc_pb_pp_sw_db_pvlan_enable_get(
    SOC_SAND_IN int unit
  )
{
  return Soc_pb_pp_sw_db.device[unit]->oper_mode->ipv4_info.pvlan_enable;
}

void
  soc_pb_pp_sw_db_split_horizon_enable_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 split_horizon_enable
  )
{
  Soc_pb_pp_sw_db.device[unit]->oper_mode->split_horizon_filter_enable = split_horizon_enable;
  BCM_DIRTY_MARK;
}

uint8
  soc_pb_pp_sw_db_split_horizon_enable_get(
    SOC_SAND_IN int unit
  )
{
  return Soc_pb_pp_sw_db.device[unit]->oper_mode->split_horizon_filter_enable;
}
void
  soc_pb_pp_sw_db_system_vsi_enable_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 system_vsi_enable
  )
{
  Soc_pb_pp_sw_db.device[unit]->oper_mode->system_vsi_enable = system_vsi_enable;
  BCM_DIRTY_MARK;
}

uint8
  soc_pb_pp_sw_db_system_vsi_enable_get(
    SOC_SAND_IN int unit
  )
{
  return Soc_pb_pp_sw_db.device[unit]->oper_mode->system_vsi_enable;
}

void
  soc_pb_pp_sw_db_hairpin_enable_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 hairpin_enable
  )
{
  Soc_pb_pp_sw_db.device[unit]->oper_mode->hairpin_enable = hairpin_enable;
  BCM_DIRTY_MARK;
}

uint8
  soc_pb_pp_sw_db_hairpin_enable_get(
    SOC_SAND_IN int unit
  )
{
  return Soc_pb_pp_sw_db.device[unit]->oper_mode->hairpin_enable;
}

uint8
  soc_pb_pp_sw_db_ipv4_host_extend_enable_get(
    SOC_SAND_IN int unit
  )
{
  return Soc_pb_pp_sw_db.device[unit]->oper_mode->ipv4_info.ipv4_host_extend;
}


uint32
  soc_pb_pp_sw_db_mpls_ether_types_set(
    SOC_SAND_IN int             unit,
    SOC_SAND_IN SOC_SAND_PP_ETHER_TYPE    mpls_ether_types[2]
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_PB_PP_COPY(Soc_pb_pp_sw_db.device[unit]->oper_mode->mpls_ether_types,mpls_ether_types, SOC_SAND_PP_ETHER_TYPE, 2);
  BCM_DIRTY_MARK;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_mpls_ether_types_set()",0,0);
}

uint32
  soc_pb_pp_sw_db_mpls_ether_types_get(
    SOC_SAND_IN int                  unit,
    SOC_SAND_OUT SOC_SAND_PP_ETHER_TYPE        mpls_ether_types[2]
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PB_PP_COPY(mpls_ether_types,Soc_pb_pp_sw_db.device[unit]->oper_mode->mpls_ether_types, SOC_SAND_PP_ETHER_TYPE, 2);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_mpls_ether_types_get()",0,0);
}

/*
 *  Used for In-LIF program differentiation:
 *  TRUE if use of External-Key (*VLAN) (*VLAN) but not IP, Trill, MPLS..
 */
uint8
  soc_pb_pp_sw_db_isem_ext_key_enabled_get(
    SOC_SAND_IN int unit
  )
{
  return Soc_pb_pp_sw_db.device[unit]->isem->ext_key_enabled;
}
void
  soc_pb_pp_sw_db_isem_ext_key_enabled_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 ext_key_enabled
  )
{
  Soc_pb_pp_sw_db.device[unit]->isem->ext_key_enabled = ext_key_enabled;
  BCM_DIRTY_MARK;
}

uint8
  soc_pb_pp_sw_db_lag_lb_key_is_symtrc_get(
    SOC_SAND_IN int unit
  )
{
  return Soc_pb_pp_sw_db.device[unit]->lag->lb_key_is_symtrc;
}
void
  soc_pb_pp_sw_db_lag_lb_key_is_symtrc_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 lb_key_is_symtrc
  )
{
  Soc_pb_pp_sw_db.device[unit]->lag->lb_key_is_symtrc = lb_key_is_symtrc;
  BCM_DIRTY_MARK;
}


SOC_PB_PP_HASH_MASKS
  soc_pb_pp_sw_db_lag_lb_key_mask_get(
    SOC_SAND_IN int unit
  )
{
  return Soc_pb_pp_sw_db.device[unit]->lag->masks;
}
void
  soc_pb_pp_sw_db_lag_lb_key_mask_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN SOC_PB_PP_HASH_MASKS lb_key_masks
  )
{
  Soc_pb_pp_sw_db.device[unit]->lag->masks = lb_key_masks;
  BCM_DIRTY_MARK;
}

uint32
  soc_pb_pp_sw_db_llp_mirror_port_vlan_is_exist_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_PB_PP_PORT            pp_port_ndx,
    SOC_SAND_IN  uint32              internal_vid_ndx,
    SOC_SAND_IN  uint8             is_exist
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_ERR_IF_ABOVE_MAX(pp_port_ndx, (SOC_PB_PP_NOF_PORTS-1), SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);

  Soc_pb_pp_sw_db.device[unit]->llp_mirror->llp_mirror_port_vlan[pp_port_ndx][internal_vid_ndx] = is_exist;
  BCM_DIRTY_MARK;
   
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_llp_mirror_port_vlan_is_exist_set()",0,0);
}

uint32
  soc_pb_pp_sw_db_llp_mirror_port_vlan_is_exist_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_PB_PP_PORT            pp_port_ndx,
    SOC_SAND_IN  uint32              internal_vid_ndx,
    SOC_SAND_OUT uint8             *is_exist
    )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_ERR_IF_ABOVE_MAX(pp_port_ndx, (SOC_PB_PP_NOF_PORTS-1), SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);

  *is_exist = Soc_pb_pp_sw_db.device[unit]->llp_mirror->llp_mirror_port_vlan[pp_port_ndx][internal_vid_ndx];
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_llp_mirror_port_vlan_is_exist_get()",0,0);
}

 
/* helper function for soc_pb_pp_ssr_PB_PP_SW_DB_SSR_V01_data_save*/
STATIC uint32
  soc_pb_pp_ssr_copy_data_from_dev_fields_to_buffer(
      SOC_SAND_IN  int      unit,
      SOC_SAND_OUT uint8     **buffer_ptr,
      SOC_SAND_IN  uint8      *additional_data_orig,
      SOC_SAND_IN  uint32      additional_data_size
    )
{
  SOC_PB_PP_SW_DB_DEVICE
    *dev_ptr;
  SOC_PB_PP_SW_DB_DEVICE
    tmp_dev;
  uint8 *buffer;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  dev_ptr = Soc_pb_pp_sw_db.device[unit];

  if (dev_ptr == NULL)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_NULL_POINTER_ERR, 10, exit);
  }

  buffer = *buffer_ptr;

  /*SOC_PB_PP_MGMT_OPERATION_MODE *oper_mode*/
  tmp_dev.oper_mode = (SOC_PB_PP_MGMT_OPERATION_MODE *)buffer;
  if((buffer - additional_data_orig + sizeof(SOC_PB_PP_MGMT_OPERATION_MODE)) > additional_data_size) {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_PP_SCACHE_SIZE_TOO_SMALL_ERR, 10, exit);        
  }
  buffer += sizeof(SOC_PB_PP_MGMT_OPERATION_MODE);
  *(tmp_dev.oper_mode) = *(dev_ptr->oper_mode);

  /*SOC_PB_PP_SW_DB_LLP_FILTER *llp_filter*/
  tmp_dev.llp_filter = (SOC_PB_PP_SW_DB_LLP_FILTER *)buffer;
  if((buffer - additional_data_orig + sizeof(SOC_PB_PP_SW_DB_LLP_FILTER)) > additional_data_size) {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_PP_SCACHE_SIZE_TOO_SMALL_ERR, 20, exit);        
  }
  buffer += sizeof(SOC_PB_PP_SW_DB_LLP_FILTER);
  *(tmp_dev.llp_filter) = *(dev_ptr->llp_filter);

  /*SOC_PB_PP_SW_DB_LLP_TRAP *llp_trap*/
  tmp_dev.llp_trap = (SOC_PB_PP_SW_DB_LLP_TRAP *)buffer;
  if((buffer - additional_data_orig + sizeof(SOC_PB_PP_SW_DB_LLP_TRAP)) > additional_data_size) {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_PP_SCACHE_SIZE_TOO_SMALL_ERR, 30, exit);        
  }
  buffer += sizeof(SOC_PB_PP_SW_DB_LLP_TRAP);
  *(tmp_dev.llp_trap) = *(dev_ptr->llp_trap);

  /*SOC_PB_PP_SW_DB_LLP_MIRROR *llp_mirror*/
  tmp_dev.llp_mirror = (SOC_PB_PP_SW_DB_LLP_MIRROR *)buffer;
  if((buffer - additional_data_orig + sizeof(SOC_PB_PP_SW_DB_LLP_MIRROR)) > additional_data_size) {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_PP_SCACHE_SIZE_TOO_SMALL_ERR, 40, exit);        
  }
  buffer += sizeof(SOC_PB_PP_SW_DB_LLP_MIRROR);
  *(tmp_dev.llp_mirror) = *(dev_ptr->llp_mirror);

  /*SOC_PB_PP_SW_DB_LLP_VID_ASSIGN *llp_vid_assign*/
  tmp_dev.llp_vid_assign = (SOC_PB_PP_SW_DB_LLP_VID_ASSIGN *)buffer;
  if((buffer - additional_data_orig + sizeof(SOC_PB_PP_SW_DB_LLP_VID_ASSIGN)) > additional_data_size) {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_PP_SCACHE_SIZE_TOO_SMALL_ERR, 50, exit);        
  }
  buffer += sizeof(SOC_PB_PP_SW_DB_LLP_VID_ASSIGN);
  *(tmp_dev.llp_vid_assign) = *(dev_ptr->llp_vid_assign);

  /*SOC_PB_PP_SW_DB_EG_MIRROR *eg_mirror*/
  tmp_dev.eg_mirror = (SOC_PB_PP_SW_DB_EG_MIRROR *)buffer;
  if((buffer - additional_data_orig + sizeof(SOC_PB_PP_SW_DB_EG_MIRROR)) > additional_data_size) {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_PP_SCACHE_SIZE_TOO_SMALL_ERR, 60, exit);        
  }
  buffer += sizeof(SOC_PB_PP_SW_DB_EG_MIRROR);
  *(tmp_dev.eg_mirror) = *(dev_ptr->eg_mirror);

  /*SOC_PB_PP_SW_DB_LLP_COS *llp_cos*/
  tmp_dev.llp_cos = (SOC_PB_PP_SW_DB_LLP_COS *)buffer;
  if((buffer - additional_data_orig + sizeof(SOC_PB_PP_SW_DB_LLP_COS)) > additional_data_size) {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_PP_SCACHE_SIZE_TOO_SMALL_ERR, 70, exit);        
  }
  buffer += sizeof(SOC_PB_PP_SW_DB_LLP_COS);
  *(tmp_dev.llp_cos) = *(dev_ptr->llp_cos);

  /*SOC_PB_PPSW_DB_ETH_POLICER_MTR_PROFILE *eth_policer_mtr_profile*/
  tmp_dev.eth_policer_mtr_profile = (SOC_PB_PPSW_DB_ETH_POLICER_MTR_PROFILE *)buffer;
  if((buffer - additional_data_orig + sizeof(SOC_PB_PPSW_DB_ETH_POLICER_MTR_PROFILE)) > additional_data_size) {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_PP_SCACHE_SIZE_TOO_SMALL_ERR, 80, exit);        
  }
  buffer += sizeof(SOC_PB_PPSW_DB_ETH_POLICER_MTR_PROFILE);
  *(tmp_dev.eth_policer_mtr_profile) = *(dev_ptr->eth_policer_mtr_profile);

  if(dev_ptr->ipv4_info != NULL)
  {
    /*SOC_PB_PP_SW_DB_IPV4_INFO *ipv4_info*/
    tmp_dev.ipv4_info = (SOC_PB_PP_SW_DB_IPV4_INFO *)buffer;
    if((buffer - additional_data_orig + sizeof(SOC_PB_PP_SW_DB_IPV4_INFO)) > additional_data_size) {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_PP_SCACHE_SIZE_TOO_SMALL_ERR, 90, exit);        
    }
    buffer += sizeof(SOC_PB_PP_SW_DB_IPV4_INFO);
    *(tmp_dev.ipv4_info) = *(dev_ptr->ipv4_info);

    /* patch zeros in places that contain pointers. these pointers wont be needed in the restoration */
    sal_memset(&tmp_dev.ipv4_info->vrf_modified_bitmask, 0x0, sizeof(uint32 *));
    sal_memset(&tmp_dev.ipv4_info->free_list_size, 0x0, sizeof(uint32 *));
    sal_memset(&tmp_dev.ipv4_info->free_list, 0x0, sizeof(uint16 **));
    sal_memset(&tmp_dev.ipv4_info->lpm_mngr.init_info.nof_bits_per_bank, 0x0, sizeof(uint32 *));
    sal_memset(&tmp_dev.ipv4_info->lpm_mngr.init_info.bank_to_mem, 0x0, sizeof(uint32 *));
    sal_memset(&tmp_dev.ipv4_info->lpm_mngr.init_info.nof_rows_per_mem, 0x0, sizeof(uint32 *));
    sal_memset(&tmp_dev.ipv4_info->lpm_mngr.init_info.mem_allocators, 0x0, sizeof(SOC_SAND_ARR_MEM_ALLOCATOR_INFO *));
    sal_memset(&tmp_dev.ipv4_info->lpm_mngr.init_info.lpms, 0x0, sizeof(SOC_SAND_PAT_TREE_INFO *));
    sal_memset(&tmp_dev.ipv4_info->lpm_mngr.init_info.rev_ptrs, 0x0, sizeof(SOC_SAND_GROUP_MEM_LL_INFO **));
    sal_memset(&tmp_dev.ipv4_info->lpm_mngr.data_info.bit_depth_per_bank, 0x0, sizeof(uint32 *));

  }
  /*SOC_PB_PP_SW_DB_ILM_INFO *ilm_info*/
  tmp_dev.ilm_info = (SOC_PB_PP_SW_DB_ILM_INFO *)buffer;
  if((buffer - additional_data_orig + sizeof(SOC_PB_PP_SW_DB_ILM_INFO)) > additional_data_size) {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_PP_SCACHE_SIZE_TOO_SMALL_ERR, 100, exit);        
  }
  buffer += sizeof(SOC_PB_PP_SW_DB_ILM_INFO);
  *(tmp_dev.ilm_info) = *(dev_ptr->ilm_info);

  /*SOC_PB_PP_SW_DB_FWD_MACT *fwd_mact*/
  tmp_dev.fwd_mact = (SOC_PB_PP_SW_DB_FWD_MACT *)buffer;
  if((buffer - additional_data_orig + sizeof(SOC_PB_PP_SW_DB_FWD_MACT)) > additional_data_size) {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_PP_SCACHE_SIZE_TOO_SMALL_ERR, 110, exit);        
  }
  buffer += sizeof(SOC_PB_PP_SW_DB_FWD_MACT);
  *(tmp_dev.fwd_mact) = *(dev_ptr->fwd_mact);

  /*SOC_PB_PP_SW_DB_LIF_COS *lif_cos*/
  tmp_dev.lif_cos = (SOC_PB_PP_SW_DB_LIF_COS *)buffer;
  if((buffer - additional_data_orig + sizeof(SOC_PB_PP_SW_DB_LIF_COS)) > additional_data_size) {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_PP_SCACHE_SIZE_TOO_SMALL_ERR, 120, exit);        
  }
  buffer += sizeof(SOC_PB_PP_SW_DB_LIF_COS);
  *(tmp_dev.lif_cos) = *(dev_ptr->lif_cos);

  /*SOC_PB_PP_SW_DB_LIF_TABLE *lif_table*/
  tmp_dev.lif_table = (SOC_PB_PP_SW_DB_LIF_TABLE *)buffer;
  if((buffer - additional_data_orig + sizeof(SOC_PB_PP_SW_DB_LIF_TABLE)) > additional_data_size) {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_PP_SCACHE_SIZE_TOO_SMALL_ERR, 130, exit);        
  }
  buffer += sizeof(SOC_PB_PP_SW_DB_LIF_TABLE);
  *(tmp_dev.lif_table) = *(dev_ptr->lif_table);

  /*SOC_PB_PP_SW_DB_FEC *fec*/
  tmp_dev.fec = (SOC_PB_PP_SW_DB_FEC *)buffer;
  if((buffer - additional_data_orig + sizeof(SOC_PB_PP_SW_DB_FEC)) > additional_data_size) {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_PP_SCACHE_SIZE_TOO_SMALL_ERR, 140, exit);        
  }
  buffer += sizeof(SOC_PB_PP_SW_DB_FEC);
  *(tmp_dev.fec) = *(dev_ptr->fec);

  /*SOC_PB_PP_SW_DB_DIAG *diag*/
  tmp_dev.diag = (SOC_PB_PP_SW_DB_DIAG *)buffer;
  if((buffer - additional_data_orig + sizeof(SOC_PB_PP_SW_DB_DIAG)) > additional_data_size) {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_PP_SCACHE_SIZE_TOO_SMALL_ERR, 150, exit);        
  }
  buffer += sizeof(SOC_PB_PP_SW_DB_DIAG);
  *(tmp_dev.diag) = *(dev_ptr->diag);

  /*SOC_PB_PP_SW_DB_FP *fp*/
  tmp_dev.fp = (SOC_PB_PP_SW_DB_FP *)buffer;
  if((buffer - additional_data_orig + sizeof(SOC_PB_PP_SW_DB_FP)) > additional_data_size) {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_PP_SCACHE_SIZE_TOO_SMALL_ERR, 160, exit);        
  }
  buffer += sizeof(SOC_PB_PP_SW_DB_FP);
  *(tmp_dev.fp) = *(dev_ptr->fp);

  /*SOC_PB_PP_SW_DB_ISEM *isem*/
  tmp_dev.isem = (SOC_PB_PP_SW_DB_ISEM *)buffer;
  if((buffer - additional_data_orig + sizeof(SOC_PB_PP_SW_DB_ISEM)) > additional_data_size) {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_PP_SCACHE_SIZE_TOO_SMALL_ERR, 170, exit);        
  }
  buffer += sizeof(SOC_PB_PP_SW_DB_ISEM);
  *(tmp_dev.isem) = *(dev_ptr->isem);

  /*SOC_PB_PP_SW_DB_LAG *lag*/
  tmp_dev.lag = (SOC_PB_PP_SW_DB_LAG *)buffer;
  if((buffer - additional_data_orig + sizeof(SOC_PB_PP_SW_DB_LAG)) > additional_data_size) {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_PP_SCACHE_SIZE_TOO_SMALL_ERR, 180, exit);        
  }
  buffer += sizeof(SOC_PB_PP_SW_DB_LAG);
  *(tmp_dev.lag) = *(dev_ptr->lag);

  /*SOC_PB_PP_SW_DB_L2_LIF *l2_lif*/
  tmp_dev.l2_lif = (SOC_PB_PP_SW_DB_L2_LIF *)buffer;
  if((buffer - additional_data_orig + sizeof(SOC_PB_PP_SW_DB_L2_LIF)) > additional_data_size) {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_PP_SCACHE_SIZE_TOO_SMALL_ERR, 190, exit);        
  }
  buffer += sizeof(SOC_PB_PP_SW_DB_L2_LIF);
  *(tmp_dev.l2_lif) = *(dev_ptr->l2_lif);


  /* patch zeros in places that contain data structures. ds will be restored using the save/load routine */
  sal_memset(&tmp_dev.llp_trap->l3_protocols_multi_set, 0x0, sizeof(SOC_SAND_MULTI_SET_INFO));
  sal_memset(&tmp_dev.llp_mirror->mirror_profile_multi_set, 0x0, sizeof(SOC_SAND_MULTI_SET_INFO));
  sal_memset(&tmp_dev.eg_mirror->mirror_profile_multi_set, 0x0, sizeof(SOC_SAND_MULTI_SET_INFO));
  sal_memset(&tmp_dev.llp_cos->ether_type_multi_set, 0x0, sizeof(SOC_SAND_MULTI_SET_INFO));
  sal_memset(&tmp_dev.eth_policer_mtr_profile->eth_meter_profile_multi_set, 0x0, sizeof(SOC_SAND_MULTI_SET_INFO));
  sal_memset(&tmp_dev.eth_policer_mtr_profile->global_meter_profile_multi_set, 0x0, sizeof(SOC_SAND_MULTI_SET_INFO));
  sal_memset(&tmp_dev.l2_lif->vlan_compression_range_multi_set, 0x0, sizeof(SOC_SAND_MULTI_SET_INFO));
  
  *buffer_ptr = buffer;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_pb_pp_ssr_copy_data_from_dev_fields_to_buffer", 0, 0);
}

/* helper function for soc_pb_pp_ssr_PB_PP_SW_DB_SSR_V01_data_load*/
STATIC uint32
  soc_pb_pp_ssr_copy_data_from_buffer_to_dev_fields(
      SOC_SAND_IN  int     unit,
      SOC_SAND_OUT const uint8 **buffer_ptr
    )
{
  SOC_PB_PP_SW_DB_DEVICE
    *dev_ptr;
  const uint8
    *buffer;
  uint32
    res = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  dev_ptr = Soc_pb_pp_sw_db.device[unit];

  if (dev_ptr == NULL)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_NULL_POINTER_ERR, 10, exit);
  }

  buffer = *buffer_ptr;

  /*SOC_PB_PP_MGMT_OPERATION_MODE *oper_mode*/
  SOC_PETRA_ALLOC_ANY_SIZE(dev_ptr->oper_mode, SOC_PB_PP_MGMT_OPERATION_MODE, 1);
  *(dev_ptr->oper_mode) = *(const SOC_PB_PP_MGMT_OPERATION_MODE *)buffer;
  buffer += sizeof(SOC_PB_PP_MGMT_OPERATION_MODE);

  /*SOC_PB_PP_SW_DB_LLP_FILTER *llp_filter*/
  SOC_PETRA_ALLOC_ANY_SIZE(dev_ptr->llp_filter, SOC_PB_PP_SW_DB_LLP_FILTER, 1);
  *(dev_ptr->llp_filter) = *(const SOC_PB_PP_SW_DB_LLP_FILTER *)buffer;
  buffer += sizeof(SOC_PB_PP_SW_DB_LLP_FILTER);
  
  /*SOC_PB_PP_SW_DB_LLP_TRAP *llp_trap*/
  SOC_PETRA_ALLOC_ANY_SIZE(dev_ptr->llp_trap, SOC_PB_PP_SW_DB_LLP_TRAP, 1);
  *(dev_ptr->llp_trap) = *(const SOC_PB_PP_SW_DB_LLP_TRAP *)buffer;
  buffer += sizeof(SOC_PB_PP_SW_DB_LLP_TRAP);
 
  /*SOC_PB_PP_SW_DB_LLP_MIRROR *llp_mirror*/
  SOC_PETRA_ALLOC_ANY_SIZE(dev_ptr->llp_mirror, SOC_PB_PP_SW_DB_LLP_MIRROR, 1);
  *(dev_ptr->llp_mirror) = *(const SOC_PB_PP_SW_DB_LLP_MIRROR *)buffer;
  buffer += sizeof(SOC_PB_PP_SW_DB_LLP_MIRROR);
  
  /*SOC_PB_PP_SW_DB_LLP_VID_ASSIGN *llp_vid_assign*/
  SOC_PETRA_ALLOC_ANY_SIZE(dev_ptr->llp_vid_assign, SOC_PB_PP_SW_DB_LLP_VID_ASSIGN, 1);
  *(dev_ptr->llp_vid_assign) = *(const SOC_PB_PP_SW_DB_LLP_VID_ASSIGN *)buffer;
  buffer += sizeof(SOC_PB_PP_SW_DB_LLP_VID_ASSIGN);
  
  /*SOC_PB_PP_SW_DB_EG_MIRROR *eg_mirror*/
  SOC_PETRA_ALLOC_ANY_SIZE(dev_ptr->eg_mirror, SOC_PB_PP_SW_DB_EG_MIRROR, 1);
  *(dev_ptr->eg_mirror) = *(const SOC_PB_PP_SW_DB_EG_MIRROR *)buffer;
  buffer += sizeof(SOC_PB_PP_SW_DB_EG_MIRROR);
 
  /*SOC_PB_PP_SW_DB_LLP_COS *llp_cos*/
  SOC_PETRA_ALLOC_ANY_SIZE(dev_ptr->llp_cos, SOC_PB_PP_SW_DB_LLP_COS, 1);
  *(dev_ptr->llp_cos) = *(const SOC_PB_PP_SW_DB_LLP_COS *)buffer;
  buffer += sizeof(SOC_PB_PP_SW_DB_LLP_COS);

  /*SOC_PB_PPSW_DB_ETH_POLICER_MTR_PROFILE *eth_policer_mtr_profile*/
  SOC_PETRA_ALLOC_ANY_SIZE(dev_ptr->eth_policer_mtr_profile, SOC_PB_PPSW_DB_ETH_POLICER_MTR_PROFILE, 1);
  *(dev_ptr->eth_policer_mtr_profile) = *(const SOC_PB_PPSW_DB_ETH_POLICER_MTR_PROFILE *)buffer;
  buffer += sizeof(SOC_PB_PPSW_DB_ETH_POLICER_MTR_PROFILE);
  
  /*SOC_PB_PP_SW_DB_IPV4_INFO *ipv4_info*/
  if (dev_ptr->oper_mode->ipv4_info.nof_vrfs)
  {
    SOC_PETRA_ALLOC_ANY_SIZE(dev_ptr->ipv4_info, SOC_PB_PP_SW_DB_IPV4_INFO, 1);
    *(dev_ptr->ipv4_info) = *(const SOC_PB_PP_SW_DB_IPV4_INFO *)buffer;
    buffer += sizeof(SOC_PB_PP_SW_DB_IPV4_INFO);
  }
  else
  {
    dev_ptr->ipv4_info = NULL; /* no SSR support from old version */
  }
 
  /*SOC_PB_PP_SW_DB_ILM_INFO *ilm_info*/
  SOC_PETRA_ALLOC_ANY_SIZE(dev_ptr->ilm_info, SOC_PB_PP_SW_DB_ILM_INFO, 1);
  *(dev_ptr->ilm_info) = *(const SOC_PB_PP_SW_DB_ILM_INFO *)buffer;
  buffer += sizeof(SOC_PB_PP_SW_DB_ILM_INFO);
  
  /*SOC_PB_PP_SW_DB_FWD_MACT *fwd_mact*/
  SOC_PETRA_ALLOC_ANY_SIZE(dev_ptr->fwd_mact, SOC_PB_PP_SW_DB_FWD_MACT, 1);
  *(dev_ptr->fwd_mact) = *(const SOC_PB_PP_SW_DB_FWD_MACT *)buffer;
  buffer += sizeof(SOC_PB_PP_SW_DB_FWD_MACT);
  
  /*SOC_PB_PP_SW_DB_LIF_COS *lif_cos*/
  SOC_PETRA_ALLOC_ANY_SIZE(dev_ptr->lif_cos, SOC_PB_PP_SW_DB_LIF_COS, 1);
  *(dev_ptr->lif_cos) = *(const SOC_PB_PP_SW_DB_LIF_COS *)buffer;
  buffer += sizeof(SOC_PB_PP_SW_DB_LIF_COS);
  
  /*SOC_PB_PP_SW_DB_LIF_TABLE *lif_table*/
  SOC_PETRA_ALLOC_ANY_SIZE(dev_ptr->lif_table, SOC_PB_PP_SW_DB_LIF_TABLE, 1);
  *(dev_ptr->lif_table) = *(const SOC_PB_PP_SW_DB_LIF_TABLE *)buffer;
  buffer += sizeof(SOC_PB_PP_SW_DB_LIF_TABLE);
  
  /*SOC_PB_PP_SW_DB_FEC *fec*/
  SOC_PETRA_ALLOC_ANY_SIZE(dev_ptr->fec, SOC_PB_PP_SW_DB_FEC, 1);
  *(dev_ptr->fec) = *(const SOC_PB_PP_SW_DB_FEC *)buffer;
  buffer += sizeof(SOC_PB_PP_SW_DB_FEC);
  
  /*SOC_PB_PP_SW_DB_DIAG *diag*/
  SOC_PETRA_ALLOC_ANY_SIZE(dev_ptr->diag, SOC_PB_PP_SW_DB_DIAG, 1);
  *(dev_ptr->diag) = *(const SOC_PB_PP_SW_DB_DIAG *)buffer;
  buffer += sizeof(SOC_PB_PP_SW_DB_DIAG);
  
  /*SOC_PB_PP_SW_DB_FP *fp*/
  SOC_PETRA_ALLOC_ANY_SIZE(dev_ptr->fp, SOC_PB_PP_SW_DB_FP, 1);
  *(dev_ptr->fp) = *(const SOC_PB_PP_SW_DB_FP *)buffer;
  buffer += sizeof(SOC_PB_PP_SW_DB_FP);
  
  /*SOC_PB_PP_SW_DB_ISEM *isem*/
  SOC_PETRA_ALLOC_ANY_SIZE(dev_ptr->isem, SOC_PB_PP_SW_DB_ISEM, 1);
  *(dev_ptr->isem) = *(const SOC_PB_PP_SW_DB_ISEM *)buffer;
  buffer += sizeof(SOC_PB_PP_SW_DB_ISEM);
  
  /*SOC_PB_PP_SW_DB_LAG *lag*/
  SOC_PETRA_ALLOC_ANY_SIZE(dev_ptr->lag, SOC_PB_PP_SW_DB_LAG, 1);
  *(dev_ptr->lag) = *(const SOC_PB_PP_SW_DB_LAG *)buffer;
  buffer += sizeof(SOC_PB_PP_SW_DB_LAG);
  
  /*SOC_PB_PP_SW_DB_L2_LIF *l2_lif*/
  SOC_PETRA_ALLOC_ANY_SIZE(dev_ptr->l2_lif, SOC_PB_PP_SW_DB_L2_LIF, 1);
  *(dev_ptr->l2_lif) = *(const SOC_PB_PP_SW_DB_L2_LIF *)buffer;
  buffer += sizeof(SOC_PB_PP_SW_DB_L2_LIF);
  
  *buffer_ptr = buffer;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_pb_pp_ssr_copy_data_from_buffer_to_dev_fields", 0, 0);
}


uint32
  soc_pb_pp_ssr_PB_PP_SW_DB_SSR_V01_additional_buff_size_get(
    SOC_SAND_IN  int   unit,
    SOC_SAND_OUT uint32    *buff_byte_size
  )
{
  uint32
    ds_size_for_save,
    res,
    lpm_indx,
    mem_indx;
  SOC_PB_PP_SW_DB_DEVICE
    *dev_ptr;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  dev_ptr = Soc_pb_pp_sw_db.device[unit];

  if (dev_ptr == NULL)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_NULL_POINTER_ERR, 10, exit);
  }

  *buff_byte_size = 0;

  /*size of all the db fields*/
  *buff_byte_size += sizeof(SOC_PB_PP_MGMT_OPERATION_MODE);
  *buff_byte_size += sizeof(SOC_PB_PP_SW_DB_LLP_FILTER);
  *buff_byte_size += sizeof(SOC_PB_PP_SW_DB_LLP_TRAP);
  *buff_byte_size += sizeof(SOC_PB_PP_SW_DB_LLP_MIRROR);
  *buff_byte_size += sizeof(SOC_PB_PP_SW_DB_LLP_VID_ASSIGN);
  *buff_byte_size += sizeof(SOC_PB_PP_SW_DB_EG_MIRROR);
  *buff_byte_size += sizeof(SOC_PB_PP_SW_DB_LLP_COS);
  *buff_byte_size += sizeof(SOC_PB_PPSW_DB_ETH_POLICER_MTR_PROFILE);
  *buff_byte_size += sizeof(SOC_PB_PP_SW_DB_ILM_INFO);
  *buff_byte_size += sizeof(SOC_PB_PP_SW_DB_FWD_MACT);
  *buff_byte_size += sizeof(SOC_PB_PP_SW_DB_LIF_COS);
  *buff_byte_size += sizeof(SOC_PB_PP_SW_DB_LIF_TABLE);
  *buff_byte_size += sizeof(SOC_PB_PP_SW_DB_FEC);
  *buff_byte_size += sizeof(SOC_PB_PP_SW_DB_DIAG);
  *buff_byte_size += sizeof(SOC_PB_PP_SW_DB_FP);
  *buff_byte_size += sizeof(SOC_PB_PP_SW_DB_ISEM);
  *buff_byte_size += sizeof(SOC_PB_PP_SW_DB_LAG);
  *buff_byte_size += sizeof(SOC_PB_PP_SW_DB_L2_LIF);
  if (dev_ptr->ipv4_info != NULL)
  {
    *buff_byte_size += sizeof(SOC_PB_PP_SW_DB_IPV4_INFO);
  }

  
  if (dev_ptr->ipv4_info != NULL)
  {
  /***************************************************************************************************************/
    /* SOC_PB_PP_SW_DB_IPV4_INFO contain pointers - next block sums the size needed to be allocated to hold their data */
    /***************************************************************************************************************/
    /*ipv4_info->vrf_modified_bitmask*/
    *buff_byte_size += ((sizeof(*dev_ptr->ipv4_info->vrf_modified_bitmask)) * SOC_PB_PP_SW_IPV4_VRF_BITMAP_SIZE);
    /*dev_ptr->ipv4_info.free_list_size*/
    *buff_byte_size += (dev_ptr->ipv4_info->lpm_mngr.init_info.nof_mems * sizeof(uint32));
    /*dev_ptr->ipv4_info.free_list[]*/
    for (mem_indx = 0; mem_indx < dev_ptr->ipv4_info->lpm_mngr.init_info.nof_mems; ++mem_indx)
    {
      *buff_byte_size += (SOC_PB_PP_SW_DB_FREE_LIST_MAX_SIZE * sizeof(uint16));
    }
    /*lpm_init_info->nof_bits_per_bank*/
    *buff_byte_size += (sizeof(*dev_ptr->ipv4_info->lpm_mngr.init_info.nof_bits_per_bank) * dev_ptr->ipv4_info->lpm_mngr.init_info.nof_banks);
    /*lpm_init_info->bank_to_mem*/
    *buff_byte_size += (sizeof(*dev_ptr->ipv4_info->lpm_mngr.init_info.bank_to_mem) * dev_ptr->ipv4_info->lpm_mngr.init_info.nof_banks);
    /*lpm_init_info->nof_rows_per_mem*/
    *buff_byte_size += (sizeof(*dev_ptr->ipv4_info->lpm_mngr.init_info.nof_rows_per_mem) * dev_ptr->ipv4_info->lpm_mngr.init_info.nof_mems);
    /*data_info_ptr->bit_depth_per_bank*/
    *buff_byte_size += (sizeof(uint32) * (dev_ptr->ipv4_info->lpm_mngr.init_info.nof_banks+1));
  }
  
  
  /***************************************************************************************************************/
  /*                                      sum all data structures size                                           */
  /***************************************************************************************************************/
  /*llp_trap->l3_protocols_multi_set*/
  res = soc_sand_multi_set_get_size_for_save(
          &dev_ptr->llp_trap->l3_protocols_multi_set,
          &ds_size_for_save
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  *buff_byte_size += ds_size_for_save;

  /*llp_mirror->mirror_profile_multi_set*/
  res = soc_sand_multi_set_get_size_for_save(
          &dev_ptr->llp_mirror->mirror_profile_multi_set,
          &ds_size_for_save
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  *buff_byte_size += ds_size_for_save;

  /*eg_mirror->mirror_profile_multi_set*/
  res = soc_sand_multi_set_get_size_for_save(
          &dev_ptr->eg_mirror->mirror_profile_multi_set,
          &ds_size_for_save
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  *buff_byte_size += ds_size_for_save;

  /*llp_cos->ether_type_multi_set*/
  res = soc_sand_multi_set_get_size_for_save(
          &dev_ptr->llp_cos->ether_type_multi_set,
          &ds_size_for_save
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  *buff_byte_size += ds_size_for_save;

  /*eth_policer_mtr_profile->eth_meter_profile_multi_set*/
  res = soc_sand_multi_set_get_size_for_save(
          &dev_ptr->eth_policer_mtr_profile->eth_meter_profile_multi_set,
          &ds_size_for_save
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  *buff_byte_size += ds_size_for_save;

  /*eth_policer_mtr_profile->global_meter_profile_multi_set*/
  res = soc_sand_multi_set_get_size_for_save(
          &dev_ptr->eth_policer_mtr_profile->global_meter_profile_multi_set,
          &ds_size_for_save
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  *buff_byte_size += ds_size_for_save;

  /*l2_lif->vlan_compression_range_multi_set*/
  res = soc_sand_multi_set_get_size_for_save(
          &dev_ptr->l2_lif->vlan_compression_range_multi_set,
          &ds_size_for_save
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
  *buff_byte_size += ds_size_for_save;


  if (dev_ptr->ipv4_info != NULL)
  {
    /*memory allocated for data structures in soc_pb_pp_ipv4_lpm_mngr_create(&ipv4_info->lpm_mngr)*/
      /*memory allocated for soc_sand_pat_tree_create for dev_ptr->ipv4_info->lpm_mngr.init_info.lpms[lpm_indx]*/
    for (lpm_indx = 0; lpm_indx < dev_ptr->ipv4_info->lpm_mngr.init_info.nof_lpms; ++lpm_indx)
    {
      res = soc_sand_pat_tree_get_size_for_save(
              &dev_ptr->ipv4_info->lpm_mngr.init_info.lpms[lpm_indx],
              0,
              &ds_size_for_save
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
      *buff_byte_size += ds_size_for_save;
    }
      /*memory allocated for soc_sand_arr_mem_allocator_create for dev_ptr->ipv4_info->lpm_mngr.init_info.mem_allocators[mem_indx]*/
    for (mem_indx = 0; mem_indx < dev_ptr->ipv4_info->lpm_mngr.init_info.nof_mems; ++mem_indx)
    {
      res = soc_sand_arr_mem_allocator_get_size_for_save(
              &dev_ptr->ipv4_info->lpm_mngr.init_info.mem_allocators[mem_indx],
              0,
              &ds_size_for_save
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
      *buff_byte_size += ds_size_for_save;
    }
      /*memory allocated for soc_sand_group_mem_ll_create for dev_ptr->ipv4_info->lpm_mngr.init_info.rev_ptrs[mem_indx]*/
    if (dev_ptr->ipv4_info->lpm_mngr.init_info.flags & SOC_PB_PP_LPV4_LPM_SUPPORT_DEFRAG)
    {
      /* first bank has no backward pointers */
      for (mem_indx = 1; mem_indx < dev_ptr->ipv4_info->lpm_mngr.init_info.nof_mems; ++mem_indx)
      {
        res = soc_sand_group_mem_ll_get_size_for_save(
                dev_ptr->ipv4_info->lpm_mngr.init_info.rev_ptrs[mem_indx],
                &ds_size_for_save
               );
        SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
        *buff_byte_size += ds_size_for_save;
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_pb_pp_ssr_PB_PP_SW_DB_SSR_V01_additional_buff_size_get", 0, 0);
}

uint32
  soc_pb_pp_ssr_PB_PP_SW_DB_SSR_V01_data_save(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_OUT SOC_PB_PP_SW_DB_DEVICE_SSR_V01     *ssr_data,
    SOC_SAND_OUT uint8                          *additional_data,
    SOC_SAND_IN  uint32                          additional_data_size
  )
{
  uint32
    act_size,
    mem_indx,
    lpm_indx,
    size = 0,
    res;
  SOC_PB_PP_SW_DB_DEVICE
    *dev_ptr; 
  uint8      *additional_data_orig;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(ssr_data);

  dev_ptr = Soc_pb_pp_sw_db.device[unit];

  if (dev_ptr == NULL)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_NULL_POINTER_ERR, 10, exit);
  }

  additional_data_orig = additional_data;


  /************************************************************************/
  /*                       save the db fields                             */
  /************************************************************************/
  res = soc_pb_pp_ssr_copy_data_from_dev_fields_to_buffer(unit,
                                                      &additional_data,
                                                      additional_data_orig,
                                                      additional_data_size);



  

  /************************************************************************/
  /*                  save data from inside field pointers                */
  /************************************************************************/
  if(dev_ptr->ipv4_info != NULL)
  {
    /*save ipv4_info->vrf_modified_bitmask*/
    size = ((sizeof(*dev_ptr->ipv4_info->vrf_modified_bitmask)) * SOC_PB_PP_SW_IPV4_VRF_BITMAP_SIZE);
    if((additional_data - additional_data_orig + size) > additional_data_size) {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_PP_SCACHE_SIZE_TOO_SMALL_ERR, 10, exit);        
    }
    res = SOC_SAND_OK; sal_memcpy(
            additional_data,
            dev_ptr->ipv4_info->vrf_modified_bitmask,
            size
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    additional_data += size;

    /*save dev_ptr->ipv4_info.free_list_size*/
    size = (dev_ptr->ipv4_info->lpm_mngr.init_info.nof_mems * sizeof(uint32));
    if((additional_data - additional_data_orig + size) > additional_data_size) {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_PP_SCACHE_SIZE_TOO_SMALL_ERR, 20, exit);        
    }
    res = SOC_SAND_OK; sal_memcpy(
            additional_data,
            dev_ptr->ipv4_info->free_list_size,
            size
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    additional_data += size;

    /*save dev_ptr->ipv4_info.free_list[]*/
    size = (SOC_PB_PP_SW_DB_FREE_LIST_MAX_SIZE * sizeof(uint16));
    for (mem_indx = 0; mem_indx < dev_ptr->ipv4_info->lpm_mngr.init_info.nof_mems; ++mem_indx)
    {
      if((additional_data - additional_data_orig + size) > additional_data_size) {
          SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_PP_SCACHE_SIZE_TOO_SMALL_ERR, 30, exit);        
      }
      res = SOC_SAND_OK; sal_memcpy(
              additional_data,
              dev_ptr->ipv4_info->free_list[mem_indx],
              size
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
      additional_data += size;
    }

    /*save lpm_init_info->nof_bits_per_bank*/
    size = (sizeof(*dev_ptr->ipv4_info->lpm_mngr.init_info.nof_bits_per_bank) * dev_ptr->ipv4_info->lpm_mngr.init_info.nof_banks);
    if((additional_data - additional_data_orig + size) > additional_data_size) {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_PP_SCACHE_SIZE_TOO_SMALL_ERR, 40, exit);        
    }
    res = SOC_SAND_OK; sal_memcpy(
            additional_data,
            dev_ptr->ipv4_info->lpm_mngr.init_info.nof_bits_per_bank,
            size
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    additional_data += size;

    /*save lpm_init_info->bank_to_mem*/
    size = (sizeof(*dev_ptr->ipv4_info->lpm_mngr.init_info.bank_to_mem) * dev_ptr->ipv4_info->lpm_mngr.init_info.nof_banks);
    if((additional_data - additional_data_orig + size) > additional_data_size) {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_PP_SCACHE_SIZE_TOO_SMALL_ERR, 50, exit);        
    }
    res = SOC_SAND_OK; sal_memcpy(
            additional_data,
            dev_ptr->ipv4_info->lpm_mngr.init_info.bank_to_mem,
            size
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    additional_data += size;

    /*save lpm_init_info->nof_rows_per_mem*/
    size = (sizeof(*dev_ptr->ipv4_info->lpm_mngr.init_info.nof_rows_per_mem) * dev_ptr->ipv4_info->lpm_mngr.init_info.nof_mems);
    if((additional_data - additional_data_orig + size) > additional_data_size) {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_PP_SCACHE_SIZE_TOO_SMALL_ERR, 60, exit);        
    }
    res = SOC_SAND_OK; sal_memcpy(
            additional_data,
            dev_ptr->ipv4_info->lpm_mngr.init_info.nof_rows_per_mem,
            size
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
    additional_data += size;

    /*save data_info_ptr->bit_depth_per_bank*/
    size = (sizeof(uint32) * (dev_ptr->ipv4_info->lpm_mngr.init_info.nof_banks+1));
    if((additional_data - additional_data_orig + size) > additional_data_size) {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_PP_SCACHE_SIZE_TOO_SMALL_ERR, 70, exit);        
    }
    res = SOC_SAND_OK; sal_memcpy(
            additional_data,
            dev_ptr->ipv4_info->lpm_mngr.data_info.bit_depth_per_bank,
            size
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
    additional_data += size;
  }

  
  /************************************************************************/
  /*                  save data from data structures                      */
  /************************************************************************/

  /*llp_trap->l3_protocols_multi_set*/
  res = soc_sand_multi_set_get_size_for_save(
          &dev_ptr->llp_trap->l3_protocols_multi_set,
          &size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
  if((additional_data - additional_data_orig + size) > additional_data_size) {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_PP_SCACHE_SIZE_TOO_SMALL_ERR, 80, exit);        
  }
  res = soc_sand_multi_set_save(
          &dev_ptr->llp_trap->l3_protocols_multi_set,
          additional_data,
          size,
          &act_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
  additional_data += act_size;

  /*llp_mirror->mirror_profile_multi_set*/
  res = soc_sand_multi_set_get_size_for_save(
          &dev_ptr->llp_mirror->mirror_profile_multi_set,
          &size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
  if((additional_data - additional_data_orig + size) > additional_data_size) {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_PP_SCACHE_SIZE_TOO_SMALL_ERR, 90, exit);        
  }
  res = soc_sand_multi_set_save(
          &dev_ptr->llp_mirror->mirror_profile_multi_set,
          additional_data,
          size,
          &act_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
  additional_data += act_size;

  /*eg_mirror->mirror_profile_multi_set*/
  res = soc_sand_multi_set_get_size_for_save(
          &dev_ptr->eg_mirror->mirror_profile_multi_set,
          &size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);
  if((additional_data - additional_data_orig + size) > additional_data_size) {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_PP_SCACHE_SIZE_TOO_SMALL_ERR, 100, exit);        
  }
  res = soc_sand_multi_set_save(
          &dev_ptr->eg_mirror->mirror_profile_multi_set,
          additional_data,
          size,
          &act_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);
  additional_data += act_size;
  
  /*llp_cos->ether_type_multi_set*/
  res = soc_sand_multi_set_get_size_for_save(
          &dev_ptr->llp_cos->ether_type_multi_set,
          &size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);
  if((additional_data - additional_data_orig + size) > additional_data_size) {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_PP_SCACHE_SIZE_TOO_SMALL_ERR, 110, exit);        
  }
  res = soc_sand_multi_set_save(
          &dev_ptr->llp_cos->ether_type_multi_set,
          additional_data,
          size,
          &act_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);
  additional_data += act_size;
  
  /*eth_policer_mtr_profile->eth_meter_profile_multi_set*/
  res = soc_sand_multi_set_get_size_for_save(
          &dev_ptr->eth_policer_mtr_profile->eth_meter_profile_multi_set,
          &size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);
  if((additional_data - additional_data_orig + size) > additional_data_size) {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_PP_SCACHE_SIZE_TOO_SMALL_ERR, 120, exit);        
  }
  res = soc_sand_multi_set_save(
          &dev_ptr->eth_policer_mtr_profile->eth_meter_profile_multi_set,
          additional_data,
          size,
          &act_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);
  additional_data += act_size;
  

  /*eth_policer_mtr_profile->global_meter_profile_multi_set*/
  res = soc_sand_multi_set_get_size_for_save(
          &dev_ptr->eth_policer_mtr_profile->global_meter_profile_multi_set,
          &size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 190, exit);
  if((additional_data - additional_data_orig + size) > additional_data_size) {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_PP_SCACHE_SIZE_TOO_SMALL_ERR, 130, exit);        
  }
  res = soc_sand_multi_set_save(
          &dev_ptr->eth_policer_mtr_profile->global_meter_profile_multi_set,
          additional_data,
          size,
          &act_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);
  additional_data += act_size;
  

  /*l2_lif->vlan_compression_range_multi_set*/
  res = soc_sand_multi_set_get_size_for_save(
          &dev_ptr->l2_lif->vlan_compression_range_multi_set,
          &size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 210, exit);
  if((additional_data - additional_data_orig + size) > additional_data_size) {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_PP_SCACHE_SIZE_TOO_SMALL_ERR, 140, exit);        
  }
  res = soc_sand_multi_set_save(
          &dev_ptr->l2_lif->vlan_compression_range_multi_set,
          additional_data,
          size,
          &act_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 220, exit);
  additional_data += act_size;
  if(dev_ptr->ipv4_info != NULL)
  {
    /*save pat trees at dev_ptr->ipv4_info->lpm_mngr.init_info.lpms[lpm_indx]*/
    for (lpm_indx = 0; lpm_indx < dev_ptr->ipv4_info->lpm_mngr.init_info.nof_lpms; ++lpm_indx)
    {
      res = soc_sand_pat_tree_get_size_for_save(
              &dev_ptr->ipv4_info->lpm_mngr.init_info.lpms[lpm_indx],
              0,
              &size
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 230, exit);
      if((additional_data - additional_data_orig + size) > additional_data_size) {
          SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_PP_SCACHE_SIZE_TOO_SMALL_ERR, 150, exit);        
      }
      res = soc_sand_pat_tree_save(
              &dev_ptr->ipv4_info->lpm_mngr.init_info.lpms[lpm_indx],
              0,
              additional_data,
              size,
              &act_size
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 240, exit);
      additional_data += act_size;
    }
     
    /*save mem allocators at dev_ptr->ipv4_info->lpm_mngr.init_info.mem_allocators[mem_ndx]*/
    for (mem_indx = 0; mem_indx < dev_ptr->ipv4_info->lpm_mngr.init_info.nof_mems; ++mem_indx)
    {
      res = soc_sand_arr_mem_allocator_get_size_for_save(
              &dev_ptr->ipv4_info->lpm_mngr.init_info.mem_allocators[mem_indx],
              0,
              &size
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 250, exit);
      if((additional_data - additional_data_orig + size) > additional_data_size) {
          SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_PP_SCACHE_SIZE_TOO_SMALL_ERR, 160, exit);        
      }
      res = soc_sand_arr_mem_allocator_save(
              &dev_ptr->ipv4_info->lpm_mngr.init_info.mem_allocators[mem_indx],
              0,
              additional_data,
              size,
              &act_size
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 260, exit);
      additional_data += act_size;
    }

     /*save soc_sand_group_mem_ll at dev_ptr->ipv4_info->lpm_mngr.init_info.rev_ptrs[mem_indx]*/
    if (dev_ptr->ipv4_info->lpm_mngr.init_info.flags & SOC_PB_PP_LPV4_LPM_SUPPORT_DEFRAG)
    {
      /* first bank has no backward pointers */
      for (mem_indx = 1; mem_indx < dev_ptr->ipv4_info->lpm_mngr.init_info.nof_mems; ++mem_indx)
      {
        res = soc_sand_group_mem_ll_get_size_for_save(
                dev_ptr->ipv4_info->lpm_mngr.init_info.rev_ptrs[mem_indx],
                &size
               );
        SOC_SAND_CHECK_FUNC_RESULT(res, 270, exit);
        if((additional_data - additional_data_orig + size) > additional_data_size) {
            SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_PP_SCACHE_SIZE_TOO_SMALL_ERR, 170, exit);        
        }
        res = soc_sand_group_mem_ll_save(
              dev_ptr->ipv4_info->lpm_mngr.init_info.rev_ptrs[mem_indx],
              additional_data,
              size,
              &act_size
            );
        SOC_SAND_CHECK_FUNC_RESULT(res, 260, exit);
        additional_data += act_size;
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_pb_pp_ssr_PB_PP_SW_DB_SSR_V01_data_save", 0, 0);
}

uint32
  soc_pb_pp_ssr_PB_PP_SW_DB_SSR_V01_data_load(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint8                      *ssr_stream,
    SOC_SAND_IN  uint32                      size_of_pb_pp_ssr,
    SOC_SAND_IN  uint8                      *additional_data
  )
{
  uint32
    res,
    size,
    lpm_indx,
    mem_indx;
  SOC_PB_PP_SW_DB_DEVICE
    *dev_ptr;
  const uint8
    *buffer;
  SOC_SAND_PAT_TREE_LOAD_INFO
    standart_pat_tree_load_info;
  SOC_SAND_ARR_MEM_ALLOCATOR_LOAD_INFO
    standart_arr_mem_allocator_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(ssr_stream);

  standart_pat_tree_load_info.flags = 0;
  standart_pat_tree_load_info.node_set_fun = soc_pb_pp_sw_db_pat_tree_node_set;
  standart_pat_tree_load_info.node_get_fun = soc_pb_pp_sw_db_pat_tree_node_get;
  standart_pat_tree_load_info.node_ref_get_fun = NULL;/*soc_pb_pp_sw_db_pat_tree_node_ref_get;*/
  standart_pat_tree_load_info.root_set_fun = soc_pb_pp_sw_db_pat_tree_root_set;
  standart_pat_tree_load_info.root_get_fun = soc_pb_pp_sw_db_pat_tree_root_get;
  standart_pat_tree_load_info.node_data_is_identical_fun = soc_pb_pp_ipv4_lpm_mngr_pat_tree_node_data_is_identical_fun;
  standart_pat_tree_load_info.node_is_skip_in_lpm_identical_data_query_fun = soc_pb_pp_ipv4_lpm_mngr_pat_node_is_skip_in_lpm_identical_data_query_fun;

  standart_arr_mem_allocator_info.flags = 0;
  standart_arr_mem_allocator_info.entry_set_fun = soc_pb_pp_sw_db_ipv4_arr_mem_allocator_entry_set;
  standart_arr_mem_allocator_info.entry_get_fun = soc_pb_pp_sw_db_ipv4_arr_mem_allocator_entry_get;
  standart_arr_mem_allocator_info.free_set_fun = soc_pb_pp_sw_db_ipv4_arr_mem_allocator_free_entry_set;
  standart_arr_mem_allocator_info.free_get_fun = soc_pb_pp_sw_db_ipv4_arr_mem_allocator_free_entry_get;
  standart_arr_mem_allocator_info.read_fun = soc_pb_pp_sw_db_ipv4_arr_mem_allocator_read;
  standart_arr_mem_allocator_info.write_fun = soc_pb_pp_sw_db_ipv4_arr_mem_allocator_write;
  standart_arr_mem_allocator_info.entry_move_fun = soc_pb_pp_sw_db_ipv4_lpm_mem_align;

  Soc_pb_pp_sw_db.device[unit] = NULL;
  SOC_PETRA_ALLOC_ANY_SIZE( Soc_pb_pp_sw_db.device[unit], SOC_PB_PP_SW_DB_DEVICE, 1 );

  dev_ptr = Soc_pb_pp_sw_db.device[unit];

  /************************************************************************/
  /*                       load the db fields                             */
  /************************************************************************/
  buffer = additional_data;
  res = soc_pb_pp_ssr_copy_data_from_buffer_to_dev_fields(
          unit,
          &buffer
        );

  /************************************************************************/
  /*                  load data from inside field pointers                */
  /************************************************************************/

  /*load ipv4_info->vrf_modified_bitmask*/
  if (dev_ptr->ipv4_info != NULL)
  {
    SOC_PETRA_ALLOC_ANY_SIZE(dev_ptr->ipv4_info->vrf_modified_bitmask, uint32, SOC_PB_PP_SW_IPV4_VRF_BITMAP_SIZE);
    size = ((sizeof(*dev_ptr->ipv4_info->vrf_modified_bitmask)) * SOC_PB_PP_SW_IPV4_VRF_BITMAP_SIZE);
    res = SOC_SAND_OK; sal_memcpy(
            dev_ptr->ipv4_info->vrf_modified_bitmask,
            buffer,
            size
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    buffer += size;

    /*load dev_ptr->ipv4_info.free_list_size*/
    SOC_PETRA_ALLOC_ANY_SIZE(dev_ptr->ipv4_info->free_list_size, uint32, dev_ptr->ipv4_info->lpm_mngr.init_info.nof_mems);
    size = (dev_ptr->ipv4_info->lpm_mngr.init_info.nof_mems * sizeof(uint32));
    res = SOC_SAND_OK; sal_memcpy(
            dev_ptr->ipv4_info->free_list_size,
            buffer,
            size
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    buffer += size;

    /*allocate free list pointers*/
    SOC_PETRA_ALLOC_ANY_SIZE(dev_ptr->ipv4_info->free_list,uint16 *, dev_ptr->ipv4_info->lpm_mngr.init_info.nof_mems);
    size = (dev_ptr->ipv4_info->lpm_mngr.init_info.nof_mems * sizeof(uint16 *));
    res = SOC_SAND_OK; sal_memset(
            dev_ptr->ipv4_info->free_list,
            0x0,
            size
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);



    /*load dev_ptr->ipv4_info.free_list[]*/
    size = (SOC_PB_PP_SW_DB_FREE_LIST_MAX_SIZE * sizeof(uint16));
    for (mem_indx = 0; mem_indx < dev_ptr->ipv4_info->lpm_mngr.init_info.nof_mems; ++mem_indx)
    {
      SOC_PETRA_ALLOC_ANY_SIZE(dev_ptr->ipv4_info->free_list[mem_indx], uint16, SOC_PB_PP_SW_DB_FREE_LIST_MAX_SIZE);
      res = SOC_SAND_OK; sal_memcpy(
              dev_ptr->ipv4_info->free_list[mem_indx],
              buffer,
              size
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
      buffer += size;
    }

    /*load lpm_init_info->nof_bits_per_bank*/
    SOC_PETRA_ALLOC_ANY_SIZE(dev_ptr->ipv4_info->lpm_mngr.init_info.nof_bits_per_bank, uint32, dev_ptr->ipv4_info->lpm_mngr.init_info.nof_banks);
    size = (sizeof(*dev_ptr->ipv4_info->lpm_mngr.init_info.nof_bits_per_bank) * dev_ptr->ipv4_info->lpm_mngr.init_info.nof_banks);
    res = SOC_SAND_OK; sal_memcpy(
            dev_ptr->ipv4_info->lpm_mngr.init_info.nof_bits_per_bank,
            buffer,
            size
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    buffer += size;

    /*load lpm_init_info->bank_to_mem*/
    SOC_PETRA_ALLOC_ANY_SIZE(dev_ptr->ipv4_info->lpm_mngr.init_info.bank_to_mem, uint32, dev_ptr->ipv4_info->lpm_mngr.init_info.nof_banks);
    size = (sizeof(*dev_ptr->ipv4_info->lpm_mngr.init_info.bank_to_mem) * dev_ptr->ipv4_info->lpm_mngr.init_info.nof_banks);
    res = SOC_SAND_OK; sal_memcpy(
            dev_ptr->ipv4_info->lpm_mngr.init_info.bank_to_mem,
            buffer,
            size
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    buffer += size;

    /*load lpm_init_info->nof_rows_per_mem*/
    SOC_PETRA_ALLOC_ANY_SIZE(dev_ptr->ipv4_info->lpm_mngr.init_info.nof_rows_per_mem, uint32, dev_ptr->ipv4_info->lpm_mngr.init_info.nof_mems);
    size = (sizeof(*dev_ptr->ipv4_info->lpm_mngr.init_info.nof_rows_per_mem) * dev_ptr->ipv4_info->lpm_mngr.init_info.nof_mems);
    res = SOC_SAND_OK; sal_memcpy(
            dev_ptr->ipv4_info->lpm_mngr.init_info.nof_rows_per_mem,
            buffer,
            size
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    buffer += size;

    /*load data_info_ptr->bit_depth_per_bank*/
    SOC_PETRA_ALLOC_ANY_SIZE(dev_ptr->ipv4_info->lpm_mngr.data_info.bit_depth_per_bank, uint32, (dev_ptr->ipv4_info->lpm_mngr.init_info.nof_banks+1));
    size = (sizeof(uint32) * (dev_ptr->ipv4_info->lpm_mngr.init_info.nof_banks+1));
    res = SOC_SAND_OK; sal_memcpy(
            dev_ptr->ipv4_info->lpm_mngr.data_info.bit_depth_per_bank,
            buffer,
            size
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
    buffer += size;
  }

  /************************************************************************/
  /*                  load data from data structures                      */
  /************************************************************************/

  /*llp_trap->l3_protocols_multi_set*/
  res = soc_sand_multi_set_load(
          &buffer,
          soc_pb_sw_db_buffer_set_entry,
          soc_pb_sw_db_buffer_get_entry,
          &dev_ptr->llp_trap->l3_protocols_multi_set
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  /*llp_mirror->mirror_profile_multi_set*/
  res = soc_sand_multi_set_load(
          &buffer,
          soc_pb_sw_db_buffer_set_entry,
          soc_pb_sw_db_buffer_get_entry,
          &dev_ptr->llp_mirror->mirror_profile_multi_set
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

  /*eg_mirror->mirror_profile_multi_set*/
  res = soc_sand_multi_set_load(
          &buffer,
          soc_pb_sw_db_buffer_set_entry,
          soc_pb_sw_db_buffer_get_entry,
          &dev_ptr->eg_mirror->mirror_profile_multi_set
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  /*llp_cos->ether_type_multi_set*/
  res = soc_sand_multi_set_load(
          &buffer,
          soc_pb_sw_db_buffer_set_entry,
          soc_pb_sw_db_buffer_get_entry,
          &dev_ptr->llp_cos->ether_type_multi_set
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
  
  /*eth_policer_mtr_profile->eth_meter_profile_multi_set*/
  res = soc_sand_multi_set_load(
          &buffer,
          soc_pb_sw_db_buffer_set_entry,
          soc_pb_sw_db_buffer_get_entry,
          &dev_ptr->eth_policer_mtr_profile->eth_meter_profile_multi_set
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

  /*eth_policer_mtr_profile->global_meter_profile_multi_set*/
  res = soc_sand_multi_set_load(
          &buffer,
          soc_pb_sw_db_buffer_set_entry,
          soc_pb_sw_db_buffer_get_entry,
          &dev_ptr->eth_policer_mtr_profile->global_meter_profile_multi_set
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);

  /*l2_lif->vlan_compression_range_multi_set*/
  res = soc_sand_multi_set_load(
          &buffer,
          soc_pb_sw_db_buffer_set_entry,
          soc_pb_sw_db_buffer_get_entry,
          &dev_ptr->l2_lif->vlan_compression_range_multi_set
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);

  if(dev_ptr->ipv4_info!=NULL)
  {
    /*allocate pointers for pat_trees*/
    SOC_PETRA_ALLOC_ANY_SIZE(dev_ptr->ipv4_info->lpm_mngr.init_info.lpms, SOC_SAND_PAT_TREE_INFO, dev_ptr->ipv4_info->lpm_mngr.init_info.nof_lpms);
    size = (dev_ptr->ipv4_info->lpm_mngr.init_info.nof_lpms * sizeof(SOC_SAND_PAT_TREE_INFO));
    res = SOC_SAND_OK; sal_memset(
            dev_ptr->ipv4_info->lpm_mngr.init_info.lpms,
            0x0,
            size
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 143, exit);

    /*allocate pointers for mem_allocators*/
    SOC_PETRA_ALLOC_ANY_SIZE(dev_ptr->ipv4_info->lpm_mngr.init_info.mem_allocators, SOC_SAND_ARR_MEM_ALLOCATOR_INFO, dev_ptr->ipv4_info->lpm_mngr.init_info.nof_mems);
    size = (dev_ptr->ipv4_info->lpm_mngr.init_info.nof_mems * sizeof(SOC_SAND_ARR_MEM_ALLOCATOR_INFO));
    res = SOC_SAND_OK; sal_memset(
            dev_ptr->ipv4_info->lpm_mngr.init_info.mem_allocators,
            0x0,
            size
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 147, exit);

    /*allocate pointers for group_mem_ll if needed*/
    if (dev_ptr->ipv4_info->lpm_mngr.init_info.flags & SOC_PB_PP_LPV4_LPM_SUPPORT_DEFRAG)
    {
      SOC_PETRA_ALLOC_ANY_SIZE(dev_ptr->ipv4_info->lpm_mngr.init_info.rev_ptrs,SOC_SAND_GROUP_MEM_LL_INFO*,dev_ptr->ipv4_info->lpm_mngr.init_info.nof_mems);
      /* allocate member groups */
      /* for first there is no pointers on it*/
      for (mem_indx = 1; mem_indx < dev_ptr->ipv4_info->lpm_mngr.init_info.nof_mems; ++mem_indx)
      {
        SOC_PETRA_ALLOC_ANY_SIZE(dev_ptr->ipv4_info->lpm_mngr.init_info.rev_ptrs[mem_indx],SOC_SAND_GROUP_MEM_LL_INFO,1);
      }
    }

    
    /*load pat trees at dev_ptr->ipv4_info->lpm_mngr.init_info.lpms[lpm_indx]*/
    for (lpm_indx = 0; lpm_indx < dev_ptr->ipv4_info->lpm_mngr.init_info.nof_lpms; ++lpm_indx)
    {
     res = soc_sand_pat_tree_load(
              &buffer,
              &standart_pat_tree_load_info,
              &dev_ptr->ipv4_info->lpm_mngr.init_info.lpms[lpm_indx]
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);
    }
     
    /*load mem allocators at dev_ptr->ipv4_info->lpm_mngr.init_info.lpms[lpm_indx]*/
    for (mem_indx = 0; mem_indx < dev_ptr->ipv4_info->lpm_mngr.init_info.nof_mems; ++mem_indx)
    {
      res = soc_sand_arr_mem_allocator_load(
              &buffer,
              &standart_arr_mem_allocator_info,
              &dev_ptr->ipv4_info->lpm_mngr.init_info.mem_allocators[mem_indx]
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);
    }

    /*load soc_sand_group_mem_ll at dev_ptr->ipv4_info->lpm_mngr.init_info.rev_ptrs[mem_indx]*/
    if (dev_ptr->ipv4_info->lpm_mngr.init_info.flags & SOC_PB_PP_LPV4_LPM_SUPPORT_DEFRAG)
    {
      /* first bank has no backward pointers */
      for (mem_indx = 1; mem_indx < dev_ptr->ipv4_info->lpm_mngr.init_info.nof_mems; ++mem_indx)
      {
        res = soc_sand_group_mem_ll_load(
                &buffer,
                soc_pb_pp_sw_db_lpm_ll_group_entry_set,
                soc_pb_pp_sw_db_lpm_ll_group_entry_get,
                soc_pb_pp_sw_db_lpm_ll_member_entry_set,
                soc_pb_pp_sw_db_lpm_ll_member_entry_get,
                dev_ptr->ipv4_info->lpm_mngr.init_info.rev_ptrs[mem_indx]
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_pb_pp_ssr_PB_PP_SW_DB_SSR_V01_data_load", 0, 0);
}


/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_pb_pp_api_sw_db module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_sw_db_get_procs_ptr(void)
{
  return Soc_pb_pp_procedure_desc_element_sw_db;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     soc_pb_pp_api_sw_db module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_sw_db_get_errs_ptr(void)
{
  return Soc_pb_pp_error_desc_element_sw_db;
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>
