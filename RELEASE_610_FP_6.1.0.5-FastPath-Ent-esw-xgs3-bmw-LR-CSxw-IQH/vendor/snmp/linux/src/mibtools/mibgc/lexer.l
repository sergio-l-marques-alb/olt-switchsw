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

/* FUTURE - TAB distances not computed correctly */

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ======================== INCLUDES ========================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

#include "sr_conf.h"

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif  /* HAVE_STDIO_H */
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif /* HAVE_STDLIB_H */
#ifdef HAVE_STRING_H
#include <string.h>
#endif /* HAVE_STRING_H */
#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif /* HAVE_CTYPE_H */

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */
 
#include "bison.h"

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ======================== DEFINES =========================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

#define MYBUFSIZE 8192

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ========================= STRUCTS  ========================= */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

struct table {
    char    * keyword;
    int     token;
};

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ========================== GLOBALS ========================= */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

/* all the globals are actually in the parser */

/* FLAGS */
extern int globalCppCommentFlag;    /* make comments work like C++ comments */
extern int no_code_flag;            /* errors, don't produce output file    */

/* track text positions for error reporting */
extern int line_cntr;
extern int col_cntr;

/* needed for string handling and unterminated string error reporting */
extern int stringBufSize;
extern int stringPos;
extern int string_line_cntr;
extern int string_col_cntr;
extern int stringWarnOn;

/* BUFFERS */
extern char * lineBuf;              /* captures line for error reporting    */
extern char * stringBuf;            /* capturing quoted strings             */
extern char * firstLineBuf;         /* for unterminated string reporting    */

