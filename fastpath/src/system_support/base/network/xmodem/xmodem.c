/*
 *  XMODEM -- Implements the Christensen XMODEM protocol,
 *            for packetized file up/downloading.
 *
 *  See the README file for some notes on SYS V adaptations.
 *  The program has been successfully run on VAXes (4.3BSD) and SUN-3/4s
 *  (SunOS 3.x) against MEX-PC and ZCOMM/DSZ.
 *
 *  See the README and update.doc files for history and change notes.
 *
 *  Please send bug fixes, additions and comments to:
 *  grandi@noao.edu
 */
#include <fcntl.h>
#include "xmodem.h"

/* Number of seconds to sleep while host is switching from
** XMODEM transfer to terminal mode.
*/
#define TERM_DELAY  5
/* #define XMODEM_DEBUG */

int ttyspeed;   /* tty speed (bits per second) */
unsigned char xbuff[BBUFSIZ];  /* buffer for data */
int xnbchr;    /* number of chars read so far for buffered read */
long xfilelength;  /* length specified in YMODEM header */
long xfileread;    /* characters actually read so far in file */
char xfilename[256]; /* place to construct filenames */
int yfilesleft;   /* # of files left for YMODEM header */
long ytotleft;    /* # of bytes left for YMODEM header */

FILE *xlogfp;    /* descriptor for LOG file */

/* option flags and state variables */
char  XMITTYPE; /* text or binary? */
int xdebugflag;    /* keep debugging info in log? */
int xmoredebugflag;  /* keep even more debugging info in log? */
int xrecvflag; /* receive? */
int xsendflag; /* send? */
int xbatch;    /* batch? (Now used as a state variable) */
int xcrcmode;  /* CRC or checksums? */
int xdelflag;  /* don't delete old log file? */
int xlogflag;  /* keep log? */
int xlongpack;   /* do not use long packets on transmit? */
int xmdm7bat;  /* MODEM7 batch protocol */
int xymdmbat;  /* YMODEM batch protocol */
int xtoobusy;  /* turn off sleeping in packet read routine */
int xtipflag;  /* for tip ~C command */
int xdelayflag;  /* for startup delay */
int xnoeot;    /* suppress EOT verification */
int xcancan;   /* allow CAN-CAN aborts anytime */
int ymodemg;  /* YMODEM-G variant of YMODEM */
int xchecklength;  /* Are we truncating a file to a YMODEM length? */

