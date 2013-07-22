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

/* 
 * Build this module only if config files are desired
 */

#ifdef SR_CONFIG_FP

#include "sr_conf.h"

#include <stdio.h>

#include <string.h>


#include <ctype.h>

#include <malloc.h>

#include <stdlib.h>

#include <unistd.h>


#include <sys/types.h>

#include <sys/stat.h>

#include <fcntl.h>

#include <errno.h>


#include "sr_snmp.h"
#include "compat.h"
#include "sr_cfg.h"
#include "lookup.h"
#include "v2table.h"
#include "scan.h"
#include "oid_lib.h"
#include "diag.h"




SR_FILENAME



/* static functions */
#ifndef SR_NO_COMMENTS_IN_CONFIG
static stableConfigFileInfo_t *srUserCommentsInfo_head = NULL;
static stableConfigFileInfo_t *srUserCommentsInfo_tail = NULL;

static stableConfigFileInfo_t *srMallocCommentStructure(const char *filename);
static SR_INT32 srInitializeCommentStructures(const char *filename);
static stableConfigFileInfo_t *srFindCommentEntry(const char *filename);
static SR_INT32 srAddCommentEntry(stableConfigFileInfo_t *ptr);
static SR_INT32 srAddCommentLineEntry(stableConfigFileInfo_t *ptr, char *line);
static void srFreeCommentLineEntries(srCommentLine_t *head_ptr,
                                     srCommentLine_t *tail_ptr);
#endif /* SR_NO_COMMENTS_IN_CONFIG */

static void FillInScalarVar
    SR_PROTOTYPE((const PARSER_RECORD_TYPE *rtp,
                  ValType *vp));
static int CreateTableEntry
    SR_PROTOTYPE((const PARSER_RECORD_TYPE *rtp,
                  ValType *vp));
static char *ReadLines
    SR_PROTOTYPE((char *buf,
                  int size,
                  ConfigFile *cfp));
static int ReadConfigLine
    SR_PROTOTYPE((ConfigFile *cfp,
                  char *buf,
                  int size));
static void nexttoken
    SR_PROTOTYPE((char **str1,
                  char **str2));
static void FreeValTypes
    SR_PROTOTYPE((const SnmpType *tt,
                  ValType *vp,
                  int count));
static int ProcessConfigRecord
    SR_PROTOTYPE((int linecount,
                  char *cfgline,
                  const SnmpType *type_table,
                  const PARSER_CONVERTER *converters,
                  ValType *values));
static int ProcessConfigRecordForcingIndexOrder
    SR_PROTOTYPE((int linecount,
                  char *cfgline,
                  const SnmpType *type_table,
                  const PARSER_CONVERTER *converters,
                  ValType *values));
#ifndef SR_NO_WRITE_CONFIG_FILE
static int WriteScalarEntry
    SR_PROTOTYPE((ConfigFile *cfgp,
                  const char *label,
                  const SnmpType *type_table,
                  const PARSER_CONVERTER *converters,
                  void *scalar));
static int WriteTableEntry
    SR_PROTOTYPE((ConfigFile *cfgp,
                  const char *label,
                  const SnmpType *type_table,
                  const PARSER_CONVERTER *converters,
                  char *entry,
                  SR_INT32 rs_offset,
                  SR_INT32 st_offset,
                  SR_INT32 us_offset));
static int WriteTableEntryForcingIndexOrder
    SR_PROTOTYPE((ConfigFile *cfgp,
                  const char *label,
                  const SnmpType *type_table,
                  const PARSER_CONVERTER *converters,
                  char *entry,
                  SR_INT32 rs_offset,
                  SR_INT32 st_offset,
                  SR_INT32 us_offset));
#endif /* SR_NO_WRITE_CONFIG_FILE */

/*
 * ReadLines:
 *
 * This routine simulates a call to fgets, except that if a line ends with
 * a backslash (line continuation), the next line will be read and appended.
 * the maximum length of all lines read is still specified by the size arg.
 */
static char *
ReadLines(buf, size, cfp)
    char *buf;
    int size;
    ConfigFile *cfp;
{
    char *orig_buf = buf;

    int len;
    cfp->startline = 0;
    while (sr_fgets(buf, size, cfp) != NULL) {
        cfp->linecount++;
        len = strlen(buf);
        if (len == 0) {
            continue;
        }
        if (buf[len-1] == '\n') {
            buf[len-1] = '\0';
            len--;
        }
        if (len == 0) {
            continue;
        }
        if (cfp->startline == 0) {
            cfp->startline = cfp->linecount;
        }
        if (buf[len-1] != '\\') {
            return(orig_buf);
        }
        buf += len - 1;
        buf[0] = '\0';
        size -= len + 1;
    }
    if (orig_buf == buf) {
        return NULL;
    }
    return orig_buf;
}

/*
 * ReadConfigLine:
 * this routine reads through a config file until it finds a non-blank,
 * non-comment line.  It strips out any comments, then returns the line
 */
static int
ReadConfigLine(cfp, buf, size)
    ConfigFile *cfp;
    char *buf;
    int size;
{
    int status;
    char *p;

    /* skip comments and whitespace */
    status = 0;
    while(status == 0 && ReadLines(buf, size, cfp) != 0) {
        /* keep track of the line number */

        /* strip out comments */
/*
        if((p = strchr(buf, '#')) != 0) {
            *p = 0;
        }
*/
        /* see if there is anything left on the line */
        for(p = buf; *p != 0; p++) {
            if(!isspace((unsigned char)*p)) {
#ifndef SR_NO_COMMENTS_IN_CONFIG
                /*
                 * If the line starts with ##, we have a user comment line
                 * to save so we can write it out to the config file when
                 * WriteConfigFile() is called.
                 */
                if (*p == '#') {
                    if (*(p+1) == '#') {
                        if (srAddCommentLineEntry(cfp->curCommentEntry, p) == -1) {
                            DPRINTF((APCONFIG,
                                     "ReadConfigLine: Unable to add user comment line to list.\n"));
                        }
                        break;
                    }
                }
#endif /* SR_NO_COMMENTS_IN_CONFIG */
                if (*p == '#') {
                    break;
                }
                status = 1;
                break;
            }
        }
        /* if line begins with white space, scrunch it over */
        for (p = buf; *p == ' '; p++) ;
        if (p != buf) {
           char *q = buf;
	   /*
	    * At this point, q points to buf, and p points to buf+1.
	    * 
	    * The next while loop copies all characters in the array
	    * (through the first NULL character) to one position
	    * previous in the array.  the leading space character is
	    * clobbered by the second character in the array, and the
	    * last non-NULL character in the array is clobbered as the
	    * NULL character is copied.
	    * 
	    * When the element to which p points contains a NULL, the
	    * NULL character is copied, and the "assignment expression"
	    * evaluates as false, which causes execution to break out
	    * of the while loop.
	    */
           while ((*q++ = *p++)) ;
        }
    }

#ifdef SR_DEBUG
    if (strlen(buf) > SR_MAX_LOG_MESSAGE_SIZE - 50) {
        DPRINTF((APCONFIG, "ReadConfigLine: buffer is too long to print\n"));
    } else {
        DPRINTF((APCONFIG, "ReadConfigLine: buffer is: %s\n", buf));
    }
#endif /* SR_DEBUG */
    /* return nonzero if there is anything in buf */
    return(status);
}

static void
nexttoken(str1, str2)
    char **str1, **str2;
{
    int bsc = 0;
    char *s1 = *str2, *s2;
    char tokend;

    if (s1 == NULL) {
        *str1 = NULL;
        return;
    }

    while ((*s1 == ' ') || (*s1 == '\t')) {
        s1++;
    }
    tokend = ' ';
    if (*s1 == '\"') {
        tokend = '\"';
        s1++;
    } else
    if (*s1 == '\'') {
        tokend = '\'';
        s1++;
    }
    s2 = s1;
/*
    while ((*s2 != '\0') && (*s2 != tokend)) {
        s2++;
    }
*/
    while (1) {
        if (*s2 == '\0') {
            break;
        }
        if ((*s2 == tokend) && (bsc == 0)) {
            break;
        }
        if (*s2 == '\\') {
            bsc = (bsc + 1) % 2;
        } else {
            bsc = 0;
        }
        s2++;
    }
    if (*s2 != '\0') {
        *s2 = '\0';
        s2++;
    } else {
        s2 = NULL;
    }
    if (*s1 == '\0') {
        s1 = NULL;
    }

    *str1 = s1;
    *str2 = s2;
}

/* ----------------------------------------------------------------------
 */
static void 
writeConfigLogHeader(const char *logfilename, const char *configFilename) 
{
    write_config_log(logfilename, "\n\n----------------------------");
    write_config_log(logfilename, "----------------------------\n");
    write_config_log(logfilename, "Parsing config file:  ");
    write_config_log(logfilename, configFilename);
    write_config_log(logfilename, "\n");
    write_config_log(logfilename, "----------------------------");
    write_config_log(logfilename, "----------------------------\n");
    return;
}


