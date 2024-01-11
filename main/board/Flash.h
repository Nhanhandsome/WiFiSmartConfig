/*
 * Flash.h
 *
 *  Created on: Jan 11, 2024
 *      Author: MinhNhan
 */

#ifndef MAIN_BOARD_FLASH_H_
#define MAIN_BOARD_FLASH_H_
#include "nvs_flash.h"
class Flash {
private:
	nvs_handle_t flash_handle;
public:
	Flash();
	virtual ~Flash();
	void Init(void);
	void Erase(void);
	int Write(const char* key,const char* value);
	int Read(const char* key,char* value);
	void EraseKey(const char* key);
};

#endif /* MAIN_BOARD_FLASH_H_ */
