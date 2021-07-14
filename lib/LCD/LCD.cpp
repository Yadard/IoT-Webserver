#include "LCD.hpp"

#ifdef UNIT_TEST
size_t MEMORYUSAGE = 0;
uint32_t LCD::DataBus::_ID = 0;
#endif

LCD::DataBus::Iterator LCD::DataBus::begin(){
    return Iterator(pins);
}
LCD::DataBus::Iterator LCD::DataBus::begin() const{
    return Iterator(pins);
}

LCD::DataBus::Iterator LCD::DataBus::end(){
    if(mode == Mode::M8bit)
        return Iterator(pins + 8);
    else
        return Iterator(pins + 4);
}
LCD::DataBus::Iterator LCD::DataBus::end() const{
    if(mode == Mode::M8bit)
        return Iterator(pins + 8);
    else
        return Iterator(pins + 4);
}

LCD::DataBus::Iterator& LCD::DataBus::Iterator::operator=(const pin* element){
    CurrentElement = element;
    return *this;
}

LCD::DataBus::Iterator& LCD::DataBus::Iterator::operator++(){
    ++CurrentElement;
    return *this;
}

LCD::DataBus::Iterator LCD::DataBus::Iterator::operator++(int){
    Iterator it = *this;
    ++*this;
    return it;
}

bool LCD::DataBus::Iterator::operator!=(const LCD::DataBus::Iterator& it){
    return CurrentElement != it.CurrentElement;
}

LCD::pin LCD::DataBus::Iterator::operator*(){
    return *CurrentElement;
}

uint8_t LCD::DataBus::getSize(){
    return mode == Mode::M8bit ? 8 : 4;
}

void LCD::DataBus::parseByte(const Byte byte){
    if(mode == Mode::M8bit)
        for (uint8_t i = 0; i < 8; i++)
        {
            if(byte & (0x80 >> i))
                digitalWrite(pins[i], HIGH);
            else
                digitalWrite(pins[i], LOW);
        }
    else
        for (uint8_t i = 0; i < 4; i++)
        {
            if(byte & (0x08 >> i))
                digitalWrite(pins[i], HIGH);
            else
                digitalWrite(pins[i], LOW);
        }
}
void LCD::DataBus::parseByte(const Byte byte) const{
    if(mode == Mode::M8bit)
        for (uint8_t i = 0; i < 8; i++)
        {
            if(byte & (0x80 >> i))
                digitalWrite(pins[i], HIGH);
            else
                digitalWrite(pins[i], LOW);
        }
    else
        for (uint8_t i = 0; i < 4; i++)
        {
            if(byte & (0x08 >> i))
                digitalWrite(pins[i], HIGH);
            else
                digitalWrite(pins[i], LOW);
        }
}

void LCD::setup(){
    for (LCD::pin pin : BUS){
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
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

void LCD::sendHalf(Byte byte, Sending mode) {
    bool State_RW = digitalRead(RW);
    Byte halfByte = byte & 0x0F;
    if(mode == Sending::Command){
        digitalWrite(RS, LOW);
    }
    else{
        digitalWrite(RS, HIGH);
    }
    if(State_RW != digitalRead(RW))
        delay(1);
    BUS.parseByte(halfByte & 0x0F);
    delay(1);
    digitalWrite(E, HIGH);
    delay(1);
    digitalWrite(E, LOW);
}

void LCD::send(Byte byte, Sending mode){
    if(BUS.mode == DataBus::Mode::M4bit){
        sendHalf(byte >> 4, mode);       //Most Significant Bits First, AKA Higher Nibble
        sendHalf(byte & 0x0F, mode);     //Least Significant Bits First, AKA Lowwer Nibble
    }
    else{
        if(mode == Sending::Command)
            digitalWrite(RS, LOW);
        else
            digitalWrite(RS, HIGH);
        delay(1);
        BUS.parseByte(byte);
        delay(1);
        digitalWrite(E, HIGH);
        delay(1);
        digitalWrite(E, LOW);
        delay(2);
    }
}

void LCD::Screen::setScreenMode(const ArgList Args){
    Byte command = 0x08 | (Args & 0x07);
    parent.send(command, Sending::Command);
}

void LCD::Screen::setFunction(const ArgList Args){
    Byte command = 0x20 | ((Args & 0x18) >> 1);
    if(parent.BUS.mode == LCD::DataBus::Mode::M4bit)
        command &= 0xEF;
    else
        command |= 0x10; 
    parent.send(command, Sending::Command);
    ets_delay_us(50);
}

bool LCD::Screen::moveCursor(uint8_t row, uint8_t column){
    #ifdef DEBUG
    Serial.printf("Received: %ux%u\n",row,column);
    #endif
    Cursor.Y = row;
    Cursor.X = column;
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
    
    parent.send(command, Sending::Command);
    ets_delay_us(50);
    return true;
}

void LCD::Screen::clearScreen(){
    parent.send(0x01, Sending::Command);
    delay(2);
}

void LCD::Screen::returnHome(){
    parent.send(0x02, Sending::Command);
    delay(2);
}

void LCD::init(const Screen::ArgList Args){
    Sending s_mode = Sending::Command;

    digitalWrite(PW, HIGH);
    delay(20);
    send(0x03, s_mode);
    delay(10);
    send(0x03, s_mode);
    delay(1);
    send(0x03, s_mode);
    if(BUS.mode == LCD::DataBus::Mode::M4bit)
        sendHalf(0x02, s_mode);
    else
        send(0x03, s_mode);
    Screen.setFunction(Args & 0x18);
    Screen.setScreenMode(Args & 0x07);
    Screen.clearScreen();
    Screen.returnHome();
}

bool LCD::available(){
    return busy == false;
}

void LCD::write(const char Char){
    if(Screen.Cursor.X == 16){
        if(Screen.Cursor.Y == 0)
            ++Screen.Cursor.Y;
        else
            --Screen.Cursor.Y;
        Screen.moveCursor(Screen.Cursor.Y, 0);
    }
    send(Char, Sending::Char);
    ++Screen.Cursor.X;
    ets_delay_us(50);
}

void LCD::print(const char* string, size_t len){
    for (size_t i = 0; i < len; i++)
    {
        write(string[i]);
    }
}

void LCD::print(const char* string){
    for (size_t i = 0; true; i++)
    {
        if(string[i] == '\0')
            return;
        write(string[i]);
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
    char format[5] = {'%', '.', '0'+(char)precision, 'f', '\0'};
    sprintf(str, format, data);
    print(str);
}