SR_INT32
ConvToken_integer(direction, token, value)
    SR_INT32 direction;
    char **token;
    void *value;
{
    SR_INT32 *local = (SR_INT32 *)value;
    switch (direction) {
        case PARSER_CONVERT_FROM_TOKEN:
            *local = strtol(*token, NULL, 10);
            return 0;
        case PARSER_CONVERT_TO_TOKEN:
            *token = (char *)malloc(32);
            if (*token == NULL) {
                return -1;
            }
            sprintf(*token, "%d", *local);
#if SIZEOF_INT == 4
            sprintf(*token, "%d", *local);
#else	/*  SIZEOF_INT == 4 */
            sprintf(*token, "%ld", *local);
#endif	/*  SIZEOF_INT == 4 */
            return 0;
        default:
            return -1;
    }
}

SR_INT32
ConvToken_unsigned(direction, token, value)
    SR_INT32 direction;
    char **token;
    void *value;
{
    SR_UINT32 *local = (SR_UINT32 *)value;
    switch (direction) {
        case PARSER_CONVERT_FROM_TOKEN:
            sscanf(*token, "%du", (unsigned int *) local);
            return 0;
        case PARSER_CONVERT_TO_TOKEN:
            *token = (char *)malloc(32);
            if (*token == NULL) {
                return -1;
            }
            sprintf(*token, "%du", (unsigned int) (*local));
            return 0;
        default:
            return -1;
    }
}

SR_INT32
ConvToken_counter64(direction, token, value)
    SR_INT32 direction;
    char **token;
    void *value;
{
  UInt64 **local = (UInt64 **)value;
  char *temp_p;

  switch (direction) {
      case PARSER_CONVERT_FROM_TOKEN:
	temp_p = strdup( (*token) );

	if( strlen(temp_p) != 18 ) {
	  DPRINTF((APWARN, "Counter64:Token '%s' is of wrong length\n", temp_p));
	  free(temp_p);
	  return -1;
	}

	(*local) = (UInt64 *)malloc( sizeof(UInt64) );
	if (*local == NULL) {
	  DPRINTF((APWARN, "Counter64:Failed to allocate memory\n"));
	  free(temp_p);
	  return -1;
	}

	sscanf( (temp_p+10), "%lx", (unsigned long *) &(*local)->little_end );
	temp_p[10] = 0;
	sscanf( (temp_p+2), "%lx", (unsigned long *) &(*local)->big_end );
	free(temp_p);
        return 0;

      case PARSER_CONVERT_TO_TOKEN:
	*token=(char *)malloc( 20 );            /* '0x' + 8 char + 8 char + NULL */
	if (*token == NULL) return -1;
	if (*local == NULL) {
	    sprintf(*token, "0x%08x%08x", 0, 0);
	} else {
	    sprintf(*token, "0x%08x%08x",
		    (*local)->big_end, (*local)->little_end);
	}
	return 0;

      default:
	return -1;
  }
}

static const ParserIntegerTranslation boolean_Translations[] = {
    { "true", 1 },
    { "false", 0 },
    { "yes", 1 },
    { "no", 0 },
    { "1", 1 },
    { "0", 0 },
    { NULL, 0 }
};
const ParserIntegerTranslationTable boolean_TranslationTable = {
    INTEGER_TYPE,
    "boolean",
    16,
    boolean_Translations
};

static const ParserIntegerTranslation storageType_Translations[] = {
    { "other", SR_OTHER },
    { "volatile", SR_VOLATILE },
    { "nonVolatile", SR_NONVOLATILE },
    { "readOnly", SR_READONLY },
    { "permanent", SR_PERMANENT },
    { "1", SR_OTHER },
    { "2", SR_VOLATILE },
    { "3", SR_NONVOLATILE },
    { "4", SR_READONLY },
    { "5", SR_PERMANENT },
    { NULL, 0 }
};
const ParserIntegerTranslationTable storageType_TranslationTable = {
    INTEGER_TYPE,
    "StorageType",
    16,
    storageType_Translations
};

SR_INT32
ConvToken_integerTranslation(
    SR_INT32 direction,
    char **token,
    void *value,
    ParserIntegerTranslationTable *pitt)
{
    SR_INT32 *local = (SR_INT32 *)value;
    int i;
    switch (direction) {
        case PARSER_CONVERT_FROM_TOKEN:
            for (i = 0; pitt->pit[i].token != NULL; i++) {
                if (strcmp(*token, pitt->pit[i].token) == 0) {
                    *local = pitt->pit[i].val;
                    return 0;
                }
            }
            /*
             * If no match in translation table, try converting string to
             * an integer. 
             */
            *local = strtol(*token, NULL, 10);
            return 0;
        case PARSER_CONVERT_TO_TOKEN:
            *token = (char *)malloc(pitt->max_token_length);
            if (*token == NULL) {
                return -1;
            }
            for (i = 0; pitt->pit[i].token != NULL; i++) {
                if (*local == pitt->pit[i].val) {
                    strcpy(*token, pitt->pit[i].token);
                    return 0;
                }
            }
            /*
             * If no match in translation table, try converting number to
             * a string. 
             */
            free(*token);
            *token = NULL;
            *token = (char *)malloc(32);
            if (*token == NULL) {
                return -1;
            }
            sprintf(*token, "%d", *local);
#if SIZEOF_INT == 4
            sprintf(*token, "%d", *local);
#else	/*  SIZEOF_INT == 4 */
            sprintf(*token, "%ld", *local);
#endif	/*  SIZEOF_INT == 4 */
            return 0;
        default:
            return -1;
    }
}

/*
 * FreeValTypes
 *
 * The functions frees the members of a ValType array.  The count argument
 * specifies how many members of the array are valid.  The tt argument
 * specifies the type_table which describes the types of the ValType
 * elements.  If count is equal to -1, the whole tt array is scanned.
 */
static void
FreeValTypes(tt, vp, count)
    const SnmpType *tt;
    ValType *vp;
    int count;
{
    int i;

    if (count == -1) {
        count = 32768;
    }

    for (i = 0; (tt[i].type != -1) && (i < count); i++) {
        switch (tt[i].type) {
            case OCTET_PRIM_TYPE:
                if (vp[i].osval != NULL) {
                    FreeOctetString(vp[i].osval);
                }
                break;
            case OBJECT_ID_TYPE:
                if (vp[i].oidval != NULL) {
                    FreeOID(vp[i].oidval);
                }
                break;
            default:
                break;
        }
    }
}

/*
 * ProcessConfigRecord:
 *   This routine attempts to parse a config file line according to a format.
 *
 *   Arguments:
 *      cfgline - The text to be parsed
 *      type_table - describes the type of each element in the entry
 *      converters - contains pointers to token converter functions and
 *                   default values for elements not stored in the file
 *      vp      - Array of ValType structure where the results are placed
 *
 *   The return value is 0 on success, and on failure is the number of
 *   tokens successfully matched before the error occured.
 *
 *   It is always the responsibility of the caller to free the contents
 *   of the ValType array, including when an error occurs.
 */
static int
ProcessConfigRecord(linecount, cfgline, type_table, converters, values)
    int linecount;
    char *cfgline;
    const SnmpType *type_table;
    const PARSER_CONVERTER *converters;
    ValType *values;
{
    FNAME("ProcessConfigRecord")
    char *cfgp, *tok = NULL;
    int matchcount = 0;
    SR_INT32 cv;
    const SnmpType *st;
    const PARSER_CONVERTER *pc;
    ValType *vp;

    /* get initial pointers to the line from the file */
    cfgp = cfgline;

    /* start parsing entries */
    for (pc = converters, st = type_table, vp = values;
         pc->conv_func != NULL; pc++, st++, vp++) {

        /* Extract the token from the configuration entry */
        if ((pc->defval == NULL) ||
            (pc->conv_func == SR_INTEGER_TRANSLATION)) {
            nexttoken(&tok, &cfgp);
            if (tok == NULL) {
                   DPRINTF((APWARN, "%s: Error, incomplete entry at line %d\n",
                         Fname, linecount));
                   goto fail;
            }
        } else {
            if (strcmp(pc->defval, "default:2048") == 0) {
/* NOTE: This is a special case to deal with snmpTargetAddrMMS so that
 * an agent can properly convert old format snmpTargetAddrEntry lines
 * to the new format which includes the mms value.  This code will be
 * removed in 16.x */
                nexttoken(&tok, &cfgp);
                if (tok == NULL) {
                    tok = pc->defval + 8;
                }
            } else if ((strncmp(pc->defval, "optional:default=", 17) == 0) &&
                       (strlen(pc->defval) > 17)) {
                /* special case to deal with variables added to a MIB in a
                 * later release. this allows the agent to read a config file
                 * with or without the new variable. if the new variable is
                 * present in the config file, it will be used, otherwise the
                 * default value will be used. note that optional values must
                 * be the last objects on a line in the config file.
                 */
                 nexttoken(&tok, &cfgp);
                 if (tok == NULL) {
                     tok = pc->defval + 17;
                 }
            } else {
                tok = pc->defval;
            }
        }

        /* Parse the token */
        switch (st->type) {
            case INTEGER_TYPE:
                if (pc->conv_func == SR_INTEGER_TRANSLATION) {
                    cv = ConvToken_integerTranslation(PARSER_CONVERT_FROM_TOKEN,
                        &tok, &vp->slval,
                        (ParserIntegerTranslationTable *)pc->defval);
                    break;
                }
                cv = (*pc->conv_func)(PARSER_CONVERT_FROM_TOKEN,
                                      &tok, &vp->slval);
                break;
            case COUNTER_TYPE:
            case GAUGE_TYPE:
            case TIME_TICKS_TYPE:
                cv = (*pc->conv_func)(PARSER_CONVERT_FROM_TOKEN,
                                      &tok, &vp->ulval);
                break;
            case OCTET_PRIM_TYPE:
                cv = (*pc->conv_func)(PARSER_CONVERT_FROM_TOKEN,
                                      &tok, &vp->osval);
                break;
            case OBJECT_ID_TYPE:
                cv = (*pc->conv_func)(PARSER_CONVERT_FROM_TOKEN,
                                      &tok, &vp->oidval);
		break;
	    case IP_ADDR_PRIM_TYPE:
                cv = (*pc->conv_func)(PARSER_CONVERT_FROM_TOKEN,
                                      &tok, &vp->ulval);
		break;
            case COUNTER_64_TYPE:
                cv = (*pc->conv_func)(PARSER_CONVERT_FROM_TOKEN,
                                      &tok, &vp->uint64val);
		break;
            default:
                DPRINTF((APWARN, 
			 "%s: Error, unsupported token type %d at line %d\n",
			 Fname, st->type, linecount));
                goto fail;
        }
        if (cv != 0) {
            DPRINTF((APWARN,
		     "%s: Error, cannot parse token %s\n", Fname, tok));
            goto fail;
        }

        /* count matched tokens */
        matchcount++;
    }

                   

    /* issue warning if there's anything else on the line */
    nexttoken(&tok, &cfgp);
    if (cfgp != NULL) {
        DPRINTF((APERROR, "%s: Warning, extra characters on line %d\n",
                 Fname, linecount));
        return 1;
    }

    return 0;

  fail:
                   
    FreeValTypes(type_table, values, matchcount);
    return -1;
}

