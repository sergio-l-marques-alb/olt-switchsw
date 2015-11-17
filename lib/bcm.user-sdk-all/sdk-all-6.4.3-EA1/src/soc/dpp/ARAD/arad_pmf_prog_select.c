/* $Id: arad_pmf_prog_select.c,v 1.79 Broadcom SDK $
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
 * $
*/

#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)


#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FP

/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/bsl.h>

#include <soc/dcmn/error.h>
#include <soc/mem.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>
#include <soc/dpp/SAND/SAND_FM/sand_pp_general.h>
#include <soc/dpp/ARAD/arad_pmf_prog_select.h>
#include <soc/dpp/ARAD/arad_pmf_low_level.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_pmf_low_level_pgm.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_trap_mgmt.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/PPC/ppc_api_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */



/* flags */
typedef enum
{
    /* remak this entry to be remove  */
    ARAD_PMF_SEL_LINE_REMOVE = 0x1,
    /* remak this entry as newly added i.e. from current calculation */
    ARAD_PMF_SEL_LINE_NEW = 0x2,
    ARAD_PMF_SEL_LINE_VALID = 0x4
} ARAD_PMF_SEL_LINE_ACTION;


#ifdef ARAD_PMF_OAM_MIRROR_WA_ENABLED
    #define ARAD_PMF_OAM_MIRROR_WA_NOF_STATIC_PROG_SEL_LINES(unit) \
        (soc_property_get((unit), spn_NUM_OAMP_PORTS, 0)? SOC_PPC_TRAP_CODE_OAM_FTMH_MIRROR_TRAP_CODES_NUM :0)
#else
    #define ARAD_PMF_OAM_MIRROR_WA_NOF_STATIC_PROG_SEL_LINES(unit) (0)
#endif /* ARAD_PMF_OAM_MIRROR_WA_ENABLED */

/* 
 * Indicate first line of programmable program selection space: 
 * - for Ethernet, after the OAM WA lines in top of the program selection table 
 * - for TM, after the OAM WA lines, the Ethernet space and the static programs
 */
#define ARAD_PMF_PROG_SELECT_FIRST_LINE(unit, is_tm, init_info) \
    (is_tm ? (ARAD_PMF_NOF_LINES - init_info.nof_reserved_lines[ARAD_PMF_PSL_TYPE_TM]): ARAD_PMF_OAM_MIRROR_WA_NOF_STATIC_PROG_SEL_LINES(unit)) 

/* } */
/*************
 * MACROS    *
 *************/
/* { */

/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */





typedef enum
{
  ARAD_PMF_SEL_GROUP_INTERSECT,
  ARAD_PMF_SEL_GROUP_CONTAIN,
  ARAD_PMF_SEL_GROUP_CONTAINED,
  ARAD_PMF_SEL_GROUP_EQUAL,
  ARAD_PMF_SEL_GROUP_DISJOINT
} ARAD_PMF_SEL_GROUP_RELATION;


typedef struct
{
  /*
   *  which index to start writing at
   */
  uint32 start_index;
  /*
   *  direction of loading information,
   *  TRUE: low index to high index, i.e. ++
   *  else high index to low index, i.e. --
   */
  uint8 low_to_high;

} ARAD_PMF_HW_LOAD_INFO;


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
#if ARAD_DEBUG_IS_LVL1
void ARAD_PMF_SEL_GROUP_print(
    SOC_SAND_IN ARAD_PMF_SEL_GROUP *group
  );

#endif /* ARAD_DEBUG_IS_LVL1 */


uint32
  ARAD_PMF_SEL_GROUP_clear(
    SOC_SAND_OUT ARAD_PMF_SEL_GROUP *in_group
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

    SOC_SAND_CHECK_NULL_INPUT(in_group);
    sal_memset(in_group,0x0, sizeof(ARAD_PMF_SEL_GROUP));
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PMF_SEL_GROUP_clone()",0,0);
}

