/*
 * Copyright (C) 2013, Broadcom Corporation. All Rights Reserved.
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include <linux/err.h>
#include <linux/delay.h>
#include <linux/kobject.h>
#include <linux/device.h>
#include <linux/notifier.h>
#include <linux/platform_device.h>
#include <linux/stat.h>
#include <linux/ctype.h>
#include <linux/in.h>


#include <net/xfrm.h>
#include <linux/pfkeyv2.h>

#include "paemsg_int.h"
#include "pae_cmds.h"
#include "pae_regs.h"
#include "pae_shared.h"
#include "pae_stats.h"
#include "pae_addr.h"
#include "pae_arp.h"



static ssize_t pae_dev_attr_action_stats_store(struct device *dev, struct device_attribute *attr,
                                               const char *buf, size_t count);

static ssize_t pae_dev_attr_action_stats_show(struct device *dev,
                                              struct device_attribute *devattr, char *buf);

dev_attr_action_stats_t dev_attr_action_stats[PAE_NUM_ACTIONS][NUM_STATS_ATTRS] = {
    {
        {
            .name = "0.hits",
            .dev_attr = { .attr = {
                    .name = dev_attr_action_stats[0][0].name,
                    .mode = S_IRUSR | S_IWUSR,
                },
                          .show = pae_dev_attr_action_stats_show,
                          .store = pae_dev_attr_action_stats_store
            }
        },
        {
            .name = "0.bytes",
            .dev_attr = {
                .attr = {
                    .name = dev_attr_action_stats[0][1].name,
                    .mode = S_IRUSR | S_IWUSR,
                },
                .show = pae_dev_attr_action_stats_show,
                .store = pae_dev_attr_action_stats_store
            }
        },
        {
            .name = "0.elapsed",
            .dev_attr = {
                .attr = {
                    .name = dev_attr_action_stats[0][2].name,
                    .mode = S_IRUSR | S_IWUSR,
                },
                .show = pae_dev_attr_action_stats_show,
                .store = pae_dev_attr_action_stats_store
            }
        },
    }
};


static unsigned pae_default_mtu = 1500;

int pae_add_action_stats_sysfs(int idx)
{
    int i, rv;
    if (dev_attr_action_stats[idx][0].name[0]) {
        /* already in use */
        return -EEXIST;
    }

    sprintf(dev_attr_action_stats[idx][0].name, "%d.hits", idx);
    sprintf(dev_attr_action_stats[idx][1].name, "%d.bytes", idx);
    sprintf(dev_attr_action_stats[idx][2].name, "%d.elapsed", idx);

    for (i = 0; i < NUM_STATS_ATTRS; ++i) {
        sysfs_attr_init(&dev_attr_action_stats[idx].dev_attr.attr);
        dev_attr_action_stats[idx][i].dev_attr.attr.name = dev_attr_action_stats[idx][i].name;
        dev_attr_action_stats[idx][i].dev_attr.attr.mode = S_IRUSR | S_IWUSR;
        dev_attr_action_stats[idx][i].dev_attr.show = pae_dev_attr_action_stats_show;
        dev_attr_action_stats[idx][i].dev_attr.store = pae_dev_attr_action_stats_store;
        
        rv = pae_add_action_stats_attribute(&dev_attr_action_stats[idx][i].dev_attr.attr);
        if (rv) {
            break;
        }
    }
    return rv;
}


void pae_remove_action_stats_sysfs(int idx)
{
    int i;
    
    /* Only delete if it actually exists */
    if (dev_attr_action_stats[idx][0].name[0]) {
        for (i = 0; i < NUM_STATS_ATTRS; ++i) {
            pae_remove_action_stats_attribute(&dev_attr_action_stats[idx][i].dev_attr.attr);
        }
        dev_attr_action_stats[idx][0].name[0] = 0;
    }
}


void pae_set_default_mtu(unsigned mtu)
{
    pae_default_mtu = mtu;
}


int pae_feature_supported(u32 feature_mask)
{
    u32 all_features = pae_get_tcm_u32(R5_FEATURES);
    return ((all_features & feature_mask) == feature_mask);
}


unsigned pae_do_cmd(unsigned cmd)
{
    int i;

    if (pae_status == PAE_STOPPED) {
        printk(KERN_WARNING "PAE is stopped, cannot execute command\n");
        return -ENXIO;
    }
    pae_set_tcm_u32(PAE_CMD_RESP_ADDR, PAE_CMD_RESP_PENDING);
    pae_set_tcm_u32(PAE_CMD_TYPE_ADDR, cmd);

    for (i = 0; i < PAE_MAX_CMD_RESP_ITER; ++i) {
        if (pae_get_tcm_u32(PAE_CMD_TYPE_ADDR) == 0) {
            break;
        }
        udelay(10);
    }

    if (pae_get_tcm_u32(PAE_CMD_TYPE_ADDR) != 0) {
        printk(KERN_ERR "No response from PAE for command %u\n", cmd);
        return -ETIMEDOUT;
    }

    return pae_get_tcm_u32(PAE_CMD_RESP_ADDR);
}


int pae_add_action_with_mtu(unsigned idx, unsigned action_type, unsigned mtu, u8 *action_data, unsigned action_data_len_bytes)
{
    unsigned action_data_len_words = (action_data_len_bytes + 3) >> 2;
    int i;
    int rv;

    if (pae_status == PAE_STOPPED) {
        printk(KERN_WARNING "PAE is stopped, cannot execute command\n");
        return -ENXIO;
    }

    if (action_data_len_bytes > PAE_CMD_BUF_SIZE - 8) {
        return -EINVAL;
    }

    pae_set_tcm_u32(PAE_CMD_BUF_ADDR, idx);
    pae_set_tcm_u8(PAE_CMD_BUF_ADDR + 4, action_type);
    pae_set_tcm_u8(PAE_CMD_BUF_ADDR + 5, action_data_len_words + 1); // +1 for action header word

    pae_set_tcm_u16(PAE_CMD_BUF_ADDR + 6, mtu);

    /* To avoid leaking previous contents in case the data isn't */
    /* an even number of words, zero the last word */
    pae_set_tcm_u32(PAE_CMD_BUF_ADDR + 8 + (4 * (action_data_len_bytes / 4)), 0);

    for (i = 0; i < action_data_len_bytes; ++i) {
        pae_set_tcm_u8(PAE_CMD_BUF_ADDR + 8 + i, action_data[i]);
    }

    rv = pae_do_cmd(PAE_ACTION_ADD);
    if (rv != 0) {
        return rv;
    }

    return pae_add_action_stats_sysfs(idx);
}


int pae_add_action(unsigned idx, int action_type, u8 *action_data, unsigned action_data_len_bytes)
{
    return pae_add_action_with_mtu(idx, action_type, pae_default_mtu, action_data, action_data_len_bytes);
}


int pae_delete_action(unsigned idx)
{
    int rv;

    if (pae_status == PAE_STOPPED) {
        printk(KERN_WARNING "PAE is stopped, cannot execute command\n");
        return -ENXIO;
    }

    pae_set_tcm_u32(PAE_CMD_BUF_ADDR, idx);

    rv = pae_do_cmd(PAE_ACTION_DELETE);
    if (rv == 0) {
        pae_remove_action_stats_sysfs(idx);
    }
    return rv;
}


