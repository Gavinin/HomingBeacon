//
// Created by Gavin Wang on 6/23/23.
//

#include <Arduino.h>
#include "wifiService.h"
#include "entity/index.h"
#include "config/user_config.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const String AP_SSID = "HomingBeacon";
const String AP_PASSWORD = "12345678";

ESP8266WebServer server(80);

WifiService::WifiService() = default;

/**
 * Handle user configuration page.
 */
void handleUserConfigurationPage() {
    server.send(200, "text/html", (new Index())->PAGE);
}

/**
 * Save user input to driver file system
 */
void handleSetInfoSave() {
    UserConfig *userConfigInp = new UserConfig();
    userConfigInp->wifi_ssid = server.arg("wifi_ssid");
    userConfigInp->wifi_password = server.arg("wifi_password");
    userConfigInp->ddns_hostname = server.arg("ddns_hostname");
    userConfigInp->ddns_domain = server.arg("ddns_domain");
    userConfigInp->ddns_password = server.arg("ddns_password");
    userConfigInp->ddns_url = server.arg("ddns_url");
    userConfigInp->ip_service_url = server.arg("ip_service_url");
    userConfigInp->refresh_timeout = server.arg("refresh_timeout");
    userConfigInp->wifi_error_times = server.arg("wifi_error_times");

    //check user input legally
    if (UserConfigService::checkConfigProperties(userConfigInp)) {
        Serial.printf("configFile: %s \n%s \n%s \n%s \n%s \n%s", userConfigInp->wifi_ssid.c_str(),
                      userConfigInp->wifi_password.c_str(), userConfigInp->ddns_hostname.c_str(),
                      userConfigInp->ddns_domain.c_str(),
                      userConfigInp->ddns_password.c_str(), userConfigInp->ddns_url.c_str());


        UserConfigService::saveConfigToFS(userConfigInp);
        server.send(200, "text/plain", "Configuration saved. Restarting...");

    } else {
        server.send(200, "text/plain", "Configuration is incorrect. Restarting...");
    }
    delete (userConfigInp);
    delay(2000);
    EspClass::restart();
}

/**
 * Check driver whether send data to server.
 */
void WifiService::handleClient() {
    server.handleClient();
}

/**
 * Set driver to AP mode , receive data from user setting page.
 */
void WifiService::startAccessPoint(ESP8266WiFiClass wifi) {
    wifi.mode(WIFI_AP);
    wifi.softAP(AP_SSID, AP_PASSWORD);

    Serial.println("AP IP address: " + WiFi.softAPIP().toString());

    server.on("/", handleUserConfigurationPage);
    server.on("/set_info", handleSetInfoSave);

    server.begin();

}

/**
 * @return Wifi stsaatus
 */
bool WifiService::isWifiConnect(ESP8266WiFiClass wifiClass) {
    return wifiClass.status() == WL_CONNECTED;
}

bool WifiService::isWifiNotConnect(ESP8266WiFiClass wifiClass) {
    return !isWifiConnect(wifiClass);
}

