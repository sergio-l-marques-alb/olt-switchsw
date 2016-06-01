/*
 * $Id: xml_api.c,v 1.00 Broadcom SDK $
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
 * File:    xml+apidataio_utils.c
 * Purpose:    Routines for handling XML DB and misc. file i/o
 */

#include <sal/core/libc.h>
#include <sal/appl/sal.h>
#include <sal/appl/io.h>
#include <sal/types.h>
#include <shared/bsl.h>
#include <shared/util.h>
#include <shared/error.h>

#include <shared/libxml/xml_api.h>

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

static uint32 xml_file_to_buf(char *filePath, char **buf, off_t *size_p)
{
    FILE *in;
    int nread;
    uint32 res = _SHR_E_NONE;
    int size;
    char *file_buf;

    /* Open file for reading */
    in = sal_fopen(filePath, "r");
    if (!in) {
        /* failure to open existing file is error */
        res = _SHR_E_EXISTS;
        goto out;
    }

    sal_fseek (in, 0, SEEK_END);
    size = sal_ftell (in);
    if (sal_fseek (in, 0, SEEK_SET) != 0){
        sal_fclose (in);
        return -1;
    }

    if(size == 0) {
        res = _SHR_E_PARAM;
        sal_fclose(in);
        goto out;
    }

    /* Allocate memory buffer for file contents */
    if((file_buf = sal_alloc(size, "buf")) == NULL) {
        cli_out("Failed to allocate memory");
        res = _SHR_E_MEMORY;
        sal_fclose(in);
        goto out;
    }

    /* Read file into memory */
    nread = sal_fread(file_buf, sizeof(char), size, in);
    if (nread != size) {
        cli_out("Short read from:\"%s\"\n", filePath);
        res = _SHR_E_INTERNAL;
        sal_fclose(in);
        /* We shouldn't fix such defects */
        /* coverity[tainted_data] */
        sal_free(file_buf);
        goto out;
    }

    *size_p = size;
    *buf = file_buf;
    sal_fclose(in);

out:
    return res;
}

void *xml_get_top(char *filepath, char *topname, int flags)
{
    xmlDocPtr doc;
    xmlNodePtr curTop = NULL;
    char *buf = NULL;
    off_t size;

    /* copy file contents into memory buffer */
    if(xml_file_to_buf(filepath, &buf, &size) != _SHR_E_NONE) {
        if(flags & CONF_OPEN_CREATE)
            curTop = xml_create_top(topname);
        goto out;
    }

    /* parse buffer: this will build an hierarchy tree */
    if((doc = xmlParseMemory(buf, size)) == NULL) {
        goto out;
    }

    /* get the tree root */
    if((curTop = xmlDocGetRootElement(doc)) == NULL) {
        xmlFreeDoc(doc);
        goto out;
    }

    if(xmlStrcmp(curTop->name, (const xmlChar *)topname)) {
        /* Bad file format "rules" should be the top entry always */
        curTop = NULL;
        xmlFreeDoc(doc);
        goto out;
    }

out:
    if(buf)
        sal_free(buf);
    return curTop;
}

void *xml_create_top(char *topname)
{
    xmlDocPtr doc;
    xmlNodePtr curTop = NULL;

    if((doc = xmlNewDoc((const xmlChar *)"1.0")) == NULL) {
        cli_out("xmlNewDoc() failed\n");
        goto out;
    }

    if((curTop = xmlNewNode(NULL, (const xmlChar *)topname)) == NULL)    {
        cli_out("xmlNewNode() failed\n");
        goto out;
    }

    xmlDocSetRootElement(doc, curTop);

    xml_special(curTop, "\n");

out:
    return curTop;
}

void xml_close_top(void *top)
{
    xmlNodePtr node = (xmlNodePtr)top;
    if(node && node->doc)
        xmlFreeDoc(node->doc);
}

void *xml_get_first(void *parent, char *name)
{
    xmlNodePtr node;

    for(node = ((xmlNodePtr)parent)->xmlChildrenNode; node; node = node->next) {

        if(xmlStrcmp(node->name, (const xmlChar *)name))
            continue;
        else
            return node;
    }

    return NULL;
}

void *xml_get_next(void *prev)
{
    xmlNodePtr node;

    if(prev == NULL)
        return NULL;

    for(node = ((xmlNodePtr)prev)->next; node; node = node->next) {

        if(xmlStrcmp(node->name, ((xmlNodePtr)prev)->name))
            continue;
        else
            return node;
    }

    return NULL;
}

