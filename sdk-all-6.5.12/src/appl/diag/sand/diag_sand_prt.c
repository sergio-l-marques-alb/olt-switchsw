/*
 * $Id: diag_sand_prt.c,v 1.00 Broadcom SDK $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:    diag_sand_prt.c
 * Purpose:    Beautified cli output routine
 */

#include <sal/core/libc.h>
#include <sal/core/alloc.h>
#include <sal/appl/sal.h>
#include <sal/appl/io.h>
#include <sal/types.h>
#include <shared/bsl.h>
#include <shared/util.h>

#include <shared/dbx/dbx_xml.h>
#include <shared/dbx/dbx_file.h>
#include <shared/utilex/utilex_rhlist.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <soc/sand/sand_aux_access.h>

#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

int
diag_sand_get_shift(
    char *str)
{
    int i_shift;
    for (i_shift = 0; str[i_shift] == ' '; i_shift++)
    {
    }
    /*
     * Each shift has 2 spaces 
     */
    return i_shift / 2;
}

void
diag_sand_prt_char(
    int num,
    char ch)
{
    while (num--)
        cli_out("%c", ch);
    return;
}

static void
diag_sand_prt_row(
    int num,
    char ch)
{
    while (num--)
        cli_out("%c", ch);
    cli_out("\n");
    return;
}

int
diag_sand_prt_width(
    void)
{
#if !defined(VXWORKS) && !defined(__KERNEL__)
    struct winsize w;

    ioctl(0, TIOCGWINSZ, &w);
    return w.ws_col;
#else
    return 0;
#endif
}

static int
prt_token_max_size(
    char *prt_str)
{
    int prt_size = sal_strlen(prt_str);
    int i_ch, max_size = 0, cur_size = 0;

    for (i_ch = 0; i_ch < prt_size; i_ch++)
    {
        /*
         * Take next row on new line
         */
        cur_size++;
        if (prt_str[i_ch] == '\n')
        {
            if (cur_size > max_size)
                max_size = cur_size;
            cur_size = 0;
        }
    }

    if (max_size == 0)
        max_size = prt_size;

    return max_size;
}

static int
prt_token_has_eol(
    char *prt_str)
{
    int prt_size = sal_strlen(prt_str);
    int i_ch;

    for (i_ch = 0; i_ch < prt_size; i_ch++)
    {
        if (prt_str[i_ch] == '\n')
            return TRUE;
    }

    return FALSE;
}

static int
prt_get_token_size(
    char *prt_str,
    int column_width)
{
    int token_size = 0, prt_size = sal_strlen(prt_str);
    int i_ch;

    for (i_ch = 0; i_ch < prt_size; i_ch++)
    {
        /*
         * Take next row on new line
         */
        if (prt_str[i_ch] == '\n')
        {
            /*
             * If it is still inside MAX column size, take this offset as last, otherwise use previous and leave \n for 
             * next round 
             */
            if (i_ch < column_width)
            {
                token_size = i_ch + 1;
                prt_str[i_ch] = ' ';
            }
            break;
        }

        if ((prt_str[i_ch] == ' ') || (prt_str[i_ch] == ',') || (prt_str[i_ch] == '.') || (prt_str[i_ch] == '\t') ||
            (prt_str[i_ch] == '_'))
        {
            if (i_ch < column_width)
            {   /* Keep last character that we may break the line on it */
                token_size = i_ch + 1;
            }
            else
            {
                break;
            }
        }
    }

    if (token_size == 0)
    {
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META("Single token is longer than:%d\n"), column_width));
        token_size = column_width - 1;
    }
    return token_size;
}

