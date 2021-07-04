#include <WiFi.h>
#include <Arduino.h>
#include <SPIFFS.h>
#include "ESPAsyncWebServer.h"
#include <ESPmDNS.h>
#include "Secret.hpp"
#include "Scheduler.hpp"

#define checkWS(x) if(gclient != nullptr && gclient->status() == WS_CONNECTED){x;}
#define MAX_USERS 5
#define DEBUG

AsyncWebServer server(80); //HTTP Server
AsyncWebSocket ws("/ws");
AsyncWebSocketClient* gclient= nullptr;

String Home_page;
Scheduler<void(*)(AsyncWebSocketClient* client)> blink([](AsyncWebSocketClient* client){
	digitalWrite(23, !digitalRead(23));
	if(client != nullptr && client->status() == WS_CONNECTED && client->canSend()){
		client->text("read=" + String(digitalRead(23)));
	}
}, 250000);
int Users = 0;

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
	if(type == WS_EVT_CONNECT){
		Serial.println("Conected to websocket");
		gclient = client;
	}
	else if (type == WS_EVT_DISCONNECT){
		Serial.println("Websocket client connection finished");
   		gclient = nullptr;
	}
}

void setup() {
	Serial.begin(115200);
	WiFi.begin(SSID, PASS);
	Serial.printf("[WiFI] Conecting. Pass=%s", PASS);
	for (size_t i = 0; WiFi.status() != WL_CONNECTED; i++)
	{
		if(i == 10){
			i = 0;
			Serial.print("\n[WiFi] reconnecting");
			WiFi.reconnect();
		}
		delay(500);
		Serial.write('.');
	}
	
	Serial.printf("\n[WiFi] connected on %s : IP ", SSID);
	Serial.println(WiFi.localIP());

	if(!MDNS.begin("ESP32")){
		//http://ESP32.local
		Serial.println("[MDNS] Error on settings, restarting in 1s");
		delay(1000);
		ESP.restart();
	}

	if(!SPIFFS.begin(true)){
		Serial.println("An Error has occurred while mounting SPIFFS");
		return;
	}
	File file;
	if(!(file = SPIFFS.open("/test.html"))){
		Serial.println("An Error has occurred while opening SPIFFS");
		return;
	}
	Home_page = file.readStringUntil('\0');
	Home_page.replace("placeholder=\"delay\"", "placeholder=\"" + String(blink.getInterval()/1000) + "\"");
	Serial.println("[Server] setting up...");
	ws.onEvent(onWsEvent);
	server.addHandler(&ws);
	server.on("/", HTTP_GET, [](AsyncWebServerRequest* request){
		digitalWrite(23, digitalRead(23));
		request->send(200, "text/html", Home_page.c_str());
	});
	server.on("/post", HTTP_POST, [](AsyncWebServerRequest* request){}, NULL,
	[](AsyncWebServerRequest* request, uint8_t *data, size_t len, size_t index, size_t total){
		data[len] = '\0';
		String d((const char*) data);
		int temp;
		bool valid = true;
		if(!(temp = d.toInt())){
			if(d[0] != '0'){
				request->send(406, "text/plain", "INVALID NUMBER");
				valid = false;
			}
		}
		if(valid)
			blink = temp * 1000;
		Serial.printf("Str_Input = %s | Delay = %u | msg = ", d.c_str(), blink.getInterval());
		Serial.println("delay=" + String(blink.getInterval()));
		checkWS(gclient->text("delay=" + String(blink.getInterval()/1000)));
		request->send(200);
	});
	server.begin();
	Serial.println("[Server] Started");
	pinMode(23, OUTPUT);
	blink.startCount();
}

void loop() {
	if(blink.work())
		blink.f_work(gclient);
}