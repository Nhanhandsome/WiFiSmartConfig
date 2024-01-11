/*
 * WIFI.cpp
 *
 *  Created on: Jan 11, 2024
 *      Author: MinhNhan
 */

#include "WIFI.h"

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

//static const char *TAG_AP = "WiFi AP";
//static const char *TAG_STA = "WiFi Sta";

#define staSSID_Key "sta_ssid"
#define staPASS_Key "sta_key"
#define apSSID_Key "ap_ssid"
#define apPass_Key "ap_pass"

#define ap_ssid_default "SmartWifi"
#define ap_pass_default "12345678"

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
		int32_t event_id, void *event_data) {

	WIFI *p_wifi = (WIFI*) arg;

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *) event_data;
        printf( "Station "MACSTR" joined, AID=%d",
                 MAC2STR(event->mac), event->aid);
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *) event_data;
        printf( "Station "MACSTR" left, AID=%d",
                 MAC2STR(event->mac), event->aid);
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
        printf( "Station started");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
        printf( "Got IP:" IPSTR, IP2STR(&event->ip_info.ip));
        p_wifi->setRetryNum(0);
        xEventGroupSetBits(p_wifi->getWifiEventGroup(), WIFI_CONNECTED_BIT);
    }
}

WIFI::WIFI() {
	this->sta_pass = "12345678";
	this->sta_ssid = "Selex";
	this->ap_ssid = ap_ssid_default;
	this->ap_pass = ap_pass_default;
	strcpy((char*)this->wifiApConfig.ap.ssid, this->ap_ssid.c_str());
	strcpy((char*)this->wifiApConfig.ap.password, this->ap_pass.c_str());
	this->wifiApConfig.ap.channel = 10;
	this->wifiApConfig.ap.ssid_len = strlen(this->ap_ssid.c_str());
	this->wifiApConfig.ap.max_connection = 5;
	this->wifiApConfig.ap.authmode = WIFI_AUTH_WPA2_PSK;
	this->wifiApConfig.ap.pmf_cfg.required = false;

	strcpy((char*)this->wifiStaConfig.sta.ssid, this->sta_ssid.c_str());
	strcpy((char*)this->wifiStaConfig.sta.password, this->sta_pass.c_str());
	this->wifiStaConfig.sta.scan_method = WIFI_ALL_CHANNEL_SCAN;
	this->wifiStaConfig.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
	this->wifiStaConfig.sta.sae_pwe_h2e = WPA3_SAE_PWE_BOTH;

	this->wifiEventGroup = xEventGroupCreate();

	/* Register Event handler */
	ESP_ERROR_CHECK(
			esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, (void*)this, NULL));
	ESP_ERROR_CHECK(
			esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, (void*)this, NULL));

	this->flash = NULL;
	this->retryNum = 3;
	this->netif_ap = NULL;
	this->netif_sta = NULL;
}

WIFI::~WIFI() {
	// TODO Auto-generated destructor stub
}

void WIFI::begin() {

	ESP_ERROR_CHECK(esp_netif_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());

	if (this->flash == NULL) {
		this->flash = new Flash();
		this->flash->Init();
	}
	char staSSID[50];
	char staPass[50];
	char apSSID[50];
	char apPass[50];
	if (this->flash->Read(staSSID_Key, staSSID) == -1
			|| this->flash->Read(staPASS_Key, staPass) == -1) {
		this->sta_pass = "12345678";
		this->sta_ssid = "Selex";
		this->flash->Write(staSSID_Key,this->sta_ssid.c_str() );
		this->flash->Write(staPASS_Key, this->sta_pass.c_str());
	} else {
		this->sta_ssid = staSSID;
		this->sta_pass = staPass;
	}

	strcpy((char*)this->wifiApConfig.ap.ssid, this->ap_ssid.c_str());
	strcpy((char*)this->wifiApConfig.ap.password, this->ap_pass.c_str());
	this->wifiApConfig.ap.ssid_len = strlen(this->ap_ssid.c_str());

	if (this->flash->Read(apSSID_Key, apSSID) == -1
			|| this->flash->Read(apPass_Key, apPass) == -1) {
		this->ap_ssid = ap_ssid_default;
		this->ap_pass = ap_pass_default;
		this->flash->Write(apSSID_Key,this->ap_ssid.c_str());
		this->flash->Write(apPass_Key,this->ap_pass.c_str());
	} else {
		this->ap_ssid = apSSID;
		this->ap_pass = apPass;
	}

	strcpy((char*)this->wifiStaConfig.sta.ssid, this->sta_ssid.c_str());
	strcpy((char*)this->wifiStaConfig.sta.password, this->sta_pass.c_str());

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));

	this->netif_ap = esp_netif_create_default_wifi_ap();

	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &this->wifiApConfig));

	this->netif_sta = esp_netif_create_default_wifi_sta();

	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &this->wifiStaConfig));

CONNECT_WIFI:

	ESP_ERROR_CHECK(esp_wifi_start());

    EventBits_t bits = xEventGroupWaitBits(this->wifiEventGroup,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) {
        printf("connected to ap SSID:%s password:%s",
                 this->sta_ssid.c_str(),this->sta_pass.c_str());
    } else if (bits & WIFI_FAIL_BIT) {
        printf("Failed to connect to SSID:%s, password:%s",
                 this->sta_ssid.c_str(),this->sta_pass.c_str());
        esp_wifi_stop();
        this->retryNum++;
        goto CONNECT_WIFI;
    } else {
        printf("UNEXPECTED EVENT");
        return;
    }

}