void *xml_get_content_str(void *parent, char *nodename, char *target, int size)
{
    xmlNodePtr cur, textnode;

    if((cur = xml_get_first(parent, nodename)) == NULL)
        goto exit;

    /* There should be text node inside that will reveal its content */
    if((textnode = xml_get_first(cur, "text")) == NULL){
        strcpy(target, "");
        goto exit;
    }

    if(size <= sal_strlen((char *)textnode->content)) {
        cli_out("Target size:%d Content size:%d for %s\n", size, (int)sal_strlen((char *)textnode->content), textnode->content);
        strcpy(target, "");
        goto exit;
    }

    sprintf(target, "%s", textnode->content);

exit:
    return cur;
}

void *xml_set_content_str(void *parent, char *nodename, char *source, int depth)
{
    xmlNodePtr cur;

    if((cur = xml_add_node(parent, nodename, depth)) == NULL)
        goto exit;

    /* There should be text node inside that will reveal its content */
    xml_special(cur, source);

exit:
    return cur;
}

void *xml_set_content_int(void *parent, char *nodename, int source, int depth)
{
    xmlNodePtr cur;
    char temp[16];
    if((cur = xml_add_node(parent, nodename, depth)) == NULL)
        goto exit;

    /* There should be text node inside that will reveal its content */
    sprintf(temp, "%d", source);
    xml_special(cur, temp);

exit:
    return cur;
}

void *xml_get_content_int(void *parent, char *nodename, int *target)
{
    xmlNodePtr cur, textnode;

    if((cur = xml_get_first(parent, nodename)) == NULL)
        goto exit;

    /* There should be text node inside that will reveal its content */
    if((textnode = xml_get_first(cur, "text")) == NULL)
        goto exit;

    *target = _shr_ctoi((char *)textnode->content);

exit:
    return cur;
}

void *xml_get_text(void *parent, char *nodename)
{
    xmlNodePtr cur, textnode;

    if((cur = xml_get_first(parent, nodename)) == NULL)
        return NULL;

    /* There should be text node inside that will reveal tis content */
    if((textnode = xml_get_first(cur, "text")) == NULL)
        return NULL;

    return textnode->content;
}

int xml_get_self(void *cur, char *target, int size)
{
    xmlNodePtr textnode;
    int real_size = 0;

    /* There should be text node inside that will reveal its content */
    if((textnode = xml_get_first(cur, "text")) == NULL){
        strcpy(target, "");
        goto exit;
    }

    if(size <= sal_strlen((char *)textnode->content)) {
        cli_out("Target size:%d Content size:%d for %s\n", size, (int)sal_strlen((char *)textnode->content), textnode->content);
        strcpy(target, "");
        goto exit;
    }

    real_size = sal_strlen((char *)textnode->content);
    sprintf(target, "%s", textnode->content);

exit:
    return real_size;
}


void *xml_add_node(void *parent, char *name, int indent_num)
{
    xmlNodePtr node;
    int i;

    /* Check if it is the first node in parent, add one more new line */
    if(((xmlNodePtr)parent)->xmlChildrenNode == NULL) {
        xml_special(parent, "\n");
    }

    /* Each node should start with indent */
    for(i = 0; i < indent_num; i++) {
        if(!(node = xmlNewText((const xmlChar *)"\t"))) {
            cli_out("xmlNewText() failed\n");
            goto out;
        }

        if(!(node = xmlAddChild(parent, node))) {
            cli_out("xmlNewText() failed\n");
            goto out;
        }
    }

    node = xmlNewTextChild(parent, NULL, (const xmlChar *)name, NULL);
    if (!node)
        cli_out("xmlNewChild() failed\n");

    xml_special(parent, "\n");
/*
    for(i = 0; i < indent_num; i++)
        xml_special(node, "\t");
*/
out:
    return node;
}

void *xml_add_prev(void *parent, void *sibling, char *name, int indent_num)
{
    xmlNodePtr node, tmp_node;
    int i;


    /* Each node should start with indent */
    node = xmlNewTextChild(parent, NULL, (const xmlChar *)name, NULL);
    if(!node) {
        cli_out("xmlNewChild() failed\n");
        goto out;
    }

    if(!(node = xmlAddPrevSibling(sibling, node))) {
        cli_out("xmlAddNextSibling() failed\n");
        goto out;
    }

    for(i = 0; i < indent_num; i++) {
        if(!(tmp_node = xmlNewText((const xmlChar *)"\t"))) {
            cli_out("xmlNewText() failed\n");
            goto out;
        }

        if(!(tmp_node = xmlAddNextSibling(node, tmp_node))) {
            cli_out("xmlNewText() failed\n");
            goto out;
        }
    }

    tmp_node = xmlNewText((const xmlChar *)"\n");
    if(!tmp_node)
    {
        cli_out("xmlNewText() failed\n");
        goto out;
    }

    tmp_node = xmlAddNextSibling(node, tmp_node);
    if(!tmp_node)
    {
        cli_out("xmlNewText() failed\n");
        goto out;
    }

out:
    return node;
}

