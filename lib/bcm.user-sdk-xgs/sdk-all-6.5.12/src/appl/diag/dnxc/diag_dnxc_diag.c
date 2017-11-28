/** \file diag_dnxc_diag.c
 *
 * diagnostic pack for fabric
 */
/*
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_FABRIC

/*
 * INCLUDE FILES:
 * {
 */
 /*
  * shared
  */
#include <shared/bsl.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
/*appl*/
#include <appl/diag/diag.h>
#include <appl/diag/system.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/dnxf/diag_dnxf_fabric.h>
/*bcm*/
#include <bcm/fabric.h>
/*soc*/
#ifdef BCM_DNXF_SUPPORT
#include <soc/dnxf/cmn/dnxf_diag.h>
#include <soc/dnxf/cmn/mbcm.h>
#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/ramon/ramon_cgm.h>
#endif /* BCM_DNXF_SUPPORT */
/* Put your new common defines in this file*/
#include <soc/dnxc/legacy/dnxc_defs.h>
#include <soc/dnxc/legacy/fabric.h>
/*sal*/
#include <sal/appl/sal.h>

extern shr_error_e sh_dnx_diag_counter_graphical_print(
    int unit,
    int cdsp,
    int non_zero,
    int in_hex,
    uint32 interval);

/*************
 *  DEFINES  *
 *************/

/*************
 *  MACROES  *
 *************/

/* currently 126 blocks in Ramon and 213 blocks in Jericho2 */
#define BLOCK_MAX_INDEX (256)
#define NOF_BITS_PER_WORD (32)
#define FULL_32_BITS_MASK (0xffffffff)

/*************
 * FUNCTIONS *
 *************/

/**
 * \brief
 *   translate hex value to a string"
 * \param [in] val - hex value stored in the uint32 array
 * \param [in] nof_bit - number of bits this decimal value occupies
 * \param [out] buf - buffer to store the string
 * \return
 *      Standard error handling
 */
shr_error_e
format_value_hex_to_str(
    uint32 *val,
    int nof_bit,
    char *buf)
{
    int i;
    int nof_byte;
    int nof_words, nof_bit_in_mask, mask;
    uint8 *source = (uint8 *) val;

    if (NULL == val)
    {
        LOG_CLI(("Invalid parameter: empty decimal value\n"));
        return _SHR_E_PARAM;
    }
    if (NULL == buf)
    {
        LOG_CLI(("Invalid parameter: empty space to store string\n"));
        return _SHR_E_PARAM;
    }

    nof_byte = BITS2BYTES(nof_bit);
    nof_words = BITS2WORDS(nof_bit);
    nof_bit_in_mask = nof_bit % NOF_BITS_PER_WORD;
    mask = (0 == nof_bit_in_mask) ? (FULL_32_BITS_MASK) : ((1 << nof_bit_in_mask) - 1);
    val[nof_words - 1] &= mask;
    /*
     * Skip leading zeroes
     */
    for (i = nof_byte - 1; i > 0; i--)
    {
        if (source[i])
        {
            break;
        }
    }
    /*
     * Print first word
     */
    sal_sprintf(buf, "0x%x", source[i]);
    /*
     * Print rest of words, if any
     */
    while (--i >= 0)
    {
        sal_sprintf(buf + sal_strlen(buf), "%02x", source[i]);
    }

    return _SHR_E_NONE;
}

/**
 * \brief
 *   translate decimal value (max 64 bits) to a string"
 * \param [in] val - decimal value stored in the uint32 array
 * \param [in] nof_bit - number of bits this decimal value occupies
 * \param [out] buf - buffer to store the string
 * \return
 *      Standard error handling
 */
