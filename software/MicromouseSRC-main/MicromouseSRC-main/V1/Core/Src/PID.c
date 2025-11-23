/*
 * PID.c
 *
 *  Created on: Mar 7, 2025
 *      Author: kinhz
 */

#include "PID.h"

PID pid_forward_left;
PID pid_forward_right;
PID pid_steering;
PID pid_stable;
PID pid_turn_left;
PID pid_turn_right;
PID pid_wall_stable;
PID pid_wall_front_left;
PID pid_wall_front_right;

int16_t debug_pid;

void PID_init(PID *pid, float kp, float ki, float kd, float integrator, float prev_error) {
	pid->kp = kp;
	pid->ki = ki;
	pid->kd = kd;
	pid->integrator = integrator;
	pid->prev_error = prev_error;
}

float PID_calc(PID *pid, float set_point, float real_data, uint32_t delta_time) {
	float error = set_point - real_data;

	float P = (pid->kp) * error;

	pid->integrator += error * delta_time / 1000;
	float I = ( pid->integrator ) * ( pid->ki );

	float derivative = ( error - ( pid->prev_error ) ) * 1000 / delta_time;
	debug_pid = delta_time;
	float D = pid->kd * derivative;
	pid->prev_error = error;

	return P + I + D;
}

void PID_reset(PID *pid) {
	pid->integrator = 0;
	pid->prev_error = 0;
}
