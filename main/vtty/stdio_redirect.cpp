#include "global_tty.hpp"
#include <cstdio>
#include <cstdarg>

extern "C" {

int tty_puts(const char* str) {
    if (global_tty) {
        global_tty->write(str);
        global_tty->write("\r\n");
    }
    return 0;
}

int tty_putchar(int c) {
    if (global_tty) {
        global_tty->write_char(static_cast<char>(c));
    }
    return c;
}

int tty_fputc(int c, FILE* /*stream*/) {
    return tty_putchar(c);
}

int tty_fputs(const char* str, FILE* /*stream*/) {
    if (global_tty) {
        global_tty->write(str);
    }
    return 0;
}

static int tty_vprintf_internal(const char* fmt, va_list ap) {
    char buffer[256];
    int len = vsnprintf(buffer, sizeof(buffer), fmt, ap);
    if (len > 0 && global_tty) {
        global_tty->write(buffer);
    }
    return len;
}

int tty_printf(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int len = tty_vprintf_internal(fmt, ap);
    va_end(ap);
    return len;
}

int tty_vfprintf(FILE* /*stream*/, const char* fmt, va_list ap) {
    return tty_vprintf_internal(fmt, ap);
}

int tty_fprintf(FILE* stream, const char* fmt, ...) {
    (void)stream;
    va_list ap;
    va_start(ap, fmt);
    int len = tty_vprintf_internal(fmt, ap);
    va_end(ap);
    return len;
}

size_t tty_fwrite(const void* ptr, size_t size, size_t nmemb, FILE* /*stream*/) {
    size_t total = size * nmemb;
    const char* data = static_cast<const char*>(ptr);
    if (global_tty) {
        for (size_t i = 0; i < total; ++i) {
            global_tty->write_char(data[i]);
        }
    }
    return nmemb;
}

} // extern "C"
