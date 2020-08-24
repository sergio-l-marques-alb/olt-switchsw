#ifndef _Q2A_INTR_CB_FUNC_H_
#define _Q2A_INTR_CB_FUNC_H_

/*************
 * INCLUDES  *
 *************/

/*************
 * FUNCTIONS *
 *************/
/* init function - add handler function to db */
void q2a_interrupt_cb_init(
    int unit);

int q2a_intr_handle_generic_none(
    int unit,
    int block_instance,
    uint32 en_jer_interrupt,
    char *msg);
int q2a_intr_recurring_action_handle_generic_none(
    int unit,
    int block_instance,
    uint32 en_jer_interrupt,
    char *msg);
#endif /* _Q2A_INTR_CB_FUNC_H_ */
