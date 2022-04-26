/** \file kaps_verifier_parse.c
 *
 * Ctests for KAPS
 *
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#include <arpa/inet.h>

#include "kaps_verifier_parse.h"
#include "kaps_key_internal.h"
#include "kaps_bitmap.h"

void
kaps_parse_destroy_key(
    struct kaps_parse_key *key)
{
    while (key)
    {
        struct kaps_parse_key *tmp = key->next;
        kaps_sysfree(key);
        key = tmp;
    }
}

void
kaps_parse_destroy(
    struct kaps_parse_record *entries)
{
    if (entries)
    {
        if (entries->user_ad_data)
            kaps_sysfree(entries->user_ad_data);
        kaps_sysfree(entries->data);
        kaps_sysfree(entries);
    }
}

void
kaps_parse_print_key(
    struct kaps_parse_key *key)
{
    kaps_printf("+KBP-STANDARD-FORMAT\n");
    for (; key; key = key->next)
    {
        kaps_printf("(%s,%d,%s) ", key->fname, key->width, kaps_key_get_type_internal(key->type));
    }
    kaps_printf("\n");
}

kaps_status
kaps_parse_print_bits(
    uint32_t nbits,
    const uint8_t * data,
    const uint8_t * mask)
{
    int i;
    char *bits[] = { "0", "1", "*" };

    for (i = 0; i < nbits; i++)
    {
        if (i && ((i % 640) == 0))
            kaps_printf("\n");
        if (i % 4 == 0)
            kaps_printf(" ");
        kaps_printf("%s", bits[kaps_array_get_bit(data, mask, i)]);
    }
    return KAPS_OK;
}

kaps_status
kaps_parse_print_record(
    struct kaps_parse_key * key,
    const struct kaps_parse_record * record)
{
    struct kaps_parse_key *tmp;
    uint32_t boffset = 0, rindex = 0, width, nbytes;
    int32_t i;

    if (key == NULL || record == NULL)
        return KAPS_INVALID_ARGUMENT;

    for (tmp = key; tmp; tmp = tmp->next)
    {
        switch (tmp->type)
        {
            case KAPS_KEY_FIELD_EM:
            case KAPS_KEY_FIELD_TABLE_ID:
                width = tmp->width;
                kaps_sassert(width >= 8);
                while (width)
                {
                    if (record->mask[boffset] == 0xFF)
                        kaps_printf("**");
                    else
                    {
                        kaps_printf("%02x", record->data[boffset]);
                    }
                    boffset++;
                    width -= 8;
                }
                kaps_printf(" ");
                break;
            case KAPS_KEY_FIELD_PREFIX:
                nbytes = tmp->width / KAPS_BITS_IN_BYTE;
                kaps_printf("%02x", record->data[boffset]);
                for (i = 1; i < nbytes; i++)
                {
                    kaps_printf(".%02x", record->data[boffset + i]);
                }
                kaps_printf("/");
                kaps_printf("%02x", record->mask[boffset]);
                for (i = 1; i < nbytes; i++)
                {
                    kaps_printf(".%02x", record->mask[boffset + i]);
                }
                boffset += nbytes;
                kaps_printf(" ");
                break;
            case KAPS_KEY_FIELD_RANGE:
                kaps_printf("%04x-%04x ", record->ranges[rindex].lo, record->ranges[rindex].hi);
                rindex++;
                boffset += 2;
                break;
            case KAPS_KEY_FIELD_TERNARY:
                kaps_parse_print_bits(tmp->width, (uint8_t *) & record->data[boffset],
                                      (uint8_t *) & record->mask[boffset]);
                boffset += (tmp->width / KAPS_BITS_IN_BYTE);
                kaps_printf(" ");
                break;
            default:
                return KAPS_INVALID_FORMAT;
        }
    }
    kaps_printf("\n");
    return KAPS_OK;
}

static kaps_status
parse_hex_bytes(
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

static kaps_status
parse_ternary_bits(
    const char *buf,
    const char *format,
    uint8_t * data,
    uint8_t * mask)
{
    int bit_pos;

    for (bit_pos = 0; format[0]; format++, buf++)
        if (format[0] == 't')
        {
            if (buf[0] == '0')
            {
                kaps_array_set_bit(data, bit_pos, 0);
                kaps_array_set_bit(mask, bit_pos, 0);
            }
            else if (buf[0] == '1')
            {
                kaps_array_set_bit(data, bit_pos, 1);
                kaps_array_set_bit(mask, bit_pos, 0);
            }
            else if (buf[0] == '*')
            {
                kaps_array_set_bit(data, bit_pos, 0);
                kaps_array_set_bit(mask, bit_pos, 1);
            }
            else
                return KAPS_INTERNAL_ERROR;
            bit_pos++;
        }
        else if (format[0] != buf[0])
            return KAPS_INTERNAL_ERROR;
    return KAPS_OK;
}

static void
compute_mask(
    int32_t width,
    int32_t mask_len,
    int32_t eindex,
    struct kaps_parse_record *entry)
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

static kaps_status
parse_prefix(
    const char *fs,
    struct kaps_parse_record *entry,
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

    if (sal_strnstr(fs, " ", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH))
    {
        temp = sal_strnstr(fs, " ", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
        sal_strncpy(ip, fs, temp - fs);
        /*
         * Say the length of fs is 30 and the number of bytes to copy is 15, then \0 will not be placed by strncpy in
         * ip. So we are explicitly placing the \0
         */
        ip[temp - fs] = 0;
    }
    else
    {
        sal_strncpy(ip, fs, sizeof(ip));
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
            if (sal_strnstr(token, "/", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH))
            {
                char *p = sal_strnrchr(token, '/', SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);

                *p = 0;
                mask_len = sal_atoi(p + 1);
                if (mask_len > width)
                    return KAPS_PARSE_ERROR;
            }
            byte = sal_atoi(token);
            if (byte > 0xFF)
                return KAPS_PARSE_ERROR;
            entry->data[i++] = byte;
        }
    }

    if (count > (width / KAPS_BITS_IN_BYTE))
        return KAPS_PARSE_ERROR;

    compute_mask(width / KAPS_BITS_IN_BYTE, mask_len, index, entry);
    return KAPS_OK;
}