/* using a table is faster than using flex to match each string */
/*    string to match       token to return */
static struct table reserved[95] = {
    { "ACCESS",                 ACCESS,               },
    { "AGENT-CAPABILITIES",     AGENT_CAPABILITIES,   },
    { "ANY",                    ANY,                  },
    { "APPLICATION",            APPLICATION,          },
    { "AUGMENTS",               AUGMENTS,             },
    { "BEGIN",                  BEGIN_,               },    /* BEGIN conflicts with state changing "BEGIN" */
    { "BIT",                    BIT,                  },
    { "BITS",                   BITS,                 },
    { "BY",                     BY,                   },
    { "CHOICE",                 CHOICE,               },
    { "CONTACT-INFO",           CONTACT_INFO,         },    /* yacc doesn't like "-" in names */
    { "CREATION-REQUIRES",      CREATION_REQUIRES,    },
    { "Counter",                Counter,              },
    { "Counter32",              Counter32,            },
    { "Counter64",              Counter64,            },
    { "DEFINED",                DEFINED,              },
    { "DEFINITIONS",            DEFINITIONS,          },
    { "DEFVAL",                 DEFVAL,               },
    { "DESCRIPTION",            DESCRIPTION,          },
    { "DISPLAY-HINT",           DISPLAY_HINT,         },
    { "END",                    END,                  },
    { "ENTERPRISE",             ENTERPRISE,           },
    { "EXPLICIT",               EXPLICIT,             },
    { "EXPORTS",                EXPORTS,              },
    { "FROM",                   FROM,                 },
    { "GROUP",                  GROUP,                },
    { "Gauge",                  Gauge,                },
    { "Gauge32",                Gauge32,              },
    { "IDENTIFIER",             IDENTIFIER,           },
    { "IMPLICIT",               IMPLICIT,             },
    { "IMPLIED",                IMPLIED,              },
    { "IMPORTS",                IMPORTS,              },
    { "INCLUDES",               INCLUDES,             },
    { "INDEX",                  INDEX,                },
    { "INTEGER",                INTEGER,              },
    { "Integer32",              Integer32,            },
    { "IpAddress",              IpAddress,            },
    { "LAST-UPDATED",           LAST_UPDATED,         },
    { "MANDATORY-GROUPS",       MANDATORY_GROUPS,     },
    { "MAX",                    MAX,                  },
    { "MAX-ACCESS",             MAX_ACCESS,           },
    { "MIN",                    MIN,                  },
    { "MIN-ACCESS",             MIN_ACCESS,           },
    { "MODULE",                 MODULE,               },
    { "MODULE-COMPLIANCE",      MODULE_COMPLIANCE,    },
    { "MODULE-CONFORMANCE",     MODULE_CONFORMANCE,   },
    { "MODULE-IDENTITY",        MODULE_IDENTITY,      },
    { "NOTIFICATION-GROUP",     NOTIFICATION_GROUP,   },
    { "NOTIFICATION-TYPE",      NOTIFICATION_TYPE,    },
    { "NOTIFICATIONS",          NOTIFICATIONS,        },
    { "NULL",                   NULL_,                },    /* NULL keyword, don't confuse with the C keyword */
    { "OBJECT",                 OBJECT,               },
    { "OBJECT-GROUP",           OBJECT_GROUP,         },
    { "OBJECT-IDENTITY",        OBJECT_IDENTITY,      },
    { "OBJECT-TYPE",            OBJECT_TYPE,          },
    { "OBJECTS",                OBJECTS,              },
    { "OCTET",                  OCTET,                },
    { "OF",                     OF,                   },
    { "ORGANIZATION",           ORGANIZATION,         },
    { "ObjectSyntax",           ObjectSyntax,         },
    { "Opaque",                 Opaque,               },
    { "PRIVATE",                PRIVATE,              },
    { "PRODUCT-RELEASE",        PRODUCT_RELEASE,      },
    { "REFERENCE",              REFERENCE,            },
    { "REVISION",               REVISION,             },
    { "SEQUENCE",               SEQUENCE,             },
    { "SIZE",                   SIZE,                 },
    { "STATUS",                 STATUS,               },
    { "STRING",                 STRING,               },
    { "STRUCT",                 STRUCT,               },
    { "SUPPORTS",               SUPPORTS,             },
    { "SYNTAX",                 SYNTAX,               },
    { "TEXTUAL-CONVENTION",     TEXTUAL_CONVENTION,   },
    { "TRAP-TYPE",              TRAP_TYPE,            },
    { "TimeTicks",              TimeTicks,            },
    { "UInteger32",             UInteger32,           },
    { "UNION",                  UNION,                },
    { "UNITS",                  UNITS,                },
    { "UNIVERSAL",              UNIVERSAL,            },
    { "Unsigned32",             Unsigned32,           },
    { "VARIABLES",              VARIABLES,            },
    { "VARIATION",              VARIATION,            },
    { "WRITE-SYNTAX",           WRITE_SYNTAX,         },
    { "accessible-for-notify",  accessible_for_notify },
    { "current",                currrent,             },
    { "deprecated",             deprecated,           },
    { "mandatory",              mandatory,            },
    { "not-accessible",         not_accessible,       },
    { "not-implemented",        not_implemented,      },
    { "obsolete",               obsolete,             },
    { "optional",               optional,             },
    { "read-create",            read_create,          },
    { "read-only",              read_only,            },
    { "read-write",             read_write,           },
    { "write-only",             write_only,           }
};

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ================== FUNCTION PROTOTYPES ===================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

extern void print_warning(char * my_msg, char * buf);
extern void print_note(char * my_msg, char * buf);

int binarySearch (char * key, struct table * mytable, int low, int high);

void make_stringBuf_bigger();
void clear_input();

%}

%{
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ======================== DEFINITIONS ======================= */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/*
rfc2578

UppercaseIdentifier - A module name begins with an upper-case
letter and continues with zero or more letters, digits, or
hyphens, except that a hyphen can not be the last character, nor
can there be two consecutive hyphens."

    '&', '-', and '/'  added for garbage mibs

LowercaseIdentifier - An identifier consists of one or more
letters or digits, and its initial character must be a
lower-case letter.  Note that hyphens are not allowed by this
specification (except for use by information modules converted
from SMIv1 which did allow hyphens).

    '&', '-', and '/'  added for garbage mibs

For all descriptors appearing in an information module, the
descriptor shall be unique and mnemonic, and shall not exceed 64
characters in length.  (However, descriptors longer than 32
characters are not recommended.)

 */

%}

