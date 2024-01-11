/*
 * Flash.cpp
 *
 *  Created on: Jan 11, 2024
 *      Author: MinhNhan
 */

#include "Flash.h"


#define STORE_KEY "storage"

Flash::Flash() {
	// TODO Auto-generated constructor stub
	this->flash_handle = 0;
}

Flash::~Flash() {
	// TODO Auto-generated destructor stub
}

void Flash::Init(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
	ret = nvs_open(STORE_KEY, NVS_READWRITE, &flash_handle);
	if(ret != ESP_OK){
		//return ;
	}
	//ESP_ERROR_CHECK(nvs_flash_erase());
    ESP_ERROR_CHECK( ret );
}

void Flash::Erase(void) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    esp_err_t ret = nvs_flash_init();
    ESP_ERROR_CHECK( ret );
}

int Flash::Write(const char *key, const char *value) {
	nvs_open(STORE_KEY, NVS_READWRITE, &flash_handle);
	esp_err_t ret = nvs_set_str(flash_handle, key, value);
	if(ret != ESP_OK){
		return ret;
	}
	nvs_commit(flash_handle);
	nvs_close(flash_handle);
	return ret;
}

int Flash::Read(const char *key, char *value) {
	esp_err_t ret;
	nvs_open(STORE_KEY, NVS_READWRITE, &flash_handle);
	size_t size = -1;
	ret = nvs_get_str(flash_handle, key, value, &size);
	if(ret != ESP_OK){
		return -1;
	}

	nvs_close(flash_handle);
	return (int)size;
}

void Flash::EraseKey(const char *key) {
	nvs_open(STORE_KEY, NVS_READWRITE, &flash_handle);
	nvs_erase_key(flash_handle, key);
	nvs_commit(flash_handle);
	nvs_close(flash_handle);
}
