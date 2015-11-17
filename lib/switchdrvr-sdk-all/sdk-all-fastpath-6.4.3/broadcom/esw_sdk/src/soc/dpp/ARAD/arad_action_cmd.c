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
#include <soc/dpp/drv.h>
#include <soc/dpp/mbcm.h> 

/* } */
/*************
 * DEFINES   *
 *************/
/* { */
#define SNOOP_MIRROR_MACRO_CHOOSER(cmd_type,mirror_macro,snoop_macro)  (cmd_type==SOC_TMC_CMD_TYPE_MIRROR ? mirror_macro : snoop_macro)    

#define ARAD_ACTION_CMD_SNOOP_INFO_SNOOP_SIZE_MAX                (ARAD_ACTION_NOF_CMD_SIZE_BYTESS-1)
#define ARAD_ACTION_CMD_SNOOP_INFO_SNOOP_PROB_MAX                (1023)
#define ARAD_ACTION_CMD_SNOOP_INFO_SNOOP_PROB_MIN                (0)
#define ARAD_ACTION_CMD_MIRROR_INFO_MIRROR_PROB_MAX              (0xffffffff)
#define ARAD_ACTION_CMD_MIRROR_INFO_MIRROR_PROB_MIN              (0)

#define ARAD_ACTION_CMD_SNOOP_SIZE_NOF_BITS                      (2)


/*  choose the correct ofsett in snoop/mirror dest/tm tables  according to the target type(snoop or mirro)*/
#define ARAD_ACTION_CMD_SNOOP_MIRROR_TABLE_OFFSET(cmd_type)				 ( SNOOP_MIRROR_MACRO_CHOOSER(cmd_type,(SOC_DPP_IMP_DEFS_GET(unit,mirror_snoop_dest_tm_table_offset)), (16 - (SOC_DPP_IMP_DEFS_GET(unit,mirror_snoop_dest_tm_table_offset)))))
/*
SNP_MIR_CMD_MAP should contain 16 entries for mirroring and 16 for snoop. 
In Arad, this table is 64 entries large. This bug was fixed in Jericho.
*/
#define ARAD_ACTION_CMD_IPT_SNP_MIR_CMD_MAP_SNOOP_MIRROR_OFFSET(cmd_type)  ( SNOOP_MIRROR_MACRO_CHOOSER(cmd_type,0,(SOC_DPP_IMP_DEFS_GET(unit,cmd_ipt_snp_mir_cmd_map_snoop_offset))))

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



/*
*  given an encoding destination  &  destination type return TRUE if the encoding destination encode
*  as destination type, else return FALSE
*  PARAMETERS:
*  destination - the encoded destination get from the HW
*  type - is one of the INGRESS_DESTINATION_TYPE_MULTICAST, INGRESS_DESTINATION_TYPE_QUEUE, INGRESS_DESTINATION_TYPE_SYS_PHY_PORT, INGRESS_DESTINATION_TYPE_LAG
*/

STATIC 
uint8
 is_destination_of_type(
	  SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  uint32                destination,
      SOC_SAND_IN  uint32                type


	  )
{

	return (type & 0xff) == destination>>((type >>8) &0xff);
}


/*
 * check if other fap id which is the destination fap is actually the local fap
*/

STATIC 
uint8
 is_fap_id_in_local_device(
	  SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  uint32                local_fap_id,
      SOC_SAND_IN  uint32                other_fap_id
	  )
{
	return (other_fap_id >= local_fap_id && other_fap_id < local_fap_id + SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) ? 1 : 0;

}


/* decoding the comand size*/

