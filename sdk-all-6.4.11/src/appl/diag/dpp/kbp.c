/*
 * $Id: kbp.c,v 1.23 Broadcom SDK $
 * 
 * $Copyright: Copyright 2016 Broadcom Corporation.
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
#include <shared/swstate/access/sw_state_access.h>
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
#include <soc/dpp/JER/JER_PP/jer_pp_kaps_diag.h>
#include <soc/dpp/JER/JER_PP/jer_pp_kaps_entry_mgmt.h>
#include <soc/dpp/JER/JER_PP/jer_pp_kaps_xpt.h>
#include <soc/dpp/JER/JER_PP/jer_pp_kaps_arm_image.h>

/*************
 * DEFINES   *
 *************/

#define BYTE_0(x)  ((x)  & 0x000000FF)
#define BYTE_1(x)  (((x) & 0x0000FF00)>>8)
#define BYTE_2(x)  (((x) & 0x00FF0000)>>16)
#define BYTE_3(x)  (((x) & 0xFF000000)>>24)

#define KBP_CMD_DPP_RESET_DEVICE_ACTION_BEFORE 0x1
#define KBP_CMD_DPP_RESET_DEVICE_ACTION_AFTER  0x2
#define KBP_CMD_DPP_RESET_DEVICE_ACTION_BOTH   0x3

#define ARAD_KBP_FRWRD_AND_ACL_BUFFER_SIZE_BITS 1024

static FILE *kbp_file_fp[SOC_MAX_NUM_DEVICES];
static FILE *kaps_file_fp[SOC_MAX_NUM_DEVICES];

#define UINT64_SIZE_IN_BITS 64
#define UINT32_SIZE_IN_BITS 32
#define UINT8_SIZE_IN_BITS 8

#define KEY_NAME_CHAR_LENGTH 20

#define KBP_CMD_DPP_KAPS_ARM_IMAGE_LOAD_BUF_SIZE 5

#define KBP_IPV4_INSERTION_RATE_LIMIT	1200
#define KBP_ACL_INSERTION_RATE_LIMIT	130

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

STATIC int 
 dpp_kbp_ipv4_print_timers(int num_of_entries)
{
	uint32
	  cnt_ndx,i;
    uint32
        total_time_32bit;
	COMPILER_UINT64
	  total_time_1000, total_time_100, timer_total ;
	double rate = -1;

	LOG_CLI((BSL_META("\r\n")));
	if (Soc_sand_ll_timer_total == 0){
        LOG_CLI((BSL_META("No timers were hit, total measured execution time: 0\n\r")));
	}else{
        LOG_CLI((BSL_META(" KBP IPV4 Timers Measurements by layers (net time per timer) \n\r")));
        LOG_CLI((BSL_META(" +-----------------------------------------------------------------------------------------+\n\r")));
        LOG_CLI((BSL_META(" | Timer Name                             |Hit Count |Total Time[us] |Per Hit[us] |Percent |\n\r")));
        LOG_CLI((BSL_META(" +-----------------------------------------------------------------------------------------+\n\r")));

        COMPILER_64_SET(timer_total, 0, Soc_sand_ll_timer_cnt[ARAD_KBP_TIMERS_APPL].total_time);
        total_time_32bit = Soc_sand_ll_timer_cnt[ARAD_KBP_TIMERS_APPL].total_time;
        for (cnt_ndx = ARAD_KBP_IPV4_TIMERS_NOF_TIMERS-1; cnt_ndx >= ARAD_KBP_TIMERS_APPL; cnt_ndx--){

            if (Soc_sand_ll_timer_cnt[cnt_ndx].nof_hits != 0){

				if (cnt_ndx > ARAD_KBP_TIMERS_SOC) {
                    /* Its a kbp timer -
                     *  Reduce its total time from the higher layres total time
                     */
					for (i=0;i<=ARAD_KBP_TIMERS_SOC;i++) {
                        Soc_sand_ll_timer_cnt[i].total_time -= Soc_sand_ll_timer_cnt[cnt_ndx].total_time;
					}
				}else{
                    /* Its a soc/bcm/full timer -
                     *  Reduce its total time from the higher layres total time
                     */
					for (i=ARAD_KBP_TIMERS_APPL;i<cnt_ndx;i++) {
                        Soc_sand_ll_timer_cnt[i].total_time -= Soc_sand_ll_timer_cnt[cnt_ndx].total_time;
					}
                }
				COMPILER_64_SET(total_time_1000, 0, Soc_sand_ll_timer_cnt[cnt_ndx].total_time);
                COMPILER_64_SET(total_time_100, 0, Soc_sand_ll_timer_cnt[cnt_ndx].total_time);
                COMPILER_64_UMUL_32(total_time_1000,1000);
                COMPILER_64_UMUL_32(total_time_100,100);
                COMPILER_64_UDIV_64(total_time_1000,timer_total);
                COMPILER_64_UDIV_64(total_time_100,timer_total);
                LOG_CLI((BSL_META(" |%-40s| %-9d|%-15d|%-12d|%3d.%1d%%  |\n\r"), 
                        Soc_sand_ll_timer_cnt[cnt_ndx].name, 
                        Soc_sand_ll_timer_cnt[cnt_ndx].nof_hits,
                        Soc_sand_ll_timer_cnt[cnt_ndx].total_time, 
                        Soc_sand_ll_timer_cnt[cnt_ndx].total_time / Soc_sand_ll_timer_cnt[cnt_ndx].nof_hits,
                        COMPILER_64_LO(total_time_100),
                        COMPILER_64_LO(total_time_1000) - COMPILER_64_LO(total_time_100)*10
                        ));

                LOG_CLI((BSL_META(" +-----------------------------------------------------------------------------------------+\n\r")));
            }
        }
        total_time_32bit -= Soc_sand_ll_timer_cnt[ARAD_KBP_TIMERS_APPL].total_time;
        LOG_CLI((BSL_META(" Total Time for %d Entries: %u[us]\n\r"), num_of_entries,total_time_32bit));
        LOG_CLI((BSL_META(" Total Time per Entry: %8.3f[us]\n\r"), (double)total_time_32bit/(double)num_of_entries));
		rate = (double)(1000000*num_of_entries)/(double)total_time_32bit;
        LOG_CLI((BSL_META(" Entries Insertaion Rate: %.0f[1/sec]\n\r"), rate));
        LOG_CLI((BSL_META(" +_________________________________________________________________________________________+\n\r")));

    } /* Timer hits != 0 */
	return (int)rate;
}

STATIC int
 dpp_kbp_acl_print_timers(int num_of_entries)
{
	uint32
	  cnt_ndx,i;
    uint32
        total_time_32bit;
	COMPILER_UINT64
	  total_time_1000, total_time_100, timer_total ;
	double rate = -1;

	LOG_CLI((BSL_META("\r\n")));
	if (Soc_sand_ll_timer_total == 0){
        LOG_CLI((BSL_META("No timers were hit, total measured execution time: 0\n\r")));
	}else{
        LOG_CLI((BSL_META(" KBP ACL Timers Measurements by layers (net time per timer) \n\r")));
        LOG_CLI((BSL_META(" +-----------------------------------------------------------------------------------------+\n\r")));
        LOG_CLI((BSL_META(" | Timer Name                             |Hit Count |Total Time[us] |Per Hit[us] |Percent |\n\r")));
        LOG_CLI((BSL_META(" +-----------------------------------------------------------------------------------------+\n\r")));

        COMPILER_64_SET(timer_total, 0, Soc_sand_ll_timer_cnt[ARAD_KBP_TIMERS_APPL].total_time);
        total_time_32bit = Soc_sand_ll_timer_cnt[ARAD_KBP_TIMERS_APPL].total_time;
        for (cnt_ndx = ARAD_KBP_ACL_TIMERS_NOF_TIMERS-1; cnt_ndx >= ARAD_KBP_TIMERS_APPL; cnt_ndx--){

            if (Soc_sand_ll_timer_cnt[cnt_ndx].nof_hits != 0){

				if (cnt_ndx > ARAD_KBP_TIMERS_SOC) {
                    /* Its a kbp timer -
                     *  Reduce its total time from the higher layres total time
                     */
					for (i=0;i<=ARAD_KBP_TIMERS_SOC;i++) {
                        Soc_sand_ll_timer_cnt[i].total_time -= Soc_sand_ll_timer_cnt[cnt_ndx].total_time;
					}
				}else{
                    /* Its a soc/bcm/full timer -
                     *  Reduce its total time from the higher layres total time
                     */
					for (i=ARAD_KBP_TIMERS_APPL;i<cnt_ndx;i++) {
                        Soc_sand_ll_timer_cnt[i].total_time -= Soc_sand_ll_timer_cnt[cnt_ndx].total_time;
					}
                }
				COMPILER_64_SET(total_time_1000, 0, Soc_sand_ll_timer_cnt[cnt_ndx].total_time);
                COMPILER_64_SET(total_time_100, 0, Soc_sand_ll_timer_cnt[cnt_ndx].total_time);
                COMPILER_64_UMUL_32(total_time_1000,1000);
                COMPILER_64_UMUL_32(total_time_100,100);
                COMPILER_64_UDIV_64(total_time_1000,timer_total);
                COMPILER_64_UDIV_64(total_time_100,timer_total);
                LOG_CLI((BSL_META(" |%-40s| %-9d|%-15d|%-12d|%3d.%1d%%  |\n\r"), 
                        Soc_sand_ll_timer_cnt[cnt_ndx].name, 
                        Soc_sand_ll_timer_cnt[cnt_ndx].nof_hits,
                        Soc_sand_ll_timer_cnt[cnt_ndx].total_time, 
                        Soc_sand_ll_timer_cnt[cnt_ndx].total_time / Soc_sand_ll_timer_cnt[cnt_ndx].nof_hits,
                        COMPILER_64_LO(total_time_100),
                        COMPILER_64_LO(total_time_1000) - COMPILER_64_LO(total_time_100)*10
                        ));

                LOG_CLI((BSL_META(" +-----------------------------------------------------------------------------------------+\n\r")));
            }
        }
        total_time_32bit -= Soc_sand_ll_timer_cnt[ARAD_KBP_TIMERS_APPL].total_time;
        LOG_CLI((BSL_META(" Total Time for %d Entries: %u[us]\n\r"), num_of_entries,total_time_32bit));
        LOG_CLI((BSL_META(" Total Time per Entry: %8.3f[us]\n\r"), (double)total_time_32bit/(double)num_of_entries));
		rate = (double)(1000000*num_of_entries)/(double)total_time_32bit;
		LOG_CLI((BSL_META(" Entries Insertaion Rate: %.0f[1/sec]\n\r"), rate));
        LOG_CLI((BSL_META(" +_________________________________________________________________________________________+\n\r")));

    } /* Timer hits != 0 */
	return (int)rate;
}

