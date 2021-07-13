#include<Arduino.h>
#include<unity.h>

#include "LCD.hpp"

const LCD::pin pinos[] = {18, 19, 22, 23};
#define RS (LCD::pin)5
#define RW (LCD::pin)16 
#define ENABLE (LCD::pin)17
#define PW (LCD::pin)15

/*
* The intellisense is error squilling this line claming:
! "class "LCD::DataBus" has no suitable copy constructor".
* But on the src/main.cpp, this error doesn't appear, and it works fine when compiled. Quite strange.
* If you know why send me a message.'
*/
LCD lcd({18,19,22,23}, RW, RS, ENABLE, PW);
char message[50];

void LCDSetup_initializedPinsToLOW(void){
    std::pair<LCD::pin, bool> pins[] = {{18, true}, {19, true}, {22, true}, {23, true}, {PW, true}, {RW, true}, {RS, true}, {ENABLE, true}};
    lcd.setup();
    for(auto p: pins){
        sprintf(message, "pin %hu wasn't initialized", p.first);
        TEST_ASSERT_TRUE_MESSAGE(p.second != digitalRead(p.first), message);
    }
}

void LCDSendHalf_IsCorrect(){
    for (size_t i = 0; i <= 0x0F; i++)
    {
        lcd.sendHalf(i);
        for (size_t j = 0; j < 4; j++)
        {
            if(i & (0x08 >> j)){
                sprintf(message, "checking pin %hu, it should be HIGH: ", pinos[j]);
                TEST_ASSERT_TRUE_MESSAGE(digitalRead(pinos[j]), message);
            }
            else{
                sprintf(message, "checking pin %hu, it should be LOW: ", pinos[j]);
                TEST_ASSERT_FALSE_MESSAGE(digitalRead(pinos[j]), message);
            }
        }
        
    }
}

//TODO: implement 8-bit mode testing.

void setup(){
    delay(2000);
    Serial.begin(115200);

    UNITY_BEGIN();
    RUN_TEST(LCDSetup_initializedPinsToLOW);
    RUN_TEST(LCDSendHalf_IsCorrect);
    UNITY_END();
}

void loop(){
}
