/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/util/util_pstring.c
 *
 * @purpose
 *
 * @component CLI
 *
 * @comments  none
 *
 * @create    07/01/2007
 *
 * @create    Rama Sasthri, Kristipati
 *
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_base_common.h"
#include <stdio.h>
#include <string.h>

#include "datatypes.h"
#include "osapi.h"
#include "util_pstring.h"
#include "usmdb_util_api.h"

void ewsTelnetWrite (void * context, L7_char8 * text);
void cliFormat (void * context, L7_char8 * text);
void cliWrite (L7_char8 * text);
void cliWriteSerial (L7_char8 * text);
void setLastError (L7_char8 * text);
void cliSyntaxBottom(void *context);
L7_char8 *cliPrompt(void *context);

static L7_char8 * globalStrBuf = L7_NULLPTR;
static L7_int32 globalStrBufLen = 0;
/*********************************************************************
 * @purpose  helper routine to add blanks
 *
 * @param  L7_int32 lcr         @b{(input))  Leading carriage return
 * @param  L7_int32 tcr         @b{(input))  Trailing carriage return
 * @param  L7_int32 lsp         @b{(input))  Leading space
 * @param  L7_int32 tsp         @b{(input))  Trailing space
 * @param  L7_char8 * prefix    @b{(input))  A string which represents type of message. For Example prefix can be "Error"
 * @param  L7_char8 * buf       @b{(output)) buffer to hold the string with added blanks
 * @param  L7_int32 size        @b{(input))  size of the buffer
 * @param  L7_char8 * text      @b{(input))  message
 *
 * @returns  buf
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_char8 *pStringBufAddBlanks (L7_int32 lcr, L7_int32 tcr, L7_int32 lsp, L7_int32 tsp,
    L7_char8 * prefix, L7_char8 * buf, L7_int32 size, L7_char8 * text)
{
  L7_int32 remind = size - 1;
  L7_char8 * dp = buf;
  L7_char8 * sp;

  while (lcr-- > 0 && remind > 2)
  {
    *dp++ = '\r';
    *dp++ = '\n';
    remind -= 2;
  }

  while (lsp-- > 0 && remind > 1)
  {
    *dp++ = ' ';
    remind--;
  }

  if (L7_NULLPTR != (sp = prefix))
  {
    while (*sp != '\0' && remind > 1)
    {
      *dp++ = *sp++;
      remind--;
    }
  }

  if (L7_NULLPTR != (sp = text))
  {
    while (*sp != '\0' && remind > 1)
    {
      *dp++ = *sp++;
      remind--;
    }
  }

  while (tsp-- > 0 && remind > 1)
  {
    *dp++ = ' ';
    remind--;
  }

  while (tcr-- > 0 && remind > 2)
  {
    *dp++ = '\r';
    *dp++ = '\n';
    remind -= 2;
  }

  *dp = '\0';
  return buf;
}
/*********************************************************************
 * @purpose  helper routine to add carriage return

 * @param  L7_char8 * text      @b{(input))  message
 * @param  L7_int32 lcr         @b{(input))  Leading carriage return
 * @param  L7_int32 tcr         @b{(input))  Trailing carriage return
 * @param  L7_int32 lsp         @b{(input))  Leading space
 * @param  L7_int32 tsp         @b{(input))  Trailing space
 * @param  L7_char8 * prefix    @b{(input))  A string which represents type of message.
 *
 * @returns
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_char8 *pStringAddCR (L7_char8 * text, L7_int32 lcr, L7_int32 tcr, L7_int32 lsp, L7_int32 tsp,
    L7_char8 * prefix)
{
  L7_int32 len = strlen (text) + lcr + tcr;

  if (globalStrBufLen < len)
  {
    if (globalStrBuf != L7_NULLPTR)
    {
      osapiFree (L7_CLI_WEB_COMPONENT_ID, globalStrBuf);
      globalStrBuf = L7_NULLPTR;
    }
    globalStrBufLen = 0;
  }

  if (globalStrBuf == L7_NULLPTR)
  {
    globalStrBufLen = (len < PSTRING_BUG_SIZE) ? 1024 : len;
    globalStrBuf = osapiMalloc (L7_CLI_WEB_COMPONENT_ID, globalStrBufLen);
  }

  if (globalStrBuf == L7_NULLPTR)
  {
    globalStrBufLen = 0;
    return text;
  }

  return pStringBufAddBlanks (lcr, tcr, lsp, tsp, prefix, globalStrBuf, globalStrBufLen, text);
}
/*********************************************************************
 * @purpose  helper routine to write the text to the telnet console after adding the blanks

 * @param  L7_int32 lcr         @b{(input))  Leading carriage return
 * @param  L7_int32 tcr         @b{(input))  Trailing carriage return
 * @param  L7_int32 lsp         @b{(input))  Leading space
 * @param  L7_int32 tsp         @b{(input))  Trailing space
 * @param  L7_char8 * prefix    @b{(input))  A string which represents type of message.
 * @param  void *context                     emweb context
 * @param  L7_char8 * text      @b{(input))  message
 *
 * @returns none
 *
 * @comments
 *
 * @end
 *********************************************************************/
