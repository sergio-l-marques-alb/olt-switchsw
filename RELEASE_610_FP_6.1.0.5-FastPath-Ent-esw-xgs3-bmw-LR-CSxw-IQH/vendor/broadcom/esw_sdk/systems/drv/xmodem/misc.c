/*
 * $Id: misc.c,v 1.1 2011/04/18 17:11:09 mruas Exp $
 */
#include "xmodem.h"

/* forward and external references */
void error(char *, int);
extern void restoremodes(int);
extern void flushin(void);
extern int readbyte(int);

/*  Print Help Message  */
void xmhelp(void)
{
    fprintf(stderr, "Usage:  \txmodem ");
    fprintf(stderr, " -[rb!rt!ra!sb!st!sa][options] filename\n");
    fprintf(stderr, "Major Commands --");
    fprintf(stderr, "\n\trb <-- Receive Binary");
    fprintf(stderr, "\n\trt <-- Receive Text");
    fprintf(stderr, "\n\tra <-- Receive Apple macintosh text");
    fprintf(stderr, "\n\tsb <-- Send Binary");
    fprintf(stderr, "\n\tst <-- Send Text");
    fprintf(stderr, "\n\tsa <-- Send Apple macintosh text");
    fprintf(stderr, "\nOptions --");
    fprintf(stderr, "\n\ty  <-- Use YMODEM Batch Mode on transmit");
    fprintf(stderr, "\n\tg  <-- Select YMODEM-G Mode on receive");
    fprintf(stderr, "\n\tm  <-- Use MODEM7 Batch Mode on transmit");
    fprintf(stderr, "\n\tk  <-- Use 1K packets on transmit");
    fprintf(stderr, "\n\tc  <-- Select CRC mode on receive");
    fprintf(stderr, "\n\tt  <-- Indicate a TOO BUSY Unix system");
    fprintf(stderr, "\n\td  <-- Delete xmodem.log file before starting");
    fprintf(stderr, "\n\tl  <-- (ell) Turn OFF Log File Entries");
    fprintf(stderr,
	    "\n\tx  <-- Include copious debugging information in log file");
    fprintf(stderr, "\n\tp  <-- Use with SunOS tip ~C command");
    fprintf(stderr, "\n\tw  <-- Wait before initial handshake");
    fprintf(stderr, "\n\te  <-- Supress EOT confirmation");
    fprintf(stderr, "\n\tn  <-- Allow mid-transfer CAN-CAN aborts");
    fprintf(stderr, "\n");
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
	error("Invalid Send/Receive Parameter - not t or b",
	      TRUE); /* was false */
    return('\0');
}

/* return a string containing transmission type */
char*
prtype(char ichar)
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
extern jmp_buf xmEnv;
void error(char *msg, int mode)
{
    if (mode)
	restoremodes(TRUE);  /* put back normal tty modes */
    fprintf(stderr, "\r\n%s\n", msg);
    if ((LOGFLAG || XDEBUG) && (LOGFP != NULL)){   
	fprintf(LOGFP, "XMODEM Fatal Error:  %s\n", msg);
	fclose(LOGFP);
    }
    longjmp(xmEnv, -1);        /* was an exit(-1) */
}

/* Construct a proper (i.e. pretty) sector count for messages */
char*
sectdisp(long recvsectcnt, int bufsize, int plus1)
{
    static char string[20];
    if (plus1)
	recvsectcnt += (bufsize == 128) ? 1 : 8;
    if (bufsize == 128 || recvsectcnt == 0)
	sprintf (string, "%d", (int)recvsectcnt);
    else
	sprintf (string, "%d-%d", (int)recvsectcnt-7, (int)recvsectcnt);
    return(string);
}

/* type out debugging info */
void xmdebug(char *str)
{
    if (XDEBUG && (LOGFP != NULL))
	fprintf(LOGFP,"DEBUG: '%s'\n",str);
}

/* print elapsed time and rate of transfer in logfile */
int quant[] = { 60, 60, 24};	
char sep[3][10] = { "second", "minute", "hour" };

int
prtime (long numsect, time_t seconds, FILE *fileid)
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
    
    for (i=0; i<3; i++) {
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
    fprintf (fileid, "\n");
    if (rate != 0)
	fprintf (fileid, "Transfer Rate = %d Characters per Second\n", rate);

    return(0);
}

/* Print elapsed time estimate */
int
projtime (long numsect, FILE *fd)
{
    register int i;
    register int seconds;
    int nums[3];
    
    if (numsect == 0)
	return (0);
    
    /* constant below should really be 1280; reduced to 90% to account
     *  for time lost in overhead
     */

    seconds = 1422 * numsect / ttyspeed + 1;
    
    for (i=0; i<3; i++) {
	nums[i] = (seconds % quant[i]);
	seconds /= quant[i];
    }
    
    fprintf (fd, "Estimated transmission time ");
    
    while (--i >= 0)
	if (nums[i])
	    fprintf (fd, "%d %s%c ", nums[i], &sep[i][0],
		     nums[i] == 1 ? ' ' : 's');
    fprintf (fd, "\n");
    return (0);
}

/* Really flush input by waiting for line to clear */
void
reallyflush(void)
{
    if (XDEBUG)
	fprintf(LOGFP,"DEBUG: Waiting for line to clear\n");
    flushin();
    while(readbyte(1) != TIMEOUT)
	;
    if (XDEBUG)
	fprintf(LOGFP,"DEBUG: Line is clear\n");
}