UppercaseIdentifier         [A-Z][a-zA-Z0-9]*([\-][a-zA-Z0-9]+)*
ExtendedUppercaseIdentifier [A-Z][\&a-zA-Z0-9\-\_/]*
LowercaseIdentifier         [a-z][a-zA-Z0-9]*([\-][a-zA-Z0-9]+)*
ExtendedLowercaseIdentifier [a-z][\&a-zA-Z0-9\-\_/]*
NumberIdentifier            [0-9][\&a-zA-Z0-9\-\_/]*

oid                         [0-9]+([.][0-9]+)*

hex_digit                   [0-9a-fA-F]
end_of_line                 [\r][\n]|[\n]|[\r]         
whitespace                  [ \t\f\v]

%{
/* 
 * The default start state is "INITIAL".  INITIAL, however, is
 * only used to process the first line into the buffer, since the
 * first line doesn't start with a NEWLINE.  "start" is the 
 * normal processing state.
 *
 * The comment state traps and discards comments.
 * The -c option forces comments to be treated like C++ comments
 *
 * The quote state is used to capture quoted strings
 * and information about possible unterminated strings.
 *
 * The action_garbage state traps and discards ASN.1 action sequences.
 *
 * The macro_garbage state traps and discards ASN.1 macro sequences.
 *
 */
%}

%s start comment quote action_garbage macro_garbage

%%

%{
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ==================== GARBAGE COLLECTION ==================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
%}

.*{whitespace}+"MACRO"  { 
    BEGIN macro_garbage;
}

<macro_garbage>"END" { BEGIN start; }

<macro_garbage>. { /* toss */ }

<macro_garbage>{end_of_line} { 
    /* this is only thing we care about here, counting lines */
    line_cntr++; 
}

%{
/* ============================================================ */
%}

"%{" {
    BEGIN action_garbage;
}

<action_garbage>"%}" { BEGIN start; }

<action_garbage>. { /* toss */ }

<action_garbage>{end_of_line}    { 
    /* this is only thing we care about here, counting lines */
    line_cntr++; 
}

%{
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ==================== FILL BUFFER PHASE  ==================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
%}

<INITIAL>[\r][\n]   {   /* eat up end of line */
    line_cntr++;
    col_cntr = 0;
    yyless(2); 
}

<INITIAL>[\n]       {   /* eat up end of line */
    line_cntr++;
    col_cntr = 0;
    yyless(1); 
}

<INITIAL>[\r]       {   /* eat up end of line */
    line_cntr++;
    col_cntr = 0;
    yyless(1); 
}

<INITIAL>.*         { /* copy the first line into the line buffer
                     then put back EVERYTHING for scanning */
    memset(lineBuf, 0, MYBUFSIZE );
    strncpy(lineBuf, yytext, MYBUFSIZE);
    yyless(0);
    BEGIN start;
}

<start>[\r][\n].*   { /* copy the current line into the line buffer
                         then put back EVERYTHING EXCEPT THE CR and NEWLINE 
                         for scanning for tokens */
    memset(lineBuf, 0, MYBUFSIZE );
    strncpy(lineBuf, yytext+2, MYBUFSIZE);
    line_cntr++;
    col_cntr = 0;
    yyless(2);
}

<start>[\n].*   { /* copy the current line into the line buffer
                     then put back EVERYTHING EXCEPT THE NEWLINE 
                     for scanning for tokens */
    memset(lineBuf, 0, MYBUFSIZE );
    /* skip newline */
    strncpy(lineBuf, yytext+1, MYBUFSIZE);
    line_cntr++;
    col_cntr = 0;
    yyless(1); 
}

