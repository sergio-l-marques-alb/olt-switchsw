/** \file ctest_dnx_dataset_file_parser.c
 *
 * parsing prefixes from the dataset file
 *
 * the function ctest_dnx_parse_standard_dataset is used to parse prefixes from dataset
 *
 * the first few lines of the dataset file looks like
 *
 * +KBP-STANDARD-FORMAT-1
 * (VPN, 16, em)(SIP, 32, prefix)
 * 0001 0.0.0.0/0
 * 0001 1.0.4.0/22
 * 0001 1.0.4.0/24
 *
 *
 * the function ctest_dnx_parse_standard_dataset takes the filename along with path,
 * requested number of entries to parse as input. It gives the parsed entries, the number
 * of entries parsed, number of lines in the file and the Key. In the example above,
 * the fields VPN and SIP together form the Key
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_KAPSDNX

#include <soc/dnx/mdb.h>

#include "ctest_dnx_dataset_file_parser.h"
#include "ctest_dnx_kaps.h"
#include "kaps_errors.h"
#include "kaps_portable.h"

#include <arpa/inet.h>
#include <netdb.h>

static void
ctest_kaps_compute_mask(
    int32 width,
    int32 mask_len,
    int32 eindex,
    struct ctest_kaps_parse_record *entry)
{
    int32 i;

    mask_len = (width * KAPS_BITS_IN_BYTE) - mask_len;
    for (i = 0; i < width; i++)
    {
        entry->mask[eindex + i] = 0x00;
    }

    for (i = width - ((mask_len + 7) / 8); i < width; i++)
    {
        if (!mask_len)
            entry->mask[eindex + i] = 0x00;
        else if (!(mask_len % 8))
            entry->mask[eindex + i] = 0xFF;
        else
        {
            entry->mask[eindex + i] = 0xFF >> (8 - (mask_len % 8));
            mask_len -= mask_len % 8;
            continue;
        }
        if (mask_len)
            mask_len -= 8;
    }
}

static shr_error_e
ctest_kaps_parse_prefix(
    int unit,
    const char *fs,
    struct ctest_kaps_parse_record *entry,
    int32 width,
    int32 eindex,
    int32 *len)
{
    int32 i, index;
    char ip[1024];
    char *temp;
    struct in6_addr addr;
    int32 mask_len = 0;
    uint32 count = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (sal_strnstr(fs, " ", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH))
    {
        temp = sal_strnstr(fs, " ", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
        sal_strlcpy(ip, fs, temp - fs);
        /*
         * Say the length of fs is 30 and the number of bytes to copy is 15, then \0 will not be placed by strncpy in
         * ip. So we are explicitly placing the \0
         */
    }
    else
    {
        sal_strlcpy(ip, fs, sizeof(ip));
    }

    i = index = eindex;
    *len = sal_strnlen(ip, sizeof(ip));

    if (sal_strnstr(ip, ":", sizeof(ip)))
    {
        char *p = sal_strnrchr(ip, '/', sizeof(ip));

        *p = 0;
        mask_len = sal_atoi(p + 1);
        if (inet_pton(AF_INET6, ip, &addr) != 1)
        {
            LOG_CLI((BSL_META("could not parse IP %s using inet_pton \n"), ip));
            SHR_ERR_EXIT(_SHR_E_FAIL, "Error parsing IP address");
        }

        for (i = 0; i < 16; i++, eindex++)
        {
            entry->data[eindex] = addr.s6_addr[i];
        }
    }
    else
    {
        char *token;
        char *buf = ip;

        for (token = kaps_strsep(&buf, "."); token; token = kaps_strsep(&buf, "."))
        {
            uint32 byte = 0;

            count++;
            if (sal_strnstr(token, "/", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH))
            {
                char *p = sal_strnrchr(token, '/', SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);

                *p = 0;
                mask_len = sal_atoi(p + 1);
                if (mask_len > width)
                {
                    SHR_ERR_EXIT(_SHR_E_FAIL, "Error parsing IP address: Mask Length greater than width \n");
                }
            }

            byte = sal_atoi(token);
            if (byte > 0xFF)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "Error parsing IP address: The parsed byte is greater than 255 ");
            }

            entry->data[i++] = byte;
        }
    }

    if (count > (width / KAPS_BITS_IN_BYTE))
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Error parsing IP address: Number of bytes exceeded ");
    }

    ctest_kaps_compute_mask(width / KAPS_BITS_IN_BYTE, mask_len, index, entry);