int pae_update_action(unsigned idx, unsigned offset, u8 *updated_data, unsigned update_len_bytes)
{
    int i;

    if (pae_status == PAE_STOPPED) {
        printk(KERN_WARNING "PAE is stopped, cannot execute command\n");
        return -ENXIO;
    }

    pae_set_tcm_u32(PAE_CMD_BUF_ADDR, idx);
    pae_set_tcm_u32(PAE_CMD_BUF_ADDR + 4, offset);
    pae_set_tcm_u32(PAE_CMD_BUF_ADDR + 8, update_len_bytes);

    /* To avoid leaking previous contents in case the data isn't */
    /* an even number of words, zero the last word */
    pae_set_tcm_u32(PAE_CMD_BUF_ADDR + 12 + (4 * (update_len_bytes / 4)), 0);

    for (i = 0; i < update_len_bytes; ++i) {
        pae_set_tcm_u8(PAE_CMD_BUF_ADDR + 12 + i, updated_data[i]);
    }

    return pae_do_cmd(PAE_ACTION_UPDATE);
}

int pae_get_action_stats(unsigned idx, pae_action_stats_t *stats)
{
    if (pae_status == PAE_STOPPED) {
        printk(KERN_WARNING "PAE is stopped, cannot execute command\n");
        return -ENXIO;
    }

    if (idx >= PAE_NUM_ACTIONS) {
        return -EINVAL;
    }

    stats->bytes = pae_get_tcm_u32(ACTION_PERF_BASE +
                                   idx * ACTION_PERF_SIZE_PER +
                                   ACTION_PERF_BYTES_OFFSET);

    stats->packets = pae_get_tcm_u32(ACTION_PERF_BASE +
                                     idx * ACTION_PERF_SIZE_PER
                                     + ACTION_PERF_PACKETS_OFFSET);

    stats->last_hit_time_r5  = pae_get_tcm_u32(ACTION_PERF_BASE +
                                               idx * ACTION_PERF_SIZE_PER
                                               + ACTION_PERF_HIT_TIME_OFFSET);
    return 0;
}

int pae_add_tuple_rule(uint8_t ip_version, uint8_t *src_addr, uint8_t *dest_addr, 
    uint16_t src_port, uint16_t dest_port, uint8_t protocol, uint32_t action_idx, uint32_t flags)
{
    unsigned cmd,addr_bytes,rv;
    uint8_t curs = 0;
    uint8_t i;
    uint32_t proto_flags = (uint32_t)protocol | (flags<<8);

    if (!pae_feature_supported(PAE_FEATURE_BIT_ANY_TUPLE_HASH)) {
        return -EINVAL;
    }

    switch (ip_version) {
    case 4:
        cmd = PAE_TUPLE_RULE_ADD_IPV4;
        addr_bytes = 4;
        break;
    case 6:
        cmd = PAE_TUPLE_RULE_ADD_IPV6;
        addr_bytes = 16;
        break;
    default :
        return -EINVAL;
        /* NOTREACHED */
    }

    pae_set_tcm_u32(PAE_CMD_BUF_ADDR + curs, action_idx);
    curs += 4;

    for (i = 0; i < addr_bytes; ++i) {
        pae_set_tcm_u8(PAE_CMD_BUF_ADDR + curs++, src_addr[i]);
    }
    for (i = 0; i < addr_bytes; ++i) {
        pae_set_tcm_u8(PAE_CMD_BUF_ADDR + curs++, dest_addr[i]);
    }

    pae_set_tcm_u16(PAE_CMD_BUF_ADDR + curs, htons(src_port));
    curs += 2;
    pae_set_tcm_u16(PAE_CMD_BUF_ADDR + curs, htons(dest_port));
    curs += 2;
    pae_set_tcm_u32(PAE_CMD_BUF_ADDR + curs, proto_flags);

    rv = pae_do_cmd(cmd);
    return rv;
}

int pae_delete_tuple_rule(uint8_t ip_version, uint8_t *src_addr, uint8_t *dest_addr, 
    uint16_t src_port, uint16_t dest_port, uint8_t protocol, uint32_t flags)
{
    unsigned cmd,addr_bytes,rv;
    uint8_t curs = 0;
    uint8_t i;
    uint32_t proto_flags = (uint32_t)protocol | (flags << 8);

    if (!pae_feature_supported(PAE_FEATURE_BIT_ANY_TUPLE_HASH)) {
        return -EINVAL;
    }

    switch (ip_version) {
    case 4:
        cmd = PAE_TUPLE_RULE_DELETE_IPV4;
        addr_bytes = 4;
        break;
    case 6:
        cmd = PAE_TUPLE_RULE_DELETE_IPV6;
        addr_bytes = 16;
        break;
    default :
        return -EINVAL;
        /* NOTREACHED */
    }

    for (i = 0; i < addr_bytes; ++i) {
        pae_set_tcm_u8(PAE_CMD_BUF_ADDR + curs++, src_addr[i]);
    }
    for (i = 0; i < addr_bytes; ++i) {
        pae_set_tcm_u8(PAE_CMD_BUF_ADDR + curs++, dest_addr[i]);
    }

    pae_set_tcm_u16(PAE_CMD_BUF_ADDR + curs, htons(src_port));
    curs += 2;
    pae_set_tcm_u16(PAE_CMD_BUF_ADDR + curs, htons(dest_port));
    curs += 2;
    pae_set_tcm_u32(PAE_CMD_BUF_ADDR + curs, proto_flags);

    rv = pae_do_cmd(cmd);
    return rv;
}


int pae_set_tuple_mask(uint8_t ip_version, uint8_t *src_addr, uint8_t *dest_addr, 
    uint16_t src_port, uint16_t dest_port, uint8_t protocol)
{
    unsigned cmd,addr_bytes,rv;
    uint8_t curs = 0;
    uint8_t i;

    if (pae_status == PAE_STOPPED) {
        printk(KERN_WARNING "PAE is stopped, cannot execute command\n");        
        return 0;
    }

    if (!pae_feature_supported(PAE_FEATURE_BIT_MASKED_TUPLE_HASH)) {
        return -EINVAL;
    }

    switch (ip_version) {
    case 4:
        cmd = PAE_TUPLE_MASK_SET_IPV4;
        addr_bytes = 4;
        break;
    case 6:
        cmd = PAE_TUPLE_MASK_SET_IPV6;
        addr_bytes = 16;
        break;
    default :
        return -EINVAL;
        /* NOTREACHED */
    }

    for (i = 0; i < addr_bytes; ++i) {
        pae_set_tcm_u8(PAE_CMD_BUF_ADDR + curs++, src_addr[i]);
    }
    for (i = 0; i < addr_bytes; ++i) {
        pae_set_tcm_u8(PAE_CMD_BUF_ADDR + curs++, dest_addr[i]);
    }

    pae_set_tcm_u16(PAE_CMD_BUF_ADDR + curs, htons(src_port));
    curs += 2;
    pae_set_tcm_u16(PAE_CMD_BUF_ADDR + curs, htons(dest_port));
    curs += 2;

    pae_set_tcm_u32(PAE_CMD_BUF_ADDR + curs, protocol);
    curs += 4;

    rv = pae_do_cmd(cmd);
    return rv;
}

