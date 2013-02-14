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

#include "sr_conf.h"


#include <stdio.h>

#include <string.h>



#include <ctype.h>

#include <malloc.h>


#include <stdlib.h>

#ifdef WANT_SSCANF_PROTO
extern int sscanf();
#endif	/* WANT_SSCANF_PROTO */

#include "sr_snmp.h"
#include "sr_cfg.h"
#include "lookup.h"
#include "v2table.h"
#include "scan.h"
#include "mib_tree.h"
#include "oid_lib.h"
#include "diag.h"
SR_FILENAME


int ReadMIBFileEntry(ConfigFile *cfgp, OID_TREE_ELE **otepp);

extern MIB_TREE       *default_mib_tree;

/*
 * NewMIBFromFile:
 *
 * This is provided for compatibility.  It first frees all MIB trees
 * attached to the default MIB tree, and then calls ReadMIBFile with
 * a NULL for mib_tree and SR_COL_REPLACE for collision.
 */
int
NewMIBFromFile(filename)
    const char *filename;
{
    MIB_TREE *kill_tree;
    OID oid;
    SR_UINT32 sid;

    while (default_mib_tree != NULL) {
        kill_tree = default_mib_tree;
        default_mib_tree = default_mib_tree->next;
        FreeMIBTree(kill_tree);
    }

    oid.length = 1;
    oid.oid_ptr = &sid;
    sid = 1;
    default_mib_tree = CreateMIBTree("default", &oid);
    if (default_mib_tree == NULL) {
        return -1;
    }
    return MergeMIBFromFile(filename);
}

/*
 * MergeMIBFromFile:
 *
 * This is provided for compatibility.  It calls ReadMIBFile with
 * a NULL for mib_tree and SR_COL_REPLACE for collision.
 */
int
MergeMIBFromFile(filename)
    const char *filename;
{
    return ReadMIBFile(filename, NULL, SR_COL_REPLACE);
}

/* this structure moves some local state variables off of the stack and into
 * a malloc-ed data structure.  It helps prevent stack overflows during the
 * variable addition process, which is recursive. */
typedef struct MIBFromFileInfoSt {
    char            buf[1024];
    char            name[255];
    char            number[255];
    char            type[32];
    char            access[32];
    char            size_range[255];
    char            enum_name[255];
    char            index_name[255];
} MIBFromFileInfo;

/*
 * ReadMIBFileEntry:
 *
 * This function reads entries from a MIB file, and returns an OID_TREE_ELE
 * structure in the otepp parameter.  The otepp parameter should contain a
 * pointer to the previously read entry, or NULL if this is the first
 * call during processing of a particular file.
 *
 * There are currently three types of entries which can be read.  The first
 * type is a new MIB variable definition, in which case the context of otepp
 * are discarded and a new OID_TREE_ELE structure is returned.  The second
 * and third types of entries are lists of integer/english enumerations,
 * and lists of index variables.  When these are encountered, they are
 * simply filled into the current OID_TREE_ELE pointed to by otepp.
 *
 * The return value is 0 on success, -1 on failure, and 1 on EOF.
 */
