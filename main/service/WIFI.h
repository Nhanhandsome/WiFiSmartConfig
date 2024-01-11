/*
 * WIFI.h
 *
 *  Created on: Jan 11, 2024
 *      Author: MinhNhan
 */

#ifndef MAIN_SERVICE_WIFI_H_
#define MAIN_SERVICE_WIFI_H_

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif_net_stack.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include "lwip/inet.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#if IP_NAPT
#include "lwip/lwip_napt.h"
#endif
#include "lwip/err.h"
#include "lwip/sys.h"
#include "Flash.h"
#include "string"

using namespace std;


class WIFI {
private:
	int retryNum;
	EventGroupHandle_t wifiEventGroup;
	wifi_config_t wifiApConfig;
	wifi_config_t wifiStaConfig;
	Flash *flash;
	string sta_ssid;
	string sta_pass;
	string ap_ssid;
	string ap_pass;
	esp_netif_t *netif_ap;
	esp_netif_t *netif_sta;
public:
	WIFI();
	virtual ~WIFI();
	void begin();

	Flash* getFlash() const {
		return flash;
	}

	void setFlash(Flash *flash) {
		this->flash = flash;
	}

	const string& getApPass() const {
		return ap_pass;
	}

	void setApPass(const string &apPass) {
		ap_pass = apPass;
	}

	const string& getApSsid() const {
		return ap_ssid;
	}

	void setApSsid(const string &apSsid) {
		ap_ssid = apSsid;
	}

	const string& getStaPass() const {
		return sta_pass;
	}

	void setStaPass(const string &staPass) {
		sta_pass = staPass;
	}

	const string& getStaSsid() const {
		return sta_ssid;
	}

	void setStaSsid(const string &staSsid) {
		sta_ssid = staSsid;
	}

	int getRetryNum() const {
		return retryNum;
	}

	void setRetryNum(int retryNum) {
		this->retryNum = retryNum;
	}

	EventGroupHandle_t getWifiEventGroup() const {
		return wifiEventGroup;
	}

	void setWifiEventGroup(EventGroupHandle_t wifiEventGroup) {
		this->wifiEventGroup = wifiEventGroup;
	}
};

#endif /* MAIN_SERVICE_WIFI_H_ */
