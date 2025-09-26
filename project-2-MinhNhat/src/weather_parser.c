/**
 * @file weather_parser.c
 * @brief Weather data parser implementation
 */

 /*********************
 *    INCLUDES
 *********************/
#include "weather_parser.h"
#include "binary_io.h"
#include "json_writer.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>

/*********************
 *    FUNCTIONS
 *********************/
const char* battery_status_to_string(uint8_t status)
{
    switch (status)
    {
        case BATTERY_NORMAL:    return "normal";
        case BATTERY_LOW:       return "low"; 
        case BATTERY_EMERGENCY: return "emergency";
        default:                return "unknown";
    }
}

int read_header(file_header_t *header, FILE *f)
{
    // Read file ID (4 bytes)
    if (!read_exact(header->file_id, FILE_ID_SIZE, f))
    {
        return 0;
    }
    header->file_id[FILE_ID_SIZE] = '\0'; // Null terminate
    
    // Read version (2 bytes)
    if (!read_u16_le(&header->version, f))
    {
        return 0;
    }
    
    // Read record count (4 bytes)
    if (!read_u32_le(&header->count, f))
    {
        return 0;
    }
    
    return 1;
}

int read_weather_record(weather_record_t *record, FILE *f)
{
    return read_u32_le(&record->sensor_id, f) &&
           read_exact(&record->battery, 1, f) &&
           read_u32_le(&record->timestamp, f) &&
           read_f64_le(&record->lat, f) &&
           read_f64_le(&record->lon, f) &&
           read_f32_le(&record->temperature, f) &&
           read_f32_le(&record->humidity, f) &&
           read_f32_le(&record->pressure, f) &&
           read_u16_le(&record->co2, f) &&
           read_f32_le(&record->wind_speed, f) &&
           read_u16_le(&record->wind_dir, f) &&
           read_f32_le(&record->rain, f) &&
           read_f32_le(&record->uv, f) &&
           read_f32_le(&record->light, f);
}

int validate_file_size(FILE *f, uint32_t record_count)
{
    long current_pos = ftell(f);
    if (current_pos < 0)
    {
        return 0;
    }
    
    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    fseek(f, current_pos, SEEK_SET);
    
    if (file_size < 0)
    {
        return 0;
    }
    
    long expected_size = HEADER_SIZE + (long)RECORD_SIZE * record_count;
    
    if (file_size != expected_size)
    {
        fprintf(stderr, "WARNING: File size mismatch. Expected: %ld, Actual: %ld\n", 
                expected_size, file_size);
        return 0;
    }
    
    return 1;
}

int parse_weather_file(const char *input_file, const char *output_file)
{
    printf("Converting: %s -> %s\n", input_file, output_file);
    
    // Open input file
    FILE *fin = fopen(input_file, "rb");
    if (!fin)
    {
        fprintf(stderr, "ERROR: Cannot open input file '%s': %s\n", 
                input_file, strerror(errno));
        return 1;
    }
    
    // Read and validate header
    file_header_t header;
    if (!read_header(&header, fin))
    {
        fprintf(stderr, "ERROR: Failed to read file header\n");
        fclose(fin);
        return 1;
    }
    
    printf("File ID: %s\n", header.file_id);
    printf("Version: %u\n", header.version);
    printf("Record count: %u\n", header.count);
    
    // Validate file size (optional but recommended)
    validate_file_size(fin, header.count);
    
    // Create output directory
    create_output_directory("data");
    
    // Open output file
    FILE *fout = fopen(output_file, "w");
    if (!fout)
    {
        fprintf(stderr, "ERROR: Cannot open output file '%s': %s\n", 
                output_file, strerror(errno));
        fclose(fin);
        return 1;
    }
    
    // Write JSON header
    write_json_header(&header, fout);
    
    // Process records
    uint32_t records_processed = 0;
    for (uint32_t i = 0; i < header.count; i++)
    {
        weather_record_t record;
        
        if (!read_weather_record(&record, fin))
        {
            fprintf(stderr, "ERROR: Failed to read record %u\n", i + 1);
            break;
        }
        
        write_json_record(&record, fout, (i == header.count - 1));
        records_processed++;
    }
    
    // Write JSON footer
    write_json_footer(fout);
    
    // Cleanup
    fclose(fin);
    fclose(fout);
    
    if (records_processed == header.count)
    {
        printf("SUCCESS: Converted %u records to JSON format\n", records_processed);
        return 0;
    } 
    else
    {
        printf("WARNING: Only processed %u out of %u records\n", 
               records_processed, header.count);
        return 1;
    }
}