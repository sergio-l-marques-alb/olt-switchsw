#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* $Id: arad_action_cmd.c,v 1.14 Broadcom SDK $
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

#define _ERR_MSG_MODULE_NAME BSL_SOC_MIRROR
#include <shared/bsl.h>
/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/mem.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/arad_api_framework.h>
#include <soc/dpp/ARAD/arad_framework.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_action_cmd.h>
#include <soc/dpp/ARAD/arad_api_action_cmd.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>

#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/ARAD/arad_mgmt.h>
#include <soc/dpp/PPD/ppd_api_llp_mirror.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define ARAD_ACTION_CMD_SNOOP_INFO_SNOOP_SIZE_MAX                (ARAD_ACTION_NOF_CMD_SIZE_BYTESS-1)
#define ARAD_ACTION_CMD_SNOOP_INFO_SNOOP_PROB_MAX                (1023)
#define ARAD_ACTION_CMD_SNOOP_INFO_SNOOP_PROB_MIN                (0)
#define ARAD_ACTION_CMD_MIRROR_INFO_MIRROR_PROB_MAX              (0xffffffff)
#define ARAD_ACTION_CMD_MIRROR_INFO_MIRROR_PROB_MIN              (0)

#define ARAD_ACTION_CMD_SNOOP_SIZE_NOF_BITS                      (2)
#define ARAD_ACTION_CMD_MIRROR_TABLE_OFFSET                      ((SOC_IS_JERICHO(unit)) ? 0 : 16)
#define ARAD_ACTION_CMD_SNOOP_TABLE_OFFSET                      (16 - (ARAD_ACTION_CMD_MIRROR_TABLE_OFFSET))

/*
SNP_MIR_CMD_MAP should contain 16 entries for mirroring and 16 for snoop. 
In Arad, this table is 64 entries large. This bug was fixed in Jericho.
*/
#define ARAD_ACTION_CMD_IPT_SNP_MIR_CMD_MAP_SNOOP_OFFSET (1<<(5- SOC_IS_JERICHO(unit)))
#define ARAD_ACTION_CMD_IPT_SNP_MIR_CMD_MAP_MIRROR_OFFSET 0

/*
masks for all used mirror/snoop dest 
 
 
jericho masks 
// Ingress Destination Encoding
// NOTE: This encoding is only rellevant in the IDR->IRR->IQM(IngressShaping)->IRR data path

//        +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
//  Bit   | 18| 17| 16| 15| 14| 13| 12| 11| 10| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//        +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
//        | 1 | 1 |                   Flow-ID                     (17 bits)           |
//        +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
//        | 1 | 0 |   Egress/Fabric   Multicast-ID                (17 bits)           |
//        +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
//        | 0 | 1 |   Ingress         Multicast-ID                (17 bits)           |
//        +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
//        | 0 | 0 | 1 |               System-Port-ID (DSPA)       (16 bits)           |
//        +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
//        | 0 | 0 | 1 | 1 |           LAG-ID                      (15 bits)           |
//        +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
//        | 0 | 0 | 1 | 0 |           Physical-System-Port-ID     (15 bits)           |
//        +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
//
 
 
*/ 


#define CREATE_32BITS_MASK_SIMPLE(mask,pos) (create_32bits_mask(mask, pos, 0, 0))
#define QUEUE_DEST_MASK  (SOC_IS_JERICHO(unit) ? CREATE_32BITS_MASK_SIMPLE("11",17) : CREATE_32BITS_MASK_SIMPLE("01",17))
#define MULTICAST_DEST_MASK  (SOC_IS_JERICHO(unit) ? CREATE_32BITS_MASK_SIMPLE("01",17) : CREATE_32BITS_MASK_SIMPLE("001",16))
#define SYS_PHY_PORT_DEST_MASK   (SOC_IS_JERICHO(unit) ? CREATE_32BITS_MASK_SIMPLE("0010",15) : CREATE_32BITS_MASK_SIMPLE("0",17))
#define LAG_DEST_MASK  CREATE_32BITS_MASK_SIMPLE("0011",15)


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

STATIC
uint32
  create_32bits_mask(
	  SOC_SAND_IN char *mask_str,
	  SOC_SAND_IN uint32 start,
	  SOC_SAND_IN uint32  fill_bit_before,
	  SOC_SAND_IN uint32  fill_bit_after
	  )
{
	int i,j;
	int len = sal_strlen(mask_str);
	int end_pos = start + len;
	int end_len = sizeof(uint32)*8 - end_pos;
	uint32 out[]={0};

	if (start > 0) {
		fill_bit_before ? SHR_BITSET_RANGE(out,0,start) : SHR_BITCLR_RANGE(out, 0, start)  ;
	}

	for (j=start,i=len-1;i>=0;i--,j++) {
		mask_str[i]!='0' ? SHR_BITSET(out,j) : SHR_BITCLR(out,j);
	}

	if (end_len < sizeof(uint32)*8 - 1) {
		fill_bit_after ? SHR_BITSET_RANGE(out, end_pos, end_len) : SHR_BITCLR_RANGE(out, end_pos, end_len)  ;
	}

	return out[0];
}

STATIC 
uint32
  ipt_arad_snp_mir_stamp_tables_set(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  uint32                action_ndx,
      SOC_SAND_IN  soc_ppd_mirror_pkt_header_updates_t  *mirror_pkt_header_updates
      )
{
    uint32
      res = SOC_SAND_OK,memory,field;

    uint32 data[] = {0,0,0,0,0,0,0,0,0,0,0};


    uint32 value[] = {0,0,0};

   SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ACTION_CMD_SNOOP_SET_UNSAFE);

   if (!SOC_IS_JERICHO(unit)) {
       return res;
   }





    memory = IPT_ARAD_SNP_MIR_STAMP_ENm ;