static shr_error_e
diag_sand_prt_xml(
    prt_control_t * prt_ctr,
    sh_sand_control_t * sand_control,
    char *output_file)
{
#if !defined(NO_FILEIO)
    xml_node top_node, cmd_node, table_node;
    int k, k_cl;
    rhhandle_t prt_row_start;
    char *prt_row_offset;
    void *parent_node;
    int parent_id = PRT_TITLE_ID;
    int shift_n;
    rhentry_t *info_entry;
    int depth_base = 0;
    char device_name[RHNAME_MAX_SIZE];
    char *sand_name;

    SHR_FUNC_INIT_VARS(prt_ctr->unit);

    if (prt_ctr->list == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_EMPTY, "No list in control structure");
    }

    if (prt_ctr->col_num == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "No columns defined for table:%s\n", prt_ctr->col[PRT_TITLE_ID].name);
    }

    /*
     * Check if advanced table creation was used
     */
    if (prt_ctr->col[0].parent_id == 0)
    {   /* First column having parent_id 0 is signal than no advanced XMl formatting was used */
        PRT_XML_TYPE xml_type = PRT_XML_CHILD;
        /*
         * Always assign title id to first column parent and make it child 
         */
        for (k = 0; k < prt_ctr->col_num; k++)
        {
            if (ISEMPTY(prt_ctr->col[k].name))
            {
                sprintf(prt_ctr->col[k].name, "column_%d", k);
            }
            else
            {
                utilex_str_escape(prt_ctr->col[k].name, '_');
            }
            prt_ctr->col[k].parent_id = parent_id;
            prt_ctr->col[k].type = xml_type;
            if (xml_type == PRT_XML_ATTRIBUTE)
            {   /* From this column and forward all columns will be attribute of the first column without empty cells */
                continue;
            }
            RHITERATOR(prt_row_start, prt_ctr->list)
            {
                prt_row_offset = ((char *) prt_row_start) + sizeof(rhentry_t) + prt_ctr->col[k].cell_offset;
                if (ISEMPTY(prt_row_offset))
                {
                    break;
                }
            }
            /*
             *  If all rows have non empty strings on specific column then next column may be its attribute
             *  If there are holes, next column will be child
             */
            parent_id = k;
            if (prt_row_start == NULL)
            {   /* End of list reached, no empty cells discovered, make all next columns attributes */
                xml_type = PRT_XML_ATTRIBUTE;
            }
            else
            {   /* empty cells were found, so next column will be child and not attribute */
                xml_type = PRT_XML_CHILD;
            }
        }
    }

    sand_name = shr_access_get_sand_name(prt_ctr->unit);

    if ((top_node = dbx_xml_top_get(output_file, sand_name, CONF_OPEN_CREATE)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "failed to create:%s\n", output_file);
    }

    /*
     * Check if device was already recorded on XML top node, if not record all params 
     */
    if ((dbx_xml_property_get_str(top_node, "device", device_name, RHNAME_MAX_SIZE)) != _SHR_E_NONE)
    {
        RHDATA_SET_INT(top_node, "unit", prt_ctr->unit);
        RHDATA_SET_STR(top_node, "device", (char *) soc_dev_name(prt_ctr->unit));
        RHDATA_SET_STR(top_node, "family", SOC_CHIP_STRING(prt_ctr->unit));
    }
    else
    {
        if (sal_strcasecmp(device_name, (char *) soc_dev_name(prt_ctr->unit)))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "File was created for:%s this one is:%s\n", device_name,
                         (char *) soc_dev_name(prt_ctr->unit));
        }
    }

    /*
     *  Header comes from special ID
     */
    if (sand_control != NULL)
    {
        rhhandle_t rhhandle;
        void *cur_node;
        cmd_node = top_node;
        RHITERATOR(rhhandle, sand_control->command_list)
        {
            /*
             * If first command is equal to sand_name, skip it on node creation, was created above 
             */
            if ((rhhandle == utilex_rhlist_entry_get_first(sand_control->command_list))
                && !sal_strcasecmp(RHNAME(rhhandle), sand_name))
                continue;
            /*
             * Always look for existing node of it is not last Create new node for last command in the list to
             * differentiate multiple running of the same command 
             */
            depth_base++;
            /*
             * If this is last keyword in comamnd look for matching timestamp
             */
            if (rhhandle == utilex_rhlist_entry_get_last(sand_control->command_list))
            {
                char time_str[SH_SAND_MAX_TIME_SIZE];
                char options_str[SH_SAND_MAX_TOKEN_SIZE];
                RHDATA_ITERATOR(cur_node, cmd_node, RHNAME(rhhandle))
                {
                    RHDATA_GET_STR_CONT_EXT(cur_node, "timestamp", time_str, SH_SAND_MAX_TIME_SIZE);
                    RHDATA_GET_STR_CONT_EXT(cur_node, "options", options_str, SH_SAND_MAX_TOKEN_SIZE);
                    if (!sal_strcasecmp(sand_control->time_str, time_str)
                        && !sal_strcasecmp(sand_control->options_str, options_str))
                        break;
                }
            }
            else
            {   /* For not last command just check such node exists */
                cur_node = dbx_xml_child_get_first(cmd_node, RHNAME(rhhandle));
            }
            /*
             * Create node if none was found
             */
            if (cur_node == NULL)
            {
                if ((cur_node = dbx_xml_child_add(cmd_node, RHNAME(rhhandle), depth_base)) == NULL)
                {
                    SHR_ERR_EXIT(_SHR_E_FAIL, "failed to add:%s\n", RHNAME(rhhandle));
                }
                if (rhhandle == utilex_rhlist_entry_get_last(sand_control->command_list))
                {
                    RHDATA_SET_STR(cur_node, "options", sand_control->options_str);
                    RHDATA_SET_STR(cur_node, "timestamp", sand_control->time_str);
                }
            }
            cmd_node = cur_node;
        }
    }
    else
    {
        depth_base++;
        if ((cmd_node = dbx_xml_child_add(top_node, "command", depth_base)) == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "failed to add:%s\n", prt_ctr->col[PRT_TITLE_ID].name);
        }
    }
    /*
     * Increase indent for table node to come
     */
    depth_base++;
    /*
     * Create node of table inside leaf, there may be multiple tables inside the leaf
     */
    if ((table_node = dbx_xml_child_add(cmd_node, "table", depth_base)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "failed to add table node\n");
    }
    /*
     * Increase indent for content group nodes(infos, headers, entries) node to come
     */
    depth_base++;

    RHDATA_SET_STR(table_node, "title", prt_ctr->col[PRT_TITLE_ID].name);

    /*
     * Add Info List to table node
     */
    {
        void *infos_node;
        if ((infos_node = dbx_xml_child_add(table_node, "infos", depth_base)) == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "failed to add infos node to:%s\n", prt_ctr->col[PRT_TITLE_ID].name);
        }
        RHITERATOR(info_entry, prt_ctr->info_list)
        {
            void *info_node;
            if ((info_node = dbx_xml_child_add(infos_node, "info", depth_base + 1)) == NULL)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "failed to add info node to:%s\n", prt_ctr->col[PRT_TITLE_ID].name);
            }
            RHDATA_SET_STR(info_node, "text", RHNAME(info_entry));
        }
        dbx_xml_node_end(infos_node, depth_base);
    }
    /*
     * Fill Header Information as Headers Node as list
     */
    {
        void *headers_node, *node;
        if ((headers_node = dbx_xml_child_add(table_node, "headers", depth_base)) == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "failed to add headers node to:%s\n", prt_ctr->col[PRT_TITLE_ID].name);
        }
        for (k = 0; k < prt_ctr->col_num; k++)
        {
            if ((node = dbx_xml_child_add(headers_node, "header", depth_base + 1)) == NULL)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "failed to add header node\n");
            }
            utilex_str_replace(prt_ctr->col[0].name, ' ', '_');
            RHDATA_SET_INT(node, "id", k);
            RHDATA_SET_STR(node, "name", prt_ctr->col[k].name);
            RHDATA_SET_INT(node, "type", prt_ctr->col[k].type);
        }
        dbx_xml_node_end(headers_node, depth_base);
    }
    /*
     * Add Entries Group node
     */
    if ((prt_ctr->col[PRT_TITLE_ID].node[0] = dbx_xml_child_add(table_node, "entries", depth_base)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "failed to add headers node to:%s\n", prt_ctr->col[PRT_TITLE_ID].name);
    }
    prt_ctr->col[PRT_TITLE_ID].depth = depth_base;
    /*
     * Now we'll iterate on all the row and all the cells inside
     */
    RHITERATOR(prt_row_start, prt_ctr->list)
    {
        for (k = 0; k < prt_ctr->col_num; k++)
        {
            prt_row_offset = ((char *) prt_row_start) + sizeof(rhentry_t) + prt_ctr->col[k].cell_offset;
            /*
             * No use for empty cells
             */
            if (ISEMPTY(prt_row_offset))
                continue;
            /*
             * Check if it is shifted and by how many
             */
            shift_n = diag_sand_get_shift(prt_row_offset);
            /*
             * Eliminate white spaces
             */
            prt_row_offset += shift_n * 2;
            /*
             * Figure out who is the parent
             */
            if ((shift_n != 0) && (prt_ctr->col[k].type == PRT_XML_CHILD))
            {   /* 
                 * We have shift and this column is child, we make shifted cell child to its non-shifted or less shifted
                 * brother instead of parent
                 * If shifted cell is attribute it will be assigned to its parent column anyway
                 */
                parent_node = prt_ctr->col[k].node[shift_n - 1];
            }
            else
            {
                int parent_id = prt_ctr->col[k].parent_id;
                /*
                 * If parent node is NULL look for its parent and so on until title
                 */
                do
                {
                    shift_n = prt_ctr->col[parent_id].cur_shift_id;
                    parent_node = prt_ctr->col[parent_id].node[shift_n];
                    parent_id = prt_ctr->col[parent_id].parent_id;
                }
                while ((parent_node == NULL) && (parent_id != PRT_TITLE_ID));
            }

            if (parent_node == NULL)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "Inconsistency in inheritance Parent:%d for:%s is not node\n",
                             prt_ctr->col[k].parent_id, prt_ctr->col[k].name);
            }

            switch (prt_ctr->col[k].type)
            {
                case PRT_XML_CHILD:
                {       /* 
                         * If the place is occupied it should be ended together with all underlying shifted nodes
                         * and further columns
                         */
                    int i_sh;
                    for (k_cl = k; k_cl < prt_ctr->col_num; k_cl++)
                    {
                        if (prt_ctr->col[k_cl].type == PRT_XML_CHILD)
                        {
                            for (i_sh = shift_n; i_sh < PRT_MAX_SHIFT_NUM; i_sh++)
                            {
                                if (prt_ctr->col[k_cl].node[i_sh] != NULL)
                                {
                                    dbx_xml_node_end(prt_ctr->col[k_cl].node[i_sh], prt_ctr->col[k_cl].depth + i_sh);
                                    prt_ctr->col[k_cl].node[i_sh] = NULL;
                                    prt_ctr->col[k_cl].cur_shift_id = (i_sh != 0) ? (i_sh - 1) : 0;
                                }
                            }
                        }
                    }
                }
                    prt_ctr->col[k].depth = prt_ctr->col[prt_ctr->col[k].parent_id].depth + 1;
                    /*
                     * Make sure shift id is pointing to right place
                     */
                    prt_ctr->col[k].cur_shift_id = shift_n;
                    prt_ctr->col[k].node[shift_n] = dbx_xml_child_add(parent_node, prt_ctr->col[k].name,
                                                                      prt_ctr->col[k].depth + shift_n);
                    RHDATA_SET_STR(prt_ctr->col[k].node[shift_n], "node_content", prt_row_offset);
                    break;
                case PRT_XML_ATTRIBUTE:
                    RHDATA_SET_STR(parent_node, prt_ctr->col[k].name, prt_row_offset);
                    break;
                case PRT_XML_NONE:
                default:
                    break;
            }
        }
    }

    for (k = 0; k < prt_ctr->col_num; k++)
    {
        for (shift_n = 0; shift_n < PRT_MAX_SHIFT_NUM; shift_n++)
        {
            if (prt_ctr->col[k].node[shift_n] != NULL)
            {
                dbx_xml_node_end(prt_ctr->col[k].node[shift_n], prt_ctr->col[k].depth + shift_n);
            }
        }
    }

    /*
     * Close "entries" node, depth_base wa son level of entries
     */
    dbx_xml_node_end(prt_ctr->col[PRT_TITLE_ID].node[0], prt_ctr->col[PRT_TITLE_ID].depth);
    depth_base--;
    dbx_xml_node_end(table_node, depth_base);
    depth_base--;
    dbx_xml_node_end(cmd_node, depth_base);

    dbx_xml_top_save(top_node, output_file);
    dbx_xml_top_close(top_node);

