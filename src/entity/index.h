//
// Created by Gavin Wang on 6/23/23.
//

#ifndef ESP8266_DDNS_INDEX_H
#define ESP8266_DDNS_INDEX_H

#include <Arduino.h>

class Index {

public:
    const String PAGE = R"(<!DOCTYPE html><html lang="en"><head><meta charset="utf-8"><meta name="viewport"content="width=device-width, initial-scale=1.0"><title>Wifi Settings</title></head><style>*,*:before,*:after{-moz-box-sizing:border-box;-webkit-box-sizing:border-box;box-sizing:border-box}body{font-family:'Nunito',sans-serif;color:#384047}form{max-width:300px;margin:10px auto;padding:10px 20px;background:#f4f7f8;border-radius:8px}h1{margin:0 0 30px 0;text-align:center}input[type="text"],input[type="password"],input[type="url"],textarea,select{background:rgba(255,255,255,0.1);border:none;font-size:16px;height:auto;margin:0;outline:0;padding:15px;width:100%;background-color:#e8eeef;color:#8a97a0;box-shadow:0 1px 0 rgba(0,0,0,0.03)inset;margin-bottom:30px}button{padding:3vh 8vh 3vh 8vh;color:#FFF;background-color:#2590e3;font-size:1.5vh;text-align:center;font-style:normal;border-radius:5px;width:100%;border:1px solid#2590e3;border-width:1px 1px 3px;box-shadow:0-1px 0 rgba(255,255,255,0.1)inset;margin-bottom:10px}fieldset{margin-bottom:30px;border:none}legend{font-size:1.4em;margin-bottom:10px}label{display:block;margin-bottom:8px}@media screen and(min-width:480px){form{max-width:720px}}</style><body><div class="row"><div class="col-md-12"><form method='post'action='/set_info'><h1>Settings</h1><fieldset><legend>WiFi and DDNS Configuration</legend><label for="wifi_ssid">WiFi SSID:</label><input type='text'id="wifi_ssid"name='wifi_ssid'><br><label for="wifi_password">WiFi Password:</label><input type='text'id="wifi_password"name='wifi_password'><br><label for="ddns_hostname">DDNS Hostname:</label><input type='text'id="ddns_hostname"name='ddns_hostname'><br><label for="ddns_domain">DDNS Domain:</label><input type='text'id="ddns_domain"name='ddns_domain'><br><label for="ddns_password">DDNS Password:</label><input type='text'id="ddns_password"name='ddns_password'><br><label for="ddns_url">DDNS URL:</label><input type='url'id="ddns_url"name='ddns_url'value="http://dynamicdns.park-your-domain.com/update?host=$DDNS_HOSTNAME&domain=$DDNS_DOMAIN&password=$DDNS_PASSWORD&ip=$PUBLIC_IP"><br><label for="ip_service_url">IP Server:</label><input type='url'id="ip_service_url"name='ip_service_url'value="http://members.3322.org/dyndns/getip"><br><label for="refresh_timeout">Delay:</label><select id="refresh_timeout"name="refresh_timeout"><option value="5">5 Minutes</option><option value="10">10 Minutes</option><option value="30">30 Minutes</option><option value="60">1 Hour</option><option value="180">3 Hours</option><option value="360">6 Hours</option><option value="720">12 Hours</option><option value="1440">24 Hours</option></select></fieldset><button type="submit">Save</button></form></div></div></body></html>)";
};


#endif //ESP8266_DDNS_INDEX_H
