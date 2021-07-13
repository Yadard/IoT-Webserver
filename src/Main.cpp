#include <WiFi.h>
#include <Arduino.h>
#include <SPIFFS.h>
#include "ESPAsyncWebServer.h"
#include <ESPmDNS.h>
#include "Secret.hpp"
#include "Scheduler.hpp"
#include "LCD.hpp"

#define checkWS(x) if(gclient != nullptr && gclient->status() == WS_CONNECTED){x;}
#define MAX_USERS 5
#define BLINK_LED 21

AsyncWebServer server(80); //HTTP Server
AsyncWebSocket ws("/ws");
AsyncWebSocketClient* gclient= nullptr;

bool updateStatus = false;
#define RS (uint8_t)5
#define RW (uint8_t)16 
#define ENABLE (uint8_t)17
#define PW (uint8_t)15
#define DISPLAY_DELAY() lcd.Screen.moveCursor(0, 0); lcd.print("In: "); lcd.print(blink.getInterval()); lcd.print(" us")

LCD lcd({18, 19, 22, 23}, RW, RS, ENABLE, PW);
String Home_page;

/*FIXME: the LCD is causing major overhead in low order x<100 ms
*! Maybe a async programming can solve this.
*/
Scheduler<void(*)(AsyncWebSocketClient* client)> blink([](AsyncWebSocketClient* client){
	static bool sendedBounds = false;
	lcd.Screen.moveCursor(1, 0);
	lcd.print("El: ");
	lcd.print(esp_timer_get_time());
	lcd.print(" us");
	digitalWrite(BLINK_LED, !digitalRead(BLINK_LED));
	if(blink.getInterval() >= 70000){
		if(client != nullptr && client->status() == WS_CONNECTED && client->canSend()){
			client->text("read=" + String(digitalRead(BLINK_LED)));
		}
		sendedBounds = false;
	}
	else{
		if(client != nullptr && client->status() == WS_CONNECTED && client->canSend() && !sendedBounds){
			client->text("read=1");
			sendedBounds = true;
		}
	}
}, 250000);

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
	lcd.setup();
	pinMode(BLINK_LED, OUTPUT);
	lcd.init(LCD::Screen::Args::SCREEN_2LINE | LCD::Screen::Args::SHOW_DISPLAY | LCD::Screen::Args::SHOW_CURSOR | LCD::Screen::Args::BLIKING_CURSOR);
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
		if(valid){
			blink = temp * 1000;
			updateStatus = true;
		}
		Serial.printf("Str_Input = %s | Delay = %u | msg = ", d.c_str(), blink.getInterval());
		Serial.println("delay=" + String(blink.getInterval()));
		checkWS(gclient->text("delay=" + String(blink.getInterval()/1000)));
		request->send(200);
	});
	server.begin();
	Serial.println("[Server] Started");
	DISPLAY_DELAY();
	blink.startCount();
	Serial.println("\n\n\n");
}

void loop() {
	if(blink.work())
		blink.f_work(gclient);
	if(updateStatus){
		DISPLAY_DELAY();
		updateStatus = false;
	}
}