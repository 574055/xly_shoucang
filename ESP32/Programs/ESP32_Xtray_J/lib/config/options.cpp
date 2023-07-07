#include "defines.h"

#include <cstdint>


// Use array representation instead of bit representation for compatibility with SerialTuning lib,
// which takes pointers for modification.
uint8_t options[NUM_OPTIONS];


void option::loadDefaults()
{
    static const bool DEFAULT_OPTIONS[] = {
#define X_DEFAULT(D, ...) D,
        OPTIONS_LIST(X_DEFAULT)
#undef X_DEFAULT
    };

    for (int i = 0; i < NUM_OPTIONS; i++)
        set(static_cast<EnableOption>(i), DEFAULT_OPTIONS[i]);
}


const uint8_t& option::get(EnableOption opt)
{
    // return (options & (1UL << opt)) != 0;
    return options[opt];
}


void option::set(EnableOption opt, bool value)
{
    // options |= (1UL << opt);
    options[opt] = value;
}


void option::toggle(EnableOption opt)
{
    // options ^= (1UL << opt);
    options[opt] = !options[opt];
}


const char* option::getLabel(EnableOption opt)
{
#define X_LABEL(D, E, S) S,
    static const char* str[] = {OPTIONS_LIST(X_LABEL)};
#undef X_LABEL
    return str[opt];
}


bool option::isAddress(void* ptr)
{
    return &options[0] <= ptr && ptr <= &options[NUM_OPTIONS - 1];
}