<start>[\r].*   { /* copy the current line into the line buffer
                     then put back EVERYTHING EXCEPT THE CARRIAGE RETURN
                     for scanning for tokens */
    memset(lineBuf, 0, MYBUFSIZE );
    strncpy(lineBuf, yytext+1, MYBUFSIZE);
    line_cntr++;
    col_cntr = 0;
    yyless(1); 
}

%{
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ======================= WHITESPACE ========================= */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
%}

<start>{whitespace} { /* toss excess whitespace so it doesn't
                         appear on the output console */
    col_cntr+=yyleng; 
}

%{
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ===================== PUNCTUATION ========================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
%}

<start>"::=" {
    col_cntr+=yyleng;
    return CCE; 
}

<start>"|"   {
    col_cntr+=yyleng;
    return BAR; 
}

<start>"["   {
    col_cntr+=yyleng;
    return LBRACKET; 
}

<start>"]"   {
    col_cntr+=yyleng;
    return RBRACKET; 
}

<start>"."   {
    col_cntr+=yyleng;
    return DOT; 
}

<start>".."  {
    col_cntr+=yyleng;
    return DOT_DOT; 
}

<start>";"   {
    col_cntr+=yyleng;
    return SEMICOLON; 
}

<start>","   {
    col_cntr+=yyleng;
    return COMMA; 
}

<start>"{"   {
    col_cntr+=yyleng;
    return LBRACE; 
}

<start>"}"   {
    col_cntr+=yyleng;
    return RBRACE; 
}

<start>"("   {
    col_cntr+=yyleng;
    return LPAREN; 
}

<start>")"   {
    col_cntr+=yyleng;
    return RPAREN; 
}


%{
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ================ POSITIVE DECIMAL NUMBERS ================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
%}

<start>[0-9]+ {
    /* point to string so bison can copy it */
    yylval.char_ptr = yytext;
    col_cntr+=yyleng;
    return POS_NUMBER; 
}

%{
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ================ NEGATIVE DECIMAL NUMBERS ================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
%}

<start>-[0-9]+ {
    /* point to string after minus sign so yacc can copy it */
    yylval.char_ptr = yytext+1;
    col_cntr+=yyleng;
    return NEG_NUMBER; 
}

%{
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ======================== BINARY NUMBERS ==================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
%}

<start>'[01]*'[Bb] {   
    /*

    A binary string consists of a number (possibly zero) of zeros and
    ones preceded by a single (') and followed by either the pair ('B) or
    ('b), where the number is a multiple of eight.
                                ~~~~~~~~~~~~~~~~~
    */

    col_cntr+=yyleng;

    /* if mod by 8 doesn't yield 0 then it isn't divisible by 8 */ 
    if( (strlen(yytext) - 3) % 8 ) {
        print_warning("*** ERROR ***  Binary string is not a multiple of 8 bits.", lineBuf);
    }

    /* point to string so yacc can copy it */
    yylval.char_ptr = yytext;
    /*yylval.char_ptr = yytext+1; */
    return BINARY_NUMBER; 
}

%{
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ===================== HEXADECIMAL NUMBERS ================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
%}

<start>'{hex_digit}*'[Hh] {
    /*

    A hexadecimal string consists of an even number (possibly zero) of
                                        ~~~~
    hexadecimal digits, preceded by a single (') and followed by either
    the pair ('H) or ('h).  Digits specified via letters can be in upper
    or lower case.

    Note that ASN.1 comments can not be enclosed inside any of these
    types of strings.

    */

    col_cntr+=yyleng;

    /* if mod by 2 doesn't yield 0 then it isn't divisible by 2 */ 
    if( (strlen(yytext) - 3) % 2 ) {

        /* did some bozo head add an extra leading 0 */
        if(yytext[1] == '0') {

            if( (strlen(yytext+1) - 3) % 2 ) {

                print_warning("*** ERROR ***  Hexadecimal string is not an even number of hex digits.", lineBuf);

            } else {

                print_note("*** NOTE ***  Extra 0 at beginning of hex string ignored.", lineBuf);

                /* point to string so yacc can copy it */
                yytext[1] = '\'';
                yylval.char_ptr = yytext+1;
                /* yylval.char_ptr = yytext+2; */
                return HEX_NUMBER;
 
            }

        }

    } 

    /* point to string so yacc can copy it */
    yylval.char_ptr = yytext;
    /* yylval.char_ptr = yytext+1; */
    return HEX_NUMBER; 
}

