/*
 *
 * Copyright (C) 1992-2006 by SNMP Research, Incorporated.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by SNMP Research, Incorporated.
 *
 * Restricted Rights Legend:
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions as set forth in subparagraph (c)(1)(ii) of the Rights
 *  in Technical Data and Computer Software clause at DFARS 252.227-7013;
 *  subparagraphs (c)(4) and (d) of the Commercial Computer
 *  Software-Restricted Rights Clause, FAR 52.227-19; and in similar
 *  clauses in the NASA FAR Supplement and other corresponding
 *  governmental regulations.
 */

/*
 *                PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 *
 */

#include "sr_conf.h"

#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <ctype.h>

#include <unistd.h>

#include "sr_proto.h"

int main
    SR_PROTOTYPE((int argc,
                  char **argv));

/* global variables */
char linebuf[256];
char linebuf2[256];
char dummy1[128], dummy2[128], dummy3[128], dummy4[128];


int
main(argc, argv)
int argc;
char **argv;
{
    char *fin = NULL, *fout = NULL, *tmp1 = NULL, *tmp2 = NULL;
    int skipcnt, pagenum, rfcnum;
    FILE *fpin = stdin, *fpout = stdout;

    /* check the command line */
    if(argc > 3) {
	fprintf(stderr,
		"Usage: %s [input filename | -] [output filename | -]\n",
		argv[0]);
	exit(-1);
    }
    else if (argc == 3) {
	fin = argv[1];
	fout = argv[2];
    }
    else if (argc == 2) {
	fin = argv[1];
    }

    if (fin != NULL && strcmp(fin, "-")) {
	/* open the input file */
	if((fpin = fopen(fin, "r")) == NULL) {
	    fprintf(stderr, "%s: Can't open '%s' for reading\n",
		argv[0], fin);
	    exit(-1);
	}
    }

    if (fout != NULL && strcmp(fout, "-")) {
	/* open the output file */
	if((fpout = fopen(fout, "w")) == NULL) {
	    fprintf(stderr, "%s: Can't open '%s' for writing\n",
		argv[0], fout);
	    fclose(fpin);
	    exit(-1);
	}
    }

    /* now parse the file, dropping everything until the start of the 
     * asn.1 specs */
    while(fgets(linebuf, sizeof(linebuf), fpin) != NULL) {

	if(sscanf(linebuf, "%s %s %s %s", dummy1, dummy2, dummy3, dummy4)
	== 4 
	&& strcmp(dummy2, "DEFINITIONS") == 0 
	&& strcmp(dummy3, "::=") == 0 
	&& strcmp(dummy4, "BEGIN") == 0) {
	    /* echo the line and get on with the file */
	    fputs(linebuf, fpout);
	    break;
	}  

        if( fgets(linebuf2, sizeof(linebuf2), fpin) == NULL){
           break;
        } else {

           while( linebuf2[0] == '\n'){
               if( fgets(linebuf2, sizeof(linebuf2), fpin) == NULL){
               break; 
            }
           }
        
        if(sscanf(linebuf2, "%s %s %s %s", dummy1, dummy2, dummy3, dummy4)
        == 4
        && strcmp(dummy2, "DEFINITIONS") == 0
        && strcmp(dummy3, "::=") == 0
        && strcmp(dummy4, "BEGIN") == 0) {
            /* echo the line and get on with the file */
            fputs(linebuf2, fpout);
            break;
        } else if ( strcmp(dummy1, "DEFINITIONS") == 0
                    && strcmp(dummy2, "::=") == 0 
                    && strcmp(dummy3, "BEGIN") == 0){
               /* echo the line and get on with the file */
               fputs(linebuf, fpout);
               fputs(linebuf2, fpout);
               break;
        } 

      }
     }

    /* drop line breaks, and continue until we see the END directive */
    skipcnt = 0;
    while(fgets(linebuf, sizeof(linebuf), fpin) != NULL) {
	/* check for end of file */
	if((tmp1 = strrchr(linebuf, 'E')) != 0
	&& strcmp(tmp1, "END\n") == 0){

            if(tmp1 == linebuf){         /* END is at beginning of line */
	        fputs(linebuf, fpout);
	        break;
            } else {
            tmp2 = tmp1;             /* check char before E */
            tmp2--;

            if((*tmp2 == ' ') || (*tmp2 == '\t')){  
            fputs(linebuf, fpout);
            break;
            }
	   }
         }

	/* check for page breaks */
	if((tmp1 = strrchr(linebuf, '[')) != NULL
	&&sscanf(tmp1, "[%s %d]", dummy1, &pagenum) == 2
	&& strcmp(dummy1, "Page") == 0
	&& pagenum > 0) {
	    skipcnt = 3;
	}
	if((tmp1 = strrchr(linebuf, (char) 12)) != NULL) { /* 12 = CTRL-L */
	    if (skipcnt == 0) {
		skipcnt = 1;
            }
        }
	/* check for RFC line that is separate from a page break */
	if((tmp1 = strrchr(linebuf, 'R')) != NULL
	    && sscanf(linebuf, "%s %d                         %s                        %s %s", dummy1, &rfcnum, dummy2, dummy3, dummy4) == 5
	    && strcmp(dummy1, "RFC") == 0 ) 
	{
	    skipcnt = 1;
	}

	/* print the line if we're not skipping */
	if(skipcnt == 0) {
	    fputs(linebuf, fpout);
	} else {
	    skipcnt--;
	}
    }

    /* close the input file */
    fclose(fpin);

    /* close the output file */
    fclose(fpout);

    /* return a good status */
    return(0);
}

