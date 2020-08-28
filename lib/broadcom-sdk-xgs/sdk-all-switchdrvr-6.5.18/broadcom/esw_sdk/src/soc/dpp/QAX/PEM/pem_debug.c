#include "pem_debug_user_interface.h"
#include "pem_debug.h"
#include "pem_meminfo_init.h"
#include "pem_physical_access.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>



#ifdef _MSC_VER
#if (_MSC_VER >= 1400) 
#pragma warning(push)
#pragma warning(disable: 4996) /* To avoid Microsoft compiler warning (4996 for _CRT_SECURE_NO_WARNINGS equivalent)*/
#endif
#endif

static pem_debug_info_t pem_debug_info;

int pem_debug_print_all_pem_access(int stage_num, io_field_e input_or_output)
{
  int context, field_id;
  unsigned int load_dbus[PEM_DEBUG_NOF_ENTRIES_FOR_DBUS_UINT_ARR];
  unsigned int update_dbus[PEM_DEBUG_NOF_ENTRIES_FOR_DBUS_UINT_ARR];
  unsigned int fifo_entry[PEM_DEBUG_NOF_ENTRIES_FOR_FIFO_UINT_ARR];
  
  if (stage_num < 0 || stage_num >= PEM_NOF_SOFT_STAGES)
    return -1;

  /*printf file format*/
  if (INPUT_PACKET == input_or_output)
    printf("/*<INPUT_PACKET> <stage> <context> <field_name> <field_value>*/\r\n");
  else /*OUTPUT_PACKET*/
    printf("/*<OUTPUT_PACKET> <stage> <context> <field_name> <field_value>*/\r\n");

  /*get current context*/
  context = get_last_packet_context(stage_num);
  
  /*do actual read from design of 2 dbuses and fifo entry*/
  if (-1 == read_fifo_and_dbuses(stage_num, load_dbus, update_dbus, fifo_entry, 0))
    return -1;

  for (field_id = 0; field_id < PEM_DEBUG_MAX_FIELD_ID; ++field_id){
    if (INPUT_PACKET == input_or_output)
      pem_debug_print_input_field_by_id(field_id, load_dbus, fifo_entry, stage_num, context); 
    else /*OUTPUT_PACKET*/
      pem_debug_print_output_field_by_id(field_id, update_dbus, fifo_entry, stage_num, context);
    }

  return 0;
}

int pem_debug_print_pem_access_all_inputs(int stage_num)
{
  return pem_debug_print_all_pem_access(stage_num, INPUT_PACKET);
}

int pem_debug_print_pem_access_all_outputs(int stage_num)
{
    return pem_debug_print_all_pem_access(stage_num, OUTPUT_PACKET);
}

int pem_debug_print_pem_access_field(int stage_num, const char* field_name, io_field_e input_or_output)
{
    int field_id, context, found = 0;
    unsigned int load_dbus[PEM_DEBUG_NOF_ENTRIES_FOR_DBUS_UINT_ARR];
    unsigned int update_dbus[PEM_DEBUG_NOF_ENTRIES_FOR_DBUS_UINT_ARR];
    unsigned int fifo_entry[PEM_DEBUG_NOF_ENTRIES_FOR_FIFO_UINT_ARR];
    
    if (stage_num < 0 || stage_num >= PEM_NOF_SOFT_STAGES)
      return -1;

    context = get_last_packet_context(stage_num);
    
    /*do actual read from design of 2 dbuses and fifo entry*/
    if (-1 == read_fifo_and_dbuses(stage_num, load_dbus, update_dbus, fifo_entry, 0))
      return -1;

    /*look for field by name*/
    for (field_id = 0; field_id < PEM_DEBUG_MAX_FIELD_ID; ++field_id)
    {
      /*search for field*/
      if (pem_debug_info.debug_info_arr[stage_num].fifo_desc_arr[field_id] == NULL)
        continue;
      /*else, if field_name is current field, break*/
      else if (!strcmp(field_name, pem_debug_info.debug_info_arr[stage_num].fifo_desc_arr[field_id]->field_name)){
        found = 1;    
        break;
      }
    }

    /*if found, print field from FIFO if possible. o.w print from load_dbus*/
    if (found){
      if (input_or_output == INPUT_PACKET)
      {
        printf("/*<INPUT_PACKET> <stage> <context> <field_name> <field_value>*/\r\n");
        pem_debug_print_input_field_by_id(field_id, load_dbus, fifo_entry, stage_num, context);
      }
      else /*OUTPUT_PACKET*/
      {
        printf("/*<OUTPUT_PACKET> <stage> <context> <field_name> <field_value>*/\r\n");
        pem_debug_print_output_field_by_id(field_id, update_dbus, fifo_entry, stage_num, context);
      }
    }

    return 0;
}

int pem_debug_print_pem_access_single_input(int stage_num, const char* field_name)
{
    return pem_debug_print_pem_access_field(stage_num, field_name, INPUT_PACKET);
}

int pem_debug_print_pem_access_single_output(int stage_num, const char* field_name)
{   
    return pem_debug_print_pem_access_field(stage_num, field_name, OUTPUT_PACKET);
}