int pae_get_times(uint32_t *cur_time, uint32_t *idle_time)
{
    if (pae_status == PAE_STOPPED) {
        printk(KERN_WARNING "PAE is stopped, cannot execute command\n");
        return -ENXIO;
    }

    *cur_time = pae_get_tcm_u32(R5_CUR_TIME);
    *idle_time = pae_get_tcm_u32(R5_IDLE_TIME);
    
    return 0;
}


uint32_t pae_read_memory(uint32_t addr)
{
    if (pae_status == PAE_STOPPED) {
        printk(KERN_WARNING "PAE is stopped, cannot execute command\n");
        return -ENXIO;
    }

    pae_set_tcm_u32(PAE_CMD_BUF_ADDR, addr);
    return pae_do_cmd(PAE_MEMORY_READ);
}


uint32_t pae_write_memory(uint32_t addr, uint32_t val)
{
    if (pae_status == PAE_STOPPED) {
        printk(KERN_WARNING "PAE is stopped, cannot execute command\n");
        return -ENXIO;
    }

    pae_set_tcm_u32(PAE_CMD_BUF_ADDR, addr);
    pae_set_tcm_u32(PAE_CMD_BUF_ADDR + 4, val);
    return pae_do_cmd(PAE_MEMORY_WRITE);
}


static unsigned parse_unsigned(const char *buf, size_t count, const char **remaining_buf, size_t *remaining_count, unsigned *value)
{
    char tmp_buf[16];
    int tmp_size=0;
    int got_nonspace = 0;

    const char *curs = buf;
    int rv;

    *remaining_count = count;
    while (*remaining_count && tmp_size < sizeof(tmp_buf) && *curs) {
        if (!isspace(*curs)) {
            tmp_buf[tmp_size++] = *curs;
            got_nonspace = 1;
        } else if (got_nonspace) {
            // break on space once we've seen something usable
            break;
        }
        curs++;
        --*remaining_count;
    }

    *remaining_buf = curs;

    if (tmp_size == sizeof(tmp_buf)) {
        printk(KERN_INFO "Number length too big\n");
        /* number was bigger than is sensible, so just bail */
        return -EINVAL;
    }
    
    if (!got_nonspace) {
        /* no non-whitespace */
        return -EINVAL;
    }

    tmp_buf[tmp_size] = 0;  /* null terminate */
    if ((rv = kstrtouint(tmp_buf, 0, value))) {
        printk(KERN_INFO "Invalid number\n");
        return -EINVAL;
    }

    /* printk("Returning value of %u\n", *value); */
    *remaining_buf = curs;
    
    return 0;
}


static unsigned parse_unsigned_ext(const char *buf, size_t count, const char **remaining_buf, size_t *remaining_count, uint32_t *value, int *size_bytes)
{
    char tmp_buf[16];
    int tmp_size=0;
    int got_nonspace = 0;

    const char *curs = buf;
    int rv;
    
    *remaining_count = count;
    while (*remaining_count && tmp_size < sizeof(tmp_buf) && *curs) {
        if (!isspace(*curs)) {
            tmp_buf[tmp_size++] = *curs;
            got_nonspace = 1;
        } else if (got_nonspace) {
            // break on space once we've seen something usable
            break;
        }
        curs++;
        --*remaining_count;
    }

    *remaining_buf = curs;

    if (tmp_size == sizeof(tmp_buf)) {
        printk(KERN_INFO "Number length too big\n");
        /* number was bigger than is sensible, so just bail */
        return -EINVAL;
    }
    
    if (!got_nonspace) {
        /* no non-whitespace */
        return -EINVAL;
    }

    tmp_buf[tmp_size] = 0;  /* null terminate */
    if ((rv = kstrtouint(tmp_buf, 0, value))) {
        printk(KERN_INFO "Invalid number\n");
        return -EINVAL;
    }

    if (tmp_buf[0] == '0' && tmp_buf[1] == 'x') {
        *size_bytes = (tmp_size - 2 + 1) / 2;
    } else {
        *size_bytes = 1;
    }

    /* printk("Returning value of %u\n", *value); */
    *remaining_buf = curs;
    
    return 0;
}


uint8_t hexdigittoint(char d) 
{
    return ((d) <= '9' ? (d) - '0' : (tolower(d)) - 'a' + 10);
}

// convert arbitrary length hex bytes into network ordered byte array
static int hex2mem(const char *src, uint8_t *dest, size_t source_count, size_t dest_count)
{
    int digits;
    uint8_t nibble;
    uint8_t low;

    if (memchr(src,' ',source_count)) {
        digits = (int)(memchr(src,' ',source_count) - (void *)src);
    } else {
        digits = source_count;
    }
    
    if ((!digits) || (!source_count) || (!dest_count)) {
        return 0;
    }

    memset(dest,0,dest_count);

    low = digits & 0x01;
    // odd number of bytes require some special handling
    if (low) 
    {
        nibble = hexdigittoint(*src++);
        dest[(dest_count) - digits/2 - low] |= nibble;
        digits--;
        source_count--;
        low = 0;
    }

    while (source_count--) {
        nibble = hexdigittoint(*src++);
        if (((int)dest_count - digits/2) < 0) {
            return 0;
        }
        if (low) {
            dest[(dest_count) - digits/2] |= nibble;
            digits -= 2;
        } else {
            dest[(dest_count) - digits/2] |= (nibble << 4);
        }
        low = !low;
    }

    return digits;
}

static void skip_whitespace(const char **remaining_buf, size_t *remaining_count)
{
    size_t count = *remaining_count;
    const char *curs = *remaining_buf;

    while(count && isspace(*curs)) {
        curs++;
        count--;
    }

    *remaining_count = count;
    *remaining_buf = curs;
}

static void skip_nonwhitespace(const char **remaining_buf, size_t *remaining_count)
{
    size_t count = *remaining_count;
    const char *curs = *remaining_buf;

    while(count && (!isspace(*curs))) {
        curs++;
        count--;
    }

    *remaining_count = count;
    *remaining_buf = curs;
}

/* ADD ACTION SYSFS */

/* the result of the last add command, so it can be returned if/when the file is read */
static int add_cmd_result;

/* echo $idx $type $mtu $action_data > /sys/devices/platform/bcmiproc-pae/action-add */

static ssize_t write_add_action(struct device *dev, struct device_attribute *attr,
                                const char *buf, size_t count)
{
    const char *curs = buf;
    unsigned idx, action_type, mtu;
    u8 action_buf[PAE_CMD_BUF_SIZE - 8];
    uint32_t val;
    unsigned action_len = 0;
    size_t count_left = count;
    int size_bytes;

    if (pae_status == PAE_STOPPED) {
        printk(KERN_WARNING "PAE is stopped, cannot execute command\n");        
        return 0;
    }

    if (parse_unsigned(curs, count_left, &curs, &count_left, &idx)) {
        printk(KERN_INFO "Bad index\n");
        add_cmd_result = -1;
        return -EINVAL;
    }

    if (parse_unsigned(curs, count_left, &curs, &count_left, &action_type)) {
        printk(KERN_INFO "Bad action type\n");
        add_cmd_result = -1;
        return -EINVAL;
    }

    if (parse_unsigned(curs, count_left, &curs, &count_left, &mtu)) {
        printk(KERN_INFO "Bad MTU\n");
        add_cmd_result = -1;
        return -EINVAL;
    }

    while (parse_unsigned_ext(curs, count_left, &curs, &count_left, &val, &size_bytes) == 0) {
        int i;
        if (size_bytes < 0 || (val > (unsigned)((1 << (size_bytes*8)) - 1))) {
            printk(KERN_INFO "Bad action data: must be single bytes (in decimal), or padded hex (0x01, 0x0123, 0x012345678)\n");
            break;
        }
        
        for (i = 0; i < size_bytes; ++i) {
            action_buf[action_len] = (val & 0xff);
            val >>= 8;
            ++action_len;
        }
    }

    add_cmd_result = pae_add_action_with_mtu(idx, action_type, mtu, action_buf, action_len);

    printk("Added action.  Result: %d\n", add_cmd_result);

    return count;
}


