/**
 * @file json_writer.h
 * @brief JSON output writer for weather data
 */

#ifndef JSON_WRITER_H
#define JSON_WRITER_H

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
 * @brief Write JSON file header with metadata
 * 
 * @param header File header structure
 * @param f Output file pointer
 */
void write_json_header(const file_header_t *header, FILE *f);

/**
 * @brief Write a single weather record as JSON
 * 
 * @param record Weather record structure
 * @param f Output file pointer
 * @param is_last Whether this is the last record (affects comma)
 */
void write_json_record(const weather_record_t *record, FILE *f, int is_last);

/**
 * @brief Write JSON file footer
 * 
 * @param f Output file pointer
 */
void write_json_footer(FILE *f);

/**
 * @brief Create output directory if it doesn't exist
 * 
 * @param dir_path Directory path to create
 * 
 * @return 0 on success, -1 on error
 */
int create_output_directory(const char *dir_path);

#ifdef __cplusplus
}
#endif

#endif // JSON_WRITER_H