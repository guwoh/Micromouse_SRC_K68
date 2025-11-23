/*
 * Motor.h
 *
 *  Created on: Feb 28, 2025
 *      Author: kinhz
 */

#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_

#include "main.h"


extern uint32_t prev_time_pid_forward;
extern uint32_t prev_time_pid_rotation;
extern int32_t debug_pwm;
extern uint8_t MOUSE_STATE;

typedef struct {
	TIM_HandleTypeDef *M_TIM1;
	TIM_HandleTypeDef *M_TIM2;
	uint16_t m_channel1;
	uint16_t m_channel2;
} Motor;

void Motor_Init(Motor *motor, TIM_HandleTypeDef *tim1, TIM_HandleTypeDef *tim2,
		uint16_t channel1, uint16_t channel2);

void Set_Motor_speed(Motor *motor, int16_t speed);
void Move_to_middle(float distance, float speed);
void Turn_left90();
void Turn_right90();
void Motor_stop();
void Adjuster();
void Move_onecell();
void Turn_left90_2();
void Turn_right90_2();
void Back();

extern Motor motor_left;
extern Motor motor_right;

#endif /* INC_MOTOR_H_ */
