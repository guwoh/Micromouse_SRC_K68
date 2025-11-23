/*
 * Encoder.c
 *
 *  Created on: Sep 21, 2025
 *      Author: kinhz
 */

#include "Encoder.h"

Encoder encoder_left;
Encoder encoder_right;

int16_t COUNT90 = 270;
uint16_t COUNT18cm = 1371;

uint16_t prev_time_en = 0;
uint32_t prev_distance = 0;

int32_t encoder_cnt_left = 0;
uint16_t curr_en_cnt_left = 0;
uint16_t prev_en_cnt_left = 0;
float speed_left = 0;


int32_t encoder_cnt_right = 0;
uint16_t curr_en_cnt_right = 0;
uint16_t prev_en_cnt_right = 0;
float speed_right = 0;


void Encoder_Init (Encoder *encoder, TIM_HandleTypeDef *en_tim) {
	encoder->EN_TIM = en_tim;
	HAL_TIM_Encoder_Start(en_tim, TIM_CHANNEL_ALL);
}

void reset_encoder() {
	encoder_cnt_left = 0;
	encoder_cnt_right = 0;
	prev_distance = 0;
}

void read_en_left(Encoder *encoder) {
	curr_en_cnt_left = __HAL_TIM_GET_COUNTER(encoder->EN_TIM);
	int16_t en_increase = (int16_t) -(curr_en_cnt_left - prev_en_cnt_left);
	encoder_cnt_left += en_increase;
	prev_en_cnt_left = curr_en_cnt_left;
	speed_left = left_pulse_per_sec(0.01, en_increase);
}

void read_en_right(Encoder *encoder) {
	curr_en_cnt_right = __HAL_TIM_GET_COUNTER(encoder->EN_TIM);
	int16_t en_increase = (int16_t) -(curr_en_cnt_right - prev_en_cnt_right);
	encoder_cnt_right += en_increase;
	prev_en_cnt_right = curr_en_cnt_right;
	speed_right = right_pulse_per_sec(0.01, en_increase);
}

float left_pulse_per_sec(float delta_time, int16_t count) { // delta_time is micro second

	return (float) (count * 100);
}

float right_pulse_per_sec(float delta_time, int16_t count) {

	return (float) (count * 100);
}



