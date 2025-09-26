/**
 * @file weather_types.h
 * @brief Data structures and constants for weather data parsing
 */

#ifndef WEATHER_TYPES_H
#define WEATHER_TYPES_H

/*********************
 *    INCLUDES
 *********************/
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      CONSTANTS
 *********************/
#define HEADER_SIZE 10
#define RECORD_SIZE 57
#define FILE_ID_SIZE 4

/*********************
 *      ENUMS
 *********************/
typedef enum {
    BATTERY_NORMAL = 0,
    BATTERY_LOW = 1,
    BATTERY_EMERGENCY = 2
} battery_status_t;

/*********************
 *      STRUCTS
 *********************/

/**
 * @brief File header structure
 */
typedef struct {
    char file_id[FILE_ID_SIZE + 1];  // +1 for null terminator
    uint16_t version;
    uint32_t count;
} file_header_t;

/**
 * @brief Weather record structure
 */
typedef struct {
    uint32_t sensor_id;
    uint8_t battery;
    uint32_t timestamp;
    double lat;
    double lon;
    float temperature;
    float humidity;
    float pressure;
    uint16_t co2;
    float wind_speed;
    uint16_t wind_dir;
    float rain;
    float uv;
    float light;
} weather_record_t;

/*********************
 *    FUNCTIONS
 *********************/

/**
 * @brief Convert battery status code to string
 * 
 * @param status Battery status code
 * 
 * @return String representation of battery status
 */
const char* battery_status_to_string(uint8_t status);

#ifdef __cplusplus
}
#endif

#endif // WEATHER_TYPES_H