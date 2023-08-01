//
// Created by Gavin Wang on 6/23/23.
//

#include <LittleFS.h>
#include "user_config.h"
#include "utils/stringUtil.h"
#include <ArduinoJson.h>

const String CONFIG_FILE = "/config.txt";


/**
 * Save param userConfig to local file system.
 * @param userConfig user configuration
 */
void UserConfigService::saveConfigToFS(UserConfig *userConfig) {
    File configFile = LittleFS.open(CONFIG_FILE, "w");
    if (!configFile) {
        Serial.println("Failed to open userConfig file for writing");
        return;
    }
    DynamicJsonDocument doc(1024);
    doc["wifi_ssid"] = userConfig->wifi_ssid.c_str();
    doc["wifi_password"] = userConfig->wifi_password.c_str();
    doc["ddns_hostname"] = userConfig->ddns_hostname.c_str();
    doc["ddns_domain"] = userConfig->ddns_domain.c_str();
    doc["ddns_password"] = userConfig->ddns_password.c_str();
    doc["ddns_url"] = userConfig->ddns_url.c_str();
    doc["ip_service_url"] = userConfig->ip_service_url.c_str();
    doc["refresh_timeout"] = userConfig->refresh_timeout.c_str();
    doc["wifi_error_times"] = userConfig->wifi_error_times.c_str();

    String jsonStr;
    serializeJson(doc, jsonStr);
    configFile.println(jsonStr);

    configFile.close();
}

/**
 * Delete config from file system.
 */
void UserConfigService::deleteConfigFromFS() {
    bool result = LittleFS.remove(CONFIG_FILE);
    if (!result) {
        Serial.println("Failed to open config file for writing");
        return;
    }
}

/**
 * Check UserConfig correctly.
 * @param config user input
 * @return is correctly
 */
bool UserConfigService::checkConfigProperties(UserConfig *config) {

    if (config->wifi_ssid == "") {
        Serial.println("wifi_ssid:" + config->wifi_ssid);
        return false;
    }
    if (config->wifi_password == "") {
        Serial.println("wifi_password:" + config->wifi_password);
        return false;
    }
    if (config->ddns_domain == "") {
        Serial.println("ddns_domain:" + config->ddns_domain);
        return false;
    }
    if (config->ddns_url == "") {
        Serial.println("ddns_url:" + config->ddns_url);
        return false;
    }
    if (config->ip_service_url == "") {
        Serial.println("ip_service_url:" + config->ip_service_url);
        return false;
    }

    if (config->refresh_timeout == "0" || StringUtil::isNotDigital(config->refresh_timeout.c_str())) {
        Serial.println("refresh_timeout:" + config->refresh_timeout);
        return false;
    }

    if (StringUtil::isNotDigital(config->wifi_error_times.c_str())) {
        Serial.println("wifi_error_times:" + config->wifi_error_times);
        return false;
    }

    return true;
}

/**
 * Reading user setting from file system.
 * @return
 */
bool UserConfigService::readConfig() {
    bool isFileExists = LittleFS.exists(CONFIG_FILE);
    Serial.printf("readConfig: %d", isFileExists);
    if (isFileExists) {
        File configFile = LittleFS.open(CONFIG_FILE, "r");
        if (configFile) {

            DynamicJsonDocument doc(1024);
            deserializeJson(doc, configFile.readStringUntil('\n').c_str());
            Serial.println("configFile: exist");

            this->config.wifi_ssid = doc["wifi_ssid"].as<String>();
            this->config.wifi_password = doc["wifi_password"].as<String>();
            this->config.ddns_hostname = doc["ddns_hostname"].as<String>();
            this->config.ddns_domain = doc["ddns_domain"].as<String>();
            this->config.ddns_password = doc["ddns_password"].as<String>();
            this->config.ddns_url = doc["ddns_url"].as<String>();
            this->config.ip_service_url = doc["ip_service_url"].as<String>();
            this->config.refresh_timeout = doc["refresh_timeout"].as<String>();
            this->config.wifi_error_times = doc["wifi_error_times"].as<String>();

            Serial.printf("local configFile: %s \n%s \n%s \n%s \n%s \n%s",
                          this->config.wifi_ssid.c_str(),
                          this->config.wifi_password.c_str(), this->config.ddns_hostname.c_str(),
                          this->config.ddns_domain.c_str(),
                          this->config.ddns_password.c_str(), this->config.ddns_url.c_str());

            configFile.close();

            return true;
        } else {
            Serial.println("configFile: isn't exist");
        }
    }
    return false;
}

UserConfig UserConfigService::getUserConfig() {
    return this->config;
}