/* If the extension doesn't exist, this field is ignored. */
    field = OAM_TS_EXT_ENf ;/*field size in bits (48) */
    soc_mem_field_set(unit, memory, data, field, value);

/* If PPH doesn't exist, this field is ignored. */
    field = PPH_BASE_ENf ;/*field size in bits (56) */
    soc_mem_field_set(unit, memory, data, field, value);

/* If the extension doesn't exist, this field is ignored. */
    field = FTMH_STACK_EXT_ENf ;/*field size in bits (16) */
    soc_mem_field_set(unit, memory, data, field, value);

/* If the extension doesn't exist, this field is ignored. */
    field = PPH_FHEI_EXT_ENf ;/*field size in bits (40) */
    soc_mem_field_set(unit, memory, data, field, value);

/* If the extension doesn't exist, this field is ignored. */
    field = PPH_EEI_EXT_ENf ;/*field size in bits (24) */
    soc_mem_field_set(unit, memory, data, field, value);

/* If the extension doesn't exist, this field is ignored. */
    field = FTMH_DSP_EXT_ENf ;/*field size in bits (16) */
    soc_mem_field_set(unit, memory, data, field, value);

/*  */
    field = FTMH_BASE_ENf ;/*field size in bits (72) */
    soc_mem_field_set(unit, memory, data, field, value);

/* If the extension doesn't exist, this field is ignored. */
    field = FTMH_LB_EXT_ENf ;/*field size in bits (8) */
    soc_mem_field_set(unit, memory, data, field, value);

/* If the extension doesn't exist, this field is ignored. */
    field = PPH_LEARN_EXT_ENf ;/*field size in bits (40) */
    soc_mem_field_set(unit, memory, data, field, value);




    res = WRITE_IPT_ARAD_SNP_MIR_STAMP_ENm(unit, MEM_BLOCK_ANY, action_ndx, data);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);



    memory = IPT_ARAD_SNP_MIR_STAMP_VALUESm ;


/* if the extension existed in the original packet, then this extension will be stamped (if enabled to). If the extension doesn't exist, this field is ignored. */
    field = FTMH_STACK_EXTf ;/*field size in bits (16) */
    soc_mem_field_set(unit, memory, data, field, value);

/* If the extension existed in the original packet, then this extension will be stamped (if enabled to). If the extension doesn't exist, this field is ignored. */
    field = PPH_EEI_EXTf ;/*field size in bits (24) */
    soc_mem_field_set(unit, memory, data, field, value);

/*  */
    field = FTMH_BASEf ;/*field size in bits (72) */
    soc_mem_field_set(unit, memory, data, field, value);

/* if the extension existed in the original packet, then this extension will be stamped (if enabled to). If the extension doesn't exist, this field is ignored. */
    field = FTMH_DSP_EXTf ;/*field size in bits (16) */
    soc_mem_field_set(unit, memory, data, field, value);

/* if the extension existed in the original packet, then this extension will be stamped (if enabled to). If the extension doesn't exist, this field is ignored. */
    field = OAM_TS_EXTf ;/*field size in bits (48) */
    soc_mem_field_set(unit, memory, data, field, value);

/* If the extension existed in the original packet, then this extension will be stamped (if enabled to). If the extension doesn't exist, this field is ignored. */
    field = PPH_LEARN_EXTf ;/*field size in bits (40) */
    soc_mem_field_set(unit, memory, data, field, value);

/* If the FHEI ext is 5 bytes, take all the CFG. If it is 3 bytes, take only lower 24 bits (LSBs). Decide according to : Pph.FheiSizeCode */
    field = PPH_FHEI_EXTf ;/*field size in bits (40) */
    soc_mem_field_set(unit, memory, data, field, value);

/* if the extension existed in the original packet, then this extension will be stamped (if enabled to). If the extension doesn't exist, this field is ignored. */
    field = FTMH_LB_EXTf ;/*field size in bits (8) */
    soc_mem_field_set(unit, memory, data, field, value);

/* If the PPH existed in the original packet, then this header will be stamped (if enabled to). If the PPH doesn't exist, this field is ignored. */
    field = PPH_BASEf ;/*field size in bits (56) */
    soc_mem_field_set(unit, memory, data, field, value);


    res = WRITE_IPT_ARAD_SNP_MIR_STAMP_VALUESm(unit, MEM_BLOCK_ANY, action_ndx, data);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);


    exit:
   SOC_SAND_EXIT_AND_SEND_ERROR("ipt_arad_snp_mir_stamp_tables_set()", action_ndx, 0);
}


STATIC 
uint32
 irr_snoop_mirror_size_get(
     SOC_SAND_IN  int                   unit,
     SOC_SAND_IN  uint32                   action_ndx,
     SOC_SAND_IN  uint32 reg,
     SOC_SAND_IN  uint32 field,
     SOC_TMC_ACTION_CMD_SIZE_BYTES *size,
     SOC_SAND_IN uint32 out_of_range_error_inedx
     )
{
  uint32
    res = SOC_SAND_OK,
    size_enc,
    fld_val;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ACTION_CMD_SNOOP_SET_UNSAFE);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, field, &fld_val));
    size_enc = SOC_SAND_GET_BITS_RANGE(fld_val, (2 * action_ndx) - 1 , (2 * action_ndx) - 2);
    switch (size_enc)
    {
      case 0x0:
        *size = SOC_TMC_ACTION_CMD_SIZE_BYTES_64;
        break;
      case 0x1:
        *size = SOC_TMC_ACTION_CMD_SIZE_BYTES_128;
        break;
      case 0x2:
        *size = SOC_TMC_ACTION_CMD_SIZE_BYTES_192;
          break;
      case 0x3:
        *size = SOC_TMC_ACTION_CMD_SIZE_BYTES_ALL_PCKT;
        break;
      default:
        SOC_SAND_SET_ERROR_CODE(out_of_range_error_inedx, 10, exit);
        break;
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in irr_snoop_mirror_size_set()", action_ndx, 0);
}




