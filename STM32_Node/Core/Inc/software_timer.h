/*
 * software_timer.h
 *
 *  Created on: 12 Apr 2026
 *      Author: LENOVO
 */

#ifndef INC_SOFTWARE_TIMER_H_
#define INC_SOFTWARE_TIMER_H_

#include "global.h"

void setTimer(int index, int duration);
int isTimeExpired(int index);
void timerRun();

#endif /* INC_SOFTWARE_TIMER_H_ */
