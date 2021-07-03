#pragma once

#include<WiFi.h>
#include<Arduino.h>
#include<SPIFFS.h>
#include<ESPAsyncWebServer.h>
#include<ESPmDNS.h>

class WebServer{
    const char* m_SSID;
    const char* m_PASSWORD;
    const unsigned short m_PORT;
    File m_file_handler;
    String* m_HTML = nullptr;

public:
    WebServer(const char* SSID, const char* PASSWORD, unsigned short port) : m_SSID(SSID), m_PASSWORD(PASSWORD), m_PORT(port) {}

    ~WebServer();

    bool SetHTML(){

    }

    bool loadHTML(){
        
    }

    bool setPage(){

    }

    

};