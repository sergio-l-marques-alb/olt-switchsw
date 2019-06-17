/** \file diag_dnxc_diag.c
 *
 * diagnostic pack for fabric
 */
/*
 * $Copyright: (c) 2018 Broadcom.
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
#include <appl/diag/dnxc/diag_dnxc_diag.h>
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
    uint32 interval,
    sh_sand_control_t * sand_control);
extern shr_error_e sh_dnx_diag_block_gtimer_start(
    int unit,
    int nof_block,
    int *block_ids,
    int interval);
extern shr_error_e sh_dnx_diag_block_gtimer_stop(
    int unit,
    int nof_block,
    int *block_ids);

/*************
 * FUNCTIONS *
 *************/
/*
 * { DEFINES
 */
#define DIAG_DNXC_DIAG_COUNTER_FULL_MAN             "Display the counters of the internal blocks in variable ways. " \
                                                    "For example, to display the counters of the blocks specified by block name, or to display specific" \
                                                    " counters in graphical view"

#define DIAG_DNXC_DIAG_COUNTER_OPTIONS  \
     /** name type desc default ext */ \
    {"block", SAL_FIELD_TYPE_STR, "Block name, empty means all", "", NULL, NULL, SH_SAND_ARG_FREE}, \
    {"graphical", SAL_FIELD_TYPE_BOOL, "display counters in graphic view", "false", NULL}, \
    {"hex", SAL_FIELD_TYPE_BOOL, "display counter in hex format", "false", NULL}, \
    {"NonZero", SAL_FIELD_TYPE_BOOL, "display non-zero counter only", "false", NULL}, \
    {"interval", SAL_FIELD_TYPE_UINT32, "interval for gtimer (second)", "0", NULL}