static ssize_t read_add_action(struct device *dev,
                               struct device_attribute *devattr, char *buf)
{
    return sprintf(buf, "%d\n", add_cmd_result);
}


struct device_attribute dev_attr_pae_action_add =
    __ATTR(action_add, S_IRUSR | S_IWUSR, read_add_action, write_add_action);


/* DELETE ACTION SYSFS */

/* the result of the last delete command, so it can be returned if/when the file is read */
static int delete_cmd_result;

static ssize_t write_delete_action(struct device *dev, struct device_attribute *attr,
                                   const char *buf, size_t count)
{
    const char *curs = buf;
    unsigned count_left = count;
    unsigned idx;
    
    if (pae_status == PAE_STOPPED) {
        printk(KERN_WARNING "PAE is stopped, cannot execute command\n");        
        return 0;
    }

    if (parse_unsigned(curs, count_left, &curs, &count_left, &idx)) {
        printk(KERN_INFO "Bad index\n");
        delete_cmd_result = -1;
        return -EINVAL;
    }

    delete_cmd_result = pae_delete_action(idx);

    printk("Deleted action.  Result: %d\n", delete_cmd_result);

    return count;
}


static ssize_t read_delete_action(struct device *dev,
                               struct device_attribute *devattr, char *buf)
{
    return sprintf(buf, "%d\n", delete_cmd_result);
}


struct device_attribute dev_attr_pae_action_delete =
    __ATTR(action_delete, S_IRUSR | S_IWUSR, read_delete_action, write_delete_action);


/* UPDATE ACTION SYSFS */

/* the result of the last update command, so it can be returned if/when the file is read */
static int update_cmd_result;

static ssize_t write_update_action(struct device *dev, struct device_attribute *attr,
                                const char *buf, size_t count)
{
    const char *curs = buf;
    unsigned idx, offset;
    u8 update_buf[PAE_CMD_BUF_SIZE - 12];
    unsigned val, update_len = 0;
    size_t count_left = count;

    if (pae_status == PAE_STOPPED) {
        printk(KERN_WARNING "PAE is stopped, cannot execute command\n");        
        return 0;
    }

    if (parse_unsigned(curs, count_left, &curs, &count_left, &idx)) {
        printk(KERN_INFO "Bad index\n");
        add_cmd_result = -1;
        return -EINVAL;
    }

    if (parse_unsigned(curs, count_left, &curs, &count_left, &offset)) {
        printk(KERN_INFO "Bad offset\n");
        add_cmd_result = -1;
        return -EINVAL;
    }

    while (parse_unsigned(curs, count_left, &curs, &count_left, &val) == 0) {
        if (val > 255) {
            printk(KERN_INFO "Update data must be bytes (value was %d)\n", val);
            break;
        }
        update_buf[update_len++] = val;
    }

    add_cmd_result = pae_update_action(idx, offset, update_buf, update_len);

    printk("Updated action.  Result: %d\n", add_cmd_result);

    return count;
}


static ssize_t read_update_action(struct device *dev,
                                  struct device_attribute *devattr, char *buf)
{
    return sprintf(buf, "%d\n", update_cmd_result);
}


static int parse_tuple(const char **curs, size_t *count_left,
                       unsigned *ip_version,
                       uint8_t src_addr_ipv4[4],
                       uint8_t dest_addr_ipv4[4],
                       uint8_t src_addr_ipv6[16],
                       uint8_t dest_addr_ipv6[16],
                       unsigned *src_port, unsigned *dest_port,
                       unsigned *protocol)
{
    unsigned addr_len = 0;

    if (parse_unsigned(*curs, *count_left, curs, count_left, ip_version)) {
        printk(KERN_INFO "Bad IP version\n");
        return -EINVAL;
    }

    if (*ip_version == 4) {
        skip_whitespace(curs, count_left);
        if (!*count_left) {
                printk(KERN_INFO "Bad Input\n");
                return -EINVAL;
        }
        if (memchr(*curs,'.', *count_left)) {
            // ipv4 string address
            if (pae_parse_ipv4(*curs, src_addr_ipv4) == 0) {
                printk(KERN_INFO "Invalid source address\n");
                return -EINVAL;
            }
            skip_nonwhitespace(curs, count_left);
            skip_whitespace(curs, count_left);
            if (!*count_left) {
                    printk(KERN_INFO "Bad Input\n");
                    return -EINVAL;
            }
            if (pae_parse_ipv4(*curs, dest_addr_ipv4) == 0) {
                printk(KERN_INFO "Invalid destination address\n");
                return -EINVAL;
            }
            skip_nonwhitespace(curs, count_left);
            skip_whitespace(curs, count_left);
            if (!*count_left) {
                    printk(KERN_INFO "Bad Input\n");
                    return -EINVAL;
            }
        } else {
            // ipv4 hex address
            if (memchr(*curs,' ',*count_left)) {
                addr_len = (const char *)memchr(*curs, ' ', *count_left) - *curs - 1;
            } else {
                printk(KERN_INFO "Invalid source address\n");
                return -EINVAL;
            }
            hex2mem(*curs, src_addr_ipv4, addr_len, sizeof(src_addr_ipv4));
            *curs += (addr_len + 1);
            *count_left -= (addr_len + 1);

            skip_whitespace(curs,count_left);

            if (memchr(*curs,' ', *count_left)) {
                addr_len = (const char *)memchr(*curs, ' ', *count_left) - *curs - 1;
            } else {
                printk(KERN_INFO "Invalid destination address\n");
                return -EINVAL;
            }
            hex2mem(*curs,dest_addr_ipv4,addr_len,sizeof(dest_addr_ipv4));
            *curs += (addr_len + 1);
            *count_left -= (addr_len + 1);
        }
    } else if (*ip_version == 6) {
        skip_whitespace(curs, count_left);
        if (memchr(*curs, ':', *count_left)) {
            // ipv6 string address
            if (pae_parse_ipv6(*curs, src_addr_ipv6) == 0) {
                printk(KERN_INFO "Invalid source address\n");
                return -EINVAL;
            }
            skip_nonwhitespace(curs, count_left);
            skip_whitespace(curs, count_left);
            if (!*count_left) {
                    printk(KERN_INFO "Bad Input\n");
                    return -EINVAL;
            }
            if (pae_parse_ipv6(*curs, dest_addr_ipv6) == 0) {
                printk(KERN_INFO "Invalid destination address\n");
                return -EINVAL;
            }
            skip_nonwhitespace(curs, count_left);
            skip_whitespace(curs, count_left);
            if (!*count_left) {
                    printk(KERN_INFO "Bad Input\n");
                    return -EINVAL;
            }
        } else {
            if (memchr(*curs, ' ', *count_left)) {
                addr_len = (const char *)memchr(*curs, ' ', *count_left) - *curs - 1;
            } else {
                printk(KERN_INFO "Invalid source address\n");
                return -EINVAL;
            }
            hex2mem(*curs, src_addr_ipv6, addr_len, sizeof(src_addr_ipv6));
            *curs += (addr_len + 1);
            *count_left -= (addr_len + 1);

            skip_whitespace(curs, count_left);

            if (memchr(*curs, ' ', *count_left)) {
                addr_len = (const char *)memchr(*curs, ' ', *count_left) - *curs - 1;
            } else {
                printk(KERN_INFO "Invalid destination address\n");
                return -EINVAL;
            }
            hex2mem(*curs, dest_addr_ipv6, addr_len, sizeof(dest_addr_ipv6));
            *curs += (addr_len + 1);
            *count_left -= (addr_len + 1);
        }
    } else {
        printk(KERN_INFO "Invalid IP version\n");
        return -EINVAL;
    }

    if (parse_unsigned((const char *)*curs, *count_left, curs, count_left, src_port)) {
        printk(KERN_INFO "Bad source port\n");
        add_cmd_result = -1;
        return -EINVAL;
    }

    if (parse_unsigned((const char *)*curs, *count_left, curs, count_left, dest_port)) {
        printk(KERN_INFO "Bad destination port\n");
        add_cmd_result = -1;
        return -EINVAL;
    }

    if (parse_unsigned((const char *)*curs, *count_left, curs, count_left, protocol)) {
        printk(KERN_INFO "Bad protocol\n");
        add_cmd_result = -1;
        return -EINVAL;
    }

    return 0;
}

