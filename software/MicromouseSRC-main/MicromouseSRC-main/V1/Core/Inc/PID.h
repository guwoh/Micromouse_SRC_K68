/*
 * PID.h
 *
 *  Created on: Mar 7, 2025
 *      Author: kinhz
 */

#ifndef INC_PID_H_
#define INC_PID_H_

#include "main.h"
#include "math.h"
#include "Sensor.h"
#include "Motor.h"

extern int16_t debug_pid;

typedef struct {
	float kp;
	float ki;
	float kd;
	float integrator;
	float prev_error;
} PID;

void PID_init(PID *pid, float kp, float ki, float kd, float integrator, float prev_error);
float PID_calc(PID *pid, float set_point, float real_data, uint32_t delta_time);
void PID_reset(PID *pid);

extern PID pid_forward_left;
extern PID pid_forward_right;
extern PID pid_steering;
extern PID pid_turn_left;
extern PID pid_turn_right;
extern PID pid_wall_stable;
extern PID pid_wall_front_left;
extern PID pid_wall_front_right;

#endif /* INC_PID_H_ */