/*
 * }
 */
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

    if (val[1] > 0)
    {
        sal_snprintf(buf, PRT_COLUMN_WIDTH, "0x%x", val[1]);
        sal_snprintf(buf + sal_strlen(buf), PRT_COLUMN_WIDTH, "%08x", val[0]);
    }
    else
    {
        sal_snprintf(buf, PRT_COLUMN_WIDTH, "0x%x", val[0]);
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
 * \param [out] overflow - indicates if sum of val_1+val_2 has overflow
 * \return
 *      Standard error handling
 */
shr_error_e
sum_value_to_str(
    uint32 *val_1,
    uint32 *val_2,
    int nof_bit,
    int in_hex,
    char *buf,
    int *overflow)
{
    uint64 val_64_1, val_64_2, val_64_sum;
    int nof_bit_i, nof_words, nof_bit_in_mask, mask;

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
    COMPILER_64_ZERO(val_64_sum);
    COMPILER_64_ADD_64(val_64_sum, val_64_1);
    COMPILER_64_ADD_64(val_64_sum, val_64_2);
    if (COMPILER_64_LT(val_64_sum, val_64_1) || COMPILER_64_LT(val_64_sum, val_64_2))
    {
        *overflow = TRUE;
    }
    else
    {
        *overflow = FALSE;
    }

    if (in_hex)
    {
        if (COMPILER_64_HI(val_64_sum) > 0)
        {
            sal_snprintf(buf, PRT_COLUMN_WIDTH, "0x%x", COMPILER_64_HI(val_64_sum));
            sal_snprintf(buf + sal_strlen(buf), PRT_COLUMN_WIDTH, "%08x", COMPILER_64_LO(val_64_sum));
        }
        else
        {
            sal_snprintf(buf, PRT_COLUMN_WIDTH, "0x%x", COMPILER_64_LO(val_64_sum));
        }
    }
    else
    {
        format_uint64_decimal(buf, val_64_sum, ',');
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
shr_error_e
counter_block_name_match(
    int unit,
    char *match_n,
    int *nof_block,
    int *block_ids)
{
    shr_error_e rv = _SHR_E_NOT_FOUND;
    int i_bl, cmp_len;
    soc_info_t *si = NULL;

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

    si = &SOC_INFO(unit);

    *nof_block = 0;
    cmp_len = strlen(match_n);
    for (i_bl = 0; SOC_BLOCK_INFO(unit, i_bl).type >= 0; i_bl++)
    {
        /*
         * Skip blocks that are not enabled
         */
        if (si->block_valid[i_bl] == 0)
        {
            continue;
        }

        /*
         * For the block name with index (0, 1, ....., 9), only the index specific block is matched
         * For the block name without index, all the blocks with any index are matched.
         */
        if (ASCII_CODE_OF_9 >= match_n[cmp_len - 1] && match_n[cmp_len - 1] >= ASCII_CODE_OF_0)
        {
            if (cmp_len != strlen(SOC_BLOCK_NAME(unit, i_bl)))
            {
                continue;
            }
        }
        if (!sal_strncasecmp(SOC_BLOCK_NAME(unit, i_bl), match_n, cmp_len))
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
 *   To start gtimer of the blocks specified by block index array
 * \param [in] unit - Unit #
 * \param [in] nof_block - number of blocks contained in block_ids array
 * \param [in] block_ids - an array to contain the blockes
 * \param [in] interval - indicate how long gtimer keeps enabled (second)
 * \return
 *      Standard error handling
 */
shr_error_e
sh_dnxc_diag_block_gtimer_start(
    int unit,
    int nof_block,
    int *block_ids,
    int interval)
{
    int rv = _SHR_E_NONE;
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_IS_DNX(unit))
    {
#ifdef BCM_DNX_SUPPORT
        rv = sh_dnx_diag_block_gtimer_start(unit, nof_block, block_ids, interval);
#endif
    }
    SHR_IF_ERR_EXIT_WITH_LOG(rv, "Failed(%d) sh_dnxc_diag_block_gtimer_start%s%s\n", rv, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   To stop gtimer of the blocks specified by block index array
 * \param [in] unit - Unit #
 * \param [in] nof_block - number of blocks contained in block_ids array
 * \param [in] block_ids - an array to contain the blockes
 * \return
 *      Standard error handling
 */
shr_error_e
sh_dnxc_diag_block_gtimer_stop(
    int unit,
    int nof_block,
    int *block_ids)
{
    int rv = _SHR_E_NONE;
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_IS_DNX(unit))
    {
#ifdef BCM_DNX_SUPPORT
        rv = sh_dnx_diag_block_gtimer_stop(unit, nof_block, block_ids);
#endif
    }
    SHR_IF_ERR_EXIT_WITH_LOG(rv, "Failed(%d) in sh_dnxc_diag_block_gtimer_stop%s%s\n", rv, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   to display the counters in the blocks specified by option "name"
 *   or the counters in all the blocks when setting option "all"
 * \param [in] unit - Unit #
 * \param [in] match_n - pointer to string to filter block name
 * \param [in] non_zero - indicate to display non_zero counters only
 * \param [in] in_hex - indicate to display counters in hex format
 * \param [in] interval - indicate the interval of g-timer (second)
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
    int i_fld, i_bl, i_ind, first_array_ind;
    int numels, object_col_id;
    int idx = 0, nof_block = 0;
    int block_ids[BLOCK_MAX_INDEX];
    soc_reg_t reg;
    soc_field_info_t *fld;
    soc_reg_above_64_val_t fld_value;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(NO_UNIT);

    PRT_TITLE_SET("BLOCK COUNTERS");

    /*
     * add column for displaying counters per block
     */
    PRT_COLUMN_ADDX(PRT_XML_CHILD, PRT_TITLE_ID, &object_col_id, "No.");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Block");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Counter");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Numels");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Value");

    sal_memset(block_ids, 0, BLOCK_MAX_INDEX * sizeof(int));
    if (counter_block_name_match(unit, match_n, &nof_block, block_ids) != _SHR_E_NONE)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "no matched block found\n");
    }

    if (interval)
    {
        SHR_IF_ERR_EXIT(sh_dnxc_diag_block_gtimer_start(unit, nof_block, block_ids, interval));
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

        if (!SOC_REG_IS_ARRAY(unit, reg) || SOC_REG_NUMELS(unit, reg) == 0)
        {
            /*
             * Simulate index 0 for non arrayed registers, should not happen
             */
            numels = 1;
            first_array_ind = 0;
        }
        else
        {
            numels = reginfo->numels;
            first_array_ind = reginfo->first_array_index;
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
            for (i_ind = first_array_ind; i_ind < first_array_ind + numels; i_ind++)
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

    if (interval)
    {
        SHR_IF_ERR_EXIT(sh_dnxc_diag_block_gtimer_stop(unit, nof_block, block_ids));
    }

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
    int block_is_present = FALSE;
    int non_zero, in_hex, graphical;

    SHR_FUNC_INIT_VARS(NO_UNIT);

    /*
     * Get parameters
     */
    SH_SAND_IS_PRESENT("block", block_is_present);
    SH_SAND_GET_STR("block", match_n);
    SH_SAND_GET_BOOL("GRaphical", graphical);

    if (block_is_present && graphical)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Option GRAPHICAL cannot be provided together with BLOCK option!\n");
    }
    SH_SAND_GET_BOOL("NonZero", non_zero);
    SH_SAND_GET_BOOL("hex", in_hex);
    SH_SAND_GET_UINT32("InTerVal", interval);

    if (graphical)
    {
#ifdef BCM_DNXF_SUPPORT
        if (SOC_IS_DNXF(unit))
        {
            if (non_zero)
            {
                SHR_CLI_EXIT(_SHR_E_PARAM, "Option NON_ZERO cannot be provided together with GRAPHICAL option!\n");
            }
            SHR_IF_ERR_EXIT(diag_dnxf_counters_graphical_print(unit, sand_control));
        }
#endif
#ifdef BCM_DNX_SUPPORT
        if (SOC_IS_DNX(unit))
        {
            int cdsp;
            SH_SAND_GET_BOOL("CoreDiSPlay", cdsp);
            SHR_IF_ERR_EXIT(sh_dnx_diag_counter_graphical_print(unit, cdsp, non_zero, in_hex, interval, sand_control));
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
sh_dnxc_avs_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 avs_val;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_switch_rov_get(unit, 0, &avs_val));

    LOG_CLI(("AVS Status:0x%x\n", avs_val));

exit:
    SHR_FUNC_EXIT;
}

/*
 * LOCAL DIAG PACK:
 * }
 */

/**
 * List of the supported commands, pointer to command function and command usage function.
 */
static sh_sand_option_t sh_dnxf_diag_counters_options[] = {
    DIAG_DNXC_DIAG_COUNTER_OPTIONS,
    {NULL}
};

/* *INDENT-OFF* */
static sh_sand_option_t sh_dnx_diag_counters_options[] = {
    DIAG_DNXC_DIAG_COUNTER_OPTIONS,
    {"CoreDiSPlay", SAL_FIELD_TYPE_BOOL, "display counter on core base, FAP only", "false", NULL},
    {NULL}
};

static sh_sand_man_t sh_dnx_diag_counters_man = {
    .brief = "Display the counters of the internal blocks",
    .full = DIAG_DNXC_DIAG_COUNTER_FULL_MAN,
    .synopsis = "[<block_name>] [graphical] [nz] [hex] [interval=<integer>]",
    .examples = ""
            "CGM\n"
            "CGM nz\n"
            "CGM hex\n"
            "CGM interval=1\n"
            "graphical\n"
            "graphical hex\n"
            "graphical interval=1\n"
            "graphical CoreDiSPlay"
};

static sh_sand_man_t sh_dnxf_diag_counters_man = {
    .brief = "Display the counters of the internal blocks",
    .full = DIAG_DNXC_DIAG_COUNTER_FULL_MAN,
    .synopsis = "[<block_name>] [graphical] [nz] [hex] [interval=<integer>]",
    .examples = ""
            "RTP \n"
            "DCH nz\n"
            "graphical\n"
            "graphical hex\n"
            "graphical interval=10"
};

sh_sand_man_t sh_dnxc_avs_man = {
    .brief = "Display the AVS value of the device",
    .full = "Display AVS field read result into one graphic"
};

sh_sand_cmd_t sh_dnxc_avs_cmds[] = {
    /*
     * keyword, action, command, options, man
     */
    {"read", sh_dnxc_avs_cmd, NULL, NULL, &sh_dnxc_avs_man},
    {NULL}
};

/**
 * Diag command should be used for backward compatibility requests only - new functionality should be assigned
 * appropriate specific commands
 */
sh_sand_cmd_t sh_dnxc_diag_cmds[] = {
    /*keyword,  action,                   command, options,                       man                                     Flags               Conditional CB */
    {"counter", sh_dnxc_diag_counter_cmd, NULL,    sh_dnx_diag_counters_options,  &sh_dnx_diag_counters_man,  NULL, NULL, SH_CMD_CONDITIONAL, sh_cmd_is_dnx},
    {"counter", sh_dnxc_diag_counter_cmd, NULL,    sh_dnxf_diag_counters_options, &sh_dnxf_diag_counters_man, NULL, NULL, SH_CMD_CONDITIONAL, sh_cmd_is_dnxf},
    {NULL}
};

sh_sand_man_t sh_dnxc_diag_man = {
    .brief = "Miscellaneous facilities for displaying diagnostics information"
};
/* *INDENT-ON* */
