/* $Id: petra_tbl_access.c,v 1.8 Broadcom SDK $
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


/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_tbl_access.h>
#include <soc/dpp/Petra/petra_chip_tbls.h>
#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_bitstream.h>
#include <soc/dpp/Petra/petra_api_mgmt.h>
#include <soc/dpp/Petra/petra_general.h>
#include <soc/dpp/Petra/petra_shadow.h>
#include <soc/dpp/Petra/petra_sw_db.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PETRA_WRED_NOF_DROP_PRECEDENCE   (SOC_PETRA_NOF_DROP_PRECEDENCE)

#define SOC_PETRA_TBL_ACC_DRAM_NDX_NOF_BITS   3

#define SOC_PETRA_QDR_QDR_DLL_MEM_TBL_ENTRY_SIZE 1


/* } */

/*************
 *  MACROS   *
 *************/
/* { */
#define SOC_PETRA_TBL_SIZE_CHECK                                                     \
    if (offset >= tbl->addr.base + tbl->addr.size)                              \
    {                                                                           \
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_TBL_RANGE_OUT_OF_LIMIT_ERR, 10045, exit);        \
    }

#define SOC_PETRA_TBL_INDEXED_SIZE_CHECK(idx_offst)                                     \
    if (offset >= tbl->addr.base + idx_offst + tbl->addr.size)                              \
    {                                                                           \
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_TBL_RANGE_OUT_OF_LIMIT_ERR, 10078, exit);        \
    }

#define SOC_PETRA_TBL_WRITE_IND_OPT_SET(width_longs, reverse_order)                 \
          SOC_SAND_SET_TBL_WRITE_RVRS_BITS_IN_WORD(reverse_order) |                 \
          SOC_SAND_SET_TBL_WRITE_SIZE_BITS_IN_WORD((width_longs)* sizeof(uint32))


#define SOC_PETRA_TBL_REPS_RESET \
          soc_sand_indirect_set_nof_repetitions_unsafe(unit, 0);soc_petra_shd_indirect_set_nof_repetitions_unsafe(unit, 0);

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

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
void
  Soc_petra_tbls_nof_reps_clear(void)
{
  uint32
    dev_i;

  for (dev_i = 0; dev_i < SOC_SAND_MAX_DEVICE; dev_i++)
  {
    soc_sand_indirect_set_nof_repetitions_unsafe(dev_i, 0);                                                 \
    soc_petra_shd_indirect_set_nof_repetitions_unsafe(dev_i, 0);                                            \
  }
}

/* $Id: petra_tbl_access.c,v 1.8 Broadcom SDK $
 *  Generic "set repetitions' function.
 *  Most Soc_petra blocks have a "count" field in indirect access register.
 *  For those that don't (e.g. rtp, sch), this function enables a mechanism
 *  that mimics multiple write to the table, by requesting
 *  to loop on the entries from inner indirect write layer.
 */
STATIC uint32
  soc_petra_gen_set_reps_for_tbl_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN   uint32   nof_reps
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_GEN_SET_REPS_FOR_TBL_UNSAFE);

  SOC_SAND_ERR_IF_ABOVE_MAX
  (
    unit, (SOC_SAND_MAX_DEVICE - 1),
    SOC_PETRA_DEVICE_ID_ABOVE_OUT_OF_RANGE_ERR, 10, exit
  );

  soc_sand_indirect_set_nof_repetitions_unsafe(unit, nof_reps);
  soc_petra_shd_indirect_set_nof_repetitions_unsafe(unit, nof_reps+1); /* The shadowing code mimics he HW, decreasing by one. Add one here to compensate */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_gen_set_reps_for_tbl_unsafe()",0,0);
}

/*
 *  Set number of repetitions for table write.
 *  Each write command is executed
 *  IndirectCommandCount number of times. The address is
 *  advanced by one for each write command. If set to 0,
 *  only one operation is performed.
 */
uint32
  soc_petra_olp_set_reps_for_tbl_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN   uint32   nof_reps
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OLP_SET_REPS_FOR_TBL_UNSAFE);

  regs = soc_petra_regs();

  SOC_PETRA_FLD_SET(regs->olp.indirect_command_reg.indirect_command_count, nof_reps + 1, 10, exit);

  soc_petra_shd_indirect_set_nof_repetitions_unsafe(unit, nof_reps + 1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_olp_set_reps_for_tbl_unsafe()",SOC_PETRA_OLP_ID,0);
}

/*
 *  Set number of repetitions for table write.
 *  Each write command is executed
 *  IndirectCommandCount number of times. The address is
 *  advanced by one for each write command. If set to 0,
 *  only one operation is performed.
 */
uint32
  soc_petra_ire_set_reps_for_tbl_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN   uint32   nof_reps
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IRE_SET_REPS_FOR_TBL_UNSAFE);

  regs = soc_petra_regs();

  SOC_PETRA_FLD_SET(regs->ire.indirect_command_reg.indirect_command_count, nof_reps + 1, 10, exit);

  soc_petra_shd_indirect_set_nof_repetitions_unsafe(unit, nof_reps + 1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ire_set_reps_for_tbl_unsafe()",SOC_PETRA_IRE_ID,0);
}

/*
 *  Set number of repetitions for table write.
 *  Each write command is executed
 *  IndidrctCommandCount number of times. The address is
 *  advanced by one for each write command. If set to 0,
 *  only one operation is performed.
 */
uint32
  soc_petra_idr_set_reps_for_tbl_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN   uint32   nof_reps
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IDR_SET_REPS_FOR_TBL_UNSAFE);

  regs = soc_petra_regs();

  SOC_PETRA_FLD_SET(regs->idr.indirect_command_reg.indirect_command_count, nof_reps + 1, 10, exit);

  soc_petra_shd_indirect_set_nof_repetitions_unsafe(unit, nof_reps + 1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_idr_set_reps_for_tbl_unsafe()",SOC_PETRA_IDR_ID,0);
}

/*
 *  Set number of repetitions for table write.
 *  Each write command is executed
 *  IndirectCommandCount number of times. The address is
 *  advanced by one for each write command. If set to 0,
 *  only one operation is performed.
 */
uint32
  soc_petra_irr_set_reps_for_tbl_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN   uint32   nof_reps
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IRR_SET_REPS_FOR_TBL_UNSAFE);

  regs = soc_petra_regs();

  SOC_PETRA_FLD_SET(regs->irr.indirect_command_reg.indirect_command_count, nof_reps + 1, 10, exit);

  soc_petra_shd_indirect_set_nof_repetitions_unsafe(unit, nof_reps + 1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_irr_set_reps_for_tbl_unsafe()",SOC_PETRA_IRR_ID,0);
}

/*
 *  Set number of repetitions for table write.
 *  Each write command is executed
 *  IndirectCommandCount number of times. The address is
 *  advanced by one for each write command. If set to 0,
 *  only one operation is performed.
 */
uint32
  soc_petra_ihp_set_reps_for_tbl_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN   uint32   nof_reps
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_SET_REPS_FOR_TBL_UNSAFE);

  regs = soc_petra_regs();

  SOC_PETRA_FLD_SET(regs->ihp.indirect_command_reg.indirect_command_count, nof_reps + 1, 10, exit);

  soc_petra_shd_indirect_set_nof_repetitions_unsafe(unit, nof_reps + 1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ihp_set_reps_for_tbl_unsafe()",SOC_PETRA_IHP_ID,0);
}

/*
 *  Set number of repetitions for table write for QDR block
 *  Each write command is executed nof_reps times.
 *  The address is advanced by one for each write command.
 *  If set to 0 only one operation is performed.
 */
uint32
  soc_petra_qdr_set_reps_for_tbl_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              nof_reps
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_QDR_SET_REPS_FOR_TBL_UNSAFE);

  regs = soc_petra_regs();

  SOC_PB_FLD_SET(regs->qdr.indirect_command_reg.indirect_command_count, nof_reps + 1, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_qdr_set_reps_for_tbl_unsafe()", nof_reps, 0);
}

/*
 *  Set number of repetitions for table write.
 *  Each write command is executed
 *  IndirectCommandCount number of times. The address is
 *  advanced by one for each write command. If set to 0,
 *  only one operation is performed.
 */
uint32
  soc_petra_iqm_set_reps_for_tbl_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN   uint32   nof_reps
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_SET_REPS_FOR_TBL_UNSAFE);

  regs = soc_petra_regs();

  SOC_PETRA_FLD_SET(regs->iqm.indirect_command_reg.indirect_command_count, nof_reps + 1, 10, exit);

  soc_petra_shd_indirect_set_nof_repetitions_unsafe(unit, nof_reps + 1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_set_reps_for_tbl_unsafe()",SOC_PETRA_IQM_ID,0);
}

/*
 *  Set number of repetitions for table write.
 *  Each write command is executed
 *  IndirectCommandCount number of times. The address is
 *  advanced by one for each write command. If set to 0,
 *  only one operation is performed.
 */
uint32
  soc_petra_ips_set_reps_for_tbl_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN   uint32   nof_reps
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPS_SET_REPS_FOR_TBL_UNSAFE);

  regs = soc_petra_regs();

  SOC_PETRA_FLD_SET(regs->ips.indirect_command_reg.indirect_command_count, nof_reps + 1, 10, exit);

  soc_petra_shd_indirect_set_nof_repetitions_unsafe(unit, nof_reps + 1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ips_set_reps_for_tbl_unsafe()",SOC_PETRA_IPS_ID,0);
}



/*
 *  Set number of repetitions for table write.
 *  Each write command is executed
 *  IndirectCommandCount number of times. The address is
 *  advanced by one for each write command. If set to 0,
 *  only one operation is performed.
 */
uint32
  soc_petra_ipt_set_reps_for_tbl_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN   uint32   nof_reps
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPT_SET_REPS_FOR_TBL_UNSAFE);

  regs = soc_petra_regs();

  SOC_PETRA_FLD_SET(regs->ipt.indirect_command_reg.indirect_command_count, nof_reps + 1, 10, exit);

  soc_petra_shd_indirect_set_nof_repetitions_unsafe(unit, nof_reps + 1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ips_set_reps_for_tbl_unsafe()",SOC_PETRA_IPS_ID,0);
}
/*
 *  Set number of repetitions for table write.
 *  Each write command is executed
 *  IndirectCommandCount number of times. The address is
 *  advanced by one for each write command. If set to 0,
 *  only one operation is performed.
 */
uint32
  soc_petra_dpi_set_reps_for_tbl_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN   uint32  block_id,
    SOC_SAND_IN   uint32   nof_reps
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DPI_SET_REPS_FOR_TBL_UNSAFE);

  regs = soc_petra_regs();

  SOC_PETRA_FLD_SET(regs->dpi.indirect_command_reg.indirect_command_count, nof_reps + 1, 10, exit);

  soc_petra_shd_indirect_set_nof_repetitions_unsafe(unit, nof_reps + 1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_dpi_set_reps_for_tbl_unsafe()",block_id,0);
}

/*
 *  Set number of repetitions for table write.
 *  Each write command is executed
 *  IndirectCommandCount number of times. The address is
 *  advanced by one for each write command. If set to 0,
 *  only one operation is performed.
 */
uint32
  soc_petra_rtp_set_reps_for_tbl_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN   uint32   nof_reps
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_RTP_SET_REPS_FOR_TBL_UNSAFE);

  res = soc_petra_gen_set_reps_for_tbl_unsafe(
          unit,
          nof_reps
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_rtp_set_reps_for_tbl_unsafe()",SOC_PETRA_RTP_ID,0);
}

/*
 *  Set number of repetitions for table write.
 *  Each write command is executed
 *  IndirectCommandCount number of times. The address is
 *  advanced by one for each write command. If set to 0,
 *  only one operation is performed.
 */
uint32
  soc_petra_egq_set_reps_for_tbl_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN   uint32   nof_reps
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_SET_REPS_FOR_TBL_UNSAFE);

  regs = soc_petra_regs();

  SOC_PETRA_FLD_SET(regs->egq.indirect_command_reg.indirect_command_count, nof_reps + 1, 10, exit);

  soc_petra_shd_indirect_set_nof_repetitions_unsafe(unit, nof_reps + 1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_set_reps_for_tbl_unsafe()",SOC_PETRA_EGQ_ID,0);
}

/*
 *  Set number of repetitions for table write.
 *  Each write command is executed
 *  IndirectCommandCount number of times. The address is
 *  advanced by one for each write command. If set to 0,
 *  only one operation is performed.
 */
uint32
  soc_petra_cfc_set_reps_for_tbl_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN   uint32   nof_reps
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_CFC_SET_REPS_FOR_TBL_UNSAFE);

  regs = soc_petra_regs();

  SOC_PETRA_FLD_SET(regs->cfc.indirect_command_reg.indirect_command_count, nof_reps + 1, 10, exit);

  soc_petra_shd_indirect_set_nof_repetitions_unsafe(unit, nof_reps + 1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_cfc_set_reps_for_tbl_unsafe()",SOC_PETRA_CFC_ID,0);
}

/*
 *  Set number of repetitions for table write.
 *  Each write command is executed
 *  IndirectCommandCount number of times. The address is
 *  advanced by one for each write command. If set to 0,
 *  only one operation is performed.
 */
uint32
  soc_petra_sch_set_reps_for_tbl_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN   uint32   nof_reps
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_SET_REPS_FOR_TBL_UNSAFE);

  res = soc_petra_gen_set_reps_for_tbl_unsafe(
          unit,
          nof_reps
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_sch_set_reps_for_tbl_unsafe()",SOC_PETRA_SCH_ID,0);
}

/*
 *  Set number of repetitions for table write.
 *  Each write command is executed
 *  IndirectCommandCount number of times. The address is
 *  advanced by one for each write command. If set to 0,
 *  only one operation is performed.
 */
uint32
  soc_petra_epni_set_reps_for_tbl_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN   uint32   nof_reps
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EPNI_SET_REPS_FOR_TBL_UNSAFE);

  regs = soc_petra_regs();

  SOC_PETRA_FLD_SET(regs->epni.indirect_command_reg.indirect_command_count, nof_reps + 1, 10, exit);

  soc_petra_shd_indirect_set_nof_repetitions_unsafe(unit, nof_reps + 1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_epni_set_reps_for_tbl_unsafe()",SOC_PETRA_CFC_ID,0);
}