STATIC cmd_result_t
cmd_dpp_kbp_cpu_record_send(int unit, args_t *a)
{
    soc_reg_above_64_val_t msb, lsb;
    parse_table_t pt; 
    cmd_result_t rv = CMD_OK;
    uint32 
        opcode,
        soc_sand_rv, core;
    int lsb_enable;
    
    SOC_REG_ABOVE_64_CLEAR(msb);
    SOC_REG_ABOVE_64_CLEAR(lsb);

    if (ARG_CNT(a) > 0) {
        parse_table_init(0,&pt);

        parse_table_add(&pt,"Core",PQ_INT , (void *) (0), &core, NULL);
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
        soc_sand_rv = aradplus_kbp_cpu_record_send(unit, core, opcode, msb, lsb, lsb_enable, NULL);
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
    uint32 addr, addr_short, nbo_addr, core;
    arad_kbp_rop_write_t wr_data;
    uint32 soc_sand_rv;

    if (ARG_CNT(a) > 0) {
        parse_table_init(0,&pt);

        parse_table_add(&pt,"Core",PQ_INT , (void *) (0), &core, NULL);
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
    
    soc_sand_rv = arad_kbp_rop_write(unit, core, &wr_data);
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
    uint32 addr, nbo_addr, core;
    int i;
    uint32 soc_sand_rv;

    if (ARG_CNT(a) > 0) {
        parse_table_init(0,&pt);

        parse_table_add(&pt,"Core",PQ_INT , (void *) (0), &core, NULL);
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
  
    soc_sand_rv = arad_kbp_rop_read(unit, core, &rd_data);
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
#if (defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__))
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

#endif
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
#if (defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__))
    int val = 0;
    cmd_result_t ret;
    int kbp_reset_address, kbp_c_reset_bit, kbp_s_reset_bit;
    kbp_init_user_data_t* user_data = handle;

    if (SOC_IS_JERICHO(0)) {
        if (user_data->kbp_mdio_id == SOC_DPP_CONFIG(0)->arad->init.elk.kbp_mdio_id[0]) {
            kbp_c_reset_bit = 0; 
            kbp_s_reset_bit = 1;
        } else {
            kbp_c_reset_bit = 2; 
            kbp_s_reset_bit = 3;
        }
        kbp_reset_address = 0x20;
    } else {
        kbp_c_reset_bit = 7;
        kbp_s_reset_bit = 6;
        kbp_reset_address = 0x2;
    }

    ret = cpu_i2c_read(0x40, kbp_reset_address, CPU_I2C_ALEN_LONG_DLEN_LONG, &val);
    if( ret != CMD_OK){
        return KBP_INTERNAL_ERROR;
    }

    if (!s_reset_low) {
        SHR_BITSET(&val, kbp_s_reset_bit);
    } else {
        SHR_BITCLR(&val, kbp_s_reset_bit);
    }
    if(!c_reset_low){
        SHR_BITSET(&val, kbp_c_reset_bit);
    } else {
        SHR_BITCLR(&val, kbp_c_reset_bit);
    }

    ret = cpu_i2c_write(0x40, kbp_reset_address, CPU_I2C_ALEN_LONG_DLEN_LONG, val);
    if( ret != CMD_OK){
        return KBP_INTERNAL_ERROR;
    }
#endif

    /*in case of out of reset sleep for link lock*/
    if((!c_reset_low) && (!s_reset_low)){
        sal_usleep(150000);
    }
    return KBP_OK;
}

int dpp_kbp_file_open(int unit, char *filename, int device_type)
{
    uint8 is_warmboot;
    char prefixed_file_name[SOC_PROPERTY_NAME_MAX + 256];
    char    *stable_filename = NULL;

    FILE **file_fp = NULL;
    if (NULL == filename) {
        return 0; 
    }

    if (device_type == KBP_DEVICE_12K) {
        file_fp = &kbp_file_fp[unit];
    }
    else if (device_type == KBP_DEVICE_KAPS){
        file_fp = &kaps_file_fp[unit];
    }


    if (*file_fp == NULL) {
        is_warmboot = SOC_WARM_BOOT(unit);

        prefixed_file_name[0] = '\0' ;

        stable_filename = soc_property_get_str(unit, spn_STABLE_FILENAME);

        /* prefixing with unique file name to enable more than one parallel run from same SDK folder  */
        /* assuming stable_filename is unique for each separate run */
        if (NULL != stable_filename) {
            sal_strncat(prefixed_file_name, stable_filename, sizeof(prefixed_file_name) - 1);

            sal_strncat(prefixed_file_name, "_", sizeof(prefixed_file_name) - sal_strlen(prefixed_file_name) - 1);
        }
        sal_strncat(prefixed_file_name, filename, sizeof(prefixed_file_name) - sal_strlen(prefixed_file_name) - 1);


        if ((*file_fp =
             sal_fopen(prefixed_file_name, is_warmboot ? "r+" : "w+")) == 0) {
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


int dpp_kbp_init_appl(int unit, uint32 second_kbp_supported, ARAD_INIT_ELK *elk_ptr) {

    int rv = BCM_E_NONE;
    uint32 ilkn_num_lanes, ilkn_metaframe;
    int ilkn_rate;
    rx_los_state_t port_stable_state;
    bcm_port_t kbp_port;
    uint32 soc_sand_rv;
    uint32 core = 0;

    /* Give the Interface time to sync/adjust */
    soc_sand_rv = arad_kbp_ilkn_interface_param_get(unit, core, &kbp_port, &ilkn_num_lanes, &ilkn_rate, &ilkn_metaframe);
    rv = handle_sand_result(soc_sand_rv);
    if (BCM_FAILURE(rv)) {
       cli_out("Error:  arad_kbp_ilkn_interface_param_get() Failed\n");
       return rv;
    }
	if(!SAL_BOOT_PLISIM){
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
	}
    soc_sand_rv = dpp_kbp_file_open(unit, "kbp", KBP_DEVICE_12K);

    rv = handle_sand_result(soc_sand_rv);
    if (BCM_FAILURE(rv)) {
        cli_out("Error: dpp_kbp_file_open(%d)\n", unit);
        return rv;
    }

    arad_kbp_warmboot_register(unit, kbp_file_fp[unit], &dpp_kbp_file_read_func, &dpp_kbp_file_write_func);

    soc_sand_rv = arad_kbp_init_app(unit, second_kbp_supported, elk_ptr);
    rv = handle_sand_result(soc_sand_rv);
    if (BCM_FAILURE(rv)) {
        cli_out("Error: arad_kbp_init_appl(%d, &elk)\n", unit);
        return rv;
    }
    return rv;
}

int dpp_kaps_init(int unit) {

    cmd_result_t rv = CMD_OK;
    uint32 soc_sand_rv;

    soc_sand_rv = dpp_kbp_file_open(unit, "kaps", KBP_DEVICE_KAPS);
    rv = handle_sand_result(soc_sand_rv);
    if (BCM_FAILURE(rv)) {
        cli_out("Error: dpp_kbp_file_open(%d)\n", unit);
        return rv;
    }

    jer_kaps_warmboot_register(unit, kaps_file_fp[unit], &dpp_kbp_file_read_func, &dpp_kbp_file_write_func);

    soc_sand_rv = jer_kaps_init_app(unit);
    rv = handle_sand_result(soc_sand_rv);
    if (BCM_FAILURE(rv)) {
        cli_out("Error: jer_kaps_init_app(%d)\n", unit);
        return rv;
    }
    return rv;
}

STATIC cmd_result_t
_cmd_dpp_kbp_init_appl(int unit, uint32 second_elk_enable, args_t *a)
{
    parse_table_t pt; 
    ARAD_INIT_ELK elk, *elk_ptr=NULL;
    uint32 enable;

    sal_memset(&elk, 0x0, sizeof(elk));

    if (ARG_CNT(a) > (0 + second_elk_enable)) {
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

    if (dpp_kbp_init_appl(unit, second_elk_enable, elk_ptr) < 0) {
        return CMD_FAIL;
    }
    return CMD_OK;
}

STATIC cmd_result_t
cmd_dpp_kbp_init_appl(int unit, args_t *a)
{
    parse_table_t pt;
    uint32 second_elk_enable=0;

    int num_of_cores = SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores;

    if(num_of_cores == 2){
       second_elk_enable = 1;
    }

    if (ARG_CNT(a) > 0) {
        parse_table_init(0,&pt);
        parse_table_add(&pt,"SecondElkEnable",PQ_INT , (void *) (0), &second_elk_enable, NULL);
        
        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n",
                    ARG_CMD(a), ARG_CUR(a));
            return CMD_USAGE;
        }
    }    

    if (_cmd_dpp_kbp_init_appl(unit, second_elk_enable, a) < 0) {
        return CMD_FAIL;
    }
    return CMD_OK;
}

STATIC cmd_result_t
cmd_dpp_kbp_kaps_init(int unit, args_t *a)
{
    parse_table_t pt; 

    if (ARG_CNT(a) > 0) {
        parse_table_init(0,&pt);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n",
                    ARG_CMD(a), ARG_CUR(a));
            return CMD_USAGE;
        }
    }

    if (dpp_kaps_init(unit) < 0) {
        return CMD_FAIL;
    }
    return CMD_OK;
}

STATIC cmd_result_t
cmd_dpp_kaps_search(int unit, args_t *a)
{
    parse_table_t pt;
    uint32 ip_search_ids[JER_KAPS_NOF_SEARCHES] = {0};

    uint32 sum_of_searches = 0;
    uint32 search_id = JER_KAPS_NOF_SEARCHES;

    ip6_addr_t dip_v6 = {0}, sip_v6 = {0}, mc_group_v6 = {0}; /*_SHR_L3_IP6_ADDRLEN (16) Bytes*/
    SOC_SAND_PP_IPV6_ADDRESS dip_v6_32,
           sip_v6_32,
           mc_group_v6_32;
    uint32 dip_v4 = 0, sip_v4 = 0, mc_group_v4 = 0, vrf = 0, inrif = 0;
    cmd_result_t rv = CMD_OK;

    int32 i;
    uint8 key[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES][JER_KAPS_KEY_BUFFER_NOF_BYTES];

    JER_KAPS_SEARCH_CONFIG search_cfg;

    uint32  prefix_len;
    ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    uint32 mask[SOC_SAND_PP_IPV6_ADDRESS_NOF_UINT32S] = {SOC_SAND_U32_MAX, SOC_SAND_U32_MAX, SOC_SAND_U32_MAX, SOC_SAND_U32_MAX};
    SOC_DPP_DBAL_SW_TABLE_IDS table_id;
    SOC_DPP_DBAL_TABLE_INFO table;

    memset(key, 0, sizeof(key[0][0]) * JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES * JER_KAPS_KEY_BUFFER_NOF_BYTES);
    for (i =0; i < JER_KAPS_NOF_SEARCHES; i++) {
        ip_search_ids[i] = 0;
    }

    if (ARG_CNT(a) > 0) {
        parse_table_init(0,&pt);
        parse_table_add(&pt,"IPV4_UC", PQ_INT, (void *) (0), &ip_search_ids[JER_KAPS_IPV4_UC_SEARCH_ID], NULL);
        parse_table_add(&pt,"IPV6_UC", PQ_INT, (void *) (0), &ip_search_ids[JER_KAPS_IPV6_UC_SEARCH_ID], NULL);
        parse_table_add(&pt,"IPV4_MC", PQ_INT, (void *) (0), &ip_search_ids[JER_KAPS_IPV4_MC_SEARCH_ID], NULL);
        parse_table_add(&pt,"IPV6_MC", PQ_INT, (void *) (0), &ip_search_ids[JER_KAPS_IPV6_MC_SEARCH_ID], NULL);
        /*search_id allows to directly pick the number of the desired search*/
        parse_table_add(&pt,"SEARCH_ID", PQ_INT, (void *) (0), &search_id, NULL);

        parse_table_add(&pt,"SIP", PQ_IP, (void *) (0), &sip_v4, NULL);
        parse_table_add(&pt,"SIP6", PQ_IP6, (void *) (0), &sip_v6, NULL);
        parse_table_add(&pt,"DIP", PQ_IP, (void *) (0), &dip_v4, NULL);
        parse_table_add(&pt,"DIP6", PQ_IP6, (void *) (0), &dip_v6, NULL);
        parse_table_add(&pt,"MC_GROUP", PQ_IP, (void *) (0), &mc_group_v4, NULL);
        parse_table_add(&pt,"MC_GROUP6", PQ_IP6, (void *) (0), &mc_group_v6, NULL);
        parse_table_add(&pt,"INRIF", PQ_INT, (void *) (0), &inrif, NULL);
        parse_table_add(&pt,"VRF", PQ_INT, (void *) (0), &vrf, NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n",
            ARG_CMD(a), ARG_CUR(a));
            return CMD_USAGE;
        }

        parse_arg_eq_done(&pt);

        for (i =0; i < JER_KAPS_NOF_SEARCHES; i++) {
            if (ip_search_ids[i] != 0) {
                search_id = i;
            }
            sum_of_searches += ip_search_ids[i];
        }

        /*Can only choose one JER_KAPS_IP_SEARCH_ID*/
        if (sum_of_searches > 1 || search_id >= JER_KAPS_NOF_SEARCHES) {
            cli_out("%s: Invalid option: %s\n",
                    ARG_CMD(a), ARG_CUR(a));
            return CMD_USAGE;
        }

        jer_kaps_search_config_get(unit, search_id, &search_cfg);

        if (!search_cfg.valid_tables_num) {
            cli_out("%s: Invalid option: %s\n",
                    ARG_CMD(a), ARG_CUR(a));
            return CMD_USAGE;
        }

        /* Convert from uint8 to uint32*/
        rv = _bcm_l3_bcm_ipv6_addr_to_sand_ipv6_addr(unit, sip_v6, &sip_v6_32);
        if (BCM_FAILURE(rv)) {
            cli_out("Error: jer_kaps_search_generic(%d), uint8 to uint32 failure\n", unit);
            return rv;
        }
        rv = _bcm_l3_bcm_ipv6_addr_to_sand_ipv6_addr(unit, dip_v6, &dip_v6_32);
        if (BCM_FAILURE(rv)) {
            cli_out("Error: jer_kaps_search_generic(%d), uint8 to uint32 failure\n", unit);
            return rv;
        }
        rv = _bcm_l3_bcm_ipv6_addr_to_sand_ipv6_addr(unit, mc_group_v6, &mc_group_v6_32);
        if (BCM_FAILURE(rv)) {
            cli_out("Error: jer_kaps_search_generic(%d), uint8 to uint32 failure\n", unit);
            return rv;
        }

        /* Initialize qual_vals to include all the possible quals*/
        DBAL_QUAL_VALS_CLEAR(qual_vals);
        DBAL_QUAL_VAL_ENCODE_FWD_IPV4_SIP(&qual_vals[0], sip_v4, SOC_SAND_NOF_BITS_IN_UINT32);
        DBAL_QUAL_VAL_ENCODE_IPV6_SIP_LOW(&qual_vals[1], sip_v6_32.address, mask);
        DBAL_QUAL_VAL_ENCODE_IPV6_SIP_HIGH(&qual_vals[2], sip_v6_32.address, mask);
        if (mc_group_v4) { /* The search uses the mc_group or the dip as the DIP qual*/
            DBAL_QUAL_VAL_ENCODE_FWD_IPV4_DIP(&qual_vals[3], mc_group_v4, SOC_SAND_NOF_BITS_IN_UINT32);
        } else {
            DBAL_QUAL_VAL_ENCODE_FWD_IPV4_DIP(&qual_vals[3], dip_v4, SOC_SAND_NOF_BITS_IN_UINT32);
        }
        if (mc_group_v6[0]) {
            DBAL_QUAL_VAL_ENCODE_IPV6_DIP_LOW(&qual_vals[4], mc_group_v6_32.address, mask);
            DBAL_QUAL_VAL_ENCODE_IPV6_DIP_HIGH(&qual_vals[5], mc_group_v6_32.address, mask);
        } else {
            DBAL_QUAL_VAL_ENCODE_IPV6_DIP_LOW(&qual_vals[4], dip_v6_32.address, mask);
            DBAL_QUAL_VAL_ENCODE_IPV6_DIP_HIGH(&qual_vals[5], dip_v6_32.address, mask);
        }
        DBAL_QUAL_VAL_ENCODE_IN_RIF(&qual_vals[6], inrif, SOC_SAND_U32_MAX);

        for (i=0; i < JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES; i++) {

            /*vrf!=0 only for private*/
            if (search_cfg.tbl_id[i] < JER_KAPS_IP_PUBLIC_INDEX) {
                DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[7], vrf, SOC_SAND_U32_MAX);
            } else{
                DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[7], 0, SOC_SAND_U32_MAX);
            }

            rv = jer_pp_kaps_table_id_to_dbal_translate(unit, search_cfg.tbl_id[i], &table_id);
            if (BCM_FAILURE(rv)) {
                cli_out("Error: jer_kaps_search_generic(%d), table translation failed\n", unit);
                return rv;
            }

            rv = sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table);
            if (BCM_FAILURE(rv)) {
                cli_out("Error: jer_kaps_search_generic(%d), key buffer construction failed\n", unit);
                return rv;
            }

            rv = arad_pp_dbal_entry_key_to_kbp_buffer(unit, &table, JER_KAPS_KEY_BUFFER_NOF_BYTES, qual_vals, &prefix_len, key[i]);
            if (BCM_FAILURE(rv)) {
                cli_out("Error: jer_kaps_search_generic(%d), key buffer construction failed\n", unit);
                return rv;
            }
        }
    }

    rv = jer_kaps_search_generic(unit, search_id, key, NULL, NULL, NULL, NULL); /* No need for return values*/
    if (BCM_FAILURE(rv)) {
        cli_out("Error: jer_kaps_search_generic(%d)\n", unit);
        return rv;
    }

    return rv;
}

