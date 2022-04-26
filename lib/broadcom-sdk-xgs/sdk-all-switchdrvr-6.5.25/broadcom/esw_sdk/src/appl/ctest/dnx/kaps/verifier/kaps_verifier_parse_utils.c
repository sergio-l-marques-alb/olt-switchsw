/** \file kaps_verifier_parse_utils.c
 *
 * Ctests for KAPS
 *
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#include "kaps_verifier_parse.h"
#include "src/appl/ctest/dnx/kaps/ctest_dnx_kaps.h"

#include "kaps_pfx_hash_table.h"
#include "kaps_lpm_algo.h"
#include "kaps_fib_cmn_pfxbundle.h"
#include "kaps_algo_common.h"
#include "kaps_device_internal.h"
#include "kaps_ad_internal.h"
#include "kaps_key_internal.h"
#include "kaps_algo_hw.h"

static void
utils_set_range_in_key(
    uint8_t * key,
    uint32_t offset_8,
    uint16_t portval)
{
    key[offset_8] = (portval >> 8) & 0xFF;
    key[offset_8 + 1] = portval & 0xFF;
}

static uint16_t
utils_get_range_from_key(
    const uint8_t * key,
    uint32_t offset_8)
{
    uint16_t portval;
    portval = (key[offset_8] << 8);
    portval |= key[offset_8 + 1] & 0xFF;
    return portval;
}

static kaps_status
acl_match_key_to_parse_record(
    const uint8_t * key,
    const struct kaps_parse_record *entry,
    int32_t width_8)
{
    int i;

    for (i = (width_8 - 1); i >= 0; i--)
    {
        if ((entry->data[i] & ~entry->mask[i]) != (key[i] & ~entry->mask[i]))
            return KAPS_MISMATCH;       /* mismatch */
    }

    for (i = 0; i < entry->num_ranges; i++)
    {
        uint16_t rvalue = utils_get_range_from_key(key, entry->ranges[i].offset_8);
        if (rvalue < entry->ranges[i].lo || rvalue > entry->ranges[i].hi)
            return KAPS_MISMATCH;       /* mismatch */
    }
    return KAPS_MATCH;  /* match */
}

kaps_status
kaps_parse_generate_random_valid_key(
    const struct kaps_parse_record * e,
    uint8_t * key,
    uint32_t width_8)
{
    uint32_t i;
    kaps_status status;
    uint32_t seed;

    if (e == NULL || key == NULL)
        return KAPS_INVALID_ARGUMENT;

    for (i = 0; i < width_8; i++)
    {
        seed = e->data[i] + i;
        key[i] = kaps_random_r(&seed) % 256;
        key[i] |= e->data[i] & ~e->mask[i];     /* Setting bits we care about to 1 */
        key[i] &= e->data[i] | e->mask[i];      /* Setting bits we care about to 0 */
    }
    for (i = 0; i < e->num_ranges; i++)
    {
        seed = e->priority + i;
        switch (kaps_random_r(&seed) % 3)
        {
            case 0:
                utils_set_range_in_key(key, e->ranges[i].offset_8, e->ranges[i].lo);
                break;
            case 1:
                utils_set_range_in_key(key, e->ranges[i].offset_8, e->ranges[i].hi);
                break;
            default:
                utils_set_range_in_key(key, e->ranges[i].offset_8,
                                       e->ranges[i].lo + kaps_random_r(&seed) % (e->ranges[i].hi - e->ranges[i].lo +
                                                                                 1));
        }
    }

    status = acl_match_key_to_parse_record(key, e, width_8);
    if (status != KAPS_MATCH)
        return status;
    return KAPS_OK;
}

