/*
 * $Id: kbp.c,v 1.23 Broadcom SDK $
 * 
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
 *
 */

#if defined(INCLUDE_KBP)

/*************
 * INCLUDES  *
 *************/

#include <shared/bsl.h>

#include <sal/appl/sal.h>

#include <soc/i2c.h>

#include <soc/mcm/memregs.h>

#include <soc/dpp/SAND/Management/sand_low_level.h>

#include <soc/dpp/ARAD/arad_kbp.h>
#include <soc/dpp/ARAD/arad_kbp_rop.h>

#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/field_int.h>

#include <appl/diag/system.h>
#include <appl/diag/shell.h>
#include <appl/diag/dpp/kbp.h>

#include <appl/dcmn/rx_los/rx_los.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_diag.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_fp.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_diag.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_flp_init.h>
/*************
 * DEFINES   *
 *************/


#define KBP_CMD_DPP_RESET_DEVICE_ACTION_BEFORE 0x1
#define KBP_CMD_DPP_RESET_DEVICE_ACTION_AFTER  0x2
#define KBP_CMD_DPP_RESET_DEVICE_ACTION_BOTH   0x3

#define ARAD_KBP_FRWRD_AND_ACL_BUFFER_SIZE_BITS 1024

static FILE *kbp_file_fp[SOC_MAX_NUM_DEVICES];

/*************
* FUNCTIONS *
*************/

/* This function parse a string of digits to array of uint8 hexadesimal numbers  
 * Example : if data_string is "12345678" (and parse_data_length=8), will return parsed_data = {0x12, 0x34, 0x56, 0x78}
 */
STATIC
int cmd_dpp_kbp_parse_string_of_data(int unit, char* data_string, uint8* parsed_data, uint32 parse_data_length /*in bytes*/)
{
    char temp[5] = "0x00";
    int i;

    if(data_string != NULL && *data_string == '0' && 
    ( *(data_string+1) == 'x' || *(data_string+1) == 'X')) {
        data_string += 2;
    }

    for(i=0; i<parse_data_length; ++i) {
        if( data_string != NULL && *data_string != 0 && *(data_string +1)!=0) {
            temp[2]=*data_string;
            temp[3]=*(data_string + 1);
            parsed_data[i] = (uint8)parse_integer(temp);
            data_string += 2;
        } else {
            return -1;
        }        
    }
    
    return 0;

}


STATIC cmd_result_t
cmd_dpp_kbp_cpu_record_send(int unit, args_t *a)
{
    soc_reg_above_64_val_t msb, lsb;
    parse_table_t pt; 
    cmd_result_t rv = CMD_OK;
    uint32 
        opcode,
        soc_sand_rv;
    int lsb_enable;
    
    SOC_REG_ABOVE_64_CLEAR(msb);
    SOC_REG_ABOVE_64_CLEAR(lsb);

    if (ARG_CNT(a) > 0) {
        parse_table_init(0,&pt);

            /* fill msb data */
        parse_table_add(&pt,"msb0",PQ_INT , (void *) (0), &msb[0], NULL);
        parse_table_add(&pt,"msb1",PQ_INT , (void *) (0), &msb[1], NULL);
        parse_table_add(&pt,"msb2",PQ_INT , (void *) (0), &msb[2], NULL);
        parse_table_add(&pt,"msb3",PQ_INT , (void *) (0), &msb[3], NULL);
        parse_table_add(&pt,"msb4",PQ_INT , (void *) (0), &msb[4], NULL);
        parse_table_add(&pt,"msb5",PQ_INT , (void *) (0), &msb[5], NULL);
        parse_table_add(&pt,"msb6",PQ_INT , (void *) (0), &msb[6], NULL);
        parse_table_add(&pt,"msb7",PQ_INT , (void *) (0), &msb[7], NULL);
        parse_table_add(&pt,"msb8",PQ_INT , (void *) (0), &msb[8], NULL);
        parse_table_add(&pt,"msb9",PQ_INT , (void *) (0), &msb[9], NULL);
        parse_table_add(&pt,"msb10",PQ_INT , (void *) (0), &msb[10], NULL);
        parse_table_add(&pt,"msb11",PQ_INT , (void *) (0), &msb[11], NULL);
        parse_table_add(&pt,"msb12",PQ_INT , (void *) (0), &msb[12], NULL);
        parse_table_add(&pt,"msb13",PQ_INT , (void *) (0), &msb[13], NULL);
        parse_table_add(&pt,"msb14",PQ_INT , (void *) (0), &msb[14], NULL);
        parse_table_add(&pt,"msb15",PQ_INT , (void *) (0), &msb[15], NULL);
            /* fill lsb data */
        parse_table_add(&pt,"lsb0",PQ_INT , (void *) (0), &lsb[0], NULL);
        parse_table_add(&pt,"lsb1",PQ_INT , (void *) (0), &lsb[1], NULL);
        parse_table_add(&pt,"lsb2",PQ_INT , (void *) (0), &lsb[2], NULL);
        parse_table_add(&pt,"lsb3",PQ_INT , (void *) (0), &lsb[3], NULL);
        parse_table_add(&pt,"lsb4",PQ_INT , (void *) (0), &lsb[4], NULL);
        parse_table_add(&pt,"lsb5",PQ_INT , (void *) (0), &lsb[5], NULL);
        parse_table_add(&pt,"lsb6",PQ_INT , (void *) (0), &lsb[6], NULL);
        parse_table_add(&pt,"lsb7",PQ_INT , (void *) (0), &lsb[7], NULL);
        parse_table_add(&pt,"lsb8",PQ_INT , (void *) (0), &lsb[8], NULL);
        parse_table_add(&pt,"lsb9",PQ_INT , (void *) (0), &lsb[9], NULL);
        parse_table_add(&pt,"lsb10",PQ_INT , (void *) (0), &lsb[10], NULL);
        parse_table_add(&pt,"lsb11",PQ_INT , (void *) (0), &lsb[11], NULL);
        parse_table_add(&pt,"lsb12",PQ_INT , (void *) (0), &lsb[12], NULL);
        parse_table_add(&pt,"lsb13",PQ_INT , (void *) (0), &lsb[13], NULL);
        parse_table_add(&pt,"lsb14",PQ_INT , (void *) (0), &lsb[14], NULL);
        parse_table_add(&pt,"lsb15",PQ_INT , (void *) (0), &lsb[15], NULL);

        parse_table_add(&pt,"OPcode",PQ_INT , (void *) (0), &opcode, NULL);

        parse_table_add(&pt,"LSBenable",PQ_INT , (void *) (0), &lsb_enable, NULL);
      
        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n",
                    ARG_CMD(a), ARG_CUR(a));
            return CMD_USAGE;
        }
    } else {
        return CMD_USAGE;
    }

#ifdef BCM_88660
    if (SOC_IS_ARADPLUS(unit)) {
        soc_sand_rv = aradplus_kbp_cpu_record_send(unit, opcode, msb, lsb, lsb_enable, NULL);
    } else
#endif
    {
        soc_sand_rv = arad_kbp_cpu_record_send(unit, opcode, msb, lsb, lsb_enable);
    }

    if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
        cli_out("Error: arad_kbp_cpu_record_send(%d , 0x%x /* opcode*/, &msb_data, &lsb_data)\n", unit, opcode);
        return CMD_FAIL;
    }

    return rv;

}