/* Get node with specific string in property name */
void *xml_get_node_with_name(void *parent, char *entry, char *name)
{
    xmlNodePtr node = NULL;
    char tmp_name[RHNAME_MAX_SIZE];

    RHDATA_ITERATOR(node, parent, entry) {
        RHDATA_GET_STR_CONT(node, "name", tmp_name);
        if(!strcmp(name, tmp_name))
            break;
    }

    return node;
}

/* Get node with specific string in property name */
void *xml_get_node_with_id(void *parent, char *entry, int id)
{
    xmlNodePtr node = NULL;
    int tmp_id;

    RHDATA_ITERATOR(node, parent, entry) {
        RHDATA_GET_INT_CONT(node, "id", tmp_id);
        if(id == tmp_id)
            break;
    }

    return node;
}

void *xml_special(void *parent, char *str)
{
    xmlNodePtr node;

    node = xmlNewText((const xmlChar *)str);
    if(!node)
    {
        cli_out("xmlNewText() failed\n");
        goto out;
    }

    node = xmlAddChild(parent, node);
    if (!node)
    {
        cli_out("xmlNewText() failed\n");
        goto out;
    }

out:
    return node;
}

void *xml_close_node(void *parent, int depth)
{
    xmlNodePtr node = NULL;
    int i;
    for(i = 0; i < depth; i++)
        node = xml_special(parent, "\t");
    return node;
}

void xml_delete_node(void *node)
{
    xmlNodePtr nodePrev = ((xmlNodePtr)node)->prev;

    /* We have found the entry to be deleted */
    xmlUnlinkNode(node);
    xmlFreeNode(node);
    /* We need to delete the previous empty node as well to avoid empty lines */
    if(nodePrev) {
        xmlUnlinkNode(nodePrev);
        xmlFreeNode(nodePrev);
    }
}

uint32 xml_set_int(void *node, char *property, int source)
{
    uint32 res = _SHR_E_NONE;
    char tmp[256];
    xmlAttrPtr attr;

    snprintf(tmp, sizeof(tmp), "%d", source);
    attr = xmlNewProp((xmlNodePtr)node, (const xmlChar *)property, (const xmlChar *)tmp);
    if(!attr) {
        cli_out("add property %s failed\n", property);
        res = _SHR_E_INTERNAL;
        goto out;
    }

out:
    return res;
}

uint32 xml_set_hex(void *node, char *property, int source)
{
    uint32 res = _SHR_E_NONE;
    char tmp[256];
    xmlAttrPtr attr;

    snprintf(tmp, sizeof(tmp), "0x%x", source);
    attr = xmlNewProp((xmlNodePtr)node, (const xmlChar *)property, (const xmlChar *)tmp);
    if(!attr) {
        cli_out("add property %s failed\n", property);
        res = _SHR_E_INTERNAL;
        goto out;
    }

out:
    return res;
}

uint32 xml_mod_int(void *node, char *property, int source)
{
    uint32 res = _SHR_E_NONE;
    char tmp[1024];
    xmlAttrPtr attr;

    snprintf(tmp, sizeof(tmp), "%d", source);
    attr = xmlHasProp((xmlNodePtr)node, (const xmlChar *)property);
    if(!attr)
        attr = xmlNewProp((xmlNodePtr)node, (const xmlChar *)property, (const xmlChar *)tmp);
    else
        attr = xmlSetProp((xmlNodePtr)node, (const xmlChar *)property, (const xmlChar *)tmp);
    if (!attr) {
        cli_out("mod property %s failed\n", property);
        res = _SHR_E_INTERNAL;
        goto out;
    }

out:
    return res;
}

uint32 xml_del_property(void *node, char *property)
{
    uint32 res = _SHR_E_NONE;
    xmlAttrPtr attr;

    attr = xmlHasProp((xmlNodePtr)node, (const xmlChar *)property);
    if(!attr) {
        res = _SHR_E_INTERNAL;
        goto out;
    }

    xmlRemoveProp(attr);

out:
    return res;
}

