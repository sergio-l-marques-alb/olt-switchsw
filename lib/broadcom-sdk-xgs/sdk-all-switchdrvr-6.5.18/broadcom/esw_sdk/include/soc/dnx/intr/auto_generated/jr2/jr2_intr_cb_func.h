
#ifndef _JR2_INTR_CB_FUNC_H_
#define _JR2_INTR_CB_FUNC_H_

/*************
 * INCLUDES  *
 *************/

/*************
 * FUNCTIONS *
 *************/
/* init function - add handler function to db */
void jr2_interrupt_cb_init(
    int unit);

int jr2_intr_handle_generic_none(
    int unit,
    int block_instance,
    uint32 en_jer_interrupt,
    char *msg);
int jr2_intr_recurring_action_handle_generic_none(
    int unit,
    int block_instance,
    uint32 en_jer_interrupt,
    char *msg);
#endif /* _JR2_INTR_CB_FUNC_H_ */