STATIC 
uint32
 irr_snoop_mirror_size_set(
     SOC_SAND_IN  int                   unit,
     SOC_SAND_IN  uint32                   action_ndx,
     SOC_SAND_IN  uint32 reg,
     SOC_SAND_IN  uint32 field,
     SOC_SAND_IN  uint32 nof,
     SOC_SAND_IN  SOC_TMC_ACTION_CMD_SIZE_BYTES size,
     SOC_SAND_IN uint32 out_of_range_error_inedx
     )
{
  uint32
    res = SOC_SAND_OK,
    size_enc,
     reg_val;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ACTION_CMD_SNOOP_SET_UNSAFE);
    /* Encode snoop/Mirror size */
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, field, &reg_val));

    switch (size)
    {
      case SOC_TMC_ACTION_CMD_SIZE_BYTES_64:
        size_enc = 0x0;
        break;
      case SOC_TMC_ACTION_CMD_SIZE_BYTES_128:
        size_enc = 0x1;
        break;
      case SOC_TMC_ACTION_CMD_SIZE_BYTES_192:
        size_enc = 0x2;
        break;
      case SOC_TMC_ACTION_CMD_SIZE_BYTES_ALL_PCKT:
        size_enc = 0x3;
        break;
      default:
        SOC_SAND_SET_ERROR_CODE(out_of_range_error_inedx, 10, exit);
        break;
    }

    /* (2 * action_ndx) - 2)  must be greater than 0*/
    res = soc_sand_bitstream_set_field(&reg_val, ((2 * action_ndx) - 2) * (action_ndx>0),nof,size_enc);
    SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, reg, REG_PORT_ANY, 0, field,  reg_val));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in irr_snoop_mirror_size_set()", action_ndx, 0);
}


