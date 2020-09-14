/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * This File is Auto-generated - DO NOT TOUCH IT!!!!
 */

#include <shared/dbx/dbx_pre_compiled_xml_parser_internal.h>
#if defined(BCM_DNX_SUPPORT)
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>
/** Auto-generated includes */
#endif /* (BCM_DNX_SUPPORT) */
#include <soc/sand/auto_generated/common_dbx_pre_compiled.h>
#if defined(BCM_DNX_SUPPORT)
/** Auto-generated includes */
#endif /* (BCM_DNX_SUPPORT) */

/* *INDENT-OFF* */

#if !defined(NO_FILEIO)
void*
dbx_pre_compiled_common_top_get(
    int unit,
    char *filepath,
    char *topname,
    int flags)
{
    int i;

#if defined(BCM_DNX_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DNXF_SUPPORT)
    xml_node curTop = NULL;
#endif /* (BCM_DNX_SUPPORT) || (BCM_PETRA_SUPPORT) || (BCM_DNXF_SUPPORT) */


    for(i = 0; i < 1; i++)
    {
        /** Auto-generated top_get call per image.*/

#if defined(BCM_DNX_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DNXF_SUPPORT)
        curTop = dbx_xml_top_get_internal(filepath, topname, flags, pre_compiled_common_common_xml_to_buf);
        if(curTop)
        {
            return curTop;
        }
#endif /* (BCM_DNX_SUPPORT) || (BCM_PETRA_SUPPORT) || (BCM_DNXF_SUPPORT) */

        /** Auto-generated top_get call per image.*/

        /*
         * We can't find non-fast-prototype updated file path.
         */

#if defined(BCM_DNX_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DNXF_SUPPORT)
        curTop = dbx_file_get_xml_top(unit, filepath, topname, (flags & ~(CONF_OPEN_PER_DEVICE | CONF_OPEN_ALTER_LOC)) | CONF_OPEN_NO_ERROR_REPORT);
        if(curTop)
        {
            return curTop;
        }
#endif /* (BCM_DNX_SUPPORT) || (BCM_PETRA_SUPPORT) || (BCM_DNXF_SUPPORT) */



    }
    /*
     * We can't find non-fast-prototype.
     */
    return dbx_file_get_xml_top(unit, filepath, topname, flags);
}
#else
void*
dbx_pre_compiled_common_top_get(
    int unit,
    char *filepath,
    char *topname,
    int flags)
{
    return NULL;
}
#endif  /* !(NO_FILEIO) */
/* *INDENT-ON* */