#define RMFE_NONE    0
#define RMFE_COMMENT 1
#define RMFE_ENUMER  2
#define RMFE_INDEX   3
int
ReadMIBFileEntry(cfgp, otepp)
    ConfigFile *cfgp;
    OID_TREE_ELE **otepp;
{
    OID_TREE_ELE *otep = *otepp;
    static MIBFromFileInfo *mffip = NULL;
    int context = RMFE_NONE;
    short type, access, got_size;
    int cc, implied;
    static char *Fname = "ReadMIBFileEntry";
    char *synerr_str;
    SR_INT32 enum_num;
    ENUMER *ne = NULL;
    MIB_INDEX *mi = NULL;
    unsigned long t_ul, t_ul2;
    long t_l, t_l2;

    if (mffip == NULL) {
        mffip = (MIBFromFileInfo *)malloc(sizeof(MIBFromFileInfo));
        if (mffip == NULL) {
            return -1;
        }
    }

    if (otep != NULL) {
        ne = otep->enumer;
        if (ne != NULL) {
            while (ne->next != NULL) {
                ne = ne->next;
            }
        }
        mi = otep->mib_index;
        if (mi != NULL) {
            while (mi->next != NULL) {
                mi = mi->next;
            }
        }
    }
    while (fgets(mffip->buf, sizeof(mffip->buf), cfgp->fp) != NULL) {
        cfgp->linecount++;

        switch (context) {

        case RMFE_NONE:
            /* Check for blank lines */
            if (strspn(mffip->buf, " \t\n") == strlen(mffip->buf)) {
                continue;
            }
            /* Check for beginning or ending of comment lines */
            if (strstr(mffip->buf, "/*")) {
                context = RMFE_COMMENT;
                continue;
            }
            /* Check if this is an enumerations entry */
            if (mffip->buf[0] == '(') {
                context = RMFE_ENUMER;
                continue;
            }
            /* Check if this is an index entry */
            if (mffip->buf[0] == '{') {
                context = RMFE_INDEX;
                continue;
            }
            /* Check if this is a new variable entry */
            if (((mffip->buf[0] >= 'a') && (mffip->buf[0] <= 'z')) ||
                ((mffip->buf[0] >= 'A') && (mffip->buf[0] <= 'Z'))) {
                cc = sscanf(mffip->buf, "%s %s %[^( \n\f\t](%[^( \n\f\t])",
                            mffip->name, mffip->number,
                            mffip->type, mffip->size_range);
                if (cc == 4) {
                    got_size = 1;
                } else if (cc == 3) {
                    got_size = 0;
                } else {
                    synerr_str = "wrong number of fields";
                    goto syntax_warning;
                }
                type = TypeStringToShort(mffip->type);
                if (type == 0) {
                    synerr_str = "unknown type";
                    goto syntax_warning;
                }
                if (type != NON_LEAF_TYPE) {
                    cc = sscanf(mffip->buf, "%*s %*s %*s %s", mffip->access);
                    if (cc != 1) {
                        synerr_str = "missing access specifier";
                        goto syntax_warning;
                    }
                    access = AccessStringToShort(mffip->access);
                    if (access == 0) {
                        synerr_str = "bad access specifier";
                        goto syntax_warning;
                    }
                } else {
                    access = NOT_ACCESSIBLE_ACCESS;
                }
                otep = NewOID_TREE_ELE();
                if (otep == NULL) {
                    return -1;
                }
                otep->oid_name = (char *)malloc(strlen(mffip->name) + 1);
                otep->oid_number_str = (char *)malloc(strlen(mffip->number)+1);
                if ((otep->oid_name == NULL) || (otep->oid_number_str == NULL)){
                    ReleaseOID_TREE_ELE(otep);
                    return -1;
                }
                strcpy(otep->oid_name, mffip->name);
                strcpy(otep->oid_number_str, mffip->number);
                otep->type = type;
                otep->access = access;
                otep->range_lower = -0x7fffffff;
                otep->range_upper = 0x7fffffff;
                otep->size_lower = 0;
                otep->size_upper = 0xffffffff;
                otep->enumer = NULL;
                otep->mib_index = NULL;

#define U_RANGE_FORMAT "%lu..%lu)"
#define RANGE_FORMAT "%ld..%ld)"

                if (got_size) {
                    switch (type) {
                    case PHYS_ADDRESS_TYPE:
                    case DISPLAY_STRING_TYPE:
                    case OCTET_PRIM_TYPE:
                    case OBJECT_ID_TYPE:
                    case TIME_TICKS_TYPE:
                    case GAUGE_TYPE:
                    case COUNTER_TYPE:
                    case SERVICES_TYPE:
                        sscanf(mffip->size_range, U_RANGE_FORMAT, &t_ul, &t_ul2);
                        otep->size_lower = (SR_UINT32)t_ul;
                        otep->size_upper = (SR_UINT32)t_ul2;
                        
                        break;
                    case INTEGER_TYPE:
                        sscanf(mffip->size_range, RANGE_FORMAT, &t_l, &t_l2);
                        otep->range_lower = (SR_INT32)t_l;
                        otep->range_upper = (SR_INT32)t_l2;
                        break;
#ifdef SR_SNMPv2_PDU
                    case COUNTER_64_TYPE:
                        break;
#endif /* SR_SNMPv2_PDU */
                    case NETWORK_ADDRESS_TYPE:
                    case IP_ADDR_PRIM_TYPE:
                    case NON_LEAF_TYPE:
                    case AGGREGATE_TYPE:
                    default:
                        synerr_str = "bad range specifier";
                        goto syntax_error;
                    }
                }
                *otepp = otep;
                return 0;
            }
            synerr_str = "bad variable name, must start with a-zA-Z";
            goto syntax_warning;

        case RMFE_COMMENT:
            /* Check for beginning or ending of comment lines */
            if (strstr(mffip->buf, "*/")) {
                context = RMFE_NONE;
            }
            continue;

        case RMFE_ENUMER:
            if (mffip->buf[0] == ')') {
                return 0;
            }
            if (otep == NULL) {
                synerr_str = "enumerations found before mib variable found";
                goto syntax_warning;
            }
            cc = sscanf(mffip->buf, INT32_FORMAT" %s", &enum_num, mffip->enum_name);
            if (cc != 2) {
                synerr_str = "bad enumeration entry";
                DPRINTF((APERROR, "%s: %s at %d in %s\n", Fname, synerr_str,
                         cfgp->linecount, cfgp->filename));
                continue;
            }
            if (ne == NULL) {
                otep->enumer = ne = (ENUMER *)malloc(sizeof(ENUMER));
                if (ne == NULL) {
                    synerr_str = "cannot malloc enumeration entry";
                    DPRINTF((APERROR, "%s: %s at %d in %s\n",
                             Fname, synerr_str,
                             cfgp->linecount, cfgp->filename));
                    continue;
                }
            } else {
                ne->next = (ENUMER *)malloc(sizeof(ENUMER));
                if (ne->next == NULL) {
                    synerr_str = "cannot malloc enumeration entry";
                    DPRINTF((APERROR, "%s: %s at %d in %s\n",
                             Fname, synerr_str,
                             cfgp->linecount, cfgp->filename));
                    continue;
                }
                ne = ne->next;
            }
            ne->name = (char *)malloc(strlen(mffip->enum_name) + 1);
            if (ne->name == NULL) {
                free(ne);
                ne = otep->enumer;
                if (ne != NULL) {
                    while (ne->next != NULL) {
                        ne = ne->next;
                    }
                } else {
                    otep->enumer = NULL;
                }
                synerr_str = "cannot malloc enumeration name";
                DPRINTF((APERROR, "%s: %s at %d in %s\n",
                         Fname, synerr_str,
                         cfgp->linecount, cfgp->filename));
                continue;
            } else {
                strcpy(ne->name, mffip->enum_name);
            }
            ne->number = enum_num;
            ne->next = NULL;
            continue;

        case RMFE_INDEX:
            if (mffip->buf[0] == '}') {
                return 0;
            }
            if (otep == NULL) {
                synerr_str = "mib indices found before mib variable found";
                goto syntax_warning;
            }
            implied = 1;
            cc = sscanf(mffip->buf, "IMPLIED %s", mffip->index_name);
            if (cc != 1) {
                implied = 0;
                cc = sscanf(mffip->buf, "%s", mffip->index_name);
                if (cc != 1) {
                    synerr_str = "bad mib index entry";
                    DPRINTF((APERROR, "%s: %s at %d in %s\n", Fname, synerr_str,
                             cfgp->linecount, cfgp->filename));
                    continue;
                }
            }
            if (mi == NULL) {
                otep->mib_index = mi = (MIB_INDEX *)malloc(sizeof(MIB_INDEX));
                if (mi == NULL) {
                    synerr_str = "cannot malloc mib_index entry";
                    DPRINTF((APERROR, "%s: %s at %d in %s\n",
                             Fname, synerr_str,
                             cfgp->linecount, cfgp->filename));
                    continue;
                }
            } else {
                mi->next = (MIB_INDEX *)malloc(sizeof(MIB_INDEX));
                if (mi->next == NULL) {
                    synerr_str = "cannot malloc mib_index entry";
                    DPRINTF((APERROR, "%s: %s at %d in %s\n",
                             Fname, synerr_str,
                             cfgp->linecount, cfgp->filename));
                    continue;
                }
                mi = mi->next;
            }
            mi->name = (char *)malloc(strlen(mffip->index_name) + 1);
            if (mi->name == NULL) {
                free(mi);
                mi = otep->mib_index;
                if (mi != NULL) {
                    while (mi->next != NULL) {
                        mi = mi->next;
                    }
                } else {
                    otep->mib_index = NULL;
                }
                synerr_str = "cannot malloc mib_index name";
                DPRINTF((APERROR, "%s: %s at %d in %s\n",
                         Fname, synerr_str,
                         cfgp->linecount, cfgp->filename));
                continue;
            } else {
                strcpy(mi->name, mffip->index_name);
            }
            mi->implied = implied;
            mi->next = NULL;
            continue;
        }
    }
    return feof(cfgp->fp) ? 1 : -1;

  syntax_warning:
    /* Invalid entry */
    DPRINTF((APERROR, "%s: %s at %d in %s\n", Fname, synerr_str,
             cfgp->linecount, cfgp->filename));
    return 0;

  syntax_error:
    /* Invalid entry */
    DPRINTF((APERROR, "%s: %s at %d in %s\n", Fname, synerr_str,
             cfgp->linecount, cfgp->filename));
    return -1;
}