int pem_debug_print_pem_access_relevant_inputs(int stage_num)
{
    int field_id;
    const pem_load_dbus_desc_t* load_dbus_field_data;
    const pem_fifo_desc_t* fifo_field_data;
    unsigned int load_dbus[PEM_DEBUG_NOF_ENTRIES_FOR_DBUS_UINT_ARR];
    unsigned int fifo_entry[PEM_DEBUG_NOF_ENTRIES_FOR_FIFO_UINT_ARR];
    int context;
    int dump_header_offset_array;
    int dump_qualifiers_array;
    int dump_forwarding_offset_index;
    int dump_packet_header;
    const char* curr_field_name;
    unsigned int* read_data;

    if ((stage_num < 0) || (stage_num >= PEM_NOF_SOFT_STAGES))
    {
      return -1;
    }
  
    context = get_last_packet_context(stage_num);
    dump_header_offset_array     = pem_debug_info.debug_info_arr[stage_num].dbus_desc_per_context_arr[context].dump_header_offsets_array;
    dump_qualifiers_array        = pem_debug_info.debug_info_arr[stage_num].dbus_desc_per_context_arr[context].dump_qualifiers_array;
    dump_forwarding_offset_index = pem_debug_info.debug_info_arr[stage_num].dbus_desc_per_context_arr[context].dump_forwarding_offset_idx;
    dump_packet_header           = pem_debug_info.debug_info_arr[stage_num].dbus_desc_per_context_arr[context].dump_packet_header;

    /*do actual read from design of 2 dbuses and fifo entry*/
    if (-1 == read_fifo_and_dbuses(stage_num, load_dbus, NULL, fifo_entry, 0))
    {
      return -1;
    }

    printf("IN_PACKET_START\r\n");
    printf("/*<INPUT_PACKET> <stage> <context> <field_name> <field_value>*/\r\n");

    for (field_id = 0; field_id < PEM_DEBUG_MAX_FIELD_ID; ++field_id)
    {
      /*search for field*/
      if (pem_debug_info.debug_info_arr[stage_num].fifo_desc_arr[field_id] == NULL)
        continue;

      fifo_field_data = pem_debug_info.debug_info_arr[stage_num].fifo_desc_arr[field_id];
      load_dbus_field_data = pem_debug_info.debug_info_arr[stage_num].dbus_desc_per_context_arr[context].load_dbus_desc_arr[field_id];
      curr_field_name = pem_debug_info.debug_info_arr[stage_num].fifo_desc_arr[field_id]->field_name;

      /*check if current field has to be dumped - arrays, forwarding_offset_index or packet_header*/

      if (  (dump_packet_header && !strcmp(curr_field_name, PEM_DEBUG_PACKET_HEADER))  ||
            (dump_forwarding_offset_index && !strcmp(curr_field_name, PEM_DEBUG_FORWARDING_OFFSET_ARRAY)) ||        
            (dump_qualifiers_array && !strncmp(curr_field_name, PEM_DEBUG_QUALIFIERS_ARRAY_PREFIX, PEM_DEBUG_QUALIFIERS_ARRAY_PREFIX_SIZE))  ||
            (dump_header_offset_array && !strncmp(curr_field_name, PEM_DEBUG_HEADER_OFFSET_ARRAY_PREFIX, PEM_DEBUG_HEADER_OFFSET_ARRAY_PREFIX_SIZE)) )
      {
        /*read value from fifo and printf field*/
        read_data = pem_debug_read_data_from_fifo(fifo_field_data, 0);
        if (pem_debug_print_field(fifo_field_data->field_name, read_data, fifo_field_data->width_in_bits, stage_num, context, "INPUT_PACKET ", ""))
        {
          free(read_data);
          return -1;
        }
        free(read_data);
      }

      /*print field if exists in load dbus and not one of the above (array, header, foi)*/
      if (load_dbus_field_data != NULL)
      {
        read_data = pem_debug_read_data_from_load_dbus(load_dbus_field_data, load_dbus);
        if (pem_debug_print_field(fifo_field_data->field_name, read_data, fifo_field_data->width_in_bits, stage_num, context, "INPUT_PACKET ", ""))
        {
          free(read_data);
          return -1;
        }
        free(read_data);
      }
    }
    printf("IN_PACKET_END\r\n"); 
    return 0;
}

int pem_debug_print_pem_access_relevant_outputs(int stage_num)
{
  pem_update_command_t* update_command;
  unsigned int* read_data = NULL;
  char field_name[MAX_NAME_LENGTH];
  int idx_offset, command_id, array_entry_if_set_relative;
  int forwarding_offset_index;
  int context;
  unsigned int update_dbus[PEM_DEBUG_NOF_ENTRIES_FOR_DBUS_UINT_ARR];
  unsigned int fifo_entry[PEM_DEBUG_NOF_ENTRIES_FOR_FIFO_UINT_ARR];

  if (stage_num < 0 || stage_num >= PEM_NOF_SOFT_STAGES)
    return -1;
  forwarding_offset_index = get_forwarding_offset_index_for_last_packet(stage_num);
  context = get_last_packet_context(stage_num);
  
  /*do actual read from design of 2 dbuses and fifo entry*/
    if (-1 == read_fifo_and_dbuses(stage_num, NULL, update_dbus, fifo_entry, 0))
      return -1;
  
  printf("OUT_PACKET_START\r\n");
  printf("/*<OUTPUT_PACKET> <stage> <context> <field_name> <field_value>*/\r\n");
  /*run over all update commands - if 'STORE_ENTRY' and id matches then print.*/
  /*if 'STORE_RELATIVE_ARRAY_EMTRY' and real id (base_arr_id+index_to_array) is field id then print it.*/
  /*O.w said field is not in update dbus*/
  for (command_id = 0; command_id < PEM_DEBUG_NOF_UPDATE_COMMANDS; ++command_id){
    update_command = pem_debug_info.debug_info_arr[stage_num].dbus_desc_per_context_arr[context].update_commands_arr[command_id];
    if (update_command == NULL) 
      continue;
    /*if it is a set relative entry, then the actual entry to set is forwarding_offset_index+index_offset (mod arr size)*/
    idx_offset = update_command->idx_offset;
    array_entry_if_set_relative = (forwarding_offset_index+idx_offset) % PEM_DEBUG_ARRAY_MAX_SIZE;
    /*in case of set_relative, correct name should be set, as only array's name is present*/
    if (update_command->is_set_relative)
      sprintf(field_name, "%s[%d]", update_command->field_or_array_name, array_entry_if_set_relative);
    else /*store field - just copy name from command*/
      sprintf(field_name, "%s", update_command->field_or_array_name);
    /*if found the actual field interested in, print it*/
    read_data = pem_debug_read_data_from_update_dbus(update_command, update_dbus);
    if (pem_debug_print_field(field_name, read_data, update_command->field_width, stage_num, context, "OUTPUT_PACKET", "")){
      free(read_data);
      return -1;
    }
    free(read_data);
  }
  printf("OUT_PACKET_END\r\n");
  return 0;
}

static int get_pem_context_from_fifo(int stage_num, unsigned int* fifo_entry){
    int field_id;
    unsigned int* read_data;
    int context, mask = 0x1F; /*5 bits field*/
    
    /*look for field by name*/
    for (field_id = 0; field_id < PEM_DEBUG_MAX_FIELD_ID; ++field_id)
    {
      /*search for field*/
      if (pem_debug_info.debug_info_arr[stage_num].fifo_desc_arr[field_id] == NULL)
        continue;
      /*else, if field_name is current field, break*/
      else if (!strcmp("pem_context", pem_debug_info.debug_info_arr[stage_num].fifo_desc_arr[field_id]->field_name)){
        if (pem_debug_info.debug_info_arr[stage_num].fifo_desc_arr[field_id]->fifo_lsb_loaction != -1){
          read_data = pem_debug_read_data_from_fifo(pem_debug_info.debug_info_arr[stage_num].fifo_desc_arr[field_id], fifo_entry);
          context = *read_data & mask;
          free(read_data);
          return context;
        }
      }
    }

    return -1;
}

