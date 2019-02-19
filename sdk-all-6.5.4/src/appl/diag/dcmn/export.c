/*
 * $Id: export.c, 2015/03/30 idanu Exp $
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
 * DNX Export facilities
 */

#include <shared/bsl.h>

#include <sal/appl/io.h>
#include <sal/appl/sal.h>

#include <appl/diag/dcmn/diag.h>
#include <appl/diag/dcmn/export.h>
#include <appl/diag/dcmn/rate_calc.h>
#include <appl/diag/dcmn/diag_signals_dump.h>

#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/TMC/tmc_api_diagnostics.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dcmn/error.h>

#include <shared/swstate/access/sw_state_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_diag.h>
#include <soc/dpp/PPD/ppd_api_diag.h>
#include <soc/dpp/ARAD/arad_egr_prog_editor.h>

#define TX_CMD_BUFFER_SIZE                  1000
#define PACKET_DATA_BUFFER_SIZE             1000
#define EXPORT_DCMN_SHELL_CMD_BUFFER_SIZE   200

#include <appl/diag/dcmn/diag_pp_data.h>

/*Local Functions 
 */
STATIC cmd_result_t  diag_dnx_export_usage(int unit, args_t *args);

STATIC void diag_dnx_export_ready_usage(int unit);
STATIC cmd_result_t diag_dnx_export_ready(int unit, args_t* a);

STATIC void diag_dnx_export_property_usage(int unit);
STATIC cmd_result_t diag_dnx_export_property(int unit, args_t* a);

STATIC void diag_dnx_export_pp_usage(int unit);
STATIC cmd_result_t diag_dnx_export_pp(int unit, args_t* a);

STATIC void diag_dnx_export_diag_usage(int unit);
STATIC cmd_result_t diag_dnx_export_diag(int unit, args_t* a);

STATIC void diag_dnx_export_dump_usage(int unit);
STATIC cmd_result_t diag_dnx_export_dump(int unit, args_t* a);

STATIC cmd_result_t diag_dnx_export_dump_get_last_packet(int unit, char * last_data, int last_data_buff_size , bcm_port_t* last_port, uint32 core);
STATIC cmd_result_t diag_dnx_export_signals_dump(int unit, int core);

STATIC cmd_result_t diag_dnx_export_stat_path_clear(int unit);
STATIC cmd_result_t diag_dnx_export_device_info_dump(int unit);
STATIC cmd_result_t diag_dnx_export_stat_path_dump(int unit, bcm_port_t  in_port, char *data);



/* 
 *Export diag pack types
 */
typedef enum diag_dnx_export_type_e
{
    diag_dnx_export_type_usage         = 0,
    diag_dnx_export_type_ready         = 1,
    diag_dnx_export_type_property      = 2,
    diag_dnx_export_type_dump          = 3,
    diag_dnx_export_type_pp            = 4,
    diag_dnx_export_type_diag          = 5,

    /*Must be last!*/
    diag_dnx_export_type_nof = 6

} diag_dnx_export_type_s;

/*
 * DNX export diag pack
 */
const diag_dnx_table_t diag_dnx_export_pack[] = {
    /*CMD_NAME,    CMD_ACTION,                   CMD_USAGE,                              CMD_ID*/
    {"usage",      diag_dnx_export_usage,        NULL,                                   diag_dnx_export_type_usage},
    {"ready",      diag_dnx_export_ready,        diag_dnx_export_ready_usage,            diag_dnx_export_type_ready},
    {"property",   diag_dnx_export_property,     diag_dnx_export_property_usage,         diag_dnx_export_type_property},
    {"dump",       diag_dnx_export_dump,         diag_dnx_export_dump_usage,             diag_dnx_export_type_dump},
    {"pp",         diag_dnx_export_pp,           diag_dnx_export_pp_usage,               diag_dnx_export_type_pp},
    {"diag",       diag_dnx_export_diag,         diag_dnx_export_diag_usage,             diag_dnx_export_type_diag},
    {NULL,         NULL,                         NULL,                                   DNX_DIAG_ID_UNAVAIL}
};

