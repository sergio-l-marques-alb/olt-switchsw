/*
 * $Id: sand_dataio_utils.c,v 1.00 Broadcom SDK $
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
 * File:    sand_dataio_utils.c
 * Purpose:    Misc. routines used by export/import/show facilities
 */

#include <sal/core/libc.h>
#include <sal/appl/sal.h>
#include <sal/appl/io.h>
#include <sal/types.h>
#include <shared/bsl.h>
#include <shared/util.h>
#include <shared/error.h>

#include <soc/dpp/SAND/Utils/sand_dataio_utils.h>

void *dataio_alloc(unsigned int size)
{
    void *mem;
    if((mem = sal_alloc(size, "bcmutils")) != NULL)
        sal_memset(mem, 0, size);

    return mem;
}

void dataio_fillstr(char *str, int num, char ch)
{
    int i;
    for(i = 0; i < num; i++)
        str[i] = ch;

    str[num] = 0;
}

char **dataio_split_mstr(char *string, char *delim, uint32 maxtokens, uint32 *realtokens)
{
    char **tokens = NULL;
    int i = 0;
    char *next;

    if(ISEMPTY(string))
         goto out;

    tokens = sal_alloc(sizeof(char *) * maxtokens, "tokens");
    if(tokens == NULL)
        goto out;

    memset(tokens, 0, sizeof(char *) * maxtokens);

    for(i = 0; i < maxtokens; i++)
        tokens[i] = NULL;

    i = 0;
    while(((next = strstr(string, delim)) != NULL) && (i < (maxtokens - 1))) {
        tokens[i] = sal_alloc(sal_strlen(string) + 1, "token");
        sal_memset(tokens[i], 0, sal_strlen(string) + 1);
        /* Copy strictly number of characters until next */
        sal_memcpy(tokens[i++], string, next - string);
        /* move string to after the delimiter */
        string = next + sal_strlen(delim);
    }
    /* Once end of line or maxtokens achieved - string will keep the last part */
    tokens[i] = sal_alloc(sal_strlen(string) + 1, "token");
    sal_memset(tokens[i], 0, sal_strlen(string) + 1);
    strcpy(tokens[i++], string);

out:
    *realtokens = i;
    return tokens;
}

void dataio_free_tokens(char **tokens, uint32 token_num)
{
    int i = 0;

    for(i = 0; i < token_num; i++)
        sal_free(tokens[i]);

    sal_free(tokens);
}

void dataio_swap_long(uint8 *buffer, int uint32_num)
{
    uint32 tmp_value;
    uint8 *tmp_array = (uint8 *)(&tmp_value);
    int i;

    for(i = 0; i < uint32_num; i++) {
        tmp_value = *((uint32 *)buffer);
        buffer[0] = tmp_array[3];
        buffer[1] = tmp_array[2];
        buffer[2] = tmp_array[1];
        buffer[3] = tmp_array[0];
        buffer += 4;
    }
}

int dataio_pow(int n, int k)
{
    int i, result = 1;
    for(i = 0; i < k; i++)
        result *= n;

    return result;
}

void dataio_replace_white_space(char *dest, char *source)
{
    int i;
    for(i = 0; i < sal_strlen(source); i++) {
        if(source[i] == ' ')
            dest[i] = '_';
        else
            dest[i] = source[i];
    }
    dest[i] = 0;
    return;
}

uint32 dataio_stoul(char *str, uint32 *value_p)
{
    uint32 res = _SHR_E_NONE;
    int i;
    char ch;

    for(i = 0; i < strlen(str); i++) {
        ch = str[i];
        if(((ch >= '0') && (ch <= '9')) || ((ch >= 'a') && (ch <= 'f')) || ((ch >= 'A') && (ch <= 'F')))
            continue;
        else
            return _SHR_E_PARAM;
    }

    *value_p = _shr_ctoi(str);

    return res;
}

#if !defined(__KERNEL__)
FILETYPE_E dataio_get_filetype(char *file_n)
{
    char **tokens;
    uint32 realtokens = 0;
    FILETYPE_E ret = FILETYPE_MAX;

    if((tokens = dataio_split_mstr(file_n, ".", 4, &realtokens)) == NULL) {
       return ret;
    }

    switch(realtokens) {
        case 1:
            ret = FILETYPE_NONE;
            break;
        case 2:
            if(!sal_strcmp(tokens[1], "csv") || !sal_strcmp(tokens[1], "txt"))
                ret = FILETYPE_CSV;
            else if(!sal_strcmp(tokens[1], "xml"))
                ret = FILETYPE_XML;
            else if(!sal_strcmp(tokens[1], "v"))
                ret = FILETYPE_VERILOG;
            else
                ret = FILETYPE_UKNOWN;
            break;
        default:
            ret = FILETYPE_AMB;
            break;
    }

    dataio_free_tokens(tokens, realtokens);
    return ret;
}

int dataio_get_filepath(char *dir, char *name, char *filepath)
{
    int length = 6;

    if(!ISEMPTY(dir))
        length += sal_strlen(dir);

    if(!ISEMPTY(name))
        length += sal_strlen(name);
    else /* name cannot be empty */
        return _SHR_E_PARAM;

    if(length >= RHFILE_MAX_SIZE)
        return _SHR_E_PARAM;

    if(dir != NULL) {
        sal_strcpy(filepath, dir);
        sal_strcat(filepath, "/");
    }
    else /* Make it NULL string */
        filepath[0] = 0;

    sal_strcat(filepath, name);
    if(dataio_get_filetype(name) != FILETYPE_XML)
        /* Add xml extension if there no one already */
        strcat(filepath, ".xml");
    return _SHR_E_NONE;
}

uint32 dataio_dir_exists(char *path)
{
    uint32 res = _SHR_E_NONE;
    SAL_DIR* dir;

    dir = sal_opendir(path);
    if (!dir) {
        /* No such directory*/
        res = _SHR_E_EXISTS;
        goto out;
    }

    sal_closedir(dir);
out:
    return res;
}

uint32 dataio_file_exists(char *filename)
{
    uint32 res = _SHR_E_NONE;
    char filepath[RHFILE_MAX_SIZE];
    FILE *in;

    if((res = dataio_get_filepath(NULL, filename, filepath)) != _SHR_E_NONE)
        goto out;

    in = sal_fopen(filepath, "r");
    if (!in) {
        /* No such file - not an error - just warning */
        res = _SHR_E_EXISTS;
        goto out;
    }

    sal_fclose(in);

out:
    return res;
}
#endif /* !defined(__KERNEL__) */