STATIC 
uint32
 decoding_command_size(
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


/* encoding the comand size*/

STATIC 
uint32
 encoding_command_size(
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



/*
* decode the destination got from the underlying chip (arad,jericho,...) into id and the dest type
*/
STATIC void dpp_decode_destination_type_and_id(
     SOC_SAND_IN  int             unit,
     SOC_SAND_OUT   SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO    *info,
     SOC_SAND_IN uint32 destination
     )
{
    if (is_destination_of_type(unit,destination ,SOC_DPP_IMP_DEFS_GET(unit,mirror_snoop_destination_queue_encoding)))
    {
      info->cmd.dest_id.type = ARAD_DEST_TYPE_QUEUE;
      info->cmd.dest_id.id = destination & ~INGRESS_DESTINATION_TYPE_QUEUE;    /*  SOC_SAND_GET_BITS_RANGE(destination, 14 , 0); */
    }
    else if (is_destination_of_type(unit,destination ,SOC_DPP_IMP_DEFS_GET(unit,mirror_snoop_destination_multicast_encoding)))
    {
      info->cmd.dest_id.type = ARAD_DEST_TYPE_MULTICAST;
      info->cmd.dest_id.id =  destination & ~INGRESS_DESTINATION_TYPE_MULTICAST;    /* SOC_SAND_GET_BITS_RANGE(destination, 13 , 0); */
    }
    else if (is_destination_of_type(unit,destination ,SOC_DPP_IMP_DEFS_GET(unit,mirror_snoop_destination_lag_encoding)))
    {
        info->cmd.dest_id.type = INGRESS_DESTINATION_TYPE_LAG;
        info->cmd.dest_id.id = destination & ~INGRESS_DESTINATION_TYPE_LAG;    /*  SOC_SAND_GET_BITS_RANGE(destination, 14 , 0); */
    }
    else {
      info->cmd.dest_id.type = ARAD_DEST_TYPE_SYS_PHY_PORT;
      info->cmd.dest_id.id = destination & ~INGRESS_DESTINATION_TYPE_SYS_PHY_PORT;    /*  SOC_SAND_GET_BITS_RANGE(destination, 14 , 0); */
    }
}

/*
* encode the id and the dest type into destination in  format understood by the underlying chip(arad,jericho,....
*/
STATIC uint32 dpp_encode_destination_type_and_id(
     SOC_SAND_IN  int             unit,
     SOC_SAND_IN   SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO    *info,
     SOC_SAND_OUT uint32 *destination
     )
{

    SOCDNX_INIT_FUNC_DEFS;

    switch(info->cmd.dest_id.type)
    {
    case ARAD_DEST_TYPE_QUEUE:
      *destination = info->cmd.dest_id.id  == (uint32)(-1) ?
        (1 << 18) - 1 :                    /* mark to drop */
        info->cmd.dest_id.id | INGRESS_DESTINATION_TYPE_QUEUE;  /* regular queue / flow id */
      break;
    case ARAD_DEST_TYPE_MULTICAST:
      *destination = info->cmd.dest_id.id | INGRESS_DESTINATION_TYPE_MULTICAST;
      break;
    case ARAD_DEST_TYPE_SYS_PHY_PORT:
      *destination = info->cmd.dest_id.id | INGRESS_DESTINATION_TYPE_SYS_PHY_PORT;

      break;

    case ARAD_DEST_TYPE_LAG:
      *destination = info->cmd.dest_id.id | INGRESS_DESTINATION_TYPE_LAG;
      break;
    default:
      SOCDNX_EXIT_WITH_ERR(SOC_E_BADID, (_BSL_SOCDNX_MSG("mirror destination type illegal\n")));
      break;
    }  

exit:
    SOCDNX_FUNC_RETURN;
}
  

/*
 * config  the destination  and the tm parameters for a mirrrored destination
*/

STATIC
uint32
  dpp_snoop_mirror_destination_config_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32          entry_offset,
    SOC_SAND_IN   SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO    *info
  )
{


  uint32
    res = SOC_SAND_OK;
  uint32 data[] = {0,0,0,0,0,0,0,0,0,0,0};

  soc_mem_t mem=  IRR_SNOOP_MIRROR_DEST_TABLEm;
  soc_field_t field;
  uint32 destination;

  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_IF_ERR_EXIT(dpp_encode_destination_type_and_id(unit, info, &destination));


    soc_mem_field32_set( unit, mem, data,  DESTINATIONf, destination);
    soc_mem_field32_set(unit, mem, data, TCf, info->cmd.tc.value );
    soc_mem_field32_set(unit, mem, data, TC_OWf, (uint32)info->cmd.tc.enable );
    soc_mem_field32_set(unit, mem, data, DPf, info->cmd.dp.value );
    soc_mem_field32_set(unit, mem, data, DP_OWf, (uint32)info->cmd.dp.enable );
    res = soc_mem_write(unit, mem, MEM_BLOCK_ANY, entry_offset, data);
    SOCDNX_IF_ERR_EXIT(res);
    

    mem= SOC_IS_JERICHO(unit) ? IRR_SNOOP_MIRROR_TM_TABLEm : IRR_SNOOP_MIRROR_TABLE_1m;
    sal_memset(data, 0,sizeof(data));
    soc_mem_field32_set(unit, mem, data, COUNTER_PTR_0f, info->cmd.counter_ptr_1.value );
    soc_mem_field32_set(unit, mem, data, COUNTER_PTR_0_OWf, (uint32)info->cmd.counter_ptr_1.enable );
  /* Perform the same between OW and update */
    soc_mem_field32_set(unit, mem, data, COUNTER_PTR_0_UPDATEf, (uint32)info->cmd.counter_ptr_1.enable );
    soc_mem_field32_set(unit, mem, data, COUNTER_PTR_1f, info->cmd.counter_ptr_2.value );
    soc_mem_field32_set(unit, mem, data, COUNTER_PTR_1_OWf, (uint32)info->cmd.counter_ptr_2.enable );
  /* Perform the same between OW and update */
    soc_mem_field32_set(unit, mem, data, COUNTER_PTR_1_UPDATEf, info->cmd.counter_ptr_2.enable );

    

    /* from here just jer*/

    if (SOC_IS_JERICHO(unit)) {
    /* Header delta size. */
        field = HEADER_DELTAf ;/*field size in bits (8) */
        soc_mem_field32_set(unit, mem, data, field, (uint32)0);

    /* In PP port. */
        field = IN_PP_PORTf ;/*field size in bits (8) */
        soc_mem_field32_set(unit, mem, data, field, (uint32)0);
        


    /* Statistics Vsq pointer. */
        field = ST_VSQ_PTRf ;/*field size in bits (8) */
        soc_mem_field32_set(unit, mem, data, field, (uint32)0);
        

    /* If set then Header delta size is overwritten. */
        field = HEADER_DELTA_OWf ;/*field size in bits (1) */
        soc_mem_field32_set(unit, mem, data, field, (uint32)0);
        

    /* If set then In PP port is overwritten. */
        field = IN_PP_PORT_OWf ;/*field size in bits (1) */
        soc_mem_field32_set(unit, mem, data, field, (uint32)0);
        

    /* Ignore CP. */
        field = IGNORE_CPf ;/*field size in bits (1) */
        soc_mem_field32_set(unit, mem, data, field, (uint32)0);
        

    /* If set then Ethernet encapsulation is overwritten. */
        field = ETH_ENCAPSULATION_OWf ;/*field size in bits (1) */
        soc_mem_field32_set(unit, mem, data, field, (uint32)0);
        

    /* LAG load-balancing key. */
        field = LAG_LB_KEYf ;/*field size in bits (16) */
        soc_mem_field32_set(unit, mem, data, field, (uint32)0);
        

    /* Ethernet encapsulation. */
        field = ETH_ENCAPSULATIONf ;/*field size in bits (2) */
        soc_mem_field32_set(unit, mem, data, field, (uint32)0);
        

    /* If set then Header truncate size is overwritten. */
        field = HEADER_TRUNCATE_OWf ;/*field size in bits (1) */
        soc_mem_field32_set(unit, mem, data, field, (uint32)0);
        

    /* If set then Statistics Vsq pointer is overwritten. */
        field = ST_VSQ_PTR_OWf ;/*field size in bits (1) */
        soc_mem_field32_set(unit, mem, data, field, (uint32)0);
        

    /* If set then Header append size is overwritten. */
        field = HEADER_APPEND_OWf ;/*field size in bits (1) */
        soc_mem_field32_set(unit, mem, data, field, (uint32)0);
        

    /* Header truncate size. */
        field = HEADER_TRUNCATEf ;/*field size in bits (8) */
        soc_mem_field32_set(unit, mem, data, field, (uint32)0);
        

    /* Header append size. */
        field = HEADER_APPENDf ;/*field size in bits (8) */
        soc_mem_field32_set(unit, mem, data, field, (uint32)0);
        

    /* If set then Ignore CP is overwritten. */
        field = IGNORE_CP_OWf ;/*field size in bits (1) */
        soc_mem_field32_set(unit, mem, data, field, (uint32)0);
        

    /* If set then LAG load-balancing key is overwritten. */
        field = LAG_LB_KEY_OWf ;/*field size in bits (1) */
        soc_mem_field32_set(unit, mem, data, field, (uint32)0);
        
    }



    SOCDNX_IF_ERR_EXIT( soc_mem_write(unit, mem, MEM_BLOCK_ANY,  entry_offset,data));

    if (SOC_IS_JERICHO(unit)) {
        SOCDNX_IF_ERR_EXIT(READ_EPNI_REC_CMD_CONFm(unit, MEM_BLOCK_ANY, entry_offset, data));
        soc_mem_field32_set(unit, EPNI_REC_CMD_CONFm, data, MIRROR_PROBABILITYf, 0xffffffff);
        SOCDNX_IF_ERR_EXIT(WRITE_EPNI_REC_CMD_CONFm(unit,  MEM_BLOCK_ANY, entry_offset, data));


    }

#ifdef JERICHO_SPECIFRIC_MIRROR_SUPPORT
    if (SOC_IS_JERICHO(unit)) {


        memory = IPT_ARAD_SNP_MIR_STAMP_ENm ;
    /* If the extension doesn't exist, this field is ignored. */
        field = OAM_TS_EXT_ENf ;/*field size in bits (48) */
        soc_mem_field_set(unit, memory, data, field, value);

            /* If PPH doesn't exist, this field is ignored. */
        field = PPH_BASE_ENf ;/*field size in bits (56) */
        soc_mem_field_set(unit, memory, data, field, value);

            /* If the extension doesn't exist, this field is ignored. */
        field = FTMH_STACK_EXT_ENf ;/*field size in bits (16) */
        soc_mem_field32_set(unit, memory, data, field, value[0]);

            /* If the extension doesn't exist, this field is ignored. */
        field = PPH_FHEI_EXT_ENf ;/*field size in bits (40) */
        soc_mem_field_set(unit, memory, data, field, value);

            /* If the extension doesn't exist, this field is ignored. */
        field = PPH_EEI_EXT_ENf ;/*field size in bits (24) */
        soc_mem_field32_set(unit, memory, data, field, value[0]);

            /* If the extension doesn't exist, this field is ignored. */
        field = FTMH_DSP_EXT_ENf ;/*field size in bits (16) */
        soc_mem_field32_set(unit, memory, data, field, value[0]);

            /*  */
        field = FTMH_BASE_ENf ;/*field size in bits (72) */
        soc_mem_field_set(unit, memory, data, field, value);

            /* If the extension doesn't exist, this field is ignored. */
        field = FTMH_LB_EXT_ENf ;/*field size in bits (8) */
        soc_mem_field32_set(unit, memory, data, field, value[0]);

            /* If the extension doesn't exist, this field is ignored. */
        field = PPH_LEARN_EXT_ENf ;/*field size in bits (40) */
        soc_mem_field_set(unit, memory, data, field, value);


        SOCDNX_IF_ERR_EXIT(WRITE_IPT_ARAD_SNP_MIR_STAMP_ENm(unit, MEM_BLOCK_ANY, action_ndx, data));




        memory = IPT_ARAD_SNP_MIR_STAMP_VALUESm ;
    /* if the extension existed in the original packet, then this extension will be stamped (if enabled to). If the extension doesn't exist, this field is ignored. */
        field = FTMH_STACK_EXTf ;/*field size in bits (16) */
        soc_mem_field32_set(unit, memory, data, field, value[0]);

            /* If the extension existed in the original packet, then this extension will be stamped (if enabled to). If the extension doesn't exist, this field is ignored. */
        field = PPH_EEI_EXTf ;/*field size in bits (24) */
        soc_mem_field32_set(unit, memory, data, field, value[0]);

            /*  */
        field = FTMH_BASEf ;/*field size in bits (72) */
        soc_mem_field_set(unit, memory, data, field, value);

            /* if the extension existed in the original packet, then this extension will be stamped (if enabled to). If the extension doesn't exist, this field is ignored. */
        field = FTMH_DSP_EXTf ;/*field size in bits (16) */
        soc_mem_field32_set(unit, memory, data, field, value[0]);

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
        soc_mem_field32_set(unit, memory, data, field, value[0]);

            /* If the PPH existed in the original packet, then this header will be stamped (if enabled to). If the PPH doesn't exist, this field is ignored. */
        field = PPH_BASEf ;/*field size in bits (56) */
        soc_mem_field_set(unit, memory, data, field, value);


        SOCDNX_IF_ERR_EXIT(WRITE_IPT_ARAD_SNP_MIR_STAMP_VALUESm(unit, MEM_BLOCK_ANY, action_ndx, data));
    }
#endif


exit:
    SOCDNX_FUNC_RETURN;
}


/*
 * get the configuration  destination and tm parameters  for mirrrored destination
*/


STATIC
uint32
  dpp_snoop_mirror_destination_config_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32          entry_offset,
    SOC_SAND_OUT   ARAD_ACTION_CMD_SNOOP_MIRROR_INFO    *info
  )
{
    uint32
      res = SOC_SAND_OK;
    uint32
      data[4]={0,0,0,0};
    soc_mem_t mem=  IRR_SNOOP_MIRROR_DEST_TABLEm;
    uint32 destination;


    SOCDNX_INIT_FUNC_DEFS;




    res = soc_mem_read(unit, mem, MEM_BLOCK_ANY, entry_offset, data);
    SOCDNX_IF_ERR_EXIT(res);
    destination   = soc_mem_field32_get(unit, mem, data, DESTINATIONf);
    dpp_decode_destination_type_and_id(unit,info,destination);

    info->cmd.tc.value   = soc_mem_field32_get(unit, mem, data, TCf);
    info->cmd.tc.enable   = SOC_SAND_NUM2BOOL(soc_mem_field32_get(unit, mem, data, TC_OWf));
    info->cmd.dp.value   = soc_mem_field32_get(unit, mem, data, DPf);
    info->cmd.dp.enable   = SOC_SAND_NUM2BOOL(soc_mem_field32_get(unit, mem, data, DP_OWf));

    mem= SOC_IS_JERICHO(unit) ? IRR_SNOOP_MIRROR_TM_TABLEm : IRR_SNOOP_MIRROR_TABLE_1m;

    sal_memset(data, 0,sizeof(data));

    res = soc_mem_read(unit, mem, MEM_BLOCK_ANY, entry_offset, data);

    SOCDNX_IF_ERR_EXIT(res);

    info->cmd.counter_ptr_1.value   = soc_mem_field32_get(unit, mem, data, COUNTER_PTR_0f);
    info->cmd.counter_ptr_1.enable = SOC_SAND_NUM2BOOL(soc_mem_field32_get(unit, mem, data, COUNTER_PTR_0_OWf));
    info->cmd.counter_ptr_2.value   = soc_mem_field32_get(unit, mem, data, COUNTER_PTR_1f);
    info->cmd.counter_ptr_2.enable = SOC_SAND_NUM2BOOL(soc_mem_field32_get(unit, mem, data, COUNTER_PTR_1_OWf));




exit:
    SOCDNX_FUNC_RETURN;
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
    SOC_SAND_IN  ARAD_ACTION_CMD_SNOOP_MIRROR_INFO    *info,
    SOC_SAND_IN  SOC_TMC_CMD_TYPE                     cmd_type
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
  res = READ_IPT_SNP_MIR_CMD_MAPm(unit, MEM_BLOCK_ANY,  ARAD_ACTION_CMD_IPT_SNP_MIR_CMD_MAP_SNOOP_MIRROR_OFFSET(cmd_type) | action_ndx, mem_entry);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 1, exit);

  soc_mem_field32_set(
          unit,
          IPT_SNP_MIR_CMD_MAPm,
          mem_entry,
          OUTLIFf,
          info->cmd.outlif.value);
  

  res = WRITE_IPT_SNP_MIR_CMD_MAPm(unit, MEM_BLOCK_ANY,  ARAD_ACTION_CMD_IPT_SNP_MIR_CMD_MAP_SNOOP_MIRROR_OFFSET(cmd_type) | action_ndx, mem_entry);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 1, exit);


  /* Encode snoop size */
  encoding_command_size(unit,action_ndx,IRR_SNOOP_SIZEr,SNOOP_SIZEf,ARAD_ACTION_CMD_SNOOP_SIZE_NOF_BITS,info->size,ARAD_ACTION_CMD_SNOOP_SIZE_OUT_OF_RANGE_ERR);
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
          ARAD_ACTION_CMD_SNOOP_MIRROR_TABLE_OFFSET(cmd_type) + action_ndx,
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
          ARAD_ACTION_CMD_SNOOP_MIRROR_TABLE_OFFSET(cmd_type) + action_ndx,
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
    SOC_SAND_OUT ARAD_ACTION_CMD_SNOOP_MIRROR_INFO    *info,
    SOC_SAND_IN  SOC_TMC_CMD_TYPE                     cmd_type
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
  res = READ_IPT_SNP_MIR_CMD_MAPm(unit, MEM_BLOCK_ANY,  ARAD_ACTION_CMD_IPT_SNP_MIR_CMD_MAP_SNOOP_MIRROR_OFFSET(cmd_type) | action_ndx, mem_entry);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 1, exit);

  info->cmd.outlif.value = 
      soc_mem_field32_get(
          unit,
          IPT_SNP_MIR_CMD_MAPm,
          mem_entry,
          OUTLIFf);
  info->cmd.outlif.enable = 1;
  
  /* Encode snoop size */

  res=decoding_command_size(unit,action_ndx, IRR_SNOOP_SIZEr,SNOOP_SIZEf,&(info->size),ARAD_ACTION_CMD_SNOOP_SIZE_OUT_OF_RANGE_ERR);
  SOC_SAND_CHECK_FUNC_RESULT(res, 2, exit);

  res = arad_irr_snoop_mirror_table0_tbl_get_unsafe(
          unit,
          ARAD_ACTION_CMD_SNOOP_MIRROR_TABLE_OFFSET(cmd_type) + action_ndx,
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
          ARAD_ACTION_CMD_SNOOP_MIRROR_TABLE_OFFSET(cmd_type) + action_ndx,
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
    SOC_SAND_IN  ARAD_ACTION_CMD_SNOOP_MIRROR_INFO   *info,
    SOC_SAND_IN  SOC_TMC_CMD_TYPE                    cmd_type
  )
{
  uint32
    res = SOC_SAND_OK,
    fld_val,
    mem_entry[2],
    sys_fap_id_self,
    mapped_fap_port_id,
    mapped_fap_id;
   
  ARAD_PP_IHB_MRR_ACT_PROFILE_TBL_DATA
    mrr_act_profile_tbl_data;
  

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ACTION_CMD_MIRROR_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  ARAD_CLEAR(&mrr_act_profile_tbl_data,ARAD_PP_IHB_MRR_ACT_PROFILE_TBL_DATA,1);

    /* overwrite outlif */
  /* 0 << 5 as msb entries for Mirroring */
  res = READ_IPT_SNP_MIR_CMD_MAPm(unit, MEM_BLOCK_ANY, ARAD_ACTION_CMD_IPT_SNP_MIR_CMD_MAP_SNOOP_MIRROR_OFFSET(cmd_type) | action_ndx, mem_entry);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 1, exit);

  soc_mem_field32_set(
          unit,
          IPT_SNP_MIR_CMD_MAPm,
          mem_entry,
          OUTLIFf,
          info->cmd.outlif.value);




  /* Encode mirror size */
  if ((SOC_IS_JERICHO(unit))) {
      encoding_command_size(unit, action_ndx, IRR_MIRROR_SIZEr, MIRROR_SIZEf, ARAD_ACTION_CMD_SNOOP_SIZE_NOF_BITS, info->size, ARAD_ACTION_CMD_SNOOP_SIZE_OUT_OF_RANGE_ERR); 
      SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);
  }



  /* Encode destination type and id */

  if (info->cmd.dest_id.type== ARAD_DEST_TYPE_SYS_PHY_PORT) {
      /* Check if the port is local */
      res = arad_sys_phys_to_local_port_map_get_unsafe(unit, info->cmd.dest_id.id, &mapped_fap_id, &mapped_fap_port_id);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mgmt_system_fap_id_get, (unit, &sys_fap_id_self)));


      if (!is_fap_id_in_local_device(unit, sys_fap_id_self, mapped_fap_id)) {
          soc_mem_field32_set(
              unit,
              IPT_SNP_MIR_CMD_MAPm,
              mem_entry,
              DSPf,
              info->cmd.dest_id.id);
      }
  }

  res = WRITE_IPT_SNP_MIR_CMD_MAPm(unit, MEM_BLOCK_ANY, ARAD_ACTION_CMD_IPT_SNP_MIR_CMD_MAP_SNOOP_MIRROR_OFFSET(cmd_type) | action_ndx, mem_entry);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 1, exit);
 
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR ,dpp_snoop_mirror_destination_config_set(unit, ARAD_ACTION_CMD_SNOOP_MIRROR_TABLE_OFFSET(cmd_type) + action_ndx ,info)); 
 


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
    SOC_SAND_OUT ARAD_ACTION_CMD_SNOOP_MIRROR_INFO   *info,
    SOC_SAND_IN  SOC_TMC_CMD_TYPE                    cmd_type
  )
{
  uint32
    res = SOC_SAND_OK,
    fld_val,
    mem_entry[2];
   
    
  ARAD_IRR_SNOOP_MIRROR_TABLE0_TBL_DATA
    snp_mrr_tbl0_data;
  ARAD_IRR_SNOOP_MIRROR_TABLE1_TBL_DATA
    snp_mrr_tbl1_data;
  ARAD_PP_IHB_MRR_ACT_PROFILE_TBL_DATA
    mrr_act_profile_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ACTION_CMD_MIRROR_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  ARAD_ACTION_CMD_MIRROR_INFO_clear(info);

  ARAD_CLEAR(&snp_mrr_tbl0_data,ARAD_IRR_SNOOP_MIRROR_TABLE0_TBL_DATA,1);
  ARAD_CLEAR(&snp_mrr_tbl1_data,ARAD_IRR_SNOOP_MIRROR_TABLE1_TBL_DATA,1);
  ARAD_CLEAR(&mrr_act_profile_tbl_data,ARAD_PP_IHB_MRR_ACT_PROFILE_TBL_DATA,1);

 /* 0 << 5 as lsb entries for Mirroring */
  res = READ_IPT_SNP_MIR_CMD_MAPm(unit, MEM_BLOCK_ANY, ARAD_ACTION_CMD_IPT_SNP_MIR_CMD_MAP_SNOOP_MIRROR_OFFSET(cmd_type) | action_ndx, mem_entry);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 1, exit);

  info->cmd.outlif.value = 
      soc_mem_field32_get(
          unit,
          IPT_SNP_MIR_CMD_MAPm,
          mem_entry,
          OUTLIFf);
  info->cmd.outlif.enable = info->cmd.outlif.value ? 1 : 0;

   res = dpp_snoop_mirror_destination_config_get(unit, ARAD_ACTION_CMD_SNOOP_MIRROR_TABLE_OFFSET(cmd_type) + action_ndx,info);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 2, exit); 


  


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

