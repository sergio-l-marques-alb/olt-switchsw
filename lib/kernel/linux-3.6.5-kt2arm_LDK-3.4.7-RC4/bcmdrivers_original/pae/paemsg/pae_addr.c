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
#ifdef STANDALONE
#include <string.h>
#include <stdio.h>
#else
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/ctype.h>
#endif
#include "pae_addr.h"

#define NS_INT16SZ       2
#define NS_INADDRSZ      4
#define NS_IN6ADDRSZ    16

/*
 * WARNING: Don't even consider trying to compile this on a system where
 * sizeof(int) < 4.  sizeof(int) > 4 is fine; all the world's not a VAX.
 */

/* int
 * pae_parse_ipv4(src, dst)
 *      like inet_aton() but without all the hexadecimal and shorthand.
 * return:
 *      1 if `src' is a valid dotted quad, else 0.
 * notice:
 *      does not touch `dst' unless it's returning 1.
 * author:
 *      Paul Vixie, 1996.
 */

static inline int done(int ch)
{
    int rc = 0;
    switch (ch) {
    case ' ' :
    case 0 :
    case 0x0a :
        rc = 1;
        break;
    default:
        break;
    }
    return rc;
}

int pae_parse_ipv4(src, dst)
        const char *src;
        unsigned char *dst;
{
        static const char digits[] = "0123456789";
        int saw_digit, octets, ch;
        unsigned char tmp[NS_INADDRSZ], *tp;

        saw_digit = 0;
        octets = 0;
        *(tp = tmp) = 0;
        while (!done(ch = *src++)) {
                const char *pch;

                if ((pch = strchr(digits, ch)) != 0) {
                        unsigned int new = *tp * 10 + (pch - digits);

                        if (new > 255) {
                                printk(KERN_INFO "New > 255");
                                return (0);
                        }
                        *tp = new;
                        if (! saw_digit) {
                                if (++octets > 4) {
                                        printk(KERN_INFO "Octets > 4");
                                        return (0);
                                }
                                saw_digit = 1;
                        }
                } else if (ch == '.' && saw_digit) {
                        if (octets == 4) {
                                printk(KERN_INFO "Octets == 4");
                                return (0);
                        }
                        *++tp = 0;
                        saw_digit = 0;
                } else {
                        printk(KERN_INFO "invalid char in input");
                        return (0);
                }
        }
        if (octets < 4) {
                printk(KERN_INFO "too few octets: %d",octets);
                return (0);
        }
        memcpy(dst, tmp, NS_INADDRSZ);
        return (1);
}

/* int
 * pae_parse_ipv6(src, dst)
 *      convert presentation level address to network order binary form.
 * return:
 *      1 if `src' is a valid [RFC1884 2.2] address, else 0.
 * notice:
 *      (1) does not touch `dst' unless it's returning 1.
 *      (2) :: in a full address is silently ignored.
 * credit:
 *      inspired by Mark Andrews.
 * author:
 *      Paul Vixie, 1996.
 */
int pae_parse_ipv6(src, dst)
        const char *src;
        unsigned char *dst;
{
        static const char xdigits_l[] = "0123456789abcdef",
                          xdigits_u[] = "0123456789ABCDEF";
        unsigned char tmp[NS_IN6ADDRSZ], *tp, *endp, *colonp;
        const char *xdigits, *curtok;
        int ch, saw_xdigit;
        unsigned int val;

        memset((tp = tmp), '\0', NS_IN6ADDRSZ);
        endp = tp + NS_IN6ADDRSZ;
        colonp = 0;
        /* Leading :: requires some special handling. */
        if (*src == ':')
                if (*++src != ':')
                        return (0);
        curtok = src;
        saw_xdigit = 0;
        val = 0;
        while (!done(ch = *src++)) {
                const char *pch;

                if ((pch = strchr((xdigits = xdigits_l), ch)) == 0)
                        pch = strchr((xdigits = xdigits_u), ch);
                if (pch != 0) {
                        val <<= 4;
                        val |= (pch - xdigits);
                        if (val > 0xffff)
                                return (0);
                        saw_xdigit = 1;
                        continue;
                }
                if (ch == ':') {
                        curtok = src;
                        if (!saw_xdigit) {
                                if (colonp)
                                        return (0);
                                colonp = tp;
                                continue;
                        }
                        if (tp + NS_INT16SZ > endp)
                                return (0);
                        *tp++ = (unsigned char) (val >> 8) & 0xff;
                        *tp++ = (unsigned char) val & 0xff;
                        saw_xdigit = 0;
                        val = 0;
                        continue;
                }
                if (ch == '.' && ((tp + NS_INADDRSZ) <= endp) &&
                    pae_parse_ipv4(curtok, tp) > 0) {
                        tp += NS_INADDRSZ;
                        saw_xdigit = 0;
                        break;  /* '\0' was seen by pae_parse_ipv4(). */
                }
                return (0);
        }
        if (saw_xdigit) {
                if (tp + NS_INT16SZ > endp)
                        return (0);
                *tp++ = (unsigned char) (val >> 8) & 0xff;
                *tp++ = (unsigned char) val & 0xff;
        }
        if (colonp != 0) {
                /*
                 * Since some memmove()'s erroneously fail to handle
                 * overlapping regions, we'll do the shift by hand.
                 */
                const int n = tp - colonp;
                int i;

                for (i = 1; i <= n; i++) {
                        endp[- i] = colonp[n - i];
                        colonp[n - i] = 0;
                }
                tp = endp;
        }
        if (tp != endp)
                return (0);
        memcpy(dst, tmp, NS_IN6ADDRSZ);
        return (1);
}

#ifdef STANDALONE
main()
{
    char output[40];
    memset(output,0,sizeof(output));
    if (pae_parse_ipv6("fe80::1",output))
    {
        write(1,output,sizeof(output));
    }
}
#endif
