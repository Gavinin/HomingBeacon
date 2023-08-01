//
// Created by Gavin Wang on 8/2/23.
//

#include "stringUtil.h"

bool StringUtil::isDigital(const char *digitalString) {
    if (!digitalString || !*digitalString) return false;
    if (*digitalString == '+' || *digitalString == '-') digitalString++;
    while (*digitalString) {
        if (!isdigit(*digitalString)) return false;
        digitalString++;
    }
    return true;
}

bool StringUtil::isNotDigital(const char *digitalString) {
    return !(StringUtil::isDigital(digitalString));
}
