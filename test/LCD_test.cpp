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
LCD* lcd;
char message[50];

void setUp(){
    Serial.begin(115200);
    lcd = new LCD({18,19,22,23}, RW, RS, ENABLE, PW);
}

void LCDSetup_initializedPinsToLOW(void){
    LCD::pin pins[] = {18, 19, 22, 23, PW, RW, RS, ENABLE};
    lcd->setup();
    for(auto p: pins){
        sprintf(message, "pin %hu wasn't initialized", p);
        TEST_ASSERT_FALSE_MESSAGE(digitalRead(p), message);
    }
}

void LCDSendHalf_IsCorrect(){
    for (size_t i = 0; i <= 0x0F; i++)
    {
        lcd->sendHalf(i);
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

void LCD_MemoryLeaks(void){
    size_t before = MEMORYUSAGE;
    {
    LCD lcd_foo({18,19,22,23}, RW, RS, ENABLE, PW);
    }
    TEST_ASSERT_TRUE_MESSAGE(MEMORYUSAGE == before, "Memory leak in one instance test!\n");
    const size_t testAmount = 10;
    for (size_t i = 0; i < testAmount; i++)
    {
        LCD lcd_foo({18,19,22,23}, RW, RS, ENABLE, PW);
    }
    sprintf(message, "Memory leak when instanciating %u LCD classes", testAmount);
    TEST_ASSERT_TRUE_MESSAGE(MEMORYUSAGE == before, message);
}

//TODO: implement 8-bit mode testing.

void setup(){
    delay(2000);

    UNITY_BEGIN();
    RUN_TEST(LCDSetup_initializedPinsToLOW);
    RUN_TEST(LCDSendHalf_IsCorrect);
    RUN_TEST(LCD_MemoryLeaks);
    UNITY_END();
}

void loop(){
}
