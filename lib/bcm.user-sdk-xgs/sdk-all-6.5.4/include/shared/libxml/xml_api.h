/*
 * $Id: xml_api.h,v 1.00 Broadcom SDK $
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
 * File:    xml_api.h
 * Purpose:    Types and structures used when working with data IO
 */

#ifndef __XML_API_H
#define __XML_API_H

#ifdef __KERNEL__
#include <linux/string.h>
#else
#include <string.h>
#endif
#include <soc/dpp/SAND/Utils/sand_dataio_utils.h>

/* open file flags */
#define  CONF_OPEN_CREATE     0x0001

typedef struct {
    char    name[RHNAME_MAX_SIZE];
    char    value[RHNAME_MAX_SIZE];
} attribute_param_t;

void    *xml_get_top(char *filename, char *table_name, int flags);
void    *xml_create_top(char *topname);
void     xml_close_top(void *top);
void    *xml_special(void *parent, char *str);
void    *xml_close_node(void *parent, int depth);
void    *xml_get_first(void *parent, char *name);
void    *xml_get_next(void *prev);
void    *xml_get_text(void *cur, char *name);
int      xml_get_self(void *cur, char *target, int size);
void    *xml_get_content_str(void *parent, char *nodename, char *target, int size);
void    *xml_get_content_int(void *parent, char *nodename, int *target);
void    *xml_set_content_str(void *parent, char *nodename, char *source, int depth);
void    *xml_set_content_int(void *parent, char *nodename, int source, int depth);

uint32 xml_get_attributes(void *node, attribute_param_t *attribute_param, char *exclude_property, int max_num);
uint32 xml_get_string(void *node, char *property, char *target, uint32 target_size);
uint32 xml_get_int(void *node, char *property, int *target);
uint32 xml_get_num(void *node, char *property, void *target, int target_size);
uint32 xml_set_str(void *node, char *property, char *source);
uint32 xml_set_int(void *node, char *property, int source);
uint32 xml_set_hex(void *node, char *property, int source);
uint32 xml_mod_str(void *node, char *property, char *source);
uint32 xml_mod_int(void *node, char *property, int source);

uint32   xml_del_property(void *node, char *property);
void    *xml_add_prev(void *parent, void *sibling, char *name, int indent_num);
void    *xml_add_node(void *parent, char *name, int indent_num);
void     xml_delete_node(void *node);
void     xml_save(void *top, char *filename);
void    *xml_get_node_with_name(void *parent, char *entry, char *name);
void    *xml_get_node_with_id(void *parent, char *entry, int id);

#define RHDATA_ITERATOR(node, parent, name)                        \
    for(node = xml_get_first(parent, name); node; node = xml_get_next(node))

#define RHDATA_SAFE_ITERATOR(node, parent, name)                        \
void *next_node;\
    for(node = xml_get_first(parent, name), next_node = xml_get_next(node); node; node = next_node, next_node = xml_get_next(node))

#define RHDATA_GET_NODE_NUM(mnum, mparent, mname) {            \
            void *mnode;                                       \
            RHDATA_ITERATOR(mnode, mparent, mname)             \
                mnum++;                                        \
        }

#define RHDATA_GET_STR_CONT(node, string, target)                                            \
    if((xml_get_string(node, string, target, RHNAME_MAX_SIZE)) != _SHR_E_NONE) {    \
        cli_out("No \"%s\" inside node, skipping\n", string);                                \
        continue;                                                                            \
    }

#define RHDATA_GET_LSTR_CONT(node, string, target)                                           \
    if((xml_get_string(node, string, target, RHSTRING_MAX_SIZE)) != _SHR_E_NONE) {  \
        cli_out("No \"%s\" inside node, skipping\n", string);                                \
        continue;                                                                            \
    }

#define RHDATA_GET_STR_STOP(node, string, target)                                               \
    if((res = xml_get_string(node, string, target, RHNAME_MAX_SIZE)) != _SHR_E_NONE) { \
        cli_out("No \"%s\" inside node, leaving\n", string);                                    \
        goto exit;                                                                              \
    }