uint32
  ARAD_PMF_LINE_INFO_clear(
    SOC_SAND_OUT ARAD_SW_DB_PMF_PSL_LINE_INFO *line
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

    SOC_SAND_CHECK_NULL_INPUT(line);
    sal_memset(line,0x0, sizeof(ARAD_SW_DB_PMF_PSL_LINE_INFO));

    ARAD_PMF_SEL_GROUP_clear(&line->group);
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PMF_SEL_GROUP_clone()",0,0);
}


 
uint32 
  ARAD_PMF_SEL_GROUP_to_line(
      SOC_SAND_IN  int                         unit,
      SOC_SAND_IN  ARAD_FP_DATABASE_STAGE         stage,
      SOC_SAND_IN  uint32                         pfg_id,
      SOC_SAND_IN  uint32                         flags,
      SOC_SAND_IN  ARAD_PMF_SEL_GROUP             *group,
      SOC_SAND_OUT  ARAD_SW_DB_PMF_PSL_LINE_INFO  *line
  )
{
    ARAD_PMF_SEL_INIT_INFO  
        init_info;
    uint32
        is_tm = (flags & ARAD_PMF_SEL_ALLOC_TM)? 1: 0,
        res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    sal_memcpy(&line->group,group, sizeof(ARAD_PMF_SEL_GROUP));
    line->flags = 0;
    line->line_id = pfg_id;
    sal_memset(line->groups, 0x0, sizeof(uint32) * ARAD_PMF_GROUP_LEN);
    soc_sand_bitstream_set_bit(line->groups, pfg_id);

    /* 
     * Set the default program to be the regular Ethernet program: 
     * - it will be changed if any intersection with another line anyway 
     * - if no intersection, no DB for this PFG only. The regular Ethernet 
     * program is the default one for all Ethernet packets. Only Databases without 
     * any program selection will be inserted there. 
     */
    res = arad_sw_db_pmf_psl_init_info_get(unit, stage, &init_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    line->prog_id = init_info.pmf_pgm_default[is_tm];

    ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PMF_SEL_GROUP_clone()",0,0);
}


/*
 * alloc program id
 */
uint32 
  arad_pmf_sel_prog_alloc(
      SOC_SAND_IN  int               unit,
      SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  uint8                    is_for_tm, 
      SOC_SAND_OUT   uint32              *alloced_prog_id,
      SOC_SAND_OUT   uint8               *found
  )
{
    uint32
        pmf_pgm_ndx_min,
        pmf_pgm_ndx_max,
      program_rsrc = 0,
      prog_id=0;
    uint32
      res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *found = 0;

    res = arad_pmf_prog_select_pmf_pgm_borders_get(
              unit,
              stage,
              is_for_tm, 
              &pmf_pgm_ndx_min,
              &pmf_pgm_ndx_max
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

    res = arad_sw_db_pgm_program_rsrc_get(unit, stage, &program_rsrc);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    for(prog_id = pmf_pgm_ndx_min; prog_id < pmf_pgm_ndx_max; ++prog_id) {
        if(!SHR_BITGET(&program_rsrc,prog_id)){
            *alloced_prog_id = prog_id;
            *found = 1;
            break;
        }
    }

    if(*found) {
        SHR_BITSET(&program_rsrc,prog_id);

        res = arad_sw_db_pgm_program_rsrc_set(unit, stage, &program_rsrc);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_sel_prog_alloc()", prog_id, stage);
}


uint32 
  arad_pmf_sel_prog_free(
      SOC_SAND_IN  int               unit,
      SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  uint32                       is_for_tm,
      SOC_SAND_IN   uint32              prog_id,
      SOC_SAND_IN  uint32                   pmf_pgm_to
  )
{
    uint32
      pgms_db_pmb[ARAD_BIT_TO_U32(ARAD_SW_DB_PMF_NOF_DBS)],
        is_tm = is_for_tm,
      program_rsrc = 0,
        pmf_pgm_clean,
      res;
    ARAD_PMF_SEL_INIT_INFO  
        init_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_sw_db_pgm_program_rsrc_get(unit, stage, &program_rsrc);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
    SHR_BITCLR(&program_rsrc,prog_id);
    res = arad_sw_db_pgm_program_rsrc_set(unit, stage, &program_rsrc);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

    /* Clear the DB bitmap */
    sal_memset(pgms_db_pmb, 0x0, ARAD_BIT_TO_U32(ARAD_SW_DB_PMF_NOF_DBS) * sizeof(uint32));
    res = arad_sw_db_pmf_psl_pgm_dbs_set(unit, stage, prog_id, pgms_db_pmb);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 25, exit);

    /* See if the Program was the Default Ethernet, update it then */
    res = arad_sw_db_pmf_psl_init_info_get(unit, stage, &init_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    if (init_info.pmf_pgm_default[is_tm] == prog_id) {
        init_info.pmf_pgm_default[is_tm] = pmf_pgm_to;
        res = arad_sw_db_pmf_psl_init_info_set(unit, stage, &init_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);
    }

    /* 
     * Copy at ingress PMF the Raw PMF-Program for suppressing - nothing defined in HW 
     * Asummption Program-type = PMF-Program, at least for Raw 
     * Copy at SLB and Egress PMF the last program 
     */
    if (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF) {
        pmf_pgm_clean = ARAD_PMF_PROGRAM_STATIC_INDEX_GET(init_info.nof_reserved_lines[ARAD_PMF_PSL_TYPE_TM], 
                                              ARAD_PMF_PROG_SELECT_TM_PMF_PGM_MIN, 
                                              ARAD_PMF_PGM_TYPE_RAW);
    }
    else {
        pmf_pgm_clean = ARAD_PMF_LOW_LEVEL_PMF_PGM_NDX_MAX;
    }
    res = arad_pmf_psl_pmf_pgm_duplicate(
            unit, 
            stage,               
            pmf_pgm_clean, 
            prog_id, 
            FALSE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 95, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_sel_prog_free()", prog_id, 0);
}

STATIC
soc_mem_t 
  arad_pmf_sel_line_hw_table_get(
      SOC_SAND_IN  int                               unit,
      SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage
  )
{
    soc_mem_t
        pmf_sel_line_hw_table;

    switch (stage) {
    case ARAD_FP_DATABASE_STAGE_EGRESS:
        pmf_sel_line_hw_table = EGQ_PMF_PROGRAM_SELECTION_CAMm;
        break;
    case ARAD_FP_DATABASE_STAGE_INGRESS_FLP:
        if(SOC_IS_JERICHO(unit)) {
            pmf_sel_line_hw_table = IHP_FLP_PROGRAM_SELECTION_CAMm;
        } else {
            pmf_sel_line_hw_table = IHB_FLP_PROGRAM_SELECTION_CAMm;
        }
        break;
#ifdef BCM_88660_A0
    case ARAD_FP_DATABASE_STAGE_INGRESS_SLB:
        pmf_sel_line_hw_table = IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm;
        break;
#endif /* BCM_88660_A0 */
    default:
    case ARAD_FP_DATABASE_STAGE_INGRESS_PMF:
        pmf_sel_line_hw_table = IHB_PMF_PROGRAM_SELECTION_CAMm;
        break;
    }
    return pmf_sel_line_hw_table;
}

/*
 *  write to program selection table
 *  - match key (data + mask)
 *  - valid (true/false)
 *  - program
 */
uint32 
  arad_pmf_sel_line_hw_write(
      SOC_SAND_IN  int                unit,
      SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  uint32                 entry_offset,
      SOC_SAND_IN   ARAD_SW_DB_PMF_PSL_LINE_INFO      *line,
      SOC_SAND_IN   uint32                prog,
      SOC_SAND_IN   uint8               valid
  )
{
    uint32
      data[SOC_DPP_DEFS_MAX(IHB_PMF_PROGRAM_SELECTION_CAM_NOF_LONGS)];
    uint32
      res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_CLEAR(data, uint32, SOC_DPP_DEFS_MAX(IHB_PMF_PROGRAM_SELECTION_CAM_NOF_LONGS));

    /* line format like 0-83 dat, 84:167: mask, 168:172 program, 173:173 valid */

    /* copy data */
    SHR_BITCOPY_RANGE(data,0,line->group.value,0,ARAD_PMF_SEL_HW_DATA_NOF_BITS);
    /* copy mask */
    SHR_BITCOPY_RANGE(data,ARAD_PMF_SEL_HW_DATA_NOF_BITS,line->group.mask,0,ARAD_PMF_SEL_HW_DATA_NOF_BITS);

    /* copy program and valid*/
    soc_mem_field32_set(unit, arad_pmf_sel_line_hw_table_get(unit, stage), data, PROGRAMf, prog);
    soc_mem_field32_set(unit, arad_pmf_sel_line_hw_table_get(unit, stage), data, VALIDf, valid);

    /* write to Hw */
    res = soc_mem_write(
            unit,
            arad_pmf_sel_line_hw_table_get(unit, stage),
            MEM_BLOCK_ANY,
            entry_offset,
            data
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

    {
        char
            string_to_print[ARAD_PMF_SEL_LINE_NOF_BITS + 100];
        uint32
            bit_indx,
            mask_val,
            val;
        sal_sprintf(string_to_print, "     ");
        sal_sprintf(string_to_print + sal_strlen(string_to_print), "Program selection: write to stage %s, line %d, ", SOC_PPC_FP_DATABASE_STAGE_to_string(stage), entry_offset);
    if(valid) 
    {
            sal_sprintf(string_to_print + sal_strlen(string_to_print), "  program: %d, ", prog);
    }
    else
    {
            sal_sprintf(string_to_print + sal_strlen(string_to_print), "  program: invalid, ");
    }
    

        for(bit_indx = 0; bit_indx < ARAD_PMF_SEL_LINE_NOF_BITS; ++bit_indx) {
            mask_val = soc_sand_bitstream_test_bit(line->group.mask, bit_indx);
            if(mask_val == 1) 
            {
                sal_sprintf(string_to_print + sal_strlen(string_to_print), "*");
            }
            else
            {
                val = soc_sand_bitstream_test_bit(line->group.value, bit_indx);
                sal_sprintf(string_to_print + sal_strlen(string_to_print), "%d", val);
            }
            if((bit_indx+1)%32 == 0) 
            {
                sal_sprintf(string_to_print + sal_strlen(string_to_print), " ");
            }
        }
        LOG_DEBUG(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "%s\n\r"), string_to_print));
    }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_sel_line_hw_write()", entry_offset, stage);
}


/*
 *  write to program selection table
 *  - match key (data + mask)
 *  - valid (true/false)
 *  - program
 */
uint32 
  arad_pmf_sel_line_valid_set(
      SOC_SAND_IN  int                unit,
      SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  uint32                 entry_offset,
      SOC_SAND_IN   uint8               is_valid,
      SOC_SAND_IN   uint32               valid_or_program
  )
{
    uint32
      data[SOC_DPP_DEFS_MAX(IHB_PMF_PROGRAM_SELECTION_CAM_NOF_LONGS)];
    uint32
      res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_CLEAR(data, uint32, SOC_DPP_DEFS_MAX(IHB_PMF_PROGRAM_SELECTION_CAM_NOF_LONGS));

    res = soc_mem_read(
            unit,
            arad_pmf_sel_line_hw_table_get(unit, stage),
            MEM_BLOCK_ANY,
            entry_offset,
            data
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

    /* copy program and valid*/
    if (is_valid) {
        soc_mem_field32_set(unit, arad_pmf_sel_line_hw_table_get(unit, stage), data, VALIDf, valid_or_program);
    }
    else {
        soc_mem_field32_set(unit, arad_pmf_sel_line_hw_table_get(unit, stage), data, PROGRAMf, valid_or_program);
    }

    /* write to Hw */
    res = soc_mem_write(
            unit,
            arad_pmf_sel_line_hw_table_get(unit, stage),
            MEM_BLOCK_ANY,
            entry_offset,
            data
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_sel_line_valid_set()", entry_offset, stage);
}


uint32
  arad_pmf_psl_to_group_map(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  ARAD_PMF_PSL                    *tbl_data,
    SOC_SAND_OUT  ARAD_PMF_SEL_GROUP                 *group
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[SOC_DPP_DEFS_MAX(IHB_PMF_PROGRAM_SELECTION_CAM_NOF_LONGS)];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_PMF_PROGRAM_SELECTION_CAM_TBL_SET_UNSAFE);

  ARAD_PMF_SEL_GROUP_clear(group);

  ARAD_CLEAR(data, uint32, SOC_DPP_DEFS_MAX(IHB_PMF_PROGRAM_SELECTION_CAM_NOF_LONGS));

  switch (stage) {
  case ARAD_FP_DATABASE_STAGE_EGRESS:
      /* Egress PMF */
      res = arad_pp_egq_pmf_program_selection_cam_tbl_data_set_unsafe(
                unit,
                &(tbl_data->egress_pmf),
                data
              );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
      break;
  case ARAD_FP_DATABASE_STAGE_INGRESS_PMF:
      /* Ingress PMF */
      if (!SOC_IS_ARDON(unit)) {
          res = arad_pp_ihb_pmf_program_selection_cam_tbl_data_set_unsafe(
                    unit,
                    &(tbl_data->ingress_pmf),
                    data
                  );
          SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      }
      break;
#ifdef BCM_88660_A0
  case ARAD_FP_DATABASE_STAGE_INGRESS_SLB:
      if (SOC_IS_ARADPLUS(unit)) {
          /* Ingress SLB */
          if (!SOC_IS_ARDON(unit)) {
              res = arad_pp_ihb_slb_program_selection_cam_tbl_data_set_unsafe(
                        unit,
                        &(tbl_data->ingress_slb),
                        data
                      );
              SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
          }
          break;
      }
#endif /* BCM_88660_A0 */
  case ARAD_FP_DATABASE_STAGE_INGRESS_FLP:
      /* no program selection management in FLP */
  default:
      SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT, 30, exit);
  }


  /* line format like 0-83 dat, 84:167: mask, 168:172 program, 173:173 valid */

  /* copy data */
  SHR_BITCOPY_RANGE(group->value,0,data,0,ARAD_PMF_SEL_HW_DATA_NOF_BITS);
  /* copy mask */
  SHR_BITCOPY_RANGE(group->mask,0,data,ARAD_PMF_SEL_HW_DATA_NOF_BITS, ARAD_PMF_SEL_HW_DATA_NOF_BITS);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_psl_to_group_map()", stage, 0);
}


/*
 *  given new/update line, get program-id to use
 *  - program
 */
uint32 
  arad_pmf_sel_calc_program(
      SOC_SAND_IN  int                unit,
      SOC_SAND_IN   ARAD_SW_DB_PMF_PSL_LINE_INFO      *line,
      SOC_SAND_OUT   uint32               *prog
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *prog = line->prog_id;

  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_sel_calc_program()", 0, 0);
}



STATIC uint8
  arad_pmf_bitstream_equal(
    SOC_SAND_IN    uint32       *bit_stream1,
    SOC_SAND_IN       uint32    *bit_stream2,
    SOC_SAND_IN       uint32    size /* in longs */
  )
{
  uint32 i;
  /*
   */
  for(i=0; i<size; ++i)
  {
    if(bit_stream1[i] != bit_stream2[i])
    {
        return FALSE;
    }
  }
  return TRUE;
}


uint32
  ARAD_PMF_SEL_GROUP_clone(
    SOC_SAND_IN ARAD_PMF_SEL_GROUP *in_group,
    SOC_SAND_OUT ARAD_PMF_SEL_GROUP *out_group
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

    SOC_SAND_CHECK_NULL_INPUT(in_group);
    sal_memcpy(out_group,in_group, sizeof(ARAD_PMF_SEL_GROUP));
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PMF_SEL_GROUP_clone()",0,0);

}

/* check if two groups are disjoint */
/* if unmasked bit value is different then no intersection*/
uint8
  arad_pmf_sel_group_is_disjoint(
    SOC_SAND_IN  ARAD_PMF_SEL_GROUP *group1,
    SOC_SAND_IN  ARAD_PMF_SEL_GROUP *group2
  )
{
    ARAD_PMF_SEL_GROUP
        value_diff;

    
    ARAD_PMF_SEL_GROUP_clone(group1, &value_diff);
    /* or masks */
    soc_sand_bitstream_or(value_diff.mask, group2->mask, ARAD_PMF_SEL_LINE_LEN);
    /* !mask */
    soc_sand_bitstream_not(value_diff.mask, ARAD_PMF_SEL_LINE_LEN);
    /* xor values */
    soc_sand_bitstream_xor(value_diff.value, group2->value, ARAD_PMF_SEL_LINE_LEN);
    /* and diff-value with mask if zero then equal */
    soc_sand_bitstream_and(value_diff.value, value_diff.mask, ARAD_PMF_SEL_LINE_LEN);
    /* check if have one then not equal */
    if(soc_sand_bitstream_have_one(value_diff.value,ARAD_PMF_SEL_LINE_LEN))
    {
        return TRUE;
    }
    return FALSE;
}




ARAD_PMF_SEL_GROUP_RELATION
  arad_pmf_sel_group_rel_check(
    SOC_SAND_IN  ARAD_PMF_SEL_GROUP *group1,
    SOC_SAND_IN  ARAD_PMF_SEL_GROUP *group2
  )
{
    ARAD_PMF_SEL_GROUP
        or_group;
    uint8
        gr1_contain = FALSE,
        gr2_contain = FALSE;
    
    /* if value is different then no intersection*/
    if(arad_pmf_sel_group_is_disjoint(group1, group2))
    {
        return ARAD_PMF_SEL_GROUP_DISJOINT;
    }

    /* copy 1 to tmp group */
    ARAD_PMF_SEL_GROUP_clone(group1, &or_group);

    /* calc or on mask */
    soc_sand_bitstream_or(or_group.mask, group2->mask, ARAD_PMF_SEL_LINE_LEN);

    /* according to mask, if mask is 0 in gr1 --> 0 in gr2, then gr1 contain gr2 */
    if(arad_pmf_bitstream_equal(or_group.mask, group1->mask, ARAD_PMF_SEL_LINE_LEN))
    {
        gr1_contain = TRUE;
    }

    if(arad_pmf_bitstream_equal(or_group.mask, group2->mask, ARAD_PMF_SEL_LINE_LEN))
    {
        gr2_contain = TRUE;
    }

    if(gr1_contain && gr2_contain) 
    {
        return ARAD_PMF_SEL_GROUP_EQUAL;
    }
    if(gr1_contain) 
    {
        return ARAD_PMF_SEL_GROUP_CONTAIN;
    }
    if(gr2_contain) 
    {
        return ARAD_PMF_SEL_GROUP_CONTAINED;
    }

    return ARAD_PMF_SEL_GROUP_INTERSECT;
}

uint32
  ARAD_PMF_SEL_GROUP_build(
    SOC_SAND_IN  ARAD_PMF_SEL_GROUP *group1,
    SOC_SAND_IN  ARAD_PMF_SEL_GROUP *group2,
    SOC_SAND_OUT  ARAD_PMF_SEL_GROUP *res_group
  )
{
    
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

    SOC_SAND_CHECK_NULL_INPUT(group1);
    SOC_SAND_CHECK_NULL_INPUT(group2);
    SOC_SAND_CHECK_NULL_INPUT(res_group);


    ARAD_PMF_SEL_GROUP_clone(group1, res_group);
    /* and masks */
    soc_sand_bitstream_and(res_group->mask, group2->mask, ARAD_PMF_SEL_LINE_LEN);

    /* or values, for masked data, value is zero. if not masked in both, then value is equal */
    soc_sand_bitstream_or(res_group->value, group2->value, ARAD_PMF_SEL_LINE_LEN);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PMF_SEL_GROUP_build()",0,0);
}


uint32
  ARAD_PMF_SEL_GROUP_intersection(
    SOC_SAND_IN  ARAD_PMF_SEL_GROUP *group1,
    SOC_SAND_IN  ARAD_PMF_SEL_GROUP *group2,
    SOC_SAND_OUT  ARAD_PMF_SEL_GROUP *res_group,
    SOC_SAND_OUT  ARAD_PMF_SEL_GROUP_RELATION    *rel
  )
{
    uint32
        res;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

    SOC_SAND_CHECK_NULL_INPUT(group1);
    SOC_SAND_CHECK_NULL_INPUT(group2);
    SOC_SAND_CHECK_NULL_INPUT(res_group);


    *rel = arad_pmf_sel_group_rel_check(group1,group2);
    /* if value is different then no intersection*/
    if(*rel == ARAD_PMF_SEL_GROUP_DISJOINT)
    {
        goto exit;
    }

    res = ARAD_PMF_SEL_GROUP_build(group1, group2, res_group);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PMF_SEL_GROUP_intersection()",0,0);
}

/*
 * calc intersection between two lines. 
 * line1: is the new added line 
 */ 
uint32
  arad_pmf_line_intersection(
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE        stage,
    SOC_SAND_IN  ARAD_SW_DB_PMF_PSL_LINE_INFO *line1,
    SOC_SAND_IN  ARAD_SW_DB_PMF_PSL_LINE_INFO *line2,
    SOC_SAND_OUT  ARAD_SW_DB_PMF_PSL_LINE_INFO *res_line,
    SOC_SAND_OUT  uint8          *empty
  )
{
    ARAD_PMF_SEL_GROUP_RELATION
        rel;
    uint32
        res = 0;

    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

    SOC_SAND_CHECK_NULL_INPUT(line1);
    SOC_SAND_CHECK_NULL_INPUT(line2);
    SOC_SAND_CHECK_NULL_INPUT(res_line);

    ARAD_PMF_LINE_INFO_clear(res_line);

    /* intersect group */
    res = ARAD_PMF_SEL_GROUP_intersection(&(line1->group), &(line2->group), &(res_line->group), &rel);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    *empty = (rel == ARAD_PMF_SEL_GROUP_DISJOINT);
    /* groups in this line */
    if(!*empty) 
    {
        /* calc groups used in this intersection */
        sal_memcpy(res_line->groups, line1->groups,sizeof(line1->groups));
        soc_sand_bitstream_or(res_line->groups, line2->groups, ARAD_PMF_GROUP_LEN);
    }

    /* 
     * Update the program according to the other line 
     * The new PFG has no new Database at its creation 
     */
    res_line->prog_id = line2->prog_id;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_line_intersection()",0,0);
}


/*
* check if given intersection line exist or included on given layer and above
* 
*/

uint32 
  arad_pmf_is_line_covered_at_level(
      SOC_SAND_IN  int                unit,
      SOC_SAND_IN  ARAD_SW_DB_PMF_PSL_LINE_INFO       *line,
      SOC_SAND_IN  ARAD_SW_DB_PMF_PSL_LEVEL_INFO      *level_info,
      SOC_SAND_OUT uint8                *covered
  )
{
    uint32
      indx;
    ARAD_PMF_SEL_GROUP_RELATION
        rel;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    for(indx = 0; indx < level_info->nof_lines; ++indx) 
    {
        rel = arad_pmf_sel_group_rel_check(&(line->group),&(level_info->lines[indx].group));
        if(rel == ARAD_PMF_SEL_GROUP_EQUAL || rel == ARAD_PMF_SEL_GROUP_CONTAINED)
        {

            *covered = TRUE;
            goto exit;
        }
    }

    *covered = FALSE;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_is_line_covered_at_level()",0,0);
}


uint32 
  arad_pmf_check_group_on_upper_levels(
      SOC_SAND_IN  int                unit,
      SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN     ARAD_SW_DB_PMF_PSL_LINE_INFO    *group_line,  /* group in line representation */
      SOC_SAND_IN     uint32              start_level,
      SOC_SAND_IN  uint32                       flags,
      SOC_SAND_OUT    uint8             *covered
      
  )
{
    uint32
      level_indx;
    ARAD_SW_DB_PMF_PSL_LEVEL_INFO
        cur_level;
    uint32
        is_tm = (flags & ARAD_PMF_SEL_ALLOC_TM)? 1: 0,
        res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    for(level_indx = start_level; level_indx < ARAD_PMF_NOF_LEVELS; ++level_indx) 
    {
        /* get level info */
        res = arad_sw_db_level_info_get(unit, stage, level_indx, is_tm, &cur_level);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        res = arad_pmf_is_line_covered_at_level(unit,group_line,&cur_level,covered);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

        if(*covered) 
        {
            goto exit;
        }
    }

    *covered = FALSE;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_check_group_on_upper_levels()", stage, 0);
}



uint32 
  arad_pmf_add_line_at_level(
      SOC_SAND_IN  int                unit,
      SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  uint32                            flags,
      SOC_SAND_IN  ARAD_SW_DB_PMF_PSL_LINE_INFO       *line,
      SOC_SAND_INOUT  ARAD_SW_DB_PMF_PSL_LEVEL_INFO   *level_info
  )
{
    uint32
      indx;
    ARAD_PMF_SEL_GROUP_RELATION
        rel;
    uint8
        covered;
    uint32
        res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if ( flags & ARAD_PMF_SEL_ALLOC_ADVANCED_MODE ) {
        sal_memcpy(&level_info->lines[line->line_id], line, sizeof(ARAD_SW_DB_PMF_PSL_LINE_INFO));
        level_info->lines[line->line_id].flags |= ARAD_PMF_SEL_LINE_VALID;
        ARAD_DO_NOTHING_AND_EXIT;
    }

    /* 
     * Check if this line included in any level 
     * Not for delete: always covered since the same line is at the next level
     */
    if (!(flags & ARAD_PMF_SEL_ALLOC_FOR_DELETE)) {
        res = arad_pmf_check_group_on_upper_levels(unit, stage, line,level_info->level_index+1, flags, &covered);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        /* this line is covered by higher level, no need to add at this level */
        if(covered)
        {
            goto exit;
        }
    }


    for(indx = 0; indx < level_info->nof_lines; ++indx) 
    {
        /* 
         * No need to check during delete: 
         * -  if the new line is equal to another line, then they 
         * must have different PFG bitmaps (a verification was done already 
         * that this PFG bitmap is not present in this level). But 
         * then, their intersection should exist, not these lines 
         * - if the new line is contained, then its intersection should exist, 
         * not this one 
         */
        if (flags & ARAD_PMF_SEL_ALLOC_FOR_DELETE) {
            break;
        }

        rel = arad_pmf_sel_group_rel_check(&(line->group),&(level_info->lines[indx].group));
        if(rel == ARAD_PMF_SEL_GROUP_CONTAINED)
        {
            /* nothing to do */
            goto exit;
        }
        if(rel == ARAD_PMF_SEL_GROUP_EQUAL)
        {
            level_info->lines[indx].flags |= ARAD_PMF_SEL_LINE_REMOVE;
            level_info->nof_removed_lines++;
            /* nothing to do */
            goto exit;
        }
        if((rel == ARAD_PMF_SEL_GROUP_DISJOINT) || (rel == ARAD_PMF_SEL_GROUP_INTERSECT))
        {
            continue;
        }

        if(rel == ARAD_PMF_SEL_GROUP_CONTAIN)
        {
            level_info->lines[indx].flags |= ARAD_PMF_SEL_LINE_REMOVE;
            level_info->nof_removed_lines++;
        }
    }

    if(!(flags & ARAD_PMF_SEL_ALLOC_DO_NOT_ADD)) {
        /* add this group to this level, add it in new lines area  */
        sal_memcpy(&level_info->lines[level_info->nof_lines + level_info->nof_new_lines], line, sizeof(ARAD_SW_DB_PMF_PSL_LINE_INFO));
        level_info->lines[level_info->nof_lines + level_info->nof_new_lines].flags |= ARAD_PMF_SEL_LINE_VALID;
        level_info->nof_new_lines++;
    }
    
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_add_line_at_level()", stage, 0);
}

/* copy removed lines from level x, to level x+1*/
uint32 
  arad_pmf_level_transfer_lines(
      SOC_SAND_IN  int                unit,
      SOC_SAND_IN  ARAD_SW_DB_PMF_PSL_LEVEL_INFO      *from_level,
      SOC_SAND_INOUT  ARAD_SW_DB_PMF_PSL_LEVEL_INFO   *to_level,  
      SOC_SAND_OUT    uint8             *empty
      
  )
{
    uint32
      indx;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *empty = TRUE;

    for(indx = 0; indx < from_level->nof_lines; ++indx) 
    {
        if( from_level->lines[indx].flags & ARAD_PMF_SEL_LINE_REMOVE)
        {
            *empty = FALSE;
            sal_memcpy(&to_level->lines[to_level->nof_lines + to_level->nof_new_lines],
                       &from_level->lines[indx],
                       sizeof(ARAD_SW_DB_PMF_PSL_LINE_INFO));
            to_level->nof_new_lines++;
        }
    }

    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_level_transfer_lines()",0,0);
}

/* 
 * Compute if a new line is needed according to the 
 * previous and current level 
 */
uint32 
  arad_pmf_add_group_at_level(
      SOC_SAND_IN  int                            unit,
      SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  uint32                            pfg_id,
      SOC_SAND_IN  uint32                            flags,
      SOC_SAND_IN     ARAD_SW_DB_PMF_PSL_LINE_INFO    *group_line,  /* group in line representation */
      SOC_SAND_INOUT  ARAD_SW_DB_PMF_PSL_LEVEL_INFO   *prev_level,
      SOC_SAND_INOUT  ARAD_SW_DB_PMF_PSL_LEVEL_INFO   *next_level,  
      SOC_SAND_OUT    uint8                           *intersected
  )
{
    uint32
      local_flags,
      curr_level_line_indx,
      prev_level_line_indx;
    ARAD_SW_DB_PMF_PSL_LINE_INFO       
        cur_line;
    uint8
        is_first_pfg,
        is_tm = (flags & ARAD_PMF_SEL_ALLOC_TM)? 1: 0,
        duplicate_found,
        is_empty;
    uint32
        pfg_idx,
        groups_curr[ARAD_PMF_GROUP_LEN],
        res;
    ARAD_PP_FP_PFG_INFO      
        pfg_info;
    ARAD_PMF_PSL         
        psl;
    ARAD_PMF_SEL_GROUP
        res_group,
        pmf_group;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *intersected = FALSE;

    if (!(flags & ARAD_PMF_SEL_ALLOC_FOR_DELETE)) {
        /* 
         * Procedure for addition: 
         * - run over all the level-1 and intersect with the buffer to insert. If empty, skip 
         * - If not empty, see how to insert this intersection to the current level
         */
        for(prev_level_line_indx = 0; prev_level_line_indx < prev_level->nof_lines; ++prev_level_line_indx) 
        {
            res = arad_pmf_line_intersection(stage, group_line, &prev_level->lines[prev_level_line_indx], &cur_line, &is_empty);
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
            if(is_empty) 
            {
                continue;
            }
            *intersected = TRUE;
            res = arad_pmf_add_line_at_level(unit, stage, flags, &cur_line, next_level);
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
        }
        /*
         * Special case if no line in previous level, but lines in current level
         */
        if ((!prev_level->nof_lines) && next_level->nof_lines) {
            local_flags = flags | ARAD_PMF_SEL_ALLOC_DO_NOT_ADD;
            res = arad_pmf_add_line_at_level(unit, stage, local_flags, group_line, next_level);
            SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
        }
    }
    else {
        /*
         * Deletion procedure:
         *  - Run over all the current level and see if the current PFG is inside
         *  - If not, skip
         *  - Otherwise, run over all the level-1 lines and see if it is already present
         *      - If so, just delete the current line
         *      - Otherwise, insert the line:
         *          - Same PMF-Program (all the Databases relevant to the PFG were deleted before anyway)
         *          - Re-compute the buffer according to the present PFGs
         */
        for(curr_level_line_indx = 0; curr_level_line_indx < next_level->nof_lines; ++curr_level_line_indx) 
        {
            sal_memcpy(groups_curr, next_level->lines[curr_level_line_indx].groups, sizeof(groups_curr));

            if(!soc_sand_bitstream_test_bit(groups_curr, pfg_id)) 
            {
                continue;
            }
            /* Remove the PFG from the current group */
            soc_sand_bitstream_test_and_reset_bit(groups_curr, pfg_id);

            /* Find if there is a duplicate in previous level */
            duplicate_found = FALSE;
            for(prev_level_line_indx = 0; prev_level_line_indx < prev_level->nof_lines; ++prev_level_line_indx) 
            {
                if(sal_memcmp(groups_curr, prev_level->lines[prev_level_line_indx].groups, sizeof(groups_curr)) == 0) 
                {
                    /* Same list of suppported PFGs */
                    duplicate_found = TRUE;
                    break;
                }
            }

            /* No duplicate, insert in the previous level */
            if (!duplicate_found) {
                /* Compute the current line */
                sal_memcpy(&cur_line, &(next_level->lines[curr_level_line_indx]), sizeof(cur_line));
                soc_sand_bitstream_test_and_reset_bit(cur_line.groups, pfg_id);

                /* Rebuild the buffer */
                cur_line.flags = ARAD_PMF_SEL_LINE_NEW;
                ARAD_PMF_SEL_GROUP_clear(&(cur_line.group));
                is_first_pfg = TRUE;
                for (pfg_idx = 0; pfg_idx < ARAD_PP_FP_NOF_PFGS; pfg_idx++) {
                    if(!soc_sand_bitstream_test_bit(groups_curr, pfg_idx)) 
                    {
                        continue;
                    }

                    /* Retrieve the group */
                    arad_sw_db_pfg_info_get(unit, stage, pfg_idx, &pfg_info);
                    if (!pfg_info.is_array_qualifier) {
                        LOG_ERROR(BSL_LS_SOC_FP,
                                  (BSL_META_U(unit,
                                              "Unit %d Stage %s Pfg index %d - pre-selector not found.\n\r"),
                                   unit, SOC_PPC_FP_DATABASE_STAGE_to_string(stage), pfg_id));
                        /* Inconsistency between the code */
                        SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT, 40, exit);
                    }
                    /* Translate the PFG info to the table */
                    ARAD_PMF_PSL_clear(unit, stage, &psl, 1+is_tm, TRUE);
                    res = arad_pmf_psl_map(unit, &pfg_info, is_tm, &psl);
                    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
                    /* From PSL to Group */
                    res = arad_pmf_psl_to_group_map(unit, stage, &psl, &pmf_group);
                    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

                    /* Check if this is the first PFG */
                    if(is_first_pfg)
                    {
                        sal_memcpy(&(cur_line.group), &pmf_group, sizeof(ARAD_PMF_SEL_GROUP));
                        is_first_pfg = FALSE;
                    }
                    else {
                        /* Build the intersection */
                        res = ARAD_PMF_SEL_GROUP_build(&(cur_line.group), &pmf_group, &res_group);
                        SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
                        /* Copy the result */
                        sal_memcpy(&(cur_line.group), &res_group, sizeof(ARAD_PMF_SEL_GROUP));
                    }
                }

                /* Add it to previous level */
                res = arad_pmf_add_line_at_level(unit, stage, flags, &cur_line, prev_level);
                SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
            }

            /* Set the line to be removed */
            next_level->lines[curr_level_line_indx].flags |= ARAD_PMF_SEL_LINE_REMOVE;
            next_level->nof_removed_lines++;
        }

    }
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_add_group_at_level()",0,0);
}




/* sort table entries so
   in each level entries to be moved to higher level are in top i.e. lower indexes.
   */
uint32 
  arad_pmf_table_sort(
      SOC_SAND_IN  int                    unit,  
      SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  uint32                       flags
  )
{
    int32
        level_indx,
        line_indx;
    ARAD_SW_DB_PMF_PSL_LEVEL_INFO
        *curr_level = NULL;
    uint32
        entry_indx;
    uint8
        line_remove_to_copy,
        line_remove_to_copy_sorted,
        first_in_level = TRUE;
    uint32
        is_tm = (flags & ARAD_PMF_SEL_ALLOC_TM)? 1: 0,
        res;
    ARAD_PMF_SEL_INIT_INFO  
        init_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_sw_db_pmf_psl_init_info_get(
            unit,
             stage, 
            &init_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    entry_indx = ARAD_PMF_PROG_SELECT_FIRST_LINE(unit, is_tm, init_info);

    ARAD_ALLOC(curr_level, ARAD_SW_DB_PMF_PSL_LEVEL_INFO, 1, "arad_pmf_table_sort.curr_level");

    /* traverse over all levels from high to low */
    for(level_indx = ARAD_PMF_NOF_LEVELS - 1; level_indx >= 0; --level_indx) 
    {
        /* get cur level info */
        res = arad_sw_db_level_info_get(unit, stage, level_indx, is_tm, curr_level);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

        /* reset for next iteration */
        first_in_level = TRUE;
        
        /* move high index first, upper in table*/
        /* move only enties moving to higher level */
        for(line_remove_to_copy = 0 ; line_remove_to_copy <= 1 ; line_remove_to_copy++) {
            /* 
             * In case of addition, set the To-remove lines closer to 0 (move them first): 
             * - the new lines of the next level must be copy vs them in case of few lines spare 
             * For delete, by symnetry, set the to-remove lines further to 0: 
             * - the new lines of the previous level must be copy vs them in case of few lines spare 
             */
            line_remove_to_copy_sorted = (flags & ARAD_PMF_SEL_ALLOC_FOR_DELETE)? line_remove_to_copy: (!line_remove_to_copy);
            for(line_indx = curr_level->nof_lines-1 ; line_indx >= 0 ; --line_indx) 
            {
                /* 
                 * if this line not to move, skip it 
                 * First consider the lines not to be removed, after that the to-remove lines 
                 */
                if (((line_remove_to_copy_sorted == 0) && (!(curr_level->lines[line_indx].flags & ARAD_PMF_SEL_LINE_REMOVE))) 
                    || ((line_remove_to_copy_sorted == 1) && (curr_level->lines[line_indx].flags & ARAD_PMF_SEL_LINE_REMOVE))) 
                {
                    continue;
                }

                if(first_in_level) {
                    first_in_level = FALSE;
                    curr_level->last_index =  entry_indx;
                }

                
                res = arad_pmf_sel_line_hw_write(
                        unit,
                        stage,
                        entry_indx++,
                        &curr_level->lines[line_indx],
                        curr_level->lines[line_indx].prog_id,
                        TRUE
                      );
                SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
            }
        }

        /* update SW image for this level if was any change */
        if(!first_in_level) 
        {
            curr_level->first_index = entry_indx - 1;
            
            res = arad_sw_db_level_info_set(unit, stage, level_indx, is_tm, curr_level);
            SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
        }
    }

exit:
    ARAD_FREE(curr_level);
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_table_sort()", stage,0);
}


uint32
  arad_pmf_prog_select_verify_space(
      SOC_SAND_IN  int                    unit,
      SOC_SAND_IN  ARAD_FP_DATABASE_STAGE    stage,
      SOC_SAND_IN  uint32                   flags,
      SOC_SAND_IN  int32                     total_in_use,
      SOC_SAND_OUT  uint8                    *success
  )
{
    ARAD_PMF_SEL_INIT_INFO  
        init_info;
    uint32
        is_tm = (flags & ARAD_PMF_SEL_ALLOC_TM)? 1: 0,
        available_entries,
        res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_sw_db_pmf_psl_init_info_get(
            unit,
             stage, 
            &init_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if (is_tm) {    
        available_entries = init_info.nof_reserved_lines[ARAD_PMF_PSL_TYPE_TM];
    }
    else {
        available_entries = ARAD_PMF_NOF_LINES - init_info.nof_reserved_lines[ARAD_PMF_PSL_TYPE_ETH] - 1;
        /* Update if needed according to OAM Mirror WA */
        available_entries -= ARAD_PMF_OAM_MIRROR_WA_NOF_STATIC_PROG_SEL_LINES(unit);
    }

    *success = total_in_use <= available_entries;
    if ((*success) == 0) {
        LOG_DEBUG(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "    ")));
        LOG_DEBUG(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "No place in program selection at stage %s,: \n\r"), SOC_PPC_FP_DATABASE_STAGE_to_string(stage)));
        LOG_DEBUG(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "    ")));
        LOG_DEBUG(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "Available entries: %d, due to the %d reserved lines is less than "), available_entries, init_info.nof_reserved_lines[0]));
        LOG_DEBUG(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "the needed entries: %d"), total_in_use));
        LOG_DEBUG(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "\n\r")));
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_prog_select_verify_space()", 0, 0);
}

