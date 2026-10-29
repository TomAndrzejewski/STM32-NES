/*
 * BatteryControl.h
 *
 *  Created on: 27 paz 2026
 *      Author: tomasz
 */

#ifndef SOURCES_INC_BATTERYCONTROL_H_
#define SOURCES_INC_BATTERYCONTROL_H_

#include <stdint.h>

void BATTERY_Init(void);
int  BATTERY_GetVoltage(float* voltage);


#endif // SOURCES_INC_BATTERYCONTROL_H_