STATIC cmd_result_t
cmd_dpp_kbp_rop_write(int unit, args_t *a)
{
    parse_table_t pt; 
    cmd_result_t rv = CMD_OK;
    char *data_str=NULL, *mask_str=NULL;
    uint32 addr, addr_short, nbo_addr;
    arad_kbp_rop_write_t wr_data;
    uint32 soc_sand_rv;

    if (ARG_CNT(a) > 0) {
        parse_table_init(0,&pt);

        parse_table_add(&pt,"ADdr",PQ_INT , (void *) (0), &addr, NULL);
        parse_table_add(&pt,"ADdrShort",PQ_INT , (void *) (0), &addr_short, NULL);
        parse_table_add(&pt,"DAta",PQ_STRING , (void *) ("00000000000000000000"), &data_str, NULL);
        parse_table_add(&pt,"Mask",PQ_STRING , (void *) ("00000000000000000000"), &mask_str, NULL);
        parse_table_add(&pt,"ValidBit",PQ_INT , (void *) (0), &wr_data.vBit, NULL);
        parse_table_add(&pt,"WriteMode",PQ_INT , (void *) (0), &wr_data.writeMode, NULL); 
        
        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n",
                    ARG_CMD(a), ARG_CUR(a));
            return CMD_USAGE;
        }
    } else {
        return CMD_USAGE;
    }
    nbo_addr = soc_htonl(addr);
    sal_memcpy(wr_data.addr, &nbo_addr, sizeof(uint32));
    nbo_addr = soc_htonl(addr_short);
    sal_memcpy(wr_data.addr_short, &nbo_addr, sizeof(uint32));


    cli_out("\n");

    /* set DAta */
    if(cmd_dpp_kbp_parse_string_of_data(unit, data_str, wr_data.data, NLM_DATA_WIDTH_BYTES)  != 0 ) {
        cli_out("DAta didn't type correctly, please type 20 digits, with no spaces.\n"
                "Example: DAta=0x00000000001234567890\n");
        return CMD_FAIL;
    }
    /* set Mask */
    if(mask_str[0] != 0) {
       if(cmd_dpp_kbp_parse_string_of_data(unit, mask_str, wr_data.mask, NLM_DATA_WIDTH_BYTES)  != 0 ) {
            cli_out("Mask didn't type correctly, please type 20 digits, with no spaces.\n"
                    "Example: Mask=00000000001234567890\n");
            return CMD_FAIL;
        } 
    } else {
        sal_memset(wr_data.mask, 0x0, NLM_DATA_WIDTH_BYTES);
    }
    
    soc_sand_rv = arad_kbp_rop_write(unit, &wr_data);
    if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
        cli_out("Error: arad_kbp_rop_write(%d, &wr_data) \n", unit);
        return CMD_FAIL;
    }

    return rv;

}

STATIC cmd_result_t
cmd_dpp_kbp_rop_read(int unit, args_t *a)
{
    parse_table_t pt; 
    cmd_result_t rv = CMD_OK;
    arad_kbp_rop_read_t rd_data;
    uint32 addr, nbo_addr;
    int i;
    uint32 soc_sand_rv;

    if (ARG_CNT(a) > 0) {
        parse_table_init(0,&pt);

        parse_table_add(&pt,"ValidBit", PQ_INT , (void *) (0), &rd_data.vBit, NULL);
        parse_table_add(&pt,"TadaType", PQ_INT , (void *) (0), &rd_data.dataType, NULL);
        parse_table_add(&pt,"ADdr", PQ_INT , (void *) (0), &addr, NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n",
                    ARG_CMD(a), ARG_CUR(a));
            return CMD_USAGE;

        } 

      
    } else {
        return CMD_USAGE;
    }

    nbo_addr = soc_htonl(addr);
    sal_memcpy(rd_data.addr, &nbo_addr, sizeof(uint32));
  
    soc_sand_rv = arad_kbp_rop_read(unit, &rd_data);
    if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
        cli_out("Error: arad_kbp_rop_read(%d, &rd_data) \n", unit);
        return CMD_FAIL;
    }

    cli_out("0x%02x%02x%02x%02x: ", rd_data.addr[0], rd_data.addr[1], rd_data.addr[2], rd_data.addr[3]);

    for(i=0; i<11; ++i) {
        cli_out("0x%02x ", rd_data.data[i]);
    }

    cli_out("\n");

    return rv;

}

STATIC cmd_result_t
cmd_dpp_kbp_reset_device_action(int unit, uint32 action)
{
    cmd_result_t ret = CMD_OK;
#if defined(__DUNE_GTO_BCM_CPU__)
    uint32 before = 0x0, after = 0x0;

    if ((action == KBP_CMD_DPP_RESET_DEVICE_ACTION_BEFORE) || (action == KBP_CMD_DPP_RESET_DEVICE_ACTION_BOTH)) {
        before = 0x1;
    } 
    if ((action == KBP_CMD_DPP_RESET_DEVICE_ACTION_AFTER) || (action == KBP_CMD_DPP_RESET_DEVICE_ACTION_BOTH)) {
        after = 0x1;
    } 
    
    cli_out("%s(): Reset KBP action: before=%d, after=%d\n", FUNCTION_NAME(), before, after);

    if (before == 0x1) {
        /* Set both to Output */
        ret = cpu_i2c_write(0x40, 0x3, CPU_I2C_ALEN_LONG_DLEN_LONG, 0xc0);
        if( ret != CMD_OK){
            cli_out("Error in %s(): cpu_i2c_write(). FAILED !!!\n", FUNCTION_NAME());
            return ret;
        }

        /* Assert both to Low */
        ret = cpu_i2c_write(0x40, 0x2, CPU_I2C_ALEN_LONG_DLEN_LONG, 0xc0);
        if( ret != CMD_OK){
            cli_out("Error in %s(): cpu_i2c_write(). FAILED !!!\n", FUNCTION_NAME());
            return ret;
        }
        sal_usleep(1500);
        ret = cpu_i2c_write(0x40, 0x2, CPU_I2C_ALEN_LONG_DLEN_LONG, 0x0);
        if( ret != CMD_OK){
            cli_out("Error in %s(): cpu_i2c_write(). FAILED !!!\n", FUNCTION_NAME());
            return ret;
        }

        /* Wait for a minimum of 1ms after power rails are stable and de-assert SRST_L. */
        sal_usleep(1000);

        /* Assert SRST (gpio 6) to High (means DeAssert) (right button) */
        ret = cpu_i2c_write(0x40, 0x2, CPU_I2C_ALEN_LONG_DLEN_LONG, 0x40);
        if( ret != CMD_OK){
            cli_out("Error in %s(): cpu_i2c_write(). FAILED !!!\n", FUNCTION_NAME());
            return ret;
        }
    }

    if (after == 0x1) {
        /* Wait for a minimum of 1.5ms after de-assertion of SRST_L then de-assert CRST_L */
        sal_usleep(1500);

        /* Assert CRST (gpio 7) to High (means DeAssert) (left button) */
        ret = cpu_i2c_write(0x40, 0x2, CPU_I2C_ALEN_LONG_DLEN_LONG, 0xc0);
        if( ret != CMD_OK){
            cli_out("Error in %s(): cpu_i2c_write(). FAILED !!!\n", FUNCTION_NAME());
            return ret;
        }
    }

#endif /* (__DUNE_GTO_BCM_CPU__) */
    return ret;
}


STATIC cmd_result_t
cmd_dpp_kbp_reset_device(int unit, args_t *a)
{
    parse_table_t pt; 
    cmd_result_t ret = CMD_OK;
    uint32 before = 0x1, after = 0x1;

    if (ARG_CNT(a) > 0) {
        parse_table_init(0,&pt);

        parse_table_add(&pt,"Before",PQ_INT , (void *) (1), &before, NULL);
        parse_table_add(&pt,"After",PQ_INT , (void *) (1), &after, NULL);
  
        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n",
                    ARG_CMD(a), ARG_CUR(a));
            return CMD_USAGE;
        }
    } 

    if (before == 0x1) {
        ret = cmd_dpp_kbp_reset_device_action(unit, KBP_CMD_DPP_RESET_DEVICE_ACTION_BEFORE);
        if( ret != CMD_OK){
            cli_out("Error in %s(): cmd_dpp_kbp_reset_device_action() - Before. FAILED !!!\n", FUNCTION_NAME());
            return ret;
        }
    }

    if (after == 0x1) {
        ret = cmd_dpp_kbp_reset_device_action(unit, KBP_CMD_DPP_RESET_DEVICE_ACTION_AFTER);
        if( ret != CMD_OK){
            cli_out("Error in %s(): cmd_dpp_kbp_reset_device_action() - After. FAILED !!!\n", FUNCTION_NAME());
            return ret;
        }
    }
   
    return ret;

}

