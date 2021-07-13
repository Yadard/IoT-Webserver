#ifndef LCD_HPP

//Archieve of the LCD lib

#include "Arduino.h"
#define LCD_HPP

class LCD{
public:
    typedef unsigned char Byte;
    typedef uint8_t pin;

private:

    struct DataBus{
        friend class LCD;

        const pin* pins;
        const enum class Mode{M4bit, M8bit} mode;

        DataBus(pin d4, pin d5, pin d6, pin d7) : pins(new pin[4] {d4, d5, d6, d7}), mode(Mode::M4bit) {}
        DataBus(pin d0, pin d1, pin d2, pin d3, pin d4, pin d5, pin d6, pin d7) : pins(new pin[8] {d0, d1, d2, d3, d4, d5, d6, d7}), mode(Mode::M8bit) {}

        DataBus(DataBus& other) : mode(other.mode) {
            const uint8_t size = other.mode == Mode::M8bit ? 8 : 4;
            pins = new pin[size];
            memcpy((void *)pins, other.pins, size * sizeof(pin));
            for (size_t i = 0; i < size; i++)
            {
                Serial.printf("[%d] = pin %hu\n", i, pins[i]);
            }
            
        }

        ~DataBus(){
            delete[] pins;
        }

        uint8_t getSize();

        void parseByte(const Byte byte);
        void parseByte(const Byte byte) const;

        class Iterator;

        Iterator begin();
        Iterator begin() const;

        Iterator end();
        Iterator end() const;

        class Iterator{
            const pin* CurrentElement;
        public:
            Iterator(const pin* element) noexcept : CurrentElement(element) {}

            Iterator& operator=(const pin* element);

            Iterator& operator++();

            Iterator operator++(int);

            bool operator!=(const Iterator& it);

            pin operator*();
        };
    };

    //TODO: All the code bellow can be separated into struct by categories. Ex:
    /*
    * Screen
    * └──► Screen functions []
    * 
    * Writing
    * └──► Writing functions []
    */

public:
    LCD(DataBus db, pin rw, pin rs, pin e, pin pw) : BUS(db), RW(rw), RS(rs), E(e), PW(pw), Screen(*this) {}

    enum class Sending {Command, Char};
    void sendHalf(Byte byte, Sending mode = Sending::Command);
    void send(Byte byte, Sending mode = Sending::Command);

    // TODO: this ARG system needs to be refactor to fit the system above.
    class Screen{
        LCD& parent;
    public:
        Screen(LCD& parent) : parent(parent) {}

        struct{
            uint8_t X = 0;
            uint8_t Y = 0;
        } Cursor;

        typedef uint8_t ArgList;
        enum Args{BLIKING_CURSOR = 1, SHOW_CURSOR = 2, SHOW_DISPLAY = 4, FONT_5x11=8, SCREEN_2LINE=16};

        void setScreenMode(ArgList ARGS);

        void setFunction(ArgList ARGS);

        bool moveCursor(uint8_t row, uint8_t column);

        void clearScreen();

        void returnHome();
    };

    void setup();

    void init(const Screen::ArgList ScreenArgs);

    bool available();

    //TODO: change to write;
    void write(const char Char);

    //TODO: finish print() overloads and start printf(), println();
    void print(const char* String, size_t len);
    void print(const char* String);


    void print(int64_t);
    void print(int32_t);
    void print(int16_t);
    void print(int8_t);
    
    void print(uint64_t);
    void print(uint32_t);
    void print(uint16_t);
    void print(uint8_t);

    void print(double, uint8_t);

    void printline(const char * String);

private:
    const DataBus BUS;
    const pin RW;
    const pin RS;
    const pin E;
    const pin PW;

    bool busy = false;
    const size_t CHAR_BUFFER_MAX = 32;
    const uint8_t FLOAT_MAX_PRECISION = 9;

public:
    Screen Screen;
};
#endif