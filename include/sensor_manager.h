#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Arduino.h>

extern float roll;
extern float pitch;

void sensorsInit();
void sensorsUpdate();
void sensorsSetCpuLoad(int cpuLoadPercent);
#endif