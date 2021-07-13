#include "LCD.hpp"

#ifdef DEBUG
void LCD::setup(){
    Serial.println("DATA BUS:");
    for (size_t i = 0; i < BUS.m_size; i++)
    {
        pinMode(BUS.m_pins[i], OUTPUT);
        digitalWrite(BUS.m_pins[i], LOW);
        Serial.printf("\t[%u] = %u\n", i, BUS.m_pins[i]);
    }
    
    Serial.printf("[RW] = %u\n[RS] = %u\n[E] = %u\n[PW] = %u", RW, RS, E, PW);
    pinMode(PW, OUTPUT);
    pinMode(RW, OUTPUT);
    pinMode(RS, OUTPUT);
    pinMode(E, OUTPUT);

    digitalWrite(PW, LOW);
    digitalWrite(RW, LOW);
    digitalWrite(RS, LOW);
    digitalWrite(E, LOW);
    Serial.write('\n');
}

void LCD::parseByte(const Byte byte){
    Serial.printf("\tParsing %0x:\n", byte);
    Serial.print("\t\t");
    for (size_t i = 0; i < BUS.m_size; i++)
    {
        if(byte & (0x08 >> i)){
            digitalWrite(BUS.m_pins[i], HIGH);
            Serial.printf("%d", digitalRead(BUS.m_pins[i]) == HIGH ? 1 : 0);
        }
        else{
            digitalWrite(BUS.m_pins[i], LOW);
            Serial.printf("%d", digitalRead(BUS.m_pins[i]) == HIGH ? 1 : 0);
        }
    }
    Serial.write('\n');
}

void LCD::send(Byte byte, sendMode mode, bool onlyHalf){
    busy = true;
    Byte halfByte = byte;
    Serial.printf("Sending data (RW = %s)[", digitalRead(RW) == HIGH ? "HIGH" : "LOW");
    if(mode == sendMode::CMD){
        digitalWrite(RS, LOW);
        Serial.println("CMD]:");
    }
    else{
        digitalWrite(RS, HIGH);
        Serial.println("CH]:");
    }
    delay(1);
    Serial.write('\t');
    if(onlyHalf){
        Serial.printf("DATA: %0X [onlyHalf]\n", halfByte);
        parseByte(halfByte & 0x0F);
        delay(1);
        Serial.printf("\tENABLING[500 un]\n");
        digitalWrite(E, HIGH);
        delay(1);
        digitalWrite(E, LOW);
        delay(2);
        return;
    }
    if(BUS.m_size == 4){
        Serial.printf("DATA: %0X [All (4-bit)]\n", halfByte);
        for (size_t i = 0; i < 2; i++)
        {
            if(i)
                halfByte = byte & 0x0F;
            else
                halfByte = byte >> 4;
            parseByte(halfByte);
            delay(1);
            // while(Serial.available() <= 0);
            Serial.printf("\tENABLING[500 un]\n");
            digitalWrite(E, HIGH);
            delay(1);
            digitalWrite(E, LOW);
            // while(Serial.available() > 0) Serial.read();
            delay(2);
        }
        return;
    }
    else{
        Serial.printf("DATA: %0X [All (8-bit)]\n", halfByte);
        parseByte(halfByte);
        delay(1);
        digitalWrite(E, HIGH);
        delay(1);
        digitalWrite(E, LOW);
        delay(2);
        return;
    }
    busy = false;
}

#else

void LCD::setup(){
    for (size_t i = 0; i < BUS.m_size; i++)
    {
        pinMode(BUS.m_pins[i], OUTPUT);
        digitalWrite(BUS.m_pins[i], LOW);
    }
    pinMode(PW, OUTPUT);
    pinMode(RW, OUTPUT);
    pinMode(RS, OUTPUT);
    pinMode(E, OUTPUT);

    digitalWrite(PW, LOW);
    digitalWrite(RW, LOW);
    digitalWrite(RS, LOW);
    digitalWrite(E, LOW);
}

void LCD::parseByte(const Byte byte){
    for (size_t i = 0; i < BUS.m_size; i++)
    {
        if(byte & (0x08 >> i))
            digitalWrite(BUS.m_pins[i], HIGH);
        else
            digitalWrite(BUS.m_pins[i], LOW);
    }
}