STATIC int
diag_dnx_export_support(int unit, int diag_id)
{
    switch (diag_id)
    {
       default:
           return TRUE;
           break;
    }
}

/*DNX export diag pack usage*/
STATIC cmd_result_t 
diag_dnx_export_usage(int unit, args_t *args)
{
    cli_out("Export diag pack usage:\n");
    cli_out("-----------------------\n");
    diag_dnx_usage_print(unit, diag_dnx_export_pack, diag_dnx_export_support);


    cli_out("export: ready for command\n");
    return CMD_OK;
}

char cmd_dpp_export_usage_str[] = "DNX export diag pack - type 'export usage' for additionl information\n";
/*DNX export pack main function*/
cmd_result_t 
cmd_dpp_export(int unit, args_t *args)
{
    char *cmd_name;   
    cmd_result_t res;

    if(!SOC_IS_DPP(unit))
        return CMD_FAIL;

    cmd_name = ARG_GET(args);
    res = diag_dnx_dispatch(unit, diag_dnx_export_pack, diag_dnx_export_support, cmd_name, args);
    if (res != CMD_OK)
    {
        cli_out("EXPORT_CMD: FAILED\n");
    } else {
        cli_out("EXPORT_CMD: PASS\n");
    }
    if (res == CMD_USAGE)
    {
        diag_dnx_export_usage(unit, NULL);
        return CMD_FAIL;
    }

    return res;
}

/*Diag Export ready*/
STATIC void
diag_dnx_export_ready_usage(int unit)
{
    cli_out("'ready' - printout when the driver is ready for new command\n");
}

STATIC cmd_result_t
diag_dnx_export_ready(int unit, args_t* a)
{
    cli_out("export: ready for command\n");
    return CMD_OK;
}

/*Diag Export Property*/
STATIC void
diag_dnx_export_property_usage(int unit)
{
    cli_out("'property <soc property name> [...]' - display soc property value or NULL if undefined\n");
}

/*suffix types to parse*/
typedef enum diag_dnx_export_property_suffix_type_e {
    diag_dnx_export_property_suffix_type_none           = 0,
    diag_dnx_export_property_suffix_type_str            = 1,
    diag_dnx_export_property_suffix_type_port           = 2,
    diag_dnx_export_property_suffix_type_sp             = 3,
    diag_dnx_export_property_suffix_type_counter_engine = 4
} diag_dnx_export_property_suffix_type_t;

STATIC cmd_result_t
diag_dnx_export_property(int unit, args_t* args)
{
    char *name, *value, *value_port, *suffix = NULL;
    parse_table_t pt;
    diag_dnx_export_property_suffix_type_t suffix_type;
    bcm_port_t port;
    int i;

    name = ARG_GET(args);
    if (name == NULL)
    {
        return CMD_USAGE;
    }

    /* parsing input from user */
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "suffix", PQ_DFL|PQ_STRING, NULL, &suffix, NULL);
    if (parse_arg_eq(args, &pt) < 0) {
      parse_arg_eq_done(&pt);
      return CMD_USAGE;
    }
    
    if (suffix == NULL)
    {
        suffix_type = diag_dnx_export_property_suffix_type_none;
    } 
    else if (!sal_strncasecmp(suffix, "<port>", strlen(suffix))) 
    {
        suffix_type = diag_dnx_export_property_suffix_type_port;
    } 
    else if (!sal_strncasecmp(suffix, "<sp>", strlen(suffix)))
    {
        suffix_type = diag_dnx_export_property_suffix_type_sp;
    }
    else if (!sal_strncasecmp(suffix, "<engine-id>", strlen(suffix))) 
    {
        suffix_type = diag_dnx_export_property_suffix_type_counter_engine;
    }
    else
    {
        suffix_type = diag_dnx_export_property_suffix_type_str;
    }

    switch (suffix_type)
    {
       case diag_dnx_export_property_suffix_type_none:
           value  = soc_property_get_str(unit, name);
           cli_out("export_soc_property:%s:%s:\n", name, value != NULL ? value : "NULL");
           break;
       case diag_dnx_export_property_suffix_type_str:
           value  = soc_property_suffix_num_only_suffix_str_get (unit, -1, name, suffix);
           cli_out("export_soc_property:%s_%s:%s:\n", name, suffix, value != NULL ? value : "NULL");
           break;
       case diag_dnx_export_property_suffix_type_port:
           value  = soc_property_get_str(unit, name);
           cli_out("export_soc_property:%s:%s:\n", name, value != NULL ? value : "NULL");
           PBMP_PORT_ITER(unit, port)
           {   
               value_port = soc_property_port_get_str(unit, port, name);
               if ((value == NULL && value_port != NULL) ||
                   (value != NULL && value_port != NULL && !sal_strncasecmp(value, value_port, strlen(value))))
               {
                   cli_out("export_soc_property:%s_%s:%s:\n", name, SOC_PORT_NAME(unit, port), value);
               }
           }
           break;
       case diag_dnx_export_property_suffix_type_counter_engine:
           for (i = 0; i < SOC_DPP_DEFS_GET(unit, nof_counter_processors); i++) {
               value = soc_property_suffix_num_only_suffix_str_get (unit, i, name, "");
               if (value != NULL) {
                   cli_out("export_soc_property:%s_%d:%s:\n", name, i, value);
               }
           }
           break;
       default:
           break;
    }

    return CMD_OK;
}