/*
 * ReadMIBFile:
 *
 * This function reads a file containing MIB information and merges this
 * info into a MIB tree (specified by the mib_tree argument).  If the
 * mib_tree argument is NULL, the info is read into the default MIB tree.
 * The collision argument can take one of two values.  If it is SR_COL_REPLACE,
 * entries in the file will replace entries in the tree on a collision.  If
 * it is SR_COL_DISCARD, entries in the file will be discarded on a collision.
 *
 * The return value is -1 on failure, 0 on success.
 */
int
ReadMIBFile(filename, mib_tree, collision)
    const char *filename;
    MIB_TREE *mib_tree;
    int collision;
{
    ConfigFile *cfgp;
    OID_TREE_ELE *otep, *last_otep, *oteps, *oteps_tail;
    int status;

    if (mib_tree == NULL) {
        mib_tree = default_mib_tree;
    }

    if (OpenConfigFile(filename, FM_READ, &cfgp) != FS_OK) {
        return -1;
    }

    otep = NULL;
    last_otep = NULL;
    oteps = NULL;
    oteps_tail = NULL;
    while (!(status = ReadMIBFileEntry(cfgp, &otep))) {
        if (otep != last_otep) {
            if (otep != NULL) {
                if (oteps == NULL) {
                    oteps = otep;
                    oteps_tail = otep;
                } else {
                    oteps_tail->next = otep;
                    oteps_tail = otep;
                }
            }
            last_otep = otep;
        }
    }

    CloseConfigFile(cfgp);

    if (status < 0) {
        while (oteps != NULL) {
            otep = oteps;
            oteps = oteps->next;
            ReleaseOID_TREE_ELE(otep);
        }
        return -1;
    }

    if (collision == SR_COL_REPLACE) {
        while (oteps != NULL) {
            otep = oteps;
            oteps = oteps->next;
            otep->next = NULL;
            if (AddOTEToMibTree(mib_tree, otep, collision) != otep) {
                ReleaseOID_TREE_ELE(otep);
            }
        }
    } else {
        while (oteps != NULL) {
            otep = oteps;
            oteps = oteps->next;
            otep->next = NULL;
            if (AddOTEToMibTree(mib_tree, otep, collision) == NULL) {
                ReleaseOID_TREE_ELE(otep);
            }
        }
    }
    return 0;
}