%{
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ========== UppercaseIdentifiers and Reserved Words ========= */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
%}

<start>{UppercaseIdentifier} {   

    int retValue;

    /* UppercaseIdentifiers are in positions 0 to 82 */
    if( (retValue = binarySearch (yytext, reserved, 0, 82)) ) {

        col_cntr+=yyleng;
        return retValue;

    }

    /* no match for reserved word, so return UPPERCASE_IDENTIFIER token */

    /* point to string so yacc can copy it */
    yylval.char_ptr = yytext;

    col_cntr+=yyleng;
    return UPPERCASE_IDENTIFIER; 
}

%{
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ========== LowercaseIdentifiers and Reserved Words ========= */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
%}

<start>{LowercaseIdentifier} {   

    int retValue;

    /* LowercaseIdentifiers are in positions 83 to 94 */
    if((retValue = binarySearch (yytext, reserved, 83, 94))) {

        col_cntr+=yyleng;
        return retValue;

    }

    /* no match for reserved word, so return LOWERCASE_IDENTIFIER token */

    /* point to string so yacc can copy it */
    yylval.char_ptr = yytext;
    col_cntr+=yyleng;
    return LOWERCASE_IDENTIFIER; 
}

%{
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ==================== Illegal Identifiers =================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
%}

<start>{NumberIdentifier} { 

    print_warning("*** ERROR ***  Identifiers should not start with numbers.", lineBuf);

    /* point to string so yacc can copy it */
    yylval.char_ptr = yytext;
    col_cntr+=yyleng;
    return LOWERCASE_IDENTIFIER; 
}

<start>{ExtendedUppercaseIdentifier} { 

    print_warning("*** ERROR ***  You are using an illegal character in the UppercaseIdentifier (&, /, or _ ).", lineBuf);
    /* point to string so yacc can copy it */
    yylval.char_ptr = yytext;
    col_cntr+=yyleng;
    return UPPERCASE_IDENTIFIER; 
}

<start>{ExtendedLowercaseIdentifier} { 

    print_warning("*** ERROR ***  You are using an illegal character in the LowercaseIdentifier (&, /, or _).", lineBuf);

    /* point to string so yacc can copy it */
    yylval.char_ptr = yytext;
    col_cntr+=yyleng;
    return LOWERCASE_IDENTIFIER; 
}

%{
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ====================== QUOTED STRINGS ====================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
%}

<start>["].* {   
    /* copy the first line that contains a quoted string, 
       then put back everything except the quote mark  */

    /* get ready to start collecting pieces of new quoted string */
    stringPos = 0;
    memset(stringBuf, 0, stringBufSize );

    /* possible unterminated string reporting info*/
    memset(firstLineBuf, 0, MYBUFSIZE );
    strcpy(firstLineBuf, lineBuf);
    string_col_cntr  = col_cntr+1;
    string_line_cntr = line_cntr;

    /* normal error reporting column position */
    col_cntr += 1;

    /* put back everything except begin quote mark */
    yyless(1);

    BEGIN quote; 
}

<quote>[\\]["]  {   /* ESCAPED QUOTE IN STRING AS IN PRINTF */
                    /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */ 
    /* normal error reporting column position */
    col_cntr+=yyleng;

    /* info for normal error reporting */
    strncat(lineBuf, yytext, 2);

    /* idiot mib writers can have strings up to 16 pages in length */
    if(stringPos >= stringBufSize - 2) {
        make_stringBuf_bigger();
    }

    /* add char to new quoted string */
    stringBuf[stringPos++] = yytext[0];
    stringBuf[stringPos++] = yytext[1];
}

