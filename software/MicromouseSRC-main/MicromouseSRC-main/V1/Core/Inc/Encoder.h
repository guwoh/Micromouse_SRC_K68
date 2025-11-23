/*
 * Encoder.h
 *
 *  Created on: Sep 21, 2025
 *      Author: kinhz
 */

#ifndef INC_ENCODER_H_
#define INC_ENCODER_H_

#include "main.h"


extern int16_t COUNT90;
extern uint16_t COUNT18cm;

extern uint32_t prev_distance;
extern uint16_t prev_time_en;

extern int32_t encoder_cnt_left;
extern uint16_t curr_en_cnt_left;
extern uint16_t prev_en_cnt_left;
extern float speed_left;


extern int32_t encoder_cnt_right;
extern uint16_t curr_en_cnt_right;
extern uint16_t prev_en_cnt_right;
extern float speed_right;

typedef struct {
	TIM_HandleTypeDef *EN_TIM;
} Encoder;

void Encoder_Init(Encoder *encoder, TIM_HandleTypeDef *en_tim);
void read_en_left(Encoder *encoder);
void read_en_right(Encoder *encoder);
float left_pulse_per_sec(float delta_time, int16_t count);
float right_pulse_per_sec(float delta_time, int16_t count);
void reset_en();

extern Encoder encoder_left;
extern Encoder encoder_right;

#endif /* INC_ENCODER_H_ */