void LCD::send(Byte byte, sendMode mode, bool onlyHalf){
    busy = true;
    Byte halfByte = byte;
    if(mode == sendMode::CMD){
        digitalWrite(RS, LOW);
    }
    else{
        digitalWrite(RS, HIGH);
    }
    delay(1);
    if(onlyHalf){
        parseByte(halfByte & 0x0F);
        delay(1);
        digitalWrite(E, HIGH);
        delay(1);
        digitalWrite(E, LOW);
        delay(2);
        return;
    }
    if(BUS.m_size == 4){
        for (size_t i = 0; i < 2; i++)
        {
            if(i)
                halfByte = byte & 0x0F;
            else
                halfByte = byte >> 4;
            parseByte(halfByte);
            delay(1);
            digitalWrite(E, HIGH);
            delay(1);
            digitalWrite(E, LOW);
            delay(2);
        }
        return;
    }
    else{
        parseByte(halfByte);
        delay(1);
        digitalWrite(E, HIGH);
        delay(1);
        digitalWrite(E, LOW);
        delay(2);
        return;
    }
    busy = false;
}
#endif

void LCD::init(const ARG_LIST ARGS){
    sendMode s_mode = sendMode::CMD;
    digitalWrite(PW, HIGH);
    delay(20);
    send(0x03, s_mode);
    delay(10);
    send(0x03, s_mode);
    delay(1);
    send(0x03, s_mode);
    if(BUS.m_size == 4)
        send(0x02, s_mode, true);
    else if (BUS.m_size == 8)
        send(0x03, s_mode);
    delay(1);   
    setFunction(ARGS & 0x18);
    setScreenMode(ARGS & 0x07);
    clearScreen();
    returnHome();
}

void LCD::setScreenMode(const ARG_LIST ARGS){
    Byte command = 0x08 | (ARGS & 0x07);
    send(command, sendMode::CMD);   
}

void LCD::setFunction(ARG_LIST ARGS){
    Byte command = 0x20 | ((ARGS & 0x18) >> 1);
    if(BUS.m_size == 4)
        command &= 0xEF;
    else if (BUS.m_size == 8)
        command |= 0x10; 
    send(command, sendMode::CMD);
    ets_delay_us(50);
}

bool LCD::moveCursor(uint8_t row, uint8_t column){
    #ifdef DEBUG
    Serial.printf("Received: %ux%u\n",row,column);
    #endif
    current.cursor.Y = row;
    current.cursor.X = column;
    Byte command = 0x80;
    if(row == 1)
        command |= 0x40;
    else if (row == 0)
        command &= 0xBF;
    else
        return false;

    if(column > 15)
        return false;
    else
        command |= column;
    
    send(command, sendMode::CMD);
    ets_delay_us(50);
    return true;
}

void LCD::clearScreen(){
    send(0x01, sendMode::CMD);
    delay(2);
}

void LCD::returnHome(){
    send(0x02, sendMode::CMD);
    delay(2);
}

bool LCD::available(){
    return busy == false;
}

void LCD::sendChar(const char c){
    if(current.cursor.X == 16){
        if(current.cursor.Y == 0)
            ++currentLine;
        else
            --currentLine;
        moveCursor(currentLine, 0);
    }
    send(c, sendMode::CH);
    ++current.cursor.X;
    ets_delay_us(50);
}

void LCD::print(const char* c, size_t len){
    for (size_t i = 0; i < len; i++)
    {
        
        sendChar(c[i]);
    }
}

void LCD::print(const char* c){
    for (size_t i = 0; true; i++)
    {
        if(c[i] == '\0')
            return;
        sendChar(c[i]);
    }
}

void LCD::print(int64_t data){
    char str[22];
    sprintf(str, "%lld", data);
    print(str);
}

void LCD::print(int32_t data){
    char str[11];
    sprintf(str, "%d", data);
    print(str);
}

void LCD::print(int16_t data){
    char str[6];
    sprintf(str, "%hd", data);
    print(str);
}

void LCD::print(int8_t data){
    char str[4];
    sprintf(str, "%hd", data);
    print(str);
}

void LCD::print(uint64_t data){
    char str[22];
    sprintf(str, "%llu", data);
    print(str);
}

void LCD::print(uint32_t data){
    char str[11];
    sprintf(str, "%u", data);
    print(str);
}

void LCD::print(uint16_t data){
    char str[6];
    sprintf(str, "%hu", data);
    print(str);
}

void LCD::print(uint8_t data){
    char str[4];
    sprintf(str, "%hu", data);
    print(str);
}

void LCD::print(double data, uint8_t precision = 2){
    if(precision > FLOAT_MAX_PRECISION)
        return;
    char str[CHAR_BUFFER_MAX];
    char format[5] = {'%', '.', '0'+precision, 'f', '\0'};
    sprintf(str, format, data);
    print(str);
}