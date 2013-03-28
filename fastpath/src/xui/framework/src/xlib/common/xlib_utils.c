/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename xlib_utils.c
 *
 * @purpose
 *
 * @component XLIB
 *
 * @comments
 *
 * @create 04/17/2007
 *
 * @author Rama Sasthri, Kristipati
 * @end
 *
 **********************************************************************/

/*! \file */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define XLIB_INCLUDE_ERROR_CODE_STRINGS 1
#include "xlib_private.h"

extern int xLibTraceCurrentLine;
extern char *xLibTraceCurrentFile;
#ifdef _L7_OS_ECOS_
char *strdup (const char *src)
{
  int len;
  char *dst;

  len = strlen (src) + 1;
  if ((dst = (char *) xLibMalloc (len)) == NULL)
  {
    return (NULL);
  }
  strcpy (dst, src);
  return (dst);
}
#endif

static int xLibMallocCreateFailCount = 0;
void xLibMallocFailCountCreate(int count)
{
  xLibMallocCreateFailCount = count;
}

char *xLibMalloc (int size)
{
  void *ptr;

  if(xLibMallocCreateFailCount > 0)
  {
    --xLibMallocCreateFailCount;
    printf("xLibMalloc: Creating malloc failure\n");
    return NULL;
  }

  ptr = osapiMallocTry(L7_CLI_WEB_COMPONENT_ID , size+1);
  if (ptr == NULL)
  {
    XLIB_TRACE ("xLibMalloc failed");
    osapiDebugMallocSummary(0, 0);
    return ptr;  
  }  
  memset (ptr, 0, size);
  return ptr;
}

xlibValen_t xlibAllocValen(xLibU16_t len)
{
  xlibValen_t valen_t ;
  memset(valen_t.value,0,sizeof(valen_t.value));
  valen_t.valen = sizeof(valen_t.value);
  return valen_t;
}

void xLibFree (void *ptr)
{
  osapiFree(L7_CLI_WEB_COMPONENT_ID , ptr);
}

void xLibLog (char *file, int line, const char *func)
{
  sysapiPrintf ("%s %s %d\r\n", file, func, line);
}

char *xLibStrncpy (char *dest, const char *src, int n)
{
  char *ret = strncpy (dest, src, n);
  dest[n] = 0;
  return ret;
}

char *xLibStrDup (char *src)
{
  int len;
  char *dst;

  len = strlen (src) + 1;
  if ((dst = (char *) xLibMalloc (len)) == NULL)
  {
    return NULL;
  }
  strcpy (dst, src);
  return dst;
}

void xLibWarn (void)
{
}

void xLibTrace (int lvl, const char *file, int line, const char *fmt, ...)
{
  int rc, off = 0;
  char buf[512];
  va_list args;

  l7utilsFilenameStrip ((char**) & file);

  if (xLibTraceLevel < lvl)
  {
    return;
  }

  if (file != NULL)
  {
    rc = snprintf (&buf[off], sizeof (buf) - off - 1, "%s:%d ::", (char *) file, line);
    off = strlen (buf);
  }
  va_start (args, fmt);
  rc = vsnprintf (&buf[off], sizeof (buf) - off - 1, fmt, args);
  va_end (args);

  off = strlen (buf);
  while (buf[off] == 0 || buf[off] == '\n' || buf[off] == '\r')
  {
    buf[off--] = 0;
  }
  sysapiPrintf ("%s\r\n", buf);

  if(lvl > 2)
  {
    sysapiPrintf ("[SOURCED FROM %s:%d]\r\n", xLibTraceCurrentFile, xLibTraceCurrentLine);
  }

  if (lvl == 1)
  {
    xLibWarn ();
  }
}

void xLibBufPrint (char *msg, char *data, int len)
{
  int i;
  printf ("%s : ", msg);
  for (i = 0; i < len; i++)
  {
    printf ("%02x ", data[i] & 0xff);
  }
  printf ("\r\n");
}

void xLibObjValuePrint (char *msg, char *data, int len, int oid, int type)
{
  int i;
  printf ("%s : 0x%08x ", msg, oid);
  for (i = 0; i < len; i++)
  {
    printf ("%02x ", data[i] & 0xff);
  }
  printf ("\r\n");
}