#if 0
static kaps_status
acl_add_entries(
    struct kaps_device *device,
    struct kaps_parse_db_conf *c,
    struct kaps_db *db,
    struct kaps_ad_db *ad,
    struct kaps_parse_record *entries,
    uint32_t nentries)
{
    uint32_t nadded = 0;
    kaps_status status;
    uint8_t ad_bytes[KAPS_HW_MAX_UDA_WIDTH_8];
    uint32_t has_ad = 0;
    uint32_t ad_cap = 0;
    uint32_t visited_ad_cap = 0;

    if (ad)
        has_ad = 1;
    kaps_memset(ad_bytes, 0, sizeof(ad_bytes));

    while (nadded < nentries)
    {
        int r;
        struct kaps_entry *e;
        struct kaps_ad *ad_entry = NULL;
        struct kaps_parse_record *add = get_next_entry(nadded, entries, nentries, c);

        KAPS_TRY(kaps_db_add_ace(db, add->data, add->mask, add->priority, &e));
        add->e = e;
        for (r = 0; r < add->num_ranges; r++)
        {
            KAPS_TRY(kaps_entry_add_range(db, e, add->ranges[r].lo, add->ranges[r].hi, r));
        }

        if (has_ad)
        {
            uint8_t ad_bytes[KAPS_HW_MAX_UDA_WIDTH_8];

            /*
             * assume we have at least 32b 
             */
            ad_bytes[0] = ((add->priority & 0xFF0000) >> 16);
            ad_bytes[1] = ((add->priority & 0x00FF00) >> 8);
            ad_bytes[2] = ((add->priority & 0x0000FF));

            ad = get_next_ad_entry(&ad_cap, ad);

            if (ad != NULL)
            {
                status = kaps_ad_db_add_entry(ad, ad_bytes, &ad_entry);
                if (status == KAPS_OUT_OF_UDA)
                {
                    add->visited = 0;
                    add->e = NULL;
                    KAPS_TRY(kaps_db_delete_entry(db, e));
                    c->status = status;
                    return KAPS_OK;
                }
            }
            else
            {
                ad_entry = get_next_visited_ad_entry(entries, nentries, &visited_ad_cap);
                if (ad_entry == NULL)
                {
                    visited_ad_cap = 0;
                    ad_entry = get_next_visited_ad_entry(entries, nentries, &visited_ad_cap);
                }
            }
            KAPS_TRY(kaps_entry_add_ad(db, e, ad_entry));
            add->ad_data = ad_entry;
        }
        if (c->flag_incremental)
        {
            status = kaps_db_install(db);
            if (status == KAPS_OUT_OF_INDEX
                || status == KAPS_OUT_OF_DBA
                || status == KAPS_OUT_OF_UDA
                || status == KAPS_OUT_OF_UIT || status == KAPS_EXHAUSTED_PCM_RESOURCE || status == KAPS_OUT_OF_AD)
            {
                add->visited = 0;
                add->e = NULL;
                c->status = status;
                KAPS_TRY(kaps_db_delete_entry(db, e));
                return KAPS_OK;
            }
            else if (status != KAPS_OK)
            {
                c->status = status;
                return status;
            }
        }
        nadded++;
    }

    status = kaps_db_install(db);
    c->status = status;
    return status;
}
#endif

static kaps_status
write_bits_in_arr(
    uint8_t * Arr,
    uint32_t ArrSize,
    uint32_t EndPos,
    uint32_t StartPos,
    uint32_t Value)
{
    uint32_t startByteIdx = 0;
    uint32_t endByteIdx = 0;
    uint32_t byte = 0;
    uint32_t len = 0;
    uint8_t maskValue = 0;

    len = (EndPos - StartPos + 1);
    kaps_sassert((len <= 32));

    /*
     * Value is unsigned 32 bit variable, so it can not be greater than ~0.
     */
    if (len != 32)
    {
        kaps_sassert(Value <= ((uint32_t) (~(~0 << len))));
    }
    startByteIdx = ArrSize - ((StartPos >> 3) + 1);
    endByteIdx = ArrSize - ((EndPos >> 3) + 1);

    if (startByteIdx == endByteIdx)
    {
        maskValue = (uint8_t) (0xFE << ((EndPos & 7)));
        maskValue |= ((1 << (StartPos & 7)) - 1);
        Arr[startByteIdx] &= maskValue;
        Arr[startByteIdx] |= (uint8_t) ((Value << (StartPos & 7)));
        return 0;
    }
    if (StartPos & 7)
    {
        maskValue = (uint8_t) ((1 << (StartPos & 7)) - 1);
        Arr[startByteIdx] &= maskValue;
        Arr[startByteIdx] |= (uint8_t) ((Value << (StartPos & 7)));
        startByteIdx--;
        Value >>= (8 - (StartPos & 7));
    }
    for (byte = startByteIdx; byte > endByteIdx; byte--)
    {
        Arr[byte] = (uint8_t) (Value);
        Value >>= 8;
    }
    maskValue = (uint8_t) (0xFE << ((EndPos & 7)));
    Arr[byte] &= maskValue;
    Arr[byte] |= Value;

    return KAPS_OK;
}