/*
 * write information to hw 
 * first move entries to top 
 * then from bottom start to add updated lines 
 */
uint32 
  arad_pmf_psl_hw_commit(
      SOC_SAND_IN  int                    unit,
      SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  uint32                       flags
  )
{
    int32
        first_line,
        level_indx,
        entry_indx,
        line_indx;
    ARAD_SW_DB_PMF_PSL_LEVEL_INFO
        *curr_level = NULL,
        *updated_level_info = NULL;
    uint32
        new_prog,
        update_level_entry_index=0;
    ARAD_PMF_SEL_INIT_INFO  
        init_info;
    uint8
        first_in_level = TRUE,
        had_entries = FALSE,
        update_level_info = FALSE;
    uint32
        is_tm = (flags & ARAD_PMF_SEL_ALLOC_TM)? 1: 0,
        res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_sw_db_pmf_psl_init_info_get(
            unit,
             stage, 
            &init_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    /* move all entries to top of table */
    res = arad_pmf_table_sort(unit, stage, flags);
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

    entry_indx = ARAD_PMF_NOF_LINES  - 1
        - (is_tm? ARAD_PMF_PROG_SELECT_TM_PMF_PGM_MIN: init_info.nof_reserved_lines[ARAD_PMF_PSL_TYPE_ETH]);

    ARAD_ALLOC(curr_level, ARAD_SW_DB_PMF_PSL_LEVEL_INFO, 1, "arad_pmf_psl_hw_commit.curr_level");
    ARAD_ALLOC(updated_level_info, ARAD_SW_DB_PMF_PSL_LEVEL_INFO, 1, "arad_pmf_psl_hw_commit.updated_level_info");
    /* traverse over all levels */
    for(level_indx = 0; level_indx < ARAD_PMF_NOF_LEVELS; ++level_indx) 
    {
        /* get cur level info */
        res = arad_sw_db_level_info_get(unit, stage, level_indx, is_tm, curr_level);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

        updated_level_info->level_index = curr_level->level_index;

        /* reset for next iteration */
        update_level_entry_index = 0;
        first_in_level = TRUE;
        had_entries = FALSE;
        update_level_info = FALSE;
        /* traverse over levels lines*/
        /* start from old lines and up */
        /* write from high to low index */
        for(line_indx = 0 ; line_indx < curr_level->nof_lines; ++line_indx) 
        {
            had_entries = TRUE;
            /* if this line to remove skip it */
            if (curr_level->lines[line_indx].flags & ARAD_PMF_SEL_LINE_REMOVE) 
            {
                continue;
            }

            sal_memcpy(&updated_level_info->lines[update_level_entry_index],&curr_level->lines[line_indx], sizeof(ARAD_SW_DB_PMF_PSL_LINE_INFO));

            if(first_in_level) {
                first_in_level = FALSE;
                updated_level_info->first_index =  entry_indx;
            }

            /* write line to HW*/
            res = arad_pmf_sel_line_hw_write(
                    unit,
                    stage,
                    entry_indx--,
                    &curr_level->lines[line_indx],
                    curr_level->lines[line_indx].prog_id,
                    TRUE
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

            ++update_level_entry_index;
        }
        
        /* write new lines */
        for(line_indx = curr_level->nof_lines ; line_indx < curr_level->nof_lines + curr_level->nof_new_lines; ++line_indx) 
        {
            /* if this line to remove skip it */
            if (curr_level->lines[line_indx].flags & ARAD_PMF_SEL_LINE_REMOVE) 
            {
                LOG_ERROR(BSL_LS_SOC_FP,
                          (BSL_META_U(unit,
                                      "Unit %d Level index %d Line index %d : Line is for remove.\n\r"),
                           unit, level_indx, line_indx));
                SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT, 40, exit);
            }

           res = arad_pmf_sel_calc_program(unit,&curr_level->lines[line_indx],&new_prog);
           SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

           sal_memcpy(&updated_level_info->lines[update_level_entry_index],&curr_level->lines[line_indx], sizeof(ARAD_SW_DB_PMF_PSL_LINE_INFO));

           updated_level_info->lines[update_level_entry_index].prog_id = new_prog;

           if(first_in_level) {
               first_in_level = FALSE;
               updated_level_info->first_index =  entry_indx;
           }

            /* write line to HW */
            res = arad_pmf_sel_line_hw_write(
                    unit,
                    stage,
                    entry_indx--,
                    &updated_level_info->lines[update_level_entry_index],
                    updated_level_info->lines[update_level_entry_index].prog_id,
                    TRUE
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

            ++update_level_entry_index;
        }

        /* update SW image for this level */
        updated_level_info->nof_new_lines = 0;
        updated_level_info->nof_removed_lines = 0;

        /* had entries but now empty */
        if(had_entries && first_in_level) 
        {
            updated_level_info->nof_lines = 0;
            updated_level_info->last_index = 0;
            updated_level_info->first_index =  0;
            update_level_info = TRUE;
        }
        else if (!first_in_level) /* level has entries */
        {
            updated_level_info->nof_lines = update_level_entry_index;
            updated_level_info->last_index = entry_indx + 1;
            update_level_info = TRUE;
        }

        if(update_level_info == TRUE) {
            
            res = arad_sw_db_level_info_set(unit, stage, level_indx, is_tm, updated_level_info);
            SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
        }
    }

    /* reset rest of entries in hadrware set valid to zero */
    first_line = ARAD_PMF_PROG_SELECT_FIRST_LINE(unit, is_tm, init_info);
    for( ; entry_indx >= first_line; entry_indx--)
    {
        res = arad_pmf_sel_line_valid_set(unit, stage, entry_indx, TRUE /* is_valid_field */, FALSE /*valid value */);
        SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
    }

exit:
    ARAD_FREE(curr_level);
    ARAD_FREE(updated_level_info);
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_psl_hw_commit()",stage,0);
}

/* 
 * Add a new PFG to the pool. 
 * Algorithm details: 
 * - loop on all the levels. 
 *      Level n means there are n databases for each line.
 * - For each level l, 
 *   - add a new line if needed, by intersection of current line
 *      with a line of Level l-1
 *   - indicate which line is not needed any more
 */
uint32 
  ARAD_PMF_SEL_GROUP_add(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  ARAD_FP_DATABASE_STAGE   stage,
      SOC_SAND_IN  uint32                   pfg_id,
      SOC_SAND_IN  uint32                   flags,
      SOC_SAND_IN  ARAD_PMF_SEL_GROUP       *group,
      SOC_SAND_OUT  uint8                   *success
  )
{
    uint32
      indx,
        is_tm = (flags & ARAD_PMF_SEL_ALLOC_TM)? 1: 0,
        res;
    ARAD_SW_DB_PMF_PSL_LEVEL_INFO
        *prev_level = NULL,
        *curr_level = NULL;
    ARAD_SW_DB_PMF_PSL_LINE_INFO
        *group_line_ptr,
        group_line;
    uint8 
        intersected = FALSE,
        lines_alloc_succes;
        /* empty = FALSE;*/
    int32
        total_in_use=0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* Translate the buffer to SW DB line */
    if (!(flags & ARAD_PMF_SEL_ALLOC_FOR_DELETE)) {
        res = ARAD_PMF_SEL_GROUP_to_line(
                unit,
                stage,
                pfg_id,
                flags,
                group,
                &group_line
            );
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }
    group_line_ptr = (!(flags & ARAD_PMF_SEL_ALLOC_FOR_DELETE))? &group_line : NULL;

    ARAD_ALLOC(prev_level, ARAD_SW_DB_PMF_PSL_LEVEL_INFO, 1, "ARAD_PMF_SEL_GROUP_add.prev_level");
    ARAD_ALLOC(curr_level, ARAD_SW_DB_PMF_PSL_LEVEL_INFO, 1, "ARAD_PMF_SEL_GROUP_add.curr_level");

    if ( flags & ARAD_PMF_SEL_ALLOC_ADVANCED_MODE ) {

       /* get current level 1 info */
        /*res = arad_sw_db_level_info_get(unit, stage, 0, is_tm, curr_level);*/
        res = arad_sw_db_level_info_get(unit, stage, 1, is_tm, curr_level);
        SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
        if (!(flags & ARAD_PMF_SEL_ALLOC_CHECK_ONLY)) {
            res = arad_pmf_add_line_at_level(unit, stage, flags, &group_line, curr_level);
            SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);
            res = arad_sw_db_level_info_set(unit, stage, 1, is_tm, curr_level);
            SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
        }
        *success = 1;
        ARAD_DO_NOTHING_AND_EXIT;
    }

    /* 
     * The levels are considered one by one from 0 up to the highest 
     * Each time, only the current level and the previous one are considered 
     * In the verification inside, higher levels may be considered from the 
     * SW DB, but are still not changed, so it is OK. 
     * For the SW DB consideration when checking only: 
     * - get the level 0 
     * - get the current level 
     * - Except for level 0, get the previous level info not from the SW DB 
     * but from the last computation  
     */
    /* get prev level info */

    res = arad_sw_db_level_info_get(unit, stage, 0, is_tm, prev_level);
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

    /* run over all levels, but loop will stop when no more intersections created  */
    for(indx = 1; indx < ARAD_PMF_NOF_LEVELS; ++indx) 
    {
        /* get current level info */
        res = arad_sw_db_level_info_get(unit, stage, indx, is_tm, curr_level);
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

        /* move removed lines from previous level to current level 
        res = arad_pmf_level_transfer_lines(unit,prev_level,curr_level,&empty);
        SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);*/

        /* add new group to current level */

        /* group_line ptr can be null only if the flag ARAD_PMF_SEL_ALLOC_FOR_DELETE is set
         * This is also verified in arad_pmf_add_group_at_level
         */
        /* coverity[var_deref_model : FALSE] */
        res = arad_pmf_add_group_at_level(unit, stage, pfg_id, flags, group_line_ptr, prev_level, curr_level, &intersected);
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

        /* not correct, code to be removed because maybe there some intersection in high levels only */
        /* group was not intersected with any group of the current level, no need to continue */
        /*if(!intersected) {
            break;
        }*/

        /* store information of next level as was update, prev level is const */
        /* set nex level info */
        if (!(flags & ARAD_PMF_SEL_ALLOC_CHECK_ONLY)) {
            res = arad_sw_db_level_info_set(unit, stage, indx, is_tm, curr_level);
            SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

            /* Update the previous level also since it can be modified */
            if (flags & ARAD_PMF_SEL_ALLOC_FOR_DELETE) {
                res = arad_sw_db_level_info_set(unit, stage, indx-1, is_tm, prev_level);
                SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);
            }
        }

        /* 
         * Copy the current level to the previous level info for next round: 
         * we use the previous image of previous level and not the current image because: 
         * 1. It is forbidden to see lines with the current PFG, otherwise the next level does
         * not have the good number of PFGs                                                                                                                                                                 :
         * 2. The deleted line are mandatory to get their intersection in the next level                                                                                                                                                                                                 :
         */
        sal_memcpy(prev_level, curr_level,sizeof(ARAD_SW_DB_PMF_PSL_LEVEL_INFO));

        
        total_in_use += curr_level->nof_lines;
        total_in_use += curr_level->nof_new_lines;
        total_in_use -= curr_level->nof_removed_lines;
        if (total_in_use < 0) {
            LOG_DEBUG(BSL_LS_SOC_FP,
                      (BSL_META_U(unit,
                                  "    ")));
            LOG_DEBUG(BSL_LS_SOC_FP,
                      (BSL_META_U(unit,
                                  "The number of needed entries at stage %s, level %d is %u: \n\r"), 
                       SOC_PPC_FP_DATABASE_STAGE_to_string(stage), indx, total_in_use));
            LOG_DEBUG(BSL_LS_SOC_FP,
                      (BSL_META_U(unit,
                                  "    ")));
            LOG_DEBUG(BSL_LS_SOC_FP,
                      (BSL_META_U(unit,
                                  "For this level, we use %d lines, we add %d lines, and we remove %d lines"), 
                       curr_level->nof_lines, curr_level->nof_new_lines, curr_level->nof_removed_lines));
            LOG_DEBUG(BSL_LS_SOC_FP,
                      (BSL_META_U(unit,
                                  "\n\r")));
        }
    }

   
    res = arad_pmf_prog_select_verify_space(unit, stage, flags, total_in_use, &lines_alloc_succes);
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    if(!lines_alloc_succes) {
        LOG_DEBUG(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "    ")));
        LOG_DEBUG(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "Program selection: no space to alloc the new line for PFG %d \n\r"), pfg_id));
        *success = 0;
        goto exit;
    }

    *success = 1;


