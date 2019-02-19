/*
 * $Id: send.c,v 1.1 2011/04/18 17:11:09 mruas Exp $
 */
/**  send a file  **/

/*
 * Operation of this routine depends on on MDM7BAT and YMDMBAT flags.
 *
 * If "name" is NULL; close out the BATCH send.
 */

#include "xmodem.h"

/* External and forward references */

extern void sendbyte(char);
extern void error(char *, int);
extern int projtime(long, FILE *);
extern int readbyte(int);
extern void flushin(void);
extern int send_name(char *);
extern int getbyte(int, char *);
extern int prtime(long, time_t, FILE *);

extern void writebuf(char *, int);

void
sfile(char *name)
{

    char *sectdisp();
    char *strcpy();
    char *unix_cpm();
    char *cpmify();
    long countnl();

    /* CRC-16 constant values, see getput.c */    
    extern unsigned short crctab[1<<B];	

    register int bufctr, 		/* array index for data buffer */
	sectnum;			/* packet number for packet header */

    register unsigned short checksum; 	/* checksum/crc */

    char blockbuf[BBUFSIZ+6];	/* holds packet as it is constructed */
    
    struct stat filestatbuf;	/* file status info */
    
    int fd = -1,	/* file descriptor for file being transmitted */
	attempts,	/* number of attempts made to transmit a packet */
	nlflag, 	/* flag that we have to send a LF in next packet */
	sendfin,       	/* flag that we are sending the last packet */
	closeout,	/* flag that we are closing out batch send */
	startup,	/* flag that we are starting batch send */
	tmode,		/* TRUE for text mode */
	amode,		/* TRUE for apple mode */
	filepack,	/* TRUE when sending first packet */
	buf1024,	/* TRUE when sending 1K packets */
	bbufcnt,	/* array index for packet */
	firstchar,	/* first character in protocol transaction */
	bufsize,	/* packet size (128 or 1024) */
	sendresp;  	/* response char to sent block received from remote*/
	long sentsect;	/* count of 128 byte sectors actually sent */
	long expsect=0;	/* count of 128 byte sectors expected to be sent */
	time_t start;	/* starting time of transfer */
	char c;
	nbchr = 0;      /* clear buffered read char count */

	CRCMODE = FALSE;  /* Receiver determines use of crc or checksum */
	YMODEMG = FALSE;  /* Receiver determines use YMODEM-G */
	buf1024 = LONGPACK; /* set packet size flag to command line switch */

	/* indicate state of batch transfer */
	closeout = FALSE; startup = TRUE; filepack = FALSE;	

	tmode = (XMITTYPE == 't') ? TRUE : FALSE;	/* set text mode */
	amode = (XMITTYPE == 'a') ? TRUE : FALSE;	/* set apple mode */

	/* Check on NULL file name */
	if (strcmp(name,"") == 0){
	    if (BATCH)
		closeout = TRUE;
	    else{
		sendbyte(CAN);
		sendbyte(CAN);
		sendbyte(CAN);
		error("NULL file name in send", TRUE);
	    }
	}

	if (!closeout){		/* Are we closing down batch? */
	                        /* no; let's send a file */
	    logit("----\nXMODEM Send Function\n");
	    tlogit("----\nXMODEM Send Function\n");

	    if ((fd = open(name, 0,0)) < 0){  
		sendbyte(CAN);
		sendbyte(CAN);
		sendbyte(CAN);
		error("Can't open file for send", TRUE);
	    }
	
	    stat(name, &filestatbuf);  /* get file status bytes */
	    if (tmode)		   /* count up NLs */
		filestatbuf.st_size += countnl(fd);
	    expsect = (filestatbuf.st_size/128) + 1;
	
	    if (LOGFLAG){   
		fprintf(LOGFP, "File Name: %s\n", name);
		fprintf(LOGFP,"File Size %ldK, %ld Records, %ld Bytes\n",
			(filestatbuf.st_size/1024)+1,
			expsect, filestatbuf.st_size);
		projtime(expsect, LOGFP);
	    }
	    if (TIPFLAG){
		fprintf(stderr, "File Name: %s\n", name);
		fprintf(stderr,"File Size %ldK, %ld Records, %ld Bytes\n",
			(filestatbuf.st_size/1024)+1,
			expsect, filestatbuf.st_size);
		projtime(expsect, stderr);
	    }
	}
	else{
	    logit("----\nXMODEM Send Function\n");
	    logit("Closing down Batch Transmission\n");
	    tlogit("Closing down Batch Transmission\n");
	}

	bufsize = buf1024 ? 1024 : 128;		/* set sector size */

	if (buf1024 && !closeout){
	    logit("1K packet mode chosen on command line\n");
	    tlogit("1K packet mode chosen on command line\n");
	}

        sendfin = nlflag = FALSE;
  	attempts = 0;

	/* wait for and read startup character */
restart:
	do{
	    while (((firstchar=readbyte(1)) != NAK) &&
		    (firstchar != CRCCHR) &&
		    (firstchar != GCHR) &&
		   (firstchar != CAN))
		if (++attempts > NAKMAX){
		    if (MDM7BAT && startup){
			sendbyte(ACK);
			sendbyte(EOT);
		    }
		    error("Remote System Not Responding", TRUE);
		}
		if ((firstchar & 0x7f) == CAN)
		    if (readbyte(3) == CAN)
			error("Send Canceled by CAN-CAN",TRUE);

		if (firstchar == GCHR){
		    CRCMODE = TRUE;
		    YMODEMG = TRUE;
		    CANCAN = TRUE;
		    if (!closeout){
			logit("Receiver invoked YMODEM-G and CRC modes\n");
			tlogit("Receiver invoked YMODEM-G and CRC modes\n");
		    }
		}
		if (firstchar == CRCCHR){
		    CRCMODE = TRUE;
		    if (!closeout){
			logit("Receiver invoked CRC mode\n");
			tlogit("Receiver invoked CRC mode\n");
		    }
		    if (readbyte(1) == KCHR){
			buf1024 = TRUE;
			logit("Receiver invoked 1K packet mode\n");
			tlogit("Receiver invoked 1K packet mode\n");
		    }
		}
	}
	while (firstchar != NAK && firstchar != CRCCHR && firstchar != GCHR)
	    ; /* SPIN */

	if (MDM7BAT && closeout){	/* close out MODEM7 batch */
	    sendbyte(ACK);
	    sendbyte (EOT);
	    flushin();
	    readbyte(2); 	/* flush junk */
	    return;
	}

	if (MDM7BAT && startup){		/* send MODEM7 file name */
		if (send_name(unix_cpm(name)) == -1){
		    attempts = 0;
		    goto restart;
		}
		startup = FALSE;
		attempts = 0;
		goto restart;
	}

	sectnum = 1;

	if (YMDMBAT){	/* Fudge for YMODEM transfer (to send name packet) */
	    sectnum = 0;
	    bufsize = 128;
	    filepack = TRUE;
	}
	
	attempts = sentsect = 0;
	start = time((time_t *) 0);
	/* outer packet building/sending loop; loop till whole file is sent */
        do{   
	    if (closeout && YMDMBAT && sectnum == 1)	/* close out YMODEM */
		return;
	    /* get set to send YMODEM data packets */
	    if (YMDMBAT && sectnum == 1){
		bufsize = buf1024 ? 1024 : 128;

		do{
		    /* establish handshaking again */
		    while (((firstchar=readbyte(2)) != CRCCHR) &&
			   (firstchar != GCHR) &&
			   (firstchar != NAK) &&
			   (firstchar != CAN))
			if (++attempts > ERRORMAX)
			    error("YMODEM protocol botch, C or G expected",
				  TRUE);
		    if ((firstchar&0x7f) == CAN)
			if (readbyte(3) == CAN)
			    error("Send Canceled by CAN-CAN", TRUE);
		} while ((firstchar != CRCCHR) &&
			 (firstchar != GCHR) &&
			 (firstchar != NAK));
		attempts = 0;
	    }

	    if ((bufsize == 1024) && (attempts > KSWMAX)){
		logit("Reducing packet size to 128"
		      " due to excessive errors\n");
		tlogit("Reducing packet size to 128"
		       " due to excessive errors\n");
		bufsize = 128;
	    }

	    if ((bufsize == 1024) && ((expsect - sentsect) < 8)) {
		logit("Reducing packet size to 128 for tail end of file\n");
		tlogit("Reducing packet size to 128 for tail end of file\n");
		bufsize = 128;
	    }

		if (sectnum > 0){	/* data packet */
		    for (bufctr=0; bufctr < bufsize;){
			if (nlflag){
			    buff[bufctr++] = LF;  /* leftover newline */
			    nlflag = FALSE;
			}
			if (getbyte(fd, &c) == EOF){
			    sendfin = TRUE;  /* this is the last sector */
			    if (!bufctr)  /* if EOF on sector boundary */
				break;  /* avoid sending extra sector */
			    /* pad with Ctrl-Z for CP/M EOF
			     * (even do for binary files)
			     */
			    buff[bufctr++] = CTRLZ;  
			    continue;
			}
	
			if (tmode && c == LF){  /* text mode & Unix newline? */
			    buff[bufctr++] = CR;  /* insert carriage return */
			    if (bufctr < bufsize)
				buff[bufctr++] = LF;  /* insert LF */
			    else
				nlflag = TRUE;  /* insert on next sector */
			}	
			/* Apple mode & Unix newline? */
			else if (amode && c == LF)   
			    buff[bufctr++] = CR; /* substitute CR */
			else
			    buff[bufctr++] = c;  
			/* copy the char without change */
		    }

		    if (!bufctr)  /* if EOF on sector boundary */
			break;  /* avoid sending empty sector */
		}      
		else {	/* YMODEM filename packet */
		    for (bufctr=0; bufctr<1024; bufctr++)  /* zero packet */
			buff[bufctr]=0;
		    if (!closeout){
			strcpy((char *)buff, cpmify(name));
				
			/* put in file name, length, mode, */
			/* dummy SN, files, bytes remaining and file type */
			{
			    register char *p;
			    p = (char *)buff + strlen(buff) + 1;
			    sprintf(p, "%lu %lo %o 0 %d %ld 0",
				    filestatbuf.st_size, 
				    filestatbuf.st_mtime, filestatbuf.st_mode,
				    yfilesleft, ytotleft);
			    if (XDEBUG)
				fprintf(LOGFP, "DEBUG: YMODEM header"
					" information: %s %s\n", buff, p);
			}
			/* need to have long packet? */	
			if (buff[125]){
			    bufsize = 1024;
			    if (XDEBUG)
				fprintf(LOGFP, "DEBUG: YMODEM header"
					" sent in 1024 byte packet\n");
			}
			/* put in KMD kludge information */
			buff[bufsize-2]	= (expsect & 0xff);        
			buff[bufsize-1] = ((expsect >> 8) & 0xff);

			/* update totals */
			ytotleft -= filestatbuf.st_size;
			if (--yfilesleft <= 0)
			    ytotleft = 0;
			if (ytotleft < 0)
			    ytotleft = 0;
		    }
		}
		/* start building block to be sent */
		bbufcnt = 0;		
		/* start of packet char */
		blockbuf[bbufcnt++] = (bufsize == 1024) ? STX : SOH;    
		blockbuf[bbufcnt++] = sectnum;	    /* current sector # */
		blockbuf[bbufcnt++] = ~sectnum;   /* and its complement */

               	checksum = 0;  /* initialize checksum */
               	for (bufctr=0; bufctr < bufsize; bufctr++){
		    blockbuf[bbufcnt++] = buff[bufctr];
		    
		    if (CRCMODE)
			checksum = (checksum<<B) ^ crctab[(checksum>>(W-B)) ^
							 buff[bufctr]];

			else
			    checksum = ((checksum+buff[bufctr]) & 0xff);
		}
		/* put in CRC */
		if (CRCMODE){	
		    checksum &= 0xffff;
		    blockbuf[bbufcnt++] = ((checksum >> 8) & 0xff);
		    blockbuf[bbufcnt++] = (checksum & 0xff);
		}
		else	/* put in checksum */
		    blockbuf[bbufcnt++] = checksum;

            	attempts = 0;
	
            	do{     /* inner packet loop */

		    writebuf(blockbuf, bbufcnt); /* write the block */
		    if (!YMODEMG)
			flushin();      /* purge anything in input queue */

		    if (XDEBUG)
			fprintf (LOGFP, "DEBUG: %d byte Packet %02xh"
				 " (%02xh) sent, checksum %02xh %02xh\n", 
				 bbufcnt, blockbuf[1]&0xff,
				 blockbuf[2]&0xff,
				 blockbuf[bufsize+3]&0xff,
				 blockbuf[bufsize+4]&0xff);

		    attempts++;
		    sendresp = (YMODEMG) ? ACK : readbyte(10);
		    /* get response from remote  (or fake it for YMODEM-G) */
		    
		    if (sendresp != ACK){
			if (sendresp == TIMEOUT){
			    logitarg("Timeout on sector %s\n",
				     sectdisp(sentsect,bufsize,1));
			    tlogitarg("Timeout on sector %s\n",
				      sectdisp(sentsect,bufsize,1));
			}
			if (sendresp == CAN){
			    if (CANCAN){
				if (readbyte(3) == CAN)
				    error("Send Canceled by CAN-CAN",TRUE);
			    }
			    else{
				logitarg("ignored CAN on sector %s\n",
					 sectdisp(sentsect,bufsize,1));
				tlogitarg("ignored CAN on sector %s\n",
					  sectdisp(sentsect,bufsize,1));
			    }
			}
			else if (sendresp == NAK){
			    logitarg("NAK on sector %s\n",
				     sectdisp(sentsect,bufsize,1));
			    tlogitarg("NAK on sector %s\n",
				      sectdisp(sentsect,bufsize,1));
			}
			else{
			    logitarg("Non-ACK on sector %s\n",
				     sectdisp(sentsect,bufsize,1));
			    tlogitarg("Non-ACK on sector %s\n",
				      sectdisp(sentsect,bufsize,1));
			}
		    }
		} while((sendresp != ACK) && (attempts < ERRORMAX));	
		/* close of inner loop */
       		sectnum++;  /* increment to next sector number */
		if (!filepack)
		    sentsect += (bufsize == 128) ? 1 : 8;
		filepack = FALSE;
		if (TIPFLAG && sentsect % 32 == 0)
		    tlogitarg("Sector %s sent\n",
			      sectdisp(sentsect,bufsize,0));
	} while (!sendfin && ( attempts < ERRORMAX));	/* end of outer loop */

	if (attempts >= ERRORMAX){
	    sendbyte(CAN);
	    sendbyte(CAN);
	    sendbyte(CAN);
	    sendbyte(CAN);
	    sendbyte(CAN);
	    error ("Too many errors in transmission", TRUE);
	}

    	sendbyte(EOT);  /* send 1st EOT to close down transfer */
    	attempts = 0;
	/* wait for ACK of EOT */	
    	while ((readbyte(15) != ACK) && (attempts++ < EOTMAX)){ 	
	    if (attempts > 1){
		logitarg("EOT not ACKed, try %d\n", attempts);
		tlogitarg("EOT not ACKed, try %d\n", attempts);
	    }
	    sendbyte(EOT);
	}

    	if (attempts >= RETRYMAX)
	    error("Remote System Not Responding on Completion", TRUE);

    	close(fd);

    	logit("Send Complete\n");
    	tlogit("Send Complete\n");
	if (LOGFLAG)
	    prtime(sentsect, time((time_t *) 0) - start, LOGFP);
	if (TIPFLAG)
	    prtime(sentsect, time((time_t *) 0) - start, stderr);
}
