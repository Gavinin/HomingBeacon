//
// Created by Gavin Wang on 30/6/23.
//

#ifndef ESP8266_DDNS_CONFIG_H
#define ESP8266_DDNS_CONFIG_H

#include <Arduino.h>

class UserConfig {
public:
    String wifi_ssid;
    String wifi_password;
    String ddns_hostname;
    String ddns_domain;
    String ddns_password;
    String ddns_url;
    String ip_service_url;
    String refresh_timeout;
};


#endif //ESP8266_DDNS_CONFIG_H