/*
 * Read indirect table pge_mem_tbl from block OLP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_olp_pge_mem_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_OLP_PGE_MEM_TBL_DATA* OLP_pge_mem_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_OLP_PGE_MEM_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_OLP_PGE_MEM_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OLP_PGE_MEM_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          OLP_pge_mem_tbl_data,
          0x0,
          sizeof(SOC_PETRA_OLP_PGE_MEM_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->olp.pge_mem_tbl);
  
  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_OLP_ID,
          SOC_PETRA_OLP_PGE_MEM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->pge_mem),
          &(OLP_pge_mem_tbl_data->pge_mem)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_olp_pge_mem_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table pge_mem_tbl from block OLP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_olp_pge_mem_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_OLP_PGE_MEM_TBL_DATA* OLP_pge_mem_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_OLP_PGE_MEM_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_OLP_PGE_MEM_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OLP_PGE_MEM_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->olp.pge_mem_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(OLP_pge_mem_tbl_data->pge_mem),
          &(tbl->pge_mem),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_OLP_ID,
          SOC_PETRA_OLP_PGE_MEM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_olp_pge_mem_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table nif_ctxt_map_tbl from block IRE,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ire_nif_ctxt_map_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IRE_NIF_CTXT_MAP_TBL_DATA* IRE_nif_ctxt_map_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IRE_NIF_CTXT_MAP_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IRE_NIF_CTXT_MAP_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IRE_NIF_CTXT_MAP_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IRE_nif_ctxt_map_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IRE_NIF_CTXT_MAP_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->ire.nif_ctxt_map_tbl), res;

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IRE_ID,
          SOC_PETRA_IRE_NIF_CTXT_MAP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->fap_port),
          &(IRE_nif_ctxt_map_tbl_data->fap_port)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ire_nif_ctxt_map_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table nif_ctxt_map_tbl from block IRE,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ire_nif_ctxt_map_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IRE_NIF_CTXT_MAP_TBL_DATA* IRE_nif_ctxt_map_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IRE_NIF_CTXT_MAP_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IRE_NIF_CTXT_MAP_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IRE_NIF_CTXT_MAP_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->ire.nif_ctxt_map_tbl), res;

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IRE_nif_ctxt_map_tbl_data->fap_port),
          &(tbl->fap_port),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IRE_ID,
          SOC_PETRA_IRE_NIF_CTXT_MAP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ire_nif_ctxt_map_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table nif_port2ctxt_bit_map_tbl from block IRE,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ire_nif_port2ctxt_bit_map_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IRE_NIF_PORT2CTXT_BIT_MAP_TBL_DATA* IRE_nif_port2ctxt_bit_map_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IRE_NIF_PORT2CTXT_BIT_MAP_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IRE_NIF_PORT2CTXT_BIT_MAP_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IRE_NIF_PORT2CTXT_BIT_MAP_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IRE_nif_port2ctxt_bit_map_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IRE_NIF_PORT2CTXT_BIT_MAP_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->ire.nif_port2ctxt_bit_map_tbl), res;

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IRE_ID,
          SOC_PETRA_IRE_NIF_PORT2CTXT_BIT_MAP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->contexts_bit_mapping),
          (IRE_nif_port2ctxt_bit_map_tbl_data->contexts_bit_mapping)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ire_nif_port2ctxt_bit_map_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table nif_port2ctxt_bit_map_tbl from block IRE,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ire_nif_port2ctxt_bit_map_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IRE_NIF_PORT2CTXT_BIT_MAP_TBL_DATA* IRE_nif_port2ctxt_bit_map_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IRE_NIF_PORT2CTXT_BIT_MAP_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IRE_NIF_PORT2CTXT_BIT_MAP_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IRE_NIF_PORT2CTXT_BIT_MAP_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->ire.nif_port2ctxt_bit_map_tbl), res;

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          (IRE_nif_port2ctxt_bit_map_tbl_data->contexts_bit_mapping),
          &(tbl->contexts_bit_mapping),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IRE_ID,
          SOC_PETRA_IRE_NIF_PORT2CTXT_BIT_MAP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ire_nif_port2ctxt_bit_map_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table rcy_ctxt_map_tbl from block IRE,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ire_rcy_ctxt_map_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IRE_RCY_CTXT_MAP_TBL_DATA* IRE_rcy_ctxt_map_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IRE_RCY_CTXT_MAP_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IRE_RCY_CTXT_MAP_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IRE_RCY_CTXT_MAP_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IRE_rcy_ctxt_map_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IRE_RCY_CTXT_MAP_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->ire.rcy_ctxt_map_tbl), res;

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IRE_ID,
          SOC_PETRA_IRE_RCY_CTXT_MAP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->fap_port),
          &(IRE_rcy_ctxt_map_tbl_data->fap_port)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ire_rcy_ctxt_map_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table rcy_ctxt_map_tbl from block IRE,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ire_rcy_ctxt_map_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IRE_RCY_CTXT_MAP_TBL_DATA* IRE_rcy_ctxt_map_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IRE_RCY_CTXT_MAP_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IRE_RCY_CTXT_MAP_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IRE_RCY_CTXT_MAP_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->ire.rcy_ctxt_map_tbl), res;

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IRE_rcy_ctxt_map_tbl_data->fap_port),
          &(tbl->fap_port),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IRE_ID,
          SOC_PETRA_IRE_RCY_CTXT_MAP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ire_rcy_ctxt_map_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table complete_pc_tbl from block IDR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_idr_complete_pc_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IDR_COMPLETE_PC_TBL_DATA* IDR_complete_pc_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IDR_COMPLETE_PC_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IDR_COMPLETE_PC_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IDR_COMPLETE_PC_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IDR_complete_pc_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IDR_COMPLETE_PC_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->idr.complete_pc_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IDR_ID,
          SOC_PETRA_IDR_COMPLETE_PC_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->pcb_pointer),
          &(IDR_complete_pc_tbl_data->pcb_pointer)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->count),
          &(IDR_complete_pc_tbl_data->count)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->ecc),
          &(IDR_complete_pc_tbl_data->ecc)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 42, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_idr_complete_pc_tbl_get_unsafe()",entry_offset,0);
}

/*
 * Write indirect table complete_pc_tbl from block IDR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_idr_complete_pc_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IDR_COMPLETE_PC_TBL_DATA* IDR_complete_pc_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IDR_COMPLETE_PC_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IDR_COMPLETE_PC_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IDR_COMPLETE_PC_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->idr.complete_pc_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IDR_complete_pc_tbl_data->pcb_pointer),
          &(tbl->pcb_pointer),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(IDR_complete_pc_tbl_data->count),
          &(tbl->count),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_petra_field_in_place_set(
          &(IDR_complete_pc_tbl_data->ecc),
          &(tbl->ecc),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IDR_ID,
          SOC_PETRA_IDR_COMPLETE_PC_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 33, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_idr_complete_pc_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table is_ingress_replication_db_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_irr_is_ingress_replication_db_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IRR_IS_INGRESS_REPLICATION_DB_TBL_DATA* IRR_is_ingress_replication_db_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IRR_IS_INGRESS_REPLICATION_DB_TBL_ENTRY_SIZE];
  uint32
    fld_idx;
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IRR_IS_INGRESS_REPLICATION_DB_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IRR_IS_INGRESS_REPLICATION_DB_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IRR_is_ingress_replication_db_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IRR_IS_INGRESS_REPLICATION_DB_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->irr.is_ingress_replication_db_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IRR_ID,
          SOC_PETRA_IRR_IS_INGRESS_REPLICATION_DB_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  for (fld_idx=0 ; fld_idx < SOC_PETRA_MULT_ING_REP_TBL_NOF_FLDS_PER_DEVICE; fld_idx++)
  {
    err = soc_petra_field_in_place_get(
            data,
            &(tbl->is_ingress_replication[fld_idx]),
            &(IRR_is_ingress_replication_db_tbl_data->is_ingress_replication[fld_idx])
          );
    SOC_SAND_CHECK_FUNC_RESULT(err, 40+fld_idx, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_irr_is_ingress_replication_db_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table is_ingress_replication_db_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_irr_is_ingress_replication_db_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IRR_IS_INGRESS_REPLICATION_DB_TBL_DATA* IRR_is_ingress_replication_db_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IRR_IS_INGRESS_REPLICATION_DB_TBL_ENTRY_SIZE];
  uint32
    fld_idx;
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IRR_IS_INGRESS_REPLICATION_DB_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IRR_IS_INGRESS_REPLICATION_DB_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->irr.is_ingress_replication_db_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  for (fld_idx = 0 ; fld_idx < SOC_PETRA_MULT_ING_REP_TBL_NOF_FLDS_PER_DEVICE; fld_idx++)
  {
    err = soc_petra_field_in_place_set(
      &(IRR_is_ingress_replication_db_tbl_data->is_ingress_replication[fld_idx]),
      &(tbl->is_ingress_replication[fld_idx]),
      data
      );
    SOC_SAND_CHECK_FUNC_RESULT(err, 30+fld_idx, exit);
  }

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IRR_ID,
          SOC_PETRA_IRR_IS_INGRESS_REPLICATION_DB_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 34, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_irr_is_ingress_replication_db_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table egress_replication_multicast_db_tbl from block IRR,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_irr_egress_replication_multicast_db_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IRR_EGRESS_REPLICATION_MULTICAST_DB_TBL_DATA* IRR_egress_replication_multicast_db_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IRR_ING_EG_REPLICATION_MULTICAST_DB_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IRR_EGRESS_REPLICATION_MULTICAST_DB_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IRR_EGRESS_REPLICATION_MULTICAST_DB_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 5, exit);

  err = SOC_SAND_OK; sal_memset(
          IRR_egress_replication_multicast_db_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IRR_EGRESS_REPLICATION_MULTICAST_DB_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 6, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 7, exit);

  tbl = SOC_PETRA_TBL_REF(tables->irr.egress_replication_multicast_db_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IRR_ID,
          SOC_PETRA_IRR_ING_EG_REPLICATION_MULTICAST_DB_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->out_lif1),
          &(IRR_egress_replication_multicast_db_tbl_data->out_lif[0])
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->out_port1),
          &(IRR_egress_replication_multicast_db_tbl_data->port[0])
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 21, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->out_lif2),
          &(IRR_egress_replication_multicast_db_tbl_data->out_lif[1])
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 22, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->out_port2),
          &(IRR_egress_replication_multicast_db_tbl_data->port[1])
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 23, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->out_lif3),
          &(IRR_egress_replication_multicast_db_tbl_data->out_lif[2])
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 24, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->out_port3),
          &(IRR_egress_replication_multicast_db_tbl_data->port[2])
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 25, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->link_ptr),
          &(IRR_egress_replication_multicast_db_tbl_data->link_ptr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 26, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_irr_egress_replication_multicast_db_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table egress_replication_multicast_db_tbl from block IRR,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_irr_egress_replication_multicast_db_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IRR_EGRESS_REPLICATION_MULTICAST_DB_TBL_DATA* IRR_egress_replication_multicast_db_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IRR_ING_EG_REPLICATION_MULTICAST_DB_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IRR_EGRESS_REPLICATION_MULTICAST_DB_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IRR_EGRESS_REPLICATION_MULTICAST_DB_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 5, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 6, exit);

  tbl = SOC_PETRA_TBL_REF(tables->irr.egress_replication_multicast_db_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IRR_egress_replication_multicast_db_tbl_data->out_lif[0]),
          &(tbl->out_lif1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  err = soc_petra_field_in_place_set(
          &(IRR_egress_replication_multicast_db_tbl_data->port[0]),
          &(tbl->out_port1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 21, exit);

  err = soc_petra_field_in_place_set(
          &(IRR_egress_replication_multicast_db_tbl_data->out_lif[1]),
          &(tbl->out_lif2),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 22, exit);

  err = soc_petra_field_in_place_set(
          &(IRR_egress_replication_multicast_db_tbl_data->port[1]),
          &(tbl->out_port2),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 23, exit);

  err = soc_petra_field_in_place_set(
          &(IRR_egress_replication_multicast_db_tbl_data->out_lif[2]),
          &(tbl->out_lif3),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 24, exit);

  err = soc_petra_field_in_place_set(
          &(IRR_egress_replication_multicast_db_tbl_data->port[2]),
          &(tbl->out_port3),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 25, exit);

  err = soc_petra_field_in_place_set(
          &(IRR_egress_replication_multicast_db_tbl_data->link_ptr),
          &(tbl->link_ptr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 26, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IRR_ID,
          SOC_PETRA_IRR_ING_EG_REPLICATION_MULTICAST_DB_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 3, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_irr_egress_replication_multicast_db_tbl_set_unsafe()",0,0);
}



/*
 * Read indirect table ingress_replication_multicast_db_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_irr_ingress_replication_multicast_db_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL_DATA* IRR_ingress_replication_multicast_db_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IRR_ING_EG_REPLICATION_MULTICAST_DB_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 5, exit);

  err = SOC_SAND_OK; sal_memset(
          IRR_ingress_replication_multicast_db_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 6, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 7, exit);

  tbl = SOC_PETRA_TBL_REF(tables->irr.ingress_replication_multicast_db_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IRR_ID,
          SOC_PETRA_IRR_ING_EG_REPLICATION_MULTICAST_DB_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->base_queue1),
          &(IRR_ingress_replication_multicast_db_tbl_data->base_queue[0])
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->is_queue_number1),
          &(IRR_ingress_replication_multicast_db_tbl_data->is_queue_number[0])
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 21, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->base_queue2),
          &(IRR_ingress_replication_multicast_db_tbl_data->base_queue[1])
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 22, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->is_queue_number2),
          &(IRR_ingress_replication_multicast_db_tbl_data->is_queue_number[1])
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 23, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->out_lif1),
          &(IRR_ingress_replication_multicast_db_tbl_data->out_lif[0])
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 24, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->out_lif2),
          &(IRR_ingress_replication_multicast_db_tbl_data->out_lif[1])
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 25, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->egress_data),
          &(IRR_ingress_replication_multicast_db_tbl_data->egress_data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 26, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->link_ptr),
          &(IRR_ingress_replication_multicast_db_tbl_data->link_ptr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 27, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_irr_ingress_replication_multicast_db_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table ingress_replication_multicast_db_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_irr_ingress_replication_multicast_db_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL_DATA* IRR_ingress_replication_multicast_db_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IRR_ING_EG_REPLICATION_MULTICAST_DB_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 5, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 1, exit);

  tbl = SOC_PETRA_TBL_REF(tables->irr.ingress_replication_multicast_db_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IRR_ingress_replication_multicast_db_tbl_data->base_queue[0]),
          &(tbl->base_queue1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  err = soc_petra_field_in_place_set(
          &(IRR_ingress_replication_multicast_db_tbl_data->is_queue_number[0]),
          &(tbl->is_queue_number1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 21, exit);

  err = soc_petra_field_in_place_set(
          &(IRR_ingress_replication_multicast_db_tbl_data->base_queue[1]),
          &(tbl->base_queue2),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 22, exit);

  err = soc_petra_field_in_place_set(
          &(IRR_ingress_replication_multicast_db_tbl_data->is_queue_number[1]),
          &(tbl->is_queue_number2),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 23, exit);

  err = soc_petra_field_in_place_set(
          &(IRR_ingress_replication_multicast_db_tbl_data->out_lif[0]),
          &(tbl->out_lif1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 24, exit);

  err = soc_petra_field_in_place_set(
          &(IRR_ingress_replication_multicast_db_tbl_data->out_lif[1]),
          &(tbl->out_lif2),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 25, exit);

  err = soc_petra_field_in_place_set(
          &(IRR_ingress_replication_multicast_db_tbl_data->egress_data),
          &(tbl->egress_data),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 26, exit);

  err = soc_petra_field_in_place_set(
          &(IRR_ingress_replication_multicast_db_tbl_data->link_ptr),
          &(tbl->link_ptr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 27, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IRR_ID,
          SOC_PETRA_IRR_ING_EG_REPLICATION_MULTICAST_DB_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 3, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_irr_ingress_replication_multicast_db_tbl_set_unsafe()",0,0);
}


/*
 * Read indirect table mirror_table_tbl from block IRR,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_irr_mirror_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IRR_MIRROR_TABLE_TBL_DATA* IRR_mirror_table_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IRR_MIRROR_TABLE_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IRR_MIRROR_TABLE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IRR_MIRROR_TABLE_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IRR_mirror_table_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IRR_MIRROR_TABLE_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->irr.mirror_table_tbl), res;

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IRR_ID,
          SOC_PETRA_IRR_MIRROR_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->mirror_destination),
          &(IRR_mirror_table_tbl_data->mirror_destination)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->is_queue_number),
          &(IRR_mirror_table_tbl_data->is_queue_number)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->is_outbound_mirror),
          &(IRR_mirror_table_tbl_data->is_outbound_mirror)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 42, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->mirror_traffic_class),
          &(IRR_mirror_table_tbl_data->mirror_traffic_class)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 43, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->traffic_class_over_write),
          &(IRR_mirror_table_tbl_data->traffic_class_over_write)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 44, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->mirror_drop_precedence),
          &(IRR_mirror_table_tbl_data->mirror_drop_precedence)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 45, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->drop_precedence_over_write),
          &(IRR_mirror_table_tbl_data->drop_precedence_over_write)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 46, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_irr_mirror_table_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table mirror_table_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_irr_mirror_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IRR_MIRROR_TABLE_TBL_DATA* IRR_mirror_table_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IRR_MIRROR_TABLE_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IRR_MIRROR_TABLE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IRR_MIRROR_TABLE_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->irr.mirror_table_tbl), res;

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
    &(IRR_mirror_table_tbl_data->is_queue_number),
    &(tbl->is_queue_number),
    data
    );
  SOC_SAND_CHECK_FUNC_RESULT(err, 33, exit);
  err = soc_petra_field_in_place_set(
    &(IRR_mirror_table_tbl_data->is_multicast),
    &(tbl->is_multicast),
    data
    );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);
  err = soc_petra_field_in_place_set(
    &(IRR_mirror_table_tbl_data->mirror_destination),
    &(tbl->mirror_destination),
    data
    );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(IRR_mirror_table_tbl_data->is_outbound_mirror),
          &(tbl->is_outbound_mirror),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 34, exit);

  err = soc_petra_field_in_place_set(
          &(IRR_mirror_table_tbl_data->mirror_traffic_class),
          &(tbl->mirror_traffic_class),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 35, exit);

  err = soc_petra_field_in_place_set(
          &(IRR_mirror_table_tbl_data->traffic_class_over_write),
          &(tbl->traffic_class_over_write),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 36, exit);

  err = soc_petra_field_in_place_set(
          &(IRR_mirror_table_tbl_data->mirror_drop_precedence),
          &(tbl->mirror_drop_precedence),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 37, exit);

  err = soc_petra_field_in_place_set(
          &(IRR_mirror_table_tbl_data->drop_precedence_over_write),
          &(tbl->drop_precedence_over_write),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 38, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IRR_ID,
          SOC_PETRA_IRR_MIRROR_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 39, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_irr_mirror_table_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table snoop_table_tbl from block IRR,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_irr_snoop_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IRR_SNOOP_TABLE_TBL_DATA* IRR_snoop_table_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IRR_SNOOP_TABLE_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IRR_SNOOP_TABLE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IRR_SNOOP_TABLE_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IRR_snoop_table_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IRR_SNOOP_TABLE_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->irr.snoop_table_tbl), res;

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IRR_ID,
          SOC_PETRA_IRR_SNOOP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->snoop_destination),
          &(IRR_snoop_table_tbl_data->snoop_destination)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->is_multicast),
          &(IRR_snoop_table_tbl_data->is_multicast)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 42, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->is_queue_number),
          &(IRR_snoop_table_tbl_data->is_queue_number)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 43, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->snoop_traffic_class),
          &(IRR_snoop_table_tbl_data->snoop_traffic_class)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 44, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->traffic_class_over_write),
          &(IRR_snoop_table_tbl_data->traffic_class_over_write)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 45, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->snoop_drop_precedence),
          &(IRR_snoop_table_tbl_data->snoop_drop_precedence)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 46, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->drop_precedence_over_write),
          &(IRR_snoop_table_tbl_data->drop_precedence_over_write)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 47, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_irr_snoop_table_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table snoop_table_tbl from block IRR,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_irr_snoop_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IRR_SNOOP_TABLE_TBL_DATA* IRR_snoop_table_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IRR_SNOOP_TABLE_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IRR_SNOOP_TABLE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IRR_SNOOP_TABLE_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->irr.snoop_table_tbl), res;

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IRR_snoop_table_tbl_data->snoop_destination),
          &(tbl->snoop_destination),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(IRR_snoop_table_tbl_data->is_queue_number),
          &(tbl->is_queue_number),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_petra_field_in_place_set(
          &(IRR_snoop_table_tbl_data->snoop_traffic_class),
          &(tbl->snoop_traffic_class),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

  err = soc_petra_field_in_place_set(
          &(IRR_snoop_table_tbl_data->traffic_class_over_write),
          &(tbl->traffic_class_over_write),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 33, exit);

  err = soc_petra_field_in_place_set(
          &(IRR_snoop_table_tbl_data->snoop_drop_precedence),
          &(tbl->snoop_drop_precedence),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 34, exit);

  err = soc_petra_field_in_place_set(
          &(IRR_snoop_table_tbl_data->drop_precedence_over_write),
          &(tbl->drop_precedence_over_write),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 35, exit);

  err = soc_petra_field_in_place_set(
          &(IRR_snoop_table_tbl_data->is_multicast),
          &(tbl->is_multicast),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 36, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IRR_ID,
          SOC_PETRA_IRR_SNOOP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 36, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_irr_snoop_table_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table glag_to_lag_range_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_irr_glag_to_lag_range_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IRR_GLAG_TO_LAG_RANGE_TBL_DATA* IRR_glag_to_lag_range_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IRR_GLAG_TO_LAG_RANGE_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IRR_GLAG_TO_LAG_RANGE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IRR_GLAG_TO_LAG_RANGE_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IRR_glag_to_lag_range_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IRR_GLAG_TO_LAG_RANGE_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->irr.glag_to_lag_range_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IRR_ID,
          SOC_PETRA_IRR_GLAG_TO_LAG_RANGE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->glag_to_lag_range),
          &(IRR_glag_to_lag_range_tbl_data->glag_to_lag_range)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_irr_glag_to_lag_range_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table glag_to_lag_range_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_irr_glag_to_lag_range_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IRR_GLAG_TO_LAG_RANGE_TBL_DATA* IRR_glag_to_lag_range_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IRR_GLAG_TO_LAG_RANGE_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IRR_GLAG_TO_LAG_RANGE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IRR_GLAG_TO_LAG_RANGE_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->irr.glag_to_lag_range_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IRR_glag_to_lag_range_tbl_data->glag_to_lag_range),
          &(tbl->glag_to_lag_range),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IRR_ID,
          SOC_PETRA_IRR_GLAG_TO_LAG_RANGE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_irr_glag_to_lag_range_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table smooth_division_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_irr_smooth_division_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              lag_size,
    SOC_SAND_IN   uint32              hash_val,
    SOC_SAND_OUT  SOC_PETRA_IRR_SMOOTH_DIVISION_TBL_DATA* IRR_smooth_division_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    entry_offset,
    data[SOC_PETRA_IRR_SMOOTH_DIVISION_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IRR_SMOOTH_DIVISION_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IRR_SMOOTH_DIVISION_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IRR_smooth_division_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IRR_SMOOTH_DIVISION_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->irr.smooth_division_tbl);

  entry_offset = lag_size << SOC_PETRA_IRR_GLAG_DEVISION_HASH_NOF_BITS;
  entry_offset |= hash_val;

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IRR_ID,
          SOC_PETRA_IRR_SMOOTH_DIVISION_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->smooth_division),
          &(IRR_smooth_division_tbl_data->smooth_division)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_irr_smooth_division_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table smooth_division_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_irr_smooth_division_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              lag_size,
    SOC_SAND_IN   uint32              hash_val,
    SOC_SAND_IN   SOC_PETRA_IRR_SMOOTH_DIVISION_TBL_DATA* IRR_smooth_division_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    entry_offset,
    data[SOC_PETRA_IRR_SMOOTH_DIVISION_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IRR_SMOOTH_DIVISION_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IRR_SMOOTH_DIVISION_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->irr.smooth_division_tbl);

  entry_offset = lag_size << SOC_PETRA_IRR_GLAG_DEVISION_HASH_NOF_BITS;
  entry_offset |= hash_val;

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IRR_smooth_division_tbl_data->smooth_division),
          &(tbl->smooth_division),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IRR_ID,
          SOC_PETRA_IRR_SMOOTH_DIVISION_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_irr_smooth_division_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table glag_mapping_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_irr_glag_mapping_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              lag_ndx,
    SOC_SAND_IN   uint32              port_ndx,
    SOC_SAND_OUT  SOC_PETRA_IRR_GLAG_MAPPING_TBL_DATA* IRR_glag_mapping_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    entry_offset,
    offset  = 0,
    data[SOC_PETRA_IRR_GLAG_MAPPING_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IRR_GLAG_MAPPING_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IRR_GLAG_MAPPING_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IRR_glag_mapping_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IRR_GLAG_MAPPING_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->irr.glag_mapping_tbl);

  entry_offset = 0;
  entry_offset =
    (lag_ndx << SOC_PETRA_IRR_GLAG_MAPPING_LAG_PORT_INDEX_NOF_BITS) | port_ndx;

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IRR_ID,
          SOC_PETRA_IRR_GLAG_MAPPING_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->glag_mapping),
          &(IRR_glag_mapping_tbl_data->glag_mapping)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_irr_glag_mapping_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table glag_mapping_tbl from block IRR,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_irr_glag_mapping_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              lag_ndx,
    SOC_SAND_IN   uint32              port_ndx,
    SOC_SAND_IN   SOC_PETRA_IRR_GLAG_MAPPING_TBL_DATA* IRR_glag_mapping_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    entry_offset,
    offset  = 0,
    data[SOC_PETRA_IRR_GLAG_MAPPING_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IRR_GLAG_MAPPING_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IRR_GLAG_MAPPING_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->irr.glag_mapping_tbl);

  entry_offset = 0;
  entry_offset =
    (lag_ndx << SOC_PETRA_IRR_GLAG_MAPPING_LAG_PORT_INDEX_NOF_BITS) | port_ndx;

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IRR_glag_mapping_tbl_data->glag_mapping),
          &(tbl->glag_mapping),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IRR_ID,
          SOC_PETRA_IRR_GLAG_MAPPING_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_irr_glag_mapping_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table destination_table_tbl from block IRR,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_irr_destination_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IRR_DESTINATION_TABLE_TBL_DATA* IRR_destination_table_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IRR_DESTINATION_TABLE_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IRR_DESTINATION_TABLE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IRR_DESTINATION_TABLE_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IRR_destination_table_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IRR_DESTINATION_TABLE_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->irr.destination_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IRR_ID,
          SOC_PETRA_IRR_DESTINATION_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->queue_number),
          &(IRR_destination_table_tbl_data->queue_number)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->queue_valid),
          &(IRR_destination_table_tbl_data->queue_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_irr_destination_table_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table destination_table_tbl from block IRR,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_irr_destination_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IRR_DESTINATION_TABLE_TBL_DATA* IRR_destination_table_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IRR_DESTINATION_TABLE_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IRR_DESTINATION_TABLE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IRR_DESTINATION_TABLE_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->irr.destination_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IRR_destination_table_tbl_data->queue_number),
          &(tbl->queue_number),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(IRR_destination_table_tbl_data->queue_valid),
          &(tbl->queue_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IRR_ID,
          SOC_PETRA_IRR_DESTINATION_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_irr_destination_table_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table glag_next_member_tbl from block IRR,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_irr_glag_next_member_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IRR_GLAG_NEXT_MEMBER_TBL_DATA* IRR_glag_next_member_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IRR_GLAG_NEXT_MEMBER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IRR_GLAG_NEXT_MEMBER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IRR_GLAG_NEXT_MEMBER_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IRR_glag_next_member_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IRR_GLAG_NEXT_MEMBER_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->irr.glag_next_member_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IRR_ID,
          SOC_PETRA_IRR_GLAG_NEXT_MEMBER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->offset),
          &(IRR_glag_next_member_tbl_data->offset)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->rr_lb_mode),
          &(IRR_glag_next_member_tbl_data->rr_lb_mode)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_irr_glag_next_member_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table glag_next_member_tbl from block IRR,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_irr_glag_next_member_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IRR_GLAG_NEXT_MEMBER_TBL_DATA* IRR_glag_next_member_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IRR_GLAG_NEXT_MEMBER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IRR_GLAG_NEXT_MEMBER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IRR_GLAG_NEXT_MEMBER_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->irr.glag_next_member_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IRR_glag_next_member_tbl_data->offset),
          &(tbl->offset),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(IRR_glag_next_member_tbl_data->rr_lb_mode),
          &(tbl->rr_lb_mode),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IRR_ID,
          SOC_PETRA_IRR_GLAG_NEXT_MEMBER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_irr_glag_next_member_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table rlag_next_member_tbl from block IRR,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_irr_rlag_next_member_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IRR_RLAG_NEXT_MEMBER_TBL_DATA* IRR_rlag_next_member_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IRR_RLAG_NEXT_MEMBER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IRR_RLAG_NEXT_MEMBER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IRR_RLAG_NEXT_MEMBER_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IRR_rlag_next_member_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IRR_RLAG_NEXT_MEMBER_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->irr.rlag_next_member_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IRR_ID,
          SOC_PETRA_IRR_RLAG_NEXT_MEMBER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->two_lsb),
          &(IRR_rlag_next_member_tbl_data->two_lsb)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->two_msb),
          &(IRR_rlag_next_member_tbl_data->two_msb)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_irr_rlag_next_member_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table rlag_next_member_tbl from block IRR,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_irr_rlag_next_member_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IRR_RLAG_NEXT_MEMBER_TBL_DATA* IRR_rlag_next_member_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IRR_RLAG_NEXT_MEMBER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IRR_RLAG_NEXT_MEMBER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IRR_RLAG_NEXT_MEMBER_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->irr.rlag_next_member_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IRR_rlag_next_member_tbl_data->two_lsb),
          &(tbl->two_lsb),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(IRR_rlag_next_member_tbl_data->two_msb),
          &(tbl->two_msb),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IRR_ID,
          SOC_PETRA_IRR_RLAG_NEXT_MEMBER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_irr_rlag_next_member_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table port_info_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ihp_port_info_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IHP_PORT_INFO_TBL_DATA* IHP_port_info_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IHP_PORT_INFO_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IHP_PORT_INFO_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_PORT_INFO_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IHP_port_info_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IHP_PORT_INFO_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->ihp.port_info_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IHP_ID,
          SOC_PETRA_IHP_PORT_INFO_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->port_type),
          &(IHP_port_info_tbl_data->port_type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->bytes_to_skip),
          &(IHP_port_info_tbl_data->bytes_to_skip)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->custom_command_select),
          &(IHP_port_info_tbl_data->custom_command_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 42, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->statistics_profile_extension),
          &(IHP_port_info_tbl_data->statistics_profile_extension)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 43, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->port_type_extension),
          &(IHP_port_info_tbl_data->port_type_extension)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 45, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->statistics_profile),
          &(IHP_port_info_tbl_data->statistics_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 46, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->use_lag_member),
          &(IHP_port_info_tbl_data->use_lag_member)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 47, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->has_mirror),
          &(IHP_port_info_tbl_data->has_mirror)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 48, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->mirror_is_mc),
          &(IHP_port_info_tbl_data->mirror_is_mc)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 49, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->bytes_to_remove),
          &(IHP_port_info_tbl_data->bytes_to_remove)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 50, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->header_remove),
          &(IHP_port_info_tbl_data->header_remove)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 51, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->append_ftmh),
          &(IHP_port_info_tbl_data->append_ftmh)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 52, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->append_prog_header),
          &(IHP_port_info_tbl_data->append_prog_header)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 53, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ihp_port_info_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table port_info_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ihp_port_info_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IHP_PORT_INFO_TBL_DATA* IHP_port_info_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    nof_reps,
    data[SOC_PETRA_IHP_PORT_INFO_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IHP_PORT_INFO_TBL
    *tbl;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_PORT_INFO_TBL_SET_UNSAFE);

  regs = soc_petra_regs();

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->ihp.port_info_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;
 /*
  * get counter before get data and store it a side.
  */

  SOC_PA_FLD_GET(regs->ihp.indirect_command_reg.indirect_command_count, nof_reps, 20, exit);

  /*
  * get Data before read cause this table is shared.
  */
  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IHP_ID,
          SOC_PETRA_IHP_PORT_INFO_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 25, exit);
 /*
  * restore counter
  */

  err = soc_petra_ihp_set_reps_for_tbl_unsafe(unit, nof_reps);
  SOC_SAND_CHECK_FUNC_RESULT(err, 27, exit);

  err = soc_petra_field_in_place_set(
          &(IHP_port_info_tbl_data->port_type),
          &(tbl->port_type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(IHP_port_info_tbl_data->bytes_to_skip),
          &(tbl->bytes_to_skip),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_petra_field_in_place_set(
          &(IHP_port_info_tbl_data->custom_command_select),
          &(tbl->custom_command_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

  err = soc_petra_field_in_place_set(
          &(IHP_port_info_tbl_data->statistics_profile_extension),
          &(tbl->statistics_profile_extension),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 33, exit);


  err = soc_petra_field_in_place_set(
          &(IHP_port_info_tbl_data->port_type_extension),
          &(tbl->port_type_extension),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 35, exit);

  err = soc_petra_field_in_place_set(
          &(IHP_port_info_tbl_data->statistics_profile),
          &(tbl->statistics_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 36, exit);

  err = soc_petra_field_in_place_set(
          &(IHP_port_info_tbl_data->use_lag_member),
          &(tbl->use_lag_member),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 37, exit);

  err = soc_petra_field_in_place_set(
          &(IHP_port_info_tbl_data->has_mirror),
          &(tbl->has_mirror),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 38, exit);

  err = soc_petra_field_in_place_set(
          &(IHP_port_info_tbl_data->mirror_is_mc),
          &(tbl->mirror_is_mc),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 39, exit);

  err = soc_petra_field_in_place_set(
          &(IHP_port_info_tbl_data->bytes_to_remove),
          &(tbl->bytes_to_remove),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_set(
          &(IHP_port_info_tbl_data->header_remove),
          &(tbl->header_remove),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

  err = soc_petra_field_in_place_set(
          &(IHP_port_info_tbl_data->append_ftmh),
          &(tbl->append_ftmh),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 42, exit);

  err = soc_petra_field_in_place_set(
          &(IHP_port_info_tbl_data->append_prog_header),
          &(tbl->append_prog_header),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 43, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IHP_ID,
          SOC_PETRA_IHP_PORT_INFO_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 44, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ihp_port_info_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table port_to_system_port_id_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ihp_port_to_system_port_id_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IHP_PORT_TO_SYSTEM_PORT_ID_TBL_DATA* IHP_port_to_system_port_id_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IHP_PORT_TO_SYSTEM_PORT_ID_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IHP_PORT_TO_SYSTEM_PORT_ID_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_PORT_TO_SYSTEM_PORT_ID_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IHP_port_to_system_port_id_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IHP_PORT_TO_SYSTEM_PORT_ID_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->ihp.port_to_system_port_id_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IHP_ID,
          SOC_PETRA_IHP_PORT_TO_SYSTEM_PORT_ID_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->port_to_system_port_id),
          &(IHP_port_to_system_port_id_tbl_data->port_to_system_port_id)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ihp_port_to_system_port_id_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table port_to_system_port_id_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ihp_port_to_system_port_id_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IHP_PORT_TO_SYSTEM_PORT_ID_TBL_DATA* IHP_port_to_system_port_id_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IHP_PORT_TO_SYSTEM_PORT_ID_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IHP_PORT_TO_SYSTEM_PORT_ID_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_PORT_TO_SYSTEM_PORT_ID_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->ihp.port_to_system_port_id_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IHP_port_to_system_port_id_tbl_data->port_to_system_port_id),
          &(tbl->port_to_system_port_id),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IHP_ID,
          SOC_PETRA_IHP_PORT_TO_SYSTEM_PORT_ID_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ihp_port_to_system_port_id_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table static_header_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ihp_static_header_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IHP_STATIC_HEADER_TBL_DATA* IHP_static_header_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IHP_STATIC_HEADER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IHP_STATIC_HEADER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_STATIC_HEADER_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IHP_static_header_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IHP_STATIC_HEADER_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->ihp.static_header_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IHP_ID,
          SOC_PETRA_IHP_STATIC_HEADER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->shaping),
          &(IHP_static_header_tbl_data->shaping)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->shaping_itmh),
          &(IHP_static_header_tbl_data->shaping_itmh)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->itmh),
          (IHP_static_header_tbl_data->itmh)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 42, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ihp_static_header_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table static_header_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ihp_static_header_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IHP_STATIC_HEADER_TBL_DATA* IHP_static_header_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IHP_STATIC_HEADER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IHP_STATIC_HEADER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_STATIC_HEADER_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->ihp.static_header_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IHP_static_header_tbl_data->shaping),
          &(tbl->shaping),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(IHP_static_header_tbl_data->shaping_itmh),
          &(tbl->shaping_itmh),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_petra_field_in_place_set(
          (IHP_static_header_tbl_data->itmh),
          &(tbl->itmh),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IHP_ID,
          SOC_PETRA_IHP_STATIC_HEADER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 33, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ihp_static_header_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table system_port_my_port_table_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ihp_system_port_my_port_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IHP_SYSTEM_PORT_MY_PORT_TABLE_TBL_DATA* IHP_system_port_my_port_table_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IHP_SYSTEM_PORT_MY_PORT_TABLE_TBL_ENTRY_SIZE];
  uint32
    fld_idx = 0;
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IHP_SYSTEM_PORT_MY_PORT_TABLE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_SYSTEM_PORT_MY_PORT_TABLE_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IHP_system_port_my_port_table_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IHP_SYSTEM_PORT_MY_PORT_TABLE_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->ihp.system_port_my_port_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IHP_ID,
          SOC_PETRA_IHP_SYSTEM_PORT_MY_PORT_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  for (fld_idx = 0; fld_idx < SOC_PETRA_IHP_SYS_MY_PORT_TBL_FLDS; fld_idx++)
  {
    err = soc_petra_field_in_place_get(
          data,
          &(tbl->system_port_my_port_table[fld_idx]),
          &(IHP_system_port_my_port_table_tbl_data->system_port_my_port_table[fld_idx])
        );
    SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ihp_system_port_my_port_table_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table system_port_my_port_table_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ihp_system_port_my_port_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IHP_SYSTEM_PORT_MY_PORT_TABLE_TBL_DATA* IHP_system_port_my_port_table_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IHP_SYSTEM_PORT_MY_PORT_TABLE_TBL_ENTRY_SIZE];
  uint32
    fld_idx = 0;
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IHP_SYSTEM_PORT_MY_PORT_TABLE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_SYSTEM_PORT_MY_PORT_TABLE_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->ihp.system_port_my_port_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  for (fld_idx = 0; fld_idx < SOC_PETRA_IHP_SYS_MY_PORT_TBL_FLDS; fld_idx++)
  {
    err = soc_petra_field_in_place_set(
            &(IHP_system_port_my_port_table_tbl_data->system_port_my_port_table[fld_idx]),
            &(tbl->system_port_my_port_table[fld_idx]),
            data
          );
    SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);
  }

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IHP_ID,
          SOC_PETRA_IHP_SYSTEM_PORT_MY_PORT_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ihp_system_port_my_port_table_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table ptc_commands1_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ihp_ptc_commands1_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IHP_PTC_COMMANDS_TBL_DATA* IHP_ptc_commands1_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IHP_PTC_COMMANDS1_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IHP_PTC_COMMANDS1_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_PTC_COMMANDS1_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IHP_ptc_commands1_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IHP_PTC_COMMANDS_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->ihp.ptc_commands1_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IHP_ID,
          SOC_PETRA_IHP_PTC_COMMANDS1_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->ptc_commands),
          (IHP_ptc_commands1_tbl_data->ptc_commands)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ihp_ptc_commands1_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table ptc_commands1_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ihp_ptc_commands1_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IHP_PTC_COMMANDS_TBL_DATA* IHP_ptc_commands1_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IHP_PTC_COMMANDS1_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IHP_PTC_COMMANDS1_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_PTC_COMMANDS1_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->ihp.ptc_commands1_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          (IHP_ptc_commands1_tbl_data->ptc_commands),
          &(tbl->ptc_commands),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IHP_ID,
          SOC_PETRA_IHP_PTC_COMMANDS1_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ihp_ptc_commands1_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table ptc_commands2_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ihp_ptc_commands2_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IHP_PTC_COMMANDS_TBL_DATA* IHP_ptc_commands2_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IHP_PTC_COMMANDS2_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IHP_PTC_COMMANDS2_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_PTC_COMMANDS2_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IHP_ptc_commands2_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IHP_PTC_COMMANDS_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->ihp.ptc_commands2_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IHP_ID,
          SOC_PETRA_IHP_PTC_COMMANDS2_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->ptc_commands),
          (IHP_ptc_commands2_tbl_data->ptc_commands)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ihp_ptc_commands2_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table ptc_commands2_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ihp_ptc_commands2_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IHP_PTC_COMMANDS_TBL_DATA* IHP_ptc_commands2_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IHP_PTC_COMMANDS2_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IHP_PTC_COMMANDS2_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_PTC_COMMANDS2_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->ihp.ptc_commands2_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          (IHP_ptc_commands2_tbl_data->ptc_commands),
          &(tbl->ptc_commands),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IHP_ID,
          SOC_PETRA_IHP_PTC_COMMANDS2_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ihp_ptc_commands2_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table ptc_key_program_lut_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ihp_ptc_key_program_lut_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IHP_PTC_KEY_PROGRAM_LUT_TBL_DATA* IHP_ptc_key_program_lut_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IHP_PTC_KEY_PROGRAM_LUT_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IHP_PTC_KEY_PROGRAM_LUT_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_PTC_KEY_PROGRAM_LUT_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IHP_ptc_key_program_lut_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IHP_PTC_KEY_PROGRAM_LUT_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->ihp.ptc_key_program_lut_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IHP_ID,
          SOC_PETRA_IHP_PTC_KEY_PROGRAM_LUT_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->ptc_key_program_ptr),
          &(IHP_ptc_key_program_lut_tbl_data->ptc_key_program_ptr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->ptc_key_program_var),
          &(IHP_ptc_key_program_lut_tbl_data->ptc_key_program_var)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->ptc_cos_profile),
          &(IHP_ptc_key_program_lut_tbl_data->ptc_cos_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 42, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ihp_ptc_key_program_lut_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table ptc_key_program_lut_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ihp_ptc_key_program_lut_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IHP_PTC_KEY_PROGRAM_LUT_TBL_DATA* IHP_ptc_key_program_lut_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IHP_PTC_KEY_PROGRAM_LUT_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IHP_PTC_KEY_PROGRAM_LUT_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_PTC_KEY_PROGRAM_LUT_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->ihp.ptc_key_program_lut_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IHP_ptc_key_program_lut_tbl_data->ptc_key_program_ptr),
          &(tbl->ptc_key_program_ptr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(IHP_ptc_key_program_lut_tbl_data->ptc_key_program_var),
          &(tbl->ptc_key_program_var),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_petra_field_in_place_set(
          &(IHP_ptc_key_program_lut_tbl_data->ptc_cos_profile),
          &(tbl->ptc_cos_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IHP_ID,
          SOC_PETRA_IHP_PTC_KEY_PROGRAM_LUT_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 33, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ihp_ptc_key_program_lut_tbl_set_unsafe()",0,0);
}

/*
 * This is a generic function for key program tables 0 - 4
 * Write indirect table key_program_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ihp_key_program_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   uint32             program_ndx,
    SOC_SAND_IN   uint32             instr_ndx,
    SOC_SAND_IN   SOC_PETRA_IHP_KEY_PROGRAM_TBL_DATA* IHP_key_program_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    tbl_offst = 0,
    offset  = 0,
    data[SOC_PETRA_IHP_KEY_PROGRAM_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IHP_KEY_PROGRAM_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_KEY_PROGRAM_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);


  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    program_ndx, SOC_PETRA_IHP_KEY_NOF_PROGS-1,
    SOC_PETRA_TBL_PROG_IDX_OUT_OF_RANGE_ERR, 25, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    instr_ndx, SOC_PETRA_IHP_KEY_PROG_NOF_INSTR-1,
    SOC_PETRA_TBL_INSTR_IDX_OUT_OF_RANGE_ERR, 27, exit
  );

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->ihp.key_program_tbl);

  tbl_offst = SOC_PA_TBL(tables->ihp.key_program1_tbl.addr.base) - SOC_PA_TBL(tables->ihp.key_program0_tbl.addr.base);

  offset =  tbl->addr.base + tbl_offst*program_ndx + entry_offset;

  SOC_PETRA_TBL_INDEXED_SIZE_CHECK(tbl_offst*program_ndx);

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IHP_ID,
          SOC_PETRA_IHP_KEY_PROGRAM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 29, exit);

  err = soc_petra_field_in_place_set(
          &(IHP_key_program_tbl_data->select),
          &(tbl->select[instr_ndx]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(IHP_key_program_tbl_data->length),
          &(tbl->length[instr_ndx]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_petra_field_in_place_set(
          &(IHP_key_program_tbl_data->nibble_shift),
          &(tbl->nibble_shift[instr_ndx]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

  err = soc_petra_field_in_place_set(
          &(IHP_key_program_tbl_data->byte_shift),
          &(tbl->byte_shift[instr_ndx]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 33, exit);

  err = soc_petra_field_in_place_set(
          &(IHP_key_program_tbl_data->offset_select),
          &(tbl->offset_select[instr_ndx]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 34, exit);


  err = soc_sand_tbl_write_unsafe(
            unit,
            data,
            offset,
            sizeof(data),
            SOC_PETRA_IHP_ID,
            SOC_PETRA_IHP_KEY_PROGRAM_TBL_ENTRY_SIZE * sizeof(uint32)
          );
    SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ihp_key_program_tbl_set_unsafe()",0,0);
}


/*
 * This is a generic function for key program tables 0 - 4
 * Read indirect table key_program_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ihp_key_program_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   uint32             program_ndx,
    SOC_SAND_IN   uint32             instr_ndx,
    SOC_SAND_OUT   SOC_PETRA_IHP_KEY_PROGRAM_TBL_DATA* IHP_key_program_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    tbl_offst = 0,
    offset  = 0,
    data[SOC_PETRA_IHP_KEY_PROGRAM_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IHP_KEY_PROGRAM_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_KEY_PROGRAM_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    program_ndx, SOC_PETRA_IHP_KEY_NOF_PROGS-1,
    SOC_PETRA_TBL_PROG_IDX_OUT_OF_RANGE_ERR, 25, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    instr_ndx, SOC_PETRA_IHP_KEY_PROG_NOF_INSTR-1,
    SOC_PETRA_TBL_INSTR_IDX_OUT_OF_RANGE_ERR, 27, exit
  );

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->ihp.key_program_tbl);

  tbl_offst = SOC_PA_TBL(tables->ihp.key_program1_tbl.addr.base) - SOC_PA_TBL(tables->ihp.key_program0_tbl.addr.base);

  offset =  tbl->addr.base + tbl_offst*program_ndx + entry_offset;

  SOC_PETRA_TBL_INDEXED_SIZE_CHECK(tbl_offst*program_ndx);

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IHP_ID,
          SOC_PETRA_IHP_KEY_PROGRAM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->select[instr_ndx]),
          &(IHP_key_program_tbl_data->select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->length[instr_ndx]),
          &(IHP_key_program_tbl_data->length)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->nibble_shift[instr_ndx]),
          &(IHP_key_program_tbl_data->nibble_shift)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 42, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->byte_shift[instr_ndx]),
          &(IHP_key_program_tbl_data->byte_shift)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 43, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->offset_select[instr_ndx]),
          &(IHP_key_program_tbl_data->offset_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 44, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ihp_key_program_tbl_get_unsafe()",0,0);
}

/*
 * Read indirect table key_program0_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ihp_key_program0_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IHP_KEY_PROGRAM0_TBL_DATA* IHP_key_program0_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IHP_KEY_PROGRAM0_TBL_ENTRY_SIZE];
  uint32
    fld_idx = 0;
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IHP_KEY_PROGRAM0_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_KEY_PROGRAM0_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IHP_key_program0_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IHP_KEY_PROGRAM0_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->ihp.key_program0_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IHP_ID,
          SOC_PETRA_IHP_KEY_PROGRAM0_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  for (fld_idx = 0; fld_idx < SOC_PETRA_IHP_KEY_PROG_NOF_INSTR; fld_idx++)
  {
    err = soc_petra_field_in_place_get(
            data,
            &(tbl->select[fld_idx]),
            &(IHP_key_program0_tbl_data->select[fld_idx])
          );
    SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

    err = soc_petra_field_in_place_get(
            data,
            &(tbl->length[fld_idx]),
            &(IHP_key_program0_tbl_data->length[fld_idx])
          );
    SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

    err = soc_petra_field_in_place_get(
            data,
            &(tbl->nibble_shift[fld_idx]),
            &(IHP_key_program0_tbl_data->nibble_shift[fld_idx])
          );
    SOC_SAND_CHECK_FUNC_RESULT(err, 42, exit);

    err = soc_petra_field_in_place_get(
            data,
            &(tbl->byte_shift[fld_idx]),
            &(IHP_key_program0_tbl_data->byte_shift[fld_idx])
          );
    SOC_SAND_CHECK_FUNC_RESULT(err, 43, exit);

    err = soc_petra_field_in_place_get(
            data,
            &(tbl->offset_select[fld_idx]),
            &(IHP_key_program0_tbl_data->offset_select[fld_idx])
          );
    SOC_SAND_CHECK_FUNC_RESULT(err, 44, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ihp_key_program0_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table key_program0_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ihp_key_program0_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IHP_KEY_PROGRAM0_TBL_DATA* IHP_key_program0_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IHP_KEY_PROGRAM0_TBL_ENTRY_SIZE];
  uint32
    fld_idx = 0;
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IHP_KEY_PROGRAM0_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_KEY_PROGRAM0_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->ihp.key_program0_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  for (fld_idx = 0; fld_idx < SOC_PETRA_IHP_KEY_PROG_NOF_INSTR; fld_idx++)
  {
    err = soc_petra_field_in_place_set(
            &(IHP_key_program0_tbl_data->select[fld_idx]),
            &(tbl->select[fld_idx]),
            data
          );
    SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

    err = soc_petra_field_in_place_set(
            &(IHP_key_program0_tbl_data->length[fld_idx]),
            &(tbl->length[fld_idx]),
            data
          );
    SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

    err = soc_petra_field_in_place_set(
            &(IHP_key_program0_tbl_data->nibble_shift[fld_idx]),
            &(tbl->nibble_shift[fld_idx]),
            data
          );
    SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

    err = soc_petra_field_in_place_set(
            &(IHP_key_program0_tbl_data->byte_shift[fld_idx]),
            &(tbl->byte_shift[fld_idx]),
            data
          );
    SOC_SAND_CHECK_FUNC_RESULT(err, 33, exit);

    err = soc_petra_field_in_place_set(
            &(IHP_key_program0_tbl_data->offset_select[fld_idx]),
            &(tbl->offset_select[fld_idx]),
            data
          );
    SOC_SAND_CHECK_FUNC_RESULT(err, 34, exit);
  }

  err = soc_sand_tbl_write_unsafe(
            unit,
            data,
            offset,
            sizeof(data),
            SOC_PETRA_IHP_ID,
            SOC_PETRA_IHP_KEY_PROGRAM0_TBL_ENTRY_SIZE * sizeof(uint32)
          );
    SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ihp_key_program0_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table key_program1_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ihp_key_program1_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IHP_KEY_PROGRAM1_TBL_DATA* IHP_key_program1_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IHP_KEY_PROGRAM1_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IHP_KEY_PROGRAM1_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_KEY_PROGRAM1_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IHP_key_program1_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IHP_KEY_PROGRAM1_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->ihp.key_program1_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IHP_ID,
          SOC_PETRA_IHP_KEY_PROGRAM1_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->key_program1),
          &(IHP_key_program1_tbl_data->key_program1)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ihp_key_program1_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table key_program1_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ihp_key_program1_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IHP_KEY_PROGRAM1_TBL_DATA* IHP_key_program1_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IHP_KEY_PROGRAM1_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IHP_KEY_PROGRAM1_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_KEY_PROGRAM1_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->ihp.key_program1_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IHP_key_program1_tbl_data->key_program1),
          &(tbl->key_program1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IHP_ID,
          SOC_PETRA_IHP_KEY_PROGRAM1_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ihp_key_program1_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table key_program2_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ihp_key_program2_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IHP_KEY_PROGRAM2_TBL_DATA* IHP_key_program2_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IHP_KEY_PROGRAM2_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IHP_KEY_PROGRAM2_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_KEY_PROGRAM2_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IHP_key_program2_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IHP_KEY_PROGRAM2_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->ihp.key_program2_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IHP_ID,
          SOC_PETRA_IHP_KEY_PROGRAM2_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->instruction4),
          &(IHP_key_program2_tbl_data->instruction4)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->instruction3),
          &(IHP_key_program2_tbl_data->instruction3)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->exact_match_enable),
          &(IHP_key_program2_tbl_data->exact_match_enable)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 42, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->exact_match_mask_index),
          &(IHP_key_program2_tbl_data->exact_match_mask_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 43, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ihp_key_program2_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table key_program2_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ihp_key_program2_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IHP_KEY_PROGRAM2_TBL_DATA* IHP_key_program2_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IHP_KEY_PROGRAM2_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IHP_KEY_PROGRAM2_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_KEY_PROGRAM2_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->ihp.key_program2_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IHP_key_program2_tbl_data->instruction4),
          &(tbl->instruction4),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(IHP_key_program2_tbl_data->instruction3),
          &(tbl->instruction3),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_petra_field_in_place_set(
          &(IHP_key_program2_tbl_data->exact_match_enable),
          &(tbl->exact_match_enable),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

  err = soc_petra_field_in_place_set(
          &(IHP_key_program2_tbl_data->exact_match_mask_index),
          &(tbl->exact_match_mask_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 33, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IHP_ID,
          SOC_PETRA_IHP_KEY_PROGRAM2_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 34, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ihp_key_program2_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table key_program3_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ihp_key_program3_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IHP_KEY_PROGRAM3_TBL_DATA* IHP_key_program3_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IHP_KEY_PROGRAM3_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IHP_KEY_PROGRAM3_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_KEY_PROGRAM3_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IHP_key_program3_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IHP_KEY_PROGRAM3_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->ihp.key_program3_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IHP_ID,
          SOC_PETRA_IHP_KEY_PROGRAM3_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->key_program3),
          &(IHP_key_program3_tbl_data->key_program3)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ihp_key_program3_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table key_program3_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ihp_key_program3_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IHP_KEY_PROGRAM3_TBL_DATA* IHP_key_program3_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IHP_KEY_PROGRAM3_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IHP_KEY_PROGRAM3_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_KEY_PROGRAM3_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->ihp.key_program3_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IHP_key_program3_tbl_data->key_program3),
          &(tbl->key_program3),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IHP_ID,
          SOC_PETRA_IHP_KEY_PROGRAM3_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ihp_key_program3_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table key_program4_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ihp_key_program4_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IHP_KEY_PROGRAM4_TBL_DATA* IHP_key_program4_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IHP_KEY_PROGRAM4_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IHP_KEY_PROGRAM4_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_KEY_PROGRAM4_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IHP_key_program4_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IHP_KEY_PROGRAM4_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->ihp.key_program4_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IHP_ID,
          SOC_PETRA_IHP_KEY_PROGRAM4_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->instruction9),
          &(IHP_key_program4_tbl_data->instruction9)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->instruction10),
          &(IHP_key_program4_tbl_data->instruction10)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->tcam_match_enable),
          &(IHP_key_program4_tbl_data->tcam_match_enable)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 42, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->tcam_match_select),
          &(IHP_key_program4_tbl_data->tcam_match_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 43, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->tcam_key_and_value),
          &(IHP_key_program4_tbl_data->tcam_key_and_value)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 44, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->tcam_key_or_value),
          &(IHP_key_program4_tbl_data->tcam_key_or_value)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 45, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->bytes_to_remove_hdr_sel),
          &(IHP_key_program4_tbl_data->bytes_to_remove_hdr_sel)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 46, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->bytes_to_remove_hdr_size),
          &(IHP_key_program4_tbl_data->bytes_to_remove_hdr_size)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 47, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->cos_hdr_var_mask_select),
          &(IHP_key_program4_tbl_data->cos_hdr_var_mask_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 48, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ihp_key_program4_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table key_program4_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ihp_key_program4_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IHP_KEY_PROGRAM4_TBL_DATA* IHP_key_program4_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IHP_KEY_PROGRAM4_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IHP_KEY_PROGRAM4_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_KEY_PROGRAM4_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->ihp.key_program4_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IHP_key_program4_tbl_data->instruction9),
          &(tbl->instruction9),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(IHP_key_program4_tbl_data->instruction10),
          &(tbl->instruction10),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_petra_field_in_place_set(
          &(IHP_key_program4_tbl_data->tcam_match_enable),
          &(tbl->tcam_match_enable),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

  err = soc_petra_field_in_place_set(
          &(IHP_key_program4_tbl_data->tcam_match_select),
          &(tbl->tcam_match_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 33, exit);

  err = soc_petra_field_in_place_set(
          &(IHP_key_program4_tbl_data->tcam_key_and_value),
          &(tbl->tcam_key_and_value),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 34, exit);

  err = soc_petra_field_in_place_set(
          &(IHP_key_program4_tbl_data->tcam_key_or_value),
          &(tbl->tcam_key_or_value),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 35, exit);

  err = soc_petra_field_in_place_set(
          &(IHP_key_program4_tbl_data->bytes_to_remove_hdr_sel),
          &(tbl->bytes_to_remove_hdr_sel),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 36, exit);

  err = soc_petra_field_in_place_set(
          &(IHP_key_program4_tbl_data->bytes_to_remove_hdr_size),
          &(tbl->bytes_to_remove_hdr_size),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 37, exit);

  err = soc_petra_field_in_place_set(
          &(IHP_key_program4_tbl_data->cos_hdr_var_mask_select),
          &(tbl->cos_hdr_var_mask_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 38, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IHP_ID,
          SOC_PETRA_IHP_KEY_PROGRAM4_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 39, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ihp_key_program4_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table programmable_cos_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 * Each entry contains 16 set. Use set_ndx for the requested set
 */

uint32
  soc_petra_ihp_programmable_cos_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   uint32             set_ndx,
    SOC_SAND_OUT  SOC_PETRA_IHP_PROGRAMMABLE_COS_TBL_DATA* IHP_programmable_cos_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IHP_PROGRAMMABLE_COS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IHP_PROGRAMMABLE_COS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_PROGRAMMABLE_COS_TBL_GET_UNSAFE);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    set_ndx, SOC_PETRA_IHP_PRGR_COS_TBL_FLDS,
    SOC_PETRA_TBL_PRGR_COS_SET_IDX_OUT_OF_RANGE_ERR, 5, exit
  );

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IHP_programmable_cos_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IHP_PROGRAMMABLE_COS_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->ihp.programmable_cos_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IHP_ID,
          SOC_PETRA_IHP_PROGRAMMABLE_COS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->drop_precedence[set_ndx]),
          &(IHP_programmable_cos_tbl_data->drop_precedence)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->traffic_class[set_ndx]),
          &(IHP_programmable_cos_tbl_data->traffic_class)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ihp_programmable_cos_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table programmable_cos_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 * Each entry contains 16 set. Use set_ndx for the requested set
 */