STATIC cmd_result_t
cmd_dpp_kaps_diag_01(int unit, args_t *a)
{
    uint32 soc_sand_rv;
    cmd_result_t rv = CMD_OK;

    soc_sand_rv = jer_pp_kaps_diag_01(unit);
    if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
        cli_out("Error: jer_kaps_diag_01(%d)\n", unit);
        rv = CMD_FAIL;
    }

    return rv;
}

STATIC cmd_result_t
cmd_dpp_kaps_diag_02(int unit, args_t *a)
{
    uint32 soc_sand_rv;
    cmd_result_t rv = CMD_OK;

    soc_sand_rv = jer_pp_kaps_diag_02(unit);
    if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
        cli_out("Error: jer_kaps_diag_02(%d)\n", unit);
        rv = CMD_FAIL;
    }

    return rv;
}

STATIC cmd_result_t
cmd_dpp_kaps_show(int unit, args_t *a)
{
    SOC_DPP_DBAL_SW_TABLE_IDS tbl_idx = SOC_DPP_DBAL_SW_TABLE_ID_INVALID;
    uint32 soc_sand_rv;
    parse_table_t pt;
    uint32 count_only = 0;

    SOC_DPP_DBAL_TABLE_INFO dbal_table;

    if (ARG_CNT(a) > 0) {
        parse_table_init(0,&pt);
        parse_table_add(&pt,"DBAL_TBL_ID", PQ_INT, (void *) (SOC_DPP_DBAL_SW_TABLE_ID_INVALID), &tbl_idx, NULL);
        parse_table_add(&pt,"CountOnly", PQ_INT, (void *) (0), &count_only, NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n",
                    ARG_CMD(a), ARG_CUR(a));
            return CMD_USAGE;
        }
    }


    if (tbl_idx != SOC_DPP_DBAL_SW_TABLE_ID_INVALID) {
        soc_sand_rv = jer_kaps_show_table(unit, tbl_idx, count_only ? 0 : 1);

        if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
            cli_out("Error: dpp_kaps_show(%d)\n", unit);
            return CMD_FAIL;
        }
    } else {
        /*print all if not given a specific table*/
        for (tbl_idx = 0; tbl_idx < SOC_DPP_DBAL_SW_NOF_TABLES; tbl_idx++) {

            soc_sand_rv = sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, tbl_idx, &dbal_table);
            if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
                cli_out("Error: dpp_kaps_show(%d) - retrieve dbal table\n", unit);
                return CMD_FAIL;
            }

            /* Only print initiated tables in print all */
            if ((dbal_table.physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS) && (dbal_table.is_table_initiated)) {
                soc_sand_rv = jer_kaps_show_table(unit, tbl_idx, count_only ? 0 : 1);

                if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
                    cli_out("Error: dpp_kaps_show(%d)\n", unit);
                    return CMD_FAIL;
                }
            }
        }
    }
    return CMD_OK;
}