void ewsTelnetWriteAddBlanks (L7_int32 lcr, L7_int32 tcr, L7_int32 lsp, L7_int32 tsp,
    L7_char8 * prefix, void * context, L7_char8 * text)
{
  L7_char8 buf[PSTRING_BUG_SIZE];
  pStringBufAddBlanks (lcr, tcr, lsp, tsp, prefix, buf, sizeof (buf), text);
  ewsTelnetWrite (context, buf);
}

/*********************************************************************
 * @purpose  helper routine to write the text to the console after adding the blanks

 * @param  L7_int32 lcr         @b{(input))  Leading carriage return
 * @param  L7_int32 tcr         @b{(input))  Trailing carriage return
 * @param  L7_int32 lsp         @b{(input))  Leading space
 * @param  L7_int32 tsp         @b{(input))  Trailing space
 * @param  L7_char8 * prefix    @b{(input))  A string which represents type of message.
 * @param  L7_char8 * text      @b{(input))  message

 * @returns none
 *
 * @comments
 *
 * @end
 *********************************************************************/
void cliWriteAddBlanks (L7_int32 lcr, L7_int32 tcr, L7_int32 lsp, L7_int32 tsp,
    L7_char8 * prefix, L7_char8 * text)
{
  L7_char8 buf[PSTRING_BUG_SIZE];
  pStringBufAddBlanks (lcr, tcr, lsp, tsp, prefix, buf, sizeof (buf), text);
  cliWrite (buf);
}

/*********************************************************************
 * @purpose  helper routine to write the text to the serial console after adding the blanks
 *
 * @param  L7_int32 lcr         @b{(input))  Leading carriage return
 * @param  L7_int32 tcr         @b{(input))  Trailing carriage return
 * @param  L7_int32 lsp         @b{(input))  Leading space
 * @param  L7_int32 tsp         @b{(input))  Trailing space
 * @param  L7_char8 * prefix    @b{(input))  A string which represents type of message.
 * @param  L7_char8 * text      @b{(input))  message

 * @returns none
 * @comments
 *
 * @end
 *********************************************************************/
void cliWriteSerialAddBlanks (L7_int32 lcr, L7_int32 tcr, L7_int32 lsp, L7_int32 tsp,
    L7_char8 * prefix, L7_char8 * text)
{
  L7_char8 buf[PSTRING_BUG_SIZE];
  pStringBufAddBlanks (lcr, tcr, lsp, tsp, prefix, buf, sizeof (buf), text);
  cliWriteSerial (buf);
}