static int does_exist_update_command_by_field_id(int stage_num, int context, int field_id){
  pem_update_command_t* update_command;
  int idx_offset, command_id, array_entry_if_set_relative, real_id, context_ndx, nof_contexts_to_look_in;

  int forwarding_offset_index = get_forwarding_offset_index_for_last_packet(stage_num);

  /*if no such field, return*/
  if (pem_debug_info.debug_info_arr[stage_num].fifo_desc_arr[field_id] == NULL)
    return -1;

  /*if context is -1, look for field in all contexts.
  o.w only 1 iteration looking in relevant context    */
  nof_contexts_to_look_in = (context == -1) ? PEM_NOF_CONTEXTS : 1;
  context_ndx = (context == -1) ? 0 : context;

  for (; nof_contexts_to_look_in > 0; --nof_contexts_to_look_in, ++context_ndx){
    for (command_id = PEM_DEBUG_NOF_UPDATE_COMMANDS-1; command_id >= 0; --command_id){
      update_command = pem_debug_info.debug_info_arr[stage_num].dbus_desc_per_context_arr[context_ndx].update_commands_arr[command_id];
      if (update_command == NULL) 
        continue;
      /*if it is a set relative entry, then the actual entry to set is forwarding_offset_index+index_offset (mod arr size)*/
      idx_offset = update_command->idx_offset;
      array_entry_if_set_relative = (forwarding_offset_index+idx_offset) % PEM_DEBUG_ARRAY_MAX_SIZE;
      /*id for set relative is the base_arr_id+actual_entry*/
      real_id = (update_command->is_set_relative) ? update_command->field_id+array_entry_if_set_relative : update_command->field_id;
      /*if real_id is the id looking for return true*/
      if (real_id == field_id) /*not the relevant field*/
        return 1;  
    }
  }

  /*not found such field - return false*/
  return 0; 
}

int pem_debug_print_pem_access_packet_in_fifo(int stage_num, int packet_backwards_offset)
{
   unsigned int fifo_entry_data[PEM_DEBUG_NOF_ENTRIES_FOR_FIFO_UINT_ARR];
   pem_fifo_desc_t* fifo_field_data;
   unsigned int* read_data = NULL;
   unsigned int field_id;
   int context, exists_in_update;

   if (stage_num < 0 || stage_num >= PEM_NOF_SOFT_STAGES)
      return -1;

    /*do actual read from design of 2 dbuses and fifo entry*/   
   if (-1 == read_fifo_and_dbuses(stage_num, NULL, NULL, fifo_entry_data, packet_backwards_offset))
      return -1;

   /*get context from packet in fifo*/
   context = get_pem_context_from_fifo(stage_num, fifo_entry_data); /*if not fount then 2 is returned*/

  printf("/*FIFO_FIELD_DEBUG <stage> <context> <field_name> <value> <remark>*/\r\n");

  for (field_id = 0; field_id < PEM_DEBUG_MAX_FIELD_ID; ++field_id){    
       /*if no such field, skip*/
      if (pem_debug_info.debug_info_arr[stage_num].fifo_desc_arr[field_id] == NULL)
        continue;
      /*check if the field is updated*/
        exists_in_update = does_exist_update_command_by_field_id(stage_num, context, field_id);
      /* field exists. if exists in FIFO, print value, o.w skip    */
      fifo_field_data = pem_debug_info.debug_info_arr[stage_num].fifo_desc_arr[field_id];      
      /*read_data is NULL means the value printed is ?. if in Fifo, read it*/
      if (fifo_field_data->fifo_lsb_loaction == -1)
        read_data = NULL;
      else
        read_data = pem_debug_read_data_from_fifo(fifo_field_data, fifo_entry_data);     
      /*check if field is updated*/
      if (exists_in_update){
        if (context != -1) /*exists in update and context is known, then value is modified*/
          pem_debug_print_field(fifo_field_data->field_name, read_data, fifo_field_data->width_in_bits, stage_num, context, "FIFO_FIELD_DEBUG", "/*field is update by PEM*/");
        else /*context is -1: this means context is unkown, and some contexts update the field*/
          pem_debug_print_field(fifo_field_data->field_name, read_data, fifo_field_data->width_in_bits, stage_num, context, "FIFO_FIELD_DEBUG", "/*field may be updated by PEM*/");
      }
      else{ /*not updated by PEM*/
        pem_debug_print_field(fifo_field_data->field_name, read_data, fifo_field_data->width_in_bits, stage_num, context, "FIFO_FIELD_DEBUG", "");
      }
      /*free data read*/
      free(read_data);
  }


   return 0;
}

int pem_debug_print_available_fields(int stage_num)
{
  pem_fifo_desc_t* fifo_field_data = NULL;
  int field_id, exists_for_load, exists_for_update, width;
  char* field_name;

  printf("/*PEM_DEBUG <stage> <width_in_bits> <exists_in_stage_input> <exists_in_stage_output> <field_name>*/\r\n");

  for (field_id = 0; field_id < PEM_DEBUG_MAX_FIELD_ID; ++field_id)
  {
    /*search for field*/
    if (pem_debug_info.debug_info_arr[stage_num].fifo_desc_arr[field_id] == NULL)
      continue;

    fifo_field_data = pem_debug_info.debug_info_arr[stage_num].fifo_desc_arr[field_id];
    /*take properties*/
    field_name = fifo_field_data->field_name;    
    exists_for_load = fifo_field_data->exists_for_load;
    exists_for_update = fifo_field_data->exists_for_update;
    width = fifo_field_data->width_in_bits;
    /*print field*/
    printf("PEM_DEBUG %d %4d %d %d %s\r\n", stage_num, width, exists_for_load, exists_for_update, field_name);
  }

  return 0;
}

/*************************************************************************************************************************************************/
void pem_debug_print_input_field_by_id(unsigned int field_id, unsigned int* dbus, unsigned int* fifo, unsigned int stage_num, unsigned int context)
{
      pem_fifo_desc_t* fifo_field_data;
      pem_load_dbus_desc_t* load_dbus_field_data;
      unsigned int* read_data = NULL;

      /*if no such field, return*/
      if (pem_debug_info.debug_info_arr[stage_num].fifo_desc_arr[field_id] == NULL)
        return;
      /*field exists. if exists in FIFO, print value from there, else if exists in dbus print from dbus. O.w. cannot print field as it is lost*/
      fifo_field_data = pem_debug_info.debug_info_arr[stage_num].fifo_desc_arr[field_id];
      load_dbus_field_data = pem_debug_info.debug_info_arr[stage_num].dbus_desc_per_context_arr[context].load_dbus_desc_arr[field_id];      
      /*if field is not loaded and not in fifo then cannot restore - print ?*/
      if (NULL == load_dbus_field_data && fifo_field_data->fifo_lsb_loaction == -1){
        pem_debug_print_field(fifo_field_data->field_name, NULL, fifo_field_data->width_in_bits, stage_num, context, "INPUT_PACKET ", "");
        return;
      }

      /*getting here means the field exists either in load dbus, in fifo or both*/
      /*better read from fifo as whole field is present (for dbus not all bits must exist if not needed)*/
      if (fifo_field_data->fifo_lsb_loaction != -1)
        read_data = pem_debug_read_data_from_fifo(fifo_field_data, fifo);
      else /*not present in fifo, therefore read from dbus               */
        read_data = pem_debug_read_data_from_load_dbus(load_dbus_field_data, dbus);

      /*printf field*/
      pem_debug_print_field(fifo_field_data->field_name, read_data, fifo_field_data->width_in_bits, stage_num, context, "INPUT_PACKET ", "");
      free(read_data);
}