STATIC cmd_result_t
cmd_dpp_kaps_arm_thread(int unit, args_t *a)
{
    uint32 soc_sand_rv;
    parse_table_t pt;
    uint32 enable_dma_thread = 1;
    uint32 print_status = 1;
    uint32 wait_arm = 0;
    uint32 enable_arm = 2;

    if (ARG_CNT(a) > 0) {
        parse_table_init(0,&pt);

        parse_table_add(&pt,"enable_dma_thread", PQ_INT, (void *) (1), &enable_dma_thread, NULL);
        parse_table_add(&pt,"print_status", PQ_INT, (void *) (1), &print_status, NULL);
        parse_table_add(&pt,"wait_arm", PQ_INT, (void *) (0), &wait_arm, NULL);
        parse_table_add(&pt,"enable_arm", PQ_INT, (void *) (2), &enable_arm, NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n",
                    ARG_CMD(a), ARG_CUR(a));
            return CMD_USAGE;
        }
    }

    if (enable_arm == 1) {
        soc_sand_rv = dpp_kaps_arm_image_load_default(unit);
        if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
            cli_out("Error: dpp_kaps_arm_image_load_default(%d)\n", unit);
            return CMD_FAIL;
        }
    }

    soc_sand_rv = jer_pp_xpt_dma_state(unit, print_status, enable_dma_thread, wait_arm);
    if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
        cli_out("Error: jer_pp_xpt_dma_state(%d)\n", unit);
        return CMD_FAIL;
    }

    if (enable_arm == 0) {
        soc_sand_rv = jer_pp_xpt_arm_deinit(unit);
        if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
            cli_out("Error: jer_pp_xpt_arm_deinit(%d)\n", unit);
            return CMD_FAIL;
        }
    }

    return CMD_OK;
}

STATIC cmd_result_t
dpp_kaps_arm_image_load(int unit, char *file, int cpu_halt, int load_file, int init_tcm)
{
    int rv = CMD_OK, 
        i = 0, 
        entry_num = 0,
        buf_size = KBP_CMD_DPP_KAPS_ARM_IMAGE_LOAD_BUF_SIZE;

    FILE * volatile fp = NULL;

    uint32 input_32[KBP_CMD_DPP_KAPS_ARM_IMAGE_LOAD_BUF_SIZE + 2];
    uint32 tmp_input_32 = 0x0;

    if (load_file != 0x0) {
        /*Initialize the TCM memories*/
        if (init_tcm) {
            rv = jer_pp_xpt_arm_init(unit);
            if (rv != SOC_SAND_OK) {
                LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "Error: Failed to jer_pp_xpt_arm_init()\n")));
                return CMD_FAIL;
            }
        }

        if (file == NULL) {
            LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "Error: No file specified\n")));
            return CMD_FAIL;
        }

        /* Open File */
        fp = sal_fopen(file, "rt");
        if (fp == NULL) {
            LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "Error: Unable to open file: %s\n"), file));
            return CMD_FAIL;
        }

        /* Clear input buffer */
        sal_memset(input_32, 0x0, buf_size * sizeof(uint32));

        /* Loop on reading the ARM Application. reading line by line. */
        while (sal_fgets((char *)input_32, (buf_size * sizeof(uint32)) + 1, fp)) {

            LOG_VERBOSE(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "entry_num=%d\n"), entry_num));
            LOG_VERBOSE(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "input_32:")));
            for (i = 0; i < ((buf_size * sizeof(uint32)) + 2) / sizeof(uint32); i++) {
                LOG_VERBOSE(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "0x%08x "), input_32[i]));
            }
            LOG_VERBOSE(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "\n")));

            /* Convert the Binary (ASCII) file into HEX, and trim the ECC byte */
            tmp_input_32  = xdigit2i(BYTE_2(input_32[2]));
            tmp_input_32 |= xdigit2i(BYTE_3(input_32[2])) << 4;
            tmp_input_32 |= xdigit2i(BYTE_0(input_32[1])) << 8;
            tmp_input_32 |= xdigit2i(BYTE_1(input_32[1])) << 12;
            tmp_input_32 |= xdigit2i(BYTE_2(input_32[1])) << 16;
            tmp_input_32 |= xdigit2i(BYTE_3(input_32[1])) << 20;
            tmp_input_32 |= xdigit2i(BYTE_0(input_32[0])) << 24;
            tmp_input_32 |= xdigit2i(BYTE_1(input_32[0])) << 28;
            input_32[1] = tmp_input_32;
            tmp_input_32  = xdigit2i(BYTE_2(input_32[4]));
            tmp_input_32 |= xdigit2i(BYTE_3(input_32[4])) << 4;
            tmp_input_32 |= xdigit2i(BYTE_0(input_32[3])) << 8;
            tmp_input_32 |= xdigit2i(BYTE_1(input_32[3])) << 12;
            tmp_input_32 |= xdigit2i(BYTE_2(input_32[3])) << 16;
            tmp_input_32 |= xdigit2i(BYTE_3(input_32[3])) << 20;
            tmp_input_32 |= xdigit2i(BYTE_0(input_32[2])) << 24;
            tmp_input_32 |= xdigit2i(BYTE_1(input_32[2])) << 28;
            input_32[0] = tmp_input_32;

            LOG_VERBOSE(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "entry_num=%d\n"), entry_num));
            LOG_VERBOSE(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "input_32:")));
            for (i = 0; i < ((buf_size * sizeof(uint32)) + 2) / sizeof(uint32); i++) {
                LOG_VERBOSE(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "0x%08x "), input_32[i]));
            }
            LOG_VERBOSE(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "\n")));

            /* Writing the file to KAPS_TCM from offset 0x0 (start of ATCM) using SBUS */
            rv = jer_pp_xpt_arm_load_file_entry(unit, input_32, entry_num);
            if (rv != SOC_SAND_OK) {
                LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "Error: Failed to jer_pp_xpt_arm_start()\n")));
                return CMD_FAIL;
            }

            entry_num += 1;
        }

        rv = sal_fclose(fp);
        if (rv != SOC_SAND_OK) {
            LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "Error: Failed to sal_fclose()\n")));
            return CMD_FAIL;
        }
    }

    rv = jer_pp_xpt_arm_start_halt(unit, cpu_halt);
    if (rv != SOC_SAND_OK) {
        LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "Error: Failed to jer_pp_xpt_arm_start()\n")));
        return CMD_FAIL;
    }

    return CMD_OK;
}