STATIC int32_t
cmd_dpp_kbp_callback_reset_device(void *handle, int32_t s_reset_low, int32_t c_reset_low){
#if defined(__DUNE_GTO_BCM_CPU__)
    int val = 0;
    cmd_result_t ret;

    if(!s_reset_low){
        SHR_BITSET(&val, 6);
    }
    if(!c_reset_low){
        SHR_BITSET(&val, 7);
    }
    ret = cpu_i2c_write(0x40, 0x2, CPU_I2C_ALEN_LONG_DLEN_LONG, val);
    if( ret != CMD_OK){
        return KBP_INTERNAL_ERROR;
    }
#endif /*__DUNE_GTO_BCM_CPU__*/
    /*in case of out of reset sleep for link lock*/
    if((!c_reset_low) && (!s_reset_low)){
        sal_usleep(150000);
    }
    return KBP_OK;
}


STATIC cmd_result_t
cmd_dpp_kbp_print_master(int unit)
{
    uint32 soc_sand_rv;
    uint32 pmf_progs_bmp_final;
    uint32 pfg_bmp[ARAD_PMF_NOF_LINES_MAX_ALL_STAGES];
    uint32 flp_program;
    ARAD_KBP_LTR_CONFIG config;
    int32 search;
    ARAD_PP_DIAG_FRWRD_LKUP_INFO lookup_info;
    ARAD_KBP_FRWRD_IP_TBL_ID table_id;
    uint32 acl_db_idx;
    ARAD_PP_FP_RESOURCE_DIAG_INFO *resource_diag_info = NULL;
    ARAD_PP_FP_RESOURCE_KEY *cur_qual;
    int32 qual_idx;
    int8 acl_idx = -1;
    uint32 offset = ARAD_KBP_FRWRD_AND_ACL_BUFFER_SIZE_BITS;
    uint32 shift = 0;
    uint32 nof_bytes;
    uint32 buffer[32];
    uint32 qual_val[2];
    uint8 ppd_app_type;
    uint8 frwrd_searched_alraedy = FALSE;
    uint32 total_length;

    /* getting the flp program */
    soc_sand_rv = arad_pp_fp_packet_diag_pgm_bmp_get_unsafe(unit,
                                                            ARAD_FP_DATABASE_STAGE_INGRESS_FLP,
                                                            &pmf_progs_bmp_final,
                                                            pfg_bmp);
    if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
        cli_out("Failed in arad_pp_fp_packet_diag_pgm_bmp_get_unsafe.\n");
        goto fail;
    }

    /* the flp program is the first bit set in pmf_progs_bmp_final */
    for(flp_program = 0; flp_program < SOC_DPP_DEFS_GET(unit, nof_flp_programs); ++flp_program) {
        if((1 << flp_program) & pmf_progs_bmp_final) {
            break;
        }
    }
    if(flp_program == SOC_DPP_DEFS_GET(unit, nof_flp_programs)) {
        cli_out("No flp program have been found.\n");
        goto fail;
    }

    /* taking the required configuration*/
    soc_sand_rv = arad_kbp_ltr_config_get(unit, flp_program, &config);
    if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
        cli_out("Failed in arad_kbp_ltr_config_get.\n");
        goto fail;
    }

    /* AppType */
    soc_sand_rv = arad_pp_sw_db_flp_prog_index_to_app_get(unit, flp_program, &ppd_app_type);
    if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
        cli_out("Failed in arad_pp_sw_db_flp_prog_index_to_app_get.\n");
        goto fail;
    }        
    cli_out("AppType : %s.\n\r", arad_pp_flp_prog_id_to_prog_name(unit, ppd_app_type));

    /* calculation needed to shift the offset */
    total_length = 0;
    for(search = 0; search < ARAD_KBP_MAX_NUM_OF_PARALLEL_SEARCHES; ++search) {
        if(config.search_type[search] == ARAD_KBP_DB_TYPE_FORWARDING) {
            if(frwrd_searched_alraedy) {
                continue;
            }

            soc_sand_rv = arad_pp_diag_frwrd_lkup_info_get(unit, &lookup_info);
            if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
                cli_out("Failed in arad_pp_diag_frwrd_lkup_info_get.\n");
                goto fail;
            }

            /* verifying that the forwarding is in kbp - if internal, not part of the master key */
            if(!lookup_info.is_kbp) {
                continue;
            }

            frwrd_searched_alraedy = TRUE;

            soc_sand_rv = arad_pp_diag_get_frwrd_type_size(arad_pp_diag_fwd_lkup_type_to_frwrd_ip_tbl_id(lookup_info.frwrd_type), &nof_bytes);
            if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
                cli_out("Failed in arad_pp_diag_get_frwrd_type_size.\n");
                goto fail;
            }
            total_length += nof_bytes;
        } else if (config.search_type[search] == ARAD_KBP_DB_TYPE_ACL) {
            table_id = config.tbl_id[search];
            acl_db_idx = table_id - ARAD_KBP_ACL_TABLE_ID_OFFSET;
            if(!resource_diag_info) {
                resource_diag_info = sal_alloc(sizeof(ARAD_PP_FP_RESOURCE_DIAG_INFO), "cmd_dpp_kbp_print_master.resource_diag_info");
                if(!resource_diag_info) {
                    cli_out("Memory allocation failure\n");
                    goto fail;
                }
            }
            soc_sand_rv = arad_pp_fp_resource_diag_get_unsafe(unit, ARAD_PP_FP_RESOURCE_MODE_USAGE, resource_diag_info);
            if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
                cli_out("Failed in arad_pp_fp_resource_diag_get_unsafe.\n");
                goto fail;
            }

            qual_idx = resource_diag_info->db[acl_db_idx].nof_ces - 1;
            cur_qual = &(resource_diag_info->db[acl_db_idx].key_qual[qual_idx]);
            total_length += SOC_SAND_DIV_ROUND_UP(cur_qual->key_loc.msb + 1, SOC_SAND_NOF_BITS_IN_CHAR);;
        }
    }
    
    for(search = ARAD_KBP_MAX_NUM_OF_PARALLEL_SEARCHES - 1; search >= 0; --search) {
        if(config.search_type[search] == ARAD_KBP_DB_TYPE_ACL) {
            table_id = config.tbl_id[search];
            acl_db_idx = table_id - ARAD_KBP_ACL_TABLE_ID_OFFSET;
            if(!resource_diag_info) {
                resource_diag_info = sal_alloc(sizeof(ARAD_PP_FP_RESOURCE_DIAG_INFO), "cmd_dpp_kbp_print_master.resource_diag_info");
                if(!resource_diag_info) {
                    cli_out("Memory allocation failure\n");
                    goto fail;
                }
            }
            soc_sand_rv = arad_pp_fp_resource_diag_get_unsafe(unit, ARAD_PP_FP_RESOURCE_MODE_USAGE, resource_diag_info);
            if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
                cli_out("Failed in arad_pp_fp_resource_diag_get_unsafe.\n");
                goto fail;
            }
            cur_qual = &(resource_diag_info->db[acl_db_idx].key_qual[0]);
            shift = offset - 8*total_length + cur_qual->key_loc.lsb;
            break;
        }
    }

    frwrd_searched_alraedy = FALSE;
    
    /* detemining if we have forwarding and the number of acls. */
    for(search = 0; search < ARAD_KBP_MAX_NUM_OF_PARALLEL_SEARCHES; ++search) {
        if(config.search_type[search] == ARAD_KBP_DB_TYPE_FORWARDING) {
            if(frwrd_searched_alraedy) {
                continue;
            }

            /* we have already get the lookup information */
            
            /* verifying that the forwarding is in kbp - if internal, not part of the master key */
            if(!lookup_info.is_kbp) {
                continue;
            }

            frwrd_searched_alraedy = TRUE;
            cli_out("Lookup %d\n\r", search);
            switch(lookup_info.frwrd_type)
            {
                case ARAD_PP_DIAG_FWD_LKUP_TYPE_IPV4_UC:
                    ARAD_PP_DIAG_IPV4_UNICAST_RPF_print_with_offsets(&lookup_info.lkup_key.kbp_ipv4_unicast_rpf, offset - shift);
                    break;
                case ARAD_PP_DIAG_FWD_LKUP_TYPE_IPV4_MC:
                    ARAD_PP_DIAG_IPV4_MULTICAST_print_with_offsets(&lookup_info.lkup_key.kbp_ipv4_multicast, offset - shift);
                    break;
                case ARAD_PP_DIAG_FWD_LKUP_TYPE_IPV6_UC:
                    ARAD_PP_DIAG_IPV6_UNICAST_RPF_print_with_offsets(&lookup_info.lkup_key.kbp_ipv6_unicast_rpf, offset - shift);
                    break;
                case ARAD_PP_DIAG_FWD_LKUP_TYPE_IPV6_MC:
                    ARAD_PP_DIAG_IPV6_MULTICAST_print_with_offsets(&lookup_info.lkup_key.kbp_ipv6_multicast, offset - shift);
                    break;
                case ARAD_PP_DIAG_FWD_LKUP_TYPE_ILM:
                    ARAD_PP_DIAG_MPLS_print_with_offsets(&lookup_info.lkup_key.kbp_mpls, offset - shift);
                    break;
                case ARAD_PP_DIAG_FWD_LKUP_TYPE_TRILL_UC:
                    ARAD_PP_DIAG_TRILL_UNICAST_print_with_offsets(&lookup_info.lkup_key.kbp_trill_unicast, offset - shift);
                    break;
                case ARAD_PP_DIAG_FWD_LKUP_TYPE_TRILL_MC:
                    ARAD_PP_DIAG_TRILL_MULTICAST_print_with_offsets(&lookup_info.lkup_key.kbp_trill_multicast, offset - shift);
                    break;
                default:
                    cli_out("Invalid Forwarding type.\n");
                    goto fail;
            }
            soc_sand_rv = arad_pp_diag_get_frwrd_type_size(arad_pp_diag_fwd_lkup_type_to_frwrd_ip_tbl_id(lookup_info.frwrd_type), &nof_bytes);
            if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
                cli_out("Failed in arad_pp_diag_get_frwrd_type_size.\n");
                goto fail;
            }
            cli_out("\n\r");
            offset -= 8*nof_bytes;
        } else if (config.search_type[search] == ARAD_KBP_DB_TYPE_ACL) {
            if(acl_idx < 0) {
                soc_sand_rv = arad_pp_diag_get_request(unit, 128, buffer);
                if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
                    cli_out("Failed in arad_pp_diag_get_request.\n");
                    goto fail;
                }
            }
            table_id = config.tbl_id[search];
            acl_db_idx = table_id - ARAD_KBP_ACL_TABLE_ID_OFFSET;
            if(!resource_diag_info) {
                resource_diag_info = sal_alloc(sizeof(ARAD_PP_FP_RESOURCE_DIAG_INFO), "cmd_dpp_kbp_print_master.resource_diag_info");
                if(!resource_diag_info) {
                    cli_out("Memory allocation failure\n");
                    goto fail;
                }
            }
            soc_sand_rv = arad_pp_fp_resource_diag_get_unsafe(unit, ARAD_PP_FP_RESOURCE_MODE_USAGE, resource_diag_info);
            if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
                cli_out("Failed in arad_pp_fp_resource_diag_get_unsafe.\n");
                goto fail;
            }
            ++acl_idx;

            cli_out("ACL-%c. DB-ID %d.\n", 'A' + acl_idx, acl_db_idx);
            qual_idx = resource_diag_info->db[acl_db_idx].nof_ces - 1;
            cur_qual = &(resource_diag_info->db[acl_db_idx].key_qual[qual_idx]);
            offset -= 8*SOC_SAND_DIV_ROUND_UP(cur_qual->key_loc.msb + 1, SOC_SAND_NOF_BITS_IN_CHAR);

            cli_out("\n\r");
            cli_out("--------------------------------------------------------------------------------\n\r");
            cli_out("%-50s|%-4s|%-4s|%-18s|\n\r", "Field name", "MSB", "LSB", "Last-packet Value");
            cli_out("--------------------------------------------------------------------------------\n\r");
            for(qual_idx = resource_diag_info->db[acl_db_idx].nof_ces - 1; qual_idx >= 0; --qual_idx) {
                cur_qual = &(resource_diag_info->db[acl_db_idx].key_qual[qual_idx]);
                qual_val[0] = 0;
                qual_val[1] = 0;
                SHR_BITCOPY_RANGE(qual_val, 0, buffer, cur_qual->key_loc.lsb + offset, cur_qual->key_loc.msb - cur_qual->key_loc.lsb + 1);
                if(cur_qual->key_loc.msb - cur_qual->key_loc.lsb + 1 > 32) {
                    cli_out("%-50s|%-4d|%-4d|0x%-8x%08x|\n\r",
                            ARAD_PP_FP_QUAL_TYPE_to_string(cur_qual->qual_type),
                            cur_qual->key_loc.msb + offset - shift,
                            cur_qual->key_loc.lsb + offset - shift,
                            qual_val[1],
                            qual_val[0]);
                } else {
                    cli_out("%-50s|%-4d|%-4d|0x%-16x|\n\r",
                            ARAD_PP_FP_QUAL_TYPE_to_string(cur_qual->qual_type),
                            cur_qual->key_loc.msb + offset - shift,
                            cur_qual->key_loc.lsb + offset - shift,
                            qual_val[0]);
                }
            }
            cli_out("\n");
        }
    }
    if(resource_diag_info) {
        sal_free(resource_diag_info);
    }
    return CMD_OK;

