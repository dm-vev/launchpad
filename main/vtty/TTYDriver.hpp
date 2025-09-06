#pragma once

class TTYDriver {
public:
    virtual void init() = 0;
    virtual void write(const char* str) = 0;
    virtual void write_char(char c) = 0;
    virtual char read() = 0;
    virtual ~TTYDriver() = default;
};