int dpp_kaps_arm_image_load_default(int unit)
{
    cmd_result_t rv = CMD_OK;
    uint32 soc_sand_rv;

    /*Initialize the TCM memories*/
    rv = jer_pp_xpt_arm_init(unit);
    if (rv != SOC_SAND_OK) {
        LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "Error: Failed to jer_pp_xpt_arm_init()\n")));
        return CMD_FAIL;
    }

    /*Load default software image to ARM*/
    rv = jer_pp_kaps_arm_image_load_default(unit);
    if (BCM_FAILURE(rv)) {
        cli_out("Error: jer_pp_kaps_arm_image_load_default(%d)\n", unit);
        return rv;
    }

    /*Unhalt ARM*/
    rv = jer_pp_xpt_arm_start_halt(unit, 0 /*cpu_halt*/);
    if (rv != SOC_SAND_OK) {
        LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "Error: Failed to jer_pp_xpt_arm_start()\n")));
        return CMD_FAIL;
    }

    /*Enable DMA thread*/
    soc_sand_rv = jer_pp_xpt_dma_state(unit, 0 /*print_status*/, 1 /*enable_dma_thread*/, 0 /*wait_arm*/);
    if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
        cli_out("Error: jer_pp_xpt_dma_state(%d)\n", unit);
        return CMD_FAIL;
    }

    return rv;
}

STATIC cmd_result_t
cmd_dpp_kaps_arm_image_load(int unit, args_t *a)
{
    int rv = CMD_OK,
        cpu_halt = 0,
        load_file = 1,
        init_tcm = 1;
    parse_table_t pt;

    char *file = NULL;

    if (ARG_CNT(a) > 0) {
        parse_table_init(0, &pt);

        parse_table_add(&pt, "File", PQ_STRING, (void *)0, &file, NULL);
        parse_table_add(&pt, "CpuHalt", PQ_INT, (void *)0, &cpu_halt, NULL);
        parse_table_add(&pt, "LoadFile", PQ_INT, (void *)1, &load_file, NULL);
        parse_table_add(&pt, "InitTCM", PQ_INT, (void *)1, &init_tcm, NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "%s: Invalid option: %s\n"), ARG_CMD(a), ARG_CUR(a)));
            return CMD_FAIL;
        }
    }

    LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "%s: buf_size=%d, cpu_halt=%d, load_file=%d, init_tcm=%d\n"), ARG_CMD(a), KBP_CMD_DPP_KAPS_ARM_IMAGE_LOAD_BUF_SIZE, cpu_halt, load_file, init_tcm));

    rv = dpp_kaps_arm_image_load(unit, file, cpu_halt, load_file, init_tcm);
    if (rv != SOC_SAND_OK) {
        LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "%s: Error: Failed to dpp_kaps_arm_image_load()\n"), ARG_CMD(a)));
        return CMD_FAIL;
    }

    return CMD_OK;
}
STATIC cmd_result_t
cmd_dpp_sdk_ver(int unit, args_t *a)
{
    const char *ver_str;

    if (ARG_CNT(a) > 0) {
        cli_out("%s: Invalid option: %s\n",
                ARG_CMD(a), ARG_CUR(a));
        return CMD_USAGE;
    }

    ver_str = kbp_device_get_sdk_version();

    if (ver_str == NULL) {
        cli_out("Error: dpp_kaps_get_sdk_ver(%d), sdk_ver string pointer is NULL\n", unit);
        return CMD_FAIL;
    }
    cli_out(ver_str);
    cli_out("\n");

    return CMD_OK;
}

STATIC cmd_result_t
cmd_dpp_kbp_deinit_appl(int unit, args_t *a)
{
    uint32 soc_sand_rv;
    cmd_result_t rv = CMD_OK;
    parse_table_t pt;
    uint32 second_elk_enable=0;

    if (ARG_CNT(a) > 0) {
        parse_table_init(0,&pt);
				parse_table_add(&pt,"SecondElkEnable",PQ_INT , (void *) (0), &second_elk_enable, NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n",
                    ARG_CMD(a), ARG_CUR(a));
            return CMD_USAGE;
        }
    }

    soc_sand_rv = arad_kbp_deinit_app(unit, second_elk_enable);
    if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
        cli_out("Error: arad_kbp_deinit_appl(%d, &elk)\n", unit);
        rv = CMD_FAIL;
    }

    return rv;
}

STATIC cmd_result_t
cmd_dpp_kaps_deinit(int unit, args_t *a)
{
    uint32 soc_sand_rv;
    cmd_result_t rv = CMD_OK;

    soc_sand_rv = jer_kaps_deinit_app(unit);
    if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
        cli_out("Error: arad_kbp_deinit_appl(%d, &elk)\n", unit);
        rv = CMD_FAIL;
    }

    return rv;
}