/* Diag Export PP */
STATIC void
diag_dnx_export_pp_usage(int unit)
{
    cli_out("'pp [stage=<parser/vt/tt/flp>] [file=target_file ]' - export PP Stage programs&tables into targte_file.xml\n");
}

STATIC cmd_result_t
diag_dnx_export_pp(int unit, args_t* args)
{
    parse_table_t  pt;
    cmd_result_t res = CMD_OK;

    char *file_n = NULL;
    char *stage_n = NULL;

    /* Get parameters */
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "file", PQ_STRING, 0, (void *)&file_n, NULL);
    parse_table_add(&pt, "stage", PQ_STRING, 0, (void *)&stage_n, NULL);

    if (0 > parse_arg_eq(args, &pt))
        res = CMD_USAGE;
    else
        res = dpp_export_pp(unit, stage_n, file_n);
    return res;
}

/* Diag commands wrapped with export */
STATIC void
diag_dnx_export_diag_usage(int unit)
{
    cli_out("'diag \"<diag command>\" \n");
}

STATIC cmd_result_t
diag_dnx_export_diag(int unit, args_t* args)
{
    return sh_process_command(unit, ARG_GET(args));
}

/***********
* dump information for export
* recieves: port data 
*  
* The method of using a script to send a packet 
* is currently not supported 
************/ 
STATIC cmd_result_t
diag_dnx_export_dump(int unit, args_t* a){
    char
        tx_cmd[TX_CMD_BUFFER_SIZE],
        * data=NULL;
    char last_data[PACKET_DATA_BUFFER_SIZE];
    bcm_port_t port = -1;
    int core = 0, user_core = 0;
    uint32 flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    int resend = 1;
    char *filename = NULL;
    char shell_cmd[EXPORT_DCMN_SHELL_CMD_BUFFER_SIZE];

    cmd_result_t
        rv = CMD_OK;

    parse_table_t pt;

    /*Supported for ARRAD/Jericho only*/
    if (!SOC_IS_ARAD(unit)) {
        return CMD_FAIL;
    }
    
    /*Parse args*/
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "PSRC", PQ_DFL|PQ_INT, &port, &port, NULL);
    parse_table_add(&pt, "DATA", PQ_DFL|PQ_STRING, NULL, &data, NULL);
    parse_table_add(&pt, "CORE", PQ_DFL|PQ_INT, &user_core, &user_core, NULL);
    parse_table_add(&pt, "FILE", PQ_DFL|PQ_STRING, NULL, &filename, NULL);
    parse_table_add(&pt, "RESEND", PQ_DFL|PQ_INT, &resend, &resend, NULL);
    if(parse_arg_eq(a, &pt) < 0) {
        cli_out("invalid option: %s\n", ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    if(filename != NULL) {
        /* Start Log */
        sal_sprintf(shell_cmd, "log file=%s append=no", filename);
        rv = sh_process_command(unit,shell_cmd);
        ASSERT_EXIT(rv, shell_cmd);
    }

    
    
    /*resend/send a packet - otherwise assume the the packet sent over cleared counters*/
    if (resend)
    {
        /* create tx command*/
        sal_memset(tx_cmd,0,TX_CMD_BUFFER_SIZE);
        if ((port == -1) || (data == NULL)) {

            /*Fill last_data buffer*/
            sal_memset(last_data,0,PACKET_DATA_BUFFER_SIZE);

            cli_out("INFO: PSRC or DATA was not recieved - using last packet\n");

            rv = diag_dnx_export_dump_get_last_packet(unit, last_data, PACKET_DATA_BUFFER_SIZE , &port, user_core);
            ASSERT_EXIT(rv, "diag_dnx_export_dump_get_last_packet");

            data = last_data;

            sal_snprintf(tx_cmd,TX_CMD_BUFFER_SIZE - 1 ,"tx 1 PSRC=%d DATA=%s", port, data);

            rv = bcm_port_get(unit, port, &flags, &interface_info, &mapping_info);
            ASSERT_EXIT(rv, "bcm_port_get failed");

            core = mapping_info.core;

        }
        else
        {
            sal_snprintf(tx_cmd,TX_CMD_BUFFER_SIZE - 1 ,"tx 1 PSRC=%d DATA=%s", port, data);
            
            
            rv = bcm_port_get(unit, port, &flags, &interface_info, &mapping_info);
            ASSERT_EXIT(rv, "bcm_port_get failed");

            core = mapping_info.core;
        }
        cli_out("INFO: %s", tx_cmd + 4 );

        rv = diag_dnx_export_stat_path_clear(unit);
        ASSERT_EXIT(rv, "diag_dnx_export_path_stat_clear");

        /* send tx command*/
        rv = sh_process_command(unit,tx_cmd);
        ASSERT_EXIT(rv, "tx");

    }
    

    cli_out("\n===export_dump===\n");


    /*device dump*/
    rv = diag_dnx_export_device_info_dump(unit);
    ASSERT_EXIT(rv, "diag_dnx_export_device_info_dump");

    /*signals dump*/
    rv = diag_dnx_export_signals_dump(unit, core);
    ASSERT_EXIT(rv, "diag_dnx_export_signals_dump");
    /*path stat dump*/
    rv = diag_dnx_export_stat_path_dump(unit, port, data);
    ASSERT_EXIT(rv, "diag_dnx_export_path_stat_clear");


    cli_out("\n===export_dump===\n");

    if(filename != NULL) {
        sal_sprintf(shell_cmd, "log off");
        rv = sh_process_command(unit,shell_cmd);
        ASSERT_EXIT(rv, shell_cmd);
    }

exit:
    return rv;
}

STATIC void
diag_dnx_export_dump_usage(int unit){
    cli_out("'dump [PSRC=<port number> DATA=<data>] [CORE=<core>] [FILE=<file name>] [RESEND=<0/1>]' - sends a packet and creates export dump; \n\
            To send a new packet, fill the PSRC and DATA fields. \n\
            To send the last packet fill the CORE field only. \n\
            To capture part of the data without resending new packet use 'RESEND=0'\n");
}

/***********
* dump signals for export
************/ 
STATIC cmd_result_t
diag_dnx_export_signals_dump(int unit, int core){

    uint32 sand_res;
    cmd_result_t
        rv = CMD_OK;
    int core_index;
    char shell_cmd[RHFILE_MAX_SIZE];

    /*Supported for ARRAD/Jericho only*/
    if (!SOC_IS_ARAD(unit)) {
        return CMD_FAIL;
    }

    cli_out("\n===dump_signal===\n");
    sand_res = arad_diag_signals_dump(unit, core, 0);
    if (SOC_SAND_FAILURE(sand_res))
    {
        cli_out("diag signals dump failed \n");
        return CMD_FAIL;
    }
    cli_out("\n===dump_signal===\n");

    if (SOC_DPP_DEFS_GET(unit, nof_cores) > 1) {
        SOC_DPP_CORES_ITER(SOC_CORE_ALL, core_index) {
            cli_out("\n===dump_signal core_%d===\n", core_index);

            sand_res = arad_diag_signals_dump(unit, core_index, 0);
            if (SOC_SAND_FAILURE(sand_res))
            {
                cli_out("diag signals dump failed \n");
                return CMD_FAIL;
            }

            cli_out("\n===dump_signal core_%d===\n", core_index);
        }
    }

    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core_index) {
        cli_out("\n===table_view core_%d===\n", core_index);

        sprintf(shell_cmd, "diag pp vtt_dump last=1 core=%d", core);
        sh_process_command(unit,shell_cmd);

        sprintf(shell_cmd, "diag pp flp_dump last=1 core=%d", core);
        sh_process_command(unit,shell_cmd);

        cli_out("\n===table_view core_%d===\n", core_index);
    }
    return rv;
}