static kaps_status
gen_lpm_prefix(
    uint8_t * pfxData_p,
    uint32_t table_width,
    uint32_t iterValue,
    uint32_t * plen,
    uint32_t test,
    uint32_t random_flag)
{
    uint8_t i;
    uint8_t userPfxData[5] = { 0, };
    uint8_t pfxData[21] = { 0, };
    uint32_t pfxLen = 0;
    uint32_t a, b, c, d;
    int32_t noOfBytes;
    uint32_t iter;
    uint32_t seedNum = 0;

    /*
     * static counter = 0; 
     */
    uint32_t pfxType;
    uint32_t tblWidth = table_width;
    uint32_t prefMinLen = 8;
    uint32_t prefMaxLen = table_width;
    uint32_t basePfxWidth = (((tblWidth / 80) >> 1) * 80);
    uint32_t test_has_too_many_duplicates;

    seedNum = ((uint32_t) ((kaps_random_r(&iterValue) << 16) | kaps_random_r(&iterValue)));

    iter = iterValue;
    pfxType = test;

    d = pfxType % 10;   /* get D value */

    if (d == 0)
        pfxData[0] = 0;
    else if (d == 1)
        pfxData[0] = (uint8_t) ((iter & 1) ? 0xf : 0);
    else if (d == 2)
        pfxData[0] = (uint8_t) seedNum;
    else if (d == 3)
        pfxData[0] = (uint8_t) (((8 * iter) / 0xF8) % 0xFF);

    pfxType = pfxType / 10;
    c = pfxType % 10;   /* get C value */

    if (c == 0)
        pfxData[1] = 0;
    else if (c == 1)
        pfxData[1] = (uint8_t) ((iter & 1) ? 0xf : 0);
    else if (c == 2)
        pfxData[1] = (uint8_t) (seedNum >> 16);
    else if (c == 3)
        pfxData[1] = (uint8_t) ((8 * iter) % 0xF8);
    else
    {
        pfxData[1] = (uint8_t) c;
    }

    pfxType = pfxType / 10;
    b = pfxType % 10;   /* Get B (variable part of the prefix) value */

    pfxType = pfxType / 10;
    a = pfxType % 10;   /* Get A value (plen) */

    switch (a)
    {

        case 0:
            pfxLen = tblWidth;
            break;

        case 1:
            pfxLen = ((iter & 1) ? tblWidth : (tblWidth - 2));
            break;

        case 2:
            /*
             * if randrange is specified generate plen which randomly very between pfxLenLow to pfxLenHigh. else
             * generate plen which randomly very between 16 to 36 
             */
            do
            {
                pfxLen =
                    prefMinLen + ((uint8_t) (seedNum >> 8) % ((prefMaxLen) ? ((prefMaxLen - prefMinLen) + 1) : 21));
            }
            while (pfxLen <= 7);

            break;

        case 3:
            pfxLen = ((tblWidth - 7) + (iter & 0x7));
            break;

        case 4:
            pfxLen = (iter >> 12) ? ((tblWidth - 31) + (iter & 0x1f)) : ((tblWidth - 3) + (iter & 0x3));
            break;

        case 5:
            pfxLen = (iter & 0x3ff) ? (tblWidth - (iter & 0x7)) : (24 - ((iter >> 12) & 0xf));
            break;

        case 6:
            switch (tblWidth)
            {

                case 36:       /* Tbl Id + IpV4 */
                    pfxLen = 5 + (((uint8_t) (seedNum >> 8)) % 32);
                    break;
                case 48:       /* Tbl Id + VPNID + IpV4 */
                    pfxLen = 17 + (((uint8_t) (seedNum >> 8)) % 32);
                    break;
                case 132:      /* Tbl Id + IpV6 */
                    pfxLen = 5 + (((uint8_t) (seedNum >> 8)) % 128);
                    break;
                case 144:      /* Tbl Id + VPNID + IpV6 */
                    pfxLen = 17 + (((uint8_t) (seedNum >> 8)) % 128);
                    break;
                default:
                    pfxLen = 16 + (((uint8_t) (seedNum >> 8)) % 32);
                    pfxLen += basePfxWidth;
                    break;
            }
            break;
        case 7:
            /*
             * if m_reversePfxOrder == 1 if randrange is specified generate plen which decrements sequentially between
             * pfxLenHigh to pfxLenLow. else generate plen which decrements sequentially between 36 to 8 else if
             * randrange is specified generate plen which increments sequentially between pfxLenLow to pfxLenHigh. else
             * generate plen which increments sequentially between 8 to 36
             * 
             * Note : 8-25 will generate more duplicate prefixes (max 719 unique in 1M) 
             */

            do
            {
                pfxLen = prefMinLen + (iter % ((prefMaxLen) ? ((prefMaxLen - prefMinLen) + 1) : 29));
            }
            while (pfxLen <= 7);
            break;

        case 8:
#if 0
            randval = seedNum;
            weightage = ((double) (randval & 0xffffff)) / (double) 0xffffff;

            if (weightage <= 0.5552)
                pfxLen = 24;
            else if (weightage <= 0.6358)
                pfxLen = 23;
            else if (weightage <= 0.6993)
                pfxLen = 22;
            else if (weightage <= 0.7424)
                pfxLen = 21;
            else if (weightage <= 0.7966)
                pfxLen = 20;
            else if (weightage <= 0.8599)
                pfxLen = 19;
            else if (weightage <= 0.8818)
                pfxLen = 18;
            else if (weightage <= 0.8988)
                pfxLen = 17;
            else if (weightage <= 0.9626)
                pfxLen = 16;
            else
            {
                randval = (randval & 0xffff) * (randval >> 16);
                pfxLen = 25 + (randval & 7);    /* use only 1 random generator ! */
            }
            switch (tblWidth)
            {
                case 36:
                    pfxLen += 4;
                    break;
                case 48:
                    pfxLen += 16;
                    break;
                case 132:
                    pfxLen += 100;
                    break;
                case 144:
                    pfxLen += 112;
                    break;
                default:
                    if (tblWidth < 48)
                        pfxLen += 4;
                    else
                        pfxLen += 16;
                    pfxLen += basePfxWidth;
                    break;
            }
#endif
            break;
        case 9:
            pfxLen = 8 + (((uint8_t) (seedNum >> 24)) % (tblWidth - 7));
            break;
    }

    test_has_too_many_duplicates = 0;
    if ((2500 <= test && test < 2600) || (7500 <= test && test < 7600))
    {
        test_has_too_many_duplicates = 1;
    }

    if (test_has_too_many_duplicates && pfxLen < 24)
    {
        random_flag = 1;
    }

    /*
     * If the prefixes are short (<= 16), we should not generate too many duplicates , otherwise FTT takes a lot of
     * time to search So randomising these entries
     */
    if (pfxLen <= 8)
    {
        pfxData[0] = (uint8_t) (0xFF & (kaps_random_r(&seedNum)));

    }
    else if (pfxLen <= 16)
    {
        pfxData[0] = (uint8_t) (0xFF & (kaps_random_r(&seedNum)));
        pfxData[1] = (uint8_t) (0xFF & (kaps_random_r(&seedNum)));
    }

    /*
     * Calculate the remaining bytes in the prefix 
     */
    noOfBytes = (pfxLen / 8);
    if ((noOfBytes - 2) < 2)
    {
        noOfBytes = 2;
    }
    else if (!(pfxLen % 8))
    {
        noOfBytes--;
    }

    if (random_flag)
    {
        /*
         * all random value up to plen 
         */
        pfxLen = table_width;
        noOfBytes = (pfxLen + 7) / 8;
        for (i = 0; i < noOfBytes; i++)
            pfxData[i] = (uint8_t) (0xFF & (kaps_random_r(&seedNum)));
    }
    else
    {
        switch (b)
        {
            case 0:
                for (i = 2; i <= noOfBytes; i++)
                    pfxData[i] = (uint8_t) (0xFF & ((uint32_t) iter >> (8 * (i - 2))));
                break;

            case 1:
                for (i = 2; i <= noOfBytes; i++)
                {
                    pfxData[(noOfBytes + 2) - i] = (uint8_t) (0xFF & iter);
                    if (5 - i >= 0)
                        pfxData[(noOfBytes + 2) - i] = pfxData[(noOfBytes + 2) - i] + userPfxData[5 - i];
                    iter = iter >> 8;
                }
                break;

            case 2:
                for (i = 2; i <= noOfBytes;)
                {
                    uint8_t j;

                    for (j = 0; j < 4 && i <= noOfBytes; j++, i++)
                        pfxData[i + j] = ((uint8_t) seedNum >> (8 * j));

                    j = 0;
                    seedNum = ((uint32_t) ((kaps_random_r(&iterValue) << 16) | kaps_random_r(&iterValue)));
                }
                break;

            case 3:
                pfxData[3] = (uint8_t) ((8 * iter) % 0xF8);
                pfxData[2] = (uint8_t) (((8 * iter) / 0xF8) % 0xFF);

                pfxData[5] = (uint8_t) ((8 * (iter >> 16)) % 0xF8);
                pfxData[4] = (uint8_t) (((8 * (iter >> 16)) / 0xF8) % 0xFF);
                break;

            case 4:
                for (i = 2; i <= noOfBytes; i++)
                {
                    pfxData[(noOfBytes) - i] = (uint8_t) (0xFF & (iter >> (8 * (i - 2))));
                    if ((5 - i) >= 0)
                        pfxData[(noOfBytes) - i] = pfxData[(noOfBytes) - i] + userPfxData[5 - i];
                }
                break;

            case 5:
                for (i = 2; i <= noOfBytes; i++)
                {
                    uint8_t j;

                    for (j = 0; j < 4 && i <= noOfBytes; j++)
                        pfxData[i] = ((uint8_t) seedNum >> (8 * i));

                    j = 0;
                    seedNum = ((uint32_t) ((kaps_random_r(&iterValue) << 16) | kaps_random_r(&iterValue)));
                }

                if (pfxLen > 44)
                    pfxData[2] = 0;

                break;

            default:
                kaps_printf("Incorrect pfx Pattern");
                return 1;
        }
    }

    /*
     * Append the table id into the prefix 
     */
    write_bits_in_arr(pfxData, 20, 159, (160 - 4), (iter % 0xF));

    if (pfxLen > table_width)
        pfxLen = table_width;

    *plen = pfxLen;     /* assign the prefix variable length */
    kaps_memcpy(pfxData_p, pfxData, table_width / KAPS_BITS_IN_BYTE);

    /*
     * kaps_printf("\n GEN"); 
     */
    /*
     * print_prefix(pfxLen, pfxData); 
     */
    return KAPS_OK;
}

