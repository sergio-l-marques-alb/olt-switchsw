
#include "xmodem.h"

/**  receive a file  **/

/* returns TRUE if in the midst of a batch transfer */
/* returns FALSE if no more files are coming */

/* This routine is one HUGE do-while loop with far to many indented levels.
 * I chose this route to facilitate error processing and to avoid GOTOs.
 * Given the troubles I've had keeping the nested IF statements straight,
 * I was probably mistaken...
 */

int rfile(char *name)
{

  char *sectdisp();
  char *cpm_unix();
  char *strcpy();

  int fd = 0,     /* file descriptor for created file */
  checksum,   /* packet checksum */
  firstchar,  /* first character of a packet */
  sectnum,    /* number of last received packet (modulo 128) */
  sectcurr,   /* second byte of packet--should be packet number (mod 128) */
  sectcomp,   /* third byte of packet--should be complement of sectcurr */
  tmode,      /* text mode if true */
  amode,      /* apple mode if true */
  errors,     /* count of errors for each packet */
  sterrors,   /* count of errors during startup handshake */
  errorflag,  /* set true when packet (or first char of putative packet) is invalid */
  fatalerror, /* set within main "read-packet" Do-While when bad error found */
  inchecksum, /* incoming checksum or CRC */
  expsect,    /* expected number of sectors (YMODEM batch) */
  firstwait,  /* seconds to wait for first character in a packet */
  nocancanabort, /* if true, don't allow CAN-CAN abort */
  bufsize;    /* packet size (128 or 1024) */
  long recvsectcnt;   /* running sector count (128 byte sectors) */
  long modtime;       /* Unix style file mod time from YMODEM header */
  int filemode;       /* Unix style file mode from YMODEM header */
  int serial; /* serial # from YMODEM header */
  int filesleft;    /* # of files left from YMODEM header */
  long totalleft;   /* # of bytes left from YMODEM header */
  int yfiletype;    /* file type from YMODEM header */
  long readbackup;    /* "backup" value for characters read in file */
  /* time_t timep[2];  */  /* used in setting mod time of received file */
  char *p;    /* generic pointer */
  int bufctr; /* number of real chars in read packet */
  unsigned char *nameptr; /* ptr in filename for MODEM7 protocol */
  time_t start;       /* starting time of transfer */
  int openflag = FALSE;   /* is file open for writing? */
  int attempts;                                           /*d1651*/

  logit("----\nXMODEM File Receive Function\n");
  tlogit("----\nXMODEM File Receive Function\n");
  if (xcrcmode)
  {
    logit("CRC mode requested on command line\n");
    tlogit("CRC mode requested on command line\n");
  }

  if (ymodemg)
  {
    logit("YMODEM-G mode requested on command line\n");
    tlogit("YMODEM-G mode requested on command line\n");
  }

  xbatch = FALSE;          /* don't know if really are in batch mode ! */
  fatalerror = FALSE;
  firstwait = WAITFIRST;  /* For first packet, wait short time */
  sectnum     = (int)0;
  errors      = (int)0;
  recvsectcnt = (long)0;
  bufsize = 128;
  modtime = 0l; filemode = 0;
  serial = 0; filesleft = 0; totalleft = 0l; yfiletype = 0;
  xfilelength = 0l; xfileread =0l; xchecklength = FALSE;
  nocancanabort = FALSE;
  attempts=0;                                           /*d1651*/

  tmode = (XMITTYPE == 't') ? TRUE : FALSE;
  amode = (XMITTYPE == 'a') ? TRUE : FALSE;

/* start up transfer */

  sterrors = 0;
  flushin();         /* flush input queue */

  if (ymodemg)
    sendbyte(GCHR);
  else if (xcrcmode && !xmdm7bat)
  {
    sendbyte(CRCCHR);
    if (xlongpack && !xmdm7bat)
      sendbyte(KCHR);
  }
  else
    sendbyte(NAK);


  do                  /* start of MAIN Do-While loop to read packets */
  {
    errorflag = FALSE;
    do              /* start by reading first byte in packet */
    {
      firstchar = readbyte(firstwait);
    }
    while ((firstchar != SOH)
           && (firstchar != STX)
           && (firstchar != EOT)
           && (firstchar != ACK || recvsectcnt > 0)
           && (firstchar != TIMEOUT)
           && (firstchar != CAN || nocancanabort));


    if (firstchar == EOT && !xnoeot)           /* check for REAL EOT */
    {
      flushin();
      sendbyte(NAK);              /* NAK the EOT */
      if ((firstchar = readbyte(5)) != EOT)   /* check next character */
      {
        logit("Spurious EOT detected; ignored\n");
        tlogit("Spurious EOT detected; ignored\n");
        if ((firstchar == SOH) || (firstchar == STX) ||
            (firstchar == ACK && recvsectcnt == 0) ||
            (firstchar == CAN && !nocancanabort) ||
            (firstchar == TIMEOUT))
        {
          int i; /* satisfy splint */
          i = 1;
        }
        else
        {
          firstchar = 0;
          errorflag = TRUE;
        }
      }
    }

    if (firstchar == TIMEOUT)       /* timeout? */
    {
      if (recvsectcnt > 0)
      {
        logitarg("Timeout on Sector %s\n", sectdisp(recvsectcnt,bufsize,1));
        tlogitarg("Timeout on Sector %s\n", sectdisp(recvsectcnt,bufsize,1));
      }
      /*start d1651*/
      attempts++;
      if (attempts==10)
      {
        xmodem_error("ABORTED -- Remote system is not sending file", TRUE);
        return(-1);
      }
      /*d1651*/
      errorflag = TRUE;
    }
    else
      attempts=0;                                                  /*d1651*/

    if (firstchar == CAN)           /* bailing out? (only at beginning or if xcancan flag set) */
    {
      if ((readbyte(3) & 0x7f) == CAN)
      {
        if (openflag)
        {
          (void)close(fd);
          (void)unlink(name);
          xmodem_error("Reception Canceled by CAN-CAN; partial file deleted",TRUE);
          return -1;
        }
        else
          xmodem_error("Reception Canceled by CAN-CAN",TRUE);
        return -1;
      }
      else
      {
        errorflag = TRUE;
        logit("Received single CAN character\n");
        tlogit("Received single CAN character\n");
      }
    }

    if (firstchar == ACK)           /* MODEM7 batch? (only at beginning) */
    {
      int i,c;

      logit("MODEM7 Batch Protocol\n");
      tlogit("MODEM7 Batch Protocol\n");
      nameptr = xbuff;
      checksum = 0;

      for (i=0; i<NAMSIZ; i++)
      {
        c = readbyte(3);

        if (c == CAN)
        {
          if (readbyte(3) == CAN)
          {
            xmodem_error("Program Canceled by CAN-CAN", TRUE);
            return -1;
          }
          else
          {
            logit("Received single CAN character in MODEM7 filename\n");
            tlogit("Received single CAN character in MODEM7 filename\n");
            errorflag = TRUE;
            break;
          }
        }

        if (c == EOT && i == 0)
        {
          sendbyte(ACK);          /* acknowledge EOT */
          logit("MODEM7 Batch Receive Complete\n");
          tlogit("MODEM7 Batch Receive Complete\n");
          return(FALSE);
        }

        if (c == TIMEOUT)
        {
          logit("Timeout waiting for MODEM7 filename character\n");
          tlogit("Timeout waiting for MODEM7 filename character\n");
          errorflag = TRUE;
          break;
        }

        if (c == BAD_NAME)
        {
          logit("Error during MODEM7 filename transfer\n");
          tlogit("Error during MODEM7 filename transfer\n");
          errorflag = TRUE;
          break;
        }

        *nameptr++ = (unsigned char)c;
        checksum += c;
        sendbyte(ACK);
      }

      if (!errorflag)
      {
        c = readbyte(3);
        if (c == CTRLZ)     /* OK; end of string found */
        {
          sendbyte(checksum + CTRLZ);
          if (readbyte(15) == ACK)     /* file name found! */
          {
            xmdebug("MODEM7 file name OK");
            *nameptr = '\000';  /* unixify the file name */
            name = cpm_unix(xbuff);
            xbatch = TRUE;
            logitarg("MODEM7 file name: %s\n", name);
            tlogitarg("MODEM7 file name: %s\n", name);
            errors = 0;     /* restart crc handshake */
            OSAPI_SEC_SLEEP(2);
          }
          else
          {
            logit("Checksum error in MODEM7 filename\n");
            tlogit("Checksum error in MODEM7 filename\n");
            errorflag = TRUE;
          }
        }
        else
        {
          logit("Length error in MODEM7 filename\n");
          tlogit("Length error in MODEM7 filename\n");
          errorflag = TRUE;
        }
      }
    }


    if (firstchar == SOH || firstchar == STX)  /* start reading packet */
    {
      bufsize = (firstchar == SOH) ? 128 : 1024;

      if (recvsectcnt == 0)           /* 1st data packet, initialize */
      {
        if (bufsize == 1024)
        {
          logit("1K packet mode chosen\n");
          tlogit("1K packet mode chosen\n");
        }
        start = 0;
        errors = 0;
        firstwait = PACKWAIT;
      }

      sectcurr = readbyte(3);
      sectcomp = readbyte(3);
      if ((sectcurr + sectcomp) == 0xff)  /* is packet number checksum correct? */
      {
        if (sectcurr == ((sectnum+1) & 0xff))   /* is packet number correct? */
        {
          if (xdebugflag == TRUE)
            fprintf(xlogfp,"DEBUG: packet with sector number %d started\n\r", sectnum);

          /* Read, process and calculate checksum for a buffer of data */

          readbackup = xfileread;
          if (readbuf(bufsize, 1, tmode, amode, recvsectcnt, &checksum, &bufctr) != TIMEOUT)
          {

            /* verify checksum or CRC */

            if (xcrcmode)
            {
              checksum &= 0xffff;
              inchecksum = readbyte(3) & 0xff;  /* get 16-bit CRC */
              inchecksum = (inchecksum<<8) | (readbyte(3) & 0xff);
            }

            else
              inchecksum = readbyte(3) & 0xff;  /* get simple 8-bit checksum */

            if (inchecksum == checksum) /* good checksum, hence good packet */
            {
              xmdebug("checksum ok");
              errors = 0;
              recvsectcnt += (bufsize == 128) ? 1 : 8;
              nocancanabort = xcancan ? FALSE : TRUE;
              sectnum = sectcurr;

              if (!openflag)      /* open output file if necessary */
              {
                openflag = TRUE;
                if ((fd = creat(name, CREATMODE)) < 0)
                {
                  sendbyte(CAN); sendbyte(CAN); sendbyte(CAN);
                  xmodem_error("Can't create file for receive", TRUE);
                  return -1;
                }
                if (!xbatch)
                {
                  logitarg("File Name: %s\n", name);
                  tlogitarg("File Name: %s\n", name);
                }
              }

              if (write(fd, (char *) xbuff, bufctr) != bufctr)
              {
                (void)close(fd);
                (void)unlink(name);
                xmodem_error("File Write Error", TRUE);
                return -1;
              }
              else
              {
                if (xtipflag && recvsectcnt % 32 == 0)
                  tlogitarg("Sector %s received\n", sectdisp(recvsectcnt,bufsize,0));
                if (!ymodemg)
                {
                  flushin();          /* flush input */
                  sendbyte(ACK);      /* ACK the received packet */
                }
              }
            }

            /* Start handling various errors and special conditions */

            else        /* bad checksum */
            {
              logitarg("Checksum Error on Sector %s:  ", sectdisp(recvsectcnt,bufsize,1));
              logitarg("sent=%x  ", (unsigned int)inchecksum);
              logitarg("recvd=%x\n", (unsigned int)checksum);
              tlogitarg("Checksum Error on Sector %s:  ", sectdisp(recvsectcnt,bufsize,1));
              tlogitarg("sent=%x  ", (unsigned int)inchecksum);
              tlogitarg("recvd=%x\n", (unsigned int)checksum);
              xfileread = readbackup;
              errorflag = TRUE;
              if (ymodemg)
                fatalerror = TRUE;
            }
          }

          else    /* read timeout */
          {
            logitarg("Timeout while reading sector %s\n",sectdisp(recvsectcnt,bufsize,1));
            tlogitarg("Timeout while reading sector %s\n",sectdisp(recvsectcnt,bufsize,1));
            xfileread = readbackup;
            errorflag = TRUE;
            if (ymodemg)
              fatalerror = TRUE;
          }
        }

        else        /* sector number is wrong OR Ymodem filename */
        {
          if (sectcurr == 0 && recvsectcnt == 0)  /* Ymodem file-name packet */
          {
            logit("YMODEM Batch Protocol\n");
            tlogit("YMODEM Batch Protocol\n");

            /* Read and process a file-name packet */

            if (readbuf(bufsize, 1, FALSE, FALSE, recvsectcnt, &checksum, &bufctr) != TIMEOUT)
            {

              /* verify checksum or CRC */

              if (xcrcmode)
              {
                checksum &= 0xffff;
                inchecksum = readbyte(3) & 0xff;  /* get 16-bit CRC */
                inchecksum = (inchecksum<<8) | (readbyte(3) & 0xff);
              }

              else
                inchecksum = readbyte(3) & 0xff;  /* get simple 8-bit checksum */

              if (inchecksum == checksum) /* good checksum, hence good filename */
              {
                xmdebug("checksum ok");
                (void)strcpy(name, (char *)xbuff);
                expsect = ((xbuff[bufsize-1]<<8) | xbuff[bufsize-2]);
                xbatch = TRUE;
                xymdmbat = TRUE;
                if (strlen(name) == 0)  /* check for no more files */
                {
                  flushin();          /* flush input */
                  sendbyte(ACK);      /* ACK the packet */
                  logit("YMODEM Batch Receive Complete\n");
                  tlogit("YMODEM Batch Receive Complete\n");
                  return(FALSE);
                }
                unixify(name);       /* make filename canonical */

                /* read rest of YMODEM header */
                p = (char *)xbuff + strlen((char *)xbuff) + 1;
                if (xdebugflag == TRUE)
                  fprintf(xlogfp, "DEBUG: header info: %s\n\r", p);
                sscanf(p, "%ld%lo%o%o%d%ld%d",
                       &xfilelength, (unsigned long int *)&modtime, (unsigned int *)&filemode,
                       (unsigned int *)&serial, &filesleft, &totalleft,
                       &yfiletype);
                logitarg("YMODEM file name: %s\n", name);
                tlogitarg("YMODEM file name: %s\n", name);
                xfileread = 0l;
                if (xfilelength)
                {
                  xchecklength = TRUE;
                  logitarg("YMODEM file size: %ld\n", xfilelength);
                  tlogitarg("YMODEM file size: %ld\n", xfilelength);
                }
                else if (expsect)
                  logitarg("YMODEM estimated file length %d sectors\n", expsect);
                if (modtime)
                {
                  logitarg("YMODEM file date: %s", ctime((time_t *)&modtime));
                }
                if (filemode)
                  logitarg("YMODEM file mode: %o\n", (unsigned int)filemode);

                if (filesleft)
                {
                  logitarg("YMODEM %d file(s) left to receive ", filesleft);
                  logitarg("containing %ld bytes\n", totalleft);
                  tlogitarg("YMODEM %d file(s) left to receive ", filesleft);
                  tlogitarg("containing %ld bytes\n", totalleft);
                }
                if (serial)
                  logitarg("YMODEM sender's serial number: %d\n", serial);
                if (yfiletype)
                  logitarg("YMODEM file type %d\n", yfiletype);

                openflag = TRUE;  /* open the file for writing */
                if ((fd = creat(name, CREATMODE)) < 0)
                {
                  sendbyte(CAN); sendbyte(CAN); sendbyte(CAN);
                  xmodem_error("Can't create file for receive", TRUE);
                  return -1;
                }
                if (!ymodemg)
                {
                  flushin();    /* flush the input stream */
                  sendbyte(ACK);    /* ACK the filename packet */
                }
                /* do initial handshake to start file transfer */
                if (ymodemg)
                  sendbyte(GCHR);
                else if (xcrcmode)
                {
                  sendbyte(CRCCHR);
                  if (xlongpack)
                    sendbyte(KCHR);
                }
                else
                  sendbyte(NAK);
                firstwait = WAITFIRST;  /* reset waiting time */
                sterrors = 0;
              }

              else                /* bad filename checksum */
              {
                logit("checksum error on filename sector\n");
                tlogit("checksum error on filename sector\n");
                errorflag = TRUE;
                if (ymodemg)
                  fatalerror = TRUE;
              }
            }
            else
            {
              logit("Timeout while reading filename packet\n");
              tlogit("Timeout while reading filename packet\n");
              errorflag = TRUE;
              if (ymodemg)
                fatalerror = TRUE;
            }
          }

          else if (sectcurr == sectnum)   /* duplicate sector? */
          {
            logitarg("Duplicate sector %s flushed\n", sectdisp(recvsectcnt,bufsize,0));
            tlogitarg("Duplicate sector %s flushed\n", sectdisp(recvsectcnt,bufsize,0));
            if (ymodemg)
            {
              errorflag = TRUE;
              fatalerror = TRUE;
            }
            else
            {
              reallyflush();                  /* REALLY flush input */
              sendbyte(ACK);
            }
          }
          else                /* no, real phase error */
          {
            logitarg("Phase Error - Expected packet is %s\n", sectdisp(recvsectcnt,bufsize,1));
            tlogitarg("Phase Error - Expected packet is %s\n", sectdisp(recvsectcnt,bufsize,1));
            errorflag = TRUE;
            fatalerror = TRUE;
          }
        }
      }

      else        /* bad packet number checksum */
      {
        logitarg("Header Sector Number Error on Sector %s\n", sectdisp(recvsectcnt, bufsize,1));
        tlogitarg("Header Sector Number Error on Sector %s\n", sectdisp(recvsectcnt, bufsize,1));
        errorflag = TRUE;
        if (ymodemg)
          fatalerror = TRUE;
        else
          reallyflush();
      }

    }           /* END reading packet loop */

    if (errorflag && !fatalerror && recvsectcnt != 0)   /* Handle errors */
    {
      errors++;

      if (errors >= ERRORMAX)     /* over error limit? */
        fatalerror = TRUE;
      else                        /* flush input and NAK the packet */
      {
        reallyflush();
        sendbyte(NAK);
      }
    }

    if (recvsectcnt == 0 && errorflag && !fatalerror && firstchar != EOT)   /* handle startup handshake */
    {
      sterrors++;
      firstwait = WAITFIRST + sterrors;

      if (sterrors >= STERRORMAX)
        fatalerror = TRUE;

      else if (xcrcmode && xmdm7bat && !xbatch)
        sendbyte(NAK);

      else if (xcrcmode && sterrors == CRCSWMAX && !xymdmbat)
      {
        xcrcmode = FALSE;
        logit("Sender not accepting CRC request, changing to checksum\n");
        tlogit("Sender not accepting CRC request, changing to checksum\n");
        sendbyte(NAK);
      }

      else if (!xcrcmode && sterrors == CRCSWMAX && !xymdmbat)
      {
        xcrcmode = TRUE;
        logit("Sender not accepting checksum request, changing to CRC\n");
        tlogit("Sender not accepting checksum request, changing to CRC\n");
        sendbyte(CRCCHR);
        if (xlongpack && !xmdm7bat)
          sendbyte(KCHR);
      }

      else if (ymodemg)
        sendbyte(GCHR);

      else if (xcrcmode)
      {
        sendbyte(CRCCHR);
        if (xlongpack && !xmdm7bat)
          sendbyte(KCHR);
      }

      else
        sendbyte(NAK);
    }
  }
  while ((firstchar != EOT) && !fatalerror);   /* end of MAIN Do-While */

  if ((firstchar == EOT) && !fatalerror)  /* normal exit? */
  {
    if (openflag)       /* close the file */
      (void)close(fd);
    sendbyte(ACK);      /* ACK the EOT */
    logit("Receive Complete\n");
    tlogit("Receive Complete\n");
    if (xtipflag)
      prtime (recvsectcnt,  0, stdout);

    if (openflag && modtime)   /* set file modification time */
    {
      /* NO vxWorks capability to easily set file mod time now */
      /* timep[0] = time((time_t *) 0); */
      /* timep[1] = modtime; */
      /* utime(name, timep); */
    }

    if (xbatch)          /* send appropriate return code */
      return(TRUE);
    else
      return(FALSE);
  }
  else                /* no, error exit */
  {
    if (openflag)
    {
      sendbyte(CAN); sendbyte(CAN); sendbyte(CAN); sendbyte(CAN); sendbyte(CAN);
      (void)close(fd);
      (void)unlink(name);
      reallyflush();  /* wait for line to settle */
      xmodem_error("ABORTED -- Too Many Errors -- Deleting File", TRUE);
      return -1;
    }
    else if (recvsectcnt != 0)
    {
      sendbyte(CAN); sendbyte(CAN); sendbyte(CAN); sendbyte(CAN); sendbyte(CAN);
      reallyflush();  /* wait for line to settle */
      xmodem_error("ABORTED -- Too Many Errors", TRUE);
      return -1;
    }
    else
    {
      reallyflush();  /* wait for line to settle */
      xmodem_error("ABORTED -- Remote system is not responding", TRUE);
      return -1;
    }

  }

  return(FALSE);
}