int xmodem(char *const_arg)
{
  int argc;     /* Changed for vxWorks */
  char *argv[20];
  char arg[80];
  FILE *fopen();
  char *unix_cpm();
  char *strcpy();
  char *strcat();
  char *prtype();
  char *fname = xfilename;   /* convenient place to stash file names */
  char *stamptime();    /* for timestamp */
  char *defname = "xmodem.in";  /* default file name if none given */
  struct stat filestatbuf;  /* file status info */
  int index;
  char flag;
  long expsect;
  int fd;
  int rc;
  L7_uint32 CharSize;




  /* ALT - Need to make my own copy of the argument string because
  ** caller may pass a pointer to a constant.
  */
  memset (arg, 0, sizeof (arg));
  (void)strncpy (arg, const_arg, sizeof (arg) - 1);

  /* ALT - Initialize argv and argc from the arg string.
  */
  argc = 1;
  argv[0] = "xmodem";
  argv [argc] = strtok (arg, " ");
  while ( argv[argc] != NULL )
  {
    argc++;
    argv[argc] = strtok (NULL, " ");
  };


  /* initialize option flags */

  XMITTYPE = 't';   /* assume text transfer */
#ifdef XMODEM_DEBUG
  xdebugflag = TRUE;    /* don't keep debugging info in log */
  xlogfp = fopen("xmodem.log", "w");
#else
  xdebugflag = FALSE;    /* don't keep debugging info in log */
#endif
  xmoredebugflag = FALSE;  /* don't keep even more debugging info in log */
  xrecvflag = FALSE; /* not receive */
  xsendflag = FALSE; /* not send either */
  xbatch = FALSE;    /* nor batch */
  xcrcmode = FALSE;  /* use checksums for now */
  xdelflag = FALSE;  /* don't delete old log file */
  xlogflag = FALSE;    /* keep log */
  xlongpack = FALSE;   /* do not use long packets on transmit */
  xmdm7bat = FALSE;  /* no MODEM7 batch mode */
  xymdmbat = FALSE;  /* no YMODEM batch mode */
  xtoobusy = FALSE;  /* not too busy for sleeping in packet read */
  xtipflag = FALSE;  /* no special logging on stderr */
  xdelayflag = FALSE;  /* don't delay startup for a while */
  xnoeot = FALSE;    /* don't suppress EOT verification */
  xcancan = FALSE;   /* don't allow CAN-CAN aborts in mid-transfer */
  ymodemg = FALSE;  /* no YMODEM-G */

#if 0
  fprintf(stderr, "XMODEM Version %s", VERSION);
  fprintf(stderr, " -- vxWorks File Transfer Facility\n\r");
#endif


  if ( argc == 1 )
  {
    xmhelp();
    return -1;
    /* exit(-1); Note, all exits converted to return for vxWorks */
  }

  index = 0;    /* set index for flag loop */

  stopsig();    /* suppress keyboard stop signal */

  while ( (flag = argv[1][index++]) != '\0' )
    switch ( flag )
    {
      case '-' : break;
      case 'X' :
      case 'x' :
        if ( xdebugflag == TRUE )
        {
          xmoredebugflag = TRUE;
        }
        else
        {
          xdebugflag = TRUE;  /* turn on debugging log */
        }
        break;
      case 'C' :
      case 'c' : xcrcmode = TRUE; /* enable CRC on receive */
        break;
      case 'D' :
      case 'd' : xdelflag = TRUE;  /* delete log file */
        break;
      case 'L' :
      case 'l' : xlogflag = FALSE;  /* turn off log  */
        break;
      case 'm' :
      case 'M' : xmdm7bat = TRUE;  /* turn on MODEM7 batch protocol */
        xbatch   = TRUE;
        break;
      case 'y' :
      case 'Y' : xymdmbat = TRUE;  /* turn on YMODEM batch protocol */
        xbatch   = TRUE;
        break;
      case 'k' :
      case 'K' : xlongpack = TRUE;  /* use 1K packets on transmit */
        break;
      case 't' :
      case 'T' : xtoobusy = TRUE;  /* turn off sleeping */
        break;
        /* Don't need this for vxWorks, but what the hell */
      case 'p' :
      case 'P' : xtipflag = TRUE;  /* turn on special handling for SunOS tip */
        break;
      case 'w' :
      case 'W' : xdelayflag = TRUE;  /* delay startup */
        break;
      case 'e' :
      case 'E' : xnoeot = TRUE;  /* turn off EOT verification */
        break;
      case 'n' :
      case 'N' : xcancan = TRUE;  /* allow mid-transfer CAN-CAN */
        break;
      case 'g' :
      case 'G' : ymodemg = TRUE;  /* YMODEM-G mode */
        xcancan = TRUE;
        xcrcmode = TRUE;
        xymdmbat = TRUE;
        break;
      case 'R' :
      case 'r' : xrecvflag = TRUE;  /* receive file */
        break;
      case 'B' :
      case 'b' : XMITTYPE = 'b';
        break;
      case 'S' :
      case 's' : xsendflag = TRUE;  /* send file */
        break;
      default  : fprintf(stdout, "Invalid Flag %c ignored\n\r", flag);
        break;
    }


  getspeed();   /* get tty-speed for time estimates */

  if ( xrecvflag && xsendflag )
  {
    xmodem_error("XMODEM Fatal Error- Both Send and Receive Functions Specified", TRUE); /* was false */
    return -1;
  }

  if ( xmdm7bat && (xymdmbat || ymodemg) )
  {
    xmodem_error("XMODEM Fatal Error - Both YMODEM and MODEM7 Batch Protocols Specified", TRUE); /* was false */
    return -1;
  }

  if ( !xrecvflag && !xsendflag )
  {
    xmodem_error("XMODEM Fatal Error - Either Send or Receive Function must be chosen!",TRUE); /* was false */
    return -1;
  };

  if ( xsendflag && argc==2 )
  {
    xmodem_error("XMODEM Fatal Error - No file specified to send",TRUE); /* was false */
    return -1;
  };

  if ( xrecvflag && argc==2 )
  {
    /* assume we really want CRC-16 in batch, unless we specify MODEM7 mode, unless we explicitly set xcrcmode */
    if ( !xcrcmode )
      xcrcmode = xmdm7bat ? FALSE : TRUE;
    fprintf(stdout, "Ready for xbatch RECEIVE");
    fprintf(stdout, " in %s mode\n\r", prtype(XMITTYPE));
    if ( !xtipflag )
      fprintf(stdout, "Send several Control-X characters to cancel before transfer starts.\n\r");
    logit("Batch Receive Started");
    logitarg(" in %s mode\n", prtype(XMITTYPE));
    (void)strcpy(fname, defname);
  }

  if ( xrecvflag && argc>2 )
  {
    if ( open(argv[2], 0,0) != -1 )  /* check for overwriting */
    {
      logit("Warning -- Target File Exists and is Being Overwritten\n");
      fprintf(stdout, "Warning -- Target File Exists and is Being Overwritten\n\r");
    }
    fprintf(stdout, "Ready to RECEIVE File %s", argv[2]);
    fprintf(stdout, " in %s mode\n\r", prtype(XMITTYPE));
    if ( !xtipflag )
      fprintf(stdout, "Send several Control-X characters to cancel before transfer starts.\n\r");
    logitarg("Receiving in %s mode\n\r", prtype(XMITTYPE));

    memset (fname, 0, sizeof (xfilename));
    (void)strncpy(fname,argv[2], sizeof(xfilename) - 1);
  }

  /* if ( sysapiSerialPortParmGet(SYSAPISERIALPORT_CHARSIZE, &CharSize) != L7_SUCCESS ) */
    CharSize = 8;

  if ( xrecvflag )
  {
    if ( xdelayflag )    /* delay if -w requested */
      OSAPI_SEC_SLEEP(TIPDELAY);

    /* Assume about 256 characters needed to be printed out above. So figure time to do so */
    OSAPI_MSEC_SLEEP((256*CharSize*1000) / ttyspeed );
    setmodes();   /* set tty modes for transfer */


    do
    {
      rc = rfile (fname);
      if ( rc == -1 )
      {
        flushin();
        restoremodes(FALSE); /* restore normal tty modes */

        OSAPI_SEC_SLEEP(TERM_DELAY); /* give other side time to return to terminal mode */
        return -1;
      };
    } while ( rc != FALSE );  /* receive files */

    flushin();
    restoremodes(FALSE);  /* restore normal tty modes */

    OSAPI_SEC_SLEEP(TERM_DELAY); /* give other side time to return to terminal mode */
    return 0;
  }

  if ( xsendflag && xbatch )
  {
    if ( xymdmbat )
    {
      ytotleft = 0l;
      yfilesleft = 0;
      for ( index=2; index<argc; index++ )
      {
        fd=open(argv[index],0,0);
        if (fd >= 0)
        {
	  (void) fstat (fd, &filestatbuf);
          yfilesleft++;
          ytotleft += filestatbuf.st_size;
          if ( XMITTYPE == 't' )
          {
              ytotleft += countnl(fd);
          }
          (void)close(fd);
        }
      }
      if ( xdebugflag == TRUE )
        fprintf(xlogfp, "DEBUG YMODEM file count: %d, %ld bytes\n\r", yfilesleft, ytotleft);

      fprintf(stdout, "Ready to YMODEM xbatch SEND");
      fprintf(stdout, " in %s mode\n\r", prtype(XMITTYPE));
      logit("YMODEM Batch Send Started");
      logitarg(" in %s mode\n", prtype(XMITTYPE));
    }
    else if ( xmdm7bat )
    {
      fprintf(stdout, "Ready to MODEM7 xbatch SEND");
      fprintf(stdout, " in %s mode\n\r", prtype(XMITTYPE));
      logit("MODEM7 Batch Send Started");
      logitarg(" in %s mode\n", prtype(XMITTYPE));
    }
    if ( !xtipflag )
      fprintf(stdout, "Send several Control-X characters to cancel before transfer starts.\n\r");

    if ( xdelayflag )    /* delay if -w requested */
      OSAPI_SEC_SLEEP(TIPDELAY); /* give other side time to return to terminal mode */

    /* Assume about 512 characters needed to be printed out above. So figure time to do so. */
    /* Any less seems to cause timing problems. */
    OSAPI_MSEC_SLEEP((512*CharSize*1000) / ttyspeed );

    setmodes();
    for ( index=2; index<argc; index++ )
    {
      if ( stat(argv[index], &filestatbuf) < 0 )
      {
        logitarg("\n\rFile %s not found\n", argv[index]);
        tlogitarg("\nFile %s not found\n", argv[index]);
        continue;
      }
      rc = sfile(argv[index]);
      flushin();
      restoremodes(FALSE);
      if ( rc == -1 ) return -1;
    }
    sfile("");
    flushin();
    restoremodes(FALSE);

    logit("Batch Send Complete\n");
    tlogit("Batch Send Complete\n");
    OSAPI_SEC_SLEEP(TERM_DELAY);
    return 0;
  }

  if ( xsendflag && !xbatch )
  {
    if ( stat(argv[2], &filestatbuf) < 0 )
    {
      xmodem_error("Can't find requested file", TRUE); /* was false */
      return -1;
    };
    if ( filestatbuf.st_size == 0 )
    {
      xmodem_error("Requested file size is 0", TRUE); /* was false */
      return -1;
    }
    expsect = (filestatbuf.st_size/128)+1;

    fprintf(stdout, "File %s Ready to SEND", argv[2]);
    fprintf(stdout, " in %s mode\n\r", prtype(XMITTYPE));
    fprintf(stdout, "Estimated File Size %ldK, %ld Sectors, %ld Bytes\n\r",
            (filestatbuf.st_size/1024)+1, expsect,
            (long int)filestatbuf.st_size);
    projtime(expsect, stdout);
    if ( !xtipflag )
      fprintf(stdout, "Send several Control-X characters to cancel before transfer starts.\n\r");
    logitarg("Sending in %s mode\n", prtype(XMITTYPE));
    OSAPI_MSEC_SLEEP((512*CharSize*1000) / ttyspeed );

    setmodes();
    rc = sfile(argv[2]);
    flushin();
    restoremodes(FALSE);
    if ( rc == -1 ) return rc;
    OSAPI_SEC_SLEEP(TERM_DELAY);
    return 0;
  }
#ifdef XMODEM_DEBUG
  fclose(xlogfp);
#endif
  return 0;
}
