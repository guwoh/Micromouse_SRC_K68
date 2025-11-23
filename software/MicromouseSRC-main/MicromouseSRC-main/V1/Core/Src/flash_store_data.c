/*
 * flash_store_data.c
 *
 *  Created on: Oct 11, 2025
 *      Author: kinhz
 */
#include "flash_store_data.h"
#include "stdint.h"
#include "stm32f1xx_it.h"

typedef uint32_t flash_data_type;
#define DATA_SIZE sizeof(flash_data_type)

void store_data_memory(uint32_t memory_address, uint8_t *data,
		uint16_t data_length) {
	uint8_t single_word_data[DATA_SIZE];
	FLASH_EraseInitTypeDef flash_erase_struct;
	uint32_t PageError = 0;
	HAL_FLASH_Unlock();

	flash_erase_struct.TypeErase = FLASH_TYPEERASE_PAGES;
	flash_erase_struct.PageAddress = memory_address;
	flash_erase_struct.NbPages = 1 + data_length / FLASH_PAGE_SIZE;
//	flash_erase_struct.NbPages = 1;
	flash_erase_struct.Banks = FLASH_BANK_1;
	HAL_FLASHEx_Erase(&flash_erase_struct, &PageError);

	uint32_t i = 0;
	while (i <= data_length) {
		single_word_data[i % DATA_SIZE] = data[i];
		i++;
		if (i % DATA_SIZE == 0) {
			HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,
					memory_address + i - DATA_SIZE,
					*((uint32_t*) single_word_data));
		}
	}
	//convert data if something left
	if (i % DATA_SIZE != 0) {
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,
				memory_address + i - i % DATA_SIZE,
				*((uint32_t*) single_word_data));
	}

	HAL_FLASH_Lock();

}

void read_flash_memory(uint32_t memory_address, uint8_t *data,
		uint16_t data_length) {
	for (int i = 0; i < data_length; i++) {
		*(data + i) = (*(uint8_t*) (memory_address + i));
	}
}