void pem_debug_print_output_field_by_id(unsigned int field_id, unsigned int* dbus, unsigned int* fifo, unsigned int stage_num, unsigned int context)
{
  pem_fifo_desc_t* fifo_field_data; 
  pem_update_command_t* update_command;
  unsigned int* read_data = NULL;
  unsigned int zeros_array[1024] = {0}; /*used to print zeros when field not in update dbus nor fifo*/
  char field_name[MAX_NAME_LENGTH];
  int idx_offset, command_id, array_entry_if_set_relative, real_id_to_print;
  int forwarding_offset_index = get_forwarding_offset_index_for_last_packet(stage_num);

  /*if no such field, return*/
  if (pem_debug_info.debug_info_arr[stage_num].fifo_desc_arr[field_id] == NULL)
    return;

  /*run over all update commands - if 'STORE_ENTRY' and id matches then print.*/
  /*if 'STORE_RELATIVE_ARRAY_EMTRY' and real id (base_arr_id+index_to_array) is field id then print it.*/
  /*O.w said field is not in update dbus*/
  for (command_id = PEM_DEBUG_NOF_UPDATE_COMMANDS-1; command_id >= 0; --command_id){
    update_command = pem_debug_info.debug_info_arr[stage_num].dbus_desc_per_context_arr[context].update_commands_arr[command_id];
    if (update_command == NULL) 
      continue;
    /*if it is a set relative entry, then the actual entry to set is forwarding_offset_index+index_offset (mod arr size)*/
    idx_offset = update_command->idx_offset;
    array_entry_if_set_relative = (forwarding_offset_index+idx_offset) % PEM_DEBUG_ARRAY_MAX_SIZE;
    /*id for set relative is the base_arr_id+actual_entry*/
    real_id_to_print = (update_command->is_set_relative) ? field_id+array_entry_if_set_relative : field_id;
    if (real_id_to_print != update_command->field_id) /*not the relevant field*/
      continue;
    /*in case of set_relative, correct name should be set, as only array's name is present*/
    if (update_command->is_set_relative)
      sprintf(field_name, "%s[%d]", update_command->field_or_array_name, array_entry_if_set_relative);
    else /*store field - just copy name from command*/
      sprintf(field_name, "%s", update_command->field_or_array_name);
    /*if found the actual field interested in, print it*/
    read_data = pem_debug_read_data_from_update_dbus(update_command, dbus);
    pem_debug_print_field(field_name, read_data, update_command->field_width, stage_num, context, "OUTPUT_PACKET", "");
    free(read_data);
    return;
  }

  /*getting here means field is not is update DBUS.*/
  /*print from FIFO if available, and o.w print zeros if exists for update*/
  fifo_field_data = pem_debug_info.debug_info_arr[stage_num].fifo_desc_arr[field_id];
  if (fifo_field_data->exists_for_update == -1) /*does not exist for update*/
    return;

  if (fifo_field_data->fifo_lsb_loaction != -1) /*exists in FIFO*/
    read_data = pem_debug_read_data_from_fifo(fifo_field_data, fifo);
  else /*print zeros*/
    read_data = zeros_array;

  pem_debug_print_field(fifo_field_data->field_name, read_data, fifo_field_data->width_in_bits, stage_num, context, "OUTPUT_PACKET", "");
  if (read_data != zeros_array)
    free(read_data);

}

/******************************************************************************************************************************************************/
/*Init database*/

int init_debug_info_database()
{
  int stage_ndx, context, field_id, command_id;

  for (stage_ndx = 0; stage_ndx < PEM_NOF_SOFT_STAGES; ++stage_ndx){
    for (field_id = 0; field_id < PEM_DEBUG_MAX_FIELD_ID; ++field_id){
      pem_debug_info.debug_info_arr[stage_ndx].fifo_desc_arr[field_id] = NULL;
    }
    for (context = 0; context < PEM_NOF_CONTEXTS; ++context){
      pem_debug_info.debug_info_arr[stage_ndx].dbus_desc_per_context_arr[context].dump_packet_header = 0;
      pem_debug_info.debug_info_arr[stage_ndx].dbus_desc_per_context_arr[context].dump_header_offsets_array = 0;
      pem_debug_info.debug_info_arr[stage_ndx].dbus_desc_per_context_arr[context].dump_qualifiers_array = 0;
    for (field_id = 0; field_id < PEM_DEBUG_MAX_FIELD_ID; ++field_id){      
      pem_debug_info.debug_info_arr[stage_ndx].dbus_desc_per_context_arr[context].load_dbus_desc_arr[field_id] = NULL;
    }
    for (command_id = 0; command_id < PEM_DEBUG_NOF_UPDATE_COMMANDS      ; ++command_id){            
      pem_debug_info.debug_info_arr[stage_ndx].dbus_desc_per_context_arr[context].update_commands_arr[command_id] = NULL;
    }
    }  
  }

    return 0;
}

/********************************************************************************************************************************************************************/
/*read ucode and create DB - that is:
                                    1. Fifo desc per stage
                                    2. LOAD dbus desc per stage per context
                                    3. UPDATE dbus desc per stage per context
                                    4. FifoConfig reg and Fifo mem address
                                    5. LOAD dbus names for read
                                    6. UPDATE dbus names for read */

void debug_load_dbus_info_insert(const char* line)
{
  char key_word[MAX_NAME_LENGTH], debug_load_or_update_command[MAX_NAME_LENGTH], debug_name[MAX_NAME_LENGTH];
  int debug_stage, debug_context, debug_field_id, debug_field_width, debug_lsb_location_on_dbus_or_fifo, debug_field_msb, debug_field_lsb;

  if (sscanf(line, "%s %d %d %d %d %d %d %d %s %s", key_word, &debug_stage, &debug_context, &debug_field_id,
    &debug_field_width, &debug_field_msb, &debug_field_lsb, &debug_lsb_location_on_dbus_or_fifo,
    debug_load_or_update_command, debug_name) != DEBUG_NOF_LOAD_DESC_TOKEN){
       printf("Bad line format. Skip and continue with next line.\n");
  } else{
    add_field_desc_on_load_dbus(debug_stage, debug_context, debug_field_id, debug_name, debug_field_width, debug_field_msb, debug_field_lsb, 
      debug_lsb_location_on_dbus_or_fifo, debug_load_or_update_command); 
  }
}