struct kaps_pre_gen_info
{
    struct pfx_hash_table *ht;
    struct kaps_allocator *default_allocator;
    struct kaps_acl_entry *e;
    struct kaps_db *db;
    struct kaps_ad_db *ad;
    struct kaps_acl_entry **e_array;
    struct kaps_entry **hash_slot;
    struct kaps_device *device;
    struct kaps_key *key;
    struct kaps_key *mkey;
    struct kaps_instruction *inst;
};

kaps_status
kaps_parse_generate_prefixes(
    uint32_t pattern,
    uint32_t table_width_1,
    uint32_t num_requested,
    struct kaps_parse_record **result,
    uint32_t * num_entries)
{
    uint32_t cnt = 0;
    struct kaps_parse_record *entry, *entries;
    kaps_status status;
    uint8_t *data_mask, *ptr;
    int32_t i, step_8;

    if (result == NULL)
        return KAPS_INVALID_ARGUMENT;

    entries = (struct kaps_parse_record *) kaps_sysmalloc(num_requested * sizeof(struct kaps_parse_record));
    if (!entries)
    {
        return KAPS_OUT_OF_MEMORY;
    }
    kaps_sassert(num_requested);
    kaps_memset(entries, 0, num_requested * sizeof(struct kaps_parse_record));
    data_mask = kaps_syscalloc(1, 2 * num_requested * ((table_width_1 + KAPS_BITS_IN_BYTE - 1) / KAPS_BITS_IN_BYTE));
    step_8 = (table_width_1 + KAPS_BITS_IN_BYTE - 1) / KAPS_BITS_IN_BYTE;

    if (data_mask == NULL)
    {
        kaps_sysfree(entries);
        return KAPS_OUT_OF_MEMORY;
    }
    ptr = data_mask;
    for (i = 0; i < num_requested; i++)
    {
        entries[i].data = ptr;
        ptr += step_8;
        entries[i].mask = ptr;
        kaps_memset(ptr, -1, step_8);
        ptr += step_8;
    }

    entry = entries;
    while (cnt < num_requested)
    {
        uint32_t plen_1;
        int32_t nbytes, nbits, rem;

        status = gen_lpm_prefix(entry->data, table_width_1, cnt, &plen_1, pattern, 0);
        if (status != KAPS_OK)
        {
            kaps_sysfree(entry);
            return status;
        }
        entry->priority = table_width_1 - plen_1;
        entry->length = plen_1;

        /*
         * Make sure we zero the correct number of
         * bytes in the mask part
         */
        nbytes = plen_1 / KAPS_BITS_IN_BYTE;
        nbits = nbytes * KAPS_BITS_IN_BYTE;
        rem = plen_1 - nbits;
        if (nbytes)
            kaps_memset(entry->mask, 0, nbytes);
        for (i = 0; i < rem; i++)
            kaps_array_set_bit(entry->mask, nbits + i, 0);

        /*
         * Make sure we zero the correct number of
         * bytes in the data part
         */
        if (rem)
        {
            for (i = rem; i < KAPS_BITS_IN_BYTE; ++i)
                kaps_array_set_bit(entry->data, nbits + i, 0);
        }

        nbytes = (plen_1 + KAPS_BITS_IN_BYTE - 1) / KAPS_BITS_IN_BYTE;
        for (i = nbytes; i < step_8; ++i)
            entry->data[i] = 0;

        entry++;
        cnt++;
    }

    *num_entries = cnt;
    *result = entries;
    return KAPS_OK;
}