fail:
    if(resource_diag_info) {
        sal_free(resource_diag_info);
    }
    return CMD_FAIL;
}

STATIC cmd_result_t
cmd_dpp_kbp_print(int unit, args_t *a)
{
    parse_table_t pt; 
    cmd_result_t rv = CMD_OK;
    char *file_name=NULL;
    uint32 print_level = 0;
    char *arg_master;

    if (ARG_CNT(a) > 0) {
        /* option: print master */
        arg_master = ARG_CUR(a);
        if (arg_master && (sal_strncasecmp(arg_master, "master", strlen(arg_master)) == 0) ) {
            arg_master = ARG_GET(a);
            return cmd_dpp_kbp_print_master(unit);
        }
        parse_table_init(0,&pt);

        parse_table_add(&pt,"File",PQ_STRING , (void *) ("kbp_print.txt"), &file_name, NULL);
      
        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n",
                    ARG_CMD(a), ARG_CUR(a));
            return CMD_USAGE;
        }

        print_level = 1;
    } 

    arad_kbp_sw_print(unit, print_level, file_name); 

    return rv;
}

int dpp_kbp_file_open(int unit, char *filename)
{
    uint8 is_warmboot;
    if (NULL == filename) {
        return 0; 
    }

    if (kbp_file_fp[unit] == NULL) {
        is_warmboot = SOC_WARM_BOOT(unit);

        if ((kbp_file_fp[unit] =
             sal_fopen(filename, is_warmboot ? "r+" : "w+")) == 0) {
             cli_out("Error:  sal_fopen() Failed\n");
             return SOC_E_FAIL;
        }
    }

    return SOC_E_NONE;
}