struct device_attribute dev_attr_pae_action_update =
    __ATTR(action_update, S_IRUSR | S_IWUSR, read_update_action, write_update_action);


/* ADD TUPLE_RULE SYSFS */
/* the result of the last update command, so it can be returned if/when the file is read */
static int add_tuple_rule_result;

/* syntax ip_version source_ip dest_ip source_port dest_port protocol action_idx:
4 192.1.1.1 192.2.2.2 22 22 50 1
6 fe80::2aa:aa:4ca2 fe80::2aa:aa:3000 22 22 50 1
4 c0010101 c0020202 22 22 50 1
6 fe80000000000000000002aa00aa4ca2 fe80000000000000000002aa00aa3000 22 22 50 1
*/
static ssize_t write_tuple_rule_add(struct device *dev, struct device_attribute *attr,
                              const char *buf, size_t count)
{
    const char *curs = buf;
    size_t count_left = count;
    unsigned ip_version;
    uint8_t src_addr_ipv4[4];
    uint8_t dest_addr_ipv4[4];
    uint8_t src_addr_ipv6[16];
    uint8_t dest_addr_ipv6[16];
    unsigned src_port, dest_port, protocol, action_idx, flags;

    add_tuple_rule_result = -1;

    if (pae_status == PAE_STOPPED) {
        printk(KERN_WARNING "PAE is stopped, cannot execute command\n");        
        return 0;
    }

    add_tuple_rule_result = parse_tuple(&curs, &count_left, &ip_version, src_addr_ipv4, dest_addr_ipv4,
                                        src_addr_ipv6, dest_addr_ipv6, &src_port, &dest_port, &protocol);

    if (add_tuple_rule_result) {
        return -EINVAL;
    }

    if ((protocol != IPPROTO_TCP) && (protocol != IPPROTO_UDP) && (protocol != IPPROTO_ESP) && (protocol != IPPROTO_AH)) {
        printk(KERN_INFO "Unsupported protocol : %d\n", protocol);
        add_tuple_rule_result = -1;
        return -EINVAL;
    }

    if (parse_unsigned((const char *)curs, count_left, &curs, &count_left, &action_idx)) {
        printk(KERN_INFO "Bad action index\n");
        add_tuple_rule_result = -1;
        return -EINVAL;
    }

    if (parse_unsigned((const char *)curs, count_left, &curs, &count_left, &flags)) {
        flags = 0;
    }

    if (ip_version == 4) {
        add_tuple_rule_result = pae_add_tuple_rule((uint8_t)ip_version, src_addr_ipv4, dest_addr_ipv4,
            (uint16_t)src_port, (uint16_t)dest_port, (uint8_t)protocol, action_idx, flags);
    } else {
        add_tuple_rule_result = pae_add_tuple_rule((uint8_t)ip_version, src_addr_ipv6, dest_addr_ipv6,
            (uint16_t)src_port, (uint16_t)dest_port, (uint8_t)protocol, action_idx, flags);
    }
    printk(KERN_INFO "Add Tuple_Rule.  Result: %d\n", add_tuple_rule_result);

    return count;
}

static ssize_t read_tuple_rule_add(struct device *dev,
                                  struct device_attribute *devattr, char *buf)
{
    return sprintf(buf, "%d\n", add_tuple_rule_result);
}

struct device_attribute dev_attr_pae_tuple_rule_add =
    __ATTR(tuple_rule_add, S_IRUSR | S_IWUSR, read_tuple_rule_add, write_tuple_rule_add);


static int delete_tuple_rule_result;

static ssize_t write_tuple_rule_delete(struct device *dev, struct device_attribute *attr,
                                   const char *buf, size_t count)
{
    const char *curs = buf;
    size_t count_left = count;
    unsigned ip_version;
    uint8_t src_addr_ipv4[4];
    uint8_t dest_addr_ipv4[4];
    uint8_t src_addr_ipv6[16];
    uint8_t dest_addr_ipv6[16];
    unsigned src_port, dest_port, protocol, flags;
    
    delete_tuple_rule_result = -1;

    if (pae_status == PAE_STOPPED) {
        printk(KERN_WARNING "PAE is stopped, cannot execute command\n");        
        return 0;
    }

    delete_tuple_rule_result = parse_tuple(&curs, &count_left, &ip_version, src_addr_ipv4, dest_addr_ipv4,
                                   src_addr_ipv6, dest_addr_ipv6, &src_port, &dest_port, &protocol);

    if (delete_tuple_rule_result) {
        return -EINVAL;
    }

    if ((protocol != IPPROTO_TCP) && (protocol != IPPROTO_UDP) && (protocol != IPPROTO_ESP) && (protocol != IPPROTO_AH)) {
        printk(KERN_INFO "Unsupported protocol : %d\n", protocol);
        add_tuple_rule_result = -1;
        return -EINVAL;
    }

    if (parse_unsigned((const char *)curs, count_left, &curs, &count_left, &flags)) {
        flags = 0;
    }

    if (ip_version == 4) {
        delete_tuple_rule_result = pae_delete_tuple_rule((uint8_t)ip_version, src_addr_ipv4, dest_addr_ipv4,
            (uint16_t)src_port, (uint16_t)dest_port, (uint8_t)protocol, flags);
    } else {
        delete_tuple_rule_result = pae_delete_tuple_rule((uint8_t)ip_version, src_addr_ipv6, dest_addr_ipv6,
            (uint16_t)src_port, (uint16_t)dest_port, (uint8_t)protocol, flags);
    }
    printk(KERN_INFO "Deleted Tuple_Rule.  Result: %d\n", delete_tuple_rule_result);

    return count;
}

