/*
 * Speed_profile.h
 *
 *  Created on: Oct 22, 2025
 *      Author: kinhz
 */

#ifndef INC_CALCULATION_H_
#define INC_CALCULATION_H_

#include "main.h"

#define perimeter 263.89
#define onecell 180

extern float SPEED_MAX;
extern float _SPEED;
extern float ACC;
extern float PREV_SPEED;
extern float TURN_SPEED;
extern float PREV_TURNSPEED;
extern float ACC_TURN;
extern float OMEGA;
extern uint16_t cnt_cell;
extern uint16_t CNT_EXT;

void update_speed(uint32_t dt, uint8_t state);
float mm_to_cnt(float distance);
void calc_extra_cnt();
uint8_t check_state();
float cnt_to_omega(uint32_t cnt_left, uint32_t cnt_right);
float cnt_to_mm(uint32_t cnt);
void mouse_correction();
void update_turnspeed(uint32_t dt, uint8_t state);


#endif /* INC_CALCULATION_H_ */
