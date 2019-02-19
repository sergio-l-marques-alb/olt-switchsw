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

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DBALDNX

/*************
 * INCLUDES  *
 *************/
#include <shared/bsl.h>
#include <sal/appl/sal.h>
#include <shared/bslnames.h>
#include <soc/mcm/memregs.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <bcm_int/dpp/error.h>
#include <appl/diag/system.h>
#include <appl/diag/diag.h>
#include <src/soc/dnx/dbal/dbal.h>
#include "diag_dnx_dbal_tests.h"

/*************
 * DEFINES   *
 *************/
#define CMD_MAX_STRING_LENGTH   30
#define CMD_MAX_NOF_INPUTS      5

typedef cmd_result_t(
  *CMD_DIAG_FUNC) (
  int unit,
  char *params[CMD_MAX_NOF_INPUTS]);

typedef struct
  {
    char comand_name[CMD_MAX_STRING_LENGTH];
    char comand_name_short[CMD_MAX_STRING_LENGTH / 4];
    int num_of_mandatory_params;
    int num_of_optional_params;
    CMD_DIAG_FUNC diag_func;

  } cmd_string_func_t;

typedef struct
  {
    CMD_DIAG_FUNC diag_func;
    char *inputs[CMD_MAX_NOF_INPUTS];
    char *test_description;

  } regression_test_list_t;

/*************
* FUNCTIONS *
*************/
cmd_result_t
cmd_dbal_tables_dump(
  int unit,
  char *in_label[CMD_MAX_NOF_INPUTS])
{
  int res = CMD_OK;
  dbal_labels_e label;

  res = dbal_label_string_to_id(unit, in_label[0],&label);
  if (res != CMD_OK)
  {
    return res;
  }

  dbal_table_dump_all(unit, label, 0);

  return res;
}

cmd_result_t
cmd_dbal_tables_info_dump(
  int unit,
  char *table_name[CMD_MAX_NOF_INPUTS])
{
  int i;
  dbal_logical_table_t *table;
  int val = -1;

  if (0)
  {
    for (i = 0; i < DBAL_NOF_TABLES; i++)
    {
      dbal_logical_table_get(unit, i, &table);
      if (!sal_strcasecmp(table_name[0], table->table_name))
      {
        dbal_table_info_print(unit, i);
      }
    }

    if (i == DBAL_NOF_TABLES)
    {
      cli_out("No table found \n");
    }
  }

  val = sal_ctoi(table_name[0], 0);
  if ((val > DBAL_NOF_TABLES - 1) || (val < 0))
  {
    cli_out("table ID out of bounds \n");
    return CMD_OK;
  }
  else
  {
    dbal_table_info_print(unit, val);
  }

  return CMD_OK;
}

cmd_result_t
cmd_dbal_fields_dump(
  int unit,
  char *in_label[CMD_MAX_NOF_INPUTS])
{
  int res = CMD_OK;
  dbal_labels_e label;

  res = dbal_label_string_to_id(unit, in_label[0],&label);
  if (res != CMD_OK)
  {
    return res;
  }

  dbal_fields_dump_all(unit, label);

  return CMD_OK;
}

cmd_result_t
cmd_dbal_labels_dump(
  int unit,
  char *unused[CMD_MAX_NOF_INPUTS])
{
  dbal_labels_e label;

  cli_out("Existing labels: \n");
  cli_out("---------------- \n");
  for (label = 0; label < DBAL_NOF_LABEL_TYPES; label++)
  {
    cli_out("\t%s \n", dbal_label_to_string(unit, label));
  }

  return CMD_OK;
}


cmd_result_t
cmd_dbal_tables_entries_dump(
  int unit,
  char *table_name[CMD_MAX_NOF_INPUTS])
{
  dbal_logical_table_t *table;
  int table_id = -1;
  int iter;
  int field_id;   

  table_id = sal_ctoi(table_name[0], 0);

  dbal_logical_table_get(unit, table_id, &table);

  if ((table_id > DBAL_NOF_TABLES - 1) || (table_id < 0))
  {
    cli_out("table ID out of bounds \n");
    return CMD_OK;
  }
  else
  {
    if (table->access_type == DBAL_ACCESS_PHY_TABLE)
    {
      dbal_table_entries_print_from_shadow(unit, table_id, 0, 0);
    }
    else
    {
      uint32 returned_field_pointers[DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS][DBAL_FIELD_ARRAY_MAX_SIZE/4] = {{0}};
      dbal_iterator_info_t    iterator_info;
      dbal_table_field_info_t field_info = { 0 };

      dbal_table_iterator_init(unit, table_id, DBAL_ITER_MODE_GET_ALL_BUT_DEFAULT_ENTRIES, &iterator_info);      

      for(iter = 0; iter < table->num_of_fields; iter++)
      {
        field_id = table->fields_info[iter].field_id;
        DBAL_ITERATOR_RETURNED_INFO_SET(iterator_info, field_id, returned_field_pointers[iter])      
        /* dbal_table_iterator_info_dump(unit,&iterator_info); */
      }

      while (!iterator_info.is_end)
      {
        dbal_table_iterator_get_next(unit, &iterator_info);

        if (!iterator_info.is_end)
        {
          cli_out("entry %d: ", iterator_info.entry_number);
          for (iter = 0; iter < iterator_info.num_of_fields; iter++)
          {
            dbal_table_field_info_get(unit, table_id, iterator_info.requested_fields_info[iter].field_id, &field_info);          
            dbal_field_from_buffer_print(unit, iterator_info.requested_fields_info[iter].field_id, iterator_info.requested_fields_info[iter].field_val,
                                         NULL, field_info.field_nof_bits, 0);          
          }
          cli_out("\n");
        }        
      }

      dbal_table_iterator_destroy(unit, &iterator_info);
    }

  }

  return CMD_OK;
}

