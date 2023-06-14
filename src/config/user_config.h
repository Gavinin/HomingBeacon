//
// Created by Gavin Wang on 6/23/23.
//

#ifndef ESP8266_DDNS_USER_CONFIG_H
#define ESP8266_DDNS_USER_CONFIG_H

#include <Arduino.h>
#include "entity/config.h"

class UserConfigService {
private:
    UserConfig config;

public:
    UserConfigService() = default;

    UserConfig getUserConfig();

    bool readConfig();

    static void deleteConfigFromFS();

    static void saveConfigToFS(UserConfig *userConfig);

    static bool checkConfigProperties(UserConfig *config);

};


#endif //ESP8266_DDNS_USER_CONFIG_H