static kaps_status
pre_gen_prfx_create_hash_table(
    struct kaps_pre_gen_info *info,
    enum kaps_device_type dev_type,
    uint32_t table_width_1,
    uint32_t num_requested)
{
    uint32_t master_key_width = 0;
    KAPS_TRY(kaps_default_allocator_create(&info->default_allocator));
    KAPS_TRY(kaps_device_init(info->default_allocator, dev_type, KAPS_DEVICE_DEFAULT, NULL, NULL, &info->device));
    KAPS_TRY(kaps_db_init(info->device, KAPS_DB_LPM, 0, num_requested, &info->db));

    master_key_width = table_width_1;
    if (dev_type != KAPS_DEVICE_KAPS)
        KAPS_TRY(kaps_db_set_property(info->db, KAPS_PROP_INDEX_RANGE, 0, (16 * 1024 * 1024) - 1));

    if (dev_type == KAPS_DEVICE_KAPS)
    {
        master_key_width = 160;
        KAPS_TRY(kaps_ad_db_init(info->device, 1, num_requested / 2, 20, &info->ad));
        KAPS_TRY(kaps_db_set_ad(info->db, info->ad));
    }

    KAPS_TRY(kaps_key_init(info->device, &info->key));
    KAPS_TRY(kaps_key_add_field(info->key, "SIP", table_width_1, KAPS_KEY_FIELD_PREFIX));
    KAPS_TRY(kaps_db_set_key(info->db, info->key));
    KAPS_TRY(kaps_pfx_hash_table_create(info->default_allocator, 100, 70, 100, table_width_1, 0, info->db, &info->ht));

    KAPS_TRY(kaps_instruction_init(info->device, 1, 0, &info->inst));
    KAPS_TRY(kaps_key_init(info->device, &info->mkey));
    KAPS_TRY(kaps_key_add_field(info->mkey, "SIP", master_key_width, KAPS_KEY_FIELD_PREFIX));
    KAPS_TRY(kaps_instruction_set_key(info->inst, info->mkey));
    KAPS_TRY(kaps_instruction_add_db(info->inst, info->db, 0));
    KAPS_TRY(kaps_instruction_install(info->inst));

    KAPS_TRY(kaps_device_lock(info->device));

    return KAPS_OK;
}