exit:
    SHR_FUNC_EXIT;
#else
    return _SHR_E_NONE;
#endif /* !defined NO_FILEIO */
}

static shr_error_e
diag_sand_prt_column_show(
    prt_control_t * prt_ctr,
    char *columns_str)
{
    uint32 realtokens;
    char **column_nums = NULL;
    int i_col, show_col_id;
    char *end_ptr;

    SHR_FUNC_INIT_VARS(prt_ctr->unit);
    /*
     * If string is all keep all columns visible, else initiate columns to invisible,
     * so that only the ones which are presented in the list will be shown
     */
    if (!sal_strcasecmp(columns_str, "all"))
    {
        SHR_EXIT();
    }
    else
    {
        PRT_COLUMN_ITERATOR(i_col, prt_ctr)
        {
            prt_ctr->col[i_col].dont_show = TRUE;
        }
    }

    if ((column_nums = utilex_str_split(columns_str, ",", prt_ctr->col_num + 1, &realtokens)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Faled to parse column numbers list:%s\n", columns_str);
    }

    if (realtokens > prt_ctr->col_num)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Too much numbers in column list:%s\n", columns_str);
    }
    for (i_col = 0; i_col < realtokens; i_col++)
    {
        /*
         * Test on range
         */
        uint32 range_realtokens;
        char **range_column_nums = NULL;
        if ((range_column_nums =
             utilex_str_split(column_nums[i_col], "-", prt_ctr->col_num + 1, &range_realtokens)) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_INTERNAL, "Failed to parse column list token:%s\n", column_nums[i_col]);
        }
        if (range_realtokens == 2)
        {
            int show_col_id_start, show_col_id_end, i_ran;
            show_col_id_start = sal_strtol(range_column_nums[0], &end_ptr, 0);
            if ((end_ptr[0] != 0) || (show_col_id_start > prt_ctr->col_num) || (show_col_id_start <= 0))
            {
                utilex_str_split_free(range_column_nums, range_realtokens);
                SHR_CLI_EXIT(_SHR_E_INTERNAL, "Element:\"%s\" in the column list is not a valid column number\n",
                             range_column_nums[0]);
            }
            show_col_id_end = sal_strtol(range_column_nums[1], &end_ptr, 0);
            if ((end_ptr[0] != 0) || (show_col_id_end > prt_ctr->col_num) || (show_col_id_end <= 0))
            {
                utilex_str_split_free(range_column_nums, range_realtokens);
                SHR_CLI_EXIT(_SHR_E_INTERNAL, "Element:\"%s\" in the column list is not a valid column number\n",
                             range_column_nums[1]);
            }
            if (show_col_id_start > show_col_id_end)
            {
                utilex_str_split_free(range_column_nums, range_realtokens);
                SHR_CLI_EXIT(_SHR_E_INTERNAL, "Start column id:%d is greater than end column id:%d\n",
                             show_col_id_start, show_col_id_end);
            }
            for (i_ran = show_col_id_start; i_ran <= show_col_id_end; i_ran++)
            {
                prt_ctr->col[i_ran - 1].dont_show = FALSE;
            }
            /*
             * We have range indeed 
             */
        }
        else if (range_realtokens != 1)
        {
            utilex_str_split_free(range_column_nums, range_realtokens);
            SHR_CLI_EXIT(_SHR_E_INTERNAL, "Error - there may not more than 1 '-' inside 1 range:%s\n",
                         column_nums[i_col]);
        }
        else
        {
            /*
             * No range sign '-'inside
             * Go through all numbers in list and assign appropriate status of dont show
             */
            show_col_id = sal_strtol(column_nums[i_col], &end_ptr, 0);
            if ((end_ptr[0] != 0) || (show_col_id > prt_ctr->col_num) || (show_col_id <= 0))
            {
                utilex_str_split_free(range_column_nums, range_realtokens);
                SHR_CLI_EXIT(_SHR_E_INTERNAL, "Element:\"%s\" in the column list is not a valid column number\n",
                             column_nums[i_col]);
            }
            prt_ctr->col[show_col_id - 1].dont_show = FALSE;
        }
        utilex_str_split_free(range_column_nums, range_realtokens);
    }

