/**
 * \file sand_pparse.c
 *
 * Implementation of dynamic packet parsing engine.
 */
/*
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAGDNX

#include <sal/appl/sal.h>

#include <shared/bitop.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/dbx/dbx_xml.h>
#include <shared/dbx/dbx_file.h>
#include <shared/utilex/utilex_str.h>

#include <soc/sand/sand_pparse.h>
#include <soc/drv.h>


shr_error_e pparse_init(
    int unit,
    rhlist_t **pdb)
{
    SHR_FUNC_INIT_VARS(unit);
#if (!defined(NO_FILEIO))
    SHR_IF_ERR_EXIT(pparse_pdb_init(unit,"DNX-Devices.xml",0,pdb));
#else
    SHR_EXIT_WITH_LOG(_SHR_E_UNAVAIL, "PParse DB is not available when compiled without file system. %s%s%s\n", EMPTY, EMPTY, EMPTY);
#endif

exit:
    SHR_FUNC_EXIT;
}

shr_error_e pparse_packet_process(
    int unit,
    rhlist_t * parsing_db,
    pparse_packet_info_t * packet_info,
    rhlist_t ** pit)
{

    rhlist_t * parse_list = NULL;
    pparse_parsing_object_t * first_obj = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(packet_info, _SHR_E_PARAM, "packet_info");
    /*
     * If the parsing tree is NULL, allocate a new one.
     */
    if (NULL == *pit)
    {
        SHR_IF_ERR_EXIT(pparse_parsed_info_tree_create(unit, pit));
    }
    /*
     * Init an empty parsing stack
     */
    SHR_IF_ERR_EXIT(pparse_list_create(unit, &parse_list));
    /*
     * Get 1st structure and push it to the parsing stack
     */
    SHR_IF_ERR_EXIT(pparse_list_push(unit, packet_info->data_desc, parse_list, &first_obj));
    /*
     * Parse while the stack is not empty
     */
    SHR_IF_ERR_EXIT(pparse_list_process(unit, parsing_db, packet_info, parse_list, pit));

exit:
    pparse_list_free(unit, parse_list);
    SHR_FUNC_EXIT;
}

shr_error_e pparse_decode(
    int unit,
    char *desc,
    uint32 *data,
    uint32 data_size_bits,
    rhlist_t **parsed_info)
{
    rhlist_t *pdb = NULL;
    pparse_packet_info_t pkt;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(pparse_init(unit, &pdb));

    SHR_BITCOPY_RANGE(pkt.data, 0, data, 0, data_size_bits);
    pkt.data_desc = desc;
    pkt.data_size = data_size_bits;

    SHR_IF_ERR_EXIT(pparse_packet_process(unit, pdb, &pkt, parsed_info));

exit:
    utilex_rhlist_free_all(pdb);
    SHR_FUNC_EXIT;
}

static shr_error_e
pparse_packet_compose(
    int unit,
    rhlist_t * pdb,
    rhlist_t * packet_objects,
    uint32 max_data_size_bits,
    uint32 * data,
    uint32 * data_size_bits);