static kaps_status
pre_gen_prfx_add_to_hash_table(
    struct kaps_pre_gen_info *info,
    struct kaps_lpm_entry *entry,
    uint32_t * is_dup)
{
    *is_dup = 0;
    info->hash_slot = NULL;

    KAPS_TRY(kaps_pfx_hash_table_locate
             (info->ht, entry->pfx_bundle->m_data, entry->pfx_bundle->m_nPfxSize, &info->hash_slot));

    /*
     * If the entry is a duplicate that is already present in hash table then free it and generate a new entry in its
     * place
     */
    if (info->hash_slot)
    {
        *is_dup = 1;
        return KAPS_OK;
    }

    /*
     * Insert to the element to the hash table
     */
    KAPS_TRY(kaps_pfx_hash_table_insert(info->ht, (struct kaps_entry *) entry));

    return KAPS_OK;
}

static kaps_status
pre_gen_prfx_destroy_hash_table(
    struct kaps_pre_gen_info *info)
{
    KAPS_TRY(kaps_pfx_hash_table_destroy(info->ht));
    KAPS_TRY(kaps_db_destroy(info->db));
    KAPS_TRY(kaps_device_destroy(info->device));
    KAPS_TRY(kaps_default_allocator_destroy(info->default_allocator));
    return KAPS_OK;
}

#define PU_NLMCMROUND2_(x)     (((uint32_t)x+1)&~(uint32_t)1)
#define PU_NLMCMBUNDLESIZE(s1, s2)  (PU_NLMCMROUND2_(s1) + PU_NLMCMROUND2_(s2) - sizeof(uint8_t) * 2 + sizeof(kaps_pfx_bundle))
#define PfxBundle__GetNumPfxBytes(x)   (((uint32_t) (x) + 7) >> 3)