int
dpp_kbp_file_close(int unit)
{
    if (kbp_file_fp[unit]) {
        sal_fclose(kbp_file_fp[unit]);
        kbp_file_fp[unit] = 0;
    }

    return 0;
}

STATIC int
dpp_kbp_file_read_func(void * handle, uint8_t *buffer, uint32_t size, uint32_t offset)
{
    size_t result;

    if (!handle) {
        return SOC_E_FAIL;
    }

    if (0 != fseek(handle, offset, SEEK_SET)) {
        return SOC_E_FAIL;
    }

    result = fread(buffer, 1, size, handle);
    if(result < size) {
        return SOC_E_FAIL;
    }

    return SOC_E_NONE;
}

STATIC int
dpp_kbp_file_write_func(void * handle, uint8_t *buffer, uint32_t size, uint32_t offset)

{
    size_t result;

    if (!handle) {
        return SOC_E_UNIT;
    }

    if (0 != fseek(handle, offset, SEEK_SET)) {
        return SOC_E_FAIL;
    }

    result = fwrite(buffer, 1, size, handle);
    if (result != size) {
        return SOC_E_MEMORY;
    }
    fflush(handle);

    return SOC_E_NONE;
}


int dpp_kbp_init_appl(int unit, ARAD_INIT_ELK *elk_ptr) {

    cmd_result_t rv = CMD_OK;
    uint32 ilkn_num_lanes, ilkn_metaframe;
    int ilkn_rate;
    rx_los_state_t port_stable_state;
    bcm_port_t kbp_port;
    uint32 soc_sand_rv;

    /* Give the Interface time to sync/adjust */
    soc_sand_rv = arad_kbp_ilkn_interface_param_get(unit, &kbp_port, &ilkn_num_lanes, &ilkn_rate, &ilkn_metaframe);
    rv = handle_sand_result(soc_sand_rv);
    if (BCM_FAILURE(rv)) {
       cli_out("Error:  arad_kbp_ilkn_interface_param_get() Failed\n");
       return rv;
    }

    rv = rx_los_port_stable(unit, kbp_port, KBP_PORT_STABLE_TIMEOUT, &port_stable_state);
    if (BCM_FAILURE(rv)) {
       cli_out("Error:  rx_los_port_stable() Failed\n");
       return rv;
    }
    if ((port_stable_state != rx_los_state_ideal_state) && (port_stable_state != rx_los_states_count)) {
        /* No interface signal */
        cli_out("Error:  No signal from KBP ilkn. port_stable_state=%d\n", port_stable_state);
        return BCM_E_FAIL;
    }

    soc_sand_rv = dpp_kbp_file_open(unit, "kbp_warmboot");
    rv = handle_sand_result(soc_sand_rv);
    if (BCM_FAILURE(rv)) {
        cli_out("Error: dpp_kbp_file_open(%d)\n", unit);
        return rv;
    }

    arad_kbp_warmboot_register(unit, kbp_file_fp[unit], &dpp_kbp_file_read_func, &dpp_kbp_file_write_func);

    soc_sand_rv = arad_kbp_init_app(unit, elk_ptr);
    rv = handle_sand_result(soc_sand_rv);
    if (BCM_FAILURE(rv)) {
        cli_out("Error: arad_kbp_init_appl(%d, &elk)\n", unit);
        return rv;
    }
    return rv;
}

STATIC cmd_result_t
cmd_dpp_kbp_init_appl(int unit, args_t *a)
{
    parse_table_t pt; 
    ARAD_INIT_ELK elk, *elk_ptr=NULL;
    uint32 enable;

    sal_memset(&elk, 0x0, sizeof(elk));

    if (ARG_CNT(a) > 0) {
        parse_table_init(0,&pt);

        parse_table_add(&pt,"ENable",PQ_INT , (void *) (0), &enable, NULL);
        parse_table_add(&pt,"TcamDevType",PQ_INT , (void *) (0), &elk.tcam_dev_type, NULL);
        parse_table_add(&pt,"Ip4UcRpfFwdTableSize",PQ_INT , (void *) (0), &elk.fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0], NULL);
        parse_table_add(&pt,"Ip4McFwdTableSize",PQ_INT , (void *) (0), &elk.fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV4_MC], NULL);
        parse_table_add(&pt,"Ip6UcRpfFwdTableSize",PQ_INT , (void *) (0), &elk.fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_0], NULL);
        parse_table_add(&pt,"Ip6McFwdTableSize",PQ_INT , (void *) (0), &elk.fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV6_MC], NULL);
        parse_table_add(&pt,"TrillUcFwdTableSize",PQ_INT , (void *) (0), &elk.fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_TRILL_UC], NULL);
        parse_table_add(&pt,"TrillMcFwdTableSize",PQ_INT , (void *) (0), &elk.fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_TRILL_MC], NULL);
        parse_table_add(&pt,"MplsFwdTableSize",PQ_INT , (void *) (0), &elk.fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_LSR], NULL);
  
        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n",
                    ARG_CMD(a), ARG_CUR(a));
            return CMD_USAGE;
        }

        elk.enable = (uint8)enable;

        elk_ptr = &elk;

    } else {
        elk_ptr = NULL;
    }

    if (dpp_kbp_init_appl(unit, elk_ptr) < 0) {
        return CMD_FAIL;
    }
    return CMD_OK;
}

STATIC cmd_result_t
cmd_dpp_kbp_deinit_appl(int unit, args_t *a)
{
    uint32 soc_sand_rv;
    cmd_result_t rv = CMD_OK;

    soc_sand_rv = arad_kbp_deinit_app(unit);
    if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
        cli_out("Error: arad_kbp_deinit_appl(%d, &elk)\n", unit);
        rv = CMD_FAIL;
    }

    return rv;
}


int dpp_kbp_init_kbp_interface(int unit, uint32 kbp_mdio_id, uint32 kbp_ilkn_rev) {

    uint32 soc_sand_rv;
    int rv = BCM_E_NONE;
#if defined(__DUNE_GTO_BCM_CPU__)

    /* Set gpio-6 and gpio-7 to Output */
    rv = cpu_i2c_write(0x40, 0x3, CPU_I2C_ALEN_LONG_DLEN_LONG, 0xc0);
    if(BCM_FAILURE(rv)){
        cli_out("Error in %s(): cpu_i2c_write(). FAILED !!!\n", FUNCTION_NAME());
        return rv;
    }
#endif /*__DUNE_GTO_BCM_CPU__*/

    /* Device Configuration Using MDIO */
    soc_sand_rv = arad_kbp_init_kbp_interface(unit, kbp_mdio_id, kbp_ilkn_rev, cmd_dpp_kbp_callback_reset_device);
    rv = handle_sand_result(soc_sand_rv);
    if(BCM_FAILURE(rv)){
        cli_out("Error: arad_kbp_init_kbp_interface(%d, &elk)\n", unit);
    }
    return rv;   
}