/*
 * ProcessConfigRecordForcingIndexOrder:
 *   This routine attempts to parse a config file line according to a format.
 *
 *   Arguments:
 *      cfgline - The text to be parsed
 *      type_table - describes the type of each element in the entry
 *      converters - contains pointers to token converter functions and
 *                   default values for elements not stored in the file
 *      vp      - Array of ValType structure where the results are placed
 *
 *   The return value is 0 on success, and on failure is the number of
 *   tokens successfully matched before the error occured.
 *
 *   It is always the responsibility of the caller to free the contents
 *   of the ValType array, including when an error occurs.
 */
static int
ProcessConfigRecordForcingIndexOrder(
    int linecount,
    char *cfgline,
    const SnmpType *type_table,
    const PARSER_CONVERTER *converters,
    ValType *values)
{
    FNAME("ProcessConfigRecordForcingIndexOrder")
    char *cfgp, *tok = NULL;
    SR_INT32 cv;
    const SnmpType *st;
    const PARSER_CONVERTER *pc;
    ValType *vp;
    int i;
    int num_idx, cur_idx;
    int num_cvtrs, cur_cvtrs;

    /* get initial pointers to the line from the file */
    cfgp = cfgline;

    /* Count number of indices and converters */
    num_idx = 0;
    num_cvtrs = 0;
    for (pc = converters, st = type_table; pc->conv_func != NULL; pc++, st++) {
        if (st->indextype != -1) {
            num_idx++;
        }
        num_cvtrs++;
    }
    memset(values, 0, sizeof(ValType) * num_cvtrs);

    /* start parsing entries */
    cur_idx = 0;
    cur_cvtrs = 0;
    while (cur_cvtrs < num_cvtrs) {
/*
    for (pc = converters, st = type_table, vp = values;
         pc->conv_func != NULL; pc++, st++, vp++) {
*/
        if (cur_idx < num_idx) {
            /* need to process the next index */
            for (pc = converters, st = type_table, vp = values;
                 pc->conv_func != NULL; pc++, st++, vp++) {
                if (st->indextype == cur_idx) {
                    break;
                }
            }
            cur_idx++;
            cur_cvtrs++;
        } else {
            /* need to process the next non-index */
            for (pc = converters, st = type_table, vp = values, i = 0;
                 pc->conv_func != NULL; pc++, st++, vp++) {
                if (st->indextype == -1) {
                    if (i == (cur_cvtrs - cur_idx)) {
                        break;
                    }
                    i++;
                }
            }
            cur_cvtrs++;
        }

        /* Extract the token from the configuration entry */
        if ((pc->defval == NULL) ||
            (pc->conv_func == SR_INTEGER_TRANSLATION)) {
            nexttoken(&tok, &cfgp);
            if (tok == NULL) {
                DPRINTF((APWARN, "%s: Error, incomplete entry at line %d\n",
                         Fname, linecount));
                goto fail;
            }
        } else {
            tok = pc->defval;
        }

        /* Parse the token */
        switch (st->type) {
            case INTEGER_TYPE:
                if (pc->conv_func == SR_INTEGER_TRANSLATION) {
                    cv = ConvToken_integerTranslation(PARSER_CONVERT_FROM_TOKEN,
                        &tok, &vp->slval,
                        (ParserIntegerTranslationTable *)pc->defval);
                    break;
                }
                cv = (*pc->conv_func)(PARSER_CONVERT_FROM_TOKEN,
                                      &tok, &vp->slval);
                break;
            case COUNTER_TYPE:
            case GAUGE_TYPE:
            case TIME_TICKS_TYPE:
                cv = (*pc->conv_func)(PARSER_CONVERT_FROM_TOKEN,
                                      &tok, &vp->ulval);
                break;
            case OCTET_PRIM_TYPE:
                cv = (*pc->conv_func)(PARSER_CONVERT_FROM_TOKEN,
                                      &tok, &vp->osval);
                break;
            case OBJECT_ID_TYPE:
                cv = (*pc->conv_func)(PARSER_CONVERT_FROM_TOKEN,
                                      &tok, &vp->oidval);
                break;
	    case IP_ADDR_PRIM_TYPE:
                cv = (*pc->conv_func)(PARSER_CONVERT_FROM_TOKEN,
                                      &tok, &vp->ulval);
		break;
            case COUNTER_64_TYPE:
                cv = (*pc->conv_func)(PARSER_CONVERT_FROM_TOKEN,
                                      &tok, &vp->uint64val);
		break;
            default:
                DPRINTF((APWARN, 
			 "%s: Error, unsupported token type %d at line %d\n",
			 Fname, st->type, linecount));
                goto fail;
        }
        if (cv != 0) {
            DPRINTF((APWARN,
		     "%s: Error, cannot parse token %s\n", Fname, tok));
            goto fail;
        }
    }

    /* issue warning if there's anything else on the line */
    nexttoken(&tok, &cfgp);
    if (cfgp != NULL) {
        DPRINTF((APERROR, "%s: Warning, extra characters on line %d\n",
                 Fname, linecount));
        return 1;
    }

    return 0;

  fail:
    FreeValTypes(type_table, values, num_cvtrs);
    return -1;
}

int
ParseConfigFile(filename, rt)
    const char *filename;
    const PARSER_RECORD_TYPE *rt[];
{
    if (ParseConfigFileWithErrorLog(filename, NULL, rt) == -1) {
        return -1;
    }
    return 0;
}

