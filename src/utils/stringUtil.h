//
// Created by Gavin Wang on 8/2/23.
//
#include <Arduino.h>

#ifndef ESP8266_DDNS_STRINGUTIL_H
#define ESP8266_DDNS_STRINGUTIL_H


class StringUtil {
public:
    static bool isDigital(const char *digitalString);

    static bool isNotDigital(const char *digitalString);
};


#endif //ESP8266_DDNS_STRINGUTIL_H