/*********************************************************************
 * @purpose  helper routine to format the text after adding the blanks
 *
 * @param  L7_int32 lcr         @b{(input))  Leading carriage return
 * @param  L7_int32 tcr         @b{(input))  Trailing carriage return
 * @param  L7_int32 lsp         @b{(input))  Leading space
 * @param  L7_int32 tsp         @b{(input))  Trailing space
 * @param  L7_char8 * prefix    @b{(input))  A string which represents type of message.
 * @param  void *context                     emweb context
 * @param  L7_char8 * text      @b{(input))  message

 * @returns none
 * @comments
 *
 * @end
 *********************************************************************/
void cliFormatAddBlanks (L7_int32 lcr, L7_int32 tcr, L7_int32 lsp, L7_int32 tsp,
    L7_char8 * prefix, void * context, L7_char8 * text)
{
  L7_char8 buf[PSTRING_BUG_SIZE];
  pStringBufAddBlanks (lcr, tcr, lsp, tsp, prefix, buf, sizeof (buf), text);
  cliFormat (context, buf);
}

/*********************************************************************
 * @purpose   routine to set the last error
 *
 * @param  L7_int32 lcr         @b{(input))  Leading carriage return
 * @param  L7_int32 tcr         @b{(input))  Trailing carriage return
 * @param  L7_int32 lsp         @b{(input))  Leading space
 * @param  L7_int32 tsp         @b{(input))  Trailing space
 * @param  L7_char8 * prefix    @b{(input))  A string which represents type of message.
 * @param  L7_char8 * text      @b{(input))  message

 * @returns none
 *
 * @comments
 *
 * @end
 *********************************************************************/
void setLastErrorAddBlanks (L7_int32 lcr, L7_int32 tcr, L7_int32 lsp, L7_int32 tsp,
    L7_char8 * prefix, L7_char8 * text)
{
  L7_char8 buf[PSTRING_BUG_SIZE];
  pStringBufAddBlanks (lcr, tcr, lsp, tsp, prefix, buf, sizeof (buf), text);
  setLastError (buf);
}