/*********************************************************************
*     Set a snoop action profile in the snoop action profile
 *     table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_action_cmd_snoop_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   action_ndx,
    SOC_SAND_IN  ARAD_ACTION_CMD_SNOOP_MIRROR_INFO    *info
  )
{
  uint32
    res = SOC_SAND_OK,
    mem_entry[2],
    fld_val;
   
    
  ARAD_IRR_SNOOP_MIRROR_TABLE0_TBL_DATA
    snp_mrr_tbl0_data;
  ARAD_IRR_SNOOP_MIRROR_TABLE1_TBL_DATA
    snp_mrr_tbl1_data;
  ARAD_PP_IHB_SNP_ACT_PROFILE_TBL_DATA
    snp_act_profile_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ACTION_CMD_SNOOP_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  ARAD_CLEAR(&snp_mrr_tbl0_data,ARAD_IRR_SNOOP_MIRROR_TABLE0_TBL_DATA,1);
  ARAD_CLEAR(&snp_mrr_tbl1_data,ARAD_IRR_SNOOP_MIRROR_TABLE1_TBL_DATA,1);
  ARAD_CLEAR(&snp_act_profile_tbl_data,ARAD_PP_IHB_SNP_ACT_PROFILE_TBL_DATA,1);


  /* overwrite outlif */
  /* 1 << 5 as lsb entries for Mirroring */
  res = READ_IPT_SNP_MIR_CMD_MAPm(unit, MEM_BLOCK_ANY, ARAD_ACTION_CMD_IPT_SNP_MIR_CMD_MAP_SNOOP_OFFSET | action_ndx, mem_entry);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 1, exit);

  soc_mem_field32_set(
          unit,
          IPT_SNP_MIR_CMD_MAPm,
          mem_entry,
          OUTLIFf,
          info->cmd.outlif.value);
  

  res = WRITE_IPT_SNP_MIR_CMD_MAPm(unit, MEM_BLOCK_ANY, ARAD_ACTION_CMD_IPT_SNP_MIR_CMD_MAP_SNOOP_OFFSET | action_ndx, mem_entry);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 1, exit);


  /* Encode snoop size */
  irr_snoop_mirror_size_set(unit,action_ndx,IRR_SNOOP_SIZEr,SNOOP_SIZEf,ARAD_ACTION_CMD_SNOOP_SIZE_NOF_BITS,info->size,ARAD_ACTION_CMD_SNOOP_SIZE_OUT_OF_RANGE_ERR);
  SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

 
  /* Encode destination type and id */
  switch(info->cmd.dest_id.type)
  {
  case ARAD_DEST_TYPE_QUEUE:
    snp_mrr_tbl0_data.destination = info->cmd.dest_id.id | ( 1 << 17);
    break;
  case ARAD_DEST_TYPE_MULTICAST:
    snp_mrr_tbl0_data.destination = info->cmd.dest_id.id | ( 1 << 16);
    break;
  case ARAD_DEST_TYPE_SYS_PHY_PORT:
    arad_ports_logical_sys_id_build(unit, FALSE, 0, 0, info->cmd.dest_id.id, &snp_mrr_tbl0_data.destination);
    break;
  case ARAD_DEST_TYPE_LAG:
    arad_ports_logical_sys_id_build(unit, TRUE, info->cmd.dest_id.id, 0, 0, &snp_mrr_tbl0_data.destination);
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(ARAD_ACTION_CMD_DEST_ID_OUT_OF_RANGE_ERR, 30, exit);
    break;
  }

  snp_mrr_tbl0_data.dp          = info->cmd.dp.value;
  snp_mrr_tbl0_data.dp_ow       = SOC_SAND_BOOL2NUM(info->cmd.dp.enable);
  snp_mrr_tbl0_data.tc          = info->cmd.tc.value;
  snp_mrr_tbl0_data.tc_ow       = SOC_SAND_BOOL2NUM(info->cmd.tc.enable);

  /* After setting all parameters in struct, write it to HW*/
  res = arad_irr_snoop_mirror_table0_tbl_set_unsafe(
          unit,
          ARAD_ACTION_CMD_SNOOP_TABLE_OFFSET + action_ndx,
          &snp_mrr_tbl0_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  snp_mrr_tbl1_data.counter_ptr0    = info->cmd.counter_ptr_1.value;
  snp_mrr_tbl1_data.counter_ptr0_ow = SOC_SAND_BOOL2NUM(info->cmd.counter_ptr_1.enable);
  snp_mrr_tbl1_data.counter_ptr1    = info->cmd.counter_ptr_2.value;
  snp_mrr_tbl1_data.counter_ptr1_ow = SOC_SAND_BOOL2NUM(info->cmd.counter_ptr_2.enable);
  snp_mrr_tbl1_data.dp_cmd          = info->cmd.meter_dp.value;
  snp_mrr_tbl1_data.dp_cmd_ow       = SOC_SAND_BOOL2NUM(info->cmd.meter_dp.enable);
  snp_mrr_tbl1_data.meter_ptr0      = info->cmd.meter_ptr_low.value;
  snp_mrr_tbl1_data.meter_ptr0_ow   = SOC_SAND_BOOL2NUM(info->cmd.meter_ptr_low.enable);
  snp_mrr_tbl1_data.meter_ptr1      = info->cmd.meter_ptr_up.value;
  snp_mrr_tbl1_data.meter_ptr1_ow   = SOC_SAND_BOOL2NUM(info->cmd.meter_ptr_up.enable);
  
  /* After setting all parameters in struct, write it to HW*/
  res = arad_irr_snoop_mirror_table1_tbl_set_unsafe(
          unit,
          ARAD_ACTION_CMD_MIRROR_TABLE_OFFSET + action_ndx,
          &snp_mrr_tbl1_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, ECI_GLOBAL_INGRESS_MULTICAST_INDICATIONr, REG_PORT_ANY, 0, SNOOP_IS_ING_MCf, &fld_val));
  SOC_SAND_SET_BIT(fld_val,SOC_SAND_NUM2BOOL(info->cmd.is_ing_mc),action_ndx);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_INGRESS_MULTICAST_INDICATIONr, REG_PORT_ANY, 0, SNOOP_IS_ING_MCf,  fld_val));

  /* Set snoop probability */
  snp_act_profile_tbl_data.sampling_probability = info->prob;

  res = arad_pp_ihb_snp_act_profile_tbl_set_unsafe(
          unit,
          action_ndx,
          &snp_act_profile_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  res = ipt_arad_snp_mir_stamp_tables_set(unit,action_ndx,NULL);
  SOC_SAND_CHECK_FUNC_RESULT(res, 71, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_action_cmd_snoop_set_unsafe()", action_ndx, 0);
}

uint32
  arad_action_cmd_snoop_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   action_ndx,
    SOC_SAND_IN  ARAD_ACTION_CMD_SNOOP_MIRROR_INFO    *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ACTION_CMD_SNOOP_SET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(action_ndx, ARAD_ACTION_NDX_MIN, ARAD_ACTION_NDX_MAX, ARAD_ACTION_NDX_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_STRUCT_VERIFY(ARAD_ACTION_CMD_SNOOP_INFO, info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_action_cmd_snoop_set_verify()", action_ndx, 0);
}

uint32
  arad_action_cmd_snoop_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   action_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ACTION_CMD_SNOOP_GET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(action_ndx, ARAD_ACTION_NDX_MIN, ARAD_ACTION_NDX_MAX, ARAD_ACTION_NDX_OUT_OF_RANGE_ERR, 10, exit);

  /* Implanted */
  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_action_cmd_snoop_get_verify()", action_ndx, 0);
}

/*********************************************************************
*     Set a snoop action profile in the snoop action profile
 *     table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_action_cmd_snoop_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   action_ndx,
    SOC_SAND_OUT ARAD_ACTION_CMD_SNOOP_MIRROR_INFO    *info
  )
{
  uint32
    res = SOC_SAND_OK,
    fld_val,
    lag_id,
    mem_entry[2],
    lag_member_id,
    sys_phys_port_id;
   
    
  ARAD_IRR_SNOOP_MIRROR_TABLE0_TBL_DATA
    snp_mrr_tbl0_data;
  ARAD_IRR_SNOOP_MIRROR_TABLE1_TBL_DATA
    snp_mrr_tbl1_data;
  ARAD_PP_IHB_SNP_ACT_PROFILE_TBL_DATA
    snp_act_profile_tbl_data;
  uint8
    is_lag_not_phys;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ACTION_CMD_SNOOP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  ARAD_ACTION_CMD_SNOOP_INFO_clear(info);

  ARAD_CLEAR(&snp_mrr_tbl0_data,ARAD_IRR_SNOOP_MIRROR_TABLE0_TBL_DATA,1);
  ARAD_CLEAR(&snp_mrr_tbl1_data,ARAD_IRR_SNOOP_MIRROR_TABLE1_TBL_DATA,1);
  ARAD_CLEAR(&snp_act_profile_tbl_data,ARAD_PP_IHB_MRR_ACT_PROFILE_TBL_DATA,1);

  
  /* 1 << 5 as lsb entries for Mirroring */
  res = READ_IPT_SNP_MIR_CMD_MAPm(unit, MEM_BLOCK_ANY, ARAD_ACTION_CMD_IPT_SNP_MIR_CMD_MAP_SNOOP_OFFSET | action_ndx, mem_entry);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 1, exit);

  info->cmd.outlif.value = 
      soc_mem_field32_get(
          unit,
          IPT_SNP_MIR_CMD_MAPm,
          mem_entry,
          OUTLIFf);
  info->cmd.outlif.enable = 1;
  
  /* Encode snoop size */

  res=irr_snoop_mirror_size_get(unit,action_ndx, IRR_SNOOP_SIZEr,SNOOP_SIZEf,&(info->size),ARAD_ACTION_CMD_SNOOP_SIZE_OUT_OF_RANGE_ERR);
  SOC_SAND_CHECK_FUNC_RESULT(res, 2, exit);

  res = arad_irr_snoop_mirror_table0_tbl_get_unsafe(
          unit,
          ARAD_ACTION_CMD_SNOOP_TABLE_OFFSET + action_ndx,
          &snp_mrr_tbl0_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /* Encode destination type and id */
  if (snp_mrr_tbl0_data.destination & ( 1 << 15))
  {
    info->cmd.dest_id.type = ARAD_DEST_TYPE_QUEUE;
    info->cmd.dest_id.id = SOC_SAND_GET_BITS_RANGE(snp_mrr_tbl0_data.destination, 14 , 0);
  }
  else if (snp_mrr_tbl0_data.destination & ( 1 << 14))
  {
    info->cmd.dest_id.type = ARAD_DEST_TYPE_MULTICAST;
    info->cmd.dest_id.id = SOC_SAND_GET_BITS_RANGE(snp_mrr_tbl0_data.destination, 13 , 0);
  }
  else
  {
    arad_ports_logical_sys_id_parse(
		unit, 
      snp_mrr_tbl0_data.destination,
      &is_lag_not_phys,
      &lag_id,
      &lag_member_id,
      &sys_phys_port_id
    );
  
    if (is_lag_not_phys)
    {
      info->cmd.dest_id.type = ARAD_DEST_TYPE_LAG;
      info->cmd.dest_id.id = lag_id;
    }
    else
    {
      info->cmd.dest_id.type = ARAD_DEST_TYPE_SYS_PHY_PORT;
      info->cmd.dest_id.id = sys_phys_port_id;
    }
  }
  
  info->cmd.dp.value  = snp_mrr_tbl0_data.dp;
  info->cmd.dp.enable = SOC_SAND_NUM2BOOL(snp_mrr_tbl0_data.dp_ow);
  info->cmd.tc.value  = snp_mrr_tbl0_data.tc;
  info->cmd.tc.enable = SOC_SAND_NUM2BOOL(snp_mrr_tbl0_data.tc_ow);

  res = arad_irr_snoop_mirror_table1_tbl_get_unsafe(
          unit,
          ARAD_ACTION_CMD_MIRROR_TABLE_OFFSET + action_ndx,
          &snp_mrr_tbl1_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  info->cmd.counter_ptr_1.value  = snp_mrr_tbl1_data.counter_ptr0;
  info->cmd.counter_ptr_1.enable = SOC_SAND_NUM2BOOL(snp_mrr_tbl1_data.counter_ptr0_ow);
  info->cmd.counter_ptr_2.value  = snp_mrr_tbl1_data.counter_ptr1;
  info->cmd.counter_ptr_2.enable = SOC_SAND_NUM2BOOL(snp_mrr_tbl1_data.counter_ptr1_ow);
  info->cmd.meter_dp.value       = snp_mrr_tbl1_data.dp_cmd;
  info->cmd.meter_dp.enable      = SOC_SAND_NUM2BOOL(snp_mrr_tbl1_data.dp_cmd_ow);
  info->cmd.meter_ptr_low.value  = snp_mrr_tbl1_data.meter_ptr0;
  info->cmd.meter_ptr_low.enable = SOC_SAND_NUM2BOOL(snp_mrr_tbl1_data.meter_ptr0_ow);
  info->cmd.meter_ptr_up.value   = snp_mrr_tbl1_data.meter_ptr1;
  info->cmd.meter_ptr_up.enable  = SOC_SAND_NUM2BOOL(snp_mrr_tbl1_data.meter_ptr1_ow);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, ECI_GLOBAL_INGRESS_MULTICAST_INDICATIONr, REG_PORT_ANY, 0, SNOOP_IS_ING_MCf, &fld_val));
  info->cmd.is_ing_mc = SOC_SAND_NUM2BOOL(SOC_SAND_GET_BIT(fld_val,action_ndx));
  
  /* get snoop probability */
  res = arad_pp_ihb_snp_act_profile_tbl_get_unsafe(
          unit,
          action_ndx,
          &snp_act_profile_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  info->prob = snp_act_profile_tbl_data.sampling_probability;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_action_cmd_snoop_get_unsafe()", action_ndx, 0);
}

