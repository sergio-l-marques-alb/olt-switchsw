#include <bcm/types.h>
#include <appl/diag/eav/eav_list.h>

#ifndef EAV_CONNDB_H
#define EAV_CONNDB_H

typedef struct _dc_entry_s {
  struct list_head list;
  bcm_mac_t mac0;
  bcm_mac_t mac1;
  unsigned char port0;
  unsigned char port1;
  unsigned char from;
  unsigned char local;
  int refcount;
  int master;
  struct _portinfo_s {
    unsigned int  q4bandwidth;
    unsigned int  q5bandwidth;
    unsigned int  timeinfo;
  } portinfo[2];
} _dc_entry_t;

#define _MAXCONNECTIONS  512

typedef struct _dc_entries_db_s {
  _dc_entry_t entries[_MAXCONNECTIONS];
  struct list_head free;
  struct list_head used;
  int nused, total;
} _dc_entries_db_t;

void _dc_entries_db_init( _dc_entries_db_t *pdb );

_dc_entry_t * _dc_entries_db_search( _dc_entries_db_t *pdb, bcm_mac_t mac0, bcm_mac_t mac1,
					    unsigned char port0, unsigned char port1);

_dc_entry_t * _dc_entries_db_insert( _dc_entries_db_t *pdb, bcm_mac_t mac0, bcm_mac_t mac1,
			   unsigned char port0, unsigned char port1, int fromport,
			   int local );

int _dc_entries_db_remove( _dc_entries_db_t *pdb, bcm_mac_t mac0, bcm_mac_t mac1,
			   unsigned char port0, unsigned char port1, int fromport,
			   int local );

_dc_entry_t * _dc_entries_db_insert_replace( _dc_entries_db_t *pdb, bcm_mac_t mac0, bcm_mac_t mac1,
				   unsigned char port0, unsigned char port1, int fromport,
					     int local );

int _dc_entries_db_show( _dc_entries_db_t *pdb );  

#endif