int dpp_kbp_init_kbp_interface(int unit, uint32 core, uint32 kbp_mdio_id, uint32 kbp_ilkn_rev) {

    uint32 soc_sand_rv, flags;
    int rv = BCM_E_NONE;
    soc_pbmp_t ilkn_pbmp;
    soc_port_t    port, kbp_port = SOC_GPORT_INVALID;
    ARAD_INIT_ELK* elk;
    int kbp_gpio_address, kbp_gpio_mask;
    int tmp_core;

    elk = &(SOC_DPP_CONFIG(unit)->arad->init.elk);
    
    if ((elk->kbp_recover_enable) && !SOC_WARM_BOOT(unit)) {
        ilkn_pbmp = PBMP_IL_ALL(unit);
        SOC_PBMP_ITER(ilkn_pbmp, port) {
            rv = soc_port_sw_db_flags_get(unit, port, &flags);
            if(!BCM_FAILURE(rv)){
                rv = soc_port_sw_db_core_get(unit, port, &tmp_core);
                if(BCM_FAILURE(rv)){
                    cli_out("Error in %s():soc_port_sw_db_core_get failed\n", FUNCTION_NAME());
                    return rv;
                }
                if (SOC_PORT_IS_ELK_INTERFACE(flags) && (core == tmp_core)) {
                    kbp_port = port;
                }
            }
        }

        /* in case we didn't find a valid port - exit with BCM_E_PORT error*/
        if ( kbp_port == SOC_GPORT_INVALID) {
            rv = BCM_E_PORT;
            cli_out("Error in %s(): KBP port was not defined\n", FUNCTION_NAME());
            return rv;
        }
    }


    if (SOC_IS_JERICHO(unit)) {
        kbp_gpio_address = 0x1f;
        kbp_gpio_mask = 0xf;
    } else {
        kbp_gpio_address = 0x3;
        kbp_gpio_mask = 0xc0;
    }
    
#if (defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__))
    /* Set kbp gpio's to Output */
    rv = cpu_i2c_write(0x40, kbp_gpio_address, CPU_I2C_ALEN_LONG_DLEN_LONG, kbp_gpio_mask);
    if(BCM_FAILURE(rv)){
        cli_out("Error in %s(): cpu_i2c_write(). FAILED !!!\n", FUNCTION_NAME());
        return rv;
    }
#endif

    if ((elk->kbp_recover_enable) && !SOC_WARM_BOOT(unit)) {
        /* Temporary till bcm_port_control_set(unit, kbp_port,bcmPortControlRxEnable, 0) will be fixed for Jericho */

        rv = bcm_port_control_set(unit, kbp_port, bcmPortControlRxEnable, 0);/*port=32,47=rx enable enum */
        if(BCM_FAILURE(rv)){
            cli_out("Error: ELK RX disabled, arad_kbp_init_kbp_interface(%d, &elk)\n", unit);
            return rv;   
        }
        sal_usleep(2000);
    }

    /* Device Configuration Using MDIO */
    soc_sand_rv = arad_kbp_init_kbp_interface(unit, core, kbp_mdio_id, kbp_ilkn_rev, cmd_dpp_kbp_callback_reset_device);
    
    if ((elk->kbp_recover_enable) && (!SOC_WARM_BOOT(unit))){
        /* Temporary till bcm_port_control_set(unit, kbp_port,bcmPortControlRxEnable, 1) will be fixed for Jericho */

        rv = bcm_port_control_set(unit, kbp_port,bcmPortControlRxEnable, 1);/*port=32,47=rx enable enum */

        if(BCM_FAILURE(rv)){
            cli_out("Error: ELK RX enabled, arad_kbp_init_kbp_interface(%d, &elk)\n", unit);
            return rv;
        }
        sal_usleep(2000);
    }

    rv = BCM_E_NONE;
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
        kbp_mdio_id, kbp_ilkn_rev, core;

    if (ARG_CNT(a) > 0) {
        parse_table_init(0,&pt);

       /* KBP mdio ID format (kbp_mdio_id default for Negev is 0x101):
        * bits [5:6,8:9] - bus ID.
        * Arad has 8 external buses (0-7), two of these buses has connection on board (buses 4 and 5).
        * Assuming the KBP connected to bus 4 ==> 4b'0100 ==> bit[8]=1.
        * bit [7] - Internal select. Set to 0 for external phy access.
        * bits [0:4] - phy/kbp id ==> 0x1    
        */ 

        parse_table_add(&pt,"core",PQ_INT , (void *) (0), &core, NULL);
        parse_table_add(&pt,"mdio_id",PQ_INT , (void *) INT_TO_PTR(ARAD_KBP_APPL_MDIO_DEFAULT_ID), &kbp_mdio_id, NULL);
        parse_table_add(&pt,"ilkn_rev",PQ_INT , (void *) (ARAD_KBP_APPL_ILKN_REVERSE_DEFAULT), &kbp_ilkn_rev, NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n",
                    ARG_CMD(a), ARG_CUR(a));
            return CMD_USAGE;
        }
    } 

    if (dpp_kbp_init_kbp_interface(unit, core, kbp_mdio_id, kbp_ilkn_rev) < 0) {
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

/* measure the rate of entries insertion to kbp for IPV4 application*/
STATIC int
cmd_dpp_kbp_test_add_measure_rate(int unit, args_t *a)
{
    parse_table_t pt; 
    bcm_error_t rv = CMD_OK;

    int i,rate=0;
    int entry_num;
    int entry_num_print_mod;

    int fec = 0x20000400;
    int encap_id = 0x40001000;
    int vrf = 0;
    int host = 0x0a00ff01;
    int cache_mode = 0;
    /* bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x03, 0x00, 0x01}; */
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    bcm_l3_host_t l3host;

    if (ARG_CNT(a) >= 0) {
    
        parse_table_init(0,&pt);

        parse_table_add(&pt,"Host"              ,PQ_INT , (void *) (0x0a00ff01)         , &host     , NULL);
        parse_table_add(&pt,"Vrf"               ,PQ_INT , (void *) (0x0)                , &vrf      , NULL);
        parse_table_add(&pt,"Fec"               ,PQ_INT , (void *) (0x20000400)         , &fec      , NULL);
        parse_table_add(&pt,"EncapId"           ,PQ_INT , (void *) (0x40001000)         , &encap_id , NULL);
        parse_table_add(&pt,"EntryNum"          ,PQ_INT , (void *) (5000)               , &entry_num , NULL);
        parse_table_add(&pt,"EntryNumPrintMod"  ,PQ_INT , (void *) (1000)               , &entry_num_print_mod , NULL);
        parse_table_add(&pt,"ChachMode"         ,PQ_INT , (void *) (0)                  , &cache_mode , NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n",
                    ARG_CMD(a), ARG_CUR(a));
            return CMD_USAGE;
        }
        
        cli_out("host=0x%x, vrf=0x%x, fec=0x%x, encap_id=0x%x, entry_num=0x%x, entry_num_print_mod=0x%x cache_mode=%d\n", 
                host, vrf, fec, encap_id, entry_num, entry_num_print_mod,cache_mode);

    } else {
        return CMD_USAGE;   
    }

    soc_sand_ll_timer_clear();
    soc_sand_ll_timer_set("ARAD_KBP_TIMERS_APPL", ARAD_KBP_TIMERS_APPL);

    if(cache_mode != 0){
        soc_sand_ll_timer_set("ARAD_KBP_TIMERS_BCM", ARAD_KBP_TIMERS_BCM);
        bcm_switch_control_set(unit, bcmSwitchFieldCache, 1);
        soc_sand_ll_timer_stop(ARAD_KBP_TIMERS_BCM);
    }

    bcm_l3_host_t_init(&l3host);
    l3host.l3a_flags = 0;
    l3host.l3a_ip_addr = host;
    l3host.l3a_vrf = vrf;
    l3host.l3a_intf = 0; /* point to FEC to get out-interface  */
    l3host.l3a_modid = 0;

    l3host.l3a_port_tgid = 201; /* egress port to send packet to */

    /* set encap id to point to MAC address */
    /* as encap id is valid (!=0), host will point to FEC + outlif (MAC), and FEC will be "fixed" not to overwrite outlif */
    l3host.encap_id = encap_id;
    sal_memcpy(l3host.l3a_nexthop_mac, next_hop_mac, 6); /* next hop mac attached directly */

    for (i = 0; i < entry_num; i ++)
    {
        /* printf("******************** bcm_l3_host_add i=%d, l3host.l3a_ip_addr=0x%x\n", i, l3host.l3a_ip_addr, rv); */
		soc_sand_ll_timer_set("ARAD_KBP_TIMERS_BCM", ARAD_KBP_TIMERS_BCM);
		rv = bcm_l3_host_add(unit, &l3host);
		soc_sand_ll_timer_stop(ARAD_KBP_TIMERS_BCM);

		if (rv != BCM_E_NONE) {
             printf("Error, bcm_l3_host_add Failed, l3host.l3a_ip_addr=0x%x, rv=0x%x\n", l3host.l3a_ip_addr, rv);
        }
        
        if ((i % entry_num_print_mod) == 1) {
            printf("time=%u[us] i=%d, l3host.l3a_ip_addr=0x%x\n", sal_time_usecs(), i, l3host.l3a_ip_addr);
        }

        l3host.l3a_ip_addr++;
    }

    if(cache_mode != 0){
        soc_sand_ll_timer_set("ARAD_KBP_TIMERS_BCM", ARAD_KBP_TIMERS_BCM);
        bcm_switch_control_set(unit, bcmSwitchFieldCommit, 0);
        soc_sand_ll_timer_stop(ARAD_KBP_TIMERS_BCM);
    }

    soc_sand_ll_timer_stop(ARAD_KBP_TIMERS_APPL);

    soc_sand_ll_timer_stop_all();
    /*soc_sand_ll_timer_print_all();*/

	rate = dpp_kbp_ipv4_print_timers(entry_num);

    return rate;
}

STATIC uint32	
get_kbp_entry_params(uint32 					unit,
					 uint32						frwrd_table_id,
					 struct kbp_db 				**db_p,
					 struct kbp_ad_db			**ad_db_p)
{
	int32 res;
	SOC_SAND_INIT_ERROR_DEFINITIONS(0);
   
    res = arad_kbp_alg_kbp_db_get(
              unit,
              frwrd_table_id, 
              db_p
          );

    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    SOC_SAND_CHECK_NULL_PTR(db_p, 30, exit);

    res =  arad_kbp_alg_kbp_ad_db_get(
            unit,
            frwrd_table_id, 
            ad_db_p
        );

    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