void debug_update_dbus_info_insert(const char* line)
{
  char key_word[MAX_NAME_LENGTH], debug_load_or_update_command[MAX_NAME_LENGTH], debug_name[MAX_NAME_LENGTH];
  int debug_stage, debug_context, debug_field_id, debug_field_width, debug_lsb_location_on_dbus_or_fifo, idx_offset;

  if (sscanf(line, "%s %d %d %d %d %d %d %s %s", key_word, &debug_stage, &debug_context,
    &debug_field_id, &debug_field_width, &debug_lsb_location_on_dbus_or_fifo, &idx_offset,
    debug_load_or_update_command, debug_name) != DEBUG_NOF_UPDATE_DESC_TOKEN){
      printf("Bad line format. Skip and continue with next line.\n");
  } else{
    add_update_command_desc(debug_stage, debug_context, debug_field_id, debug_name, debug_field_width, debug_lsb_location_on_dbus_or_fifo, idx_offset, debug_load_or_update_command); 
  }
}

void debug_fifo_field_desc_info_insert(const char* line)
{
  char key_word[MAX_NAME_LENGTH], debug_name[MAX_NAME_LENGTH];
  int debug_stage, debug_field_id, debug_field_width, debug_lsb_location_on_dbus_or_fifo, debug_exists_for_load, debug_exists_for_update;

  if (sscanf(line, "%s %d %d %d %d %d %d %s", key_word, &debug_stage, &debug_field_id, 
    &debug_field_width, &debug_lsb_location_on_dbus_or_fifo, &debug_exists_for_load,
    &debug_exists_for_update, debug_name) != DEBUG_NOF_FIFO_DESC_TOKEN){
        printf("Bad line format. Skip and continue with next line.\n");
  } else{
    add_field_desc_on_fifo(debug_stage, debug_field_id, debug_name, debug_field_width, debug_lsb_location_on_dbus_or_fifo, debug_exists_for_load, debug_exists_for_update);
  }
}

void debug_fifoconfig_reg_info_insert(const char* line)
{
  char key_word[MAX_NAME_LENGTH], reg_name[MAX_NAME_LENGTH], address_s[MAX_NAME_LENGTH];
  int stage_num;
  unsigned int address, block_id, entry_width;

  if (sscanf(line, "%s %d %s %s %u", key_word, &stage_num, reg_name, address_s, &entry_width) != DEBUG_NOF_FIFOCONFIG_REG_INFO_TOKEN){
    printf("Bad line format. Skip and continue with next line.\n");
  } else{
    address = qax_hexstr2addr(address_s, &block_id);
    add_fifoconfig_reg_info(stage_num, reg_name, address, block_id, entry_width);
  }
}


void debug_fifo_mem_info_insert(const char* line)
{
  char key_word[MAX_NAME_LENGTH], reg_name[MAX_NAME_LENGTH], address_s[MAX_NAME_LENGTH];
  int stage_num;
  unsigned int address, block_id, entry_width;

  if (sscanf(line, "%s %d %s %s %u", key_word, &stage_num, reg_name, address_s, &entry_width) != DEBUG_NOF_FIFO_MEM_INFO_TOKEN){
    printf("Bad line format. Skip and continue with next line.\n");
  } else{
    address = qax_hexstr2addr(address_s, &block_id);
    add_fifo_mem_info(stage_num, reg_name, address, block_id, entry_width);
  }
}

void debug_load_dbus_name_info_insert(const char* line)
{
  char key_word[MAX_NAME_LENGTH], dbus_name[MAX_NAME_LENGTH];
  int stage_num;
  char* dbus_name_p;

  if (sscanf(line, "%s %d %s", key_word, &stage_num, dbus_name) != DEBUG_NOF_DEBUG_BUS_NAME_TOKEN){
    printf("Bad line format. Skip and continue with next line.\n");
  } else{
    /*copy name to database*/
    dbus_name_p = pem_debug_info.debug_info_arr[stage_num].load_dbus_debug_name;
    strcpy(dbus_name_p, dbus_name);
  }
}

void debug_update_dbus_name_info_insert(const char* line)
{
  char key_word[MAX_NAME_LENGTH], dbus_name[MAX_NAME_LENGTH];
  int stage_num;
  char* dbus_name_p;

  if (sscanf(line, "%s %d %s", key_word, &stage_num, dbus_name) != DEBUG_NOF_DEBUG_BUS_NAME_TOKEN){
    printf("Bad line format. Skip and continue with next line.\n");
  } else{
    /*copy name to database*/
    if(stage_num >= 0)
    {
      dbus_name_p = pem_debug_info.debug_info_arr[stage_num].update_dbus_debug_name;
      strcpy(dbus_name_p, dbus_name);
    }
  }
}

void debug_cbus_dbus_name_info_insert(const char* line)
{
    char key_word[MAX_NAME_LENGTH], dbus_name[MAX_NAME_LENGTH];
  int stage_num;
  char* dbus_name_p;

  if (sscanf(line, "%s %d %s", key_word, &stage_num, dbus_name) != DEBUG_NOF_DEBUG_BUS_NAME_TOKEN){
    printf("Bad line format. Skip and continue with next line.\n");
  } else{
    /*copy name to database*/
    dbus_name_p = pem_debug_info.debug_info_arr[stage_num].cbus_debug_name;
    strcpy(dbus_name_p, dbus_name);
  }
}