<quote>["].* {   
    /* END QUOTED STRING   */
    /*
    A character string is preceded and followed by the quote character
    ("), and consists of an arbitrary number (possibly zero) of:

    - any 7-bit displayable ASCII characters except quote ("),
    - tab characters,
    - spaces, and
    - line terminator characters (\n or \r\n).
    */


    /* info for normal error reporting */
    strncat(lineBuf, yytext, MYBUFSIZE);

    /* point to captured string so bison can copy it */
    yylval.char_ptr = stringBuf;

    /* put back everything except end quote mark */
    yyless(1);

    /* normal error reporting column position */
    col_cntr+=1;

    BEGIN start;

    return QUOTED_STRING; 
}


<quote>"}" {    /* the following productions are used for identifying
                   possible unterminated strings */

    /* normal error reporting column position */
    col_cntr+=yyleng;

    /* info for normal error reporting */
    strncat(lineBuf, yytext, 1);

    /* idiot mib writers can have strings up to 16 pages in length */
    if(stringPos >= stringBufSize - 1) {
        make_stringBuf_bigger();
    }

    /* possible unterminated string */
    stringWarnOn = 1;

    /* add char to new quoted string */
    stringBuf[stringPos++] = yytext[0];

}

<quote>"REFERENCE" {
    /* normal error reporting column position */
    col_cntr+=yyleng;

    /* info for normal error reporting */
    strncat(lineBuf, yytext, 9);

    /* don't run out of room */
    if(stringPos >= stringBufSize - 9) {
        make_stringBuf_bigger();
    }

    /* possible unterminated string */
    stringWarnOn = 1;

    /* add to new quoted string */
    stringBuf[stringPos++] = yytext[0];
    stringBuf[stringPos++] = yytext[1];
    stringBuf[stringPos++] = yytext[2];
    stringBuf[stringPos++] = yytext[3];
    stringBuf[stringPos++] = yytext[4];
    stringBuf[stringPos++] = yytext[5];
    stringBuf[stringPos++] = yytext[6];
    stringBuf[stringPos++] = yytext[7];
    stringBuf[stringPos++] = yytext[8];
}

<quote>"CONTACT-INFO" {
    /* normal error reporting column position */
    col_cntr+=yyleng;

    /* info for normal error reporting */
    strncat(lineBuf, yytext, 12);

    /* don't run out of room */
    if(stringPos >= stringBufSize - 12) {
        make_stringBuf_bigger();
    }

    /* possible unterminated string */
    stringWarnOn = 1;

    /* add to new quoted string */
    stringBuf[stringPos++] = yytext[0];
    stringBuf[stringPos++] = yytext[1];
    stringBuf[stringPos++] = yytext[2];
    stringBuf[stringPos++] = yytext[3];
    stringBuf[stringPos++] = yytext[4];
    stringBuf[stringPos++] = yytext[5];
    stringBuf[stringPos++] = yytext[6];
    stringBuf[stringPos++] = yytext[7];
    stringBuf[stringPos++] = yytext[8];
    stringBuf[stringPos++] = yytext[9];
    stringBuf[stringPos++] = yytext[10];
    stringBuf[stringPos++] = yytext[11];
}

<quote>"DESCRIPTION" {
    /* normal error reporting column position */
    col_cntr+=yyleng;

    /* info for normal error reporting */
    strncat(lineBuf, yytext, 11);

    /* don't run out of room */
    if(stringPos >= stringBufSize - 11) {
        make_stringBuf_bigger();
    }

    /* possible unterminated string */
    stringWarnOn = 1;

    /* add to new quoted string */
    stringBuf[stringPos++] = yytext[0];
    stringBuf[stringPos++] = yytext[1];
    stringBuf[stringPos++] = yytext[2];
    stringBuf[stringPos++] = yytext[3];
    stringBuf[stringPos++] = yytext[4];
    stringBuf[stringPos++] = yytext[5];
    stringBuf[stringPos++] = yytext[6];
    stringBuf[stringPos++] = yytext[7];
    stringBuf[stringPos++] = yytext[8];
    stringBuf[stringPos++] = yytext[9];
    stringBuf[stringPos++] = yytext[10];
}