/*********************************************************************
*     Set a mirror action profile in the mirror action profile
 *     table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32 
  arad_action_cmd_mirror_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   action_ndx,
    SOC_SAND_IN  ARAD_ACTION_CMD_SNOOP_MIRROR_INFO   *info
  )
{
  uint32
    res = SOC_SAND_OK,
    fld_val,
    mem_entry[2],
    sys_fap_id_self,
    mapped_fap_port_id,
    mapped_fap_id;
   
  ARAD_IRR_SNOOP_MIRROR_TABLE0_TBL_DATA
    snp_mrr_tbl0_data;
  ARAD_IRR_SNOOP_MIRROR_TABLE1_TBL_DATA
    snp_mrr_tbl1_data;
  ARAD_PP_IHB_MRR_ACT_PROFILE_TBL_DATA
    mrr_act_profile_tbl_data;
  

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ACTION_CMD_MIRROR_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  ARAD_CLEAR(&snp_mrr_tbl0_data,ARAD_IRR_SNOOP_MIRROR_TABLE0_TBL_DATA,1);
  ARAD_CLEAR(&snp_mrr_tbl1_data,ARAD_IRR_SNOOP_MIRROR_TABLE1_TBL_DATA,1);
  ARAD_CLEAR(&mrr_act_profile_tbl_data,ARAD_PP_IHB_MRR_ACT_PROFILE_TBL_DATA,1);

    /* overwrite outlif */
  /* 0 << 5 as msb entries for Mirroring */
  res = READ_IPT_SNP_MIR_CMD_MAPm(unit, MEM_BLOCK_ANY, ARAD_ACTION_CMD_IPT_SNP_MIR_CMD_MAP_MIRROR_OFFSET | action_ndx, mem_entry);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 1, exit);

  soc_mem_field32_set(
          unit,
          IPT_SNP_MIR_CMD_MAPm,
          mem_entry,
          OUTLIFf,
          info->cmd.outlif.value);




  /* Encode snoop size */
  if ((SOC_IS_JERICHO(unit))) {
      irr_snoop_mirror_size_set(unit, action_ndx, IRR_MIRROR_SIZEr, MIRROR_SIZEf, ARAD_ACTION_CMD_SNOOP_SIZE_NOF_BITS, info->size, ARAD_ACTION_CMD_SNOOP_SIZE_OUT_OF_RANGE_ERR); 
      SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);
  }



  /* Encode destination type and id */
  switch(info->cmd.dest_id.type)
  {
  case ARAD_DEST_TYPE_QUEUE:
    snp_mrr_tbl0_data.destination = info->cmd.dest_id.id  == (uint32)(-1) ?
      (1 << 18) - 1 :                    /* mark to drop */
      info->cmd.dest_id.id | QUEUE_DEST_MASK;  /* regular queue / flow id */
    break;
  case ARAD_DEST_TYPE_MULTICAST:
	snp_mrr_tbl0_data.destination = info->cmd.dest_id.id | MULTICAST_DEST_MASK;
    snp_mrr_tbl0_data.destination = info->cmd.dest_id.id | ( 1 << 16);
    break;
  case ARAD_DEST_TYPE_SYS_PHY_PORT:
	snp_mrr_tbl0_data.destination = info->cmd.dest_id.id | SYS_PHY_PORT_DEST_MASK;

    /* Check if the port is local */
    res = arad_sys_phys_to_local_port_map_get_unsafe(unit, info->cmd.dest_id.id, &mapped_fap_id, &mapped_fap_port_id);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    res = arad_mgmt_system_fap_id_get_unsafe(
          unit,
          &sys_fap_id_self
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    if (sys_fap_id_self != mapped_fap_id) {
     uint32 destination; 
               
     /* Set IPT_SNP_MIR_CMD_MAP destination */
	destination = info->cmd.dest_id.id | SYS_PHY_PORT_DEST_MASK;

    soc_mem_field32_set(
          unit,
          IPT_SNP_MIR_CMD_MAPm,
          mem_entry,
          DSPf,
          destination);
      
    }
    break;

  case ARAD_DEST_TYPE_LAG:
	snp_mrr_tbl0_data.destination = info->cmd.dest_id.id | LAG_DEST_MASK;
    res = arad_ports_logical_sys_id_build(unit, TRUE, info->cmd.dest_id.id, 0, 0, &snp_mrr_tbl0_data.destination);
    SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(ARAD_ACTION_CMD_DEST_ID_OUT_OF_RANGE_ERR, 30, exit);
    break;
  }  

  res = WRITE_IPT_SNP_MIR_CMD_MAPm(unit, MEM_BLOCK_ANY, ARAD_ACTION_CMD_IPT_SNP_MIR_CMD_MAP_MIRROR_OFFSET | action_ndx, mem_entry);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 1, exit);

  snp_mrr_tbl0_data.dp          = info->cmd.dp.value;
  snp_mrr_tbl0_data.dp_ow       = SOC_SAND_BOOL2NUM(info->cmd.dp.enable);
  snp_mrr_tbl0_data.tc          = info->cmd.tc.value;
  snp_mrr_tbl0_data.tc_ow       = SOC_SAND_BOOL2NUM(info->cmd.tc.enable);

  /* After setting all parameters in struct, write it to HW*/
  res = arad_irr_snoop_mirror_table0_tbl_set_unsafe(
    unit,
    ARAD_ACTION_CMD_MIRROR_TABLE_OFFSET + action_ndx,
    &snp_mrr_tbl0_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  snp_mrr_tbl1_data.counter_ptr0    = info->cmd.counter_ptr_1.value;
  snp_mrr_tbl1_data.counter_ptr0_ow = SOC_SAND_BOOL2NUM(info->cmd.counter_ptr_1.enable);
  snp_mrr_tbl1_data.counter_ptr1    = info->cmd.counter_ptr_2.value;
  snp_mrr_tbl1_data.counter_ptr1_ow = SOC_SAND_BOOL2NUM(info->cmd.counter_ptr_2.enable);
  snp_mrr_tbl1_data.dp_cmd          = info->cmd.meter_dp.value;
  snp_mrr_tbl1_data.dp_cmd_ow       = SOC_SAND_BOOL2NUM(info->cmd.meter_dp.enable);
  snp_mrr_tbl1_data.meter_ptr0      = info->cmd.meter_ptr_low.value;
  snp_mrr_tbl1_data.meter_ptr0_ow   = SOC_SAND_BOOL2NUM(info->cmd.meter_ptr_low.enable);
  snp_mrr_tbl1_data.meter_ptr1      = info->cmd.meter_ptr_up.value;
  snp_mrr_tbl1_data.meter_ptr1_ow   = SOC_SAND_BOOL2NUM(info->cmd.meter_ptr_up.enable);

  /* After setting all parameters in struct, write it to HW*/
  res = arad_irr_snoop_mirror_table1_tbl_set_unsafe(
    unit,
    ARAD_ACTION_CMD_SNOOP_TABLE_OFFSET + action_ndx, /* The index of Table1 are inverted with table0 */
    &snp_mrr_tbl1_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, ECI_GLOBAL_INGRESS_MULTICAST_INDICATIONr, REG_PORT_ANY, 0, MIRROR_IS_ING_MCf, &fld_val));
  SOC_SAND_SET_BIT(fld_val,SOC_SAND_NUM2BOOL(info->cmd.is_ing_mc), action_ndx);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_INGRESS_MULTICAST_INDICATIONr, REG_PORT_ANY, 0, MIRROR_IS_ING_MCf,  fld_val));

  /* Set mirror probability */
  mrr_act_profile_tbl_data.sampling_probability = info->prob;

  res = arad_pp_ihb_mrr_act_profile_tbl_set_unsafe(
          unit,
          action_ndx,
          &mrr_act_profile_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  res = ipt_arad_snp_mir_stamp_tables_set(unit,ARAD_ACTION_CMD_MIRROR_TABLE_OFFSET + action_ndx,NULL);
  SOC_SAND_CHECK_FUNC_RESULT(res, 71, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_action_cmd_mirror_set_unsafe()", action_ndx, 0);
}

uint32
  arad_action_cmd_mirror_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   action_ndx,
    SOC_SAND_IN  ARAD_ACTION_CMD_SNOOP_MIRROR_INFO   *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ACTION_CMD_MIRROR_SET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(action_ndx, ARAD_ACTION_NDX_MIN, ARAD_ACTION_NDX_MAX, ARAD_ACTION_NDX_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_STRUCT_VERIFY(ARAD_ACTION_CMD_MIRROR_INFO, info, 20, exit);

  /* IMPLEMENTED */

  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_action_cmd_mirror_set_verify()", action_ndx, 0);
}

uint32
  arad_action_cmd_mirror_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   action_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ACTION_CMD_MIRROR_GET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(action_ndx, ARAD_ACTION_NDX_MIN, ARAD_ACTION_NDX_MAX, ARAD_ACTION_NDX_OUT_OF_RANGE_ERR, 10, exit);

  /* IMPLEMENTED */

  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_action_cmd_mirror_get_verify()", action_ndx, 0);
}

