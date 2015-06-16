#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* $Id: arad_diagnostics.c,v 1.27 Broadcom SDK $
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

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_DIAG

/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/bsl.h>

#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_64cnt.h>
#include <soc/dpp/SAND/SAND_FM/sand_trigger.h>
#include <soc/dpp/SAND/SAND_FM/sand_mem_access.h>
#include <soc/dpp/SAND/SAND_FM/sand_trigger.h>
#include <soc/dpp/SAND/SAND_FM/sand_indirect_access.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>

#include <soc/dpp/ARAD/arad_api_general.h>
#include <soc/dpp/ARAD/arad_api_framework.h>
#include <soc/dpp/ARAD/arad_framework.h>
#include <soc/dpp/ARAD/arad_api_diagnostics.h>
#include <soc/dpp/ARAD/arad_chip_defines.h>
#include <soc/dpp/ARAD/arad_framework.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_mgmt.h>
#include <soc/dpp/ARAD/arad_diagnostics.h>
#include <soc/dpp/ARAD/arad_api_diagnostics.h>
#include <soc/dpp/ARAD/arad_header_parsing_utils.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_scheduler_device.h>
#include <soc/dpp/ARAD/arad_scheduler_ports.h>
#include <soc/dpp/ARAD/arad_scheduler_end2end.h>
#include <soc/dpp/ARAD/arad_scheduler_flows.h>
#include <soc/dpp/ARAD/arad_ingress_packet_queuing.h>
#include <soc/dpp/ARAD/arad_ingress_traffic_mgmt.h>
#include <soc/dpp/ARAD/arad_egr_queuing.h>
#include <soc/dpp/ARAD/arad_scheduler_elements.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_api_debug.h>
#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/ARAD/arad_nif.h>
#include <soc/dpp/ARAD/arad_debug.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/hwstate/hw_log.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

#define ARAD_NOF_DEBUG_HEADER_REGS (2)
#define ARAD_NOF_FLDS_IN_DEBUG_HEADER_REG (512 / 32)
#define ARAD_DIAG_ITMH_BIT_START (992)
#define ARAD_DIAG_ITMH_SIZE_IN_BITS     (32)

#define ARAD_DIAG_BLK_NOF_BITS (768)

/* } */
#define ARAD_DIAG_FLD_FILL(prm_fld, prm_addr_msb, prm_addr_lsb, prm_fld_msb, prm_fld_lsb)  \
  (prm_fld)->base = (prm_addr_msb << 16) + prm_addr_lsb;  \
  (prm_fld)->msb = prm_fld_msb;  \
  (prm_fld)->lsb= prm_fld_lsb;

#define ARAD_DIAG_FLD_READ(prm_fld, prm_blk, prm_addr_msb, prm_addr_lsb, prm_fld_msb, prm_fld_lsb, prm_err_num)  \
  ARAD_DIAG_FLD_FILL(prm_fld, prm_addr_msb, prm_addr_lsb, prm_fld_msb, prm_fld_lsb);    \
  res = arad_diag_dbg_val_get_unsafe(      \
      unit,    \
      prm_blk,      \
      prm_fld,      \
      regs_val      \
    );              \
  SOC_SAND_CHECK_FUNC_RESULT(res, prm_err_num, exit);


/*************
 * GLOBALS   *
 *************/
/* { */

static uint8 Arad_diag_blk_msb_max[] = {24,22,5,11};
static uint8 Arad_plus_diag_blk_msb_max[] = {24,24,5,11};
static uint8 Arad_diag_blk_lsb_max[] = {6,11,2,3};
static uint8 Arad_diag_blk_id[] = {ARAD_IHP_ID,ARAD_IHB_ID,ARAD_EGQ_ID,ARAD_EPNI_ID};


/* } */

/*********************************************************************
 *     read diagnostic value
 *********************************************************************/