static ssize_t read_tuple_rule_delete(struct device *dev,
                                  struct device_attribute *devattr, char *buf)
{
    return sprintf(buf, "%d\n", delete_tuple_rule_result);
}

struct device_attribute dev_attr_pae_tuple_rule_delete =
    __ATTR(tuple_rule_delete, S_IRUSR | S_IWUSR, read_tuple_rule_delete, write_tuple_rule_delete);


/* Tuple Mask SYSFS */

/* syntax ip_version source_ip dest_ip source_port dest_port protocol:
4 192.1.1.1 192.2.2.2 22 22 50
6 fe80::2aa:aa:4ca2 fe80::2aa:aa:3000 22 22 50
4 c0010101 c0020202 22 22 50
6 fe80000000000000000002aa00aa4ca2 fe80000000000000000002aa00aa3000 22 22 50
*/
static ssize_t write_tuple_mask(struct device *dev, struct device_attribute *attr,
                                    const char *buf, size_t count)
{
    const char *curs = buf;
    size_t count_left = count;
    unsigned ip_version;
    uint8_t src_addr_ipv4[4];
    uint8_t dest_addr_ipv4[4];
    uint8_t src_addr_ipv6[16];
    uint8_t dest_addr_ipv6[16];
    unsigned src_port, dest_port, protocol;
    int rv = -1;

    if (pae_status == PAE_STOPPED) {
        printk(KERN_WARNING "PAE is stopped, cannot execute command\n");        
        return 0;
    }

    rv = parse_tuple(&curs, &count_left, &ip_version, src_addr_ipv4, dest_addr_ipv4,
                     src_addr_ipv6, dest_addr_ipv6, &src_port, &dest_port, &protocol);

    if (rv) {
        return add_tuple_rule_result;
    }

    if (ip_version == 4) {
        rv = pae_set_tuple_mask((uint8_t)ip_version, src_addr_ipv4, dest_addr_ipv4,
            (uint16_t)src_port, (uint16_t)dest_port, (uint8_t)protocol);
    } else {
        rv = pae_set_tuple_mask((uint8_t)ip_version, src_addr_ipv6, dest_addr_ipv6,
            (uint16_t)src_port, (uint16_t)dest_port, (uint8_t)protocol);
    }
    printk(KERN_INFO "Set Tuple Mask.  Result: %d\n", rv);

    return count;
}

static ssize_t read_tuple_mask(struct device *dev,
                               struct device_attribute *devattr, char *buf)
{
    return 0;
}

struct device_attribute dev_attr_pae_tuple_mask =
    __ATTR(tuple_mask, S_IRUSR | S_IWUSR, read_tuple_mask, write_tuple_mask);


/* Action statistics SysFS */

static ssize_t pae_dev_attr_action_stats_store(struct device *dev, struct device_attribute *attr,
                                               const char *buf, size_t count)
{
    return count;
}

static ssize_t pae_dev_attr_action_stats_show(struct device *dev,
                                              struct device_attribute *devattr, char *buf)
{
    /* Use the attribute's name to determine what to return */
    /* e.g. 0.hits, 35.bytes, 100.elapsed */
    int rv;
    int idx;
    const char *periodpos = 0;
    const char *remaining;
    size_t remaining_count;
    pae_cumulative_action_stats_t stats;
    
    if (pae_status == PAE_STOPPED) {
        printk(KERN_WARNING "PAE is stopped, cannot execute command\n");        
        return -ENXIO;
    }

    periodpos = strchr(devattr->attr.name, '.');
    if (!periodpos) {
        printk("Bad filename '%s', no '.'\n", devattr->attr.name);
    }

    rv = parse_unsigned(devattr->attr.name, periodpos - devattr->attr.name, &remaining, &remaining_count, &idx);
    if (rv) {
        printk("Could not parse number in '%s'\n", devattr->attr.name);
        return 0;
    }

    rv = pae_get_cumulative_action_stats(idx, &stats);
    if (rv) {
        printk("Failed to get stats\n");
        return 0;
    }
    
    switch (periodpos[1]) {
    case 'h':  /* "hits" */
        return sprintf(buf, "%llu", (long long unsigned)stats.packets);
        break;
    case 'b':  /* "bytes" */
        return sprintf(buf, "%llu", (long long unsigned)stats.bytes);
        break;
    case 'e':  /* "elapsed" */
        {
            uint64_t current_jiffies_64 = get_jiffies_64();
            uint64_t elapsed_jiffies = current_jiffies_64 - stats.last_hit_jiffies;
            unsigned long elapsed_sec = do_div(elapsed_jiffies, HZ);
            unsigned long elapsed_ms = (uint32_t)((elapsed_jiffies - ((uint64_t)elapsed_sec * HZ)) * 1000) / HZ;

            return sprintf(buf, "%lu.%03lu", elapsed_sec, elapsed_ms);
            return 0;
        }
        break;
    default:
        printk("Bad filename '%s' ('%c')\n", devattr->attr.name, remaining[1]);
        return 0;
    }
}


/* MEMORY SYSFS */

/* the result of the last memory command, so it can be returned if/when the file is read */
static unsigned memory_result;

static ssize_t write_memory(struct device *dev, struct device_attribute *attr,
                            const char *buf, size_t count)
{
    const char *curs = buf;
    unsigned count_left = count;
    unsigned addr, val;
    
    if (pae_status == PAE_STOPPED) {
        printk(KERN_WARNING "PAE is stopped, cannot execute command\n");        
        return -ENXIO;
    }

    if (parse_unsigned(curs, count_left, &curs, &count_left, &addr)) {
        printk(KERN_INFO "Bad address\n");
        memory_result = 0;
        return -EINVAL;
    }

    if (parse_unsigned(curs, count_left, &curs, &count_left, &val)) {
        /* No value, treat as memory read */
        memory_result = pae_read_memory(addr);
        printk("Read PAE memory %08x: %08x\n", addr, memory_result);
        return count;
    } else {
        memory_result = pae_write_memory(addr, val);
        printk("Wrote PAE memory %08x: %08x (result %d)\n", addr, val, memory_result);
        return count;
    }
}


static ssize_t read_memory(struct device *dev,
                           struct device_attribute *devattr, char *buf)
{
    return sprintf(buf, "%d\n", memory_result);
}


struct device_attribute dev_attr_pae_memory =
    __ATTR(memory, S_IRUSR | S_IWUSR, read_memory, write_memory);


/* IDLE TIME SYSFS */

static uint64_t cur_time_offset, idle_time_offset;

static ssize_t write_idle_time(struct device *dev, struct device_attribute *attr,
                               const char *buf, size_t count)
{
    if (pae_status == PAE_STOPPED) {
        printk(KERN_WARNING "PAE is stopped, cannot execute command\n");        
        return -ENXIO;
    }

    pae_get_cumulative_times(&cur_time_offset, &idle_time_offset);
    
    return count;
}


static ssize_t read_idle_time(struct device *dev,
                              struct device_attribute *devattr, char *buf)
{
    uint64_t cur_time, idle_time;
    
    if (pae_status == PAE_STOPPED) {
        printk(KERN_WARNING "PAE is stopped, cannot execute command\n");        
        return -ENXIO;
    }

