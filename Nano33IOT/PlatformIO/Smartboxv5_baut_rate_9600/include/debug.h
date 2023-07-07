// Helper macros for debugging application code.
// Prefer to use this ONLY in .cpp files, as we don't want macros being
// included/leaking everywhere. You'll need to define a `DEBUG_OPTION` prior to
// `#include "debug.h"`.

#ifndef DEBUG_OPTION
#error "Expected a debug option to use debugging macros. Select a `DEBUG_*` option from defines.h."
#endif


#if 0 // TODO - implement optionGet

#define debugimpl(func, ...)           \
    do {                               \
        if (optionGet(DEBUG_OPTION)) { \
            Serial.func(__VA_ARGS__);  \
        }                              \
    } while (0)
#define debug(...)   debugimpl(print, __VA_ARGS__)
#define debugln(...) debugimpl(println, __VA_ARGS__)

#include <cstring>

#define debugf(...)                                        \
    do {                                                   \
        if (optionGet(DEBUG_OPTION)) {                     \
            static char buffer[128];                       \
            snprintf(buffer, sizeof(buffer), __VA_ARGS__); \
            Serial.print(buffer);                          \
        }                                                  \
    } while (0)

#else

#define debug(...)
#define debugf(...)
#define debugln(...)

#endif