uint32
  arad_diag_dbg_val_get_unsafe(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint32              blk,
    SOC_SAND_IN  ARAD_DIAG_REG_FIELD *fld,
    SOC_SAND_OUT uint32              reg_val[ARAD_DIAG_DBG_VAL_LEN]
  )
{
  uint32
    reg32 = 0,
    res = SOC_SAND_OK;
  soc_reg_above_64_val_t
    data_above_64,
    field_above_64;
  soc_reg_t
      reg_00e0r, reg_00e1r, 
      ihp_00e0r = SOC_IS_JERICHO(unit)? IHP_REG_010Cr: IHP_REG_00E0r,
      ihp_00e1r  = SOC_IS_JERICHO(unit)? IHP_REG_010Dr: IHP_REG_00E1r,
      ihb_00e0r = SOC_IS_JERICHO(unit)? IHB_REG_00FEr: IHB_REG_00E0r,
      ihb_00e1r  = SOC_IS_JERICHO(unit)? IHB_REG_00FFr: IHB_REG_00E1r;
      int hw_access;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DIAG_DBG_VAL_GET_UNSAFE);
  ARAD_CLEAR(reg_val,uint32,ARAD_DIAG_DBG_VAL_LEN);

  soc_hw_set_immediate_hw_access(unit, &hw_access);

  if ((blk == ARAD_IHP_ID) || (blk == ARAD_IHB_ID))
  {
    reg_00e0r = (blk == ARAD_IHP_ID)? ihp_00e0r: ihb_00e0r;
    reg_00e1r = (blk == ARAD_IHP_ID)? ihp_00e1r: ihb_00e1r;
    ARAD_FLD_TO_REG(reg_00e0r, FIELD_0_30f, fld->base, reg32, 11, exit);
    ARAD_FLD_TO_REG(reg_00e0r, FIELD_31_31f, 1, reg32, 16, exit);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  21,  exit, ARAD_REG_ACCESS_ERR, soc_reg32_set(unit, reg_00e0r, REG_PORT_ANY, 0, reg32));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 31, exit, soc_reg_above_64_get(unit, reg_00e1r, REG_PORT_ANY, 0, data_above_64));
    soc_reg_above_64_field_get(unit, reg_00e1r , data_above_64, FIELD_0_255f, field_above_64);    
  }
  else if (blk == ARAD_EGQ_ID)
  {
      ARAD_FLD_TO_REG(EGQ_DBG_COMMANDr, DBG_ADDRESSf, fld->base, reg32, 10, exit);
      ARAD_FLD_TO_REG(EGQ_DBG_COMMANDr, DBG_SAMPLE_ENABLEf, 1, reg32, 15, exit);
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,WRITE_EGQ_DBG_COMMANDr(unit, SOC_CORE_ALL,  reg32));
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_EGQ_DBG_DATAr(unit, REG_PORT_ANY, data_above_64));
      soc_reg_above_64_field_get(unit, EGQ_DBG_DATAr, data_above_64, DBG_DATAf, field_above_64);    
  }
  else if (blk == ARAD_EPNI_ID)
  {
      ARAD_FLD_TO_REG(EPNI_DBG_COMMANDr, DBG_ADDRESSf, fld->base, reg32, 10, exit);
      ARAD_FLD_TO_REG(EPNI_DBG_COMMANDr, DBG_SAMPLE_ENABLEf, 1, reg32, 15, exit);
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,WRITE_EPNI_DBG_COMMANDr(unit, SOC_CORE_ALL,  reg32));
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_EPNI_DBG_DATAr(unit, REG_PORT_ANY, data_above_64));
      soc_reg_above_64_field_get(unit, EPNI_DBG_DATAr, data_above_64, DBG_DATAf, field_above_64);    
  }

  res = soc_sand_bitstream_get_any_field(field_above_64,fld->lsb,(fld->msb - fld->lsb +1),reg_val);
exit:
    soc_hw_restore_immediate_hw_access(unit, hw_access);

  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_diag_dbg_val_get_unsafe()", 0, 0);
}