int
ParseConfigFileWithErrorLog(filename, logfilename, rt)
    const char *filename;
    const char *logfilename;
    const PARSER_RECORD_TYPE *rt[];
{
    FNAME("ParseConfigFileWithErrorLog")
    int status = 0;
    ConfigFile *cfgp = NULL;
    char *cfgline = NULL, *cfgdata = NULL;
    int rti;
    int st;
    ValType *cfg_vals = NULL;
    int i, max_cfg_vals;
    int len;
    int configLogHeaderWritten = 0;
#ifndef SR_NO_COMMENTS_IN_CONFIG
    stableConfigFileInfo_t *curCommentEntry = NULL;
    stableConfigFileInfo_t *newCommentEntry = NULL;
#endif /* SR_NO_COMMENTS_IN_CONFIG */

    /* Determine what the size of cfg_vals should be */
    max_cfg_vals = 0;
    for (rti = 0; rt[rti] != NULL; rti++) {
        for (i=0; rt[rti]->type_table[i].type != -1; i++);
        if (i > max_cfg_vals) {
            max_cfg_vals = i;
        }
    }

    /* allocate space for the configuration information */
    cfg_vals = (ValType *) malloc(max_cfg_vals * sizeof(ValType));
    if (cfg_vals == NULL) {
        goto fail;
    }

    cfgline = (char *)malloc(CFGLINESIZE);
    if (cfgline == NULL) {
        goto fail;
    }

    /* Open config file */
    if (OpenConfigFile(filename, FM_READ, &cfgp) != FS_OK) {
        goto fail;
    }

#ifndef SR_NO_COMMENTS_IN_CONFIG
    if (srUserCommentsInfo_head == NULL) {
        if (srInitializeCommentStructures(filename) == -1) {
            DPRINTF((APERROR,
                     "%s: Could not initialize user comment structures.\n",
                     Fname));
        }
    }
    curCommentEntry = srFindCommentEntry(filename);

    /* Free previous list of comments before reading in configuration file */
    if (curCommentEntry != NULL) {
        srFreeCommentLineEntries(curCommentEntry->comments_head_ptr,
                                 curCommentEntry->comments_tail_ptr);
        curCommentEntry->comments_head_ptr = NULL;
        curCommentEntry->comments_tail_ptr = NULL;
        cfgp->curCommentEntry = curCommentEntry;
    }
    else {
        newCommentEntry = srMallocCommentStructure(filename);
        if (newCommentEntry != NULL) {
            srAddCommentEntry(newCommentEntry);
            cfgp->curCommentEntry = newCommentEntry;
        }
        else {
            cfgp->curCommentEntry = NULL;
        }
    }
#endif /* SR_NO_COMMENTS_IN_CONFIG */

    /* Read records from config file */
    while (!sr_feof(cfgp)) {
        /* Read a record */
        if (!ReadConfigLine(cfgp, cfgline, CFGLINESIZE)) {
            continue;
        }

        /* Process it */
        for (rti = 0; rt[rti] != NULL; rti++) {
            if (!strncmp(cfgline, rt[rti]->label, strlen(rt[rti]->label))) {
                break;
            }
        }
        if (rt[rti] == NULL) {
            DPRINTF((APWARN, "Ignoring unknown record type: %s\n", cfgline));
            if (!configLogHeaderWritten) {
                 writeConfigLogHeader(logfilename, filename);
                 configLogHeaderWritten = 1;
            }
            write_config_log(logfilename, "PARSING_ERROR:  ");
            write_config_log(logfilename, cfgline);
            write_config_log(logfilename, "\n");
            status++;
            continue;
        }
        DPRINTF((APCONFIG, "Reading config recordtype %s at line %d\n",
                 rt[rti]->label, cfgp->linecount));
        cfgdata = cfgline + strlen(rt[rti]->label);
        len = strlen(cfgline) - 1;
        if ((rt[rti]->type & 0x00f0) == PARSER_FORCE_INDEX_ORDER) {
            st = ProcessConfigRecordForcingIndexOrder(cfgp->linecount,
                                                      cfgdata,
                                                      rt[rti]->type_table,
                                                      rt[rti]->converters,
                                                      cfg_vals);
        } else {
            st = ProcessConfigRecord(cfgp->linecount,
                                     cfgdata,
                                     rt[rti]->type_table,
                                     rt[rti]->converters,
                                     cfg_vals);
        }
        for (;len >= 0; len--) {
            if (cfgline[len] == '\0') {
                cfgline[len] = ' ';
            }
        }
        if (st) {
            if (!configLogHeaderWritten) {
                 writeConfigLogHeader(logfilename, filename);
                 configLogHeaderWritten = 1;
            }
            write_config_log(logfilename, "PARSING_ERROR:  ");
            write_config_log(logfilename, cfgline);
            write_config_log(logfilename, "\n");
            status++;
        }
        if (st == -1) {
            continue;
        }

        if ((rt[rti]->type & 0x000f) == PARSER_SCALAR) {
            FillInScalarVar(rt[rti], cfg_vals);
        } else
        if ((rt[rti]->type & 0x000f) == PARSER_TABLE) {
            if (CreateTableEntry(rt[rti], cfg_vals)) {
                if (!configLogHeaderWritten) {
                    writeConfigLogHeader(logfilename, filename);
                    configLogHeaderWritten = 1;
                }
                write_config_log(logfilename, "PARSING_ERROR:  ");
                write_config_log(logfilename, cfgline);
                write_config_log(logfilename, "\n");
                status++;
                FreeValTypes(rt[rti]->type_table, cfg_vals, -1);
            }
        } else {
            DPRINTF((APERROR, "%s: Bad PARSER_RECORD_TYPE\n", Fname));
            FreeValTypes(rt[rti]->type_table, cfg_vals, -1);
        }
    }

    /* Close config file */
    CloseConfigFile(cfgp);
    free(cfgline);
    free(cfg_vals);
    return 0;

  fail:
    CloseConfigFile(cfgp);

    if (cfg_vals != NULL) {
        free(cfg_vals);
    }
    if (cfgline != NULL) {
        free(cfgline);
    }
    return -1;
}

static void
FillInScalarVar(rtp, vp)
    const PARSER_RECORD_TYPE *rtp;
    ValType *vp;
{
    OctetString **os;
    OID **oid;
    UInt64 **u64;

    switch (rtp->type_table[0].type) {
        case INTEGER_TYPE:
            *(SR_INT32 *)(rtp->scalar) = vp[0].slval;
            break;
        case COUNTER_TYPE:
        case GAUGE_TYPE:
        case TIME_TICKS_TYPE:
            *(SR_UINT32 *)(rtp->scalar) = vp[0].ulval;
            break;
        case OCTET_PRIM_TYPE:
            os = (OctetString **)(rtp->scalar);
            if (*os != NULL) {
                FreeOctetString(*os);
            }
            *os = vp[0].osval;
            break;
        case OBJECT_ID_TYPE:
            oid = (OID **)(rtp->scalar);
            if (*oid != NULL) {
                FreeOID(*oid);
            }
            *oid = vp[0].oidval;
            break;
       case IP_ADDR_PRIM_TYPE:
         *(SR_UINT32 *)(rtp->scalar) = vp[0].ulval;
         break;
       case COUNTER_64_TYPE:
            u64 = (UInt64 **)(rtp->scalar);
            if (*u64 != NULL) {
                FreeUInt64(*u64);
            }
            *u64 = vp[0].uint64val;
	    break;
        default:
            break;
    }
}

/*
 * This routine will create a row in a table and populate it with the
 * contents of vp, as read from a config file.
 */
static int
CreateTableEntry(rtp, vp)
    const PARSER_RECORD_TYPE *rtp;
    ValType *vp;
{
    FNAME("CreateTableEntry")
    int index;
    void *entry;
    int i;
    int nitems;
    int offset;
    int maxOffset = -1;
    int indexOfMaxOffset = 0;	/* initialized to eliminate diagnostic */

    /* Plug in index fields */
    for (i = 0; rtp->type_table[i].type != -1; i++) {
        if (rtp->type_table[i].indextype != -1) {
            int indexno = rtp->type_table[i].indextype;
            switch (rtp->type_table[i].type) {
            case INTEGER_TYPE:
                rtp->table->tip[indexno].value.uint_val = vp[i].slval;
                break;
            case COUNTER_TYPE:
            case GAUGE_TYPE:
            case TIME_TICKS_TYPE:
                rtp->table->tip[indexno].value.uint_val = vp[i].ulval;
                break;
            case OCTET_PRIM_TYPE:
                rtp->table->tip[indexno].value.octet_val = vp[i].osval;
                break;
            case OBJECT_ID_TYPE:
                rtp->table->tip[indexno].value.oid_val = vp[i].oidval;
                break;
           case IP_ADDR_PRIM_TYPE:
                rtp->table->tip[indexno].value.uint_val = vp[i].ulval;
                break;
            }
        }
    }

    /* Create the new row */
    nitems = rtp->table->nitems;
    index = NewTableEntry(rtp->table);
    if (index == -1) {
        DPRINTF((APERROR, "%s: cannot create new row in table %s\n", Fname, rtp->label));
        return -1;
    }
    if (nitems == rtp->table->nitems) {
        DPRINTF((APERROR, "%s: attempt to create row that already exists in table: %s.\n",
                 Fname, rtp->label));
        return -1;
    }
    entry = (void *) rtp->table->tp[index];
    offset = 0;
   /*
    *  Stepping through the fields in the table entry, free the 
    *  index fields (they are copied into the row by InstallIndices()),
    *  and fill in the rest of the fields. 
    */ 
  
    for (i = 0; rtp->type_table[i].type != -1; i++) {
        offset = rtp->type_table[i].byteoffset;
        if (offset > maxOffset) {
           indexOfMaxOffset = i;
           maxOffset = rtp->type_table[i].byteoffset;
        }
        /* if we are an index, free the field, if it is a ptr */
        if (rtp->type_table[i].indextype != -1) {
            switch (rtp->type_table[i].type) {
                case OCTET_PRIM_TYPE:
                    FreeOctetString(vp[i].osval);
                    break;
                case OBJECT_ID_TYPE:
                    FreeOID(vp[i].oidval);
                    break;
                default:
                    break;
            }
        } else {
            /* 
             * Transfer config value to the table.  
             */
            switch (rtp->type_table[i].type) {
                case INTEGER_TYPE:
                    *(SR_INT32 *)((char *) entry + offset) = vp[i].slval;
                    break;
                case COUNTER_TYPE:
                case GAUGE_TYPE:
                case TIME_TICKS_TYPE:
                    *(SR_UINT32 *)((char *)entry + offset) = vp[i].ulval;
                    break;
                case OCTET_PRIM_TYPE:
                    *(OctetString **)((char *)entry + offset) = vp[i].osval;
                    break;
                case OBJECT_ID_TYPE:
                    *(OID **)((char *)entry + offset) = vp[i].oidval;
                    break;
	        case IP_ADDR_PRIM_TYPE:
                    *(SR_UINT32 *)((char *)entry + offset) = vp[i].ulval;
	            break;
  	        case COUNTER_64_TYPE:
                    *(UInt64 **)((char *)entry + offset) = vp[i].uint64val;
                    break;
                default:
                    break;
            }
        }
    }

    /*
     *  Clear out the rest of the table row.  Advance the
     *  place we start clearing beyond the end of the rightmost field.
     */
    switch (rtp->type_table[indexOfMaxOffset].type) {
        case INTEGER_TYPE:
            offset = maxOffset + sizeof(SR_INT32);
            break;
        case COUNTER_TYPE:
        case GAUGE_TYPE:
        case TIME_TICKS_TYPE:
            offset = maxOffset + sizeof(SR_UINT32);
            break;
        case OCTET_PRIM_TYPE:
            offset = maxOffset + sizeof(OctetString *);
            break;
        case OBJECT_ID_TYPE:
            offset = maxOffset + sizeof(OID *);
            break;
        case IP_ADDR_PRIM_TYPE:
            offset = maxOffset + sizeof(SR_UINT32);
            break;
        default:
            break;
    }
    if (offset < rtp->table->rowsize && offset != 0) {
        memset((char *)entry + offset, 0xff, 
            (int)(rtp->table->rowsize - offset));
    }

    return 0;
}