uint32
  soc_petra_ihp_programmable_cos_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   uint32             set_ndx,
    SOC_SAND_IN   SOC_PETRA_IHP_PROGRAMMABLE_COS_TBL_DATA* IHP_programmable_cos_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IHP_PROGRAMMABLE_COS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IHP_PROGRAMMABLE_COS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_PROGRAMMABLE_COS_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->ihp.programmable_cos_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IHP_ID,
          SOC_PETRA_IHP_PROGRAMMABLE_COS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 25, exit);

  err = soc_petra_field_in_place_set(
          &(IHP_programmable_cos_tbl_data->drop_precedence),
          &(tbl->drop_precedence[set_ndx]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(IHP_programmable_cos_tbl_data->traffic_class),
          &(tbl->traffic_class[set_ndx]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IHP_ID,
          SOC_PETRA_IHP_PROGRAMMABLE_COS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ihp_programmable_cos_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table programmable_cos1_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ihp_programmable_cos1_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IHP_PROGRAMMABLE_COS1_TBL_DATA* IHP_programmable_cos1_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IHP_PROGRAMMABLE_COS1_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IHP_PROGRAMMABLE_COS1_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_PROGRAMMABLE_COS1_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IHP_programmable_cos1_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IHP_PROGRAMMABLE_COS1_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->ihp.programmable_cos1_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IHP_ID,
          SOC_PETRA_IHP_PROGRAMMABLE_COS1_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->programmable_cos1),
          (IHP_programmable_cos1_tbl_data->programmable_cos1)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ihp_programmable_cos1_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table programmable_cos1_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ihp_programmable_cos1_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IHP_PROGRAMMABLE_COS1_TBL_DATA* IHP_programmable_cos1_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IHP_PROGRAMMABLE_COS1_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IHP_PROGRAMMABLE_COS1_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_PROGRAMMABLE_COS1_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->ihp.programmable_cos1_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          (IHP_programmable_cos1_tbl_data->programmable_cos1),
          &(tbl->programmable_cos1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IHP_ID,
          SOC_PETRA_IHP_PROGRAMMABLE_COS1_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ihp_programmable_cos1_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table bdb_link_list_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_bdb_link_list_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_BDB_LINK_LIST_TBL_DATA* IQM_bdb_link_list_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_BDB_LINK_LIST_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_BDB_LINK_LIST_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_BDB_LINK_LIST_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IQM_bdb_link_list_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IQM_BDB_LINK_LIST_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.bdb_link_list_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_BDB_LINK_LIST_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->bdb_link_list),
          &(IQM_bdb_link_list_tbl_data->bdb_link_list)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_bdb_link_list_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table bdb_link_list_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_bdb_link_list_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_BDB_LINK_LIST_TBL_DATA* IQM_bdb_link_list_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_BDB_LINK_LIST_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_BDB_LINK_LIST_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_BDB_LINK_LIST_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.bdb_link_list_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IQM_bdb_link_list_tbl_data->bdb_link_list),
          &(tbl->bdb_link_list),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_BDB_LINK_LIST_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_bdb_link_list_tbl_set_unsafe()",unit,entry_offset);
}

