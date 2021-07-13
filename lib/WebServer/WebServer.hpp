#pragma once

#include <WiFi.h>
#include <Arduino.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>

#include "Secret.hpp"

class WebServer{
    const unsigned short m_PORT;
    File m_file_handler;
    String* m_HTML = nullptr;

public:
    WebServer(unsigned short port) : m_PORT(port) {}

    ~WebServer();

    bool SetHTML(){

    }

    bool loadHTML(){
        
    }

    bool setPage(){

    }

    

};