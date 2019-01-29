/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename pw_scramble.c
*
* @purpose Password scramble utility
*
* @component utility
*
* @comments The Password Scramble utility is used to re-arrange the
*           bits in a password.
*
* @create 11/26/2001
*
* @author kmcdowell
*
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/


#include "pw_scramble_api.h"
#include "user_mgr_api.h"
#include "user_manager_exports.h"

/* Begin Function Declarations: ..\..\..\..\l7public\api\pw_scramble_api.h */

#define AES_USERKEY "78ej6t3p8024s2r5" /* fixed, but randomly generated, must have length of AES_BLOCK_SIZE */
#define AES_ENCRYPTION_BITS 128

L7_RC_t pwEncryptAes(L7_char8 *password, L7_char8 *encryptedPw, L7_uint32 length);
L7_RC_t pwEncryptMD5(L7_char8 *password, L7_char8 *encryptedPw);
extern void ewsAuthHashMD5Password(const char *password, char **digest );

/*********************************************************************
*
* @purpose  Scramble the bits of the entered password
*
* @param    *oldPw    pointer to an unscrambled password
* @param    *pw       pointer to the returned, scrambled password
*
* @returns  L7_SUCCESS  on success
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
pwScramble(L7_char8 *oldPw, L7_char8 *pw)
{
  return pwScrambleLength( oldPw, pw, L7_PASSWORD_SIZE );

}


/*********************************************************************
*
* @purpose  Scramble the bits of the entered password
*
* @param    *oldPw    pointer to an unscrambled password
* @param    *pw       pointer to the returned, scrambled password
* @param    length    (input) number of bytes to scramble
*
* @returns  L7_SUCCESS  on success
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
pwScrambleLength(L7_char8 *oldPw, L7_char8 *pw, L7_uint32 length)
{
  L7_uint32 i;
  L7_BOOL clear = L7_FALSE;

  memset(pw, 0, length);

  for (i=0; i < length-1; i++)
  {
      if (oldPw[i] == 0)
      {
          clear = L7_TRUE;
      }
      if (clear != L7_TRUE)
      {
          if (oldPw[i] & 1)      pw[i] = pw[i] | (1<<3);
          if (oldPw[i] & (1<<1)) pw[i] = pw[i] | (1<<1);
          if (oldPw[i] & (1<<2)) pw[i] = pw[i] | (1<<6);
          if (oldPw[i] & (1<<3)) pw[i] = pw[i] | (1<<4);
          if (oldPw[i] & (1<<4)) pw[i] = pw[i] |  1;
          if (oldPw[i] & (1<<5)) pw[i] = pw[i] | (1<<2);
          if (oldPw[i] & (1<<6)) pw[i] = pw[i] | (1<<5);
          if (oldPw[i] & (1<<7)) pw[i] = pw[i] | (1<<7);
      }
      pw[i] += 38;
  }
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  Unscramble the bits of the entered password
*
* @param    *oldPw    pointer to a scrambled password
* @param    *pw       pointer to the returned, unscrambled password
*
* @returns  L7_SUCCESS  on success
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
pwUnscramble(L7_char8 *oldPw, L7_char8 *pw)

{
   return pwUnscrambleLength( oldPw, pw, L7_PASSWORD_SIZE);
}


/*********************************************************************
*
* @purpose  Unscramble the bits of the entered password
*
* @param    *oldPw    pointer to a scrambled password
* @param    *pw       pointer to the returned, unscrambled password
* @param    length    (input) number of bytes to unscramble
*
* @returns  L7_SUCCESS  on success
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
pwUnscrambleLength(L7_char8 *oldPw, L7_char8 *pw, L7_uint32 length)

{
   L7_uint32 i;

   memset(pw, 0, length);

   for (i=0; i < length-1; i++)
   {
      oldPw[i] -= 38;
      if (oldPw[i] & 1)      pw[i] = pw[i] | (1<<4);
      if (oldPw[i] & (1<<1)) pw[i] = pw[i] | (1<<1);
      if (oldPw[i] & (1<<2)) pw[i] = pw[i] | (1<<5);
      if (oldPw[i] & (1<<3)) pw[i] = pw[i] |  1;
      if (oldPw[i] & (1<<4)) pw[i] = pw[i] | (1<<3);
      if (oldPw[i] & (1<<5)) pw[i] = pw[i] | (1<<6);
      if (oldPw[i] & (1<<6)) pw[i] = pw[i] | (1<<2);
      if (oldPw[i] & (1<<7)) pw[i] = pw[i] | (1<<7);

      oldPw[i] += 38;
   }
   return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Encrypt entered password
*
* @param    *password       (input) pointer to an encrypted password
* @param    *encryptedPw    (output) pointer to the returned, encrypted password
* @param    length          (input) length of the source password. Not used for MD5.
* @param    alg             (input)  encryption algorithm
*
* @returns  L7_SUCCESS  on success
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pwEncrypt(L7_char8 *password, L7_char8 *encryptedPw, L7_uint32 length,
                  L7_PASSWORD_ENCRYPT_ALG_t alg)
{
  switch (alg)
  {
    case L7_PASSWORD_ENCRYPT_AES:
      return pwEncryptAes(password, encryptedPw, length);

    case L7_PASSWORD_ENCRYPT_MD5:
      return pwEncryptMD5(password, encryptedPw);

    default:
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_LOG_COMPONENT_DEFAULT,
                "Unhandled encryption algorithm %d", alg);
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Encrypt entered password
*
* @param    *password       (input) pointer to an encrypted password
* @param    *encryptedPw    (output) pointer to the returned, encrypted password
* @param    length          (input)  number of bytes in encrypted password
* @param    alg             (input)  encryption algorithm
*
* @returns  L7_SUCCESS  on success
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pwEncryptAes(L7_char8 *password, L7_char8 *encryptedPw, L7_uint32 length)
{
  L7_uchar8 ivec[AES_BLOCK_SIZE];
  L7_uchar8 *encryptedBinaryData=NULL;
  AES_KEY key;
  L7_uchar8 *tempP = encryptedPw;
  L7_uchar8 nybble;
  L7_uint32 i;

  if (length == 0)
    return L7_ERROR;

  memset(ivec, 0, sizeof(ivec));

  /* don't encrypt null passwords */
  if (strlen(password) == 0)
  {
    /* encrypted password is null as well */
    encryptedPw[0] = 0;
    return L7_SUCCESS;
  }

  if (strlen(password) > length)
  {
    return L7_FAILURE;
  }

  if (AES_set_encrypt_key(AES_USERKEY, AES_ENCRYPTION_BITS, &key) < 0)
  {
    return L7_FAILURE;
  }

  /* allocate memory for encrypted data */
  encryptedBinaryData = osapiMalloc(0, length);
  if (encryptedBinaryData == NULL)
  {
    return L7_ERROR;
  }
  memset(encryptedBinaryData, 0, length);

  AES_cbc_encrypt(password, encryptedBinaryData, length, &key, ivec, AES_ENCRYPT);

  /* convert to hex */
  for (i = 0; i < length; i++)
  {
    nybble = (encryptedBinaryData[i] >> 4) & 0x0f;
    *tempP++ = nybble > 9 ? nybble - 10 + 'a' : nybble + '0';
    nybble = encryptedBinaryData[i] & 0x0f;
    *tempP++ = nybble > 9 ? nybble - 10 + 'a' : nybble + '0';
  }
  *tempP = '\0';

  osapiFree(0, encryptedBinaryData);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Decrypt encrypted password
