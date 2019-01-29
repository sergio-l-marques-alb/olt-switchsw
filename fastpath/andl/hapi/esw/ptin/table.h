#ifndef _TABLE_H_
#define _TABLE_H_

#include <stdlib.h>
#include <string.h>
#include <ethsrv_oam.h>

typedef u32 T_index;



/*
typedef struct {
    T_val   in,
            out;
} __attribute__ ((packed)) T_ORDERED_TABLE_ENTRY;
*/



//TABLE TEMPLATES
#define typedef_TABLE_TEMPLATE(T,N,T_TABLE)\
typedef struct {\
    /*put nothing here*/\
    T_index         n_used[N];  /*don't change this order*/\
    T               table[N];   /*don't change this order*/\
} __attribute__ ((packed)) T_TABLE;
//Structure "T" must start with key (field(s) that identify the instance)
//This(ese) field(s) must be 4 byte multiples




//typedef_TABLE_TEMPLATE(u32,1024,T_TABLE)
//just to define "T_TABLE"

  //#define pointer2table_index(p2t_table, index, sizeof_table_entry)   (& ((u8*) ((T_TABLE*)(p2t_table))->table)[(index)*(sizeof_table_entry)])
//#define index2n_used(p2t_table, index)                              (((T_TABLE*)(p2t_table))->n_used[index])
#define pointer2table_index(p2t_table, index, N, sizeof_table_entry)    (& ((u8*) (p2t_table))[(index)*(sizeof_table_entry)+(N)*sizeof(T_index)])
#define index2n_used(p2t_table, index, N)                               ((T_index *)(p2t_table))[index]




extern int entry_is_empty   (const void *t_table, T_index index, T_index N, u32 sizeof_table_entry);
extern int init_entry       (void *t_table, T_index index, T_index N, u32 sizeof_table_entry);
extern int init_table       (void *t_table, u32 sizeof_table_entry, T_index N);

extern T_index content2index(const void *ent, u32 sizeof_table_entry, T_index N);
extern T_index find_entry   (const void *t_table, const void *ent, u32 sizeof_table_entry, u32 sizeof_table_entry_key, T_index N, T_index initial_index, T_index *p1st_free);
extern int wr_entry         (void *t_table, const void *ent, u32 sizeof_table_entry, T_index N, T_index index);
extern T_index add_entry    (void *t_table, const void *ent, u32 sizeof_table_entry, u32 sizeof_table_entry_key, T_index N, T_index initial_index, int count_n_used, int check_if_exists);
extern T_index del_entry    (void *t_table, const void *ent, u32 sizeof_table_entry, u32 sizeof_table_entry_key, T_index N, T_index initial_index);
#endif /*_TABLE_H_*/