static uint32_t
get_length_of_unmask_data(
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

kaps_status
kaps_parse_entry(
    enum kaps_db_type db_type,
    const char *buf,
    struct kaps_parse_key * key,
    struct kaps_parse_record * entry,
    uint32_t key_width_1,
    uint32_t acl_priority,
    int32_t flag_colon_notation,
    int32_t flag_user_notation,
    uint32_t ad_width,
    uint32_t em_with_mask)
{
    int width, eindex, rindex;
    const char *fs;
    int i;
    uint32_t xml_priority = 0;
    uint32_t xml_ad_data = 0;

    fs = buf;

    eindex = rindex = 0;
    for (; key; key = key->next)
    {
        while (sal_isspace(*fs))
            fs++;

        if (!sal_strncmp(key->fname, "PRIORITY", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) && flag_user_notation)
        {
            sscanf(fs, "%x", &xml_priority);
            fs += 6;
        }
        else if (!sal_strncmp(key->fname, "AD", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) && flag_user_notation)
        {
            sscanf(fs, "%x", &xml_ad_data);
            if (ad_width)
            {
                while (sal_isspace(*fs))
                    fs++;

                width = ad_width;
                i = 0;
                while (width)
                {
                    KAPS_TRY(parse_hex_bytes(fs, "b", entry->user_ad_data + i));
                    width -= 8;
                    kaps_sassert(width >= 0);
                    fs += 2;
                    i++;
                }
            }
        }
        else
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
                            KAPS_TRY(parse_hex_bytes(fs, "b", entry->data + eindex));
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
                    if (em_with_mask == 0)
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
                                KAPS_TRY(parse_hex_bytes(fs, "b", entry->data + eindex));
                                entry->mask[eindex] = 0;
                            }
                            width -= 8;
                            kaps_sassert(width >= 0);
                            eindex++;
                            fs += 2;
                        }
                    }
                    else
                    {
                        i = eindex;
                        while (width > 0)
                        {
                            if (width > 8)
                            {
                                KAPS_TRY(parse_hex_bytes(fs, "b.", entry->data + i));
                                fs += 3;
                            }
                            else
                            {
                                KAPS_TRY(parse_hex_bytes(fs, "b/", entry->data + i));
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
                                KAPS_TRY(parse_hex_bytes(fs, "b.", entry->mask + i));
                                fs += 3;
                            }
                            else
                            {
                                KAPS_TRY(parse_hex_bytes(fs, "b", entry->mask + i));
                                fs += 2;
                            }
                            width -= 8;
                            ++i;
                        }
                        eindex += (key->width / 8);
                    }
                    break;
                case KAPS_KEY_FIELD_PREFIX:
                    width = key->width;
                    kaps_sassert(width % 8 == 0);
                    if (flag_colon_notation)
                    {
                        int32_t len = 0;

                        KAPS_TRY(parse_prefix(fs, entry, width, eindex, &len));
                        fs += len;
                    }
                    else
                    {
                        i = eindex;
                        while (width > 0)
                        {
                            if (width > 8)
                            {
                                KAPS_TRY(parse_hex_bytes(fs, "b.", entry->data + i));
                                fs += 3;
                            }
                            else
                            {
                                KAPS_TRY(parse_hex_bytes(fs, "b/", entry->data + i));
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
                                KAPS_TRY(parse_hex_bytes(fs, "b.", entry->mask + i));
                                fs += 3;
                            }
                            else
                            {
                                KAPS_TRY(parse_hex_bytes(fs, "b", entry->mask + i));
                                fs += 2;
                            }
                            width -= 8;
                            ++i;
                        }
                    }

                    eindex += (key->width / 8);
                    break;
                case KAPS_KEY_FIELD_RANGE:     /* 16-bit port range */
                    if (rindex >= KAPS_HW_MAX_RANGE_COMPARES)
                        return KAPS_INTERNAL_ERROR;
                    entry->ranges[rindex].offset_8 = eindex;
                    if (sscanf(fs, "%4hx-%4hx", &entry->ranges[rindex].lo, &entry->ranges[rindex].hi) != 2)
                        return KAPS_INTERNAL_ERROR;
                    eindex += 2;
                    rindex += 1;
                    entry->num_ranges++;
                    fs += 9;
                    break;
                case KAPS_KEY_FIELD_TERNARY:
                    /*
                     * Ternary bits 
                     */
                    width = key->width;
                    kaps_sassert(width >= 8);
                    while (width)
                    {
                        while (sal_isspace(*fs))
                            fs++;
                        KAPS_TRY(parse_ternary_bits(fs, "tttt tttt", entry->data + eindex, entry->mask + eindex));
                        eindex += 1;
                        fs += 9;
                        width -= 8;
                        kaps_sassert(width >= 0);
                    }
                    break;
                default:
                    return KAPS_INVALID_FORMAT;
            }
        }
    }
    if (ad_width)
    {
        while (sal_isspace(*fs))
            fs++;

        width = ad_width;
        i = 0;
        while (width)
        {
            KAPS_TRY(parse_hex_bytes(fs, "b", entry->user_ad_data + i));
            width -= 8;
            kaps_sassert(width >= 0);
            fs += 2;
            i++;
        }
    }
    if (db_type == KAPS_DB_LPM)
    {
        entry->length = get_length_of_unmask_data(entry->mask, key_width_1 / KAPS_BITS_IN_BYTE);
        entry->priority = key_width_1 - entry->length;
    }
    else if (db_type == KAPS_DB_ACL)
    {
        if (flag_user_notation)
        {
            entry->priority = xml_priority;
        }
        else
        {
            if ((acl_priority + 1) > KAPS_HW_MINIMUM_PRIORITY)
                entry->priority = KAPS_HW_MINIMUM_PRIORITY;
            else
                entry->priority = acl_priority + 1;
        }
    }
    else
    {
        entry->priority = 0;
    }

    return KAPS_OK;
}