/*********************************************************************
*     Enable/disable diagnostic APIs.affects only APIs with
 *     type: need_sample
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_diag_sample_enable_set_unsafe(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  uint8                                 enable
  )
{
  uint32
    reg32,
    tmp;
  uint32
    res = SOC_SAND_OK;
  soc_reg_t
      ihp_00e0r = SOC_IS_JERICHO(unit)? IHP_REG_010Cr: IHP_REG_00E0r,
      ihb_00e0r = SOC_IS_JERICHO(unit)? IHB_REG_00FEr: IHB_REG_00E0r;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DIAG_SAMPLE_ENABLE_SET_UNSAFE);

  tmp = SOC_SAND_BOOL2NUM(enable);


  /* ihp enable set */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg32_get(unit, ihp_00e0r, REG_PORT_ANY, 0, &reg32));
  ARAD_FLD_TO_REG(ihp_00e0r, FIELD_31_31f, tmp, reg32, 15, exit);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR, soc_reg32_set(unit, ihp_00e0r, REG_PORT_ANY, 0, reg32));
  /* ihb enable set */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg32_get(unit, ihb_00e0r, REG_PORT_ANY, 0, &reg32));
  ARAD_FLD_TO_REG(ihb_00e0r, FIELD_31_31f, tmp, reg32, 15, exit);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR, soc_reg32_set(unit, ihb_00e0r, REG_PORT_ANY, 0, reg32));
  /* epni enable set */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,READ_EPNI_DBG_COMMANDr(unit, SOC_CORE_ALL, &reg32));
  ARAD_FLD_TO_REG(EPNI_DBG_COMMANDr, DBG_SAMPLE_ENABLEf, tmp, reg32, 55, exit);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,WRITE_EPNI_DBG_COMMANDr(unit, SOC_CORE_ALL,  reg32));
  /* egq enable set */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,READ_EGQ_DBG_COMMANDr(unit, SOC_CORE_ALL, &reg32));
  ARAD_FLD_TO_REG(EGQ_DBG_COMMANDr, DBG_SAMPLE_ENABLEf, tmp, reg32, 75, exit);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,WRITE_EGQ_DBG_COMMANDr(unit, SOC_CORE_ALL,  reg32));
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_diag_sample_enable_set_unsafe()", 0, 0);
}

uint32
  arad_diag_sample_enable_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT uint8                                 *enable
  )
{
  uint32
    reg32;
  uint32
    res = SOC_SAND_OK;
  soc_reg_t
      ihp_00e0r = SOC_IS_JERICHO(unit)? IHP_REG_010Cr: IHP_REG_00E0r;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DIAG_SAMPLE_ENABLE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(enable);

  /* ihp enable get */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR, soc_reg32_get(unit, ihp_00e0r, REG_PORT_ANY, 0, &reg32));
  ARAD_FLD_FROM_REG(ihp_00e0r, FIELD_31_31f, *enable, reg32, 15, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_diag_sample_enable_get_unsafe()", 0, 0);
}

#if ARAD_DEBUG

void
  arad_diag_sig_val_print(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint8            block_id,
    SOC_SAND_IN  ARAD_DIAG_REG_FIELD  *fld,
    SOC_SAND_IN  uint32           reg_val[ARAD_DIAG_DBG_VAL_LEN],
    SOC_SAND_IN  uint32           flags
  )
{
  uint32
    idx,
    nof_vals;

  nof_vals = (block_id == ARAD_EPNI_ID)?24:8;

  LOG_CLI((BSL_META_U(unit,
                      "%02d:0x%08x:"), block_id,fld->base));
  for (idx = 0; idx < nof_vals; ++idx)
  {
    LOG_CLI((BSL_META_U(unit,
                        " 0x%08x"), reg_val[idx]));
   /* if (idx % 5 == 4 && idx != nof_vals-1)
    {
      LOG_CLI((BSL_META_U(unit,
                          "\n\r%11s"),""));
    }*/
  }
  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));
}

