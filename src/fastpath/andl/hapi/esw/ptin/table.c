//**************************************************************************************
//
//ABSTRACT:
//Table manipulation functions
//
//
//HISTORY:
//
//
//COPYRIGHT:
//PT Inovacao (a.k.a. PTIn, a.k.a. CET)
//
//
//CONTACT:
//rcosta@ptinovacao.pt
//
//**************************************************************************************

//EXTERNAL TYPES/VARIABLES/ROUTINES/OBJECTS / #includeS ********************************
#include <table.h>

//OWN TYPES/VARIABLES/OBJECTS***********************************************************

//PUBLIC VARIABLES/OBJECTS**************************************************************
//OWN ROUTINES**************************************************************************
//PUBLIC ROUTINES***********************************************************************

int entry_is_empty      (const void *t_table, T_index index, T_index N, u32 sizeof_table_entry)        {return 0==index2n_used(t_table, index, N);}

int init_entry          (void *t_table, T_index index, T_index N, u32 sizeof_table_entry)              {index2n_used(t_table, index, N)=0;return 0;}

int init_table          (void *t_table, u32 sizeof_table_entry, T_index N)   {
T_index i;
    for (i=0; i<N; i++) init_entry(t_table,i,N,sizeof_table_entry);
    return 0;
}//init_table




T_index content2index   (const void *ent, u32 sizeof_table_entry_key, T_index N) {
u32 x=0, *p;
//u8 *p8;
T_index i;

    for (i=0, p=(u32*)ent;
         i< sizeof_table_entry_key/4;
         i++)
        x^= p[i]>>24 ^ p[i]<<24 ^ p[i]>>8 ^ p[i]<<8;

    //for (p8=&p[i], i=0; 
    //     i< sizeof_table_entry_key%4;
    //     i++)
    //    x^= (u32)p8[i]<<(8*(3-i));

    return x%N;
}//content2index




T_index find_entry      (const void *t_table, const void *ent, u32 sizeof_table_entry, u32 sizeof_table_entry_key, T_index N, T_index initial_index, T_index *p1st_free) {
T_index i, _1st_free=(T_index)-1;

    if (initial_index>=N)   initial_index=  content2index(ent, sizeof_table_entry_key, N);

    for (i=0; i<N; i++) {
        if (index2n_used(t_table, initial_index, N)) {
            if (0==memcmp(ent, pointer2table_index(t_table, initial_index, N, sizeof_table_entry), sizeof_table_entry_key)) {
                if (NULL!=p1st_free) *p1st_free=_1st_free;
                return initial_index;
            }
        }
        else
        if (_1st_free>=N) _1st_free=initial_index;

        if (++initial_index>=N) initial_index=0;
    }

    if (NULL!=p1st_free) *p1st_free=_1st_free;

    return (T_index)-1;
}//find_entry




int wr_entry            (void *t_table, const void *ent, u32 sizeof_table_entry, T_index N, T_index index) {
    if (index>=N) return 1;

    memcpy(pointer2table_index(t_table, index, N,  sizeof_table_entry), ent, sizeof_table_entry);
    index2n_used(t_table, index, N)=1;
    return 0;
}//wr_entry




T_index add_entry       (void *t_table, const void *ent, u32 sizeof_table_entry, u32 sizeof_table_entry_key, T_index N, T_index initial_index, int count_n_used, int check_if_exists) {
T_index i, _1st_free=(T_index)-1;

    if (initial_index>=N)   initial_index=  content2index(ent, sizeof_table_entry_key, N);

    for (i=0; i<N; i++) {
        if (index2n_used(t_table, initial_index, N)) {
            if (check_if_exists &&  0==memcmp(pointer2table_index(t_table, initial_index, N, sizeof_table_entry), ent, sizeof_table_entry_key))  {
                if (count_n_used) index2n_used(t_table, initial_index, N)++;
                else memcpy(pointer2table_index(t_table, initial_index, N, sizeof_table_entry), ent, sizeof_table_entry);
                return initial_index;
            }
        }
        else
        if (_1st_free>=N) {
            _1st_free=initial_index;
            if (!check_if_exists) break;
        }

        if (++initial_index>=N) initial_index=0;
    }//for

    if (_1st_free>=N) return (T_index)-1;

    memcpy(pointer2table_index(t_table, _1st_free, N, sizeof_table_entry), ent, sizeof_table_entry);

    index2n_used(t_table, _1st_free, N)++;
    return _1st_free;
}//add_entry




T_index del_entry   (void *t_table, const void *ent, u32 sizeof_table_entry, u32 sizeof_table_entry_key, T_index N, T_index initial_index) {
    if (NULL==ent) {
        if (initial_index>=N) return (T_index)-1;
        if (index2n_used(t_table, initial_index, N))    index2n_used(t_table, initial_index, N)--;
        return initial_index;
    }

    initial_index=  find_entry(t_table, ent, sizeof_table_entry, sizeof_table_entry_key, N, initial_index, NULL);

    if (initial_index<N) {
        if (index2n_used(t_table, initial_index, N)) {
            --index2n_used(t_table, initial_index, N);
            return initial_index;
        }
        else return (T_index)-1;
    }
    else
    return (T_index)-1;
}//del_entry