cmd_result_t
cmd_dbal_jer2_example(
  int unit,
  char *mode[CMD_MAX_NOF_INPUTS])
{
  int val = -1;

  val = sal_ctoi(mode[0], 0);
  dbal_jer2_example(unit, val);

  return CMD_OK;

}

cmd_result_t
cmd_dbal_mode_set(
  int unit,
  char *mode[CMD_MAX_NOF_INPUTS])
{
  int val = -1;

  val = sal_ctoi(mode[0], 0);

  dbal_sw_work_mode_set(unit, val);

  return CMD_OK;
}

cmd_result_t
cmd_dbal_print_mode_set(
  int unit,
  char *mode[CMD_MAX_NOF_INPUTS])
{
  int val = -1;
  bsl_severity_t severity = 0;

  val = sal_ctoi(mode[0], 0);

  if (!sal_strcasecmp(mode[0], "NONE"))
  {
    dbal_log_severity_get(unit, &severity);
    cli_out("Log severity: %s (%d)\n", bsl_severity2str(severity), severity);
  }
  else
  {
    if ((val > 6) || (val < -1))
    {
      cli_out("severity out of bounds \n");
      return CMD_OK;
    }
    else
    {
      dbal_log_severity_set(unit, val);
    }
  }

  return CMD_OK;
}

cmd_result_t
cmd_dbal_handles_info_dump(
  int unit,
  char *handle_id[CMD_MAX_NOF_INPUTS])
{
  dbal_handles_info_dump(unit, -1);

  return CMD_OK;
}

cmd_result_t
cmd_dbal_xml_parsing_validation(
  int unit,
  char *unused[CMD_MAX_NOF_INPUTS])
{
  return diag_dbal_test_xml_parsing(unit);
}

cmd_result_t
cmd_dbal_logical_tables_test(
  int unit,
  char *test_params[CMD_MAX_NOF_INPUTS])
{
  int res;
  int mode;
  dbal_tables_e table_id;

  /* second token is the test mode (default is 0)*/
  mode = sal_ctoi(test_params[1],0);
  if (mode > 1)
  {
    cli_out("unknown test mode, mode=%d.\n",mode);
    return CMD_FAIL;
  }

  if (sal_strcasecmp(test_params[0],"ALL") == 0)
  {
    int i;
    for (i=0;i<DBAL_NOF_TABLES;i++)
    {
      res = diag_dbal_test_logical_table(unit, i, mode);
      if (res != CMD_OK)
      {
        return res;
      }
    }
    cli_out("logical table test (all tables) - PASS\n");
    return res;
  }

  /* first token should be the table name*/
  res = dbal_logical_table_string_to_id(unit,test_params[0],&table_id);
  if (res != CMD_OK)
  {
    cli_out("unknown table, %s.\n",test_params[0]);
    return CMD_FAIL;
  }

  return diag_dbal_test_logical_table(unit, table_id, mode);
}

cmd_result_t
cmd_dbal_regression(
  int unit,
  char *test_params[CMD_MAX_NOF_INPUTS])
{
  int res,i;
  int bsl_severity;
  int nof_func;
  int pass_counter=0;

  regression_test_list_t test_list[] = {
    {cmd_dbal_jer2_example,           {"0"}       ,"jer2 dbal example, scenario=0"},
    {cmd_dbal_jer2_example,           {"1"}       ,"jer2 dbal example, scenario=1"},
    {cmd_dbal_jer2_example,           {"4"}       ,"jer2 dbal example, scenario=4"},
    {cmd_dbal_jer2_example,           {"8"}       ,"jer2 dbal example, scenario=8"},
    {cmd_dbal_xml_parsing_validation, {"NONE"}    ,"xml parsing test"},
    {cmd_dbal_logical_tables_test,    {"ALL","0"} ,"basic logical tables test (all tables)"}
  };

  SHR_GET_SEVERITY_FOR_MODULE(bsl_severity);
  SHR_SET_SEVERITY_FOR_MODULE(bslSeverityError);

  nof_func = sizeof(test_list)/sizeof(regression_test_list_t);

  for (i=0;i<nof_func;i++)
  {
    cli_out("***************************************************************************\n"
            "*** Regression test (%d/%d) :  %s ***\n"
            "***************************************************************************\n",
            i+1,nof_func,test_list[i].test_description);

    res = test_list[i].diag_func(unit,test_list[i].inputs);

    cli_out("***************************************\n"
            "*** Regression test (%02d/%02d) :  %s ***\n"
            "***************************************\n",
            i+1, nof_func,res==CMD_OK?"PASS":"FAIL");

    if (res == CMD_OK)
      pass_counter++;
  }

  SHR_SET_SEVERITY_FOR_MODULE(bsl_severity);

  if (pass_counter == nof_func)
  {
    cli_out("****************************************\n"
            "******** DBAL REGRESSION PASSED ********\n"
            "****************************************\n");
    return CMD_OK;

  }
  else
  {
    cli_out("****************************************\n"
            "******** DBAL REGRESSION FAILED ********\n"
            "****************************************\n");
    return CMD_OK;
  }
}

