//
// Created by Gavin Wang on 6/23/23.
//

#ifndef ESP8266_DDNS_WIFI_SERVICE_H
#define ESP8266_DDNS_WIFI_SERVICE_H


#include <functional>
#include <ESP8266WiFi.h>
#include "config/user_config.h"

class WifiService {

public:


    WifiService();

    static void handleClient();

    static void startAccessPoint(ESP8266WiFiClass wifiClass);

    static bool isWifiConnect(ESP8266WiFiClass wifiClass);

    static bool isWifiNotConnect(ESP8266WiFiClass wifiClass);

};

#endif //ESP8266_DDNS_WIFI_SERVICE_H
