/*
 * $Id: techsupport.c Broadcom SDK $
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
 * File:    techsupport.c
 * Purpose: This file contains the generic infrastructure code
 *          for techsupport utility.
 *
 */

#include <appl/diag/parse.h>
#include <shared/bsl.h>
#include <bcm/error.h>
#include <appl/diag/shell.h>
#include <sal/core/time.h>
#include <appl/diag/techsupport.h>
#include <soc/drv.h>
#include <soc/ea/allenum.h>
#include <sal/appl/sal.h>
#include <bcm/types.h>

char command_techsupport_usage[] =
     "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "techsupport <option> [args...]\n"
#else
    "techsupport basic - collects basic config/setup information from switch\n\t"
    "techSupport <feature_name> [diag] [reg] "
    "[mem] [list] [verbose]- collects feature specific debug information\n\t"
    "When <feature_name> is "COMMAND_L3UC" - collects L3 unicast related debug information\n\t"
    "When <feature_name> is "COMMAND_L3MC" - collects L3 multicast related debug information\n\t"
    "When <feature_name> is "COMMAND_MPLS" - collects mpls related debug information\n\t"
    "When <feature_name> is "COMMAND_NIV" - collects niv related debug information\n\t"
    "When <feature_name> is "COMMAND_RIOT" - collects riot related debug information\n\t"
    "When <feature_name> is "COMMAND_VXLAN" - collects vxlan related debug information\n\t"
    "When <feature_name> is "COMMAND_VLAN" - collects vlan related debug information\n"
#endif
    ;
extern cmd_result_t command_techsupport(int unit, args_t *a);

mbcm_techsupport_t *mbcm_techsupport[BCM_MAX_NUM_UNITS];

extern techsupport_data_t techsupport_l3uc_trident2plus_data;
extern techsupport_data_t techsupport_l3mc_trident2plus_data;
extern techsupport_data_t techsupport_mpls_trident2plus_data;
extern techsupport_data_t techsupport_niv_trident2plus_data;
extern techsupport_data_t techsupport_riot_trident2plus_data;
extern techsupport_data_t techsupport_vxlan_trident2plus_data;
extern techsupport_data_t techsupport_vlan_trident2plus_data;

/* Global structure that maintains different features for "Trident2plus" chipset */
mbcm_techsupport_t mbcm_trident2plus_techsupport[] = {
    {COMMAND_L3MC, &techsupport_l3mc_trident2plus_data},
    {COMMAND_L3UC, &techsupport_l3uc_trident2plus_data},
    {COMMAND_MPLS, &techsupport_mpls_trident2plus_data},
    {COMMAND_NIV, &techsupport_niv_trident2plus_data},
    {COMMAND_RIOT, &techsupport_riot_trident2plus_data},
    {COMMAND_VXLAN, &techsupport_vxlan_trident2plus_data},
    {COMMAND_VLAN, &techsupport_vlan_trident2plus_data},
    {NULL, NULL} /* Must be the last element in this structure */
  };


static struct techsupport_cmd {
    const char *techsupport_cmd;
    int (*techsupport_func) (int unit, args_t *a,
                             techsupport_data_t *techsupport_feature_data);
} techsupport_cmds[] = {
    {COMMAND_BASIC, techsupport_basic },
    {COMMAND_L3MC, techsupport_l3mc},
    {COMMAND_L3UC, techsupport_l3uc},
    {COMMAND_MPLS, techsupport_mpls},
    {COMMAND_NIV, techsupport_niv},
    {COMMAND_RIOT, techsupport_riot},
    {COMMAND_VLAN, techsupport_vlan},
    {COMMAND_VXLAN, techsupport_vxlan},
    {NULL, NULL } /* Must be the last element in this structure */
  };

