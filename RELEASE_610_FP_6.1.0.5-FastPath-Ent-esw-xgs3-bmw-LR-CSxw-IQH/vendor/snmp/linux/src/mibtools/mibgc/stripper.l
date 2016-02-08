%{
/*
 *
 * Copyright (C) 1999-2006 by SNMP Research, Incorporated.
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
 *
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

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ======================== INCLUDES ========================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

#include "sr_conf.h"
#include "sr_copyright.h"

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif  /* HAVE_STDIO_H */
#ifdef HAVE_STRING_H
#include <string.h>
#endif /* HAVE_STRING_H */
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif /* HAVE_STDLIB_H */

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#ifdef TIME_WITH_SYS_TIME
#include <time.h>
#endif /* TIME_WITH_SYS_TIME */
#else /* HAVE_SYS_TIME_H */
#ifdef HAVE_TIME_H
#include <time.h>
#endif /* HAVE_TIME_H */
#endif /* HAVE_SYS_TIME_H */

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ======================== DEFINES =========================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

/* #define YYDEBUG */
#define MYBUFSIZE 8192

/* table size = use a prime number for the table size to minimize
 * the number of collisions, and thus maximize speed.
 */
#define TABLE_SIZE 499

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ========================= STRUCTS  ========================= */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

/* structures */
typedef struct tree_tag {
    char * string;
    char * revisionHistory;
    struct tree_tag * next_tree_node;
    struct tree_tag * last_tree_node_in_list;
} tree;

typedef tree * symbol_table; 

symbol_table * revisionTable = NULL;

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ========================= GLOBALS   ======================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

char filenameBuffer[MYBUFSIZE];
char lineBuf[MYBUFSIZE];

char * ptr                  = NULL;
char * outputFileName       = NULL;
char * inputFileName        = NULL;
char * outputDirectoryName  = NULL;
FILE * outputFilePtr        = NULL;
int i                       = 0;
int beginsSeen              = 0;
int endsSeen                = 0;
int suppressOutputFlag      = 0;
int verboseOutputFlag       = 0;
int outputFilenameFlag      = 0;
int outputSet               = 0; /* default is no output */
int inputSet                = 0; /* default is no input */
tree * inputFilenameList    = NULL;
tree * tmpTreePtr           = NULL;

int openOutputFlag = 1;                  /* open the output file */
int helpFlag                        = 0; /* print help info */

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ================== FUNCTION PROTOTYPES ===================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

tree * nodeLookup(char * string, tree * symboltable[] );
tree * malloc_node(tree * node);

void changeComments(char * ptr);
void nodeInsert(tree * node, tree * symboltable[] );
void openOutputFile(char * desiredOutputName);
void write_header(tree *);

int hashpjw(char * s, int num_entries);

symbol_table * malloc_table(symbol_table * new_table);

%}

%{
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ======================== DEFINITIONS ======================= */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
%}

whitespace	[ \t\f\v]
ident		[a-zA-Z][a-zA-Z0-9_\-]*
end_of_line [\r][\n]|[\n]|[\r]         
splitter    {whitespace}|{end_of_line}
start_com   [--]
comment     {start_com}.*{end_of_line}|{start_com}.*{start_com}
splatter    {splitter}|{comment}
draft       "INTERNET-DRAFT"|"Internet Draft"|"Internet-Draft"

%{
/* 
 * all these states dump to the output file
 */
%}

%s dumpState commentState quoteState macroState

%%

%{
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ============== SKIP PAGE SEPARATION CRAP =================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
%}

