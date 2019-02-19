/*
 * $Id: arrakis.c, 2015/03/30 idanu Exp $
 *
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
 * 
 * DNX ARRAKIS DIAG
 */


#include <shared/bsl.h>

#include <sal/appl/io.h>
#include <sal/appl/sal.h>

#include <appl/diag/dcmn/diag.h>

#include <bcm/error.h>

#include <soc/drv.h>
#include <soc/dcmn/error.h>

#ifdef BCM_DPP_SUPPORT
#include <soc/dpp/dpp_config_defs.h>
#endif

/* 
 *Local Functions 
 */
STATIC cmd_result_t  diag_dnx_arrakis_usage(int unit, args_t *args);

STATIC void diag_dnx_arrakis_ready_usage(int unit);
STATIC cmd_result_t diag_dnx_arrakis_ready(int unit, args_t* a);

STATIC void diag_dnx_arrakis_property_usage(int unit);
STATIC cmd_result_t diag_dnx_arrakis_property(int unit, args_t* a);


/* 
 *Arrakis diag pack types
 */
typedef enum diag_dnx_arrakis_type_e
{
    diag_dnx_arrakis_type_usage = 0,
    diag_dnx_arrakis_type_ready = 1,
    diag_dnx_arrakis_type_property = 2,

    /*Must be last!*/
    diag_dnx_arrakis_type_nof = 3
} diag_dnx_arrakis_type_s;

/*
 * DNX arrakis diag pack
 */
const diag_dnx_table_t diag_dnx_arrakis_pack[] = {
    /*CMD_NAME,             CMD_ACTION,                             CMD_USAGE,                              CMD_ID*/
    {"usage",               diag_dnx_arrakis_usage,                 NULL,                                   diag_dnx_arrakis_type_usage},
    {"ready",               diag_dnx_arrakis_ready,                 diag_dnx_arrakis_ready_usage,           diag_dnx_arrakis_type_ready},
    {"property",            diag_dnx_arrakis_property,              diag_dnx_arrakis_property_usage,        diag_dnx_arrakis_type_property},
    {NULL,                  NULL,                                   NULL,                                   DNX_DIAG_ID_UNAVAIL}
};

STATIC int
diag_dnx_arrakis_support(int unit, int diag_id)
{
    switch (diag_id)
    {
       default:
           return TRUE;
           break;
    }
}

/*DNX arrakis diag pack usage*/
STATIC cmd_result_t 
diag_dnx_arrakis_usage(int unit, args_t *args)
{

    cli_out("Arrakis diag pack usage:\n");
    cli_out("-----------------------\n");
    diag_dnx_usage_print(unit, diag_dnx_arrakis_pack, diag_dnx_arrakis_support);


    cli_out("arrakis: ready for command\n");
    return CMD_OK;
}



char diag_dnx_arrakis_diag_pack_usage_str[] = "DNX arrakis diag pack - type 'arrakis usage' for additionl information\n";
/*DNX arrakis pack main function*/
cmd_result_t 
diag_dnx_arrakis_diag_pack(int unit, args_t *args)
{
    char *cmd_name;   
    cmd_result_t res;

    
    cmd_name = ARG_GET(args);
    res = diag_dnx_dispatch(unit, diag_dnx_arrakis_pack, diag_dnx_arrakis_support, cmd_name, args);
    if (res == CMD_USAGE)
    {
        diag_dnx_arrakis_usage(unit, NULL);
    }

    return CMD_FAIL;
}


/*Diag Arrakis ready*/

STATIC void
diag_dnx_arrakis_ready_usage(int unit)
{
    cli_out("'ready' - printout when the driver is ready for new command\n");
}

STATIC cmd_result_t
diag_dnx_arrakis_ready(int unit, args_t* a)
{
    cli_out("arrakis: ready for command\n");
    return CMD_OK;
}