exit:
    ARAD_FREE(prev_level);
    ARAD_FREE(curr_level);
    SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PMF_SEL_GROUP_add()",0,0);
}

/* Add this PFG to the program selection */
uint32 
  arad_pmf_psl_add(
      SOC_SAND_IN  int            unit,
      SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  uint32               pfg_id,
      SOC_SAND_IN  uint32                       flags,
      SOC_SAND_IN  ARAD_PMF_PSL       *psl,
      SOC_SAND_OUT  uint8             *success
  )
{
    ARAD_PMF_SEL_GROUP
        *pmf_group_ptr,
        pmf_group;
    uint32
        res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* Translate the table to buffer (data - mask) */
    if (!(flags & ARAD_PMF_SEL_ALLOC_FOR_DELETE)) {
        res = arad_pmf_psl_to_group_map(
                unit,
                stage,
                psl,
                &pmf_group
            );
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }
    pmf_group_ptr = (!(flags & ARAD_PMF_SEL_ALLOC_FOR_DELETE))? &pmf_group: NULL;

    /* Add a PFG (PMF SEL Group) */

    /* pmf_group_ptr will be NULL iffthe flag ARAD_PMF_SEL_ALLOC_FOR_DELETE is set.
       In this case will be no call to ARAD_PMF_SEL_GROUP_to_line */
    /* coverity[var_deref_model : FALSE] */
    res = ARAD_PMF_SEL_GROUP_add(
            unit,
            stage, 
            pfg_id,
            flags,
            pmf_group_ptr,
            success
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);



exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_psl_add()",0,0);
}

void
ARAD_PMF_PSL_clear(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_OUT ARAD_PMF_PSL *select_data,
    uint8 ethernet_or_tm_only, /* 0 - FALSE, 1 - Ethernet, 2 - TM */
    uint8 mask_all
)
{
    sal_memset(select_data, 0x0, sizeof(ARAD_PMF_PSL));
    if(mask_all)
    {
        if (stage == ARAD_FP_DATABASE_STAGE_EGRESS) {
            /* Egress PMF */
            select_data->egress_pmf.egress_pmf_profile_mask         = 7;
            select_data->egress_pmf.format_code_mask                = 7;
            select_data->egress_pmf.value_1_mask                    = 0xff;
            select_data->egress_pmf.value_2_mask                    = 0xff;
            select_data->egress_pmf.header_code_mask                = 0xf;
            select_data->egress_pmf.qualifier_mask                  = 0xf;
            select_data->egress_pmf.ethernet_tag_format_mask        = 0x1f;
        }
#ifdef BCM_88660_A0
        else if (SOC_IS_ARADPLUS(unit) && (stage == ARAD_FP_DATABASE_STAGE_INGRESS_SLB)) {
              select_data->ingress_slb.parser_leaf_context_mask     = 0xF;
              select_data->ingress_slb.vsi_profile_mask             = 0xF;
              select_data->ingress_slb.in_lif_profile_mask          = 0xF;
              select_data->ingress_slb.flp_program_mask             = 0x1F;
              select_data->ingress_slb.packet_format_code_mask      = 0x3F;
              select_data->ingress_slb.pfq_next_protocol_fwd_mask   = 0xF;
              select_data->ingress_slb.pfq_next_protocol_fwd_next_mask = 0xF;
              select_data->ingress_slb.forwarding_code_mask         = 0xF;
              select_data->ingress_slb.forwarding_offset_index_mask = 0x7;
              select_data->ingress_slb.destination_msbs_mask        = 0x3F;
              select_data->ingress_slb.unknown_addr_mask            = 0x1;
          }
#endif /* BCM_88660_A0 */

        else {
            /* Ingress PMF */
            select_data->ingress_pmf.mask_parser_leaf_context         = 0xf;
            select_data->ingress_pmf.mask_in_port_profile             = 0x7;
            select_data->ingress_pmf.mask_ptc_profile                 = 0x0; /* Field not used in Driver */
            select_data->ingress_pmf.mask_packet_format_code          = 0x3f;
            select_data->ingress_pmf.mask_packet_format_qualifier_0   = 0x7;
            select_data->ingress_pmf.mask_packet_format_qualifier_fwd = 0x7ff;
            select_data->ingress_pmf.mask_forwarding_code             = 0xf;
            select_data->ingress_pmf.mask_forwarding_offset_index     = 0x3;
            select_data->ingress_pmf.mask_forwarding_offset_index_ext = 0x3;
            select_data->ingress_pmf.mask_cpu_trap_code               = 0xff;
            select_data->ingress_pmf.mask_in_lif_profile              = 0xf;
            select_data->ingress_pmf.mask_out_lif_range               = 0x3;
            select_data->ingress_pmf.mask_llvp_incoming_tag_structure = 0xf;
            select_data->ingress_pmf.mask_vt_processing_profile       = 0x7;
            select_data->ingress_pmf.mask_vt_lookup_0_found           = 0x1;
            select_data->ingress_pmf.mask_vt_lookup_1_found           = 0x1;
            select_data->ingress_pmf.mask_tt_processing_profile       = 0x7;
            select_data->ingress_pmf.mask_tt_lookup_0_found           = 0x1;
            select_data->ingress_pmf.mask_tt_lookup_1_found           = 0x1;
            select_data->ingress_pmf.mask_fwd_prcessing_profile       = 0x7;
            select_data->ingress_pmf.mask_lem_1st_lkp_found            = 0x1;
            select_data->ingress_pmf.mask_lem_2nd_lkp_found            = 0x1;
            select_data->ingress_pmf.mask_lpm_1st_lkp_found            = 0x1;
            select_data->ingress_pmf.mask_lpm_2nd_lkp_found            = 0x1;
            select_data->ingress_pmf.mask_tcam_found                  = 0x1;
            select_data->ingress_pmf.mask_tcam_traps_found            = 0x1;
            select_data->ingress_pmf.mask_elk_error                   = 0x1;
            select_data->ingress_pmf.mask_elk_found_result            = 0xff;
#ifdef BCM_88660_A0
            /* Additional field */
            if (SOC_IS_ARADPLUS(unit) && (!(SOC_IS_ARDON(unit)))) {
              select_data->ingress_pmf.mask_forwarding_offset_index  = 0x7;
              select_data->ingress_pmf.mask_stamp_native_vsi      = 0x1;
            }
#endif /* BCM_88660_A0 */
            if (SOC_IS_JERICHO(unit)) {
                select_data->ingress_pmf.mask_in_rif_profile          = 0x3F;
                select_data->ingress_pmf.mask_tcam_found_1            = 0x1;
                select_data->ingress_pmf.mask_tcam_traps_found_1      = 0x1;
                select_data->ingress_pmf.mask_rpf_stamp_native_vsi    = 0x1;
            }
     }
    }

    if (ethernet_or_tm_only == 1) {
        /*
         * At egress, set a shared space between Ethernet and TM. 
         * To differentiate, the user can use Forwarding-Type = Any / TM 
         */
        if (stage == ARAD_FP_DATABASE_STAGE_INGRESS_PMF) {
            /* For Ethernet, their MSB is 0 */
            select_data->ingress_pmf.mask_packet_format_code          = 0x1f;
            select_data->ingress_pmf.packet_format_code               = 0x0;
        }
    }
    if (ethernet_or_tm_only == 2) {
        if (stage == ARAD_FP_DATABASE_STAGE_INGRESS_PMF) {
            /* For TM, their MSB is 1 */
            select_data->ingress_pmf.mask_packet_format_code          = 0x1f;
            select_data->ingress_pmf.packet_format_code               = ARAD_PARSER_PFC_TM;
            select_data->ingress_pmf.mask_ptc_profile                 = 0x0;
            select_data->ingress_pmf.ptc_profile                      = 0x0;
        }
    }

    if (stage == ARAD_FP_DATABASE_STAGE_EGRESS) {
        select_data->egress_pmf.valid = 0;
        select_data->egress_pmf.program = 0;
    }
#ifdef BCM_88660_A0
    else if (SOC_IS_ARADPLUS(unit) && (stage == ARAD_FP_DATABASE_STAGE_INGRESS_SLB)) {
        select_data->ingress_slb.valid     = 0;
        select_data->ingress_slb.program   = 0;
    }
#endif /* BCM_88660_A0 */
    else {
        select_data->ingress_pmf.valid = 0;
        select_data->ingress_pmf.program = 0;
    }
}

void
ARAD_PMF_SEL_INIT_INFO_clear(
    SOC_SAND_OUT ARAD_PMF_SEL_INIT_INFO *info)
{
    sal_memset(info, 0x0, sizeof(ARAD_PMF_SEL_INIT_INFO));
}


void
ARAD_PMF_PSL_reset(
    SOC_SAND_OUT ARAD_PMF_PSL *select_data
)
{
    sal_memset(select_data, 0x0, sizeof(ARAD_PMF_PSL));
}


uint32
  arad_pmf_prog_select_init(
    SOC_SAND_IN  int                 unit
  )
{
    uint32
        pgm_sel_tbl_data[SOC_DPP_DEFS_MAX(IHB_PMF_PROGRAM_SELECTION_CAM_NOF_LONGS)],
        line_ndx,
        res;
    ARAD_FP_DATABASE_STAGE
        stage;


    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* Set all the lines to be invalid - at ingress and egress */
    for(stage = 0; stage < SOC_PPC_NOF_FP_DATABASE_STAGES; stage++) {
        if ((stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP) 
            || (( SOC_IS_ARAD_B1_AND_BELOW(unit) || SOC_IS_ARDON(unit) ) && stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_SLB)) {
            /* hard-coded preselection */
            continue;
        }
        sal_memset(pgm_sel_tbl_data, 0x0, SOC_DPP_DEFS_MAX(IHB_PMF_PROGRAM_SELECTION_CAM_NOF_LONGS) * sizeof(uint32));
        soc_mem_field32_set(unit, arad_pmf_sel_line_hw_table_get(unit, stage), pgm_sel_tbl_data, VALIDf, 0x0);/* Invalid entry */
        for (line_ndx = 0; line_ndx < ARAD_PMF_NOF_LINES; line_ndx++) {
            res = soc_mem_write(
                    unit,
                    arad_pmf_sel_line_hw_table_get(unit, stage),
                    MEM_BLOCK_ANY,
                    line_ndx,
                    pgm_sel_tbl_data
                  );
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
        }
    }

 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_prog_select_init()", 0, 0);
}

/* 
 * The TM and PP applications are divided in 2 distinct spaces of 
 * PMF-Programs: simpler for dynamic allocation, when doing 
 * a loop 
 */