#ifndef SR_NO_WRITE_CONFIG_FILE
static int
WriteScalarEntry(cfgp, label, type_table, converters, scalar)
    ConfigFile *cfgp;
    const char *label;
    const SnmpType *type_table;
    const PARSER_CONVERTER *converters;
    void *scalar;
{
    FNAME("WriteScalarEntry")
    SR_INT32 cv;
    char *token;

    sr_fprintf(cfgp, label);

    switch (type_table->type) {
        case INTEGER_TYPE:
            if (*converters->conv_func == SR_INTEGER_TRANSLATION) {
                cv = ConvToken_integerTranslation(PARSER_CONVERT_TO_TOKEN,
                      &token, scalar,
                      (ParserIntegerTranslationTable *)converters->defval);
            } else {
                cv = (*converters->conv_func)(PARSER_CONVERT_TO_TOKEN,
                                              &token, scalar);
            }
            break;
        case COUNTER_TYPE:
        case GAUGE_TYPE:
        case TIME_TICKS_TYPE:
            cv = (*converters->conv_func)(PARSER_CONVERT_TO_TOKEN,
                                          &token, scalar);
            break;
        case OCTET_PRIM_TYPE:
            cv = (*converters->conv_func)(PARSER_CONVERT_TO_TOKEN,
                                          &token, scalar);
            break;
        case OBJECT_ID_TYPE:
            cv = (*converters->conv_func)(PARSER_CONVERT_TO_TOKEN,
                                          &token, scalar);
            break;
       case IP_ADDR_PRIM_TYPE:
            cv = (*converters->conv_func)(PARSER_CONVERT_TO_TOKEN,
                                         &token, scalar);
            break;
       case COUNTER_64_TYPE:
            cv = (*converters->conv_func)(PARSER_CONVERT_TO_TOKEN,
                                         &token, scalar);
            break;
        default:
            DPRINTF((APWARN, "%s: unsupported token type %d\n",
                              Fname, type_table->type));
            goto fail;
    }
    if (cv != 0) {
        DPRINTF((APWARN, "%s: could not make token for %s\n",
                 Fname, label ? label : "<NULL>"));
        goto fail;
    }

    sr_fprintf(cfgp, " ");
    sr_fprintf(cfgp, token);
    sr_fprintf(cfgp, "\n");
    free(token);
    return 0;

  fail:
    sr_fprintf(cfgp, "\n");
    return -1;
}

static int
WriteTableEntry(cfgp, label, type_table, converters, entry,
                rs_offset, st_offset, us_offset)
    ConfigFile *cfgp;
    const char *label;
    const SnmpType *type_table;
    const PARSER_CONVERTER *converters;
    char *entry;
    SR_INT32 rs_offset;
    SR_INT32 st_offset;
    SR_INT32 us_offset;
{
    FNAME("WriteTableEntry")
    int nc, sl;
    SR_INT32 cv = 0;
    char *token = NULL;
    const SnmpType *tt;
    const PARSER_CONVERTER *pc;

    /* 
     *  Offset checks:
     *    rs_offset: row status offset.  Do not write if row status is
     *               not 1 (RS_ACTIVE)
     *    st_offset: storage type offset.  Do not write if storage type
     *               not SR_NONVOLATILE, SR_PERMANATE or SR_READONLY.
     *    us_offset: user skip offset.  Do not write if user_skip is nonzero.
     */

    if (rs_offset >= 0) {
        if ((*(SR_INT32 *)(entry + rs_offset)) != 1) {
            return 0;
        }
    }

    if (st_offset >= 0) {
        if ((*(SR_INT32 *)(entry + st_offset)) < 3) {
            return 0;
        }
    }

    if (us_offset >= 0) {
        if ((*(SR_INT32 *)(entry + us_offset)) > 0) {
            return 0;
        }
    }

    sr_fprintf(cfgp, label);

    nc = strlen(label);
    for (pc = converters, tt = type_table; pc->conv_func != NULL; pc++, tt++) {

        switch (tt->type) {
            case INTEGER_TYPE:
                if ((pc->defval == NULL) ||
                    (strcmp(pc->defval, "default:2048") == 0) ||
                    ((strncmp(pc->defval, "optional:default", strlen("optional:default")) == 0) &&
                     (strlen(pc->defval) > (strlen("optional:default")+1)))) {
                    cv = (*pc->conv_func)(PARSER_CONVERT_TO_TOKEN,
                          &token, (void *)(entry + tt->byteoffset));
                } else if (pc->conv_func == SR_INTEGER_TRANSLATION) {
                    cv = ConvToken_integerTranslation(PARSER_CONVERT_TO_TOKEN,
                          &token, (void *)(entry + tt->byteoffset),
                          (ParserIntegerTranslationTable *)pc->defval);
                }
                break;
            case COUNTER_TYPE:
            case GAUGE_TYPE:
            case TIME_TICKS_TYPE:
                if ((pc->defval == NULL) ||
                    ((strncmp(pc->defval, "optional:default", strlen("optional:default")) == 0) &&
                     (strlen(pc->defval) > (strlen("optional:default")+1)))) {
                    cv = (*pc->conv_func)(PARSER_CONVERT_TO_TOKEN,
                          &token, (void *)(entry + tt->byteoffset));
                }
                break;
            case OCTET_PRIM_TYPE:
                if ((pc->defval == NULL) ||
                    ((strncmp(pc->defval, "optional:default", strlen("optional:default")) == 0) &&
                     (strlen(pc->defval) > (strlen("optional:default")+1)))) {
                    cv = (*pc->conv_func)(PARSER_CONVERT_TO_TOKEN,
                          &token, (void *)(entry + tt->byteoffset));
                }
                break;
            case OBJECT_ID_TYPE:
                if (pc->defval == NULL) {
                    cv = (*pc->conv_func)(PARSER_CONVERT_TO_TOKEN,
                          &token, (void *)(entry + tt->byteoffset));
                }
                break;

	    case IP_ADDR_PRIM_TYPE:
                if (pc->defval == NULL) {
                    cv = (*pc->conv_func)(PARSER_CONVERT_TO_TOKEN,
                          &token, (void *)(entry + tt->byteoffset));
                }
                break;


	    case COUNTER_64_TYPE:
                if (pc->defval == NULL) {
                    cv = (*pc->conv_func)(PARSER_CONVERT_TO_TOKEN,
                          &token, (void *)(entry + tt->byteoffset));
                }
		break;

            default:
                DPRINTF((APWARN, "%s: unsupported token type %d\n",
                                  Fname, tt->type));
                goto fail;
        }
        if (cv != 0) {
            DPRINTF((APWARN, "%s: error parsing token\n", Fname));
            if (token != NULL) {
                free(token);
                token = NULL;
            }
            goto fail;
        }
        if ((pc->defval != NULL) &&
            (pc->conv_func != SR_INTEGER_TRANSLATION)) {
            if ( (strcmp(pc->defval, "default:2048")) &&
                 (strncmp(pc->defval, "optional:default",
                          strlen("optional:default")) || 
                  (strlen(pc->defval) < (strlen("optional:default")+1))) ) {
                continue;
            }
        }

        sl = strlen(token);
        if ((nc + sl) > 77) {
            sr_fprintf(cfgp, " \\\n    ");
            sr_fprintf(cfgp, token);
            nc = sl + 4;
        } else {
            sr_fprintf(cfgp, " ");
            sr_fprintf(cfgp, token);
            nc += sl + 1;
        }
        free(token);
        token = NULL;
    }
    sr_fprintf(cfgp, "\n");
    return 0;

  fail:
    sr_fprintf(cfgp, "\n");
    return -1;
}