static kaps_pfx_bundle *
pre_gen_prfx_bundle_create(
    uint32_t table_width)
{
    uint32_t tmp = PfxBundle__GetNumPfxBytes(table_width);
    uint32_t sz = PU_NLMCMBUNDLESIZE(tmp, 0);   /* no ad */

    kaps_pfx_bundle *ret = (kaps_pfx_bundle *) kaps_syscalloc(1, sz);
    if (ret == NULL)
        return NULL;

    /*
     * We do not initialize the associated data 
     */
    ret->m_nIndex = 0;
    ret->m_nPfxSize = table_width;      /* will update according to plen */
#ifdef KAPS_LPM_DEBUG
    ret->m_nSeqNum = 1;
#endif
/* ret->m_data : will update according to plen Bytes */
    return ret;
}

static void
pre_gen_prfx_bundle_fill(
    kaps_pfx_bundle * bundle,
    uint32_t plen,
    uint8_t * data)
{
    uint32_t tmp = PfxBundle__GetNumPfxBytes(plen);
#ifdef KAPS_LPM_DEBUG
    static uint32_t idxCnt = 0;
#endif

    /*
     * We do not initialize the plen and the data 
     */
    bundle->m_nIndex = 0xFFFFFFFF;
#ifdef KAPS_LPM_DEBUG
    bundle->m_nSeqNum = idxCnt++;
#endif
    bundle->m_nPfxSize = plen;
    kaps_memcpy(bundle->m_data, data, tmp);
    return;
}

