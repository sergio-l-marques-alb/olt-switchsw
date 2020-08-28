

/******************************************************************************
*
* FILE
*
*
* DESCRIPTION
*
*
* NOTE
*
*
******************************************************************************/

#ifndef SAL_H
#define SAL_H

/******************************************************************************
*INCLUDE FILES
******************************************************************************/



/******************************************************************************
*CONSTANT AND MACRO DEFINE
******************************************************************************/

/******************************************************************************
*VARIABLE DEFINE
******************************************************************************/



/******************************************************************************
*LOCAL FUNCTION PROTOTYPE DECLARATION
******************************************************************************/



/******************************************************************************
*GLOBAL FUNCTION DEFINITION
******************************************************************************/
#ifdef __cplusplus
extern "C" 
{
#endif
extern void time_delay_ns(unsigned long int delay_conut);
extern void mod_print(const char *data, ...);
extern int sys_init(void);
extern int parse_line(char *line, char *argv[]);
extern void table_print(char *string, unsigned long int print_val);
extern void time_delay(unsigned long int delay_conut);
extern void diag_print_title(const char *title, ...);
extern void diag_print_2_title(const char *title1, const char *title2);
extern void diag_cnt_val_to_str(unsigned int *cnt_val, int val_dep, int is_hex, char *str_buff, int buff_len);
extern void diag_print_cnt(const char *cnt1, char *val1, const char *cnt2, char *val2);
extern void diag_print_line(void);
#ifdef __cplusplus
}
#endif


#endif