/*********************************************************************
*     Set a mirror action profile in the mirror action profile
 *     table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_action_cmd_mirror_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   action_ndx,
    SOC_SAND_OUT ARAD_ACTION_CMD_SNOOP_MIRROR_INFO   *info
  )
{
  uint32
    res = SOC_SAND_OK,
    fld_val,
    lag_id,
    lag_member_id,
    mem_entry[2],
    sys_phys_port_id;
   
    
  ARAD_IRR_SNOOP_MIRROR_TABLE0_TBL_DATA
    snp_mrr_tbl0_data;
  ARAD_IRR_SNOOP_MIRROR_TABLE1_TBL_DATA
    snp_mrr_tbl1_data;
  ARAD_PP_IHB_MRR_ACT_PROFILE_TBL_DATA
    mrr_act_profile_tbl_data;
  uint8
    is_lag_not_phys;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ACTION_CMD_MIRROR_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  ARAD_ACTION_CMD_MIRROR_INFO_clear(info);

  ARAD_CLEAR(&snp_mrr_tbl0_data,ARAD_IRR_SNOOP_MIRROR_TABLE0_TBL_DATA,1);
  ARAD_CLEAR(&snp_mrr_tbl1_data,ARAD_IRR_SNOOP_MIRROR_TABLE1_TBL_DATA,1);
  ARAD_CLEAR(&mrr_act_profile_tbl_data,ARAD_PP_IHB_MRR_ACT_PROFILE_TBL_DATA,1);

 /* 0 << 5 as lsb entries for Mirroring */
  res = READ_IPT_SNP_MIR_CMD_MAPm(unit, MEM_BLOCK_ANY,ARAD_ACTION_CMD_IPT_SNP_MIR_CMD_MAP_MIRROR_OFFSET | action_ndx, mem_entry);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 1, exit);

  info->cmd.outlif.value = 
      soc_mem_field32_get(
          unit,
          IPT_SNP_MIR_CMD_MAPm,
          mem_entry,
          OUTLIFf);
  info->cmd.outlif.enable = info->cmd.outlif.value ? 1 : 0;

  res = arad_irr_snoop_mirror_table0_tbl_get_unsafe(
    unit,
    ARAD_ACTION_CMD_MIRROR_TABLE_OFFSET + action_ndx,
    &snp_mrr_tbl0_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /* Encode destination type and id */
  if (snp_mrr_tbl0_data.destination & ( 1 << 17))
  {
    info->cmd.dest_id.type = ARAD_DEST_TYPE_QUEUE;
    info->cmd.dest_id.id = SOC_SAND_GET_BITS_RANGE(snp_mrr_tbl0_data.destination, 16 , 0);
  }
  else if (snp_mrr_tbl0_data.destination & ( 1 << 16))
  {
    info->cmd.dest_id.type = ARAD_DEST_TYPE_MULTICAST;
    info->cmd.dest_id.id = SOC_SAND_GET_BITS_RANGE(snp_mrr_tbl0_data.destination, 15 , 0);
  }
  else
  {
    arad_ports_logical_sys_id_parse(
		unit, 
      snp_mrr_tbl0_data.destination,
      &is_lag_not_phys,
      &lag_id,
      &lag_member_id,
      &sys_phys_port_id
      );

    if (is_lag_not_phys)
    {
      info->cmd.dest_id.type = ARAD_DEST_TYPE_LAG;
      info->cmd.dest_id.id = lag_id;
    }
    else
    {
      info->cmd.dest_id.type = ARAD_DEST_TYPE_SYS_PHY_PORT;
      info->cmd.dest_id.id = sys_phys_port_id;
    }
  }

  info->cmd.dp.value  = snp_mrr_tbl0_data.dp;
  info->cmd.dp.enable = SOC_SAND_NUM2BOOL(snp_mrr_tbl0_data.dp_ow);
  info->cmd.tc.value  = snp_mrr_tbl0_data.tc;
  info->cmd.tc.enable = SOC_SAND_NUM2BOOL(snp_mrr_tbl0_data.tc_ow);

  res = arad_irr_snoop_mirror_table1_tbl_get_unsafe(
    unit,
    ARAD_ACTION_CMD_SNOOP_TABLE_OFFSET + action_ndx,
    &snp_mrr_tbl1_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  info->cmd.counter_ptr_1.value  = snp_mrr_tbl1_data.counter_ptr0;
  info->cmd.counter_ptr_1.enable = SOC_SAND_NUM2BOOL(snp_mrr_tbl1_data.counter_ptr0_ow);
  info->cmd.counter_ptr_2.value  = snp_mrr_tbl1_data.counter_ptr1;
  info->cmd.counter_ptr_2.enable = SOC_SAND_NUM2BOOL(snp_mrr_tbl1_data.counter_ptr1_ow);
  info->cmd.meter_dp.value       = snp_mrr_tbl1_data.dp_cmd;
  info->cmd.meter_dp.enable      = SOC_SAND_NUM2BOOL(snp_mrr_tbl1_data.dp_cmd_ow);
  info->cmd.meter_ptr_low.value  = snp_mrr_tbl1_data.meter_ptr0;
  info->cmd.meter_ptr_low.enable = SOC_SAND_NUM2BOOL(snp_mrr_tbl1_data.meter_ptr0_ow);
  info->cmd.meter_ptr_up.value   = snp_mrr_tbl1_data.meter_ptr1;
  info->cmd.meter_ptr_up.enable  = SOC_SAND_NUM2BOOL(snp_mrr_tbl1_data.meter_ptr1_ow);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, ECI_GLOBAL_INGRESS_MULTICAST_INDICATIONr, REG_PORT_ANY, 0, MIRROR_IS_ING_MCf, &fld_val));
  info->cmd.is_ing_mc = SOC_SAND_NUM2BOOL(SOC_SAND_GET_BIT(fld_val, action_ndx));

  /* Get mirror probablity */
  res = arad_pp_ihb_mrr_act_profile_tbl_get_unsafe(
    unit,
    action_ndx,
    &mrr_act_profile_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  
  info->prob = mrr_act_profile_tbl_data.sampling_probability;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_action_cmd_mirror_get_unsafe()", action_ndx, 0);
}