int add_field_desc_on_load_dbus  (int stage_num, int context, int field_id, char* name, int width, int field_msb, int field_lsb, int lsb_dbus_location, char* load_command){
  int insertion_index = (field_id + PEM_DEBUG_MAX_FIELD_ID) % PEM_DEBUG_MAX_FIELD_ID;
  /*allocate a new dbus_descriptor*/
  pem_load_dbus_desc_t* dbus_desc = (pem_load_dbus_desc_t*)sal_alloc(sizeof(pem_load_dbus_desc_t), "dbus_desc");
  if (NULL == dbus_desc) {
    free(dbus_desc);
    return -1;
  }
  /*allocate mem for name and copy*/
  dbus_desc->field_or_array_name = (char*)sal_alloc(MAX_NAME_LENGTH, "dbus_desc->field_or_array_name");
  if (NULL == dbus_desc->field_or_array_name){
    free(dbus_desc->field_or_array_name);
    free(dbus_desc);
    return -1;
  }
  strcpy(dbus_desc->field_or_array_name, name);
  /*fill descriptor*/
  dbus_desc->field_width = width;
  dbus_desc->field_msb_on_dbus = field_msb;
  dbus_desc->field_lsb_on_dbus = field_lsb;
  dbus_desc->lsb_location_on_dbus = lsb_dbus_location;

  /*mark if header, foi, arrays should be dumped*/
  if (0 == strncmp(load_command, "SELECT_HEADER_BY_VAR_INDEX", MAX_NAME_LENGTH)){
    pem_debug_info.debug_info_arr[stage_num].dbus_desc_per_context_arr[context].dump_packet_header = 1;
    pem_debug_info.debug_info_arr[stage_num].dbus_desc_per_context_arr[context].dump_header_offsets_array = 1;
    pem_debug_info.debug_info_arr[stage_num].dbus_desc_per_context_arr[context].dump_forwarding_offset_idx = 1;
  } 
  else if (0 == strncmp(load_command, "SELECT_HEADER_BY_CONST_INDEX", MAX_NAME_LENGTH)){
    pem_debug_info.debug_info_arr[stage_num].dbus_desc_per_context_arr[context].dump_packet_header = 1;
    pem_debug_info.debug_info_arr[stage_num].dbus_desc_per_context_arr[context].dump_header_offsets_array = 1;
  }
  else if (0 == strncmp(load_command, "SELECT_ARRAY_ENTRY_BY_VAR_INDEX", MAX_NAME_LENGTH)){
    /*can optimize and check array name*/
    pem_debug_info.debug_info_arr[stage_num].dbus_desc_per_context_arr[context].dump_header_offsets_array = 1;
    pem_debug_info.debug_info_arr[stage_num].dbus_desc_per_context_arr[context].dump_qualifiers_array = 1;
  }

  /*insert to DB*/
  pem_debug_info.debug_info_arr[stage_num].dbus_desc_per_context_arr[context].load_dbus_desc_arr[insertion_index] = dbus_desc;
  
  return 0;
}


int add_update_command_desc(int stage_num, int context, int field_id, char* name, int width, int lsb_dbus_location, int idx_offset, char* update_command_s){  
  static int curr_command_id = 0; /*current arary entry to insert command*/
  /*allocate a new update command descriptor*/
  pem_update_command_t* update_command = (pem_update_command_t*)sal_alloc(sizeof(pem_update_command_t), "update_command");
  
  if (NULL == update_command){
    free(update_command);
    return -1;
  }
  /*allocate mem for name and copy*/
  update_command->field_or_array_name = (char*)sal_alloc(MAX_NAME_LENGTH, "update_command->field_or_array_name");
  if (NULL == update_command->field_or_array_name){
    free(update_command->field_or_array_name);
    free(update_command);
    return -1;
  }
  strcpy(update_command->field_or_array_name, name);
  /*fill descriptor*/
  update_command->field_width = width;
  update_command->field_id = field_id;
  update_command->idx_offset = idx_offset;
  update_command->lsb_location_on_dbus = lsb_dbus_location;
  update_command->is_set_relative = (0 == strncmp(update_command_s, "STORE_ARRAY_ENTRY_BY_VAR", MAX_NAME_LENGTH)) ? 1 : 0;
   
  /*insert to DB*/
  pem_debug_info.debug_info_arr[stage_num].dbus_desc_per_context_arr[context].update_commands_arr[curr_command_id] = update_command;
  /*advance command id for next insertion*/
  ++curr_command_id;

  return 0;
}

int add_field_desc_on_fifo(int stage_num, int field_id, char* name, int width, int lsb_loc_on_fifo, int exists_for_load, int exists_for_update){
  int insertion_index = (field_id + PEM_DEBUG_MAX_FIELD_ID) % PEM_DEBUG_MAX_FIELD_ID;
  /*allocate a new dbus_descriptor*/
  pem_fifo_desc_t* fifo_desc = (pem_fifo_desc_t*)sal_alloc(sizeof(pem_fifo_desc_t), "fifo_desc");
  
  if (NULL == fifo_desc) return -1;
  /*allocate mem for name and copy*/
  fifo_desc->field_name = (char*)malloc(MAX_NAME_LENGTH);
  if (NULL == fifo_desc->field_name){
    free(fifo_desc);
    return -1;
  }
  strcpy(fifo_desc->field_name, name);
  /*fill descriptor*/
  fifo_desc->field_id = field_id;
  fifo_desc->width_in_bits = width;
  fifo_desc->fifo_lsb_loaction = lsb_loc_on_fifo;
  fifo_desc->exists_for_load = exists_for_load;
  fifo_desc->exists_for_update = exists_for_update;

  /*insert to DB*/
  pem_debug_info.debug_info_arr[stage_num].fifo_desc_arr[insertion_index] = fifo_desc;

  return 0;
}

int add_fifoconfig_reg_info(int stage_num, char* name, unsigned int address, unsigned int block_id, unsigned int entry_width)
{
  pem_debug_reg_mem_info_t* reg = (pem_debug_reg_mem_info_t*)sal_alloc(sizeof(pem_debug_reg_mem_info_t), "reg");
  if (NULL == reg){
    free(reg);
    return -1;
  }
  /*allocate mem for name and copy*/
  reg->name = (char*)sal_alloc(MAX_NAME_LENGTH, "reg->name");
  if (NULL == reg->name) {
    free(reg->name);
    free(reg);
    return -1;
  }
  strcpy(reg->name, name);

  reg->address = address;
  reg->block_id = block_id;
  reg->entry_width = entry_width;

  pem_debug_info.debug_info_arr[stage_num].fifo_config_reg = reg;

  return 0;
}

int add_fifo_mem_info(int stage_num, char* name, unsigned int address, unsigned int block_id, unsigned int entry_width)
{
  pem_debug_reg_mem_info_t* mem = (pem_debug_reg_mem_info_t*)sal_alloc(sizeof(pem_debug_reg_mem_info_t),"mem");
  if (NULL == mem){
    free(mem);
    return -1;
  }
  /*allocate mem for name and copy*/
  mem->name = (char*)sal_alloc(MAX_NAME_LENGTH, "mem->name");
  if (NULL == mem->name){
    free(mem->name);
    free(mem);
    return -1;
  }
  strcpy(mem->name, name);

  mem->address = address;
  mem->block_id = block_id;
  mem->entry_width = entry_width;

  pem_debug_info.debug_info_arr[stage_num].fifo_mem = mem;

  return 0;
}


/******************************************************************************************************************************************************/
/*get context and foi*/