shr_error_e
format_value_dec_to_str(
    uint32 *val,
    int nof_bit,
    char *buf)
{
    uint64 val_64;
    int nof_words, nof_bit_in_mask, mask;

    if (NULL == val)
    {
        LOG_CLI(("Invalid parameter: empty decimal value\n"));
        return _SHR_E_PARAM;
    }
    if (NULL == buf)
    {
        LOG_CLI(("Invalid parameter: empty space to store string\n"));
        return _SHR_E_PARAM;
    }

    nof_words = BITS2WORDS(nof_bit);
    nof_bit_in_mask = nof_bit % NOF_BITS_PER_WORD;
    mask = (0 == nof_bit_in_mask) ? (FULL_32_BITS_MASK) : ((1 << nof_bit_in_mask) - 1);
    val[nof_words - 1] &= mask;
    /*
     * set the second word to 0 if decimal value occupy one word only
     */
    if (1 == nof_words)
    {
        val[1] = 0;
    }

    COMPILER_64_SET(val_64, val[1], val[0]);
    format_uint64_decimal(buf, val_64, ',');

    return _SHR_E_NONE;
}

/**
 * \brief
 *   translate decimal (max 64 bits) or hex value  to a string"
 * \param [in] val - the value stored in the uint32 array
 * \param [in] nof_bit - number of bits this decimal value occupies
 * \param [in] in_hex - indicate decimal or hex string is required
 * \param [out] buf - buffer to store the string
 * \return
 *      Standard error handling
 */
shr_error_e
format_value_to_str(
    uint32 *val,
    int nof_bit,
    int in_hex,
    char *buf)
{
    if (NULL == val)
    {
        LOG_CLI(("Invalid parameter: empty decimal value\n"));
        return _SHR_E_PARAM;
    }
    if (NULL == buf)
    {
        LOG_CLI(("Invalid parameter: empty space to store string\n"));
        return _SHR_E_PARAM;
    }

    /*
     * when counter length is larger than 64 bits, to display in hex format 
     */
    if (in_hex || (nof_bit > 64))
    {
        format_value_hex_to_str(val, nof_bit, buf);
    }
    else
    {
        format_value_dec_to_str(val, nof_bit, buf);
    }

    return _SHR_E_NONE;
}

/**
 * \brief
 *   add two values together and then translate the sum (decimal or hex) to a string
 *   the supported maximal value is 64 bits.
 * \param [in] val_1 - value 1 stored in the uint32 array
 * \param [in] val_2 - value 2 stored in the uint32 array
 * \param [in] nof_bit - number of bits this decimal value occupies
 * \param [in] in_hex - indicate decimal or hex string is required
 * \param [out] buf - buffer to store the string
 * \return
 *      Standard error handling
 */
shr_error_e
sum_value_to_str(
    uint32 *val_1,
    uint32 *val_2,
    int nof_bit,
    int in_hex,
    char *buf)
{
    int i;
    uint64 val_64_1, val_64_2;
    int nof_bit_i, nof_words, nof_byte, nof_bit_in_mask, mask;
    uint8 *source;

    if ((NULL == val_1) || (NULL == val_2))
    {
        LOG_CLI(("Invalid parameter: empty value\n"));
        return _SHR_E_PARAM;
    }
    if (NULL == buf)
    {
        LOG_CLI(("Invalid parameter: empty space to store string\n"));
        return _SHR_E_PARAM;
    }

    nof_bit_i = nof_bit > 64 ? 64 : nof_bit;
    nof_byte = BITS2BYTES(nof_bit_i);
    nof_words = BITS2WORDS(nof_bit_i);
    nof_bit_in_mask = nof_bit_i % NOF_BITS_PER_WORD;
    mask = (0 == nof_bit_in_mask) ? (FULL_32_BITS_MASK) : ((1 << nof_bit_in_mask) - 1);
    val_1[nof_words - 1] &= mask;
    val_2[nof_words - 1] &= mask;
    /*
     * set the second word to 0 if decimal value occupy one word only 
     */
    if (1 == nof_words)
    {
        val_1[1] = 0;
        val_2[1] = 0;
    }
    COMPILER_64_SET(val_64_1, val_1[1], val_1[0]);
    COMPILER_64_SET(val_64_2, val_2[1], val_2[0]);
    COMPILER_64_ADD_64(val_64_1, val_64_2);

    if (in_hex)
    {
        source = (uint8 *) &val_64_1;
        /*
         * Skip leading zeroes 
         */
        for (i = nof_byte - 1; i > 0; i--)
        {
            if (source[i])
            {
                break;
            }
        }
        /*
         * Print first word 
         */
        sal_sprintf(buf, "0x%x", source[i]);
        /*
         * Print rest of words, if any 
         */
        while (--i >= 0)
        {
            sal_sprintf(buf + sal_strlen(buf), "%02x", source[i]);
        }
    }
    else
    {
        format_uint64_decimal(buf, val_64_1, ',');
    }

    return _SHR_E_NONE;
}

