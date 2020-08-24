/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * This File is Auto-generated - DO NOT TOUCH IT!!!!
 */

#if defined(BCM_DNX_SUPPORT)

#include <shared/dbx/dbx_pre_compiled_xml_parser_internal.h>
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>

/** Auto-generated includes */
#include <bcm_int/dnx/kbp/auto_generated/standard_1_dbx_pre_compiled_kbp_xml_parser.h>
/** Auto-generated includes */

/* *INDENT-OFF* */

void*
dbx_pre_compiled_kbp_top_get(
    int unit,
    char *filepath,
    char *topname,
    int flags)
{
    uint8 is_image;

    /** Auto-generated top_get call per image.*/
    dnx_pp_prgm_current_image_check(unit, "standard_1", &is_image);
    if(is_image)
    {
        return dbx_xml_top_get_internal(filepath, topname, flags, pre_compiled_kbp_standard_1_xml_to_buf);
    }
    /** Auto-generated top_get call per image.*/

    /*
     * We can't find non-fast-prototype imag.
     */
    return dbx_file_get_xml_top(unit, filepath, topname, flags);
}
/* *INDENT-ON* */
#else /* (BCM_DNX_SUPPORT) */

typedef int make_iso_compilers_happy;

#endif /* (BCM_DNX_SUPPORT) */
