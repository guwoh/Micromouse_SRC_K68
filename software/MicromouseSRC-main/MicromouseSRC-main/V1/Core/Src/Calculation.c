/*
 * Speed_profile.c
 *
 *  Created on: Oct 22, 2025
 *      Author: kinhz
 */

#include <Calculation.h>

float SPEED_MAX = 300; // mm/s
float _SPEED = 0;
float ACC = 1000; // mm/s/s
float PREV_SPEED = 0;
float TURN_SPEED = 0;
float PREV_TURNSPEED = 0;
float OMEGA = 0;
float ACC_TURN = 900; // mm/s/s
uint16_t CNT_EXT = 0;
uint16_t cnt_cell = 0;

void update_turnspeed(uint32_t dt, uint8_t state) {
	if (state == 1) {
		TURN_SPEED = mm_to_cnt(ACC_TURN) * dt / 1000 + PREV_TURNSPEED;
		if (TURN_SPEED >= 1500)
			TURN_SPEED = 1500;
		PREV_TURNSPEED = TURN_SPEED;
	} else if (state == 0) {
		TURN_SPEED = -mm_to_cnt(ACC_TURN) * dt / 1000 + PREV_TURNSPEED;
		if (TURN_SPEED < 0)
			TURN_SPEED = 0;
		PREV_TURNSPEED = TURN_SPEED;
	}
}

void update_speed(uint32_t dt, uint8_t state) {
	if (state == 1) {
		_SPEED = ACC * dt / 1000 + PREV_SPEED;
		if (_SPEED >= SPEED_MAX)
			_SPEED = SPEED_MAX;
		PREV_SPEED = _SPEED;
	} else if (state == 0) {
		_SPEED = -ACC * dt / 1000 + PREV_SPEED;
		if (_SPEED <= 0)
			_SPEED = 0;
		PREV_SPEED = _SPEED;
	}

}

// CNT_EXT (count_extra) : use for turn
// + CNT_EXT to angle_target so need check + or - and the direction

void calc_extra_cnt() {
	int16_t count;
	if (get(&mouse_state, mouse_state.index - 1) == 1) { // prev forward
		count = mm_to_cnt(180) - encoder_cnt_left;
		encoder_cnt_left = 0;
		encoder_cnt_left -= count;
		count = mm_to_cnt(180) - encoder_cnt_right;
		encoder_cnt_right = 0;
		encoder_cnt_right -= count;

		if (get(&mouse_state, mouse_state.index == 2)) {
			CNT_EXT = encoder_cnt_right - encoder_cnt_left;
		} else if (get(&mouse_state, mouse_state.index == 3)) {
			CNT_EXT = encoder_cnt_left - encoder_cnt_right;
		}
	} else if (get(&mouse_state, mouse_state.index - 1) == 2) { // prev turn right
		if (get(&mouse_state, mouse_state.index) == 1) {
			count = -(mm_to_cnt(perimeter) / 4 - 60)
					+ (encoder_cnt_left - encoder_cnt_right) / 2;
			if (count > 0) {
				reset_encoder();
				encoder_cnt_left = count;
			} else {
				reset_encoder();
				encoder_cnt_right = -count;
			}
		} else if (get(&mouse_state, mouse_state.index) == 3) {
			CNT_EXT = -(mm_to_cnt(perimeter) / 4 - 60)
					+ (encoder_cnt_left - encoder_cnt_right) / 2;
		}

	} else if (get(&mouse_state, mouse_state.index - 1) == 3) { // prev turn left
		if (get(&mouse_state, mouse_state.index) == 1) {
			count = -(mm_to_cnt(perimeter) / 4 - 50)
					+ (-encoder_cnt_left + encoder_cnt_right) / 2;
			if (count > 0) {
				reset_encoder();
				encoder_cnt_right = count;
			} else {
				reset_encoder();
				encoder_cnt_left = -count;
			}
		} else if (get(&mouse_state, mouse_state.index) == 2) {
			CNT_EXT = -(mm_to_cnt(perimeter) / 4 - 50)
					+ (-encoder_cnt_left + encoder_cnt_right) / 2;
		}
	}
}

uint8_t check_state() {
	if (get(&mouse_state, mouse_state.index + 1) == 1) {
		return 1;
	} else {
		if ((mm_to_cnt(180) - encoder_cnt_left) <= mm_to_cnt(30)) {
			return 0;
		}
		return 1;
	}
}

float cnt_to_omega(uint32_t cnt_left, uint32_t cnt_right) {
	uint32_t cnt_delta = cnt_left - cnt_right;
	return (cnt_delta / mm_to_cnt(perimeter) * 360);
}

float mm_to_cnt(float distance) {
	return distance * 820 / 106.81;
}

float cnt_to_mm(uint32_t cnt) {
	return cnt * 106.81 / 820;
}

