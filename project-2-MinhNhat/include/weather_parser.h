/**
 * @file weather_parser.h
 * @brief Weather data binary file parser
 */

#ifndef WEATHER_PARSER_H
#define WEATHER_PARSER_H

/*********************
 *    INCLUDES
 *********************/
#include <stdio.h>
#include "weather_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *    FUNCTIONS
 *********************/

/**
 * @brief Read file header from binary file
 * 
 * @param header Pointer to store header data
 * @param f File pointer
 * 
 * @return 1 on success, 0 on failure
 */
int read_header(file_header_t *header, FILE *f);

/**
 * @brief Read one weather record from binary file
 * 
 * @param record Pointer to store record data
 * @param f File pointer
 * 
 * @return 1 on success, 0 on failure
 */
int read_weather_record(weather_record_t *record, FILE *f);

/**
 * @brief Validate file size against expected size
 * 
 * @param f File pointer
 * @param record_count Number of records expected
 * 
 * @return 1 if valid, 0 if invalid (with warning)
 */
int validate_file_size(FILE *f, uint32_t record_count);

/**
 * @brief Parse entire weather data file
 * 
 * @param input_file Path to input binary file
 * @param output_file Path to output JSON file
 * 
 * @return 0 on success, non-zero on error
 */
int parse_weather_file(const char *input_file, const char *output_file);

#ifdef __cplusplus
}
#endif

#endif // WEATHER_PARSER_H