*
* @param    *password       (output) pointer to an buffer for decrypted password
* @param    *encryptedPw    (input) pointer to the encrypted password
*
* @returns  L7_SUCCESS  on success
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pwDecrypt(L7_char8 *password, L7_char8 *encryptedPw, L7_uint32 length)
{
  L7_uchar8 ivec[AES_BLOCK_SIZE];
  AES_KEY key;
  L7_uchar8 *encryptedBinaryData=NULL;
  L7_uchar8 nybble;
  L7_uint32 i, j;

  memset(ivec, 0, sizeof(ivec));

  if (length == 0)
    return L7_ERROR;

  if (AES_set_decrypt_key(AES_USERKEY, AES_ENCRYPTION_BITS, &key) < 0)
  {
    return L7_FAILURE;
  }

  /* allocate memory for decrypted data */
  encryptedBinaryData = osapiMalloc(0, length);
  if (encryptedBinaryData == NULL)
  {
    return L7_ERROR;
  }
  memset(encryptedBinaryData, 0, length);

  /* convert hex to ascii */
  for (i=0,j=0; i<(length*2); i++,j++)
  {
    nybble = (L7_uchar8) encryptedPw[i];
    encryptedBinaryData[j] = ((nybble >= '0' && nybble <= '9') ? nybble - '0' : nybble + 10 - 'a') << 4;
    i++;
    nybble = (L7_uchar8) encryptedPw[i];
    encryptedBinaryData[j] += (nybble >= '0' && nybble <= '9')? nybble - '0' : nybble + 10 - 'a';
  }

  /* perform decryption */
  AES_cbc_encrypt(encryptedBinaryData, password, length, &key, ivec, AES_DECRYPT);

  osapiFree(0, encryptedBinaryData);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Encrypt entered password using MD5
*
* @param    *password       (input) pointer to an encrypted password
* @param    *encryptedPw    (output) pointer to the returned, encrypted password
*
* @returns  L7_SUCCESS  on success
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pwEncryptMD5(L7_char8 *password, L7_char8 *encryptedPw)
{
#ifdef CLI_WEB_PRESENT
  ewsAuthHashMD5Password(password, (char **)&encryptedPw );
  return L7_SUCCESS;
#else
  return L7_FAILURE;
#endif
}

/*
 * Takes a user provided string, encrypts, decrypts and prints decrypted output.
 * For example debugTestAes('samplestring') should print 'samplestring'
 */
void debugTestAes(L7_char8 *str)
{
  const unsigned char key16[16]=
  {0x12,0x34,0x56,0x78,0x9a,0xbc,0xde,0xf0,
   0x34,0x56,0x78,0x9a,0xbc,0xde,0xf0,0x12};
  unsigned char iv[16],tmpiv[16];
  char out[1024],in[1024];
  AES_KEY aeskey;
  int lth;

  lth = strlen(str) + 1;
  strncpy(in,str, lth);
  AES_set_encrypt_key(key16,128,&aeskey);
  memcpy(tmpiv,iv,sizeof(iv));
  AES_cbc_encrypt(in,out, lth, &aeskey, tmpiv,AES_ENCRYPT);
  AES_set_decrypt_key(key16,128,&aeskey);
  memcpy(tmpiv,iv,sizeof(iv)); /* reset IV */
  AES_cbc_encrypt(out,in, sizeof(out),&aeskey, tmpiv,AES_DECRYPT);
  printf("%s\n",in);
}


void debugTestPasswordEncrypt(L7_char8 *str)
{
  L7_RC_t rc;
  L7_uchar8 encryptedHex[L7_ENCRYPTED_PASSWORD_SIZE*2];
  L7_uchar8 decryptedPass[L7_PASSWORD_SIZE*2];

  memset(encryptedHex, 0, sizeof(encryptedHex));
  memset(decryptedPass, 0, sizeof(decryptedPass));

  printf("Clear text   : %s\n", str);

  /* 64 */
  if ((rc = pwEncrypt(str, encryptedHex, L7_PASSWORD_SIZE-1, L7_PASSWORD_ENCRYPT_AES)) != L7_SUCCESS)
  {
    printf("Encryption 64 failed: rc = %d\n", rc);
  }
  else
  {

    printf("Encrypted 64 hex: %s\n", encryptedHex);

    if ((rc = pwDecrypt(decryptedPass, encryptedHex, L7_PASSWORD_SIZE-1)) != L7_SUCCESS)
    {
      printf("Decryption 64 failed: rc = %d\n", rc);
    }
    else
    {
      printf("Decrypted 64    : %s\n", decryptedPass);
    }
  }


  /* 128 */
  if ((rc = pwEncrypt(str, encryptedHex, (L7_PASSWORD_SIZE-1)*2, L7_PASSWORD_ENCRYPT_AES)) != L7_SUCCESS)
  {
    printf("Encryption 128 failed: rc = %d\n", rc);
  }
  else
  {
    printf("Encrypted 128 hex: %s\n", encryptedHex);

    if ((rc = pwDecrypt(decryptedPass, encryptedHex, (L7_PASSWORD_SIZE-1)*2)) != L7_SUCCESS)
    {
      printf("Decryption 128 failed: rc = %d\n", rc);
    }
    else
    {
      printf("Decrypted 128    : %s\n", decryptedPass);
    }
  }


}

void debugTestPasswordDecrypt(L7_char8 *str)
{
  L7_RC_t rc;
  L7_uchar8 encryptedHex[L7_ENCRYPTED_PASSWORD_SIZE];
  L7_uchar8 decryptedPass[L7_PASSWORD_SIZE];

  memset(encryptedHex, 0, sizeof(encryptedHex));
  memset(decryptedPass, 0, sizeof(decryptedPass));
  memcpy(encryptedHex, str, sizeof(encryptedHex));

  if ((rc = pwDecrypt(decryptedPass, encryptedHex, L7_PASSWORD_SIZE-1)) != L7_SUCCESS)
  {
    printf("Decryption failed: rc = %d\n", rc);
    return;
  }
  pwDecrypt(decryptedPass, encryptedHex, L7_PASSWORD_SIZE-1);
  printf("Decrypted : %s\n", decryptedPass);
}

void testPasswordEncryptMD5(const char *password)
{
#ifdef CLI_WEB_PRESENT
  char key[33];
  char *keyP = key;

  ewsAuthHashMD5Password(password, (char **)&keyP);
  printf("\n encrypted password = %s", key);
#else
  printf("\n Requires CLI/WEB");
#endif
}
