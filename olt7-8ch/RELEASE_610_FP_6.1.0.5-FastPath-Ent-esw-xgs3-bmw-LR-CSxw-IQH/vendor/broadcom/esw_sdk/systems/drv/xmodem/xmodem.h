/* $Id: xmodem.h,v 1.1 2011/04/18 17:11:09 mruas Exp $ */
/* Xmodem Driver header file */

#ifndef __XMODEM_H
   
#include "vxWorks.h"		/* Added for vxWorks */
#include "sysLib.h"
#include "setjmp.h"		/* Also added for vxWorks */
#include "ctype.h"
#include "stdioLib.h"		/* Changed for vxWorks */
#include "sioLib.h"             /* Changed for vxWorks */
#include "types.h"
#include "stat.h"		
#include "string.h"
#include "systime.h"	        /* Changed for vxWorks */
#include "time.h"
#include "ioLib.h"
#include "fioLib.h"
#include "sigLib.h"		/* Changed for vxWorks */
#include "taskLib.h"		/* Added for vxWorks */
#include "selectLib.h"		/* Added for vxWorks */


/* Routines from rtcTime.stub.c */
IMPORT   time_t   rtcUnixTime(time_t *tloc);
IMPORT   char     *rtcConvUnixTime(time_t tloc); /* Not needed with vx 5.1 */


/* define macros to print messages in log file */
#define  logit(string) if(LOGFLAG)fprintf(LOGFP,string)
#define  logitarg(string,argument) if(LOGFLAG)fprintf(LOGFP,string,argument)
#define  tlogit(string) if(TIPFLAG)fprintf(stderr,string)
#define  tlogitarg(string,argument) if(TIPFLAG)fprintf(stderr,string,argument)
/* sleep definition added for vxWorks */
#define  sleep(sec) taskDelay(sysClkRateGet() * (sec))
/* vxWorks addition.  Created this command to give number of seconds since
   1/1/70 */
#define time(location) rtcUnixTime((location))
/* #define ctime(seconds) rtcConvUnixTime((seconds)) -- don't need with vx5.1*/
#ifndef FALSE
#define      FALSE      0
#endif
#ifndef TRUE
#define      TRUE       1
#endif


/*  ASCII Constants  */
#define      SOH  	001 
#define	     STX	002
#define	     ETX	003
#define      EOT	004
#define	     ENQ	005
#define      ACK  	006
#define	     LF		012   /* Unix LF/NL */
#define	     CR		015  
#define      NAK  	025
#define	     SYN	026
#define	     CAN	030
#define	     ESC	033

/*  XMODEM Constants  */
#define      TIMEOUT  	-1
#define      ERRORMAX  	10    /* maximum errors tolerated while transferring a packet */
#define      WAITFIRST  1     /* seconds between startup characters in read */
#define      PACKWAIT   5     /* seconds to wait for start of packet */
#define      STERRORMAX	60    /* maximum "errors" tolerated in read startup */
#define      CRCSWMAX	30    /* maximum time to try CRC mode before switching */
#define      NAKMAX	120   /* maximum times to wait for initial NAK when sending */
#define      RETRYMAX  	5     /* maximum retries to be made certain handshaking routines */
#define      KSWMAX	5     /* maximum errors before switching to 128 byte packets */
#define      EOTMAX	10    /* maximum times sender will send an EOT to end transfer */
#define      SLEEPNUM	100   /* target number of characters to collect during sleepy time */
#define	     BBUFSIZ	1024  /* buffer size */
#define      NAMSIZ	11    /* length of a CP/M file name string */
#define	     CTRLZ	032   /* CP/M EOF for text (usually!) */
#define      CRCCHR	'C'   /* CRC request character */
#define      KCHR	'K'   /* 1K block request character */
#define      GCHR	'G'   /* Ymodem-G request character */
#define      BAD_NAME	'u'   /* Bad filename indicator */
#define      TIPDELAY	15    /* seconds to delay handshake startup when -w */

#define      CREATMODE	0644  /* mode for created files */

/* GLOBAL VARIABLES */

int ttyspeed;		/* tty speed (bits per second) */
unsigned char buff[BBUFSIZ];	/* buffer for data */
int nbchr;		/* number of chars read so far for buffered read */
long filelength;	/* length specified in YMODEM header */
long fileread;		/* characters actually read so far in file */
char filename[256];	/* place to construct filenames */
int yfilesleft;		/* # of files left for YMODEM header */
long ytotleft;		/* # of bytes left for YMODEM header */

FILE *LOGFP;		/* descriptor for LOG file */

/* option flags and state variables */
char	XMITTYPE;	/* text or binary? */
int	XDEBUG;		/* keep debugging info in log? */
int	MOREDEBUG;	/* keep even more debugging info in log? */
int	RECVFLAG;	/* receive? */
int	SENDFLAG;	/* send? */
int	BATCH;		/* batch? (Now used as a state variable) */
int	CRCMODE;	/* CRC or checksums? */
int	DELFLAG;	/* don't delete old log file? */
int	LOGFLAG;	/* keep log? */
int	LONGPACK; 	/* do not use long packets on transmit? */
int	MDM7BAT;	/* MODEM7 batch protocol */
int	YMDMBAT;	/* YMODEM batch protocol */
int	TOOBUSY;	/* turn off sleeping in packet read routine */
int	TIPFLAG;	/* for tip ~C command */
int	DELAYFLAG;	/* for startup delay */
int	NOEOT;		/* suppress EOT verification */
int	CANCAN;		/* allow CAN-CAN aborts anytime */
int	YMODEMG;	/* YMODEM-G variant of YMODEM */

int	CHECKLENGTH;	/* Are we truncating a file to a YMODEM length? */


/*   CRC-16 constants.  From Usenet contribution by Mark G. Mendel, 
     Network Systems Corp.  (ihnp4!umn-cs!hyper!mark)
*/

    /* the CRC polynomial. */
#define	P	0x1021

    /* number of bits in CRC */
#define W	16

    /* the number of bits per char */
#define B	8

/* FLASH filesystem synch */
IMPORT void sysSynchFs(void);

#endif /* __XMODEM_H */