/*
 * Read indirect table dynamic_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_dynamic_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_DYNAMIC_TBL_DATA* IQM_dynamic_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_DYNAMIC_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_DYNAMIC_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_DYNAMIC_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IQM_dynamic_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IQM_DYNAMIC_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.dynamic_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_DYNAMIC_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->pq_head_ptr),
          &(IQM_dynamic_tbl_data->pq_head_ptr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->que_not_empty),
          &(IQM_dynamic_tbl_data->que_not_empty)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->pq_inst_que_size),
          &(IQM_dynamic_tbl_data->pq_inst_que_size)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 42, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->pq_avrg_szie),
          &(IQM_dynamic_tbl_data->pq_avrg_szie)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 43, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_dynamic_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table dynamic_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_dynamic_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_DYNAMIC_TBL_DATA* IQM_dynamic_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_DYNAMIC_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_DYNAMIC_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_DYNAMIC_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.dynamic_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IQM_dynamic_tbl_data->pq_head_ptr),
          &(tbl->pq_head_ptr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(IQM_dynamic_tbl_data->que_not_empty),
          &(tbl->que_not_empty),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_petra_field_in_place_set(
          &(IQM_dynamic_tbl_data->pq_inst_que_size),
          &(tbl->pq_inst_que_size),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

  err = soc_petra_field_in_place_set(
          &(IQM_dynamic_tbl_data->pq_avrg_szie),
          &(tbl->pq_avrg_szie),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 33, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_DYNAMIC_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 34, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_dynamic_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table static_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_static_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_STATIC_TBL_DATA* IQM_static_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_STATIC_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_STATIC_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_STATIC_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IQM_static_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IQM_STATIC_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.static_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_STATIC_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->credit_class),
          &(IQM_static_tbl_data->credit_class)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->rate_class),
          &(IQM_static_tbl_data->rate_class)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->connection_class),
          &(IQM_static_tbl_data->connection_class)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 42, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->traffic_class),
          &(IQM_static_tbl_data->traffic_class)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 43, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->que_signature),
          &(IQM_static_tbl_data->que_signature)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 44, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_static_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table static_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_static_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_STATIC_TBL_DATA* IQM_static_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_STATIC_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_STATIC_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_STATIC_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.static_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IQM_static_tbl_data->credit_class),
          &(tbl->credit_class),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(IQM_static_tbl_data->rate_class),
          &(tbl->rate_class),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_petra_field_in_place_set(
          &(IQM_static_tbl_data->connection_class),
          &(tbl->connection_class),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

  err = soc_petra_field_in_place_set(
          &(IQM_static_tbl_data->traffic_class),
          &(tbl->traffic_class),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 33, exit);

  err = soc_petra_field_in_place_set(
          &(IQM_static_tbl_data->que_signature),
          &(tbl->que_signature),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 34, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_STATIC_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 35, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_static_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table packet_queue_tail_pointer_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_packet_queue_tail_pointer_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_PACKET_QUEUE_TAIL_POINTER_TBL_DATA* IQM_packet_queue_tail_pointer_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_PACKET_QUEUE_TAIL_POINTER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_PACKET_QUEUE_TAIL_POINTER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_PACKET_QUEUE_TAIL_POINTER_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IQM_packet_queue_tail_pointer_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IQM_PACKET_QUEUE_TAIL_POINTER_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.packet_queue_tail_pointer_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_PACKET_QUEUE_TAIL_POINTER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->tail_ptr),
          &(IQM_packet_queue_tail_pointer_tbl_data->tail_ptr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_packet_queue_tail_pointer_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table packet_queue_tail_pointer_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_packet_queue_tail_pointer_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_PACKET_QUEUE_TAIL_POINTER_TBL_DATA* IQM_packet_queue_tail_pointer_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_PACKET_QUEUE_TAIL_POINTER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_PACKET_QUEUE_TAIL_POINTER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_PACKET_QUEUE_TAIL_POINTER_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.packet_queue_tail_pointer_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IQM_packet_queue_tail_pointer_tbl_data->tail_ptr),
          &(tbl->tail_ptr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_PACKET_QUEUE_TAIL_POINTER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_packet_queue_tail_pointer_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table packet_queue_red_weight_table_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_packet_queue_red_weight_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_DATA* IQM_packet_queue_red_weight_table_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IQM_packet_queue_red_weight_table_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.packet_queue_red_weight_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->pq_weight),
          &(IQM_packet_queue_red_weight_table_tbl_data->pq_weight)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->avrg_en),
          &(IQM_packet_queue_red_weight_table_tbl_data->avrg_en)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_packet_queue_red_weight_table_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table packet_queue_red_weight_table_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_packet_queue_red_weight_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_DATA* IQM_packet_queue_red_weight_table_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.packet_queue_red_weight_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IQM_packet_queue_red_weight_table_tbl_data->pq_weight),
          &(tbl->pq_weight),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(IQM_packet_queue_red_weight_table_tbl_data->avrg_en),
          &(tbl->avrg_en),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_packet_queue_red_weight_table_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table credit_discount_table_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_credit_discount_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_CREDIT_DISCOUNT_TABLE_TBL_DATA* IQM_credit_discount_table_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_CREDIT_DISCOUNT_TABLE_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_CREDIT_DISCOUNT_TABLE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_CREDIT_DISCOUNT_TABLE_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IQM_credit_discount_table_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IQM_CREDIT_DISCOUNT_TABLE_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.credit_discount_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_CREDIT_DISCOUNT_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->crdt_disc_val),
          &(IQM_credit_discount_table_tbl_data->crdt_disc_val)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->discnt_sign),
          &(IQM_credit_discount_table_tbl_data->discnt_sign)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_credit_discount_table_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table credit_discount_table_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_credit_discount_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_CREDIT_DISCOUNT_TABLE_TBL_DATA* IQM_credit_discount_table_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_CREDIT_DISCOUNT_TABLE_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_CREDIT_DISCOUNT_TABLE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_CREDIT_DISCOUNT_TABLE_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.credit_discount_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IQM_credit_discount_table_tbl_data->crdt_disc_val),
          &(tbl->crdt_disc_val),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(IQM_credit_discount_table_tbl_data->discnt_sign),
          &(tbl->discnt_sign),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_CREDIT_DISCOUNT_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_credit_discount_table_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table full_user_count_memory_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_full_user_count_memory_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_FULL_USER_COUNT_MEMORY_TBL_DATA* IQM_full_user_count_memory_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_FULL_USER_COUNT_MEMORY_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_FULL_USER_COUNT_MEMORY_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_FULL_USER_COUNT_MEMORY_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IQM_full_user_count_memory_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IQM_FULL_USER_COUNT_MEMORY_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.full_user_count_memory_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_FULL_USER_COUNT_MEMORY_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->flus_cnt),
          &(IQM_full_user_count_memory_tbl_data->flus_cnt)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_full_user_count_memory_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table full_user_count_memory_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_full_user_count_memory_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_FULL_USER_COUNT_MEMORY_TBL_DATA* IQM_full_user_count_memory_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_FULL_USER_COUNT_MEMORY_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_FULL_USER_COUNT_MEMORY_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_FULL_USER_COUNT_MEMORY_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.full_user_count_memory_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IQM_full_user_count_memory_tbl_data->flus_cnt),
          &(tbl->flus_cnt),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_FULL_USER_COUNT_MEMORY_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_full_user_count_memory_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table mini_multicast_user_count_memory_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_mini_multicast_user_count_memory_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_MINI_MULTICAST_USER_COUNT_MEMORY_TBL_DATA* IQM_mini_multicast_user_count_memory_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_MINI_MULTICAST_USER_COUNT_MEMORY_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_MINI_MULTICAST_USER_COUNT_MEMORY_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_MINI_MULTICAST_USER_COUNT_MEMORY_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IQM_mini_multicast_user_count_memory_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IQM_MINI_MULTICAST_USER_COUNT_MEMORY_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.mini_multicast_user_count_memory_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_MINI_MULTICAST_USER_COUNT_MEMORY_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->mn_us_cnt),
          IQM_mini_multicast_user_count_memory_tbl_data->mn_us_cnt
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_mini_multicast_user_count_memory_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table mini_multicast_user_count_memory_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_mini_multicast_user_count_memory_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_MINI_MULTICAST_USER_COUNT_MEMORY_TBL_DATA* IQM_mini_multicast_user_count_memory_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_MINI_MULTICAST_USER_COUNT_MEMORY_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_MINI_MULTICAST_USER_COUNT_MEMORY_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_MINI_MULTICAST_USER_COUNT_MEMORY_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.mini_multicast_user_count_memory_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          IQM_mini_multicast_user_count_memory_tbl_data->mn_us_cnt,
          &(tbl->mn_us_cnt),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_MINI_MULTICAST_USER_COUNT_MEMORY_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_mini_multicast_user_count_memory_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table packet_queue_red_parameters_table_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_packet_queue_red_parameters_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32                                              rt_cls_ndx,
    SOC_SAND_IN   uint32                                             drop_precedence_ndx,
    SOC_SAND_OUT  SOC_PETRA_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_DATA* IQM_packet_queue_red_parameters_table_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    entry_offset,
    data[SOC_PETRA_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IQM_packet_queue_red_parameters_table_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.packet_queue_red_parameters_table_tbl);

  entry_offset =
    (rt_cls_ndx * SOC_PETRA_WRED_NOF_DROP_PRECEDENCE) + drop_precedence_ndx;

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->pq_max_que_size_exp),
          &(IQM_packet_queue_red_parameters_table_tbl_data->pq_max_que_size_exp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->pq_max_que_size_mnt),
          &(IQM_packet_queue_red_parameters_table_tbl_data->pq_max_que_size_mnt)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->pq_wred_en),
          &(IQM_packet_queue_red_parameters_table_tbl_data->pq_wred_en)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 42, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->pq_c2),
          &(IQM_packet_queue_red_parameters_table_tbl_data->pq_c2)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 43, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->pq_c3),
          &(IQM_packet_queue_red_parameters_table_tbl_data->pq_c3)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 44, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->pq_c1),
          &(IQM_packet_queue_red_parameters_table_tbl_data->pq_c1)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 45, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->pq_avrg_max_th),
          &(IQM_packet_queue_red_parameters_table_tbl_data->pq_avrg_max_th)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 46, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->pq_avrg_min_th),
          &(IQM_packet_queue_red_parameters_table_tbl_data->pq_avrg_min_th)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 47, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->pq_wred_pckt_sz_ignr),
          &(IQM_packet_queue_red_parameters_table_tbl_data->pq_wred_pckt_sz_ignr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 48, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->addmit_logic),
          &(IQM_packet_queue_red_parameters_table_tbl_data->addmit_logic)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 49, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_packet_queue_red_parameters_table_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table packet_queue_red_parameters_table_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_packet_queue_red_parameters_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32                                              rt_cls_ndx,
    SOC_SAND_IN   uint32                                             drop_precedence_ndx,
    SOC_SAND_IN   SOC_PETRA_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_DATA* IQM_packet_queue_red_parameters_table_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    entry_offset,
    data[SOC_PETRA_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.packet_queue_red_parameters_table_tbl);

  entry_offset =
    (rt_cls_ndx * SOC_PETRA_WRED_NOF_DROP_PRECEDENCE) + drop_precedence_ndx;

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IQM_packet_queue_red_parameters_table_tbl_data->pq_max_que_size_mnt),
          &(tbl->pq_max_que_size_mnt),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(IQM_packet_queue_red_parameters_table_tbl_data->pq_max_que_size_exp),
          &(tbl->pq_max_que_size_exp),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_petra_field_in_place_set(
          &(IQM_packet_queue_red_parameters_table_tbl_data->pq_wred_en),
          &(tbl->pq_wred_en),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

  err = soc_petra_field_in_place_set(
          &(IQM_packet_queue_red_parameters_table_tbl_data->pq_c2),
          &(tbl->pq_c2),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 33, exit);

  err = soc_petra_field_in_place_set(
          &(IQM_packet_queue_red_parameters_table_tbl_data->pq_c3),
          &(tbl->pq_c3),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 34, exit);

  err = soc_petra_field_in_place_set(
          &(IQM_packet_queue_red_parameters_table_tbl_data->pq_c1),
          &(tbl->pq_c1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 35, exit);

  err = soc_petra_field_in_place_set(
          &(IQM_packet_queue_red_parameters_table_tbl_data->pq_avrg_max_th),
          &(tbl->pq_avrg_max_th),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 36, exit);

  err = soc_petra_field_in_place_set(
          &(IQM_packet_queue_red_parameters_table_tbl_data->pq_avrg_min_th),
          &(tbl->pq_avrg_min_th),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 37, exit);

  err = soc_petra_field_in_place_set(
          &(IQM_packet_queue_red_parameters_table_tbl_data->pq_wred_pckt_sz_ignr),
          &(tbl->pq_wred_pckt_sz_ignr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 38, exit);

  err = soc_petra_field_in_place_set(
          &(IQM_packet_queue_red_parameters_table_tbl_data->addmit_logic),
          &(tbl->addmit_logic),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 39, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_packet_queue_red_parameters_table_tbl_set_unsafe()",0,0);
}

uint32
  soc_petra_iqm_packet_descriptor_fifos_memory_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PETRA_IQM_PACKET_DESCRIPTOR_FIFOS_MEMORY_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_PACKET_DESCRIPTOR_FIFOS_MEMORY_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_PACKET_DESCRIPTOR_FIFOS_MEMORY_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_PACKET_DESCRIPTOR_FIFOS_MEMORY_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PETRA_IQM_PACKET_DESCRIPTOR_FIFOS_MEMORY_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PETRA_IQM_PACKET_DESCRIPTOR_FIFOS_MEMORY_TBL_DATA, 1);

  res = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.packet_descriptor_fifos_memory_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_PACKET_DESCRIPTOR_FIFOS_MEMORY_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tx_pd),
          &(tbl_data->tx_pd)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_packet_descriptor_fifos_memory_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_petra_iqm_packet_descriptor_fifos_memory_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PETRA_IQM_PACKET_DESCRIPTOR_FIFOS_MEMORY_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_PACKET_DESCRIPTOR_FIFOS_MEMORY_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_PACKET_DESCRIPTOR_FIFOS_MEMORY_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_PACKET_DESCRIPTOR_FIFOS_MEMORY_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PETRA_IQM_PACKET_DESCRIPTOR_FIFOS_MEMORY_TBL_ENTRY_SIZE);

  res = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.packet_descriptor_fifos_memory_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->tx_pd),
          &(tbl->tx_pd),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_PACKET_DESCRIPTOR_FIFOS_MEMORY_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_packet_descriptor_fifos_memory_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_petra_iqm_tx_descriptor_fifos_memory_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PETRA_IQM_TX_DESCRIPTOR_FIFOS_MEMORY_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_TX_DESCRIPTOR_FIFOS_MEMORY_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_TX_DESCRIPTOR_FIFOS_MEMORY_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_TX_DESCRIPTOR_FIFOS_MEMORY_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PETRA_IQM_TX_DESCRIPTOR_FIFOS_MEMORY_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PETRA_IQM_TX_DESCRIPTOR_FIFOS_MEMORY_TBL_DATA, 1);

  res = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.tx_descriptor_fifos_memory_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_TX_DESCRIPTOR_FIFOS_MEMORY_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tx_dscr),
          (tbl_data->tx_dscr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_tx_descriptor_fifos_memory_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_petra_iqm_tx_descriptor_fifos_memory_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PETRA_IQM_TX_DESCRIPTOR_FIFOS_MEMORY_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_TX_DESCRIPTOR_FIFOS_MEMORY_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_TX_DESCRIPTOR_FIFOS_MEMORY_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_TX_DESCRIPTOR_FIFOS_MEMORY_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PETRA_IQM_TX_DESCRIPTOR_FIFOS_MEMORY_TBL_ENTRY_SIZE);

  res = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.tx_descriptor_fifos_memory_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          (tbl_data->tx_dscr),
          &(tbl->tx_dscr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_TX_DESCRIPTOR_FIFOS_MEMORY_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_tx_descriptor_fifos_memory_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table vsq_descriptor_rate_class_group_a_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_vsq_descriptor_rate_class_group_a_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_DATA* IQM_vsq_descriptor_rate_class_group_a_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IQM_vsq_descriptor_rate_class_group_a_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.vsq_descriptor_rate_class_group_a_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->vsq_rc_a),
          &(IQM_vsq_descriptor_rate_class_group_a_tbl_data->vsq_rc_a)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_vsq_descriptor_rate_class_group_a_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table vsq_descriptor_rate_class_group_a_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_vsq_descriptor_rate_class_group_a_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_DATA* IQM_vsq_descriptor_rate_class_group_a_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.vsq_descriptor_rate_class_group_a_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IQM_vsq_descriptor_rate_class_group_a_tbl_data->vsq_rc_a),
          &(tbl->vsq_rc_a),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_vsq_descriptor_rate_class_group_a_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table vsq_descriptor_rate_class_group_b_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_vsq_descriptor_rate_class_group_b_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_DATA* IQM_vsq_descriptor_rate_class_group_b_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IQM_vsq_descriptor_rate_class_group_b_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.vsq_descriptor_rate_class_group_b_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->vsq_rc_b),
          &(IQM_vsq_descriptor_rate_class_group_b_tbl_data->vsq_rc_b)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_vsq_descriptor_rate_class_group_b_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table vsq_descriptor_rate_class_group_b_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_vsq_descriptor_rate_class_group_b_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_DATA* IQM_vsq_descriptor_rate_class_group_b_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.vsq_descriptor_rate_class_group_b_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IQM_vsq_descriptor_rate_class_group_b_tbl_data->vsq_rc_b),
          &(tbl->vsq_rc_b),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_vsq_descriptor_rate_class_group_b_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table vsq_descriptor_rate_class_group_c_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_vsq_descriptor_rate_class_group_c_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_DATA* IQM_vsq_descriptor_rate_class_group_c_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IQM_vsq_descriptor_rate_class_group_c_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.vsq_descriptor_rate_class_group_c_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->vsq_rc_c),
          &(IQM_vsq_descriptor_rate_class_group_c_tbl_data->vsq_rc_c)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_vsq_descriptor_rate_class_group_c_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table vsq_descriptor_rate_class_group_c_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_vsq_descriptor_rate_class_group_c_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_DATA* IQM_vsq_descriptor_rate_class_group_c_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.vsq_descriptor_rate_class_group_c_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IQM_vsq_descriptor_rate_class_group_c_tbl_data->vsq_rc_c),
          &(tbl->vsq_rc_c),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_vsq_descriptor_rate_class_group_c_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table vsq_descriptor_rate_class_group_d_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_vsq_descriptor_rate_class_group_d_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_DATA* IQM_vsq_descriptor_rate_class_group_d_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IQM_vsq_descriptor_rate_class_group_d_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.vsq_descriptor_rate_class_group_d_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->vsq_rc_d),
          &(IQM_vsq_descriptor_rate_class_group_d_tbl_data->vsq_rc_d)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_vsq_descriptor_rate_class_group_d_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table vsq_descriptor_rate_class_group_d_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_vsq_descriptor_rate_class_group_d_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_DATA* IQM_vsq_descriptor_rate_class_group_d_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.vsq_descriptor_rate_class_group_d_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IQM_vsq_descriptor_rate_class_group_d_tbl_data->vsq_rc_d),
          &(tbl->vsq_rc_d),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_vsq_descriptor_rate_class_group_d_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table vsq_qsize_memory_group_a_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_vsq_qsize_memory_group_a_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_A_TBL_DATA* IQM_vsq_qsize_memory_group_a_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_A_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_A_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_A_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IQM_vsq_qsize_memory_group_a_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_A_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.vsq_qsize_memory_group_a_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_A_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->vsq_size_wrds),
          &(IQM_vsq_qsize_memory_group_a_tbl_data->vsq_size_wrds)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->vsq_size_bds),
          &(IQM_vsq_qsize_memory_group_a_tbl_data->vsq_size_bds)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_vsq_qsize_memory_group_a_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table vsq_qsize_memory_group_a_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_vsq_qsize_memory_group_a_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_A_TBL_DATA* IQM_vsq_qsize_memory_group_a_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_A_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_A_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_A_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.vsq_qsize_memory_group_a_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IQM_vsq_qsize_memory_group_a_tbl_data->vsq_size_wrds),
          &(tbl->vsq_size_wrds),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(IQM_vsq_qsize_memory_group_a_tbl_data->vsq_size_bds),
          &(tbl->vsq_size_bds),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_A_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_vsq_qsize_memory_group_a_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table vsq_qsize_memory_group_b_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_vsq_qsize_memory_group_b_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_B_TBL_DATA* IQM_vsq_qsize_memory_group_b_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_B_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_B_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_B_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IQM_vsq_qsize_memory_group_b_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_B_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.vsq_qsize_memory_group_b_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_B_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->vsq_size_wrds),
          &(IQM_vsq_qsize_memory_group_b_tbl_data->vsq_size_wrds)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->vsq_size_bds),
          &(IQM_vsq_qsize_memory_group_b_tbl_data->vsq_size_bds)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_vsq_qsize_memory_group_b_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table vsq_qsize_memory_group_b_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_vsq_qsize_memory_group_b_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_B_TBL_DATA* IQM_vsq_qsize_memory_group_b_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_B_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_B_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_B_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.vsq_qsize_memory_group_b_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IQM_vsq_qsize_memory_group_b_tbl_data->vsq_size_wrds),
          &(tbl->vsq_size_wrds),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(IQM_vsq_qsize_memory_group_b_tbl_data->vsq_size_bds),
          &(tbl->vsq_size_bds),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_B_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_vsq_qsize_memory_group_b_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table vsq_qsize_memory_group_c_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_vsq_qsize_memory_group_c_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_C_TBL_DATA* IQM_vsq_qsize_memory_group_c_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_C_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_C_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_C_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IQM_vsq_qsize_memory_group_c_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_C_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.vsq_qsize_memory_group_c_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_C_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->vsq_size_wrds),
          &(IQM_vsq_qsize_memory_group_c_tbl_data->vsq_size_wrds)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->vsq_size_bds),
          &(IQM_vsq_qsize_memory_group_c_tbl_data->vsq_size_bds)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_vsq_qsize_memory_group_c_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table vsq_qsize_memory_group_c_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_vsq_qsize_memory_group_c_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_C_TBL_DATA* IQM_vsq_qsize_memory_group_c_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_C_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_C_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_C_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.vsq_qsize_memory_group_c_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IQM_vsq_qsize_memory_group_c_tbl_data->vsq_size_wrds),
          &(tbl->vsq_size_wrds),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(IQM_vsq_qsize_memory_group_c_tbl_data->vsq_size_bds),
          &(tbl->vsq_size_bds),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_C_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_vsq_qsize_memory_group_c_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table vsq_qsize_memory_group_d_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_vsq_qsize_memory_group_d_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_D_TBL_DATA* IQM_vsq_qsize_memory_group_d_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_D_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_D_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_D_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IQM_vsq_qsize_memory_group_d_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_D_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.vsq_qsize_memory_group_d_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_D_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->vsq_size_wrds),
          &(IQM_vsq_qsize_memory_group_d_tbl_data->vsq_size_wrds)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->vsq_size_bds),
          &(IQM_vsq_qsize_memory_group_d_tbl_data->vsq_size_bds)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_vsq_qsize_memory_group_d_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table vsq_qsize_memory_group_d_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_vsq_qsize_memory_group_d_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_D_TBL_DATA* IQM_vsq_qsize_memory_group_d_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_D_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_D_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_D_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.vsq_qsize_memory_group_d_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IQM_vsq_qsize_memory_group_d_tbl_data->vsq_size_wrds),
          &(tbl->vsq_size_wrds),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(IQM_vsq_qsize_memory_group_d_tbl_data->vsq_size_bds),
          &(tbl->vsq_size_bds),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_D_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_vsq_qsize_memory_group_d_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table vsq_average_qsize_memory_group_a_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_vsq_average_qsize_memory_group_a_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_A_TBL_DATA* IQM_vsq_average_qsize_memory_group_a_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_A_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_A_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_A_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IQM_vsq_average_qsize_memory_group_a_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_A_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.vsq_average_qsize_memory_group_a_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_A_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->vsq_avrg_size),
          &(IQM_vsq_average_qsize_memory_group_a_tbl_data->vsq_avrg_size)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_vsq_average_qsize_memory_group_a_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table vsq_average_qsize_memory_group_a_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_vsq_average_qsize_memory_group_a_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_A_TBL_DATA* IQM_vsq_average_qsize_memory_group_a_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_A_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_A_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_A_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.vsq_average_qsize_memory_group_a_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IQM_vsq_average_qsize_memory_group_a_tbl_data->vsq_avrg_size),
          &(tbl->vsq_avrg_size),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_A_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_vsq_average_qsize_memory_group_a_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table vsq_average_qsize_memory_group_b_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_vsq_average_qsize_memory_group_b_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_B_TBL_DATA* IQM_vsq_average_qsize_memory_group_b_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_B_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_B_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_B_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IQM_vsq_average_qsize_memory_group_b_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_B_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.vsq_average_qsize_memory_group_b_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_B_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->vsq_avrg_size),
          &(IQM_vsq_average_qsize_memory_group_b_tbl_data->vsq_avrg_size)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_vsq_average_qsize_memory_group_b_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table vsq_average_qsize_memory_group_b_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_vsq_average_qsize_memory_group_b_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_B_TBL_DATA* IQM_vsq_average_qsize_memory_group_b_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_B_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_B_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_B_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.vsq_average_qsize_memory_group_b_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IQM_vsq_average_qsize_memory_group_b_tbl_data->vsq_avrg_size),
          &(tbl->vsq_avrg_size),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_B_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_vsq_average_qsize_memory_group_b_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table vsq_average_qsize_memory_group_c_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_vsq_average_qsize_memory_group_c_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_C_TBL_DATA* IQM_vsq_average_qsize_memory_group_c_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_C_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_C_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_C_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IQM_vsq_average_qsize_memory_group_c_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_C_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.vsq_average_qsize_memory_group_c_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_C_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->vsq_avrg_size),
          &(IQM_vsq_average_qsize_memory_group_c_tbl_data->vsq_avrg_size)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_vsq_average_qsize_memory_group_c_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table vsq_average_qsize_memory_group_c_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_vsq_average_qsize_memory_group_c_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_C_TBL_DATA* IQM_vsq_average_qsize_memory_group_c_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_C_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_C_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_C_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.vsq_average_qsize_memory_group_c_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IQM_vsq_average_qsize_memory_group_c_tbl_data->vsq_avrg_size),
          &(tbl->vsq_avrg_size),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_C_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_vsq_average_qsize_memory_group_c_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table vsq_average_qsize_memory_group_d_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_vsq_average_qsize_memory_group_d_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_D_TBL_DATA* IQM_vsq_average_qsize_memory_group_d_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_D_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_D_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_D_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IQM_vsq_average_qsize_memory_group_d_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_D_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.vsq_average_qsize_memory_group_d_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_D_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->vsq_avrg_size),
          &(IQM_vsq_average_qsize_memory_group_d_tbl_data->vsq_avrg_size)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_vsq_average_qsize_memory_group_d_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table vsq_average_qsize_memory_group_d_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_vsq_average_qsize_memory_group_d_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_D_TBL_DATA* IQM_vsq_average_qsize_memory_group_d_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_D_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_D_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_D_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.vsq_average_qsize_memory_group_d_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IQM_vsq_average_qsize_memory_group_d_tbl_data->vsq_avrg_size),
          &(tbl->vsq_avrg_size),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_D_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_vsq_average_qsize_memory_group_d_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table vsq_flow_control_parameters_table_group_a_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_vsq_flow_control_parameters_table_group_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              group_id,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_DATA* IQM_vsq_flow_control_parameters_table_group_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_GET_UNSAFE);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    group_id, SOC_PETRA_NOF_VSQ_GROUPS-1,
    SOC_PETRA_ITM_VSQ_GROUP_ID_OUT_OF_RANGE_ERR, 5, exit
  );

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IQM_vsq_flow_control_parameters_table_group_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.vsq_flow_control_parameters_table_group_tbl[group_id]);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->wred_en),
          &(IQM_vsq_flow_control_parameters_table_group_tbl_data->wred_en)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->avrg_size_en),
          &(IQM_vsq_flow_control_parameters_table_group_tbl_data->avrg_size_en)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->red_weight_q),
          &(IQM_vsq_flow_control_parameters_table_group_tbl_data->red_weight_q)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 42, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->set_threshold_words_mnt),
          &(IQM_vsq_flow_control_parameters_table_group_tbl_data->set_threshold_words_mnt)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 43, exit);
  err = soc_petra_field_in_place_get(
          data,
          &(tbl->set_threshold_words_exp),
          &(IQM_vsq_flow_control_parameters_table_group_tbl_data->set_threshold_words_exp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 44, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->clear_threshold_words_mnt),
          &(IQM_vsq_flow_control_parameters_table_group_tbl_data->clear_threshold_words_mnt)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 45, exit);
  err = soc_petra_field_in_place_get(
          data,
          &(tbl->clear_threshold_words_exp),
          &(IQM_vsq_flow_control_parameters_table_group_tbl_data->clear_threshold_words_exp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 46, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->set_threshold_bd_mnt),
          &(IQM_vsq_flow_control_parameters_table_group_tbl_data->set_threshold_bd_mnt)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 47, exit);
  err = soc_petra_field_in_place_get(
          data,
          &(tbl->set_threshold_bd_exp),
          &(IQM_vsq_flow_control_parameters_table_group_tbl_data->set_threshold_bd_exp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 48, exit);


  err = soc_petra_field_in_place_get(
          data,
          &(tbl->clear_threshold_bd_mnt),
          &(IQM_vsq_flow_control_parameters_table_group_tbl_data->clear_threshold_bd_mnt)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 49, exit);
  err = soc_petra_field_in_place_get(
          data,
          &(tbl->clear_threshold_bd_exp),
          &(IQM_vsq_flow_control_parameters_table_group_tbl_data->clear_threshold_bd_exp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_vsq_flow_control_parameters_table_group_a_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table vsq_flow_control_parameters_table_group_a_tbl from block IQM,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_vsq_flow_control_parameters_table_group_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              group_id,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_DATA* IQM_vsq_flow_control_parameters_table_group_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_SET_UNSAFE);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    group_id, SOC_PETRA_NOF_VSQ_GROUPS-1,
    SOC_PETRA_ITM_VSQ_GROUP_ID_OUT_OF_RANGE_ERR, 5, exit
  );

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.vsq_flow_control_parameters_table_group_tbl[group_id]);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IQM_vsq_flow_control_parameters_table_group_tbl_data->wred_en),
          &(tbl->wred_en),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(IQM_vsq_flow_control_parameters_table_group_tbl_data->avrg_size_en),
          &(tbl->avrg_size_en),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_petra_field_in_place_set(
          &(IQM_vsq_flow_control_parameters_table_group_tbl_data->red_weight_q),
          &(tbl->red_weight_q),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

  err = soc_petra_field_in_place_set(
          &(IQM_vsq_flow_control_parameters_table_group_tbl_data->set_threshold_words_mnt),
          &(tbl->set_threshold_words_mnt),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 33, exit);
  err = soc_petra_field_in_place_set(
          &(IQM_vsq_flow_control_parameters_table_group_tbl_data->set_threshold_words_exp),
          &(tbl->set_threshold_words_exp),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 34, exit);

  err = soc_petra_field_in_place_set(
          &(IQM_vsq_flow_control_parameters_table_group_tbl_data->clear_threshold_words_mnt),
          &(tbl->clear_threshold_words_mnt),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 35, exit);

  err = soc_petra_field_in_place_set(
          &(IQM_vsq_flow_control_parameters_table_group_tbl_data->clear_threshold_words_exp),
          &(tbl->clear_threshold_words_exp),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 36, exit);

  err = soc_petra_field_in_place_set(
          &(IQM_vsq_flow_control_parameters_table_group_tbl_data->set_threshold_bd_mnt),
          &(tbl->set_threshold_bd_mnt),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 37, exit);

  err = soc_petra_field_in_place_set(
          &(IQM_vsq_flow_control_parameters_table_group_tbl_data->set_threshold_bd_exp),
          &(tbl->set_threshold_bd_exp),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 38, exit);

  err = soc_petra_field_in_place_set(
          &(IQM_vsq_flow_control_parameters_table_group_tbl_data->clear_threshold_bd_mnt),
          &(tbl->clear_threshold_bd_mnt),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 39, exit);

  err = soc_petra_field_in_place_set(
          &(IQM_vsq_flow_control_parameters_table_group_tbl_data->clear_threshold_bd_exp),
          &(tbl->clear_threshold_bd_exp),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 37, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_vsq_flow_control_parameters_table_group_a_tbl_set_unsafe()",0,0);
}


/*
 * Read indirect table vsq_queue_parameters_table_group_a_tbl from block IQM,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_vsq_queue_parameters_table_group_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              group_id,
    SOC_SAND_IN   uint32              vsq_rt_cls_ndx,
    SOC_SAND_IN   uint32             drop_precedence_ndx,
    SOC_SAND_OUT  SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA* IQM_vsq_queue_parameters_table_group_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    entry_offset,
    data[SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_A_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_GET_UNSAFE);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    group_id, SOC_PETRA_NOF_VSQ_GROUPS-1,
    SOC_PETRA_ITM_VSQ_GROUP_ID_OUT_OF_RANGE_ERR, 5, exit
  );

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IQM_vsq_queue_parameters_table_group_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.vsq_queue_parameters_table_group_tbl[group_id]);

  entry_offset =
    (vsq_rt_cls_ndx * SOC_PETRA_WRED_NOF_DROP_PRECEDENCE) + drop_precedence_ndx;

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_A_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->c2),
          &(IQM_vsq_queue_parameters_table_group_tbl_data->c2)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->c3),
          &(IQM_vsq_queue_parameters_table_group_tbl_data->c3)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->c1),
          &(IQM_vsq_queue_parameters_table_group_tbl_data->c1)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 42, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->max_avrg_th),
          &(IQM_vsq_queue_parameters_table_group_tbl_data->max_avrg_th)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 43, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->min_avrg_th),
          &(IQM_vsq_queue_parameters_table_group_tbl_data->min_avrg_th)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 44, exit);
  err = soc_petra_field_in_place_get(
          data,
          &(tbl->vq_wred_pckt_sz_ignr),
          &(IQM_vsq_queue_parameters_table_group_tbl_data->vq_wred_pckt_sz_ignr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 45, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->vq_max_szie_bds_mnt),
          &(IQM_vsq_queue_parameters_table_group_tbl_data->vq_max_szie_bds_mnt)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 46, exit);

  err = soc_petra_field_in_place_get(
    data,
    &(tbl->vq_max_szie_bds_exp),
    &(IQM_vsq_queue_parameters_table_group_tbl_data->vq_max_szie_bds_exp)
    );
  SOC_SAND_CHECK_FUNC_RESULT(err, 47, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->vq_max_size_words_mnt),
          &(IQM_vsq_queue_parameters_table_group_tbl_data->vq_max_size_words_mnt)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 48, exit);

  err = soc_petra_field_in_place_get(
    data,
    &(tbl->vq_max_size_words_exp),
    &(IQM_vsq_queue_parameters_table_group_tbl_data->vq_max_size_words_exp)
    );
  SOC_SAND_CHECK_FUNC_RESULT(err, 49, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_vsq_queue_parameters_table_group_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table vsq_queue_parameters_table_group_a_tbl from block IQM,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_vsq_queue_parameters_table_group_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              group_id,
    SOC_SAND_IN   uint32              vsq_rt_cls_ndx,
    SOC_SAND_IN   uint32             drop_precedence_ndx,
    SOC_SAND_IN   SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA* IQM_vsq_queue_parameters_table_group_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    entry_offset,
    data[SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_A_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_SET_UNSAFE);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    group_id, SOC_PETRA_NOF_VSQ_GROUPS-1,
    SOC_PETRA_ITM_VSQ_GROUP_ID_OUT_OF_RANGE_ERR, 5, exit
  );

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.vsq_queue_parameters_table_group_tbl[group_id]);

  entry_offset =
    (vsq_rt_cls_ndx * SOC_PETRA_WRED_NOF_DROP_PRECEDENCE) + drop_precedence_ndx;

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IQM_vsq_queue_parameters_table_group_tbl_data->c2),
          &(tbl->c2),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(IQM_vsq_queue_parameters_table_group_tbl_data->c3),
          &(tbl->c3),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_petra_field_in_place_set(
          &(IQM_vsq_queue_parameters_table_group_tbl_data->c1),
          &(tbl->c1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

  err = soc_petra_field_in_place_set(
          &(IQM_vsq_queue_parameters_table_group_tbl_data->max_avrg_th),
          &(tbl->max_avrg_th),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 33, exit);

  err = soc_petra_field_in_place_set(
          &(IQM_vsq_queue_parameters_table_group_tbl_data->min_avrg_th),
          &(tbl->min_avrg_th),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 34, exit);
  err = soc_petra_field_in_place_set(
          &(IQM_vsq_queue_parameters_table_group_tbl_data->vq_wred_pckt_sz_ignr),
          &(tbl->vq_wred_pckt_sz_ignr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 35, exit);

  err = soc_petra_field_in_place_set(
          &(IQM_vsq_queue_parameters_table_group_tbl_data->vq_max_szie_bds_mnt),
          &(tbl->vq_max_szie_bds_mnt),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 36, exit);

  err = soc_petra_field_in_place_set(
    &(IQM_vsq_queue_parameters_table_group_tbl_data->vq_max_szie_bds_exp),
    &(tbl->vq_max_szie_bds_exp),
    data
    );
  SOC_SAND_CHECK_FUNC_RESULT(err, 37, exit);

  err = soc_petra_field_in_place_set(
          &(IQM_vsq_queue_parameters_table_group_tbl_data->vq_max_size_words_mnt),
          &(tbl->vq_max_size_words_mnt),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 38, exit);
  err = soc_petra_field_in_place_set(
    &(IQM_vsq_queue_parameters_table_group_tbl_data->vq_max_size_words_exp),
    &(tbl->vq_max_size_words_exp),
    data
    );
  SOC_SAND_CHECK_FUNC_RESULT(err, 39, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_A_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_vsq_queue_parameters_table_group_a_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table system_red_parameters_table_tbl from block IQM,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_system_red_parameters_table_tbl_get_unsafe(
    SOC_SAND_IN   int                                       unit,
    SOC_SAND_IN   uint32                                        rt_cls_ndx,
    SOC_SAND_IN   uint32                                       drop_precedence_ndx,
    SOC_SAND_OUT  SOC_PETRA_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_DATA* IQM_system_red_parameters_table_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    entry_offset,
    data[SOC_PETRA_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IQM_system_red_parameters_table_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.system_red_parameters_table_tbl);

  entry_offset =
    (rt_cls_ndx * SOC_PETRA_WRED_NOF_DROP_PRECEDENCE) + drop_precedence_ndx;

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->adm_th),
          &(IQM_system_red_parameters_table_tbl_data->adm_th)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

  err = soc_petra_field_in_place_get(
        data,
        &(tbl->sys_red_en),
        &(IQM_system_red_parameters_table_tbl_data->sys_red_en)
      );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->prob_th),
          &(IQM_system_red_parameters_table_tbl_data->prob_th)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 42, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->drp_th),
          &(IQM_system_red_parameters_table_tbl_data->drp_th)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 43, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->drp_prob_indx1),
          &(IQM_system_red_parameters_table_tbl_data->drp_prob_indx1)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 44, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->drp_prob_indx2),
          &(IQM_system_red_parameters_table_tbl_data->drp_prob_indx2)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 45, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_system_red_parameters_table_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table system_red_parameters_table_tbl from block IQM,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_system_red_parameters_table_tbl_set_unsafe(
    SOC_SAND_IN   int                                       unit,
    SOC_SAND_IN   uint32                                        rt_cls_ndx,
    SOC_SAND_IN   uint32                                       drop_precedence_ndx,
    SOC_SAND_IN   SOC_PETRA_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_DATA* IQM_system_red_parameters_table_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    entry_offset,
    data[SOC_PETRA_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.system_red_parameters_table_tbl);

  entry_offset =
    (rt_cls_ndx * SOC_PETRA_WRED_NOF_DROP_PRECEDENCE) + drop_precedence_ndx;

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
        &(IQM_system_red_parameters_table_tbl_data->sys_red_en),
        &(tbl->sys_red_en),
        data
      );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(IQM_system_red_parameters_table_tbl_data->adm_th),
          &(tbl->adm_th),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_petra_field_in_place_set(
          &(IQM_system_red_parameters_table_tbl_data->prob_th),
          &(tbl->prob_th),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

  err = soc_petra_field_in_place_set(
          &(IQM_system_red_parameters_table_tbl_data->drp_th),
          &(tbl->drp_th),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 33, exit);

  err = soc_petra_field_in_place_set(
          &(IQM_system_red_parameters_table_tbl_data->drp_prob_indx1),
          &(tbl->drp_prob_indx1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 34, exit);

  err = soc_petra_field_in_place_set(
          &(IQM_system_red_parameters_table_tbl_data->drp_prob_indx2),
          &(tbl->drp_prob_indx2),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 35, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 35, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_system_red_parameters_table_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table system_red_drop_probability_values_tbl from block IQM,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_system_red_drop_probability_values_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_DATA* IQM_system_red_drop_probability_values_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IQM_system_red_drop_probability_values_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.system_red_drop_probability_values_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->drp_prob),
          &(IQM_system_red_drop_probability_values_tbl_data->drp_prob)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_system_red_drop_probability_values_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table system_red_drop_probability_values_tbl from block IQM,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_system_red_drop_probability_values_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_DATA* IQM_system_red_drop_probability_values_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.system_red_drop_probability_values_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IQM_system_red_drop_probability_values_tbl_data->drp_prob),
          &(tbl->drp_prob),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_system_red_drop_probability_values_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table system_red_tbl from block IQM,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_system_red_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_SYSTEM_RED_TBL_DATA* IQM_system_red_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    indx,
    data[SOC_PETRA_IQM_SYSTEM_RED_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_SYSTEM_RED_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_SYSTEM_RED_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IQM_system_red_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IQM_SYSTEM_RED_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.system_red_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_SYSTEM_RED_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  for (indx = 0 ; indx < (SOC_PETRA_SYS_RED_NOF_Q_RNGS - 1) ; ++indx)
  {
    err = soc_petra_field_in_place_get(
            data,
            &(tbl->qsz_rng_th[indx]),
            &(IQM_system_red_tbl_data->qsz_rng_th[indx])
          );
    SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_system_red_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table system_red_tbl from block IQM,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_iqm_system_red_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_SYSTEM_RED_TBL_DATA* IQM_system_red_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    indx,
    data[SOC_PETRA_IQM_SYSTEM_RED_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_SYSTEM_RED_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IQM_SYSTEM_RED_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->iqm.system_red_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  for (indx = 0 ; indx < (SOC_PETRA_SYS_RED_NOF_Q_RNGS - 1) ; ++indx)
  {
    err = soc_petra_field_in_place_set(
            &(IQM_system_red_tbl_data->qsz_rng_th[indx]),
            &(tbl->qsz_rng_th[indx]),
            data
          );
    SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);
  }

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IQM_ID,
          SOC_PETRA_IQM_SYSTEM_RED_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 45, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_iqm_system_red_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table system_physical_port_lookup_table_tbl from block QDR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_qdr_memory_tbl_get_unsafe(
    SOC_SAND_IN   int                 unit,
    SOC_SAND_IN   uint32                  entry_offset,
    SOC_SAND_OUT  SOC_PETRA_QDR_MEM_TBL_DATA*   QDR_memory_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_QDR_MEM_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_QDR_MEM_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_QDR_MEMORY_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          QDR_memory_tbl_data,
          0x0,
          sizeof(SOC_PETRA_QDR_MEM_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->qdr.qdr_mem);

  offset = tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_QDR_ID,
          SOC_PETRA_QDR_MEM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->data),
          &(QDR_memory_tbl_data->data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_qdr_memory_tbl_get_unsafe()",0,0);
}


/*
 * Write indirect table system_physical_port_lookup_table_tbl from block QDR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_qdr_memory_tbl_set_unsafe(
    SOC_SAND_IN   int                 unit,
    SOC_SAND_IN   uint32                  entry_offset,
    SOC_SAND_IN   SOC_PETRA_QDR_MEM_TBL_DATA*   QDR_memory_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_QDR_MEM_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_QDR_MEM_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_QDR_MEMORY_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->qdr.qdr_mem);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(QDR_memory_tbl_data->data),
          &(tbl->data),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_QDR_ID,
          SOC_PETRA_QDR_MEM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_qdr_memory_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table qdr_dll_mem_tbl from block QDR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_qdr_qdr_dll_mem_tbl_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                       entry_offset,
    SOC_SAND_OUT SOC_PETRA_QDR_QDR_DLL_MEM_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_QDR_QDR_DLL_MEM_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_QDR_QDR_DLL_MEM_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PETRA_CLEAR(data, uint32, SOC_PETRA_QDR_QDR_DLL_MEM_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PETRA_QDR_QDR_DLL_MEM_TBL_DATA, 1);

  res = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->qdr.qdr_dll_mem);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_QDR_ID,
          SOC_PETRA_QDR_QDR_DLL_MEM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->qdr_dll_mem),
          &(tbl_data->qdr_dll_mem)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_qdr_qdr_dll_mem_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table qdr_dll_mem_tbl from block QDR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_qdr_qdr_dll_mem_tbl_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                       entry_offset,
    SOC_SAND_IN  SOC_PETRA_QDR_QDR_DLL_MEM_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_QDR_QDR_DLL_MEM_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_QDR_QDR_DLL_MEM_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PETRA_CLEAR(data, uint32, SOC_PETRA_QDR_QDR_DLL_MEM_TBL_ENTRY_SIZE);

  res = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->qdr.qdr_dll_mem);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->qdr_dll_mem),
          &(tbl->qdr_dll_mem),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_QDR_ID,
          SOC_PETRA_QDR_QDR_DLL_MEM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_qdr_qdr_dll_mem_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table system_physical_port_lookup_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ips_system_physical_port_lookup_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_DATA* IPS_system_physical_port_lookup_table_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IPS_system_physical_port_lookup_table_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->ips.system_physical_port_lookup_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IPS_ID,
          SOC_PETRA_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->sys_phy_port),
          &(IPS_system_physical_port_lookup_table_tbl_data->sys_phy_port)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ips_system_physical_port_lookup_table_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table system_physical_port_lookup_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ips_system_physical_port_lookup_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_DATA* IPS_system_physical_port_lookup_table_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->ips.system_physical_port_lookup_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IPS_system_physical_port_lookup_table_tbl_data->sys_phy_port),
          &(tbl->sys_phy_port),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IPS_ID,
          SOC_PETRA_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ips_system_physical_port_lookup_table_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table destination_device_and_port_lookup_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ips_destination_device_and_port_lookup_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_DATA* IPS_destination_device_and_port_lookup_table_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IPS_destination_device_and_port_lookup_table_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->ips.destination_device_and_port_lookup_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IPS_ID,
          SOC_PETRA_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->dest_port),
          &(IPS_destination_device_and_port_lookup_table_tbl_data->dest_port)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->dest_dev),
          &(IPS_destination_device_and_port_lookup_table_tbl_data->dest_dev)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ips_destination_device_and_port_lookup_table_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table destination_device_and_port_lookup_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ips_destination_device_and_port_lookup_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_DATA* IPS_destination_device_and_port_lookup_table_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->ips.destination_device_and_port_lookup_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IPS_destination_device_and_port_lookup_table_tbl_data->dest_port),
          &(tbl->dest_port),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(IPS_destination_device_and_port_lookup_table_tbl_data->dest_dev),
          &(tbl->dest_dev),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IPS_ID,
          SOC_PETRA_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ips_destination_device_and_port_lookup_table_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table flow_id_lookup_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ips_flow_id_lookup_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IPS_FLOW_ID_LOOKUP_TABLE_TBL_DATA* IPS_flow_id_lookup_table_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IPS_FLOW_ID_LOOKUP_TABLE_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IPS_FLOW_ID_LOOKUP_TABLE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPS_FLOW_ID_LOOKUP_TABLE_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IPS_flow_id_lookup_table_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IPS_FLOW_ID_LOOKUP_TABLE_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->ips.flow_id_lookup_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IPS_ID,
          SOC_PETRA_IPS_FLOW_ID_LOOKUP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->base_flow),
          &(IPS_flow_id_lookup_table_tbl_data->base_flow)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->sub_flow_mode),
          &(IPS_flow_id_lookup_table_tbl_data->sub_flow_mode)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ips_flow_id_lookup_table_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table flow_id_lookup_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ips_flow_id_lookup_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IPS_FLOW_ID_LOOKUP_TABLE_TBL_DATA* IPS_flow_id_lookup_table_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IPS_FLOW_ID_LOOKUP_TABLE_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IPS_FLOW_ID_LOOKUP_TABLE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPS_FLOW_ID_LOOKUP_TABLE_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->ips.flow_id_lookup_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IPS_flow_id_lookup_table_tbl_data->base_flow),
          &(tbl->base_flow),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(IPS_flow_id_lookup_table_tbl_data->sub_flow_mode),
          &(tbl->sub_flow_mode),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IPS_ID,
          SOC_PETRA_IPS_FLOW_ID_LOOKUP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ips_flow_id_lookup_table_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table queue_type_lookup_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ips_queue_type_lookup_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_DATA* IPS_queue_type_lookup_table_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IPS_queue_type_lookup_table_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->ips.queue_type_lookup_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IPS_ID,
          SOC_PETRA_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->queue_type_lookup_table),
          &(IPS_queue_type_lookup_table_tbl_data->queue_type_lookup_table)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ips_queue_type_lookup_table_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table queue_type_lookup_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ips_queue_type_lookup_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_DATA* IPS_queue_type_lookup_table_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->ips.queue_type_lookup_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IPS_queue_type_lookup_table_tbl_data->queue_type_lookup_table),
          &(tbl->queue_type_lookup_table),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IPS_ID,
          SOC_PETRA_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ips_queue_type_lookup_table_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table queue_priority_map_select_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ips_queue_priority_map_select_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_DATA* IPS_queue_priority_map_select_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IPS_queue_priority_map_select_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->ips.queue_priority_map_select_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IPS_ID,
          SOC_PETRA_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->queue_priority_map_select),
          &(IPS_queue_priority_map_select_tbl_data->queue_priority_map_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ips_queue_priority_map_select_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table queue_priority_map_select_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ips_queue_priority_map_select_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_DATA* IPS_queue_priority_map_select_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->ips.queue_priority_map_select_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IPS_queue_priority_map_select_tbl_data->queue_priority_map_select),
          &(tbl->queue_priority_map_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IPS_ID,
          SOC_PETRA_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ips_queue_priority_map_select_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table queue_priority_maps_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ips_queue_priority_maps_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_DATA* IPS_queue_priority_maps_table_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IPS_queue_priority_maps_table_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->ips.queue_priority_maps_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IPS_ID,
          SOC_PETRA_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->queue_priority_maps_table),
          (IPS_queue_priority_maps_table_tbl_data->queue_priority_maps_table)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ips_queue_priority_maps_table_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table queue_priority_maps_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ips_queue_priority_maps_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_DATA* IPS_queue_priority_maps_table_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->ips.queue_priority_maps_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          (IPS_queue_priority_maps_table_tbl_data->queue_priority_maps_table),
          &(tbl->queue_priority_maps_table),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IPS_ID,
          SOC_PETRA_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ips_queue_priority_maps_table_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table queue_size_based_thresholds_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ips_queue_size_based_thresholds_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_DATA* IPS_queue_size_based_thresholds_table_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IPS_queue_size_based_thresholds_table_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->ips.queue_size_based_thresholds_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IPS_ID,
          SOC_PETRA_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->off_to_slow_msg_th),
          &(IPS_queue_size_based_thresholds_table_tbl_data->off_to_slow_msg_th)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->off_to_norm_msg_th),
          &(IPS_queue_size_based_thresholds_table_tbl_data->off_to_norm_msg_th)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->slow_to_norm_msg_th),
          &(IPS_queue_size_based_thresholds_table_tbl_data->slow_to_norm_msg_th)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 42, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->norm_to_slow_msg_th),
          &(IPS_queue_size_based_thresholds_table_tbl_data->norm_to_slow_msg_th)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 43, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->fsm_th_mul),
          &(IPS_queue_size_based_thresholds_table_tbl_data->fsm_th_mul)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 44, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ips_queue_size_based_thresholds_table_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table queue_size_based_thresholds_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ips_queue_size_based_thresholds_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_DATA* IPS_queue_size_based_thresholds_table_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->ips.queue_size_based_thresholds_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IPS_queue_size_based_thresholds_table_tbl_data->off_to_slow_msg_th),
          &(tbl->off_to_slow_msg_th),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(IPS_queue_size_based_thresholds_table_tbl_data->off_to_norm_msg_th),
          &(tbl->off_to_norm_msg_th),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_petra_field_in_place_set(
          &(IPS_queue_size_based_thresholds_table_tbl_data->slow_to_norm_msg_th),
          &(tbl->slow_to_norm_msg_th),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

  err = soc_petra_field_in_place_set(
          &(IPS_queue_size_based_thresholds_table_tbl_data->norm_to_slow_msg_th),
          &(tbl->norm_to_slow_msg_th),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 33, exit);

  err = soc_petra_field_in_place_set(
          &(IPS_queue_size_based_thresholds_table_tbl_data->fsm_th_mul),
          &(tbl->fsm_th_mul),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 34, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IPS_ID,
          SOC_PETRA_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 35, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ips_queue_size_based_thresholds_table_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table credit_balance_based_thresholds_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ips_credit_balance_based_thresholds_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_DATA* IPS_credit_balance_based_thresholds_table_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IPS_credit_balance_based_thresholds_table_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->ips.credit_balance_based_thresholds_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IPS_ID,
          SOC_PETRA_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->backoff_enter_qcr_bal_th),
          &(IPS_credit_balance_based_thresholds_table_tbl_data->backoff_enter_qcr_bal_th)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->backoff_exit_qcr_bal_th),
          &(IPS_credit_balance_based_thresholds_table_tbl_data->backoff_exit_qcr_bal_th)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->backlog_enter_qcr_bal_th),
          &(IPS_credit_balance_based_thresholds_table_tbl_data->backlog_enter_qcr_bal_th)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 42, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->backlog_exit_qcr_bal_th),
          &(IPS_credit_balance_based_thresholds_table_tbl_data->backlog_exit_qcr_bal_th)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 43, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ips_credit_balance_based_thresholds_table_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table credit_balance_based_thresholds_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ips_credit_balance_based_thresholds_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_DATA* IPS_credit_balance_based_thresholds_table_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->ips.credit_balance_based_thresholds_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IPS_credit_balance_based_thresholds_table_tbl_data->backoff_enter_qcr_bal_th),
          &(tbl->backoff_enter_qcr_bal_th),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(IPS_credit_balance_based_thresholds_table_tbl_data->backoff_exit_qcr_bal_th),
          &(tbl->backoff_exit_qcr_bal_th),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_petra_field_in_place_set(
          &(IPS_credit_balance_based_thresholds_table_tbl_data->backlog_enter_qcr_bal_th),
          &(tbl->backlog_enter_qcr_bal_th),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

  err = soc_petra_field_in_place_set(
          &(IPS_credit_balance_based_thresholds_table_tbl_data->backlog_exit_qcr_bal_th),
          &(tbl->backlog_exit_qcr_bal_th),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 33, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IPS_ID,
          SOC_PETRA_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 34, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ips_credit_balance_based_thresholds_table_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table empty_queue_credit_balance_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ips_empty_queue_credit_balance_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_DATA* IPS_empty_queue_credit_balance_table_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IPS_empty_queue_credit_balance_table_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->ips.empty_queue_credit_balance_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IPS_ID,
          SOC_PETRA_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->empty_qsatisfied_cr_bal),
          &(IPS_empty_queue_credit_balance_table_tbl_data->empty_qsatisfied_cr_bal)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->max_empty_qcr_bal),
          &(IPS_empty_queue_credit_balance_table_tbl_data->max_empty_qcr_bal)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->exceed_max_empty_qcr_bal),
          &(IPS_empty_queue_credit_balance_table_tbl_data->exceed_max_empty_qcr_bal)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 42, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ips_empty_queue_credit_balance_table_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table empty_queue_credit_balance_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ips_empty_queue_credit_balance_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_DATA* IPS_empty_queue_credit_balance_table_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->ips.empty_queue_credit_balance_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IPS_empty_queue_credit_balance_table_tbl_data->empty_qsatisfied_cr_bal),
          &(tbl->empty_qsatisfied_cr_bal),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(IPS_empty_queue_credit_balance_table_tbl_data->max_empty_qcr_bal),
          &(tbl->max_empty_qcr_bal),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_petra_field_in_place_set(
          &(IPS_empty_queue_credit_balance_table_tbl_data->exceed_max_empty_qcr_bal),
          &(tbl->exceed_max_empty_qcr_bal),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IPS_ID,
          SOC_PETRA_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 33, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ips_empty_queue_credit_balance_table_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table credit_watchdog_thresholds_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ips_credit_watchdog_thresholds_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_DATA* IPS_credit_watchdog_thresholds_table_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IPS_credit_watchdog_thresholds_table_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->ips.credit_watchdog_thresholds_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IPS_ID,
          SOC_PETRA_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->wd_status_msg_gen_period),
          &(IPS_credit_watchdog_thresholds_table_tbl_data->wd_status_msg_gen_period)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->wd_delete_qth),
          &(IPS_credit_watchdog_thresholds_table_tbl_data->wd_delete_qth)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ips_credit_watchdog_thresholds_table_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table credit_watchdog_thresholds_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ips_credit_watchdog_thresholds_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_DATA* IPS_credit_watchdog_thresholds_table_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->ips.credit_watchdog_thresholds_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IPS_credit_watchdog_thresholds_table_tbl_data->wd_status_msg_gen_period),
          &(tbl->wd_status_msg_gen_period),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(IPS_credit_watchdog_thresholds_table_tbl_data->wd_delete_qth),
          &(tbl->wd_delete_qth),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IPS_ID,
          SOC_PETRA_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ips_credit_watchdog_thresholds_table_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table queue_descriptor_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ips_queue_descriptor_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_DATA* IPS_queue_descriptor_table_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IPS_QUEUE_DESCRIPTOR_TABLE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IPS_queue_descriptor_table_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->ips.queue_descriptor_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IPS_ID,
          SOC_PETRA_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->cr_bal),
          &(IPS_queue_descriptor_table_tbl_data->cr_bal)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->crs),
          &(IPS_queue_descriptor_table_tbl_data->crs)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->one_pkt_deq),
          &(IPS_queue_descriptor_table_tbl_data->one_pkt_deq)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 42, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->wd_last_cr_time),
          &(IPS_queue_descriptor_table_tbl_data->wd_last_cr_time)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 43, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->in_dqcq),
          &(IPS_queue_descriptor_table_tbl_data->in_dqcq)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 44, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->wd_delete),
          &(IPS_queue_descriptor_table_tbl_data->wd_delete)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 45, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->fsmrq_ctrl),
          &(IPS_queue_descriptor_table_tbl_data->fsmrq_ctrl)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 46, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ips_queue_descriptor_table_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table queue_descriptor_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ips_queue_descriptor_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_DATA* IPS_queue_descriptor_table_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IPS_QUEUE_DESCRIPTOR_TABLE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->ips.queue_descriptor_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IPS_queue_descriptor_table_tbl_data->cr_bal),
          &(tbl->cr_bal),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(IPS_queue_descriptor_table_tbl_data->crs),
          &(tbl->crs),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_petra_field_in_place_set(
          &(IPS_queue_descriptor_table_tbl_data->one_pkt_deq),
          &(tbl->one_pkt_deq),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

  err = soc_petra_field_in_place_set(
          &(IPS_queue_descriptor_table_tbl_data->wd_last_cr_time),
          &(tbl->wd_last_cr_time),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 33, exit);

  err = soc_petra_field_in_place_set(
          &(IPS_queue_descriptor_table_tbl_data->in_dqcq),
          &(tbl->in_dqcq),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 34, exit);

  err = soc_petra_field_in_place_set(
          &(IPS_queue_descriptor_table_tbl_data->wd_delete),
          &(tbl->wd_delete),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 35, exit);

  err = soc_petra_field_in_place_set(
          &(IPS_queue_descriptor_table_tbl_data->fsmrq_ctrl),
          &(tbl->fsmrq_ctrl),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 36, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IPS_ID,
          SOC_PETRA_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 37, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ips_queue_descriptor_table_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table queue_size_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ips_queue_size_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IPS_QUEUE_SIZE_TABLE_TBL_DATA* IPS_queue_size_table_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IPS_QUEUE_SIZE_TABLE_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IPS_QUEUE_SIZE_TABLE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPS_QUEUE_SIZE_TABLE_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IPS_queue_size_table_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IPS_QUEUE_SIZE_TABLE_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->ips.queue_size_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IPS_ID,
          SOC_PETRA_IPS_QUEUE_SIZE_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->exponent),
          &(IPS_queue_size_table_tbl_data->exponent)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->mantissa),
          &(IPS_queue_size_table_tbl_data->mantissa)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->qsize_4b),
          &(IPS_queue_size_table_tbl_data->qsize_4b)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 42, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ips_queue_size_table_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table queue_size_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ips_queue_size_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IPS_QUEUE_SIZE_TABLE_TBL_DATA* IPS_queue_size_table_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IPS_QUEUE_SIZE_TABLE_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IPS_QUEUE_SIZE_TABLE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPS_QUEUE_SIZE_TABLE_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->ips.queue_size_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IPS_queue_size_table_tbl_data->exponent),
          &(tbl->exponent),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(IPS_queue_size_table_tbl_data->mantissa),
          &(tbl->mantissa),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_petra_field_in_place_set(
          &(IPS_queue_size_table_tbl_data->qsize_4b),
          &(tbl->qsize_4b),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IPS_ID,
          SOC_PETRA_IPS_QUEUE_SIZE_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 33, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ips_queue_size_table_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table system_red_max_queue_size_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ips_system_red_max_queue_size_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_DATA* IPS_system_red_max_queue_size_table_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IPS_system_red_max_queue_size_table_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->ips.system_red_max_queue_size_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IPS_ID,
          SOC_PETRA_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->maxqsz),
          &(IPS_system_red_max_queue_size_table_tbl_data->maxqsz)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->maxqsz_age),
          &(IPS_system_red_max_queue_size_table_tbl_data->maxqsz_age)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ips_system_red_max_queue_size_table_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table system_red_max_queue_size_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ips_system_red_max_queue_size_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_DATA* IPS_system_red_max_queue_size_table_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->ips.system_red_max_queue_size_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(IPS_system_red_max_queue_size_table_tbl_data->maxqsz),
          &(tbl->maxqsz),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(IPS_system_red_max_queue_size_table_tbl_data->maxqsz_age),
          &(tbl->maxqsz_age),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IPS_ID,
          SOC_PETRA_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ips_system_red_max_queue_size_table_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table bdq_tbl from block IPT,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ipt_bdq_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IPT_BDQ_TBL_DATA* IPT_bdq_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IPT_BDQ_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IPT_BDQ_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPT_BDQ_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IPT_bdq_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IPT_BDQ_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->ipt.bdq_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IPT_ID,
          SOC_PETRA_IPT_BDQ_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->bdq),
          &(IPT_bdq_tbl_data->bdq)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ipt_bdq_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table bdq_tbl from block IPT,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ipt_bdq_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IPT_BDQ_TBL_DATA* IPT_bdq_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IPT_BDQ_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IPT_BDQ_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPT_BDQ_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->ipt.bdq_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          &(IPT_bdq_tbl_data->bdq),
          &(tbl->bdq),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IPT_ID,
          SOC_PETRA_IPT_BDQ_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ipt_bdq_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table pcq_tbl from block IPT,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ipt_pcq_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IPT_PCQ_TBL_DATA* IPT_pcq_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IPT_PCQ_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IPT_PCQ_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPT_PCQ_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IPT_pcq_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IPT_PCQ_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->ipt.pcq_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IPT_ID,
          SOC_PETRA_IPT_PCQ_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->pcq),
          (IPT_pcq_tbl_data->pcq)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ipt_pcq_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table pcq_tbl from block IPT,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ipt_pcq_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IPT_PCQ_TBL_DATA* IPT_pcq_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IPT_PCQ_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IPT_PCQ_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPT_PCQ_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->ipt.pcq_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          (IPT_pcq_tbl_data->pcq),
          &(tbl->pcq),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IPT_ID,
          SOC_PETRA_IPT_PCQ_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ipt_pcq_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table sop_mmu_tbl from block IPT,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ipt_sop_mmu_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IPT_SOP_MMU_TBL_DATA* IPT_sop_mmu_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IPT_SOP_MMU_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IPT_SOP_MMU_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPT_SOP_MMU_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IPT_sop_mmu_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IPT_SOP_MMU_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->ipt.sop_mmu_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IPT_ID,
          SOC_PETRA_IPT_SOP_MMU_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->sop_mmu),
          (IPT_sop_mmu_tbl_data->sop_mmu)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ipt_sop_mmu_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table sop_mmu_tbl from block IPT,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ipt_sop_mmu_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IPT_SOP_MMU_TBL_DATA* IPT_sop_mmu_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_IPT_SOP_MMU_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IPT_SOP_MMU_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPT_SOP_MMU_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->ipt.sop_mmu_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          (IPT_sop_mmu_tbl_data->sop_mmu),
          &(tbl->sop_mmu),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IPT_ID,
          SOC_PETRA_IPT_SOP_MMU_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ipt_sop_mmu_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table mop_mmu_tbl from block IPT,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ipt_mop_mmu_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IPT_MOP_MMU_TBL_DATA* IPT_mop_mmu_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IPT_MOP_MMU_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IPT_MOP_MMU_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPT_MOP_MMU_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IPT_mop_mmu_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IPT_MOP_MMU_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->ipt.mop_mmu_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IPT_ID,
          SOC_PETRA_IPT_MOP_MMU_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->mop_mmu),
          &(IPT_mop_mmu_tbl_data->mop_mmu)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ipt_mop_mmu_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table mop_mmu_tbl from block IPT,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ipt_mop_mmu_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IPT_MOP_MMU_TBL_DATA* IPT_mop_mmu_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IPT_MOP_MMU_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IPT_MOP_MMU_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPT_MOP_MMU_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->ipt.mop_mmu_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          &(IPT_mop_mmu_tbl_data->mop_mmu),
          &(tbl->mop_mmu),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IPT_ID,
          SOC_PETRA_IPT_MOP_MMU_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ipt_mop_mmu_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table fdtctl_tbl from block IPT,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ipt_fdtctl_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IPT_FDTCTL_TBL_DATA* IPT_fdtctl_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IPT_FDTCTL_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IPT_FDTCTL_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPT_FDTCTL_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IPT_fdtctl_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IPT_FDTCTL_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->ipt.fdtctl_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IPT_ID,
          SOC_PETRA_IPT_FDTCTL_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->fdtctl),
          &(IPT_fdtctl_tbl_data->fdtctl)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ipt_fdtctl_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table fdtctl_tbl from block IPT,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ipt_fdtctl_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IPT_FDTCTL_TBL_DATA* IPT_fdtctl_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IPT_FDTCTL_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IPT_FDTCTL_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPT_FDTCTL_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->ipt.fdtctl_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          &(IPT_fdtctl_tbl_data->fdtctl),
          &(tbl->fdtctl),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IPT_ID,
          SOC_PETRA_IPT_FDTCTL_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ipt_fdtctl_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table fdtdata_tbl from block IPT,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ipt_fdtdata_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IPT_FDTDATA_TBL_DATA* IPT_fdtdata_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IPT_FDTDATA_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IPT_FDTDATA_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPT_FDTDATA_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IPT_fdtdata_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IPT_FDTDATA_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->ipt.fdtdata_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IPT_ID,
          SOC_PETRA_IPT_FDTDATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->fdtdata),
          (IPT_fdtdata_tbl_data->fdtdata)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ipt_fdtdata_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table fdtdata_tbl from block IPT,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ipt_fdtdata_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IPT_FDTDATA_TBL_DATA* IPT_fdtdata_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IPT_FDTDATA_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IPT_FDTDATA_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPT_FDTDATA_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->ipt.fdtdata_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          (IPT_fdtdata_tbl_data->fdtdata),
          &(tbl->fdtdata),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IPT_ID,
          SOC_PETRA_IPT_FDTDATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ipt_fdtdata_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table egqctl_tbl from block IPT,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ipt_egqctl_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IPT_EGQCTL_TBL_DATA* IPT_egqctl_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IPT_EGQCTL_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IPT_EGQCTL_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPT_EGQCTL_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IPT_egqctl_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IPT_EGQCTL_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->ipt.egqctl_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IPT_ID,
          SOC_PETRA_IPT_EGQCTL_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->egqctl),
          &(IPT_egqctl_tbl_data->egqctl)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ipt_egqctl_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table egqctl_tbl from block IPT,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ipt_egqctl_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IPT_EGQCTL_TBL_DATA* IPT_egqctl_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IPT_EGQCTL_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IPT_EGQCTL_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPT_EGQCTL_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->ipt.egqctl_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          &(IPT_egqctl_tbl_data->egqctl),
          &(tbl->egqctl),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IPT_ID,
          SOC_PETRA_IPT_EGQCTL_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ipt_egqctl_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table egqdata_tbl from block IPT,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ipt_egqdata_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IPT_EGQDATA_TBL_DATA* IPT_egqdata_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IPT_EGQDATA_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IPT_EGQDATA_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPT_EGQDATA_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IPT_egqdata_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IPT_EGQDATA_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->ipt.egqdata_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IPT_ID,
          SOC_PETRA_IPT_EGQDATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->egqdata),
          (IPT_egqdata_tbl_data->egqdata)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ipt_egqdata_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table egqdata_tbl from block IPT,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ipt_egqdata_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IPT_EGQDATA_TBL_DATA* IPT_egqdata_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IPT_EGQDATA_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IPT_EGQDATA_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPT_EGQDATA_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->ipt.egqdata_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          (IPT_egqdata_tbl_data->egqdata),
          &(tbl->egqdata),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IPT_ID,
          SOC_PETRA_IPT_EGQDATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ipt_egqdata_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table dll_ram_tbl from block DPI,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_dpi_dll_ram_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   uint32             dram_ndx,
    SOC_SAND_OUT  SOC_PETRA_DPI_DLL_RAM_TBL_DATA* DPI_dll_ram_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_DPI_DLL_RAM_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_DPI_DLL_RAM_TBL
    *tbl;
  uint32
    module_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DPI_DLL_RAM_TBL_GET_UNSAFE);

  module_id = SOC_PETRA_DPI_A_ID + dram_ndx;

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          DPI_dll_ram_tbl_data,
          0x0,
          sizeof(SOC_PETRA_DPI_DLL_RAM_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->dpi.dll_ram_tbl);

  offset = (tbl->addr.base + entry_offset);

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          module_id,
          SOC_PETRA_DPI_DLL_RAM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 2, exit);
  

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->dll_ram),
          (DPI_dll_ram_tbl_data->dll_ram)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_dpi_dll_ram_tbl_get_unsafe()",dram_ndx,0);
}

/*
 * Write indirect table dll_ram_tbl from block DPI,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_dpi_dll_ram_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   uint32             dram_ndx,
    SOC_SAND_IN   SOC_PETRA_DPI_DLL_RAM_TBL_DATA* DPI_dll_ram_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_DPI_DLL_RAM_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_DPI_DLL_RAM_TBL
    *tbl;
  uint32
    module_id = SOC_PETRA_DPI_A_ID + dram_ndx;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DPI_DLL_RAM_TBL_SET_UNSAFE);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 1, exit);

  tbl = SOC_PETRA_TBL_REF(tables->dpi.dll_ram_tbl);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_field_in_place_set(
          (DPI_dll_ram_tbl_data->dll_ram),
          &(tbl->dll_ram),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  offset = (tbl->addr.base + entry_offset);

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          module_id,
          SOC_PETRA_DPI_DLL_RAM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 3, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_dpi_dll_ram_tbl_set_unsafe()",dram_ndx,0);
}

/*
 * Read indirect table unicast_distribution_memory_for_data_cells_tbl from block RTP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_rtp_unicast_distribution_memory_for_data_cells_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_DATA* RTP_unicast_distribution_memory_for_data_cells_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          RTP_unicast_distribution_memory_for_data_cells_tbl_data,
          0x0,
          sizeof(SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->rtp.unicast_distribution_memory_for_data_cells_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_RTP_ID,
          SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->unicast_distribution_memory_for_data_cells),
          (RTP_unicast_distribution_memory_for_data_cells_tbl_data->unicast_distribution_memory_for_data_cells)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_rtp_unicast_distribution_memory_for_data_cells_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table unicast_distribution_memory_for_data_cells_tbl from block RTP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_rtp_unicast_distribution_memory_for_data_cells_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_DATA* RTP_unicast_distribution_memory_for_data_cells_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL
    *tbl;
  uint32
    ind_options;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->rtp.unicast_distribution_memory_for_data_cells_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          (RTP_unicast_distribution_memory_for_data_cells_tbl_data->unicast_distribution_memory_for_data_cells),
          &(tbl->unicast_distribution_memory_for_data_cells),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  ind_options = SOC_PETRA_TBL_WRITE_IND_OPT_SET(SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_ENTRY_SIZE, FALSE);
  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_RTP_ID,
          ind_options
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);
  SOC_PETRA_TBL_REPS_RESET;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_rtp_unicast_distribution_memory_for_data_cells_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table unicast_distribution_memory_for_control_cells_tbl from block RTP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_rtp_unicast_distribution_memory_for_control_cells_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_DATA* RTP_unicast_distribution_memory_for_control_cells_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          RTP_unicast_distribution_memory_for_control_cells_tbl_data,
          0x0,
          sizeof(SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->rtp.unicast_distribution_memory_for_control_cells_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_RTP_ID,
          SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->unicast_distribution_memory_for_control_cells),
          (RTP_unicast_distribution_memory_for_control_cells_tbl_data->unicast_distribution_memory_for_control_cells)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_rtp_unicast_distribution_memory_for_control_cells_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table unicast_distribution_memory_for_control_cells_tbl from block RTP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_rtp_unicast_distribution_memory_for_control_cells_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_DATA* RTP_unicast_distribution_memory_for_control_cells_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL
    *tbl;
  uint32
    ind_options;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->rtp.unicast_distribution_memory_for_control_cells_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          (RTP_unicast_distribution_memory_for_control_cells_tbl_data->unicast_distribution_memory_for_control_cells),
          &(tbl->unicast_distribution_memory_for_control_cells),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  ind_options = SOC_PETRA_TBL_WRITE_IND_OPT_SET(SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_ENTRY_SIZE, FALSE);
  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_RTP_ID,
          ind_options
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);
  SOC_PETRA_TBL_REPS_RESET;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_rtp_unicast_distribution_memory_for_control_cells_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table nif_scm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_nif_scm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32             mal_ndx,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_SCM_TBL_DATA* EGQ_nif_scm_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_NIF_SCM_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_NIF_SCM_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_NIF_SCM_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          EGQ_nif_scm_tbl_data,
          0x0,
          sizeof(SOC_PETRA_EGQ_SCM_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.nif_scm_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  /*
   *  Got to the correct table, per MAL index
   */
  offset += mal_ndx * SOC_PETRA_EGQ_NIF_SCM_TBL_OFFSET;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_NIF_SCM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->port_cr_to_add),
          &(EGQ_nif_scm_tbl_data->port_cr_to_add)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->ofp_index),
          &(EGQ_nif_scm_tbl_data->ofp_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_nif_scm_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table nif_scm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_nif_scm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32             mal_ndx,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_SCM_TBL_DATA* EGQ_nif_scm_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_NIF_SCM_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_NIF_SCM_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_NIF_SCM_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.nif_scm_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  /*
   *  Got to the correct table, per MAL index
   */
  offset += mal_ndx * SOC_PETRA_EGQ_NIF_SCM_TBL_OFFSET;

  err = soc_petra_field_in_place_set(
          &(EGQ_nif_scm_tbl_data->port_cr_to_add),
          &(tbl->port_cr_to_add),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(EGQ_nif_scm_tbl_data->ofp_index),
          &(tbl->ofp_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_NIF_SCM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_nif_scm_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table nifab_nch_scm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_nifab_nch_scm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_NIFAB_NCH_SCM_TBL_DATA* EGQ_nifab_nch_scm_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_NIFAB_NCH_SCM_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_NIFAB_NCH_SCM_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_NIFAB_NCH_SCM_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          EGQ_nifab_nch_scm_tbl_data,
          0x0,
          sizeof(SOC_PETRA_EGQ_NIFAB_NCH_SCM_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->egq.nifab_nch_scm_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_NIFAB_NCH_SCM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->port_cr_to_add),
          &(EGQ_nifab_nch_scm_tbl_data->port_cr_to_add)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->ofp_index),
          &(EGQ_nifab_nch_scm_tbl_data->ofp_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_nifab_nch_scm_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table nifab_nch_scm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_nifab_nch_scm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_NIFAB_NCH_SCM_TBL_DATA* EGQ_nifab_nch_scm_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_NIFAB_NCH_SCM_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_NIFAB_NCH_SCM_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_NIFAB_NCH_SCM_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->egq.nifab_nch_scm_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(EGQ_nifab_nch_scm_tbl_data->port_cr_to_add),
          &(tbl->port_cr_to_add),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(EGQ_nifab_nch_scm_tbl_data->ofp_index),
          &(tbl->ofp_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_NIFAB_NCH_SCM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_nifab_nch_scm_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table rcy_scm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_rcy_scm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_SCM_TBL_DATA* EGQ_rcy_scm_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_RCY_SCM_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_RCY_SCM_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_RCY_SCM_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          EGQ_rcy_scm_tbl_data,
          0x0,
          sizeof(SOC_PETRA_EGQ_SCM_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.rcy_scm_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_RCY_SCM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->port_cr_to_add),
          &(EGQ_rcy_scm_tbl_data->port_cr_to_add)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->ofp_index),
          &(EGQ_rcy_scm_tbl_data->ofp_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_rcy_scm_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table rcy_scm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_rcy_scm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_SCM_TBL_DATA* EGQ_rcy_scm_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_RCY_SCM_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_RCY_SCM_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_RCY_SCM_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.rcy_scm_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(EGQ_rcy_scm_tbl_data->port_cr_to_add),
          &(tbl->port_cr_to_add),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(EGQ_rcy_scm_tbl_data->ofp_index),
          &(tbl->ofp_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_RCY_SCM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_rcy_scm_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table cpu_scm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_cpu_scm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_SCM_TBL_DATA* EGQ_cpu_scm_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_CPU_SCM_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_CPU_SCM_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_CPU_SCM_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          EGQ_cpu_scm_tbl_data,
          0x0,
          sizeof(SOC_PETRA_EGQ_SCM_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.cpu_scm_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_CPU_SCM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->port_cr_to_add),
          &(EGQ_cpu_scm_tbl_data->port_cr_to_add)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->ofp_index),
          &(EGQ_cpu_scm_tbl_data->ofp_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_cpu_scm_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table cpu_scm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_cpu_scm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_SCM_TBL_DATA* EGQ_cpu_scm_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_CPU_SCM_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_CPU_SCM_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_CPU_SCM_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.cpu_scm_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(EGQ_cpu_scm_tbl_data->port_cr_to_add),
          &(tbl->port_cr_to_add),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(EGQ_cpu_scm_tbl_data->ofp_index),
          &(tbl->ofp_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_CPU_SCM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_cpu_scm_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table ccm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_ccm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_CCM_TBL_DATA* EGQ_ccm_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_CCM_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_CCM_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_CCM_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          EGQ_ccm_tbl_data,
          0x0,
          sizeof(SOC_PETRA_EGQ_CCM_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.ccm_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_CCM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->interface_select),
          &(EGQ_ccm_tbl_data->interface_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_ccm_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table ccm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_ccm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_CCM_TBL_DATA* EGQ_ccm_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_CCM_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_CCM_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_CCM_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.ccm_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(EGQ_ccm_tbl_data->interface_select),
          &(tbl->interface_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_CCM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_ccm_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table pmc_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_pmc_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_PMC_TBL_DATA* EGQ_pmc_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_PMC_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_PMC_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_PMC_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          EGQ_pmc_tbl_data,
          0x0,
          sizeof(SOC_PETRA_EGQ_PMC_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.pmc_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_PMC_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->port_max_credit),
          &(EGQ_pmc_tbl_data->port_max_credit)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_pmc_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table pmc_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_pmc_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_PMC_TBL_DATA* EGQ_pmc_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_PMC_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_PMC_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_PMC_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.pmc_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(EGQ_pmc_tbl_data->port_max_credit),
          &(tbl->port_max_credit),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_PMC_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_pmc_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table cbm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_cbm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_CBM_TBL_DATA* EGQ_cbm_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_CBM_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_CBM_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_CBM_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          EGQ_cbm_tbl_data,
          0x0,
          sizeof(SOC_PETRA_EGQ_CBM_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.cbm_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_CBM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->cbm),
          &(EGQ_cbm_tbl_data->cbm)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_cbm_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table cbm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_cbm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_CBM_TBL_DATA* EGQ_cbm_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_CBM_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_CBM_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_CBM_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.cbm_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(EGQ_cbm_tbl_data->cbm),
          &(tbl->cbm),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_CBM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_cbm_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table fbm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_fbm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_FBM_TBL_DATA* EGQ_fbm_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_FBM_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_FBM_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_FBM_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          EGQ_fbm_tbl_data,
          0x0,
          sizeof(SOC_PETRA_EGQ_FBM_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.fbm_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_FBM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->free_buffer_memory),
          &(EGQ_fbm_tbl_data->free_buffer_memory)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_fbm_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table fbm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_fbm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_FBM_TBL_DATA* EGQ_fbm_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_FBM_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_FBM_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_FBM_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.fbm_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(EGQ_fbm_tbl_data->free_buffer_memory),
          &(tbl->free_buffer_memory),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_FBM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_fbm_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table fdm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_fdm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_FDM_TBL_DATA* EGQ_fdm_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_FDM_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_FDM_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_FDM_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          EGQ_fdm_tbl_data,
          0x0,
          sizeof(SOC_PETRA_EGQ_FDM_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.fdm_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_FDM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->free_descriptor_memory),
          (EGQ_fdm_tbl_data->free_descriptor_memory)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_fdm_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table fdm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_fdm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_FDM_TBL_DATA* EGQ_fdm_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_FDM_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_FDM_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_FDM_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.fdm_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          (EGQ_fdm_tbl_data->free_descriptor_memory),
          &(tbl->free_descriptor_memory),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_FDM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_fdm_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table dwm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_dwm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_DWM_TBL_DATA* EGQ_dwm_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_DWM_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_DWM_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_DWM_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          EGQ_dwm_tbl_data,
          0x0,
          sizeof(SOC_PETRA_EGQ_DWM_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.dwm_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_DWM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->mc_or_mc_low_queue_weight),
          &(EGQ_dwm_tbl_data->mc_or_mc_low_queue_weight)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->uc_or_uc_low_queue_weight),
          &(EGQ_dwm_tbl_data->uc_or_uc_low_queue_weight)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_dwm_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table dwm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_dwm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_DWM_TBL_DATA* EGQ_dwm_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_DWM_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_DWM_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_DWM_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.dwm_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(EGQ_dwm_tbl_data->mc_or_mc_low_queue_weight),
          &(tbl->mc_or_mc_low_queue_weight),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(EGQ_dwm_tbl_data->uc_or_uc_low_queue_weight),
          &(tbl->uc_or_uc_low_queue_weight),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_DWM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_dwm_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table rrdm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_rrdm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_RRDM_TBL_DATA* EGQ_rrdm_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_RRDM_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_RRDM_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_RRDM_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          EGQ_rrdm_tbl_data,
          0x0,
          sizeof(SOC_PETRA_EGQ_RRDM_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.rrdm_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_RRDM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->crcremainder),
          &(EGQ_rrdm_tbl_data->crcremainder)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->reas_state),
          &(EGQ_rrdm_tbl_data->reas_state)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->eopfrag_num),
          &(EGQ_rrdm_tbl_data->eopfrag_num)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 42, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->nxt_frag_number),
          &(EGQ_rrdm_tbl_data->nxt_frag_number)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 43, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->stored_seg_size),
          &(EGQ_rrdm_tbl_data->stored_seg_size)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 44, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->fix129),
          &(EGQ_rrdm_tbl_data->fix129)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 45, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_rrdm_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table rrdm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_rrdm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_RRDM_TBL_DATA* EGQ_rrdm_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_RRDM_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_RRDM_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_RRDM_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.rrdm_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(EGQ_rrdm_tbl_data->crcremainder),
          &(tbl->crcremainder),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(EGQ_rrdm_tbl_data->reas_state),
          &(tbl->reas_state),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_petra_field_in_place_set(
          &(EGQ_rrdm_tbl_data->eopfrag_num),
          &(tbl->eopfrag_num),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

  err = soc_petra_field_in_place_set(
          &(EGQ_rrdm_tbl_data->nxt_frag_number),
          &(tbl->nxt_frag_number),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 33, exit);

  err = soc_petra_field_in_place_set(
          &(EGQ_rrdm_tbl_data->stored_seg_size),
          &(tbl->stored_seg_size),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 34, exit);

  err = soc_petra_field_in_place_set(
          &(EGQ_rrdm_tbl_data->fix129),
          &(tbl->fix129),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 35, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_RRDM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 36, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_rrdm_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table rpdm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_rpdm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_RPDM_TBL_DATA* EGQ_rpdm_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_RPDM_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_RPDM_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_RPDM_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          EGQ_rpdm_tbl_data,
          0x0,
          sizeof(SOC_PETRA_EGQ_RPDM_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.rpdm_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_RPDM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->packet_start_buffer_pointer),
          &(EGQ_rpdm_tbl_data->packet_start_buffer_pointer)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->packet_buffer_write_pointer),
          &(EGQ_rpdm_tbl_data->packet_buffer_write_pointer)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->reas_state),
          &(EGQ_rpdm_tbl_data->reas_state)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 42, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->packet_frag_cnt),
          &(EGQ_rpdm_tbl_data->packet_frag_cnt)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 43, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_rpdm_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table rpdm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_rpdm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_RPDM_TBL_DATA* EGQ_rpdm_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_RPDM_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_RPDM_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_RPDM_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.rpdm_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(EGQ_rpdm_tbl_data->packet_start_buffer_pointer),
          &(tbl->packet_start_buffer_pointer),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(EGQ_rpdm_tbl_data->packet_buffer_write_pointer),
          &(tbl->packet_buffer_write_pointer),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_petra_field_in_place_set(
          &(EGQ_rpdm_tbl_data->reas_state),
          &(tbl->reas_state),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

  err = soc_petra_field_in_place_set(
          &(EGQ_rpdm_tbl_data->packet_frag_cnt),
          &(tbl->packet_frag_cnt),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 33, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_RPDM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 34, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_rpdm_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table pct_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_pct_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_PCT_TBL_DATA* EGQ_pct_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_PCT_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_PCT_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_PCT_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          EGQ_pct_tbl_data,
          0x0,
          sizeof(SOC_PETRA_EGQ_PCT_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->egq.pct_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_PCT_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->outbound_mirr),
          &(EGQ_pct_tbl_data->outbound_mirr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->is_mirroring),
          &(EGQ_pct_tbl_data->is_mirroring)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->otmh_lif_ext_ena),
          &(EGQ_pct_tbl_data->otmh_lif_ext_ena)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 42, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->otmh_dest_ext_ena),
          &(EGQ_pct_tbl_data->otmh_dest_ext_ena)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 43, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->otmh_src_ext_ena),
          &(EGQ_pct_tbl_data->otmh_src_ext_ena)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 44, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->port_type),
          &(EGQ_pct_tbl_data->port_type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 45, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->port_channel_number),
          &(EGQ_pct_tbl_data->port_channel_number)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 46, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->header_compensation_type),
          &(EGQ_pct_tbl_data->header_compensation_type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 47, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_pct_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table pct_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_pct_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_PCT_TBL_DATA* EGQ_pct_tbl_data
  )
{
  uint32
    res;
  uint32
    offset  = 0,
    nof_reps,
    data[SOC_PETRA_EGQ_PCT_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_PCT_TBL
    *tbl;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_PCT_TBL_SET_UNSAFE);

  regs = soc_petra_regs();

  res = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->egq.pct_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;
 /*
  * get counter before get data and store it a side.
  */
  SOC_PA_FLD_GET(regs->egq.indirect_command_reg.indirect_command_count, nof_reps, 20, exit);

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_PCT_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

 /*
  * restore counter
  */
  res = soc_petra_egq_set_reps_for_tbl_unsafe(unit, nof_reps);
  SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);

  res = soc_petra_field_in_place_set(
          &(EGQ_pct_tbl_data->outbound_mirr),
          &(tbl->outbound_mirr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(EGQ_pct_tbl_data->is_mirroring),
          &(tbl->is_mirroring),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(EGQ_pct_tbl_data->otmh_lif_ext_ena),
          &(tbl->otmh_lif_ext_ena),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(EGQ_pct_tbl_data->otmh_dest_ext_ena),
          &(tbl->otmh_dest_ext_ena),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(EGQ_pct_tbl_data->otmh_src_ext_ena),
          &(tbl->otmh_src_ext_ena),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(EGQ_pct_tbl_data->port_type),
          &(tbl->port_type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(EGQ_pct_tbl_data->port_channel_number),
          &(tbl->port_channel_number),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_petra_field_in_place_set(
          &(EGQ_pct_tbl_data->header_compensation_type),
          &(tbl->header_compensation_type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_PCT_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_pct_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table ppct_tbl from block EGQ,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_ppct_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_PPCT_TBL_DATA* EGQ_ppct_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_PPCT_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_PPCT_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_PPCT_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          EGQ_ppct_tbl_data,
          0x0,
          sizeof(SOC_PETRA_EGQ_PPCT_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->egq.ppct_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_PPCT_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->multicast_lag_load_balancing_enable),
          &(EGQ_ppct_tbl_data->multicast_lag_load_balancing_enable)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 43, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->sys_port_id),
          &(EGQ_ppct_tbl_data->sys_port_id)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 44, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->glag_member_port_id),
          &(EGQ_ppct_tbl_data->glag_member_port_id)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 45, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->glag_port_id),
          &(EGQ_ppct_tbl_data->glag_port_id)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 46, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->port_is_glag_member),
          &(EGQ_ppct_tbl_data->port_is_glag_member)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 47, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->port_type),
          &(EGQ_ppct_tbl_data->port_type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 61, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_ppct_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table ppct_tbl from block EGQ,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_ppct_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_PPCT_TBL_DATA* EGQ_ppct_tbl_data
  )
{
  uint32
    res;
  uint32
    offset  = 0,
    nof_reps,
    data[SOC_PETRA_EGQ_PPCT_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_PPCT_TBL
    *tbl;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_PPCT_TBL_SET_UNSAFE);

  regs = soc_petra_regs();

  res = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->egq.ppct_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

 /*
  * get counter before get data and store it a side.
  */
  SOC_PA_FLD_GET(regs->egq.indirect_command_reg.indirect_command_count, nof_reps, 22, exit);

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_PPCT_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

 /*
  * restore counter
  */
  res = soc_petra_egq_set_reps_for_tbl_unsafe(unit, nof_reps);
  SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);

  res = soc_petra_field_in_place_set(
          &(EGQ_ppct_tbl_data->multicast_lag_load_balancing_enable),
          &(tbl->multicast_lag_load_balancing_enable),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(EGQ_ppct_tbl_data->sys_port_id),
          &(tbl->sys_port_id),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(EGQ_ppct_tbl_data->glag_member_port_id),
          &(tbl->glag_member_port_id),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(EGQ_ppct_tbl_data->glag_port_id),
          &(tbl->glag_port_id),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_petra_field_in_place_set(
          &(EGQ_ppct_tbl_data->port_is_glag_member),
          &(tbl->port_is_glag_member),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  res = soc_petra_field_in_place_set(
          &(EGQ_ppct_tbl_data->port_type),
          &(tbl->port_type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 51, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_PPCT_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_ppct_tbl_set_unsafe()",0,0);
}


/*
 * Read indirect table vlan_table_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_vlan_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_VLAN_TABLE_TBL_DATA* EGQ_vlan_table_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_VLAN_TABLE_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_VLAN_TABLE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_VLAN_TABLE_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          EGQ_vlan_table_tbl_data,
          0x0,
          sizeof(SOC_PETRA_EGQ_VLAN_TABLE_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.vlan_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_VLAN_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->vlan_membership),
          (EGQ_vlan_table_tbl_data->vlan_membership)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_vlan_table_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table vlan_table_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_vlan_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_VLAN_TABLE_TBL_DATA* EGQ_vlan_table_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_VLAN_TABLE_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_VLAN_TABLE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_VLAN_TABLE_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.vlan_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          (EGQ_vlan_table_tbl_data->vlan_membership),
          &(tbl->vlan_membership),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_VLAN_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_vlan_table_tbl_set_unsafe()",0,0);
}


/*
 * Read indirect table cfc_flow_control_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_cfc_flow_control_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_CFC_FLOW_CONTROL_TBL_DATA* EGQ_cfc_flow_control_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_CFC_FLOW_CONTROL_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_CFC_FLOW_CONTROL_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_CFC_FLOW_CONTROL_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          EGQ_cfc_flow_control_tbl_data,
          0x0,
          sizeof(SOC_PETRA_EGQ_CFC_FLOW_CONTROL_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.cfc_flow_control_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_CFC_FLOW_CONTROL_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->cfc_flow_control),
          (EGQ_cfc_flow_control_tbl_data->cfc_flow_control)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_cfc_flow_control_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table cfc_flow_control_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_cfc_flow_control_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_CFC_FLOW_CONTROL_TBL_DATA* EGQ_cfc_flow_control_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_CFC_FLOW_CONTROL_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_CFC_FLOW_CONTROL_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_CFC_FLOW_CONTROL_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.cfc_flow_control_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          (EGQ_cfc_flow_control_tbl_data->cfc_flow_control),
          &(tbl->cfc_flow_control),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_CFC_FLOW_CONTROL_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_cfc_flow_control_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table nifa_flow_control_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_nifa_flow_control_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_NIFA_FLOW_CONTROL_TBL_DATA* EGQ_nifa_flow_control_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_NIFA_FLOW_CONTROL_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_NIFA_FLOW_CONTROL_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_NIFA_FLOW_CONTROL_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          EGQ_nifa_flow_control_tbl_data,
          0x0,
          sizeof(SOC_PETRA_EGQ_NIFA_FLOW_CONTROL_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.nifa_flow_control_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_NIFA_FLOW_CONTROL_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->nifa_flow_control),
          &(EGQ_nifa_flow_control_tbl_data->nifa_flow_control)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_nifa_flow_control_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table nifa_flow_control_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_nifa_flow_control_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_NIFA_FLOW_CONTROL_TBL_DATA* EGQ_nifa_flow_control_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_NIFA_FLOW_CONTROL_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_NIFA_FLOW_CONTROL_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_NIFA_FLOW_CONTROL_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.nifa_flow_control_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          &(EGQ_nifa_flow_control_tbl_data->nifa_flow_control),
          &(tbl->nifa_flow_control),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_NIFA_FLOW_CONTROL_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_nifa_flow_control_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table nifb_flow_control_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_nifb_flow_control_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_NIFB_FLOW_CONTROL_TBL_DATA* EGQ_nifb_flow_control_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_NIFB_FLOW_CONTROL_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_NIFB_FLOW_CONTROL_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_NIFB_FLOW_CONTROL_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          EGQ_nifb_flow_control_tbl_data,
          0x0,
          sizeof(SOC_PETRA_EGQ_NIFB_FLOW_CONTROL_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.nifb_flow_control_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_NIFB_FLOW_CONTROL_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->nifb_flow_control),
          &(EGQ_nifb_flow_control_tbl_data->nifb_flow_control)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_nifb_flow_control_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table nifb_flow_control_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_nifb_flow_control_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_NIFB_FLOW_CONTROL_TBL_DATA* EGQ_nifb_flow_control_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_NIFB_FLOW_CONTROL_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_NIFB_FLOW_CONTROL_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_NIFB_FLOW_CONTROL_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.nifb_flow_control_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          &(EGQ_nifb_flow_control_tbl_data->nifb_flow_control),
          &(tbl->nifb_flow_control),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_NIFB_FLOW_CONTROL_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_nifb_flow_control_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table cpu_last_header_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_cpu_last_header_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_CPU_LAST_HEADER_TBL_DATA* EGQ_cpu_last_header_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_CPU_LAST_HEADER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_CPU_LAST_HEADER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_CPU_LAST_HEADER_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          EGQ_cpu_last_header_tbl_data,
          0x0,
          sizeof(SOC_PETRA_EGQ_CPU_LAST_HEADER_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.cpu_last_header_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_CPU_LAST_HEADER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->cpu_last_header),
          (EGQ_cpu_last_header_tbl_data->cpu_last_header)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_cpu_last_header_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table cpu_last_header_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_cpu_last_header_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_CPU_LAST_HEADER_TBL_DATA* EGQ_cpu_last_header_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_CPU_LAST_HEADER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_CPU_LAST_HEADER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_CPU_LAST_HEADER_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.cpu_last_header_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          (EGQ_cpu_last_header_tbl_data->cpu_last_header),
          &(tbl->cpu_last_header),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_CPU_LAST_HEADER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_cpu_last_header_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table ipt_last_header_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_ipt_last_header_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_IPT_LAST_HEADER_TBL_DATA* EGQ_ipt_last_header_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_IPT_LAST_HEADER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_IPT_LAST_HEADER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_IPT_LAST_HEADER_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          EGQ_ipt_last_header_tbl_data,
          0x0,
          sizeof(SOC_PETRA_EGQ_IPT_LAST_HEADER_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.ipt_last_header_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_IPT_LAST_HEADER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->ipt_last_header),
          (EGQ_ipt_last_header_tbl_data->ipt_last_header)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_ipt_last_header_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table ipt_last_header_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_ipt_last_header_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_IPT_LAST_HEADER_TBL_DATA* EGQ_ipt_last_header_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_IPT_LAST_HEADER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_IPT_LAST_HEADER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_IPT_LAST_HEADER_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.ipt_last_header_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          (EGQ_ipt_last_header_tbl_data->ipt_last_header),
          &(tbl->ipt_last_header),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_IPT_LAST_HEADER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_ipt_last_header_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table fdr_last_header_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_fdr_last_header_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_FDR_LAST_HEADER_TBL_DATA* EGQ_fdr_last_header_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_FDR_LAST_HEADER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_FDR_LAST_HEADER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_FDR_LAST_HEADER_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          EGQ_fdr_last_header_tbl_data,
          0x0,
          sizeof(SOC_PETRA_EGQ_FDR_LAST_HEADER_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.fdr_last_header_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_FDR_LAST_HEADER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->fdr_last_header),
          (EGQ_fdr_last_header_tbl_data->fdr_last_header)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_fdr_last_header_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table fdr_last_header_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_fdr_last_header_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_FDR_LAST_HEADER_TBL_DATA* EGQ_fdr_last_header_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_FDR_LAST_HEADER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_FDR_LAST_HEADER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_FDR_LAST_HEADER_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.fdr_last_header_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          (EGQ_fdr_last_header_tbl_data->fdr_last_header),
          &(tbl->fdr_last_header),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_FDR_LAST_HEADER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_fdr_last_header_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table cpu_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_cpu_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_CPU_PACKET_COUNTER_TBL_DATA* EGQ_cpu_packet_counter_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_CPU_PACKET_COUNTER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_CPU_PACKET_COUNTER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_CPU_PACKET_COUNTER_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          EGQ_cpu_packet_counter_tbl_data,
          0x0,
          sizeof(SOC_PETRA_EGQ_CPU_PACKET_COUNTER_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.cpu_packet_counter_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_CPU_PACKET_COUNTER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->cpu_packet_counter),
          (EGQ_cpu_packet_counter_tbl_data->cpu_packet_counter)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_cpu_packet_counter_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table cpu_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_cpu_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_CPU_PACKET_COUNTER_TBL_DATA* EGQ_cpu_packet_counter_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_CPU_PACKET_COUNTER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_CPU_PACKET_COUNTER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_CPU_PACKET_COUNTER_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.cpu_packet_counter_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          (EGQ_cpu_packet_counter_tbl_data->cpu_packet_counter),
          &(tbl->cpu_packet_counter),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_CPU_PACKET_COUNTER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_cpu_packet_counter_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table ipt_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_ipt_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_IPT_PACKET_COUNTER_TBL_DATA* EGQ_ipt_packet_counter_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_IPT_PACKET_COUNTER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_IPT_PACKET_COUNTER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_IPT_PACKET_COUNTER_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          EGQ_ipt_packet_counter_tbl_data,
          0x0,
          sizeof(SOC_PETRA_EGQ_IPT_PACKET_COUNTER_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.ipt_packet_counter_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_IPT_PACKET_COUNTER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->ipt_packet_counter),
          (EGQ_ipt_packet_counter_tbl_data->ipt_packet_counter)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_ipt_packet_counter_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table ipt_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_ipt_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_IPT_PACKET_COUNTER_TBL_DATA* EGQ_ipt_packet_counter_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_IPT_PACKET_COUNTER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_IPT_PACKET_COUNTER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_IPT_PACKET_COUNTER_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.ipt_packet_counter_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          (EGQ_ipt_packet_counter_tbl_data->ipt_packet_counter),
          &(tbl->ipt_packet_counter),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_IPT_PACKET_COUNTER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_ipt_packet_counter_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table fdr_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_fdr_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_FDR_PACKET_COUNTER_TBL_DATA* EGQ_fdr_packet_counter_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_FDR_PACKET_COUNTER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_FDR_PACKET_COUNTER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_FDR_PACKET_COUNTER_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          EGQ_fdr_packet_counter_tbl_data,
          0x0,
          sizeof(SOC_PETRA_EGQ_FDR_PACKET_COUNTER_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.fdr_packet_counter_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_FDR_PACKET_COUNTER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->fdr_packet_counter),
          (EGQ_fdr_packet_counter_tbl_data->fdr_packet_counter)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_fdr_packet_counter_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table fdr_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_fdr_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_FDR_PACKET_COUNTER_TBL_DATA* EGQ_fdr_packet_counter_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_FDR_PACKET_COUNTER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_FDR_PACKET_COUNTER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_FDR_PACKET_COUNTER_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.fdr_packet_counter_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          (EGQ_fdr_packet_counter_tbl_data->fdr_packet_counter),
          &(tbl->fdr_packet_counter),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_FDR_PACKET_COUNTER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_fdr_packet_counter_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table rqp_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_rqp_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_RQP_PACKET_COUNTER_TBL_DATA* EGQ_rqp_packet_counter_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_RQP_PACKET_COUNTER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_RQP_PACKET_COUNTER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_RQP_PACKET_COUNTER_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          EGQ_rqp_packet_counter_tbl_data,
          0x0,
          sizeof(SOC_PETRA_EGQ_RQP_PACKET_COUNTER_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.rqp_packet_counter_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_RQP_PACKET_COUNTER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->rqp_packet_counter),
          (EGQ_rqp_packet_counter_tbl_data->rqp_packet_counter)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_rqp_packet_counter_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table rqp_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_rqp_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_RQP_PACKET_COUNTER_TBL_DATA* EGQ_rqp_packet_counter_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_RQP_PACKET_COUNTER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_RQP_PACKET_COUNTER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_RQP_PACKET_COUNTER_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.rqp_packet_counter_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          (EGQ_rqp_packet_counter_tbl_data->rqp_packet_counter),
          &(tbl->rqp_packet_counter),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_RQP_PACKET_COUNTER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_rqp_packet_counter_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table rqp_discard_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_rqp_discard_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL_DATA* EGQ_rqp_discard_packet_counter_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          EGQ_rqp_discard_packet_counter_tbl_data,
          0x0,
          sizeof(SOC_PETRA_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.rqp_discard_packet_counter_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->rqp_discard_packet_counter),
          (EGQ_rqp_discard_packet_counter_tbl_data->rqp_discard_packet_counter)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_rqp_discard_packet_counter_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table rqp_discard_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_rqp_discard_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL_DATA* EGQ_rqp_discard_packet_counter_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.rqp_discard_packet_counter_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          (EGQ_rqp_discard_packet_counter_tbl_data->rqp_discard_packet_counter),
          &(tbl->rqp_discard_packet_counter),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_rqp_discard_packet_counter_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table ehp_unicast_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_ehp_unicast_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL_DATA* EGQ_ehp_unicast_packet_counter_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          EGQ_ehp_unicast_packet_counter_tbl_data,
          0x0,
          sizeof(SOC_PETRA_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.ehp_unicast_packet_counter_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->ehp_unicast_packet_counter),
          (EGQ_ehp_unicast_packet_counter_tbl_data->ehp_unicast_packet_counter)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_ehp_unicast_packet_counter_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table ehp_unicast_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_ehp_unicast_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL_DATA* EGQ_ehp_unicast_packet_counter_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.ehp_unicast_packet_counter_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          (EGQ_ehp_unicast_packet_counter_tbl_data->ehp_unicast_packet_counter),
          &(tbl->ehp_unicast_packet_counter),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_ehp_unicast_packet_counter_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table ehp_multicast_high_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_ehp_multicast_high_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL_DATA* EGQ_ehp_multicast_high_packet_counter_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          EGQ_ehp_multicast_high_packet_counter_tbl_data,
          0x0,
          sizeof(SOC_PETRA_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.ehp_multicast_high_packet_counter_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->ehp_multicast_high_packet_counter),
          (EGQ_ehp_multicast_high_packet_counter_tbl_data->ehp_multicast_high_packet_counter)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_ehp_multicast_high_packet_counter_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table ehp_multicast_high_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_ehp_multicast_high_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL_DATA* EGQ_ehp_multicast_high_packet_counter_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.ehp_multicast_high_packet_counter_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          (EGQ_ehp_multicast_high_packet_counter_tbl_data->ehp_multicast_high_packet_counter),
          &(tbl->ehp_multicast_high_packet_counter),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_ehp_multicast_high_packet_counter_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table ehp_multicast_low_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_ehp_multicast_low_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL_DATA* EGQ_ehp_multicast_low_packet_counter_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          EGQ_ehp_multicast_low_packet_counter_tbl_data,
          0x0,
          sizeof(SOC_PETRA_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.ehp_multicast_low_packet_counter_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->ehp_multicast_low_packet_counter),
          (EGQ_ehp_multicast_low_packet_counter_tbl_data->ehp_multicast_low_packet_counter)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_ehp_multicast_low_packet_counter_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table ehp_multicast_low_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_ehp_multicast_low_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL_DATA* EGQ_ehp_multicast_low_packet_counter_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.ehp_multicast_low_packet_counter_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          (EGQ_ehp_multicast_low_packet_counter_tbl_data->ehp_multicast_low_packet_counter),
          &(tbl->ehp_multicast_low_packet_counter),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_ehp_multicast_low_packet_counter_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table ehp_discard_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_ehp_discard_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL_DATA* EGQ_ehp_discard_packet_counter_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          EGQ_ehp_discard_packet_counter_tbl_data,
          0x0,
          sizeof(SOC_PETRA_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.ehp_discard_packet_counter_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->ehp_discard_packet_counter),
          (EGQ_ehp_discard_packet_counter_tbl_data->ehp_discard_packet_counter)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_ehp_discard_packet_counter_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table ehp_discard_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_ehp_discard_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL_DATA* EGQ_ehp_discard_packet_counter_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.ehp_discard_packet_counter_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          (EGQ_ehp_discard_packet_counter_tbl_data->ehp_discard_packet_counter),
          &(tbl->ehp_discard_packet_counter),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_ehp_discard_packet_counter_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table pqp_unicast_high_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_pqp_unicast_high_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL_DATA* EGQ_pqp_unicast_high_packet_counter_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          EGQ_pqp_unicast_high_packet_counter_tbl_data,
          0x0,
          sizeof(SOC_PETRA_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.pqp_unicast_high_packet_counter_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->pqp_unicast_high_packet_counter),
          (EGQ_pqp_unicast_high_packet_counter_tbl_data->pqp_unicast_high_packet_counter)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_pqp_unicast_high_packet_counter_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table pqp_unicast_high_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_pqp_unicast_high_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL_DATA* EGQ_pqp_unicast_high_packet_counter_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.pqp_unicast_high_packet_counter_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          (EGQ_pqp_unicast_high_packet_counter_tbl_data->pqp_unicast_high_packet_counter),
          &(tbl->pqp_unicast_high_packet_counter),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_pqp_unicast_high_packet_counter_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table pqp_unicast_low_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_pqp_unicast_low_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL_DATA* EGQ_pqp_unicast_low_packet_counter_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          EGQ_pqp_unicast_low_packet_counter_tbl_data,
          0x0,
          sizeof(SOC_PETRA_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.pqp_unicast_low_packet_counter_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->pqp_unicast_low_packet_counter),
          (EGQ_pqp_unicast_low_packet_counter_tbl_data->pqp_unicast_low_packet_counter)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_pqp_unicast_low_packet_counter_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table pqp_unicast_low_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_pqp_unicast_low_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL_DATA* EGQ_pqp_unicast_low_packet_counter_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.pqp_unicast_low_packet_counter_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          (EGQ_pqp_unicast_low_packet_counter_tbl_data->pqp_unicast_low_packet_counter),
          &(tbl->pqp_unicast_low_packet_counter),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_pqp_unicast_low_packet_counter_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table pqp_multicast_high_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_pqp_multicast_high_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL_DATA* EGQ_pqp_multicast_high_packet_counter_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          EGQ_pqp_multicast_high_packet_counter_tbl_data,
          0x0,
          sizeof(SOC_PETRA_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.pqp_multicast_high_packet_counter_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->pqp_multicast_high_packet_counter),
          (EGQ_pqp_multicast_high_packet_counter_tbl_data->pqp_multicast_high_packet_counter)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_pqp_multicast_high_packet_counter_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table pqp_multicast_high_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_pqp_multicast_high_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL_DATA* EGQ_pqp_multicast_high_packet_counter_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.pqp_multicast_high_packet_counter_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          (EGQ_pqp_multicast_high_packet_counter_tbl_data->pqp_multicast_high_packet_counter),
          &(tbl->pqp_multicast_high_packet_counter),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_pqp_multicast_high_packet_counter_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table pqp_multicast_low_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_pqp_multicast_low_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL_DATA* EGQ_pqp_multicast_low_packet_counter_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          EGQ_pqp_multicast_low_packet_counter_tbl_data,
          0x0,
          sizeof(SOC_PETRA_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.pqp_multicast_low_packet_counter_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->pqp_multicast_low_packet_counter),
          (EGQ_pqp_multicast_low_packet_counter_tbl_data->pqp_multicast_low_packet_counter)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_pqp_multicast_low_packet_counter_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table pqp_multicast_low_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_pqp_multicast_low_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL_DATA* EGQ_pqp_multicast_low_packet_counter_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.pqp_multicast_low_packet_counter_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          (EGQ_pqp_multicast_low_packet_counter_tbl_data->pqp_multicast_low_packet_counter),
          &(tbl->pqp_multicast_low_packet_counter),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_pqp_multicast_low_packet_counter_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table pqp_unicast_high_bytes_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_pqp_unicast_high_bytes_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL_DATA* EGQ_pqp_unicast_high_bytes_counter_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          EGQ_pqp_unicast_high_bytes_counter_tbl_data,
          0x0,
          sizeof(SOC_PETRA_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.pqp_unicast_high_bytes_counter_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->pqp_unicast_high_bytes_counter),
          (EGQ_pqp_unicast_high_bytes_counter_tbl_data->pqp_unicast_high_bytes_counter)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_pqp_unicast_high_bytes_counter_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table pqp_unicast_high_bytes_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_pqp_unicast_high_bytes_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL_DATA* EGQ_pqp_unicast_high_bytes_counter_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.pqp_unicast_high_bytes_counter_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          (EGQ_pqp_unicast_high_bytes_counter_tbl_data->pqp_unicast_high_bytes_counter),
          &(tbl->pqp_unicast_high_bytes_counter),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_pqp_unicast_high_bytes_counter_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table pqp_unicast_low_bytes_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_pqp_unicast_low_bytes_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL_DATA* EGQ_pqp_unicast_low_bytes_counter_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          EGQ_pqp_unicast_low_bytes_counter_tbl_data,
          0x0,
          sizeof(SOC_PETRA_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.pqp_unicast_low_bytes_counter_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->pqp_unicast_low_bytes_counter),
          (EGQ_pqp_unicast_low_bytes_counter_tbl_data->pqp_unicast_low_bytes_counter)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_pqp_unicast_low_bytes_counter_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table pqp_unicast_low_bytes_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_pqp_unicast_low_bytes_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL_DATA* EGQ_pqp_unicast_low_bytes_counter_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.pqp_unicast_low_bytes_counter_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          (EGQ_pqp_unicast_low_bytes_counter_tbl_data->pqp_unicast_low_bytes_counter),
          &(tbl->pqp_unicast_low_bytes_counter),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_pqp_unicast_low_bytes_counter_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table pqp_multicast_high_bytes_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_pqp_multicast_high_bytes_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL_DATA* EGQ_pqp_multicast_high_bytes_counter_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          EGQ_pqp_multicast_high_bytes_counter_tbl_data,
          0x0,
          sizeof(SOC_PETRA_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.pqp_multicast_high_bytes_counter_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->pqp_multicast_high_bytes_counter),
          (EGQ_pqp_multicast_high_bytes_counter_tbl_data->pqp_multicast_high_bytes_counter)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_pqp_multicast_high_bytes_counter_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table pqp_multicast_high_bytes_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_pqp_multicast_high_bytes_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL_DATA* EGQ_pqp_multicast_high_bytes_counter_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.pqp_multicast_high_bytes_counter_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          (EGQ_pqp_multicast_high_bytes_counter_tbl_data->pqp_multicast_high_bytes_counter),
          &(tbl->pqp_multicast_high_bytes_counter),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_pqp_multicast_high_bytes_counter_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table pqp_multicast_low_bytes_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_pqp_multicast_low_bytes_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL_DATA* EGQ_pqp_multicast_low_bytes_counter_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          EGQ_pqp_multicast_low_bytes_counter_tbl_data,
          0x0,
          sizeof(SOC_PETRA_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.pqp_multicast_low_bytes_counter_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->pqp_multicast_low_bytes_counter),
          (EGQ_pqp_multicast_low_bytes_counter_tbl_data->pqp_multicast_low_bytes_counter)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_pqp_multicast_low_bytes_counter_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table pqp_multicast_low_bytes_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_pqp_multicast_low_bytes_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL_DATA* EGQ_pqp_multicast_low_bytes_counter_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.pqp_multicast_low_bytes_counter_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          (EGQ_pqp_multicast_low_bytes_counter_tbl_data->pqp_multicast_low_bytes_counter),
          &(tbl->pqp_multicast_low_bytes_counter),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_pqp_multicast_low_bytes_counter_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table pqp_discard_unicast_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_pqp_discard_unicast_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL_DATA* EGQ_pqp_discard_unicast_packet_counter_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          EGQ_pqp_discard_unicast_packet_counter_tbl_data,
          0x0,
          sizeof(SOC_PETRA_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.pqp_discard_unicast_packet_counter_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->pqp_discard_unicast_packet_counter),
          (EGQ_pqp_discard_unicast_packet_counter_tbl_data->pqp_discard_unicast_packet_counter)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_pqp_discard_unicast_packet_counter_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table pqp_discard_unicast_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_pqp_discard_unicast_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL_DATA* EGQ_pqp_discard_unicast_packet_counter_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.pqp_discard_unicast_packet_counter_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          (EGQ_pqp_discard_unicast_packet_counter_tbl_data->pqp_discard_unicast_packet_counter),
          &(tbl->pqp_discard_unicast_packet_counter),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_pqp_discard_unicast_packet_counter_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table pqp_discard_multicast_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_pqp_discard_multicast_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL_DATA* EGQ_pqp_discard_multicast_packet_counter_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          EGQ_pqp_discard_multicast_packet_counter_tbl_data,
          0x0,
          sizeof(SOC_PETRA_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.pqp_discard_multicast_packet_counter_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->pqp_discard_multicast_packet_counter),
          (EGQ_pqp_discard_multicast_packet_counter_tbl_data->pqp_discard_multicast_packet_counter)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_pqp_discard_multicast_packet_counter_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table pqp_discard_multicast_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_pqp_discard_multicast_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL_DATA* EGQ_pqp_discard_multicast_packet_counter_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.pqp_discard_multicast_packet_counter_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          (EGQ_pqp_discard_multicast_packet_counter_tbl_data->pqp_discard_multicast_packet_counter),
          &(tbl->pqp_discard_multicast_packet_counter),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_pqp_discard_multicast_packet_counter_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table fqp_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_fqp_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_FQP_PACKET_COUNTER_TBL_DATA* EGQ_fqp_packet_counter_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_FQP_PACKET_COUNTER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_FQP_PACKET_COUNTER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_FQP_PACKET_COUNTER_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          EGQ_fqp_packet_counter_tbl_data,
          0x0,
          sizeof(SOC_PETRA_EGQ_FQP_PACKET_COUNTER_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.fqp_packet_counter_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_FQP_PACKET_COUNTER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->fqp_packet_counter),
          (EGQ_fqp_packet_counter_tbl_data->fqp_packet_counter)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_fqp_packet_counter_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table fqp_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_fqp_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_FQP_PACKET_COUNTER_TBL_DATA* EGQ_fqp_packet_counter_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_EGQ_FQP_PACKET_COUNTER_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_EGQ_FQP_PACKET_COUNTER_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGQ_FQP_PACKET_COUNTER_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->egq.fqp_packet_counter_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          (EGQ_fqp_packet_counter_tbl_data->fqp_packet_counter),
          &(tbl->fqp_packet_counter),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_EGQ_ID,
          SOC_PETRA_EGQ_FQP_PACKET_COUNTER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_fqp_packet_counter_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table recycle_to_out_going_fap_port_mapping_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_cfc_recycle_to_out_going_fap_port_mapping_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_CFC_RECYCLE_TO_OUT_GOING_FAP_PORT_MAPPING_TBL_DATA* CFC_recycle_to_out_going_fap_port_mapping_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_CFC_RECYCLE_TO_OUT_GOING_FAP_PORT_MAPPING_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_CFC_RECYCLE_TO_OUT_GOING_FAP_PORT_MAPPING_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_CFC_RECYCLE_TO_OUT_GOING_FAP_PORT_MAPPING_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          CFC_recycle_to_out_going_fap_port_mapping_tbl_data,
          0x0,
          sizeof(SOC_PETRA_CFC_RECYCLE_TO_OUT_GOING_FAP_PORT_MAPPING_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->cfc.recycle_to_out_going_fap_port_mapping_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_CFC_ID,
          SOC_PETRA_CFC_RECYCLE_TO_OUT_GOING_FAP_PORT_MAPPING_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->egq_ofp_num),
          &(CFC_recycle_to_out_going_fap_port_mapping_tbl_data->egq_ofp_num)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_cfc_recycle_to_out_going_fap_port_mapping_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table recycle_to_out_going_fap_port_mapping_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_cfc_recycle_to_out_going_fap_port_mapping_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_CFC_RECYCLE_TO_OUT_GOING_FAP_PORT_MAPPING_TBL_DATA* CFC_recycle_to_out_going_fap_port_mapping_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_CFC_RECYCLE_TO_OUT_GOING_FAP_PORT_MAPPING_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_CFC_RECYCLE_TO_OUT_GOING_FAP_PORT_MAPPING_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_CFC_RECYCLE_TO_OUT_GOING_FAP_PORT_MAPPING_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->cfc.recycle_to_out_going_fap_port_mapping_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(CFC_recycle_to_out_going_fap_port_mapping_tbl_data->egq_ofp_num),
          &(tbl->egq_ofp_num),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_CFC_ID,
          SOC_PETRA_CFC_RECYCLE_TO_OUT_GOING_FAP_PORT_MAPPING_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_cfc_recycle_to_out_going_fap_port_mapping_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table nif_a_class_based_to_ofp_mapping_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_cfc_nif_a_class_based_to_ofp_mapping_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_CFC_NIF_CLASS_BASED_TO_OFP_MAPPING_TBL_DATA* CFC_nif_a_class_based_to_ofp_mapping_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_CFC_NIF_A_CLASS_BASED_TO_OFP_MAPPING_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_CFC_NIF_A_CLASS_BASED_TO_OFP_MAPPING_TBL
    *tbl;
  uint32
    fld_idx = 0;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_CFC_NIF_A_CLASS_BASED_TO_OFP_MAPPING_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);


  err = SOC_SAND_OK; sal_memset(
          CFC_nif_a_class_based_to_ofp_mapping_tbl_data,
          0x0,
          sizeof(SOC_PETRA_CFC_NIF_CLASS_BASED_TO_OFP_MAPPING_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->cfc.nif_a_class_based_to_ofp_mapping_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_CFC_ID,
          SOC_PETRA_CFC_NIF_A_CLASS_BASED_TO_OFP_MAPPING_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  for (fld_idx = 0; fld_idx < SOC_PETRA_NOF_CFC_NIF_CLS2OFP_MAP_FLDS; fld_idx++)
  {
    err = soc_petra_field_in_place_get(
          data,
          &(tbl->egq_ofp_num[fld_idx]),
          &(CFC_nif_a_class_based_to_ofp_mapping_tbl_data->egq_ofp_num[fld_idx])
        );
   SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_cfc_nif_a_class_based_to_ofp_mapping_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table nif_a_class_based_to_ofp_mapping_tbl from block CFC,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_cfc_nif_a_class_based_to_ofp_mapping_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_CFC_NIF_CLASS_BASED_TO_OFP_MAPPING_TBL_DATA* CFC_nif_a_class_based_to_ofp_mapping_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_CFC_NIF_A_CLASS_BASED_TO_OFP_MAPPING_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_CFC_NIF_A_CLASS_BASED_TO_OFP_MAPPING_TBL
    *tbl;
  uint32
    fld_idx = 0;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_CFC_NIF_A_CLASS_BASED_TO_OFP_MAPPING_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->cfc.nif_a_class_based_to_ofp_mapping_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  for (fld_idx = 0; fld_idx < SOC_PETRA_NOF_CFC_NIF_CLS2OFP_MAP_FLDS; fld_idx++)
  {
    err = soc_petra_field_in_place_set(
            &(CFC_nif_a_class_based_to_ofp_mapping_tbl_data->egq_ofp_num[fld_idx]),
            &(tbl->egq_ofp_num[fld_idx]),
            data
          );
    SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);
  }

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_CFC_ID,
          SOC_PETRA_CFC_NIF_A_CLASS_BASED_TO_OFP_MAPPING_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_cfc_nif_a_class_based_to_ofp_mapping_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table nif_b_class_based_to_ofp_mapping_tbl from block CFC,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_cfc_nif_b_class_based_to_ofp_mapping_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_CFC_NIF_CLASS_BASED_TO_OFP_MAPPING_TBL_DATA* CFC_nif_b_class_based_to_ofp_mapping_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_CFC_NIF_B_CLASS_BASED_TO_OFP_MAPPING_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_CFC_NIF_B_CLASS_BASED_TO_OFP_MAPPING_TBL
    *tbl;
  uint32
    fld_idx = 0;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_CFC_NIF_B_CLASS_BASED_TO_OFP_MAPPING_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);


  err = SOC_SAND_OK; sal_memset(
          CFC_nif_b_class_based_to_ofp_mapping_tbl_data,
          0x0,
          sizeof(SOC_PETRA_CFC_NIF_CLASS_BASED_TO_OFP_MAPPING_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->cfc.nif_b_class_based_to_ofp_mapping_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_CFC_ID,
          SOC_PETRA_CFC_NIF_B_CLASS_BASED_TO_OFP_MAPPING_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  for (fld_idx = 0; fld_idx < SOC_PETRA_NOF_CFC_NIF_CLS2OFP_MAP_FLDS; fld_idx++)
  {
    err = soc_petra_field_in_place_get(
            data,
            &(tbl->egq_ofp_num[fld_idx]),
            &(CFC_nif_b_class_based_to_ofp_mapping_tbl_data->egq_ofp_num[fld_idx])
          );
    SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_cfc_nif_b_class_based_to_ofp_mapping_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table nif_b_class_based_to_ofp_mapping_tbl from block CFC,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_cfc_nif_b_class_based_to_ofp_mapping_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_CFC_NIF_CLASS_BASED_TO_OFP_MAPPING_TBL_DATA* CFC_nif_b_class_based_to_ofp_mapping_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_CFC_NIF_B_CLASS_BASED_TO_OFP_MAPPING_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_CFC_NIF_B_CLASS_BASED_TO_OFP_MAPPING_TBL
    *tbl;
  uint32
    fld_idx = 0;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_CFC_NIF_B_CLASS_BASED_TO_OFP_MAPPING_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->cfc.nif_b_class_based_to_ofp_mapping_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;


  for (fld_idx = 0; fld_idx < SOC_PETRA_NOF_CFC_NIF_CLS2OFP_MAP_FLDS; fld_idx++)
  {
    err = soc_petra_field_in_place_set(
            &(CFC_nif_b_class_based_to_ofp_mapping_tbl_data->egq_ofp_num[fld_idx]),
            &(tbl->egq_ofp_num[fld_idx]),
            data
          );
    SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);
  }

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_CFC_ID,
          SOC_PETRA_CFC_NIF_B_CLASS_BASED_TO_OFP_MAPPING_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_cfc_nif_b_class_based_to_ofp_mapping_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table a_schedulers_based_flow_control_to_ofp_mapping_tbl from block CFC,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_cfc_a_schedulers_based_flow_control_to_ofp_mapping_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_CFC_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_DATA* CFC_a_schedulers_based_flow_control_to_ofp_mapping_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_CFC_A_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_CFC_A_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_CFC_A_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          CFC_a_schedulers_based_flow_control_to_ofp_mapping_tbl_data,
          0x0,
          sizeof(SOC_PETRA_CFC_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->cfc.a_schedulers_based_flow_control_to_ofp_mapping_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_CFC_ID,
          SOC_PETRA_CFC_A_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->ofp_hr),
          &(CFC_a_schedulers_based_flow_control_to_ofp_mapping_tbl_data->ofp_hr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->lp_ofp_valid),
          &(CFC_a_schedulers_based_flow_control_to_ofp_mapping_tbl_data->lp_ofp_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->hp_ofp_valid),
          &(CFC_a_schedulers_based_flow_control_to_ofp_mapping_tbl_data->hp_ofp_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 42, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_cfc_a_schedulers_based_flow_control_to_ofp_mapping_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table a_schedulers_based_flow_control_to_ofp_mapping_tbl from block CFC,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_cfc_a_schedulers_based_flow_control_to_ofp_mapping_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_CFC_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_DATA* CFC_a_schedulers_based_flow_control_to_ofp_mapping_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_CFC_A_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_CFC_A_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_CFC_A_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->cfc.a_schedulers_based_flow_control_to_ofp_mapping_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(CFC_a_schedulers_based_flow_control_to_ofp_mapping_tbl_data->ofp_hr),
          &(tbl->ofp_hr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(CFC_a_schedulers_based_flow_control_to_ofp_mapping_tbl_data->lp_ofp_valid),
          &(tbl->lp_ofp_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_petra_field_in_place_set(
          &(CFC_a_schedulers_based_flow_control_to_ofp_mapping_tbl_data->hp_ofp_valid),
          &(tbl->hp_ofp_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_CFC_ID,
          SOC_PETRA_CFC_A_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 33, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_cfc_a_schedulers_based_flow_control_to_ofp_mapping_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table b_schedulers_based_flow_control_to_ofp_mapping_tbl from block CFC,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_cfc_b_schedulers_based_flow_control_to_ofp_mapping_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_CFC_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_DATA* CFC_b_schedulers_based_flow_control_to_ofp_mapping_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_CFC_B_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_CFC_B_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_CFC_B_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          CFC_b_schedulers_based_flow_control_to_ofp_mapping_tbl_data,
          0x0,
          sizeof(SOC_PETRA_CFC_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->cfc.b_schedulers_based_flow_control_to_ofp_mapping_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_CFC_ID,
          SOC_PETRA_CFC_B_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->ofp_hr),
          &(CFC_b_schedulers_based_flow_control_to_ofp_mapping_tbl_data->ofp_hr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->lp_ofp_valid),
          &(CFC_b_schedulers_based_flow_control_to_ofp_mapping_tbl_data->lp_ofp_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->hp_ofp_valid),
          &(CFC_b_schedulers_based_flow_control_to_ofp_mapping_tbl_data->hp_ofp_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 42, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_cfc_b_schedulers_based_flow_control_to_ofp_mapping_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table b_schedulers_based_flow_control_to_ofp_mapping_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_cfc_b_schedulers_based_flow_control_to_ofp_mapping_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_CFC_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_DATA* CFC_b_schedulers_based_flow_control_to_ofp_mapping_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_CFC_B_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_CFC_B_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_CFC_B_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->cfc.b_schedulers_based_flow_control_to_ofp_mapping_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(CFC_b_schedulers_based_flow_control_to_ofp_mapping_tbl_data->ofp_hr),
          &(tbl->ofp_hr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(CFC_b_schedulers_based_flow_control_to_ofp_mapping_tbl_data->lp_ofp_valid),
          &(tbl->lp_ofp_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_petra_field_in_place_set(
          &(CFC_b_schedulers_based_flow_control_to_ofp_mapping_tbl_data->hp_ofp_valid),
          &(tbl->hp_ofp_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_CFC_ID,
          SOC_PETRA_CFC_B_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 33, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_cfc_b_schedulers_based_flow_control_to_ofp_mapping_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table out_of_band_rx_a_calendar_mapping_tbl from block CFC,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_cfc_out_of_band_rx_a_calendar_mapping_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_CFC_OUT_OF_BAND_RX_CALENDAR_MAPPING_TBL_DATA* CFC_out_of_band_rx_a_calendar_mapping_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_CFC_OUT_OF_BAND_RX_A_CALENDAR_MAPPING_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_CFC_OUT_OF_BAND_RX_A_CALENDAR_MAPPING_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_CFC_OUT_OF_BAND_RX_A_CALENDAR_MAPPING_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          CFC_out_of_band_rx_a_calendar_mapping_tbl_data,
          0x0,
          sizeof(SOC_PETRA_CFC_OUT_OF_BAND_RX_CALENDAR_MAPPING_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->cfc.out_of_band_rx_a_calendar_mapping_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_CFC_ID,
          SOC_PETRA_CFC_OUT_OF_BAND_RX_A_CALENDAR_MAPPING_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->fc_index),
          &(CFC_out_of_band_rx_a_calendar_mapping_tbl_data->fc_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->fc_dest_sel),
          &(CFC_out_of_band_rx_a_calendar_mapping_tbl_data->fc_dest_sel)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_cfc_out_of_band_rx_a_calendar_mapping_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table out_of_band_rx_a_calendar_mapping_tbl from block CFC,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_cfc_out_of_band_rx_a_calendar_mapping_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_CFC_OUT_OF_BAND_RX_CALENDAR_MAPPING_TBL_DATA* CFC_out_of_band_rx_a_calendar_mapping_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_CFC_OUT_OF_BAND_RX_A_CALENDAR_MAPPING_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_CFC_OUT_OF_BAND_RX_A_CALENDAR_MAPPING_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_CFC_OUT_OF_BAND_RX_A_CALENDAR_MAPPING_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->cfc.out_of_band_rx_a_calendar_mapping_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(CFC_out_of_band_rx_a_calendar_mapping_tbl_data->fc_index),
          &(tbl->fc_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(CFC_out_of_band_rx_a_calendar_mapping_tbl_data->fc_dest_sel),
          &(tbl->fc_dest_sel),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_CFC_ID,
          SOC_PETRA_CFC_OUT_OF_BAND_RX_A_CALENDAR_MAPPING_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_cfc_out_of_band_rx_a_calendar_mapping_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table out_of_band_rx_b_calendar_mapping_tbl from block CFC,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_cfc_out_of_band_rx_b_calendar_mapping_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_CFC_OUT_OF_BAND_RX_CALENDAR_MAPPING_TBL_DATA* CFC_out_of_band_rx_b_calendar_mapping_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_CFC_OUT_OF_BAND_RX_B_CALENDAR_MAPPING_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_CFC_OUT_OF_BAND_RX_B_CALENDAR_MAPPING_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_CFC_OUT_OF_BAND_RX_B_CALENDAR_MAPPING_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          CFC_out_of_band_rx_b_calendar_mapping_tbl_data,
          0x0,
          sizeof(SOC_PETRA_CFC_OUT_OF_BAND_RX_CALENDAR_MAPPING_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->cfc.out_of_band_rx_b_calendar_mapping_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_CFC_ID,
          SOC_PETRA_CFC_OUT_OF_BAND_RX_B_CALENDAR_MAPPING_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->fc_index),
          &(CFC_out_of_band_rx_b_calendar_mapping_tbl_data->fc_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->fc_dest_sel),
          &(CFC_out_of_band_rx_b_calendar_mapping_tbl_data->fc_dest_sel)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_cfc_out_of_band_rx_b_calendar_mapping_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table out_of_band_rx_b_calendar_mapping_tbl from block CFC,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_cfc_out_of_band_rx_b_calendar_mapping_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_CFC_OUT_OF_BAND_RX_CALENDAR_MAPPING_TBL_DATA* CFC_out_of_band_rx_b_calendar_mapping_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_CFC_OUT_OF_BAND_RX_B_CALENDAR_MAPPING_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_CFC_OUT_OF_BAND_RX_B_CALENDAR_MAPPING_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_CFC_OUT_OF_BAND_RX_B_CALENDAR_MAPPING_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->cfc.out_of_band_rx_b_calendar_mapping_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(CFC_out_of_band_rx_b_calendar_mapping_tbl_data->fc_index),
          &(tbl->fc_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(CFC_out_of_band_rx_b_calendar_mapping_tbl_data->fc_dest_sel),
          &(tbl->fc_dest_sel),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_CFC_ID,
          SOC_PETRA_CFC_OUT_OF_BAND_RX_B_CALENDAR_MAPPING_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_cfc_out_of_band_rx_b_calendar_mapping_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table out_of_band_tx_calendar_mapping_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_cfc_out_of_band_tx_calendar_mapping_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_CFC_OUT_OF_BAND_TX_CALENDAR_MAPPING_TBL_DATA* CFC_out_of_band_tx_calendar_mapping_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_CFC_OUT_OF_BAND_TX_CALENDAR_MAPPING_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_CFC_OUT_OF_BAND_TX_CALENDAR_MAPPING_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_CFC_OUT_OF_BAND_TX_CALENDAR_MAPPING_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          CFC_out_of_band_tx_calendar_mapping_tbl_data,
          0x0,
          sizeof(SOC_PETRA_CFC_OUT_OF_BAND_TX_CALENDAR_MAPPING_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->cfc.out_of_band_tx_calendar_mapping_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_CFC_ID,
          SOC_PETRA_CFC_OUT_OF_BAND_TX_CALENDAR_MAPPING_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->fc_index),
          &(CFC_out_of_band_tx_calendar_mapping_tbl_data->fc_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->fc_source_sel),
          &(CFC_out_of_band_tx_calendar_mapping_tbl_data->fc_source_sel)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_cfc_out_of_band_tx_calendar_mapping_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table out_of_band_tx_calendar_mapping_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_cfc_out_of_band_tx_calendar_mapping_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_CFC_OUT_OF_BAND_TX_CALENDAR_MAPPING_TBL_DATA* CFC_out_of_band_tx_calendar_mapping_tbl_data
  )
{
  uint32
    err, res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PETRA_CFC_OUT_OF_BAND_TX_CALENDAR_MAPPING_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_CFC_OUT_OF_BAND_TX_CALENDAR_MAPPING_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_CFC_OUT_OF_BAND_TX_CALENDAR_MAPPING_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(111, exit);
  tbl = SOC_PA_TBL_REF(tables->cfc.out_of_band_tx_calendar_mapping_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(CFC_out_of_band_tx_calendar_mapping_tbl_data->fc_index),
          &(tbl->fc_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(CFC_out_of_band_tx_calendar_mapping_tbl_data->fc_source_sel),
          &(tbl->fc_source_sel),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_CFC_ID,
          SOC_PETRA_CFC_OUT_OF_BAND_TX_CALENDAR_MAPPING_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_cfc_out_of_band_tx_calendar_mapping_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table cal_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_sch_cal_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_SCH_CAL_TBL_DATA* SCH_cal_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_SCH_CAL_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_SCH_CAL_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_CAL_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          SCH_cal_tbl_data,
          0x0,
          sizeof(SOC_PETRA_SCH_CAL_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->sch.cal_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_SCH_ID,
          SOC_PETRA_SCH_CAL_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->hrsel),
          &(SCH_cal_tbl_data->hrsel)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_sch_cal_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table cal_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_sch_cal_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_SCH_CAL_TBL_DATA* SCH_cal_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_SCH_CAL_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_SCH_CAL_TBL
    *tbl;
  uint32
    ind_options;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_CAL_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->sch.cal_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(SCH_cal_tbl_data->hrsel),
          &(tbl->hrsel),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  ind_options = SOC_PETRA_TBL_WRITE_IND_OPT_SET(SOC_PETRA_SCH_CAL_TBL_ENTRY_SIZE, FALSE);
  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_SCH_ID,
          ind_options
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);
  SOC_PETRA_TBL_REPS_RESET;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_sch_cal_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table drm_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_sch_drm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_SCH_DRM_TBL_DATA* SCH_drm_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_SCH_DRM_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_SCH_DRM_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_DRM_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          SCH_drm_tbl_data,
          0x0,
          sizeof(SOC_PETRA_SCH_DRM_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->sch.drm_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_SCH_ID,
          SOC_PETRA_SCH_DRM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->device_rate),
          &(SCH_drm_tbl_data->device_rate)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_sch_drm_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table drm_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_sch_drm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_SCH_DRM_TBL_DATA* SCH_drm_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_SCH_DRM_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_SCH_DRM_TBL
    *tbl;
  uint32
    ind_options;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_DRM_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->sch.drm_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(SCH_drm_tbl_data->device_rate),
          &(tbl->device_rate),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  ind_options = SOC_PETRA_TBL_WRITE_IND_OPT_SET(SOC_PETRA_SCH_DRM_TBL_ENTRY_SIZE, FALSE);
  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_SCH_ID,
          ind_options
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);
  SOC_PETRA_TBL_REPS_RESET;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_sch_drm_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table dsm_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_sch_dsm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_SCH_DSM_TBL_DATA* SCH_dsm_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_SCH_DSM_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_SCH_DSM_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_DSM_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          SCH_dsm_tbl_data,
          0x0,
          sizeof(SOC_PETRA_SCH_DSM_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->sch.dsm_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_SCH_ID,
          SOC_PETRA_SCH_DSM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->dual_shaper_ena),
          &(SCH_dsm_tbl_data->dual_shaper_ena)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_sch_dsm_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table dsm_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_sch_dsm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_SCH_DSM_TBL_DATA* SCH_dsm_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_SCH_DSM_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_SCH_DSM_TBL
    *tbl;
  uint32
    ind_options;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_DSM_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->sch.dsm_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(SCH_dsm_tbl_data->dual_shaper_ena),
          &(tbl->dual_shaper_ena),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  ind_options = SOC_PETRA_TBL_WRITE_IND_OPT_SET(SOC_PETRA_SCH_DSM_TBL_ENTRY_SIZE, FALSE);
  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_SCH_ID,
          ind_options
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);
  SOC_PETRA_TBL_REPS_RESET;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_sch_dsm_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table fdms_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_sch_fdms_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_SCH_FDMS_TBL_DATA* SCH_fdms_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_SCH_FDMS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_SCH_FDMS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_FDMS_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          SCH_fdms_tbl_data,
          0x0,
          sizeof(SOC_PETRA_SCH_FDMS_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->sch.fdms_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_SCH_ID,
          SOC_PETRA_SCH_FDMS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->sch_number),
          &(SCH_fdms_tbl_data->sch_number)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->cos),
          &(SCH_fdms_tbl_data->cos)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->hrsel_dual),
          &(SCH_fdms_tbl_data->hrsel_dual)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 42, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_sch_fdms_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table fdms_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_sch_fdms_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_SCH_FDMS_TBL_DATA* SCH_fdms_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_SCH_FDMS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_SCH_FDMS_TBL
    *tbl;
  uint32
    ind_options;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_FDMS_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->sch.fdms_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(SCH_fdms_tbl_data->sch_number),
          &(tbl->sch_number),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(SCH_fdms_tbl_data->cos),
          &(tbl->cos),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_petra_field_in_place_set(
          &(SCH_fdms_tbl_data->hrsel_dual),
          &(tbl->hrsel_dual),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

  ind_options = SOC_PETRA_TBL_WRITE_IND_OPT_SET(SOC_PETRA_SCH_FDMS_TBL_ENTRY_SIZE, FALSE);
  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_SCH_ID,
          ind_options
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 33, exit);
  SOC_PETRA_TBL_REPS_RESET;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_sch_fdms_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table shds_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_sch_shds_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_SCH_SHDS_TBL_DATA* SCH_shds_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_SCH_SHDS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_SCH_SHDS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_SHDS_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          SCH_shds_tbl_data,
          0x0,
          sizeof(SOC_PETRA_SCH_SHDS_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->sch.shds_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_SCH_ID,
          SOC_PETRA_SCH_SHDS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->peak_rate_man_even),
          &(SCH_shds_tbl_data->peak_rate_man_even)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->peak_rate_exp_even),
          &(SCH_shds_tbl_data->peak_rate_exp_even)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->max_burst_even),
          &(SCH_shds_tbl_data->max_burst_even)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 42, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->slow_rate2_sel_even),
          &(SCH_shds_tbl_data->slow_rate2_sel_even)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 43, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->peak_rate_man_odd),
          &(SCH_shds_tbl_data->peak_rate_man_odd)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 44, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->peak_rate_exp_odd),
          &(SCH_shds_tbl_data->peak_rate_exp_odd)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 45, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->max_burst_odd),
          &(SCH_shds_tbl_data->max_burst_odd)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 46, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->slow_rate2_sel_odd),
          &(SCH_shds_tbl_data->slow_rate2_sel_odd)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 47, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->max_burst_update_even),
          &(SCH_shds_tbl_data->max_burst_update_even)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 48, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->max_burst_update_odd),
          &(SCH_shds_tbl_data->max_burst_update_odd)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 49, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_sch_shds_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table shds_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_sch_shds_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_SCH_SHDS_TBL_DATA* SCH_shds_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_SCH_SHDS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_SCH_SHDS_TBL
    *tbl;
  uint32
    ind_options;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_SHDS_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->sch.shds_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(SCH_shds_tbl_data->peak_rate_man_even),
          &(tbl->peak_rate_man_even),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(SCH_shds_tbl_data->peak_rate_exp_even),
          &(tbl->peak_rate_exp_even),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_petra_field_in_place_set(
          &(SCH_shds_tbl_data->max_burst_even),
          &(tbl->max_burst_even),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

  err = soc_petra_field_in_place_set(
          &(SCH_shds_tbl_data->slow_rate2_sel_even),
          &(tbl->slow_rate2_sel_even),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 33, exit);

  err = soc_petra_field_in_place_set(
          &(SCH_shds_tbl_data->peak_rate_man_odd),
          &(tbl->peak_rate_man_odd),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 34, exit);

  err = soc_petra_field_in_place_set(
          &(SCH_shds_tbl_data->peak_rate_exp_odd),
          &(tbl->peak_rate_exp_odd),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 35, exit);

  err = soc_petra_field_in_place_set(
          &(SCH_shds_tbl_data->max_burst_odd),
          &(tbl->max_burst_odd),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 36, exit);

  err = soc_petra_field_in_place_set(
          &(SCH_shds_tbl_data->slow_rate2_sel_odd),
          &(tbl->slow_rate2_sel_odd),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 37, exit);

  err = soc_petra_field_in_place_set(
          &(SCH_shds_tbl_data->max_burst_update_even),
          &(tbl->max_burst_update_even),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 38, exit);

  err = soc_petra_field_in_place_set(
          &(SCH_shds_tbl_data->max_burst_update_odd),
          &(tbl->max_burst_update_odd),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 39, exit);

  ind_options = SOC_PETRA_TBL_WRITE_IND_OPT_SET(SOC_PETRA_SCH_SHDS_TBL_ENTRY_SIZE, FALSE);
  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_SCH_ID,
          ind_options
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);
  SOC_PETRA_TBL_REPS_RESET;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_sch_shds_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table sem_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_sch_sem_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_SCH_SEM_TBL_DATA* SCH_sem_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_SCH_SEM_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_SCH_SEM_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_SEM_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          SCH_sem_tbl_data,
          0x0,
          sizeof(SOC_PETRA_SCH_SEM_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->sch.sem_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_SCH_ID,
          SOC_PETRA_SCH_SEM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->sch_enable),
          &(SCH_sem_tbl_data->sch_enable)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_sch_sem_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table sem_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_sch_sem_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_SCH_SEM_TBL_DATA* SCH_sem_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_SCH_SEM_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_SCH_SEM_TBL
    *tbl;
  uint32
    ind_options;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_SEM_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->sch.sem_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(SCH_sem_tbl_data->sch_enable),
          &(tbl->sch_enable),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  ind_options = SOC_PETRA_TBL_WRITE_IND_OPT_SET(SOC_PETRA_SCH_SEM_TBL_ENTRY_SIZE, FALSE);
  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_SCH_ID,
          ind_options
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);
  SOC_PETRA_TBL_REPS_RESET;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_sch_sem_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table fsf_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_sch_fsf_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_SCH_FSF_TBL_DATA* SCH_fsf_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_SCH_FSF_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_SCH_FSF_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_FSF_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          SCH_fsf_tbl_data,
          0x0,
          sizeof(SOC_PETRA_SCH_FSF_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->sch.fsf_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_SCH_ID,
          SOC_PETRA_SCH_FSF_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->sfenable),
          &(SCH_fsf_tbl_data->sfenable)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_sch_fsf_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table fsf_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_sch_fsf_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_SCH_FSF_TBL_DATA* SCH_fsf_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_SCH_FSF_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_SCH_FSF_TBL
    *tbl;
  uint32
    ind_options;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_FSF_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->sch.fsf_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(SCH_fsf_tbl_data->sfenable),
          &(tbl->sfenable),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  ind_options = SOC_PETRA_TBL_WRITE_IND_OPT_SET(SOC_PETRA_SCH_FSF_TBL_ENTRY_SIZE, FALSE);
  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_SCH_ID,
          ind_options
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);
  SOC_PETRA_TBL_REPS_RESET;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_sch_fsf_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table fgm_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_sch_fgm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_SCH_FGM_TBL_DATA* SCH_fgm_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_SCH_FGM_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_SCH_FGM_TBL
    *tbl;
  uint32
    fld_idx = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_FGM_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          SCH_fgm_tbl_data,
          0x0,
          sizeof(SOC_PETRA_SCH_FGM_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->sch.fgm_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_SCH_ID,
          SOC_PETRA_SCH_FGM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  for (fld_idx = 0; fld_idx < SOC_PETRA_TBL_FGM_NOF_GROUPS_ONE_LINE; fld_idx++)
  {
    err = soc_petra_field_in_place_get(
          data,
          &(tbl->flow_group[fld_idx]),
          &(SCH_fgm_tbl_data->flow_group[fld_idx])
        );
    SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_sch_fgm_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table fgm_tbl from block SCH,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_sch_fgm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_SCH_FGM_TBL_DATA* SCH_fgm_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_SCH_FGM_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_SCH_FGM_TBL
    *tbl;
  uint32
    ind_options;
  uint32
    fld_idx = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_FGM_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->sch.fgm_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  for (fld_idx = 0; fld_idx < SOC_PETRA_TBL_FGM_NOF_GROUPS_ONE_LINE; fld_idx++)
  {
    err = soc_petra_field_in_place_set(
            &(SCH_fgm_tbl_data->flow_group[fld_idx]),
            &(tbl->flow_group[fld_idx]),
            data
          );
    SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);
  }

  ind_options = SOC_PETRA_TBL_WRITE_IND_OPT_SET(SOC_PETRA_SCH_FGM_TBL_ENTRY_SIZE, FALSE);
  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_SCH_ID,
          ind_options
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 33, exit);
  SOC_PETRA_TBL_REPS_RESET;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_sch_fgm_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table shc_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_sch_shc_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_SCH_SHC_TBL_DATA* SCH_shc_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_SCH_SHC_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_SCH_SHC_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_SHC_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          SCH_shc_tbl_data,
          0x0,
          sizeof(SOC_PETRA_SCH_SHC_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->sch.shc_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_SCH_ID,
          SOC_PETRA_SCH_SHC_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->hrmode),
          &(SCH_shc_tbl_data->hrmode)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->hrmask_type),
          &(SCH_shc_tbl_data->hrmask_type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_sch_shc_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table shc_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_sch_shc_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_SCH_SHC_TBL_DATA* SCH_shc_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_SCH_SHC_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_SCH_SHC_TBL
    *tbl;
  uint32
    ind_options;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_SHC_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->sch.shc_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(SCH_shc_tbl_data->hrmode),
          &(tbl->hrmode),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_petra_field_in_place_set(
          &(SCH_shc_tbl_data->hrmask_type),
          &(tbl->hrmask_type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

  ind_options = SOC_PETRA_TBL_WRITE_IND_OPT_SET(SOC_PETRA_SCH_SHC_TBL_ENTRY_SIZE, FALSE);
  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_SCH_ID,
          ind_options
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 33, exit);
  SOC_PETRA_TBL_REPS_RESET;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_sch_shc_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table scc_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_sch_scc_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_SCH_SCC_TBL_DATA* SCH_scc_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_SCH_SCC_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_SCH_SCC_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_SCC_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          SCH_scc_tbl_data,
          0x0,
          sizeof(SOC_PETRA_SCH_SCC_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->sch.scc_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_SCH_ID,
          SOC_PETRA_SCH_SCC_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->clsch_type),
          &(SCH_scc_tbl_data->clsch_type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_sch_scc_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table scc_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_sch_scc_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_SCH_SCC_TBL_DATA* SCH_scc_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_SCH_SCC_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_SCH_SCC_TBL
    *tbl;
  uint32
    ind_options;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_SCC_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->sch.scc_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(SCH_scc_tbl_data->clsch_type),
          &(tbl->clsch_type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  ind_options = SOC_PETRA_TBL_WRITE_IND_OPT_SET(SOC_PETRA_SCH_SCC_TBL_ENTRY_SIZE, FALSE);
  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_SCH_ID,
          ind_options
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);
  SOC_PETRA_TBL_REPS_RESET;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_sch_scc_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table sct_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_sch_sct_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_SCH_SCT_TBL_DATA* SCH_sct_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_SCH_SCT_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_SCH_SCT_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_SCT_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          SCH_sct_tbl_data,
          0x0,
          sizeof(SOC_PETRA_SCH_SCT_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->sch.sct_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_SCH_ID,
          SOC_PETRA_SCH_SCT_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->clconfig),
          &(SCH_sct_tbl_data->clconfig)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->af0_inv_weight),
          &(SCH_sct_tbl_data->af0_inv_weight)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->af1_inv_weight),
          &(SCH_sct_tbl_data->af1_inv_weight)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 42, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->af2_inv_weight),
          &(SCH_sct_tbl_data->af2_inv_weight)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 43, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->af3_inv_weight),
          &(SCH_sct_tbl_data->af3_inv_weight)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 44, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->wfqmode),
          &(SCH_sct_tbl_data->wfqmode)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 45, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->enh_clen),
          &(SCH_sct_tbl_data->enh_clen)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 46, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->enh_clsphigh),
          &(SCH_sct_tbl_data->enh_clsphigh)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 47, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_sch_sct_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table sct_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_sch_sct_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_SCH_SCT_TBL_DATA* SCH_sct_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_SCH_SCT_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_SCH_SCT_TBL
    *tbl;
  uint32
    ind_options;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_SCT_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->sch.sct_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(SCH_sct_tbl_data->clconfig),
          &(tbl->clconfig),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(SCH_sct_tbl_data->af0_inv_weight),
          &(tbl->af0_inv_weight),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_petra_field_in_place_set(
          &(SCH_sct_tbl_data->af1_inv_weight),
          &(tbl->af1_inv_weight),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

  err = soc_petra_field_in_place_set(
          &(SCH_sct_tbl_data->af2_inv_weight),
          &(tbl->af2_inv_weight),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 33, exit);

  err = soc_petra_field_in_place_set(
          &(SCH_sct_tbl_data->af3_inv_weight),
          &(tbl->af3_inv_weight),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 34, exit);

  err = soc_petra_field_in_place_set(
          &(SCH_sct_tbl_data->wfqmode),
          &(tbl->wfqmode),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 35, exit);

  err = soc_petra_field_in_place_set(
          &(SCH_sct_tbl_data->enh_clen),
          &(tbl->enh_clen),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 36, exit);

  err = soc_petra_field_in_place_set(
          &(SCH_sct_tbl_data->enh_clsphigh),
          &(tbl->enh_clsphigh),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 37, exit);

  ind_options = SOC_PETRA_TBL_WRITE_IND_OPT_SET(SOC_PETRA_SCH_SCT_TBL_ENTRY_SIZE, FALSE);
  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_SCH_ID,
          ind_options
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 38, exit);
  SOC_PETRA_TBL_REPS_RESET;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_sch_sct_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table fqm_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_sch_fqm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_SCH_FQM_TBL_DATA* SCH_fqm_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_SCH_FQM_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_SCH_FQM_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_FQM_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          SCH_fqm_tbl_data,
          0x0,
          sizeof(SOC_PETRA_SCH_FQM_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->sch.fqm_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_SCH_ID,
          SOC_PETRA_SCH_FQM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->base_queue_num),
          &(SCH_fqm_tbl_data->base_queue_num)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->sub_flow_mode),
          &(SCH_fqm_tbl_data->sub_flow_mode)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->flow_slow_enable),
          &(SCH_fqm_tbl_data->flow_slow_enable)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 42, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_sch_fqm_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table fqm_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_sch_fqm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_SCH_FQM_TBL_DATA* SCH_fqm_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_SCH_FQM_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_SCH_FQM_TBL
    *tbl;
  uint32
    ind_options;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_FQM_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->sch.fqm_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(SCH_fqm_tbl_data->base_queue_num),
          &(tbl->base_queue_num),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(SCH_fqm_tbl_data->sub_flow_mode),
          &(tbl->sub_flow_mode),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_petra_field_in_place_set(
          &(SCH_fqm_tbl_data->flow_slow_enable),
          &(tbl->flow_slow_enable),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

  ind_options = SOC_PETRA_TBL_WRITE_IND_OPT_SET(SOC_PETRA_SCH_FQM_TBL_ENTRY_SIZE, FALSE);
  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_SCH_ID,
          ind_options
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 33, exit);
  SOC_PETRA_TBL_REPS_RESET;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_sch_fqm_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table ffm_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_sch_ffm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_SCH_FFM_TBL_DATA* SCH_ffm_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_SCH_FFM_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_SCH_FFM_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_FFM_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          SCH_ffm_tbl_data,
          0x0,
          sizeof(SOC_PETRA_SCH_FFM_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->sch.ffm_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_SCH_ID,
          SOC_PETRA_SCH_FFM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->device_number),
          &(SCH_ffm_tbl_data->device_number)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_sch_ffm_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table ffm_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_sch_ffm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_SCH_FFM_TBL_DATA* SCH_ffm_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_SCH_FFM_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_SCH_FFM_TBL
    *tbl;
  uint32
    ind_options;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_FFM_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->sch.ffm_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(SCH_ffm_tbl_data->device_number),
          &(tbl->device_number),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  ind_options = SOC_PETRA_TBL_WRITE_IND_OPT_SET(SOC_PETRA_SCH_FFM_TBL_ENTRY_SIZE, FALSE);
  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_SCH_ID,
          ind_options
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);
  SOC_PETRA_TBL_REPS_RESET;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_sch_ffm_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table soc_tmctbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_sch_tmc_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_SCH_TMC_TBL_DATA* SCH_tmc_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_SCH_TMC_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_SCH_TMC_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_TMC_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          SCH_tmc_tbl_data,
          0x0,
          sizeof(SOC_PETRA_SCH_TMC_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->sch.soc_tmctbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_SCH_ID,
          SOC_PETRA_SCH_TMC_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->token_count),
          &(SCH_tmc_tbl_data->token_count)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->slow_status),
          &(SCH_tmc_tbl_data->slow_status)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_sch_tmc_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table soc_tmctbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_sch_tmc_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_SCH_TMC_TBL_DATA* SCH_tmc_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_SCH_TMC_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_SCH_TMC_TBL
    *tbl;
  uint32
    ind_options;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_TMC_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->sch.soc_tmctbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(SCH_tmc_tbl_data->token_count),
          &(tbl->token_count),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(SCH_tmc_tbl_data->slow_status),
          &(tbl->slow_status),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  ind_options = SOC_PETRA_TBL_WRITE_IND_OPT_SET(SOC_PETRA_SCH_TMC_TBL_ENTRY_SIZE, FALSE);
  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_SCH_ID,
          ind_options
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);
  SOC_PETRA_TBL_REPS_RESET;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_sch_tmc_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table pqs_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_sch_pqs_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_SCH_PQS_TBL_DATA* SCH_pqs_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_SCH_PQS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_SCH_PQS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_PQS_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          SCH_pqs_tbl_data,
          0x0,
          sizeof(SOC_PETRA_SCH_PQS_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->sch.pqs_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_SCH_ID,
          SOC_PETRA_SCH_PQS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->max_qsz),
          &(SCH_pqs_tbl_data->max_qsz)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->flow_id),
          &(SCH_pqs_tbl_data->flow_id)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->aging_bit),
          &(SCH_pqs_tbl_data->aging_bit)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 42, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_sch_pqs_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table pqs_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_sch_pqs_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_SCH_PQS_TBL_DATA* SCH_pqs_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_SCH_PQS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_SCH_PQS_TBL
    *tbl;
  uint32
    ind_options;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_PQS_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->sch.pqs_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(SCH_pqs_tbl_data->max_qsz),
          &(tbl->max_qsz),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(SCH_pqs_tbl_data->flow_id),
          &(tbl->flow_id),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_petra_field_in_place_set(
          &(SCH_pqs_tbl_data->aging_bit),
          &(tbl->aging_bit),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

  ind_options = SOC_PETRA_TBL_WRITE_IND_OPT_SET(SOC_PETRA_SCH_PQS_TBL_ENTRY_SIZE, FALSE);
  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_SCH_ID,
          ind_options
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 33, exit);
  SOC_PETRA_TBL_REPS_RESET;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_sch_pqs_tbl_set_unsafe()",0,0);
}

/*
 * Write indirect table scheduler_init_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_sch_scheduler_init_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_SCH_SCHEDULER_INIT_TBL_DATA* SCH_scheduler_init_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_SCH_SCHEDULER_INIT_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_SCH_SCHEDULER_INIT_TBL
    *tbl;
  uint32
    ind_options;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_SCHEDULER_INIT_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->sch.scheduler_init_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(SCH_scheduler_init_tbl_data->schinit),
          &(tbl->schinit),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  ind_options = SOC_PETRA_TBL_WRITE_IND_OPT_SET(SOC_PETRA_SCH_SCHEDULER_INIT_TBL_ENTRY_SIZE, FALSE);
  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_SCH_ID,
          ind_options
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);
  SOC_PETRA_TBL_REPS_RESET;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_sch_scheduler_init_tbl_set_unsafe()",0,0);
}

/*
 * Write indirect table force_status_message_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_sch_force_status_message_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_SCH_FORCE_STATUS_MESSAGE_TBL_DATA* SCH_force_status_message_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_SCH_FORCE_STATUS_MESSAGE_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_SCH_FORCE_STATUS_MESSAGE_TBL
    *tbl;
  uint32
    ind_options;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_FORCE_STATUS_MESSAGE_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->sch.force_status_message_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_petra_field_in_place_set(
          &(SCH_force_status_message_tbl_data->message_flow_id),
          &(tbl->message_flow_id),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(SCH_force_status_message_tbl_data->message_type),
          &(tbl->message_type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  ind_options = SOC_PETRA_TBL_WRITE_IND_OPT_SET(SOC_PETRA_SCH_FORCE_STATUS_MESSAGE_TBL_ENTRY_SIZE, FALSE);
  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_SCH_ID,
          ind_options
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);
  SOC_PETRA_TBL_REPS_RESET;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_sch_force_status_message_tbl_set_unsafe()",0,0);
}

/*
 * Get the number of banks and number of Drams
 */
uint32
  soc_petra_mmu_dram_address_space_info_get_unsafe(
    SOC_SAND_IN    int                               unit,
    SOC_SAND_OUT   uint32                                *nof_drams,
    SOC_SAND_OUT   uint32                                *nof_banks,
    SOC_SAND_OUT   uint32                                *nof_cols
  )
{
  uint32
    res;
  uint32
    reg_val,
    tmp_val;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_DRAM_ADDRESS_SPACE_INFO_GET_UNSAFE);


  regs = soc_petra_regs();
  SOC_PETRA_REG_GET(regs->mmu.general_configuration_reg, reg_val , 30, exit);

  SOC_PETRA_FLD_FROM_REG(regs->mmu.general_configuration_reg.dram_num, tmp_val, reg_val, 40, exit);

 /*
  * check IF DRAM ID in range.
  */
  switch(tmp_val) {
  case 0x3:
    *nof_drams = 2;
    break;
  case 0x2:
    *nof_drams = 3;
    break;
  case 0x1:
    *nof_drams = 4;
    break;
  case 0x0:
  default:
    *nof_drams = 6;
    break;
  }
 /*
  * check If BANK ID in range.
  */
  SOC_PETRA_FLD_FROM_REG(regs->mmu.general_configuration_reg.dram_bank_num, tmp_val, reg_val, 60, exit);

  switch(tmp_val)
  {
    case 0x0:
      *nof_banks = SOC_PETRA_DRAM_NUM_BANKS_4;
      break;
    case 0x1:
    default:
      *nof_banks = SOC_PETRA_DRAM_NUM_BANKS_8;
      break;
  }

  SOC_PETRA_FLD_FROM_REG(regs->mmu.general_configuration_reg.dram_col_num, tmp_val, reg_val, 80, exit);

  switch(tmp_val) {
  case 0x0:
    *nof_cols = 256;
    break;
  case 0x1:
    *nof_cols = 512;
    break;
  case 0x2:
    *nof_cols = 1024;
    break;
  case 0x3:
    *nof_cols = 2048;
    break;
  case 0x4:
    *nof_cols = 4096;
    break;
  default:
    *nof_cols = 8192;
    break;
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_dram_address_space_info_get()",0,0);
}

/*
 * Write indirect table dram_address_space_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_dram_address_space_tbl_verify(
    SOC_SAND_IN   int                               unit,
    SOC_SAND_IN   uint32                                dram_ndx,
    SOC_SAND_IN   uint32                                bank_ndx,
    SOC_SAND_IN   uint32                                row_ndx,
    SOC_SAND_IN   uint32                                col_ndx
  )
{
  uint32
    res;
  uint32
    offset  = 0;
  uint32
    nof_banks,
    nof_drams,
    nof_cols;
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_DRAM_ADDRESS_SPACE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_DRAM_ADDRESS_SPACE_TBL_SET_UNSAFE);


  res = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.dram_address_space_tbl);

  res = soc_petra_mmu_dram_address_space_info_get_unsafe(
    unit,
    &nof_drams,
    &nof_banks,
    &nof_cols
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
 /*
  * check If BANK ID in range.
  */
  if (bank_ndx >= nof_banks)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_TBL_RANGE_OUT_OF_LIMIT_ERR, 70, exit);
  }
 /*
  * check If Column ID in range.
  */
  if (col_ndx >= nof_cols)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_TBL_RANGE_OUT_OF_LIMIT_ERR, 90, exit);
  }

  offset =  tbl->addr.base + row_ndx;
  SOC_PETRA_TBL_SIZE_CHECK;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_idf_tbl_get_unsafe()",0,0);
}
/*
 * Read indirect table dram_address_space_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_dram_address_space_tbl_get_unsafe(
    SOC_SAND_IN   int                               unit,
    SOC_SAND_IN   uint32                                dram_ndx,
    SOC_SAND_IN   uint32                                bank_ndx,
    SOC_SAND_IN   uint32                                row_ndx,
    SOC_SAND_IN   uint32                                col_ndx,
    SOC_SAND_OUT  SOC_PETRA_MMU_DRAM_ADDRESS_SPACE_TBL_DATA*  MMU_dram_address_space_tbl_data
  )
{
  uint32
    res;
  uint32
    offset  = 0,
    cols_bits,
    col,
    data[SOC_PETRA_MMU_DRAM_ADDRESS_SPACE_TBL_ENTRY_SIZE];
  uint32
    start_from;
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_MMU_DRAM_ADDRESS_SPACE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_DRAM_ADDRESS_SPACE_TBL_GET_UNSAFE);

  res = soc_petra_mmu_dram_address_space_tbl_verify(
          unit,
          dram_ndx,
          bank_ndx,
          row_ndx,
          col_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  res = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = SOC_SAND_OK; sal_memset(
          MMU_dram_address_space_tbl_data,
          0x0,
          sizeof(SOC_PETRA_MMU_DRAM_ADDRESS_SPACE_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  res = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.dram_address_space_tbl);


  regs = soc_petra_regs();
  SOC_PA_FLD_IGET(regs->drc.drc_general_configurations_reg.num_cols, cols_bits, dram_ndx, 30, exit);
  cols_bits += 8;
  cols_bits -= 4; /*since 4 lsb are not written*/

  col = col_ndx >> 4;

  offset = bank_ndx;
  start_from = 3;
  offset |= SOC_SAND_SET_BITS_RANGE(dram_ndx, start_from + SOC_PETRA_TBL_ACC_DRAM_NDX_NOF_BITS - 1, start_from);
  start_from += SOC_PETRA_TBL_ACC_DRAM_NDX_NOF_BITS;

  offset |= SOC_SAND_SET_BITS_RANGE(col, cols_bits + start_from - 1, start_from);
  start_from += cols_bits;
  offset |= SOC_SAND_SET_BITS_RANGE(row_ndx, 31, start_from);

  offset =  tbl->addr.base + offset;
  SOC_PETRA_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_DRAM_ADDRESS_SPACE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->data),
          (MMU_dram_address_space_tbl_data->data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_dram_address_space_tbl_get_unsafe()",0,0);
}

