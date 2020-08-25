
#ifndef _JER2_INTR_CB_FUNC_H_
#define _JER2_INTR_CB_FUNC_H_

/*************
 * INCLUDES  *
 *************/

/*************
 * FUNCTIONS *
 *************/
/* init function - add handler function to db */
void jer2_interrupt_cb_init(
    int unit);

int jer2_intr_handle_generic_none(
    int unit,
    int block_instance,
    uint32 en_jer_interrupt,
    char *msg);
int jer2_intr_recurring_action_handle_generic_none(
    int unit,
    int block_instance,
    uint32 en_jer_interrupt,
    char *msg);
#endif /* _JER2_INTR_CB_FUNC_H_ */