/**
 * \brief
 *   Check if the register match the blocks specified by "*match_n"
 * \param [in] unit - Unit #
 * \param [in] match_n - to specify block name that block should match with
 * \param [out] nof_block - to get number of blocks that this register matches with
 * \param [out] block_ids - an integer array to get all of block ID that this register matches with
 * \return
 *      Standard error handling
 */
static shr_error_e
counter_block_name_match(
    int unit,
    char *match_n,
    int *nof_block,
    int *block_ids)
{
    shr_error_e rv = _SHR_E_NOT_FOUND;
    int i_bl;

    if (NULL == nof_block)
    {
        LOG_CLI(("Invalid parameter: nof_block\n"));
        return _SHR_E_PARAM;
    }
    if (NULL == block_ids)
    {
        LOG_CLI(("Invalid parameter: block_ids\n"));
        return _SHR_E_PARAM;
    }

    *nof_block = 0;
    sal_memset(block_ids, 0, BLOCK_MAX_INDEX * sizeof(int));
    for (i_bl = 0; SOC_BLOCK_INFO(unit, i_bl).type >= 0; i_bl++)
    {
        if (sal_strcasestr(SOC_BLOCK_NAME(unit, i_bl), match_n))
        {
            if (*nof_block >= BLOCK_MAX_INDEX)
            {
                LOG_CLI(("Number of block:%d exceeded maximum:%d\n", *nof_block, BLOCK_MAX_INDEX));
                rv = _SHR_E_PARAM;
                break;
            }
            block_ids[*nof_block] = i_bl;
            *nof_block += 1;
            rv = _SHR_E_NONE;
        }
    }

    return rv;
}

/**
 * \brief
 *   to display the counters in the blocks specified by option "name"
 *   or the counters in all the blocks when setting option "all"
 * \param [in] unit - Unit #
 * \param [in] match_n - pointer to string to filter block name
 * \param [in] non_zero - indicate to display non_zero counters only
 * \param [in] in_hex - indicate to display counters in hex format
 * \param [in] interval - indicate the interval of g-timer
 * \param [in] sand_control -
 * \return
 *      Standard error handling
 */