static int
WriteTableEntryForcingIndexOrder(
    ConfigFile *cfgp,
    const char *label,
    const SnmpType *type_table,
    const PARSER_CONVERTER *converters,
    char *entry,
    SR_INT32 rs_offset,
    SR_INT32 st_offset,
    SR_INT32 us_offset)
{
    FNAME("WriteTableEntry")
    int nc, sl;
    SR_INT32 cv = 0;
    char *token;
    const SnmpType *tt;
    const PARSER_CONVERTER *pc;
    int cur_idx = 0, found_index = 1;

    /* 
     *  Offset checks:
     *    rs_offset: row status offset.  Do not write if row status is
     *               not 1 (RS_ACTIVE)
     *    st_offset: storage type offset.  Do not write if storage type
     *               not SR_NONVOLATILE, SR_PERMANENT or SR_READONLY.
     *    us_offset: user skip offset.  Do not write if user_skip is nonzero.
     */

    if (rs_offset >= 0) {
        if ((*(SR_INT32 *)(entry + rs_offset)) != 1) {
            return 0;
        }
    }

    if (st_offset >= 0) {
        if ((*(SR_INT32 *)(entry + st_offset)) < 3) {
            return 0;
        }
    }

    if (us_offset >= 0) {
        if ((*(SR_INT32 *)(entry + us_offset)) > 0) {
            return 0;
        }
    }

    sr_fprintf(cfgp, label);

    nc = strlen(label);

    while (found_index) {
        found_index = 0;
        for (pc = converters, tt = type_table;
             pc->conv_func != NULL; pc++, tt++) {

            if (tt->indextype != cur_idx) {
                continue;
            }
            found_index = 1;
            cur_idx++;

            switch (tt->type) {
                case INTEGER_TYPE:
                    if ((pc->defval == NULL) ||
                        (strcmp(pc->defval, "default:2048") == 0) ||
                        ((strncmp(pc->defval, "optional:default", 
                                 strlen("optional:default")) == 0) && 
                         (strlen(pc->defval) > (strlen("optional:default") + 1))) ) {
                        cv = (*pc->conv_func)(PARSER_CONVERT_TO_TOKEN,
                              &token, (void *)(entry + tt->byteoffset));
                    } else if (pc->conv_func == SR_INTEGER_TRANSLATION) {
                        cv = ConvToken_integerTranslation(
                              PARSER_CONVERT_TO_TOKEN,
                              &token, (void *)(entry + tt->byteoffset),
                              (ParserIntegerTranslationTable *)pc->defval);
                    }
                    break;
                case COUNTER_TYPE:
                case GAUGE_TYPE:
                case TIME_TICKS_TYPE:
                    if (pc->defval == NULL) {
                        cv = (*pc->conv_func)(PARSER_CONVERT_TO_TOKEN,
                              &token, (void *)(entry + tt->byteoffset));
                    }
                    break;
                case OCTET_PRIM_TYPE:
                    if ((pc->defval == NULL) ||
                        ((strncmp(pc->defval, "optional:default", 
                                 strlen("optional:default")) == 0) && 
                        (strlen(pc->defval) > (strlen("optional:default")+1)))) {
                        cv = (*pc->conv_func)(PARSER_CONVERT_TO_TOKEN,
                              &token, (void *)(entry + tt->byteoffset));
                    }
                    break;
                case OBJECT_ID_TYPE:
                    if (pc->defval == NULL) {
                        cv = (*pc->conv_func)(PARSER_CONVERT_TO_TOKEN,
                              &token, (void *)(entry + tt->byteoffset));
                    }
                    break;
               case IP_ADDR_PRIM_TYPE:
                    if (pc->defval == NULL) {
                        cv = (*pc->conv_func)(PARSER_CONVERT_TO_TOKEN,
                              &token, (void *)(entry + tt->byteoffset));
                    }
                   break;
                default:
                    DPRINTF((APWARN, "%s: unsupported token type %d\n",
                                      Fname, tt->type));
                    goto fail;
            }
            if (cv != 0) {
                DPRINTF((APWARN, "%s: error parsing token\n", Fname));
                goto fail;
            }
            if ((pc->defval != NULL) &&
                (pc->conv_func != SR_INTEGER_TRANSLATION)) {
                if ( (strcmp(pc->defval, "default:2048")) &&
                     (strncmp(pc->defval, "optional:default",
                              strlen("optional:default")) || 
                      (strlen(pc->defval) < (strlen("optional:default")+1))) ) {
                    continue;
                }
            }

            sl = strlen(token);
            if ((nc + sl) > 77) {
                sr_fprintf(cfgp, " \\\n    ");
                sr_fprintf(cfgp, token);
                nc = sl + 4;
            } else {
                sr_fprintf(cfgp, " ");
                sr_fprintf(cfgp, token);
                nc += sl + 1;
            }
            free(token);
            token = NULL;
        }
    }

    for (pc = converters, tt = type_table; pc->conv_func != NULL; pc++, tt++) {
        if (tt->indextype != -1) {
            continue;
        }

        switch (tt->type) {
            case INTEGER_TYPE:
                if ((pc->defval == NULL) ||
                    (strcmp(pc->defval, "default:2048") == 0) ||
                    ((strncmp(pc->defval, "optional:default", 
                             strlen("optional:default")) == 0) &&
                     (strlen(pc->defval) > (strlen("optional:default")+1))) ) {
                    cv = (*pc->conv_func)(PARSER_CONVERT_TO_TOKEN,
                          &token, (void *)(entry + tt->byteoffset));
                } else if (pc->conv_func == SR_INTEGER_TRANSLATION) {
                    cv = ConvToken_integerTranslation(PARSER_CONVERT_TO_TOKEN,
                          &token, (void *)(entry + tt->byteoffset),
                          (ParserIntegerTranslationTable *)pc->defval);
                }
                break;
            case COUNTER_TYPE:
            case GAUGE_TYPE:
            case TIME_TICKS_TYPE:
                if (pc->defval == NULL) {
                    cv = (*pc->conv_func)(PARSER_CONVERT_TO_TOKEN,
                          &token, (void *)(entry + tt->byteoffset));
                }
                break;
            case OCTET_PRIM_TYPE:
                if ((pc->defval == NULL) ||
                    ((strncmp(pc->defval, "optional:default", 
                             strlen("optional:default")) == 0) &&
                     (strlen(pc->defval) > (strlen("optional:default")+1)))) {
                    cv = (*pc->conv_func)(PARSER_CONVERT_TO_TOKEN,
                          &token, (void *)(entry + tt->byteoffset));
                }
                break;
            case OBJECT_ID_TYPE:
                if (pc->defval == NULL) {
                    cv = (*pc->conv_func)(PARSER_CONVERT_TO_TOKEN,
                          &token, (void *)(entry + tt->byteoffset));
                }
                break;
           case IP_ADDR_PRIM_TYPE:
                if (pc->defval == NULL) {
                    cv = (*pc->conv_func)(PARSER_CONVERT_TO_TOKEN,
                          &token, (void *)(entry + tt->byteoffset));
                }
                break;
            default:
                DPRINTF((APWARN, "%s: unsupported token type %d\n",
                                  Fname, tt->type));
                goto fail;
        }
        if (cv != 0) {
            DPRINTF((APWARN, "%s: error parsing token\n", Fname));
            goto fail;
        }
        if ((pc->defval != NULL) &&
            (pc->conv_func != SR_INTEGER_TRANSLATION)) {
            if ( (strcmp(pc->defval, "default:2048")) &&
                 (strncmp(pc->defval, "optional:default",
                          strlen("optional:default")) || 
                  (strlen(pc->defval) < (strlen("optional:default")+1))) ) {
                continue;
            }
        }

        sl = strlen(token);
        if ((nc + sl) > 77) {
            sr_fprintf(cfgp, " \\\n    ");
            sr_fprintf(cfgp, token);
            nc = sl + 4;
        } else {
            sr_fprintf(cfgp, " ");
            sr_fprintf(cfgp, token);
            nc += sl + 1;
        }
        free(token);
        token = NULL;
    }

    sr_fprintf(cfgp, "\n");
    return 0;

  fail:
    sr_fprintf(cfgp, "\n");
    return -1;
}

