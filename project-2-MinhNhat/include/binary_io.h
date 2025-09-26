/**
 * @file binary_io.h
 * @brief Binary I/O utilities for reading little-endian data
 */

#ifndef BINARY_IO_H
#define BINARY_IO_H

/*********************
 *    INCLUDES
 *********************/
#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *    FUNCTIONS
 *********************/
/**
 * @brief Read exact number of bytes from file
 * 
 * @param dst Destination buffer
 * @param n Number of bytes to read
 * @param f File pointer
 * 
 * @return 1 on success, 0 on failure
 */
int read_exact(void *dst, size_t n, FILE *f);

/**
 * @brief Read 16-bit unsigned integer (little-endian)
 * 
 * @param out Pointer to store the result
 * @param f File pointer
 * 
 * @return 1 on success, 0 on failure
 */
int read_u16_le(uint16_t *out, FILE *f);

/**
 * @brief Read 32-bit unsigned integer (little-endian)
 * 
 * @param out Pointer to store the result
 * @param f File pointer
 * 
 * @return 1 on success, 0 on failure
 */
int read_u32_le(uint32_t *out, FILE *f);

/**
 * @brief Read 32-bit float (little-endian, IEEE-754)
 * 
 * @param out Pointer to store the result
 * @param f File pointer
 * 
 * @return 1 on success, 0 on failure
 */
int read_f32_le(float *out, FILE *f);

/**
 * @brief Read 64-bit double (little-endian, IEEE-754)
 * 
 * @param out Pointer to store the result
 * @param f File pointer
 * 
 * @return 1 on success, 0 on failure
 */
int read_f64_le(double *out, FILE *f);

#ifdef __cplusplus
}
#endif

#endif // BINARY_IO_H