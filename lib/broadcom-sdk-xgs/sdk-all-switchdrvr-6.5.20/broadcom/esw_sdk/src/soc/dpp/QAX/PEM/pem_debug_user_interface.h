#ifndef PEM_DEBUG_USER_INTERFACE_H
#define PEM_DEBUG_USER_INTERFACE_H


/*  Dump all packet fields per stage with values
*/
int pem_debug_print_pem_access_all_inputs(int stage_num);
int pem_debug_print_pem_access_all_outputs(int stage_num);

/*  Dump all fields needed to create an app_sim test file with values
*/
int pem_debug_print_pem_access_relevant_inputs(int stage_num);
int pem_debug_print_pem_access_relevant_outputs(int stage_num);

/*  Dump selected fields with value
*/
int pem_debug_print_pem_access_single_input(int stage_num, const char* field_name);
int pem_debug_print_pem_access_single_output(int stage_num, const char* field_name);

/*  Dump Fifo entry
*/
int pem_debug_print_pem_access_packet_in_fifo(int stage_num, int packet_backwards_offset);  


/* Print all the existing (for input, output or both) fields per stage - without values
*/
int pem_debug_print_available_fields(int stage_num);





#endif