exit:
    if (column_nums != NULL)
    {
        utilex_str_split_free(column_nums, realtokens);
    }
    SHR_FUNC_EXIT;
}

shr_error_e
diag_sand_prt(
    prt_control_t * prt_ctr,
    sh_sand_control_t * sand_control,
    char *prt_filename)
{
    int i_rows, k, n;

    prt_format_t *column = NULL;
    int column_width;
    int total_width = 1;
    rhhandle_t prt_row_start;
    char *prt_row_offset;
    rhentry_t *entry;
    int row_num;
    char *tmp_str = NULL;
    int silence = FALSE;
    SHR_FUNC_INIT_VARS(prt_ctr->unit);

    if (prt_ctr->col_num == 0)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "No columns defined for table:%s\n", prt_ctr->col[PRT_TITLE_ID].name);
    }

    if (sand_control != NULL)
    {
        char *columns_str;
        SH_SAND_GET_BOOL("silence", silence);
        SH_SAND_GET_STR("columns", columns_str);
        SHR_IF_ERR_EXIT(diag_sand_prt_column_show(prt_ctr, columns_str));
        if (prt_filename == NULL)
        {
            SH_SAND_GET_STR("file", prt_filename);
        }
    }

    if (!ISEMPTY(prt_filename))
    {
        if (dbx_file_get_type(prt_filename) != DBX_FILE_XML)
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Output file must have .xml suffix. file was:\"%s\"\n", prt_filename);
        }

        diag_sand_prt_xml(prt_ctr, sand_control, prt_filename);
        SHR_EXIT();
    }

    if (prt_ctr->list == NULL)
    {
        if (silence == FALSE)
        {
            SHR_CLI_EXIT(_SHR_E_INTERNAL, "No content for table:%s\n", prt_ctr->col[PRT_TITLE_ID].name);
        }
        else
        {
            /** coverity[copy_paste_error : FALSE] */
            SHR_CLI_EXIT(_SHR_E_INTERNAL, "");
        }
    }

    row_num = RHLNUM(prt_ctr->list);
    column = sal_alloc(sizeof(prt_format_t) * prt_ctr->col_num, "column format");
    sal_memset(column, 0, sizeof(prt_format_t) * prt_ctr->col_num);

    /**
     * Step 1 - Figure out longest cells in each column
     */
    RHITERATOR(prt_row_start, prt_ctr->list)
    {
        PRT_COLUMN_ITERATOR(k, prt_ctr)
        {
            prt_row_offset = ((char *) prt_row_start) + sizeof(rhentry_t) + prt_ctr->col[k].cell_offset;

            /*
             * remove trailing white spaces
             */
            for (n = sal_strlen(prt_row_offset) - 1; n > 1; n--)
                if (prt_row_offset[n] == ' ')
                    prt_row_offset[n] = 0;
                else
                    break;
            column_width = prt_token_max_size(prt_row_offset);

            if (column_width > column[k].width)
                column[k].width = column_width;
        }
    }

    /**
     * Step 2 - Create format per column and count total line width
     */
    PRT_COLUMN_ITERATOR(k, prt_ctr)
    {
        int col_name_size = sal_strlen(prt_ctr->col[k].name);
        /*
         * Sync with Header Cell(actually column name) width
         */
        /*
         * For columns defined as flex we'll make sure that column name will be also splitted to match minimal width
         */
        if ((prt_ctr->col[k].flex_type == 0) || (col_name_size <= column[k].width) ||
            ((col_name_size > column[k].width) && (col_name_size <= PRT_COLUMN_WIDTH_SPL)))
        {
            column[k].width = (col_name_size > column[k].width) ? col_name_size : column[k].width;
        }
        else
        {
            column[k].width = (PRT_COLUMN_WIDTH_SPL > column[k].width) ? PRT_COLUMN_WIDTH_SPL : column[k].width;
        }
        /**
         * Cannot be more that PRT_COLUMN_WIDTH
         */
        if (column[k].width >= PRT_COLUMN_WIDTH)
            column[k].width = PRT_COLUMN_WIDTH - 1;
        /*
         * aggregate total row size
         */
        if (column[k].width == 0)
        {
            /*
             * Just add | without spaces - it is intended for special separation
             */
            total_width += column[k].width + 1; /* for | between fields and spaces before and after */
            /*
             * create format string for each column;
             */
            sprintf(column[k].format, "|%ss", "%");
        }
        else
        {
            total_width += column[k].width + 3; /* for | between fields and spaces before and after */
            /*
             * create format string for each column;
             */
            if (prt_ctr->col[k].align == TRUE)
                sprintf(column[k].format, "|%s%ds ", "%", column[k].width + 1);
            else
                sprintf(column[k].format, "| %s-%ds", "%", column[k].width + 1);
        }
    }

    /*
     * Step 3- Print title surrounded by '=' lines and inside info
     */
    if (!ISEMPTY(prt_ctr->col[PRT_TITLE_ID].name))
    {
        int title_width, header_width, info_width;
        char title_format[20];
        rhentry_t *info_entry;
        /*
         * In case title is longer than table width - make header separation line same size as title 
         */
        title_width = sal_strlen(prt_ctr->col[PRT_TITLE_ID].name);
        /*
         * Make line according to longest info line 
         */
        RHITERATOR(info_entry, prt_ctr->info_list)
        {
            if ((info_width = strlen(RHNAME(info_entry))) > title_width)
                title_width = info_width;
        }
        /*
         * "- 3" is two '|' and one space ' ' 
         */
        header_width = title_width < (total_width - 3) ? total_width : title_width + 3;
        sprintf(title_format, "| %s-%ds|\n", "%", header_width - 3);

        diag_sand_prt_row(header_width, '=');
        cli_out(title_format, prt_ctr->col[PRT_TITLE_ID].name);
        RHITERATOR(info_entry, prt_ctr->info_list)
        {
            diag_sand_prt_row(header_width, '-');
            cli_out(title_format, RHNAME(info_entry));
        }
        diag_sand_prt_row(header_width, '=');
        /*
         * In case title_width is more than total_width, update total_width and last column format 
         */
        if (title_width > (total_width - 3))
        {
            int last_visible_col_id = k;
            PRT_COLUMN_ITERATOR(k, prt_ctr)
            {
                last_visible_col_id = k;
            }
            sprintf(column[last_visible_col_id - 1].format, "| %s-%ds", "%",
                    column[last_visible_col_id - 1].width + 1 + (title_width + 3 - total_width));
            total_width = title_width + 3;
        }
    }
    else
        diag_sand_prt_row(total_width, '=');
    /*
     * Step 4 - Print Header Line from prt_ctr->col
     */
    tmp_str = sal_alloc(PRT_COLUMN_WIDTH_BIG, "temp");
    {
        int cells_to_print;
        char *to_ptr;
        int token_size;
        int i_line = 0;
/*        cli_out(column[k].format, prt_ctr->col[k].name); */
        do
        {
            /**
             * For each line zero cell_to_print, as if there will be no more lines in the row
             */
            cells_to_print = 0;
            if (i_line++ == 0)
            {
                /*
                 * Initialize next pointer for the first line in the row
                 */
                PRT_COLUMN_ITERATOR(k, prt_ctr)
                {
                    column[k].next_ptr = prt_ctr->col[k].name;
                }
            }
            else
            {   /* 
                 * If the line is not first, issue EOL
                 */
                cli_out("|\n");
            }

            PRT_COLUMN_ITERATOR(k, prt_ctr)
            {
                token_size = sal_strlen(column[k].next_ptr);
                if ((token_size > column[k].width) || prt_token_has_eol(column[k].next_ptr))
                {   /**
                     * Split the string to print portions fitting into the width
                     * Adjust token_size
                     */

                    switch (prt_ctr->col[k].flex_type)
                    {
                            /*
                             * Never mind how regular cells were splitted, header will be splitted always ASCII
                             */
                        case PRT_FLEX_BINARY:
                        case PRT_FLEX_ASCII:
                            token_size = prt_get_token_size(column[k].next_ptr, column[k].width);
                            break;
                        case PRT_FLEX_NONE:
                            SHR_CLI_EXIT(_SHR_E_INTERNAL, "Small column width:%d for the string:%d\n", column[k].width,
                                         token_size);
                            break;
                    }
                    memcpy(tmp_str, column[k].next_ptr, token_size);
                    cells_to_print++;
                    /*
                     * Make token NULL ended
                     */
                    tmp_str[token_size] = 0;
                    to_ptr = tmp_str;
                }
                else
                {
                    to_ptr = column[k].next_ptr;
                }
                cli_out(column[k].format, to_ptr);
                column[k].next_ptr += token_size;
            }
        }
        while (cells_to_print != 0);
    }
    cli_out("|\n");
    diag_sand_prt_row(total_width, '=');

    /*
     * Step 5 - Print cells
     */
    i_rows = 0;
    RHITERATOR(prt_row_start, prt_ctr->list)
    {
        int cells_to_print;
        char *to_ptr;
        int token_size;
        /**
         * Row is the one allocated by PRT user, may have multiple printed lines inside due to long cells
         */
        int i_line = 0;
        entry = (rhentry_t *) prt_row_start;
        if ((i_rows != 0) && (entry->mode == PRT_ROW_SEP_UNDERSCORE_BEFORE))
        {
            diag_sand_prt_row(total_width, '-');
        }
        do
        {
            /**
             * For each line zero cell_to_print, as if there will be no more lines in the row
             */
            cells_to_print = 0;
            if (i_line++ == 0)
            {
                /*
                 * Initialize next pointer for the first line in the row
                 */
                PRT_COLUMN_ITERATOR(k, prt_ctr)
                {
                    column[k].next_ptr = ((char *) prt_row_start) + sizeof(rhentry_t) + prt_ctr->col[k].cell_offset;
                }
            }
            else
            {   /* 
                 * If the line is not first, issue EOL
                 */
                cli_out("|\n");
            }

            PRT_COLUMN_ITERATOR(k, prt_ctr)
            {
                token_size = sal_strlen(column[k].next_ptr);
                if ((token_size > column[k].width) || prt_token_has_eol(column[k].next_ptr))
                {   /**
                     * Split the string to print portions fitting into the width
                     * Adjust token_size
                     */

                    switch (prt_ctr->col[k].flex_type)
                    {
                        case PRT_FLEX_BINARY:
                            token_size = column[k].width;
                            break;
                        case PRT_FLEX_ASCII:
                            token_size = prt_get_token_size(column[k].next_ptr, column[k].width);
                            break;
                        case PRT_FLEX_NONE:
                            SHR_CLI_EXIT(_SHR_E_INTERNAL, "Small column width:%d for the string:%d\n", column[k].width,
                                         token_size);
                            break;
                    }
                    memcpy(tmp_str, column[k].next_ptr, token_size);
                    cells_to_print++;
                    /*
                     * Make token NULL ended
                     */
                    tmp_str[token_size] = 0;
                    to_ptr = tmp_str;
                }
                else
                {
                    to_ptr = column[k].next_ptr;
                }
                cli_out(column[k].format, to_ptr);
                column[k].next_ptr += token_size;
            }
        }
        while (cells_to_print != 0);

        cli_out("|\n");
        if ((i_rows == row_num - 1) || (entry->mode == PRT_ROW_SEP_EQUAL))
        {
            diag_sand_prt_row(total_width, '=');
        }
        else if (entry->mode == PRT_ROW_SEP_UNDERSCORE)
        {
            diag_sand_prt_row(total_width, '-');
        }
        i_rows++;
    }

exit:
    if (tmp_str != NULL)
    {
        sal_free(tmp_str);
    }
    if (column != NULL)
    {
        sal_free(column);
    }
    SHR_FUNC_EXIT;
}
