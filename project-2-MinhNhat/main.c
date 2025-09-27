/**
 * @file main.c
 */

/*********************
 *    INCLUDES
 *********************/
#include "weather_parser.h"
#include <string.h>
#include <stdio.h>

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void print_usage(const char *program_name)
{
    printf("Usage: %s [input_file] [output_file]\n", program_name);
    printf("\n");
    printf("Arguments:\n");
    printf("  input_file   Path to binary weather data file (default: weather_data.bin)\n");
    printf("  output_file  Path to output JSON file (default: data/weather_data.json)\n");
    printf("\n");
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
int main(int argc, char **argv)
{
    // Parse command line arguments
    const char *input_file = (argc > 1) ? argv[1] : "weather_data.bin";
    const char *output_file = (argc > 2) ? argv[2] : "data/weather_data.json";
    
    // Check for help flag
    if (argc > 1 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0))
    {
        print_usage(argv[0]);
        return 0;
    }
    
    // Parse the weather file
    return parse_weather_file(input_file, output_file);
}