/*
 * create a mask for slash notation for IPv6
 */

static void
compute_mask_64(
    int ipmask,
    uint8_t * bits,
    uint8_t * mask,
    int *sindex)
{
    uint64_t mhi, mlo;
    uint64_t all_ones = ~((uint64_t) 0);
    int mindex = *sindex;
    int i;

    /*
     * Initialize mhi and mlo to have all bits set 
     */
    mhi = all_ones;
    mlo = all_ones;

    if (ipmask > 0)
    {
        if (ipmask <= 64)
        {
            /*
             * Modify mhi. mlo will be all ones 
             */
            if (ipmask == 64)
                mhi = 0;
            else
                mhi = all_ones >> ipmask;
        }
        else
        {
            /*
             * mhi will be all zeroes, modify mlo 
             */
            int rem = ipmask - 64;

            mhi = ((uint64_t) 0);
            if (rem == 64)
                mlo = 0;
            else
                mlo = all_ones >> rem;
        }
    }

    for (i = 0; i < 8; i++, mindex++)
    {
        /*
         * Store bytes of mhi from MS to LS 
         */
        mask[mindex] = (mhi >> (56 - i * 8)) & 0xFF;
        if (mask[mindex] == 0xFF)
            bits[mindex] = 0;
    }

    for (i = 0; i < 8; i++, mindex++)
    {
        /*
         * Store bytes of mlo from MS to LS 
         */
        mask[mindex] = (mlo >> (56 - i * 8)) & 0xFF;
        if (mask[mindex] == 0xFF)
            bits[mindex] = 0;
    }
    *sindex = mindex;
}

static void
compute_mask_32(
    int ipmask,
    uint8_t * mask,
    int *sindex)
{
    int mindex = *sindex;
    int i;
    uint32_t m = 0;

    for (i = 0; i < ipmask; i++)
        m |= 1u << (31 - i);
    m = ~m;

    mask[mindex++] = (m >> 24) & 0xFF;
    mask[mindex++] = (m >> 16) & 0xFF;
    mask[mindex++] = (m >> 8) & 0xFF;
    mask[mindex++] = m & 0xFF;
    *sindex = mindex;
}

static void
utils_set_range_in_key(
    uint8_t * key,
    uint32_t offset_8,
    uint16_t portval)
{
    key[offset_8] = (portval >> 8) & 0xFF;
    key[offset_8 + 1] = portval & 0xFF;
}

static void
compute_port(
    uint16_t min,
    uint16_t max,
    uint8_t * bits,
    int *sbindex,
    uint8_t * mask,
    int *smindex)
{
    int bindex = *sbindex;
    int mindex = *smindex;

    if (min == max)
    {
        utils_set_range_in_key(bits, bindex, min);

        bindex += 2;
        mask[mindex++] = 0;
        mask[mindex++] = 0;
    }
    else
    {
        bits[bindex++] = 0;
        bits[bindex++] = 0;
        mask[mindex++] = 0xFF;
        mask[mindex++] = 0xFF;
    }

    *sbindex = bindex;
    *smindex = mindex;
}

static void
set_dont_cares_to_zero(
    uint8_t * data,
    uint8_t * mask,
    uint32_t key_size_8)
{
    int32_t i;

    for (i = 0; i < (key_size_8 * KAPS_BITS_IN_BYTE); i++)
    {
        if (kaps_array_get_bit(data, mask, i) == KAPS_DONT_CARE)
            kaps_array_set_bit(data, i, 0);
    }
}

