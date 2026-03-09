/*
 * encoder.h
 *
 *  Created on: Mar 7, 2026
 *      Author: kinhk
 */

#ifndef INC_ENCODER_H_
#define INC_ENCODER_H_

#define FIRST_REG 0x03
#define SECOND_REG 0x04
#define MT6701_ADD 0x06


extern int32_t oneCellDistance;
extern int32_t turnDistance;

extern volatile uint8_t i2c_busy_left;
extern volatile uint8_t i2c_error_left;
extern volatile uint8_t i2c_busy_right;
extern volatile uint8_t i2c_error_right;
extern volatile int16_t encoderChangeLeft;
extern volatile int16_t encoderChangeRight;

extern int32_t distanceLeft;
extern int encoderChange;
extern int32_t encoderCount;
extern int32_t oldEncoderCount;
extern uint8_t encoder_raw_left[2];
extern uint16_t encoder_left;
extern uint8_t encoder_raw_right[2];
extern uint16_t encoder_right;
extern uint16_t old_enc_cnt_left;
extern uint16_t old_enc_cnt_right;
extern int32_t enc_cnt_left;
extern int32_t enc_cnt_right;

extern uint8_t setup_flag;


/*------------------------------------------------------------------*/

void init_encoder();
void read_enc();
void get_encoder_status();
#endif /* INC_ENCODER_H_ */