STATIC cmd_result_t
cmd_dpp_kbp_init_kbp_interface(int unit, args_t *a)
{
    parse_table_t pt; 
    uint32 
        kbp_mdio_id = ARAD_KBP_APPL_MDIO_DEFAULT_ID, 
        kbp_ilkn_rev = ARAD_KBP_APPL_ILKN_REVERSE_DEFAULT;

    if (ARG_CNT(a) > 0) {
        parse_table_init(0,&pt);

       /* KBP mdio ID format (kbp_mdio_id default for Negev is 0x101):
        * bits [5:6,8:9] - bus ID.
        * Arad has 8 external buses (0-7), two of these buses has connection on board (buses 4 and 5).
        * Assuming the KBP connected to bus 4 ==> 4b'0100 ==> bit[8]=1.
        * bit [7] - Internal select. Set to 0 for external phy access.
        * bits [0:4] - phy/kbp id ==> 0x1    
        */ 

        parse_table_add(&pt,"mdio_id",PQ_INT , (void *) (ARAD_KBP_APPL_MDIO_DEFAULT_ID), &kbp_mdio_id, NULL);
        parse_table_add(&pt,"ilkn_rev",PQ_INT , (void *) (ARAD_KBP_APPL_ILKN_REVERSE_DEFAULT), &kbp_ilkn_rev, NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n",
                    ARG_CMD(a), ARG_CUR(a));
            return CMD_USAGE;
        }
    } 

    if (dpp_kbp_init_kbp_interface(unit, kbp_mdio_id, kbp_ilkn_rev) < 0) {
        return CMD_FAIL;
    }
    return CMD_OK;
}

STATIC cmd_result_t
cmd_dpp_kbp_test_ip4_rpf_appl(int unit, args_t *a)
{
    uint32 num_entries, record_base_tbl[4], ad_val_tbl[4];
    parse_table_t pt; 
    cmd_result_t rv = CMD_OK;
    uint32 soc_sand_rv;

    if (ARG_CNT(a) > 0) {
    
        parse_table_init(0,&pt);

        parse_table_add(&pt,"NumEntries"    ,PQ_INT , (void *) (1)         , &num_entries     , NULL);
        parse_table_add(&pt,"RecordBaseTbl0",PQ_INT , (void *) (0x55551234), &record_base_tbl[0], NULL);
        parse_table_add(&pt,"RecordBaseTbl1",PQ_INT , (void *) (0xeeee1234), &record_base_tbl[1], NULL);
        parse_table_add(&pt,"RecordBaseTbl2",PQ_INT , (void *) (0x66661234), &record_base_tbl[2], NULL);
        parse_table_add(&pt,"RecordBaseTbl3",PQ_INT , (void *) (0x77771234), &record_base_tbl[3], NULL);
        parse_table_add(&pt,"ADValTbl0"     ,PQ_INT , (void *) (0xdead0620), &ad_val_tbl[0]     , NULL);
        parse_table_add(&pt,"ADValTbl1"     ,PQ_INT , (void *) (0xbeaf8321), &ad_val_tbl[1]     , NULL);
        parse_table_add(&pt,"ADValTbl2"     ,PQ_INT , (void *) (0x01020304), &ad_val_tbl[2]     , NULL);
        parse_table_add(&pt,"ADValTbl3"     ,PQ_INT , (void *) (0x0a0b0c0d), &ad_val_tbl[3]     , NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n",
                    ARG_CMD(a), ARG_CUR(a));
            return CMD_USAGE;
        }
        
        cli_out("num_entries=%d, record_base_tbl[0]=0x%x, record_base_tbl[1]=0x%x, record_base_tbl[2]=0x%x, record_base_tbl[3]=0x%x, "
                "ad_val_tbl[0]=0x%x, ad_val_tbl[1]=0x%x, ad_val_tbl[2]=0x%x, ad_val_tbl[3]=0x%x\n", 
                num_entries, record_base_tbl[0], record_base_tbl[1], record_base_tbl[2], record_base_tbl[3], 
                ad_val_tbl[0], ad_val_tbl[1], ad_val_tbl[2], ad_val_tbl[3]);

        soc_sand_rv = arad_kbp_test_ip4_rpf_NlmGenericTableManager(unit, num_entries, record_base_tbl, ad_val_tbl);
        if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
            cli_out("Error: arad_kbp_test_ip4_rpf_NlmGenericTableManager(%d)\n", unit);
            return CMD_FAIL;
        } 
   
    }     
                          
    return rv;
}

STATIC cmd_result_t
cmd_dpp_kbp_test_add_rate(int unit, args_t *a)
{
    parse_table_t pt; 
    bcm_error_t rv = CMD_OK;

    int i;
    int entry_num;
    int entry_num_print_mod;

    int fec = 0x20000400;
    int encap_id = 0x40001000;
    int vrf = 0;
    int host = 0x0a00ff01;
    /* bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x03, 0x00, 0x01}; */
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    bcm_l3_host_t l3host;

    if (ARG_CNT(a) > 0) {
    
        parse_table_init(0,&pt);

        parse_table_add(&pt,"Host"              ,PQ_INT , (void *) (0x0a00ff01)         , &host     , NULL);
        parse_table_add(&pt,"Vrf"               ,PQ_INT , (void *) (0x0)                , &vrf      , NULL);
        parse_table_add(&pt,"Fec"               ,PQ_INT , (void *) (0x20000400)         , &fec      , NULL);
        parse_table_add(&pt,"EncapId"           ,PQ_INT , (void *) (0x40001000)         , &encap_id , NULL);
        parse_table_add(&pt,"EntryNum"          ,PQ_INT , (void *) (5000)               , &entry_num , NULL);
        parse_table_add(&pt,"EntryNumPrintMod"  ,PQ_INT , (void *) (1000)               , &entry_num_print_mod , NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n",
                    ARG_CMD(a), ARG_CUR(a));
            return CMD_USAGE;
        }
        
        cli_out("host=0x%x, vrf=0x%x, fec=0x%x, encap_id=0x%x, entry_num=0x%x, entry_num_print_mod=0x%x\n", 
                host, vrf, fec, encap_id, entry_num, entry_num_print_mod);

    } else {
        return CMD_USAGE;   
    }

    soc_sand_ll_timer_clear();

    bcm_l3_host_t_init(&l3host);
    l3host.l3a_flags = 0;
    l3host.l3a_ip_addr = host;
    l3host.l3a_vrf = vrf;
    l3host.l3a_intf = fec; /* point to FEC to get out-interface  */
    l3host.l3a_modid = 0;
    /* set encap id to point to MAC address */
    /* as encap id is valid (!=0), host will point to FEC + outlif (MAC), and FEC will be "fixed" not to overwrite outlif */
    l3host.encap_id = encap_id;
    sal_memcpy(l3host.l3a_nexthop_mac, next_hop_mac, 6); /* next hop mac attached directly */

    soc_sand_ll_timer_set("Full", 1);

    for (i = 0; i < entry_num; i ++)
    {
        /* printf("******************** bcm_l3_host_add i=%d, l3host.l3a_ip_addr=0x%x\n", i, l3host.l3a_ip_addr, rv); */
        rv = bcm_l3_host_add(unit, &l3host);
        if (rv != BCM_E_NONE) {
             printf("Error, bcm_l3_host_add Failed, l3host.l3a_ip_addr=0x%x, rv=0x%x\n", l3host.l3a_ip_addr, rv);
        }
        
        if ((i % entry_num_print_mod) == 1) {
            printf("time=%u[us] i=%d, l3host.l3a_ip_addr=0x%x\n", sal_time_usecs(), i, l3host.l3a_ip_addr);
        }

        l3host.l3a_ip_addr ++;
    }

    /* sand_ll_timer_stop(1); */

    soc_sand_ll_timer_stop_all();
    soc_sand_ll_timer_print_all();

    return (rv == BCM_E_NONE) ? CMD_OK : CMD_FAIL;
}