/*Diag Arrakis Property*/
STATIC void
diag_dnx_arrakis_property_usage(int unit)
{
    cli_out("'property <soc propery name> [...]' - display soc property value or NULL if undefined\n");
}

/*suffix types to parse*/
typedef enum diag_dnx_arrakis_property_suffix_type_e {
    diag_dnx_arrakis_property_suffix_type_none = 0,
    diag_dnx_arrakis_property_suffix_type_str = 1,
    diag_dnx_arrakis_property_suffix_type_port = 2,
    diag_dnx_arrakis_property_suffix_type_sp = 3, 
    diag_dnx_arrakis_property_suffix_type_counter_engine = 4
} diag_dnx_arrakis_property_suffix_type_t;

STATIC cmd_result_t
diag_dnx_arrakis_property(int unit, args_t* args)
{
    char *name, *value, *value_port, *suffix = NULL;
    parse_table_t pt;
    diag_dnx_arrakis_property_suffix_type_t suffix_type;
    bcm_port_t port;

    name = ARG_GET(args);
    

    /* parsing input from user */
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "suffix", PQ_DFL|PQ_STRING, NULL, &suffix, NULL);
    if (parse_arg_eq(args, &pt) < 0) {
      parse_arg_eq_done(&pt);
      return CMD_USAGE;
    }
    
    if (suffix == NULL)
    {
        suffix_type = diag_dnx_arrakis_property_suffix_type_none;
    } 
    else if (!sal_strncasecmp(suffix, "<port>", strlen(suffix))) 
    {
        suffix_type = diag_dnx_arrakis_property_suffix_type_port;
    } 
    else if (!sal_strncasecmp(suffix, "<sp>", strlen(suffix)))
    {
        suffix_type = diag_dnx_arrakis_property_suffix_type_sp;
    }
    else if (!sal_strncasecmp(suffix, "<engine-id>", strlen(suffix))) 
    {
        suffix_type = diag_dnx_arrakis_property_suffix_type_counter_engine;
    }
    else
    {
        suffix_type = diag_dnx_arrakis_property_suffix_type_str;
    }

    switch (suffix_type)
    {
       case diag_dnx_arrakis_property_suffix_type_none:
           value  = soc_property_get_str(unit, name);
           cli_out("arrakis_soc_property:%s:%s:\n", name, value != NULL ? value : "NULL");
           break;
       case diag_dnx_arrakis_property_suffix_type_str:
           value  = soc_property_suffix_num_only_suffix_str_get (unit, -1, name, suffix);
           cli_out("arrakis_soc_property:%s_%s:%s:\n", name, suffix, value != NULL ? value : "NULL");
           break;
       case diag_dnx_arrakis_property_suffix_type_port:
           value  = soc_property_get_str(unit, name);
           cli_out("arrakis_soc_property:%s:%s:\n", name, value != NULL ? value : "NULL");
           PBMP_PORT_ITER(unit, port)
           {   
               value_port = soc_property_port_get_str(unit, port, name);
               if ((value == NULL && value_port != NULL) ||
                   (value != NULL && value_port != NULL && !sal_strncasecmp(value, value_port, strlen(value))))
               {
                   cli_out("arrakis_soc_property:%s_%s:%s:\n", name, SOC_PORT_NAME(unit, port), value);
               }
           }
       case diag_dnx_arrakis_property_suffix_type_counter_engine:
#ifdef BCM_DPP_SUPPORT
           if (SOC_IS_DPP(unit))
           {
               int i;

               for (i = 0; i < SOC_DPP_DEFS_GET(unit, nof_counter_processors); i++)
               {
                   value = soc_property_suffix_num_only_suffix_str_get (unit, i, name, "");
                   if (value != NULL)
                   {
                       cli_out("arrakis_soc_property:%s_%d:%s:\n", name, i, value);
                   }
               }
           }
#endif /*BCM_DPP_SUPPORT*/
           break;
       default:
           break;
    }

    return CMD_OK;
}

