#include <Arduino.h>

#include <LittleFS.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "config/user_config.h"
#include "services/wifiService.h"

const uint16_t CONFIG_TIMEOUT = 60 * 1000;
const char *ntpServer = "pool.ntp.org";
const long gmtOffsetSec = 8 * 3600;
const int daylightOffsetSec = 0;
int refreshTimeout;

String oldIP = "";
int errorTime = 0;

UserConfig userConfig;
UserConfigService *userConfigService;
bool hasUserconfig = false;


ESP8266WiFiClass WiFi;

String url;

void initUrlFromUserConfig();

void sendToDDNSServer(String url);

String getIpFromIpServer();

void printTime();

void setup() {
    Serial.begin(115200);
    LittleFS.begin();
    userConfigService = new UserConfigService();
    Serial.printf("userConfigService is null: %d", userConfigService == nullptr);
    // if user config can be read
    hasUserconfig = userConfigService->readConfig();
    Serial.printf("readConfig %d", hasUserconfig);
    if (hasUserconfig) {
        // Set to AP mode , connect to WI-FI
        Serial.println("Config file exist");
        WiFi.mode(WIFI_STA);
        userConfig = userConfigService->getUserConfig();
        WiFi.begin(userConfig.wifi_ssid.c_str(), userConfig.wifi_password);
        Serial.printf("SSID: %s \tPassword: %s", userConfig.wifi_ssid.c_str(),
                      userConfig.wifi_password.c_str());
        Serial.print("\nConnecting to WiFi.");
        uint32_t configStartTime = millis();

        // Init refresh timeout
        refreshTimeout = userConfig.refresh_timeout.toInt();

        // waiting connect status
        while (WifiService::isWifiNotConnect(WiFi) && millis() - configStartTime < CONFIG_TIMEOUT) {
            delay(1000);
            Serial.print(".");
        }
        if (WifiService::isWifiConnect(WiFi)) {
            // if successful ,it will go loop func
            Serial.println("\nConnecting Wifi Successful.");
            initUrlFromUserConfig();
            configTime(gmtOffsetSec, daylightOffsetSec, ntpServer);

        } else {
            // If failed
            Serial.println("\nFailed to connect WiFi. Starting AP mode.");
            // Delete config
            UserConfigService::deleteConfigFromFS();
            // Turn station mode on
            WifiService::startAccessPoint(WiFi);
        }
    } else {
        // If it didn't find user config file
        Serial.println("\nDidn't find user config file.");
        // Turn station mode on
        WifiService::startAccessPoint(WiFi);
    }

}


void loop() {
    if (WifiService::isWifiConnect(WiFi)) {
        errorTime = 0;

        printTime();
        String publicIP = getIpFromIpServer();
        if (publicIP != "") {
            publicIP.trim();
            Serial.printf("\nPublic IP address: %s", publicIP.c_str());
            if (publicIP != oldIP) {
                oldIP = publicIP;
                // if oldIP is not similarly between current IP, update DDNS
                String urlTmp = url;
                urlTmp.replace("$PUBLIC_IP", String(publicIP.c_str()));
                Serial.printf("Address: %s", urlTmp.c_str());
                sendToDDNSServer(urlTmp);

            } else {
                Serial.print("IP not Update.");
            }
            // Sleep and restart loop
            // Get refresh timeout from user config
            delay(refreshTimeout * 60 * 1000);
        }

    } else {
        if (hasUserconfig) {
            errorTime++;
            if (errorTime > 3) {
                EspClass::restart();
            }
            Serial.printf("Wifi connect error %d times.", errorTime);
            delay(refreshTimeout * 60 * 1000);
        }
        WifiService::handleClient();
    }
}


void initUrlFromUserConfig() {
    url = userConfig.ddns_url;
    url.replace("$DDNS_DOMAIN", userConfig.ddns_domain);
    if (userConfig.ddns_hostname != "") {
        url.replace("$DDNS_HOSTNAME", userConfig.ddns_hostname);
    }
    if (userConfig.ddns_password != "") {
        url.replace("$DDNS_PASSWORD", userConfig.ddns_password);
    }

    Serial.printf("Init url: %s", url.c_str());
}

String getIpFromIpServer() {
    String result = "";
    // Get IP from WI-FI IP address
    HTTPClient ipServerHttp;
    WiFiClient ipSercerWifiClient;
    ipServerHttp.begin(ipSercerWifiClient, userConfig.ip_service_url);
    int httpCode = ipServerHttp.GET();

    Serial.printf("\n IP server connect status: %d", httpCode);
    if (httpCode == HTTP_CODE_OK) {
        result = ipServerHttp.getString();
    }

    return result;
}

void sendToDDNSServer(String url) {
    // send message to DDNS server
    HTTPClient ddnsServerHttp;
    WiFiClient ddnsSercerWifiClient;
    ddnsServerHttp.begin(ddnsSercerWifiClient, url.c_str());
    int httpCode = ddnsServerHttp.GET();

    // get result
    String response = ddnsServerHttp.getString();
    Serial.printf("\n HTTP Code: %d\nHttp response is: %s ", httpCode, response.c_str());

}

void printTime() {
    struct tm timeInfo{};
    if (getLocalTime(&timeInfo)) {
        Serial.printf("\n%d-%d-%d  %d:%d:%d", timeInfo.tm_year, timeInfo.tm_mon, timeInfo.tm_mday,
                      timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec);
    }
}