exit:
    SHR_FUNC_EXIT;
}

uint32
ctest_kaps_get_length_of_unmask_data(
    uint8_t * data,
    uint8_t num_bytes)
{
    uint32 len = 0;
    uint32 i = 0;

    for (i = 0; i < num_bytes; i++)
    {
        if (data[i] == 0)
            len += 8;
        else
            break;
    }

    if (i < num_bytes)
    {
        uint8_t byte = data[i];
        uint8_t num_of_ones = 0;

        while (byte)
        {
            num_of_ones++;
            byte >>= 1;
        }

        len += (8 - num_of_ones);
    }

    return len;
}

static shr_error_e
ctest_kaps_parse_hex_bytes(
    int unit,
    const char *buf,
    const char *format,
    uint8_t * bytes)
{
    SHR_FUNC_INIT_VARS(unit);

    for (; format[0]; format++)
        if (format[0] == 'b')
        {
            if (buf[0] >= '0' && buf[0] <= '9')
                bytes[0] = (buf[0] - '0') * 16;
            else if (buf[0] >= 'a' && buf[0] <= 'f')
                bytes[0] = (buf[0] - 'a' + 10) * 16;
            else if (buf[0] >= 'A' && buf[0] <= 'F')
                bytes[0] = (buf[0] - 'A' + 10) * 16;
            else
                SHR_ERR_EXIT(_SHR_E_FAIL, "Incorrect Hex character");

            if (buf[1] >= '0' && buf[1] <= '9')
                bytes[0] += buf[1] - '0';
            else if (buf[1] >= 'a' && buf[1] <= 'f')
                bytes[0] += buf[1] - 'a' + 10;
            else if (buf[1] >= 'A' && buf[1] <= 'F')
                bytes[0] += buf[1] - 'A' + 10;
            else
                SHR_ERR_EXIT(_SHR_E_FAIL, "Incorrect Hex character");

            buf += 2;
            bytes++;
        }
        else if (format[0] == buf[0])
            buf++;
        else
            SHR_ERR_EXIT(_SHR_E_FAIL, "Error while parsing Hex bytes");

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_kaps_parse_entry(
    int unit,
    enum kaps_db_type db_type,
    const char *buf,
    struct ctest_kaps_parse_key *key,
    struct ctest_kaps_parse_record *entry,
    uint32 key_width_1,
    int32 flag_colon_notation,
    int32 flag_user_notation)
{
    int width, eindex, rindex;
    const char *fs;
    int i;

    SHR_FUNC_INIT_VARS(unit);

    fs = buf;

    eindex = rindex = 0;
    for (; key; key = key->next)
    {
        while (sal_isspace(*fs))
            fs++;

        {
            switch (key->type)
            {
                case KAPS_KEY_FIELD_TABLE_ID:
                    /*
                     * EM field 
                     */
                    width = key->width;
                    kaps_sassert(width >= 8);
                    while (width)
                    {
                        if (fs[0] == '*' && fs[1] == '*')
                        {
                            entry->data[eindex] = 0;
                            entry->mask[eindex] = 0xFF;
                        }
                        else
                        {
                            DNX_KAPS_TRY(ctest_kaps_parse_hex_bytes(unit, fs, "b", entry->data + eindex));
                            entry->mask[eindex] = 0;
                        }
                        width -= 8;
                        kaps_sassert(width >= 0);
                        eindex++;
                        fs += 2;
                    }
                    break;

                case KAPS_KEY_FIELD_EM:
                    /*
                     * EM field 
                     */
                    width = key->width;
                    kaps_sassert(width >= 8);
                    {
                        while (width)
                        {
                            if (fs[0] == '*' && fs[1] == '*')
                            {
                                entry->data[eindex] = 0;
                                entry->mask[eindex] = 0xFF;
                            }
                            else
                            {
                                DNX_KAPS_TRY(ctest_kaps_parse_hex_bytes(unit, fs, "b", entry->data + eindex));
                                entry->mask[eindex] = 0;
                            }
                            width -= 8;
                            kaps_sassert(width >= 0);
                            eindex++;
                            fs += 2;
                        }
                    }
                    break;

                case KAPS_KEY_FIELD_PREFIX:
                    width = key->width;
                    kaps_sassert(width % 8 == 0);
                    if (flag_colon_notation)
                    {
                        int32 len = 0;

                        DNX_KAPS_TRY(ctest_kaps_parse_prefix(unit, fs, entry, width, eindex, &len));
                        fs += len;
                    }
                    else
                    {
                        i = eindex;
                        while (width > 0)
                        {
                            if (width > 8)
                            {
                                DNX_KAPS_TRY(ctest_kaps_parse_hex_bytes(unit, fs, "b.", entry->data + i));
                                fs += 3;
                            }
                            else
                            {
                                DNX_KAPS_TRY(ctest_kaps_parse_hex_bytes(unit, fs, "b/", entry->data + i));
                                fs += 3;
                            }
                            width -= 8;
                            ++i;
                        }
                        width = key->width;
                        i = eindex;
                        while (width > 0)
                        {
                            if (width > 8)
                            {
                                DNX_KAPS_TRY(ctest_kaps_parse_hex_bytes(unit, fs, "b.", entry->mask + i));
                                fs += 3;
                            }
                            else
                            {
                                DNX_KAPS_TRY(ctest_kaps_parse_hex_bytes(unit, fs, "b", entry->mask + i));
                                fs += 2;
                            }
                            width -= 8;
                            ++i;
                        }
                    }

                    eindex += (key->width / 8);
                    break;

                default:
                    SHR_ERR_EXIT(_SHR_E_FAIL, "Incorrect Key Field during parsing");
            }
        }
    }

    entry->length = ctest_kaps_get_length_of_unmask_data(entry->mask, key_width_1 / KAPS_BITS_IN_BYTE);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
ctest_kaps_scan_key(
    int unit,
    const char *fname,
    uint32 line,
    char *buf,
    struct ctest_kaps_parse_key **key)
{
    char *token;
    struct ctest_kaps_parse_key *head, *tail, *tmp;

    SHR_FUNC_INIT_VARS(unit);

    head = tail = NULL;

    for (token = kaps_strsep(&buf, "()"); token; token = kaps_strsep(&buf, "()"))
    {
        char *innertok;

        while (sal_isspace(*token))
            token++;

        if (*token == '\0')
            continue;

        innertok = kaps_strsep(&token, ",");
        while (sal_isspace(*innertok))
            innertok++;

        if (*innertok == '\0')
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "%s:%d unable to parse key specification\n", fname, line);
        }

        tmp = kaps_sysmalloc(sizeof(*tmp) + sal_strnlen(innertok, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) + 1);
        if (!tmp)
        {
            SHR_ERR_EXIT(_SHR_E_MEMORY, "Out of memory parsing key\n");
        }
        tmp->next = NULL;

        tmp->fname = tmp->buf;
        sal_strlcpy(tmp->fname, innertok, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
        innertok = kaps_strsep(&token, ",");

        while (sal_isspace(*innertok))
            innertok++;

        if (*innertok == '\0')
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "%s:%d unable to parse key specification\n", fname, line);
        }
        tmp->width = sal_atoi(innertok);

        innertok = kaps_strsep(&token, ",");
        while (sal_isspace(*innertok))
            innertok++;

        if (*innertok == '\0')
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "%s:%d unable to parse key specification\n", fname, line);
        }

        if (sal_strncmp(innertok, "ternary", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
            tmp->type = KAPS_KEY_FIELD_TERNARY;
        else if (sal_strncmp(innertok, "em", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
            tmp->type = KAPS_KEY_FIELD_EM;
        else if (sal_strncmp(innertok, "prefix", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
            tmp->type = KAPS_KEY_FIELD_PREFIX;
        else if (sal_strncmp(innertok, "range", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
            tmp->type = KAPS_KEY_FIELD_RANGE;
        else if (sal_strncmp(innertok, "tid", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
            tmp->type = KAPS_KEY_FIELD_TABLE_ID;
        else if (sal_strncmp(innertok, "priority", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
        {
            tmp->type = KAPS_KEY_FIELD_EM;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "%s:%d invalid key field type %s\n", fname, line, innertok);
        }

        if (head == NULL)
        {
            head = tail = tmp;
        }
        else
        {
            tail->next = tmp;
            tail = tmp;
        }
    }

    *key = head;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
ctest_dnx_parse_standard_dataset(
    int unit,
    enum kaps_db_type db_type,
    const char *fname,
    struct ctest_kaps_parse_record **result,
    uint32 req_num_entries,
    uint32 *num_lines,
    uint32 *num_entries,
    struct ctest_kaps_parse_key **key)
{
    FILE *fp;
    int line_no, cnt = 0;
    char buf[4096];
    shr_error_e shr_status;
    struct ctest_kaps_parse_record *entries;
    long file_pos;
    uint32 key_width_1;
    struct ctest_kaps_parse_key *tmp, *key_layout;
    uint8_t *data_mask = NULL, *ptr = NULL;
    int32 i = 0, step = 0, nlines = 0;
    int32 flag_colon_notation = 0;
    int32 flag_user_notation = 0;

    SHR_FUNC_INIT_VARS(unit);

    fp = kaps_fopen(fname, "r");
    if (!fp)
        SHR_ERR_EXIT(_SHR_E_FAIL, "Unable to open prefix file for parsing");

    /*
     * Lets count the number of lines in the file. typically
     * each line is a prefix. Its a conservative estimate
     * to prevent re-allocation of entries
     */

    if (sal_fgets(buf, sizeof(buf), fp) == NULL)
    {
        kaps_fclose(fp);
        SHR_ERR_EXIT(_SHR_E_FAIL, "Unable to get string from prefix file");
    }

    while (buf[0] == '\n')
    {
        sal_fgets(buf, sizeof(buf), fp);
    }

    if ((strncmp
         (buf, "+KBP-STANDARD-FORMAT",
          sal_strnlen("+KBP-STANDARD-FORMAT", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH)) != 0)
        &&
        (strncmp
         (buf, "+KBP-STANDARD-FORMAT-1",
          sal_strnlen("+KBP-STANDARD-FORMAT-1", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH)) != 0)
        &&
        (strncmp
         (buf, "+KBP-STANDARD-FORMAT-2",
          sal_strnlen("+KBP-STANDARD-FORMAT-2", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH)) != 0))
    {
        kaps_fclose(fp);
        SHR_ERR_EXIT(_SHR_E_FAIL, "The format of the prefix file is not supported");
    }

    if ((strncmp
         (buf, "+KBP-STANDARD-FORMAT-1",
          sal_strnlen("+KBP-STANDARD-FORMAT-1", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH)) == 0))
        flag_colon_notation = 1;

    if ((strncmp
         (buf, "+KBP-STANDARD-FORMAT-2",
          sal_strnlen("+KBP-STANDARD-FORMAT-2", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH)) == 0))
    {
        flag_user_notation = 1;
    }

    /*
     * It is in standard format, scan the key specification line
     */

    if (sal_fgets(buf, sizeof(buf), fp) == NULL)
    {
        kaps_fclose(fp);
        SHR_ERR_EXIT(_SHR_E_FAIL, "Unable to get string from prefix file");
    }

    /*
     * skip comments
     */
    do
    {
        if (*buf == '#')
        {
            cnt++;
            continue;
        }
        break;
    }
    while (sal_fgets(buf, sizeof(buf), fp) != NULL);

    shr_status = ctest_kaps_scan_key(unit, fname, cnt, buf, key);
    if (shr_status != _SHR_E_NONE)
    {
        kaps_fclose(fp);
        SHR_ERR_EXIT(shr_status, "Error while parsing key from prefix file");
    }

    key_layout = *key;
    tmp = *key;
    key_width_1 = 0;
    while (tmp)
    {
        key_width_1 += tmp->width;
        tmp = tmp->next;
    }

    file_pos = ftell(fp);
    if (-1 == file_pos)
    {
        kaps_fclose(fp);
        SHR_ERR_EXIT(_SHR_E_FAIL, "Error while using ftell on file ");
    }

    while (!feof(fp))
    {
        if (sal_fgets(buf, sizeof(buf), fp) == NULL)
        {
            break;
        }
        nlines++;
    }

    if (!nlines)
    {
        *num_lines = 0;
        *num_entries = 0;
        *result = NULL;
        kaps_fclose(fp);
        return _SHR_E_NONE;
    }

    *num_lines = nlines;
    /*
     * If we specify req_num_entries as zero, we take num_lines as req_num_entries.
     */
    if (req_num_entries)
    {
        if (nlines <= req_num_entries)
            req_num_entries = nlines;
    }
    else
    {
        req_num_entries = nlines;
    }

    (void) kaps_fseek(fp, file_pos, SEEK_SET);

    entries =
        (struct ctest_kaps_parse_record *) kaps_sysmalloc((req_num_entries) * sizeof(struct ctest_kaps_parse_record));
    if (!entries)
    {
        kaps_fclose(fp);
        SHR_ERR_EXIT(_SHR_E_MEMORY, "Memory allocation failed to stored parsed entries ");
    }

    kaps_sassert(req_num_entries);

    kaps_memset(entries, 0, (req_num_entries) * sizeof(struct ctest_kaps_parse_record));

    data_mask = kaps_syscalloc(1, 2 * (req_num_entries) * (key_width_1 / KAPS_BITS_IN_BYTE));
    step = key_width_1 / KAPS_BITS_IN_BYTE;
    if (data_mask == NULL)
    {
        kaps_sysfree(entries);
        kaps_fclose(fp);
        SHR_ERR_EXIT(_SHR_E_MEMORY, "Memory allocation failed to stored data-mask of parsed entries ");
    }

    ptr = data_mask;

    for (i = 0; i < (req_num_entries); i++)
    {
        entries[i].data = ptr;
        ptr += step;
        entries[i].mask = ptr;

        kaps_memset(ptr, -1, step);
        ptr += step;
    }

    cnt = 0;

    line_no = 0;
    while ((cnt < req_num_entries) && !feof(fp))
    {
        const char *ptr;

        line_no++;
        if (sal_fgets(buf, sizeof(buf), fp) == NULL)
            break;
        if (buf[0] == '#')
            continue;
        /*
         * skip empty lines and indented comments 
         */
        ptr = buf;
        while (sal_isspace(*ptr))
            ptr++;
        if (*ptr == '\0' || *ptr == '#')
            continue;

        shr_status =
            ctest_kaps_parse_entry(unit, db_type, ptr, key_layout, &entries[cnt], key_width_1, flag_colon_notation,
                                   flag_user_notation);
        if (shr_status != _SHR_E_NONE)
        {
            LOG_CLI((BSL_META("Error parsing line %d: %s\n"), line_no, buf));
            sal_fflush(stdout);
            kaps_sysfree(entries);
            kaps_fclose(fp);
            SHR_ERR_EXIT(shr_status, "Error parsing prefix file");
        }
        cnt++;
    }
    kaps_fclose(fp);
    *result = entries;
    *num_entries = cnt;

exit:
    SHR_FUNC_EXIT;
}