int
WriteConfigFile(filename, rt)
    const char *filename;
    const PARSER_RECORD_TYPE *rt[];
{
    FNAME("WriteConfigFile")
    ConfigFile *cfgp = NULL;
    char *backup_filename;
    int rti;
    int status;
    int i;
    const SnmpType *st;
    const PARSER_CONVERTER *ct;
#ifndef SR_NO_COMMENTS_IN_CONFIG
    srCommentLine_t *curLine;
    stableConfigFileInfo_t *curCommentEntry = NULL;
#endif /* SR_NO_COMMENTS_IN_CONFIG */

    char *fprefix;
#ifdef EIGHT_PLUS_THREE
    char *p;
#endif /* EIGHT_PLUS_THREE */

    backup_filename = (char *)malloc(strlen(filename)
                                     + strlen(BACKUP_FILE_SUFFIX)
                                     + 8);

    if (backup_filename == NULL) {
        DPRINTF((APERROR, "%s: malloc error\n", Fname));
        goto fail;
    }
    strcpy(backup_filename, filename);
#ifdef EIGHT_PLUS_THREE
    if ((p = strrchr(backup_filename, '.')) != NULL) {
        *p = '\0';
    }
#endif	/* EIGHT_PLUS_THREE */
    strcat(backup_filename, BACKUP_FILE_SUFFIX);

    if (CreateBackupFile(filename, backup_filename) == 0) {
        DPRINTF((APTRACE, "%s: Warning, cannot backup config file.\n", Fname));
        free(backup_filename);
        backup_filename = NULL;
    }

    /* Open config file */
    if (OpenConfigFile(filename, FM_WRITE, &cfgp) != FS_OK) {
        goto restore_and_fail;
    }

#ifndef SR_NO_COMMENTS_IN_CONFIG
    curCommentEntry = srFindCommentEntry(filename);
    /* 
     * Print out any user defined comment lines first. All
     * user defined comment lines start with ##.
     */
    if (curCommentEntry != NULL) {
        for (curLine = curCommentEntry->comments_head_ptr; curLine != NULL; 
             curLine = curLine->next_ptr) {
            sr_fprintf(cfgp, curLine->comment_line);
            sr_fprintf(cfgp, "\n");
        }
        sr_fprintf(cfgp, "\n");
    }
#endif /* SR_NO_COMMENTS_IN_CONFIG */

    status = 0;
    for (rti = 0; rt[rti] != NULL; rti++) {
#ifndef SR_NO_COMMENTS_IN_CONFIG
        if (rt[rti]->comment == NULL) {
            sr_fprintf(cfgp, "# Entry type: ");
            sr_fprintf(cfgp, rt[rti]->label);
            sr_fprintf(cfgp, "\n");
            sr_fprintf(cfgp, "# Entry format: ");
            fprefix = "";
            for (st = rt[rti]->type_table, ct = rt[rti]->converters; 
                st->type != -1; st++, ct++) {
                if (ct->defval != NULL) {
                    continue;
                }
                switch (st->type) {
                    case INTEGER_TYPE:
                        sr_fprintf(cfgp, fprefix);
                        sr_fprintf(cfgp, "integer\n");
                        break;
                    case COUNTER_TYPE:
                        sr_fprintf(cfgp, fprefix);
                        sr_fprintf(cfgp, "counter\n");
                        break;
                    case GAUGE_TYPE:
                        sr_fprintf(cfgp, fprefix);
                        sr_fprintf(cfgp, "gauge\n");
                        break;
                    case TIME_TICKS_TYPE:
                        sr_fprintf(cfgp, fprefix);
                        sr_fprintf(cfgp, "timeTicks\n");
                        break;
                    case OCTET_PRIM_TYPE:
                        sr_fprintf(cfgp, fprefix);
                        sr_fprintf(cfgp, "octetString\n");
                        break;
                    case OBJECT_ID_TYPE:
                        sr_fprintf(cfgp, fprefix);
                        sr_fprintf(cfgp, "OID\n");
                        break;
                    case IP_ADDR_PRIM_TYPE:
                        sr_fprintf(cfgp, fprefix);
                        sr_fprintf(cfgp, "ipAddress\n");
                        break;
                    case COUNTER_64_TYPE:
                        sr_fprintf(cfgp, fprefix);
                        sr_fprintf(cfgp, "counter64\n");
                        break;
                }
                fprefix = "#               ";
            }
        } else {
            sr_fprintf(cfgp, rt[rti]->comment);
            sr_fprintf(cfgp, "\n");
        }
#endif	/* SR_NO_COMMENTS_IN_CONFIG */
        if ((rt[rti]->type & 0x000f) == PARSER_SCALAR) {
            status = WriteScalarEntry(cfgp,
                                      rt[rti]->label,
                                      rt[rti]->type_table,
                                      rt[rti]->converters,
                                      (char *)rt[rti]->scalar);
        } else
        if ((rt[rti]->type & 0x000f) == PARSER_TABLE) {
            for (i = 0; i < rt[rti]->table->nitems; i++) {
                if ((rt[rti]->type & 0x00f0) == PARSER_FORCE_INDEX_ORDER) {
                    status = WriteTableEntryForcingIndexOrder(cfgp,
                                             rt[rti]->label,
                                             rt[rti]->type_table,
                                             rt[rti]->converters,
                                             (char *)rt[rti]->table->tp[i],
                                             rt[rti]->rs_offset,
                                             rt[rti]->st_offset,
                                             rt[rti]->us_offset);
                } else {
                    status = WriteTableEntry(cfgp,
                                             rt[rti]->label,
                                             rt[rti]->type_table,
                                             rt[rti]->converters,
                                             (char *)rt[rti]->table->tp[i],
                                             rt[rti]->rs_offset,
                                             rt[rti]->st_offset,
                                             rt[rti]->us_offset);
                }
            }
        } else {
            DPRINTF((APERROR, "%s: Bad PARSER_RECORD_TYPE\n", Fname));
            status = -1;
        }
        if (status) {
            goto restore_and_fail;
        }
        sr_fprintf(cfgp, "\n\n");
    }

    CloseConfigFile(cfgp);

    if (backup_filename) {
        free(backup_filename);
    }
    return 0;

  restore_and_fail:
    if (backup_filename) {
        if (CreateBackupFile(backup_filename, filename) == 0) {
            DPRINTF((APERROR, "%s: WARNING, CANNOT RESTORE OLD CONFIG FILE\n",
                     Fname));
        }
    } else {
        DPRINTF((APERROR, "%s: WARNING, CANNOT RESTORE OLD CONFIG FILE\n",
                 Fname));
    }

  fail:
    CloseConfigFile(cfgp);
    if (backup_filename) {
        free(backup_filename);
    }
    return -1;
}

/*
 * CreateBackupFile:  copies filename to backup_filename, using rename if the
 * system supports it 
 */
int
CreateBackupFile(filename, backup_filename)
  const char *filename;
  const char *backup_filename;
{
    FNAME("BackupFile")
    struct stat buf;
    int fd;

    if (stat(filename, &buf) != 0) {
	/* Should not happen. Be conservative. */
	buf.st_mode = S_IRUSR|S_IWUSR; 
    }
    if (rename(filename, backup_filename)) {
       DPRINTF((APTRACE, "%s:  Warning, cannot backup file %s: %s\n",
                Fname, filename, sys_errname(errno)));
        return 0;
    }
    if ((fd = creat(filename, buf.st_mode)) != -1) {
	close(fd);
    } else {
        DPRINTF((APWARN, "%s: Error creating %s with mode 0x%x: %s\n",
                Fname, filename, (unsigned int) buf.st_mode, 
                sys_errname(errno)));
    }
    return 1;
}
#endif /* SR_NO_WRITE_CONFIG_FILE */


/*
 * ParseScalerinConfigFile
 *
 * this routine parses a scaler object stored in the config file
 * then fill the value of mached scaler object in PARSER_RECORD_TYPE
 * struct.
 *
 * return value: success : 0
 *               fail    : -1
 */

int
ParseScalerinConfigFile(filename, rt)
    const char *filename;
    const PARSER_RECORD_TYPE *rt[];
{
    ConfigFile *cfgp = NULL;
    char *cfgline = NULL, *cfgdata = NULL;
    int rti;
    int st;
    ValType *cfg_vals = NULL;
    int i, max_cfg_vals;
    int len;


    /* Determine what the size of cfg_vals should be */
    max_cfg_vals = 0;
    for (rti = 0; rt[rti] != NULL; rti++) {
        for (i=0; rt[rti]->type_table[i].type != -1; i++);
        if (i > max_cfg_vals) {
            max_cfg_vals = i;
        }
    }

    /* allocate space for the configuration information */
    cfg_vals = (ValType *) malloc(max_cfg_vals * sizeof(ValType));
    if (cfg_vals == NULL) {
        goto fail;
    }

    cfgline = (char *)malloc(CFGLINESIZE);
    if (cfgline == NULL) {
        goto fail;
    }

    /* Open config file */
    if (OpenConfigFile(filename, FM_READ, &cfgp) != FS_OK) {
        goto fail;
    }

    /* Read records from config file */
    while (!sr_feof(cfgp)) {
        /* Read a record */
        if (!ReadConfigLine(cfgp, cfgline, CFGLINESIZE)) {
            continue;
        }

        /* Process it */
        for (rti = 0; rt[rti] != NULL; rti++) {
            if (!strncmp(cfgline, rt[rti]->label, strlen(rt[rti]->label))) {
                break;
            }
        }
        if (rt[rti] == NULL) {
            continue;
        }
        cfgdata = cfgline + strlen(rt[rti]->label);
        len = strlen(cfgline) - 1;

        st = ProcessConfigRecord(cfgp->linecount,
                                     cfgdata,
                                     rt[rti]->type_table,
                                     rt[rti]->converters,
                                     cfg_vals);
        for (;len >= 0; len--) {
            if (cfgline[len] == '\0') {
                cfgline[len] = ' ';
            }
        }
        if (st == -1) {
            continue;
        }

        if ((rt[rti]->type & 0x000f) == PARSER_SCALAR) {
            FillInScalarVar(rt[rti], cfg_vals);
        } 
    }

    /* Close config file */
    CloseConfigFile(cfgp);
    free(cfgline);
    free(cfg_vals);
    return 0;

  fail:
    CloseConfigFile(cfgp);

    if (cfg_vals != NULL) {
        free(cfg_vals);
    }
    if (cfgline != NULL) {
        free(cfgline);
    }
    return -1;
}

