#ifndef LCD_HPP

#include <Arduino.h>
#define LCD_HPP

extern size_t MEMORYUSAGE;


class LCD{
public:
    typedef unsigned char Byte;
    typedef uint8_t pin;

private:

    struct DataBus{
        friend class LCD;

        #ifdef UNIT_TEST
        static uint32_t _ID;
        uint32_t id;
        #endif

        const pin* pins;
        const enum class Mode{M4bit, M8bit} mode;

        DataBus(pin d7, pin d6, pin d5, pin d4) : pins(new pin[4] {d7, d6, d5, d4}), mode(Mode::M4bit) {
            #ifdef UNIT_TEST
            id = _ID++;
            size_t size = sizeof(pins);
            MEMORYUSAGE += size;
            Serial.printf("Allocating %u bytes for LCD.id == %u, Current memory usage = %u\n", size, id, MEMORYUSAGE);
            #endif
        }

        DataBus(pin d7, pin d6, pin d5, pin d4, pin d3, pin d2, pin d1, pin d0) : pins(new pin[8] {d7, d6, d5, d4, d3, d2, d1, d0}), mode(Mode::M8bit) {
            #ifdef UNIT_TEST
            id = _ID++;
            size_t size = sizeof(pins);
            MEMORYUSAGE += size;
            Serial.printf("Allocating %u bytes for LCD.id == %u, Current memory usage = %u\n", size, id, MEMORYUSAGE);
            #endif
        }

        DataBus(DataBus& other) : mode(other.mode) {
            const uint8_t size = other.mode == Mode::M8bit ? 8 : 4;
            pins = new pin[size];
            memcpy((void *)pins, other.pins, size * sizeof(pin));

            #ifdef UNIT_TEST
            id = _ID++;
            size_t m_size = sizeof(pins);
            MEMORYUSAGE += m_size;
            Serial.printf("Allocating %u bytes for LCD.id == %u, Current memory usage = %u\n", size, id, MEMORYUSAGE);
            #endif
        }

        ~DataBus(){
            #ifdef UNIT_TEST
            size_t size = sizeof(pins);
            MEMORYUSAGE -= size;
            Serial.printf("Freeing %u bytes from LCD.id == %u, Current memory usage = %u\n", size, id, MEMORYUSAGE);
            #endif

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

public:
    LCD(DataBus db, pin rw, pin rs, pin e, pin pw) : BUS(db), RW(rw), RS(rs), E(e), PW(pw), Screen(*this) {}

    enum class Sending {Command, Char};
    void sendHalf(Byte byte, Sending mode = Sending::Command);
    void send(Byte byte, Sending mode = Sending::Command);

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

//TODO: The code that use the variables below needs to be implemented:
    bool busy = false;
    const size_t CHAR_BUFFER_MAX = 32;
    const uint8_t FLOAT_MAX_PRECISION = 9;

public:
    Screen Screen;
};
#endif