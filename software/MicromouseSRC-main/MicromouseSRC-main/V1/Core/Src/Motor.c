/*
 * Motor.c
 *
 *  Created on: Feb 28, 2025
 *      Author: kinhz
 */

#include "Motor.h"

Motor motor_left;
Motor motor_right;

uint32_t prev_time_pid_forward = 0;
uint32_t prev_time_pid_rotation = 0;
int32_t debug_pwm = 0;

uint8_t MOUSE_STATE = 0;

void Motor_Init(Motor *motor, TIM_HandleTypeDef *tim1, TIM_HandleTypeDef *tim2,
		uint16_t channel1, uint16_t channel2) {
	motor->M_TIM1 = tim1;
	motor->M_TIM2 = tim2;
	motor->m_channel1 = channel1;
	motor->m_channel2 = channel2;

	HAL_TIM_PWM_Start(tim1, channel1);
	HAL_TIM_PWM_Start(tim2, channel2);
}

void Set_Motor_speed(Motor *motor, int16_t speed) {
	if (speed > 0) {
		__HAL_TIM_SET_COMPARE(motor->M_TIM1, motor->m_channel1, 0);
		__HAL_TIM_SET_COMPARE(motor->M_TIM2, motor->m_channel2, speed);
	} else {
		__HAL_TIM_SET_COMPARE(motor->M_TIM1, motor->m_channel1, -speed);
		__HAL_TIM_SET_COMPARE(motor->M_TIM2, motor->m_channel2, 0);
	}
}

void Move_to_middle(float distance, float speed) { // distance in cm ,speed in mm/s
//	int16_t count = 0;
	reset_en();

	speed = mm_to_cnt(speed);
	distance = mm_to_cnt(distance);
	prev_time_pid_forward = Millis;
	prev_time_pid_rotation = Millis;
	read_ir_adc();
//	uint32_t curt = Micros;
	PID_reset(&pid_steering);
	PID_reset(&pid_wall_stable);
	PID_reset(&pid_forward_right);
	PID_reset(&pid_forward_left);
	while ((distance > encoder_cnt_left) || (distance > encoder_cnt_right)) {
		int16_t left_pwm = PID_calc(&pid_forward_left, speed, speed_left,
				Millis - prev_time_pid_forward);
		int16_t right_pwm = PID_calc(&pid_forward_right, speed, speed_right,
				Millis - prev_time_pid_forward);
		prev_time_pid_forward = Millis;
		int16_t error_steering = PID_calc(&pid_steering, 0,
				encoder_cnt_left - encoder_cnt_right,
				Millis - prev_time_pid_rotation);
		prev_time_pid_rotation = Millis;
		left_pwm += error_steering;
		right_pwm += -error_steering;

		if (left_pwm > 10000)
			left_pwm = 10000;
		if (left_pwm < 0)
			left_pwm = 0;
		if (right_pwm > 10000)
			right_pwm = 10000;
		if (right_pwm < 0)
			right_pwm = 0;

		Set_Motor_speed(&motor_left, left_pwm);
		Set_Motor_speed(&motor_right, right_pwm);

//		elapseMillis(1000, curt);
//		curt = Micros;

		delay_ms(1);
//		read_ir_adc();
	}
	Motor_stop();
}

void Turn_left90() {
	angle = 0;
	uint32_t prev_time = micros();
	uint32_t curt = Millis;
	PID_reset(&pid_forward_right);
	PID_reset(&pid_forward_left);
	float target_angle = 13 + cnt_to_omega(encoder_cnt_left, encoder_cnt_right);
	while (fabs(target_angle - angle) > 4) {
		MPU6050_getAngle(MPU6050.I2Cx, micros() - prev_time);
		printf("%f\n\n", angle);
		int16_t pwm_target = 120 * (target_angle - angle);
		int16_t left_pwm = PID_calc(&pid_forward_left, -pwm_target, speed_left,
				micros() - prev_time);
		int16_t right_pwm = PID_calc(&pid_forward_right, pwm_target,
				speed_right, micros() - prev_time);
		prev_time = micros();
		if (left_pwm < -1400)
			left_pwm = -1400;
		if (right_pwm > 1400)
			right_pwm = 1400;
		Set_Motor_speed(&motor_left, left_pwm);
		Set_Motor_speed(&motor_right, right_pwm);

//		elapseMillis(2, curt);
//		curt = Millis;
		delay_ms(2);
	}
//	reset_en();
//	printf("%f\n", angle);
	Motor_stop();
//	delay_ms(20);
}