xLibU16_t xLibUtilHexDecode (xLibS8_t * in, xLibU8_t * out, xLibU16_t maxCount, xLibS8_t sep)
{
  xLibU16_t i, j, len, val, cnt;

  for (i = 0, cnt = 0, len = strlen (in); i < len; i = i + j, cnt++)
  {
    for (j = 0, val = 0; j < 3; j++)
    {
      xLibS8_t ch = in[i + j];
      if (ch >= '0' && ch <= '9')
      {
        val = val << 4;
        val += (ch - '0');
      }
      else if (ch >= 'a' && ch <= 'f')
      {
        val = val << 4;
        val += (ch - 'a' + 10);
      }
      else if (ch >= 'A' && ch <= 'F')
      {
        val = val << 4;
        val += (ch - 'A' + 10);
      }
      else if (ch == sep || ch == 0)
      {
        j++;
        break;
      }
      else
      {
        return 0;
      }
    }
    out[cnt] = val;
  }
  return cnt;
}

#define XLIB_64BIT_HIGH_MAX_VALUE 4440000U

/*********************************************************************
*
* @purpose Convert a 64 bit counter to a string.
*
* @param xLibU64_t num   64 bit value
* @param xLibS8_t   *buf  dotted string representation
*
* @returns XLIBRC_SUCCESS, or
* @returns XLIBRC_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
xLibRC_t xLibUtil64BitsToString (xLibU64_t num, xLibS8_t * buf)
{

  xLibU32_t i;
  xLibUL32_t value[4];
  xLibUL32_t part;
  xLibUL32_t carry = 0;

  /* If no num */
  if (&num == NULL)
  {
    return XLIBRC_FAILURE;
  }

  /* If only a 32 bit counter */
  if (num.high == 0)
  {
    sprintf (buf, "%lu", (xLibUL32_t) num.low);
    return XLIBRC_SUCCESS;
  }

  /* If 64 bit counter, then initialize value[] */
  for (i = 0; i < 4; i++)
  {
    value[i] = 0;
  }

  /* calculate the "high" part */
  part = num.high;
  while (part > XLIB_64BIT_HIGH_MAX_VALUE)
  {
    value[0] += (67296 * XLIB_64BIT_HIGH_MAX_VALUE);
    if (value[0] >= 100000)
    {
      carry = value[0] / 100000;
      value[0] = value[0] % 100000;
    }

    value[1] += (42949 * XLIB_64BIT_HIGH_MAX_VALUE) + carry;
    carry = 0;
    if (value[1] >= 100000)
    {
      value[2] += (value[1] / 100000);
      value[1] = value[1] % 100000;
    }
    part -= XLIB_64BIT_HIGH_MAX_VALUE;
  }

  value[0] += (67296 * part);
  if (value[0] >= 100000)
  {
    carry = value[0] / 100000;
    value[0] = value[0] % 100000;
  }

  value[1] += (42949 * part) + carry;
  carry = 0;
  if (value[1] >= 100000)
  {
    value[2] += (value[1] / 100000);
    value[1] = value[1] % 100000;
  }

  if (value[2] >= 100000)
  {
    value[3] = (value[2] / 100000);
    value[2] = value[2] % 100000;
  }

  /* now add the "low" part */
  part = num.low;
  for (i = 0; i < 2; i++)
  {
    value[i] += part % 100000;
    if (value[i] >= 100000)
    {
      value[i + 1] += value[i] / 100000;
      value[i] = value[i] % 100000;
    }
    part = part / 100000;
  }

  if (value[2] >= 100000)
  {
    value[3] += value[2] / 100000;
    value[2] = value[2] % 100000;
  }

  /* buf holds the 64 bit value */
  if (value[3])
  {
    sprintf (buf, "%lu%05lu%05lu%05lu", value[3], value[2], value[1], value[0]);
  }
  else if (value[2])
  {
    sprintf (buf, "%lu%05lu%05lu", value[2], value[1], value[0]);
  }
  else
  {
    sprintf (buf, "%lu%05lu", value[1], value[0]);
  }

  return XLIBRC_SUCCESS;

}

xLibS8_t *xLibRcStrErr (xLibRC_t rc)
{
  if (rc < XLIBRC_SUCCESS)
  {
    return "JUNK ERROR CODE";
  }
  return xLibRcStr[rc - XLIBRC_SUCCESS];
}