uint32 xml_mod_str(void *node, char *property, char *source)
{
    uint32 res = _SHR_E_NONE;
    xmlAttrPtr attr;

    attr = xmlSetProp((xmlNodePtr)node, (const xmlChar *)property, (const xmlChar *)source);
    if (!attr) {
        cli_out("set property %s failed", property);
        res = _SHR_E_INTERNAL;
        goto out;
    }

out:
    return res;
}

uint32 xml_set_str(void *node, char *property, char *source)
{
    uint32 res = _SHR_E_NONE;
    xmlAttrPtr attr;

    attr = xmlNewProp ((xmlNodePtr)node, (const xmlChar *)property, (const xmlChar *)source);
    if (!attr) {
        cli_out("add propery %s failed\n", property);
        res = _SHR_E_INTERNAL;
        goto out;
    }

out:
    return res;
}

void xml_save(void *top, char *filepath)
{
    xmlNodePtr node = (xmlNodePtr)top;

    /* save file! */
    xmlKeepBlanksDefault(1);

    if(node && node->doc)
        xmlSaveFormatFile(filepath, node->doc, 1);

    return;
}

uint32 xml_get_attributes(void *node, attribute_param_t *attribute_param, char *exclude_property, int max_num)
{
    xmlNodePtr int_node = (xmlNodePtr)node;
    xmlAttrPtr prop;
    uint32 res = _SHR_E_NONE;
    int i = 0;
    /*
     * Check on the properties attached to the node
     */
    prop = int_node->properties;

    while((prop != NULL) && (i < max_num)) {
        if(strcmp(exclude_property, (char *)prop->name)) {
            RHDATA_GET_STR_CONT(node, (char *)prop->name, attribute_param->value);
            strncpy(attribute_param->name, (char *)prop->name, RHNAME_MAX_SIZE - 1);

            attribute_param++;
            i++;
        }
        prop = prop->next;
    }

    return res;
}

uint32 xml_get_string(void *node, char *property, char *target, uint32 target_size)
{
    uint32 res = _SHR_E_NONE;
    char *prop;

    prop = (char *)xmlGetProp((xmlNodePtr)node, (const xmlChar *)property);
    if(!prop) {
        res = _SHR_E_INTERNAL;
        goto out;
    }

    if(target_size <= sal_strlen(prop)) {
        cli_out("Target size:%d Property size:%d for %s\n", target_size, (int)sal_strlen(prop), property);
        res = _SHR_E_PARAM;
        goto out;
    }

    sprintf(target, "%s", prop);
out:
    if(prop)
        xmlFree(prop);
    return res;
}

uint32 xml_get_int(void *node, char *property, int *target)
{
    uint32 res = _SHR_E_NONE;
    char *prop;

    prop = (char *)xmlGetProp(node, (const xmlChar *)property);
    if (!prop) {
        res = _SHR_E_INTERNAL;
        goto out;
    }

    *target = _shr_ctoi(prop);
    xmlFree(prop);

out:
    return res;
}

#if 0
uint32 xml_get_num(void *node, char *property, void *target, int target_size)
{
    uint32 res = _SHR_E_NONE;
    char *prop;

    prop = (char *)xmlGetProp(node, (const xmlChar *)property);
    if (!prop) {
        res = _SHR_E_INTERNAL;
        goto out;
    }

    if(((prop[0] == '0') && (prop[1] == 'x')) || ((prop[0] == '0') && (prop[1] == 'X'))) {
        switch(target_size) {
            case 1:
                sscanf(prop, "0x%hhx", (uint8 *)target);
                break;
            case 2:
                sscanf(prop, "0x%hx", (uint16 *)target);
                break;
            case 4:
                sscanf(prop, "0x%x", (uint32 *)target);
                break;
            case 8:
                sscanf(prop, "0x%llx", (unsigned long long *)target);
                break;
            default:
                cli_out("Bad target size for %d for:\"%s\"", target_size, property);
                res = _SHR_E_PARAM;
                break;
        }
    }
    else {
        switch(target_size) {
            case 1:
                *(char *)target = (char)_shr_ctoi(prop);
                break;
            case 2:
                *(short *)target = (short)_shr_ctoi(prop);
                break;
            case 4:
                *(int *)target = _shr_ctoi(prop);
                break;
            case 8:
                sscanf(prop, "%lld", (long long *)target);
                break;
            default:
                cli_out("Bad target size for %d for:\"%s\"", target_size, property);
                res = _SHR_E_PARAM;
                break;
        }
    }

    xmlFree(prop);

out:
    return res;
}
#endif
