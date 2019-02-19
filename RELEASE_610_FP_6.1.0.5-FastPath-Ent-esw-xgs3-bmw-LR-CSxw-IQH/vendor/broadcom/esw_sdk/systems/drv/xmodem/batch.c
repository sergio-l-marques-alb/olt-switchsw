/*
 *  Various routines for batch transfer
 */

#include "xmodem.h"

/* External and forward references */

extern void error(char *, int);
extern void xmdebug(char *);
extern void sendbyte(char);
extern int readbyte(int);

/* make sure filename sent or received in YMODEM batch is canonical. */

/* Incoming: Turn Unix '/' into CP/M ':' and translate to all lower case.
 * Remove trailing dot.
 */

void
unixify (char *name)
{
    char *ptr;    
    /* change '/' to ':' and convert to lower case */
    for (ptr=name; *ptr; ++ptr)	{
	if (*ptr == '/')
	    *ptr = ':';
	if (isupper ((int)*ptr))
	    *ptr |= 040;
	}	
    
    /* remove trailing dot if present */
    ptr--;
    if (*ptr == '.')
	*ptr = '\0';
}

/* make sure filename sent or received in YMODEM batch is canonical. */
/* Outgoing: Turn ':' into '/' (for symmetry!) and turn into all lower case.
 * Remove everything before last '/'.  Use "filename" to hold final name.
 */

char *
cpmify (char* name)
{
    char *ptr, *slash;
    char *strcpy();
    
    /* find last '/' and copy rest of name */
    
    slash = name;
    for (ptr=name; *ptr; ++ptr)
	if (*ptr == '/')
	    slash = ptr + 1;
    strcpy (filename, slash);
    
    /* change ':' to '/' and covert to all lower case */
    
    for (ptr=filename; *ptr; ++ptr){
	if (*ptr == ':')
	    *ptr = '/';
	if (isupper ((int)*ptr))
	    *ptr |= 040;
    }	
    return (filename);
}

/* convert a CP/M file name received in a MODEM7 batch transfer
 * into a unix file name mapping '/' into ':', converting to all
 * upper case and adding dot in proper place.  
 * Use "filename" to hold name.
 * Code stolen from D. Thompson's (IRTF) xmodem.c
 */

char
*cpm_unix (char *string)
{
    register int i;
    unsigned char *iptr, temp;
    register char *optr;
    
    if (*string == '\0')
	error("Null file name in MODEM7 batch receive", TRUE);
    
    for (iptr=string; (temp = *iptr) ; ) {
	temp &= 0177;			/* strips bit 7 */
	if (isupper(temp))
	    temp |= 040;		/* set bit 5 for lower case */
	if (temp == '/') 
	    temp=':';		/* map / into : */
	*iptr++ = temp;
    }

    /* put in main part of name */
    iptr=string;
    optr=filename;
    for (i=0; i<8; i++) {
	if (*iptr != ' ')
	    *optr++ = *iptr++;
    }		
    
    /* add dot if necessary */
    if (string[8] != ' ' || string[9] != ' ' || string[10] != ' ')
	*optr++ = '.';
    
    /* put in extension */
    iptr = &string[8];
    for (i=0; i<3; i++) {
	if (*iptr != ' ')
	    *optr++ = *iptr++;
    }	
    
    *optr++ = '\000';
    return (filename);
}

/* Send 11 character CP/M filename for MODEM7 batch transmission
 * Returns -1 for a protocol error; 0 if successful
 * NOTE: we tromp a little on the argument string!
 * code stolen from D. Thompson's (IRTF) xmodem.c
 */


int
send_name(char *name)
{
    register int cksum;
    register char *ptr;
    
    xmdebug("send_name");
    
    /* append cp/m EOF */
    name[NAMSIZ] = CTRLZ;
    name[NAMSIZ+1] = '\000';
    
    /* create checksum */
    ptr = name;
    cksum = 0;
    while (*ptr)
	cksum += *ptr++;
    cksum &= 0x00FF;
    
    /* send filename */
    
    sendbyte(ACK);
    ptr = name;
    sendbyte(*ptr++);
    
    while (*ptr) {
	
	switch (readbyte(15)) {
	    
	case ACK: break;
	    
	case TIMEOUT: {
	    logit("Timeout while sending MODEM7 filename\n");
	    tlogit("Timeout while sending MODEM7 filename\n");
	    sendbyte(BAD_NAME);
	    return (-1);
	}
	
	default: {
	    logit("Error while sending MODEM7 filename\n");
	    tlogit("Error while sending MODEM7 filename\n");
	    sendbyte(BAD_NAME);
	    return (-1);
	}
	}	
	
	sendbyte (*ptr++);
    }
    
    /* Check checksum returned by other side against my value */
    if (readbyte(16) != cksum) {
	logit("Bad checksum while sending MODEM7 filename\n");
	tlogit("Bad checksum while sending MODEM7 filename\n");
	sendbyte(BAD_NAME);
	return (-1);
    }

    sendbyte(ACK);
    return (0);
}	

/* Convert Unix filename to 11 character CP/M file name (8 char name,
 * 3 char extension, dot in between is not included).
 * map ':' into '/'; Use filename to hold name.
 * code stolen from D. Thompson's (IRTF) xmodem.c
 */
char*
unix_cpm(char *string)
{
    register char *iptr, *optr, temp;
    int i;
    
    char *rindex();
    char *strcpy();
    
    /* blank 11 character name */
    (void) strcpy (filename,"           ");
    
    /* strip off any path name */
    if ((iptr = rindex(string,'/')))
	iptr++;
    else
	iptr=string;
    
    /* skip leading '.'s */
    while (*iptr == '.')
	iptr++;
    
    /* copy main part of name */
    optr = filename;
    i = 8;
    while ((i--) && (*iptr) && (*iptr != '.'))
	*optr++ = *iptr++;
    
    /* advance to unix extension, or end of unix name */
    while ((*iptr != '.') && (*iptr))
	iptr++;
    
    /* skip over the  '.' */
    while (*iptr == '.')
	iptr++;
    
    /* copy extension */
    optr = &filename[8];
    i=3;
    while ((i--) && (*iptr) && (*iptr != '.'))
	*optr++ = *iptr++;
    
    filename[NAMSIZ] = '\000';
    
    /* Fuss with name */
    for (iptr=filename; (temp = *iptr) ;) {
	temp &= 0177;			/* strip bit 7 (parity bit) */
	if (islower((int)temp))
	    temp &= ~040;		/* make upper case */
	if (temp == ':')
	    temp ='/';		/* map ':' into '/' */
	*iptr++ = temp;
    }
    
    if (XDEBUG)
	fprintf (LOGFP, "DEBUG: File %s sent as %s\n", string, filename);
    
    return(filename);
}