shr_error_e
pparse_compose(
    int unit,
    rhlist_t * packet_objects,
    uint32 max_data_size_bits,
    uint32 * data,
    uint32 * data_size_bits)
{
    rhlist_t *pdb = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(packet_objects, _SHR_E_PARAM, "packet_objects");
    SHR_NULL_CHECK(data, _SHR_E_PARAM, "data");
    SHR_NULL_CHECK(data_size_bits, _SHR_E_PARAM, "data_size_bits");

    SHR_IF_ERR_EXIT(pparse_init(unit, &pdb));

    SHR_IF_ERR_EXIT(pparse_packet_compose(unit, pdb, packet_objects, max_data_size_bits, data, data_size_bits));

exit:
    utilex_rhlist_free_all(pdb);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   <b> RECURSIVE! </b>
 *   Adds the fields and sub-fields in field_list to res_list in a flatten way at the tail.
 *   If a field added has some start bit and end bit, the argument start_bit is added to both values in an intention
 *   that the fields will be added with correct global offest (global in the context of the complete flattened field
 *   list).
 * \param [in] unit - Unit #
 * \param [in] hdr - Signal structure. It's field list is added to the result. The size is used for offset calculations.
 * \param [in] start_bit - "global" offset of the current fields added to the list.
 * \param [in] prefix - accumulated prefix to add to the field name in order to later identify it.
 * \param [out] res_list - list to which all the fields should be added.
 * \return shr_error_e Standard error handling
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
pparse_field_list_append_flattened(
    int unit,
    signal_output_t * hdr,
    int start_bit,
    const char * prefix,
    rhlist_t * res_list,
	uint8 is_network)
{
    signal_output_t * field;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * For each field in the input list,
     *   if it is a final field, add it.
     *   if it has sub-fields, add them recursively
     */
    RHITERATOR(field, hdr->field_list)
    {
        char full_name[RHNAME_MAX_SIZE];
        int field_start;
        int field_end;
        sal_snprintf(full_name, RHNAME_MAX_SIZE, "%s.%s", prefix, RHNAME(field));

        if(is_network)
        {
            field_start = start_bit + (hdr->size - 1 - field->end_bit);
            field_end = start_bit + (hdr->size - 1 - field->start_bit);
        }
        else
        {
            field_start = start_bit + field->start_bit;
            field_end = start_bit + field->end_bit;
        }

        if ((!field->field_list) || (field->field_list->num == 0))
        {
            /*
             * Final field (no sub-fields)
             */
            signal_output_t * added_field;
            rhhandle_t rhhandle;
            SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail(res_list, full_name, RHID_TO_BE_GENERATED, &rhhandle));
            added_field = rhhandle;
            added_field->start_bit = field_start;
            added_field->end_bit = field_end;
        }
        else
        {
            /*
             * Sub-fields exist. Recursive call*
             */
            SHR_IF_ERR_EXIT(pparse_field_list_append_flattened(unit, field, field_start,
                                                               full_name, res_list, is_network));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/** \see sand_pparse.h */
shr_error_e
pparse_combine(
    int unit,
    rhlist_t * sig_list,
    signal_output_t * combined_sig,
	uint8 is_network)
{
    device_t * device;
    signal_output_t * sig;
    uint32 fake_data[DSIG_MAX_SIZE_UINT32];
    rhlist_t * headers_list = NULL;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Input validation
     */
    SHR_NULL_CHECK(sig_list, _SHR_E_PARAM, "sig_list");
    SHR_NULL_CHECK(combined_sig, _SHR_E_PARAM, "combined_sig");
    /*
     * Resolve device for signal expansion
     */
    if ((device = sand_signal_device_get(unit)) == NULL)
    {
        SHR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND, "Could not find device configuration for unit-%d%s%s.", unit, EMPTY, EMPTY);
    }
    /*
     * Make sure combined_sig has an allocated field list
     */
    if (!combined_sig->field_list)
    {
        if ((combined_sig->field_list = utilex_rhlist_create("headers", sizeof(signal_output_t), 0)) == NULL)
        {
            SHR_EXIT_WITH_LOG(_SHR_E_FAIL, "Could not allocate field list. %s%s%s\n", EMPTY, EMPTY, EMPTY);
        }
    }
    /*
     * Prevent uninitialized values errors in Coverity/Valgrind.
     */
    sal_memset(fake_data, 0, sizeof(fake_data));
    /*
     * Create a list to hold expanded signals
     */
    if ((headers_list = utilex_rhlist_create("headers_list", sizeof(signal_output_t), 0)) == NULL)
    {
        SHR_EXIT_WITH_LOG(_SHR_E_FAIL, "Couldn't allocate headers list %s%s%s\n", EMPTY, EMPTY, EMPTY);
    }
    /*
     * For each requested header/signal,
     *   Get their structured field list.
     *   Append it to the result field list, flattened.
     */
    RHITERATOR(sig, sig_list)
    {
        signal_output_t * header;
        SHR_IF_ERR_EXIT(sand_signal_parse_get(unit, RHNAME(sig), NULL, NULL, PRINT_BIG_ENDIAN, fake_data,
                                                   DSIG_MAX_SIZE_BITS, headers_list));
        header = (signal_output_t*) headers_list->tail;
        if (!ISEMPTY(sig->expansion))
        {
            /*
             * Prefix required
             */
            char prefix[RHNAME_MAX_SIZE];
            sal_snprintf(prefix, RHNAME_MAX_SIZE, "%s_%s", sig->expansion, RHNAME(header));
            SHR_IF_ERR_EXIT(pparse_field_list_append_flattened(unit, header, combined_sig->size,
                                                               prefix, combined_sig->field_list, is_network));
        }
        else
        {
            SHR_IF_ERR_EXIT(pparse_field_list_append_flattened(unit, header, combined_sig->size,
                                                               RHNAME(sig), combined_sig->field_list, is_network));
        }
        combined_sig->size += header->size;
    }

exit:
    if (headers_list)
    {
        sand_signal_list_free(headers_list);
    }
    SHR_FUNC_EXIT;
}

/******************************************************
 *Parsing DB utilities
 *{
 ******************************************************/
#if (!defined(NO_FILEIO))

/**
 * \brief
 *   Finds the type of a condition (soc property/dnx-data/parsed field).
 */
static shr_error_e pparse_pdb_cond_type_get(
    int unit,
    xml_node cond_node,
    pparse_db_entry_t * obj,
    pparse_condition_t *cond)
{
    char type_name[RHSTRING_MAX_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    RHDATA_GET_STR_DEF_NULL(cond_node, "type", type_name);
    /*
     * Validity
     */
    if (ISEMPTY(type_name))
    {
        SHR_EXIT_WITH_LOG(_SHR_E_EMPTY, "Condition on parsing object %s has no type (supported: 'soc' or 'field'). %s%s\n",
                     obj->entry.name, EMPTY, EMPTY);
    }
    /*
     * Options
     */
    if (!strncmp(type_name, "soc", sizeof("soc")))
    {
        cond->type = PPARSE_CONDITION_TYPE_SOC;
    }
    else if (!strncmp(type_name, "field", sizeof("field")))
    {
        cond->type = PPARSE_CONDITION_TYPE_FIELD;
    }
    
    else
    {
        /*
         * Default
         */
        SHR_EXIT_WITH_LOG(_SHR_E_UNAVAIL, "Condition on parsing object %s has unsupported type: %s.%s\n", obj->entry.name,
                     type_name, EMPTY);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Parses a field type condition from the XML input
 */
static shr_error_e pparse_pdb_cond_field_parse(
    int unit,
    xml_node cond_node,
    pparse_db_entry_t * obj,
    pparse_condition_t *cond)
{
    xml_node field_node, enable_node;

    SHR_FUNC_INIT_VARS(unit);

    if (NULL == (field_node = dbx_xml_child_get_first(cond_node, "field")))
    {
        SHR_EXIT_WITH_LOG(_SHR_E_EMPTY, "Parsing: %s - Field type condition has no <field> node %s%s", obj->entry.name, EMPTY, EMPTY);
    }
    RHDATA_GET_STR_DEF_NULL(field_node, "path", cond->content.field.path);
    if (ISEMPTY(cond->content.field.path))
    {
        SHR_EXIT_WITH_LOG(_SHR_E_EMPTY, "Parsing: %s - object with Field condition has no path %s%s", obj->entry.name, EMPTY, EMPTY);
    }

    if (NULL == (enable_node = dbx_xml_child_get_first(cond_node, "enable")))
    {
        SHR_EXIT_WITH_LOG(_SHR_E_EMPTY, "Parsing: %s - Enable condition has no <enable> node %s%s", obj->entry.name, EMPTY, EMPTY);
    }
    RHDATA_GET_INT_DEF(enable_node, "val", cond->content.field.enable_value, 1);
    RHDATA_GET_INT_DEF(enable_node, "mask", cond->content.field.enable_mask, -1);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e pparse_pdb_cond_soc_get(
    int unit,
    xml_node cond_node,
    pparse_db_entry_t * obj,
    pparse_condition_t *cond)
{
    xml_node soc_node, enable_node;

    char * socval;
    char socname[RHSTRING_MAX_SIZE];
    char expval[RHSTRING_MAX_SIZE];
    uint32 defval = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (NULL == (soc_node = dbx_xml_child_get_first(cond_node, "soc_property")))
    {
        SHR_EXIT_WITH_LOG(_SHR_E_EMPTY, "Parsing: %s - SOC type condition has no <soc_property> node %s%s", obj->entry.name, EMPTY, EMPTY);
    }
    RHDATA_GET_STR_DEF_NULL(soc_node, "name", socname);
    if (ISEMPTY(socname))
    {
        SHR_EXIT_WITH_LOG(_SHR_E_EMPTY, "Parsing: %s - SOC type condition has empty <soc_property> node %s%s", obj->entry.name, EMPTY, EMPTY);
    }

    if (NULL == (enable_node = dbx_xml_child_get_first(cond_node, "enable")))
    {
        SHR_EXIT_WITH_LOG(_SHR_E_EMPTY, "Parsing: %s - Enable condition has no <enable> node %s%s", obj->entry.name, EMPTY, EMPTY);
    }
    RHDATA_GET_STR_DEF_NULL(enable_node, "val", expval);
    if (ISEMPTY(expval))
    {
        SHR_EXIT_WITH_LOG(_SHR_E_EMPTY, "Parsing: %s - Enable condition has empty <enable> node %s%s", obj->entry.name, EMPTY, EMPTY);
    }
    socval = soc_property_get_str(unit, socname);
    cond->content.soc.enabled = ((NULL == socval) ? defval : !strcmp(socval, expval));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e pparse_parse_db_add_condition(
    int unit,
    xml_node cond_node,
    pparse_db_entry_t * obj,
    pparse_object_extension_t * ext)
{
    pparse_condition_t *cond = &ext->conditions[ext->conditions_nof];
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(cond_node, _SHR_E_EMPTY, "cond_node");

    SHR_IF_ERR_EXIT(pparse_pdb_cond_type_get(unit, cond_node, obj, cond));

    switch (ext->conditions[ext->conditions_nof].type)
    {
    case PPARSE_CONDITION_TYPE_FIELD:
        SHR_IF_ERR_EXIT(pparse_pdb_cond_field_parse(unit, cond_node, obj, cond));
        break;
    case PPARSE_CONDITION_TYPE_SOC:
        SHR_IF_ERR_EXIT(pparse_pdb_cond_soc_get(unit, cond_node, obj, cond));
        break;
    default:
        SHR_EXIT_WITH_LOG(_SHR_E_INTERNAL, "Something went wrong. %s%s%s\n", EMPTY, EMPTY, EMPTY);
    }

    ext->conditions_nof++ ;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Resolve 'include' nodes by recursively calling init with their content
 */
static shr_error_e pparse_pdb_includes_add(
    int unit,
    xml_node cur_top,
    rhlist_t ** pdb)
{
    xml_node cur_include = NULL;

    SHR_FUNC_INIT_VARS(unit);

    RHDATA_ITERATOR(cur_include, cur_top, "include")
    {
        char filename[RHSTRING_MAX_SIZE];
        char type[RHSTRING_MAX_SIZE];
        int dev_specific = 0;
        RHDATA_GET_STR_CONT(cur_include, "file", filename);
        RHDATA_GET_INT_DEF(cur_include, "device_specific", dev_specific, 0);
        RHDATA_GET_STR_DEF(cur_include, "type", type, "objects");
        if (!sal_strncasecmp(type, "objects", RHSTRING_MAX_SIZE))
        {
            SHR_IF_ERR_EXIT(pparse_pdb_init(unit, filename, dev_specific, pdb));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Add all the objects in the xml tree to the parsing database pdb.
 */
static shr_error_e pparse_pdb_objects_add(
    int unit,
    xml_node cur_top,
    rhlist_t * pdb)
{
    xml_node obj_node;
    SHR_FUNC_INIT_VARS(unit);

    RHDATA_ITERATOR(obj_node, cur_top, "object")
    {
        pparse_db_entry_t * obj;
        rhhandle_t rhhandle;
        char name[RHNAME_MAX_SIZE];
        xml_node ext_node, desc_node;
        /*
         * Add a new object to the list
         */
        RHDATA_GET_STR_CONT(obj_node, "name", name);
        SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail(pdb, name, RHID_TO_BE_GENERATED, &rhhandle));
        obj = rhhandle;
        /*
         * Add more attributes to the object
         */
        RHDATA_GET_INT_CONT(obj_node, "size", obj->size);
        RHDATA_GET_STR_DEF_NULL(obj_node, "structure", obj->struct_name);
        RHDATA_GET_STR_DEF_NULL(obj_node, "block", obj->block_name);
        RHDATA_GET_STR_DEF_NULL(obj_node, "from", obj->stage_from_name);
        if (NULL != (desc_node = dbx_xml_child_get_first(obj_node, "description")))
        {
            if (!dbx_xml_node_get_content_str(desc_node, obj->desc_str, RHSTRING_MAX_SIZE))
            {
                /*
                 * No description copied for some reason
                 */
                obj->desc_str[0] = '\0';
            }
        }
        /*
         * Get extensions
         */
        RHDATA_ITERATOR(ext_node, obj_node, "extension")
        {
            xml_node cond_node;
            pparse_object_extension_t * ext = &obj->extensions[obj->extensions_nof];
            RHDATA_GET_STR_CONT(ext_node, "name", ext->object_name);
            RHDATA_ITERATOR(cond_node, ext_node, "condition")
            {
                SHR_IF_ERR_EXIT(pparse_parse_db_add_condition(unit, cond_node, obj, ext));
            }
            obj->extensions_nof++;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Initialize the parsing DB according to filename
 * \param [in] unit - Unit #
 * \param [in] filename - where the DB (or the top if the hierarchy resides)
 * \param [in] pdb - pointer to pointer to rhlist of pparse_db_entry_t. If points to NULL, the list will be created.
 * \return shr_error_e Standard Error
 */
shr_error_e pparse_pdb_init(
    int unit,
    char * filename,
    uint8 device_specific,
    rhlist_t ** pdb)
{
    xml_node cur_top = NULL, parsing_objs_node;
    char * device_name = NULL;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Check input
     */
    if (device_specific)
    {
        device_t *device = sand_signal_device_get(unit);
        device_name = RHNAME(device);
    }
    /*
     * Open xml file if it exists
     */
    if ((cur_top = dbx_file_get_xml_top(device_name, filename, "top", 0)) == NULL)
    {
        SHR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND, "Parsing DB xml not found: %s/%s. %s\n",
                     (device_name==NULL ? "." : device_name),filename, EMPTY);
    }
    /*
     * If necessary, initialize the parsing DB structure (rhlist)
     */
    if (NULL == *pdb)
    {
        if ((*pdb = utilex_rhlist_create("pdb", sizeof(pparse_db_entry_t), 0)) == NULL)
        {
            SHR_EXIT_WITH_LOG(_SHR_E_RESOURCE, "Could not allocate parsing DB strucutre. %s%s%s\n", EMPTY, EMPTY, EMPTY);
        }
    }
    RHDATA_ITERATOR(parsing_objs_node, cur_top, "parsing-objects")
    {
        /*
         * First resolve include nodes (recursive)
         */
        SHR_IF_ERR_EXIT(pparse_pdb_includes_add(unit, parsing_objs_node, pdb));
        /*
         * Then add all objects
         */
        SHR_IF_ERR_EXIT(pparse_pdb_objects_add(unit, parsing_objs_node, *pdb));
    }

exit:
    dbx_xml_top_close(cur_top);
    SHR_FUNC_EXIT;
}

#endif /*
        * !defined(NO_FILEIO)
        */

/******************************************************
 *Parsing DB utilities
 *}
 ******************************************************/

shr_error_e pparse_buffer_prepare(
    int unit,
    pparse_parsing_object_t * pobj,
    pparse_packet_info_t * packet_info,
    uint32 * buffer)
{
    uint32 obj_size;
    uint32 *data = packet_info->data;

    SHR_FUNC_INIT_VARS(unit);

    obj_size = pobj->size;
    if (obj_size > packet_info->data_size)
    {
        uint32 diff = obj_size - packet_info->data_size;
        SHR_BITCLR_RANGE(buffer, 0, diff);
        SHR_BITCOPY_RANGE(buffer, diff, data, 0, (packet_info->data_size));
    }
    else
    {
        SHR_BITCOPY_RANGE(buffer, 0, data, (packet_info->data_size - obj_size), obj_size);
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e pparse_condition_val_get(
    int unit,
    pparse_condition_t * cond,
    rhlist_t * pit,
    uint32 * val,
    uint8 * found)
{
    char ** path;
    uint32 part_i, parts;

    rhlist_t * field_list = pit;
    signal_output_t *part_sig = NULL;

    uint32 size_to_copy = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (NULL == (path = utilex_str_split(cond->content.field.path, ".", 10, &parts)))
    {
        SHR_EXIT_WITH_LOG(_SHR_E_MEMORY, "PParse - Could not parse field path: %s%s%s\n", cond->content.field.path, EMPTY, EMPTY);
    }

    for (part_i = 0; part_i < parts; part_i++)
    {
        if (NULL == (part_sig = utilex_rhlist_entry_get_last_by_name(field_list, path[part_i])))
        {
            *found = 0;
            SHR_EXIT();
        }
        /*if (NULL == (part_sig = utilex_rhlist_entry_get_by_name(field_list, path[part_i])))
        {
            SHR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND, "PParse - field not found: %s%s%s\n", cond->content.field.path, EMPTY, EMPTY);
        }*/

        field_list = part_sig->field_list;
    }
    if (NULL != part_sig)
    {
        *found = 1;
        size_to_copy = part_sig->size > 32 ? 32 : part_sig->size;
        SHR_BITCOPY_RANGE(val, 0, part_sig->value, 0, size_to_copy);
    }
    else
    {
        /*
         * The path could not have been resolved
         */
        *found = 0;
    }

exit:
    utilex_str_split_free(path, parts);
    SHR_FUNC_EXIT;
}

shr_error_e pparse_extension_condition_apply(
    int unit,
    pparse_condition_t * cond,
    rhlist_t * pit,
    uint8 * valid)
{
    uint32 val = 0, mask = 0;
    uint8 field_found;

    SHR_FUNC_INIT_VARS(unit);

    switch (cond->type)
    {
    case PPARSE_CONDITION_TYPE_SOC:
        if (cond->content.soc.enabled == 0)
        {
            *valid = 0;
        }
        break;
    case PPARSE_CONDITION_TYPE_FIELD:
        mask = cond->content.field.enable_mask;
        SHR_IF_ERR_EXIT(pparse_condition_val_get(unit, cond, pit, &val, &field_found));
        if ((!field_found) || ((val & mask) != cond->content.field.enable_value))
        {
            *valid = 0;
        }
        break;
    default:
        SHR_EXIT_WITH_LOG(_SHR_E_INTERNAL, "Something went wrong %s%s%s\n", EMPTY, EMPTY, EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief  Check for an extension if the condition(s) to add it are met.
 * If so, add it.
 */
shr_error_e pparse_object_extension_check_conditions(
    int unit,
    pparse_object_extension_t * db_ext,
    rhlist_t * pit,
    uint8 * valid)
{
    int cond_i;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(valid, _SHR_E_INTERNAL, "valid");

    *valid = 1;
    for (cond_i = 0; cond_i < db_ext->conditions_nof; cond_i++)
    {
        /*
         * If conditions aren't met, the size would be 0
         */
        SHR_IF_ERR_EXIT(pparse_extension_condition_apply(unit, &db_ext->conditions[cond_i], pit, valid));
        if (*valid == 0)
        {
            SHR_EXIT();
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e pparse_object_get(
    int unit,
    rhlist_t * pdb,
    pparse_parsing_object_t * obj)
{
    pparse_db_entry_t * db_entry = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if ((db_entry = utilex_rhlist_entry_get_by_name(pdb, obj->rhentry.name)) == NULL)
    {
        SHR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND, "Could not find %s in the parsing DB.%s%s\n", obj->rhentry.name, EMPTY, EMPTY);
    }
    obj->struct_name = db_entry->struct_name;
    obj->block_name = db_entry->block_name;
    obj->stage_from_name = db_entry->stage_from_name;
    obj->size = db_entry->size;
    obj->extensions = db_entry->extensions;
    obj->extensions_nof = db_entry->extensions_nof;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Push next headers to the stack in reverse order
 */
shr_error_e pparse_list_next_extensions_add(
    int unit,
    pparse_parsing_object_t *next,
    rhlist_t * pit,
    rhlist_t * parse_list)
{
    int ext_i;

    SHR_FUNC_INIT_VARS(unit);

    for (ext_i = next->extensions_nof - 1; ext_i >= 0; --ext_i)
    {
        pparse_object_extension_t *ext = &next->extensions[ext_i];
        uint8 valid = 0;
        SHR_IF_ERR_EXIT(pparse_object_extension_check_conditions(unit, ext, pit, &valid));
        if (valid)
        {
            pparse_parsing_object_t *next_obj;
            SHR_IF_ERR_EXIT(pparse_list_push(unit, ext->object_name, parse_list, &next_obj));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e pparse_list_process(
    int unit,
    rhlist_t * pdb,
    pparse_packet_info_t * packet_info,
    rhlist_t * parse_list,
    rhlist_t ** pit)
{

    pparse_parsing_object_t * next = NULL;

    SHR_FUNC_INIT_VARS(unit);

    while (NULL != (next = pparse_list_get_next(unit, parse_list)))
    {
        uint32 buffer[PPARSE_MAX_DATA_WORDS];
        /*
         * Get details from the parsing DB
         */
        SHR_IF_ERR_EXIT(pparse_object_get(unit, pdb, next));

        if (next->size > 0)
        {
            uint32 parsed_size;
            /*
             * Prepare the data for the signal parsing engine on the buffer
             */
            SHR_IF_ERR_EXIT(pparse_buffer_prepare(unit, next, packet_info, buffer));
            /*
             * Use the signal parsing engine to parse the buffer into the pit
             */
            SHR_IF_ERR_EXIT_WITH_LOG(sand_signal_parse_get(unit, next->struct_name, next->block_name,
                                          next->stage_from_name, PRINT_BIG_ENDIAN, buffer, next->size, *pit),
                                     "Could not find structure: '%s'%s%s\n",next->struct_name, EMPTY, EMPTY);
            /*
             * Get actual parsed size, since expansion of structs can cause their size to actually be smaller than the
             * originally allocated size.
             */
            parsed_size = ((signal_output_t *)((*pit)->tail))->size;
            /*
             * Peel parsed data from the packet
             */
            if (packet_info->data_size > parsed_size)
            {
                packet_info->data_size -= parsed_size;
            }
            else
            {
                /*
                 * Out of data to parse
                 */
                packet_info->data_size = 0;
                break;
            }
        }
        /*
         * Add extensions if there are and they meet the conditions to add them
         */
        SHR_IF_ERR_EXIT(pparse_list_next_extensions_add(unit, next, *pit, parse_list));
        /*
         * Current header parsed => no need for the data in the stack anymore.
         */
        SHR_IF_ERR_EXIT(pparse_list_pop(unit, parse_list));
    }
exit:
    SHR_FUNC_EXIT;
}


/*
 * *********************************************************************************
 *                                    UTILITIES
 * *********************************************************************************
 */

shr_error_e pparse_parsed_info_tree_create(
    int unit,
    rhlist_t ** pit)
{
    SHR_FUNC_INIT_VARS(unit);
    if ((*pit = utilex_rhlist_create("PParse Information", sizeof(signal_output_t), 0)) == NULL)
    {
        SHR_EXIT_WITH_LOG(_SHR_E_RESOURCE, "Could not allocate parsed information buffer to parse the required packet. %s%s%s\n", EMPTY, EMPTY, EMPTY);
    }
exit:
    SHR_FUNC_EXIT;
}

void pparse_parsed_info_tree_free(
    int unit,
    rhlist_t * pit)
{
    if (pit != NULL)
    {
        utilex_rhlist_free_all(pit);
    }
}


shr_error_e pparse_list_create(
    int unit,
    rhlist_t ** plist)
{
    SHR_FUNC_INIT_VARS(unit);
    if ((*plist = utilex_rhlist_create("plist", sizeof(pparse_parsing_object_t), 0)) == NULL)
    {
        SHR_EXIT_WITH_LOG(_SHR_E_RESOURCE, "Could not allocate parsing stack to parse the required packet. %s%s%s\n", EMPTY, EMPTY, EMPTY);
    }
exit:
    SHR_FUNC_EXIT;
}

void pparse_list_free(
    int unit,
    rhlist_t * plist)
{
    utilex_rhlist_free_all(plist);
}

shr_error_e pparse_list_push(
    int unit,
    char * object_name,
    rhlist_t * plist,
    pparse_parsing_object_t ** new_item_p)
{
    rhhandle_t entry_handle;
    rhhandle_t top_handle;

    SHR_FUNC_INIT_VARS(unit);

    top_handle = utilex_rhlist_entry_get_first(plist);
    /*
     * If the list is not empty, the 1st item is being processed. Push after it.
     */
    if (top_handle != NULL)
    {
        SHR_IF_ERR_EXIT(utilex_rhlist_entry_alloc(plist, object_name, RHID_TO_BE_GENERATED, &entry_handle));
        SHR_IF_ERR_EXIT(utilex_rhlist_entry_insert_before(plist, entry_handle, ((rhentry_t *)top_handle)->next));
    }
    else
    {
        SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_top(plist, object_name, RHID_TO_BE_GENERATED, &entry_handle));
    }

    *new_item_p = entry_handle;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e pparse_list_add(
    int unit,
    char * object_name,
    rhlist_t * plist,
    pparse_parsing_object_t ** new_item_p)
{
    rhhandle_t entry_handle;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail(plist, object_name, RHID_TO_BE_GENERATED, &entry_handle));

    *new_item_p = entry_handle;

exit:
    SHR_FUNC_EXIT;
}

pparse_parsing_object_t * pparse_list_get_next(
    int unit,
    rhlist_t * plist)
{
    return utilex_rhlist_entry_get_first(plist);
}

shr_error_e pparse_list_pop(
    int unit,
    rhlist_t * plist)
{
    rhhandle_t entry ;

    SHR_FUNC_INIT_VARS(unit);

    if ((entry = utilex_rhlist_entry_get_first(plist)) == NULL)
    {
        SHR_EXIT_WITH_LOG(_SHR_E_INTERNAL, "Something went wrong - parsing stack emptied too soon. %s%s%s\n", EMPTY, EMPTY, EMPTY);
    }

    SHR_IF_ERR_EXIT(utilex_rhlist_entry_del_free(plist, entry));

exit:
    SHR_FUNC_EXIT;
}

/*************************************************
 * PPARSE COMPOSE*
 *************************************************/

/**
 * \brief
 *   If the field specified in 'cond' is not already in 'fields', add a new field to it with cond's
 *   path and enable value.
 * \param [in] unit - Unit #
 * \param [in] cond - Relevant condition structure
 * \param [out] fields - List of header fields (entry type: signal_output_t , initialized)
 * \return shr_error_e Std. err
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
pparse_compose_field_from_condition_add(
    int unit,
    pparse_condition_t * cond,
    signal_output_t * header)
{

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Make sure the field list is allocated
     */
    if (header->field_list == NULL)
    {
        if (NULL == (header->field_list = utilex_rhlist_create("header_fields", sizeof(signal_output_t), 0)))
        {
            SHR_EXIT_WITH_LOG(_SHR_E_MEMORY, "Could not allocate field list for header '%s' %s%s", RHNAME(header), EMPTY, EMPTY);
        }
    }
    /*
     * Only add the field if it doesn't already exist.
     */
    if (NULL == utilex_rhlist_entry_get_by_name(header->field_list, cond->content.field.path))
    {
        signal_output_t * new_field;
        rhhandle_t rhhandle;
        SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail(header->field_list, cond->content.field.path, RHID_TO_BE_GENERATED,
                                                     &rhhandle));
        new_field = rhhandle;
        new_field->value[0] = cond->content.field.enable_value;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Generate more field values from the parsing conditions for following objects.
 *   For example: for IPv4 to come after ETH, the ETH.Type should be 0x0800. If the ETH.Type field isn't
 *   already specified, add this value to the ETH header.
 *   pseudo code:
 *   for each obj in obj_list {
 *       if obj->next != NULL {
 *           for ext in obj->extensions {
 *               if ext->name == obj->next->name {
 *                   for cond in ext->conditions {
 *                       if cond->type == FIELD {
 *                           obj->fields.add(cond2field(cond))
 *                       }
 *                   }
 *               }
 *           }
 *       }
 *   }
 * \param [in] unit - Unit #
 * \param [in] pdb - Parsing DB (initialized)
 * \param [in_out] packet_objects - packet required objects and fields
 * \return shr_error_e Std. error
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
pparse_compose_fields_from_conditions_add(
    int unit,
    rhlist_t * pdb,
    rhlist_t * packet_objects)
{
    /*pparse_compose_object_t * pkt_obj;*/
    signal_output_t * pkt_obj;

    SHR_FUNC_INIT_VARS(unit);

    RHITERATOR(pkt_obj, packet_objects)
    {
        pparse_parsing_object_t pdb_obj;
        sal_strncpy(RHNAME(&pdb_obj), RHNAME(pkt_obj), RHNAME_MAX_SIZE - 1);
        /*
         * Get details from the parsing DB
         */
        SHR_IF_ERR_EXIT(pparse_object_get(unit, pdb, &pdb_obj));
        sal_strncpy(RHNAME(pkt_obj), pdb_obj.struct_name, RHNAME_MAX_SIZE - 1);
        if (pkt_obj->entry.next)
        {
            /*
             * There is a following object. Check if it is an extension of the current object, or indicated by it.
             * If so, turn any field-condition that enables the extension to a field value for the current object.
             */
            int ext_i;
            for (ext_i = 0; ext_i < pdb_obj.extensions_nof; ext_i++)
            {
                pparse_object_extension_t *ext = &(pdb_obj.extensions[ext_i]);
                if (0 == sal_strncasecmp(ext->object_name, pkt_obj->entry.next->name, RHNAME_MAX_SIZE))
                {
                    /*
                     * Found that the following object is indicated by the current.
                     */
                    int cond_i;
                    for (cond_i = 0; cond_i < ext->conditions_nof; cond_i++)
                    {
                        pparse_condition_t *cond = &(ext->conditions[cond_i]);
                        if (cond->type == PPARSE_CONDITION_TYPE_FIELD)
                        {
                            /*
                             * Found field-condition. Generate a field from it.
                             */
                            SHR_IF_ERR_EXIT(pparse_compose_field_from_condition_add(unit, cond, pkt_obj));
                        }
                    }
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Creates raw packet data with the headers specified in 'packet_objects'.
 *    If some rules for the sequence exist (next protocol, eth-type, ...), set the implicit field values.
 * \param [in] unit - Unit #
 * \param [in] pdb - parsing objects collection
 * \param [in] packet_objects - packet required objects and fields
 * \param [in] max_data_size_bits - max #bits on the result packet
 * \param [out] pkt - output object (data and size)
 * \return shr_error_e Std. error
 * \remark
 *   * None
 * \see
 *   * pparse_compose_fields_from_conditions_add
 */
static shr_error_e
pparse_packet_compose(
    int unit,
    rhlist_t * pdb,
    rhlist_t * packet_objects,
    uint32 max_data_size_bits,
    uint32 * data,
    uint32 * data_size_bits)
{
    signal_output_t * pkt_obj;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Add all condition-fields
     */
    SHR_IF_ERR_EXIT(pparse_compose_fields_from_conditions_add(unit, pdb, packet_objects));
    /*
     * Generate headers data and concatenate them.
     * Start from the last header (innermost), put it at the LSB and iterate backwards.
     */
    for (pkt_obj = utilex_rhlist_entry_get_last(packet_objects);
          pkt_obj != NULL;
          pkt_obj = utilex_rhlist_entry_get_prev(pkt_obj))
    {
        SHR_IF_ERR_EXIT(sand_signal_compose(unit, pkt_obj, NULL, NULL, PRINT_BIG_ENDIAN));
        if (pkt_obj->size + (*data_size_bits) > max_data_size_bits)
        {
            SHR_EXIT_WITH_LOG(_SHR_E_PARAM, "Packet size has grown too much while adding header '%s' %s%s", RHNAME(pkt_obj), EMPTY, EMPTY);
        }
        SHR_BITCOPY_RANGE(data, *data_size_bits, pkt_obj->value, 0, pkt_obj->size);
        *data_size_bits += pkt_obj->size;
    }

exit:
    SHR_FUNC_EXIT;
}