uint32
  arad_pmf_prog_select_pmf_pgm_borders_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint8                    is_for_tm, 
    SOC_SAND_OUT  uint32  *pmf_pgm_ndx_min,
    SOC_SAND_OUT  uint32  *pmf_pgm_ndx_max
  )
{
    uint32
        res;
    ARAD_PMF_SEL_INIT_INFO  
        init_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* Special case for FLP stage */
    if (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP) {
        *pmf_pgm_ndx_min = 0;
        *pmf_pgm_ndx_max = SOC_DPP_DEFS_GET(unit, nof_flp_programs);
        ARAD_DO_NOTHING_AND_EXIT;
    }

    res = arad_sw_db_pmf_psl_init_info_get(unit, stage, &init_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    *pmf_pgm_ndx_min = is_for_tm? ARAD_PMF_PROG_SELECT_TM_PMF_PGM_MIN: init_info.nof_reserved_lines[is_for_tm];
    *pmf_pgm_ndx_max = is_for_tm? (init_info.nof_reserved_lines[ARAD_PMF_PSL_TYPE_TM] + ARAD_PMF_PROG_SELECT_TM_PMF_PGM_MIN): ARAD_PMF_LOW_LEVEL_NOF_PROGS;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_prog_select_pmf_pgm_borders_get()", 0, 0);
}

uint32
  arad_pmf_prog_select_line_borders_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint8                    is_for_tm, 
    SOC_SAND_OUT  uint32  *line_ndx_min,
    SOC_SAND_OUT  uint32  *line_ndx_max
  )
{
    uint32
        res;
    ARAD_PMF_SEL_INIT_INFO  
        init_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* Special case for FLP stage */
    if (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP) {
        ARAD_DO_NOTHING_AND_EXIT;
    }

    res = arad_sw_db_pmf_psl_init_info_get(unit, stage, &init_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    *line_ndx_min = ARAD_PMF_PROG_SELECT_FIRST_LINE(unit, is_for_tm, init_info);
    *line_ndx_max = is_for_tm? ARAD_PMF_NOF_LINES : ARAD_PMF_NOF_LINES - init_info.nof_reserved_lines[ARAD_PMF_PSL_TYPE_ETH] - 2;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_prog_select_pmf_pgm_borders_get()", 0, 0);
}

uint32
  arad_pmf_prog_select_eth_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint8                      is_tm_pmf_per_port_mode,
    SOC_SAND_INOUT  ARAD_PMF_SEL_INIT_INFO  *init_info
  )
{
    ARAD_PMF_PSL
        cam_data;
    ARAD_SW_DB_PMF_PSL_LEVEL_INFO
        *dflt_level_info = NULL,
        *level_1_info = NULL;
    uint8
        is_tm,
        found;
    uint32
        res, nof_eth_presel;
#ifdef ARAD_PMF_OAM_MIRROR_WA_ENABLED
  ARAD_PMF_PSL         
      psl;
  ARAD_SW_DB_PMF_PSL_LINE_INFO       
      cur_line;
  uint32
      pmf_pgm_ndx;
#endif /* ARAD_PMF_OAM_MIRROR_WA_ENABLED */

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_sw_db_pmf_psl_init_info_set(unit, stage, init_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    /*
    res = arad_pmf_psl_add(unit, ARAD_FP_DATABASE_STAGE_INGRESS_PMF,  10, &cam_data);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit); 
    */
    ARAD_ALLOC(dflt_level_info, ARAD_SW_DB_PMF_PSL_LEVEL_INFO, 1, "arad_pmf_prog_select_eth_init.dflt_level_info");
    ARAD_ALLOC(level_1_info, ARAD_SW_DB_PMF_PSL_LEVEL_INFO, 1, "arad_pmf_prog_select_eth_init.level_1_info");

    for (is_tm = FALSE; is_tm <= (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF); is_tm++) {
        ARAD_PMF_PSL_clear(unit, stage, &cam_data, 1+is_tm, TRUE);

        sal_memset(dflt_level_info, 0x0, sizeof(ARAD_SW_DB_PMF_PSL_LEVEL_INFO)); /* Init the structure */
        res = arad_pmf_psl_to_group_map(unit, stage, &cam_data, &dflt_level_info->lines[0].group);
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

        res = arad_pmf_sel_prog_alloc(unit, stage, is_tm, &dflt_level_info->lines[0].prog_id, &found);
        SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
        if(!found) {
            SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT, 55, exit);
        }

        sal_memset(dflt_level_info->lines[0].groups, 0x0, sizeof(uint32) * ARAD_PMF_GROUP_LEN); /* no group here */
        dflt_level_info->nof_lines = 1;
        dflt_level_info->nof_new_lines = 0;
        dflt_level_info->nof_removed_lines = 0;
        dflt_level_info->level_index = 0;
        dflt_level_info->first_index = ARAD_PMF_NOF_LINES - dflt_level_info->lines[0].prog_id - 1;
        dflt_level_info->last_index = dflt_level_info->first_index;
        dflt_level_info->lines[0].flags = ARAD_PMF_SEL_LINE_VALID;
        
        
        res = arad_sw_db_level_info_set(unit, stage, 0, is_tm, dflt_level_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

        /* write to Hw */
        res = arad_pmf_sel_line_hw_write(
                unit,
                stage,
                dflt_level_info->first_index,
                &dflt_level_info->lines[0],
                dflt_level_info->lines[0].prog_id,
                TRUE
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

        /* write system headers for ethernet program  - only at ingress */
        if (stage == ARAD_FP_DATABASE_STAGE_INGRESS_PMF) {
            res = arad_pmf_low_level_pgm_set(
                     unit,
                     dflt_level_info->lines[0].prog_id,
                     (is_tm? ARAD_PMF_PGM_TYPE_TM: ARAD_PMF_PGM_TYPE_ETH)
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
        }
        if (!is_tm) {
            if (soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
                SOC_SAND_CHECK_FUNC_RESULT(arad_pmf_prog_select_eth_section_nof_lines_get(unit,stage, &nof_eth_presel), 25, exit);
                level_1_info->nof_lines = nof_eth_presel;
                level_1_info->nof_new_lines = 0;
                level_1_info->nof_removed_lines = 0;
                level_1_info->level_index = 1;
                level_1_info->first_index = dflt_level_info->last_index - 1;
                level_1_info->last_index = level_1_info->first_index - nof_eth_presel +1; 
                
                res = arad_sw_db_level_info_set(unit, stage, 1, is_tm, level_1_info);
                SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
            }
        }
        
    }
    

#ifdef ARAD_PMF_OAM_MIRROR_WA_ENABLED
  /* 
   * Set the 4 high lines to be for OAM WA: 
   * line 0-3 is for FTMH 
   */
  if (soc_property_get((unit), spn_NUM_OAMP_PORTS, 0) && (stage == ARAD_FP_DATABASE_STAGE_INGRESS_PMF)) {
	  uint32 
          prog_selection_line_indx;
      ARAD_PP_TRAP_CODE_INTERNAL
        trap_code_internal;
      ARAD_SOC_REG_FIELD
        strength_fld_fwd,
        strength_fld_snp;
      uint8
          is_oam_wa_snoop_enabled = 0;
      ARAD_PP_TRAP_CODE                           
          trap_code_ndx;

#ifdef ARAD_PMF_OAM_MIRROR_WA_SNOOP_OUT_LIF_IN_DSP_EXT_ENABLED
      /* Enable the number of */
      if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "egress_snooping_advanced", 0)) {
          is_oam_wa_snoop_enabled = 1;
      }
#endif /* ARAD_PMF_OAM_MIRROR_WA_SNOOP_OUT_LIF_IN_DSP_EXT_ENABLED */

      for (prog_selection_line_indx = 0; prog_selection_line_indx < (SOC_PPC_TRAP_CODE_OAM_FTMH_MIRROR_TRAP_CODES_NUM + is_oam_wa_snoop_enabled); prog_selection_line_indx++) {
          /* Translate the PFG info to the table */
          ARAD_PMF_PSL_clear(unit, stage, &psl, FALSE, TRUE);
          /* From PSL to Group */
          ARAD_PMF_LINE_INFO_clear(&cur_line);
          /* Set the correct trap-code: translate from SW to internal */
          trap_code_ndx = (prog_selection_line_indx == SOC_PPC_TRAP_CODE_OAM_FTMH_MIRROR_TRAP_CODES_NUM)? 
              SOC_PPC_TRAP_CODE_OAM_FTMH_MIRROR_TRAP_CODES_MIP_EGRESS_SNOOP /* snoop WA trap */: (SOC_PPC_TRAP_CODE_OAM_FTMH_MIRROR_TRAP_CODES_FIRST + prog_selection_line_indx);
          res = arad_pp_trap_mgmt_trap_code_to_internal(
                  trap_code_ndx, /* the SW trap codes must be consecutive */
                  &trap_code_internal,
                  &strength_fld_fwd,
                  &strength_fld_snp
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
          psl.ingress_pmf.cpu_trap_code = trap_code_internal; /* HW code */
          psl.ingress_pmf.mask_cpu_trap_code = 0; /* All counts */
          res = arad_pmf_psl_to_group_map(unit, stage, &psl, &(cur_line.group));
          SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

          pmf_pgm_ndx = ARAD_PMF_PROGRAM_STATIC_INDEX_GET(init_info->nof_reserved_lines[ARAD_PMF_PSL_TYPE_TM], 
                                                ARAD_PMF_PROG_SELECT_TM_PMF_PGM_MIN, ARAD_PMF_PGM_TYPE_STACK);
#ifdef ARAD_PMF_OAM_MIRROR_WA_SNOOP_OUT_LIF_IN_DSP_EXT_ENABLED
          if (prog_selection_line_indx == SOC_PPC_TRAP_CODE_OAM_FTMH_MIRROR_TRAP_CODES_NUM) {
              /* Change the PMF program to go to OAM snoop Walk-around, FTMH-like */
              pmf_pgm_ndx = ARAD_PMF_PROGRAM_STATIC_INDEX_GET(init_info->nof_reserved_lines[ARAD_PMF_PSL_TYPE_TM], 
                                                    ARAD_PMF_PROG_SELECT_TM_PMF_PGM_MIN, ARAD_PMF_PGM_TYPE_TM);
          }
#endif /* ARAD_PMF_OAM_MIRROR_WA_SNOOP_OUT_LIF_IN_DSP_EXT_ENABLED */
          res = arad_pmf_sel_line_hw_write(
                  unit,
                  stage,
                  prog_selection_line_indx, /* line 0-3 */ 
                  &cur_line,
                  pmf_pgm_ndx,
                  TRUE
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
      }
  }
#endif /* ARAD_PMF_OAM_MIRROR_WA_ENABLED */

    
exit:
  ARAD_FREE(dflt_level_info);
  ARAD_FREE(level_1_info);
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_prog_select_eth_init()", 0, 0);
}

/* Match the PPD qualifiers to the table entries */
uint32 
  arad_pmf_psl_map(
      SOC_SAND_IN  int               unit,
      SOC_SAND_IN  SOC_TMC_PMF_PFG_INFO *pfg_info,
      SOC_SAND_IN  uint8                    is_for_tm, 
      SOC_SAND_OUT ARAD_PMF_PSL         *psl
  )
{
    uint32
        fld_val,
        lif_profile_offset,
        qual_ndx;
    uint8
        is_tm = is_for_tm;
    

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_PMF_PSL_clear(unit, pfg_info->stage, psl, 1 + is_tm, TRUE /* mask_all */);

    for (qual_ndx = 0; qual_ndx < SOC_PPC_FP_NOF_QUALS_PER_PFG_MAX; qual_ndx++) {
        if ((pfg_info->qual_vals[qual_ndx].type == SOC_PPC_FP_QUAL_IRPP_INVALID)
            || (pfg_info->qual_vals[qual_ndx].type == SOC_PPC_NOF_FP_QUAL_TYPES)) {
            continue;
        }

        if (pfg_info->stage == ARAD_FP_DATABASE_STAGE_INGRESS_PMF) {
            /* Ingress stage */
            switch (pfg_info->qual_vals[qual_ndx].type) {
            case SOC_PPC_FP_QUAL_IRPP_PROCESSING_TYPE:
                psl->ingress_pmf.parser_leaf_context = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->ingress_pmf.mask_parser_leaf_context = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0xF;
                break;
            case SOC_PPC_FP_QUAL_IRPP_IN_PORT_KEY_GEN_VAR_PS:
                psl->ingress_pmf.in_port_profile = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->ingress_pmf.mask_in_port_profile = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x7;
                break;
            case SOC_PPC_FP_QUAL_IRPP_PKT_HDR_TYPE:
                psl->ingress_pmf.packet_format_code = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->ingress_pmf.mask_packet_format_code = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x3F;
                break;
            case SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER0:
                psl->ingress_pmf.packet_format_qualifier_0 = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->ingress_pmf.mask_packet_format_qualifier_0 = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x7;
                break;
            /*
             * List of the Packet-Format-qualifier for Forwarding header: 
             * each PPC qualifier is supported but:
             * - the user must also use the good forwarding-code 
             * - only the relevant bits of the psl->ingress_pmf.packet_format_qualifier_fwd are changed 
             */
            case SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_ENCAPSULATION:
                SHR_BITCOPY_RANGE(&(psl->ingress_pmf.packet_format_qualifier_fwd), 5, &(pfg_info->qual_vals[qual_ndx].val.arr[0]), 0, 2);
                fld_val = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x3;
                SHR_BITCOPY_RANGE(&(psl->ingress_pmf.mask_packet_format_qualifier_fwd), 5, &fld_val, 0, 2);
                break;
            case SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_NEXT_PROTOCOL:
                SHR_BITCOPY_RANGE(&(psl->ingress_pmf.packet_format_qualifier_fwd), 7, &(pfg_info->qual_vals[qual_ndx].val.arr[0]), 0, 4);
                fld_val = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0xF;
                SHR_BITCOPY_RANGE(&(psl->ingress_pmf.mask_packet_format_qualifier_fwd), 7, &fld_val, 0, 4);
                break;
            case SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_NEXT_PROTOCOL:
                SHR_BITCOPY_RANGE(&(psl->ingress_pmf.packet_format_qualifier_fwd), 7, &(pfg_info->qual_vals[qual_ndx].val.arr[0]), 0, 4);
                fld_val = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0xF;
                SHR_BITCOPY_RANGE(&(psl->ingress_pmf.mask_packet_format_qualifier_fwd), 7, &fld_val, 0, 4);
                break;
            case SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_IP_FRAGMENTED:
            case SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER3_IP_FRAGMENTED:
                SHR_BITCOPY_RANGE(&(psl->ingress_pmf.packet_format_qualifier_fwd), 6, &(pfg_info->qual_vals[qual_ndx].val.arr[0]), 0, 1);
                fld_val = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x1;
                SHR_BITCOPY_RANGE(&(psl->ingress_pmf.mask_packet_format_qualifier_fwd), 6, &fld_val, 0, 1);
                break;
            case SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_IP_HAS_OPTIONS:
                SHR_BITCOPY_RANGE(&(psl->ingress_pmf.packet_format_qualifier_fwd), 4, &(pfg_info->qual_vals[qual_ndx].val.arr[0]), 0, 1);
                fld_val = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x1;
                SHR_BITCOPY_RANGE(&(psl->ingress_pmf.mask_packet_format_qualifier_fwd), 4, &fld_val, 0, 1);
                break;
            case SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_BOS:
                SHR_BITCOPY_RANGE(&(psl->ingress_pmf.packet_format_qualifier_fwd), 0, &(pfg_info->qual_vals[qual_ndx].val.arr[0]), 0, 1);
                fld_val = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x1;
                SHR_BITCOPY_RANGE(&(psl->ingress_pmf.mask_packet_format_qualifier_fwd), 0, &fld_val, 0, 1);
                break;

            case SOC_PPC_FP_QUAL_IRPP_FWD_TYPE:
                psl->ingress_pmf.forwarding_code = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->ingress_pmf.mask_forwarding_code = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0xF;
                break;
            case SOC_PPC_FP_QUAL_IRPP_SUB_HEADER_NDX:
                /* Different mask size in Arad: bits 1:0 and Arad+ all bits 2:0 */
                fld_val = 0x3;
#ifdef BCM_88660_A0
                  if (SOC_IS_ARADPLUS(unit)) {
                      /* Arad+ value */
                      fld_val = 0x7;
                  }
#endif /* BCM_88660_A0 */
                psl->ingress_pmf.forwarding_offset_index = pfg_info->qual_vals[qual_ndx].val.arr[0] & fld_val;
                psl->ingress_pmf.mask_forwarding_offset_index = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & fld_val;
                break;
            case SOC_PPC_FP_QUAL_FORWARDING_OFFSET_EXTENSION:
                psl->ingress_pmf.forwarding_offset_index_ext = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->ingress_pmf.mask_forwarding_offset_index_ext = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x3;
                break;
            case SOC_PPC_FP_QUAL_IRPP_FWD_DEC_CPU_TRAP_CODE:
                psl->ingress_pmf.cpu_trap_code = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->ingress_pmf.mask_cpu_trap_code = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0xFF;
                break;
            case SOC_PPC_FP_QUAL_IN_LIF_PROFILE:
                lif_profile_offset = 0;

                
                SHR_BITCOPY_RANGE(&(psl->ingress_pmf.in_lif_profile), lif_profile_offset, &(pfg_info->qual_vals[qual_ndx].val.arr[0]), 0, (4-lif_profile_offset));
                fld_val = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & ((1 << (4-lif_profile_offset)) - 1);
                SHR_BITCOPY_RANGE(&(psl->ingress_pmf.mask_in_lif_profile), lif_profile_offset, &fld_val, 0, (4-lif_profile_offset));
                break;
            case SOC_PPC_FP_QUAL_IN_LIF_PROFILE_SAME_IF:
                
                if (soc_property_get(unit, spn_BCM886XX_LOGICAL_INTERFACE_BRIDGE_FILTER_ENABLE, 0)) { 
                    /* Only 3MSBs of LIF-profile in this case, the LSB is used by same-interface feature */
                    SHR_BITCOPY_RANGE(&(psl->ingress_pmf.in_lif_profile), 0, &(pfg_info->qual_vals[qual_ndx].val.arr[0]), 0, 1);
                    fld_val = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x1;
                    SHR_BITCOPY_RANGE(&(psl->ingress_pmf.mask_in_lif_profile), 0, &fld_val, 0, 1);
                }
                break;
            case SOC_PPC_FP_QUAL_IRPP_ETH_TAG_FORMAT:
                psl->ingress_pmf.llvp_incoming_tag_structure = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->ingress_pmf.mask_llvp_incoming_tag_structure = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0xF;
                break;
            case SOC_PPC_FP_QUAL_VT_PROCESSING_PROFILE:
                psl->ingress_pmf.vt_processing_profile = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->ingress_pmf.mask_vt_processing_profile = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x7;
                break;
            case SOC_PPC_FP_QUAL_VT_LOOKUP0_FOUND:
                psl->ingress_pmf.vt_lookup_0_found = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->ingress_pmf.mask_vt_lookup_0_found = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x1;
                break;
            case SOC_PPC_FP_QUAL_VT_LOOKUP1_FOUND:
                psl->ingress_pmf.vt_lookup_1_found = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->ingress_pmf.mask_vt_lookup_1_found = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x1;
                break;
            case SOC_PPC_FP_QUAL_TT_PROCESSING_PROFILE:
                psl->ingress_pmf.tt_processing_profile = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->ingress_pmf.mask_tt_processing_profile = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x7;
                break;
            case SOC_PPC_FP_QUAL_TT_LOOKUP0_FOUND:
                psl->ingress_pmf.tt_lookup_0_found = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->ingress_pmf.mask_tt_lookup_0_found = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x1;
                break;
            case SOC_PPC_FP_QUAL_TT_LOOKUP1_FOUND:
                psl->ingress_pmf.tt_lookup_1_found = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->ingress_pmf.mask_tt_lookup_1_found = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x1;
                break;
            case SOC_PPC_FP_QUAL_FWD_PRCESSING_PROFILE:
                psl->ingress_pmf.fwd_prcessing_profile = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->ingress_pmf.mask_fwd_prcessing_profile = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x3;
                break;
            case SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_FOUND:
                psl->ingress_pmf.lem_1st_lkp_found = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->ingress_pmf.mask_lem_1st_lkp_found = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x1;
                break;
            case SOC_PPC_FP_QUAL_LEM_2ND_LOOKUP_FOUND:
                psl->ingress_pmf.lem_2nd_lkp_found = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->ingress_pmf.mask_lem_2nd_lkp_found = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x1;
                break;
            case SOC_PPC_FP_QUAL_LPM_1ST_LOOKUP_FOUND:
                psl->ingress_pmf.lpm_1st_lkp_found = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->ingress_pmf.mask_lpm_1st_lkp_found = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x1;
                break;
            case SOC_PPC_FP_QUAL_LPM_2ND_LOOKUP_FOUND:
                psl->ingress_pmf.lpm_2nd_lkp_found = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->ingress_pmf.mask_lpm_2nd_lkp_found = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x1;
                break;
            case SOC_PPC_FP_QUAL_TCAM_MATCH:
                psl->ingress_pmf.tcam_found = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->ingress_pmf.mask_tcam_found = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x1;
                break;
            case SOC_PPC_FP_QUAL_TCAM_TRAPS0_MATCH:
                psl->ingress_pmf.tcam_traps_found = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->ingress_pmf.mask_tcam_traps_found = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x1;
                break;
            case SOC_PPC_FP_QUAL_ELK_ERROR:
                psl->ingress_pmf.elk_error = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->ingress_pmf.mask_elk_error = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x1;
                break;
            case SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_0:
            case SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_1:
            case SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_2:
            case SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_3:
                SHR_BITCOPY_RANGE(&(psl->ingress_pmf.elk_found_result), 7 - (pfg_info->qual_vals[qual_ndx].type - SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_0), 
                                    &(pfg_info->qual_vals[qual_ndx].val.arr[0]), 0, 1);
                fld_val = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x1;
                SHR_BITCOPY_RANGE(&(psl->ingress_pmf.mask_elk_found_result), 7 - (pfg_info->qual_vals[qual_ndx].type - SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_0), 
                                    &fld_val, 0, 1);
                break;
            case SOC_PPC_FP_QUAL_HEADER_OFFSET_0_UNTIL_5:
                /* Not supported intentionnally */
                break;
            /* Set this case as last one */
            case SOC_PPC_FP_QUAL_STAMP_NATIVE_VSI:
#ifdef BCM_88660_A0
                  if (SOC_IS_ARADPLUS(unit)) {
                      /* Arad+ value */
                      psl->ingress_pmf.stamp_native_vsi = pfg_info->qual_vals[qual_ndx].val.arr[0];
                      psl->ingress_pmf.mask_stamp_native_vsi = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x1;
                      break;
                  }
#endif /* BCM_88660_A0 */
                  /* No break intentionnally for this case */
            default:
                LOG_ERROR(BSL_LS_SOC_FP,
                          (BSL_META_U(unit,
                                      "Unit %d Stage Ingress Qualifier %d - Invalid qualifier.\n\r"),
                           unit, pfg_info->qual_vals[qual_ndx].type));
                SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT, 13, exit);
            }
        }
#ifdef BCM_88660_A0
        else if (SOC_IS_ARADPLUS(unit) && (pfg_info->stage == ARAD_FP_DATABASE_STAGE_INGRESS_SLB)) {
            /* Ingress stage */
            switch (pfg_info->qual_vals[qual_ndx].type) {
            case SOC_PPC_FP_QUAL_IRPP_PROCESSING_TYPE:
                psl->ingress_slb.parser_leaf_context = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->ingress_slb.parser_leaf_context_mask = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0xF;
                break;
            case SOC_PPC_FP_QUAL_IRPP_PKT_HDR_TYPE:
                psl->ingress_slb.packet_format_code = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->ingress_slb.packet_format_code_mask = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x3F;
                break;
            case SOC_PPC_FP_QUAL_IRPP_FWD_TYPE:
                psl->ingress_slb.forwarding_code = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->ingress_slb.forwarding_code_mask = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0xF;
                break;
            case SOC_PPC_FP_QUAL_IRPP_SUB_HEADER_NDX:
                fld_val = 0x7;
                psl->ingress_slb.forwarding_offset_index = pfg_info->qual_vals[qual_ndx].val.arr[0] & fld_val;
                psl->ingress_slb.forwarding_offset_index_mask = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & fld_val;
                break;
            case SOC_PPC_FP_QUAL_IN_LIF_PROFILE:
                lif_profile_offset = 0;
                SHR_BITCOPY_RANGE(&(psl->ingress_slb.in_lif_profile), lif_profile_offset, &(pfg_info->qual_vals[qual_ndx].val.arr[0]), 0, (4-lif_profile_offset));
                fld_val = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & ((1 << (4-lif_profile_offset)) - 1);
                SHR_BITCOPY_RANGE(&(psl->ingress_slb.in_lif_profile_mask), lif_profile_offset, &fld_val, 0, (4-lif_profile_offset));
                break;
            case SOC_PPC_FP_QUAL_IN_LIF_PROFILE_SAME_IF:
                
                if (soc_property_get(unit, spn_BCM886XX_LOGICAL_INTERFACE_BRIDGE_FILTER_ENABLE, 0)) { 
                    /* Only 3MSBs of LIF-profile in this case, the LSB is used by same-interface feature */
                    SHR_BITCOPY_RANGE(&(psl->ingress_slb.in_lif_profile), 0, &(pfg_info->qual_vals[qual_ndx].val.arr[0]), 0, 1);
                    fld_val = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x1;
                    SHR_BITCOPY_RANGE(&(psl->ingress_slb.in_lif_profile_mask), 0, &fld_val, 0, 1);
                }
                break;
            case SOC_PPC_FP_QUAL_VSI_PROFILE:
                psl->ingress_slb.vsi_profile = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->ingress_slb.vsi_profile_mask = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0xF;
                break;
            case SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_NEXT_PROTOCOL:
                psl->ingress_slb.pfq_next_protocol_fwd = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->ingress_slb.pfq_next_protocol_fwd_mask = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0xF;
                break;
            case SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_NEXT_PROTOCOL:
                psl->ingress_slb.pfq_next_protocol_fwd_next = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->ingress_slb.pfq_next_protocol_fwd_next_mask = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0xF;
                break;
            case SOC_PPC_FP_QUAL_UNKNOWN_ADDR:
                psl->ingress_slb.unknown_addr = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->ingress_slb.unknown_addr_mask = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x1;
                break;
            case SOC_PPC_FP_QUAL_HEADER_OFFSET_0_UNTIL_5:
                /* Not supported intentionnally */
                break;
            default:
                LOG_ERROR(BSL_LS_SOC_FP,
                          (BSL_META_U(unit,
                                      "Unit %d Stage Ingress Qualifier %d - Invalid qualifier.\n\r"),
                           unit, pfg_info->qual_vals[qual_ndx].type));
                SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT, 13, exit);
            }
        }
#endif /* BCM_88660_A0 */
        else if (pfg_info->stage == ARAD_FP_DATABASE_STAGE_EGRESS) {
            /* Egress stage */
            switch (pfg_info->qual_vals[qual_ndx].type) {
            case SOC_PPC_FP_QUAL_ERPP_OUT_PP_PORT_PMF_DATA_PS:
                psl->egress_pmf.egress_pmf_profile = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->egress_pmf.egress_pmf_profile_mask = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x7;
                break;
            case SOC_PPC_FP_QUAL_ERPP_FORMAT_CODE:
                psl->egress_pmf.format_code = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->egress_pmf.format_code_mask = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x7;
                break;
            case SOC_PPC_FP_QUAL_ERPP_SYS_VALUE1:
                psl->egress_pmf.value_1 = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->egress_pmf.value_1_mask = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0xFF;
                break;
            case SOC_PPC_FP_QUAL_ERPP_SYS_VALUE2:
                psl->egress_pmf.value_2 = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->egress_pmf.value_2_mask = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0xFF;
                break;
            case SOC_PPC_FP_QUAL_ERPP_HEADER_CODE: /* similar signals, this one is used at BCM level */
            case SOC_PPC_FP_QUAL_ERPP_FWD_CODE: 
                psl->egress_pmf.header_code = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->egress_pmf.header_code_mask = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0xF;
                break;
            case SOC_PPC_FP_QUAL_ERPP_ETH_TAG_FORMAT:
                psl->egress_pmf.ethernet_tag_format = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->egress_pmf.ethernet_tag_format_mask = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x1F;
                break;
            default:
                LOG_ERROR(BSL_LS_SOC_FP,
                          (BSL_META_U(unit,
                                      "Unit %d Stage Egress Qualifier %d - Invalid qualifier.\n\r"),
                           unit, pfg_info->qual_vals[qual_ndx].type));
                SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT, 17, exit);
            }
        }
        else {
            /* Unknown stage */
            SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT, 20, exit);
        }
    }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_psl_map()", 0, 0);
}