uint32
  soc_petra_mmu_dram_address_space_tbl_set_unsafe(
    SOC_SAND_IN   int                               unit,
    SOC_SAND_IN   uint32                                dram_ndx,
    SOC_SAND_IN   uint32                                bank_ndx,
    SOC_SAND_IN   uint32                                row_ndx,
    SOC_SAND_IN   uint32                                col_ndx,
    SOC_SAND_IN  SOC_PETRA_MMU_DRAM_ADDRESS_SPACE_TBL_DATA*  MMU_dram_address_space_tbl_data
  )
{
  uint32
    res;
  uint32
    offset  = 0,
    cols_bits,
    col,
    data[SOC_PETRA_MMU_DRAM_ADDRESS_SPACE_TBL_ENTRY_SIZE];
  uint32
    start_from;
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_MMU_DRAM_ADDRESS_SPACE_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_DRAM_ADDRESS_SPACE_TBL_SET_UNSAFE);

  res = soc_petra_mmu_dram_address_space_tbl_verify(
          unit,
          dram_ndx,
          bank_ndx,
          row_ndx,
          col_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  res = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.dram_address_space_tbl);

  regs = soc_petra_regs();
  SOC_PA_FLD_IGET(regs->drc.drc_general_configurations_reg.num_cols, cols_bits, dram_ndx ,30, exit);

  cols_bits += 8;
  cols_bits -= 4; /*since 4 lsb are not written*/

  col = col_ndx >> 4;

  offset = bank_ndx;
  start_from = 3;
  offset |= SOC_SAND_SET_BITS_RANGE(dram_ndx, start_from + SOC_PETRA_TBL_ACC_DRAM_NDX_NOF_BITS - 1, start_from);
  start_from += SOC_PETRA_TBL_ACC_DRAM_NDX_NOF_BITS;

  offset |= SOC_SAND_SET_BITS_RANGE(col, cols_bits + start_from - 1, start_from);
  start_from += cols_bits;
  offset |= SOC_SAND_SET_BITS_RANGE(row_ndx, 31, start_from);

  offset =  tbl->addr.base + offset;
  SOC_PETRA_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          (MMU_dram_address_space_tbl_data->data),
          &(tbl->data),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_DRAM_ADDRESS_SPACE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_dram_address_space_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table idf_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_idf_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_IDF_TBL_DATA* MMU_idf_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_IDF_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_IDF_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_IDF_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          MMU_idf_tbl_data,
          0x0,
          sizeof(SOC_PETRA_MMU_IDF_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.idf_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_IDF_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->data),
          &(MMU_idf_tbl_data->data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_idf_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table idf_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_idf_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_IDF_TBL_DATA* MMU_idf_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_IDF_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_IDF_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_IDF_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.idf_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          &(MMU_idf_tbl_data->data),
          &(tbl->data),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_IDF_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_idf_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table fdf_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_fdf_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_FDF_TBL_DATA* MMU_fdf_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_FDF_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_FDF_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_FDF_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          MMU_fdf_tbl_data,
          0x0,
          sizeof(SOC_PETRA_MMU_FDF_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.fdf_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_FDF_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->data),
          &(MMU_fdf_tbl_data->data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_fdf_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table fdf_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_fdf_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_FDF_TBL_DATA* MMU_fdf_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_FDF_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_FDF_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_FDF_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.fdf_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          &(MMU_fdf_tbl_data->data),
          &(tbl->data),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_FDF_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_fdf_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table rdfa_waddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_rdfa_waddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_RDFA_WADDR_STATUS_TBL_DATA* MMU_rdfa_waddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_RDFA_WADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_RDFA_WADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_RDFA_WADDR_STATUS_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          MMU_rdfa_waddr_status_tbl_data,
          0x0,
          sizeof(SOC_PETRA_MMU_RDFA_WADDR_STATUS_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.rdfa_waddr_status_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_RDFA_WADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->waddr),
          &(MMU_rdfa_waddr_status_tbl_data->waddr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->status),
          &(MMU_rdfa_waddr_status_tbl_data->status)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_rdfa_waddr_status_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table rdfa_waddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_rdfa_waddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_RDFA_WADDR_STATUS_TBL_DATA* MMU_rdfa_waddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_RDFA_WADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_RDFA_WADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_RDFA_WADDR_STATUS_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.rdfa_waddr_status_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          &(MMU_rdfa_waddr_status_tbl_data->waddr),
          &(tbl->waddr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(MMU_rdfa_waddr_status_tbl_data->status),
          &(tbl->status),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_RDFA_WADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_rdfa_waddr_status_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table rdfb_waddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_rdfb_waddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_RDFB_WADDR_STATUS_TBL_DATA* MMU_rdfb_waddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_RDFB_WADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_RDFB_WADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_RDFB_WADDR_STATUS_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          MMU_rdfb_waddr_status_tbl_data,
          0x0,
          sizeof(SOC_PETRA_MMU_RDFB_WADDR_STATUS_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.rdfb_waddr_status_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_RDFB_WADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->waddr),
          &(MMU_rdfb_waddr_status_tbl_data->waddr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->status),
          &(MMU_rdfb_waddr_status_tbl_data->status)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_rdfb_waddr_status_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table rdfb_waddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_rdfb_waddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_RDFB_WADDR_STATUS_TBL_DATA* MMU_rdfb_waddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_RDFB_WADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_RDFB_WADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_RDFB_WADDR_STATUS_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.rdfb_waddr_status_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          &(MMU_rdfb_waddr_status_tbl_data->waddr),
          &(tbl->waddr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(MMU_rdfb_waddr_status_tbl_data->status),
          &(tbl->status),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_RDFB_WADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_rdfb_waddr_status_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table rdfc_waddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_rdfc_waddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_RDFC_WADDR_STATUS_TBL_DATA* MMU_rdfc_waddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_RDFC_WADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_RDFC_WADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_RDFC_WADDR_STATUS_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          MMU_rdfc_waddr_status_tbl_data,
          0x0,
          sizeof(SOC_PETRA_MMU_RDFC_WADDR_STATUS_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.rdfc_waddr_status_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_RDFC_WADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->waddr),
          &(MMU_rdfc_waddr_status_tbl_data->waddr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->status),
          &(MMU_rdfc_waddr_status_tbl_data->status)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_rdfc_waddr_status_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table rdfc_waddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_rdfc_waddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_RDFC_WADDR_STATUS_TBL_DATA* MMU_rdfc_waddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_RDFC_WADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_RDFC_WADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_RDFC_WADDR_STATUS_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.rdfc_waddr_status_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          &(MMU_rdfc_waddr_status_tbl_data->waddr),
          &(tbl->waddr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(MMU_rdfc_waddr_status_tbl_data->status),
          &(tbl->status),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_RDFC_WADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_rdfc_waddr_status_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table rdfd_waddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_rdfd_waddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_RDFD_WADDR_STATUS_TBL_DATA* MMU_rdfd_waddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_RDFD_WADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_RDFD_WADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_RDFD_WADDR_STATUS_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          MMU_rdfd_waddr_status_tbl_data,
          0x0,
          sizeof(SOC_PETRA_MMU_RDFD_WADDR_STATUS_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.rdfd_waddr_status_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_RDFD_WADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->waddr),
          &(MMU_rdfd_waddr_status_tbl_data->waddr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->status),
          &(MMU_rdfd_waddr_status_tbl_data->status)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_rdfd_waddr_status_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table rdfd_waddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_rdfd_waddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_RDFD_WADDR_STATUS_TBL_DATA* MMU_rdfd_waddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_RDFD_WADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_RDFD_WADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_RDFD_WADDR_STATUS_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.rdfd_waddr_status_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          &(MMU_rdfd_waddr_status_tbl_data->waddr),
          &(tbl->waddr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(MMU_rdfd_waddr_status_tbl_data->status),
          &(tbl->status),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_RDFD_WADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_rdfd_waddr_status_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table rdfe_waddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_rdfe_waddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_RDFE_WADDR_STATUS_TBL_DATA* MMU_rdfe_waddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_RDFE_WADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_RDFE_WADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_RDFE_WADDR_STATUS_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          MMU_rdfe_waddr_status_tbl_data,
          0x0,
          sizeof(SOC_PETRA_MMU_RDFE_WADDR_STATUS_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.rdfe_waddr_status_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_RDFE_WADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->waddr),
          &(MMU_rdfe_waddr_status_tbl_data->waddr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->status),
          &(MMU_rdfe_waddr_status_tbl_data->status)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_rdfe_waddr_status_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table rdfe_waddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_rdfe_waddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_RDFE_WADDR_STATUS_TBL_DATA* MMU_rdfe_waddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_RDFE_WADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_RDFE_WADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_RDFE_WADDR_STATUS_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.rdfe_waddr_status_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          &(MMU_rdfe_waddr_status_tbl_data->waddr),
          &(tbl->waddr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(MMU_rdfe_waddr_status_tbl_data->status),
          &(tbl->status),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_RDFE_WADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_rdfe_waddr_status_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table rdff_waddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_rdff_waddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_RDFF_WADDR_STATUS_TBL_DATA* MMU_rdff_waddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_RDFF_WADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_RDFF_WADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_RDFF_WADDR_STATUS_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          MMU_rdff_waddr_status_tbl_data,
          0x0,
          sizeof(SOC_PETRA_MMU_RDFF_WADDR_STATUS_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.rdff_waddr_status_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_RDFF_WADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->waddr),
          &(MMU_rdff_waddr_status_tbl_data->waddr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->status),
          &(MMU_rdff_waddr_status_tbl_data->status)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_rdff_waddr_status_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table rdff_waddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_rdff_waddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_RDFF_WADDR_STATUS_TBL_DATA* MMU_rdff_waddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_RDFF_WADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_RDFF_WADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_RDFF_WADDR_STATUS_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.rdff_waddr_status_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          &(MMU_rdff_waddr_status_tbl_data->waddr),
          &(tbl->waddr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(MMU_rdff_waddr_status_tbl_data->status),
          &(tbl->status),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_RDFF_WADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_rdff_waddr_status_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table rdf_raddr_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_rdf_raddr_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_RDF_RADDR_TBL_DATA* MMU_rdf_raddr_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_RDF_RADDR_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_RDF_RADDR_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_RDF_RADDR_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          MMU_rdf_raddr_tbl_data,
          0x0,
          sizeof(SOC_PETRA_MMU_RDF_RADDR_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.rdf_raddr_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_RDF_RADDR_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->raddr),
          &(MMU_rdf_raddr_tbl_data->raddr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_rdf_raddr_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table rdf_raddr_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_rdf_raddr_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_RDF_RADDR_TBL_DATA* MMU_rdf_raddr_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_RDF_RADDR_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_RDF_RADDR_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_RDF_RADDR_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.rdf_raddr_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          &(MMU_rdf_raddr_tbl_data->raddr),
          &(tbl->raddr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_RDF_RADDR_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_rdf_raddr_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table waf_halfa_waddr_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_waf_halfa_waddr_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_WAF_HALFA_WADDR_TBL_DATA* MMU_waf_halfa_waddr_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_WAF_HALFA_WADDR_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_WAF_HALFA_WADDR_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_WAF_HALFA_WADDR_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          MMU_waf_halfa_waddr_tbl_data,
          0x0,
          sizeof(SOC_PETRA_MMU_WAF_HALFA_WADDR_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.waf_halfa_waddr_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_WAF_HALFA_WADDR_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->waddr_half_a),
          &(MMU_waf_halfa_waddr_tbl_data->waddr_half_a)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_waf_halfa_waddr_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table waf_halfa_waddr_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_waf_halfa_waddr_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_WAF_HALFA_WADDR_TBL_DATA* MMU_waf_halfa_waddr_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_WAF_HALFA_WADDR_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_WAF_HALFA_WADDR_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_WAF_HALFA_WADDR_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.waf_halfa_waddr_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          &(MMU_waf_halfa_waddr_tbl_data->waddr_half_a),
          &(tbl->waddr_half_a),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_WAF_HALFA_WADDR_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_waf_halfa_waddr_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table waf_halfb_waddr_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_waf_halfb_waddr_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_WAF_HALFB_WADDR_TBL_DATA* MMU_waf_halfb_waddr_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_WAF_HALFB_WADDR_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_WAF_HALFB_WADDR_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_WAF_HALFB_WADDR_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          MMU_waf_halfb_waddr_tbl_data,
          0x0,
          sizeof(SOC_PETRA_MMU_WAF_HALFB_WADDR_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.waf_halfb_waddr_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_WAF_HALFB_WADDR_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->waddr_half_b),
          &(MMU_waf_halfb_waddr_tbl_data->waddr_half_b)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_waf_halfb_waddr_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table waf_halfb_waddr_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_waf_halfb_waddr_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_WAF_HALFB_WADDR_TBL_DATA* MMU_waf_halfb_waddr_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_WAF_HALFB_WADDR_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_WAF_HALFB_WADDR_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_WAF_HALFB_WADDR_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.waf_halfb_waddr_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          &(MMU_waf_halfb_waddr_tbl_data->waddr_half_b),
          &(tbl->waddr_half_b),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_WAF_HALFB_WADDR_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_waf_halfb_waddr_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table wafa_halfa_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_wafa_halfa_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_WAFA_HALFA_RADDR_STATUS_TBL_DATA* MMU_wafa_halfa_raddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_WAFA_HALFA_RADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_WAFA_HALFA_RADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_WAFA_HALFA_RADDR_STATUS_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          MMU_wafa_halfa_raddr_status_tbl_data,
          0x0,
          sizeof(SOC_PETRA_MMU_WAFA_HALFA_RADDR_STATUS_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.wafa_halfa_raddr_status_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_WAFA_HALFA_RADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->raddr_half_a),
          &(MMU_wafa_halfa_raddr_status_tbl_data->raddr_half_a)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->status_half_a),
          &(MMU_wafa_halfa_raddr_status_tbl_data->status_half_a)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_wafa_halfa_raddr_status_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table wafa_halfa_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_wafa_halfa_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_WAFA_HALFA_RADDR_STATUS_TBL_DATA* MMU_wafa_halfa_raddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_WAFA_HALFA_RADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_WAFA_HALFA_RADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_WAFA_HALFA_RADDR_STATUS_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.wafa_halfa_raddr_status_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          &(MMU_wafa_halfa_raddr_status_tbl_data->raddr_half_a),
          &(tbl->raddr_half_a),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(MMU_wafa_halfa_raddr_status_tbl_data->status_half_a),
          &(tbl->status_half_a),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_WAFA_HALFA_RADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_wafa_halfa_raddr_status_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table wafb_halfa_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_wafb_halfa_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_WAFB_HALFA_RADDR_STATUS_TBL_DATA* MMU_wafb_halfa_raddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_WAFB_HALFA_RADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_WAFB_HALFA_RADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_WAFB_HALFA_RADDR_STATUS_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          MMU_wafb_halfa_raddr_status_tbl_data,
          0x0,
          sizeof(SOC_PETRA_MMU_WAFB_HALFA_RADDR_STATUS_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.wafb_halfa_raddr_status_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_WAFB_HALFA_RADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->raddr_half_a),
          &(MMU_wafb_halfa_raddr_status_tbl_data->raddr_half_a)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->status_half_a),
          &(MMU_wafb_halfa_raddr_status_tbl_data->status_half_a)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_wafb_halfa_raddr_status_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table wafb_halfa_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_wafb_halfa_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_WAFB_HALFA_RADDR_STATUS_TBL_DATA* MMU_wafb_halfa_raddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_WAFB_HALFA_RADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_WAFB_HALFA_RADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_WAFB_HALFA_RADDR_STATUS_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.wafb_halfa_raddr_status_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          &(MMU_wafb_halfa_raddr_status_tbl_data->raddr_half_a),
          &(tbl->raddr_half_a),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(MMU_wafb_halfa_raddr_status_tbl_data->status_half_a),
          &(tbl->status_half_a),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_WAFB_HALFA_RADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_wafb_halfa_raddr_status_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table wafc_halfa_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_wafc_halfa_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_WAFC_HALFA_RADDR_STATUS_TBL_DATA* MMU_wafc_halfa_raddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_WAFC_HALFA_RADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_WAFC_HALFA_RADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_WAFC_HALFA_RADDR_STATUS_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          MMU_wafc_halfa_raddr_status_tbl_data,
          0x0,
          sizeof(SOC_PETRA_MMU_WAFC_HALFA_RADDR_STATUS_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.wafc_halfa_raddr_status_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_WAFC_HALFA_RADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->raddr_half_a),
          &(MMU_wafc_halfa_raddr_status_tbl_data->raddr_half_a)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->status_half_a),
          &(MMU_wafc_halfa_raddr_status_tbl_data->status_half_a)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_wafc_halfa_raddr_status_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table wafc_halfa_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_wafc_halfa_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_WAFC_HALFA_RADDR_STATUS_TBL_DATA* MMU_wafc_halfa_raddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_WAFC_HALFA_RADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_WAFC_HALFA_RADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_WAFC_HALFA_RADDR_STATUS_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.wafc_halfa_raddr_status_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          &(MMU_wafc_halfa_raddr_status_tbl_data->raddr_half_a),
          &(tbl->raddr_half_a),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(MMU_wafc_halfa_raddr_status_tbl_data->status_half_a),
          &(tbl->status_half_a),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_WAFC_HALFA_RADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_wafc_halfa_raddr_status_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table wafd_halfa_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_wafd_halfa_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_WAFD_HALFA_RADDR_STATUS_TBL_DATA* MMU_wafd_halfa_raddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_WAFD_HALFA_RADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_WAFD_HALFA_RADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_WAFD_HALFA_RADDR_STATUS_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          MMU_wafd_halfa_raddr_status_tbl_data,
          0x0,
          sizeof(SOC_PETRA_MMU_WAFD_HALFA_RADDR_STATUS_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.wafd_halfa_raddr_status_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_WAFD_HALFA_RADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->raddr_half_a),
          &(MMU_wafd_halfa_raddr_status_tbl_data->raddr_half_a)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->status_half_a),
          &(MMU_wafd_halfa_raddr_status_tbl_data->status_half_a)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_wafd_halfa_raddr_status_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table wafd_halfa_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_wafd_halfa_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_WAFD_HALFA_RADDR_STATUS_TBL_DATA* MMU_wafd_halfa_raddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_WAFD_HALFA_RADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_WAFD_HALFA_RADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_WAFD_HALFA_RADDR_STATUS_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.wafd_halfa_raddr_status_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          &(MMU_wafd_halfa_raddr_status_tbl_data->raddr_half_a),
          &(tbl->raddr_half_a),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(MMU_wafd_halfa_raddr_status_tbl_data->status_half_a),
          &(tbl->status_half_a),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_WAFD_HALFA_RADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_wafd_halfa_raddr_status_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table wafe_halfa_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_wafe_halfa_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_WAFE_HALFA_RADDR_STATUS_TBL_DATA* MMU_wafe_halfa_raddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_WAFE_HALFA_RADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_WAFE_HALFA_RADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_WAFE_HALFA_RADDR_STATUS_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          MMU_wafe_halfa_raddr_status_tbl_data,
          0x0,
          sizeof(SOC_PETRA_MMU_WAFE_HALFA_RADDR_STATUS_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.wafe_halfa_raddr_status_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_WAFE_HALFA_RADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->raddr_half_a),
          &(MMU_wafe_halfa_raddr_status_tbl_data->raddr_half_a)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->status_half_a),
          &(MMU_wafe_halfa_raddr_status_tbl_data->status_half_a)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_wafe_halfa_raddr_status_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table wafe_halfa_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_wafe_halfa_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_WAFE_HALFA_RADDR_STATUS_TBL_DATA* MMU_wafe_halfa_raddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_WAFE_HALFA_RADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_WAFE_HALFA_RADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_WAFE_HALFA_RADDR_STATUS_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.wafe_halfa_raddr_status_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          &(MMU_wafe_halfa_raddr_status_tbl_data->raddr_half_a),
          &(tbl->raddr_half_a),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(MMU_wafe_halfa_raddr_status_tbl_data->status_half_a),
          &(tbl->status_half_a),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_WAFE_HALFA_RADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_wafe_halfa_raddr_status_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table waff_halfa_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_waff_halfa_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_WAFF_HALFA_RADDR_STATUS_TBL_DATA* MMU_waff_halfa_raddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_WAFF_HALFA_RADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_WAFF_HALFA_RADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_WAFF_HALFA_RADDR_STATUS_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          MMU_waff_halfa_raddr_status_tbl_data,
          0x0,
          sizeof(SOC_PETRA_MMU_WAFF_HALFA_RADDR_STATUS_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.waff_halfa_raddr_status_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_WAFF_HALFA_RADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->raddr_half_a),
          &(MMU_waff_halfa_raddr_status_tbl_data->raddr_half_a)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->status_half_a),
          &(MMU_waff_halfa_raddr_status_tbl_data->status_half_a)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_waff_halfa_raddr_status_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table waff_halfa_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_waff_halfa_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_WAFF_HALFA_RADDR_STATUS_TBL_DATA* MMU_waff_halfa_raddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_WAFF_HALFA_RADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_WAFF_HALFA_RADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_WAFF_HALFA_RADDR_STATUS_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.waff_halfa_raddr_status_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          &(MMU_waff_halfa_raddr_status_tbl_data->raddr_half_a),
          &(tbl->raddr_half_a),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(MMU_waff_halfa_raddr_status_tbl_data->status_half_a),
          &(tbl->status_half_a),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_WAFF_HALFA_RADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_waff_halfa_raddr_status_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table wafa_halfb_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_wafa_halfb_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_WAFA_HALFB_RADDR_STATUS_TBL_DATA* MMU_wafa_halfb_raddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_WAFA_HALFB_RADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_WAFA_HALFB_RADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_WAFA_HALFB_RADDR_STATUS_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          MMU_wafa_halfb_raddr_status_tbl_data,
          0x0,
          sizeof(SOC_PETRA_MMU_WAFA_HALFB_RADDR_STATUS_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.wafa_halfb_raddr_status_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_WAFA_HALFB_RADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->raddr_half_b),
          &(MMU_wafa_halfb_raddr_status_tbl_data->raddr_half_b)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->status_half_b),
          &(MMU_wafa_halfb_raddr_status_tbl_data->status_half_b)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_wafa_halfb_raddr_status_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table wafa_halfb_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_wafa_halfb_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_WAFA_HALFB_RADDR_STATUS_TBL_DATA* MMU_wafa_halfb_raddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_WAFA_HALFB_RADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_WAFA_HALFB_RADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_WAFA_HALFB_RADDR_STATUS_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.wafa_halfb_raddr_status_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          &(MMU_wafa_halfb_raddr_status_tbl_data->raddr_half_b),
          &(tbl->raddr_half_b),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(MMU_wafa_halfb_raddr_status_tbl_data->status_half_b),
          &(tbl->status_half_b),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_WAFA_HALFB_RADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_wafa_halfb_raddr_status_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table wafb_halfb_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_wafb_halfb_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_WAFB_HALFB_RADDR_STATUS_TBL_DATA* MMU_wafb_halfb_raddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_WAFB_HALFB_RADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_WAFB_HALFB_RADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_WAFB_HALFB_RADDR_STATUS_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          MMU_wafb_halfb_raddr_status_tbl_data,
          0x0,
          sizeof(SOC_PETRA_MMU_WAFB_HALFB_RADDR_STATUS_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.wafb_halfb_raddr_status_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_WAFB_HALFB_RADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->raddr_half_b),
          &(MMU_wafb_halfb_raddr_status_tbl_data->raddr_half_b)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->status_half_b),
          &(MMU_wafb_halfb_raddr_status_tbl_data->status_half_b)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_wafb_halfb_raddr_status_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table wafb_halfb_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_wafb_halfb_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_WAFB_HALFB_RADDR_STATUS_TBL_DATA* MMU_wafb_halfb_raddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_WAFB_HALFB_RADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_WAFB_HALFB_RADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_WAFB_HALFB_RADDR_STATUS_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.wafb_halfb_raddr_status_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          &(MMU_wafb_halfb_raddr_status_tbl_data->raddr_half_b),
          &(tbl->raddr_half_b),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(MMU_wafb_halfb_raddr_status_tbl_data->status_half_b),
          &(tbl->status_half_b),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_WAFB_HALFB_RADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_wafb_halfb_raddr_status_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table wafc_halfb_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_wafc_halfb_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_WAFC_HALFB_RADDR_STATUS_TBL_DATA* MMU_wafc_halfb_raddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_WAFC_HALFB_RADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_WAFC_HALFB_RADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_WAFC_HALFB_RADDR_STATUS_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          MMU_wafc_halfb_raddr_status_tbl_data,
          0x0,
          sizeof(SOC_PETRA_MMU_WAFC_HALFB_RADDR_STATUS_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.wafc_halfb_raddr_status_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_WAFC_HALFB_RADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->raddr_half_b),
          &(MMU_wafc_halfb_raddr_status_tbl_data->raddr_half_b)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->status_half_b),
          &(MMU_wafc_halfb_raddr_status_tbl_data->status_half_b)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_wafc_halfb_raddr_status_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table wafc_halfb_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_wafc_halfb_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_WAFC_HALFB_RADDR_STATUS_TBL_DATA* MMU_wafc_halfb_raddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_WAFC_HALFB_RADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_WAFC_HALFB_RADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_WAFC_HALFB_RADDR_STATUS_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.wafc_halfb_raddr_status_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          &(MMU_wafc_halfb_raddr_status_tbl_data->raddr_half_b),
          &(tbl->raddr_half_b),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(MMU_wafc_halfb_raddr_status_tbl_data->status_half_b),
          &(tbl->status_half_b),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_WAFC_HALFB_RADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_wafc_halfb_raddr_status_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table wafd_halfb_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_wafd_halfb_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_WAFD_HALFB_RADDR_STATUS_TBL_DATA* MMU_wafd_halfb_raddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_WAFD_HALFB_RADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_WAFD_HALFB_RADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_WAFD_HALFB_RADDR_STATUS_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          MMU_wafd_halfb_raddr_status_tbl_data,
          0x0,
          sizeof(SOC_PETRA_MMU_WAFD_HALFB_RADDR_STATUS_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.wafd_halfb_raddr_status_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_WAFD_HALFB_RADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->raddr_half_b),
          &(MMU_wafd_halfb_raddr_status_tbl_data->raddr_half_b)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->status_half_b),
          &(MMU_wafd_halfb_raddr_status_tbl_data->status_half_b)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_wafd_halfb_raddr_status_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table wafd_halfb_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_wafd_halfb_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_WAFD_HALFB_RADDR_STATUS_TBL_DATA* MMU_wafd_halfb_raddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_WAFD_HALFB_RADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_WAFD_HALFB_RADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_WAFD_HALFB_RADDR_STATUS_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.wafd_halfb_raddr_status_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          &(MMU_wafd_halfb_raddr_status_tbl_data->raddr_half_b),
          &(tbl->raddr_half_b),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(MMU_wafd_halfb_raddr_status_tbl_data->status_half_b),
          &(tbl->status_half_b),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_WAFD_HALFB_RADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_wafd_halfb_raddr_status_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table wafe_halfb_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_wafe_halfb_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_WAFE_HALFB_RADDR_STATUS_TBL_DATA* MMU_wafe_halfb_raddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_WAFE_HALFB_RADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_WAFE_HALFB_RADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_WAFE_HALFB_RADDR_STATUS_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          MMU_wafe_halfb_raddr_status_tbl_data,
          0x0,
          sizeof(SOC_PETRA_MMU_WAFE_HALFB_RADDR_STATUS_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.wafe_halfb_raddr_status_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_WAFE_HALFB_RADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->raddr_half_b),
          &(MMU_wafe_halfb_raddr_status_tbl_data->raddr_half_b)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->status_half_b),
          &(MMU_wafe_halfb_raddr_status_tbl_data->status_half_b)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_wafe_halfb_raddr_status_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table wafe_halfb_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_wafe_halfb_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_WAFE_HALFB_RADDR_STATUS_TBL_DATA* MMU_wafe_halfb_raddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_WAFE_HALFB_RADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_WAFE_HALFB_RADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_WAFE_HALFB_RADDR_STATUS_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.wafe_halfb_raddr_status_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          &(MMU_wafe_halfb_raddr_status_tbl_data->raddr_half_b),
          &(tbl->raddr_half_b),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(MMU_wafe_halfb_raddr_status_tbl_data->status_half_b),
          &(tbl->status_half_b),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_WAFE_HALFB_RADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_wafe_halfb_raddr_status_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table waff_halfb_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_waff_halfb_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_WAFF_HALFB_RADDR_STATUS_TBL_DATA* MMU_waff_halfb_raddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_WAFF_HALFB_RADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_WAFF_HALFB_RADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_WAFF_HALFB_RADDR_STATUS_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          MMU_waff_halfb_raddr_status_tbl_data,
          0x0,
          sizeof(SOC_PETRA_MMU_WAFF_HALFB_RADDR_STATUS_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.waff_halfb_raddr_status_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_WAFF_HALFB_RADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->raddr_half_b),
          &(MMU_waff_halfb_raddr_status_tbl_data->raddr_half_b)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->status_half_b),
          &(MMU_waff_halfb_raddr_status_tbl_data->status_half_b)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_waff_halfb_raddr_status_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table waff_halfb_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_waff_halfb_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_WAFF_HALFB_RADDR_STATUS_TBL_DATA* MMU_waff_halfb_raddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_WAFF_HALFB_RADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_WAFF_HALFB_RADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_WAFF_HALFB_RADDR_STATUS_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.waff_halfb_raddr_status_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          &(MMU_waff_halfb_raddr_status_tbl_data->raddr_half_b),
          &(tbl->raddr_half_b),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(MMU_waff_halfb_raddr_status_tbl_data->status_half_b),
          &(tbl->status_half_b),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_WAFF_HALFB_RADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_waff_halfb_raddr_status_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table raf_waddr_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_raf_waddr_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_RAF_WADDR_TBL_DATA* MMU_raf_waddr_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_RAF_WADDR_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_RAF_WADDR_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_RAF_WADDR_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          MMU_raf_waddr_tbl_data,
          0x0,
          sizeof(SOC_PETRA_MMU_RAF_WADDR_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.raf_waddr_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_RAF_WADDR_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->waddr),
          &(MMU_raf_waddr_tbl_data->waddr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_raf_waddr_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table raf_waddr_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_raf_waddr_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_RAF_WADDR_TBL_DATA* MMU_raf_waddr_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_RAF_WADDR_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_RAF_WADDR_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_RAF_WADDR_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.raf_waddr_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          &(MMU_raf_waddr_tbl_data->waddr),
          &(tbl->waddr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_RAF_WADDR_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_raf_waddr_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table rafa_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_rafa_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_RAFA_RADDR_STATUS_TBL_DATA* MMU_rafa_raddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_RAFA_RADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_RAFA_RADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_RAFA_RADDR_STATUS_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          MMU_rafa_raddr_status_tbl_data,
          0x0,
          sizeof(SOC_PETRA_MMU_RAFA_RADDR_STATUS_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.rafa_raddr_status_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_RAFA_RADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->raddr),
          &(MMU_rafa_raddr_status_tbl_data->raddr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->status),
          &(MMU_rafa_raddr_status_tbl_data->status)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_rafa_raddr_status_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table rafa_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_rafa_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_RAFA_RADDR_STATUS_TBL_DATA* MMU_rafa_raddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_RAFA_RADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_RAFA_RADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_RAFA_RADDR_STATUS_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.rafa_raddr_status_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          &(MMU_rafa_raddr_status_tbl_data->raddr),
          &(tbl->raddr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(MMU_rafa_raddr_status_tbl_data->status),
          &(tbl->status),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_RAFA_RADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_rafa_raddr_status_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table rafb_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_rafb_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_RAFB_RADDR_STATUS_TBL_DATA* MMU_rafb_raddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_RAFB_RADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_RAFB_RADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_RAFB_RADDR_STATUS_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          MMU_rafb_raddr_status_tbl_data,
          0x0,
          sizeof(SOC_PETRA_MMU_RAFB_RADDR_STATUS_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.rafb_raddr_status_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_RAFB_RADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->raddr),
          &(MMU_rafb_raddr_status_tbl_data->raddr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->status),
          &(MMU_rafb_raddr_status_tbl_data->status)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_rafb_raddr_status_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table rafb_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_rafb_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_RAFB_RADDR_STATUS_TBL_DATA* MMU_rafb_raddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_RAFB_RADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_RAFB_RADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_RAFB_RADDR_STATUS_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.rafb_raddr_status_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          &(MMU_rafb_raddr_status_tbl_data->raddr),
          &(tbl->raddr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(MMU_rafb_raddr_status_tbl_data->status),
          &(tbl->status),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_RAFB_RADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_rafb_raddr_status_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table rafc_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_rafc_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_RAFC_RADDR_STATUS_TBL_DATA* MMU_rafc_raddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_RAFC_RADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_RAFC_RADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_RAFC_RADDR_STATUS_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          MMU_rafc_raddr_status_tbl_data,
          0x0,
          sizeof(SOC_PETRA_MMU_RAFC_RADDR_STATUS_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.rafc_raddr_status_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_RAFC_RADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->raddr),
          &(MMU_rafc_raddr_status_tbl_data->raddr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->status),
          &(MMU_rafc_raddr_status_tbl_data->status)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_rafc_raddr_status_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table rafc_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_rafc_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_RAFC_RADDR_STATUS_TBL_DATA* MMU_rafc_raddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_RAFC_RADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_RAFC_RADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_RAFC_RADDR_STATUS_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.rafc_raddr_status_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          &(MMU_rafc_raddr_status_tbl_data->raddr),
          &(tbl->raddr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(MMU_rafc_raddr_status_tbl_data->status),
          &(tbl->status),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_RAFC_RADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_rafc_raddr_status_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table rafd_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_rafd_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_RAFD_RADDR_STATUS_TBL_DATA* MMU_rafd_raddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_RAFD_RADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_RAFD_RADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_RAFD_RADDR_STATUS_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          MMU_rafd_raddr_status_tbl_data,
          0x0,
          sizeof(SOC_PETRA_MMU_RAFD_RADDR_STATUS_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.rafd_raddr_status_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_RAFD_RADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->raddr),
          &(MMU_rafd_raddr_status_tbl_data->raddr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->status),
          &(MMU_rafd_raddr_status_tbl_data->status)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_rafd_raddr_status_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table rafd_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_rafd_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_RAFD_RADDR_STATUS_TBL_DATA* MMU_rafd_raddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_RAFD_RADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_RAFD_RADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_RAFD_RADDR_STATUS_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.rafd_raddr_status_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          &(MMU_rafd_raddr_status_tbl_data->raddr),
          &(tbl->raddr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(MMU_rafd_raddr_status_tbl_data->status),
          &(tbl->status),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_RAFD_RADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_rafd_raddr_status_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table rafe_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_rafe_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_RAFE_RADDR_STATUS_TBL_DATA* MMU_rafe_raddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_RAFE_RADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_RAFE_RADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_RAFE_RADDR_STATUS_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          MMU_rafe_raddr_status_tbl_data,
          0x0,
          sizeof(SOC_PETRA_MMU_RAFE_RADDR_STATUS_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.rafe_raddr_status_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_RAFE_RADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->raddr),
          &(MMU_rafe_raddr_status_tbl_data->raddr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->status),
          &(MMU_rafe_raddr_status_tbl_data->status)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_rafe_raddr_status_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table rafe_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_rafe_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_RAFE_RADDR_STATUS_TBL_DATA* MMU_rafe_raddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_RAFE_RADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_RAFE_RADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_RAFE_RADDR_STATUS_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.rafe_raddr_status_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          &(MMU_rafe_raddr_status_tbl_data->raddr),
          &(tbl->raddr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(MMU_rafe_raddr_status_tbl_data->status),
          &(tbl->status),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_RAFE_RADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_rafe_raddr_status_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table raff_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_raff_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_RAFF_RADDR_STATUS_TBL_DATA* MMU_raff_raddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_RAFF_RADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_RAFF_RADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_RAFF_RADDR_STATUS_TBL_GET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          MMU_raff_raddr_status_tbl_data,
          0x0,
          sizeof(SOC_PETRA_MMU_RAFF_RADDR_STATUS_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.raff_raddr_status_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_RAFF_RADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->raddr),
          &(MMU_raff_raddr_status_tbl_data->raddr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->status),
          &(MMU_raff_raddr_status_tbl_data->status)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_raff_raddr_status_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table raff_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_mmu_raff_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_RAFF_RADDR_STATUS_TBL_DATA* MMU_raff_raddr_status_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_MMU_RAFF_RADDR_STATUS_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_RAFF_RADDR_STATUS_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MMU_RAFF_RADDR_STATUS_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.raff_raddr_status_tbl);

  offset =  tbl->addr.base + entry_offset;
  SOC_PETRA_TBL_SIZE_CHECK;


  err = soc_petra_field_in_place_set(
          &(MMU_raff_raddr_status_tbl_data->raddr),
          &(tbl->raddr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_petra_field_in_place_set(
          &(MMU_raff_raddr_status_tbl_data->status),
          &(tbl->status),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_MMU_ID,
          SOC_PETRA_MMU_RAFF_RADDR_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mmu_raff_raddr_status_tbl_set_unsafe()",0,0);
}

uint32
  soc_petra_ipt_select_source_sum_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_OUT  SOC_PETRA_IPT_SELECT_SOURCE_SUM_TBL_DATA* IPT_select_source_sum_tbl_data
  )
{
  uint32
    err;
  uint32
    offset  = 0,
    data[SOC_PETRA_IPT_SELECT_SOURCE_SUM_TBL_ENTRY_SIZE];
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IPT_SELECT_SOURCE_SUM_TBL
    *tbl;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPT_SELECT_SOURCE_SUM_TBL_SET_UNSAFE);

  err = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = SOC_SAND_OK; sal_memset(
          IPT_select_source_sum_tbl_data,
          0x0,
          sizeof(SOC_PETRA_IPT_SELECT_SOURCE_SUM_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->ipt.select_source_sum_tbl);

  offset =  tbl->addr.base; /* only one single entry */

  SOC_PETRA_TBL_SIZE_CHECK;

  err = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PETRA_IPT_ID,
          SOC_PETRA_IPT_SELECT_SOURCE_SUM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 32, exit);

  err = soc_petra_field_in_place_get(
          data,
          &(tbl->select_source_sum),
          (IPT_select_source_sum_tbl_data->select_source_sum)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ipt_select_source_sum_set_unsafe()",0,0);
}

/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>