	SOC_SAND_CHECK_NULL_PTR(ad_db_p, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in get_kbp_entry_params()",0,0);

}

STATIC int
/* measure the rate of entries direct insertion to kbp for IPV4 application
 * Call directly the Transport layer function (imstead of BCM->SOC->transport 
*/

cmd_dpp_kbp_test_add_measure_rate_direct_call(int unit, args_t *a)
{
    parse_table_t pt; 

	int32 res;
    int i=0,rate=0;
	int entry_num			= 5000;
	int host 				= 0x0a00ff00;
	int vrf 				= 0;

	uint8 data[100]		 = {0};
	uint8 assoData[100]	 = {0};
	uint32 prefix_len;
	uint32 tblIndex;
	uint32 host_32bit;
	uint16 vrf_16bit;

    struct kbp_db 		*db_p 		= NULL;
    struct kbp_ad_db 	*ad_db_p 	= NULL;
    struct kbp_entry 	*db_entry 	= NULL;
    struct kbp_ad 		*ad_entry 	= NULL;

	SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (ARG_CNT(a) >= 0) {
    
        parse_table_init(0,&pt);

        parse_table_add(&pt,"Host"              ,PQ_INT , (void *) (0x0a00ff00)         , &host     , NULL);
        parse_table_add(&pt,"Vrf"               ,PQ_INT , (void *) (0x0)                , &vrf      , NULL);
        parse_table_add(&pt,"EntryNum"          ,PQ_INT , (void *) (5000)               , &entry_num , NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n",
                    ARG_CMD(a), ARG_CUR(a));
            return CMD_USAGE;
        }
        
        cli_out("host=0x%x, vrf=0x%x, entry_num=0x%x\n", host, vrf, entry_num);

    } else {
        return CMD_USAGE;   
    }

	for(i=0;i<100;i++) {
		assoData[i] = i;
	}
	prefix_len = 48;
	tblIndex   = 0;

	host_32bit = (uint32)(host&0xffffffff);
	vrf_16bit  = (uint16)(vrf&0xffff);

	soc_sand_ll_timer_clear();
    soc_sand_ll_timer_set("ARAD_KBP_TIMERS_APPL", ARAD_KBP_TIMERS_APPL);
    soc_sand_ll_timer_set("ARAD_KBP_TIMERS_BCM", ARAD_KBP_TIMERS_BCM);
	res = get_kbp_entry_params(unit,tblIndex,&db_p,&ad_db_p);

	for (i = 0; i < entry_num; i++) {
		host_32bit++;
		data[0] = (vrf_16bit>>8);
		data[1] = (vrf_16bit&0xff);
		data[2] = (host_32bit & 0xff000000)>>24;
		data[3] = (host_32bit & 0x00ff0000)>>16;
		data[4] = (host_32bit & 0x0000ff00)>>8;
		data[5] = (host_32bit & 0x000000ff);

		res = kbp_db_add_prefix(
                db_p, 
                (uint8*)data, 
                prefix_len,
                &db_entry);

		if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 40, exit);
        }

		res = kbp_ad_db_add_entry(
                ad_db_p,
                (uint8*)assoData,
                &ad_entry
              );

       if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 60, exit);
        }

	   res = kbp_entry_add_ad(
			db_p,
			db_entry,
			ad_entry
			);

	   if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
			SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 70, exit);
		}

		soc_sand_ll_timer_set("ARAD_KBP_IPV4_TIMERS_LPM_ROUTE_ADD", ARAD_KBP_IPV4_TIMERS_LPM_ROUTE_ADD);

		res = kbp_db_install(db_p);

        soc_sand_ll_timer_stop(ARAD_KBP_IPV4_TIMERS_LPM_ROUTE_ADD);

        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
			SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 80, exit);

    	}
	}

    soc_sand_ll_timer_stop(ARAD_KBP_TIMERS_BCM);
	soc_sand_ll_timer_stop(ARAD_KBP_TIMERS_APPL);
    soc_sand_ll_timer_stop_all();
	rate = dpp_kbp_ipv4_print_timers(entry_num);

    return rate;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in cmd_dpp_kbp_test_add_measure_rate_direct_call()",0,0);

}

STATIC int
/* measure the rate of entries insertion to kbp for ACLs*/
cmd_dpp_kbp_test_acl_add_measure_rate(int unit, args_t *a)
{
    int i;
    int entry_num;
    int prio_mode;
    int priority;
    int cache_mod;
    parse_table_t pt;
    int is_serial_rpf = 0; 
    bcm_field_action_t external_value_action;
    cmd_result_t rv = CMD_OK;
	int rate = 0;

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

    if (ARG_CNT(a) >= 0) {
    
        parse_table_init(0,&pt);

        parse_table_add(&pt,"PrioMode"          ,PQ_INT , (void *) (0x0)                , &prio_mode            , NULL);
        parse_table_add(&pt,"EntryNum"          ,PQ_INT , (void *) (64*1024)            , &entry_num            , NULL);
        parse_table_add(&pt,"CacheMode"        ,PQ_INT , (void *) (0)                   , &cache_mod            , NULL);
        parse_table_add(&pt,"GroupId"           ,PQ_INT , (void *) (100)                , &group_id_elk         , NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n",
                    ARG_CMD(a), ARG_CUR(a));
            return CMD_USAGE;
        }
        
        cli_out("PrioMode=0x%x, EntryNum=0x%x, CacheMode=0x%x GroupId=0x%x\n", 
                prio_mode, entry_num, cache_mod, group_id_elk);

    } else {
        return CMD_USAGE;   
    }

    soc_sand_ll_timer_clear();
    soc_sand_ll_timer_set("ARAD_KBP_TIMERS_APPL", ARAD_KBP_TIMERS_APPL);

    if (cache_mod != 0) {
        soc_sand_ll_timer_set("ARAD_KBP_TIMERS_BCM", ARAD_KBP_TIMERS_BCM);
        bcm_switch_control_set(unit, bcmSwitchFieldCache, 1);
        soc_sand_ll_timer_stop(ARAD_KBP_TIMERS_BCM);
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

        soc_sand_ll_timer_set("ARAD_KBP_TIMERS_BCM",ARAD_KBP_TIMERS_BCM);
        bcm_field_entry_create(0, group_id_elk, &ent_elk);
        bcm_field_qualify_SrcIp(0, ent_elk, i+1, 0xffffffff);
        bcm_field_action_add(0, ent_elk, external_value_action, entry_num - i, 0);
        bcm_field_entry_prio_set(0, ent_elk, priority);
        bcm_field_entry_install(0, ent_elk);
        soc_sand_ll_timer_stop(ARAD_KBP_TIMERS_BCM);
    }

    if (cache_mod != 0) {
        soc_sand_ll_timer_set("ARAD_KBP_TIMERS_BCM", ARAD_KBP_TIMERS_BCM);
        bcm_switch_control_set(unit, bcmSwitchFieldCommit, 0);
        soc_sand_ll_timer_stop(ARAD_KBP_TIMERS_BCM);
    }

    soc_sand_ll_timer_stop(ARAD_KBP_TIMERS_APPL);

    soc_sand_ll_timer_stop_all();
    rate = dpp_kbp_acl_print_timers(entry_num);

    /* step2: Destroy all the entries */
    cli_out("step2: Remove entries from Elk group\n");
    for (i=_BCM_DPP_FIELD_ENT_BIAS(unit, ExternalTcam);i<(_BCM_DPP_FIELD_ENT_BIAS(unit, ExternalTcam) + entry_num); i++)
    {
        bcm_field_entry_destroy(0,i);
    }

    soc_sand_ll_timer_stop_all();
    soc_sand_ll_timer_clear();


    return rate;
}