static int32_t
scan_key(
    const char *fname,
    uint32_t line,
    char *buf,
    struct kaps_parse_key **key,
    uint32_t * ad_width)
{
    uint8_t ad_width_flag = 0;
    char *token;
    struct kaps_parse_key *head, *tail, *tmp;
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
            kaps_printf("%s:%d unable to parse key specification\n", fname, line);
            exit(1);
        }
        tmp = kaps_sysmalloc(sizeof(*tmp) + sal_strnlen(innertok, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) + 1);
        if (!tmp)
        {
            kaps_printf("Out of memory parsing key\n");
            exit(1);
        }
        tmp->next = NULL;
        tmp->fname = tmp->buf;
        sal_strncpy(tmp->fname, innertok, sal_strnlen(innertok, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) - 1);
        innertok = kaps_strsep(&token, ",");
        while (sal_isspace(*innertok))
            innertok++;
        if (*innertok == '\0')
        {
            kaps_printf("%s:%d unable to parse key specification\n", fname, line);
            exit(1);
        }
        tmp->width = sal_atoi(innertok);
        innertok = kaps_strsep(&token, ",");
        while (sal_isspace(*innertok))
            innertok++;
        if (*innertok == '\0')
        {
            kaps_printf("%s:%d unable to parse key specification\n", fname, line);
            exit(1);
        }
        if (ad_width_flag)
        {
            kaps_printf("%s:%d ad key field should be at the end\n", fname, line);
            exit(1);
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
        else if (sal_strncmp(innertok, "ad", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
        {
            *ad_width = tmp->width;
            ad_width_flag = 1;
            kaps_sysfree(tmp);
            if (((*ad_width) <= 0) || ((*ad_width) % KAPS_BITS_IN_BYTE))
            {
                kaps_printf("%s:%d ad key field width should be multiple of 8\n", fname, line);
                exit(1);
            }
            continue;
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

static struct kaps_parse_record *
get_next_random_entry(
    struct kaps_parse_record *entries,
    uint32_t nentries)
{
    uint32_t index = kaps_random() % nentries;
    struct kaps_parse_record *add = NULL;

    if (entries[index].visited)
    {
        int j;

        for (j = index; j < nentries; j++)
        {
            if (!entries[j].visited)
            {
                add = &entries[j];
                break;
            }
        }
        if (!add)
        {
            for (j = 0; j < index; j++)
            {
                if (!entries[j].visited)
                {
                    add = &entries[j];
                    break;
                }
            }
        }
    }
    else
    {
        add = &entries[index];
    }

    kaps_sassert(add != NULL);
    if (add != NULL)
    {
        add->visited = 1;
    }
    return add;
}

kaps_status
parse_standard_db_file(
    enum kaps_db_type db_type,
    const char *fname,
    struct kaps_parse_record ** result,
    uint32_t req_num_entries,
    uint32_t ndefault_entries,
    uint32_t * ad_width,
    uint32_t * num_lines,
    uint32_t * num_entries,
    struct kaps_parse_key ** key)
{
    FILE *fp;
    int line_no, cnt = 0;
    char buf[4096];
    kaps_status status;
    struct kaps_parse_record *entries;
    long file_pos;
    uint32_t key_width_1;
    struct kaps_parse_key *tmp, *key_layout;
    uint8_t *data_mask = NULL, *ptr = NULL, *ad_data = NULL, *ad_ptr = NULL;
    int32_t i = 0, ad_step = 0, step = 0, nlines = 0;
    int32_t flag_colon_notation = 0;
    int32_t flag_user_notation = 0;
    uint32_t em_with_mask = 0;
    uint32_t t_ad_width = 0, lpm_db_spl_parse = 0;
    char lpm_spl_buf[1024];
    struct kaps_parse_key *tmp_key = NULL;
    char full_file_path[500];
    char *regressions_input_path;

    *ad_width = 0;

    regressions_input_path = getenv("REGRESSION_INPUTS");
    if (regressions_input_path)
    {
        if (sal_strnlen(regressions_input_path, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) + 1 +
            sal_strnlen(fname, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) > 500)
        {
            kaps_printf("failed to open (path too long): %s/%s\n", regressions_input_path, fname);
            return KAPS_INVALID_FILE;
        }

        sal_snprintf(full_file_path, sizeof(full_file_path), "%s/%s", regressions_input_path, fname);

    }
    else
    {
        regressions_input_path = getenv("COMMONDIR");

        if (NULL == regressions_input_path)
        {
            kaps_printf("Error: Global regressions input folder not defined\n");
            return KAPS_INTERNAL_ERROR;
        }

        sal_sprintf(full_file_path, "%s/%s/%s", regressions_input_path, "regression_inputs", fname);

    }

    fp = kaps_fopen(full_file_path, "r");
    if (!fp)
        return KAPS_INVALID_FILE;

    /*
     * Lets count the number of lines in the file. typically
     * each line is an ACL entry. Its a conservative estimate
     * to prevent re-allocation of entries
     */

    if (sal_fgets(buf, sizeof(buf), fp) == NULL)
    {
        kaps_fclose(fp);
        return KAPS_INVALID_FILE;
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
        return KAPS_INVALID_FILE;
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
        return KAPS_INVALID_FILE;
    }

    /*
     * skip comments
     */
    do
    {
        if ((strncmp(buf, "#_EM_WITH_MASK", sal_strnlen("#_EM_WITH_MASK", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH))
             == 0))
        {
            em_with_mask = 1;
        }
        if ((strncmp
             (buf, "#_LPM_SPL_FIELDS ",
              sal_strnlen("#_LPM_SPL_FIELDS ", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH)) == 0))
        {
            lpm_db_spl_parse = (cnt + 1);
            sal_strncpy(lpm_spl_buf, &buf[17], sizeof(lpm_spl_buf) - 1);
        }
        if (*buf == '#')
        {
            cnt++;
            continue;
        }
        break;
    }
    while (sal_fgets(buf, sizeof(buf), fp) != NULL);

    if (lpm_db_spl_parse)
    {
        if (scan_key(fname, lpm_db_spl_parse, lpm_spl_buf, &tmp_key, &t_ad_width))
        {
            kaps_fclose(fp);
            return KAPS_INVALID_FILE;
        }
    }

    if (scan_key(fname, cnt, buf, key, ad_width))
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
        (struct kaps_parse_record *) kaps_sysmalloc((req_num_entries + ndefault_entries) *
                                                    sizeof(struct kaps_parse_record));
    if (!entries)
    {
        kaps_fclose(fp);
        return KAPS_OUT_OF_MEMORY;
    }

    kaps_sassert(req_num_entries);
    kaps_memset(entries, 0, (req_num_entries + ndefault_entries) * sizeof(struct kaps_parse_record));
    data_mask = kaps_syscalloc(1, 2 * (req_num_entries + ndefault_entries) * (key_width_1 / KAPS_BITS_IN_BYTE));
    step = key_width_1 / KAPS_BITS_IN_BYTE;
    if (data_mask == NULL)
    {
        kaps_sysfree(entries);
        kaps_fclose(fp);
        return KAPS_OUT_OF_MEMORY;
    }

    if (*ad_width)
    {
        ad_step = *ad_width / KAPS_BITS_IN_BYTE;
        ad_data = kaps_syscalloc(1, (req_num_entries + ndefault_entries) * (*ad_width / KAPS_BITS_IN_BYTE));
        if (ad_data == NULL)
        {
            kaps_sysfree(data_mask);
            kaps_sysfree(entries);
            kaps_fclose(fp);
            return KAPS_OUT_OF_MEMORY;
        }
    }

    ptr = data_mask;
    ad_ptr = ad_data;

    for (i = 0; i < (req_num_entries + ndefault_entries); i++)
    {
        entries[i].data = ptr;
        ptr += step;
        entries[i].mask = ptr;
        if (*ad_width)
        {
            entries[i].user_ad_data = ad_ptr;
            ad_ptr += ad_step;
        }
        kaps_memset(ptr, -1, step);
        ptr += step;
    }

    cnt = 0;

    if (ndefault_entries)
    {
        cnt = ndefault_entries;
        req_num_entries += ndefault_entries;
    }

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

        while (*ptr == '\0')
            ptr++;

        if ((strncmp(ptr, "ADD:", 4) == 0))
        {
            ptr += 4;
            entries[cnt].cust_add_or_del = KAPS_CUSTOM_OPERATION_ADD;
        }
        if ((strncmp(ptr, "DEL:", 4) == 0))
        {
            ptr += 4;
            entries[cnt].cust_add_or_del = KAPS_CUSTOM_OPERATION_DEL;
        }

        if ((strncmp(ptr, "UPD:", 4) == 0))
        {
            ptr += 4;
            entries[cnt].cust_add_or_del = KAPS_CUSTOM_OPERATION_UPD;
        }

        if ((strncmp(ptr, "DB", 2) == 0))
        {
            uint32_t digit, db_value = 0;
            char c;

            ptr += 2;

            while (*ptr != ':')
            {
                c = *ptr;
                if (c >= '0' && c <= '9')
                {
                    digit = c - '0';
                    db_value = db_value * 10 + digit;
                }
                ptr += 1;
            }
            ptr += 1;
            entries[cnt].cust_db_id = db_value;
        }

        while (sal_isspace(*ptr))
            ptr++;

        if (*ptr == '\0' || *ptr == '#')
            continue;

        if (lpm_db_spl_parse)
            key_layout = tmp_key;

        status =
            kaps_parse_entry(db_type, ptr, key_layout, &entries[cnt], key_width_1, cnt, flag_colon_notation,
                             flag_user_notation, *ad_width, em_with_mask);
        if (status != KAPS_OK)
        {
            kaps_printf("Error parsing line %d: %s\n", line_no, buf);
            sal_fflush(stdout);
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

kaps_status
eval_lpm_db(
    enum kaps_db_type db_type,
    const char *db_name,
    struct kaps_parse_record * entries,
    uint32_t num_entries,
    uint32_t key_len,
    uint32_t ** histogram,
    uint32_t * max_prefix_len)
{
    uint32_t *ip_histogram;
    uint32_t i, max_len = 0, len;
    kaps_status status = KAPS_OK;

    if ((db_type != KAPS_DB_LPM) || (key_len == 0))
    {
        goto end;
    }
    ip_histogram = kaps_syscalloc((key_len + 1), sizeof(uint32_t));

    for (i = 0; i < (key_len + 1); i++)
    {
        ip_histogram[i] = 0x0;
    }

    for (i = 0; i < num_entries; i++)
    {
        len = entries[i].length;
        if (len > (key_len + 1))
        {
            kaps_printf("%s: Error evaluating DB: %s bad length %u @ %u\n", __func__, db_name, len, i);
            kaps_sysfree(ip_histogram);
            status = KAPS_PARSE_ERROR;
            goto end;
        }
        if (len > max_len)
        {
            max_len = len;
        }
        ip_histogram[entries[i].length]++;
    }
    *histogram = ip_histogram;
    *max_prefix_len = max_len;
end:
    return status;
}

kaps_status
randomize_entries(
    int32_t randomize,
    struct kaps_parse_record ** result,
    uint32_t * num_entries)
{
    struct kaps_parse_record *entry, *random_entries, *o_entries;
    int32_t i;

    if (!randomize)
        return KAPS_OK;

    random_entries = kaps_syscalloc(*num_entries, sizeof(*random_entries));
    if (random_entries == NULL)
        return KAPS_OUT_OF_MEMORY;

    /*
     * In order to be able to free memory completely,
     * keep the first random entry the same as first
     * pre-randomized entry
     */
    o_entries = *result;
    kaps_memcpy(&random_entries[0], &o_entries[0], sizeof(*entry));
    o_entries[0].visited = 1;

    for (i = 1; i < *num_entries; i++)
    {
        entry = get_next_random_entry(*result, *num_entries);
        kaps_memcpy(&random_entries[i], entry, sizeof(*entry));
    }

    kaps_sysfree(*result);
    *result = random_entries;
    return KAPS_OK;
}

kaps_status
kaps_parse_db_file(
    enum kaps_db_type db_type,
    int32_t randomize,
    const char *fname,
    uint32_t req_num_entries,
    uint32_t ndefault_entries,
    struct kaps_parse_record ** result,
    uint32_t * num_entries,
    struct kaps_parse_key ** key)
{
    FILE *fp;
    int32_t i, cnt, step;
    kaps_status status;
    int32_t line = 0, is_cbv6 = 0;
    uint32_t num_lines = 0, ad_width = 0;

    struct kaps_parse_record *entry, *entries;
    char buf[1024];
#define CB_IPV6_KEY_SIZE (16 + 16 + 2 + 2 + 1)  /* sip + dip + sport + dport + protocol */
#define CB_IPV4_KEY_SIZE (4 + 4 + 2 + 2 + 1 + 2)        /* sip + dip + sport + dport + protocol + flags */
    uint8_t *data_mask, *ptr;

    if (result == NULL || num_entries == NULL || key == NULL)
        return KAPS_INVALID_ARGUMENT;

    if (db_type == KAPS_DB_INVALID || db_type == KAPS_DB_AD)
        return KAPS_INVALID_ARGUMENT;

    status = parse_standard_db_file(db_type, fname, result, req_num_entries,
                                    ndefault_entries, &ad_width, &num_lines, num_entries, key);
    if (status == KAPS_OK)
    {
        if (*result != NULL)
        {
            return randomize_entries(randomize, result, num_entries);
        }
    }
    if (db_type != KAPS_DB_ACL)
    {
        /*
         * From this point classbench only, which is ACL
         */
        return KAPS_PARSE_ERROR;
    }

    *key = NULL;
    fp = kaps_fopen(fname, "r");
    if (!fp)
        return KAPS_INVALID_FILE;

    /*
     * count the number of lines, number of lines ~=
     * number of ACL entries. In addition we will
     * check if the type is classbench v4 or v6
     */
    cnt = 0;
    while (!feof(fp))
    {
        if (sal_fgets(buf, sizeof(buf), fp) == NULL)
            break;
        if (*buf == '&')
            is_cbv6 = 1;
        else
            is_cbv6 = 0;
        cnt++;
    }

    if (cnt == 0)
    {
        kaps_fclose(fp);
        return KAPS_INVALID_FILE;
    }

    if (req_num_entries)
    {
        if (cnt <= req_num_entries)
            req_num_entries = cnt;
    }
    else
    {
        req_num_entries = cnt;
    }

    (void) kaps_fseek(fp, 0L, SEEK_SET);

    entries = (struct kaps_parse_record *) kaps_sysmalloc(req_num_entries * sizeof(struct kaps_parse_record));
    if (!entries)
    {
        kaps_fclose(fp);
        return KAPS_OUT_OF_MEMORY;
    }

    kaps_memset(entries, 0, req_num_entries * sizeof(struct kaps_parse_record));
    if (is_cbv6)
    {
        data_mask = kaps_syscalloc(1, 2 * req_num_entries * CB_IPV6_KEY_SIZE);
        step = CB_IPV6_KEY_SIZE;
    }
    else
    {
        data_mask = kaps_syscalloc(1, 2 * req_num_entries * CB_IPV4_KEY_SIZE);
        step = CB_IPV4_KEY_SIZE;
    }
    ptr = data_mask;
    if (data_mask == NULL)
    {
        kaps_sysfree(entries);
        kaps_fclose(fp);
        return KAPS_OUT_OF_MEMORY;
    }
    for (i = 0; i < req_num_entries; i++)
    {
        entries[i].data = ptr;
        ptr += step;
        entries[i].mask = ptr;
        kaps_memset(ptr, -1, step);
        ptr += step;
    }
    entry = entries;
    cnt = 0;
    while ((cnt < req_num_entries) && !feof(fp))
    {

        uint8_t bits[CB_IPV6_KEY_SIZE], mask[CB_IPV6_KEY_SIZE];

        line++;
        if (sal_fgets(buf, sizeof(buf), fp) == NULL)
            break;

        if (buf[0] == '&')
        {
            /*
             * Classbench IPv6 format
             *
             * +flowid-flowid sip/mask dip/mask sport: sport dport : dport protocol
             */
            int fid1, fid2, i;
            char sourceip[256], destip[256];
            int sipmask, dipmask;
            int s_min, s_max, d_min, d_max;
            int protocol;
            int bindex, mindex;
            char *p;
            struct in6_addr addr;

            i = sscanf(buf, "&%d - %d %s %s %d : %d %d : %d %d",
                       &fid1, &fid2, sourceip, destip, &s_min, &s_max, &d_min, &d_max, &protocol);
            if (i != 9)
            {
                kaps_printf("%s: bad format at line %d\n", fname, line);
                exit(1);
            }

            p = sal_strnrchr(sourceip, '/', sizeof(sourceip));
            *p = 0;
            sipmask = sal_atoi(p + 1);

            p = sal_strnrchr(destip, '/', sizeof(destip));
            *p = 0;
            dipmask = sal_atoi(p + 1);

            if (inet_pton(AF_INET6, sourceip, &addr) != 1)
            {
                kaps_printf("%s: could not parse source IP %s at line %d\n", fname, sourceip, line);
                exit(1);
            }

            bindex = mindex = 0;
            for (i = 0; i < 16; i++, bindex++)
            {
                bits[bindex] = addr.s6_addr[i];
            }

            /*
             * smask 
             */
            compute_mask_64(sipmask, bits, mask, &mindex);

            if (inet_pton(AF_INET6, destip, &addr) != 1)
            {
                kaps_printf("%s: could not parse dest IP %s at line %d\n", fname, destip, line);
                exit(1);
            }
            for (i = 0; i < 16; i++, bindex++)
            {
                bits[bindex] = addr.s6_addr[i];
            }

            /*
             * dmask 
             */
            compute_mask_64(dipmask, bits, mask, &mindex);

            /*
             * sport 
             */
            compute_port(s_min, s_max, bits, &bindex, mask, &mindex);

            /*
             * dport 
             */
            compute_port(d_min, d_max, bits, &bindex, mask, &mindex);

            /*
             * protocol 
             */
            if (protocol == 0)
            {
                bits[bindex++] = 0;
                mask[mindex++] = 0xFF;
            }
            else
            {
                bits[bindex++] = protocol & 0xFF;
                mask[mindex++] = 0;
            }

            if (bindex != CB_IPV6_KEY_SIZE || mindex != CB_IPV6_KEY_SIZE)
                goto err;

            entry->num_ranges = 2;

            if (line > KAPS_HW_MINIMUM_PRIORITY)
                entry->priority = KAPS_HW_MINIMUM_PRIORITY;
            else
                entry->priority = line;

            kaps_memcpy(entry->data, bits, CB_IPV6_KEY_SIZE);
            kaps_memcpy(entry->mask, mask, CB_IPV6_KEY_SIZE);
            entry->ranges[0].offset_8 = 32;
            entry->ranges[0].lo = s_min;
            entry->ranges[0].hi = s_max;
            entry->ranges[1].offset_8 = 34;
            entry->ranges[1].lo = d_min;
            entry->ranges[1].hi = d_max;
            set_dont_cares_to_zero(entry->data, entry->mask, CB_IPV6_KEY_SIZE);
            entry++;
            cnt++;

            if (*key == NULL)
            {
                struct kaps_parse_key *tail, *tmp;

                tail = NULL;
                tmp =
                    kaps_sysmalloc(sizeof(*tmp) + sal_strnlen("SIP", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) + 1);
                tmp->fname = tmp->buf;
                tmp->width = 128;
                tmp->next = NULL;
                sal_strncpy(tmp->fname, "SIP", sal_strnlen("SIP", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) + 1);
                *key = tail = tmp;
                tmp->type = KAPS_KEY_FIELD_PREFIX;

                tmp =
                    kaps_sysmalloc(sizeof(*tmp) + sal_strnlen("DIP", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) + 1);
                tmp->fname = tmp->buf;
                tmp->width = 128;
                tmp->next = NULL;
                sal_strncpy(tmp->fname, "DIP", sal_strnlen("DIP", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) + 1);
                tail->next = tmp;
                tail = tmp;
                tmp->type = KAPS_KEY_FIELD_PREFIX;

                tmp =
                    kaps_sysmalloc(sizeof(*tmp) + sal_strnlen("SPORT", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) +
                                   1);
                tmp->fname = tmp->buf;
                tmp->width = 16;
                tmp->next = NULL;
                sal_strncpy(tmp->fname, "SPORT",
                            sal_strnlen("SPORT", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) + 1);
                tail->next = tmp;
                tail = tmp;
                tmp->type = KAPS_KEY_FIELD_RANGE;

                tmp =
                    kaps_sysmalloc(sizeof(*tmp) + sal_strnlen("DPORT", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) +
                                   1);
                tmp->fname = tmp->buf;
                tmp->width = 16;
                tmp->next = NULL;
                sal_strncpy(tmp->fname, "DPORT",
                            sal_strnlen("DPORT", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) + 1);
                tail->next = tmp;
                tail = tmp;
                tmp->type = KAPS_KEY_FIELD_RANGE;

                tmp =
                    kaps_sysmalloc(sizeof(*tmp) +
                                   sal_strnlen("PROTOCOL", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) + 1);
                tmp->fname = tmp->buf;
                tmp->width = 8;
                tmp->next = NULL;
                sal_strncpy(tmp->fname, "PROTOCOL",
                            sal_strnlen("PROTOCOL", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) + 1);
                tail->next = tmp;
                tail = tmp;
                tmp->type = KAPS_KEY_FIELD_EM;
            }

        }
        else if (buf[0] == '@')
        {       /* classbench IPv4 format */
            int sm, dm, i;
            char *p;
            int s_min, s_max, d_min, d_max;
            char sip[64], dip[64];
            int bindex, mindex;
            int pbytes[8];
            uint32_t protocol, protocol_mask;
            uint32_t flags, flags_mask;
            uint16_t fmask;

            if (sscanf(buf, "@%s %s %d : %d %d : %d 0x%x/0x%x 0x%x/0x%x",
                       sip, dip, &s_min, &s_max, &d_min, &d_max, &protocol, &protocol_mask, &flags, &flags_mask) != 10)
            {
                kaps_printf("%s: bad format at line %d\n", fname, line);
                exit(1);
            }

            if (!isdigit(sip[0]))
            {
                kaps_printf("%s: bad format at line %d\n", fname, line);
                continue;
            }

            p = sal_strnrchr(sip, '/', sizeof(sip));
            *p = 0;
            sm = sal_atoi(p + 1);

            p = sal_strnrchr(dip, '/', sizeof(dip));
            *p = 0;
            dm = sal_atoi(p + 1);

            bindex = mindex = 0;

            if (sscanf(sip, "%d.%d.%d.%d", &pbytes[0], &pbytes[1], &pbytes[2], &pbytes[3]) != 4)
            {
                kaps_printf("%s: bad format at line %d\n", fname, line);
                exit(1);
            }

            if (sscanf(dip, "%d.%d.%d.%d", &pbytes[4], &pbytes[5], &pbytes[6], &pbytes[7]) != 4)
            {
                kaps_printf("%s: bad format at line %d\n", fname, line);
                exit(1);
            }

            compute_mask_32(sm, mask, &mindex);
            compute_mask_32(dm, mask, &mindex);

            for (i = 0; i < 8; i++, bindex++)
                bits[bindex] = (uint8_t) pbytes[i];

            /*
             * sport 
             */
            compute_port(s_min, s_max, bits, &bindex, mask, &mindex);

            /*
             * dport 
             */
            compute_port(d_min, d_max, bits, &bindex, mask, &mindex);

            /*
             * protocol 
             */
            if (protocol == 0)
            {
                bits[bindex++] = 0;
                mask[mindex++] = 0xFF;
            }
            else
            {
                uint8_t prm = (uint8_t) protocol_mask;

                prm = ~prm;

                bits[bindex++] = protocol;
                mask[mindex++] = prm;
            }

            /*
             * flags 
             */
            bits[bindex++] = (flags >> 8) & 0xFF;
            bits[bindex++] = flags & 0xFF;
            fmask = flags_mask;
            fmask = ~fmask;
            mask[mindex++] = (fmask >> 8) & 0xFF;
            mask[mindex++] = fmask & 0xFF;

            if (bindex != CB_IPV4_KEY_SIZE || mindex != CB_IPV4_KEY_SIZE)
                goto err;

            entry->num_ranges = 2;
            entry->priority = line;
            kaps_memcpy(entry->data, bits, CB_IPV4_KEY_SIZE);
            kaps_memcpy(entry->mask, mask, CB_IPV4_KEY_SIZE);
            entry->ranges[0].offset_8 = 8;
            entry->ranges[0].lo = s_min;
            entry->ranges[0].hi = s_max;
            entry->ranges[1].offset_8 = 10;
            entry->ranges[1].lo = d_min;
            entry->ranges[1].hi = d_max;
            set_dont_cares_to_zero(entry->data, entry->mask, CB_IPV4_KEY_SIZE);
            entry++;
            cnt++;

            if (*key == NULL)
            {
                struct kaps_parse_key *tail, *tmp;

                tail = NULL;
                tmp =
                    kaps_sysmalloc(sizeof(*tmp) + sal_strnlen("SIP", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) + 1);
                tmp->fname = tmp->buf;
                tmp->width = 32;
                tmp->next = NULL;
                sal_strncpy(tmp->fname, "SIP", sal_strnlen("SIP", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) + 1);
                *key = tail = tmp;
                tmp->type = KAPS_KEY_FIELD_PREFIX;

                tmp =
                    kaps_sysmalloc(sizeof(*tmp) + sal_strnlen("DIP", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) + 1);
                tmp->fname = tmp->buf;
                tmp->width = 32;
                tmp->next = NULL;
                sal_strncpy(tmp->fname, "DIP", sal_strnlen("DIP", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) + 1);
                tail->next = tmp;
                tail = tmp;
                tmp->type = KAPS_KEY_FIELD_PREFIX;

                tmp =
                    kaps_sysmalloc(sizeof(*tmp) + sal_strnlen("SPORT", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) +
                                   1);
                tmp->fname = tmp->buf;
                tmp->width = 16;
                tmp->next = NULL;
                sal_strncpy(tmp->fname, "SPORT",
                            sal_strnlen("SPORT", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) + 1);
                tail->next = tmp;
                tail = tmp;
                tmp->type = KAPS_KEY_FIELD_RANGE;

                tmp =
                    kaps_sysmalloc(sizeof(*tmp) + sal_strnlen("DPORT", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) +
                                   1);
                tmp->fname = tmp->buf;
                tmp->width = 16;
                tmp->next = NULL;
                sal_strncpy(tmp->fname, "DPORT",
                            sal_strnlen("DPORT", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) + 1);
                tail->next = tmp;
                tail = tmp;
                tmp->type = KAPS_KEY_FIELD_RANGE;

                tmp =
                    kaps_sysmalloc(sizeof(*tmp) +
                                   sal_strnlen("PROTOCOL", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) + 1);
                tmp->fname = tmp->buf;
                tmp->width = 8;
                tmp->next = NULL;
                sal_strncpy(tmp->fname, "PROTOCOL",
                            sal_strnlen("PROTOCOL", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) + 1);
                tail->next = tmp;
                tail = tmp;
                tmp->type = KAPS_KEY_FIELD_EM;

                tmp =
                    kaps_sysmalloc(sizeof(*tmp) + sal_strnlen("FLAGS", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) +
                                   1);
                tmp->fname = tmp->buf;
                tmp->width = 16;
                tmp->next = NULL;
                sal_strncpy(tmp->fname, "FLAGS",
                            sal_strnlen("FLAGS", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) + 1);
                tail->next = tmp;
                tail = tmp;
                tmp->type = KAPS_KEY_FIELD_TERNARY;
            }
        }
        else
        {
            goto err;
        }
    }

    *num_entries = entry - entries;
    *result = entries;
    kaps_fclose(fp);
    return randomize_entries(randomize, result, num_entries);
err:
    kaps_sysfree(entries);
    kaps_fclose(fp);
    return KAPS_INTERNAL_ERROR;
}