STATIC cmd_result_t
cmd_dpp_kbp_test_acl_add_rate(int unit, args_t *a)
{
    int i;
    int entry_num;
    int entry_num_print_mod;
    int prio_mode;
    int priority;
    int cached_entries_num;
    parse_table_t pt;
    int is_serial_rpf = 0; 
    bcm_field_action_t external_value_action;
    cmd_result_t rv = CMD_OK;

    bcm_field_group_t group_id_elk;
    bcm_field_entry_t ent_elk;

    if (SOC_IS_ARADPLUS(unit)) {
        is_serial_rpf = (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_rpf_fwd_parallel", 0) == 0);
    }

    if (is_serial_rpf) {
        external_value_action = bcmFieldActionExternalValue1Set;
    }
    else {
        external_value_action = bcmFieldActionExternalValue2Set;
    }

    if (ARG_CNT(a) > 0) {
    
        parse_table_init(0,&pt);

        parse_table_add(&pt,"PrioMode"          ,PQ_INT , (void *) (0x0)                , &prio_mode            , NULL);
        parse_table_add(&pt,"EntryNum"          ,PQ_INT , (void *) (64*1024)            , &entry_num            , NULL);
        parse_table_add(&pt,"EntryNumPrintMod"  ,PQ_INT , (void *) (1000)               , &entry_num_print_mod  , NULL);
        parse_table_add(&pt,"CachedEntNum"      ,PQ_INT , (void *) (0)                  , &cached_entries_num   , NULL);
        parse_table_add(&pt,"GroupId"           ,PQ_INT , (void *) (100)                , &group_id_elk         , NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n",
                    ARG_CMD(a), ARG_CUR(a));
            return CMD_USAGE;
        }
        
        cli_out("PrioMode=0x%x, EntryNum=0x%x, EntryNumPrintMod=0x%x, CachedEntNum=0x%x GroupId=0x%x\n", 
                prio_mode, entry_num, entry_num_print_mod, cached_entries_num, group_id_elk);

    } else {
        return CMD_USAGE;   
    }

    soc_sand_ll_timer_clear();
    soc_sand_ll_timer_set("Full", 1);

    if (cached_entries_num > 0) {
        bcm_switch_control_set(unit, bcmSwitchFieldCache, 1);
    }

    /* step1: Add into Elk group */
    cli_out("step1: Add entries into Elk group\n");
    for (i = 0; i < entry_num; i++)
    {
        switch (prio_mode)
        {
           case 0:
               priority = 10;
               break;
           case 1:
               priority = 10 + i;
               break;
           case 2:
               priority = entry_num - i;
               break;
           default:
               cli_out("Invalid PrioMode.\n");
               rv = CMD_USAGE;
               return rv;
        }
        bcm_field_entry_create(0, group_id_elk, &ent_elk);
        bcm_field_qualify_SrcIp(0, ent_elk, i+1, 0xffffffff);
        bcm_field_action_add(0, ent_elk, external_value_action, entry_num - i, 0);
        bcm_field_entry_prio_set(0, ent_elk, priority);
        bcm_field_entry_install(0, ent_elk);
        if ((i % cached_entries_num) == (cached_entries_num-1)) {
            bcm_switch_control_set(unit, bcmSwitchFieldCommit, 0);
        }
        if ((i % entry_num_print_mod) == (entry_num_print_mod-1)) {
            soc_sand_ll_timer_stop_all();
            soc_sand_ll_timer_print_all();
            soc_sand_ll_timer_clear();
            soc_sand_ll_timer_set("Full", 1);
        }
    }

    /* step2: Destroy all the entries */
    cli_out("step2: Remove entries from Elk group\n");
    for (i=_BCM_DPP_FIELD_ENT_BIAS(unit, ExternalTcam);i<(_BCM_DPP_FIELD_ENT_BIAS(unit, ExternalTcam) + entry_num); i++)
    {
      bcm_field_entry_destroy(0,i);
    }

    return rv;
}


STATIC cmd_result_t
cmd_dpp_kbp_test_ipv4_random(int unit, args_t *a)
{
    int             i,j;
    int             entry_num = 64*1024;
    int             entry_num_print_mod = 8*1024;
    int             cached_entries_num = 0;
    parse_table_t   pt; 
    bcm_l3_route_t  routeInfo; 
    bcm_ip_t        dip;
    bcm_vrf_t       vrf = 0;
    bcm_if_t        fec_idx = 2000;
    int             prefix_len = 32;
    int             do_warmboot = 0;
    bcm_error_t     rv = BCM_E_NONE;

    if (ARG_CNT(a) > 0) {
        parse_table_init(0,&pt);

        parse_table_add(&pt,"EntryNum"          ,PQ_INT , (void *) (64*1024)            , &entry_num            , NULL);
        parse_table_add(&pt,"EntryNumPrintMod"  ,PQ_INT , (void *) (8*1024)             , &entry_num_print_mod  , NULL);
        parse_table_add(&pt,"CachedEntNum"      ,PQ_INT , (void *) (0)                  , &cached_entries_num   , NULL);
        parse_table_add(&pt,"WarmBoot"          ,PQ_INT , (void *) (0)                  , &do_warmboot          , NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n",
                    ARG_CMD(a), ARG_CUR(a));
            return CMD_USAGE;
        }
    }
    
    cli_out("EntryNum=0x%x, EntryNumPrintMod=0x%x, CachedEntNum=0x%x\n", 
            entry_num, entry_num_print_mod, cached_entries_num);

    soc_sand_ll_timer_clear();

    if (cached_entries_num > 0) {
        bcm_switch_control_set(unit, bcmSwitchFieldCache, 1);
    }

    cli_out("step1: Add routes\n");
    for (i = 0; i < entry_num/entry_num_print_mod; i++)
    {
        soc_sand_ll_timer_set("Full", 1);
        for(j=0; j<entry_num_print_mod; j++)
        {
            bcm_l3_route_t_init(&routeInfo);
            dip = (1<<(32-prefix_len))*((j+i*entry_num_print_mod)/15+1);
            routeInfo.l3a_vrf = vrf;
            routeInfo.l3a_intf = fec_idx;
            routeInfo.l3a_subnet = dip;
            routeInfo.l3a_ip_mask = 0xffffffff<<(32-prefix_len);
            rv = bcm_l3_route_add(unit,&routeInfo);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_l3_route_add Failed, routeInfo.l3a_subnet=0x%x, rv=0x%x\n", routeInfo.l3a_subnet, rv);
                return CMD_FAIL;
            }

            prefix_len--;
            if (prefix_len <= 17)
            {
                prefix_len = 32;
            }
        }
        soc_sand_ll_timer_stop_all();
        soc_sand_ll_timer_print_all();
        soc_sand_ll_timer_clear();
    }

    if (do_warmboot) {
        sh_process_command(unit, "tr 141 warmboot=1");
    }

    cli_out("step2: Update routes\n");
    fec_idx = 3000;
    prefix_len = 32;
    for (i = 0; i < entry_num/entry_num_print_mod; i++)
    {
        soc_sand_ll_timer_set("Full", 1);
        for(j=0; j<entry_num_print_mod; j++)
        {
            bcm_l3_route_t_init(&routeInfo);
            dip = (1<<(32-prefix_len))*((j+i*entry_num_print_mod)/15+1);
            routeInfo.l3a_vrf = vrf;
            routeInfo.l3a_intf = fec_idx;
            routeInfo.l3a_subnet = dip;
            routeInfo.l3a_ip_mask = 0xffffffff<<(32-prefix_len);
            routeInfo.l3a_flags |= BCM_L3_REPLACE;
            rv = bcm_l3_route_add(unit,&routeInfo);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_l3_route_add Failed, routeInfo.l3a_subnet=0x%x, rv=0x%x\n", routeInfo.l3a_subnet, rv);
                return CMD_FAIL;
            }

            prefix_len--;
            if (prefix_len <= 17)
            {
                prefix_len = 32;
            }
        }
        soc_sand_ll_timer_stop_all();
        soc_sand_ll_timer_print_all();
        soc_sand_ll_timer_clear();
    }

    if (do_warmboot) {
        sh_process_command(unit, "tr 141 warmboot=1");
    }

    cli_out("step3: Delete routes\n");
    prefix_len = 32;
    for (i = 0; i < entry_num/entry_num_print_mod; i++)
    {
        soc_sand_ll_timer_set("Full", 1);
        for(j=0; j<entry_num_print_mod; j++)
        {
            bcm_l3_route_t_init(&routeInfo);
            dip = (1<<(32-prefix_len))*((j+i*entry_num_print_mod)/15+1);
            routeInfo.l3a_vrf = vrf;
            routeInfo.l3a_subnet = dip;
            routeInfo.l3a_ip_mask = 0xffffffff<<(32-prefix_len);
            rv = bcm_l3_route_delete(unit,&routeInfo);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_l3_route_delete Failed, routeInfo.l3a_subnet=0x%x, rv=0x%x\n", routeInfo.l3a_subnet, rv);
                return CMD_FAIL;
            }

            prefix_len--;
            if (prefix_len <= 17)
            {
                prefix_len = 32;
            }
        }
        soc_sand_ll_timer_stop_all();
        soc_sand_ll_timer_print_all();
        soc_sand_ll_timer_clear();
    }

    return CMD_OK;
}

