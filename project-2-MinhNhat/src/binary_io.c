/**
 * @file binary_io.c
 * @brief Binary I/O utilities implementation
 */

 /*********************
 *    INCLUDES
 *********************/
#include "binary_io.h"
#include <string.h>

/*********************
 *    FUNCTIONS
 *********************/
int read_exact(void *dst, size_t n, FILE *f)
{
    size_t bytes_read = fread(dst, 1, n, f);
    return bytes_read == n;
}

int read_u16_le(uint16_t *out, FILE *f)
{
    uint8_t bytes[2];
    if (!read_exact(bytes, 2, f))
    {
        return 0;
    }
    *out = (uint16_t)(bytes[0] | ((uint16_t)bytes[1] << 8));
    return 1;
}

int read_u32_le(uint32_t *out, FILE *f)
{
    uint8_t bytes[4];
    if (!read_exact(bytes, 4, f))
    {
        return 0;
    }
    *out = ((uint32_t)bytes[0]) |
           ((uint32_t)bytes[1] << 8) |
           ((uint32_t)bytes[2] << 16) |
           ((uint32_t)bytes[3] << 24);
    return 1;
}

int read_f32_le(float *out, FILE *f)
{
    uint32_t u;
    if (!read_u32_le(&u, f))
    {
        return 0;
    }
    memcpy(out, &u, sizeof(float));
    return 1;
}

int read_f64_le(double *out, FILE *f)
{
    uint8_t bytes[8];
    if (!read_exact(bytes, 8, f))
    {
        return 0;
    }
    
    uint64_t u = ((uint64_t)bytes[0]) |
                 ((uint64_t)bytes[1] << 8) |
                 ((uint64_t)bytes[2] << 16) |
                 ((uint64_t)bytes[3] << 24) |
                 ((uint64_t)bytes[4] << 32) |
                 ((uint64_t)bytes[5] << 40) |
                 ((uint64_t)bytes[6] << 48) |
                 ((uint64_t)bytes[7] << 56);
    
    memcpy(out, &u, sizeof(double));
    return 1;
}