<quote>"REVISION" {
    /* normal error reporting column position */
    col_cntr+=yyleng;

    /* info for normal error reporting */
    strncat(lineBuf, yytext, 8);

    /* don't run out of room */
    if(stringPos >= stringBufSize - 8) {
        make_stringBuf_bigger();
    }

    /* possible unterminated string */
    stringWarnOn = 1;

    /* add to new quoted string */
    stringBuf[stringPos++] = yytext[0];
    stringBuf[stringPos++] = yytext[1];
    stringBuf[stringPos++] = yytext[2];
    stringBuf[stringPos++] = yytext[3];
    stringBuf[stringPos++] = yytext[4];
    stringBuf[stringPos++] = yytext[5];
    stringBuf[stringPos++] = yytext[6];
    stringBuf[stringPos++] = yytext[7];
}

<quote>"STATUS" {
    /* normal error reporting column position */
    col_cntr+=yyleng;

    /* info for normal error reporting */
    strncat(lineBuf, yytext, 6);

    /* don't run out of room */
    if(stringPos >= stringBufSize - 6) {
        make_stringBuf_bigger();
    }

    /* possible unterminated string */
    stringWarnOn = 1;

    /* add to new quoted string */
    stringBuf[stringPos++] = yytext[0];
    stringBuf[stringPos++] = yytext[1];
    stringBuf[stringPos++] = yytext[2];
    stringBuf[stringPos++] = yytext[3];
    stringBuf[stringPos++] = yytext[4];
    stringBuf[stringPos++] = yytext[5];
}

<quote>"::=" {
    /* normal error reporting column position */
    col_cntr+=yyleng;

    /* info for normal error reporting */
    strncat(lineBuf, yytext, 3);

    /* don't run out of room */
    if(stringPos >= stringBufSize - 3) {
        make_stringBuf_bigger();
    }

    /* possible unterminated string */
    stringWarnOn = 1;

    /* add to new quoted string */
    stringBuf[stringPos++] = yytext[0];
    stringBuf[stringPos++] = yytext[1];
    stringBuf[stringPos++] = yytext[2];
}

<quote>"ORGANIZATION" {
    /* normal error reporting column position */
    col_cntr+=yyleng;

    /* info for normal error reporting */
    strncat(lineBuf, yytext, 12);

    /* don't run out of room */
    if(stringPos >= stringBufSize - 12) {
        make_stringBuf_bigger();
    }

    /* possible unterminated string */
    stringWarnOn = 1;

    /* add to new quoted string */
    stringBuf[stringPos++] = yytext[0];
    stringBuf[stringPos++] = yytext[1];
    stringBuf[stringPos++] = yytext[2];
    stringBuf[stringPos++] = yytext[3];
    stringBuf[stringPos++] = yytext[4];
    stringBuf[stringPos++] = yytext[5];
    stringBuf[stringPos++] = yytext[6];
    stringBuf[stringPos++] = yytext[7];
    stringBuf[stringPos++] = yytext[8];
    stringBuf[stringPos++] = yytext[9];
    stringBuf[stringPos++] = yytext[10];
    stringBuf[stringPos++] = yytext[11];
}

<quote>"ACCESS" {
    /* normal error reporting column position */
    col_cntr+=yyleng;

    /* info for normal error reporting */
    strncat(lineBuf, yytext, 6);

    /* don't run out of room */
    if(stringPos >= stringBufSize - 6) {
        make_stringBuf_bigger();
    }

    /* possible unterminated string */
    stringWarnOn = 1;

    /* add to new quoted string */
    stringBuf[stringPos++] = yytext[0];
    stringBuf[stringPos++] = yytext[1];
    stringBuf[stringPos++] = yytext[2];
    stringBuf[stringPos++] = yytext[3];
    stringBuf[stringPos++] = yytext[4];
    stringBuf[stringPos++] = yytext[5];
}