void Turn_right90() {
	angle = 0;

	reset_en();
	uint32_t prev_time = micros();
//	uint32_t prev_time_check = Millis;
	uint32_t curt = Millis;
	PID_reset(&pid_forward_right);
	PID_reset(&pid_forward_left);
	while (fabs(80 + angle - cnt_to_omega(encoder_cnt_left, encoder_cnt_right))
			> 4) {
		MPU6050_getAngle(MPU6050.I2Cx, micros() - prev_time);

		int16_t pwm_target = 25 * (80 + angle);
		int16_t left_pwm = PID_calc(&pid_forward_left, pwm_target, speed_left,
				micros() - prev_time);
		int16_t right_pwm = PID_calc(&pid_forward_right, -pwm_target,
				speed_right, micros() - prev_time);
		prev_time = micros();
		if (left_pwm > 1600)
			left_pwm = 1600;
		if (right_pwm < -1600)
			right_pwm = -1600;
		Set_Motor_speed(&motor_left, left_pwm);
		Set_Motor_speed(&motor_right, right_pwm);

		elapseMillis(2, curt);
		curt = Millis;
	}
//	reset_en();
	Motor_stop();
//	delay_ms(20);
//	printf("%f\n", angle);
}

void Motor_stop() {
	Set_Motor_speed(&motor_right, 0);
	Set_Motor_speed(&motor_left, 0);
}

void Adjuster() {
//	int curt = Millis;
	int32_t curt = micros();
	while ((fabs(IR_FL_raw - WALL_FL) > 200)
			|| (fabs(WALL_FR - IR_FR_raw) > 200)) {
		read_ir_adc();
		Set_Motor_speed(&motor_right, 2000);
		Set_Motor_speed(&motor_left, 2000);
		elapseMicros(1000, curt);
	}
	Motor_stop();
}

void Move_onecell() {
//	reset_en();
//	calc_extra_cnt();

//	if (floodFill() != FORWARD) state = 0;
	if (get(&mouse_state, mouse_state.index - 1) != 1) {
		PID_reset(&pid_steering);
		PID_reset(&pid_wall_stable);
		PID_reset(&pid_forward_right);
		PID_reset(&pid_forward_left);
	}
	uint32_t prev_time = micros();
	uint32_t distance = mm_to_cnt(onecell);
	int16_t right_pwm;
	int16_t left_pwm;
	while ((distance > encoder_cnt_left) || (distance > encoder_cnt_right)) {
//			&& ((IR_FL < WALL_FL) || (IR_FR < WALL_FR))) {
		update_speed(micros() - prev_time, check_state());
		left_pwm = PID_calc(&pid_forward_left, mm_to_cnt(_SPEED), speed_left,
				micros() - prev_time);
//		debug_pid ++;
		right_pwm = PID_calc(&pid_forward_right, mm_to_cnt(_SPEED), speed_right,
				micros() - prev_time);
		int16_t error_steering = PID_calc(&pid_steering, 0,
				encoder_cnt_left - encoder_cnt_right, micros() - prev_time);
		int16_t error_wall = PID_calc(&pid_wall_stable, 0,
				wall_side_calibration(), micros() - prev_time);
		prev_time = micros();
		error_wall *= (speed_left + speed_right) / mm_to_cnt(2 * SPEED_MAX);
		error_wall = 0;
		left_pwm += error_steering - error_wall;
		right_pwm += -error_steering + error_wall;

		if (left_pwm > 10000)
			left_pwm = 10000;
		if (left_pwm < 0)
			left_pwm = 0;
		if (right_pwm > 10000)
			right_pwm = 10000;
		if (right_pwm < 0)
			right_pwm = 0;

		Set_Motor_speed(&motor_left, left_pwm);
		Set_Motor_speed(&motor_right, right_pwm);
		delay_ms(1);
	}
}