/* Function:  techsupport_command_execute
 * Purpose :  Executes the specified "command" and prints the time
 *            taken to execute the specified "command".
 * Parameters: unit - Unit number
 *             command - pointer to command.
*/
void
techsupport_command_execute(int unit, char *command)
{
    int  diff = 0, diff1 = 0, j = 0;
    char str[MAX_STR_LEN] = {0};
    char *pattern1 = "Output of \"";
    char *pattern2 = "\" start";
    char *pattern3 = "\" end";
    sal_usecs_t  start_time, end_time;
    unsigned int  milliseconds = 0, seconds = 0, minutes = 0;
    /* Before actually dumping the command output, head string is
     * printed. Example of head string is something like below.
     * >>>>>>>>>>>>>>>>>>>>>>Output of "config show" start>>>>>>>>>>>>>>>>>>>
     * The below logic froms the head string.
    */
    memset(str, 0, MAX_STR_LEN);
    diff = MAX_STR_LEN - (strlen(command) + strlen(pattern1) + strlen(pattern2));
    diff = (diff % 2) == 0  ?  diff : ( diff + 1 );

    for (j=0; j < (diff / 2); j++) {
        str[j]='>';
    }

    cli_out("\n%s%s%s%s%s\n\n", str, pattern1, command, pattern2,str);
    start_time = sal_time_usecs();
    sh_process_command(unit, command);
    end_time = sal_time_usecs();
    milliseconds = (end_time - start_time) / MILLISECOND_USEC;

    /* After dumping the command output, tail string is
     * printed. Example of tail string is something like below.
     * <<<<<<<<<<<<<<<<<<<<<<Output of "config show" end<<<<<<<<<<<<<<<<<<<<<<<
     * The below logic froms the tail string.
    */

    memset(str, 0, MAX_STR_LEN);
    diff1 = MAX_STR_LEN - (strlen(command) + strlen(pattern1) + strlen(pattern3));
    diff1 = (diff1 % 2) == 0  ?  diff1 : ( diff1 + 1 );

    for (j=0; j < (diff1 / 2); j++) {
        str[j]='<';
    }
    cli_out("\n%s%s%s%s%s\n", str, pattern1, command, pattern3, str);

    if (milliseconds >= 1000) {
        seconds = (end_time - start_time) / SECOND_USEC;
    }

    if (seconds > 60) {
        minutes = seconds / 60;
        cli_out("\nThe time taken to execute \"%s\" is %d minutes.\n", command, minutes);
    } else if (milliseconds >= 1000) {
        cli_out("\nThe time taken to execute \"%s\" is %d seconds.\n", command, seconds);
    } else if (milliseconds > 0 ) {
        cli_out("\nThe time taken to execute \"%s\" is %d milliseconds.\n", command, milliseconds);
    }
}
/* Function:   techsupport_feature_process
 *  Purpose :  Executes the following
 *             1) diag shell comands(techsupport_feature_data->techsupport_data_diag_cmdlist[])
 *             2) dumps the memories(techsupport_feature_data->techsupport_data_mem_list[])
 *             3) dumps the registers(techsupport_feature_data->techsupport_data_reg_list[])
 * Parameters: unit - Unit number
 *             a - pointer to argument.
 *             techsupport_data_t -structure that maintains per chip per feature debug info.
 * Returns:    CMD_OK :done
 *             CMD_FAIL : INVALID INPUT
 */