uint32
  arad_diag_signals_dump_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   flags
  )
{
  uint32
    reg_val[ARAD_DIAG_DBG_VAL_LEN];
  uint32
    i,j,k;
  ARAD_DIAG_REG_FIELD
    fld;
  uint32
    regs_val[ARAD_DIAG_DBG_VAL_LEN];
  uint8
    samp_enable;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DIAG_SIGNALS_DUMP_UNSAFE);
  
  ARAD_CLEAR(reg_val,uint32,ARAD_DIAG_DBG_VAL_LEN);

 /*
  * check if sampling is enabled/ if not retutn error
  */
  res = arad_diag_sample_enable_get_unsafe(unit,&samp_enable);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (!samp_enable)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_DIAG_SAMPLE_NOT_ENABLED_ERR, 5, exit);
  }
    
  for (i = 0; i < sizeof(Arad_diag_blk_id)/sizeof(uint8); ++i)
  {
    for (j = 0; j <= (SOC_IS_ARADPLUS(unit)? Arad_plus_diag_blk_msb_max[i]: Arad_diag_blk_msb_max[i]); ++j)
    {
      for (k = 0; k <= Arad_diag_blk_lsb_max[i]; ++k)
      {
        ARAD_DIAG_FLD_READ(&fld,Arad_diag_blk_id[i],j,k,ARAD_DIAG_BLK_NOF_BITS-1,0,15);

        arad_diag_sig_val_print(unit,Arad_diag_blk_id[i],&fld,regs_val,flags);
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_diag_signals_dump_unsafe()",0,0);
}

/*********************************************************************
*     Returns diagnostics information regarding the last
 *     packet: the incoming TM port and the corresponding PP
 *     port, port header processing type, packet headers and
 *     payload (first 128 Bytes). In case of TM port, the ITMH,
 *     which is part of that buffer, is parsed.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_diag_last_packet_info_get_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_OUT ARAD_DIAG_LAST_PACKET_INFO *last_packet
  )
{
  uint32
    port_ndx,
    small_buff_ndx,
    port_context,
      fld_ndx,
      reg_ndx,
    reg_val,
    fld_val2,
    fld_val3,
    fld_val4 = 0,
    res = SOC_SAND_OK;
  uint32
    buffer_temp;
  ARAD_PORT_HEADER_TYPE  
      header_type_incoming;
  soc_reg_above_64_val_t
      fld_value,
      reg_above_64_val[ARAD_NOF_DEBUG_HEADER_REGS];
  ARAD_HPU_ITMH_HDR
    itmh;
  uint8* buffer_ptr;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DIAG_LAST_PACKET_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(last_packet);

  ARAD_DIAG_LAST_PACKET_INFO_clear(last_packet);
  arad_ARAD_HPU_ITMH_HDR_clear(&itmh);

  SOC_REG_ABOVE_64_CLEAR(reg_above_64_val[0]);
  SOC_REG_ABOVE_64_CLEAR(reg_above_64_val[1]);
  SOC_REG_ABOVE_64_CLEAR(fld_value);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1000, exit, READ_IHP_LAST_RECEIVED_HEADER_REG_0r(unit, REG_PORT_ANY, reg_above_64_val[0]));
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1010, exit, READ_IHP_LAST_RECEIVED_HEADER_REG_1r(unit, REG_PORT_ANY, reg_above_64_val[1]));

  /* copy data of the two registers to the buffer. MSB byte/register is first in packet */
  buffer_ptr = last_packet->buffer + ARAD_NOF_DEBUG_HEADER_REGS * ARAD_NOF_FLDS_IN_DEBUG_HEADER_REG * sizeof(uint32) ;
  for (reg_ndx = 0; reg_ndx < ARAD_NOF_DEBUG_HEADER_REGS; reg_ndx++)
  {
    for (fld_ndx = 0; fld_ndx < ARAD_NOF_FLDS_IN_DEBUG_HEADER_REG; fld_ndx++)
    {
      buffer_temp = reg_above_64_val[reg_ndx][fld_ndx];
      for (small_buff_ndx = sizeof(uint32); small_buff_ndx; --small_buff_ndx)
      {
        *--buffer_ptr = (uint8)(buffer_temp & 0xff);
        buffer_temp >>= 8;
      }
    }
  }

     SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  15,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IHP_LAST_GENERATED_VALUESr, SOC_CORE_ALL, 0, LAST_PP_PORTf, &fld_val3));
     last_packet->pp_port = fld_val3;

     SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 17 , exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IHP_LAST_GENERATED_VALUESr, SOC_CORE_ALL, 0, LAST_PARSER_PROGRAM_POINTERf, &fld_val2 ));
     port_context = fld_val2;

     SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 19 , exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IHP_LAST_GENERATED_VALUESr, SOC_CORE_ALL, 0, LAST_SRC_SYS_PORTf, &fld_val2 ));
     last_packet->src_syst_port = fld_val2;

      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20 , exit, ARAD_REG_ACCESS_ERR,READ_IHP_LAST_RECEIVED_PORTr(unit, SOC_CORE_ALL, &reg_val));
      ARAD_FLD_FROM_REG(IHP_LAST_RECEIVED_PORTr, LAST_RCVD_VALIDf,fld_val4, reg_val, 22 ,exit);
      last_packet->is_valid = fld_val4;
      ARAD_FLD_FROM_REG(IHP_LAST_RECEIVED_PORTr, LAST_RCVD_PORT_TERMINATION_CONTEXTf,fld_val4, reg_val, 20 ,exit);
      last_packet->tm_port = fld_val4;
      last_packet->packet_size = soc_reg_field_get(unit, IHP_LAST_RECEIVED_PORTr, reg_val, LAST_RCVD_PACKET_SIZEf);

      port_ndx = fld_val4;
    
     res = arad_port_parse_header_type_unsafe(
             unit,
             port_ndx,
             port_context,
             &header_type_incoming
           );
    
     SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
    
     last_packet->port_header_type = header_type_incoming;
    
     if (header_type_incoming == ARAD_PORT_HEADER_TYPE_TM)
     {

      /*
       * Construct the ITMH from the retrieved data -
       *  assumption of no previous headers
       */
      res = soc_sand_bitstream_get_any_field(
              reg_above_64_val[1],
              ARAD_DIAG_ITMH_BIT_START,
              ARAD_DIAG_ITMH_SIZE_IN_BITS,
              &(itmh.base)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    
      itmh.extention_src_port = 0;
    
      res = arad_hpu_itmh_parse(
            unit,
              &itmh,
              &(last_packet->itmh)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
     }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_diag_last_packet_info_get_unsafe()", 0, 0);
}

uint32
  arad_diag_last_packet_info_get_verify(
    SOC_SAND_IN  int                unit
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DIAG_LAST_PACKET_INFO_GET_VERIFY);

  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_diag_last_packet_info_get_verify()", 0, 0);
}

uint32
  ARAD_DIAG_LAST_PACKET_INFO_verify(
    SOC_SAND_IN  ARAD_DIAG_LAST_PACKET_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tm_port, ARAD_NOF_FAP_PORTS, ARAD_DIAGNOSTICS_TM_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->pp_port, ARAD_NOF_FAP_PORTS, ARAD_DIAGNOSTICS_PP_PORT_OUT_OF_RANGE_ERR, 11, exit);
  for (ind = 0; ind < ARAD_DIAG_LAST_PCKT_SNAPSHOT_LEN_BYTES; ++ind)
  {
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_DIAG_LAST_PACKET_INFO_verify()",0,0);
}

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif /* of #if defined(BCM_88650_A0) */

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif /* of #if defined(BCM_88650_A0) */
