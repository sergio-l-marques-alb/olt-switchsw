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

/* A Bison parser, made by GNU Bison 1.875c.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     BAR = 258,
     AUGMENTS = 259,
     CCE = 260,
     APPLICATION = 261,
     ANY = 262,
     AGENT_CAPABILITIES = 263,
     accessible_for_notify = 264,
     ACCESS = 265,
     Counter64 = 266,
     Counter32 = 267,
     Counter = 268,
     CONTACT_INFO = 269,
     COMMA = 270,
     CHOICE = 271,
     BY = 272,
     BITS = 273,
     BIT = 274,
     BINARY_NUMBER = 275,
     BEGIN_ = 276,
     DISPLAY_HINT = 277,
     DESCRIPTION = 278,
     deprecated = 279,
     DEFVAL = 280,
     DEFINITIONS = 281,
     DEFINED = 282,
     currrent = 283,
     CREATION_REQUIRES = 284,
     IDENTIFIER = 285,
     HEX_NUMBER = 286,
     GROUP = 287,
     Gauge32 = 288,
     Gauge = 289,
     FROM = 290,
     EXPORTS = 291,
     EXPLICIT = 292,
     ENTERPRISE = 293,
     END = 294,
     DOT_DOT = 295,
     DOT = 296,
     LBRACE = 297,
     LAST_UPDATED = 298,
     IpAddress = 299,
     Integer32 = 300,
     INTEGER = 301,
     INDEX = 302,
     INCLUDES = 303,
     IMPORTS = 304,
     IMPLIED = 305,
     IMPLICIT = 306,
     MIN = 307,
     MAX = 308,
     MAX_ACCESS = 309,
     MANDATORY_GROUPS = 310,
     mandatory = 311,
     LPAREN = 312,
     LOWERCASE_IDENTIFIER = 313,
     LBRACKET = 314,
     NEG_NUMBER = 315,
     MODULE_IDENTITY = 316,
     MODULE_CONFORMANCE = 317,
     MODULE_COMPLIANCE = 318,
     MODULE = 319,
     MIN_ACCESS = 320,
     NULL_ = 321,
     not_implemented = 322,
     NOTIFICATION_TYPE = 323,
     NOTIFICATIONS = 324,
     NOTIFICATION_GROUP = 325,
     not_accessible = 326,
     OCTET = 327,
     obsolete = 328,
     OBJECT_TYPE = 329,
     OBJECTS = 330,
     ObjectSyntax = 331,
     OBJECT_IDENTITY = 332,
     OBJECT_GROUP = 333,
     OBJECT = 334,
     RBRACE = 335,
     QUOTED_STRING = 336,
     PRODUCT_RELEASE = 337,
     PRIVATE = 338,
     POS_NUMBER = 339,
     ORGANIZATION = 340,
     optional = 341,
     Opaque = 342,
     OID = 343,
     OF = 344,
     SEQUENCE = 345,
     SEMICOLON = 346,
     RPAREN = 347,
     REVISION = 348,
     REFERENCE = 349,
     read_write = 350,
     read_only = 351,
     read_create = 352,
     RBRACKET = 353,
     UNION = 354,
     UInteger32 = 355,
     TRAP_TYPE = 356,
     TimeTicks = 357,
     TEXTUAL_CONVENTION = 358,
     SYNTAX = 359,
     SUPPORTS = 360,
     STRUCT = 361,
     STRING = 362,
     STATUS = 363,
     SIZE = 364,
     write_only = 365,
     VARIATION = 366,
     VARIABLES = 367,
     UPPERCASE_IDENTIFIER = 368,
     Unsigned32 = 369,
     UNIVERSAL = 370,
     UNITS = 371,
     WRITE_SYNTAX = 372
   };
#endif
#define BAR 258
#define AUGMENTS 259
#define CCE 260
#define APPLICATION 261
#define ANY 262
#define AGENT_CAPABILITIES 263
#define accessible_for_notify 264
#define ACCESS 265
#define Counter64 266
#define Counter32 267
#define Counter 268
#define CONTACT_INFO 269
#define COMMA 270
#define CHOICE 271
#define BY 272
#define BITS 273
#define BIT 274
#define BINARY_NUMBER 275
#define BEGIN_ 276
#define DISPLAY_HINT 277
#define DESCRIPTION 278
#define deprecated 279
#define DEFVAL 280
#define DEFINITIONS 281
#define DEFINED 282
#define currrent 283
#define CREATION_REQUIRES 284
#define IDENTIFIER 285
#define HEX_NUMBER 286
#define GROUP 287
#define Gauge32 288
#define Gauge 289
#define FROM 290
#define EXPORTS 291
#define EXPLICIT 292
#define ENTERPRISE 293
#define END 294
#define DOT_DOT 295
#define DOT 296
#define LBRACE 297
#define LAST_UPDATED 298
#define IpAddress 299
#define Integer32 300
#define INTEGER 301
#define INDEX 302
#define INCLUDES 303
#define IMPORTS 304
#define IMPLIED 305
#define IMPLICIT 306
#define MIN 307
#define MAX 308
#define MAX_ACCESS 309
#define MANDATORY_GROUPS 310
#define mandatory 311
#define LPAREN 312
#define LOWERCASE_IDENTIFIER 313
#define LBRACKET 314
#define NEG_NUMBER 315
#define MODULE_IDENTITY 316
#define MODULE_CONFORMANCE 317
#define MODULE_COMPLIANCE 318
#define MODULE 319
#define MIN_ACCESS 320
#define NULL_ 321
#define not_implemented 322
#define NOTIFICATION_TYPE 323
#define NOTIFICATIONS 324
#define NOTIFICATION_GROUP 325
#define not_accessible 326
#define OCTET 327
#define obsolete 328
#define OBJECT_TYPE 329
#define OBJECTS 330
#define ObjectSyntax 331
#define OBJECT_IDENTITY 332
#define OBJECT_GROUP 333
#define OBJECT 334
#define RBRACE 335
#define QUOTED_STRING 336
#define PRODUCT_RELEASE 337
#define PRIVATE 338
#define POS_NUMBER 339
#define ORGANIZATION 340
#define optional 341
#define Opaque 342
#define OID 343
#define OF 344
#define SEQUENCE 345
#define SEMICOLON 346
#define RPAREN 347
#define REVISION 348
#define REFERENCE 349
#define read_write 350
#define read_only 351
#define read_create 352
#define RBRACKET 353
#define UNION 354
#define UInteger32 355
#define TRAP_TYPE 356
#define TimeTicks 357
#define TEXTUAL_CONVENTION 358
#define SYNTAX 359
#define SUPPORTS 360
#define STRUCT 361
#define STRING 362
#define STATUS 363
#define SIZE 364
#define write_only 365
#define VARIATION 366
#define VARIABLES 367
#define UPPERCASE_IDENTIFIER 368
#define Unsigned32 369
#define UNIVERSAL 370
#define UNITS 371
#define WRITE_SYNTAX 372




#if (! ( defined(YYSTYPE) ) && ! ( defined(YYSTYPE_IS_DECLARED) ))
#line 622 "parser.y"
typedef union YYSTYPE {
    int val;
    char * char_ptr;
    struct tree_tag * tree_ptr;
} YYSTYPE;
/* Line 1268 of yacc.c.  */
#line 277 "y.tab.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif	/* (! ( defined(YYSTYPE) ) && ! ( defined(YYSTYPE_IS_DECLARED) )) */

extern YYSTYPE yylval;



