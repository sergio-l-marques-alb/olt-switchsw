#include "xmodem.h"
/*  Print Help Message  */
void xmhelp(void)
{
  fprintf(stdout, "Usage:  \txmodem ");
  fprintf(stdout, " -[rb!rt!ra!sb!st!sa][options] filename... 0\n\r");
  fprintf(stdout, "Major Commands --");
  fprintf(stdout, "\n\r\trb <-- Receive Binary");
  fprintf(stdout, "\n\r\trt <-- Receive Text");
  fprintf(stdout, "\n\r\tra <-- Receive Apple macintosh text");
  fprintf(stdout, "\n\r\tsb <-- Send Binary");
  fprintf(stdout, "\n\r\tst <-- Send Text");
  fprintf(stdout, "\n\r\tsa <-- Send Apple macintosh text");
  fprintf(stdout, "\n\rOptions --");
  fprintf(stdout, "\n\r\ty  <-- Use YMODEM Batch Mode on transmit");
  fprintf(stdout, "\n\r\tg  <-- Select YMODEM-G Mode on receive");
  fprintf(stdout, "\n\r\tm  <-- Use MODEM7 Batch Mode on transmit");
  fprintf(stdout, "\n\r\tk  <-- Use 1K packets on transmit");
  fprintf(stdout, "\n\r\tc  <-- Select CRC mode on receive");
  fprintf(stdout, "\n\r\tt  <-- Indicate a TOO BUSY Unix system");
  fprintf(stdout, "\n\r\td  <-- Delete xmodem.log file before starting");
  fprintf(stdout, "\n\r\tl  <-- (ell) Turn OFF Log File Entries");
  fprintf(stdout, "\n\r\tx  <-- Include copious debugging information in log file");
  fprintf(stdout, "\n\r\tp  <-- Use with SunOS tip ~C command");
  fprintf(stdout, "\n\r\tw  <-- Wait before initial handshake");
  fprintf(stdout, "\n\r\te  <-- Supress EOT confirmation");
  fprintf(stdout, "\n\r\tn  <-- Allow mid-transfer CAN-CAN aborts");
  fprintf(stdout, "\n\r");
}

/* get type of transmission requested (text or binary) */
int gettype(char ichar)
{
  if (ichar == 't' || ichar == 'T')
    return('t');
  else if (ichar == 'b' || ichar == 'B')
    return('b');
  else if (ichar == 'a' || ichar == 'A')
    return('a');
  else
    xmodem_error("Invalid Send/Receive Parameter - not t or b", TRUE); /* was false */
  return('\0');
}

/* return a string containing transmission type */
char *prtype(char ichar)
{
  if (ichar == 't' || ichar == 'T')
    return("text");
  else if (ichar == 'b' || ichar == 'B')
    return("binary");
  else if (ichar == 'a' || ichar == 'A')
    return("apple");
  else
    return("");
}

/* print error message and exit; if mode == TRUE, restore normal tty modes */
void xmodem_error(char *msg, int mode)
{
  if (mode)
    restoremodes(TRUE);  /* put back normal tty modes */
  fprintf(stdout, "\r\n%s\n\r", msg);
  if ((xlogflag || (xdebugflag == TRUE)) && (xlogfp != NULL))
  {
    fprintf(xlogfp, "XMODEM Fatal Error:  %s\n\r", msg);
    fclose(xlogfp);
  }
  return;
}


/* Construct a proper (i.e. pretty) sector count for messages */

char *sectdisp(long recvsectcnt, int bufsize, int plus1)
{
  static char string[20];
  if (plus1)
    recvsectcnt += (bufsize == 128) ? 1 : 8;
  if (bufsize == 128 || recvsectcnt == 0)
    sprintf (string, "%d", (int) recvsectcnt);
  else
    sprintf (string, "%d-%d", (int) recvsectcnt-7, (int) recvsectcnt);
  return(string);
}

/* type out debugging info */
void xmdebug(char *str)
{
  if ((xdebugflag == TRUE) && (xlogfp != NULL))
    fprintf(xlogfp,"DEBUG: '%s'\n\r",str);
}

/* print elapsed time and rate of transfer in logfile */

int quant[] = { 60, 60, 24};
char sep[3][10] = { "second", "minute", "hour"};

int prtime (long numsect, time_t seconds, FILE *fileid)

{
  register int i;
  register int Seconds;
  int nums[3];
  int rate;

  if (numsect == 0)
    return(0);

  Seconds = (int)seconds;
  Seconds = (Seconds > 0) ? Seconds : 0;

  rate = (Seconds != 0) ? 128 * numsect/Seconds : 0;

  for (i=0; i<3; i++)
  {
    nums[i] = (Seconds % quant[i]);
    Seconds /= quant[i];
  }

  fprintf (fileid, "%ld Sectors Transfered in ", numsect);

  if (rate == 0)
    fprintf (fileid, "0 seconds");
  else
    while (--i >= 0)
      if (nums[i])
        fprintf (fileid, "%d %s%c ", nums[i], &sep[i][0],
                 nums[i] == 1 ? ' ' : 's');
  fprintf (fileid, "\n\r");

  if (rate != 0)
    fprintf (fileid, "Transfer Rate = %d Characters per Second\n\r", rate);

  return(0);
}

/* Print elapsed time estimate */

int projtime (long numsect, FILE *fd)
{
  register int i;
  register int seconds;
  int nums[3];

  if (numsect == 0)
    return(0);

/* constant below should really be 1280; reduced to 90% to account for time lost in overhead */

  seconds = 1422 * numsect / ttyspeed + 1;

  for (i=0; i<3; i++)
  {
    nums[i] = (seconds % quant[i]);
    seconds /= quant[i];
  }

  fprintf (fd, "Estimated transmission time ");

  while (--i >= 0)
    if (nums[i])
      fprintf (fd, "%d %s%c ", nums[i], &sep[i][0],
               nums[i] == 1 ? ' ' : 's');
  fprintf (fd, "\n\r");
  return(0);
}

/* Really flush input by waiting for line to clear */
void reallyflush(void)
{

  if (xdebugflag == TRUE)
    fprintf(xlogfp,"DEBUG: Waiting for line to clear\n\r");
  flushin();
  while (readbyte(1) != TIMEOUT)
    ;
  if (xdebugflag == TRUE)
    fprintf(xlogfp,"DEBUG: Line is clear\n\r");
}
