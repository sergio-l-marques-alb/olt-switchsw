/*
 * $Id: receive.c,v 1.1 2011/04/18 17:11:09 mruas Exp $
 */
/**  receive a file  **/

/* returns TRUE if in the midst of a batch transfer */
/* returns FALSE if no more files are coming */

/* This routine is one HUGE do-while loop with far to many indented levels.
 * I chose this route to facilitate error processing and to avoid GOTOs.
 * Given the troubles I've had keeping the nested IF statements straight,
 * I was probably mistaken...
 */
#include "xmodem.h"

/* External and forward references */

extern void flushin(void);
extern void sendbyte(char);
extern int readbyte(int);
extern void error(char *, int);
extern void xmdebug(char *);
extern int readbuf(int, int, int, int, long, int *, int *);
extern void unixify(char *);
extern void reallyflush(void);
extern int prtime(long, time_t, FILE *);

int
rfile(char *name)
{
    char *sectdisp();
    char *cpm_unix();
    char *strcpy();

    int fd = -1,         /* file descriptor for created file */
	checksum,   /* packet checksum */
	firstchar,  /* first character of a packet */
	sectnum,    /* number of last received packet (modulo 128) */
	sectcurr,   /* 2nd byte of packet--should be packet number (mod 128) */
	sectcomp,   /* 3rd byte of packet--should be complement of sectcurr */
	tmode,      /* text mode if true */
	amode,      /* apple mode if true */
	errors,     /* count of errors for each packet */
	sterrors,   /* count of errors during startup handshake */
	errorflag,  /* set true when packet (or first char of putative
		     * packet) is invalid
		     */
	fatalerror, /* set within main "read-packet" Do-While
		     * when bad error found
		     */
	inchecksum, /* incoming checksum or CRC */
	expsect,    /* expected number of sectors (YMODEM batch) */
	firstwait,  /* seconds to wait for first character in a packet */
	nocancanabort, /* if true, don't allow CAN-CAN abort */
	bufsize;       /* packet size (128 or 1024) */
    long recvsectcnt;  /* running sector count (128 byte sectors) */
    long modtime;      /* Unix style file mod time from YMODEM header */
    int filemode;      /* Unix style file mode from YMODEM header */
    int serial;        /* serial # from YMODEM header */
    int filesleft;     /* # of files left from YMODEM header */
    long totalleft;    /* # of bytes left from YMODEM header */
    int yfiletype;     /* file type from YMODEM header */
    long readbackup;   /* "backup" value for characters read in file */
#if 0
    /* TODO: support modifying local filename with date/time stamp */
    time_t timep[2];   /* used in setting mod time of received file */
#endif
    char *p;           /* generic pointer */
    int bufctr;        /* number of real chars in read packet */
    unsigned char *nameptr;  /* ptr in filename for MODEM7 protocol */
    time_t start = 0;        /* starting time of transfer */
    int openflag = FALSE;    /* is file open for writing? */

    logit("----\nXMODEM File Receive Function\n");
    tlogit("----\nXMODEM File Receive Function\n");

    if (CRCMODE){
	logit("CRC mode requested on command line\n");
	tlogit("CRC mode requested on command line\n");
    }

    if (YMODEMG){
	logit("YMODEM-G mode requested on command line\n");
	tlogit("YMODEM-G mode requested on command line\n");
    }

    BATCH = FALSE;          /* don't know if really are in batch mode ! */
    fatalerror = FALSE;
    firstwait = WAITFIRST;  /* For first packet, wait short time */
    sectnum = errors = recvsectcnt = 0;
    bufsize = 128;
    modtime = 0l; filemode = 0;
    serial = 0; filesleft = 0; totalleft = 0l; yfiletype = 0;
    filelength = 0l; fileread =0l; CHECKLENGTH = FALSE;
    nocancanabort = FALSE;

    tmode = (XMITTYPE == 't') ? TRUE : FALSE;
    amode = (XMITTYPE == 'a') ? TRUE : FALSE;

    /* start up transfer */

    sterrors = 0;
    flushin();         /* flush input queue */

    if (YMODEMG)
	sendbyte(GCHR);
    else if (CRCMODE && !MDM7BAT){
	sendbyte(CRCCHR);
	if (LONGPACK && !MDM7BAT)
	    sendbyte(KCHR);
    }
    else
	sendbyte(NAK);


    do{                  /* start of MAIN Do-While loop to read packets */
	errorflag = FALSE;
	do{              /* start by reading first byte in packet */
	    firstchar = readbyte(firstwait);
	} while ((firstchar != SOH) 
		 && (firstchar != STX) 
		 && (firstchar != EOT) 
		 && (firstchar != ACK || recvsectcnt > 0) 
		 && (firstchar != TIMEOUT) 
		 && (firstchar != CAN || nocancanabort));

	if (firstchar == EOT && !NOEOT){           /* check for REAL EOT */
	    flushin();
	    sendbyte(NAK);              /* NAK the EOT */
	    if ((firstchar = readbyte(5)) != EOT){   /* check next character */
		logit("Spurious EOT detected; ignored\n");
		tlogit("Spurious EOT detected; ignored\n");
		if ((firstchar == SOH) || (firstchar == STX) ||
		    (firstchar == ACK && recvsectcnt == 0) ||
		    (firstchar == CAN && !nocancanabort) ||
		    (firstchar == TIMEOUT))
		    ;
		else{
		    firstchar = 0;
		    errorflag = TRUE;
		}
	    }
	}	

	if (firstchar == TIMEOUT){       /* timeout? */
	    if (recvsectcnt > 0){
		logitarg("Timeout on Sector %s\n",
			 sectdisp(recvsectcnt,bufsize,1));
		tlogitarg("Timeout on Sector %s\n",
			  sectdisp(recvsectcnt,bufsize,1));
	    }
	    errorflag = TRUE;
	}
	/* bailing out? (only at beginning or if CANCAN flag set) */
	if (firstchar == CAN){
	    if ((readbyte(3) & 0x7f) == CAN){
		if (openflag){
		    close(fd);
		    unlink(name);
		    error("Reception Canceled by CAN-CAN;"
			  " partial file deleted",TRUE);
		}
		else
		    error("Reception Canceled by CAN-CAN",TRUE);
	    }
	    else{
		errorflag = TRUE;
		logit("Received single CAN character\n");
		tlogit("Received single CAN character\n");
	    }
	}
	/* MODEM7 batch? (only at beginning) */
	if (firstchar == ACK){
	    int i,c; 

	    logit("MODEM7 Batch Protocol\n");
	    tlogit("MODEM7 Batch Protocol\n");
	    nameptr = buff;
	    checksum = 0;

	    for (i=0; i<NAMSIZ; i++){
		c = readbyte(3);
		
		if (c == CAN){
		    if (readbyte(3) == CAN)
			error("Program Canceled by CAN-CAN", TRUE);
		    else{
			logit("Received single CAN character"
			      " in MODEM7 filename\n");
			tlogit("Received single CAN character"
			       " in MODEM7 filename\n");
			errorflag = TRUE;
			break;
		    }
		}

		if (c == EOT && i == 0){
		    sendbyte(ACK);          /* acknowledge EOT */
		    logit("MODEM7 Batch Receive Complete\n");
		    tlogit("MODEM7 Batch Receive Complete\n");
		    return (FALSE);
		}

		if (c == TIMEOUT){
		    logit("Timeout waiting for MODEM7 filename character\n");
		    tlogit("Timeout waiting for MODEM7 filename character\n");
		    errorflag = TRUE;
		    break;
		}

		if (c == BAD_NAME){
		    logit("Error during MODEM7 filename transfer\n");
		    tlogit("Error during MODEM7 filename transfer\n");
		    errorflag = TRUE;
		    break;
		}

		*nameptr++ = c;
		checksum += c;
		sendbyte(ACK);
	    }

	    if (!errorflag){
		c = readbyte(3);
		if (c == CTRLZ){     /* OK; end of string found */
		    sendbyte(checksum + CTRLZ);
		    if (readbyte(15) == ACK){     /* file name found! */
			xmdebug("MODEM7 file name OK");
			*nameptr = '\000';  /* unixify the file name */
			name = cpm_unix(buff);
			BATCH = TRUE;
			logitarg("MODEM7 file name: %s\n", name);
			tlogitarg("MODEM7 file name: %s\n", name);
			errors = 0;     /* restart crc handshake */
			sleep(2);       /* give other side a chance */
		    }
		    else{
			logit("Checksum error in MODEM7 filename\n");
			tlogit("Checksum error in MODEM7 filename\n");
			errorflag = TRUE;
		    }
		}
		else{
		    logit("Length error in MODEM7 filename\n");
		    tlogit("Length error in MODEM7 filename\n");
		    errorflag = TRUE;
		}
	    }
	}
	/* start reading packet */
	if (firstchar == SOH || firstchar == STX){
	    bufsize = (firstchar == SOH) ? 128 : 1024;

	    if (recvsectcnt == 0){           /* 1st data packet, initialize */
		if (bufsize == 1024){
		    logit("1K packet mode chosen\n");
		    tlogit("1K packet mode chosen\n");
		}
		start = time((time_t *) 0);
		errors = 0;
		firstwait = PACKWAIT;
	    }

	    sectcurr = readbyte(3);
	    sectcomp = readbyte(3);
	    /* is packet number checksum correct? */
	    if ((sectcurr + sectcomp) == 0xff){
		/* is packet number correct? */
		if (sectcurr == ((sectnum+1) & 0xff)){  
		    if (XDEBUG)
			fprintf(LOGFP,
				"DEBUG: packet with sector "
				"number %d started\n", sectnum);

		    /* Read, process and calculate checksum for
		     * a buffer of data
		     */
		    readbackup = fileread;
		    if (readbuf(bufsize, 1, tmode, amode,
				recvsectcnt, &checksum, &bufctr) != TIMEOUT){
			
			/* verify checksum or CRC */
			if (CRCMODE) {
			    checksum &= 0xffff;
			    /* get 16-bit CRC */
			    inchecksum = readbyte(3) & 0xff;  
			    inchecksum = (inchecksum<<8) |
				(readbyte(3) & 0xff);
			}
			/* get simple 8-bit checksum */
			else
			    inchecksum = readbyte(3) & 0xff;  
			/* good checksum, hence good packet */
			if (inchecksum == checksum){
			    xmdebug("checksum ok");
			    errors = 0;
			    recvsectcnt += (bufsize == 128) ? 1 : 8;
			    nocancanabort = CANCAN ? FALSE : TRUE;
			    sectnum = sectcurr; 
			    /* open output file if necessary */
			    if (!openflag){
				openflag = TRUE;
				if ((fd = creat(name, CREATMODE)) < 0) {
				    sendbyte(CAN);
				    sendbyte(CAN);
				    sendbyte(CAN);
				    error("Can't create file for receive",
					  TRUE);
				}
				if (!BATCH){
				    logitarg("File Name: %s\n", name);
				    tlogitarg("File Name: %s\n", name);
				}
			    }
			    if (write(fd, (char *) buff, bufctr) != bufctr){
				close(fd);
				unlink(name);
				error("File Write Error", TRUE);
			    }
			    else {
				if (TIPFLAG && recvsectcnt % 32 == 0)
				    tlogitarg("Sector %s received\n",
					      sectdisp(recvsectcnt,bufsize,0));
				if (!YMODEMG){
				    /* flush input */
				    flushin();          
				    /* ACK the received packet */
				    sendbyte(ACK);     
				}
			    }
			}

		    /* Start handling various errors and special conditions */
			
			else{        /* bad checksum */
			    logitarg("Checksum Error on Sector %s:  ",
				     sectdisp(recvsectcnt,bufsize,1));
			    logitarg("sent=%x  ", inchecksum);
			    logitarg("recvd=%x\n", checksum);
			    tlogitarg("Checksum Error on Sector %s:  ",
				      sectdisp(recvsectcnt,bufsize,1));
			    tlogitarg("sent=%x  ", inchecksum);
			    tlogitarg("recvd=%x\n", checksum);
			    fileread = readbackup;
			    errorflag = TRUE;
			    if (YMODEMG)
				fatalerror = TRUE;
			}
		    }
		    else{    /* read timeout */
			logitarg("Timeout while reading sector %s\n",
				 sectdisp(recvsectcnt,bufsize,1));
			tlogitarg("Timeout while reading sector %s\n",
				  sectdisp(recvsectcnt,bufsize,1));
			fileread = readbackup;
			errorflag = TRUE;
			if (YMODEMG)
			    fatalerror = TRUE;
		    }
		}
		/* sector number is wrong OR Ymodem filename */
		else{ 
		    /* Ymodem file-name packet */
		    if (sectcurr == 0 && recvsectcnt == 0){ 
			logit("YMODEM Batch Protocol\n");
			tlogit("YMODEM Batch Protocol\n");

			/* Read and process a file-name packet */

			if (readbuf(bufsize, 1, FALSE, FALSE,
				    recvsectcnt,
				    &checksum, &bufctr) != TIMEOUT) {

			    /* verify checksum or CRC */

			    if (CRCMODE) {
				checksum &= 0xffff;
				/* get 16-bit CRC */
				inchecksum = readbyte(3) & 0xff;  
				inchecksum = (inchecksum<<8) |
				    (readbyte(3) & 0xff);
			    }
			    /* get simple 8-bit checksum */
			    else
				inchecksum = readbyte(3) & 0xff;  
			    /* good checksum, hence good filename */
			    if (inchecksum == checksum) {
				xmdebug("checksum ok");
				strcpy(name, (char *)buff);
				expsect = ((buff[bufsize-1]<<8) |
					   buff[bufsize-2]);
				BATCH = TRUE;
				YMDMBAT = TRUE;
				/* check for no more files */
				if (strlen(name) == 0) {
				    flushin();          /* flush input */
				    sendbyte(ACK);      /* ACK the packet */
				    logit("YMODEM Batch Receive Complete\n");
				    tlogit("YMODEM Batch Receive Complete\n");
				    return (FALSE);
				}
				/* make filename canonical */
				unixify(name);       
				/* read rest of YMODEM header */
				p = (char *)buff + strlen((char *)buff) + 1;
				if (XDEBUG)
				    fprintf(LOGFP,
					    "DEBUG: header info: %s\n", p);
				sscanf(p, "%ld%lo%o%o%d%ld%d", 
				       &filelength, &modtime, &filemode, 
				       &serial, &filesleft, &totalleft,
				       &yfiletype);
				logitarg("YMODEM file name: %s\n", name);
				tlogitarg("YMODEM file name: %s\n", name);
				fileread = 0l;
				if (filelength){
				    CHECKLENGTH = TRUE;
				    logitarg("YMODEM file size: %ld\n",
					     filelength);
				    tlogitarg("YMODEM file size: %ld\n",
					      filelength);
				}
				else if (expsect)
				    logitarg("YMODEM estimated file "
					     "length %d sectors\n", expsect);
				if (modtime){
				    logitarg("YMODEM file date: %s",
					     ctime((time_t *)&modtime));
				}
				if (filemode){
				    logitarg("YMODEM file mode: %o\n",
					     filemode);
				}
				if (filesleft){
				    logitarg("YMODEM %d file(s) left"
					     " to receive ", filesleft);
				    logitarg("containing %ld bytes\n",
					     totalleft);
				    tlogitarg("YMODEM %d file(s) left"
					      "to receive ", filesleft);
				    tlogitarg("containing %ld bytes\n",
					      totalleft);
				}
				if (serial)
				    logitarg("YMODEM sender's serial"
					     " number: %d\n", serial);
				if (yfiletype)
				    logitarg("YMODEM file type %d\n",
					     yfiletype);
				/* open the file for writing */	    
				openflag = TRUE;	
				if ((fd = creat(name, CREATMODE)) < 0){
				    sendbyte(CAN);
				    sendbyte(CAN);
				    sendbyte(CAN);
				    error("Can't create file for receive",
					  TRUE);
				}
				if (!YMODEMG){
				    /* flush the input stream */
				    flushin();		
				    /* ACK the filename packet */
				    sendbyte(ACK);		
				}
				/* do initial handshake to start
				 * file transfer
				 */
				if (YMODEMG)
				    sendbyte(GCHR);
				else if (CRCMODE) {
				    sendbyte(CRCCHR);
				    if (LONGPACK)
					sendbyte(KCHR);
				}
				else
				    sendbyte(NAK);
				/* reset waiting time */
				firstwait = WAITFIRST; 
				sterrors = 0;
			    }
			    else{ /* bad filename checksum */
				logit("checksum error on filename sector\n");
				tlogit("checksum error on filename sector\n");
				errorflag = TRUE;
				if (YMODEMG)
				    fatalerror = TRUE;
			    }
			}
			else{
			    logit("Timeout while reading filename packet\n");
			    tlogit("Timeout while reading filename packet\n");
			    errorflag = TRUE;
			    if (YMODEMG)
				fatalerror = TRUE;
			}
		    }
		    else if (sectcurr == sectnum){   /* duplicate sector? */
			logitarg("Duplicate sector %s flushed\n",
				 sectdisp(recvsectcnt,bufsize,0));
			tlogitarg("Duplicate sector %s flushed\n",
				  sectdisp(recvsectcnt,bufsize,0));
			if (YMODEMG){
			    errorflag = TRUE;
			    fatalerror = TRUE;
			}
			else{
			    reallyflush(); /* REALLY flush input */
			    sendbyte(ACK);
			}
		    }
		    else{                /* no, real phase error */
			logitarg("Phase Error - Expected packet is %s\n",
				 sectdisp(recvsectcnt,bufsize,1));
			tlogitarg("Phase Error - Expected packet is %s\n",
				  sectdisp(recvsectcnt,bufsize,1));
			errorflag = TRUE;
			fatalerror = TRUE;
		    }
		}
	    }
	    else{        /* bad packet number checksum */
		logitarg("Header Sector Number Error on Sector %s\n",
			 sectdisp(recvsectcnt, bufsize,1));
		tlogitarg("Header Sector Number Error on Sector %s\n",
			  sectdisp(recvsectcnt, bufsize,1));
		errorflag = TRUE;
		if (YMODEMG)
		    fatalerror = TRUE;
		else
		    reallyflush();
	    }
	}           /* END reading packet loop */

	if (errorflag && !fatalerror && recvsectcnt != 0){
	    /* Handle errors */
	    errors++;

	    if (errors >= ERRORMAX)     /* over error limit? */
		fatalerror = TRUE;
	    else{                        /* flush input and NAK the packet */
		reallyflush();
		sendbyte(NAK);
	    }
	}

	if (recvsectcnt == 0 && errorflag && !fatalerror && firstchar != EOT){
	    /* handle startup handshake */
	    sterrors++;
	    firstwait = WAITFIRST + sterrors;
	    
	    if (sterrors >= STERRORMAX)
		fatalerror = TRUE;
	    
	    else if (CRCMODE && MDM7BAT && !BATCH)
		sendbyte(NAK);
		
	    else if (CRCMODE && sterrors == CRCSWMAX && !YMDMBAT){
		CRCMODE = FALSE;
		logit("Sender not accepting CRC request, "
		      "changing to checksum\n");
		tlogit("Sender not accepting CRC request, "
		       "changing to checksum\n");
		sendbyte(NAK);
	    }
	    else if (!CRCMODE && sterrors == CRCSWMAX && !YMDMBAT){
		CRCMODE = TRUE;
		logit("Sender not accepting checksum request,"
		      " changing to CRC\n");
		tlogit("Sender not accepting checksum request,"
		       " changing to CRC\n");
		sendbyte(CRCCHR);
		if (LONGPACK && !MDM7BAT)
		    sendbyte(KCHR);
	    }
	    else if (YMODEMG)
		sendbyte(GCHR);

	    else if (CRCMODE){
		sendbyte(CRCCHR);
		if (LONGPACK && !MDM7BAT)
		    sendbyte(KCHR);
	    }
	    else
		sendbyte(NAK);
	}
    } while ((firstchar != EOT) && !fatalerror);   /* end of MAIN Do-While */

    /* normal exit? */
    if ((firstchar == EOT) && !fatalerror) {
	if (openflag){       /* close the file */
	    close(fd);
	    /* Callback to system layer. */
	    sysSynchFs();
	}
	sendbyte(ACK);      /* ACK the EOT */
	logit("Receive Complete\n");
	tlogit("Receive Complete\n");
	if (LOGFLAG)
	    prtime (recvsectcnt, time((time_t *) 0) - start, LOGFP);
	if (TIPFLAG)
	    prtime (recvsectcnt, time((time_t *) 0) - start, stderr);

	if (openflag && modtime){   /* set file modification time */
	    /* NO vxWorks capability to easily set file mod time now */
	    /* timep[0] = time((time_t *) 0); */
	    /* timep[1] = modtime; */
	    /* utime(name, timep); */
	}	

	if (BATCH)          /* send appropriate return code */
	    return(TRUE);
	else
	    return(FALSE);
    }
    else{                /* no, error exit */
	if (openflag){
	    sendbyte(CAN);
	    sendbyte(CAN);
	    sendbyte(CAN);
	    sendbyte(CAN);
	    sendbyte(CAN);
	    close(fd);
	    unlink(name);
	    reallyflush();	/* wait for line to settle */
	    error("ABORTED -- Too Many Errors -- Deleting File", TRUE);
	}	
	else if (recvsectcnt != 0){
	    sendbyte(CAN);
	    sendbyte(CAN);
	    sendbyte(CAN);
	    sendbyte(CAN);
	    sendbyte(CAN);
	    reallyflush();  /* wait for line to settle */
	    error("ABORTED -- Too Many Errors", TRUE);
	}
	else{
	    reallyflush();  /* wait for line to settle */
	    error("ABORTED -- Remote system is not responding", TRUE);
	}
    }
    return(FALSE);
}
