/**
  * This Arduino sketch implements a dynamic DNS (DDNS) client on an ESP8266.
 * It reads a configuration from the LittleFS filesystem to connect to WiFi.
 * It gets the public IP address of the device from an external service.
 * If the IP changes, it updates the IP at a DDNS provider to map a hostname
 * to the device's current public IP.
 *
 * The device prints the current time and IP address. If the WiFi connection
 * fails, it falls back into Access Point (AP) mode to allow reconfiguring
 * the WiFi credentials.
 *
 * @author Gavinin
 * @licence MIT
 * @version v1.0.0
 * @create: created by Gavin on 2023-06-15
 * @github https://github.com/Gavinin/HomingBeacon
 */
#include <Arduino.h>

#include <LittleFS.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "config/user_config.h"
#include "services/wifiService.h"

// Constants
const uint16_t CONFIG_TIMEOUT = 60 * 1000;
const String NTP_SERVER = "pool.ntp.org";
const long GMT_OFFSET_SEC = 8 * 3600;
const int DAY_LIGHT_OFFSET_SEC = 0;

// Global variables
// Stores user config
UserConfig userConfig;
// Stores last seen IP
String lastIP = "";
// Refresh interval
int refreshTimeout;
// Counter for Wi-Fi errors
int errorTime = 0;
int maxErrorTime = 0;
// Flag if config file exists
bool hasUserConfig = false;
// Template for DDNS URL
String urlTemplate;

// Object instances
UserConfigService *userConfigService;
ESP8266WiFiClass WiFi;

// Function declarations
void initUrlFromUserConfig();

void sendToDDNSServer(const String &url);

String getIpFromIpServer();

void printTime();


void setup() {
    // Serial and LittleFS initialization
    Serial.begin(115200);
    LittleFS.begin();
    userConfigService = new UserConfigService();
    Serial.printf("userConfigService is null: %d", userConfigService == nullptr);
    // if user config can be read
    hasUserConfig = userConfigService->readConfig();
    Serial.printf("readConfig %d", hasUserConfig);
    if (hasUserConfig) {
        // Set to AP mode, connect to WI-FI
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
            // If successful, it will go loop func
            Serial.println("\nConnecting Wifi Successful.");
            initUrlFromUserConfig();
            // Initial maxErrorTime
            maxErrorTime = userConfig.wifi_error_times.toInt();

            // Sync time
            configTime(GMT_OFFSET_SEC, DAY_LIGHT_OFFSET_SEC, NTP_SERVER);

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
    // Handle Wi-Fi connected mode
    if (WifiService::isWifiConnect(WiFi)) {
        errorTime = 0;

        printTime();
        String currentIP = getIpFromIpServer();
        if (currentIP != "") {
            currentIP.trim();
            Serial.printf("\nPublic IP address: %s", currentIP.c_str());
            if (currentIP != lastIP) {
                lastIP = currentIP;
                // if lastIP is not similar between current IP, update DDNS
                String urlTmp = urlTemplate;
                urlTmp.replace("$PUBLIC_IP", String(currentIP.c_str()));
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
        // Handle AP mode
        // Or retry connecting to Wi-Fi

        // If config exists, but Wi-Fi isn't connected.
        // It will restart after retry 3 times.
        if (hasUserConfig) {
            errorTime++;
            if (maxErrorTime > 0 && errorTime > maxErrorTime) {
                EspClass::restart();
            }
            Serial.printf("Wifi connect error %d times.", errorTime);
            delay(refreshTimeout * 60 * 1000);
        }
        WifiService::handleClient();
    }
}

/**
 * Initializes DDNS URL Template from user config
 */
void initUrlFromUserConfig() {
    urlTemplate = userConfig.ddns_url;
    urlTemplate.replace("$DDNS_DOMAIN", userConfig.ddns_domain);
    if (userConfig.ddns_hostname != "") {
        urlTemplate.replace("$DDNS_HOSTNAME", userConfig.ddns_hostname);
    }
    if (userConfig.ddns_password != "") {
        urlTemplate.replace("$DDNS_PASSWORD", userConfig.ddns_password);
    }

    Serial.printf("Init url: %s", urlTemplate.c_str());
}

/**
 * Gets public IP from web service
 * @return current IP address
 */
String getIpFromIpServer() {
    String result = "";
    // Get IP from WI-FI IP address
    HTTPClient ipServerHttp;
    WiFiClient ipSercerWifiClient;
    // Make HTTP request
    ipServerHttp.begin(ipSercerWifiClient, userConfig.ip_service_url);
    int httpCode = ipServerHttp.GET();

    Serial.printf("\n IP server connect status: %d", httpCode);
    if (httpCode == HTTP_CODE_OK) {
        result = ipServerHttp.getString();
    }

    return result;
}

/**
 * Sends update to DDNS server
 * @param url
 */
void sendToDDNSServer(const String &url) {
    // send a message to DDNS server
    HTTPClient ddnsServerHttp;
    WiFiClient ddnsSercerWifiClient;
    ddnsServerHttp.begin(ddnsSercerWifiClient, url.c_str());
    int httpCode = ddnsServerHttp.GET();

    // get result
    String response = ddnsServerHttp.getString();
    Serial.printf("\n HTTP Code: %d\nHttp response is: %s ", httpCode, response.c_str());

}

/**
 * Print current time in Serial
 */
void printTime() {
    struct tm timeInfo{};
    if (getLocalTime(&timeInfo)) {
        Serial.printf("\n%d-%d-%d  %d:%d:%d", timeInfo.tm_year, timeInfo.tm_mon, timeInfo.tm_mday,
                      timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec);
    }
}