int get_last_packet_context(const int stage_num) {
  int cbus_val;

  char *name = pem_debug_info.debug_info_arr[stage_num].cbus_debug_name;
  /*read cbus from design*/
  if(!dpp_dsig_read(0, 0, NULL, NULL, NULL, name, (uint32 *)&cbus_val, 1))
      return -1;

  /*5 lower bits are the context*/
  return (cbus_val & PEM_DEBUG_CBUS_CONTEXT_MASK);
}

int get_forwarding_offset_index_for_last_packet(const int stage_num){
    int field_id;
    unsigned int fifo_entry[PEM_DEBUG_NOF_ENTRIES_FOR_FIFO_UINT_ARR];
    unsigned int* read_data;
    int foi;
    int mask = 0x7; /*3 bits field*/
    
    /*do actual read from design of fifo*/
    if (-1 == read_fifo_and_dbuses(stage_num, NULL, NULL, fifo_entry, 0))
      return -1;

    /*look for field by name*/
    for (field_id = 0; field_id < PEM_DEBUG_MAX_FIELD_ID; ++field_id)
    {
      /*search for field*/
      if (pem_debug_info.debug_info_arr[stage_num].fifo_desc_arr[field_id] == NULL)
        continue;
      /*else, if field_name is current field, break*/
      else if (!strcmp("forwarding_offset_index", pem_debug_info.debug_info_arr[stage_num].fifo_desc_arr[field_id]->field_name) &&
               pem_debug_info.debug_info_arr[stage_num].fifo_desc_arr[field_id]->fifo_lsb_loaction != -1){
        read_data = pem_debug_read_data_from_fifo(pem_debug_info.debug_info_arr[stage_num].fifo_desc_arr[field_id], fifo_entry);
        foi = *read_data & mask;
        free(read_data);
        return foi;
      }
    }

    return -1;
}

/******************************************************************************************************************************************************/
/*Read from LOAD/UPDATE dbus or Fifo*/

/*dbus debug data comes in bytes array - this function copies it to an unsigned int array*/
static void copy_uint8_arr_to_uint_arr(const uint8* in, unsigned int* out, int nof_entries_for_input_arr){
  /*assume array sizes are correct*/
  int out_ndx = 0, in_ndx = 0;
  unsigned int temp_val = 0, curr_in_arr_entry_uint32 = 0;
  const int in_2_out_proportion = sizeof(unsigned int) / sizeof(uint8);

  for (in_ndx = 0; in_ndx < nof_entries_for_input_arr; ++in_ndx){
    /*take 4B from in to a temp value (in 4 iterations). shift curr entry 0-3B*/
    curr_in_arr_entry_uint32 = in[in_ndx] << (in_ndx*8);
    temp_val |= curr_in_arr_entry_uint32;    
    /*insert value to out arr*/
    if (in_ndx % in_2_out_proportion == in_2_out_proportion-1)
    {
      out[out_ndx++] = temp_val;
      temp_val = 0;
    }
  }
  /*when the following cond is met, temp_val is not set to out arr*/
  /*it is only set each 4th iteration then ndx mod proportion is 0.*/
  if (in_ndx % in_2_out_proportion != 0)
    out[out_ndx] = temp_val;
}

int read_fifo_and_dbuses(const int stage_num, unsigned int* load_dbus, unsigned int* update_dbus, unsigned int* fifo_entry, int entry_offset_in_fifo){
  phy_mem_t fifoconfig_reg_read_info, fifo_mem_read_info;
  unsigned int fifo_config_reg_val = 0, fifo_entry_to_read;

  /* will contain signal value */
  uint32 value;

  char* load_dbus_debug_name = pem_debug_info.debug_info_arr[stage_num].load_dbus_debug_name;
  char* update_dbus_debug_name = pem_debug_info.debug_info_arr[stage_num].update_dbus_debug_name;

  if (!(entry_offset_in_fifo >= 0 && entry_offset_in_fifo <= 63)) /*check for legal offset - can only read past 64 packets*/
    return -1;

  /*read load dbus if not null*/
  if (NULL != load_dbus){
    if(!dpp_dsig_read(0, 0, NULL, NULL, NULL, load_dbus_debug_name, &value, 1))
      return -1;

    /*copy to output load_dbus array*/
    copy_uint8_arr_to_uint_arr((uint8 *)(&value), load_dbus, PEM_DEBUG_NOF_ENTRIES_FOR_DBUS_UINT_ARR);
  }
  /*read update dbus if not null*/
  if (NULL != update_dbus){
    if(!dpp_dsig_read(0, 0, NULL, NULL, NULL, update_dbus_debug_name, &value, 1))
      return -1;
    /*copy to output update_dbus array*/
    copy_uint8_arr_to_uint_arr((uint8 *)(&value), update_dbus, PEM_DEBUG_NOF_ENTRIES_FOR_DBUS_UINT_ARR);
  }

  /*read fifo entry if not null*/
  if (NULL != fifo_entry){
    /*fill data to read FifoConfig register*/
    fifoconfig_reg_read_info.block_identifier  = pem_debug_info.debug_info_arr[stage_num].fifo_config_reg->block_id;
    fifoconfig_reg_read_info.mem_address       = pem_debug_info.debug_info_arr[stage_num].fifo_config_reg->address;
    fifoconfig_reg_read_info.mem_width_in_bits = pem_debug_info.debug_info_arr[stage_num].fifo_config_reg->entry_width;
    fifoconfig_reg_read_info.row_index         = 0;
    /*read FifoConfig register*/
    if (phy_pem_mem_read(0, &fifoconfig_reg_read_info, &fifo_config_reg_val))
      return -1;
    /*calculate which entry to read*/
    fifo_entry_to_read = (fifo_config_reg_val & PEM_DEBUG_FIFO_LAST_POINTER_ENTRY_MASK) >> PEM_DEBUG_FIFO_CONFIG_LAST_POINTER_ENTRY_LSB_LOC;
    fifo_entry_to_read = (fifo_entry_to_read - entry_offset_in_fifo + PEM_DEBUG_FIFO_NOF_ENTRIES - 1) % PEM_DEBUG_FIFO_NOF_ENTRIES;
    /*fill data to read Fifo entry*/
    fifo_mem_read_info.block_identifier  = pem_debug_info.debug_info_arr[stage_num].fifo_mem->block_id;
    fifo_mem_read_info.mem_address       = pem_debug_info.debug_info_arr[stage_num].fifo_mem->address;
    fifo_mem_read_info.mem_width_in_bits = pem_debug_info.debug_info_arr[stage_num].fifo_mem->entry_width;
    fifo_mem_read_info.row_index         = fifo_entry_to_read;
    /*read Fifo entry*/
    if (phy_pem_mem_read(0, &fifo_mem_read_info, fifo_entry))
      return -1;
  }

  return 0;
}