/*********************************************************************
 * @purpose  snprintf routine to print the text after adding the blanks
 *
 * @param  L7_int32 lcr                @b{(input))  Leading carriage return
 * @param  L7_int32 tcr                @b{(input))  Trailing carriage return
 * @param  L7_int32 lsp                @b{(input))  Leading space
 * @param  L7_int32 tsp                @b{(input))  Trailing space
 * @param  L7_char8 * prefix           @b{(input))  A string which represents type of message.
 * @param  L7_char8 * str              @b{(input))  message
 * @param  L7_uint32 n                 @b{(input))  Size of n
 * @param  L7_char8 const *fmt         @b{(input))

 * @returns
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_int32 osapiSnprintfAddBlanks (L7_int32 lcr, L7_int32 tcr, L7_int32 lsp, L7_int32 tsp,
    L7_char8 * prefix, L7_char8 * str, L7_uint32 n, L7_char8 const * fmt, ...)
{
  L7_char8 buf[PSTRING_BUG_SIZE];
  L7_int32 rc;
  L7_int32 len;
  va_list ap;

  pStringBufAddBlanks (lcr, 0, lsp, 0, prefix, buf, sizeof (buf), L7_NULLPTR);
  len = strlen (buf);
  memset (&ap, 0, sizeof (ap));
  va_start (ap, fmt);
  rc = osapiVsnprintf (buf + len, n - len, fmt, ap);
  va_end (ap);
  len = strlen (buf);
  pStringBufAddBlanks (0, tcr, 0, tsp, L7_NULLPTR, buf + len, sizeof (buf) - len, L7_NULLPTR);
  osapiStrncpy (str, buf, n);

  return rc;
}

/*********************************************************************
 * @purpose  strncat routine for Add blanks
 *
 * @param  L7_int32 lcr                @b{(input))  Leading carriage return
 * @param  L7_int32 tcr                @b{(input))  Trailing carriage return
 * @param  L7_int32 lsp                @b{(input))  Leading space
 * @param  L7_int32 tsp                @b{(input))  Trailing space
 * @param  L7_char8 * prefix           @b{(input))  A string which represents type of message.
 * @param  L7_char8 * str              @b{(input))  message
 * @param  L7_uint32 n                 @b{(input))  Size of n
 * @param  L7_char8 const *fmt         @b{(input))

 * @returns
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_int32 osapiStrncatPrintfAddBlanks (L7_int32 lcr, L7_int32 tcr, L7_int32 lsp, L7_int32 tsp,
    L7_char8 * prefix, L7_char8 * str, L7_uint32 n, L7_char8 const * fmt, ...)
{
  L7_char8 buf[PSTRING_BUG_SIZE];
  L7_int32 rc;
  L7_int32 len;
  va_list ap;

  pStringBufAddBlanks (lcr, 0, lsp, 0, prefix, buf, sizeof (buf), L7_NULLPTR);
  len = strlen (buf);
  memset (&ap, 0, sizeof (ap));
  va_start (ap, fmt);
  rc = osapiVsnprintf (buf + len, n - len, fmt, ap);
  va_end (ap);
  len = strlen (buf);
  pStringBufAddBlanks (0, tcr, 0, tsp, L7_NULLPTR, buf + len, sizeof (buf) - len, L7_NULLPTR);
  osapiStrncat (str, buf, n);

  return rc;
}

/*********************************************************************
 * @purpose  strcat routine for add blanks
 *
 * @param  L7_int32 lcr                @b{(input))  Leading carriage return
 * @param  L7_int32 tcr                @b{(input))  Trailing carriage return
 * @param  L7_int32 lsp                @b{(input))  Leading space
 * @param  L7_int32 tsp                @b{(input))  Trailing space
 * @param  L7_char8 * prefix           @b{(input))  A string which represents type of message.
 * @param  L7_char8 * str              @b{(input))  message
 * @param  L7_char8 const *fmt         @b{(input))
 *
 * @returns
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_int32 strcatprintfAddBlanks (L7_int32 lcr, L7_int32 tcr, L7_int32 lsp, L7_int32 tsp,
    L7_char8 * prefix, L7_char8 * str, L7_char8 const * fmt, ...)
{
  L7_char8 buf[PSTRING_BUG_SIZE];
  L7_int32 rc;
  L7_int32 len;
  va_list ap;

  pStringBufAddBlanks (lcr, 0, lsp, 0, prefix, buf, sizeof (buf), L7_NULLPTR);
  len = strlen (buf);
  memset (&ap, 0, sizeof (ap));
  va_start (ap, fmt);
  rc = vsprintf (buf + len, fmt, ap);
  va_end (ap);
  len = strlen (buf);
  pStringBufAddBlanks (0, tcr, 0, tsp, L7_NULLPTR, buf + len, sizeof (buf) - len, L7_NULLPTR);
  strcat (str, buf);

  return rc;
}

/*********************************************************************
 * @purpose  sprintf routine for Add blanks
 *
 * @param  L7_int32 lcr                @b{(input))  Leading carriage return
 * @param  L7_int32 tcr                @b{(input))  Trailing carriage return
 * @param  L7_int32 lsp                @b{(input))  Leading space
 * @param  L7_int32 tsp                @b{(input))  Trailing space
 * @param  L7_char8 * prefix           @b{(input))  A string which represents type of message.
 * @param  L7_char8 * str              @b{(input))  message
 * @param  L7_char8 const *fmt         @b{(input))

 * @returns
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_int32 sprintfAddBlanks (L7_int32 lcr, L7_int32 tcr, L7_int32 lsp, L7_int32 tsp,
    L7_char8 * prefix, L7_char8 * str, L7_char8 const * fmt, ...)
{
  L7_char8 buf[PSTRING_BUG_SIZE];
  L7_int32 rc;
  L7_int32 len;
  va_list ap;

  pStringBufAddBlanks (lcr, 0, lsp, 0, prefix, buf, sizeof (buf), L7_NULLPTR);
  len = strlen (buf);
  memset (&ap, 0, sizeof (ap));
  va_start (ap, fmt);
  rc = vsprintf (buf + len, fmt, ap);
  va_end (ap);
  len = strlen (buf);
  pStringBufAddBlanks (0, tcr, 0, tsp, L7_NULLPTR, buf + len, sizeof (buf) - len, L7_NULLPTR);
  strcpy (str, buf);

  return rc;
}

/*********************************************************************
 * @purpose  strcat routine for add blanks
 *
 * @param  L7_int32 lcr                @b{(input))  Leading carriage return
 * @param  L7_int32 tcr                @b{(input))  Trailing carriage return
 * @param  L7_int32 lsp                @b{(input))  Leading space
 * @param  L7_int32 tsp                @b{(input))  Trailing space
 * @param  L7_char8 * prefix           @b{(input))  A string which represents type of message.
 * @param  L7_char8 * str              @b{(input))  message
 * @param  L7_char8 const *fmt         @b{(input))

 * @returns
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_char8 *strcatAddBlanks (L7_int32 lcr, L7_int32 tcr, L7_int32 lsp, L7_int32 tsp,
    L7_char8 * prefix, L7_char8 * dest, L7_char8 * src)
{
  L7_char8 buf[PSTRING_BUG_SIZE];
  pStringBufAddBlanks (lcr, tcr, lsp, tsp, prefix, buf, sizeof (buf), src);
  return strcat (dest, buf);
}

/*********************************************************************
 * @purpose  strncat routine for add blanks
 *
 * @param  L7_int32 lcr                @b{(input))  Leading carriage return
 * @param  L7_int32 tcr                @b{(input))  Trailing carriage return
 * @param  L7_int32 lsp                @b{(input))  Leading space
 * @param  L7_int32 tsp                @b{(input))  Trailing space
 * @param  L7_char8 * prefix           @b{(input))  A string which represents type of message.
 * @param  L7_char8 * dest             @b{(output))  buffer to hold the string
 * @param  L7_char8 * src              @b{(input))  source string
 * @param  L7_int32 size               @b{(input)}  number of bytes to concatenate

 *
 * @returns
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_char8 *osapiStrncatAddBlanks (L7_int32 lcr, L7_int32 tcr, L7_int32 lsp, L7_int32 tsp,
    L7_char8 * prefix, L7_char8 * dest, L7_char8 * src, L7_int32 size)
{
  L7_char8 buf[PSTRING_BUG_SIZE];
  pStringBufAddBlanks (lcr, tcr, lsp, tsp, prefix, buf, sizeof (buf), src);
  return osapiStrncat (dest, buf, size);
}

/*********************************************************************
 * @purpose  strcpy routine for add blanks
 *
 * @param  L7_int32 lcr                @b{(input))  Leading carriage return
 * @param  L7_int32 tcr                @b{(input))  Trailing carriage return
 * @param  L7_int32 lsp                @b{(input))  Leading space
 * @param  L7_int32 tsp                @b{(input))  Trailing space
 * @param  L7_char8 * prefix           @b{(input))  A string which represents type of message.
 * @param  L7_char8 * dest             @b{(output))  buffer to hold the string
 * @param  L7_char8 * src              @b{(input))  source string
 *
 * @returns
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_char8 *strcpyAddBlanks (L7_int32 lcr, L7_int32 tcr, L7_int32 lsp, L7_int32 tsp,
    L7_char8 * prefix, L7_char8 * dest, L7_char8 * src)
{
  L7_char8 buf[PSTRING_BUG_SIZE];
  pStringBufAddBlanks (lcr, tcr, lsp, tsp, prefix, buf, sizeof (buf), src);
  return strcpy (dest, buf);
}

/*********************************************************************
 * @purpose  Copy a string to a buffer with a bounded length (with safeguards) after adding the blanks
 *
 * @param  L7_int32 lcr                @b{(input))  Leading carriage return
 * @param  L7_int32 tcr                @b{(input))  Trailing carriage return
 * @param  L7_int32 lsp                @b{(input))  Leading space
 * @param  L7_int32 tsp                @b{(input))  Trailing space
 * @param  L7_char8 * prefix           @b{(input))  A string which represents type of message.
 * @param  L7_char8 * dest             @b{(output))  buffer to hold the string
 * @param  L7_char8 * src              @b{(input))  source string
 * @param  L7_int32 size               @b{(input)}  number of bytes to copy
 * @returns
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_char8 *osapiStrncpySafeAddBlanks (L7_int32 lcr, L7_int32 tcr, L7_int32 lsp, L7_int32 tsp,
    L7_char8 * prefix, L7_char8 * dest, L7_char8 * src, L7_int32 size)
{
  L7_char8 buf[PSTRING_BUG_SIZE];
  pStringBufAddBlanks (lcr, tcr, lsp, tsp, prefix, buf, sizeof (buf), src);
  return osapiStrncpySafe (dest, buf, size);
}

L7_char8 *cliSyntaxReturnPrompt (void *context, L7_char8 const *fmt, ...)
{
  va_list ap;
  L7_char8 str[1024];

  memset (&ap, 0, sizeof (ap));
  va_start (ap, fmt);
  (void) osapiVsnprintf (str, sizeof (str), fmt, ap);
  va_end (ap);

  ewsTelnetWrite (context, str);
  cliSyntaxBottom (context);
  return cliPrompt (context);
}

L7_char8 *cliSyntaxReturnPromptAddBlanks (L7_int32 lcr, L7_int32 tcr, L7_int32 lsp, L7_int32 tsp,
    L7_char8 * prefix, void *context, L7_char8 const * fmt, ...)
{
  L7_char8 buf[PSTRING_BUG_SIZE];
  L7_int32 rc;
  L7_int32 len;
  va_list ap;

  pStringBufAddBlanks (lcr, 0, lsp, 0, prefix, buf, sizeof (buf), L7_NULLPTR);
  len = strlen (buf);
  memset (&ap, 0, sizeof (ap));
  va_start (ap, fmt);
  rc = osapiVsnprintf (buf + len, sizeof(buf) - len, fmt, ap);
  va_end (ap);
  len = strlen (buf);
  pStringBufAddBlanks (0, tcr, 0, tsp, L7_NULLPTR, buf + len, sizeof (buf) - len, L7_NULLPTR);

  ewsTelnetWrite (context, buf);
  return cliPrompt (context);
}

L7_char8 *strUtil64toa (L7_uint32 high, L7_uint32 low, L7_char8 * inbuf, L7_uint32 inbufsize)
{
  L7_ulong64 num;
  static L7_char8 valbuf[32];
  char *tmpBuf;
  L7_uint32 size;

  num.high = high;
  num.low = low;

  if(inbuf != L7_NULLPTR)
  {
    tmpBuf = inbuf;
    size = inbufsize;
  }
  else
  {
    tmpBuf = valbuf;
    size = sizeof (valbuf);
  }

  memset (tmpBuf, 0x00, size);
  if (usmDb64BitsToString (num, tmpBuf) != L7_SUCCESS)
  {
    osapiStrncpySafe (tmpBuf, "ERROR", size);
  }
  return tmpBuf;
}

L7_char8 *strUtilUspGet (L7_uint32 unit, L7_uint32 u, L7_uint32 s, L7_uint32 p, L7_char8 * inbuf, L7_uint32 inbufsize)
{
  static L7_char8 buf[32];
  char *tmpBuf;
  L7_uint32 size;

  if(inbuf != L7_NULLPTR)
  {
    tmpBuf = inbuf;
    size = inbufsize;
  }
  else
  {
    tmpBuf = buf;
    size = sizeof (buf);
  }

  if (usmDbComponentPresentCheck (unit, L7_FLEX_STACKING_COMPONENT_ID))
  {
    osapiSnprintf (tmpBuf, size, "%d/%d/%d", u, s, p);
  }
  else
  {
    osapiSnprintf (tmpBuf, size, "%d/%d", s, p);
  }
  return tmpBuf;
}

