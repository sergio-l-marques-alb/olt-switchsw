/**  send a file  **/

/*
 * Operation of this routine depends on on xmdm7bat and xymdmbat flags.
 *
 * If "name" is NULL; close out the xbatch send.
 */

#include "xmodem.h"

int sfile(char *name)
    {

    char *sectdisp();
    char *strcpy();
    char *unix_cpm();
    char *cpmify();
    long countnl();

    extern unsigned short crctab[1<<B]; /* CRC-16 constant values, see getput.c */

    register int bufctr,        /* array index for data buffer */
    sectnum;            /* packet number for packet header */

    register unsigned short checksum;   /* checksum/crc */

    char blockbuf[BBUFSIZ+6];   /* holds packet as it is constructed */

    struct stat filestatbuf;    /* file status info */

    int fd = 0,         /* file descriptor for file being transmitted */
    attempts,       /* number of attempts made to transmit a packet */
    nlflag,         /* flag that we have to send a LF in next packet */
    sendfin,        /* flag that we are sending the last packet */
    closeout,       /* flag that we are closing out batch send */
    startup,        /* flag that we are starting batch send */
    tmode,          /* TRUE for text mode */
    amode,          /* TRUE for apple mode */
    filepack,       /* TRUE when sending first packet */
    buf1024,        /* TRUE when sending 1K packets */
    bbufcnt,        /* array index for packet */
    firstchar,      /* first character in protocol transaction */
    bufsize,        /* packet size (128 or 1024) */
    sendresp;       /* response char to sent block received from remote*/
    long sentsect;      /* count of 128 byte sectors actually sent */
    long expsect = 0;       /* count of 128 byte sectors expected to be sent */
    time_t start;       /* starting time of transfer */
    char c;

    xnbchr = 0;  /* clear buffered read char count */

    xcrcmode = FALSE;   /* Receiver determines use of crc or checksum */
    ymodemg = FALSE;    /* Receiver determines use YMODEM-G */

    buf1024 = xlongpack;    /* set packet size flag to command line switch */

    closeout = FALSE; startup = TRUE; filepack = FALSE; /* indicate state of batch transfer */

    tmode = (XMITTYPE == 't') ? TRUE : FALSE;   /* set text mode */
    amode = (XMITTYPE == 'a') ? TRUE : FALSE;   /* set apple mode */

    /* Check on NULL file name */
    if (strcmp(name,"") == 0)
        {
        if (xbatch)
            closeout = TRUE;
        else
            {
            sendbyte(CAN); sendbyte(CAN); sendbyte(CAN);
            xmodem_error("NULL file name in send", TRUE);
            return -1;
            }
        }

    if (!closeout)      /* Are we closing down batch? */
        {           /* no; let's send a file */
        logit("----\nXMODEM Send Function\n");
        tlogit("----\nXMODEM Send Function\n");

        if ((fd = open(name, 0,0)) < 0)
            {
            sendbyte(CAN); sendbyte(CAN); sendbyte(CAN);
                xmodem_error("Can't open file for send", TRUE);
            return -1;
            }

        (void)stat(name, &filestatbuf);  /* get file status bytes */
        if (tmode)         /* count up NLs */
            filestatbuf.st_size += countnl(fd);
        expsect = (filestatbuf.st_size/128) + 1;

        if (xtipflag)
            {
                fprintf(stderr, "File Name: %s\n\r", name);
            fprintf(stderr,"File Size %ldK, %ld Records, %ld Bytes\n\r",
              (filestatbuf.st_size/1024)+1, expsect, (long int)filestatbuf.st_size);
            projtime(expsect, stderr);
            }
        }
    else
        {
        logit("----\nXMODEM Send Function\n");
        logit("Closing down Batch Transmission\n");
        tlogit("Closing down Batch Transmission\n");
        }


    bufsize = buf1024 ? 1024 : 128;     /* set sector size */
    if (buf1024 && !closeout)
        {
        logit("1K packet mode chosen on command line\n");
        tlogit("1K packet mode chosen on command line\n");
        }

        sendfin = nlflag = FALSE;
    attempts = 0;

    /* wait for and read startup character */
restart:
    do
        {
        while (((firstchar=readbyte(1)) != NAK) && (firstchar != CRCCHR) && (firstchar != GCHR) && (firstchar != CAN))
            if (++attempts > NAKMAX)
                {
                if (xmdm7bat && startup)
                    {
                    sendbyte(ACK); sendbyte(EOT);
                    }
                xmodem_error("Remote System Not Responding", TRUE);
                return -1;
                }

        if ((firstchar & 0x7f) == CAN)
            if (readbyte(3) == CAN) {
                xmodem_error("Send Canceled by CAN-CAN",TRUE);
                return -1;
                   };

        if (firstchar == GCHR)
            {
            xcrcmode = TRUE;
            ymodemg = TRUE;
            xcancan = TRUE;
            if (!closeout)
                {
                logit("Receiver invoked YMODEM-G and CRC modes\n");
                tlogit("Receiver invoked YMODEM-G and CRC modes\n");
                }
            }
        if (firstchar == CRCCHR)
            {
            xcrcmode = TRUE;
            if (!closeout)
                {
                logit("Receiver invoked CRC mode\n");
                tlogit("Receiver invoked CRC mode\n");
                }
            if (readbyte(1) == KCHR)
                {
                buf1024 = TRUE;
                logit("Receiver invoked 1K packet mode\n");
                tlogit("Receiver invoked 1K packet mode\n");
                }
            }
        }
    while (firstchar != NAK && firstchar != CRCCHR && firstchar != GCHR);

    if (xmdm7bat && closeout)   /* close out MODEM7 batch */
        {
        sendbyte(ACK); sendbyte (EOT);
        flushin(); readbyte(2);     /* flush junk */
        return 0;
        }

    if (xmdm7bat && startup)        /* send MODEM7 file name */
        {
        if (send_name(unix_cpm(name)) == -1)
            {
            attempts = 0;
            goto restart;
            }
        startup = FALSE;
        attempts = 0;
        goto restart;
        }

    sectnum = 1;

    if (xymdmbat)   /* Fudge for YMODEM transfer (to send name packet) */
        {
        sectnum = 0;
        bufsize = 128;
        filepack = TRUE;
        }

    attempts = (int)0;
    sentsect = (long)0;
    start = 0;

        do          /* outer packet building/sending loop; loop till whole file is sent */
        {

        if (closeout && xymdmbat && sectnum == 1)   /* close out YMODEM */
            return 0;

        if (xymdmbat && sectnum == 1)           /* get set to send YMODEM data packets */
            {
            bufsize = buf1024 ? 1024 : 128;

            do      /* establish handshaking again */
                {
                while (((firstchar=readbyte(2)) != CRCCHR) && (firstchar != GCHR) && (firstchar != NAK) && (firstchar != CAN))
                    if (++attempts > ERRORMAX) {
                        xmodem_error("YMODEM protocol botch, C or G expected", TRUE);
                        return -1;
                        };
                if ((firstchar&0x7f) == CAN)
                    if (readbyte(3) == CAN) {
                        xmodem_error("Send Canceled by CAN-CAN", TRUE);
                        return -1;
                        };
                }
            while ((firstchar != CRCCHR) &&  (firstchar != GCHR) && (firstchar != NAK));

            attempts = 0;
            }

        if ((bufsize == 1024) && (attempts > KSWMAX))
            {
            logit("Reducing packet size to 128 due to excessive errors\n");
            tlogit("Reducing packet size to 128 due to excessive errors\n");
            bufsize = 128;
            }

        if ((bufsize == 1024) && ((expsect - sentsect) < 8))
            {
            logit("Reducing packet size to 128 for tail end of file\n");
            tlogit("Reducing packet size to 128 for tail end of file\n");
            bufsize = 128;
            }

        if (sectnum > 0)    /* data packet */
            {
            for (bufctr=0; bufctr < bufsize;)
                    {
                if (nlflag)
                        {
                    xbuff[bufctr++] = LF;  /* leftover newline */
                            nlflag = FALSE;
                        }
                if (getbyte(fd, &c) == EOF)
                    {
                    sendfin = TRUE;  /* this is the last sector */
                    if (!bufctr)  /* if EOF on sector boundary */
                            break;  /* avoid sending extra sector */
                        xbuff[bufctr++] = CTRLZ;  /* pad with Ctrl-Z for CP/M EOF (even do for binary files) */
                    continue;
                        }

                if (tmode && c == LF)  /* text mode & Unix newline? */
                        {
                    xbuff[bufctr++] = CR;  /* insert carriage return */
                        if (bufctr < bufsize)
                                xbuff[bufctr++] = LF;  /* insert LF */
                        else
                            nlflag = TRUE;  /* insert on next sector */
                    }
                else if (amode && c == LF)   /* Apple mode & Unix newline? */
                    xbuff[bufctr++] = CR; /* substitute CR */
                else
                    xbuff[bufctr++] = (unsigned char)c;  /* copy the char without change */
                    }

                if (!bufctr)  /* if EOF on sector boundary */
                    break;  /* avoid sending empty sector */
            }

        else        /* YMODEM filename packet */
            {
            for (bufctr=0; bufctr<1024; bufctr++)  /* zero packet */
                xbuff[bufctr]=0;
            if (!closeout)
                {
                (void)strcpy((char *)xbuff, cpmify(name));

                    /* put in file name, length, mode, */
                    /* dummy SN, files, bytes remaining and file type */
                    {
                    register char *p;
                    p = (char *)xbuff + strlen((char *)xbuff) + 1;
                    sprintf(p, "%lu %o %o 0 %d %ld 0", filestatbuf.st_size,
                      (unsigned int)filestatbuf.st_mtime, filestatbuf.st_mode,
                      yfilesleft, ytotleft);
                    if (xdebugflag == TRUE)
                        fprintf(xlogfp, "DEBUG: YMODEM header information: %s %s\n\r", (char *)xbuff, p);
                    }
                if (xbuff[125])     /* need to have long packet? */
                    {
                    bufsize = 1024;
                    if (xdebugflag == TRUE)
                        fprintf(xlogfp, "DEBUG: YMODEM header sent in 1024 byte packet\n\r");
                    }
                xbuff[bufsize-2]    = (unsigned char)(expsect & 0xff);        /* put in KMD kludge information */
                xbuff[bufsize-1] = (unsigned char)((expsect >> 8) & 0xff);

                /* update totals */
                ytotleft -= filestatbuf.st_size;
                if (--yfilesleft <= 0)
                    ytotleft = 0;
                if (ytotleft < 0)
                    ytotleft = 0;
                }
            }

        bbufcnt = 0;        /* start building block to be sent */
        blockbuf[bbufcnt++] = (bufsize == 1024) ? STX : SOH;    /* start of packet char */
        blockbuf[bbufcnt++] = sectnum;      /* current sector # */
        blockbuf[bbufcnt++] = ~sectnum;   /* and its complement */

                checksum = 0;  /* initialize checksum */
                for (bufctr=0; bufctr < bufsize; bufctr++)
                {
            blockbuf[bbufcnt++] = (char)xbuff[bufctr];

            if (xcrcmode)
                checksum = (checksum<<B) ^ crctab[(checksum>>(W-B)) ^ xbuff[bufctr]];

            else
                        checksum = (unsigned short)((checksum+xbuff[bufctr]) & 0xff);
                }

        if (xcrcmode)       /* put in CRC */
            {
            checksum &= 0xffff;
            blockbuf[bbufcnt++] = ((checksum >> 8) & 0xff);
            blockbuf[bbufcnt++] = (checksum & 0xff);
            }
        else            /* put in checksum */
            blockbuf[bbufcnt++] = (char)checksum;

                attempts = 0;

                do              /* inner packet loop */
                    {

            writebuf(blockbuf, bbufcnt);    /* write the block */
            if (!ymodemg)
                flushin();              /* purge anything in input queue */

            if (xdebugflag == TRUE)
                fprintf (xlogfp, "DEBUG: %d byte Packet %02xh (%02xh) sent, checksum %02xh %02xh\n\r",
                bbufcnt, (unsigned int)blockbuf[1]&0xff, (unsigned int)blockbuf[2]&0xff, (unsigned int)blockbuf[bufsize+3]&0xff, (unsigned int)blockbuf[bufsize+4]&0xff);

                    attempts++;
            sendresp = (ymodemg) ? ACK : readbyte(10);  /* get response from remote  (or fake it for YMODEM-G) */

            if (sendresp != ACK)
                {
                if (sendresp == TIMEOUT)
                    {
                    logitarg("Timeout on sector %s\n",sectdisp(sentsect,bufsize,1));
                    tlogitarg("Timeout on sector %s\n",sectdisp(sentsect,bufsize,1));
                    }
                if (sendresp == CAN)
                    {
                    if (xcancan)
                        {
                        if (readbyte(3) == CAN) {
                            xmodem_error("Send Canceled by CAN-CAN",TRUE);
                            return -1;
                            };
                        }
                    else
                        {
                    logitarg("ignored CAN on sector %s\n",sectdisp(sentsect,bufsize,1));
                    tlogitarg("ignored CAN on sector %s\n",sectdisp(sentsect,bufsize,1));
                        }
                    }
                else if (sendresp == NAK)
                    {
                    logitarg("NAK on sector %s\n",sectdisp(sentsect,bufsize,1));
                    tlogitarg("NAK on sector %s\n",sectdisp(sentsect,bufsize,1));
                    }
                else
                    {
                    logitarg("Non-ACK on sector %s\n",sectdisp(sentsect,bufsize,1));
                    tlogitarg("Non-ACK on sector %s\n",sectdisp(sentsect,bufsize,1));
                    }
                }
                    }
            while((sendresp != ACK) && (attempts < ERRORMAX));  /* close of inner loop */

            sectnum++;  /* increment to next sector number */
        if (!filepack)
            sentsect += (bufsize == 128) ? 1 : 8;
        filepack = FALSE;
        if (xtipflag && sentsect % 32 == 0)
            tlogitarg("Sector %s sent\n", sectdisp(sentsect,bufsize,0));
            }
        while (!sendfin && ( attempts < ERRORMAX)); /* end of outer loop */

    if (attempts >= ERRORMAX)
        {
        sendbyte(CAN); sendbyte(CAN); sendbyte(CAN); sendbyte(CAN); sendbyte(CAN);
        xmodem_error ("Too many errors in transmission", TRUE);
        return -1;
        }

        sendbyte(EOT);  /* send 1st EOT to close down transfer */
        attempts = 0;

        while ((readbyte(15) != ACK) && (attempts++ < EOTMAX))  /* wait for ACK of EOT */
        {
        if (attempts > 1)
            {
            logitarg("EOT not ACKed, try %d\n", attempts);
            tlogitarg("EOT not ACKed, try %d\n", attempts);
            }
        sendbyte(EOT);
        }

        if (attempts >= RETRYMAX) {
        xmodem_error("Remote System Not Responding on Completion", TRUE);
        return -1;
          };

        (void)close(fd);

        logit("Send Complete\n");
        tlogit("Send Complete\n");
    if (xtipflag)
        prtime(sentsect, 0, stderr);
   return 0;
}