shr_error_e
sh_dnxc_diag_counters_block_print(
    int unit,
    char *match_n,
    int non_zero,
    int in_hex,
    uint32 interval,
    sh_sand_control_t * sand_control)
{
    int i_fld, i_bl, i_ind;
    int numels, object_col_id;
    int idx = 0, nof_block = 0;
    int block_ids[BLOCK_MAX_INDEX];
    soc_reg_t reg;
    soc_field_info_t *fld;
    soc_reg_above_64_val_t fld_value;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(NO_UNIT);

    if (interval)
    {
        /*
         * to add code to implement g-timer
         */
    }

    PRT_TITLE_SET("BLOCK COUNTERS");

    /*
     * add column for displaying counters per block
     */
    PRT_COLUMN_ADDX(PRT_XML_CHILD, PRT_TITLE_ID, &object_col_id, "No.");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Block");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Counter");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Numels");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Value");

    if (counter_block_name_match(unit, match_n, &nof_block, block_ids) != _SHR_E_NONE)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "no matched block found\n");
    }

    for (reg = 0; reg < NUM_SOC_REG; reg++)
    {
        soc_reg_info_t *reginfo;
        soc_reg_above_64_val_t reg_value;
        char value_str[PRT_COLUMN_WIDTH_BIG];

        if (!SOC_REG_IS_VALID(unit, reg))
        {
            continue;
        }
        reginfo = &SOC_REG_INFO(unit, reg);

        if ((numels = reginfo->numels) == 0)
        {
            /*
             * Simulate index 0 for non arrayed registers, should not happen
             */
            numels = 1;
        }
        /*
         * filter out write-only registers
         */
        if (reginfo->flags & SOC_REG_FLAG_WO)
        {
            continue;
        }
        if (!SOC_REG_HAS_COUNTER_FIELDS(unit, reg))
        {
            continue;
        }
        /*
         * display this counter in all matched blocks
         */
        for (i_bl = 0; i_bl < nof_block; i_bl++)
        {
            if (!SOC_BLOCK_IS_TYPE(unit, block_ids[i_bl], reginfo->block))
            {
                continue;
            }
            for (i_ind = 0; i_ind < numels; i_ind++)
            {
                SHR_IF_ERR_EXIT_WITH_LOG(soc_reg_above_64_get
                                         (unit, reg, SOC_BLOCK_PORT(unit, block_ids[i_bl]), i_ind, reg_value),
                                         "ERROR: read from general register %s(%d).%s failed\n", SOC_REG_NAME(unit,
                                                                                                              reg),
                                         i_ind, SOC_BLOCK_NAME(unit, block_ids[i_bl]));
                for (i_fld = 0; i_fld < reginfo->nFields; i_fld++)
                {
                    fld = &reginfo->fields[i_fld];
                    if (!(fld->flags & SOCF_COUNTER))
                    {
                        continue;
                    }
                    SOC_REG_ABOVE_64_CLEAR(fld_value);
                    soc_reg_above_64_field_get(unit, reg, reg_value, fld->field, fld_value);
                    if (non_zero && SOC_REG_ABOVE_64_IS_ZERO(fld_value))
                    {
                        continue;
                    }
                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SET("%d", idx++);
                    PRT_CELL_SET("%s", SOC_BLOCK_NAME(unit, block_ids[i_bl]));
                    PRT_CELL_SET("%s", SOC_FIELD_NAME(unit, fld->field));
                    PRT_CELL_SET("%d", i_ind);
                    /*
                     * when counter length is larger than 64 bits, to display in hex format
                     */
                    if (in_hex || (fld->len > 64))
                    {
                        format_value_hex_to_str(fld_value, fld->len, value_str);
                    }
                    else
                    {
                        format_value_dec_to_str(fld_value, fld->len, value_str);
                    }
                    PRT_CELL_SET("%s", value_str);
                }
            }
        }
    }
    /*
     * coverity[stack_use_overflow : FALSE] 
     */
    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   to display the counters in the internal blocks in virable format specified by option
 * \param [in] unit - Unit #
 * \param [in] args -
 * \param [in] sand_control -
 * \return
 *      Standard error handling
 */
static shr_error_e
sh_dnxc_diag_counter_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *match_n;
    uint32 interval;
    int non_zero, in_hex, graphical;
#ifdef BCM_DNX_SUPPORT
    int cdsp;
#endif
    SHR_FUNC_INIT_VARS(NO_UNIT);

    /*
     * Get parameters
     */
    SH_SAND_GET_STR("variable", match_n);
    SH_SAND_GET_BOOL("GRaphical", graphical);
    SH_SAND_GET_BOOL("NonZero", non_zero);
    SH_SAND_GET_BOOL("hex", in_hex);
    SH_SAND_GET_UINT32("InTerVal", interval);
#ifdef BCM_DNX_SUPPORT
    SH_SAND_GET_BOOL("CoreDiSPlay", cdsp);
#endif
    if (graphical)
    {
#ifdef BCM_DNXF_SUPPORT
        if (SOC_IS_DNXF(unit))
        {
            SHR_IF_ERR_EXIT(diag_dnxf_counters_graphical_print(unit, sand_control));
        }
#endif
#ifdef BCM_DNX_SUPPORT
        if (SOC_IS_DNX(unit))
        {
            SHR_IF_ERR_EXIT(sh_dnx_diag_counter_graphical_print(unit, cdsp, non_zero, in_hex, interval));
        }
#endif
    }
    else
    {
        SHR_IF_ERR_EXIT(sh_dnxc_diag_counters_block_print(unit, match_n, non_zero, in_hex, interval, sand_control));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   to display the avs
 * \param [in] unit - Unit #
 * \param [in] args -
 * \param [in] sand_control -
 * \return
 *      Standard error handling
 */
static shr_error_e
sh_dnxc_diag_avs_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 avs_val;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_switch_rov_get(unit, 0, &avs_val));

    PRT_TITLE_SET("AVS");
    PRT_COLUMN_ADD("Status");
    PRT_COLUMN_ADD("0x%x", avs_val);
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("########");
    PRT_CELL_SET("########");
    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/*
 * LOCAL DIAG PACK:
 * }
 */