#if ARAD_DEBUG_IS_LVL1

uint32
  ARAD_ACTION_CMD_OVERRIDE_verify(
    SOC_SAND_IN  ARAD_ACTION_CMD_OVERRIDE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_ACTION_CMD_OVERRIDE_verify()",0,0);
}

uint32
  ARAD_ACTION_CMD_verify(
    SOC_SAND_IN  ARAD_ACTION_CMD *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  
  ARAD_STRUCT_VERIFY(ARAD_ACTION_CMD_OVERRIDE, &(info->tc), 11, exit);
  ARAD_STRUCT_VERIFY(ARAD_ACTION_CMD_OVERRIDE, &(info->dp), 12, exit);
  ARAD_STRUCT_VERIFY(ARAD_ACTION_CMD_OVERRIDE, &(info->meter_ptr_low), 13, exit);
  ARAD_STRUCT_VERIFY(ARAD_ACTION_CMD_OVERRIDE, &(info->meter_ptr_up), 14, exit);
  ARAD_STRUCT_VERIFY(ARAD_ACTION_CMD_OVERRIDE, &(info->meter_dp), 15, exit);
  ARAD_STRUCT_VERIFY(ARAD_ACTION_CMD_OVERRIDE, &(info->counter_ptr_1), 16, exit);
  ARAD_STRUCT_VERIFY(ARAD_ACTION_CMD_OVERRIDE, &(info->counter_ptr_2), 17, exit);
  
  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_ACTION_CMD_verify()",0,0);
}

