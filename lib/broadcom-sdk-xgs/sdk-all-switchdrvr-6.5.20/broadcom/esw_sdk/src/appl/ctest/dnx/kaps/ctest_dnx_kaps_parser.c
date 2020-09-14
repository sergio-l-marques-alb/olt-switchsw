
#include "ctest_dnx_kaps_parser.h"
#include "../libs/kaps/include/kaps_errors.h"
#include "../libs/kaps/include/kaps_portable.h"

#include <arpa/inet.h>
#include <netdb.h>

void
ctest_kaps_compute_mask(
    int32_t width,
    int32_t mask_len,
    int32_t eindex,
    struct ctest_kaps_parse_record *entry)
{
    int32_t i;

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

kaps_status
ctest_kaps_parse_prefix(
    const char *fs,
    struct ctest_kaps_parse_record *entry,
    int32_t width,
    int32_t eindex,
    int32_t * len)
{
    int32_t i, index;
    char ip[1024];
    char *temp;
    struct in6_addr addr;
    int32_t mask_len = 0;
    uint32_t count = 0;

    if (strstr(fs, " "))
    {
        temp = strstr(fs, " ");
        strncpy(ip, fs, temp - fs);
        /*
         * Say the length of fs is 30 and the number of bytes to copy is 15, then \0 will not be placed by strncpy in
         * ip. So we are explicitly placing the \0
         */
        ip[temp - fs] = 0;
    }
    else
    {
        strcpy(ip, fs);
    }

    i = index = eindex;
    *len = strlen(ip);

    if (strstr(ip, ":"))
    {
        char *p = strrchr(ip, '/');

        *p = 0;
        mask_len = atoi(p + 1);
        if (inet_pton(AF_INET6, ip, &addr) != 1)
        {
            kaps_printf("could not parse IP %s \n", ip);
            exit(1);
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
            uint32_t byte = 0;

            count++;
            if (strstr(token, "/"))
            {
                char *p = strrchr(token, '/');

                *p = 0;
                mask_len = atoi(p + 1);
                if (mask_len > width)
                    return KAPS_PARSE_ERROR;
            }
            byte = atoi(token);
            if (byte > 0xFF)
                return KAPS_PARSE_ERROR;
            entry->data[i++] = byte;
        }
    }

    if (count > (width / KAPS_BITS_IN_BYTE))
        return KAPS_PARSE_ERROR;

    ctest_kaps_compute_mask(width / KAPS_BITS_IN_BYTE, mask_len, index, entry);
    return KAPS_OK;
}

uint32_t
ctest_kaps_get_length_of_unmask_data(
    uint8_t * data,
    uint8_t num_bytes)
{
    uint32_t len = 0;
    uint32_t i = 0;

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

static kaps_status
ctest_kaps_parse_hex_bytes(
    const char *buf,
    const char *format,
    uint8_t * bytes)
{
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
                return KAPS_INTERNAL_ERROR;
            if (buf[1] >= '0' && buf[1] <= '9')
                bytes[0] += buf[1] - '0';
            else if (buf[1] >= 'a' && buf[1] <= 'f')
                bytes[0] += buf[1] - 'a' + 10;
            else if (buf[1] >= 'A' && buf[1] <= 'F')
                bytes[0] += buf[1] - 'A' + 10;
            else
                return KAPS_INTERNAL_ERROR;
            buf += 2;
            bytes++;
        }
        else if (format[0] == buf[0])
            buf++;
        else
            return KAPS_INTERNAL_ERROR;
    return KAPS_OK;
}

kaps_status
ctest_kaps_kaps_parse_entry(
    enum kaps_db_type db_type,
    const char *buf,
    struct ctest_kaps_parse_key * key,
    struct ctest_kaps_parse_record * entry,
    uint32_t key_width_1,
    int32_t flag_colon_notation,
    int32_t flag_user_notation)
{
    int width, eindex, rindex;
    const char *fs;
    int i;

    fs = buf;

    eindex = rindex = 0;
    for (; key; key = key->next)
    {
        while (isspace(*fs))
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
                            KAPS_TRY(ctest_kaps_parse_hex_bytes(fs, "b", entry->data + eindex));
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
                                KAPS_TRY(ctest_kaps_parse_hex_bytes(fs, "b", entry->data + eindex));
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
                        int32_t len = 0;

                        KAPS_TRY(ctest_kaps_parse_prefix(fs, entry, width, eindex, &len));
                        fs += len;
                    }
                    else
                    {
                        i = eindex;
                        while (width > 0)
                        {
                            if (width > 8)
                            {
                                KAPS_TRY(ctest_kaps_parse_hex_bytes(fs, "b.", entry->data + i));
                                fs += 3;
                            }
                            else
                            {
                                KAPS_TRY(ctest_kaps_parse_hex_bytes(fs, "b/", entry->data + i));
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
                                KAPS_TRY(ctest_kaps_parse_hex_bytes(fs, "b.", entry->mask + i));
                                fs += 3;
                            }
                            else
                            {
                                KAPS_TRY(ctest_kaps_parse_hex_bytes(fs, "b", entry->mask + i));
                                fs += 2;
                            }
                            width -= 8;
                            ++i;
                        }
                    }

                    eindex += (key->width / 8);
                    break;
                default:
                    return KAPS_INVALID_FORMAT;
            }
        }
    }

    entry->length = ctest_kaps_get_length_of_unmask_data(entry->mask, key_width_1 / KAPS_BITS_IN_BYTE);

    return KAPS_OK;
}

