
#ifndef _J2C_INTR_CB_FUNC_H_
#define _J2C_INTR_CB_FUNC_H_

/*************
 * INCLUDES  *
 *************/

/*************
 * FUNCTIONS *
 *************/
/* init function - add handler function to db */
void j2c_interrupt_cb_init(
    int unit);

int j2c_intr_handle_generic_none(
    int unit,
    int block_instance,
    uint32 en_jer_interrupt,
    char *msg);
int j2c_intr_recurring_action_handle_generic_none(
    int unit,
    int block_instance,
    uint32 en_jer_interrupt,
    char *msg);
#endif /* _J2C_INTR_CB_FUNC_H_ */
