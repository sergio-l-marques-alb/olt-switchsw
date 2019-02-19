/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename  util_pstring.h
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
#ifndef PSTRING_HELPER_H
#define PSTRING_HELPER_H

#define PSTRING_BUG_SIZE 1024

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
    L7_char8 * prefix, L7_char8 * buf, L7_int32 size, L7_char8 * text);
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
    L7_char8 * prefix);
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
    L7_char8 * prefix, void *context, L7_char8 * text);
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
void cliWriteAddBlanks (L7_int32 lcr, L7_int32 tcr, L7_int32 lsp, L7_int32 tsp, L7_char8 * prefix,
    L7_char8 * text);
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
    L7_char8 * prefix, L7_char8 * text);
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
void cliFormatAddBlanks (L7_int32 lcr, L7_int32 tcr, L7_int32 lsp, L7_int32 tsp, L7_char8 * prefix,
    void *context, L7_char8 * text);
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
    L7_char8 * prefix, L7_char8 * text);
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
    L7_char8 * prefix, L7_char8 * str, L7_uint32 n, L7_char8 const *fmt, ...);
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
    L7_char8 * prefix, L7_char8 * str, L7_uint32 n, L7_char8 const *fmt, ...);
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
L7_char8 *osapiStrncatAddBlanks (L7_int32 lcr, L7_int32 tcr, L7_int32 lsp, L7_int32 tsp,
    L7_char8 * prefix, L7_char8 * dest, L7_char8 * src, L7_int32 size);
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
    L7_char8 * prefix, L7_char8 * str, L7_char8 const *fmt, ...);
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
    L7_char8 * prefix, L7_char8 * dest, L7_char8 * src);
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
    L7_char8 * prefix, L7_char8 * dest, L7_char8 * src);

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
    L7_char8 * prefix, L7_char8 * dest, L7_char8 * src, L7_int32 size);

#define OSAPI_STRNCPY_SAFE_ADD_BLANKS(lcr,tcr,lsp,tsp,prefix,dst,src) \
  osapiStrncpySafeAddBlanks(lcr,tcr,lsp,tsp,prefix,dst,src,sizeof(dst))
#define OSAPI_STRNCAT_ADD_BLANKS(lcr,tcr,lsp,tsp,prefix,dst,src) \
  osapiStrncatAddBlanks(lcr,tcr,lsp,tsp,prefix,dst,src,sizeof(dst))

L7_char8 *cliSyntaxReturnPromptAddBlanks (L7_int32 lcr, L7_int32 tcr, L7_int32 lsp, L7_int32 tsp,
    L7_char8 * prefix, void *context, L7_char8 const * fmt, ...);
L7_char8 *cliSyntaxReturnPrompt (void *context, const L7_char8 * fmt, ...);
L7_char8 *strUtil64toa (L7_uint32 high, L7_uint32 low, L7_char8 * inbuf, L7_uint32 inbufsize);
L7_char8 *strUtilUspGet (L7_uint32 unit, L7_uint32 u, L7_uint32 s, L7_uint32 p, L7_char8 * inbuf, L7_uint32 inbufsize);

#endif
