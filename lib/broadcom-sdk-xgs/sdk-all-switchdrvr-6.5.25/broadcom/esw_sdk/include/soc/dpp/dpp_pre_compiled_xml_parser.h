/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * This File is Auto-generated - DO NOT TOUCH IT!!!!
 */

#include <sal/types.h>
#include <shared/shrextend/shrextend_error.h>

#ifndef _PRE_COMPILED_XML_PARSER_H
#define _PRE_COMPILED_XML_PARSER_H

shr_error_e pre_compiled_xml_to_buf(
    int unit,
    char *filePath,
    char **buf,
    long int *size_p);

int pre_compiled_xml_file_exists(
    char *filepath);

#endif /* _PRE_COMPILED_XML_PARSER_H */
