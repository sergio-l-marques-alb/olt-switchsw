###########################################################
# FILE : cpudb.i
# $Copyright: (c) 2019 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
##########################################################

%module cpudb

%{
#define SWIG_FILE_WITH_INIT
#include <sal/types.h>
#include <appl/cpudb/cpudb.h>
#include "bcm/types.h"
%}

%include "typemaps.i"
%include "cpointer.i"
%include "carrays.i"
%include "bcm/types.h"

typedef unsigned short  uint16;
typedef unsigned int  uint32;
typedef unsigned char  uint8;
typedef signed short  int16;
typedef signed int  int32;
typedef signed char  int8;

typedef struct cpudb_unit_port_s {
    int unit;
    bcm_port_t port;
    uint32 weight;
    uint32 bflags;
} cpudb_unit_port_t;
%array_class(cpudb_unit_port_t, cpudb_unit_port_t_array);
%pointer_class(cpudb_unit_port_t, cpudb_unit_port_t_ptr);

typedef struct cpudb_base_s cpudb_base_t;


typedef struct cpudb_key_s {
    bcm_mac_t key;
} cpudb_key_t;
%array_class(cpudb_key_t, cpudb_key_t_array);
%pointer_class(cpudb_key_t, cpudb_key_t_ptr);

struct cpudb_base_s {
    cpudb_key_t key;
    bcm_mac_t mac;
    volatile int dseq_num;      /* Discovery sequence number */

    int slot_id;          /* For chassis; can be used for stack order */
    int master_pri;       /* Master priority */
    uint8 app_data[CPUDB_APP_DATA_BYTES];   /* Application discovery data */

    int num_units;        /* Number of physical units controlled */
    int dest_unit;
    int dest_port;

    /* Stack port info; also part of BASE INIT */
    int num_stk_ports;

    /* Keep variable length data at end for convenience */
    /* sp_u_p:  Stack-port unit/port; Local only */
    cpudb_unit_port_t stk_ports[CPUDB_CXN_MAX];

    int pref_mod_id[BCM_MAX_NUM_UNITS];
    int mod_ids_req[BCM_MAX_NUM_UNITS];
    CPUDB_BOARD_ID board_id;
    uint32 flags;
};

typedef struct cpudb_stk_port_s {
    uint32 flags;               /* See CPUDB_SPF* below */
    cpudb_key_t tx_cpu_key;
    int tx_stk_idx;
    cpudb_key_t rx_cpu_key;
    int rx_stk_idx;
} cpudb_stk_port_t;
%array_class(cpudb_stk_port_t, cpudb_stk_port_t_array);
%pointer_class(cpudb_stk_port_t, cpudb_stk_port_t_ptr);

typedef cpudb_stk_port_t cpudb_sp_list_t[CPUDB_CXN_MAX];

typedef int cpudb_mod_list_t[BCM_MAX_NUM_UNITS];

typedef struct cpudb_entry_s cpudb_entry_t;
struct cpudb_entry_s {
    cpudb_base_t base;

    cpudb_sp_list_t sp_info; /* cpudb_stk_port_t[CPUDB_CXN_MAX] */

    /* Applications that modify DB entries should update flags */
    volatile uint32 flags; /* See CPUDB_F_* below */

    /* SYSID member */
    void *sysid;

    /* TX members */
    int tx_unit;           /* Local info */
    int tx_port;

    /* Destination module ID/port to use to address this CPU */
    int dest_mod;
    int dest_port;    /* May be remapped for mod-id remapping */

    /* Other info, post discovery */
    cpudb_mod_list_t mod_ids; /* int[BCM_MAX_NUM_UNITS] */

    void *user_cookie;         /* Application cookie */

    /* Administrivia */
    cpudb_ref_t db_ref;        /* To which DB do we belong? */
    cpudb_entry_t *next;
    cpudb_entry_t *prev;       /* For easier removal */
    cpudb_entry_t *h_next;     /* Hash chain linkage */
    cpudb_entry_t *h_prev;
    bcm_trans_ptr_t *trans_ptr;   /* Mainly used by c2c for setup, tx */
    int topo_idx;            /* Internal, local index for topo, 0..n-1; */
};

/* An instance of a database */
struct cpudb_s {
    int num_cpus;                  /* How many active */
    cpudb_entry_t *entries;
    cpudb_entry_t *local_entry;    /* Set prior to discovery */
    cpudb_entry_t *master_entry;   /* Set by discovery */

    /* Hash for key lookups */
    cpudb_entry_t *key_hash[CPUDB_HASH_ENTRY_COUNT];

    /* Administrivia */
    void *topo_cookie;             /* Cookie for topology results */
    uint32 magic;                  /* For verifying valid */
    cpudb_ref_t old_db;            /* Previous DB in system */
};

typedef struct cpudb_s cpudb_t;

typedef cpudb_t *cpudb_ref_t;


