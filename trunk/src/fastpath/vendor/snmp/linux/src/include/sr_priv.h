/*
 *
 * Copyright (C) 1992-2003 by SNMP Research, Incorporated.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by SNMP Research, Incorporated.
 *
 * Restricted Rights Legend:
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions as set forth in subparagraph (c)(1)(ii) of the Rights
 *  in Technical Data and Computer Software clause at DFARS 252.227-7013;
 *  subparagraphs (c)(4) and (d) of the Commercial Computer
 *  Software-Restricted Rights Clause, FAR 52.227-19; and in similar
 *  clauses in the NASA FAR Supplement and other corresponding
 *  governmental regulations.
 *
 */

/*
 *                PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 *
 */

#ifndef SR_SR_PRIV_H
#define SR_SR_PRIV_H

#ifdef  __cplusplus
extern "C" {
#endif

void DesDecrypt(
    const unsigned char *secret,
    const unsigned char *salt,
    unsigned char *buf,
    SR_UINT32 len);

void DesEncrypt(
    const unsigned char *secret,
    const unsigned char *salt,
    unsigned char *buf,
    SR_UINT32 len);

#ifdef SR_3DES
void DesEDE_Decrypt(
    const unsigned char *secret,
    const unsigned char *secret1,
    const unsigned char *secret2,
    const unsigned char *salt,
    unsigned char *buf,
    SR_UINT32 len);

void DesEDE_Encrypt(
    const unsigned char *secret,
    const unsigned char *secret1,
    const unsigned char *secret2,
    const unsigned char *salt,
    unsigned char *buf,
    SR_UINT32 len);
#endif /* SR_3DES */

#ifdef SR_AES
#define MAX_Nr 14
#define RK_MAXSIZE (MAX_Nr+1)*4
#define AES_KEY_SIZE 16
#define AES_IV_SIZE 16
#define AES_BLOCK_SIZE 16
#define CFB128 128
#define CFB196 196
#define CFB256 256

void AES_cfb_Decrypt(
    const unsigned char *secret,
    const unsigned char *iv,
    unsigned char *buf,
    SR_UINT32 len,
    SR_INT32 protocol);

void AES_cfb_Encrypt(
    const unsigned char *secret,
    const unsigned char *iv,
    unsigned char *buf,
    SR_UINT32 len,
    SR_INT32 protocol);

#endif /* SR_AES */

#ifdef  __cplusplus
}
#endif

#endif /* SR_SR_PRIV_H */