kaps_status
kaps_parse_generate_unique_prefixes(
    enum kaps_device_type dev_type,
    uint32_t pattern,
    uint32_t table_width_1,
    uint32_t num_requested,
    struct kaps_parse_record ** result,
    uint32_t * num_entries)
{
    uint32_t cnt = 0, random_flag = 0;
    struct kaps_parse_record *entry, *entries;
    kaps_status status;
    uint8_t *data_mask, *ptr;
    int32_t i, step_8;
    struct kaps_pre_gen_info info;
    struct kaps_lpm_entry *e;
    uint32_t is_dup = 0, iter = 0, j = 0;
    static uint32_t loop1_dups = 0, loop2_dups = 0;
    uint32_t plen_1;

    if (result == NULL)
        return KAPS_INVALID_ARGUMENT;

    e = (struct kaps_lpm_entry *) kaps_syscalloc(num_requested, sizeof(struct kaps_lpm_entry));
    for (iter = 0; iter < num_requested; iter++)
    {
        e[iter].pfx_bundle = (kaps_pfx_bundle *) pre_gen_prfx_bundle_create(table_width_1);
        if (e[iter].pfx_bundle == NULL)
        {
            for (j = 0; j < iter; j++)
                kaps_sysfree(e[j].pfx_bundle);
            kaps_sysfree(e);
            return KAPS_OUT_OF_MEMORY;
        }
    }

    entries = (struct kaps_parse_record *) kaps_sysmalloc(num_requested * sizeof(struct kaps_parse_record));
    if (!entries)
    {
        for (j = 0; j < num_requested; j++)
            if (e[j].pfx_bundle)
                kaps_sysfree(e[j].pfx_bundle);
        kaps_sysfree(e);
        return KAPS_OUT_OF_MEMORY;
    }
    kaps_sassert(num_requested);
    kaps_memset(entries, 0, num_requested * sizeof(struct kaps_parse_record));
    data_mask = kaps_syscalloc(1, 2 * num_requested * ((table_width_1 + KAPS_BITS_IN_BYTE - 1) / KAPS_BITS_IN_BYTE));
    step_8 = (table_width_1 + KAPS_BITS_IN_BYTE - 1) / KAPS_BITS_IN_BYTE;

    if (data_mask == NULL)
    {
        kaps_sysfree(entries);
        for (j = 0; j < num_requested; j++)
            if (e[j].pfx_bundle)
                kaps_sysfree(e[j].pfx_bundle);
        kaps_sysfree(e);
        return KAPS_OUT_OF_MEMORY;
    }
    ptr = data_mask;
    for (i = 0; i < num_requested; i++)
    {
        entries[i].data = ptr;
        ptr += step_8;
        entries[i].mask = ptr;
        kaps_memset(ptr, -1, step_8);
        ptr += step_8;
    }

    entry = entries;

    kaps_memset(&info, 0, sizeof(struct kaps_pre_gen_info));
    status = pre_gen_prfx_create_hash_table(&info, dev_type, table_width_1, num_requested);
    if (status != KAPS_OK)
    {
        kaps_sysfree(entries);
        for (j = 0; j < num_requested; j++)
            if (e[j].pfx_bundle)
                kaps_sysfree(e[j].pfx_bundle);
        kaps_sysfree(e);
        return status;
    }

    kaps_printf("\n\t !!! Loop0: %d Loop1: %d pattern: %d \n", loop1_dups, loop2_dups, pattern);

    while (cnt < num_requested)
    {
        int32_t nbytes, nbits, rem;

        is_dup = 0;
        iter = cnt;

        do
        {
            status = gen_lpm_prefix(entry->data, table_width_1, iter, &plen_1, pattern, random_flag);
            if (status != KAPS_OK)
            {
                kaps_status status1 = KAPS_OK;

                kaps_sysfree(entry);
                for (j = 0; j < num_requested; j++)
                    if (e[j].pfx_bundle)
                        kaps_sysfree(e[j].pfx_bundle);
                kaps_sysfree(e);

                status1 = pre_gen_prfx_destroy_hash_table(&info);

                if (status1 != KAPS_OK)
                    return KAPS_INTERNAL_ERROR;
                return KAPS_INTERNAL_ERROR;
            }
            entry->priority = table_width_1 - plen_1;
            entry->length = plen_1;

            /*
             * Make sure we zero the correct number of
             * bytes in the mask part
             */
            nbytes = plen_1 / KAPS_BITS_IN_BYTE;
            nbits = nbytes * KAPS_BITS_IN_BYTE;
            rem = plen_1 - nbits;
            if (nbytes)
                kaps_memset(entry->mask, 0, nbytes);
            for (i = 0; i < rem; i++)
                kaps_array_set_bit(entry->mask, nbits + i, 0);

            /*
             * Make sure we zero the correct number of
             * bytes in the data part
             */
            if (rem)
            {
                for (i = rem; i < KAPS_BITS_IN_BYTE; ++i)
                    kaps_array_set_bit(entry->data, nbits + i, 0);
            }

            nbytes = (plen_1 + KAPS_BITS_IN_BYTE - 1) / KAPS_BITS_IN_BYTE;
            for (i = nbytes; i < step_8; ++i)
                entry->data[i] = 0;

            pre_gen_prfx_bundle_fill(e[cnt].pfx_bundle, plen_1, entry->data);

            is_dup = 0;
            status = pre_gen_prfx_add_to_hash_table(&info, &e[cnt], &is_dup);
            if (status != KAPS_OK)
            {
                kaps_status status1 = KAPS_OK;

                kaps_sysfree(entry);
                for (j = 0; j < num_requested; j++)
                    if (e[j].pfx_bundle)
                        kaps_sysfree(e[j].pfx_bundle);
                kaps_sysfree(e);
                status1 = pre_gen_prfx_destroy_hash_table(&info);

                if (status1 != KAPS_OK)
                    return KAPS_INTERNAL_ERROR;
                return KAPS_INTERNAL_ERROR;
            }

            if (is_dup == 1)
            {
                /*
                 * reset if duplicate generated 
                 */
                kaps_memset(entry->data, 0, step_8);
                kaps_memset(entry->mask, -1, step_8);

                loop1_dups++;
                iter = kaps_random_r(&(iter));
            }
            if (loop1_dups == 100000)
            {
                kaps_printf("\n\t !!! Reached the first loop, switching to random iter: %d with test %d\n", loop1_dups,
                            pattern);
                loop1_dups = 0;
                loop2_dups++;
                random_flag = 1;
            }
            if (loop2_dups > 1)
            {
                kaps_printf("\n\t !!! Breaking the random loop, reached %d iter with test %d\n", loop2_dups, pattern);
                break;
            }
        }
        while (is_dup == 1);

        if (loop2_dups > 1)
        {
            kaps_printf("\n\t !!! Not able to generate %d prefixes (only %d) with test: %d\n", num_requested, cnt,
                        pattern);
            break;
        }

        entry++;
        cnt++;
    }

    loop1_dups = 0;
    loop2_dups = 0;

    status = pre_gen_prfx_destroy_hash_table(&info);
    if (status != KAPS_OK)
    {
        kaps_sysfree(entry);
        for (j = 0; j < num_requested; j++)
            if (e[j].pfx_bundle)
                kaps_sysfree(e[j].pfx_bundle);
        kaps_sysfree(e);
        return KAPS_INTERNAL_ERROR;
    }

    /*
     * finally release the hash table memory and the pfx_bundle 
     */
    for (iter = 0; iter < num_requested; iter++)
    {
        if (e[iter].pfx_bundle)
            kaps_sysfree(e[iter].pfx_bundle);
    }
    kaps_sysfree(e);

    *num_entries = cnt;
    *result = entries;
    return KAPS_OK;
}
