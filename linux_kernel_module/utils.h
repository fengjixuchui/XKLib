#ifndef KERNEL_CUSTOM_UTILS
#define KERNEL_CUSTOM_UTILS

#include "main.h"

#define DO_ONCE(code)                                                    \
    {                                                                    \
        static bool bOnce = false;                                       \
        if (!bOnce)                                                      \
        {                                                                \
            code;                                                        \
            bOnce = true;                                                \
        }                                                                \
    }

unsigned char hex_digit_to_byte(char c);
int hex_char_to_byte(char c1, char c2);
void swap_endian(unsigned char* addr, size_t len);
int convert_to_hexstring(uint8_t* array,
                         size_t size,
                         char output[],
                         size_t output_size);

#endif