/*
* Input: unit number, 
*           and pointers to copy information into 
* Purpose: get last packet (port&data) 
*           and copy it to the input pointers.
* Output: error detection 
*/
STATIC cmd_result_t
diag_dnx_export_dump_get_last_packet(int unit, char * last_data,int last_data_buff_size , bcm_port_t* last_port, uint32 core)
{
    SOC_TMC_DIAG_LAST_PACKET_INFO info;
    unsigned i=0;
    char * soc_prop_port;
    int rv ,
        last_data_len = 0;

    sal_memset(&info, 0x0, sizeof(info));
    /* get last packet into info*/
    info.tm_port = 0;
    rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_diag_last_packet_info_get, (unit, core, &info));
    if (soc_sand_get_error_code_from_error_word(rv) != SOC_SAND_OK) {
      cli_out("\nFailed to get the last packet information\n\n");
      return CMD_FAIL;
    }

    if (unit >= BCM_LOCAL_UNITS_MAX){
        return CMD_FAIL;
    }
    soc_prop_port = soc_property_port_get_str(unit, info.tm_port,  "tm_port_header_type_in");

    /*check the soc property port*/
    if (soc_prop_port == NULL) {
        cli_out("ERROR: cannot get last packet - soc property port (NULL) is not supported\n");
        return CMD_FAIL;
    }
    else if (!sal_strcmp("ETH",  soc_prop_port  ) ){
        i = 0;
    }
    else if (!sal_strcmp("INJECTED_2_PP",  soc_prop_port  ) ){
        i = 2;
    }
    else if (!sal_strcmp("INJECTED_2",  soc_prop_port  ) ){
        i = 2;
    }
    else{
        cli_out("ERROR: cannot get last packet - soc property port (%s) is not supported\n", soc_prop_port);
        return CMD_FAIL;
    }

    cli_out("INFO: soc property port : %s \n", soc_prop_port);

    for (   ; i < SOC_TMC_DIAG_LAST_PCKT_SNAPSHOT_LEN_BYTES_MAX; ++i) {
       sal_sprintf(last_data + last_data_len, "%02x", info.buffer[i]);  /*print 2 chars exactly, pad with zeroes if necessary*/
       last_data_len += 2;

       if (last_data_len >= last_data_buff_size - 2) {
            break;
       }
    }

    *last_port = info.pp_port;

    return CMD_OK;
}