STATIC int
cmd_dpp_kbp_test_ipv4_random_measure_rate(int unit, args_t *a)
{
    int             i;
    int             entry_num = 64*1024;
    int             cached_mode = 0;
    parse_table_t   pt; 
    bcm_l3_route_t  routeInfo; 
    bcm_ip_t        dip;
    bcm_vrf_t       vrf = 0;
    bcm_if_t        fec_idx = 2000;
    bcm_error_t     rv = BCM_E_NONE;
    int             prefix_len = 32;
	int rate;

    if (ARG_CNT(a) > 0) {
        parse_table_init(0,&pt);

        parse_table_add(&pt,"EntryNum"          ,PQ_INT , (void *) (64*1024)            , &entry_num            , NULL);
        parse_table_add(&pt,"CachedMode"      	,PQ_INT , (void *) (0)                  , &cached_mode   		, NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n",
                    ARG_CMD(a), ARG_CUR(a));
            return CMD_USAGE;
        }
    }
    
    cli_out("EntryNum=0x%x, CachedMode=0x%x\n", entry_num, cached_mode);

    soc_sand_ll_timer_clear();
    soc_sand_ll_timer_set("ARAD_KBP_TIMERS_APPL", ARAD_KBP_TIMERS_APPL);

	if (cached_mode != 0) {
		soc_sand_ll_timer_set("ARAD_KBP_TIMERS_BCM", ARAD_KBP_TIMERS_BCM);
        bcm_switch_control_set(unit, bcmSwitchFieldCache, 1);
		soc_sand_ll_timer_stop(ARAD_KBP_TIMERS_BCM);
    }

    cli_out("step1: Add routes\n");
    for (i = 0; i < entry_num; i++)
    {
			bcm_l3_route_t_init(&routeInfo);
            dip = (1<<(32-prefix_len))*((i/15)+1);
            routeInfo.l3a_vrf = vrf;
            routeInfo.l3a_intf = fec_idx;
            routeInfo.l3a_subnet = dip;
            routeInfo.l3a_ip_mask = 0xffffffff<<(32-prefix_len);
			soc_sand_ll_timer_set("ARAD_KBP_TIMERS_BCM", ARAD_KBP_TIMERS_BCM);
            rv = bcm_l3_route_add(unit,&routeInfo);
			soc_sand_ll_timer_stop(ARAD_KBP_TIMERS_BCM);
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
	soc_sand_ll_timer_stop(ARAD_KBP_TIMERS_APPL);
	soc_sand_ll_timer_stop_all();
	rate = dpp_kbp_ipv4_print_timers(entry_num);
    return rate;
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
    int             do_update = 1;
    int             do_delete = 1;
    bcm_error_t     rv = BCM_E_NONE;

    if (ARG_CNT(a) > 0) {
        parse_table_init(0,&pt);

        parse_table_add(&pt,"EntryNum"          ,PQ_INT , (void *) (64*1024)            , &entry_num            , NULL);
        parse_table_add(&pt,"EntryNumPrintMod"  ,PQ_INT , (void *) (8*1024)             , &entry_num_print_mod  , NULL);
        parse_table_add(&pt,"CachedEntNum"      ,PQ_INT , (void *) (0)                  , &cached_entries_num   , NULL);
        parse_table_add(&pt,"WarmBoot"          ,PQ_INT , (void *) (0)                  , &do_warmboot          , NULL);
        parse_table_add(&pt,"Update"            ,PQ_INT , (void *) (1)                  , &do_update            , NULL);
        parse_table_add(&pt,"Delete"            ,PQ_INT , (void *) (1)                  , &do_delete            , NULL);

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

    if (do_update) {
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
    }

    if (do_warmboot) {
        sh_process_command(unit, "tr 141 warmboot=1");
    }

    if (do_delete) {
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
    "\trop_write <core><ADdr> <DAta> <Mask> <ADdrShort> <ValidBit> <WriteMode> - write to rop.\n"
    "\t\tADdr - Address of location where data should be written. includes all :vbit, wrmode ...\n"
    "\t\tDAta Mask - data & mask , 0 by default\n"
    "\t\tADdrShort - Address of location where data should be written.\n"
    "\t\tValidBit - The valid bit which indicates if the database entry should be enabled or disabled\n"
    "\t\tWriteMode - needs to be 0 for DATABASE_DM or 1 for DATABASE_XY \n"
    "\trop_read <core><ADdr> <ValidBit> <TadaType> - \n"
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
    "\tsdk_ver - prints the KBP lib version\n"
    "\tkaps_arm <File=filename> <LoadFile=0/1> <CpuHalt=0/1> - Load KAPS ARM FW file to memory and run it.\n"
    "\t\tFile     - ARM FW load file name.\n"
    "\t\tLoadFile - If clear, ARM FW file will not be loaded. Default: 1.\n"
    "\t\tCpuHalt  - If set, ARM CPU will stay in Halt. Default: 0.\n"
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)
    "\tkbp kaps_show <DBAL_TBL_ID> <CountOnly=1> - Prints the table contents in 0xData/valid_num_of_bits/max_num_of_bits format. \n"
    "\tkbp kaps_search <IPV4_UC=1>/<IPV4_MC=1> <vrf=3> <dip=1.2.3.4> <sip=1.2.5.5> <mc_group=0xE0E0E001> <inrif=2> \n"
    "\tkbp kaps_search <IPV6_UC=1>/<IPV6_MC=1> <vrf=3> <inrif=2> <sip6=0100:1600:3500:6400:0000:0000:0000:0000> <dip6=0100:1600:5500:7800:0000:0000:0000:0000> \n"
    "\t\t\t<mc_group6=ff1e:0d0c:0b0a:0908:0706:0504:0302:0100> \n"
    "\tkbp kaps_arm_thread <enable_dma_thread=1> <print_status=1> - Enables and disables the KAPS DMA thread. Also prints thread related information.\n"
    "\tkbp TestAddMeasureRate \n"
    "\tkbp TestAclAddMeasureRate \n"
    "\tIn order to allow insertion rate for KBP, uncomment the flag ARAD_PP_KBP_TIME_MEASUREMENTS under sand_low_level.h\n"
    "\tAnd compile again.\n"
#endif /*defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)*/
    "Examples:\n"
    "\tkbp cpu_record_send OPcode=255 MSB5=1 LSBenable=0\n"
    "\tkbp cpu_lookup_reply\n"
    "\tkbp rop_read addr=0x00000102\n"
    "\tkbp rop_write addr=0x0000010b ADdrShort=0x00000000 data=0x0000000000000000ffff\n"
    "\tkbp init_kbp_interface mdio_id=257 ilkn_rev=1\n"
    "\tkbp test_ip4_rpf_appl NumEntries=2048 RecordBaseTbl0=0x55551234 RecordBaseTbl1=0xeeee1234 ADValTbl0=0xdead0620 ADValTbl1=0xbeaf8321\n"
    "\tkbp init_appl enable=1 tcamdevtype=1 Ip4UcFwdTableSize=8192 Ip4McFwdTableSize=8192\n"
    "\tkbp kaps_arm file=opsw_test_secded.hex00\n"
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

        soc_sand_rv = arad_kbp_cpu_lookup_reply(unit, 0, data);
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
		
    } else if (!sal_strcasecmp(func, "TestAddMeasureRate")) {

		int rate = cmd_dpp_kbp_test_add_measure_rate(unit, a);
        if(rate < KBP_IPV4_INSERTION_RATE_LIMIT)
		{
			printf("\nRate is lower than expected\nRate = %d, Expected limit = %d\n\n",rate,KBP_IPV4_INSERTION_RATE_LIMIT);
			return CMD_FAIL;
		}
		return CMD_OK;

    } else if (!sal_strcasecmp(func, "TestDirectCallAddMeasureRate")) {

		int rate = cmd_dpp_kbp_test_add_measure_rate_direct_call(unit, a);
        if(rate < KBP_ACL_INSERTION_RATE_LIMIT)
		{
			printf("\nRate is lower than expected\nRate = %d, Expected limit = %d\n\n",rate,KBP_ACL_INSERTION_RATE_LIMIT);
			return CMD_FAIL;
		}
		return CMD_OK;

    } else if (!sal_strcasecmp(func, "TestAclAddMeasureRate")) {

		int rate = cmd_dpp_kbp_test_acl_add_measure_rate(unit, a);
        if(rate < KBP_ACL_INSERTION_RATE_LIMIT)
		{
			printf("\nRate is lower than expected\nRate = %d, Expected limit = %d\n\n",rate,KBP_ACL_INSERTION_RATE_LIMIT);
			return CMD_FAIL;
		}
		return CMD_OK;

    } else if (!sal_strcasecmp(func, "test_ipv4_measure_rate_random")) {

		int rate = cmd_dpp_kbp_test_ipv4_random_measure_rate(unit, a);
        if(rate < KBP_IPV4_INSERTION_RATE_LIMIT)
		{
			printf("\nRate is lower than expected\nRate = %d, Expected limit = %d\n\n",rate,KBP_ACL_INSERTION_RATE_LIMIT);
			return CMD_FAIL;
		}
		return CMD_OK;

    } else if (!sal_strcasecmp(func, "TestAddRate")) {

        return cmd_dpp_kbp_test_add_rate(unit, a);

    } else if (!sal_strcasecmp(func, "TestAclAddRate")) {

        return cmd_dpp_kbp_test_acl_add_rate(unit, a);

    } else if (!sal_strcasecmp(func, "test_ipv4_random")) {

        return cmd_dpp_kbp_test_ipv4_random(unit, a);

    } else if(!sal_strcasecmp(func, "sdk_ver")) {

            return cmd_dpp_sdk_ver(unit, a);

    } else if (JER_KAPS_ENABLE(unit)) {
        if(!sal_strcasecmp(func, "kaps_init")) {

            return cmd_dpp_kbp_kaps_init(unit, a);

        } else if(!sal_strcasecmp(func, "kaps_deinit")) {

            return cmd_dpp_kaps_deinit(unit, a);

        } else if(!sal_strcasecmp(func, "kaps_search")) {

            return cmd_dpp_kaps_search(unit, a); 

        } else if(!sal_strcasecmp(func, "kaps_diag_01")) {

            return cmd_dpp_kaps_diag_01(unit, a);

        } else if(!sal_strcasecmp(func, "kaps_diag_02")) {

            return cmd_dpp_kaps_diag_02(unit, a);

        } else if(!sal_strcasecmp(func, "kaps_show")) {

            return cmd_dpp_kaps_show(unit, a);

        } else if(!sal_strcasecmp(func, "kaps_arm_thread")) {

            return cmd_dpp_kaps_arm_thread(unit, a);

        }
        else if(!sal_strcasecmp(func, "kaps_arm")) {
            return cmd_dpp_kaps_arm_image_load(unit, a);
        }
        else {

            return(CMD_USAGE);      /* Print usage line */

        }
    } else {

        return(CMD_USAGE);      /* Print usage line */
    }

    return rv;

}

#endif /* #ifdef INCLUDE_KBP */