void Turn_left90_2() {
	PREV_TURNSPEED = 0;
	reset_en();
	PID_reset(&pid_forward_right);
	PID_reset(&pid_forward_left);
	PID_reset(&pid_steering);
	read_ir_adc();
	int8_t back_t = 0;
	if(IR_LL_raw > WALL_L - 200) {
		back_t = 1;
	}
	float angle_target = mm_to_cnt(perimeter) / 4 - 50 + CNT_EXT;
	int16_t left_pwm = 0, right_pwm = 0;
	int8_t state = 1;
	uint32_t prev_time = micros();
	while (-angle_target < encoder_cnt_left) {
		update_turnspeed(micros() - prev_time, state);
		left_pwm = PID_calc(&pid_forward_left, -TURN_SPEED,
				speed_left, micros() - prev_time);
		right_pwm = PID_calc(&pid_forward_right, TURN_SPEED,
				speed_right, micros() - prev_time);
		prev_time = micros();
		if (angle_target + encoder_cnt_left < 40)
			state = 0;



		Set_Motor_speed(&motor_left, left_pwm);
		Set_Motor_speed(&motor_right, right_pwm);
		delay_ms(2);
	}
	Motor_stop();
	if(back_t == 1)
		Back();
	Move_to_middle(40, 180);
	reset_encoder();
}
void Back() {
	Set_Motor_speed(&motor_right, -1400);
	Set_Motor_speed(&motor_left, -1400);
	delay_ms(500);
	Motor_stop();
	reset_encoder();
}

//void Turn_right90_2() {
//	float angle_error = encoder_cnt_left - encoder_cnt_right;
//	PID_reset(&pid_forward_right);
//	PID_reset(&pid_forward_left);
//	PID_reset(&pid_steering);
//	float angle_target = mm_to_cnt(perimeter) / 4;
//	int16_t left_pwm = 0, right_pwm = 0;
//	while ((fabs(angle_target - encoder_cnt_left) > 70)) {
//		left_pwm = (angle_target - encoder_cnt_left) * 4;
//		right_pwm = (-angle_target - encoder_cnt_right) * 4;
//
//		if(right_pwm < -1400) right_pwm = -1400;
//		if(left_pwm > 1400) left_pwm = 1400;
//		Set_Motor_speed(&motor_left, left_pwm);
//		Set_Motor_speed(&motor_right, right_pwm);
//
//		delay_ms(2);
//	}
//	Motor_stop();
//}

void Turn_right90_2() {
	PREV_TURNSPEED = 0;
	reset_encoder();
	PID_reset(&pid_forward_right);
	PID_reset(&pid_forward_left);
	PID_reset(&pid_steering);
	int8_t back_t = 0;
		if(IR_RR_raw > WALL_R - 200) {
			back_t = 1;
		}
	float angle_target = mm_to_cnt(perimeter) / 4 - 55 + CNT_EXT;
	int16_t left_pwm = 0, right_pwm = 0;
	int8_t state = 1;
	uint32_t prev_time = micros();
	while (angle_target > encoder_cnt_left) {
		update_turnspeed(micros() - prev_time, state);
		left_pwm = PID_calc(&pid_forward_left, TURN_SPEED,
				speed_left, micros() - prev_time);
		right_pwm = PID_calc(&pid_forward_right, -TURN_SPEED,
				speed_right, micros() - prev_time);
		prev_time = micros();
		if (angle_target - encoder_cnt_left < 50)
			state = 0;

		Set_Motor_speed(&motor_left, left_pwm);
		Set_Motor_speed(&motor_right, right_pwm);
		delay_ms(2);
	}
	Motor_stop();
	if(back_t == 1) {
		Back();
	}
	Move_to_middle(40, 180);
	reset_encoder();
}