uint32
  arad_pmf_psl_pmf_pgms_get(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  ARAD_FP_DATABASE_STAGE   stage,
      SOC_SAND_IN  uint32                   groups[ARAD_PMF_GROUP_LEN],
      SOC_SAND_IN  uint8                    is_default_db, /* Relevant for all lines */
      SOC_SAND_IN  uint8                    is_for_tm, 
      SOC_SAND_OUT uint32                  *pgm_bmp_used,
      SOC_SAND_OUT uint32                  *pgm_bmp_remain, /* List of PMF-Programs not to suppress */
      SOC_SAND_OUT uint32                   presel_bmp_update[ARAD_PMF_NOF_LINES_MAX_ALL_STAGES_LEN], /* Bitmap of the Presel lines to update */
      SOC_SAND_OUT uint8                   *success
  )
{
    uint32
        pfg_ndx,
        qual_ndx,
        pmf_pgm_ndx,
        pgm_bmp_not_duplicate[1],
        nof_pmf_pgms_free,
        nof_pmf_pgms_needed,
        program_rsrc = 0,
        groups_lcl[ARAD_PMF_GROUP_LEN],
        is_tm = is_for_tm, 
        res;
    int32
        level_indx,
        line_indx;
    ARAD_SW_DB_PMF_PSL_LEVEL_INFO
        curr_level;
    ARAD_PP_FP_PFG_INFO
        pfg_info_current;
    ARAD_PP_FP_CONTROL_INDEX       
        control_ndx;
    ARAD_PP_FP_CONTROL_INFO        
        control_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /*
     * Special case for FLP: 
     * - TM is not accepted 
     * - If default DB, return all the FLP programs 
     * - Otherwise, for each valid PFG: 
     *  - Get its info from the SW DB
     *  - Get its FLP processing profile
     *  - Retrieve the list of FLP programs and add to the list
     */
    if (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP) {
        if (is_for_tm) {
            /* No meaning for TM in FLP context */
            SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT, 3, exit);
        }
        else if (is_default_db) {
            *pgm_bmp_used = (1 << SOC_DPP_DEFS_GET(unit, nof_flp_programs)) - 1;
            *pgm_bmp_remain = 0;
            *success = TRUE;
            ARAD_DO_NOTHING_AND_EXIT;
        }
        else {
            *pgm_bmp_used = 0;
            *pgm_bmp_remain = 0;
            *success = TRUE;
            sal_memcpy(groups_lcl, groups, sizeof(groups_lcl));
            for (pfg_ndx = 0; pfg_ndx < ARAD_PP_FP_NOF_PFGS; pfg_ndx++) {
                if (SHR_BITGET(groups_lcl, pfg_ndx)) {
                    /* 
                     * PFG is used, retrieve its information 
                     * Verification of the stage and PFG exists in the database create verify 
                     */ 
                    res = arad_pp_fp_packet_format_group_get_unsafe(
                              unit,
                              pfg_ndx,
                              &pfg_info_current
                            );
                    SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);

                    /* Find the FLP processing profile, and add these programs to program used */
                    for (qual_ndx = 0; qual_ndx < SOC_PPC_FP_NOF_QUALS_PER_PFG_MAX; qual_ndx++) {
                        if (pfg_info_current.qual_vals[qual_ndx].type == SOC_PPC_FP_QUAL_FWD_PRCESSING_PROFILE) {
                            /* Get the list of FLP programs */
                            ARAD_PP_FP_CONTROL_INDEX_clear(&control_ndx);
                            control_ndx.type = SOC_PPC_FP_CONTROL_TYPE_FLP_PGM_PROFILE;
                            control_ndx.val_ndx = pfg_info_current.qual_vals[qual_ndx].val.arr[0];
                            res = arad_pp_fp_control_get(unit, &control_ndx, &control_info);
                            SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);
                            *pgm_bmp_used |= control_info.val[0];
                        }
                    }
                }
            }
        }

        ARAD_DO_NOTHING_AND_EXIT;
    }

    sal_memset(presel_bmp_update, 0x0, ARAD_PMF_NOF_LINES_MAX_ALL_STAGES_LEN * sizeof(uint32));

    /* Compute the number of free PMF-Programs */
    res = arad_sw_db_pgm_program_rsrc_get(unit, stage, &program_rsrc);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
    nof_pmf_pgms_free = 0;
    for (pmf_pgm_ndx = 0; pmf_pgm_ndx < ARAD_PMF_LOW_LEVEL_NOF_PROGS; pmf_pgm_ndx++) {
        if (!SHR_BITGET(&program_rsrc, pmf_pgm_ndx)) {
            nof_pmf_pgms_free++;
        }
    }

    *pgm_bmp_used = 0;
    *pgm_bmp_not_duplicate = 0;
    /* traverse over all levels */
    for(level_indx = 0; level_indx < ARAD_PMF_NOF_LEVELS; ++level_indx) 
    {
        /* get cur level info */
        res = arad_sw_db_level_info_get(unit, stage, level_indx, is_tm, &curr_level);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

        /* traverse over levels lines*/
        for(line_indx = 0 ; line_indx < curr_level.nof_lines; ++line_indx) 
        {
            if ( (soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) && (!(curr_level.lines[line_indx].flags & ARAD_PMF_SEL_LINE_VALID )) ) {
                continue;
            }
           
            sal_memcpy(groups_lcl, groups, sizeof(groups_lcl));
            soc_sand_bitstream_and(groups_lcl, curr_level.lines[line_indx].groups, ARAD_PMF_GROUP_LEN);
            /* Check if there is an intersection - always with Default DB */
            if (groups_lcl[0] || groups_lcl[1] || is_default_db) 
            {
                /* The DB participates to this line */
                *pgm_bmp_used |= (1 << curr_level.lines[line_indx].prog_id);
                /* Set this HW line to be updated */
                SHR_BITSET(presel_bmp_update, (curr_level.first_index - line_indx));
            }
            else {
                /* The DB does not participate to this line */
                /* 
                 * Check if needed 2 PMF-Programs: 
                 * - only if the current PMF-Program is also used by another Presel line 
                 * with no intersection with these PFGs 
                 */
                *pgm_bmp_not_duplicate |= (1 << curr_level.lines[line_indx].prog_id);
            }

        }
    }

    /* Need to duplicate only if present in both bitmaps - otherwise the program is replaced or not touched */
    *pgm_bmp_remain = *pgm_bmp_not_duplicate;
    *pgm_bmp_not_duplicate &= (*pgm_bmp_used);
    nof_pmf_pgms_needed = 1; /* For any copy, needed for the transitory time */
    for (pmf_pgm_ndx = 0; pmf_pgm_ndx < ARAD_PMF_LOW_LEVEL_NOF_PROGS; pmf_pgm_ndx++) {
        if (SHR_BITGET(pgm_bmp_not_duplicate, pmf_pgm_ndx)) {
            nof_pmf_pgms_needed++;
        }
    }
    *success = (nof_pmf_pgms_needed <= nof_pmf_pgms_free)? TRUE: FALSE;

    /* Verify all these programs are really in use - otherwise inconsistency */
    if (((*pgm_bmp_used) & program_rsrc) != (*pgm_bmp_used)) {
        /* Inconsistency between the code */
        LOG_ERROR(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "Unit %d Inconsistency in program resources: Program resource 0x%x "
                              "Used program bitmap 0x%x.\n\r"), unit, program_rsrc, *pgm_bmp_used));
        SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT, 30, exit);
    }



exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_psl_pmf_pgms_get()", 0, 0);
}


/* 
 * At Egress stage, the action resolution is done 
 * according to the Key index: Key-B is always superior 
 * to Key-A. 
 * See if switching the keys is necessary
 */