#define RHDATA_GET_LSTR_STOP(node, string, target)                                                \
    if((res = xml_get_string(node, string, target, RHSTRING_MAX_SIZE)) != _SHR_E_NONE) { \
        cli_out("No \"%s\" inside node, leaving\n", string);                                      \
        goto exit;                                                                                \
    }

#define RHDATA_GET_STR_DEF(node, string, target)                                            \
    if((xml_get_string(node, string, target, RHNAME_MAX_SIZE)) != _SHR_E_NONE) {            \
        target[0] = 0;                                                                      \
    }

#define RHDATA_GET_STR_DEF1(node, string, target, default_str)                              \
    if((xml_get_string(node, string, target, RHNAME_MAX_SIZE)) != _SHR_E_NONE) {            \
        strcpy(target, default_str);                                                        \
    }

#define RHDATA_GET_INT_CONT(node, string, target)                                            \
    if((xml_get_int(node, string, (int *)(&(target)))) != _SHR_E_NONE) {                     \
        cli_out("No \"%s\" inside node, skipping\n", string);                                \
        continue;                                                                            \
    }

#define RHDATA_GET_INT_STOP(node, string, target)                                            \
    if((res = xml_get_int(node, string, &(target))) != _SHR_E_NONE) {                        \
        cli_out("No \"%s\" inside node, leaving\n", string);                                 \
        goto exit;                                                                           \
    }

#define RHDATA_GET_INT_DEF(node, string, target, def_value)                                  \
    if((xml_get_int(node, string, (int *)(&(target)))) != _SHR_E_NONE) {                     \
        target = def_value;                                                                  \
    }

#define RHDATA_GET_NUM(node, string, target)                                                   \
    if((xml_get_num(node, string, (void *)&(target), sizeof(target))) != _SHR_E_NONE) {        \
        cli_out("No \"%s\" inside node, skipping\n", string);                                  \
        continue;                                                                              \
    }

#define RHDATA_GET_NUM_CONT(node, string, target)                                              \
    if((xml_get_num(node, string, (void *)&(target), sizeof(target))) != _SHR_E_NONE) {        \
        cli_out("No \"%s\" inside node, skipping\n", string);                                  \
        continue;                                                                              \
    }

#define RHDATA_GET_NUM_STOP(node, string, target)                                              \
    if((res = xml_get_num(node, string, (void *)&(target), sizeof(target))) != _SHR_E_NONE) {  \
        cli_out("No \"%s\" inside node, leaving\n", string);                                   \
        goto exit;                                                                             \
    }

#define RHDATA_GET_NUM_DEF(node, string, target, def_value)                                    \
    if((xml_get_num(node, string, (void *)&(target), sizeof(target))) != _SHR_E_NONE) {        \
        target = def_value;                                                                    \
    }

#define RHDATA_SET_INT(node, string, source)                                 \
    if((res = xml_set_int(node, string, source)) != _SHR_E_NONE) {  \
        cli_out("Could not set property %s\n", string);                      \
        goto exit;                                                           \
    }

#define RHDATA_SET_HEX(node, string, source)                                 \
    if((res = xml_set_hex(node, string, source)) != _SHR_E_NONE) {  \
        cli_out("Could not set property %s\n", string);                      \
        goto exit;                                                           \
    }

#define RHDATA_SET_STR(node, string, source)                                 \
    if((res = xml_set_str(node, string, source)) != _SHR_E_NONE) {  \
        cli_out("Could not set property %s\n", string);                      \
        goto exit;                                                           \
    }

#define RHDATA_SET_BOOL(node, string, source)                                \
    if((res = xml_set_str(node, string, (source != 0) ? "Yes":"No")) != _SHR_E_NONE) {  \
        cli_out("Could not set property %s\n", string);                      \
        goto exit;                                                           \
    }

#define RHDATA_MOD_INT(node, string, source)                                 \
    if((res = xml_mod_int(node, string, source)) != _SHR_E_NONE) {  \
        cli_out("Could not set property %s\n", string);                      \
        goto exit;                                                           \
    }

#define RHDATA_MOD_STR(node, string, source)                                 \
    if((res = xml_mod_str(node, string, source)) != _SHR_E_NONE) {  \
        cli_out("Could not set property %s\n", string);                      \
        goto exit;                                                           \
    }

#endif /* __XML_API_H */