#ifndef SR_NO_COMMENTS_IN_CONFIG
/*
 * srInitializeCommentStructures:
 *
 *     Initialize the global structures, srUserCommentsInfo_head and
 * srUserCommentsInfo_tail. This includes mallocs and setting pointers to NULL.
 *
 * Returns:
 *	-1 - error
 *      -2 - already initialized
 *       0 - success
 */
static SR_INT32
srInitializeCommentStructures(const char *filename)
{
    FNAME("srInitializeCommentStructures")
    SR_INT32 returnVal = 0;
    SR_INT32 filenameLength = 0;

    if (srUserCommentsInfo_head != NULL) {
        return(-2);
    }          
    if (filename == NULL) {
        return(-1);
    }          
    
    srUserCommentsInfo_head = 
          (stableConfigFileInfo_t *)malloc(sizeof(stableConfigFileInfo_t));

    if (srUserCommentsInfo_head == NULL) {
        DPRINTF((APERROR, "%s: Could not malloc srUserCommentsInfo_head ptr.\n",
                 Fname));
        returnVal = -1;
        goto done;
    }          

    srUserCommentsInfo_head->filename = NULL;
    srUserCommentsInfo_head->comments_head_ptr = NULL;
    srUserCommentsInfo_head->comments_tail_ptr = NULL;
    srUserCommentsInfo_head->next_ptr = NULL;

    filenameLength = strlen(filename);
    srUserCommentsInfo_head->filename = (char *)malloc(filenameLength + 1);
    if (srUserCommentsInfo_head->filename == NULL) {
        DPRINTF((APERROR, "%s: Could not malloc filename ptr.\n", Fname));
        returnVal = -1;
        goto done;
    }          
    memset(srUserCommentsInfo_head->filename, 0, filenameLength + 1);
    memcpy(srUserCommentsInfo_head->filename, filename, filenameLength);

    srUserCommentsInfo_tail = srUserCommentsInfo_head;

done:

    if (returnVal == -1) {
        if (srUserCommentsInfo_head != NULL) {
            if (srUserCommentsInfo_head->filename != NULL) {
                free(srUserCommentsInfo_head->filename);
                srUserCommentsInfo_head->filename = NULL;
            }
            free(srUserCommentsInfo_head);
            srUserCommentsInfo_head = NULL;
        }
    }
  
    return (returnVal); 
}

/*
 * srMallocCommentStructure:
 *	This routine mallocs a new stableConfigFileInfo_t structure, and
 * then mallocs and fills in the filename element of the structure with
 * the passed in parameter, filename.
 *
 * Returns: NULL - failure
 *          pointer to struct - success
 *
 */
static stableConfigFileInfo_t *
srMallocCommentStructure(const char *filename)
{
    FNAME("srMallocCommentStructure")
    stableConfigFileInfo_t *newCommentEntry = NULL;
    SR_INT32 filenameLength;

    if (filename == NULL) {
        return NULL;
    }
    newCommentEntry = 
          (stableConfigFileInfo_t *)malloc(sizeof(stableConfigFileInfo_t));

    if (newCommentEntry == NULL) {
        DPRINTF((APERROR, "%s: Could not malloc newCommentEntry ptr.\n",
                 Fname));
        goto fail;
    }          

    newCommentEntry->filename = NULL;
    newCommentEntry->comments_head_ptr = NULL;
    newCommentEntry->comments_tail_ptr = NULL;
    newCommentEntry->next_ptr = NULL;

    filenameLength = strlen(filename);
    newCommentEntry->filename = (char *)malloc(filenameLength + 1);
    if (newCommentEntry->filename == NULL) {
        DPRINTF((APERROR, "%s: Could not malloc filename ptr.\n", Fname));
        goto fail;
    }          
    memset(newCommentEntry->filename, 0, filenameLength + 1);
    memcpy(newCommentEntry->filename, filename, filenameLength);

    return (newCommentEntry);

fail:
    if (newCommentEntry != NULL) {
        if (newCommentEntry->filename != NULL) {
            free(newCommentEntry->filename);
        }
        free(newCommentEntry);
        newCommentEntry = NULL;
    }
    return (newCommentEntry);
}

/* 
 * srAddCommentEntry:
 *
 *    Add the passed in entry, comment_ptr, to the linked list of 
 * stableConfigFileInfo_t structures we have. Each structure is a linked
 * list of user defined comments read in from a different config file. This 
 * linked list will then be written out at the top of the specified config 
 * file when WriteConfigFile() is called.
 *
 * Returns: -1 - error
 *           0 - success
 */
static SR_INT32
srAddCommentEntry(stableConfigFileInfo_t *comment_ptr)
{

    if (comment_ptr == NULL) {
        return (-1);
    }
    /* First comment structure */
    /* Set both head and tail pointers to passed in structure */
    if (srUserCommentsInfo_head == NULL) {
        srUserCommentsInfo_head = comment_ptr; 
        srUserCommentsInfo_head->next_ptr = NULL;
        srUserCommentsInfo_tail = srUserCommentsInfo_head; 
    }
    /* Should never happen */
    else if (srUserCommentsInfo_tail == NULL) {
        return (-1);
    }
    else {
        srUserCommentsInfo_tail->next_ptr = comment_ptr;
        srUserCommentsInfo_tail = comment_ptr;
        srUserCommentsInfo_tail->next_ptr = NULL;
    }

    return (0);
}

/* 
 * srAddCommentLineEntry:
 *
 *    Add the passed in comment line, line, to the linked list of user
 * defined comments read in from the config file. This linked list will
 * then be written out at the top of the config file when WriteConfigFile()
 * is called.
 *
 * Returns: -1 - error
 *           0 - success
 */
static SR_INT32
srAddCommentLineEntry(stableConfigFileInfo_t *comment_ptr, char *line)
{

    srCommentLine_t *new_line = NULL;
    SR_INT32 length;
    SR_INT32 returnVal = 0;

    if ( (line == NULL) || (comment_ptr == NULL) ) {
        returnVal = -1;
        goto done; 
    }
    length = strlen(line);

    new_line = (srCommentLine_t *) malloc(sizeof(srCommentLine_t));
    if (new_line == NULL) {
        DPRINTF((APERROR, 
                 "srAddCommentLineEntry: Could not malloc comment line entry.\n"));
        returnVal = -1;
        goto done; 
    }
    new_line->comment_line = (char *) malloc(length + 2);
    if (new_line->comment_line == NULL) {
        DPRINTF((APERROR, 
                 "srAddCommentLineEntry: Could not malloc comment line ptr.\n"));
        free(new_line);
        returnVal = -1;
        goto done; 
    }
    memset(new_line->comment_line, 0, length + 2);
    memcpy(new_line->comment_line, line, length);
    new_line->next_ptr = NULL;

    /* First comment line */
    if (comment_ptr->comments_head_ptr == NULL) {
        comment_ptr->comments_head_ptr = new_line;
        comment_ptr->comments_tail_ptr = new_line;
        comment_ptr->comments_tail_ptr->next_ptr = NULL;
    }
    /* Should not happen */
    else if (comment_ptr->comments_tail_ptr == NULL) {
        free(new_line->comment_line);
        free(new_line);
        returnVal = -1;
        goto done; 
    }
    else {
        comment_ptr->comments_tail_ptr->next_ptr = new_line;
        comment_ptr->comments_tail_ptr = new_line;
        comment_ptr->comments_tail_ptr->next_ptr = NULL;
    }

done:
    return (returnVal);
}

/* 
 * srFreeCommentLineEntries:
 *
 *     Removes and frees all entries in the user defined comment linked list
 * from the passed in pointer, head_ptr.
 *
 * Returns: -1 - error
 *           0 - success
 */
static void
srFreeCommentLineEntries(srCommentLine_t *head_ptr, srCommentLine_t *tail_ptr)
{
    srCommentLine_t *curLine, *nextLine;

    curLine = head_ptr;
    while (curLine != NULL) {
        nextLine = curLine->next_ptr;
        free(curLine->comment_line);
        free(curLine);
        curLine = nextLine; 
    }
    head_ptr = NULL;
    tail_ptr = NULL;
}

/*
 * srFindCommentEntry:
 *
 *    Search for the filename, filename, in the linked list of structures
 * defined for each active configuration file. If we find an entry where
 * the filename matches our parameter, return a pointer to that structure.
 * If no match is found, return NULL.
 *
 * Returns:
 *         pointer to matching struct - success
 *         NULL - failure
 *
 */
static stableConfigFileInfo_t *
srFindCommentEntry(const char *filename)
{
    stableConfigFileInfo_t *curEntry = NULL;
    SR_INT32 len1, len2;

    if (filename == NULL) {
        return (curEntry); 
    }

    for (curEntry = srUserCommentsInfo_head; curEntry != NULL; 
         curEntry = curEntry->next_ptr) {
        if (curEntry->filename != NULL) {
            len1 = strlen(curEntry->filename);    
            len2 = strlen(filename);    
            /* Found a match */
            if ( (len1 == len2) && 
                 (memcmp(filename, curEntry->filename, len1) == 0) ) {
                break;
            }
        }
    }
    
    return (curEntry);
}
#endif /* SR_NO_COMMENTS_IN_CONFIG */
#endif /* SR_CONFIG_FP */