/***********************************************************************************************************************************************/
/*SHIFT ARRAYS RIGHT*/
static void shift_uint_array_1_bit_right(unsigned int* arr, const int arr_nof_entries){
  int entry_ndx = arr_nof_entries-1;
  unsigned int mask = 0;
  unsigned int mask_msb_bit_is_1 = (unsigned int)(1<<31);
  unsigned int mask_msb_bit_is_0 = 0;
  int lsb_bit_of_curr_entry;
  for (;entry_ndx >= 0; --entry_ndx){
    /*check lsb bit as it is msb of next (lower) entry*/
    lsb_bit_of_curr_entry = arr[entry_ndx] & 0x1;
    /*shift entry right by 1*/
    arr[entry_ndx] >>= 1;
    /*set msb bit of current entry to lsb of previous one*/
    arr[entry_ndx] |= mask;
    /*calc mask for next entry*/
    mask = (0x1 == lsb_bit_of_curr_entry) ? mask_msb_bit_is_1 : mask_msb_bit_is_0;
  }
}

static void shift_uint_array_n_bits_right(unsigned int* arr, const int arr_nof_entries ,const int shift_amount){
  int shift_ndx = 0;
  for (shift_ndx = 0; shift_ndx < shift_amount; ++shift_ndx)
    shift_uint_array_1_bit_right(arr, arr_nof_entries);
}
/***********************************************************************************************************************************************/
/*GET FIELDS' VALUES FROM DESIGN*/

static unsigned int* copy_from_bus_to_field(const int lsb_bit_of_field, const int msb_bit_of_field, const unsigned int* bus){
  /*in which dbus arr entries the field sits*/
  const unsigned int lsb_bit_entry_in_dbus_arr = lsb_bit_of_field / PEM_DEBUG_BITS_IN_UINT;
  const unsigned int msb_bit_entry_in_dbus_arr = msb_bit_of_field / PEM_DEBUG_BITS_IN_UINT;
  /*first copy relevant entries from dbus array to field array, then shift so that lsb sits on lsb*/
  const int field_shift_amount = lsb_bit_of_field % PEM_DEBUG_BITS_IN_UINT;
  /*allocate mem for field*/
  const int field_arr_size_in_arr_entries = msb_bit_entry_in_dbus_arr - lsb_bit_entry_in_dbus_arr + 1;
  unsigned int* field_val = (unsigned int*)malloc(sizeof(unsigned int) * field_arr_size_in_arr_entries); 
  unsigned int dbus_entry_ndx, field_entry_ndx;
  /*copy data from dbus to 'field_val'*/
  for (dbus_entry_ndx = lsb_bit_entry_in_dbus_arr, field_entry_ndx = 0; dbus_entry_ndx <= msb_bit_entry_in_dbus_arr; ++dbus_entry_ndx, ++field_entry_ndx){
    field_val[field_entry_ndx] = bus[dbus_entry_ndx];
  }
  /*shift right until lsb bit sits on lsb of entry 0*/
  shift_uint_array_n_bits_right(field_val, field_arr_size_in_arr_entries, field_shift_amount);
  
  return field_val;
}

unsigned int* pem_debug_read_data_from_load_dbus(const pem_load_dbus_desc_t* dbus_desc, const unsigned int* dbus){
  const int lsb_bit_of_field = dbus_desc->field_lsb_on_dbus;
  const int msb_bit_of_field = dbus_desc->field_msb_on_dbus;

  return copy_from_bus_to_field(lsb_bit_of_field, msb_bit_of_field, dbus);
}

unsigned int* pem_debug_read_data_from_update_dbus(const pem_update_command_t* update_command, const unsigned int* dbus){
  const int lsb_bit_of_field = update_command->lsb_location_on_dbus;
  const int msb_bit_of_field = lsb_bit_of_field + update_command->field_width - 1;

  return copy_from_bus_to_field(lsb_bit_of_field, msb_bit_of_field, dbus);
}

unsigned int* pem_debug_read_data_from_fifo(const pem_fifo_desc_t* fifo_desc, const unsigned int* fifo_entry){
  const int lsb_bit_of_field = fifo_desc->fifo_lsb_loaction;
  const int msb_bit_of_field = lsb_bit_of_field + fifo_desc->width_in_bits - 1;

  return copy_from_bus_to_field(lsb_bit_of_field, msb_bit_of_field, fifo_entry);
}

/******************************************************************************************************************************************************/
/*Print some field to file*/

int pem_debug_print_field(const char* field_name, const unsigned int* field_value, int nof_bits_in_value, int stage, int context, const char* line_prefix, const char* line_postfix){
  unsigned int nof_hex_chars;
  unsigned int mask, char_index, curr_char_loc, curr_value_ndx, curr_char_value;
  unsigned int last_char_mask = (nof_bits_in_value % 4 == 0) ? 0xf : ((1 << (nof_bits_in_value % 4)) - 1); /*mask for msb hex char in case too much bits are given in field_value*/
  char* hex_value = NULL;
  char curr_char;
  
  /*if input value not in fifo and not in load dbus then lost*/
  if (field_value == NULL){
    printf("%s %d %d %s %d'h%s \r\n", line_prefix, stage, context, field_name, nof_bits_in_value, "?");
    return 0;
  }

  nof_hex_chars = (nof_bits_in_value + 3) / 4;
  hex_value = (char*)malloc(sizeof(char)*nof_hex_chars+1);
  mask = 0xf;
  curr_char_loc = nof_hex_chars-1;
  curr_value_ndx = 0;

  if (hex_value == NULL)
    return -1;

  /*convert to hex str */
  for (char_index = 0; char_index < nof_hex_chars; ++char_index, --curr_char_loc){
    /*ndx in value_arr is */
    curr_value_ndx = char_index / 8;
    /*reset mask for next array entry*/
    if (char_index % 8 == 0) 
      mask = 0xf;
    /*take 4 next bits and shift right to 4 lsbs*/
    curr_char_value = (field_value[curr_value_ndx] & mask) >> (char_index%8 * 4);
    /*for last char, mask lower bits in case not zeros*/
    if (char_index == nof_hex_chars-1)
      curr_char_value &= last_char_mask;
    /*map 4 bits to hexa char*/
    curr_char = (curr_char_value < 10) ? (char)curr_char_value + '0' :
                                         (char)curr_char_value + 'A' - 10;
    hex_value[curr_char_loc] = curr_char;
    mask = mask << 4;
  }
  hex_value[nof_hex_chars] = '\0';
  /*printf value and free mem*/
  printf("%s %d %d %s %d'h%s %s\r\n", line_prefix, stage, context, field_name, nof_bits_in_value, hex_value, line_postfix);
  free (hex_value);

  return 0;
}

/******************************************************************************************************************************************************/