/***********
* dump device info for export
************/ 
STATIC cmd_result_t
diag_dnx_export_device_info_dump(int unit)
{
    cli_out("\n===device_info===\n");
    cli_out("device:%s:\n", soc_dev_name(unit));
    cli_out("\n===device_info===\n");
    return CMD_OK;
}

STATIC cmd_result_t
diag_dnx_is_port_special(int unit, SOC_PPC_PORT port, uint32 port_type, int* result){
    int rv, i;
    bcm_gport_t                             special_ports[BCM_PIPES_MAX];
    int                                     nof_special_ports;
    /* Get special ports array */
    rv = bcm_port_internal_get(unit,port_type,BCM_PIPES_MAX,special_ports,&nof_special_ports);
    if (rv != BCM_E_NONE) {
        cli_out("Error occurred while trying to retrieve ports\n");
        return CMD_FAIL;
    }
    
    /* In case OLP Ports were defined, check if packet originates from OLP */
    if (nof_special_ports > 0) {
        for (i = 0 ; i < nof_special_ports ; i++) {
            if (port == BCM_GPORT_LOCAL_GET(special_ports[i])) {
                *result = TRUE;
                return CMD_OK;
            }
        }    
    }

    *result = FALSE;
    return CMD_OK;

}

/***********
* dump path stat info for export
************/ 
STATIC cmd_result_t
diag_dnx_export_stat_path_dump(int unit, bcm_port_t  in_port, char *data)
{
    soc_dpp_stat_path_info_t info;
    char *drop_name;
    int rv;
    cmd_result_t res;
    uint32 ret;
    SOC_PPC_DIAG_RECEIVED_PACKET_INFO       prm_rcvd_pkt_info;
    SOC_PPC_DIAG_RESULT                     ret_val;
    int                                     is_olp_port = 0, is_oamp_port = 0;

    rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_stat_path_info_get, (unit, &info));
    if (BCM_FAILURE(rv))
    {
        cli_out("ERROR: stat path failed\n");
        return CMD_FAIL;
    }



    ret = soc_ppd_diag_received_packet_info_get(
          unit,
          info.ingress_core,
          &prm_rcvd_pkt_info,
          &ret_val
        );

    rv = soc_sand_get_error_code_from_error_word(ret);
    if(BCM_FAILURE(rv)){
        cli_out("ERROR: received packet info failed\n");
        return CMD_FAIL;  
    }

    if (ret_val == SOC_PPC_DIAG_OK) {  
        /* check if port is OLP */ 
        res = diag_dnx_is_port_special(unit, prm_rcvd_pkt_info.in_pp_port, BCM_PORT_INTERNAL_OLP, &is_olp_port);
        if(res != CMD_OK) {
            return res;
        }

        /* check if port is OAMP */ 
        res = diag_dnx_is_port_special(unit, prm_rcvd_pkt_info.in_pp_port, BCM_PORT_INTERNAL_OAMP, &is_oamp_port);
        if(res != CMD_OK) {
            return res;
        }
     
    } else {
        LOG_CLI((BSL_META_U(unit, "%s Packet not found\n\r"),get_core_str(unit, info.ingress_core)));
    }


    cli_out("\n===dump_path_stat===\n");
    cli_out("in_port:%d:\n", in_port);
    cli_out("data:%s:\n", data);
    cli_out("ingress_core:%d:\n", info.ingress_core);
    cli_out("egress_core:%d:\n", info.egress_core);
    
    switch(info.drop)
    {
       case soc_dpp_stat_path_drop_stage_ingress_no_packet:
           drop_name = "NIF";
           break;
       case soc_dpp_stat_path_drop_stage_ingress_tm:
           drop_name = "ITM";
           break;
       case soc_dpp_stat_path_drop_stage_egress_tm:
           drop_name = "ETM";
           break;
       case soc_dpp_stat_path_drop_stage_none:
       default:
           drop_name = "NONE";
           break;
    }
    cli_out("drop:%s:\n", drop_name);
    cli_out("is_olp:%d:\n", is_olp_port);
    cli_out("is_oamp:%d:", is_oamp_port);
    cli_out("\n===dump_path_stat===\n");

    return CMD_OK;
}

STATIC cmd_result_t
diag_dnx_export_stat_path_clear(int unit)
{
    soc_dpp_stat_path_info_t info;
    int rv;

    rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_stat_path_info_get, (unit, &info));
    if (BCM_FAILURE(rv))
    {
        cli_out("ERROR: stat path failed\n");
        return CMD_FAIL;
    }
    return CMD_OK;
}