    pae_get_cumulative_times(&cur_time, &idle_time);
    cur_time -= cur_time_offset;
    idle_time -= idle_time_offset;

    return sprintf(buf, "%lld %lld\n", cur_time, idle_time);
}


struct device_attribute dev_attr_pae_idle_time =
    __ATTR(idle, S_IRUSR | S_IWUSR, read_idle_time, write_idle_time);


/*  CPU Load */

static ssize_t write_cpu_load(struct device *dev, struct device_attribute *attr,
                               const char *buf, size_t count)
{
    pae_reset_load();
    return count;
}


static ssize_t read_cpu_load(struct device *dev,
                              struct device_attribute *devattr, char *buf)
{
    uint32_t cur_load_ppm, max_load_ppm;
    uint32_t cur_load_int, cur_load_frac, max_load_int, max_load_frac;

    if (pae_status == PAE_STOPPED) {
        printk(KERN_WARNING "PAE is stopped, cannot execute command\n");        
        return -ENXIO;
    }

    pae_get_load(&cur_load_ppm, &max_load_ppm);
    cur_load_int = cur_load_ppm / 10000;
    cur_load_frac = cur_load_ppm - (cur_load_int * 10000);
    max_load_int = max_load_ppm / 10000;
    max_load_frac = max_load_ppm - (max_load_int * 10000);
    
    
    return sprintf(buf, "%u.%04u %u.%04u\n", cur_load_int, cur_load_frac, max_load_int, max_load_frac);
}


struct device_attribute dev_attr_pae_cpu_load =
    __ATTR(cpu, S_IRUSR | S_IWUSR, read_cpu_load, write_cpu_load);


/* ARP Table Watch */

/*  add an ip address instance to the watch table 
 *  syntax ipaddress mac address  - mix and match formats
 *  examples:
 *      192.1.1.1 78:2b:cb:91:6b:e7 eth0
 *      c0010101 782bcb916be7 wlan0
 */
static ssize_t write_arp_watch(struct device *dev, struct device_attribute *attr,
                               const char *buf, size_t count)
{
    const char delimiters[] = " \r\n,";
    char cmd_buf[count + 1];
    char *tok;
    char *cmd;
    struct pae_arp_watch_table_entry entry;
    int slot = -1;
    int i;

    if (pae_status == PAE_STOPPED) {
        printk(KERN_WARNING "PAE is stopped, cannot execute command\n");        
        return 0;
    }

    memcpy(cmd_buf,buf,count);
    cmd_buf[count] = 0;
    cmd = cmd_buf;

    if (pae_arp_watch_table_num_entries == MAX_PAE_WATCH_ENTRIES) {
        printk(KERN_WARNING "%s: PAE Watch Table Full\n",__func__);
        return -EBUSY;
    }

    /* get ip address */
    do {
        tok = strsep((char **)&cmd,delimiters);
    } while ((tok != NULL) && (!strlen(tok)));

    if (tok == NULL) {
        printk(KERN_INFO "%s Invalid IPv4 Address\n",__func__);
        return -EINVAL;
    }

    if (memchr(tok,'.',strlen(tok))) {
        if (pae_parse_ipv4(tok, entry.ipv4_address) == 0) {
            printk(KERN_INFO "Invalid IPv4 address\n");
            return -EINVAL;
        }
    } else {
        if (strlen(tok)) {
            hex2mem(tok, entry.ipv4_address, strlen(tok), sizeof(entry.ipv4_address));
        } else {
            printk(KERN_INFO "%s Invalid IPv4 Address\n",__func__);
            return -EINVAL;
        }
    }

    /* get mac address */
    do {
        tok = strsep((char **)&cmd,delimiters);
    } while ((tok != NULL) && (!strlen(tok)));

    if (tok == NULL) {
        printk(KERN_INFO "%s Invalid MAC Address\n",__func__);
        return -EINVAL;
    }

    if (memchr(tok,':',strlen(tok))) {
        sscanf(tok, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", 
            &entry.mac_address[0], &entry.mac_address[1], &entry.mac_address[2], 
            &entry.mac_address[3], &entry.mac_address[4], &entry.mac_address[5]);
    } else {
        if (strlen(tok)) {
            hex2mem(tok, entry.mac_address, strlen(tok), sizeof(entry.mac_address));
        } else {
            printk(KERN_INFO "%s Invalid MAC address\n",__func__);
            return -EINVAL;
        }
    }

    /* get interface */
    do {
        tok = strsep((char **)&cmd,delimiters);
    } while ((tok != NULL) && (!strlen(tok)));

    if (tok == NULL) {
        printk(KERN_INFO "%s Invalid Interface\n",__func__);
        return -EINVAL;
    }

    strncpy(entry.dev_name, tok ,sizeof(entry.dev_name));
    memset(&entry.callback_chain,0,sizeof(entry.callback_chain));

    printk(KERN_DEBUG "Added PAE ARP Watch %s: %d.%d.%d.%d - %02x:%02x:%02x:%02x:%02x:%02x",
            entry.dev_name,
            entry.ipv4_address[0],entry.ipv4_address[1],
            entry.ipv4_address[2],entry.ipv4_address[3],
            entry.mac_address[0], entry.mac_address[1], entry.mac_address[2], 
            entry.mac_address[3], entry.mac_address[4], entry.mac_address[5]);

    /*
     * see if ip address / interface is already being watched
     */
    for (i = 0; i < pae_arp_watch_table_num_entries; i++)
    {
        if (memcmp(&pae_arp_watch_table[i].ipv4_address, &entry.ipv4_address, sizeof(entry.ipv4_address)) == 0) {
            if (memcmp(pae_arp_watch_table[i].dev_name, entry.dev_name, strlen(entry.dev_name)) == 0) {
                slot = i;
                break;
            }
        }
    }

    if (slot >= 0) {
        /* try to be accommodating, allow user to update table */
        pae_arp_watch_table[slot] = entry;
    } else {
        /* struct copy */
        pae_arp_watch_table[pae_arp_watch_table_num_entries++] = entry;
    }

    return count;
}

int pae_add_arp_watch(uint8_t *ipv4_address, uint8_t* l2_address, const char *interface)
{
    char buf[128];
    int rc;

    sprintf(buf,"%02x%02x%02x%02x %02x%02x%02x%02x%02x%02x %s",
        ipv4_address[0], ipv4_address[1],
        ipv4_address[2], ipv4_address[3],
        l2_address[0], l2_address[1], l2_address[2],
        l2_address[3], l2_address[4], l2_address[5], interface);

    rc = write_arp_watch(NULL, NULL, buf, strlen(buf));
    if (rc == strlen(buf)) {
        rc = 0;
    } else if (rc > 0) {
        rc = -EINVAL;
    }
    return rc;
}


/*
 * dump the arp watch table
 */
static ssize_t read_arp_watch(struct device *dev,
                              struct device_attribute *devattr, char *buf)
{
    char *curs = buf;
    int i;

    if (pae_status == PAE_STOPPED) {
        printk(KERN_WARNING "PAE is stopped, cannot execute command\n");        
        return 0;
    }

    memset(buf,0,PAGE_SIZE);

