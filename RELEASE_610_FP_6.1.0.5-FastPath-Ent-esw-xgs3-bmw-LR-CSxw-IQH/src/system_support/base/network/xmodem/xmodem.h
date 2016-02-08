#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <fcntl.h>

#include "l7_common.h"
#include "osapi.h"
#include "sysapi.h"

#ifdef _L7_OS_VXWORKS_
#include <sys/times.h>
#include "taskLib.h"
#include <sigLib.h>
#include <sioLib.h>
#include <ioLib.h>
#include <selectLib.h>
#endif /* _L7_OS_VXWORKS_ */

#ifdef _l7_OS_ECOS_
#include <sys/time.h>
#endif


extern int sysClkRateGet (void);

/* define macros to print messages in log file */
#define  logit(string) if(xlogflag)fprintf(xlogfp,string)
#define  logitarg(string,argument) if(xlogflag)fprintf(xlogfp,string,argument)
#define  tlogit(string) if(xtipflag)fprintf(stderr,string)
#define  tlogitarg(string,argument) if(xtipflag)fprintf(stderr,string,argument)

/* #define ctime(seconds) rtcConvUnixTime((seconds)) -- don't need with vx5.1*/
#ifndef FALSE
#define      FALSE      0
#endif

#ifndef TRUE
#define      TRUE       1
#endif


/*  ASCII Constants  */
#define      SOH    001
#define      STX    002
#define      ETX    003
#define      EOT    004
#define      ENQ    005
#define      ACK    006
#define      LF     012   /* Unix LF/NL */
#define      CR     015
#define      NAK    025
#define      SYN    026
#define      CAN    030
#define      ESC    033

/*  XMODEM Constants  */
#define      TIMEOUT    -1
#define      ERRORMAX   10    /* maximum errors tolerated while transferring a packet */
#define      WAITFIRST  1     /* seconds between startup characters in read */
#define      PACKWAIT   5     /* seconds to wait for start of packet */
#define      STERRORMAX 60    /* maximum "errors" tolerated in read startup */
#define      CRCSWMAX   30    /* maximum time to try CRC mode before switching */
#define      NAKMAX 120   /* maximum times to wait for initial NAK when sending */
#define      RETRYMAX   5     /* maximum retries to be made certain handshaking routines */
#define      KSWMAX 5     /* maximum errors before switching to 128 byte packets */
#define      EOTMAX 10    /* maximum times sender will send an EOT to end transfer */
#define      SLEEPNUM   100   /* target number of characters to collect during sleepy time */
#define      BBUFSIZ    1024  /* buffer size */
#define      NAMSIZ 11    /* length of a CP/M file name string */
#define      CTRLZ  032   /* CP/M EOF for text (usually!) */
#define      CRCCHR 'C'   /* CRC request character */
#define      KCHR   'K'   /* 1K block request character */
#define      GCHR   'G'   /* Ymodem-G request character */
#define      BAD_NAME   'u'   /* Bad filename indicator */
#define      TIPDELAY   15    /* seconds to delay handshake startup when -w */

#define      CREATMODE  2  /* mode for created files */

extern int ttyspeed;   /* tty speed (bits per second) */
extern unsigned char xbuff[];  /* buffer for data */
extern int xnbchr;    /* number of chars read so far for buffered read */
extern long xfilelength;  /* length specified in YMODEM header */
extern long xfileread;    /* characters actually read so far in file */
extern char xfilename[]; /* place to construct filenames */
extern int yfilesleft;   /* # of files left for YMODEM header */
extern long ytotleft;    /* # of bytes left for YMODEM header */

extern FILE *xlogfp;    /* descriptor for LOG file */

/* option flags and state variables */
extern char  XMITTYPE; /* text or binary? */
extern int xdebugflag;    /* keep debugging info in log? */
extern int xmoredebugflag;  /* keep even more debugging info in log? */
extern int xrecvflag; /* receive? */
extern int xsendflag; /* send? */
extern int xbatch;    /* batch? (Now used as a state variable) */
extern int xcrcmode;  /* CRC or checksums? */
extern int xdelflag;  /* don't delete old log file? */
extern int xlogflag;  /* keep log? */
extern int xlongpack;   /* do not use long packets on transmit? */
extern int xmdm7bat;  /* MODEM7 batch protocol */
extern int xymdmbat;  /* YMODEM batch protocol */
extern int xtoobusy;  /* turn off sleeping in packet read routine */
extern int xtipflag;  /* for tip ~C command */
extern int xdelayflag;  /* for startup delay */
extern int xnoeot;    /* suppress EOT verification */
extern int xcancan;   /* allow CAN-CAN aborts anytime */
extern int ymodemg;  /* YMODEM-G variant of YMODEM */
extern int xchecklength;  /* Are we truncating a file to a YMODEM length? */



/* External and forward routine references */
extern long countnl(int);
extern void xmodem_error(char *, int);
extern void flushin(void);
extern int  getbyte(int, char *);
extern void getspeed(void);
extern int  gettype(char);
extern void napms(int);
extern int  projtime(long, FILE *);
extern int  prtime(long, time_t, FILE *);
extern int  readbuf(int, int, int, int, long, int *, int *);
extern int  readbyte(int);
extern void reallyflush(void);
extern void restoremodes(int);
extern int  rfile(char *);
extern int  select (int width, fd_set *pReadFds, fd_set *pWriteFds, fd_set *pExceptFds, struct timeval *pTimeOut);
extern int  send_name(char *);
extern void sendbyte(char);
extern void setmodes(void);
extern int  sfile(char *);
extern void stopsig(void);
extern void unixify(char *);
extern void writebuf(char *, int);
extern void xmdebug(char *);
extern void xmhelp(void);

/* external and forward references */

/*   CRC-16 constants.  From Usenet contribution by Mark G. Mendel,
     Network Systems Corp.  (ihnp4!umn-cs!hyper!mark)
*/

/* the CRC polynomial. */
#define P   0x1021

/* number of bits in CRC */
#define W   16

/* the number of bits per char */
#define B   8