int32_t
ctest_kaps_scan_key(
    const char *fname,
    uint32_t line,
    char *buf,
    struct ctest_kaps_parse_key ** key)
{
    char *token;
    struct ctest_kaps_parse_key *head, *tail, *tmp;
    head = tail = NULL;

    for (token = kaps_strsep(&buf, "()"); token; token = kaps_strsep(&buf, "()"))
    {
        char *innertok;

        while (isspace(*token))
            token++;
        if (*token == '\0')
            continue;
        innertok = kaps_strsep(&token, ",");
        while (isspace(*innertok))
            innertok++;
        if (*innertok == '\0')
        {
            kaps_printf("%s:%d unable to parse key specification\n", fname, line);
            exit(1);
        }
        tmp = kaps_sysmalloc(sizeof(*tmp) + strlen(innertok) + 1);
        if (!tmp)
        {
            kaps_printf("Out of memory parsing key\n");
            exit(1);
        }
        tmp->next = NULL;
        tmp->fname = tmp->buf;
        strcpy(tmp->fname, innertok);
        innertok = kaps_strsep(&token, ",");
        while (isspace(*innertok))
            innertok++;
        if (*innertok == '\0')
        {
            kaps_printf("%s:%d unable to parse key specification\n", fname, line);
            exit(1);
        }
        tmp->width = atoi(innertok);
        innertok = kaps_strsep(&token, ",");
        while (isspace(*innertok))
            innertok++;
        if (*innertok == '\0')
        {
            kaps_printf("%s:%d unable to parse key specification\n", fname, line);
            exit(1);
        }

        if (strcmp(innertok, "ternary") == 0)
            tmp->type = KAPS_KEY_FIELD_TERNARY;
        else if (strcmp(innertok, "em") == 0)
            tmp->type = KAPS_KEY_FIELD_EM;
        else if (strcmp(innertok, "prefix") == 0)
            tmp->type = KAPS_KEY_FIELD_PREFIX;
        else if (strcmp(innertok, "range") == 0)
            tmp->type = KAPS_KEY_FIELD_RANGE;
        else if (strcmp(innertok, "tid") == 0)
            tmp->type = KAPS_KEY_FIELD_TABLE_ID;
        else if (strcmp(innertok, "priority") == 0)
        {
            tmp->type = KAPS_KEY_FIELD_EM;
        }
        else
        {
            kaps_printf("%s:%d invalid key field type %s\n", fname, line, innertok);
            exit(1);
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
    return 0;
}

kaps_status
ctest_kaps_parse_standard_db_file(
    enum kaps_db_type db_type,
    const char *fname,
    struct ctest_kaps_parse_record ** result,
    uint32_t req_num_entries,
    uint32_t * num_lines,
    uint32_t * num_entries,
    struct ctest_kaps_parse_key ** key)
{
    FILE *fp;
    int line_no, cnt = 0;
    char buf[4096];
    kaps_status status;
    struct ctest_kaps_parse_record *entries;
    long file_pos;
    uint32_t key_width_1;
    struct ctest_kaps_parse_key *tmp, *key_layout;
    uint8_t *data_mask = NULL, *ptr = NULL;
    int32_t i = 0, step = 0, nlines = 0;
    int32_t flag_colon_notation = 0;
    int32_t flag_user_notation = 0;

    fp = kaps_fopen(fname, "r");
    if (!fp)
        return KAPS_INVALID_FILE;

    /*
     * Lets count the number of lines in the file. typically
     * each line is an ACL entry. Its a conservative estimate
     * to prevent re-allocation of entries
     */

    if (fgets(buf, sizeof(buf), fp) == NULL)
    {
        kaps_fclose(fp);
        return KAPS_INVALID_FILE;
    }
    while (buf[0] == '\n')
    {
        fgets(buf, sizeof(buf), fp);
    }
    if ((strncmp(buf, "+KBP-STANDARD-FORMAT", strlen("+KBP-STANDARD-FORMAT")) != 0) &&
        (strncmp(buf, "+KBP-STANDARD-FORMAT-1", strlen("+KBP-STANDARD-FORMAT-1")) != 0) &&
        (strncmp(buf, "+KBP-STANDARD-FORMAT-2", strlen("+KBP-STANDARD-FORMAT-2")) != 0))
    {
        kaps_fclose(fp);
        return KAPS_INVALID_FILE;
    }
    if ((strncmp(buf, "+KBP-STANDARD-FORMAT-1", strlen("+KBP-STANDARD-FORMAT-1")) == 0))
        flag_colon_notation = 1;
    if ((strncmp(buf, "+KBP-STANDARD-FORMAT-2", strlen("+KBP-STANDARD-FORMAT-2")) == 0))
    {
        flag_user_notation = 1;
    }

    /*
     * It is in standard format, scan the key specification line
     */

    if (fgets(buf, sizeof(buf), fp) == NULL)
    {
        kaps_fclose(fp);
        return KAPS_INVALID_FILE;
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
    while (fgets(buf, sizeof(buf), fp) != NULL);

    if (ctest_kaps_scan_key(fname, cnt, buf, key))
    {
        kaps_fclose(fp);
        return KAPS_INVALID_FILE;
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
        return KAPS_INVALID_FILE;
    }

    while (!feof(fp))
    {
        if (fgets(buf, sizeof(buf), fp) == NULL)
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
        return KAPS_OK;
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
        return KAPS_OUT_OF_MEMORY;
    }

    kaps_sassert(req_num_entries);
    kaps_memset(entries, 0, (req_num_entries) * sizeof(struct ctest_kaps_parse_record));
    data_mask = kaps_syscalloc(1, 2 * (req_num_entries) * (key_width_1 / KAPS_BITS_IN_BYTE));
    step = key_width_1 / KAPS_BITS_IN_BYTE;
    if (data_mask == NULL)
    {
        kaps_sysfree(entries);
        kaps_fclose(fp);
        return KAPS_OUT_OF_MEMORY;
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
        if (fgets(buf, sizeof(buf), fp) == NULL)
            break;
        if (buf[0] == '#')
            continue;
        /*
         * skip empty lines and indented comments 
         */
        ptr = buf;
        while (isspace(*ptr))
            ptr++;
        if (*ptr == '\0' || *ptr == '#')
            continue;

        status =
            ctest_kaps_kaps_parse_entry(db_type, ptr, key_layout, &entries[cnt], key_width_1, flag_colon_notation,
                                        flag_user_notation);
        if (status != KAPS_OK)
        {
            kaps_printf("Error parsing line %d: %s\n", line_no, buf);
            fflush(stdout);
            kaps_sysfree(entries);
            kaps_fclose(fp);
            return status;
        }
        cnt++;
    }
    kaps_fclose(fp);
    *result = entries;
    *num_entries = cnt;
    return KAPS_OK;
}