    for (i = 0; i < pae_arp_watch_table_num_entries; i++) {
        curs += sprintf(curs,"%s: %d.%d.%d.%d %02x:%02x:%02x:%02x:%02x:%02x\n",
            pae_arp_watch_table[i].dev_name,
            pae_arp_watch_table[i].ipv4_address[0], pae_arp_watch_table[i].ipv4_address[1],
            pae_arp_watch_table[i].ipv4_address[2], pae_arp_watch_table[i].ipv4_address[3],
            pae_arp_watch_table[i].mac_address[0], pae_arp_watch_table[i].mac_address[1],
            pae_arp_watch_table[i].mac_address[2], pae_arp_watch_table[i].mac_address[3],
            pae_arp_watch_table[i].mac_address[4], pae_arp_watch_table[i].mac_address[5]);
    }

    return (ssize_t)(curs-buf);
}

struct device_attribute dev_attr_pae_arp_watch_add =
    __ATTR(arp_watch_add, S_IRUSR | S_IWUSR, read_arp_watch, write_arp_watch);

/*
 * delete a watch entry from the watch table or decrement the reference count if
 * more than one instance of the ip address has been added
 */

static ssize_t remove_arp_watch(struct device *dev, struct device_attribute *attr,
                               const char *buf, size_t count)
{
    int i;
    int slot = -1; 
    const char *tok = buf;
    char *cmd;
    const char delimiters[] = " \r\n,";
    unsigned char ipv4_address[4];
    char cmd_buf[count + 1];

    if (pae_status == PAE_STOPPED) {
        printk(KERN_WARNING "PAE is stopped, cannot execute command\n");        
        return 0;
    }

    memcpy(cmd_buf,buf,count);
    cmd_buf[count] = 0;
    cmd = cmd_buf;

    /* get ip address */
    do {
        tok = strsep((char **)&cmd,delimiters);
    } while ((tok != NULL) && (!strlen(tok)));

    if (tok == NULL) {
        printk(KERN_INFO "%s Invalid IPv4 Address\n",__func__);
        return -EINVAL;
    }
    
    if (memchr(tok,'.',count)) {
        if (pae_parse_ipv4(tok, ipv4_address) == 0) {
            printk(KERN_INFO "Invalid IPv4 address\n");
            return -EINVAL;
        }
    } else {
        if (count) {
            hex2mem(tok, ipv4_address, count, sizeof(ipv4_address));
        } else {
            printk(KERN_INFO "%s Invalid IPv4 Address\n",__func__);
            return -EINVAL;
        }
    }

    /* get interface */
    do {
        tok = strsep((char **)&cmd,delimiters);
    } while ((tok != NULL) && (!strlen(tok)));

    if (tok == NULL) {
        printk(KERN_INFO "%s Invalid Interface\n",__func__);
        return -EINVAL;
    }

    for (i = 0; i < pae_arp_watch_table_num_entries; i++) {
        if (memcmp(pae_arp_watch_table[i].ipv4_address, ipv4_address, sizeof(ipv4_address)) == 0) {
            if (memcmp(pae_arp_watch_table[i].dev_name, tok, strlen(tok)) == 0) {
                slot = i;
                break;
            }
        }
    }

    if (slot >= 0) {
        /* someone else is watching this ip / or someone did not clean up properly */
        if (pae_arp_watch_table[slot].callback_chain.head != NULL) {
            printk(KERN_WARNING "%s: Attempt to free arp table watch with active callbacks\n",__func__);
            return -EBUSY;
        }
        if (slot != MAX_PAE_WATCH_ENTRIES) {
            /* shift the higher entries down a slot */
            memcpy(&pae_arp_watch_table[slot],&pae_arp_watch_table[slot+1],
                sizeof(pae_arp_watch_table[slot]) * ((pae_arp_watch_table_num_entries) - slot));
        } else {
            memset(&pae_arp_watch_table[slot], 0, sizeof(pae_arp_watch_table[slot]));
        }
        pae_arp_watch_table_num_entries--;
        return count;
    } else {
        printk(KERN_INFO "%s: Watch Entry Not Found!\n",__func__);
        return -EINVAL;
    }
}

int pae_delete_arp_watch(uint8_t *ipv4_address, char *interface)
{
    char buf[128];
    int rc;

    sprintf(buf,"%02x%02x%02x%02x %s",
        ipv4_address[0], ipv4_address[1],
        ipv4_address[2], ipv4_address[3], interface);

    rc = remove_arp_watch(NULL, NULL, buf, strlen(buf));
    if (rc == strlen(buf)) {
        rc = 0;
    } else if (rc > 0) {
        rc = -EINVAL;
    }
    return rc;
}

struct device_attribute dev_attr_pae_arp_watch_remove =
    __ATTR(arp_watch_remove, S_IRUSR | S_IWUSR, NULL, remove_arp_watch);

int pae_add_arp_watch_notifier(uint8_t *ipv4_address, const char *interface, struct notifier_block *nb, void *notify_data)
{
    int slot = -1;
    int i;

    /* if there is a matching entry, add the user supplied notifier to the chain */
    for (i = 0; i < pae_arp_watch_table_num_entries; i++) {
        if (memcmp(pae_arp_watch_table[i].ipv4_address, ipv4_address, sizeof(ipv4_address)) == 0) {
            if (memcmp(pae_arp_watch_table[i].dev_name, interface, strlen(interface)) == 0) {
                slot = i;
                break;
            }
        }
    }
    if (slot >= 0) {
        pae_arp_watch_table[i].notify_data = notify_data;
        return atomic_notifier_chain_register(&pae_arp_watch_table[i].callback_chain, nb);
    } else {
        return -EINVAL;
    }
}

int pae_remove_arp_watch_notifier(uint8_t *ipv4_address, const char *interface, struct notifier_block *nb)
{
    int slot = -1;
    int i;

    /* if there is a matching entry, add the user supplied notifier to the chain */
    for (i = 0; i < pae_arp_watch_table_num_entries; i++) {
        if (memcmp(pae_arp_watch_table[i].ipv4_address, ipv4_address, sizeof(ipv4_address)) == 0) {
            if (memcmp(pae_arp_watch_table[i].dev_name, interface, strlen(interface)) == 0) {
                slot = i;
                break;
            }
        }
    }
    if (slot >= 0) {
        return atomic_notifier_chain_unregister(&pae_arp_watch_table[i].callback_chain, nb);
    } else {
        return -EINVAL;
    }
}

static ssize_t write_arp_watch_timer(struct device *dev, struct device_attribute *attr,
                               const char *buf, size_t count)
{
    unsigned timer;
    const char *curs = buf;
    size_t count_left = count;
    extern void pae_register_test_callback(void);

#ifdef TEST_ARP_CALLBACK
    printk(KERN_INFO "Adding test entry and callback\n");
    pae_add_arp_watch_table_test_entry();
    pae_register_test_callback();
#endif

    if (parse_unsigned(curs, count_left, &curs, &count_left, &timer)) {
        return -EINVAL;
    } else {
        pae_arp_watch_timer = timer;
        return count;
    }
}
static ssize_t read_arp_watch_timer(struct device *dev,
                              struct device_attribute *devattr, char *buf)
{
    memset(buf,0,PAGE_SIZE);
    return sprintf(buf,"%u",pae_arp_watch_timer);
}

struct device_attribute dev_attr_pae_arp_watch_timer =
    __ATTR(arp_watch_timer, S_IRUSR | S_IWUSR, read_arp_watch_timer, write_arp_watch_timer);
