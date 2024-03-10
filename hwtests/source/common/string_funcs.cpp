#include "string_funcs.h"

#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace Common {

std::string FormatString(const char* format, ...)
{
    va_list arguments;
    char *va_str;

    va_start(arguments, format);
    vasprintf(&va_str, format, arguments);
    va_end(arguments);

    std::string out_str(va_str);
    free(va_str);

    return out_str;
}

}