<quote>. {  /* NORMAL CHARACTERS */
    /* normal error reporting column position */
    col_cntr+=yyleng;

    /* info for normal error reporting */
    strncat(lineBuf, yytext, 1);

    /* don't run out of room */
    if(stringPos >= stringBufSize - 1) {
        make_stringBuf_bigger();
    }

    /* add to new quoted string */
    stringBuf[stringPos++] = yytext[0];

}

<quote>{end_of_line} { 
    /* normal error reporting line position */
    line_cntr++; 
    /* normal error reporting column position */
    col_cntr = 0;
    /* reset buffer for normal errors */
    memset(lineBuf, 0, MYBUFSIZE );

    /* don't run out of room */
    if(stringPos >= stringBufSize - 1) {
        make_stringBuf_bigger();
    }

    /* add to new quoted string */
    stringBuf[stringPos++] = '\n';
}

%{
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ============================== OID ========================= */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
%}

<start>{oid} {
    /* point to string so bison can copy it */
    yylval.char_ptr = yytext;
    col_cntr+=yyleng;
    return OID; 
}

%{
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ====================== COMMENTS & JUNK ===================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
%}

<start>"--" { 
    /* we have to have a separate comment state to handle something like
        -- sfasdfadsf -- END -- asdfasdfasdf --
    */

    col_cntr+=yyleng;
    BEGIN comment;
} 

<comment>"--" {

    /* if we have turned on C++ comment flag, then only an end of line can end the comment */
    if(!globalCppCommentFlag) { 
        col_cntr+=yyleng;
        BEGIN start;
    }

} 

<comment>[\r] {
    /* put back the cr */
    yyless(yyleng-1);
    BEGIN start;
} 

<comment>[\n] {
    /* put back the newline */
    yyless(yyleng-1); 
    BEGIN start;
} 

<comment>[\r][\n] {
    /* put back the cr, newline */
    yyless(yyleng-2); 
    BEGIN start;
} 

<comment>. {
    col_cntr+=1;
    /* toss everything inside the comment */
} 

%{
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ========================== GARGAGE ========================= */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
%}

<<EOF>> { 
    /* end of file encountered before end of mib */  
    /* this could happen with garbage mibs       */

    /* resync states */
    BEGIN INITIAL;
    return EOF;
 }

. { 
    /* we should never see this */ 
    print_warning("*** ERROR ***  Garbage in input file.", lineBuf); 
}

%{
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ====================== USER CODE =========================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
%}

%%

int yywrap() {
    return 1;
}

/* =========================================================================== */

/* reset lexer to initial state, used when lexer called multiple times */
void
clear_input(){
    YY_FLUSH_BUFFER;
    BEGIN(INITIAL);
}

/* =========================================================================== */

/* return 0 if not found, else return token value */

int binarySearch (char * key, struct table * mytable, int low, int high) {

    int mid;
    int tmpVal;

    while(low <= high) {

        mid = (low + high) / 2;

        tmpVal = strcmp (mytable[mid].keyword, key);

        if( tmpVal > 0 ) {
            high = mid - 1;
        } else if( tmpVal < 0) {
            low = mid+1;
        } else {

            return mytable[mid].token;

        }

    }
    
    return 0;

}

/* =========================================================================== */

void
make_stringBuf_bigger(){

    /* allocate an extra MYBUFSIZE of new storage */
    char * newBuf = (char *)malloc((stringBufSize+MYBUFSIZE)*sizeof(char));
    if(!newBuf) {
            fprintf(stderr, "compiler abort - out of memory\n");
            _exit(1);
    }

    /* copy the old string buffer into the new one */
    memcpy(newBuf, stringBuf, stringBufSize);

    /* set the new storage size */
    stringBufSize = stringBufSize+MYBUFSIZE;

    /* free the old string buffer */
    free(stringBuf);

    /* set stringBuf to point to the new stringBuf */
    stringBuf = newBuf;

}

/* =========================================================================== */