/**
 * List of the supported commands, pointer to command function and command usage function.
 */

static sh_sand_option_t sh_dnxc_diag_counters_options[] = {
    /*
     * name type desc default ext
     */
    {"variable", SAL_FIELD_TYPE_STR, "free parament for block name, empty means all", "", NULL},
    {"graphical", SAL_FIELD_TYPE_BOOL, "display counters into graphic", "false", NULL},
    {"hex", SAL_FIELD_TYPE_BOOL, "display counter in hex format", "false", NULL},
    {"NonZero", SAL_FIELD_TYPE_BOOL, "display non-zero counter only or not", "false", NULL},
    {"interval", SAL_FIELD_TYPE_UINT32, "interval for gtimer", "0", NULL},
    {"CoreDiSPlay", SAL_FIELD_TYPE_BOOL, "display counter on core base", "false", NULL},
    {NULL}
};

static sh_sand_man_t sh_dnxc_diag_counters_man = {
    "Display the counters of the internal blocks",
    "Display the counters of the internal blocks in variable ways."
        "For example, to display the counters of the blocks specified by parameters, or"
        "to display counters into one graphic and so on",
    "For FE device: diag counter [<string>]|[graphical] [nz] [hex] [interval=<integer>]"
        "For FAP defice: diag counter [<string>]|[graphical] [nz] [hex] [cdsp] [interval=<integer>]",
    "diag counter - display the counters of all the blocks"
        "diag counter CGM - display the counters of block CGM"
        "diag counter CGM nz - display non-zero counters of block CGM\n"
        "diag counter CGM hex - display non-zero counters of block CGM in hex base\n"
        "diag counter CGM interval=10 - display counters of block CGM with gtimer started, interval is 10ms\n"
        "diag counter graphical - display counters into graphic\n"
        "diag counter graphical hex - display counters into graphic in hex base\n"
        "diag counter graphical interval=10 - display counters into graphic with gtimer started, interval is 10ms\n",
};

static sh_sand_option_t sh_dnxc_diag_avs_options[] = {
    /*
     * name type desc default ext
     */
    {"read", SAL_FIELD_TYPE_BOOL, "display AVS into graphic", "false", NULL},
    {NULL}
};

sh_sand_man_t sh_dnxc_diag_avs_man = {
    "Display the AVS value of the device",
    "to display AVS into one graphic and so on",
    "For FE device: AVS read",
    "AVS read - display the result of the read of the field"
};

sh_sand_cmd_t sh_dnxc_avs_cmds[] = {
    /*
     * keyword, action, command, options, man
     */
    {"read", sh_dnxc_diag_avs_cmd, NULL, sh_dnxc_diag_avs_options, &sh_dnxc_diag_avs_man},
    {NULL}
};

static sh_sand_invoke_t dnxc_diag_counter_invokes[] = {
    {"graphical", "graphical"},
    {NULL}
};

/**
 * Diag command should be used for backward compatibility requests only - new functionality should be assigned
 * appropriate specific commands
 */
sh_sand_cmd_t sh_dnxc_diag_cmds[] = {
    /*
     * keyword, action, command, options, man
     */
    {"counter", sh_dnxc_diag_counter_cmd, NULL, sh_dnxc_diag_counters_options, &sh_dnxc_diag_counters_man, NULL,
     dnxc_diag_counter_invokes},
    {NULL}
};

sh_sand_man_t sh_dnxc_diag_man = {
    "Misc facilities for displaying diagnostics  information",
    NULL,
    NULL,
    NULL,
};