uint32
  ARAD_ACTION_CMD_SNOOP_INFO_verify(
    SOC_SAND_IN  ARAD_ACTION_CMD_SNOOP_MIRROR_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  ARAD_STRUCT_VERIFY(ARAD_ACTION_CMD, &(info->cmd), 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->size, ARAD_ACTION_CMD_SNOOP_INFO_SNOOP_SIZE_MAX, ARAD_ACTION_SNOOP_SIZE_OUT_OF_RANGE_ERR, 11, exit);
  /* ARAD_ACTION_CMD_SNOOP_INFO_SNOOP_PROB_MIN may be changed and be grater than 0*/
  /* coverity[unsigned_compare : FALSE] */
  SOC_SAND_ERR_IF_OUT_OF_RANGE(info->prob, ARAD_ACTION_CMD_SNOOP_INFO_SNOOP_PROB_MIN, ARAD_ACTION_CMD_SNOOP_INFO_SNOOP_PROB_MAX, ARAD_ACTION_SNOOP_PROB_OUT_OF_RANGE_ERR, 15, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_ACTION_CMD_SNOOP_INFO_verify()",0,0);
}

uint32
  ARAD_ACTION_CMD_MIRROR_INFO_verify(
    SOC_SAND_IN  ARAD_ACTION_CMD_SNOOP_MIRROR_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  ARAD_STRUCT_VERIFY(ARAD_ACTION_CMD, &(info->cmd), 10, exit);
  /* ARAD_ACTION_CMD_MIRROR_INFO_MIRROR_PROB_MIN may be changed and be grater than 0*/
  /* coverity[unsigned_compare : FALSE] */
  SOC_SAND_ERR_IF_OUT_OF_RANGE(info->prob, ARAD_ACTION_CMD_MIRROR_INFO_MIRROR_PROB_MIN, ARAD_ACTION_CMD_MIRROR_INFO_MIRROR_PROB_MAX, ARAD_ACTION_MIRROR_PROB_OUT_OF_RANGE_ERR, 15, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_ACTION_CMD_MIRROR_INFO_verify()",0,0);
}

#endif /* ARAD_DEBUG_IS_LVL1 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif /* of #if defined(BCM_88650_A0) */

