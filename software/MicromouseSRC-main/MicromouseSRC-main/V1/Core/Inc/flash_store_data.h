/*
 * flash_store_data.h
 *
 *  Created on: Oct 11, 2025
 *      Author: kinhz
 */

#ifndef INC_FLASH_STORE_DATA_H_
#define INC_FLASH_STORE_DATA_H_

#include "main.h"
#include "stdio.h"

#define ADDRESS_1 0x0801F7E0

void store_data_memory(uint32_t memory_address, uint8_t *data, uint16_t data_length);
void read_flash_memory(uint32_t memory_address, uint8_t *data, uint16_t data_length);

#endif /* INC_FLASH_STORE_DATA_H_ */
