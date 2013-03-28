/*
 * ptin_control.h
 *
 * Created on: 2011/06/14 
 * By: Alexandre Santos (alexandre-r-santos@ptinovacao.pt) 
 * Notes: 
 */

#ifndef _PTIN_CONTROL_H
#define _PTIN_CONTROL_H

#include "ptin_include.h"


/**
 * Task that runs part of the PTin initialization and further periodic 
 * processing (alarms check) 
 * 
 * @param numArgs 
 * @param unit 
 */
extern void ptinTask(L7_uint32 numArgs, void *unit);

/**
 * Initialize alarms state
 * 
 */
extern void ptin_alarms_init(void);

#endif /* _PTIN_CONTROL_H */
