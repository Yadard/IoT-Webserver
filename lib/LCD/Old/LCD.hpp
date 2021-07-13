#ifndef LCD_HPP

/*A working and unclean code for LCD lib
*! 
*/

#include "Arduino.h"
#define LCD_HPP

class LCD{
public:
    typedef unsigned char Byte;
    typedef uint8_t pin;

private:
    bool busy = false;
    const size_t CHAR_BUFFER_MAX = 32;
    const uint8_t FLOAT_MAX_PRECISION = 9;

    struct DATA_BUS{
        const pin* m_pins;
        const uint8_t m_size;

        DATA_BUS(const pin* const pins, uint8_t size) : m_pins(pins), m_size(size) {}
    };

    struct{
        struct{
            uint8_t X = 0;
            uint8_t Y = 0;
        } cursor;
    } current;
    const DATA_BUS BUS;
    const pin RW;
    const pin RS;
    const pin E;
    const pin PW;

    uint8_t currentLine = 0;
    uint32_t chars = 0;

    enum class sendMode {CMD, CH};

    void send(Byte byte, sendMode mode, bool onlyHalf = false);

    void parseByte(const Byte byte);
public:
    LCD(const pin* const pins, uint8_t size, pin rw, pin rs, pin e, pin pw) : BUS(pins, size), RW(rw), RS(rs), E(e), PW(pw) {}

    typedef uint8_t ARG_LIST;
    enum ARG{BLIKING_CURSOR = 1, SHOW_CURSOR = 2, SHOW_DISPLAY = 4, FONT_5x11=8, SCREEN_2LINE=16};

    void setup();

    void init(ARG_LIST ARGS);

    void setScreenMode(ARG_LIST ARGS);

    void setFunction(ARG_LIST ARGS);

    bool moveCursor(uint8_t row, uint8_t column);

    void clearScreen();

    void returnHome();

    bool available();

    void sendChar(const char c);

    void print(const char* c, size_t len);
    void print(const char* c);


    void print(int64_t);
    void print(int32_t);
    void print(int16_t);
    void print(int8_t);
    
    void print(uint64_t);
    void print(uint32_t);
    void print(uint16_t);
    void print(uint8_t);

    void print(double, uint8_t);
};
#endif