{end_of_line}+.*[\[].*{end_of_line}+[\f]*{end_of_line}*{draft}.*{end_of_line}+  {
    /* skip page separation crap while in normalEmitState */    

    /* put two substitute newlines back to be parsed */
    unput('\n');
    unput('\n');
    /* put back the last two cr or lf */
    /* yyless(yyleng-2); */
    /*
    if(outputFilePtr) {
        fprintf(outputFilePtr, "\n\n");
    }
    */

}

{end_of_line}+.*{end_of_line}+[\f]{end_of_line}+.*{end_of_line}+  {
    /* skip page separation crap while in normalEmitState*/    

    /* put two substitute newlines back to be parsed */
    unput('\n');
    unput('\n');
    /* put back the last cr or lf */
    /* yyless(yyleng-2); */
    /*
    if(outputFilePtr) {
        fprintf(outputFilePtr, "\n\n");
    }
    */
}

%{
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ==================== COMMENT OUT MACROS ==================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
%}

<macroState>{end_of_line}  {   
    /* normal dump line */
    changeComments(yytext);
    fprintf(outputFilePtr, "-- %s", yytext);
}

<macroState>"END"{whitespace}.*{end_of_line} {   
    changeComments(yytext);
    fprintf(outputFilePtr, "-- %s", yytext);
    BEGIN dumpState;
}

<macroState>"END"{end_of_line} {   
    changeComments(yytext);
    fprintf(outputFilePtr, "-- %s", yytext);
    BEGIN dumpState;
}

<macroState>.  {   
    yymore();
}

%{
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ==================== NORMAL DUMP STATE ===================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
%}

<dumpState>"--" { 
    /* we have to have a separate comment state to handle something like
        -- sfasdfadsf -- END -- asdfasdfasdf --
    */

    fprintf(outputFilePtr, "%s", yytext);
    BEGIN commentState;
} 

<dumpState>"\"" { 
    fprintf(outputFilePtr, "%s", yytext);
    BEGIN quoteState;
} 

<dumpState>{whitespace}*{ident}{splitter}+"MACRO"{splitter}+.*{end_of_line} {

    char * ptr = yytext;  

    while(*ptr) {

        if( (*ptr == '\n') || (*ptr == '\r') ) {
            *ptr     = ' ';
        }
        ptr++;
    }

    fprintf(outputFilePtr, "-- %s\n", yytext);
    if(!suppressOutputFlag) {
        fprintf(stderr, "*** WARNING *** MACRO definition.\n");
    } 

    BEGIN macroState;
}

<dumpState>"END"{whitespace}.*{end_of_line} {   
    endsSeen++;

    /* fprintf(stderr, "ends: %d\n", endsSeen); */

    fprintf(outputFilePtr, "%s", yytext);

    /* is this the end of a macro or the mib ? */
    if(beginsSeen == endsSeen) {

        if(!outputFilenameFlag) {

            fclose(outputFilePtr);
            outputFilePtr = NULL;

            if(!suppressOutputFlag && verboseOutputFlag) {
                fprintf(stderr, "\n*** NOTE *** Closing output file \"%s\".\n", outputFileName);
            }

        /* print file separator */
        } else {

            fprintf(outputFilePtr, "\n-- ");
            for(i = 0 ; i < 88 ; i++) {
                fprintf(outputFilePtr, "=");
            }   
            fprintf(outputFilePtr, "\n");
        }

        beginsSeen = 0;
        endsSeen   = 0;

        BEGIN INITIAL;

    }

}

<dumpState>"END"{end_of_line} {   
    endsSeen++;

    /* fprintf(stderr, "ends: %d\n", endsSeen); */

    fprintf(outputFilePtr, "%s", yytext);

    /* is this the end of a macro or the mib ? */
    if(beginsSeen == endsSeen) {

        if(!outputFilenameFlag) {

            fclose(outputFilePtr);
            outputFilePtr = NULL;

            if(!suppressOutputFlag && verboseOutputFlag) {
                fprintf(stderr, "\n*** NOTE *** Closing output file \"%s\".\n", outputFileName);
            }

        /* print file separator */
        } else {

            fprintf(outputFilePtr, "\n-- ");
            for(i = 0 ; i < 88 ; i++) {
                fprintf(outputFilePtr, "=");
            }   
            fprintf(outputFilePtr, "\n");
        }

        beginsSeen = 0;
        endsSeen   = 0;

        BEGIN INITIAL;

    }

}

<dumpState>{end_of_line}{end_of_line}+ {   
    /* don't allow more that two newlines in a row */
    fprintf(outputFilePtr, "\n\n");
}

<dumpState>{end_of_line}  {   
    fprintf(outputFilePtr, "\n");
}

<dumpState>.  {   
    /* normal dump character */
    fprintf(outputFilePtr, "%s", yytext);
}

%{
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ================ IGNORE "END" INSIDE QUOTE  ================ */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
%}

<quoteState>"\"" { 
    fprintf(outputFilePtr, "%s", yytext);
    BEGIN dumpState;
} 

<quoteState>{end_of_line}{end_of_line}+ {
    /* don't allow more that two newlines in a row */
    fprintf(outputFilePtr, "\n\n");
} 

<quoteState>{end_of_line} {
    fprintf(outputFilePtr, "\n");
} 

<quoteState>. {
    fprintf(outputFilePtr, "%s", yytext);
} 

%{
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ============== IGNORE "END" INSIDE COMMENTS ================ */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
%}

<commentState>"--" { 
    fprintf(outputFilePtr, "%s", yytext);
    BEGIN dumpState;
} 

<commentState>[\r] {
    /* put back the cr */
    yyless(yyleng-1);
    BEGIN dumpState;
} 

<commentState>[\n] {
    /* put back the newline */
    yyless(yyleng-1); 
    BEGIN dumpState;
} 

<commentState>[\r][\n] {
    /* put back the cr, newline */
    yyless(yyleng-2); 
    BEGIN dumpState;
} 

<commentState>. {
    fprintf(outputFilePtr, "%s", yytext);
} 

%{
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ============== FIND BEGINNING OF DEFINITION ================ */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
%}

<INITIAL>{ident}{splatter}+"DEFINITIONS"{splatter}+"::="{splatter}+"BEGIN"{splatter} {
    /* found start of MIB */
    tree * tmpPtr = NULL;
    tree * tree_node = NULL;

    beginsSeen++;

    /* if no outputFileName specified, get it from the input file */
    /* MULTIPLE OUTPUT FILES POSSIBLE */
    if(!outputFilenameFlag) {

        /* skip over any whitespace at beginning of name */
        ptr = yytext;
        while( (*ptr == ' ') || (*ptr == '\t') || (*ptr == '\n') || (*ptr == '\r') ) {
            ptr++;
        }        

        memset(filenameBuffer, 0, MYBUFSIZE);
        strncpy(filenameBuffer, ptr, MYBUFSIZE);

        /* find end of name */
        ptr = filenameBuffer;
        while( (*ptr != ' ') && (*ptr != '\t') && (*ptr != '\n') && (*ptr != '\r') ) {
            ptr++;
        }        

        /* attach file suffix */
        *ptr++ = '.';
        *ptr++ = 'm';
        *ptr++ = 'y';
        *ptr++ = '\0';

        if(outputFileName) free(outputFileName);
        outputFileName = strdup(filenameBuffer);

        if(outputDirectoryName) {
            memset(filenameBuffer, 0, MYBUFSIZE);
            strncpy(filenameBuffer, outputDirectoryName, MYBUFSIZE/2);
            strcat(filenameBuffer, "/");
            strcat(filenameBuffer, outputFileName);

            if(outputFileName) free(outputFileName);
            outputFileName = strdup(filenameBuffer);
        } 

        tmpPtr = nodeLookup(outputFileName, revisionTable);
        if(!tmpPtr) {

            tree_node = malloc_node(tree_node);
            tree_node->string = strdup(outputFileName);

            memset(filenameBuffer, 0, MYBUFSIZE);
            strcpy(filenameBuffer, "-- ");
            strcat(filenameBuffer, inputFileName);
            tree_node->revisionHistory = strdup(filenameBuffer);

            nodeInsert(tree_node, revisionTable);

            tmpPtr = tree_node;

        } else {
            memset(filenameBuffer, 0, MYBUFSIZE);
            strcpy(filenameBuffer, tmpPtr->revisionHistory);
            free(tmpPtr->revisionHistory);
            strcat(filenameBuffer, "\n-- ");
            strcat(filenameBuffer, inputFileName);

            tmpPtr->revisionHistory = strdup(filenameBuffer);

        }

        openOutputFile(outputFileName);

    /* SINGLE OUTPUT FILE ONLY */
    } else {

        if(openOutputFlag) {

            if(outputDirectoryName) {
                memset(filenameBuffer, 0, MYBUFSIZE);
                strncpy(filenameBuffer, outputDirectoryName, MYBUFSIZE/2);
                strcat(filenameBuffer, "/");
                strcat(filenameBuffer, outputFileName);

                if(outputFileName) free(outputFileName);
                outputFileName = strdup(filenameBuffer);
            } 

            openOutputFile(outputFileName);

            /* single output is only produced once */
            openOutputFlag = 0;

        }

    }

    write_header(tmpPtr);

    /* start dumping to file */
    fprintf(outputFilePtr,"%s", yytext);

    BEGIN dumpState;
}

%{
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* =================== DEAL WITH GARBAGE ====================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
%}

<<EOF>> {  
    /* resync states */
    BEGIN INITIAL;
    return -1;
 }

<INITIAL>.             { /* throw everything else away */  }
<INITIAL>{end_of_line} {                                   }

%%

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ====================== USER CODE =========================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

int main(int argc, char * argv[])
{

    revisionTable = malloc_table(revisionTable);

    /* must get all the options and input filenames before we do anything */
    if(argc > 1) {

        for(i = 1 ; i < argc ; i++) {

            /* check for output filename */
            if(argv[i][0] == '-') {

                /* check for output filename option*/
                if( (argv[i][1] =='o' || argv[i][1] =='O' ) && (argc > (i+1))  ) {

                    outputFileName = strdup(argv[i+1]);
                    outputFilenameFlag = 1;
                    i++;

                } else if( (argv[i][1] =='f'  || argv[i][1] =='F') && (argc > (i+1))  ) {

                    outputDirectoryName = strdup(argv[i+1]);
                    i++;

                /* check for suppress output option */
                } else if( (argv[i][1] =='s' ) || ( argv[i][1] =='S' )) {

                    suppressOutputFlag = 1;

                /* check for verbose output option */
                } else if( (argv[i][1] =='v' ) || ( argv[i][1] =='V' )) {

                    verboseOutputFlag = 1;

                /* check for help flag */
                } else if( (argv[i][1] =='h') || (argv[i][1] =='H')) {

                    helpFlag = 1;

                /* check for help flag */
                } else if( (argv[i][1] =='-')  && ((argv[i][2] =='h') || (argv[i][2] =='H') ) ) {

                    helpFlag = 1;

                } else {

                    fprintf(stderr, "Ignoring meaningless option: %s\n", argv[i]);

                }

            /* check for input filename */
            } else {
                

                    tree * tree_node = NULL;
                    tree_node = malloc_node(tree_node);
                    tree_node->string = strdup(argv[i]);

                    inputFileName = strdup(argv[i]);

                    /* insert node at end of list */
                    if(inputFilenameList == NULL) {
                            inputFilenameList = tree_node;
                            inputFilenameList->last_tree_node_in_list = tree_node;
                    } else {
                            inputFilenameList->last_tree_node_in_list->next_tree_node = tree_node;
                            inputFilenameList->last_tree_node_in_list = tree_node;
                    }


            }

        }

    }

    /* print usage if no arguments */
    if((argc == 1) || helpFlag ) {

        fprintf(stderr, "\nmibgp strips MIBs from rfcs or cleans up MIBs.\n\n");

        fprintf(stderr, "USAGE: mibgp [-f|F outputdirectory] [-o|O outputfilename] [-s|S]\n");
	fprintf(stderr,  "           [-v|V] inputFileNames\n");

        fprintf(stderr, "   FLAGS (use either upper or lower case):\n");
        fprintf(stderr, "   -f -F   files    - directory to dump output files\n");
        fprintf(stderr, "   -h -H   help     - show help information (also --h or --H\n");
        fprintf(stderr, "   -o -O   output   - produce specified output file\n");
        fprintf(stderr, "                      if NOT used, outputfilename\n");
        fprintf(stderr, "                      becomes base name of MIB plus \".my\".\n");
        fprintf(stderr, "   -s -S   suppress - suppress warning output\n");
        fprintf(stderr, "   -v -V   verbose  - prints additional information\n\n");

    }

    /* process each of the input files in turn */
    if((argc > 1) && !helpFlag) {

        tmpTreePtr = inputFilenameList;
        while(tmpTreePtr) {

            inputFileName = tmpTreePtr->string;

            if(  !(yyin = fopen(inputFileName, "r"))  ) {
                fprintf(stderr, "\n*** ERROR *** Could not open input file \"%s\".\n", inputFileName);
                _exit(-1);
            } else {
                if(!suppressOutputFlag && verboseOutputFlag) {
                    printf("\n*** NOTE *** Opening input file \"%s\".\n", inputFileName);
                }
            } 

            beginsSeen = 0;
            endsSeen   = 0;

            yylex();
            fclose(yyin);

            if(!suppressOutputFlag && verboseOutputFlag) {
                fprintf(stderr, "\n*** NOTE *** Closing input file \"%s\".\n", inputFileName);
            }

            tmpTreePtr = tmpTreePtr->next_tree_node;

        }
    }

    return 1;
}

/* ============================================================ */

int yywrap() {

    return 1;
}

/* ============================================================ */

/* mallocs space, checks return, clears */
tree * 
malloc_node(tree * node)
{
    node = (tree *)malloc(sizeof(tree));
    if(node) {
        memset((void *)node, 0, sizeof(tree));
    } else {
        fprintf(stderr, "*** ERROR *** Compiler abort - no memory for new tree node!\n");
        _exit(1);
    }

    return(node);
}

/* ============================================================ */

void 
write_header(tree * tmpPtr)
{
    time_t myclock;

    /* get current time */
    myclock = time(0);

    fprintf(outputFilePtr, "\n-- automatically generated by SNMP Research mibgp from %s\n", inputFileName);
    fprintf(outputFilePtr, "-- ");
    fprintf(outputFilePtr, "%s", (char *)ctime(&myclock));
    fprintf(outputFilePtr, "\n");

    if(tmpPtr && tmpPtr->revisionHistory) {
        fprintf(outputFilePtr, "-- revision history\n");
        fprintf(outputFilePtr, "%s\n", tmpPtr->revisionHistory);
    }

    fprintf(outputFilePtr, "\n");

}

/* ============================================================ */

/* reuses filenameBuffer,
   clears old outputFileName, sets new
   sets outputFilePtr
*/

void
openOutputFile(char * desiredOutputName)
{

    FILE * tmpFilePtr;
    int fileCounter = 0, rename = 0;


    memset(filenameBuffer, 0, MYBUFSIZE);
    sprintf(filenameBuffer, "%s", desiredOutputName);

    /* does the output file already exist? */
	while( (tmpFilePtr = fopen(filenameBuffer, "r")) != NULL) {

        if(!suppressOutputFlag) {
            fprintf(stderr, "\n*** WARNING *** Output file \"%s\" already exists.\n", filenameBuffer);
        }

        memset(filenameBuffer, 0, MYBUFSIZE);
        sprintf(filenameBuffer, "%s_%03d", desiredOutputName, fileCounter++);

        fclose(tmpFilePtr);
        rename = 1;

	}

    if(rename) {

        /* rename .my file to next free _??? counter */

        memset(lineBuf, 0, MYBUFSIZE );
#ifndef SR_WINDOWS
        strcpy(lineBuf, "mv ");
#else
        strcpy(lineBuf, "rename ");
#endif /* SR_WINDOWS */
        strcat(lineBuf, desiredOutputName);
        strcat(lineBuf, " ");
        strcat(lineBuf, filenameBuffer);

        system(lineBuf);

    }

    /* OPEN OUTPUT FILE */
    if((outputFilePtr = fopen(outputFileName, "w")) == NULL) {
        fprintf(stderr, "\n*** ERROR *** Could not open output file \"%s\".\n", outputFileName);
        _exit(-1);
    }

    if(!suppressOutputFlag && verboseOutputFlag) {
        fprintf(stderr, "\n*** NOTE *** Opening output file \"%s\".\n", outputFileName);
    }

}

/* ============================================================ */

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* =================== HASH TABLE ============================= */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

/* INSERT: insert symbol into symbol table */
void 
nodeInsert(tree * node, tree * symboltable[] )
{
    register int location;

    /* safety catch */
    if(!node || !symboltable) return;

    /* find location to insert into symbol table */
    location  = hashpjw(node->string, TABLE_SIZE);

    /* insert at head of linked list chained at that location */
    node->next_tree_node = symboltable[location];
    symboltable[location] = node;

}

/* ============================================================ */

/* LOOKUP: lookup string name in symbol table */
/* will always find latest first */

tree * 
nodeLookup(char * string, tree * symboltable[] )
{
    register int location, compare = 1;
    tree * entry_ptr = NULL;

    /* safety catch */
    if(!string || !symboltable) return NULL;

    location = hashpjw(string, TABLE_SIZE);
    entry_ptr = symboltable[location];

    /* traverse list to find */
    while( entry_ptr &&  entry_ptr->string && (compare = strcmp(entry_ptr->string,string)) )
        entry_ptr = entry_ptr->next_tree_node;

    /* if not found return NULL */
    if(compare) {
        return(NULL);
    /* otherwise, return location */
    } else {
        return(entry_ptr);
    }  
}

/* ============================================================ */

/*
 * HASHPJW FUNCTION
 *   Adds the characters in the key "s" then mods
 *   with the number of entries in 
 *   "num_entries" to produce a number that is used
 *   to index into a hash table
 *
 *   taken from page 436 of "Compilers, Principles, Techniques, and Tools",
 *   by Aho, Sethi, Ullman
 */
int 
hashpjw(char * s, int num_entries)
{
    register char * p;
    register unsigned h = 0;
    register unsigned g;

    for(p = s ; *p != '\0' ; p++) {

        h = (h << 4) + (*p);

        if((g = h&0xf0000000)) {

            h = h ^ (g >> 24);
            h = h ^ g;

        }

    }

  return h % num_entries;

}

/* ============================================================ */

symbol_table * 
malloc_table(symbol_table * new_table)
{
    new_table = (symbol_table *)malloc(TABLE_SIZE*sizeof(tree *));

    if(!new_table) {
        fprintf(stderr, "*** ERROR *** Compiler abort - no memory for new symbol table!\n");
        exit(1);
    }

    /* clear table */
    memset((void *)new_table, 0, TABLE_SIZE*sizeof(tree *));

    return(new_table);
    
}

/* ============================================================ */

void 
changeComments(char * ptr)
{
    while(*ptr) {

        if((*ptr == '-') && (*(ptr+1)) && (*(ptr+1) == '-')  ) {
            *ptr     = '/';
            *(ptr+1) = '/';
        }
        ptr++;
    }
    
}

/* ============================================================ */