char cmd_dpp_kbp_usage[] =
    "Usage:\n"
    "\tkbp [options]\n" 
    "Usage options:\n"
    "\tcpu_record_send <LSBenable> <OPcode> <MSB> <LSB>- \n"
    "\t\tMSB LSB - msb lsb data, 0 by default, add index: MSB0, MSB1 ... MSB15\n"
    "\t\tOPcode - 251 (PIORDY), 252 (PIORDX), 253 (PIOWR), 254 (RD_LUT), 255 (WR_LUT)\n "
    "\tcpu_lookup_reply - arad_kbp_cpu_read\n"
    "\trop_write <ADdr> <DAta> <Mask> <ADdrShort> <ValidBit> <WriteMode> - write to rop.\n"
    "\t\tADdr - Address of location where data should be written. includes all :vbit, wrmode ...\n"
    "\t\tDAta Mask - data & mask , 0 by default\n"
    "\t\tADdrShort - Address of location where data should be written.\n"
    "\t\tValidBit - The valid bit which indicates if the database entry should be enabled or disabled\n"
    "\t\tWriteMode - needs to be 0 for DATABASE_DM or 1 for DATABASE_XY \n"
    "\trop_read <ADdr> <ValidBit> <TadaType> - \n"
    "\t\tADdr ValidBit see rop_write, TadaType - 0 for X and 1 for Y.\n"
    "\tinit_appl <ENable> <TcamDevType> <Ip4UcFwdTableSize>\n"
    "\tdeinit_appl\n"
    "\t\tENable - If TRUE, the ELK feature is active.\n"
    "\t\tTcamDevType - Indicate the External lookup Device type\n"
    "\t\tIp4UcFwdTableSize - IPv4 unicast forward table size. if 0x0 forwarding done in internal DB, else done External\n"
    "\tkbp print <File=FILE_NAME>\n"
    "\t\tFile - If specified, print KBP device configuration to file (HTML format - supported in chrome and firefox).\n"
    "\t\t       Otherwise print KBP software configuration of DBs and Instructions (available before and after calling kbp init_appl).\n"
    "\tkbp print master\n"
    "\t\t       Print the diagnostics of the master key.\n"
    "\ttest_ip4_rpf_appl <NumEntries> <RecordBaseTbl> <ADValTbl0>\n"
    "\treset_device <Before=0/1> <After=0/1> - Reset KBP device using Negev no board GPIO. default: Before=After=1\n"
    "\tinit_kbp_interface <mdio_id> - Init KBP interface (DCR, ILKN...)\n"
    "\t\tmdio_id - KBP mdio ID format. Default for Negev is 0x101\n"
    "\t\t          bits [5:6,8:9] - bus ID.\n"
    "\t\t          bit [7] - Internal select. Set to 0 for external phy access.\n"
    "\t\t          bits [0:4] - phy/kbp id.\n"
    "\tinit_arad_interface - Init Arad interface towards the KBP (EGW)\n"
    "Exampls:\n"
    "\tkbp cpu_record_send OPcode=255 MSB5=1 LSBenable=0\n"
    "\tkbp cpu_lookup_reply\n"
    "\tkbp rop_read addr=0x00000102\n"
    "\tkbp rop_write addr=0x0000010b ADdrShort=0x00000000 data=0x0000000000000000ffff\n"
    "\tkbp init_kbp_interface mdio_id=257 ilkn_rev=1\n"
    "\tkbp test_ip4_rpf_appl NumEntries=2048 RecordBaseTbl0=0x55551234 RecordBaseTbl1=0xeeee1234 ADValTbl0=0xdead0620 ADValTbl1=0xbeaf8321\n"
    "\tkbp init_appl enable=1 tcamdevtype=1 Ip4UcFwdTableSize=8192 Ip4McFwdTableSize=8192\n"
    ;


cmd_result_t
cmd_dpp_kbp(int unit, args_t *a)
{
    char *func;
    soc_reg_above_64_val_t data;
    cmd_result_t rv = CMD_OK;
    int i;
    uint32 soc_sand_rv;

    if (!(func = ARG_GET(a))) {    /* Nothing to do    */
        return(CMD_USAGE);      /* Print usage line */
    }

    if(!sal_strcasecmp(func, "cpu_record_send")) {

        return cmd_dpp_kbp_cpu_record_send(unit, a);

    } else if(!sal_strcasecmp(func, "cpu_lookup_reply")) {

        soc_sand_rv = arad_kbp_cpu_lookup_reply(unit, data);
        if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
            cli_out("Error: arad_kbp_cpu_lookup_reply(%d, &data)\n", unit);
            return CMD_FAIL;
        }

        cli_out("Reply Data:  ");

        for(i=0; i<11; ++i) {
            cli_out("0x%x ", data[i]);
        }

        cli_out("\n");

    } else if(!sal_strcasecmp(func, "rop_write")) {

       return cmd_dpp_kbp_rop_write(unit, a);

    } else if(!sal_strcasecmp(func, "rop_read")) {

        return cmd_dpp_kbp_rop_read(unit, a);
            
    } else if (!sal_strcasecmp(func, "reset_device")) {

        return cmd_dpp_kbp_reset_device(unit, a);

    } else if(!sal_strcasecmp(func, "init_appl")) {

        return cmd_dpp_kbp_init_appl(unit, a);
    
    } else if(!sal_strcasecmp(func, "print")) {

        return cmd_dpp_kbp_print(unit, a);
      
    } else if(!sal_strcasecmp(func, "deinit_appl")) {

        return cmd_dpp_kbp_deinit_appl(unit, a);
      
    } else if (!sal_strcasecmp(func, "init_kbp_interface")) {

        return cmd_dpp_kbp_init_kbp_interface(unit, a);

    } else if (!sal_strcasecmp(func, "init_arad_interface")) {
        soc_sand_rv = arad_kbp_init_arad_interface(unit);
        if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
            cli_out("Error: arad_kbp_init_arad_interface(%d)\n", unit);
            return CMD_FAIL;
        }
    } else if(!sal_strcasecmp(func, "test_ip4_rpf_appl")) {

        return cmd_dpp_kbp_test_ip4_rpf_appl(unit, a);                      

         
    } else if (!sal_strcasecmp(func, "TestAddRate")) {

        return cmd_dpp_kbp_test_add_rate(unit, a);

    } else if (!sal_strcasecmp(func, "TestAclAddRate")) {

        return cmd_dpp_kbp_test_acl_add_rate(unit, a);

    } else if (!sal_strcasecmp(func, "test_ipv4_random")) {

        return cmd_dpp_kbp_test_ipv4_random(unit, a);

    }else {
        return(CMD_USAGE);      /* Print usage line */
    }

    return rv;

}

#endif /* #ifdef INCLUDE_KBP */

