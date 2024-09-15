#include "plugin.h"
#include "utils.h"

int lcd_getstringsizef(const char *fmt, ...)
{
    char buffer[128];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    int width, height;
    rb->lcd_getstringsize(buffer, &width, &height);
    return width;
}