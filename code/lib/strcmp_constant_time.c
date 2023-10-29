// https://nachtimwald.com/2017/04/02/constant-time-string-comparison-in-c/

#include <stdlib.h>

int strcmp_constant_time(const char *s1, const char *s2)
{
    int m = 0;
    volatile size_t i = 0;
    volatile size_t j = 0;
    volatile size_t k = 0;

    if ( s1 == NULL || s2 == NULL )
        return 0;

    while ( 1 )
    {
        m |= s1[i] ^ s2[j];

        if ( s1[i] == '\0' )
            break;
        i++;

        if ( s2[j] != '\0' )
            j++;
        if ( s2[j] == '\0' )
            k++;
    }

    return m == 0;
}