char cmd_dnx_dbal_usage[] =
  "Logical Table Manager:\n"
  "Options:\n"
  "\tLTM TablesDump <label>             dump all tables (if label exists, dump all tables that related to a specific label)\n"
  "\tLTM TableInfoDump <table ID>       dump info related to a specific table\n"
  "\tLTM TableEntriesDump <table ID>    dump all entries related to a specific table\n"
  "\tLTM FieldsDump <label>             dump all fields (if label exists, dump all fields that related to a specific label)\n"
  "\tLTM LabelsDump                     dump all available labels \n"
  "\tLTM WorkMode <ID>                  change Access work mode, 0: Normal, 1:No HW access\n"
  "\tLTM logSeverity <severity>         change the severity of the logger, 0: No prints, 1: fatal prints ... 6: all prints, no input:dump current severity\n"
  "\tLTM HandlesStatus                  dump entry handle status and basic info \n"
  "\tLTM LogicalTablesTest <ID> <mode>  logical table test. mode: 0-basic, 1-advance \n"
  "\tLTM XmlParsingValidation           validate correctness of xml parsing \n"
  "\tLTM DbalRegression                 Runs regressino based on the existing diagnostics \n"
  /*
   * "\tLTM Demo <ID>                        adding entries to tables according to ID \n"
   */
  ;

cmd_result_t
cmd_dnx_dbal_list(
  int unit,
  args_t * a)
{
  char *function;
  char *cmd_input[CMD_MAX_NOF_INPUTS];
  int i,j,input_index=0;
  int nof_commands;
  uint8 find_input = FALSE;

  cmd_result_t rv = CMD_OK;

  const cmd_string_func_t commands_array[] = {
/* 
 * comand_name, comand_name_short, num_of_mandatory_params, num_of_optional_params, diag_func
 */
    {"DbalRegression",   "dr" , 0, 0, cmd_dbal_regression},
    {"TablesDump",       "td" , 0, 1, cmd_dbal_tables_dump},
    {"TableInfoDump",    "tid", 1, 0, cmd_dbal_tables_info_dump},
    {"TableEntriesDump", "ted", 1, 0, cmd_dbal_tables_entries_dump},
    {"FieldsDump",       "fd" , 0, 1, cmd_dbal_fields_dump},
    {"Lables",           "ld" , 0, 0, cmd_dbal_labels_dump},
    {"WorkMode",         "wm" , 1, 0, cmd_dbal_mode_set},
    {"logSeverity",      "ls" , 0, 1, cmd_dbal_print_mode_set},
    {"Demo",             "d"  , 1, 0, cmd_dbal_jer2_example},
    {"HandlesStatus",    "hs" , 0, 0, cmd_dbal_handles_info_dump},
    {"LogicalTablesTest","ltt", 1, 1, cmd_dbal_logical_tables_test},
    {"XmlParsingValid",  "xpv", 0, 0, cmd_dbal_xml_parsing_validation}
  };

  if (!(function = ARG_GET(a)))
  {     
    /* Nothing to do    */
    return (CMD_USAGE);
  }
  nof_commands = sizeof(commands_array)/sizeof(cmd_string_func_t);

  for (i = 0; i < nof_commands; i++)
  {
    if ((!sal_strcasecmp(function, commands_array[i].comand_name))
        || (!sal_strcasecmp(function, commands_array[i].comand_name_short)))
    {
      /* read mandatory params*/
      for (j = 0; j < commands_array[i].num_of_mandatory_params; j++)
      {
        if (!(cmd_input[input_index++] = ARG_GET(a)))
        {
          return (CMD_USAGE);
        }
        find_input = TRUE;
      }

      /* read optional params*/
      for (j = 0; j < commands_array[i].num_of_optional_params; j++)
      {
        if (!(cmd_input[input_index++] = ARG_GET(a)))
        {
          break;
        }
        find_input = TRUE;
      }
      if (!find_input)
      {
        cmd_input[0] = "NONE";
      }
      rv = commands_array[i].diag_func(unit, cmd_input);
      break;
    }
  }
  return rv;
}