uint32
  arad_pmf_psl_swicth_key_egq_get(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  ARAD_FP_DATABASE_STAGE   stage,
      SOC_SAND_IN  uint32                   pmf_pgm_from,
      SOC_SAND_IN  uint32                   db_id_ndx,
      SOC_SAND_IN  uint32                   db_strength,
      SOC_SAND_OUT uint8                    *switch_key_egq
  )
{
    uint32
        db_ndx,
        res;
    ARAD_SW_DB_PMF_DB_INFO
        sw_db_info;
    uint8 
        key_ndx;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *switch_key_egq = FALSE;
    if (stage == ARAD_FP_DATABASE_STAGE_EGRESS) {
        /* Compare the priority of the other DB of this program */
        for (db_ndx = 0; db_ndx < ARAD_PP_FP_NOF_DBS; ++db_ndx)
        {
            /* Skip the current DB */
            if (db_ndx == db_id_ndx) {
                continue;
            }
            res = arad_sw_db_pmf_db_info_get(unit, stage, db_ndx, &sw_db_info);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

            if (sw_db_info.valid && SHR_BITGET(sw_db_info.progs, pmf_pgm_from)) {
                /* The other DB is found, see if the key is in good place */
                key_ndx = sw_db_info.used_key[pmf_pgm_from][0]; /* Not 320b, otherwise no place */
                if (((db_strength < sw_db_info.prio) && (key_ndx == 0))
                    || ((db_strength > sw_db_info.prio) && (key_ndx == 1))) {
                    *switch_key_egq = TRUE;
                }
            }
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_psl_swicth_key_egq_get()", pmf_pgm_from, db_id_ndx);
}

/* Duplicate the SW DB and HW of PMF-Programs */
uint32
  arad_pmf_psl_pmf_pgm_duplicate(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  ARAD_FP_DATABASE_STAGE   stage,
      SOC_SAND_IN  uint32                   pmf_pgm_from,
      SOC_SAND_IN  uint32                   pmf_pgm_to,
      SOC_SAND_IN  uint8                    switch_key_egq
  )
{
    uint32
        ce_indx,
        fes_ndx,
        ce_rsrc,
        db_id_ndx,
        keys_bmp,
        line_ndx,
        pgms_db_pmb[ARAD_BIT_TO_U32(ARAD_SW_DB_PMF_NOF_DBS)],
        cycle_ndx,
        fld_val[ARAD_PMF_LOW_LEVEL_PMF_KEY_MAX_STAGE(EGRESS) + 1] = {0, 0},
        ce_group_ndx,
        key_ndx,
        key_ndx_resolved,
        table_entry[20] = {0},
        prog_used_cycle_bmp_lcl[1],
        res;
    ARAD_SW_DB_PMF_DB_INFO
        sw_db_info;
    ARAD_SW_DB_PMF_CE
      sw_db_ce;
    ARAD_SW_DB_PMF_FES
        fes_sw_db_info;
    uint8
        is_msb;
    soc_mem_t
        ce_table;
    soc_reg_above_64_val_t
            reg_above_64[ARAD_PMF_LOW_LEVEL_PMF_KEY_MAX_STAGE(EGRESS) + 1];
    soc_reg_t
        egq_key_data_base_profile[2] = {EGQ_KEYA_DATA_BASE_PROFILEr, EGQ_KEYB_DATA_BASE_PROFILEr};

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    sal_memset(table_entry, 0x0, sizeof(table_entry));

    /* 
     * Copy all the HW-dependent PMF-Program tables
     */
    if (stage == ARAD_FP_DATABASE_STAGE_EGRESS) {
        for (is_msb = FALSE; is_msb <= TRUE; is_msb++) {
            for (ce_group_ndx = 0; ce_group_ndx < ARAD_PMF_CE_NOF_GROUP_PER_LSB; ce_group_ndx++) {
                ce_table = arad_pmf_ce_table_get(stage, is_msb, 0);

                line_ndx = pmf_pgm_from + (ce_group_ndx * ARAD_PMF_LOW_LEVEL_NOF_PROGS); 
                /* At egress, only Key definition table to copy */
                res = soc_mem_read(unit, ce_table,  MEM_BLOCK_ANY, line_ndx, table_entry);
                SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

                if (switch_key_egq) {
                    /* Invert the bitmap of instructions between the Keys, get all */
                    for (key_ndx = 0; key_ndx < (ARAD_PMF_LOW_LEVEL_PMF_KEY_MAX_STAGE(EGRESS) + 1); key_ndx++) {
                    /* At egress, only Key definition table to copy */
                        fld_val[key_ndx] = soc_mem_field32_get(unit, ce_table, table_entry, 
                                               arad_pmf_ce_valid_fld_group_get(stage, ce_group_ndx, key_ndx));
                    }

                    /* Set all */
                    for (key_ndx = 0; key_ndx < (ARAD_PMF_LOW_LEVEL_PMF_KEY_MAX_STAGE(EGRESS) + 1); key_ndx++) {
                    /* At egress, only Key definition table to copy */
                        soc_mem_field32_set(unit, ce_table, table_entry, 
                                            arad_pmf_ce_valid_fld_group_get(stage, ce_group_ndx, key_ndx), 
                                            fld_val[ARAD_PMF_LOW_LEVEL_PMF_KEY_MAX_STAGE(EGRESS) - key_ndx]);
                    }
                }

                line_ndx = pmf_pgm_to + (ce_group_ndx * ARAD_PMF_LOW_LEVEL_NOF_PROGS); 
                res = soc_mem_write(unit, ce_table, MEM_BLOCK_ANY, line_ndx, table_entry);
                SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 16, exit);
            }
        }

        /* Update the access profile ids */
        /* Invert the bitmap of instructions between the Keys, get all */
        for (key_ndx = 0; key_ndx < (ARAD_PMF_LOW_LEVEL_PMF_KEY_MAX_STAGE(EGRESS) + 1); key_ndx++) {
            SOC_REG_ABOVE_64_CLEAR(reg_above_64[key_ndx]);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 18, exit, soc_reg_above_64_get(unit, egq_key_data_base_profile[key_ndx], REG_PORT_ANY, 0, reg_above_64[key_ndx]));
            key_ndx_resolved = (switch_key_egq)? (ARAD_PMF_LOW_LEVEL_PMF_KEY_MAX_STAGE(EGRESS) - key_ndx): key_ndx;
            SHR_BITCOPY_RANGE(&fld_val[key_ndx_resolved], 0, reg_above_64[key_ndx], (ARAD_PMF_KEY_TCAM_DB_PROFILE_NOF_BITS * pmf_pgm_from), ARAD_PMF_KEY_TCAM_DB_PROFILE_NOF_BITS);
        }

        for (key_ndx = 0; key_ndx < (ARAD_PMF_LOW_LEVEL_PMF_KEY_MAX_STAGE(EGRESS) + 1); key_ndx++) {
            SHR_BITCOPY_RANGE(reg_above_64[key_ndx], (ARAD_PMF_KEY_TCAM_DB_PROFILE_NOF_BITS * pmf_pgm_to), &fld_val[key_ndx], 0, ARAD_PMF_KEY_TCAM_DB_PROFILE_NOF_BITS);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 19, exit, soc_reg_above_64_set(unit, egq_key_data_base_profile[key_ndx], REG_PORT_ANY, 0, reg_above_64[key_ndx]));
        }
    }
    else if (stage == ARAD_FP_DATABASE_STAGE_INGRESS_PMF) { /* Ingress PMF */
        /* At ingress, multiple tables*/
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, READ_IHB_PROGRAM_KEY_GEN_VARm(unit, MEM_BLOCK_ANY, pmf_pgm_from, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 22, exit, WRITE_IHB_PROGRAM_KEY_GEN_VARm(unit, MEM_BLOCK_ANY, pmf_pgm_to, table_entry));

        /* 1st half of the table */
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_IHB_PMF_PASS_1_KEY_GEN_LSBm(unit, MEM_BLOCK_ANY, pmf_pgm_from, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 32, exit, WRITE_IHB_PMF_PASS_1_KEY_GEN_LSBm(unit, MEM_BLOCK_ANY, pmf_pgm_to, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 34, exit, READ_IHB_PMF_PASS_1_KEY_GEN_MSBm(unit, MEM_BLOCK_ANY, pmf_pgm_from, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 36, exit, WRITE_IHB_PMF_PASS_1_KEY_GEN_MSBm(unit, MEM_BLOCK_ANY, pmf_pgm_to, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, READ_IHB_PMF_PASS_2_KEY_GEN_LSBm(unit, MEM_BLOCK_ANY, pmf_pgm_from, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 42, exit, WRITE_IHB_PMF_PASS_2_KEY_GEN_LSBm(unit, MEM_BLOCK_ANY, pmf_pgm_to, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 44, exit, READ_IHB_PMF_PASS_2_KEY_GEN_MSBm(unit, MEM_BLOCK_ANY, pmf_pgm_from, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 46, exit, WRITE_IHB_PMF_PASS_2_KEY_GEN_MSBm(unit, MEM_BLOCK_ANY, pmf_pgm_to, table_entry));
        /* 2nd half of the table */
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 230, exit, READ_IHB_PMF_PASS_1_KEY_GEN_LSBm(unit, MEM_BLOCK_ANY, ARAD_PMF_LOW_LEVEL_NOF_PROGS+pmf_pgm_from, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 232, exit, WRITE_IHB_PMF_PASS_1_KEY_GEN_LSBm(unit, MEM_BLOCK_ANY, ARAD_PMF_LOW_LEVEL_NOF_PROGS+pmf_pgm_to, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 234, exit, READ_IHB_PMF_PASS_1_KEY_GEN_MSBm(unit, MEM_BLOCK_ANY, ARAD_PMF_LOW_LEVEL_NOF_PROGS+pmf_pgm_from, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 236, exit, WRITE_IHB_PMF_PASS_1_KEY_GEN_MSBm(unit, MEM_BLOCK_ANY, ARAD_PMF_LOW_LEVEL_NOF_PROGS+pmf_pgm_to, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 240, exit, READ_IHB_PMF_PASS_2_KEY_GEN_LSBm(unit, MEM_BLOCK_ANY, ARAD_PMF_LOW_LEVEL_NOF_PROGS+pmf_pgm_from, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 242, exit, WRITE_IHB_PMF_PASS_2_KEY_GEN_LSBm(unit, MEM_BLOCK_ANY, ARAD_PMF_LOW_LEVEL_NOF_PROGS+pmf_pgm_to, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 244, exit, READ_IHB_PMF_PASS_2_KEY_GEN_MSBm(unit, MEM_BLOCK_ANY, ARAD_PMF_LOW_LEVEL_NOF_PROGS+pmf_pgm_from, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 246, exit, WRITE_IHB_PMF_PASS_2_KEY_GEN_MSBm(unit, MEM_BLOCK_ANY, ARAD_PMF_LOW_LEVEL_NOF_PROGS+pmf_pgm_to, table_entry));

        SOC_SAND_SOC_IF_ERROR_RETURN(res, 250, exit, READ_IHB_PMF_PASS_1_LOOKUPm(unit, MEM_BLOCK_ANY, pmf_pgm_from, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 252, exit, WRITE_IHB_PMF_PASS_1_LOOKUPm(unit, MEM_BLOCK_ANY, pmf_pgm_to, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 260, exit, READ_IHB_PMF_PASS_2_LOOKUPm(unit, MEM_BLOCK_ANY, pmf_pgm_from, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 262, exit, WRITE_IHB_PMF_PASS_2_LOOKUPm(unit, MEM_BLOCK_ANY, pmf_pgm_to, table_entry));

        SOC_SAND_SOC_IF_ERROR_RETURN(res, 270, exit, READ_IHB_PMF_INITIAL_KEY_2ND_PASSm(unit, MEM_BLOCK_ANY, pmf_pgm_from, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 272, exit, WRITE_IHB_PMF_INITIAL_KEY_2ND_PASSm(unit, MEM_BLOCK_ANY, pmf_pgm_to, table_entry));

        SOC_SAND_SOC_IF_ERROR_RETURN(res, 280, exit, READ_IHB_PMF_PASS_2_KEY_UPDATEm(unit, MEM_BLOCK_ANY, pmf_pgm_from, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 282, exit, WRITE_IHB_PMF_PASS_2_KEY_UPDATEm(unit, MEM_BLOCK_ANY, pmf_pgm_to, table_entry));

        SOC_SAND_SOC_IF_ERROR_RETURN(res, 290, exit, READ_IHB_PMF_PROGRAM_COUNTERSm(unit, MEM_BLOCK_ANY, pmf_pgm_from, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 292, exit, WRITE_IHB_PMF_PROGRAM_COUNTERSm(unit, MEM_BLOCK_ANY, pmf_pgm_to, table_entry));

        SOC_SAND_SOC_IF_ERROR_RETURN(res, 300, exit, READ_IHB_PMF_PROGRAM_GENERALm(unit, MEM_BLOCK_ANY, pmf_pgm_from, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 302, exit, WRITE_IHB_PMF_PROGRAM_GENERALm(unit, MEM_BLOCK_ANY, pmf_pgm_to, table_entry));

        /* 1st half of the table */
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 310, exit, READ_IHB_PMF_FEM_PROGRAMm(unit, MEM_BLOCK_ANY, pmf_pgm_from, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 312, exit, WRITE_IHB_PMF_FEM_PROGRAMm(unit, MEM_BLOCK_ANY, pmf_pgm_to, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 320, exit, READ_IHB_PMF_FES_PROGRAMm(unit, MEM_BLOCK_ANY, pmf_pgm_from, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 322, exit, WRITE_IHB_PMF_FES_PROGRAMm(unit, MEM_BLOCK_ANY, pmf_pgm_to, table_entry));
        /* 2nd half of the table */
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 330, exit, READ_IHB_PMF_FEM_PROGRAMm(unit, MEM_BLOCK_ANY, ARAD_PMF_LOW_LEVEL_NOF_PROGS+pmf_pgm_from, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 332, exit, WRITE_IHB_PMF_FEM_PROGRAMm(unit, MEM_BLOCK_ANY, ARAD_PMF_LOW_LEVEL_NOF_PROGS+pmf_pgm_to, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 340, exit, READ_IHB_PMF_FES_PROGRAMm(unit, MEM_BLOCK_ANY, ARAD_PMF_LOW_LEVEL_NOF_PROGS+pmf_pgm_from, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 342, exit, WRITE_IHB_PMF_FES_PROGRAMm(unit, MEM_BLOCK_ANY, ARAD_PMF_LOW_LEVEL_NOF_PROGS+pmf_pgm_to, table_entry));
    }
#ifdef BCM_88660_A0
  else if (SOC_IS_ARADPLUS(unit) && (stage == ARAD_FP_DATABASE_STAGE_INGRESS_SLB)) {
        /* At ingress, multiple lines to copy */
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 400, exit, READ_IHP_FLP_CONSISTENT_HASHING_KEY_GENm(unit, MEM_BLOCK_ANY, pmf_pgm_from, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 402, exit, WRITE_IHP_FLP_CONSISTENT_HASHING_KEY_GENm(unit, MEM_BLOCK_ANY, pmf_pgm_to, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 404, exit, READ_IHP_FLP_CONSISTENT_HASHING_KEY_GENm(unit, MEM_BLOCK_ANY, ARAD_PMF_LOW_LEVEL_NOF_PROGS+pmf_pgm_from, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 406, exit, WRITE_IHP_FLP_CONSISTENT_HASHING_KEY_GENm(unit, MEM_BLOCK_ANY, ARAD_PMF_LOW_LEVEL_NOF_PROGS+pmf_pgm_to, table_entry));

        SOC_SAND_SOC_IF_ERROR_RETURN(res, 410, exit, READ_IHB_CONSISTENT_HASHING_PROGRAM_VARIABLESm(unit, MEM_BLOCK_ANY, pmf_pgm_from, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 412, exit, WRITE_IHB_CONSISTENT_HASHING_PROGRAM_VARIABLESm(unit, MEM_BLOCK_ANY, pmf_pgm_to, table_entry));
    }
#endif /* BCM_88660_A0 */
    else {
        /* Unknown stage */
        SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT, 350, exit);
    }


    /* 
     * Copy all the SW DB per PMF-Program
     */
    for (cycle_ndx = 0; cycle_ndx < ARAD_PMF_NOF_CYCLES; ++cycle_ndx) {
        /* update the resources: CE, key, FES, */
        res = arad_sw_db_pgm_ce_rsrc_get(unit, stage, pmf_pgm_from, cycle_ndx, &ce_rsrc);
        SOC_SAND_CHECK_FUNC_RESULT(res, 402, exit);
        res = arad_sw_db_pgm_ce_rsrc_set(unit, stage, pmf_pgm_to, cycle_ndx, &ce_rsrc);
        SOC_SAND_CHECK_FUNC_RESULT(res, 404, exit);

        res = arad_sw_db_pgm_key_rsrc_get(unit, stage, pmf_pgm_from, cycle_ndx, &keys_bmp);
        SOC_SAND_CHECK_FUNC_RESULT(res, 410, exit);
        if (switch_key_egq) {
            /* Switch the 2 keys (4 half-parts) */
            keys_bmp = (~keys_bmp) & 0xF;
        }
        res = arad_sw_db_pgm_key_rsrc_set(unit, stage, pmf_pgm_to, cycle_ndx, &keys_bmp);
        SOC_SAND_CHECK_FUNC_RESULT(res, 412, exit);

        for(ce_indx = 0; ce_indx < ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG; ++ce_indx) {
            res = arad_sw_db_pgm_ce_instr_get(unit, stage, pmf_pgm_from, cycle_ndx, ce_indx, &sw_db_ce);
            SOC_SAND_CHECK_FUNC_RESULT(res, 416, exit);
            res = arad_sw_db_pgm_ce_instr_set(unit, stage, pmf_pgm_to, cycle_ndx, ce_indx, &sw_db_ce);
            SOC_SAND_CHECK_FUNC_RESULT(res, 418, exit);
        }

        for(fes_ndx = 0; fes_ndx < ARAD_PMF_LOW_LEVEL_NOF_FESS; ++fes_ndx) {
            res = arad_sw_db_pgm_fes_get(unit, stage, pmf_pgm_from, fes_ndx, &fes_sw_db_info);
            SOC_SAND_CHECK_FUNC_RESULT(res, 417, exit);
            res = arad_sw_db_pgm_fes_set(unit, stage, pmf_pgm_to, fes_ndx, &fes_sw_db_info);
            SOC_SAND_CHECK_FUNC_RESULT(res, 419, exit);
        }
    }

    for (db_id_ndx = 0; db_id_ndx < ARAD_PP_FP_NOF_DBS; ++db_id_ndx)
    {
        res = arad_sw_db_pmf_db_info_get(unit, stage, db_id_ndx, &sw_db_info);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 420, exit);

        /* Remove any reference to the duplicate DB if necessary */
        if (SHR_BITGET(sw_db_info.progs, pmf_pgm_to)) {
            SHR_BITCLR(sw_db_info.progs, pmf_pgm_to);
        }

        /* Update relevant DBs */
        if (SHR_BITGET(sw_db_info.progs, pmf_pgm_from)) {
            /* Database used */
            SHR_BITSET(sw_db_info.progs, pmf_pgm_to);

            /* Update the cycle used for this program */
            *prog_used_cycle_bmp_lcl = sw_db_info.prog_used_cycle_bmp;
            if(SHR_BITGET(prog_used_cycle_bmp_lcl, pmf_pgm_from)) {
                SHR_BITSET(prog_used_cycle_bmp_lcl, pmf_pgm_to);
            }
            else{
                SHR_BITCLR(prog_used_cycle_bmp_lcl, pmf_pgm_to);
            }
            sw_db_info.prog_used_cycle_bmp = *prog_used_cycle_bmp_lcl;

            /* Copy the Key location */
            sw_db_info.used_key[pmf_pgm_to][0] = sw_db_info.used_key[pmf_pgm_from][0];
            sw_db_info.used_key[pmf_pgm_to][1] = sw_db_info.used_key[pmf_pgm_from][1];

            if (switch_key_egq) {
                /* Invert the Keys */
                sw_db_info.used_key[pmf_pgm_to][0] = ARAD_PMF_LOW_LEVEL_PMF_KEY_MAX_STAGE(EGRESS) - sw_db_info.used_key[pmf_pgm_to][0];
                sw_db_info.used_key[pmf_pgm_to][1] = ARAD_PMF_LOW_LEVEL_PMF_KEY_MAX_STAGE(EGRESS) - sw_db_info.used_key[pmf_pgm_to][1];
            }
        }

        res = arad_sw_db_pmf_db_info_set(unit, stage, db_id_ndx, &sw_db_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 422, exit);
    }

    res = arad_sw_db_pmf_psl_pgm_dbs_get(unit, stage, pmf_pgm_from, pgms_db_pmb);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 515, exit);
    res = arad_sw_db_pmf_psl_pgm_dbs_set(unit, stage, pmf_pgm_to, pgms_db_pmb);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 517, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_psl_pmf_pgm_duplicate()", pmf_pgm_from, pmf_pgm_to);
}

/* Update the Preselector line's PMF program at SW DB and HW */
uint32
  arad_pmf_psl_pmf_pgm_update(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  ARAD_FP_DATABASE_STAGE   stage,
      SOC_SAND_IN  uint32                       is_for_tm,
      SOC_SAND_IN  uint32                   presel_bmp_update[ARAD_PMF_NOF_LINES_MAX_ALL_STAGES_LEN], /* Bitmap of the Presel lines to update */
      SOC_SAND_IN  uint8                    is_for_all_lines, /* scan all lines*/
      SOC_SAND_IN  uint32                   pmf_pgm_from,
      SOC_SAND_IN  uint32                   pmf_pgm_to
  )
{
    uint32
        offset_line_ndx,
        is_tm = is_for_tm, 
        res;
    int32
        level_indx,
        line_indx;
    ARAD_SW_DB_PMF_PSL_LEVEL_INFO
        curr_level;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* 
     * Go over the lines, only the ones to update: 
     * - Update the HW 
     * - Update the SW DB 
     */
    /* traverse over all levels */
    for(level_indx = 0; level_indx < ARAD_PMF_NOF_LEVELS; ++level_indx) 
    {
        /* get cur level info */
        res = arad_sw_db_level_info_get(unit, stage, level_indx, is_tm, &curr_level);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

        /* traverse over levels lines*/
        for(line_indx = 0 ; line_indx < curr_level.nof_lines; ++line_indx) 
        {
            if ( (soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) && (!(curr_level.lines[line_indx].flags & ARAD_PMF_SEL_LINE_VALID )) ) {
                continue;
            } 
            offset_line_ndx = curr_level.first_index - line_indx;
        
            if (is_for_all_lines || (SHR_BITGET(presel_bmp_update, offset_line_ndx))) 
            {
                /* The DB participates to this line */
                if (curr_level.lines[line_indx].prog_id == pmf_pgm_from) {

                    /* Good PMF-Program to change */
                    curr_level.lines[line_indx].prog_id = pmf_pgm_to;

                    /* Update SW DB */
                    res = arad_sw_db_level_info_set(unit, stage, level_indx, is_tm, &curr_level);
                    SOC_SAND_CHECK_FUNC_RESULT(res, 23, exit);

                    /* Update in the HW */
                    res = arad_pmf_sel_line_hw_write(
                            unit,
                            stage,
                            offset_line_ndx, 
                            &curr_level.lines[line_indx],
                            curr_level.lines[line_indx].prog_id,
                            TRUE
                          );
                    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

                }
            }
        }
    }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_psl_pmf_pgm_update()", pmf_pgm_from, pmf_pgm_to);
}


uint32
  arad_pmf_psl_expected_match_get(
      SOC_SAND_IN  int                 unit,
      SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  ARAD_PMF_PSL              *psl,
      SOC_SAND_OUT uint32                  groups[ARAD_PMF_GROUP_LEN],
      SOC_SAND_OUT uint32                  *prog_id
  )
{
    ARAD_PMF_SEL_GROUP
        pmf_group;
    ARAD_SW_DB_PMF_PSL_LEVEL_INFO
        curr_level;
    uint32
        is_tm = FALSE, /* only for test */ 
        res;
    int32
        line_indx;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_pmf_psl_to_group_map(
            unit,
            stage,
            psl,
            &pmf_group
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    LOG_INFO(BSL_LS_SOC_FP,
             (BSL_META_U(unit,
                         "check:\n\r")));
    ARAD_PMF_SEL_GROUP_print(&pmf_group);
    LOG_INFO(BSL_LS_SOC_FP,
             (BSL_META_U(unit,
                         "\n\r")));

    /* get cur level info */
    res = arad_sw_db_level_info_get(unit, stage, 1, is_tm, &curr_level);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    sal_memset(groups, 0x0, sizeof(uint32) * ARAD_PMF_GROUP_LEN);
    *prog_id = 0;

    for(line_indx = curr_level.nof_lines-1 ; line_indx >= 0 ; --line_indx) 
    {
        if (!arad_pmf_sel_group_is_disjoint(&curr_level.lines[line_indx].group, &pmf_group)) 
        {
            soc_sand_bitstream_or(groups, curr_level.lines[line_indx].groups, ARAD_PMF_GROUP_LEN);
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_psl_expected_match_get()", 0, 0);
}


uint32 
  arad_pmf_sel_get_line_at_index(
      SOC_SAND_IN  int                unit,
      SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN     uint32              index,
      SOC_SAND_OUT    ARAD_SW_DB_PMF_PSL_LINE_INFO   *line  /* group in line representation */
      
  )
{
    int32
      level_indx,
        is_tm = FALSE; /* only for test */ 
    ARAD_SW_DB_PMF_PSL_LEVEL_INFO
        cur_level;
    uint32
        res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    for(level_indx = ARAD_PMF_NOF_LEVELS-1; level_indx >= 0; --level_indx) 
    {
        /* get level info */
        res = arad_sw_db_level_info_get(unit, stage, level_indx, is_tm, &cur_level);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


        if(index >= cur_level.first_index  && index <=  cur_level.last_index ) 
        {
            sal_memcpy(line,&cur_level.lines[cur_level.first_index - index],sizeof(ARAD_SW_DB_PMF_PSL_LINE_INFO));
            goto exit;
        }

    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_sel_get_line_at_index()",0,0);
}
uint32
  arad_pmf_sel_table_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 line_indx,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE stage,
    SOC_SAND_OUT uint32 *valid,
    SOC_SAND_OUT uint32 *program,
    SOC_SAND_OUT ARAD_PMF_PSL *psl_tbl_data
  )
{
  uint32
	res = SOC_SAND_OK;

  /*SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EGP_PSL_TABLE_GET);*/
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (stage == ARAD_FP_DATABASE_STAGE_EGRESS) 
  {
    res = arad_pp_egq_pmf_program_selection_cam_tbl_get_unsafe(
            unit,
            line_indx,
            &(psl_tbl_data->egress_pmf)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    *valid = psl_tbl_data->egress_pmf.valid;
    *program = psl_tbl_data->egress_pmf.program;
  }
#ifdef BCM_88660_A0
  else if (SOC_IS_ARADPLUS(unit) && (stage == ARAD_FP_DATABASE_STAGE_INGRESS_SLB))
  {
    res = arad_pp_ihb_slb_program_selection_cam_tbl_get_unsafe(
            unit,
            line_indx,
            &(psl_tbl_data->ingress_slb)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
    *valid = psl_tbl_data->ingress_slb.valid;
    *program = psl_tbl_data->ingress_slb.program;
  }
#endif /* BCM_88660_A0 */
  else if (stage == ARAD_FP_DATABASE_STAGE_INGRESS_PMF)
  {
    res = arad_pp_ihb_pmf_program_selection_cam_tbl_get_unsafe(
            unit,
            line_indx,
            &(psl_tbl_data->ingress_pmf)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    *valid = psl_tbl_data->ingress_pmf.valid;
    *program = psl_tbl_data->ingress_pmf.program;
  }
  else {
      /* Unknown stage */
      SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT, 30, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_egq_psl_table_get()", 0, 0);
}

uint32
  arad_pmf_psl_match_get(
      SOC_SAND_IN  int                 unit,
      SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  ARAD_PMF_PSL              *psl,
      SOC_SAND_OUT uint32                  groups[ARAD_PMF_GROUP_LEN],
      SOC_SAND_OUT uint32                  *prog_id
  )
{
    ARAD_PMF_SEL_GROUP
        pmf_group,
        line_group;
    uint32
        valid,
        program,
        res;
    ARAD_PMF_PSL
        tbl_data;
    int32
        line_indx;
    ARAD_SW_DB_PMF_PSL_LINE_INFO
        match_line;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_pmf_psl_to_group_map(
            unit,
            stage,
            psl,
            &pmf_group
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    sal_memset(groups, 0x0, sizeof(uint32) * ARAD_PMF_GROUP_LEN);
    *prog_id = 0;

    for(line_indx = 0 ; line_indx < ARAD_PMF_NOF_LINES ; ++line_indx) 
    {
        /* Read from the HW - can be a separate function */
        res = arad_pmf_sel_table_get(
            unit, 
            line_indx, 
            stage, 
            &valid, 
            &program, 
            &tbl_data
        ); 
        SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

        /* if not valid continue */
        if(!valid) 
        {
            continue;
        }

        res = arad_pmf_psl_to_group_map(
                unit,
                stage,
                &tbl_data,
                &line_group
            );
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

        if (!arad_pmf_sel_group_is_disjoint(&line_group, &pmf_group)) 
        {
            *prog_id = program;
            res = arad_pmf_sel_get_line_at_index(unit, stage, line_indx,&match_line);
            SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
            soc_sand_bitstream_or(groups, match_line.groups, ARAD_PMF_GROUP_LEN);
            
            break;
        }

    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_psl_match_get()", 0, 0);
}

uint32
  arad_pmf_prog_select_eth_section_nof_lines_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE   stage,
    SOC_SAND_OUT uint32                   *nof_eth_lines
  )
{
    uint32 min_indx, max_indx;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_FUNC_RESULT(arad_pmf_prog_select_line_borders_get(unit,stage, ARAD_PMF_PSL_TYPE_ETH, &min_indx,&max_indx), 25, exit);

    *nof_eth_lines = max_indx-min_indx + 1;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_prog_select_eth_section_nof_lines_get()",0,0);

}

#if ARAD_DEBUG_IS_LVL1

void ARAD_PMF_SEL_GROUPS_print(
    SOC_SAND_IN uint32 groups[ARAD_PMF_GROUP_LEN]
  )
{
    uint32
        indx;
   
    for(indx  = 0; indx < ARAD_BIT_TO_U32(ARAD_PMF_GROUP_LEN); ++indx) 
    {
      LOG_CLI((BSL_META("%08x "),groups[indx]));
    }
    
    return;
}

void ARAD_PMF_SEL_GROUP_print(
    SOC_SAND_IN ARAD_PMF_SEL_GROUP *group
  )
{
    uint32
        bit_indx,
        mask_val,
        val;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

    SOC_SAND_CHECK_NULL_INPUT(group);

    for(bit_indx = 0; bit_indx < ARAD_PMF_SEL_LINE_NOF_BITS; ++bit_indx) {
        mask_val = soc_sand_bitstream_test_bit(group->mask, bit_indx);
        if(mask_val == 1) 
        {
            LOG_CLI((BSL_META_U(unit,
                                "*")));
        }
        else
        {
            val = soc_sand_bitstream_test_bit(group->value, bit_indx);
            LOG_CLI((BSL_META_U(unit,
                                "%u"), val));
        }
        if((bit_indx+1)%32 == 0) 
        {
            LOG_CLI((BSL_META_U(unit,
                                " ")));
        }
    }

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void ARAD_PMF_LINE_INFO_print(
    SOC_SAND_IN ARAD_SW_DB_PMF_PSL_LINE_INFO *line
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

    SOC_SAND_CHECK_NULL_INPUT(line);
    LOG_CLI((BSL_META_U(unit,
                        " index %u:"), line->line_id));
    ARAD_PMF_SEL_GROUP_print(&(line->group));
    LOG_CLI((BSL_META_U(unit,
                        "  ")));
    LOG_CLI((BSL_META_U(unit,
                        "groups:")));
    ARAD_PMF_SEL_GROUPS_print(line->groups);
    LOG_CLI((BSL_META_U(unit,
                        " prog: %u"),line->prog_id));

    if(line->flags & ARAD_PMF_SEL_LINE_REMOVE) 
    {
        LOG_CLI((BSL_META_U(unit,
                            "  X")));
    }
    if(line->flags & ARAD_PMF_SEL_LINE_VALID) 
    {
        LOG_CLI((BSL_META_U(unit,
                            "  V")));
    }
    
exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void ARAD_PMF_LEVEL_INFO_print(
    SOC_SAND_IN ARAD_SW_DB_PMF_PSL_LEVEL_INFO *level,
    SOC_SAND_IN  uint32           flavor
  )
{
    int32
        line_indx;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

    SOC_SAND_CHECK_NULL_INPUT(level);

    if(level->nof_lines == 0 && level->nof_new_lines == 0) {
        if(flavor == 0) {
            LOG_CLI((BSL_META_U(unit,
                                "level %u: "),level->level_index));
            LOG_CLI((BSL_META_U(unit,
                                "EMPTY \n\r")));
        }
        goto exit;
    }
    if(flavor <= 1) {
        LOG_CLI((BSL_META_U(unit,
                            "level %u: "),level->level_index));
        /* start & end index */
        LOG_CLI((BSL_META_U(unit,
                            "[%u,%u] \n\r"),level->first_index, level->last_index));
    }
    /* lines from top to down */
    /* print level info */
    for(line_indx = level->nof_lines-1 ; line_indx >= 0 ; --line_indx) 
    {
        ARAD_PMF_LINE_INFO_print(&level->lines[line_indx]);
        LOG_CLI((BSL_META_U(unit,
                            "\n\r")));
    }
    if(level->nof_new_lines > 0) 
    {
        LOG_CLI((BSL_META_U(unit,
                            "new lines: \n\r")));
    }
    /* lines from top to down */
    /* print level info */
    for(line_indx = level->nof_lines + level->nof_new_lines - 1 ; line_indx >= (int32)level->nof_lines ; --line_indx)
    {
        ARAD_PMF_LINE_INFO_print(&level->lines[line_indx]);
        LOG_CLI((BSL_META_U(unit,
                            "\n\r")));
    }

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  arad_pmf_prog_select_print_all(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                  flavor
  )
{
    ARAD_SW_DB_PMF_PSL_LEVEL_INFO
        level;
    int32
        level_indx;
    uint32
        is_tm = FALSE, /* only for test */ 
        res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* go over existing levels */
     for(level_indx = ARAD_PMF_NOF_LEVELS - 1; level_indx >= 0; --level_indx) 
    {
        /* get cur level info */
        res = arad_sw_db_level_info_get(unit, stage, level_indx, is_tm, &level);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

        ARAD_PMF_LEVEL_INFO_print(&level,flavor);
     }

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_pmf_prog_select_test1(
    SOC_SAND_IN  int                 unit
  )
{
    ARAD_PMF_PSL
        psl1;
    uint8
        success;
    uint32
        res;
    ARAD_FP_DATABASE_STAGE
        stage = ARAD_FP_DATABASE_STAGE_INGRESS_PMF;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);


    ARAD_PMF_PSL_clear(unit, stage, &psl1, TRUE,TRUE);

    /* bitmask 0x7ff */

    /* 1. first */
    psl1.ingress_pmf.mask_packet_format_qualifier_fwd = 0x00f;
    psl1.ingress_pmf.packet_format_qualifier_fwd = 0x550;
   
    res = arad_pmf_psl_add(unit, stage, 5, 0, &psl1, &success);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    

    /* 2. disjoint */
    psl1.ingress_pmf.mask_packet_format_qualifier_fwd = 0x00f;
    psl1.ingress_pmf.packet_format_qualifier_fwd = 0x650;

    res = arad_pmf_psl_add(unit, stage,  5, 0, &psl1, &success);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    arad_pmf_prog_select_print_all(unit, stage, 2);

    /* 3. include 2 */
    psl1.ingress_pmf.mask_packet_format_qualifier_fwd = 0x0ff;
    psl1.ingress_pmf.packet_format_qualifier_fwd = 0x600;

    res = arad_pmf_psl_add(unit, stage,  5, 0, &psl1, &success);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    arad_pmf_prog_select_print_all(unit, stage, 2);

    /* 4. included by 2,3 */
    psl1.ingress_pmf.mask_packet_format_qualifier_fwd = 0x007;
    psl1.ingress_pmf.packet_format_qualifier_fwd = 0x658;

    res = arad_pmf_psl_add(unit, stage,  5, 0, &psl1, &success);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    arad_pmf_prog_select_print_all(unit, stage, 2);

    /* 5. instersected with 4 */
    psl1.ingress_pmf.mask_packet_format_qualifier_fwd = 0x00c;
    psl1.ingress_pmf.packet_format_qualifier_fwd = 0x653;

    res = arad_pmf_psl_add(unit, stage,  5, 0, &psl1, &success);
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    arad_pmf_prog_select_print_all(unit, stage, 2);
   
exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void arad_pmf_prog_test_rand_val(
        SOC_SAND_IN uint32 total_bits,
        SOC_SAND_IN uint32 unmasked_bits,
        SOC_SAND_OUT uint32 *val,
        SOC_SAND_OUT uint32 *mask
     )
{
    uint32
        tmp;
    *mask = soc_sand_os_rand() & ((1 << (unmasked_bits))-1);
    tmp = (((1 << (total_bits-unmasked_bits))-1) << unmasked_bits);
    *mask |= tmp;
    *val = soc_sand_os_rand() & ((1 << (unmasked_bits))-1);
    *val = *val & (~*mask);
}



uint32
  arad_pmf_psl_check(
      SOC_SAND_IN  int                 unit,
      SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  ARAD_PMF_PSL              *psl,
      SOC_SAND_OUT uint8                  *success
  )
{
    uint32                  
        expect_groups[ARAD_PMF_GROUP_LEN],
        act_groups[ARAD_PMF_GROUP_LEN];
    uint32                  
        expect_prog_id,
        act_prog_id;
    uint32
        res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_pmf_psl_expected_match_get(
            unit,
            stage,
            psl,
            expect_groups,
            &expect_prog_id
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    LOG_INFO(BSL_LS_SOC_FP,
             (BSL_META_U(unit,
                         "expected group: ")));
    ARAD_PMF_SEL_GROUPS_print(expect_groups);
    LOG_INFO(BSL_LS_SOC_FP,
             (BSL_META_U(unit,
                         "\n\r")));


    res = arad_pmf_psl_match_get(
            unit,
            stage,
            psl,
            act_groups,
            &act_prog_id
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

    LOG_INFO(BSL_LS_SOC_FP,
             (BSL_META_U(unit,
                         "act group: ")));
    ARAD_PMF_SEL_GROUPS_print(act_groups);
    LOG_INFO(BSL_LS_SOC_FP,
             (BSL_META_U(unit,
                         "\n\r")));


    if(sal_memcmp(expect_groups,act_groups,sizeof(uint32)*ARAD_PMF_GROUP_LEN))
    {
        LOG_INFO(BSL_LS_SOC_FP,
                 (BSL_META_U(unit,
                             "****FAIL****\n\r")));
        *success = FALSE;
    }

    *success = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_psl_check()",0,0);
}


void
  arad_pmf_prog_select_test2(
    SOC_SAND_IN  int                 unit
  )
{
    ARAD_PMF_SEL_GROUP
        pmf_group;
    ARAD_PMF_PSL
        add_psl,
        find_psl;
    uint32
        total_bits = 11,
        nof_bits = 5,
        nof_psls = 10,
        nof_lookups = 2,
        psl_index,
        lkup_indx,
        tmp_mask;
    uint8
        match;
    uint8
        success;
    uint32
        res;
    ARAD_FP_DATABASE_STAGE
        stage = ARAD_FP_DATABASE_STAGE_INGRESS_PMF;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    soc_sand_os_srand(1);

    for(psl_index = 0; psl_index < nof_psls; ++psl_index) 
    {
        /* mask all */
        ARAD_PMF_PSL_clear(unit, stage, &add_psl, TRUE,TRUE);

        /* exact line, unmask all */
        ARAD_PMF_PSL_clear(unit, stage, &find_psl, TRUE,FALSE);

        /* bitmask 0x7ff */
        arad_pmf_prog_test_rand_val(total_bits, nof_bits,&add_psl.ingress_pmf.packet_format_qualifier_fwd, &add_psl.ingress_pmf.mask_packet_format_qualifier_fwd);


       /*
        * print what we adding 
        */
        res = arad_pmf_psl_to_group_map(
                unit,
                stage,
                &add_psl,
                &pmf_group
            );
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        LOG_INFO(BSL_LS_SOC_FP,
                 (BSL_META_U(unit,
                             "adding (%u): \n\r"), psl_index));
        ARAD_PMF_SEL_GROUP_print(&pmf_group);
        LOG_INFO(BSL_LS_SOC_FP,
                 (BSL_META_U(unit,
                             "\n\r")));

        res = arad_pmf_psl_add(unit, stage,  psl_index, 0, &add_psl, &success);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

        arad_pmf_prog_select_print_all(unit, stage, 1);

        for(lkup_indx = 0; lkup_indx < nof_lookups; ++lkup_indx) 
        {
            arad_pmf_prog_test_rand_val(total_bits, nof_bits,&find_psl.ingress_pmf.packet_format_qualifier_fwd, &tmp_mask);
            res = arad_pmf_psl_check(unit, stage, &find_psl, &match);
            SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
            if(!match) 
            {
                LOG_INFO(BSL_LS_SOC_FP,
                         (BSL_META_U(unit,
                                     "%u\n\r"), lkup_indx));
                goto exit;
            }
        }

    }


exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


#endif  /*ARAD_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

#undef _ERR_MSG_MODULE_NAME

#endif /* of #if defined(BCM_88650_A0) */