int
techsupport_feature_process(int unit, args_t *a,
                          techsupport_data_t *techsupport_feature_data)
{
    int i=0;
    char *arg1;
    char command_str[MAX_STR_LEN] ={0}, command_options_flag = 0, dump_all = 1;
    char dont_execute = 0;
    /* By default, dump only the changes */
    command_options_flag |= DUMP_TABLE_CHANGED;

    /* Parse the options */
    arg1 = ARG_GET(a);
    for (;;) {
        if (arg1 != NULL && !sal_strcasecmp(arg1, VERBOSE)) {
            command_options_flag &= ~(DUMP_TABLE_CHANGED);
            arg1 = ARG_GET(a);
        } else if (arg1 != NULL && !sal_strcasecmp(arg1, REG)) {
            command_options_flag |= DUMP_REGISTERS;
            arg1 = ARG_GET(a);
            dump_all = 0;
        } else if (arg1 != NULL && !sal_strcasecmp(arg1, MEM)) {
            command_options_flag |= DUMP_MEMORIES;
            arg1 = ARG_GET(a);
            dump_all = 0;
        } else if (arg1 != NULL && !sal_strcasecmp(arg1, DIAG)) {
            command_options_flag |= DUMP_DIAG_CMDS;
            arg1 = ARG_GET(a);
            dump_all = 0;
        } else if (arg1 != NULL && !sal_strcasecmp(arg1, LIST)) {
            command_options_flag |= DUMP_LIST;
            arg1 = ARG_GET(a);
            /* if "list" option is specified, just list out the following
             * with out executing.
             * 1)diag commands
             * 2)Memory names
             * 3)Register names
             */
            dont_execute = 1;
        }
        else {
            break;
        }
    }

    /* Displays the output of feature specific diag commands */
    if (dump_all == 1 || (command_options_flag & DUMP_DIAG_CMDS)) {
        if (1 == dont_execute) {
            cli_out("\nList of Diag commands:\n");
            cli_out(">>>>>>>>>>>>>>>>>>>>>>\n");
        }
        for(i = 0; techsupport_feature_data->techsupport_data_diag_cmdlist[i] != NULL; i++) {
            if (1 == dont_execute) {
                cli_out("%s\n", techsupport_feature_data->techsupport_data_diag_cmdlist[i]);
            } else {
                techsupport_command_execute(unit, techsupport_feature_data->techsupport_data_diag_cmdlist[i]);
            }
        }
    }

    /* Dumps feature specific memory tables */
    if (dump_all == 1 || (command_options_flag & DUMP_MEMORIES)) {
        if (1 == dont_execute) {
            cli_out("\nList of Memory Table names:\n");
            cli_out(">>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
        }
        for (i=0; techsupport_feature_data->techsupport_data_mem_list[i] != INVALIDm ; i++) {
            if (1 == dont_execute) {
                cli_out("%s\n", SOC_MEM_UFNAME(unit, techsupport_feature_data->techsupport_data_mem_list[i]));
            } else {
                sal_strncpy(command_str, DUMP, sizeof(DUMP));

                if (command_options_flag & DUMP_TABLE_CHANGED) {
                    sal_strncat(command_str, CHANGE, (MAX_STR_LEN - strlen(command_str)));
                }
                sal_strncat(command_str,
                        SOC_MEM_UFNAME(unit, techsupport_feature_data->techsupport_data_mem_list[i]),
                        (MAX_STR_LEN - strlen(command_str)));
                techsupport_command_execute(unit, command_str);
            }
        }
    }

    /* Dumps feature specific registers */
    if (dump_all == 1 || (command_options_flag & DUMP_REGISTERS)) {
        if (1 == dont_execute) {
            cli_out("\nList of Registers:\n");
            cli_out(">>>>>>>>>>>>>>>>>>\n");
        }
        for (i = 0; techsupport_feature_data->techsupport_data_reg_list[i].reg != INVALIDr; i++) {
            if (1 == dont_execute) {
                cli_out("%s\n", SOC_REG_NAME(unit, techsupport_feature_data->techsupport_data_reg_list[i].reg));
            } else {
                sal_strncpy(command_str, GET_REG, sizeof(GET_REG));
                if (command_options_flag & DUMP_TABLE_CHANGED){
                    sal_strncat(command_str, CHANGE, (MAX_STR_LEN - strlen(command_str)));
                }
                sal_strncat(command_str,  SOC_REG_NAME(unit, techsupport_feature_data->techsupport_data_reg_list[i].reg),
                                      (MAX_STR_LEN - strlen(command_str)));
                techsupport_command_execute(unit, command_str);
            }
        }
    }
    return 0;
}
/* Function:    command_techsupport
 * Purpose :    Displays all the debug info for a given subfeature.
 * Parameters:  unit - Unit number
 *              a - pointer to argument.
 * Returns:     CMD_OK :done
 *              CMD_FAIL : INVALID INPUT
 */
cmd_result_t
command_techsupport(int unit, args_t *a)
{
    int rc = 0;
    sal_usecs_t  start_time = 0, end_time = 0;
    char *feature_name = ARG_GET(a);
    char feature_name_supported_flag = 0;
    unsigned int milliseconds = 0,seconds = 0,minutes = 0;
    struct techsupport_cmd *command;
    techsupport_data_t *techsupport_feature_data;
    mbcm_techsupport_t *mbcm_techsupport_ptr;

    if (NULL == mbcm_techsupport[unit]) {
        if (SOC_IS_TRIDENT2PLUS(unit)) {
            mbcm_techsupport[unit] = mbcm_trident2plus_techsupport;
        } else {
            cli_out("\nError:Command is not supported on chip %s.\n", SOC_UNIT_NAME(unit));
            return CMD_FAIL;
        }
    }

    mbcm_techsupport_ptr = mbcm_techsupport[unit];
    for (; mbcm_techsupport_ptr->techsupport_feature_name; mbcm_techsupport_ptr++) {
         if (feature_name == NULL) {
             cli_out("Error: INVALID INPUT\n");
             cli_out("For valid input use 'techSupport help' Command\n\n");
             return CMD_FAIL;
         } else if (sal_strcasecmp(feature_name, mbcm_techsupport_ptr->techsupport_feature_name) == 0) {
             techsupport_feature_data = mbcm_techsupport_ptr->techsupport_data;
             feature_name_supported_flag = 1;
             break;
         } else if (sal_strcasecmp(feature_name, "basic") == 0) {
             techsupport_feature_data = NULL;
             feature_name_supported_flag = 1;
             break;
         }
    }

    if (feature_name_supported_flag == 0) {
        cli_out("command \"techSupport %s\" not supported on chip %s.\n", feature_name, SOC_UNIT_NAME(unit));
        return CMD_FAIL;
    }

    for (command = techsupport_cmds; command->techsupport_cmd; command++) {
        if (sal_strcasecmp(feature_name, command->techsupport_cmd) == 0) {
            start_time = sal_time_usecs();
            rc = (command->techsupport_func(unit, a, techsupport_feature_data));
            end_time = sal_time_usecs();
            milliseconds = (end_time - start_time) / MILLISECOND_USEC;
            if(milliseconds >= 1000) {
                seconds = (end_time - start_time) / SECOND_USEC;
            }
            cli_out("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
            if (seconds > 60) {
                minutes = seconds / 60;
                cli_out("The time taken to execute \"techsupport %s\" [options..]"
                    " is %d minutes", command->techsupport_cmd, minutes);
            } else if (milliseconds >= 1000) {
               cli_out("The time taken to execute \"techsupport %s\" [options..]"
                   " is %d seconds", command->techsupport_cmd, seconds);
            } else if (milliseconds > 0) {
               cli_out("The time taken to execute \"techsupport %s\" [options..]"
                   " is %d milliseconds", command->techsupport_cmd, milliseconds);
            }
            cli_out("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
            return rc;
        }
    }
    return 0;
}
