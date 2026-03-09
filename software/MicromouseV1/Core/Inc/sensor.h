/*
 * sensor.h
 *
 *  Created on: Mar 7, 2026
 *      Author: kinhk
 */

#ifndef INC_SENSOR_H_
#define INC_SENSOR_H_

#define LF_EM_ON HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
#define SIDE_EM_ON HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
#define RF_EM_ON HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
#define LF_EM_OFF HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
#define SIDE_EM_OFF HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
#define RF_EM_OFF HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);

extern uint16_t sensorData[4];
extern float RFSensor;
extern float LFSensor;
extern float RSensor;
extern float LSensor;
extern int16_t RFSensor_raw;
extern int16_t LFSensor_raw;
extern int16_t RSensor_raw;
extern int16_t LSensor_raw;
extern uint16_t RThreshold;
extern uint16_t LThreshold;
extern uint16_t RFThreshold1;
extern uint16_t LFThreshold1;
extern uint16_t RFThreshold2;
extern uint16_t LFThreshold2;

extern float gainSensor;
extern uint8_t wall_R_Check;
extern uint8_t wall_L_Check;
extern uint8_t wall_RF_Check;
extern uint8_t wall_LF_Check;


void read_sensor(ADC_HandleTypeDef *hadc);
void init_sensor(ADC_HandleTypeDef *hadc);
void wall_detect();
void wall_front_detect();
int16_t sensorError();
#endif /* INC_SENSOR_H_ */
