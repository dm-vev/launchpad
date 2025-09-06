#pragma once
#include "TTYDriver.hpp"

class TTY {
private:
    TTYDriver* driver;
public:
    explicit TTY(TTYDriver* drv) : driver(drv) {
        driver->init();
    }

    void write(const char* str) { driver->write(str); }
    void write_char(char c)     { driver->write_char(c); }
    char read()                 { return driver->read(); }
};
